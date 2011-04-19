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
#include "colInt.h"
#include "colPlat.h"

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
static void *		PoolAllocCells(MemoryPool *pool, size_t number);
static void		PerformGC(void);
static void		ClearPoolBitmasks(GcMemInfo *info, 
			    unsigned int generation);
static void		MarkReachableCellsFromRoots(GcMemInfo *info);
static void		MarkReachableCellsFromParents(GcMemInfo *info);
static void		MarkRope(GcMemInfo *info, Col_Rope *ropePtr, 
			    unsigned int *generationPtr);
static void		MarkWord(GcMemInfo *info, Col_Word *wordPtr, 
			    unsigned int *generationPtr);
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
	Col_Word word = cell;
	Col_WordType * typeInfo;

	/*
	 * Get number of cells taken by word.
	 * Note: word is neither immediate nor rope.
	 */

	switch (WORD_TYPE(word)) {
	    case WORD_TYPE_VECTOR:
		return NB_CELLS(WORD_HEADER_SIZE 
			+ WORD_VECTOR_LENGTH(word)*sizeof(Col_Word));

	    case WORD_TYPE_REGULAR:
		WORD_GET_TYPE_ADDR(word, typeInfo);
		return NB_CELLS(WORD_HEADER_SIZE + typeInfo->sizeProc(word)
			+ (typeInfo->freeProc?WORD_TRAILER_SIZE:0));

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
		return NB_CELLS(ROPE_UCS_HEADER_SIZE + ROPE_UCS_LENGTH(rope));

	    case ROPE_TYPE_UCS2:
		return NB_CELLS(ROPE_UCS_HEADER_SIZE + ROPE_UCS_LENGTH(rope)*2);

	    case ROPE_TYPE_UCS4:
		return NB_CELLS(ROPE_UCS_HEADER_SIZE + ROPE_UCS_LENGTH(rope)*4);

	    case ROPE_TYPE_UTF8:
		return NB_CELLS(ROPE_UTF8_HEADER_SIZE 
			+ ROPE_UTF8_BYTELENGTH(rope));

	    case ROPE_TYPE_CUSTOM:
		return NB_CELLS(ROPE_CUSTOM_HEADER_SIZE + ROPE_CUSTOM_SIZE(rope) 
			+ (ROPE_CUSTOM_TYPE(rope)->freeProc?ROPE_CUSTOM_TRAILER_SIZE:0));

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

#define ResolveRedirectRope(ropePtr)			\
{							\
    if (ROPE_TYPE(*(ropePtr)) == ROPE_TYPE_REDIRECT) {	\
	*(ropePtr) = REDIRECT_SOURCE(*(ropePtr));	\
    }							\
}
#define ResolveRedirectWord(wordPtr)			\
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
	    *(wordPtr) = REDIRECT_SOURCE(*wordPtr);	\
	    break;					\
    }							\
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_DeclareChild --
 *
 *	Declare a rope or word as a child of another one.
 *
 *	As the GC uses a generational scheme, it must keep trace of cells
 *	from older pools that refer to newer ones. So custom ropes or words 
 *	that create children dynamically must declare them so that these 
 *	children don't get collected by mistake.
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
Col_DeclareChild(
    const void *parent,		/* Parent rope or word. */
    const void *child)		/* Child rope or word. */
{
    GcMemInfo *info = GetGcMemInfo();
    unsigned int parentGen, childGen;
    void * root;

    if (!info->pauseGC) {
	/*
	 * Can't be called outside of a GC protected section.
	 */

	/* TODO: exception */
	return;
    }

    if (CELL_TYPE(parent) /* word */) {
	/*
	 * Parent is word, child is either immediate, rope or word.
	 */

	if (WORD_PARENT(parent)) {
	    /*
	     * Already a parent.
	     */

	    return;
	} else if (IS_IMMEDIATE(child)) {
	    /*
	     * Not cell-based.
	     */

	    return;
	} else if (IS_ROPE(child)) {
	    /*
	     * Child is rope.
	     */

	    Col_Rope rope = WORD_ROPE_GET(child);
	    RESOLVE_ROPE(rope);
	    switch (ROPE_TYPE(rope)) {
		case ROPE_TYPE_NULL:
		case ROPE_TYPE_C:
		case ROPE_TYPE_EMPTY:
		    /* 
		     * Unmanaged strings. 
		     */

		    return;
	    }
	    childGen = PAGE_GENERATION(CELL_PAGE(rope));
	} else {
	    /*
	     * Child is word.
	     */

	    RESOLVE_WORD(*(Col_Word *) &child);
	    childGen = PAGE_GENERATION(CELL_PAGE(child));
	}
    } else {
	/*
	 * Parent is rope, child is rope too (inc. C strings).
	 */

	if (ROPE_PARENT(parent)) {
	    /*
	     * Already a parent.
	     */

	    return;
	}

	RESOLVE_ROPE(*(Col_Rope *) &child);
	switch (ROPE_TYPE((Col_Rope) child)) {
	    case ROPE_TYPE_NULL:
	    case ROPE_TYPE_C:
	    case ROPE_TYPE_EMPTY:
		/* 
		 * Unmanaged strings. 
		 */

		return;
	}
	childGen = PAGE_GENERATION(CELL_PAGE(child));
    }
    RESOLVE_WORD(*(Col_Word *) &parent);
    parentGen = PAGE_GENERATION(CELL_PAGE(parent));

    /*
     * Compare generations.
     */

    if (parentGen <= childGen) {
	/*
	 * Parent belongs to the same or newer generation as child, no need to 
	 * keep trace.
	 */

	return;
    }

    /* 
     * Create a new parent root, and remember parent as such.
     */

    root = PoolAllocCells(&info->pools[0], 1);
    if (CELL_TYPE(parent) /* word */) {
	WORD_SET_TYPE_ID(root, WORD_TYPE_PARENT);
	WORD_SET_PARENT(parent);
    } else {
	ROPE_SET_TYPE(root, ROPE_TYPE_PARENT);
	ROPE_SET_PARENT(parent);
    }
    PARENT_CELL(root) = parent;

    /* 
     * Insert in parent root list for the parent generation.
     */

    PARENT_NEXT(root) = info->pools[parentGen].parents;
    info->pools[parentGen].parents = root;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_PreserveRope --
 * Col_ReleaseRope --
 * Col_PreserveWord --
 * Col_ReleaseWord --
 *
 *	Preserve (resp. release) a persistent reference to a rope or word, 
 *	which prevents its collection. This creates a root cell that points to 
 *	the original cell. This allows ropes or words to be safely stored in 
 *	external structures regardless of memory management cycles.
 *
 *	Calls can be nested. A reference count is updated accordingly.
 *
 * Results:
 *	The resulting rope or word, which can be a root if the final reference 
 *	count is positive, or the original if it drops to zero.
 *
 * Side effects:
 *	Fields may be updated, and memory cells may be allocated.
 *
 *---------------------------------------------------------------------------
 */

Col_Rope 
Col_PreserveRope(
    Col_Rope rope)		/* The rope to preserve. */
{
    GcMemInfo *info = GetGcMemInfo();
    Col_Rope root;
    unsigned int generation;
    int type;

    if (!info->pauseGC) {
	/*
	 * Can't be called outside of a GC protected section.
	 */

	/* TODO: exception */
	return NULL;
    }

    type = ROPE_TYPE(rope);
    switch (type) {
	case ROPE_TYPE_NULL:
	case ROPE_TYPE_C:
	case ROPE_TYPE_EMPTY:
	    /* 
	     * Unmanaged strings. 
	     */

	    return rope;

	case ROPE_TYPE_ROOT:
	    /* 
	     * Rope is already root. Increment refcount. 
	     */

	    ROOT_REFCOUNT(rope)++;
	    return rope;
    }

    /* 
     * Create a new root rope.
     */

    root = PoolAllocCells(&info->pools[0], 1);
    ROPE_SET_TYPE(root, ROPE_TYPE_ROOT);
    ROOT_REFCOUNT(root) = 1;
    ROOT_SOURCE(root) = rope;

    /* 
     * Insert in head of the root list. 
     */

    generation = PAGE_GENERATION(CELL_PAGE(rope));
    ROOT_NEXT(root) = info->pools[generation].roots;
    info->pools[generation].roots = root;

    return root;
}

Col_Rope 
Col_ReleaseRope(
    Col_Rope rope)		/* The rope to release. */
{
    if (ROPE_TYPE(rope) != ROPE_TYPE_ROOT) {
	return rope;
    }
    if (ROOT_REFCOUNT(rope) > 1) {
	/* 
	 * Root is still valid after call. Decrement refcount. 
	 */

	ROOT_REFCOUNT(rope)--;
	return rope;
    }

    /* 
     * Release root. It will be removed from the list during GC.
     */

    ROOT_REFCOUNT(rope) = 0;
    return ROOT_SOURCE(rope);
}

Col_Word 
Col_PreserveWord(
    Col_Word word)		/* The word to preserve. */
{
    GcMemInfo *info = GetGcMemInfo();
    Col_Word root;
    unsigned int generation;
    int type;

    if (!info->pauseGC) {
	/*
	 * Can't be called outside of a GC protected section.
	 */

	/* TODO: exception */
	return NULL;
    }

    type = WORD_TYPE(word);

    switch (type) {
	case WORD_TYPE_NULL:
	case WORD_TYPE_SMALL_INT:
	case WORD_TYPE_SMALL_STRING:
	case WORD_TYPE_CHAR:
	    /* 
	     * Immediate values. 
	     */

	    return word;

	case WORD_TYPE_ROOT:
	    /* 
	     * Word is already root. Increment refcount. 
	     */

	    ROOT_REFCOUNT(word)++;
	    return word;
    }

    /* 
     * Create a new root word.
     */

    root = PoolAllocCells(&info->pools[0], 1);
    WORD_SET_TYPE_ID(root, WORD_TYPE_ROOT);
    ROOT_REFCOUNT(root) = 1;
    ROOT_SOURCE(root) = word;

    /* 
     * Insert in head of the root list. 
     */

    generation = PAGE_GENERATION(CELL_PAGE(word));
    ROOT_NEXT(root) = info->pools[generation].roots;
    info->pools[generation].roots = root;

    return root;
}

Col_Word 
Col_ReleaseWord(
    Col_Word word)		/* The word to release. */
{
    if (WORD_TYPE(word) != WORD_TYPE_ROOT) {
	return word;
    }
    if (ROOT_REFCOUNT(word) > 1) {
	/* 
	 * Root is still valid after call. Decrement refcount. 
	 */

	ROOT_REFCOUNT(word)--;
	return word;
    }

    /* 
     * Release root. It will be removed from the list during GC.
     */

    ROOT_REFCOUNT(word) = 0;
    return ROOT_SOURCE(word);
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
	 * Type needs freeing. Remember by inserting in head of list.
	 */

	GcMemInfo *info = GetGcMemInfo();
	unsigned int generation = PAGE_GENERATION(CELL_PAGE(rope));
	ROPE_CUSTOM_NEXT(rope, ROPE_CUSTOM_SIZE(rope)) 
		= info->pools[generation].sweepables;
	info->pools[generation].sweepables = rope;
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
	     * Type needs freeing. Remember by inserting in head of list.
	     */

	GcMemInfo *info = GetGcMemInfo();
	    unsigned int generation = PAGE_GENERATION(CELL_PAGE(word));
	    WORD_NEXT(word, typeInfo->sizeProc(word))
		= info->pools[generation].sweepables;
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

void *
AllocCells(
    size_t number)		/* Number of cells to allocate. */
{
    GcMemInfo *info = GetGcMemInfo();

    if (!info->pauseGC) {
	/*
	 * Can't be called outside of a GC protected section.
	 */

	/* TODO: exception */
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
 * PoolAllocCells --
 *
 *	Allocate cells in a pool, allocating new pages if needed. 
 *
 *	The maximum number of cells is AVAILABLE_CELLS.
 *
 * Results:
 *	If successful, a pointer to the first allocated cell. Else NULL.
 *	
 * Side effects:
 *	Alloc structures are updated, and memory pages may be allocated. The
 *	memory pool's alloc counter is incremented in the latter case.
 *
 *---------------------------------------------------------------------------
 */

static void * 
PoolAllocCells(
    MemoryPool *pool,		/* Pool into which to allocate cells. */
    size_t number)		/* Number of cells to allocate. */
{
    void *cells;
    void *tail;
    size_t i;
    
    if (!pool->pages) {
	/*
	 * Alloc first pages in pool.
	 */

	PoolAllocPages(pool, &pool->pages);
	for (i = 0; i < AVAILABLE_CELLS; i++) {
	    pool->lastFreePage[i] = pool->pages;
	}
    }
    cells = PageAllocCells(number, pool->lastFreePage[number-1], &tail);
    if (cells) {
	/* 
	 * Remember the cell page as the last one where a sequence of <number>
	 * cells was found. This avoids having to search into previous pages. 
	 */

	pool->lastFreePage[number-1] = CELL_PAGE(cells);
	return cells;
    }

    /* 
     * Allocate new pages then retry. 
     */

    PoolAllocPages(pool, &PAGE_NEXT(tail));
    if (!PAGE_NEXT(tail)) {
	/* TODO: exception out of memory? */ 
	return NULL;
    }
    cells = PageAllocCells(number, PAGE_NEXT(tail), &tail);
    /* ASSERT(cells) */

    /* 
     * Cell sequences equal to or larger than <number> cannot be found before 
     * this one. 
     */

    for (i = number-1; i < AVAILABLE_CELLS; i++) {
	pool->lastFreePage[i] = CELL_PAGE(cells);
    }

    return cells;
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
	PoolFreePages(&info->pools[generation]);
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
	    info->pools[generation].lastFreePage[i] 
		    = info->pools[generation].pages;
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
	ClearAllCells(page);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * MarkReachableCellsFromRoots --
 *
 *	Mark all cells reachable from the valid roots (inclusive).
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
    unsigned int generation;
    const void *root, **previousPtr;

    /*
     * Iterate in reverse so that existing lists don't get overwritten.
     */

    /*
     * First, simply mark roots for uncollected generations.
     */

    for (generation = GC_MAX_GENERATIONS-1; 
	    generation > info->maxCollectedGeneration; generation--) {
	for (root = info->pools[generation].roots; root; 
		root = ROOT_NEXT(root)) {
	    SetCells(CELL_PAGE(root), CELL_INDEX(root), 1);
	}
    }

    /*
     * Now follow roots from collected generations, and move surviving ones to 
     * the next generation.
     */

    for (/* Use last value of generation */; generation >= 1; generation--) {
	for (previousPtr = &info->pools[generation].roots, 
		root = info->pools[generation].roots; root; 
		root = *previousPtr) {
	    if (ROOT_REFCOUNT(root)) {
		/* 
		 * Follow cells with positive refcount. 
		 */

		if (CELL_TYPE(root) /* word */) {
		    MarkWord(info, (Col_Word *) &root, NULL);
		} else {
		    MarkRope(info, (Col_Rope *) &root, NULL);
		}
		previousPtr = &ROOT_NEXT(root);
	    } else {
		/* 
		 * Remove stale root from list. 
		 */

		*previousPtr = ROOT_NEXT(root);
	    }
	}

	/*
	 * Move still valid roots to the next generation: add to front.
	 */

	if (generation+1 >= GC_MAX_GENERATIONS) {
	    /*
	     * Can't promote past the last possible generation.
	     */

	    continue;
	}
	*previousPtr = info->pools[generation+1].roots;
	info->pools[generation+1].roots = info->pools[generation].roots;
	info->pools[generation].roots = NULL;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * MarkReachableCellsFromParents --
 *
 *	Mark all cells following parents from uncollectd pools having children 
 *	in collected pools.
 *
 *	Only parents with still unmarked children are followed.
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
    const void *root, **previousPtr, *parent;

    /*
     * Iterate in reverse so that existing lists don't get overwritten.
     */

    /*
     * First, follow parents from uncollected generations.
     */

    for (generation = GC_MAX_GENERATIONS-1; 
	    generation > info->maxCollectedGeneration; generation--) {
	for (previousPtr = &info->pools[generation].parents, 
		root = info->pools[generation].parents; root; 
		root = *previousPtr) {
	    parent = PARENT_CELL(root);

	    /*
	     * Follow parent. Clearing the first cell is sufficient, as it will 
	     * be set again during the mark phase. 
	     */

	    /* ASSERT(TestCell(CELL_PAGE(parent), CELL_INDEX(parent))) */
	    ClearCells(CELL_PAGE(parent), CELL_INDEX(parent), 1);
	    if (CELL_TYPE(root) /* word */) {
		MarkWord(info, (Col_Word *) &parent, NULL);
	    } else {
		MarkRope(info, (Col_Rope *) &parent, NULL);
	    }

	    if (CELL_TYPE(root)?WORD_PARENT(parent):ROPE_PARENT(parent)) {
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
     * roots or from their parent roots from uncollected generations. Now handle
     * collected parents, and move surviving parent roots to the next 
     * generation.
     */

    for (/* use last value of generation */; generation > 0; generation--) {
	for (previousPtr = &info->pools[generation].parents, 
		root = info->pools[generation].parents; root; 
		root = *previousPtr) {
#ifdef PROMOTE_COMPACT
	    if (generation == info->compactGeneration) {
		/*
		 * Parent was potentially redirected.
		 */

		if (CELL_TYPE(root) /* word */) {
		    ResolveRedirectWord((Col_Word *) &PARENT_CELL(root));
		} else {
		    ResolveRedirectRope((Col_Rope *) &PARENT_CELL(root));
		}
	    }
#endif
	    parent = PARENT_CELL(root);
	    if (TestCell(CELL_PAGE(parent), CELL_INDEX(parent))
		    && CELL_TYPE(root)?WORD_PARENT(parent):ROPE_PARENT(parent)) {
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

typedef struct MarkChildInfo
{
    GcMemInfo *info;
    unsigned int *generationPtr;
} MarkChildInfo;

static void
MarkRopeCustomChild(
    Col_Rope rope,
    Col_Rope *childPtr,
    Col_ClientData clientData)
{
    MarkRope(((MarkChildInfo *) clientData)->info, childPtr, 
	    ((MarkChildInfo *) clientData)->generationPtr);
}
static void 
MarkRope(
    GcMemInfo *info,		/* Thread local info. */
    Col_Rope *ropePtr,		/* Rope to mark and follow. */
    unsigned int *generationPtr)
				/* Youngest generation upon return. */
{
    int type;
    size_t nbCells;
    void *promoted;
    unsigned int childGen;

    /*
     * Entry point for tail recursive calls.
     */

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

	case ROPE_TYPE_REDIRECT:
	    /* 
	     * Overwrite reference. No need to follow, as it was already marked 
	     * when the redirect was created.
	     */

	    if (generationPtr 
		    && *generationPtr > PAGE_GENERATION(CELL_PAGE(*ropePtr))) {
		*generationPtr = PAGE_GENERATION(CELL_PAGE(*ropePtr));
	    }
	    *ropePtr = REDIRECT_SOURCE(*ropePtr);
	    return;
    }

    if (generationPtr 
	    && *generationPtr > PAGE_GENERATION(CELL_PAGE(*ropePtr))) {
	/*
	 * Cell is younger than caller.
	 */

	*generationPtr = PAGE_GENERATION(CELL_PAGE(*ropePtr));
    }

    /* 
     * Stop if cell is already marked. 
     */

    if (TestCell(CELL_PAGE(*ropePtr), CELL_INDEX(*ropePtr))) {
	return;
    }

    nbCells = GetNbCells(*ropePtr);
    
#ifdef PROMOTE_COMPACT
    if (PAGE_GENERATION(CELL_PAGE(*ropePtr)) == info->compactGeneration) {
	/*
	 * Ropes are relocated moved to the next generation.
	 */

	promoted = PoolAllocCells(&info->pools[info->compactGeneration+1], 
		nbCells);
	memcpy(promoted, *ropePtr, nbCells * CELL_SIZE);

	/*
	 * Replace original by redirect.
	 */

	ROPE_SET_TYPE(*ropePtr, ROPE_TYPE_REDIRECT);
	REDIRECT_SOURCE(*ropePtr) = promoted;

	/*
	 * Follow redirect to resolve its children. Clearing the first cell is 
	 * sufficient, as it will be set again during the mark phase. 
	 */

	ClearCells(CELL_PAGE(promoted), CELL_INDEX(promoted), 1);
	*ropePtr = promoted;
	goto start;
    }
#endif

    SetCells(CELL_PAGE(*ropePtr), CELL_INDEX(*ropePtr), nbCells);

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

	    ropePtr = &ROPE_SUBROPE_SOURCE(*ropePtr);
	    generationPtr = NULL;
	    goto start;
    	    
	case ROPE_TYPE_CONCAT:
	    /* 
	     * Follow left arm and tail recurse on right. 
	     */

	    MarkRope(info, &ROPE_CONCAT_LEFT(*ropePtr), NULL);
	    ropePtr = &ROPE_CONCAT_RIGHT(*ropePtr);
	    generationPtr = NULL;
	    goto start;

	case ROPE_TYPE_CUSTOM:
	    if (ROPE_CUSTOM_TYPE(*ropePtr)->childrenProc) {
		/*
		 * Follow children.
		 */

		if (ROPE_PARENT(*ropePtr)) {
		    MarkChildInfo childInfo = {info, &childGen};
		    childGen = UINT_MAX;
		    ROPE_CUSTOM_TYPE(*ropePtr)->childrenProc(*ropePtr, 
			    MarkRopeCustomChild, &childGen);
		    if (childGen >= PAGE_GENERATION(CELL_PAGE(*ropePtr))) {
			ROPE_CLEAR_PARENT(*ropePtr);
		    }
		} else {
		    MarkChildInfo childInfo = {info, NULL};
		    ROPE_CUSTOM_TYPE(*ropePtr)->childrenProc(*ropePtr, 
			    MarkRopeCustomChild, &childGen);
		}
	    }

	    /*
	     * No tail recursion.
	     */

	    return;
    	    
	case ROPE_TYPE_ROOT:
	    /* 
	     * Tail recurse on source. 
	     */

	    ropePtr = (Col_Rope *) &ROOT_SOURCE(*ropePtr);
	    generationPtr = NULL;
	    goto start;

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
    MarkWord(((MarkChildInfo *) clientData)->info, childPtr, 
	    ((MarkChildInfo *) clientData)->generationPtr);
}
static void 
MarkWord(
    GcMemInfo *info,		/* Thread local info. */
    Col_Word *wordPtr,		/* Word to mark and follow. */
    unsigned int *generationPtr)
				/* Youngest generation upon return. */
{
    Col_WordType * typeInfo;
    int type;
    size_t nbCells;
    void *promoted;
    unsigned int childGen;

    /*
     * Entry point for tail recursive calls.
     */

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
		MarkRope(info, &rope, generationPtr);
		*wordPtr = WORD_ROPE_NEW(rope);
	    }
	    return;

	case WORD_TYPE_REDIRECT:
	    /* 
	     * Overwrite reference. No need to follow, as it was already marked 
	     * when the redirect was created.
	     */

	    if (generationPtr 
		    && *generationPtr > PAGE_GENERATION(CELL_PAGE(*wordPtr))) {
		*generationPtr = PAGE_GENERATION(CELL_PAGE(*wordPtr));
	    }
	    *wordPtr = REDIRECT_SOURCE(*wordPtr);
	    return;
    }

    if (generationPtr 
	    && *generationPtr > PAGE_GENERATION(CELL_PAGE(*wordPtr))) {
	/*
	 * Cell is younger than caller.
	 */

	*generationPtr = PAGE_GENERATION(CELL_PAGE(*wordPtr));
    }

    /* 
     * Stop if cell is already marked. 
     */

    if (TestCell(CELL_PAGE(*wordPtr), CELL_INDEX(*wordPtr))) {
	return;
    }

    nbCells = GetNbCells(*wordPtr);
    
#ifdef PROMOTE_COMPACT
    if (PAGE_GENERATION(CELL_PAGE(*wordPtr)) == info->compactGeneration) {
	/*
	 * Words are relocated moved to the next generation.
	 */

	promoted = PoolAllocCells(&info->pools[info->compactGeneration+1], 
		nbCells);
	memcpy(promoted, *wordPtr, nbCells * CELL_SIZE);

	/*
	 * Replace original by redirect.
	 */

	WORD_SET_TYPE_ID(*wordPtr, WORD_TYPE_REDIRECT);
	REDIRECT_SOURCE(*wordPtr) = promoted;

	/*
	 * Follow redirect to resolve its children. Clearing the first cell is 
	 * sufficient, as it will be set again during the mark phase. 
	 */

	ClearCells(CELL_PAGE(promoted), CELL_INDEX(promoted), 1);
	*wordPtr = promoted;
	goto start;
    }
#endif

    SetCells(CELL_PAGE(*wordPtr), CELL_INDEX(*wordPtr), nbCells);

    /*
     * Follow children.
     *
     * Note: by construction, children of words other than custom are never
     * younger.
     */

    switch (type) {
	case WORD_TYPE_ROOT:
	    /* 
	     * Tail recurse on source. 
	     */

	    wordPtr = &ROOT_SOURCE(*wordPtr);
	    generationPtr = NULL;
	    goto start;

	case WORD_TYPE_VECTOR:
	    /* 
	     * Follow elements. 
	     */

	    {
		size_t i, size = WORD_VECTOR_LENGTH(*wordPtr);
		Col_Word *elements = WORD_VECTOR_ELEMENTS(*wordPtr);
		for (i = 0; i < size; i++) {
		    MarkWord(info, elements+i, NULL);
		}
	    }

	    /*
	     * Continued below on synonym.
	     */

	    break;

	case WORD_TYPE_LIST:
	    /* 
	     * Follow list root. 
	     */

	    MarkWord(info, &WORD_LIST_ROOT(*wordPtr), NULL);

	    /*
	     * Continued below on synonym.
	     */

	    break;

	case WORD_TYPE_SUBLIST:
	    /* 
	     * Tail recurse on source.
	     */

	    wordPtr = &WORD_SUBLIST_SOURCE(*wordPtr);
	    generationPtr = NULL;
	    goto start;

	case WORD_TYPE_CONCATLIST:
	    /* 
	     * Follow left arm and tail recurse on right. 
	     */

	    MarkWord(info, &WORD_CONCATLIST_LEFT(*wordPtr), NULL);
	    wordPtr = &WORD_CONCATLIST_RIGHT(*wordPtr);
	    generationPtr = NULL;
	    goto start;
    	    
	case WORD_TYPE_REGULAR:
	    WORD_GET_TYPE_ADDR(*wordPtr, typeInfo);
	    if (typeInfo->childrenProc) {
		/*
		 * Follow children.
		 */

		MarkChildInfo childInfo = {info, NULL};
		if (WORD_PARENT(*wordPtr)) {
		    childInfo.generationPtr = &childGen;
		    childGen = UINT_MAX;
		}
		typeInfo->childrenProc(*wordPtr, MarkWordChild, &childInfo);
	    }

	    /*
	     * Continued below on synonym.
	     */

	    break;

	default:
	    /*
	     * Stop recursion.
	     */

	    return;
    }

    /*
     * Follow synonym.
     */

    if (WORD_PARENT(*wordPtr)) {
	/*
	 * No tail recursion.
	 */

	/* ASSERT(type == WORD_TYPE_REGULAR */
	MarkWord(info, &WORD_SYNONYM(*wordPtr), &childGen);
	if (childGen >= PAGE_GENERATION(CELL_PAGE(*wordPtr))) {
	    WORD_CLEAR_PARENT(*wordPtr);
	}
    } else {
	/*
	 * Tail recurse.
	 */

	wordPtr = &WORD_SYNONYM(*wordPtr);
	generationPtr = NULL;
	goto start;
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
    const void *cell, **previousPtr;

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
		Col_Word word = cell;
		Col_WordType * typeInfo;

		if (generation == info->compactGeneration) {
		    /* 
		     * Word was potentially redirected.
		     */

		    ResolveRedirectWord(&word);
		}

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

		    *previousPtr = word;

		    *(Col_Word **) previousPtr = &WORD_NEXT(word, 
			    typeInfo->sizeProc(word));
		}
	    } else {
		Col_Rope rope = cell;

		if (generation == info->compactGeneration) {
		    /* 
		     * Rope was potentially redirected.
		     */

		    ResolveRedirectRope(&rope);
		}

		if (!TestCell(CELL_PAGE(rope), CELL_INDEX(rope))) {
		    /*
		     * Cleanup.
		     */

		    ROPE_CUSTOM_TYPE(rope)->freeProc(rope);

		    /* 
		     * Remove from list. 
		     */

		    *previousPtr = ROPE_CUSTOM_NEXT(rope, 
			    ROPE_CUSTOM_SIZE(rope));
		} else {
		    /*
		     * Keep, updating reference in case of redirection.
		     */

		    *previousPtr = rope;

		    *(Col_Rope **) previousPtr = &ROPE_CUSTOM_NEXT(rope, 
			    ROPE_CUSTOM_SIZE(rope));
		}
	    }
	}
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
    void *page, *prev;
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
     * Move the promoted pages to the head of the target pool's page list.
     */

    for (page = info->pools[generation].pages; page; page = PAGE_NEXT(page)) {
	PAGE_GENERATION(page) = generation+1;
	prev = page;
    }
    PAGE_NEXT(prev) = info->pools[generation+1].pages;
    info->pools[generation+1].pages = info->pools[generation].pages;
    info->pools[generation+1].nbPages += info->pools[generation].nbPages;
    info->pools[generation+1].nbAlloc += info->pools[generation].nbPages;
    info->pools[generation+1].nbSetCells += info->pools[generation].nbSetCells;
    for (i = 0; i < AVAILABLE_CELLS; i++) {
	info->pools[generation+1].lastFreePage[i] 
		= info->pools[generation+1].pages;
    }
    info->pools[generation].pages = NULL;
    info->pools[generation].nbPages = 0;
    info->pools[generation].nbAlloc = 0;
    info->pools[generation].nbSetCells = 0;
}
