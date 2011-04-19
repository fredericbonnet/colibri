/*
 * Mark-and-sweep, generational, exact GC.
 *
 * Newer cells are born in "Eden", i.e. generation 0 pool, and are promoted to 
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

#include <memory.h>

#ifdef _DEBUG
#   include <stdio.h>
#   define TRACE(format, ...) (fprintf(stderr, format, __VA_ARGS__), fflush(stderr))
#else
#   define TRACE
#endif


/*
 * Max number of generations. With a generational factor of 10, and 5 
 * generations, the oldest generation would be collected 10^5 = 100,000 times 
 * less frequently than the youngest generation. With one GC every second, 
 * that would mean about 1 major GC per day.
 */

#define GC_MAX_GENERATIONS	3

/* 
 * GC-protected section counter, i.e. nb of nested pause calls. When positive,
 * we are in a GC-protected section.
 */

static size_t pauseGC;

/* 
 * Singly-linked list of roots. Each root contains a pointer to the next one: 
 * ROPE_ROOT_NEXT for roots, STRUCT_ROOT_NEXT for containers.
 */

static const void * roots;

/*
 * Singly-linked lists of references. Each reference points to a parent and its
 * child from a younger generation.
 */

static const void * refs[GC_MAX_GENERATIONS*2][GC_MAX_GENERATIONS*2];	

/* 
 * Singly-linked list of custom ropes or words needing freeing upon deletion. 
 * Each custom rope or word contains a pointer to the next one (ROPE_CUSTOM_NEXT
 * and WORD_NEXT).
 */

static const void * freeables;

/* 
 * Number of page allocations on a pool before triggering a GC.
 */

#define GC_PAGE_ALLOC		64

/* 
 * Generational factor, i.e. nb of GCs between generations. 
 */

#define GC_GEN_FACTOR		10

/* 
 * Pools where the new cells are created. 
 */

static MemoryPool * pools[GC_MAX_GENERATIONS];

/*
 * Prototypes for functions used only in this file.
 */

static void *		PoolAllocCells(MemoryPool *pool, size_t number);
static void		PerformGC(void);
static void		ClearPoolBitmasks(int generation);
static void		MarkReachableCellsFromParents(int maxChildGeneration);
static void		MarkReachableCellsFromRoots(void);
static void		MarkRope(Col_Rope rope);
static void		MarkWord(Col_Word word);
static void		SweepFreeables(int maxGeneration);
static void		PromoteRefs(int maxChildGeneration);
static void		PromoteCells(int generation);
static void		ResolveRedirects(int generation);
static void		ResolveChildRedirects(int maxChildGeneration);
static void		ResolveRedirectRope(Col_Rope *ropePtr);
static void		ResolveRedirectWord(Col_Word *wordPtr);


/*
 *---------------------------------------------------------------------------
 *
 * GCInit --
 *
 *	Initialize the garbage collector.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Global data is initialized.
 *
 *---------------------------------------------------------------------------
 */

