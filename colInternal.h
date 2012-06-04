/*
 * Internal Header: colInternal.h
 *
 *	This header file defines internal structures and macros.
 */

#ifndef _COLIBRI_INTERNAL
#define _COLIBRI_INTERNAL

#include "colConf.h"

#ifdef _DEBUG
#   define DO_TRACE
#   define REQUIRE(x) {if (!(x)) Col_Error(COL_FATAL, ColibriDomain, COL_ERROR_ASSERTION, __FILE__, __LINE__, #x);}
#   define ASSERT(x) REQUIRE(x)
#else
#   define REQUIRE(x) (x)
#   define ASSERT(x)
#endif

#ifdef DO_TRACE
#   include <stdio.h>
#   define TRACE(format, ...) (fprintf(stderr, format, __VA_ARGS__), fflush(stderr))
#else
#   define TRACE(format, ...)
#endif


/*---------------------------------------------------------------------------
 * Internal Macro: CHAR_WIDTH
 *
 *	Get character width in buffer from format policy (numeric values are 
 *	chosen so that the lower 3 bits give the character width).
 *
 * Argument:
 *	format	- Format policy.
 *
 * Result:
 *	Character width in buffer for given string format.
 *
 * See also:
 *	<Col_StringFormat>
 *---------------------------------------------------------------------------*/

#define CHAR_WIDTH(format) \
    ((format) & 0x7)

/*---------------------------------------------------------------------------
 * Internal Macro: FORMAT_UTF
 *
 *	Whether format uses variable-width UTF format
 *
 * Argument:
 *	format	- Format policy.
 *
 * Result:
 *	Nonzero for UTF, else zero.
 *
 * See also:
 *	<Col_StringFormat>
 *---------------------------------------------------------------------------*/

#define FORMAT_UTF(format) \
    ((format) & 0x10)


/****************************************************************************
 * Internal Section: Error Handling
 ***************************************************************************/

extern const char * const ColibriDomain[];


/****************************************************************************
 * Internal Section: System Page Allocation
 *
 * Declarations:
 *	<SysPageProtect>
 *
 * Description:
 *	Pages are divided into cells. On a 32-bit system with 1024-byte logical 
 *	pages, each page stores 64 16-byte cells. On a 64-bit system with 
 *	4096-byte logical pages, each page stores 128 32-byte cells. 
 *
 *	Each page has reserved cells that store information about the page. The 
 *	remaining cells store word info. 
 ****************************************************************************/

void			SysPageProtect(void *page, int protect);

/*---------------------------------------------------------------------------
 * Internal Typedef: Page
 *
 *	Page-sized byte array.
 *
 * See also:
 *	<PAGE_SIZE>
 *---------------------------------------------------------------------------*/

typedef char Page[PAGE_SIZE];

/*---------------------------------------------------------------------------
 * Internal Typedef: Cell
 *
 *	Cell-sized byte array.
 *
 * See also:
 *	<CELL_SIZE>
 *---------------------------------------------------------------------------*/

typedef char Cell[CELL_SIZE];

/*---------------------------------------------------------------------------
 * Internal Constant: RESERVED_CELLS
 *
 *	Number of reserved cells in page.
 *
 * See also:
 *	<AVAILABLE_CELLS>
 *---------------------------------------------------------------------------*/

#define RESERVED_CELLS		1

/*---------------------------------------------------------------------------
 * Internal Constant: AVAILABLE_CELLS
 *
 *	Number of available cells in page. I.e., number of cells in page minus
 *	number of reserved cells.
 *
 * See also:
 *	<CELLS_PER_PAGE>, <RESERVED_CELLS>
 *---------------------------------------------------------------------------*/

#define AVAILABLE_CELLS		(CELLS_PER_PAGE-RESERVED_CELLS)

/*---------------------------------------------------------------------------
 * Internal Macro: NB_CELLS
 *
 *	Number of cells needed to store a given number of bytes.
 *
 * Argument:
 *	size	- Number of raw bytes to store.
 *
 * See also:
 *	<CELL_SIZE>
 *---------------------------------------------------------------------------*/

#define NB_CELLS(size) \
    (((size)+CELL_SIZE-1)/CELL_SIZE)

