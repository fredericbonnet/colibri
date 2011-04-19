/*
 * Mark-and-sweep, generational, exact GC.
 *
 * Newer cells are born in "Eden", i.e. generation 1 pool, and are promoted to 
 * older pools when they survive more than one collection.
 *
 * A GC is triggered after a number of page allocations have been made resulting
 * from cell allocation failures. They are only triggered outside of 
 * GC-protected sections to avoid collecting the newly allocated cells.
 *
 * Younger pools are collected more often than older ones. Completing a given 
 * number of GCs on a pool will include the previous generation to the next GC. 
 * For example, if this generational factor is 10, then generation x will be 
 * collected every 10 collections of generation x-1. So gen-x collections will 
 * occur every 10 gen-(x-1) collection, i.e. every 10^x gen-0 collection. 
 */

#include "colibri.h"
#include "colInternal.h"
#include "colPlatform.h"

#include <memory.h>
#include <limits.h>

/*
 * Without thread support, use static data instead of thread-local storage.
 */

#ifdef COL_THREADS
#   define GetGcMemInfo PlatGetGcMemInfo
#else /* COL_THREADS */
static GcMemInfo gcMemInfo;
#   define GetGcMemInfo() (&gcMemInfo)
#endif /* COL_THREADS */

/*
 * Prototypes for functions used only in this file.
 */

static size_t		GetNbCells(const void *cell);
static void		PreserveCell(Cell *cell);
static void		ReleaseCell(Cell *cell);
static void		PerformGC(void);
static void		ClearPoolBitmasks(GcMemInfo *info, 
			    unsigned int generation);
static void		MarkReachableCellsFromRoots(GcMemInfo *info);
static void		MarkReachableCellsFromParents(GcMemInfo *info);
static void		MarkRope(GcMemInfo *info, Col_Rope *ropePtr, 
			    Page *parentPage);
static void		MarkWord(GcMemInfo *info, Col_Word *wordPtr, 
			    Page *parentPage);
static void		SweepUnreachableCells(GcMemInfo *info);
static void		PromotePages(GcMemInfo *info, unsigned int generation);


/*
 *---------------------------------------------------------------------------
 *
 * GcInit --
 * GcCleanup --
 *
 *	Initialize/cleanup the garbage collector.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Platform-specific -- see respective implementations of callees.
 *
 *---------------------------------------------------------------------------
 */

void 
GcInit() 
{
    unsigned int generation;
    GcMemInfo *info;
    
    info = GetGcMemInfo();
    memset(info, 0, sizeof(*info));
    for (generation = 0; generation < GC_MAX_GENERATIONS; generation++) {
	PoolInit(&info->pools[generation], generation);
    }
}