void 
GCInit() 
{
    memset(pools, 0, sizeof(pools)); 
    pools[0] = PoolNew(0);

    pauseGC = 0;

    roots = NULL;
    memset((void *) refs, 0, sizeof(refs));
    freeables = NULL;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_DeclareChildRope --
 * Col_DeclareChildWord --
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
Col_DeclareChildRope(
    const void *parent,		/* Parent rope or word. */
    Col_Rope child)		/* Child rope. */
{
    int parentGen, childGen;
    Col_Rope ref;
    int type = ROPE_TYPE(child);

    switch (type) {
	case ROPE_TYPE_NULL:
	case ROPE_TYPE_C:
	case ROPE_TYPE_EMPTY:
	    /* 
	     * Unmanaged strings. 
	     */

	    return;

	case ROPE_TYPE_ROOT:
	    child = ROOT_SOURCE(child);
	    break;
    }

    /*
     * Compare generations.
     */

    if (IS_WORD(parent)) {
	RESOLVE_WORD((Col_Word) parent);
	parentGen = PAGE_GENERATION(CELL_PAGE(parent)) * 2
		+ (WORD_GENERATION(parent)?1:0);
    } else {
	RESOLVE_ROPE((Col_Rope) parent);
	parentGen = PAGE_GENERATION(CELL_PAGE(parent)) * 2
		+ (ROPE_GENERATION(parent)?1:0);
    }
    childGen = PAGE_GENERATION(CELL_PAGE(child)) * 2
	    + (ROPE_GENERATION(child)?1:0);
    if (parentGen <= childGen) {
	/*
	 * Parent belongs to the same or newer generation, no need to keep trace.
	 */

	return;
    }

    /* 
     * Create a new reference rope.
     */

    ref = AllocCells(1);
    ROPE_SET_TYPE(ref, ROPE_TYPE_REF);
    REF_PARENT(ref) = parent;
    REF_CHILD(ref) = child;

    /* 
     * Insert in ref list for the parent generation.
     */

    REF_NEXT(ref) = refs[parentGen][childGen];
    refs[parentGen][childGen] = ref;
}

EXTERN void
Col_DeclareChildWord(
    const void *parent,		/* Parent rope or word. */
    Col_Word child)		/* Child word. */
{
    int parentGen, childGen;
    Col_Word ref;
    int type = WORD_TYPE(child);

    switch (type) {
	case WORD_TYPE_NULL:
	case WORD_TYPE_SMALL_INT:
	case WORD_TYPE_SMALL_STRING:
	case WORD_TYPE_CHAR:
	    /* 
	     * Immediate values. 
	     */

	    return;

	case WORD_TYPE_ROPE:
	    /*
	     * Word is rope.
	     */

	    Col_DeclareChildRope(parent, child);
	    return;

	case WORD_TYPE_ROOT:
	    child = ROOT_SOURCE(child);
	    break;
    }

    /*
     * Compare generations.
     */

    if (IS_WORD(parent)) {
	RESOLVE_WORD((Col_Word) parent);
	parentGen = PAGE_GENERATION(CELL_PAGE(parent)) * 2
		+ (WORD_GENERATION(parent)?1:0);
    } else {
	RESOLVE_ROPE((Col_Rope) parent);
	parentGen = PAGE_GENERATION(CELL_PAGE(parent)) * 2
		+ (ROPE_GENERATION(parent)?1:0);
    }
    childGen = PAGE_GENERATION(CELL_PAGE(child)) * 2
	    + (WORD_GENERATION(child)?1:0);
    if (parentGen <= childGen) {
	/*
	 * Parent belongs to the same or newer generation, no need to keep trace.
	 */

	return;
    }

    /* 
     * Create a new reference word.
     */

    ref = AllocCells(1);
    WORD_SET_TYPE_ID(ref, WORD_TYPE_REF);
    REF_PARENT(ref) = parent;
    REF_CHILD(ref) = child;

    /* 
     * Insert in ref list for the parent generation.
     */

    REF_NEXT(ref) = refs[parentGen][childGen];
    refs[parentGen][childGen] = ref;
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
    Col_Rope root;
    int type = ROPE_TYPE(rope);

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

    root = AllocCells(1);
    ROPE_SET_TYPE(root, ROPE_TYPE_ROOT);
    ROOT_REFCOUNT(root) = 1;
    ROOT_SOURCE(root) = rope;

    /* 
     * Insert in head of the root list. 
     */

    ROOT_NEXT(root) = roots;
    roots = root;

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
    Col_Word root;
    int type = WORD_TYPE(word);

    switch (type) {
	case WORD_TYPE_NULL:
	case WORD_TYPE_SMALL_INT:
	case WORD_TYPE_SMALL_STRING:
	case WORD_TYPE_CHAR:
	    /* 
	     * Immediate values. 
	     */

	    return word;

	case WORD_TYPE_ROPE:
	    /*
	     * Word is rope.
	     */

	    return Col_PreserveRope(word);

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

    root = AllocCells(1);
    WORD_SET_TYPE_ID(root, WORD_TYPE_ROOT);
    ROOT_REFCOUNT(root) = 1;
    ROOT_SOURCE(root) = word;

    /* 
     * Insert in head of the root list. 
     */

    ROOT_NEXT(root) = roots;
    roots = root;

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

	ROPE_CUSTOM_NEXT(rope, ROPE_CUSTOM_SIZE(rope)) = freeables;
	freeables = rope;
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

	    WORD_NEXT(word, typeInfo->sizeProc(word)) = freeables;
	    freeables = word;
	}
    }
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
    pauseGC++;
}
void 
Col_ResumeGC(void)
{
    if (pauseGC > 1) {
	/* 
	 * Within nested sections. 
	 */

	pauseGC--;
	return;
    }

    if (pools[0]->alloc >= GC_PAGE_ALLOC) {
	/* 
	 * GC is needed when pages were allocated within a GC-protected 
	 * section.
	 */

	PerformGC();
    }

    /*
     * Leave protected section.
     */

    pauseGC = 0;
}