/*---------------------------------------------------------------------------
 * Internal Macros: PAGE_* Accessors
 *
 *	Page field accessor macros.
 *
 * Layout:
 *	On all architectures, the first cell is reserved and is formatted as
 *	follows:
 *
 * (start table)
 *      0     3 4     7 8                                             n
 *     +-------+-------+-----------------------------------------------+
 *   0 |  Gen  | Flags |                     Next                      |
 *     +-------+-------+-----------------------------------------------+
 *   1 |                           Group Data                          |
 *     +---------------------------------------------------------------+
 *   2 |                                                               |
 *     +                            Bitmask                            +
 *   3 |                                                               |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	PAGE_NEXT		- Get next page in pool.
 *	PAGE_SET_NEXT		- Set next page in pool. 
 *	PAGE_GENERATION		- Get generation of pool containing this page. 
 *	PAGE_SET_GENERATION	- Set generation of pool containing this page. 
 *	PAGE_FLAG		- Get flag.
 *	PAGE_SET_FLAG		- Set flag.
 *	PAGE_CLEAR_FLAG		- Clear flag.
 *	PAGE_GROUPDATA		- Data for the thread group the page belongs to.
 *	PAGE_BITMASK		- Bitmask for allocated cells in page.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Page>
 *---------------------------------------------------------------------------*/

#define PAGE_NEXT_MASK			(~(PAGE_GENERATION_MASK|PAGE_FLAGS_MASK))
#define PAGE_GENERATION_MASK		0x0F
#define PAGE_FLAGS_MASK			0xF0
#define PAGE_NEXT(page)			((Page *)((*(uintptr_t *)(page)) & PAGE_NEXT_MASK))
#define PAGE_SET_NEXT(page, next)	(*(uintptr_t *)(page) &= ~PAGE_NEXT_MASK, *(uintptr_t *)(page) |= ((uintptr_t)(next)) & PAGE_NEXT_MASK)
#define PAGE_GENERATION(page)		((*(uintptr_t *)(page)) & PAGE_GENERATION_MASK)
#define PAGE_SET_GENERATION(page, gen)	(*(uintptr_t *)(page) &= ~PAGE_GENERATION_MASK, *(uintptr_t *)(page) |= (gen) & PAGE_GENERATION_MASK)
#define PAGE_FLAG(page, flag)		((*(uintptr_t *)(page)) & (flag))
#define PAGE_SET_FLAG(page, flag)	((*(uintptr_t *)(page)) |= (flag))
#define PAGE_CLEAR_FLAG(page, flag)	((*(uintptr_t *)(page)) &= ~(flag))
#define PAGE_GROUPDATA(page)		(*((GroupData **)(page)+1))
#define PAGE_BITMASK(page)		((uint8_t *)(page)+sizeof(Page *)*2)

/*---------------------------------------------------------------------------
 * Internal Constants: PAGE_FLAG_* Constants
 *
 *	Page flag constants.
 *
 *  PAGE_FLAG_FIRST	- Marks first page in group.
 *  PAGE_FLAG_LAST	- Marks last page in group.
 *  PAGE_FLAG_PARENT	- Marks pages having parent cells.
 *
 * See also:
 *	<PAGE_FLAG>, <PAGE_SET_FLAG>, <PAGE_CLEAR_FLAG>
 *---------------------------------------------------------------------------*/

#define PAGE_FLAG_FIRST			0x10
#define PAGE_FLAG_LAST			0x20
#define PAGE_FLAG_PARENT		0x40

/*---------------------------------------------------------------------------
 * Internal Macro: PAGE_CELL
 *
 *	Get index-th cell in page.
 *
 * Arguments:
 *	page	- Page containing cell.
 *	index	- Index of cell in page.
 *
 * See also:
 *	<Cell>
 *---------------------------------------------------------------------------*/

#define PAGE_CELL(page, index) \
    ((Cell *)(page)+(index))

/*---------------------------------------------------------------------------
 * Internal Macro: CELL_PAGE
 *
 *	Get page containing the cell.
 *
 * Argument:
 *	cell	- Cell to get page for.
 *
 * See also:
 *	<Page>, <PAGE_SIZE>
 *---------------------------------------------------------------------------*/

#define CELL_PAGE(cell) \
    ((Page *)((uintptr_t)(cell) & ~(PAGE_SIZE-1)))

/*---------------------------------------------------------------------------
 * Internal Macro: CELL_PAGE
 *
 *	Get index of cell in page.
 *
 * Argument:
 *	cell	- Cell to get index for.
 *
 * See also:
 *	<PAGE_SIZE>, <CELL_SIZE>
 *---------------------------------------------------------------------------*/

#define CELL_INDEX(cell) \
    (((uintptr_t)(cell) % PAGE_SIZE) / CELL_SIZE)


