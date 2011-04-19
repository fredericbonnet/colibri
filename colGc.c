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

static size_t		GetNbCells(Col_Word word);
static void		PerformGC(void);
static void		ClearPoolBitmasks(GcMemInfo *info, 
			    unsigned int generation);
static void		MarkReachableCellsFromRoots(GcMemInfo *info);
static void		MarkReachableCellsFromParents(GcMemInfo *info,
			    Cell *parents);
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
    Col_Word word)		/* The word. */
{
    /*
     * Get number of cells taken by word.
     * Note: word is not immediate.
     */

    switch (WORD_TYPE(word)) {
	case WORD_TYPE_UCSSTR:
	    return UCSSTR_SIZE(WORD_UCSSTR_LENGTH(word)
		    * WORD_UCSSTR_FORMAT(word));

	case WORD_TYPE_UTF8STR:
	    return UTF8STR_SIZE(WORD_UTF8STR_BYTELENGTH(word));

	case WORD_TYPE_VECTOR:
	    return VECTOR_SIZE(WORD_VECTOR_LENGTH(word));

	case WORD_TYPE_MVECTOR:
	    return WORD_MVECTOR_SIZE(word);

	case WORD_TYPE_CUSTOM: {
	    Col_CustomWordType *typeInfo = WORD_TYPEINFO(word);
	    return WORD_CUSTOM_SIZE(typeInfo->sizeProc(word), typeInfo);
	}

	case WORD_TYPE_STRHASHMAP:
	case WORD_TYPE_INTHASHMAP:
	    return HASHMAP_NBCELLS;

	/* WORD_TYPE_UNKNOWN */

	default:
	    return 1;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_WordSetModified --
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
Col_WordSetModified(
    Col_Word word)			/* Modified word. */
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

    page = CELL_PAGE(word);
    generation = PAGE_GENERATION(page);
    if (PAGE_PARENT(page)) {
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
     * Create a new parent root, and insert at head of parent root list.
     */

    PAGE_PARENT(page) = 1;
    root = PoolAllocCells(&info->pools[0], 1);
    PARENT_INIT(root, info->parents, page, generation);
    info->parents = root;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_PreserveWord --
 * Col_ReleaseWord --
 *
 *	Preserve (resp. release) a persistent reference to a word, which 
 *	prevents its collection. This allows words to be safely stored in 
 *	external structures regardless of memory management cycles.
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
Col_PreserveWord(
    Col_Word word)		/* The word to preserve. */
{
    GcMemInfo *info = GetGcMemInfo();
    Cell *node, *leaf, *parent, *newParent;
    Col_Word leafSource;
    uintptr_t mask;

    if (!info->pauseGC) {
	/*
	 * Can't be called outside of a GC protected section.
	 */

	Col_Error(COL_ERROR, "Called outside of a GC-protected section");
	return;
    }

    /* 
     * Rule out immediate values. 
     */

    switch (WORD_TYPE(word)) {
	case WORD_TYPE_NIL:
	case WORD_TYPE_SMALLINT:
	case WORD_TYPE_CHAR:
	case WORD_TYPE_SMALLSTR:
	case WORD_TYPE_VOIDLIST:
	    /* 
	     * Immediate values. 
	     */

	    return;
    }

    /*
     * Search for matching entry in root trie.
     */

    node = info->roots;
    while (node) {
	if (!ROOT_IS_LEAF(node)) {
	    if ((uintptr_t) word & ROOT_NODE_MASK(node)) {
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
	if (word == leafSource) {
	    /*
	     * Found! Increment reference count.
	     */

	    ASSERT(WORD_PINNED(word));
	    ROOT_LEAF_REFCOUNT(node)++;
	    return;
	}
	break;
    }

    /*
     * Not found, insert. Pin the word so that its address doesn't change 
     * during compaction.
     */

    WORD_SET_PINNED(word);

    leaf = PoolAllocCells(&info->pools[0], 1);
    ROOT_LEAF_INIT(leaf, NULL, PAGE_GENERATION(CELL_PAGE(word)), 1, word);

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

    mask = (uintptr_t) word ^ (uintptr_t) leafSource;
    ASSERT(mask);
    mask |= mask >> 1;
    mask |= mask >> 2;
    mask |= mask >> 4;
    mask |= mask >> 8;
    mask |= mask >> 16;
    mask >>= 1;
    mask++;
    ASSERT(mask);

    /*
     * Find insertion point.
     */

    node = info->roots;
    while (node) {
	if (!ROOT_IS_LEAF(node) && mask < ROOT_NODE_MASK(node)) {
	    if ((uintptr_t) word & ROOT_NODE_MASK(node)) {
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
    newParent = PoolAllocCells(&info->pools[0], 1);
    if ((uintptr_t) word & mask) {
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
    } else if ((uintptr_t) word & ROOT_NODE_MASK(parent)) {
	ROOT_NODE_RIGHT(parent) = newParent;
    } else {
	ROOT_NODE_LEFT(parent) = newParent;
    }
}

void
Col_ReleaseWord(
    Col_Word word)		/* The word to release. */
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

    /* 
     * Rule out immediate values. 
     */

    switch (WORD_TYPE(word)) {
	case WORD_TYPE_NIL:
	case WORD_TYPE_SMALLINT:
	case WORD_TYPE_CHAR:
	case WORD_TYPE_SMALLSTR:
	case WORD_TYPE_VOIDLIST:
	    /* 
	     * Immediate values. 
	     */

	    return;
    }

    if (!WORD_PINNED(word)) {
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
	    if ((uintptr_t) word & ROOT_NODE_MASK(node)) {
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
	if (word != ROOT_LEAF_SOURCE(node)) {
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
     * Remove leaf. Unpin word as well.
     */

    ASSERT(TestCell(CELL_PAGE(node), CELL_INDEX(node)));
    ClearCells(CELL_PAGE(node), CELL_INDEX(node), 1);
    WORD_CLEAR_PINNED(word);

    parent = ROOT_PARENT(node);
    if (!parent) {
	/*
	 * Leaf was trie root.
	 */

	info->roots = NULL;
	return;
    }

    /*
     * Remove parent and replace by sibling.
     */

    ASSERT(TestCell(CELL_PAGE(parent), CELL_INDEX(parent)));
    ClearCells(CELL_PAGE(parent), CELL_INDEX(parent), 1);
    if ((uintptr_t) word & ROOT_NODE_MASK(parent)) {
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
    } else if ((uintptr_t) word & ROOT_NODE_MASK(grandParent)) {
	ROOT_NODE_RIGHT(grandParent) = sibling;
    } else {
	ROOT_NODE_LEFT(grandParent) = sibling;
    }
    ROOT_PARENT(ROOT_GET_NODE(sibling)) = grandParent;
}



/*
 *---------------------------------------------------------------------------
 *
 * DeclareCustomWord --
 *
 *	Remember custom words needing freeing upon deletion. This chains such 
 *	words in their order of creation, latest being inserted at the head of 
 *	the list. This implies that cleanup can stop traversing the list at the 
 *	first custom rope that belongs to a non GC'd pool.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Word fields may be updated.
 *
 *---------------------------------------------------------------------------
 */

void 
DeclareCustomWord(
    Col_Word word)		/* The word to declare. */
{
    if (WORD_TYPE(word) == WORD_TYPE_CUSTOM) {
	Col_CustomWordType *typeInfo = WORD_TYPEINFO(word);
	if (typeInfo->freeProc) {
	    /*
	     * Type needs freeing. Remember by inserting at head of list.
	     */

	    GcMemInfo *info = GetGcMemInfo();
	    unsigned int generation = PAGE_GENERATION(CELL_PAGE(word));
	    WORD_CUSTOM_NEXT(word) = info->pools[generation].sweepables;
	    info->pools[generation].sweepables = word;
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
    Cell *parents;

    /*
     * Traverse pools to check whether they must be collected. Eden pool is
     * always collected. Root pool uses specific management.
     */

    for (generation = 1; generation < GC_MAX_GENERATIONS; generation++) {
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
	 * Clear bitmasks on pool. Reachable words will be marked again in the
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
     * Reset the parent page list, the actual list will be rebuilt during the
     * mark phase.
     */

    parents = info->parents;
    info->parents = NULL;

    /* 
     * Mark all cells that are reachable from known roots and from parents of 
     * uncollected pools. This also marks still valid roots in the process.
     */

    MarkReachableCellsFromRoots(info);
    MarkReachableCellsFromParents(info, parents);

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
	PAGE_PARENT(page) = 0;
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
    Cell *node, *leaf, *parent;
    Col_Word source;

    node = info->roots;
    while (node) {
	ASSERT(TestCell(CELL_PAGE(node), CELL_INDEX(node)));
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
	    /*
	     * Follow root from collected generation.
	     */

	    MarkWord(info, &source, CELL_PAGE(source));
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
    GcMemInfo *info,		/* Thread local info. */
    Cell *parents)		/* Parent list. */
{
    unsigned int generation;
    Col_Word word;
    Page *page;
    size_t index, nbCells;

    /*
     * Follow cells in parent pages from uncollected generations, destroying
     * the list in the process (i.e. clearing the cells) since the actual list
     * is rebuilt during the mark phase.
     */

    while (parents) {
	ASSERT(TestCell(CELL_PAGE(parents), CELL_INDEX(parents)));
	ClearCells(CELL_PAGE(parents), CELL_INDEX(parents), 1);

	page = PARENT_PAGE(parents);
	generation = PARENT_GENERATION(parents);

	parents = PARENT_NEXT(parents);

	if (generation <= info->maxCollectedGeneration) {
	    continue;
	}

	/*
	 * Iterate over cells in page.
	 */

	ASSERT(PAGE_PARENT(page));
	PAGE_PARENT(page) = 0;
	for (index = RESERVED_CELLS; index < CELLS_PER_PAGE; index += nbCells) {
	    if (!TestCell(page, index)) {
		nbCells = 1;
		continue;
	    }

	    word = (Col_Word) PAGE_CELL(page, index);
	    nbCells = GetNbCells(word);

	    /*
	     * Follow parent. Clearing the first cell is sufficient, as it will 
	     * be set again during the mark phase. 
	     */

	    ClearCells(page, index, 1);
	    MarkWord(info, &word, page);
	}
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * MarkWord --
 *
 *	Mark word and all its children as reachable.
 *
 *	The algorithm is recursive and stops when it reaches an already set 
 *	cell. This handles loops and references to older pools, where cells are
 *	already set.
 *
 *	To limit stack growth, tail recursive using an infinite loop with 
 *	conditional return.
 *
 *	The word is passed by pointer so that it can be updated in case of 
 *	redirection.
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
    int type;
    size_t nbCells, index;
    Page *page;

    /*
     * Entry point for tail recursive calls.
     */

#define TAIL_RECURSE(_wordPtr, _parentPage) \
    wordPtr = (_wordPtr); parentPage = (_parentPage); goto start;

start:

    if (!*wordPtr || IS_IMMEDIATE(*wordPtr)) {
	/* 
	 * Immediate values. No cell to mark.
	 */

	return;
    }

    type = WORD_TYPE(*wordPtr);
#ifdef PROMOTE_COMPACT
    if (type == WORD_TYPE_REDIRECT) {
	/* 
	 * Overwrite reference and tail recurse on source.
	 */

	*wordPtr = WORD_REDIRECT_SOURCE(*wordPtr);
	TAIL_RECURSE(wordPtr, parentPage);
    }
#endif /* PROMOTE_COMPACT */

    page = CELL_PAGE(*wordPtr);
    if (PAGE_GENERATION(page) < PAGE_GENERATION(parentPage) 
	    && !PAGE_PARENT(parentPage)) {
	/* 
	 * Create a new parent root, and insert at head of parent root list.
	 */

	Cell *root = PoolAllocCells(&info->pools[0], 1);
	PAGE_PARENT(parentPage) = 1;
	PARENT_INIT(root, info->parents, parentPage, 
		PAGE_GENERATION(parentPage));
	info->parents = root;
    }

    /* 
     * Stop if cell is already marked. 
     */

    index = CELL_INDEX(*wordPtr);
    if (TestCell(page, index)) {
	return;
    }

    nbCells = GetNbCells(*wordPtr);
    
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

	WORD_SET_TYPEID(*wordPtr, WORD_TYPE_REDIRECT);
	WORD_REDIRECT_SOURCE(*wordPtr) = promoted;

	/*
	 * Tail recurse on promoted. Clearing the first cell is sufficient, as 
	 * it will be set again during the mark phase. 
	 */

	ClearCells(CELL_PAGE(promoted), CELL_INDEX(promoted), 1);
	ASSERT(!TestCell(CELL_PAGE(promoted), CELL_INDEX(promoted)));
	*wordPtr = promoted;
	TAIL_RECURSE(wordPtr, parentPage);
    }
#endif

    ASSERT(!TestCell(page, index));
    SetCells(page, index, (index+nbCells < CELLS_PER_PAGE ? nbCells 
	    : CELLS_PER_PAGE-index));
    ASSERT(TestCell(page, index));

    /*
     * Follow children.
     */

    switch (type) {
	case WORD_TYPE_WRAP:
	    /*
	     * Follow source and tail recurse on synonym.
	     */

	    MarkWord(info, &WORD_WRAP_SOURCE(*wordPtr), page);
	    TAIL_RECURSE(&WORD_SYNONYM(*wordPtr), page);

	case WORD_TYPE_UCSSTR:
	case WORD_TYPE_UTF8STR:
	    return;

	case WORD_TYPE_SUBROPE:
	    /* 
	     * Tail recurse on source.
	     */

	    TAIL_RECURSE(&WORD_SUBROPE_SOURCE(*wordPtr), page);
    	    
	case WORD_TYPE_CONCATROPE:
	    /* 
	     * Follow left arm and tail recurse on right. 
	     */

	    MarkWord(info, &WORD_CONCATROPE_LEFT(*wordPtr), page);
	    TAIL_RECURSE(&WORD_CONCATROPE_RIGHT(*wordPtr), page);

	case WORD_TYPE_INT:
	    /*
	     * Tail recurse on synonym.
	     */

	    TAIL_RECURSE(&WORD_SYNONYM(*wordPtr), page);

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
	    return;
	}

	case WORD_TYPE_LIST:
	case WORD_TYPE_MLIST:
	    /* 
	     * Follow list root and tail recurse on synonym.
	     */

	    MarkWord(info, &WORD_LIST_ROOT(*wordPtr), page);
	    TAIL_RECURSE(&WORD_SYNONYM(*wordPtr), page);

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

	case WORD_TYPE_STRHASHMAP:
	case WORD_TYPE_INTHASHMAP:
	    if (WORD_HASHMAP_BUCKETS(*wordPtr)) {
		/*
		 * Buckets are stored in a separate word.
		 */

		MarkWord(info, &WORD_HASHMAP_BUCKETS(*wordPtr), page);
	    } else {
		/*
		 * Buckets are stored inline.
		 */

		size_t i;
		Col_Word *buckets = WORD_HASHMAP_STATICBUCKETS(*wordPtr);
		for (i = 0; i < HASHMAP_STATICBUCKETS_SIZE; i++) {
		    MarkWord(info, buckets+i, page);
		}
	    }

	    /*
	     * Tail recurse on synonym.
	     */

	    TAIL_RECURSE(&WORD_SYNONYM(*wordPtr), page);

	case WORD_TYPE_MAPENTRY:
	    /*
	     * Follow key & value and tail recurse on next.
	     */

	    MarkWord(info, &WORD_MAPENTRY_KEY(*wordPtr), page);
	    MarkWord(info, &WORD_MAPENTRY_VALUE(*wordPtr), page);
	    TAIL_RECURSE(&WORD_MAPENTRY_NEXT(*wordPtr), page);

	case WORD_TYPE_INTMAPENTRY:
	    /*
	     * Follow value and tail recurse on next.
	     */

	    MarkWord(info, &WORD_MAPENTRY_VALUE(*wordPtr), page);
	    TAIL_RECURSE(&WORD_MAPENTRY_NEXT(*wordPtr), page);

	case WORD_TYPE_STRTRIEMAP:
	case WORD_TYPE_INTTRIEMAP:
	    /* 
	     * Follow trie root and tail recurse on synonym.
	     */

	    MarkWord(info, &WORD_TRIEMAP_ROOT(*wordPtr), page);
	    TAIL_RECURSE(&WORD_SYNONYM(*wordPtr), page);

	case WORD_TYPE_STRTRIENODE:
	case WORD_TYPE_INTTRIENODE:
	    /* 
	     * Follow left arm and tail recurse on right. 
	     */

	    MarkWord(info, &WORD_TRIENODE_LEFT(*wordPtr), page);
	    TAIL_RECURSE(&WORD_TRIENODE_RIGHT(*wordPtr), page);

	case WORD_TYPE_CUSTOM: {
	    Col_CustomWordType *typeInfo = WORD_TYPEINFO(*wordPtr);
	    if (typeInfo->childrenProc) {
		/*
		 * Follow children.
		 */

		typeInfo->childrenProc(*wordPtr, MarkWordChild, info);
	    }
	    return;
	}

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
	    return;
    }
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
    Col_Word word, *previousPtr;
    Col_CustomWordType *typeInfo;

    for (generation = info->maxCollectedGeneration; generation > 0;
	    generation--) {
	for (previousPtr = &info->pools[generation].sweepables, 
		word = info->pools[generation].sweepables; word; 
		word = *previousPtr) {
	    /*
	     * Note: by construction, freeable words are custom words (i.e. 
	     * with a type pointer).
	     */

#ifdef PROMOTE_COMPACT
	    if (generation == info->compactGeneration 
		    && WORD_TYPE(word) == WORD_TYPE_REDIRECT) {
		/* 
		 * Word was redirected.
		 */

		word = WORD_REDIRECT_SOURCE(word);
	    }
#endif
	    ASSERT(WORD_TYPE(word) == WORD_TYPE_CUSTOM);
	    typeInfo = WORD_TYPEINFO(word);
	    ASSERT(typeInfo->freeProc);

	    if (!TestCell(CELL_PAGE(word), CELL_INDEX(word))) {
		/*
		 * Cleanup.
		 */

		typeInfo->freeProc(word);

		/* 
		 * Remove from list. 
		 */

		*previousPtr = WORD_CUSTOM_NEXT(word);
	    } else {
		/*
		 * Keep, updating reference in case of redirection.
		 */

		*previousPtr = word;

		previousPtr = &WORD_CUSTOM_NEXT(word);
	    }
	}

	/*
	 * Move still valid freeable words to the next generation: add to front.
	 */

	if (generation+1 >= GC_MAX_GENERATIONS) {
	    /*
	     * Can't promote past the last possible generation.
	     */

	    continue;
	}
	*previousPtr = info->pools[generation+1].sweepables;
	info->pools[generation+1].sweepables 
		= info->pools[generation].sweepables;
	info->pools[generation].sweepables = WORD_NIL;
    }
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
	PAGE_GENERATION(page)++;
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
    info->pools[generation].nbSetCells = 0;
}