/*
 *---------------------------------------------------------------------------
 *
 * AllocCells --
 *
 *	Allocate cells in the youngest pool. 
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
    if (!pauseGC) {
	/*
	 * Can't alloc outside of a GC protected section.
	 */

	/* TODO: exception */
	return NULL;
    }
    
    /*
     * Alloc cells; alloc pages if needed.
     */

    return PoolAllocCells(pools[0], number);
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
    int generation, maxGeneration;

    /*
     * Traverse pools to check whether they must be collected. Eden pool is
     * always collected. Clear the collected pools' bitmask.
     */

    maxGeneration = 0;
    for (generation = 0; generation < GC_MAX_GENERATIONS && pools[generation]; 
	    generation++) {
	if (generation > 0 && pools[generation]->alloc < GC_PAGE_ALLOC) {
	    /*
	     * Stop if number of allocations is less than the threshold.
	     */

	    break;
	}
	
	pools[generation]->gc++;

	if (generation > 0 && pools[generation]->gc < GC_GEN_FACTOR) {
	    /*
	     * Skip GC cycles for older pools.
	     */

	    break;
	}

	maxGeneration = generation;

	/* 
	 * Clear bitmasks on pool. Reachable ropes will be marked again in the
	 * next step.
	 */

	ClearPoolBitmasks(generation);
    }

    /* 
     * Mark all cells that are reachable from known roots and from parents of 
     * uncollected pools. This also marks still valid refs in the process (i.e.,
     * whose child and parent are both marked).
     */

    MarkReachableCellsFromRoots();
    MarkReachableCellsFromParents(maxGeneration);

    /*
     * Perform cleanup on all custom ropes and words that need freeing.
     */

    SweepFreeables(maxGeneration);

    /* 
     * At this point all reachable cells are set, and unreachable cells are 
     * cleared. Now promote all cells that survived 2 GCs. Older pools are 
     * promoted first so that newer cells that get promoted here don't get 
     * their generation counter incremented again in older pools.
     */

    PromoteRefs(maxGeneration);
    for (generation = maxGeneration; generation >= 0; generation--) {
	PromoteCells(generation);
    }

    /* 
     * Resolve all redirections resulting from promotions, including the first 
     * untouched pool (it may contain newly promoted cells that refer to
     * redirected ones), as well as parents from older pools.
     */

    for (generation = 0; generation <= maxGeneration+1 && generation < GC_MAX_GENERATIONS; 
	    generation++) {
	ResolveRedirects(generation);
    }
    ResolveChildRedirects(maxGeneration);

    /*
     * Free empty pages after all redirections have been resolved.
     */

    for (generation = 0; generation <= maxGeneration; generation++) {
	PoolFreePages(pools[generation]);
    }

    /*
     * Finally, reset counters.
     */

    for (generation = 0; generation <= maxGeneration; generation++) {
	pools[generation]->alloc = 0;
	pools[generation]->gc = 0;
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
    int generation)		/* The pool generation to traverse. */
{
    void * page;

    /* 
     * Clear all bitmasks in pool. 
     */

    for (page = pools[generation]->pages; page; page = PAGE_NEXT(page)) {
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
MarkReachableCellsFromRoots() 
{
    const void *root, **previousPtr;

    for (previousPtr = &roots, root = roots; root; root = *previousPtr) {
	if (ROOT_REFCOUNT(root)) {
	    /* 
	     * Follow cells with positive refcount. 
	     */

	    if (IS_WORD(root)) {
		MarkWord(root);
	    } else {
		MarkRope(root);
	    }
	    previousPtr = &ROOT_NEXT(root);
	} else {
	    /* 
	     * Remove stale root from list. 
	     */

	    *previousPtr = ROOT_NEXT(root);
	}
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * MarkReachableCellsFromParents --
 *
 *	Mark all cells following parents having children in younger pools.
 *
 *	Only parents with unmarked children are followed.
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
    int maxChildGeneration)	/* Oldest collected generation. */
{
    int parentGen, childGen;
    const void *ref, *parent, *child;

    for (parentGen = (maxChildGeneration+1)*2; parentGen < GC_MAX_GENERATIONS*2;
	    parentGen++) {
	for (childGen = 0; childGen <= maxChildGeneration*2+1; childGen++) {
	    for (ref = refs[parentGen][childGen]; ref; ref = REF_NEXT(ref)) {
		child = REF_CHILD(ref);
		if (TestCell(CELL_PAGE(child), CELL_INDEX(child))) {
		    /*
		     * No need to follow parent.
		     */

		    continue;
		}

		/*
		 * Follow parent. Clearing the first cell is sufficient, as
		 * it will be set again during the mark phase. 
		 */

		parent = REF_PARENT(ref);
		/* ASSERT(TestCell(CELL_PAGE(parent), CELL_INDEX(parent))) */
		ClearCells(CELL_PAGE(parent), CELL_INDEX(parent), 1);
		if (IS_WORD(parent)) {
		    MarkWord(parent);
		} else {
		    MarkRope(parent);
		}
	    }
	}
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
    MarkRope(*childPtr);
}
static void 
MarkRope(
    Col_Rope rope)		/* Rope to mark and follow. */
{
    int type = ROPE_TYPE(rope);

    if (type == ROPE_TYPE_C) {
	/* 
	 * No cell to mark. 
	 */

	return;
    }

    /* 
     * Stop if cell is already marked. 
     */

    if (TestCell(CELL_PAGE(rope), CELL_INDEX(rope))) {
	return;
    }

    switch (type) {
	case ROPE_TYPE_C:
	case ROPE_TYPE_EMPTY:
	    /* 
	     * C strings are never stored into memory pool. Typical use case 
	     * is static data. 
	     */

	    break;

	/* 
	 * Flat strings, fixed or variable. Mark all cells.
	 */

	case ROPE_TYPE_UCS1:
	    SetCells(CELL_PAGE(rope), CELL_INDEX(rope), 
		    NB_CELLS(ROPE_UCS_HEADER_SIZE + ROPE_UCS_LENGTH(rope)));
	    break;

	case ROPE_TYPE_UCS2:
	    SetCells(CELL_PAGE(rope), CELL_INDEX(rope), 
		    NB_CELLS(ROPE_UCS_HEADER_SIZE + ROPE_UCS_LENGTH(rope)*2));
	    break;

	case ROPE_TYPE_UCS4:
	    SetCells(CELL_PAGE(rope), CELL_INDEX(rope), 
		    NB_CELLS(ROPE_UCS_HEADER_SIZE + ROPE_UCS_LENGTH(rope)*4));
	    break;

	case ROPE_TYPE_UTF8:
	    SetCells(CELL_PAGE(rope), CELL_INDEX(rope), 
		    NB_CELLS(ROPE_UTF8_HEADER_SIZE + ROPE_UTF8_BYTELENGTH(rope)));
	    break;

	case ROPE_TYPE_SUBROPE:
	    /* 
	     * Mark subrope and follow source.
	     */

	    SetCells(CELL_PAGE(rope), CELL_INDEX(rope), 1);
	    MarkRope(ROPE_SUBROPE_SOURCE(rope));
	    break;
	    
	case ROPE_TYPE_CONCAT:
	    /* 
	     * Mark concat and follow both arms. 
	     */

	    SetCells(CELL_PAGE(rope), CELL_INDEX(rope), 1);
	    MarkRope(ROPE_CONCAT_LEFT(rope));
	    MarkRope(ROPE_CONCAT_RIGHT(rope));
	    break;

	case ROPE_TYPE_CUSTOM:
	    /*
	     * Mark rope.
	     */

	    SetCells(CELL_PAGE(rope), CELL_INDEX(rope), 
		    NB_CELLS(ROPE_CUSTOM_HEADER_SIZE + ROPE_CUSTOM_SIZE(rope) 
			    + (ROPE_CUSTOM_TYPE(rope)->freeProc?ROPE_CUSTOM_TRAILER_SIZE:0)));

	    if (ROPE_CUSTOM_TYPE(rope)->childrenProc) {
		/*
		 * Follow children.
		 */

		ROPE_CUSTOM_TYPE(rope)->childrenProc(rope, MarkRopeCustomChild, 0);
	    }
	    break;
	    
	case ROPE_TYPE_ROOT:
	    /* 
	     * Mark root and follow source. 
	     */

	    SetCells(CELL_PAGE(rope), CELL_INDEX(rope), 1);
	    MarkRope(ROOT_SOURCE(rope));
	    break;
    }
}

static void
MarkWordChild(
    Col_Word word,
    Col_Word *childPtr,
    Col_ClientData clientData)
{
    MarkWord(*childPtr);
}
static void 
MarkWord(
    Col_Word word)		/* Word to mark and follow. */
{
    Col_WordType * typeInfo;
    int type;

    if (!word || IS_IMMEDIATE(word)) {
	/* 
	 * Immediate values. No cell to mark.
	 */

	return;
    }

    if (!IS_WORD(word)) {
	/*
	 * Word is rope.
	 */

	MarkRope(word);
	return;
    }

    /* 
     * Stop if cell is already marked. 
     */

    if (TestCell(CELL_PAGE(word), CELL_INDEX(word))) {
	return;
    }

    type = WORD_TYPE(word);
    switch (type) {
	case WORD_TYPE_REGULAR:
	    /* 
	     * Mark word and follow value. 
	     */

	    WORD_GET_TYPE_ADDR(word, typeInfo);
	    SetCells(CELL_PAGE(word), CELL_INDEX(word), 
		    NB_CELLS(WORD_HEADER_SIZE + typeInfo->sizeProc(word)
			    + (typeInfo->freeProc?WORD_TRAILER_SIZE:0)));
	    MarkWord(WORD_VALUE(word));

	    if (typeInfo->childrenProc) {
		/*
		 * Follow children.
		 */

		typeInfo->childrenProc(word, MarkWordChild, 0);
	    }
	    break;

	case WORD_TYPE_ROOT:
	    /* 
	     * Mark root and follow source. 
	     */

	    SetCells(CELL_PAGE(word), CELL_INDEX(word), 1);
	    MarkWord(ROOT_SOURCE(word));
	    break;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * SweepFreeables --
 *
 *	Perform cleanup for all custom ropes or words that need freeing.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Calls each cleaned rope's freeProc.
 *
 *---------------------------------------------------------------------------
 */

static void 
SweepFreeables(
    int maxGeneration)		/* Don't cleanup beyond this generation. */
{
    Col_WordType * typeInfo;
    const void *cell, **previousPtr;

    for (previousPtr = &freeables, cell = freeables; cell; cell = *previousPtr) {
	if (PAGE_GENERATION(CELL_PAGE(cell)) > maxGeneration) {
	    /*
	     * Stop there as by construction freeables are in generation 
	     * order.
	     */

	    break;
	}

	/*
	 * Note: by construction, freeable words are custom ropes or regular 
	 * words (i.e. with a type pointer).
	 */

	if (IS_WORD(cell)) {
	    Col_Word word = cell;

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
		previousPtr = &WORD_NEXT(word, typeInfo->sizeProc(word));
	    }
	} else {
	    Col_Rope rope = cell;

	    if (!TestCell(CELL_PAGE(rope), CELL_INDEX(rope))) {
		/*
		 * Cleanup.
		 */

		ROPE_CUSTOM_TYPE(rope)->freeProc(rope);

		/* 
		 * Remove from list. 
		 */

		*previousPtr = ROPE_CUSTOM_NEXT(rope, ROPE_CUSTOM_SIZE(rope));
	    } else {
		previousPtr = &ROPE_CUSTOM_NEXT(rope, ROPE_CUSTOM_SIZE(rope));
	    }
	}
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * PromoteRefs --
 *
 *	Promote refs to older generations.
 *
 *	This simply consists in moving or merging the ref lists with those of
 *	the next generation.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Overwrite promoted cells with redirect to the new cell in the
 *	older pool.
 *
 *---------------------------------------------------------------------------
 */

static void
PromoteRefs(
    int maxChildGeneration)	/* Oldest collected generation. */
{
    int parentGen, childGen, parentDstGen, childDstGen;
    const void **srcPtr, **dstPtr, *ref, *next, *parent, *child;

    /*
     * Iterate in reverse so that existing lists don't get overwritten.
     */

    for (parentGen = GC_MAX_GENERATIONS*2-1; parentGen >= 0; parentGen--) {
	/*
	 * Simply mark refs for uncollected generations.
	 */

	for (childGen = parentGen-1; childGen > maxChildGeneration*2+1; childGen--) {
	    srcPtr = &refs[parentGen][childGen];
	    for (ref = *srcPtr; ref; ref = REF_NEXT(ref)) {
		SetCells(CELL_PAGE(ref), CELL_INDEX(ref), 1);
	    }
	}

	/*
	 * Mark and promote refs for collected generations.
	 */

	if (parentGen <= maxChildGeneration*2+1
		&& parentGen < (GC_MAX_GENERATIONS-1)*2) {
	    /* 
	     * Parents were promoted.
	     */

	    parentDstGen = parentGen+1;
	} else {
	    parentDstGen = parentGen;
	}
	for (/* Use last value of childGen */; childGen >= 0; childGen--) {
	    srcPtr = &refs[parentGen][childGen];
	    if (!*srcPtr) {
		/*
		 * No parent in this generation.
		 */

		continue;
	    }

	    /*
	     * Children were promoted.
	     */

	    childDstGen = childGen+1;
	    if (childDstGen >= parentDstGen) {
		/*
		 * Refs are no longer needed.
		 */

		*srcPtr = NULL;
		continue;
	    }


	    /*
	     * Move still valid refs from the source generation to the 
	     * destination generation.
	     */

	    dstPtr = &refs[parentDstGen][childDstGen];
	    for (ref = *srcPtr; ref; ref = next) {
		next = REF_NEXT(ref);
		parent = REF_PARENT(ref);
		if (!TestCell(CELL_PAGE(parent), CELL_INDEX(parent))) {
		    continue;
		}
		child = REF_CHILD(ref);
		if (!TestCell(CELL_PAGE(child), CELL_INDEX(child))) {
		    continue;
		}

		/*
		 * Ref is valid: both parent and child are marked. Mark it
		 * as well.
		 */

		SetCells(CELL_PAGE(ref), CELL_INDEX(ref), 1);
		REF_NEXT(ref) = *dstPtr;
		*dstPtr = ref;
	    }
	    *srcPtr = NULL;
	}
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * PromoteCells --
 *
 *	Promote cells having survived 2 GCs to an older pool.
 *
 *	Promotion is done by replacing the original cell by a redirect cell
 *	that points to the new cell in the older pool. These redirections are 
 *	resolved in a later step, so they are never encountered outside of the 
 *	GC process.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Overwrite promoted cells with redirect to the new cell in the
 *	older pool.
 *
 *---------------------------------------------------------------------------
 */

static void 
PromoteCells(
    int generation)		/* The pool generation to traverse. */
{
    void *page;
    void *cell, *promoted;
    size_t i;
    size_t nbCells;
    int type;
    Col_WordType * typeInfo;

    if (generation+1 >= GC_MAX_GENERATIONS) {
	/*
	 * Can't promote past the last possible generation.
	 */

	return;
    }

    for (page = pools[generation]->pages; page; page = PAGE_NEXT(page)) {
	for (i = RESERVED_CELLS; i < CELLS_PER_PAGE; i += nbCells) {
	    nbCells = 1; /* default */
	    if (!TestCell(page, i)) {
		continue;
	    }
	    
	    cell = PAGE_CELL(page, i);
	    if (IS_WORD(cell)) {
		Col_Word word = cell;
		type = WORD_TYPE(word);

		if (type == WORD_TYPE_ROOT || type == WORD_TYPE_REF) {
		    /* 
		     * Never promote roots, as they are externally referenced. 
		     * Else redirections could not get resolved.
		     *
		     * Refs are never promoted as they must be cleared at each 
		     * GC, and marked again when still valid (see PromoteRefs).
		     */

		    continue;
		}

		/*
		 * Get number of cells taken by word.
		 * Note: word is neither immediate nor rope.
		 */

		switch (type) {
		    case WORD_TYPE_REGULAR:
			WORD_GET_TYPE_ADDR(word, typeInfo);
			nbCells = NB_CELLS(WORD_HEADER_SIZE 
				+ typeInfo->sizeProc(word)
				+ (typeInfo->freeProc?WORD_TRAILER_SIZE:0));
			break;
		}

		if (!WORD_GENERATION(word)) {
		    /* 
		     * Increment generation. 
		     */

		    WORD_INCR_GENERATION(word);
		    continue;
		}
	    } else {
		Col_Rope rope = cell;
		type = ROPE_TYPE(rope);

		if (type == ROPE_TYPE_ROOT || type == ROPE_TYPE_REF) {
		    /* 
		     * Never promote roots, as they are externally referenced. 
		     * Else redirections could not get resolved.
		     *
		     * Refs are never promoted as they must be cleared at each 
		     * GC, and marked again when still valid (see PromoteRefs).
		     */

		    continue;
		}

		/*
		 * Get number of cells taken by rope.
		 */

		switch (type) {
		    case ROPE_TYPE_UCS1:
			nbCells = NB_CELLS(ROPE_UCS_HEADER_SIZE 
				+ ROPE_UCS_LENGTH(rope));
			break;

		    case ROPE_TYPE_UCS2:
			nbCells = NB_CELLS(ROPE_UCS_HEADER_SIZE 
				+ ROPE_UCS_LENGTH(rope)*2);
			break;

		    case ROPE_TYPE_UCS4:
			nbCells = NB_CELLS(ROPE_UCS_HEADER_SIZE 
				+ ROPE_UCS_LENGTH(rope)*4);
			break;

		    case ROPE_TYPE_UTF8:
			nbCells = NB_CELLS(ROPE_UTF8_HEADER_SIZE 
				+ ROPE_UTF8_BYTELENGTH(rope));
			break;

		    case ROPE_TYPE_CUSTOM:
			nbCells = NB_CELLS(ROPE_CUSTOM_HEADER_SIZE 
				+ ROPE_CUSTOM_SIZE(rope) 
				+ (ROPE_CUSTOM_TYPE(rope)->freeProc?ROPE_CUSTOM_TRAILER_SIZE:0));
			break;
		}

		if (!ROPE_GENERATION(rope)) {
		    /* 
		     * Increment generation. 
		     */

		    ROPE_INCR_GENERATION(rope);
		    continue;
		}
	    }

	    /* 
	     * Cell already survived once, promote: copy data to older pool and 
	     * overwrite existing by redirection. Redirections will be resolved 
	     * in a later step.
	     *
	     * By construction, the older pool is up-to-date, so allocating new 
	     * cells is safe (i.e. this shouldn't overwrite unswept cells. 
	     */

	    if (!pools[generation+1]) {
		/*
		 * Create older pool.
		 */

		pools[generation+1] = PoolNew(generation+1);
	    }

	    /*
	     * Clear generation counter before copying data so that the 
	     * promoted cell has the right value. 
	     */

	    if (IS_WORD(cell)) {
		WORD_CLEAR_GENERATION(cell);
	    } else {
		ROPE_CLEAR_GENERATION(cell);
	    }

	    /*
	     * Alloc and copy cells to older pool. Copy raw data, redirected 
	     * pointers will be resolved in a next step.
	     */

	    promoted = PoolAllocCells(pools[generation+1], nbCells);
	    memcpy(promoted, cell, nbCells * CELL_SIZE);

	    /*
	     * Replace original by redirect.
	     */

	    if (IS_WORD(cell)) {
		WORD_SET_TYPE_ID(cell, WORD_TYPE_REDIRECT);
	    } else {
		ROPE_SET_TYPE(cell, ROPE_TYPE_REDIRECT);
	    }
	    REDIRECT_SOURCE(cell) = promoted;
	    if (nbCells > 1) {
		ClearCells(page, i+1, nbCells-1);
	    }
	}
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ResolveRedirects --
 *
 *	Resolve all redirect cells in a pool. This ensures that such cells are 
 *	no longer reachable outside of the GC process.
 *
 *	Redirect cells are created as part of the generational GC: oldest 
 *	cells in a pool are promoted to an older pool and are replaced in the 
 *	original pool by a redirection. These redirect cells eventually get 
 *	dereferenced once all cells pointing to the original cell are traversed
 *	at least once and updated to point to the new one.
 *
 *	Mark empty pages in the process for later cleanup.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Overwrite all references to redirect cells, and free cells containing 
 *	the latter.
 *
 *---------------------------------------------------------------------------
 */

static void
ResolveRopeCustomChild(
    Col_Rope rope,
    Col_Rope *childPtr,
    Col_ClientData clientData)
{
    ResolveRedirectRope(childPtr);
}
static void
ResolveWordChild(
    Col_Word word,
    Col_Word *childPtr,
    Col_ClientData clientData)
{
    ResolveRedirectWord(childPtr);
}
static void 
ResolveRedirects(
    int generation)		/* The pool generation to traverse. */
{
    void *page;
    void *cell;
    size_t i;
    size_t nbCells;
    int type;
    Col_WordType * typeInfo;

    if (!pools[generation]) {
	return;
    }

    for (page = pools[generation]->pages; page; page = PAGE_NEXT(page)) {
	int isEmpty = 1;	/* Cleared when page contains at least one 
				 * non-redirect cell. */

	for (i = RESERVED_CELLS; i < CELLS_PER_PAGE; i += nbCells) {
	    nbCells = 1; /* default */
	    if (!TestCell(page, i)) {
		continue;
	    }

	    cell = PAGE_CELL(page, i);
	    if (IS_WORD(cell)) {
		Col_Word word = cell;
		type = WORD_TYPE(word);

		if (type != WORD_TYPE_REDIRECT) {
		    isEmpty = 0;
		}

		/*
		 * Get number of cells taken by word, update fields of word 
		 * that potentially refer to redirected children, and free 
		 * existing redirects.
		 *
		 * Note: word is neither immediate nor rope.
		 */

		switch (type) {
		    case WORD_TYPE_REGULAR:
			WORD_GET_TYPE_ADDR(word, typeInfo);
			nbCells = NB_CELLS(WORD_HEADER_SIZE 
				+ typeInfo->sizeProc(word)
				+ (typeInfo->freeProc?WORD_TRAILER_SIZE:0));

			if (typeInfo->freeProc) {
			    /*
			     * Resolve next in list.
			     */

			    ResolveRedirectWord(&WORD_NEXT(word, 
				    typeInfo->sizeProc(word)));
			}

			/*
			 * Resolve value. This works even when it is NULL.
			 */

			ResolveRedirectWord(&WORD_VALUE(word));

			if (typeInfo->childrenProc) {
			    /*
			     * Resolve children.
			     */

			    typeInfo->childrenProc(word, ResolveWordChild, 0);
			}
			break;

		    case WORD_TYPE_ROOT:
			/*
			 * Resolve source. No need to resolve next, as roots are
			 * never promoted.
			 */

			ResolveRedirectWord((Col_Word *) &ROOT_SOURCE(word));
			break;

		    case WORD_TYPE_REDIRECT:
			/* 
			 * Clear cell. Note that this doesn't alter the redirect 
			 * rope data, that way it can still be resolved if other
			 * ropes still refer to it. 
			 */

			ClearCells(page, i, 1);
			break;

		    case WORD_TYPE_REF:
			/*
			 * Resolve parent and child. No need to resolve next, as
			 * roots are never promoted.
			 */

			ResolveRedirectWord((Col_Word *) &REF_PARENT(word));
			ResolveRedirectWord((Col_Word *) &REF_CHILD(word));
			break;
		}
	    } else {
		Col_Rope rope = cell;
		type = ROPE_TYPE(rope);

		if (type != ROPE_TYPE_REDIRECT) {
		    isEmpty = 0;
		}

		/*
		 * Get number of cells taken by rope, update fields of rope
		 * that potentially refer to redirected children, and free 
		 * existing redirects.
		 */

		switch (type) {
		    case ROPE_TYPE_UCS1:
			nbCells = NB_CELLS(ROPE_UCS_HEADER_SIZE 
				+ ROPE_UCS_LENGTH(rope));
			break;

		    case ROPE_TYPE_UCS2:
			nbCells = NB_CELLS(ROPE_UCS_HEADER_SIZE 
				+ ROPE_UCS_LENGTH(rope)*2);
			break;

		    case ROPE_TYPE_UCS4:
			nbCells = NB_CELLS(ROPE_UCS_HEADER_SIZE 
				+ ROPE_UCS_LENGTH(rope)*4);
			break;

		    case ROPE_TYPE_UTF8:
			nbCells = NB_CELLS(ROPE_UTF8_HEADER_SIZE 
				+ ROPE_UTF8_BYTELENGTH(rope));
			break;

		    case ROPE_TYPE_SUBROPE:
			/*
			 * Resolve source.
			 */

			ResolveRedirectRope(&ROPE_SUBROPE_SOURCE(rope));
			break;
                	    
		    case ROPE_TYPE_CONCAT:
			/*
			 * Resolve both arms.
			 */

			ResolveRedirectRope(&ROPE_CONCAT_LEFT(rope));
			ResolveRedirectRope(&ROPE_CONCAT_RIGHT(rope));
			break;

		    case ROPE_TYPE_CUSTOM:
			nbCells = NB_CELLS(ROPE_CUSTOM_HEADER_SIZE 
				+ ROPE_CUSTOM_SIZE(rope) 
				+ (ROPE_CUSTOM_TYPE(rope)->freeProc?ROPE_CUSTOM_TRAILER_SIZE:0));

			if (ROPE_CUSTOM_TYPE(rope)->freeProc) {
			    /*
			     * Resolve next in list.
			     */

			    ResolveRedirectRope(&ROPE_CUSTOM_NEXT(rope, 
				    ROPE_CUSTOM_SIZE(rope)));
			}

			if (ROPE_CUSTOM_TYPE(rope)->childrenProc) {
			    /*
			     * Resolve children.
			     */

			    ROPE_CUSTOM_TYPE(rope)->childrenProc(rope, 
				    ResolveRopeCustomChild, 0);
			}
			break;

		    case ROPE_TYPE_ROOT:
			/*
			 * Resolve source. No need to resolve next, as roots are
			 * never promoted.
			 */

			ResolveRedirectRope((Col_Rope *) &ROOT_SOURCE(rope));
			break;

		    case ROPE_TYPE_REDIRECT:
			/* 
			 * Clear cell. Note that this doesn't alter the redirect 
			 * rope data, that way it can still be resolved if other
			 * ropes still refer to it. 
			 */

			ClearCells(page, i, 1);
			break;

		    case ROPE_TYPE_REF:
			/*
			 * Resolve parent and child. No need to resolve next, as
			 * roots are never promoted.
			 */

			ResolveRedirectWord((Col_Word *) &REF_PARENT(rope));
			ResolveRedirectRope((Col_Rope *) &REF_CHILD(rope));
		}
	    }
	}

	/*
	 * Update flags for faster cleanup.
	 */

	if (isEmpty) {
	    PAGE_FLAGS(page) |= PAGE_FLAG_EMPTY;
	} else {
	    PAGE_FLAGS(page) &= ~PAGE_FLAG_EMPTY;
	}
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ResolveChildRedirects --
 *
 *	Resolve potentially promoted children of parents from uncollected 
 *	pools.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Overwrite all references to redirect cells, and free cells containing 
 *	the latter.
 *
 *---------------------------------------------------------------------------
 */

static void
ResolveChildRedirects(
    int maxChildGeneration)	/* Oldest collected generation. */
{
    int parentGen, childGen;
    const void *ref, *parent;
    int type;
    Col_WordType * typeInfo;

    /*
     * Note: generation (maxChildGeneration+1) is already resolved, and children
     * don't get promoted past first sub-generation of (maxChildGeneration+1).
     */

    for (parentGen = (maxChildGeneration+2)*2; parentGen < GC_MAX_GENERATIONS*2; 
	    parentGen++) {
	for (childGen = 0; childGen <= (maxChildGeneration+1)*2; childGen++) {
	    for (ref = refs[parentGen][childGen]; ref; ref = REF_NEXT(ref)) {
		parent = REF_PARENT(ref);
		if (IS_WORD(parent)) {
		    Col_Word word = parent;
		    type = WORD_TYPE(word);

		    /*
		     * Update fields of word that potentially refer to 
		     * redirected children.
		     */

		    switch (type) {
			case WORD_TYPE_REGULAR:
			    WORD_GET_TYPE_ADDR(word, typeInfo);

			    /*
			     * Resolve value. No need to resolve next, as by
			     * construction it is older and thus didn't get 
			     * promoted.
			     */

			    ResolveRedirectWord(&WORD_VALUE(word));

			    if (typeInfo->childrenProc) {
				/*
				 * Resolve children.
				 */

				typeInfo->childrenProc(word, 
				    ResolveWordChild, 0);
			    }
			    break;
		    }
		} else {
		    Col_Rope rope = parent;
		    type = ROPE_TYPE(rope);

		    /*
		     * Update fields of rope that potentially refer to 
		     * redirected children.
		     *
		     * Note: by construction, non-custom ropes never have 
		     * younger children.
		     */

		    switch (type) {
			case ROPE_TYPE_CUSTOM:
			    /*
			     * No need to resolve next, as by construction 
			     * it is older and thus didn't get promoted.
			     */

			    if (ROPE_CUSTOM_TYPE(rope)->childrenProc) {
				/*
				 * Resolve children.
				 */

				ROPE_CUSTOM_TYPE(rope)->childrenProc(rope, 
					ResolveRopeCustomChild, 0);
			    }
			    break;
		    }
		}
	    }
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
 *	Overwrite references to redirect ropes.
 *
 *---------------------------------------------------------------------------
 */

static void 
ResolveRedirectRope(
    Col_Rope *ropePtr)		/* Rope to resolve, may be overwritten. */
{
    while (ROPE_TYPE(*ropePtr) == ROPE_TYPE_REDIRECT) {
	*ropePtr = REDIRECT_SOURCE(*ropePtr);
    }
}
static void 
ResolveRedirectWord(
    Col_Word *wordPtr)		/* Word to resolve, may be overwritten. */
{
    if (!IS_WORD(wordPtr)) {
	ResolveRedirectRope((Col_Rope *) wordPtr);
    }
    while (WORD_TYPE(*wordPtr) == WORD_TYPE_REDIRECT) {
	*wordPtr = REDIRECT_SOURCE(*wordPtr);
    }
}