/****************************************************************************
 * Internal Section: Memory Pools
 *
 * Declarations:
 *	<PoolInit>, <PoolCleanup>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Typedef: MemoryPool
 *
 *	Memory pools are a set of pages storing the cells of a given generation.
 *
 * Fields:
 *	generation	- Generation level; 0 = younger, 1 = eden, 2+ = older.
 *	pages		- Pages form a singly-linked list.
 *	lastPage	- Last page in pool. 
 *	lastFreeCell	- Last cell sequence of a given size found.
 *	nbPages		- Number of pages in pool.
 *	nbAlloc		- Number of pages alloc'd since last GC.
 *	nbSetCells	- Number of set cells in pool.
 *	gc		- GC counter. Used for generational GC.
 *	sweepables	- List of cells that need sweeping when unreachable 
 *			  after a GC. 
 *
 * See also:
 *	<Page>, <Cell>
 *---------------------------------------------------------------------------*/

typedef struct MemoryPool {
    unsigned int generation;
    Page *pages;
    Page *lastPage;
    Cell *lastFreeCell[AVAILABLE_CELLS]; 
    size_t nbPages;
    size_t nbAlloc;
    size_t nbSetCells;
    size_t gc;
    Col_Word sweepables;
} MemoryPool;

void			PoolInit(MemoryPool *pool, unsigned int generation);
void			PoolCleanup(MemoryPool *pool);


/****************************************************************************
 * Internal Section: Page Allocation
 *
 * Declarations:
 *	<PoolAllocPages>, <PoolFreeEmptyPages>
 ****************************************************************************/

void			PoolAllocPages(MemoryPool *pool, size_t number);
void			PoolFreeEmptyPages(MemoryPool *pool);


/****************************************************************************
 * Internal Section: Cell Allocation
 *
 * Declarations:
 *	<PoolAllocCells>, <SetCells>, <ClearCells>, <ClearAllCells>, <TestCell>,
 *	<NbSetCells>, <AllocCells>
 ****************************************************************************/

Cell *			PoolAllocCells(MemoryPool *pool, size_t number);
void			SetCells(Page *page, size_t first, size_t number);
void			ClearCells(Page *page, size_t first, size_t number);
void			ClearAllCells(Page *page);
int			TestCell(Page *page, size_t index);
size_t			NbSetCells(Page *page);
Cell *			AllocCells(size_t number);


/****************************************************************************
 * Internal Section: Process & Threads
 *
 * Declarations:
 *	<GcInitThread>, <GcInitGroup>, <GcCleanupThread>, <GcCleanupGroup>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Typedef: GroupData
 *
 *	Group-local data.
 *
 * Fields:
 *	model			- Threading model.
 *	rootPool		- Memory pool used to store root & parent page 
 *				  descriptor cells.
 *	roots			- Root descriptors are stored in a trie indexed 
 *				  by the root cell address.
 *	parents			- Parent descriptors are stored in a linked 
 *				  list.
 *	pools			- Memory pools used to store words for 
 *				  generations older than eden 
 *				  (1 < generation < <GC_MAX_GENERATIONS>).
 *	maxCollectedGeneration	- Oldest collected generation during current GC.
 *	compactGeneration	- Generation on which to perform compaction 
 *				  during promotion (see <PROMOTE_COMPACT>).
 *	first			- Group member threads form a circular list.
 *
 * See also:
 *	<Threading Model Constants>, <MemoryPool>, <ThreadData>, 
 *	<PlatEnter>, <PlatLeave>, <AllocGroupData>, <FreeGroupData>
 *---------------------------------------------------------------------------*/

typedef struct GroupData {
    unsigned int model;
    MemoryPool rootPool;
    Cell *roots;
    Cell *parents;
    MemoryPool pools[GC_MAX_GENERATIONS-2];
    unsigned int maxCollectedGeneration;
#ifdef PROMOTE_COMPACT
    unsigned int compactGeneration;
#endif
    struct ThreadData *first;
} GroupData;

/*---------------------------------------------------------------------------
 * Internal Typedef: ThreadData
 *
 *	Thread-local data.
 *
 * Fields:
 *	next		- Next in thread group member circular list.
 *	groupData	- Group-local data.
 *	nestCount	- Number of nested calls to Col_Init. Clear structures 
 *			  once it drops to zero.
 *	errorProc	- Error procs are thread-local.
 *	pauseGC		- GC-protected section counter, i.e. nb of nested pause 
 *			  calls. When positive, we are in a GC-protected 
 *			  section.
 *	eden		- Eden, i.e. memory pool for generation 1. New cells are
 *			  always created in thread-local eden pools for minimum
 *			  contention.
 *
 * See also:
 *	<Threading Model Constants>, <MemoryPool>, <GroupData>, <PlatEnter>,
 *	<PlatLeave>
 *---------------------------------------------------------------------------*/