void 
GcCleanup() 
{
    unsigned int generation;
    GcMemInfo *info;

    info = GetGcMemInfo();
    for (generation = 0; generation < GC_MAX_GENERATIONS; generation++) {
	PoolCleanup(&info->pools[generation]);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * GetNbCells --
 *
 *	Get the number of cells taken by a rope or word.
 *
 * Results:
 *	The number of cells.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

static size_t
GetNbCells(
    const void *cell)		/* The first cell of the rope or word. */
{
    if (CELL_TYPE(cell) /* word */) {
	Col_Word word = (Col_Word) cell;
	Col_WordType * typeInfo;

	/*
	 * Get number of cells taken by word.
	 * Note: word is neither immediate nor rope.
	 */

	switch (WORD_TYPE(word)) {
	    case WORD_TYPE_VECTOR:
		return VECTOR_SIZE(WORD_VECTOR_LENGTH(word));

	    case WORD_TYPE_MVECTOR:
		return WORD_MVECTOR_SIZE(word);

	    case WORD_TYPE_REGULAR:
		WORD_GET_TYPE_ADDR(word, typeInfo);
		return NB_CELLS(WORD_HEADER_SIZE + typeInfo->sizeProc(word)
			+ (typeInfo->freeProc?WORD_TRAILER_SIZE:0));

	    case WORD_TYPE_INTHASHMAP:
		return WORD_INTHASHMAP_NBCELLS(word);

	    /* WORD_TYPE_UNKNOWN */

	    default:
		return 1;
	}
    } else {
	Col_Rope rope = cell;

	/*
	 * Get number of cells taken by rope.
	 * Note: rope is not C string, NULL or empty.
	 */

	switch (ROPE_TYPE(rope)) {
	    case ROPE_TYPE_UCS1:
		return UCS_SIZE(ROPE_UCS_LENGTH(rope));

	    case ROPE_TYPE_UCS2:
		return UCS_SIZE(ROPE_UCS_LENGTH(rope)*2);

	    case ROPE_TYPE_UCS4:
		return UCS_SIZE(ROPE_UCS_LENGTH(rope)*4);

	    case ROPE_TYPE_UTF8:
		return UTF8_SIZE(ROPE_UTF8_BYTELENGTH(rope));

	    case ROPE_TYPE_CUSTOM:
		return NB_CELLS(ROPE_CUSTOM_HEADER_SIZE + ROPE_CUSTOM_SIZE(rope) 
			+ (ROPE_CUSTOM_TYPE(rope)->freeProc?ROPE_CUSTOM_TRAILER_SIZE:0));

	    /* ROPE_TYPE_UNKNOWN */

	    default:
		return 1;
	}
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ResolveRedirectRope --
 * ResolveRedirectWord --
 *
 *	Resolve redirect cell until a regular cell is reached. The source 
 *	pointer is overwritten with the resolved cell pointer so that future 
 *	traversals will reach the final cell directly.
 *
 * Results:
 *	The referenced word or rope via wordPtr resp. ropePtr.
 *
 * Side effects:
 *	Overwrite references to redirect cells.
 *
 *---------------------------------------------------------------------------
 */

#ifdef PROMOTE_COMPACT
#   define ResolveRedirectRope(ropePtr)			\
{							\
    if (ROPE_TYPE(*(ropePtr)) == ROPE_TYPE_REDIRECT) {	\
	*(ropePtr) = ROPE_REDIRECT_SOURCE(*(ropePtr));	\
    }							\
}
#   define ResolveRedirectWord(wordPtr)			\
{							\
    switch (WORD_TYPE(*(wordPtr))) {			\
	case WORD_TYPE_ROPE: {				\
	    Col_Rope _rope_ = WORD_ROPE_GET(*(wordPtr));\
	    ResolveRedirectRope(&_rope_);		\
	    *(wordPtr) = WORD_ROPE_NEW(_rope_);		\
	    break;					\
	}						\
							\
	case WORD_TYPE_REDIRECT:			\
	    *(wordPtr) = (Col_Word) WORD_REDIRECT_SOURCE(*wordPtr);	\
	    break;					\
    }							\
}
#endif /* PROMOTE_COMPACT */

/*
 *---------------------------------------------------------------------------
 *
 * Col_SetModified --
 *
 *	Mark cell as modified.
 *
 *	As the GC uses a generational scheme, it must keep track of cells from 
 *	older generations that refer to newer ones. Cells from older generations
 *	that are marked as modified potentially point to younger cells, and must
 *	be followed during the next GC in any case so that their children don't 
 *	get collected by mistake.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory cells may be created.
 *
 *---------------------------------------------------------------------------
 */

EXTERN void
Col_SetModified(
    void *cell)			/* Modified rope or word. */
{
    GcMemInfo *info = GetGcMemInfo();
    unsigned int generation;
    Cell *root;
    Page *page;

    if (!info->pauseGC) {
	/*
	 * Can't be called outside of a GC protected section.
	 */

	Col_Error(COL_ERROR, "Called outside of a GC-protected section");
	return;
    }

    page = CELL_PAGE(cell);
    generation = PAGE_GENERATION(page);
    if (PAGE_CHILDGENERATION(page) < generation) {
	/*
	 * Already a parent.
	 */

	return;
    }
    if (generation <= 1) {
	/*
	 * Parent is from the youngest generation, no need to keep track.
	 */

	return;
    }

    /* 
     * Create a new parent root, and insert at head of parent root list for the 
     * cell generation. Reset child generation to 0, the actual value will be
     * computed during next GC.
     */

    PAGE_CHILDGENERATION(page) = 0;
    root = PoolAllocCells(&info->pools[0], 1);
    PARENT_INIT(root, info->pools[generation].parents, page);
    info->pools[generation].parents = root;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_PreserveRope --
 * Col_ReleaseRope --
 * Col_PreserveWord --
 * Col_ReleaseWord --
 * PreserveCell --
 * ReleaseCell --
 *
 *	Preserve (resp. release) a persistent reference to a rope or word, 
 *	which prevents its collection. This allows ropes or words to be safely 
 *	stored in external structures regardless of memory management cycles.
 *
 *	Calls can be nested. A reference count is updated accordingly. Once the 
 *	count drops below 1, the root becomes stale.
 *
 *	Roots are stored in a trie indexed by the root source addresses.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Fields may be updated, and memory cells may be allocated.
 *
 *---------------------------------------------------------------------------
 */

void
Col_PreserveRope(
    Col_Rope rope)		/* The rope to preserve. */
{
    switch (ROPE_TYPE(rope)) {
	case ROPE_TYPE_NULL:
	case ROPE_TYPE_C:
	case ROPE_TYPE_EMPTY:
	    /* 
	     * Unmanaged strings. 
	     */

	    return;
    }

    PreserveCell((Cell *) rope);
}

void
Col_ReleaseRope(
    Col_Rope rope)		/* The rope to release. */
{
    switch (ROPE_TYPE(rope)) {
	case ROPE_TYPE_NULL:
	case ROPE_TYPE_C:
	case ROPE_TYPE_EMPTY:
	    /* 
	     * Unmanaged strings. 
	     */

	    return;
    }

    ReleaseCell((Cell *) rope);
}

void
Col_PreserveWord(
    Col_Word word)		/* The word to preserve. */
{
    switch (WORD_TYPE(word)) {
	case WORD_TYPE_NIL:
	case WORD_TYPE_SMALL_INT:
	case WORD_TYPE_CHAR:
	case WORD_TYPE_SMALL_STRING:
	case WORD_TYPE_VOID_LIST:
	    /* 
	     * Immediate values. 
	     */

	    return;

	case WORD_TYPE_ROPE:
	    PreserveCell((Cell *) WORD_ROPE_GET(word));
	    return;
    }

    PreserveCell((Cell *) word);
}

void
Col_ReleaseWord(
    Col_Word word)		/* The word to release. */
{
    switch (WORD_TYPE(word)) {
	case WORD_TYPE_NIL:
	case WORD_TYPE_SMALL_INT:
	case WORD_TYPE_CHAR:
	case WORD_TYPE_SMALL_STRING:
	case WORD_TYPE_VOID_LIST:
	    /* 
	     * Immediate values. 
	     */

	    return;

	case WORD_TYPE_ROPE:
	    ReleaseCell((Cell *) WORD_ROPE_GET(word));
	    return;
    }

    ReleaseCell((Cell *) word);
}

static void
PreserveCell(
    Cell *cell)			/* The cell to preserve. */
{
    GcMemInfo *info = GetGcMemInfo();
    Cell *node, *leaf, *parent, *newParent, *leafSource;
    uintptr_t mask;

    if (!info->pauseGC) {
	/*
	 * Can't be called outside of a GC protected section.
	 */

	Col_Error(COL_ERROR, "Called outside of a GC-protected section");
	return;
    }

    /*
     * Search for matching entry in root trie.
     */

    node = info->roots;
    while (node) {
	if (!ROOT_IS_LEAF(node)) {
	    if ((uintptr_t) cell & ROOT_NODE_MASK(node)) {
		/*
		 * Recurse on right.
		 */

		node = ROOT_NODE_RIGHT(node);
	    } else {
		/*
		 * Recurse on left.
		 */

		node = ROOT_NODE_LEFT(node);
	    }
	    continue;
	}

	/*
	 * Leaf node.
	 */

	ASSERT(ROOT_IS_LEAF(node));
	node = ROOT_GET_NODE(node);
	leafSource = ROOT_LEAF_SOURCE(node);
	if (cell == leafSource) {
	    /*
	     * Found! Increment reference count.
	     */

	    ASSERT(CELL_TYPE(cell)?WORD_PINNED(cell):ROPE_PINNED(cell));
	    ROOT_LEAF_REFCOUNT(node)++;
	    return;
	}
	break;
    }

    /*
     * Not found, insert. Pin the cell so that its address doesn't change 
     * during compaction.
     */

    if (CELL_TYPE(cell)) {
	WORD_SET_PINNED(cell);
    } else {
	ROPE_SET_PINNED(cell);
    }

    leaf = AllocCells(1);
    ROOT_LEAF_INIT(leaf, NULL, PAGE_GENERATION(CELL_PAGE(cell)), 1, cell);

    if (!info->roots) {
	/*
	 * First leaf.
	 */

	info->roots = ROOT_GET_LEAF(leaf);
	return;
    }

    /*
     * Get diff mask between inserted key and existing leaf key, i.e. only keep 
     * the highest bit set.
     * See: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2 
     */

    mask = (uintptr_t) cell ^ (uintptr_t) leafSource;
    mask |= mask >> 1;
    mask |= mask >> 2;
    mask |= mask >> 4;
    mask |= mask >> 8;
    mask |= mask >> 16;
    mask++;
    mask >>= 1;

    /*
     * Find insertion point.
     */

    node = info->roots;
    while (node) {
	if (!ROOT_IS_LEAF(node) && mask < ROOT_NODE_MASK(node)) {
	    if ((uintptr_t) cell & ROOT_NODE_MASK(node)) {
		/*
		 * Recurse on right.
		 */

		node = ROOT_NODE_RIGHT(node);
	    } else {
		/*
		 * Recurse on left.
		 */

		node = ROOT_NODE_LEFT(node);
	    }
	    continue;
	}
	break;
    }
    ASSERT(node);

    /*
     * Insert leaf here.
     */

    parent = ROOT_PARENT(ROOT_GET_NODE(node));
    newParent = AllocCells(1);
    if ((uintptr_t) cell & mask) {
	/*
	 * Leaf is right.
	 */

	ROOT_NODE_INIT(newParent, parent, mask, node, ROOT_GET_LEAF(leaf));
    } else {
	/*
	 * Leaf is left.
	 */

	ROOT_NODE_INIT(newParent, parent, mask, ROOT_GET_LEAF(leaf), node);
    }
    ROOT_PARENT(leaf) = newParent;
    ROOT_PARENT(ROOT_GET_NODE(node)) = newParent;
    if (!parent) {
	/*
	 * Leaf was trie root.
	 */

	info->roots = newParent;
    } else if ((uintptr_t) cell & ROOT_NODE_MASK(parent)) {
	ROOT_NODE_RIGHT(parent) = newParent;
    } else {
	ROOT_NODE_LEFT(parent) = newParent;
    }
}

static void
ReleaseCell(
    Cell *cell)			/* The cell to release. */
{
    GcMemInfo *info = GetGcMemInfo();
    Cell *node, *sibling, *parent, *grandParent;

    if (!info->pauseGC) {
	/*
	 * Can't be called outside of a GC protected section.
	 */

	Col_Error(COL_ERROR, "Called outside of a GC-protected section");
	return;
    }

    if (!(CELL_TYPE(cell)?WORD_PINNED(cell):ROPE_PINNED(cell))) {
	/*
	 * Roots are always pinned.
	 */

	return;
    }

    /*
     * Search for matching entry.
     */

    if (!info->roots) {
	return;
    }
    node = info->roots;
    while (node) {
	if (!ROOT_IS_LEAF(node)) {
	    if ((uintptr_t) cell & ROOT_NODE_MASK(node)) {
		/*
		 * Recurse on right.
		 */

		node = ROOT_NODE_RIGHT(node);
	    } else {
		/*
		 * Recurse on left.
		 */

		node = ROOT_NODE_LEFT(node);
	    }
	    continue;
	}

	/*
	 * Leaf node.
	 */

	ASSERT(ROOT_IS_LEAF(node));
	node = ROOT_GET_NODE(node);
	if (cell != ROOT_LEAF_SOURCE(node)) {
	    /*
	     * Source doesn't match.
	     */

	    return;
	}
	break;
    }
    ASSERT(node);

    /*
     * Decrement reference count.
     */

    if (--ROOT_LEAF_REFCOUNT(node)) {
	/*
	 * Root is still valid.
	 */

	return;
    }

    /*
     * Remove leaf. Unpin cell as well.
     */

    if (CELL_TYPE(cell)) {
	WORD_CLEAR_PINNED(cell);
    } else {
	ROPE_CLEAR_PINNED(cell);
    }

    parent = ROOT_PARENT(node);
    if (!parent) {
	/*
	 * Leaf was trie root.
	 */

	info->roots = NULL;
	return;
    }

    /*
     * Replace parent by sibling.
     */

    if ((uintptr_t) cell & ROOT_NODE_MASK(parent)) {
	sibling = ROOT_NODE_LEFT(parent);
    } else {
	sibling = ROOT_NODE_RIGHT(parent);
    }

    grandParent = ROOT_PARENT(parent);
    if (!grandParent) {
	/*
	 * Parent was trie root.
	 */

	info->roots = sibling;
    } else if ((uintptr_t) cell & ROOT_NODE_MASK(grandParent)) {
	ROOT_NODE_RIGHT(grandParent) = sibling;
    } else {
	ROOT_NODE_LEFT(grandParent) = sibling;
    }
    ROOT_PARENT(ROOT_GET_NODE(sibling)) = grandParent;
}



/*
 *---------------------------------------------------------------------------
 *
 * DeclareCustomRope --
 * DeclareWord --
 *
 *	Remember custom ropes and words needing freeing upon deletion. This 
 *	chains such ropes in their order of creation, latest being inserted at 
 *	the head of the list. This implies that cleanup can stop traversing 
 *	the list at the first custom rope that belongs to a non GC'd pool.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Rope and word fields may be updated.
 *
 *---------------------------------------------------------------------------
 */

void 
DeclareCustomRope(
    Col_Rope rope)		/* The rope to declare. */
{
    if (ROPE_CUSTOM_TYPE(rope)->freeProc) {
	/*
	 * Type needs freeing. Remember by inserting at head of list.
	 */

	GcMemInfo *info = GetGcMemInfo();
	unsigned int generation = PAGE_GENERATION(CELL_PAGE(rope));
	ROPE_CUSTOM_NEXT(rope, ROPE_CUSTOM_SIZE(rope)) 
		= info->pools[generation].sweepables;
	info->pools[generation].sweepables = (Cell *) rope;
    }
}

void 
DeclareWord(
    Col_Word word)		/* The word to declare. */
{
    Col_WordType * typeInfo;
    if (WORD_TYPE(word) == WORD_TYPE_REGULAR) {
	WORD_GET_TYPE_ADDR(word, typeInfo);
	if (typeInfo->freeProc) {
	    /*
	     * Type needs freeing. Remember by inserting at head of list.
	     */

	GcMemInfo *info = GetGcMemInfo();
	    unsigned int generation = PAGE_GENERATION(CELL_PAGE(word));
	    WORD_NEXT(word, typeInfo->sizeProc(word))
		= info->pools[generation].sweepables;
	    info->pools[generation].sweepables = (Cell *) word;
	}
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * AllocCells --
 *
 *	Allocate cells in the youngest pool (i.e. generation 1). 
 *
 *	The maximum number of cells is AVAILABLE_CELLS.
 *
 *	Fails if outside a GC protected section.
 *
 * Results:
 *	If successful, a pointer to the first allocated cell. Else NULL.
 *
 * Side effects:
 *	Alloc structures are updated, and memory pages may be allocated.
 *
 *---------------------------------------------------------------------------
 */

Cell *
AllocCells(
    size_t number)		/* Number of cells to allocate. */
{
    GcMemInfo *info = GetGcMemInfo();

    if (!info->pauseGC) {
	/*
	 * Can't be called outside of a GC protected section.
	 */

	Col_Error(COL_ERROR, "Called outside of a GC-protected section");
	return NULL;
    }
    
    /*
     * Alloc cells; alloc pages if needed.
     */

    return PoolAllocCells(&info->pools[1], number);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_PauseGC --
 * Col_ResumeGC --
 *
 *	Pause (resp. resume) the automatic garbage collection. Calls can be 
 *	nested. Code between the outermost pause and resume calls define a 
 *	GC-protected section.
 *
 *	GC-protected sections allow the caller to create many ropes with the 
 *	guaranty that they won't be collected too early. This gives a chance 
 *	to pass them around or keep references by creating roots.
 *
 *	Leaving a GC-protected section potentially triggers a GC. Outside of a 
 *	GC-protected section, a GC can occur at any moment, invalidating newly
 *	allocated ropes.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	GC-protected sections can be entered and left. In the latter case, GC 
 *	can occur.
 *
 *---------------------------------------------------------------------------
 */

void 
Col_PauseGC() {
    GcMemInfo *info = GetGcMemInfo();

    info->pauseGC++;
}
void 
Col_ResumeGC(void)
{
    GcMemInfo *info = GetGcMemInfo();
    size_t threshold;

    if (info->pauseGC > 1) {
	/* 
	 * Within nested sections. 
	 */

	info->pauseGC--;
	return;
    }

    threshold = info->pools[2].nbPages / GC_GEN_FACTOR;
    if (info->pools[0].nbAlloc + info->pools[1].nbAlloc 
	    >= GC_THRESHOLD(threshold)) {
	/* 
	 * GC is needed when the number of pages allocated since the last GC 
	 * exceed a given threshold.
	 */

	PerformGC();
    }

    /*
     * Leave protected section.
     */

    info->pauseGC = 0;
}

/*
 *---------------------------------------------------------------------------
 *
 * PerformGC --
 *
 *	Perform a GC starting from the Eden pool. This can propagate to older 
 *	generations each time their GC count reaches GC_GEN_FACTOR.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Cells and pages can be freed. New pages can be allocated, or a new 
 *	pool created, when promoting surviving ropes to older generations.
 *
 *---------------------------------------------------------------------------
 */

static void 
PerformGC() 
{
    GcMemInfo *info = GetGcMemInfo();
    unsigned int generation;
    size_t threshold;
    size_t i;

    /*
     * Traverse pools to check whether they must be collected. Root and eden 
     * pools are always collected. Clear the collected pools' bitmask.
     */

    for (generation = 0; generation < GC_MAX_GENERATIONS; generation++) {
	if (generation > 1) {
	    if (generation+1 < GC_MAX_GENERATIONS) {
		/*
		 * Intermediary generations. 
		 */

		threshold = info->pools[generation+1].nbPages / GC_GEN_FACTOR;
		if (info->pools[generation].nbAlloc < GC_THRESHOLD(threshold)) {
		    /*
		     * Stop if number of allocations is less than the threshold.
		     */

		    break;
		}
		info->pools[generation].gc++;
		if (info->pools[generation].gc < GC_GEN_FACTOR) {
		    /*
		     * Collection frequency is logarithmic.
		     */

		    break;
		}
	    } else {
		/*
		 * Ultimate generation.
		 */

		if (info->pools[generation].nbAlloc < GC_MAX_PAGE_ALLOC) {
		    break;
		}
	    }
	}

	info->maxCollectedGeneration = generation;

	/* 
	 * Clear bitmasks on pool. Reachable ropes will be marked again in the
	 * next step.
	 */

	ClearPoolBitmasks(info, generation);
    }

#ifdef PROMOTE_COMPACT
    if (info->maxCollectedGeneration+1 < GC_MAX_GENERATIONS
	    && info->pools[info->maxCollectedGeneration+1].nbPages > 0
	    && info->pools[info->maxCollectedGeneration+1].nbSetCells 
		    < (info->pools[info->maxCollectedGeneration+1].nbPages * CELLS_PER_PAGE)
		    * PROMOTE_PAGE_FILL_RATIO) {
	info->compactGeneration = info->maxCollectedGeneration;
    } else {
	info->compactGeneration = SIZE_MAX;
    }
#endif

    /* 
     * Mark all cells that are reachable from known roots and from parents of 
     * uncollected pools. This also marks still valid roots in the process.
     */

    MarkReachableCellsFromRoots(info);
    MarkReachableCellsFromParents(info);

    /*
     * Perform cleanup on all custom ropes and words that need sweeping.
     */

    SweepUnreachableCells(info);

    /*
     * Free empty pages from collected pools before promoting them.
     */

    for (generation = 0; generation <= info->maxCollectedGeneration; 
	    generation++) {
	PoolFreeEmptyPages(&info->pools[generation]);
    }

    /* 
     * At this point all reachable cells are set, and unreachable cells are 
     * cleared. Now promote whole pages the next generation. Older pools are 
     * promoted first so that newer ones are correctly merged when promoted.
     * Roots belong to the root pool (gen 0) and are never promoted.
     */

    for (generation = info->maxCollectedGeneration; generation >= 1; 
	    generation--) {
	PromotePages(info, generation);
    }

    /*
     * Finally, reset counters.
     */

    for (generation = 0; generation <= info->maxCollectedGeneration; 
	    generation++) {
	info->pools[generation].nbAlloc = 0;
	info->pools[generation].gc = 0;
	for (i = 0; i < AVAILABLE_CELLS; i++) {
	    info->pools[generation].lastFreeCell[i] 
		    = PAGE_CELL(info->pools[generation].pages, 0);
	}
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ClearPoolBitmasks --
 *
 *	Clear all bitmasks in the pool's pages.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Clear all bitmasks in the pool's page.
 *
 *---------------------------------------------------------------------------
 */

static void 
ClearPoolBitmasks(
    GcMemInfo *info,		/* Thread local info. */
    unsigned int generation)	/* The pool generation to traverse. */
{
    void * page;

    /* 
     * Clear all bitmasks in pool. 
     */

    for (page = info->pools[generation].pages; page; page = PAGE_NEXT(page)) {
	ASSERT(PAGE_GENERATION(page) == generation);
	ASSERT(PAGE_CHILDGENERATION(page) <= generation);
	PAGE_CHILDGENERATION(page) = generation;
	ClearAllCells(page);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * MarkReachableCellsFromRoots --
 *
 *	Mark all cells reachable from the valid roots.
 *
 *	Traversal will stop at cells from uncollected pools. Cells from these
 *	pools having children in collected pools will be traversed in the
 *	next phase (MarkReachableCellsFromParents).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Set all reachable cells in page bitmasks.
 *
 *---------------------------------------------------------------------------
 */

static void 
MarkReachableCellsFromRoots(
    GcMemInfo *info)		/* Thread local info. */
{
    Cell *node, *leaf, *parent, *source;

    node = info->roots;
    while (node) {
	SetCells(CELL_PAGE(node), CELL_INDEX(node), 1);
	if (!ROOT_IS_LEAF(node)) {
	    /*
	     * Descend into left subtrie.
	     */

	    node = ROOT_NODE_LEFT(node);
	    continue;
	}

	leaf = ROOT_GET_NODE(node);
	source = ROOT_LEAF_SOURCE(leaf);
	if (ROOT_LEAF_GENERATION(leaf) <= info->maxCollectedGeneration) {
	    if (CELL_TYPE(source)) {
		MarkWord(info, (Col_Word *) &source, CELL_PAGE(source));
	    } else {
		MarkRope(info, (Col_Rope *) &source, CELL_PAGE(source));
	    }
	    if (ROOT_LEAF_GENERATION(leaf)+1 < GC_MAX_GENERATIONS) {
		ROOT_LEAF_GENERATION(leaf)++;
	    }
	}

	/*
	 * Find next branch.
	 */

	parent = ROOT_PARENT(leaf);
	while (parent && ((uintptr_t) source & ROOT_NODE_MASK(parent))) {
	    parent = ROOT_PARENT(parent);
	}
	if (!parent) {
	    /*
	     * Reached end.
	     */

	    break;
	}
	node = ROOT_NODE_RIGHT(parent);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * MarkReachableCellsFromParents --
 *
 *	Mark all cells reachable from cells in pages with potentially younger
 *	children.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Set all reachable cells in page bitmasks.
 *
 *---------------------------------------------------------------------------
 */

static void 
MarkReachableCellsFromParents(
    GcMemInfo *info)		/* Thread local info. */
{
    unsigned int generation;
    Cell *root, **previousPtr, *cell;
    Page *page;
    size_t index, nbCells;

    /*
     * Iterate in reverse so that existing lists don't get overwritten.
     */

    /*
     * First, follow cells in parent pages from uncollected generations.
     */

    for (generation = GC_MAX_GENERATIONS-1; 
	    generation > info->maxCollectedGeneration; generation--) {
	for (previousPtr = &info->pools[generation].parents, 
		root = info->pools[generation].parents; root; 
		root = *previousPtr) {
	    page = PARENT_PAGE(root);

	    /*
	     * Iterate over cells in page and get min child generation.
	     */

	    ASSERT(PAGE_GENERATION(page) == generation);
	    ASSERT(PAGE_CHILDGENERATION(page) <= generation);
	    PAGE_CHILDGENERATION(page) = generation;
	    for (index = RESERVED_CELLS; index < AVAILABLE_CELLS; 
		    index += nbCells) {
		if (!TestCell(page, index)) {
		    nbCells = 1;
		    continue;
		}

		cell = PAGE_CELL(page, index);
		nbCells = GetNbCells(cell);

		/*
		 * Follow parent. Clearing the first cell is sufficient, as it will 
		 * be set again during the mark phase. 
		 */

		ClearCells(page, index, 1);
		if (CELL_TYPE(cell)) {
		    MarkWord(info, (Col_Word *) &cell, page);
		} else {
		    MarkRope(info, (Col_Rope *) &cell, page);
		}
	    }

	    if (PAGE_CHILDGENERATION(page) < generation) {
		/*
		 * Still a parent, keep root.
		 */

		SetCells(CELL_PAGE(root), CELL_INDEX(root), 1);
		previousPtr = &PARENT_NEXT(root);
	    } else {
		/*
		 * Remove obsolete parent root from list.
		 */

		*previousPtr = PARENT_NEXT(root);
	    }
	}
    }

    /*
     * At this point all reachable cells have been marked, either by direct 
     * roots or from their parents from uncollected generations. Now handle
     * collected parent pages, and move surviving parent roots to the next 
     * generation.
     */

    for (/* use last value of generation */; generation > 0; generation--) {
	for (previousPtr = &info->pools[generation].parents, 
		root = info->pools[generation].parents; root; 
		root = *previousPtr) {
	    page = PARENT_PAGE(root);
	    ASSERT(PAGE_GENERATION(page) == generation);
	    ASSERT(PAGE_CHILDGENERATION(page) <= generation);
	    if (PAGE_CHILDGENERATION(page) < generation) {
		/*
		 * Still a parent, keep root.
		 */

		SetCells(CELL_PAGE(root), CELL_INDEX(root), 1);
		previousPtr = &PARENT_NEXT(root);
	    } else {
		/*
		 * Remove obsolete parent root from list.
		 */

		*previousPtr = PARENT_NEXT(root);
	    }
	}

	/*
	 * Move still valid parent roots to the next generation: add to front.
	 */

	if (generation+1 >= GC_MAX_GENERATIONS) {
	    /*
	     * Can't promote past the last possible generation.
	     */

	    continue;
	}
	*previousPtr = info->pools[generation+1].parents;
	info->pools[generation+1].parents = info->pools[generation].parents;
	info->pools[generation].parents = NULL;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * MarkRope --
 * MarkWord --
 *
 *	Mark rope or word and all its children as reachable.
 *
 *	The algorithm is recursive and stops when it reaches an already set 
 *	cell. This handles loops and references to older pools, where cells are
 *	already set.
 *
 *	To limit stack growth, tail recursive using an infinite loop with 
 *	conditional return.
 *
 *	The rope or word is passed by pointer so that it can be updated in
 *	case of redirection.
 *
 *	If generationPtr is non-NULL, overwritten by the cell's generation if
 *	younger than caller.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Set all reachable cells in page bitmasks.
 *
 *---------------------------------------------------------------------------
 */

static void
MarkRopeCustomChild(
    Col_Rope rope,
    Col_Rope *childPtr,
    Col_ClientData clientData)
{
    MarkRope((GcMemInfo *) clientData, childPtr, CELL_PAGE(rope));
}
static void 
MarkRope(
    GcMemInfo *info,		/* Thread local info. */
    Col_Rope *ropePtr,		/* Rope to mark and follow. */
    Page *parentPage)		/* Youngest generation upon return. */
{
    int type;
    size_t nbCells, index;
    Page *page;

    /*
     * Entry point for tail recursive calls.
     */

#undef TAIL_RECURSE
#define TAIL_RECURSE(_ropePtr, _parentPage) \
    ropePtr = (_ropePtr); parentPage = (_parentPage); goto start;

start:

    type = ROPE_TYPE(*ropePtr);
    switch (type) {
	case ROPE_TYPE_NULL:
	case ROPE_TYPE_C:
	case ROPE_TYPE_EMPTY:
	    /* 
	     * No cell to mark. 
	     */

	    return;

#ifdef PROMOTE_COMPACT
	case ROPE_TYPE_REDIRECT:
	    /* 
	     * Overwrite reference and tail recurse on source.
	     */

	    *ropePtr = ROPE_REDIRECT_SOURCE(*ropePtr);
	    TAIL_RECURSE(ropePtr, parentPage);
#endif /* PROMOTE_COMPACT */
    }

    page = CELL_PAGE(*ropePtr);
    if (PAGE_CHILDGENERATION(parentPage) > PAGE_GENERATION(page)) {
	PAGE_CHILDGENERATION(parentPage) = PAGE_GENERATION(page);
    }

    /* 
     * Stop if cell is already marked. 
     */

    index = CELL_INDEX(*ropePtr);
    if (TestCell(page, index)) {
	return;
    }

    nbCells = GetNbCells(*ropePtr);
    
#ifdef PROMOTE_COMPACT
    if (PAGE_GENERATION(page) == info->compactGeneration 
	    && !ROPE_PINNED(*ropePtr)) {
	/*
	 * Unpinned ropes are moved to the next generation.
	 */

	Col_Rope promoted = (Col_Rope) PoolAllocCells(
		&info->pools[info->compactGeneration+1], nbCells);
	memcpy((void *) promoted, *ropePtr, nbCells * CELL_SIZE);

	/*
	 * Replace original by redirect.
	 */

	ROPE_SET_TYPE(*ropePtr, ROPE_TYPE_REDIRECT);
	ROPE_REDIRECT_SOURCE(*ropePtr) = promoted;

	/*
	 * Tail recurse on promoted. Clearing the first cell is sufficient, as 
	 * it will be set again during the mark phase. 
	 */

	ClearCells(CELL_PAGE(promoted), CELL_INDEX(promoted), 1);
	*ropePtr = promoted;
	TAIL_RECURSE(ropePtr, parentPage);
    }
#endif

    SetCells(page, index, (index+nbCells < CELLS_PER_PAGE ? nbCells 
	    : CELLS_PER_PAGE-index));

    /*
     * Follow children.
     *
     * Note: by construction, children of ropes other than custom are never
     * younger.
     */

    switch (type) {
	case ROPE_TYPE_SUBROPE:
	    /* 
	     * Tail recurse on source.
	     */

	    TAIL_RECURSE(&ROPE_SUBROPE_SOURCE(*ropePtr), page);
    	    
	case ROPE_TYPE_CONCAT:
	    /* 
	     * Follow left arm and tail recurse on right. 
	     */

	    MarkRope(info, &ROPE_CONCAT_LEFT(*ropePtr), page);
	    TAIL_RECURSE(&ROPE_CONCAT_RIGHT(*ropePtr), page);

	case ROPE_TYPE_CUSTOM:
	    if (ROPE_CUSTOM_TYPE(*ropePtr)->childrenProc) {
		/*
		 * Follow children.
		 */

		ROPE_CUSTOM_TYPE(*ropePtr)->childrenProc(*ropePtr, 
			MarkRopeCustomChild, info);
	    }
	    return;

	/* ROPE_TYPE_UNKNOWN */

	default:
	    /*
	     * Stop recursion.
	     */

	    return;
    }
}

static void
MarkWordChild(
    Col_Word word,
    Col_Word *childPtr,
    Col_ClientData clientData)
{
    MarkWord((GcMemInfo *) clientData, childPtr, CELL_PAGE(word));
}
static void 
MarkWord(
    GcMemInfo *info,		/* Thread local info. */
    Col_Word *wordPtr,		/* Word to mark and follow. */
    Page *parentPage)		/* Youngest generation upon return. */
{
    Col_WordType * typeInfo;
    int type;
    size_t nbCells, index;
    Page *page;

    /*
     * Entry point for tail recursive calls.
     */

#undef TAIL_RECURSE
#define TAIL_RECURSE(_wordPtr, _parentPage) \
    wordPtr = (_wordPtr); parentPage = (_parentPage); goto start;
//#undef TAIL_RECURSE
//#define TAIL_RECURSE(_wordPtr, _parentPage) MarkWord(info, (_wordPtr), (_parentPage)); return;

start:

    if (!*wordPtr || IS_IMMEDIATE(*wordPtr)) {
	/* 
	 * Immediate values. No cell to mark.
	 */

	return;
    }

    type = WORD_TYPE(*wordPtr);
    switch (type) {
	case WORD_TYPE_ROPE:
	    /*
	     * Word is rope.
	     */

	    {
		Col_Rope rope = WORD_ROPE_GET(*wordPtr);
		MarkRope(info, &rope, parentPage);
		*wordPtr = WORD_ROPE_NEW(rope);
	    }
	    return;

#ifdef PROMOTE_COMPACT
	case WORD_TYPE_REDIRECT:
	    /* 
	     * Overwrite reference and tail recurse on source.
	     */

	    *wordPtr = WORD_REDIRECT_SOURCE(*wordPtr);
	    TAIL_RECURSE(wordPtr, parentPage);
#endif /* PROMOTE_COMPACT */
    }

    page = CELL_PAGE(*wordPtr);
    if (PAGE_CHILDGENERATION(parentPage) > PAGE_GENERATION(page)) {
	PAGE_CHILDGENERATION(parentPage) = PAGE_GENERATION(page);
    }

    /* 
     * Stop if cell is already marked. 
     */

    index = CELL_INDEX(*wordPtr);
    if (TestCell(page, index)) {
	return;
    }

    nbCells = GetNbCells((const void *) *wordPtr);
    
#ifdef PROMOTE_COMPACT
    if (PAGE_GENERATION(page) == info->compactGeneration
	    && !WORD_PINNED(*wordPtr)) {
	/*
	 * Unpinned words are moved to the next generation.
	 */

	Col_Word promoted = (Col_Word) PoolAllocCells(
		&info->pools[info->compactGeneration+1], nbCells);
	memcpy((void *) promoted, (const void *) *wordPtr, nbCells * CELL_SIZE);

	/*
	 * Replace original by redirect.
	 */

	WORD_SET_TYPE_ID(*wordPtr, WORD_TYPE_REDIRECT);
	WORD_REDIRECT_SOURCE(*wordPtr) = promoted;

	/*
	 * Tail recurse on promoted. Clearing the first cell is sufficient, as 
	 * it will be set again during the mark phase. 
	 */

	ClearCells(CELL_PAGE(promoted), CELL_INDEX(promoted), 1);
	*wordPtr = promoted;
	TAIL_RECURSE(wordPtr, parentPage);
    }
#endif

    SetCells(page, index, (index+nbCells < CELLS_PER_PAGE ? nbCells 
	    : CELLS_PER_PAGE-index));

    /*
     * Follow children.
     *
     * Note: by construction, children of words other than custom are never
     * younger.
     */

    switch (type) {
	case WORD_TYPE_VECTOR:
	case WORD_TYPE_MVECTOR: {
	    /* 
	     * Follow elements.
	     */

	    size_t i, length = WORD_VECTOR_LENGTH(*wordPtr);
	    Col_Word *elements = WORD_VECTOR_ELEMENTS(*wordPtr);
	    for (i = 0; i < length; i++) {
		MarkWord(info, elements+i, page);
	    }

	    /*
	     * Continued below on synonym.
	     */

	    break;
	}

	case WORD_TYPE_LIST:
	case WORD_TYPE_MLIST:
	    /* 
	     * Follow list root.
	     */

	    MarkWord(info, &WORD_LIST_ROOT(*wordPtr), page);

	    /*
	     * Continued below on synonym.
	     */

	    break;

	case WORD_TYPE_SUBLIST:
	    /* 
	     * Tail recurse on source.
	     */

	    TAIL_RECURSE(&WORD_SUBLIST_SOURCE(*wordPtr), page);

	case WORD_TYPE_CONCATLIST:
	case WORD_TYPE_MCONCATLIST:
	    /* 
	     * Follow left arm and tail recurse on right. 
	     */

	    MarkWord(info, &WORD_CONCATLIST_LEFT(*wordPtr), page);
	    TAIL_RECURSE(&WORD_CONCATLIST_RIGHT(*wordPtr), page);

	case WORD_TYPE_REFERENCE:
	    /* 
	     * Follow source.
	     */

	    MarkWord(info, &WORD_REFERENCE_SOURCE(*wordPtr), page);

	    /*
	     * Continued below on synonym.
	     */

	    break;

	case WORD_TYPE_INTHASHMAP:
	    if (WORD_INTHASHMAP_BUCKETS(*wordPtr)) {
		/*
		 * Buckets are stored in a separate word.
		 */

		MarkWord(info, &WORD_INTHASHMAP_BUCKETS(*wordPtr), page);
	    } else {
		/*
		 * Buckets are stored inline.
		 */

		size_t i, length = WORD_INTHASHMAP_STATICBUCKETS_SIZE(*wordPtr);
		Col_Word *buckets = WORD_INTHASHMAP_STATICBUCKETS(*wordPtr);
		for (i = 0; i < length; i++) {
		    MarkWord(info, buckets+i, page);
		}
	    }

	    /*
	     * Continued below on synonym.
	     */

	    break;

	case WORD_TYPE_INTHASHENTRY:
	    /*
	     * Follow value and tail recurse on next.
	     */

	    MarkWord(info, &WORD_INTHASHENTRY_VALUE(*wordPtr), page);
	    TAIL_RECURSE(&WORD_INTHASHENTRY_NEXT(*wordPtr), page);

	case WORD_TYPE_INTTRIEMAP:
	    /* 
	     * Follow trie root.
	     */

	    MarkWord(info, &WORD_TRIEMAP_ROOT(*wordPtr), page);

	    /*
	     * Continued below on synonym.
	     */

	    break;

	case WORD_TYPE_TRIENODE:
	    /* 
	     * Follow left arm and tail recurse on right. 
	     */

	    MarkWord(info, &WORD_TRIENODE_LEFT(*wordPtr), page);
	    TAIL_RECURSE(&WORD_TRIENODE_RIGHT(*wordPtr), page);

	case WORD_TYPE_INTTRIELEAF:
	    /*
	     * Tail recurse on value.
	     */

	    TAIL_RECURSE(&WORD_INTTRIELEAF_VALUE(*wordPtr), page);

	case WORD_TYPE_REGULAR:
	    WORD_GET_TYPE_ADDR(*wordPtr, typeInfo);
	    if (typeInfo->childrenProc) {
		/*
		 * Follow children.
		 */

		typeInfo->childrenProc(*wordPtr, MarkWordChild, info);
	    }

	    /*
	     * Continued below on synonym.
	     */

	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /*
	     * Stop recursion.
	     */

	    return;
    }

    /*
     * Tail recurse on synonym.
     */

    TAIL_RECURSE(&WORD_SYNONYM(*wordPtr), page);
}

/*
 *---------------------------------------------------------------------------
 *
 * SweepUnreachableCells --
 *
 *	Perform cleanup for all custom ropes or words that need sweeping.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Calls each cleaned cell's freeProc.
 *
 *---------------------------------------------------------------------------
 */

static void 
SweepUnreachableCells(
    GcMemInfo *info)		/* Thread local info. */
{
    unsigned int generation;
    Cell *cell, **previousPtr;

    for (generation = 1; generation <= info->maxCollectedGeneration; 
	    generation++) {
	for (previousPtr = &info->pools[generation].sweepables, 
		cell = info->pools[generation].sweepables; cell; 
		cell = *previousPtr) {
	    /*
	     * Note: by construction, freeable words are custom ropes or regular 
	     * words (i.e. with a type pointer).
	     */

	    if (CELL_TYPE(cell) /* word */) {
		Col_Word word = (Col_Word) cell;
		Col_WordType * typeInfo;

#ifdef PROMOTE_COMPACT
		if (generation == info->compactGeneration) {
		    /* 
		     * Word was potentially redirected.
		     */

		    ResolveRedirectWord(&word);
		}
#endif
		ASSERT(WORD_TYPE(word) == WORD_TYPE_REGULAR);
		WORD_GET_TYPE_ADDR(word, typeInfo);

		if (!TestCell(CELL_PAGE(word), CELL_INDEX(word))) {
		    /*
		     * Cleanup.
		     */

		    typeInfo->freeProc(word);

		    /* 
		     * Remove from list. 
		     */

		    *previousPtr = WORD_NEXT(word, typeInfo->sizeProc(word));
		} else {
		    /*
		     * Keep, updating reference in case of redirection.
		     */

		    *previousPtr = (Cell *) word;

		    previousPtr = &WORD_NEXT(word, typeInfo->sizeProc(word));
		}
	    } else {
		Col_Rope rope = (Col_Rope) cell;

#ifdef PROMOTE_COMPACT
		if (generation == info->compactGeneration) {
		    /* 
		     * Rope was potentially redirected.
		     */

		    ResolveRedirectRope(&rope);
		}
#endif

		ASSERT(ROPE_TYPE(rope) == ROPE_TYPE_CUSTOM);

		if (!TestCell(CELL_PAGE(rope), CELL_INDEX(rope))) {
		    /*
		     * Cleanup.
		     */

		    ROPE_CUSTOM_TYPE(rope)->freeProc(rope);

		    /* 
		     * Remove from list. 
		     */

		    *previousPtr = (Cell *) ROPE_CUSTOM_NEXT(rope, 
			    ROPE_CUSTOM_SIZE(rope));
		} else {
		    /*
		     * Keep, updating reference in case of redirection.
		     */

		    *previousPtr = (Cell *) rope;

		    previousPtr = &ROPE_CUSTOM_NEXT(rope, 
			    ROPE_CUSTOM_SIZE(rope));
		}
	    }
	}
    }

    /*FIXME: promote surviving sweepables! */
}

/*
 *---------------------------------------------------------------------------
 *
 * PromotePages --
 *
 *	Promote non-empty pages to the next pool.
 *
 *	This simply adds the pool's pages to the target pool.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Move pages to an older pool and update their generation info. Update
 *	page counters as well.
 *
 *---------------------------------------------------------------------------
 */

static void 
PromotePages(
    GcMemInfo *info,		/* Thread local info. */
    unsigned int generation)	/* The pool generation to traverse. */
{
    void *page;
    size_t i;

    if (generation+1 >= GC_MAX_GENERATIONS) {
	/*
	 * Can't promote past the last possible generation.
	 */

	return;
    }

    if (!info->pools[generation].pages) {
	/*
	 * Nothing to promote.
	 */

	return;
    }

    /*
     * Update promoted pages' generation.
     */

    for (page = info->pools[generation].pages; page; page = PAGE_NEXT(page)) {
	ASSERT(PAGE_GENERATION(page) == generation);
	ASSERT(PAGE_CHILDGENERATION(page) <= generation);
	PAGE_GENERATION(page)++;
	PAGE_CHILDGENERATION(page)++;
    }

    /*
     * Move the promoted pages to the head of the target pool's page list.
     * Note that root & parent lists are already updated at this point.
     */

    ASSERT(info->pools[generation].lastPage);
    PAGE_NEXT(info->pools[generation].lastPage) 
	    = info->pools[generation+1].pages;
    info->pools[generation+1].pages = info->pools[generation].pages;
    if (!info->pools[generation+1].lastPage) {
	info->pools[generation+1].lastPage = info->pools[generation].lastPage;
    }
    info->pools[generation+1].nbPages += info->pools[generation].nbPages;
    info->pools[generation+1].nbAlloc += info->pools[generation].nbPages;
    info->pools[generation+1].nbSetCells += info->pools[generation].nbSetCells;
    for (i = 0; i < AVAILABLE_CELLS; i++) {
	info->pools[generation+1].lastFreeCell[i] 
		= PAGE_CELL(info->pools[generation+1].pages, 0);
    }

    info->pools[generation].pages = NULL;
    info->pools[generation].lastPage = NULL;
    info->pools[generation].nbPages = 0;
    info->pools[generation].nbAlloc = 0;
    info->pools[generation].nbSetCells = 0;}