typedef struct ThreadData {
    struct ThreadData *next;
    GroupData *groupData;
    size_t nestCount;
    Col_ErrorProc *errorProc;
    size_t pauseGC;
    MemoryPool eden;
} ThreadData;

void			GcInitThread(ThreadData *data);
void			GcInitGroup(GroupData *data);
void			GcCleanupThread(ThreadData *data);
void			GcCleanupGroup(GroupData *data);


/****************************************************************************
 * Internal Section: Garbage Collection
 *
 * Declarations:
 *	<PerformGC>, <RememberSweepable>, <CleanupSweepables>
****************************************************************************/

void			PerformGC(GroupData *data);
void			RememberSweepable(Col_Word word, 
			    Col_CustomWordType *type);
void			CleanupSweepables(MemoryPool *pool);


/****************************************************************************
 * Internal Section: Roots and Parents
 *
 * Declarations:
 *	<UpdateParents>
 ****************************************************************************/

void			UpdateParents(GroupData *data);

/*---------------------------------------------------------------------------
 * Internal Macros: ROOT_* Accessors
 *
 *	Root cell accessor macros.
 *
 *	Roots are explicitly preserved words, using a reference count.
 *
 *	Roots are stored in a trie indexed by the root source addresses. This 
 *	forms an ordered collection that is traversed upon GC and is indexable 
 *	and modifiable in O(log(k)) worst-case, k being the bit size of a cell 
 *	pointer.
 *
 *	There are two kinds of cells that form the root trie: nodes and leaves.
 *	Nodes are intermediate cells pointing to two children. Leaves are 
 *	terminal cells pointing to the preserved word.
 *
 * Common Fields:
 *	ROOT_IS_LEAF	- Whether pointed cell is a root trie node or leaf. To
 *			  distinguish between both types the least significant
 *			  bit of leaf pointers is set.
 *	ROOT_GET_NODE	- Get actual pointer to node.
 *	ROOT_GET_LEAF	- Get actual pointer to leaf.
 *	ROOT_PARENT	- Parent node in trie. NULL for trie root.
 *
 *
 * Nodes:
 *	On all architectures the single-cell layout is as follows:
 *
 * (start table)
 *      0                                                             n
 *     +---------------------------------------------------------------+
 *   0 |                             Mask                              |
 *     +---------------------------------------------------------------+
 *   1 |                            Parent                             |
 *     +---------------------------------------------------------------+
 *   2 |                             Left                              |
 *     +---------------------------------------------------------------+
 *   3 |                             Right                             |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	ROOT_NODE_MASK	- Bitmask where only the critical bit is set, i.e. the 
 *			  highest bit where left and right subtries differ.
 *	ROOT_NODE_LEFT	- Left subtrie.
 *	ROOT_NODE_RIGHT	- Right subtrie.
 *
 *
 * Leaves:
 *	On all architectures the single-cell layout is as follows:
 *
 * (start table)
 *      0                            31                               n
 *     +-------------------------------+-------------------------------+
 *   0 |          Generation           |        Unused (n > 32)        |
 *     +-------------------------------+-------------------------------+
 *   1 |                            Parent                             |
 *     +---------------------------------------------------------------+
 *   2 |                        Reference count                        |
 *     +---------------------------------------------------------------+
 *   3 |                            Source                             |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	ROOT_LEAF_GENERATION	- Generation of the source page. Storing it here
 *				  saves a pointer dereference.
 *	ROOT_LEAF_REFCOUNT	- Reference count. Once it drops to zero the 
 *				  root is removed, however the source may 
 *				  survive if it is referenced elsewhere.
 *	ROOT_LEAF_SOURCE	- Preserved word.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<ROOT_NODE_INIT>, <ROOT_LEAF_INIT>, <Col_WordPreserve>, 
 *	<Col_WordRelease>
 *---------------------------------------------------------------------------*/

#define ROOT_IS_LEAF(cell)		((uintptr_t)(cell) & 1)
#define ROOT_GET_NODE(cell)		((Cell *)((uintptr_t)(cell) & ~1))
#define ROOT_GET_LEAF(cell)		((Cell *)((uintptr_t)(cell) | 1))

#define ROOT_PARENT(cell)		(((Cell **)(cell))[1])

#define ROOT_NODE_MASK(cell)		(((uintptr_t *)(cell))[0])
#define ROOT_NODE_LEFT(cell)		(((Cell **)(cell))[2])
#define ROOT_NODE_RIGHT(cell)		(((Cell **)(cell))[3])

#define ROOT_LEAF_GENERATION(cell)	(((unsigned int *)(cell))[0])
#define ROOT_LEAF_REFCOUNT(cell)	(((size_t *)(cell))[2])
#define ROOT_LEAF_SOURCE(cell)		(((Col_Word *)(cell))[3])

/*---------------------------------------------------------------------------
 * Internal Macro: ROOT_NODE_INIT
 *
 *	Root trie node initializer.
 *
 * Arguments:
 *	cell	- Cell to initialize. (Caution: evaluated several times after 
 *		  macro expansion)
 *	parent	- <ROOT_PARENT>
 *	mask	- <ROOT_NODE_MASK>
 *	left	- <ROOT_NODE_LEFT>
 *	right	- <ROOT_NODE_RIGHT>
 *---------------------------------------------------------------------------*/

#define ROOT_NODE_INIT(cell, parent, mask, left, right) \
    ROOT_PARENT(cell) = parent; \
    ROOT_NODE_MASK(cell) = mask; \
    ROOT_NODE_LEFT(cell) = left; \
    ROOT_NODE_RIGHT(cell) = right;

/*---------------------------------------------------------------------------
 * Internal Macro: ROOT_LEAF_INIT
 *
 *	Root trie leaf initializer.
 *
 * Arguments:
 *	cell		- Cell to initialize. (Caution: evaluated several times
 *			  during macro expansion)
 *	parent		- <ROOT_PARENT>
 *	generation	- <ROOT_LEAF_GENERATION>
 *	refcount	- <ROOT_LEAF_REFCOUNT>
 *	source		- <ROOT_LEAF_SOURCE>
 *---------------------------------------------------------------------------*/

#define ROOT_LEAF_INIT(cell, parent, generation, refcount, source) \
    ROOT_PARENT(cell) = parent; \
    ROOT_LEAF_GENERATION(cell) = generation; \
    ROOT_LEAF_REFCOUNT(cell) = refcount; \
    ROOT_LEAF_SOURCE(cell) = source;

/*---------------------------------------------------------------------------
 * Internal Macros: PARENT_* Accessors
 *
 *	Parent cell accessor macros.
 *
 *	Parents are cells pointing to other cells of a newer generation. During
 *	GC, parents from uncollected generations are traversed in addition to
 *	roots from collected generations, and the parent list is updated.
 *
 *	When a page of an older generation is written, this means that it may
 *	contain parents. Such "dirty" pages are added to the parent list at each
 *	GC, and each page is checked for potential parents.
 *
 * Layout:
 *	On all architectures the single-cell layout is as follows:
 *
 * (start table)
 *      0                                                             n
 *     +---------------------------------------------------------------+
 *   0 |                             Next                              |
 *     +---------------------------------------------------------------+
 *   1 |                             Page                              |
 *     +---------------------------------------------------------------+
 *   2 |                                                               |
 *     +                            Unused                             +
 *   3 |                                                               |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	PARENT_NEXT		- Parent nodes are linked together in a 
 *				  singly-linked list for traversal during GC.
 *	PARENT_PAGE		- Pointer to source page.
 *
 * See also:
 *	<PARENT_INIT>, <MarkReachableCellsFromParents>
 *---------------------------------------------------------------------------*/

#define PARENT_NEXT(cell)	(((Cell **)(cell))[1])
#define PARENT_PAGE(cell)	(((Page **)(cell))[2])

/*---------------------------------------------------------------------------
 * Internal Macro: PARENT_INIT
 *
 *	Parent cell initializer.
 *
 * Arguments:
 *	cell		- Cell to initialize. (Caution: evaluated several times
 *			  during macro expansion)
 *	next		- <PARENT_NEXT>
 *	page		- <PARENT_PAGE>
 *
 * See also:
 *	<MarkReachableCellsFromParents>
 *---------------------------------------------------------------------------*/

#define PARENT_INIT(cell, next, page) \
    PARENT_NEXT(cell) = next; \
    PARENT_PAGE(cell) = page;

#endif /* _COLIBRI_INTERNAL */
