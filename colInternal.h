/*
 * Internal File: colInternal.h
 *
 *	This header file defines internal structures and macros, notably 
 *	predefined word types.
 */

#ifndef _COLIBRI_INTERNAL
#define _COLIBRI_INTERNAL

#include "colConf.h"

#ifdef _DEBUG
#   define DO_TRACE
#   define ASSERT(x) {if (!(x)) Col_Error(COL_FATAL, "%s(%d) : assertion failed! (%s)", __FILE__, __LINE__, #x);}
#else
#   define ASSERT(x)
#endif

#ifdef DO_TRACE
#   include <stdio.h>
#   define TRACE(format, ...) (fprintf(stderr, format, __VA_ARGS__), fflush(stderr))
#else
#   define TRACE(format, ...)
#endif

/*
 * Byte rotation in word.
 */

#define ROTATE_LEFT(value)	(((value)>>((sizeof(value)-1)*8)) | ((value)<<8))
#define ROTATE_RIGHT(value)	(((value)<<((sizeof(value)-1)*8)) | ((value)>>8))


/****************************************************************************
 * Internal Group: Bit Handling
 *
 * Declarations:
 *	<firstZeroBitSequence>, <longestLeadingZeroBitSequence>, <nbBitsSet>
 ****************************************************************************/

extern const char firstZeroBitSequence[7][256];
extern const char longestLeadingZeroBitSequence[256];
extern const char nbBitsSet[256];


/****************************************************************************
 * Internal Group: System Page Allocation
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

#define RESERVED_CELLS \
    1

/*---------------------------------------------------------------------------
 * Internal Constant: AVAILABLE_CELLS
 *
 *	Number of available cells in page. I.e., number of cells in page minus
 *	number of reserved cells.
 *
 * See also:
 *	<CELLS_PER_PAGE>, <RESERVED_CELLS>
 *---------------------------------------------------------------------------*/

#define AVAILABLE_CELLS \
    (CELLS_PER_PAGE-RESERVED_CELLS)

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
 * Internal Macros: Page Fields
 *
 *	Accessors for page fields.
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
 * Internal Constants: Page flags
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
 * Internal Group: Memory Pools
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
 *	generation	- Generation level; 0 = younger.
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
 * Internal Group: Page Allocation
 *
 * Declarations:
 *	<PoolAllocPages>, <PoolFreeEmptyPages>
 ****************************************************************************/

void			PoolAllocPages(MemoryPool *pool, size_t number);
void			PoolFreeEmptyPages(MemoryPool *pool);


/****************************************************************************
 * Internal Group: Cell Allocation
 *
 * Declarations:
 *	<PoolAllocCells>, <SetCells>, <ClearCells>, <ClearAllCells>, <TestCell>,
 *	<NbSetCells>
 ****************************************************************************/

Cell *			PoolAllocCells(MemoryPool *pool, size_t number);
void			SetCells(Page *page, size_t first, size_t number);
void			ClearCells(Page *page, size_t first, size_t number);
void			ClearAllCells(Page *page);
int			TestCell(Page *page, size_t index);
size_t			NbSetCells(Page *page);


/****************************************************************************
 * Internal Group: Process & Threads
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
 *	<Threading Models>, <MemoryPool>, <ThreadData>, 
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
 *	<Threading Models>, <MemoryPool>, <GroupData>, <PlatEnter>, <PlatLeave>
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
 * Internal Group: Cell Allocation
 *
 * Declarations:
 *	<AllocCells>
 ****************************************************************************/

Cell *			AllocCells(size_t number);


/****************************************************************************
 * Internal Group: Garbage Collection
 *
 * Declarations:
 *	<PerformGC>
****************************************************************************/

void			PerformGC(GroupData *data);


/****************************************************************************
 * Internal Group: Word Lifetime Management
 *
 * Declarations:
 *	<DeclareCustomWord>
 ****************************************************************************/

void			DeclareCustomWord(Col_Word word, 
			    Col_CustomWordType *type);


/****************************************************************************
 * Internal Group: Roots and Parents
 *
 * Declarations:
 *	<UpdateParents>
 ****************************************************************************/

void			UpdateParents(GroupData *data);

/*---------------------------------------------------------------------------
 * Internal Macros: Root Cells
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
 *	<ROOT_NODE_INIT>, <ROOT_LEAF_INIT>, <Col_PreserveWord>, 
 *	<Col_ReleaseWord>
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
 *	Initializer for root trie nodes.
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
 *	Initializer for root trie leaves.
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
 * Internal Macros: Parent Cells
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
 *	Initializer for parent cells.
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


/****************************************************************************
 * Internal Group: Word Types
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Constants: Word Type Identifiers
 *
 *	Word types are identified by a numerical ID. 
 *
 *	WORD_TYPE_NIL		- Nil singleton.
 *	WORD_TYPE_CUSTOM	- Custom Words (<WORD_CUSTOM_INIT>)
 *	WORD_TYPE_SMALLINT	- Small Integers (<WORD_SMALLINT_NEW>)
 *	WORD_TYPE_SMALLFP	- Small Floating Points
 *	WORD_TYPE_CHAR		- Character Words (<WORD_CHAR_NEW>)
 *	WORD_TYPE_SMALLSTR	- Small String Words (<WORD_SMALLSTR_DATA>, 
 *				  <WORD_SMALLSTR_SET_LENGTH>)
 *	WORD_TYPE_CIRCLIST	- Circular Lists (<WORD_CIRCLIST_NEW>)
 *	WORD_TYPE_VOIDLIST	- Void Lists (<WORD_VOIDLIST_NEW>)
 *	WORD_TYPE_WRAP		- Wrap Words (<WORD_WRAP_INIT>)
 *	WORD_TYPE_UCSSTR	- Fixed-Width Ropes (<WORD_UCSSTR_INIT>)
 *	WORD_TYPE_UTF8STR	- Variable-Width UTF-8 ropes 
 *				  (<WORD_UTF8STR_INIT>)
 *	WORD_TYPE_SUBROPE	- Subropes (<WORD_SUBROPE_INIT>)
 *	WORD_TYPE_CONCATROPE	- Concat Ropes (<WORD_CONCATROPE_INIT>)
 *	WORD_TYPE_INT		- Integers (<WORD_INT_INIT>)
 *	WORD_TYPE_FP		- Floating Points (<WORD_FP_INIT>)
 *	WORD_TYPE_VECTOR	- Immutable Vectors (<WORD_VECTOR_INIT>)
 *	WORD_TYPE_MVECTOR	- Mutable Vectors (<WORD_MVECTOR_INIT>)
 *	WORD_TYPE_SUBLIST	- Sublists (<WORD_SUBLIST_INIT>)
 *	WORD_TYPE_CONCATLIST	- Immutable Concat Lists 
 *				  (<WORD_CONCATLIST_INIT>)
 *	WORD_TYPE_MCONCATLIST	- Mutable Concat Lists (<WORD_MCONCATLIST_INIT>)
 *	WORD_TYPE_MLIST		- Mutable Lists (<WORD_MLIST_INIT>)
 *	WORD_TYPE_STRHASHMAP	- String Hash Maps (<WORD_STRHASHMAP_INIT>)
 *	WORD_TYPE_INTHASHMAP	- Integer Hash Maps (<WORD_INTHASHMAP_INIT>)
 *	WORD_TYPE_HASHENTRY	- Immutable Hash Entries (<WORD_HASHENTRY_INIT>)
 *	WORD_TYPE_MHASHENTRY	- Mutable Hash Entries (<WORD_MHASHENTRY_INIT>)
 *	WORD_TYPE_INTHASHENTRY	- Immutable Integer Hash Entries 
 *				  (<WORD_INTHASHENTRY_INIT>)
 *	WORD_TYPE_MINTHASHENTRY	- Mutable Integer Hash Entries 
 *				  (<WORD_MINTHASHENTRY_INIT>)
 *	WORD_TYPE_STRTRIEMAP	- String Trie Maps (<WORD_STRTRIEMAP_INIT>)
 *	WORD_TYPE_INTTRIEMAP	- Integer Trie Maps (<WORD_INTTRIEMAP_INIT>)
 *	WORD_TYPE_STRTRIENODE	- Immutable String Trie Nodes 
 *				  (<WORD_STRTRIENODE_INIT>)
 *	WORD_TYPE_MSTRTRIENODE	- Mutable String Trie Nodes 
 *				  (<WORD_MSTRTRIENODE_INIT>)
 *	WORD_TYPE_INTTRIENODE	- Immutable Integer Trie Nodes 
 *				  (<WORD_INTTRIENODE_INIT>)
 *	WORD_TYPE_MINTTRIENODE	- Mutable Integer Trie Nodes 
 *				  (<WORD_MINTTRIENODE_INIT>)
 *	WORD_TYPE_TRIELEAF	- Immutable Trie Leaves (<WORD_TRIELEAF_INIT>)
 *	WORD_TYPE_MTRIELEAF	- Mutable Trie Leaves (<WORD_MTRIELEAF_INIT>)
 *	WORD_TYPE_INTTRIELEAF	- Immutable Integer Trie Leaves
 *				  (<WORD_INTTRIELEAF_INIT>)
 *	WORD_TYPE_MINTTRIELEAF	- Mutable Integer Trie Leaves
 *				  (<WORD_MINTTRIELEAF_INIT>)
 *	WORD_TYPE_REDIRECT	- Redirects
 *	WORD_TYPE_UNKNOWN	- Used as a tag in the source code to mark 
 *				  places where predefined type specific code is
 *				  needed. Search for this tag when adding new 
 *				  predefined word types.
 *
 * Note:
 *	Predefined type IDs for regular word types are chosen so that their bit
 *	0 is cleared, their bit 1 is set, and their value fit into a byte. This
 *	gives up to 64 predefined type IDs (2-254 with steps of 4).
 *
 *	Immediate word type IDs use negative values to avoid clashes with 
 *	regular word type IDs.
 *---------------------------------------------------------------------------*/
 
#define WORD_TYPE_NIL		0

#define WORD_TYPE_CUSTOM	-1

#define WORD_TYPE_SMALLINT	-2
#define WORD_TYPE_SMALLFP	-3
#define WORD_TYPE_CHAR		-4
#define WORD_TYPE_SMALLSTR	-5
#define WORD_TYPE_CIRCLIST	-6
#define WORD_TYPE_VOIDLIST	-7

#define WORD_TYPE_WRAP		2

#define WORD_TYPE_UCSSTR	6
#define WORD_TYPE_UTF8STR	10
#define WORD_TYPE_SUBROPE	14
#define WORD_TYPE_CONCATROPE	18

#define WORD_TYPE_INT		22
#define WORD_TYPE_FP		26

#define WORD_TYPE_VECTOR	30
#define WORD_TYPE_MVECTOR	34
#define WORD_TYPE_SUBLIST	38
#define WORD_TYPE_CONCATLIST	42
#define WORD_TYPE_MCONCATLIST	46
#define WORD_TYPE_MLIST		50
#define WORD_TYPE_STRHASHMAP	54
#define WORD_TYPE_INTHASHMAP	58
#define WORD_TYPE_HASHENTRY	62
#define WORD_TYPE_MHASHENTRY	66
#define WORD_TYPE_INTHASHENTRY	70
#define WORD_TYPE_MINTHASHENTRY	74
#define WORD_TYPE_STRTRIEMAP	78
#define WORD_TYPE_INTTRIEMAP	82
#define WORD_TYPE_STRTRIENODE	86
#define WORD_TYPE_MSTRTRIENODE	90
#define WORD_TYPE_INTTRIENODE	94
#define WORD_TYPE_MINTTRIENODE	98
#define WORD_TYPE_TRIELEAF	102
#define WORD_TYPE_MTRIELEAF	106
#define WORD_TYPE_INTTRIELEAF	110
#define WORD_TYPE_MINTTRIELEAF	114
#ifdef PROMOTE_COMPACT
#   define WORD_TYPE_REDIRECT	254
#endif

/*---------------------------------------------------------------------------
 * Internal Variable: immediateWordTypes
 *
 *	Lookup table for immediate word types, gives the word type ID from the
 *	first 5 bits of the word value.
 *
 * See also:
 *	<Immediate Word Fields>
 *---------------------------------------------------------------------------*/

static const int immediateWordTypes[32] = {
    WORD_TYPE_NIL,	/* 00000 */
    WORD_TYPE_SMALLINT,	/* 00001 */
    WORD_TYPE_SMALLFP,	/* 00010 */
    WORD_TYPE_SMALLINT,	/* 00011 */
    WORD_TYPE_CHAR,	/* 00100 */
    WORD_TYPE_SMALLINT,	/* 00101 */
    WORD_TYPE_SMALLFP,	/* 00110 */
    WORD_TYPE_SMALLINT,	/* 00111 */
    WORD_TYPE_CIRCLIST,	/* 01000 */
    WORD_TYPE_SMALLINT,	/* 01001 */
    WORD_TYPE_SMALLFP,	/* 01010 */
    WORD_TYPE_SMALLINT,	/* 01011 */
    WORD_TYPE_SMALLSTR,	/* 01100 */
    WORD_TYPE_SMALLINT,	/* 01101 */
    WORD_TYPE_SMALLFP,	/* 01110 */
    WORD_TYPE_SMALLINT,	/* 01111 */
    WORD_TYPE_NIL,	/* 10000 */
    WORD_TYPE_SMALLINT,	/* 10001 */
    WORD_TYPE_SMALLFP,	/* 10010 */
    WORD_TYPE_SMALLINT,	/* 10011 */
    WORD_TYPE_VOIDLIST,	/* 10100 */
    WORD_TYPE_SMALLINT,	/* 10101 */
    WORD_TYPE_SMALLFP,	/* 10110 */
    WORD_TYPE_SMALLINT,	/* 10111 */
    WORD_TYPE_CIRCLIST,	/* 11000 */
    WORD_TYPE_SMALLINT,	/* 11001 */
    WORD_TYPE_SMALLFP,	/* 11010 */
    WORD_TYPE_SMALLINT,	/* 11011 */
    WORD_TYPE_CIRCLIST,	/* 11100 */
    WORD_TYPE_SMALLINT,	/* 11101 */
    WORD_TYPE_SMALLFP,	/* 11110 */
    WORD_TYPE_SMALLINT,	/* 11111 */
};

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_TYPE
 *
 *	Get word type identifier.
 *
 * Argument:
 *	word	- Word to get type for. (Caution: evaluated several times during
 *		  macro expansion)
 *
 * Result:
 *	Word type identifier.
 *
 * See also:
 *	<Word Type Identifiers>, <immediateWordTypes>
 *---------------------------------------------------------------------------*/

/* Careful: don't give arguments with side effects! */
#define WORD_TYPE(word) \
    /* Nil? */							\
    ((!(word))?				WORD_TYPE_NIL		\
    /* Immediate Word? */					\
    :(((uintptr_t)(word))&15)?		immediateWordTypes[(((uintptr_t)(word))&31)] \
    /* Predefined Type ID? */					\
    :((((uint8_t *)(word))[0])&2)?	WORD_TYPEID(word)	\
    /* Custom Type */						\
    :					WORD_TYPE_CUSTOM)

/****************************************************************************
 * Internal Group: Immediate Words
 *
 * Files:
 *	<colWord.c>, <colWord.h>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macros: Immediate Word Fields
 *
 * Immediate Words:
 *	Immediate words are immutable datatypes that store their value directly 
 *	in the word identifier, contrary to regular words whose identifier is a 
 *	pointer to a cell-based structure. As cells are aligned on a multiple of
 *	their size (16 bytes on 32-bit systems), this means that a number of low
 *	bits are always zero in regular word identifiers (4 bits on 32-bit 
 *	systems with 16-byte cells). Immediate values are distinguished from 
 *	regular pointers by setting one of these bits. Immediate word types are
 *	identified by these bit patterns, called tags. The following binary tags
 *	are recognized:
 *
 * Nil:
 *	<WORD_TYPE_NIL>
 *
 * (start table)
 *      0                                                             n
 *     +---------------------------------------------------------------+
 *   0 |                               0                               |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	Nil is a singleton word whose identifier is <WORD_NIL> or zero.
 *
 *
 * Small Integers:
 *	<WORD_TYPE_SMALLINT>
 *
 * (start table)
 *      0 1                                                           n
 *     +-+-------------------------------------------------------------+
 *   0 |1|                            Value                            |
 *     +-+-------------------------------------------------------------+
 * (end)
 *
 *	Small integer words are integers whose width is one bit less than 
 *	machine integers. Larger integers are cell-based.
 *
 *	WORD_SMALLINT_GET	- Get value of small integer word.
 *
 *
 * Small Floating Points:
 *	<WORD_TYPE_SMALLFP>
 *
 * (start table)
 *      01 2                                                           n
 *     +--+------------------------------------------------------------+
 *   0 |01|                           Value                            |
 *     +--+------------------------------------------------------------+
 * (end)
 *
 *	Small floating point words are floating points whose lower 2 bits of the
 *	mantissa are zero, so that they are free for the tag bits. Other values
 *	are cell-based.
 *
 *	Note that because of language restrictions (bitwise operations on 
 *	floating points are forbidden), we have to use the intermediary union 
 *	type <FloatConvert> for conversions.
 *
 *	WORD_SMALLFP_GET	- Get value of small floating point word.
 *
 *
 * Unicode Chars:
 *	<WORD_TYPE_CHAR>
 *
 * (start table)
 *      0   4 5                      31                               n
 *     +-----+-------------------------+-------------------------------+
 *   0 |00100|        Codepoint        |        Unused (n > 32)        |
 *     +-----+-------------------------+-------------------------------+
 * (end)
 *
 *	Unicode char words can store one Unicode character (27-bit, which is
 *	sufficient in the current Unicode standard).
 *	
 *	WORD_CHAR_GET	- Get Unicode codepoint of char word.
 *
 *
 * Small Strings:
 *	<WORD_TYPE_SMALLSTR>
 *
 * (start table)
 *      0   4 5            7 8                                        n
 *     +-----+--------------+------------------------------------------+
 *   0 |00110| 0<=Length<=7 |                  String                  |
 *     +-----+--------------+------------------------------------------+
 * (end)
 *
 *	Small string words can store short 8-bit strings. Length is the machine
 *	word size minus one (i.e. 3 on 32-bit, 7 on 64-bit). Larger strings are
 *	cell-based.
 *	
 *	WORD_SMALLSTR_LENGTH		- Get small string length.
 *	WORD_SMALLSTR_SET_LENGTH	- Set small string length.
 *	WORD_SMALLSTR_DATA		- Small string data.
 *
 *
 * Circular lists:
 *	<WORD_TYPE_CIRCLIST>
 *
 * (start table)
 *      0  3 4                                                        n
 *     +----+----------------------------------------------------------+
 *   0 |0001|                    Core (regular list)                   |
 *     +----+----------------------------------------------------------+
 *
 *      0   4 5                                                       n
 *     +-----+---------------------------------------------------------+
 *   0 |00111|                Core length (void list)                   |
 *     +-----+---------------------------------------------------------+
 * (end)
 *	
 *	Circular lists are lists made of a core list that repeats infinitely.
 *	It can be either a regular list or an immediate void list.
 *	
 *	WORD_CIRCLIST_CORE	- Get core list.
 *
 *
 * Void lists:
 *	<WORD_TYPE_VOIDLIST>
 *
 * (start table)
 *      0   4 5                                                       n
 *     +-----+---------------------------------------------------------+
 *   0 |00101|                         Length                          |
 *     +-----+---------------------------------------------------------+
 * (end)
 *
 *	Void lists are lists whose elements are all nil. Length width is the
 *	machine word width minus 4 bits, so the maximum length is about 1/16th
 *	of the theoretical maximum. Larger void lists are built by concatenating
 *	several shorter immediate void lists.
 *	
 *	WORD_VOIDLIST_LENGTH	- Get void list length.
 *
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 *	Void list tag bit comes after the circular list tag so that void lists
 *	can be made circular. Void circular lists thus combine both tag bits.
 *
 * See also:
 *	<WORD_TYPE>, <Word Type Identifiers>
 *---------------------------------------------------------------------------*/

#define WORD_SMALLINT_GET(word)		(((intptr_t)(word))>>1)
#define WORD_SMALLFP_GET(word, c)	((c).w = (word), (c).i &= ~2, (c).f)
#define WORD_CHAR_GET(word)		((Col_Char)(((uintptr_t)(word))>>5))
#define WORD_SMALLSTR_LENGTH(value)	((((uintptr_t)(value))&0xFF)>>5)
#define WORD_SMALLSTR_SET_LENGTH(word, length) (*((uintptr_t *)&(word)) = ((length)<<5)|12)
#ifdef COL_BIGENDIAN
#   define WORD_SMALLSTR_DATA(word)	((Col_Char1  *)&(word))
#else
#   define WORD_SMALLSTR_DATA(word)	(((Col_Char1 *)&(word))+1)
#endif
#define WORD_CIRCLIST_CORE(word)	((Col_Word)(((uintptr_t)(word))&~8))
#define WORD_VOIDLIST_LENGTH(word)	(((size_t)(intptr_t)(word))>>5)

/*---------------------------------------------------------------------------
 * Internal Macros: SMALLFP_TYPE
 *
 *	C Type used by immediate floating point words.
 *
 * See also:
 *	<FloatConvert>, <WORD_SMALLFP_NEW>
 *---------------------------------------------------------------------------*/

#if SIZE_BIT == 32
#   define SMALLFP_TYPE		float
#elif SIZE_BIT == 64
#   define SMALLFP_TYPE		double
#endif

/*---------------------------------------------------------------------------
 * Internal Typedef: FloatConvert
 *
 *	Utility union type for immediate floating point words.
 *
 * See also:
 *	<WORD_TYPE_SMALLFP>, <WORD_SMALLFP_GET>, <WORD_SMALLFP_NEW>
 *---------------------------------------------------------------------------*/

typedef union {
    Col_Word w;
    uintptr_t i;
    SMALLFP_TYPE f;
} FloatConvert;

/*---------------------------------------------------------------------------
 * Internal Constants: Immediate Word Type Limits and Bitmasks
 *
 *  SMALLINT_MAX	- Maximum value of small integer words.
 *  SMALLINT_MIN	- Minimum value of small integer words.
 *  SMALLSTR_MAX_LENGTH	- Maximum length of small string words.
 *  VOIDLIST_MAX_LENGTH	- Maximum length of void list words.
 *---------------------------------------------------------------------------*/

#define SMALLINT_MAX		(INTPTR_MAX>>1)
#define SMALLINT_MIN		(INTPTR_MIN>>1)
#define SMALLSTR_MAX_LENGTH	(sizeof(Col_Word)-1)
#define VOIDLIST_MAX_LENGTH	(SIZE_MAX>>5)

/*---------------------------------------------------------------------------
 * Internal Constants: Word Singletons
 *
 *  WORD_SMALLSTR_EMPTY	- Empty string.
 *  WORD_LIST_EMPTY	- Empty list.
 *---------------------------------------------------------------------------*/

#define WORD_SMALLSTR_EMPTY	((Col_Word) 12)
#define WORD_LIST_EMPTY		WORD_VOIDLIST_NEW(0)

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_SMALLINT_NEW
 *
 *	Small integer word creation.
 *
 * Argument:
 *	value	- Integer value. Must be between <SMALLINT_MIN> and 
 *		  <SMALLINT_MAX>.
 *
 * Result:
 *	The new small integer word.
 *---------------------------------------------------------------------------*/

#define WORD_SMALLINT_NEW(value) \
    ((Col_Word)((((intptr_t)(value))<<1)|1))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_SMALLFP_NEW
 *
 *	Small floating point word creation.
 *
 * Argument:
 *	value	- Floating point value.
 *	c	- <FloatConvert> conversion structure.
 *
 * Result:
 *	The new small floating point word.
 *---------------------------------------------------------------------------*/

#define WORD_SMALLFP_NEW(value, c) \
    ((c).f = (SMALLFP_TYPE)(value), (c).i |= 2, (c).w)

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_CHAR_NEW
 *
 *	Unicode char word creation.
 *
 * Argument:
 *	value	- Unicode codepoint. Must be less than <ROPE_CHAR_MAX>.
 *
 * Result:
 *	The new Unicode char word.
 *---------------------------------------------------------------------------*/

#define WORD_CHAR_NEW(value) \
    ((Col_Word)((((uintptr_t)(value))<<5)|4))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_CIRCLIST_NEW
 *
 *	Circular list word creation.
 *
 * Argument:
 *	core	- Core list. Must be acyclic.
 *
 * Result:
 *	The new circular list word.
 *---------------------------------------------------------------------------*/

#define WORD_CIRCLIST_NEW(core) \
    ((Col_Word)(((uintptr_t)(core))|8))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_VOIDLIST_NEW
 *
 *	Void list word creation.
 *
 * Argument:
 *	length	- Void list length. Must be less than <VOIDLIST_MAX_LENGTH>.
 *
 * Result:
 *	The new void list word.
 *---------------------------------------------------------------------------*/

#define WORD_VOIDLIST_NEW(length) \
    ((Col_Word)(intptr_t)((((size_t)(length))<<5)|20))


/****************************************************************************
 * Internal Group: Regular Words
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macros: Regular Word Fields
 *
 *	Accessors for cell-based word fields.
 *
 * Common Layout:
 *	Regular word store their data in cells whose 1st machine word is used 
 *	for the word type field, which is either a numerical ID for predefined 
 *	types or a pointer to a <Col_CustomWordType> structure for custom types. 
 *	As such structures are always word-aligned, this means that the two 
 *	least significant bits of their pointer value are zero (for 
 *	architectures with at least 32 bit words) and are free for our purpose. 
 * 
 *	On little endian architectures, the LSB of the type pointer is the 
 *	cell's byte 0. On big endian architectures, we rotate the pointer value 
 *	one byte to the right so that the original LSB ends up on byte 0. That 
 *	way the two reserved bits are on byte 0 for both predefined type IDs and
 *	type pointers.
 *
 *	We use bit 0 of the type field as the pinned flag for both predefined 
 *	type IDs and type pointers. Given the above, this bit is always on byte
 *	0. When set, this means that the word isn't movable; its address remains
 *	fixed as long as this flag is set. Words can normally be moved to the 
 *	upper generation pool during the compaction phase of the GC.
 *
 *	On all architectures the single-cell layout is as follows:
 *
 * (start table)
 *      0 1                                                           n
 *     +-+-------------------------------------------------------------+
 *   0 |P|                                                             |
 *     +-+                                                             +
 *     .                                                               .
 *     .                      Type-specific data                       .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_PINNED		- Get pinned flag.
 *	WORD_SET_PINNED		- Set pinned flag.
 *	WORD_CLEAR_PINNED	- Clear pinned flag.
 *
 *
 * Predefined Word Types:
 *	Predefined type IDs for regular word types are chosen so that their bit
 *	0 is cleared and bit 1 is set, to distinguish them with type pointers 
 *	and avoid value clashes.
 *	
 *	On all architectures the single-cell layout is as follows:
 *
 * (start table)
 *      0 1 2    7                                                    n
 *     +-+-+------+----------------------------------------------------+
 *   0 |P|1| Type |                                                    |
 *     +-+-+------+                                                    +
 *     .                                                               .
 *     .                      Type-specific data                       .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_TYPEID		- Get type ID for predefined word type.
 *	WORD_SET_TYPEID		- Set type ID for predefined word type. Also 
 *				  clears the pinned flag.
 *
 *
 * Custom Word Types:
 *	On all architectures the single-cell layout is as follows:
 *
 * (start table)
 *      0 1                                                           n
 *     +-+-+-----------------------------------------------------------+
 *   0 |P|0|                         Type                              |
 *     +-+-+-----------------------------------------------------------+
 *     .                                                               .
 *     .                      Type-specific data                       .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_TYPEINFO		- Get <Col_CustomWordType> custom type 
 *				  descriptor. (Caution: word is evaluated 
 *				  several times during macro expansion)
 *	WORD_SET_TYPEINFO	- Set <Col_CustomWordType> custom type 
 *				  descriptor. Also clears the pinned flag. 
 *				  (Caution: word is evaluated several times 
 *				  during macro expansion)
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_TYPE>, <Col_CustomWordType>, <Word Type Identifiers>
 *---------------------------------------------------------------------------*/

#define WORD_PINNED(word)	(((uint8_t *)(word))[0] & 1)
#define WORD_SET_PINNED(word)	(((uint8_t *)(word))[0] |= 1)
#define WORD_CLEAR_PINNED(word)	(((uint8_t *)(word))[0] &= ~1)

#define WORD_TYPEID(word)	((((uint8_t *)(word))[0])&~1)
#define WORD_SET_TYPEID(word, type) (((uint8_t *)(word))[0] = (type))

#ifdef COL_BIGENDIAN
#   define WORD_TYPEINFO(word) \
	((Col_CustomWordType *)(ROTATE_LEFT(*(uintptr_t *)(word))&~1))
#   define WORD_SET_TYPEINFO(word, addr) \
	(*(uintptr_t *)(word) = ROTATE_RIGHT(((uintptr_t)(addr))))
#else
#   define WORD_TYPEINFO(word) \
	((Col_CustomWordType *)((*(uintptr_t *)(word))&~1))
#   define WORD_SET_TYPEINFO(word, addr) \
	(*(uintptr_t *)(word) = ((uintptr_t)(addr)))
#endif


/****************************************************************************
 * Internal Group: Custom Words
 *
 * Files:
 *	<colWord.c>, <colWord.h>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macros: Custom Word Fields
 *
 *	Accessors for custom word fields.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0                                                             n
 *     +---------------------------------------------------------------+
 *   0 |                           Type info                           |
 *     +---------------------------------------------------------------+
 *   1 |                        Next (optional)                        |
 *     +---------------------------------------------------------------+
 *     .                                                               .
 *     .                      Type-specific data                       .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_CUSTOM_NEXT    - Words with a freeProc are singly-linked together 
 *			      using this field, so that unreachable words get 
 *			      swept properly upon GC.
 *	WORD_CUSTOM_DATA    - Beginning of custom word data.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 *
 * See also:
 *	<WORD_CUSTOM_INIT>, <SweepUnreachableCells>, <Col_CustomWordType>
 *---------------------------------------------------------------------------*/

#define WORD_CUSTOM_NEXT(word)		(((Col_Word *)(word))[1])
#define WORD_CUSTOM_DATA(word, typeInfo) ((void *)(&WORD_CUSTOM_NEXT(word)+((typeInfo)->freeProc?1:0)))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_CUSTOM_INIT
 *
 *	Initializer for custom words.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times
 *			  during macro expansion)
 *	typeInfo	- <WORD_TYPEINFO>
 *
 * See also:
 *	<Col_NewCustomWord>
 *---------------------------------------------------------------------------*/

#define WORD_CUSTOM_INIT(word, typeInfo) \
    WORD_SET_TYPEINFO((word), (typeInfo));  

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_CUSTOM_SIZE
 *
 *	Get number of cells taken by the custom word.
 *
 * Arguments:
 *	size		- Byte size of custom word data.
 *	typeInfo	- <WORD_TYPEINFO>
 *
 * Result:
 *	Number of cells taken by word.
 *---------------------------------------------------------------------------*/

#define WORD_CUSTOM_SIZE(size, typeInfo) \
    (NB_CELLS((size)+sizeof(Col_CustomWordType *)+((typeInfo)->freeProc?sizeof(Cell *):0)))


/****************************************************************************
 * Internal Group: Predefined Words
 *
 * Files:
 *	<colWord.c>, <colWord.h>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macros: Wrap Word Fields
 *
 *	Accessors for wrap word fields.
 *
 *	Words may have synonyms that can take any accepted word value: immediate 
 *	values (inc. nil), or cell-based words. Words can thus be part of chains
 *	of synonyms having different types, but with semantically identical 
 *	values. Such chains form a circular linked list using this field. The 
 *	order of words in a synonym chain has no importance. Some word types 
 *	have no synonym field, in this case they must be wrapped into structures
 *	that have one when they are added to a chain of synonyms. For 
 *	performance reasons, all word types with a synonym field use the same 
 *	location for this field.
 *
 *	This type is a generic wrapper for words needing a synonym field.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                        Unused                         |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Synonym                            |
 *     +---------------------------------------------------------------+
 *   2 |                            Source                             |
 *     +---------------------------------------------------------------+
 *   3 |                            Unused                             |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_SYNONYM		- Synonym for the word. This accessor is generic
 *				  for all word types having a synonym field.
 *	WORD_WRAP_SOURCE	- Word whose synonym is given.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_WRAP_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_SYNONYM(word)	(((Col_Word *)(word))[1])
#define WORD_WRAP_SOURCE(word)	(((Col_Word *)(word))[2])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_WRAP_INIT
 *
 *	Initializer for wrap words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	source	- <WORD_WRAP_SOURCE>
 *
 * See also:
 *	<WORD_TYPE_WRAP>, <AddSynonymField>
 *---------------------------------------------------------------------------*/

#define WORD_WRAP_INIT(word, source) \
    WORD_SET_TYPEID((word), WORD_TYPE_WRAP); \
    WORD_WRAP_SOURCE(word) = (source);

/*---------------------------------------------------------------------------
 * Internal Macros: Integer Word Fields
 *
 *	Accessors for integer word fields.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                        Unused                         |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Synonym                            |
 *     +---------------------------------------------------------------+
 *   2 |                             Value                             |
 *     +---------------------------------------------------------------+
 *   3 |                            Unused                             |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_INT_VALUE	- Integer value.
 *
 * See also:
 *	<WORD_INT_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_INT_VALUE(word)	(*(((intptr_t *)(word))+1))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_INT_INIT
 *
 *	Initializer for integer words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	value	- <WORD_INT_VALUE>
 *
 * See also:
 *	<WORD_TYPE_INT>, <Col_NewIntWord>
 *---------------------------------------------------------------------------*/

#define WORD_INT_INIT(word, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_INT); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_INT_VALUE(word) = (value);

/*---------------------------------------------------------------------------
 * Internal Macros: Floating Point Word Fields
 *
 *	Accessors for floating point word fields.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                        Unused                         |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Synonym                            |
 *     +---------------------------------------------------------------+
 *   2 |                             Value                             |
 *     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 *   3 |                       (Unused on 64 bit)                      |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_FP_VALUE	- Floating point value.
 *
 * See also:
 *	<WORD_FP_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_FP_VALUE(word)	(*(((double *)(word))+1))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_FP_INIT
 *
 *	Initializer for floating point words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	value	- <WORD_FP_VALUE>
 *
 * See also:
 *	<WORD_TYPE_FP>, <Col_NewFloatWord>
 *---------------------------------------------------------------------------*/

#define WORD_FP_INIT(word, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_FP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_FP_VALUE(word) = (value);

/*---------------------------------------------------------------------------
 * Internal Macros: Redirect Word Fields
 *
 *	Accessors for redirect word fields.
 *
 *	Redirects replace existing words during compacting promotion. They only 
 *	exist during the GC.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                        Unused                         |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Source                             |
 *     +---------------------------------------------------------------+
 *   2 |                                                               |
 *     +                            Unused                             +
 *   3 |                                                               |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_REDIRECT_SOURCE	- New location of the word.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *---------------------------------------------------------------------------*/

#ifdef PROMOTE_COMPACT
#   define WORD_REDIRECT_SOURCE(word)	(((Col_Word *)(word))[1])
#endif /* PROMOTE_COMPACT*/


/****************************************************************************
 * Internal Group: Ropes
 *
 * See also:
 *	<colRope.c>, <colRope.h>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macros: Fixed-Width UCS String Fields
 *
 *	Accessors for fixed-width UCS string fields.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0             7 8            15 16                           31
 *     +---------------+---------------+-------------------------------+
 *     |      Type     |    Format     |            Length             |
 *     +---------------+---------------+-------------------------------+
 *     .                                                               .
 *     .                         Character data                        .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_UCSSTR_FORMAT	- Character format. Numeric value matches the 
 *				  character width.
 *	WORD_UCSSTR_LENGTH	- Character length. The actual byte length is 
 *				  this value multiplied by the character width.
 *	WORD_UCSSTR_DATA	- String data.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_UCSSTR_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_UCSSTR_FORMAT(word)	(((uint8_t *)(word))[1])
#define WORD_UCSSTR_LENGTH(word)	(((uint16_t *)(word))[1])
#define WORD_UCSSTR_DATA(word)		((const char *)(word)+WORD_UCSSTR_HEADER_SIZE)

/*---------------------------------------------------------------------------
 * Internal Macro: UCSSTR_SIZE
 *
 *	Get number of cells taken by the UCS string.
 *
 * Argument:
 *	byteLength	- Byte size of string.
 *
 * Result:
 *	Number of cells taken by word.
 *---------------------------------------------------------------------------*/

#define UCSSTR_SIZE(byteLength) \
    (NB_CELLS(WORD_UCSSTR_HEADER_SIZE+(byteLength)))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_UCSSTR_INIT
 *
 *	Initializer for UCS string words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	format	- <WORD_UCSSTR_FORMAT>
 *	length	- <WORD_UCSSTR_LENGTH>
 *
 * See also:
 *	<WORD_TYPE_UCSSTR>, <Col_NewRope>
 *---------------------------------------------------------------------------*/

#define WORD_UCSSTR_INIT(word, format, length) \
    WORD_SET_TYPEID((word), WORD_TYPE_UCSSTR); \
    WORD_UCSSTR_FORMAT(word) = (uint8_t) (format); \
    WORD_UCSSTR_LENGTH(word) = (uint16_t) (length);

/*---------------------------------------------------------------------------
 * Internal Macros: Variable-Width UTF-8 String Fields
 *
 *	Accessors for variable-width UTF-8 string fields.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0             7 8            15 16                           31
 *     +---------------+---------------+-------------------------------+
 *     |      Type     |    Unused     |            Length             |
 *     +---------------+---------------+-------------------------------+
 *     |          Byte length          |                               |
 *     +-------------------------------+                               +
 *     .                                                               .
 *     .                         Character data                        .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_UTF8STR_LENGTH	- Character length.
 *	WORD_UTF8STR_BYTELENGTH	- Byte length.
 *	WORD_UTF8STR_DATA	- String data.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_UTF8STR_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_UTF8STR_LENGTH(word)	(((uint16_t *)(word))[1])
#define WORD_UTF8STR_BYTELENGTH(word)	(((uint16_t *)(word))[2])
#define WORD_UTF8STR_DATA(word)		((const char *)(word)+WORD_UTF8STR_HEADER_SIZE)

/*---------------------------------------------------------------------------
 * Internal Macro: UTF8STR_SIZE
 *
 *	Get number of cells taken by the UTF-8 string.
 *
 * Argument:
 *	byteLength	- Byte size of string.
 *
 * Result:
 *	Number of cells taken by word.
 *---------------------------------------------------------------------------*/

#define UTF8STR_SIZE(byteLength) \
    (NB_CELLS(WORD_UTF8STR_HEADER_SIZE+(byteLength)))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_UTF8STR_INIT
 *
 *	Initializer for UTF-8 string words.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times 
 *			  during macro expansion)
 *	length		- <WORD_UTF8STR_LENGTH>
 *	byteLength	- <WORD_UTF8STR_BYTELENGTH>
 *
 * See also:
 *	<WORD_TYPE_UTF8STR>, <Col_NewRope>
 *---------------------------------------------------------------------------*/

#define WORD_UTF8STR_INIT(word, length, byteLength) \
    WORD_SET_TYPEID((word), WORD_TYPE_UTF8STR); \
    WORD_UTF8STR_LENGTH(word) = (uint16_t) (length); \
    WORD_UTF8STR_BYTELENGTH(word) = (uint16_t) (byteLength); \

/*---------------------------------------------------------------------------
 * Internal Constants: String Sizes and Limits
 *
 *  WORD_UCSSTR_HEADER_SIZE	- Byte size of UCS string header.
 *  UCSSTR_MAX_LENGTH		- Maximum char length of UCS strings.
 *  WORD_UTF8STR_HEADER_SIZE	- Byte size of UTF-8 string header.
 *  UTF8STR_MAX_BYTELENGTH	- Maximum byte length of UTF-8 strings. Contrary
 *				  to fixed-width versions, UTF-8 ropes are 
 *				  limited in size to one page, so that access 
 *				  performances are better.
 *---------------------------------------------------------------------------*/

#define WORD_UCSSTR_HEADER_SIZE		(sizeof(uint16_t)*2)
#define UCSSTR_MAX_LENGTH		UINT16_MAX
#define WORD_UTF8STR_HEADER_SIZE	(sizeof(uint16_t)*3)
#define UTF8STR_MAX_BYTELENGTH		(AVAILABLE_CELLS*CELL_SIZE-WORD_UTF8STR_HEADER_SIZE)

/*---------------------------------------------------------------------------
 * Internal Macros: Subrope Fields
 *
 *	Accessors for subrope fields.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8    15                                               n
 *     +-------+-------+-----------------------------------------------+
 *   0 | Type  | Depth |                    Unused                     |
 *     +-------+-------+-----------------------------------------------+
 *   1 |                            Source                             |
 *     +---------------------------------------------------------------+
 *   2 |                             First                             |
 *     +---------------------------------------------------------------+
 *   3 |                             Last                              |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_SUBROPE_DEPTH	- Depth of rope. 8 bits will code up to 255 
 *				  depth levels, which is more than sufficient 
 *				  for balanced binary trees. 
 *	WORD_SUBROPE_SOURCE	- Rope of which this one is a subrope.
 *	WORD_SUBROPE_FIRST	- First character in source.
 *	WORD_SUBROPE_LAST	- Last character in source.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_SUBROPE_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_SUBROPE_DEPTH(word)	(((uint8_t *)(word))[1])
#define WORD_SUBROPE_SOURCE(word)	(((Col_Word *)(word))[1])
#define WORD_SUBROPE_FIRST(word)	(((size_t *)(word))[2])
#define WORD_SUBROPE_LAST(word)		(((size_t *)(word))[3])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_SUBROPE_INIT
 *
 *	Initializer for subrope words.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times 
 *			  during macro expansion)
 *	depth		- <WORD_SUBROPE_DEPTH>
 *	source		- <WORD_SUBROPE_SOURCE>
 *	first		- <WORD_SUBROPE_FIRST>
 *	last		- <WORD_SUBROPE_LAST>
 *
 * See also:
 *	<WORD_TYPE_SUBROPE>, <Col_Subrope>
 *---------------------------------------------------------------------------*/

#define WORD_SUBROPE_INIT(word, depth, source, first, last) \
    WORD_SET_TYPEID((word), WORD_TYPE_SUBROPE); \
    WORD_SUBROPE_DEPTH(word) = (uint8_t) (depth); \
    WORD_SUBROPE_SOURCE(word) = (source); \
    WORD_SUBROPE_FIRST(word) = (first); \
    WORD_SUBROPE_LAST(word) = (last);

/*---------------------------------------------------------------------------
 * Internal Macros: Concat Rope Fields
 *
 *	Accessors for concat rope fields.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8    15 16           31                               n
 *     +-------+-------+---------------+-------------------------------+
 *   0 | Type  | Depth |  Left length  |        Unused (n > 32)        |
 *     +-------+-------+---------------+-------------------------------+
 *   1 |                            Length                             |
 *     +---------------------------------------------------------------+
 *   2 |                             Left                              |
 *     +---------------------------------------------------------------+
 *   3 |                             Right                             |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_CONCATROPE_DEPTH		- Depth of rope. 8 bits will code up to 
 *					  255 depth levels, which is more than 
 *					  sufficient for balanced binary trees. 
 *	WORD_CONCATROPE_LEFT_LENGTH	- Used as shortcut to avoid 
 *					  dereferencing the left arm. Zero if 
 *					  actual length is too large to fit.
 *	WORD_CONCATROPE_LENGTH		- Rope length, which is the sum of both
 *					  arms.
 *	WORD_CONCATROPE_LEFT		- Left concatenated rope.
 *	WORD_CONCATROPE_RIGHT		- Right concatenated rope.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_CONCATROPE_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_CONCATROPE_DEPTH(word)	(((uint8_t *)(word))[1])
#define WORD_CONCATROPE_LEFT_LENGTH(word) (((uint16_t *)(word))[1])
#define WORD_CONCATROPE_LENGTH(word)	(((size_t *)(word))[1])
#define WORD_CONCATROPE_LEFT(word)	(((Col_Word *)(word))[2])
#define WORD_CONCATROPE_RIGHT(word)	(((Col_Word *)(word))[3])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_CONCATROPE_INIT
 *
 *	Initializer for concat rope words.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times 
 *			  during macro expansion)
 *	depth		- <WORD_CONCATROPE_DEPTH>
 *	length		- <WORD_CONCATROPE_LENGTH>
 *	leftLength	- <WORD_CONCATROPE_LEFT_LENGTH> (Caution: evaluated 
 *			  several times during macro expansion)
 *	left		- <WORD_CONCATROPE_LEFT>
 *	right		- <WORD_CONCATROPE_RIGHT>
 *
 * See also:
 *	<WORD_TYPE_CONCATROPE>, <Col_ConcatRopes>
 *---------------------------------------------------------------------------*/

#define WORD_CONCATROPE_INIT(word, depth, length, leftLength, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_CONCATROPE); \
    WORD_CONCATROPE_DEPTH(word) = (uint8_t) (depth); \
    WORD_CONCATROPE_LENGTH(word) = (length); \
    WORD_CONCATROPE_LEFT_LENGTH(word) = (uint16_t) ((leftLength)>UINT16_MAX?0:(leftLength)); \
    WORD_CONCATROPE_LEFT(word) = (left); \
    WORD_CONCATROPE_RIGHT(word) = (right);


/****************************************************************************
 * Internal Group: Vectors
 *
 * Files:
 *	<colVector.c>, <colVector.h>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macros: Vector Word Fields
 *
 *	Accessors for vector word fields. Both immutable and mutable versions 
 *	use these fields.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                        Unused                         |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Length                             |
 *     +---------------------------------------------------------------+
 *     .                                                               .
 *     .                           Elements                            .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_VECTOR_LENGTH	- Size of element array.
 *	WORD_VECTOR_ELEMENTS	- Array of words.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_VECTOR_INIT>, <WORD_MVECTOR_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_VECTOR_LENGTH(word)	(((size_t *)(word))[1])
#define WORD_VECTOR_ELEMENTS(word)	((Col_Word *)(word)+2)

/*---------------------------------------------------------------------------
 * Internal Macros: Mutable Vector Word Fields
 *
 *	Accessors for Mutable vector word fields.
 *
 *	Mutable vectors are like immutable ones except that their content and
 *	length may vary. To this purpose they use extra fields with respect to
 *	immutable vectors.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                         Size                          |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Length                             |
 *     +---------------------------------------------------------------+
 *     .                                                               .
 *     .                           Elements                            .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_MVECTOR_SIZE	- Get the number of allocated cells.
 *	WORD_MVECTOR_SET_SIZE	- Set the number of allocated cells. (Caution: 
 *				  word is evaluated several times during macro 
 *				  expansion)
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_MVECTOR_INIT>
 *---------------------------------------------------------------------------*/

#define MVECTOR_SIZE_MASK		MVECTOR_MAX_SIZE
#ifdef COL_BIGENDIAN
#   define WORD_MVECTOR_SIZE(word)	((((size_t *)(word))[0])&MVECTOR_SIZE_MASK)
#   define WORD_MVECTOR_SET_SIZE(word, size) ((((size_t *)(word))[0])&=~MVECTOR_SIZE_MASK,(((size_t *)(word))[0])|=((size)&MVECTOR_SIZE_MASK))
#else
#   define WORD_MVECTOR_SIZE(word)	((((size_t *)(word))[0]>>CHAR_BIT)&MVECTOR_SIZE_MASK)
#   define WORD_MVECTOR_SET_SIZE(word, size) ((((size_t *)(word))[0])&=~(MVECTOR_SIZE_MASK<<CHAR_BIT),(((size_t *)(word))[0])|=(((size)&MVECTOR_SIZE_MASK)<<CHAR_BIT))
#endif

/*---------------------------------------------------------------------------
 * Internal Constants: Vector Sizes and Limits
 *
 *  WORD_VECTOR_HEADER_SIZE	- Byte size of vector header.
 *  MVECTOR_MAX_SIZE		- Maximum cell size taken by mutable vectors.
 *---------------------------------------------------------------------------*/

#define WORD_VECTOR_HEADER_SIZE	(sizeof(size_t)*2)
#define MVECTOR_MAX_SIZE	(SIZE_MAX>>CHAR_BIT)

/*---------------------------------------------------------------------------
 * Internal Macro: VECTOR_SIZE
 *
 *	Get number of cells for a vector of a given length.
 *
 * Arguments:
 *	length	- Vector length.
 *
 * Result:
 *	Number of cells taken by word.
 *---------------------------------------------------------------------------*/

#define VECTOR_SIZE(length) \
    (NB_CELLS(WORD_VECTOR_HEADER_SIZE+(length)*sizeof(Col_Word)))

/*---------------------------------------------------------------------------
 * Internal Macro: VECTOR_MAX_LENGTH
 *
 *	Get maximum vector length for a given byte size.
 *
 * Arguments:
 *	byteSize	- Available size.
 *
 * Result:
 *	Vector length fitting the given size.
 *---------------------------------------------------------------------------*/

#define VECTOR_MAX_LENGTH(byteSize) \
    (((byteSize)-WORD_VECTOR_HEADER_SIZE)/sizeof(Col_Word))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_VECTOR_INIT
 *
 *	Initializer for immutable vector words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	length	- <WORD_VECTOR_LENGTH>
 *
 * See also:
 *	<WORD_TYPE_VECTOR>, <Col_NewVector>, <Col_NewVectorNV>
 *---------------------------------------------------------------------------*/

#define WORD_VECTOR_INIT(word, length) \
    WORD_SET_TYPEID((word), WORD_TYPE_VECTOR); \
    WORD_VECTOR_LENGTH(word) = (length);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MVECTOR_INIT
 *
 *	Initializer for mutable vector words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	size	- <WORD_MVECTOR_SET_SIZE>
 *	length	- <WORD_VECTOR_LENGTH>
 *
 * See also:
 *	<WORD_TYPE_MVECTOR>, <Col_NewVector>, <Col_NewVectorNV>
 *---------------------------------------------------------------------------*/

#define WORD_MVECTOR_INIT(word, size, length) \
    WORD_SET_TYPEID((word), WORD_TYPE_MVECTOR); \
    WORD_MVECTOR_SET_SIZE((word), (size)); \
    WORD_VECTOR_LENGTH(word) = (length);


/****************************************************************************
 * Internal Group: Lists
 *
 * Files:
 *	<colList.c>, <colList.h>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macros: Sublist Word Fields
 *
 *	Accessors for sublist word fields.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8    15                                               n
 *     +-------+-------+-----------------------------------------------+
 *   0 | Type  | Depth |                    Unused                     |
 *     +-------+-------+-----------------------------------------------+
 *   1 |                            Source                             |
 *     +---------------------------------------------------------------+
 *   2 |                             First                             |
 *     +---------------------------------------------------------------+
 *   3 |                             Last                              |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_SUBLIST_DEPTH	- Depth of list. 8 bits will code up to 255 
 *				  depth levels, which is more than sufficient 
 *				  for balanced binary trees. 
 *	WORD_SUBLIST_SOURCE	- List of which this one is a sublist.
 *	WORD_SUBLIST_FIRST	- First element in source.
 *	WORD_SUBLIST_LAST	- Last element in source.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_SUBLIST_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_SUBLIST_DEPTH(word)	(((uint8_t *)(word))[1])
#define WORD_SUBLIST_SOURCE(word)	(((Col_Word *)(word))[1])
#define WORD_SUBLIST_FIRST(word)	(((size_t *)(word))[2])
#define WORD_SUBLIST_LAST(word)		(((size_t *)(word))[3])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_SUBLIST_INIT
 *
 *	Initializer for sublist words.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times 
 *			  during macro expansion)
 *	depth		- <WORD_SUBLIST_DEPTH>
 *	source		- <WORD_SUBLIST_SOURCE>
 *	first		- <WORD_SUBLIST_FIRST>
 *	last		- <WORD_SUBLIST_LAST>
 *
 * See also:
 *	<WORD_TYPE_SUBLIST>, <Col_Sublist>
 *---------------------------------------------------------------------------*/

#define WORD_SUBLIST_INIT(word, depth, source, first, last) \
    WORD_SET_TYPEID((word), WORD_TYPE_SUBLIST); \
    WORD_SUBLIST_DEPTH(word) = (uint8_t) (depth); \
    WORD_SUBLIST_SOURCE(word) = (source); \
    WORD_SUBLIST_FIRST(word) = (first); \
    WORD_SUBLIST_LAST(word) = (last);

/*---------------------------------------------------------------------------
 * Internal Macros: Concat List Word Fields
 *
 *	Accessors for concat list word fields. Both immutable and mutable
 *	versions use these fields.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8    15 16           31                               n
 *     +-------+-------+---------------+-------------------------------+
 *   0 | Type  | Depth |  Left length  |        Unused (n > 32)        |
 *     +-------+-------+---------------+-------------------------------+
 *   1 |                            Length                             |
 *     +---------------------------------------------------------------+
 *   2 |                             Left                              |
 *     +---------------------------------------------------------------+
 *   3 |                             Right                             |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_CONCATLIST_DEPTH		- Depth of list. 8 bits will code up to 
 *					  255 depth levels, which is more than 
 *					  sufficient for balanced binary trees. 
 *	WORD_CONCATLIST_LEFT_LENGTH	- Used as shortcut to avoid 
 *					  dereferencing the left arm. Zero if 
 *					  actual length is too large to fit.
 *	WORD_CONCATLIST_LENGTH		- List length, which is the sum of both
 *					  arms.
 *	WORD_CONCATLIST_LEFT		- Left concatenated list.
 *	WORD_CONCATLIST_RIGHT		- Right concatenated list.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_CONCATLIST_INIT>, <WORD_MCONCATLIST_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_CONCATLIST_DEPTH(word)	(((uint8_t *)(word))[1])
#define WORD_CONCATLIST_LEFT_LENGTH(word) (((uint16_t *)(word))[1])
#define WORD_CONCATLIST_LENGTH(word)	(((size_t *)(word))[1])
#define WORD_CONCATLIST_LEFT(word)	(((Col_Word *)(word))[2])
#define WORD_CONCATLIST_RIGHT(word)	(((Col_Word *)(word))[3])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_CONCATLIST_INIT
 *
 *	Initializer for immutable concat list words.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times 
 *			  during macro expansion)
 *	depth		- <WORD_CONCATLIST_DEPTH>
 *	length		- <WORD_CONCATLIST_LENGTH>
 *	leftLength	- <WORD_CONCATLIST_LEFT_LENGTH> (Caution: evaluated 
 *			  several times during macro expansion)
 *	left		- <WORD_CONCATLIST_LEFT>
 *	right		- <WORD_CONCATLIST_RIGHT>
 *
 * See also:
 *	<WORD_TYPE_CONCATLIST>, <Col_ConcatLists>
 *---------------------------------------------------------------------------*/

#define WORD_CONCATLIST_INIT(word, depth, length, leftLength, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_CONCATLIST); \
    WORD_CONCATLIST_DEPTH(word) = (uint8_t) (depth); \
    WORD_CONCATLIST_LENGTH(word) = (length); \
    WORD_CONCATLIST_LEFT_LENGTH(word) = (uint16_t) ((leftLength)>UINT16_MAX?0:(leftLength)); \
    WORD_CONCATLIST_LEFT(word) = (left); \
    WORD_CONCATLIST_RIGHT(word) = (right);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MCONCATLIST_INIT
 *
 *	Initializer for mutable concat list words.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times 
 *			  during macro expansion)
 *	depth		- <WORD_CONCATLIST_DEPTH>
 *	length		- <WORD_CONCATLIST_LENGTH>
 *	leftLength	- <WORD_CONCATLIST_LEFT_LENGTH> (Caution: evaluated 
 *			  several times during macro expansion)
 *	left		- <WORD_CONCATLIST_LEFT>
 *	right		- <WORD_CONCATLIST_RIGHT>
 *
 * See also:
 *	<WORD_TYPE_MCONCATLIST>, <NewMConcatList>, <UpdateMConcatNode>
 *---------------------------------------------------------------------------*/

#define WORD_MCONCATLIST_INIT(word, depth, length, leftLength, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_MCONCATLIST); \
    WORD_CONCATLIST_DEPTH(word) = (uint8_t) (depth); \
    WORD_CONCATLIST_LENGTH(word) = (length); \
    WORD_CONCATLIST_LEFT_LENGTH(word) = (uint16_t) ((leftLength)>UINT16_MAX?0:(leftLength)); \
    WORD_CONCATLIST_LEFT(word) = (left); \
    WORD_CONCATLIST_RIGHT(word) = (right);

//FIXME: remove mutable lists and replace by generic mutable wrap.

/*---------------------------------------------------------------------------
 * Internal Macros: Mutable List Word Fields
 *
 *	Accessors for mutable list word fields.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                        Unused                         |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Synonym                            |
 *     +---------------------------------------------------------------+
 *   2 |                             Root                              |
 *     +---------------------------------------------------------------+
 *   3 |                            Unused                             |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_MLIST_ROOT	- The root node, may be a vector, sublist or concat 
 *			  list. The list length is given by its root node.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_MLIST_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_MLIST_ROOT(word)		(((Col_Word *)(word))[2])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MLIST_INIT
 *
 *	Initializer for mutable list words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	root	- <WORD_MLIST_ROOT>
 *
 * See also:
 *	<WORD_TYPE_MLIST>, <Col_NewMList>
 *---------------------------------------------------------------------------*/

#define WORD_MLIST_INIT(word, root) \
    WORD_SET_TYPEID((word), WORD_TYPE_MLIST); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_MLIST_ROOT(word) = (root); \


/****************************************************************************
 * Internal Group: Maps
 *
 * Files:
 *	<colMap.c>, <colMap.h>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macros: Map Entry Fields
 *
 *	Accessors for generic map entry fields.
 *
 *	Maps associate a key to a word value. The key is usually a word 
 *	(including ropes) but specialized subtypes use integer values.
 * 
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8                                                     n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                                                       |
 *     +-------+                   Type-specific data                  +
 *   1 |                                                               |
 *     +---------------------------------------------------------------+
 *   2 |                              Key                              |
 *     +---------------------------------------------------------------+
 *   3 |                             Value                             |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_MAPENTRY_KEY	- Entry key word.
 *	WORD_INTMAPENTRY_KEY	- Integer entry key.
 *	WORD_MAPENTRY_VALUE	- Entry value word.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_HASHENTRY_INIT>, <WORD_MHASHENTRY_INIT>, <WORD_INTHASHENTRY_INIT>,
 *	<WORD_MINTHASHENTRY_INIT>, <WORD_TRIELEAF_INIT>, <WORD_MTRIELEAF_INIT>, 
 *	<WORD_INTTRIELEAF_INIT>, <WORD_MINTTRIELEAF_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_MAPENTRY_KEY(word)		(((Col_Word *)(word))[2])
#define WORD_INTMAPENTRY_KEY(word)	(((intptr_t *)(word))[2])
#define WORD_MAPENTRY_VALUE(word)	(((Col_Word *)(word))[3])


/****************************************************************************
 * Internal Group: Hash Maps
 *
 * Files:
 *	<colHash.c>, <colHash.h>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macros: Hash Map Fields
 *
 *	Accessors for hash map fields.
 *
 *	For each map entry, an integer hash value is computed from the key, and 
 *	this hash value is used to select a "bucket", i.e. an insertion point in
 *	a dynamic array. When two entries end up in the same bucket, there is a 
 *	collision. When the map exceeds a certain size or the number of 
 *	collisions exceed a certain threshold, the table is resized and entries 
 *	are rehashed.
 *
 *	For each bucket, hash entries are stored as linked lists. Each entry 
 *	stores its hash, key and value, as well as a pointer to the next entry.
 *
 *	Generic hash maps are custom word types of type <COL_HASHMAP>.
 *
 *	String hash maps are specialized hash maps using built-in compare and 
 *	hash procs on string keys.
 *
 *	Integer hash maps are specialized hash maps where the hash value is the 
 *	randomized integer key.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0                                                             n
 *     +---------------------------------------------------------------+
 *   0 |                           Type info                           |
 *     +---------------------------------------------------------------+
 *   1 |                            Synonym                            |
 *     +---------------------------------------------------------------+
 *   2 |                             Size                              |
 *     +---------------------------------------------------------------+
 *   3 |                            Buckets                            |
 *     +---------------------------------------------------------------+
 *     .                                                               .
 *     .                        Static buckets                         .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_HASHMAP_SIZE		- Number of elements in map.
 *	WORD_HASHMAP_BUCKETS		- Bucket container. If nil, buckets are 
 *					  stored in the inline static bucket 
 *					  array. Else they use a mutable vector.
 *	WORD_HASHMAP_STATICBUCKETS	- Inline bucket array for small sized maps.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_HASHMAP_INIT>, <WORD_INTHASHMAP_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_HASHMAP_SIZE(word)		(((size_t *)(word))[2])
#define WORD_HASHMAP_BUCKETS(word)	(((Col_Word *)(word))[3])
#define WORD_HASHMAP_STATICBUCKETS(word) ((Col_Word *)((char *)(word)+WORD_HASHMAP_HEADER_SIZE))

/*---------------------------------------------------------------------------
 * Internal Constants: Hash Map Sizes
 *
 *  WORD_HASHMAP_HEADER_SIZE		- Byte size of hash map header.
 *  HASHMAP_STATICBUCKETS_NBCELLS	- Number of cells allocated for static 
 *					  bucket array.
 *  HASHMAP_STATICBUCKETS_SIZE		- Number of elements in static bucket
 *					  array.
 *  HASHMAP_NBCELLS			- Number of cells taken by hash maps.
 *---------------------------------------------------------------------------*/

#define WORD_HASHMAP_HEADER_SIZE	(sizeof(Col_CustomWordType*)+sizeof(Col_Word)*2+sizeof(size_t))
#define HASHMAP_STATICBUCKETS_NBCELLS	2
#define HASHMAP_STATICBUCKETS_SIZE	(HASHMAP_STATICBUCKETS_NBCELLS*CELL_SIZE/sizeof(Col_Word))
#define HASHMAP_NBCELLS			(HASHMAP_STATICBUCKETS_NBCELLS+1)

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_HASHMAP_INIT
 *
 *	Initializer for hash map words.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times 
 *			  during macro expansion)
 *	typeInfo	- <WORD_SET_TYPEINFO>
 *---------------------------------------------------------------------------*/

#define WORD_HASHMAP_INIT(word, typeInfo) \
    WORD_SET_TYPEINFO((word), (typeInfo)); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_HASHMAP_SIZE(word) = 0; \
    WORD_HASHMAP_BUCKETS(word) = WORD_NIL;

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_STRHASHMAP_INIT
 *
 *	Initializer for string hash map words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *
 * See also:
 *	<WORD_TYPE_STRHASHMAP>, <Col_NewStringHashMap>
 *---------------------------------------------------------------------------*/

#define WORD_STRHASHMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_STRHASHMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_HASHMAP_SIZE(word) = 0; \
    WORD_HASHMAP_BUCKETS(word) = WORD_NIL;

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_INTHASHMAP_INIT
 *
 *	Initializer for integer hash map words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *
 * See also:
 *	<WORD_TYPE_INTHASHMAP>, <Col_NewIntHashMap>
 *---------------------------------------------------------------------------*/

#define WORD_INTHASHMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTHASHMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_HASHMAP_SIZE(word) = 0; \
    WORD_HASHMAP_BUCKETS(word) = WORD_NIL;

/*---------------------------------------------------------------------------
 * Internal Macros: Hash Entry Fields
 *
 *	Accessors for hash entry fields. Both immutable and mutable versions
 *	use these fields.
 *
 *	Uses generic map entry fields.
 * 
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8                                                     n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                         Hash                          |
 *     +-------+-------------------------------------------------------+
 *   1 |                             Next                              |
 *     +---------------------------------------------------------------+
 *   2 |                          Key (Generic)                        |
 *     +---------------------------------------------------------------+
 *   3 |                         Value (Generic)                       |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_HASHENTRY_HASH	- Higher order bits of the hash value for fast 
 *				  negative test and rehashing. The full value is
 *				  computed by combining these high order bits 
 *				  and the bucket index whenever possible, else 
 *				  the value is recomputed from the key.
 *	WORD_HASHENTRY_SET_HASH	- Set hash value. Only retain the high order 
 *				  bits.
 *	WORD_HASHENTRY_NEXT	- Pointer to next entry in bucket. Also used 
 *				  during iteration.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Map Entry Fields>, <WORD_HASHENTRY_INIT>, <WORD_MHASHENTRY_INIT>, 
 *	<WORD_INTHASHENTRY_INIT>, <WORD_MINTHASHENTRY_INIT>
 *---------------------------------------------------------------------------*/

#define HASHENTRY_HASH_MASK		(UINTPTR_MAX^UCHAR_MAX)
#ifdef COL_BIGENDIAN
#   define WORD_HASHENTRY_HASH(word)	((((uintptr_t *)(word))[0]<<CHAR_BIT)&HASHENTRY_HASH_MASK)
#   define WORD_HASHENTRY_SET_HASH(word, hash) ((((uintptr_t *)(word))[0])&=~(HASHENTRY_HASH_MASK>>CHAR_BIT),(((uintptr_t *)(word))[0])|=(((hash)&HASHENTRY_HASH_MASK)>>CHAR_BIT))
#else
#   define WORD_HASHENTRY_HASH(word)	((((uintptr_t *)(word))[0])&HASHENTRY_HASH_MASK)
#   define WORD_HASHENTRY_SET_HASH(word, hash) ((((uintptr_t *)(word))[0])&=~HASHENTRY_HASH_MASK,(((uintptr_t *)(word))[0])|=((hash)&HASHENTRY_HASH_MASK))
#endif
#define WORD_HASHENTRY_NEXT(word)	(((Col_Word *)(word))[1])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_HASHENTRY_INIT
 *
 *	Initializer for immutable hash entry words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_MAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *	next	- <WORD_HASHENTRY_NEXT>
 *	hash	- <WORD_HASHENTRY_SET_HASH> (Caution: evaluated several times
 *		  during macro expansion)
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_TYPE_HASHENTRY>
 *---------------------------------------------------------------------------*/

#define WORD_HASHENTRY_INIT(word, key, value, next, hash) \
    WORD_SET_TYPEID((word), WORD_TYPE_HASHENTRY); \
    WORD_MAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value); \
    WORD_HASHENTRY_NEXT(word) = (next); \
    WORD_HASHENTRY_SET_HASH(word, hash);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MHASHENTRY_INIT
 *
 *	Initializer for mutable hash entry words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_MAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *	next	- <WORD_HASHENTRY_NEXT>
 *	hash	- <WORD_HASHENTRY_SET_HASH> (Caution: evaluated several times
 *		  during macro expansion)
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_TYPE_MHASHENTRY>
 *---------------------------------------------------------------------------*/

#define WORD_MHASHENTRY_INIT(word, key, value, next, hash) \
    WORD_SET_TYPEID((word), WORD_TYPE_MHASHENTRY); \
    WORD_MAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value); \
    WORD_HASHENTRY_NEXT(word) = (next); \
    WORD_HASHENTRY_SET_HASH(word, hash);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_INTHASHENTRY_INIT
 *
 *	Initializer for immutable integer hash entry words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_INTMAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *	next	- <WORD_HASHENTRY_NEXT>
 *
 * See also:
 *	<WORD_TYPE_INTHASHENTRY>
 *---------------------------------------------------------------------------*/

#define WORD_INTHASHENTRY_INIT(word, key, value, next) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTHASHENTRY); \
    WORD_INTMAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value); \
    WORD_HASHENTRY_NEXT(word) = (next);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MINTHASHENTRY_INIT
 *
 *	Initializer for mutable integer hash entry words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_INTMAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *	next	- <WORD_HASHENTRY_NEXT>
 *
 * See also:
 *	<WORD_TYPE_MINTHASHENTRY>
 *---------------------------------------------------------------------------*/

#define WORD_MINTHASHENTRY_INIT(word, key, value, next) \
    WORD_SET_TYPEID((word), WORD_TYPE_MINTHASHENTRY); \
    WORD_INTMAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value); \
    WORD_HASHENTRY_NEXT(word) = (next);


/****************************************************************************
 * Internal Group: Trie Maps
 *
 * Files:
 *	<colTrie.c>, <colTrie.h>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macros: Trie Map Fields
 *
 *	Accessors for trie map fields.
 *
 *	Trie maps are trees that store map entries hierarchically, in such a way
 *	that all entries in a subtree share a common prefix. This implementation
 *	uses crit-bit trees for integer and string keys: each node stores a 
 *	critical bit, which is the index of the bit where both branches differ.
 *	Entries where this bit is cleared are stored in the left subtree, and 
 *	those with this bit set are stored in the right subtree, all in a 
 *	recursive manner.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                        Unused                         |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Synonym                            |
 *     +---------------------------------------------------------------+
 *   2 |                             Size                              |
 *     +---------------------------------------------------------------+
 *   3 |                             Root                              |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_TRIEMAP_SIZE	- Number of elements in map.
 *	WORD_TRIEMAP_ROOT	- Root node of the trie.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_STRTRIEMAP_INIT>, <WORD_INTTRIEMAP_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_TRIEMAP_SIZE(word)		(((size_t *)(word))[2])
#define WORD_TRIEMAP_ROOT(word)		(((Col_Word *)(word))[3])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_STRTRIEMAP_INIT
 *
 *	Initializer for string trie map words.
 *
 * Argument:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *
 * See also:
 *	<WORD_TYPE_STRTRIEMAP>, <Col_NewStringTrieMap>
 *---------------------------------------------------------------------------*/

#define WORD_STRTRIEMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_STRTRIEMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_TRIEMAP_SIZE(word) = 0; \
    WORD_TRIEMAP_ROOT(word) = WORD_NIL;

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_INTTRIEMAP_INIT
 *
 *	Initializer for integer trie map words.
 *
 * Argument:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *
 * See also:
 *	<WORD_TYPE_INTTRIEMAP>, <Col_NewIntTrieMap>
 *---------------------------------------------------------------------------*/

#define WORD_INTTRIEMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTTRIEMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_TRIEMAP_SIZE(word) = 0; \
    WORD_TRIEMAP_ROOT(word) = WORD_NIL;

/*---------------------------------------------------------------------------
 * Internal Macros: Trie Node Fields
 *
 *	Accessors for generic trie node fields.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8                                                     n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                                                       |
 *     +-------+                   Type-specific data                  +
 *   1 |                                                               |
 *     +---------------------------------------------------------------+
 *   2 |                             Left                              |
 *     +---------------------------------------------------------------+
 *   3 |                             Right                             |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_TRIENODE_LEFT	- Left subtrie. Keys have their critical bit
 *				  cleared.
 *	WORD_TRIENODE_RIGHT	- Left subtrie. Keys have their critical bit
 *				  set.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_STRTRIENODE_INIT>, <WORD_INTTRIENODE_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_TRIENODE_LEFT(word)	(((Col_Word *)(word))[2])
#define WORD_TRIENODE_RIGHT(word)	(((Col_Word *)(word))[3])

/*---------------------------------------------------------------------------
 * Internal Macros: String Trie Node Fields
 *
 *	Accessors for string trie node fields.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8                    31                               n
 *     +-------+-----------------------+-------------------------------+
 *   0 | Type  |        Mask           |        Unused (n > 32)        |
 *     +-------+-----------------------+-------------------------------+
 *   1 |                             Diff                              |
 *     +---------------------------------------------------------------+
 *   2 |                        Left (Generic)                         |
 *     +---------------------------------------------------------------+
 *   3 |                        Right (Generic)                        |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_STRTRIENODE_MASK		- Get bitmask where only the critical 
 *					  bit is set. I.e. the highest bit of 
 *					  the first differing character where 
 *					  left and right subtries differ. 24-bit
 *					  is sufficient for Unicode chars.
 *	WORD_STRTRIENODE_SET_MASK	- Set bitmask to the critical bit.
 *	WORD_STRTRIENODE_DIFF		- Index of first differing character in
 *					  string.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Trie Node Fields>, <WORD_STRTRIENODE_INIT>
 *---------------------------------------------------------------------------*/

#define STRTRIENODE_MASK		(UINT_MAX>>CHAR_BIT)
#ifdef COL_BIGENDIAN
#   define WORD_STRTRIENODE_MASK(word)	((((Col_Char *)(word))[0])&STRTRIENODE_MASK)
#   define WORD_STRTRIENODE_SET_MASK(word, mask) ((((Col_Char *)(word))[0])&=~STRTRIENODE_MASK,(((Col_Char *)(word))[0])|=((size)&STRTRIENODE_MASK))
#else
#   define WORD_STRTRIENODE_MASK(word)	((((Col_Char *)(word))[0]>>CHAR_BIT)&STRTRIENODE_MASK)
#   define WORD_STRTRIENODE_SET_MASK(word, mask) ((((Col_Char *)(word))[0])&=~(STRTRIENODE_MASK<<CHAR_BIT),(((Col_Char *)(word))[0])|=(((mask)&STRTRIENODE_MASK)<<CHAR_BIT))
#endif
#define WORD_STRTRIENODE_DIFF(word)	(((size_t *)(word))[1])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_STRTRIENODE_INIT
 *
 *	Initializer for immutable string trie nodes.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	diff	- <WORD_STRTRIENODE_DIFF>
 *	mask	- <WORD_STRTRIENODE_SET_MASK>
 *	left	- <WORD_TRIENODE_LEFT>
 *	right	- <WORD_TRIENODE_RIGHT>
 *
 * See also:
 *	<WORD_TYPE_STRTRIENODE>
 *---------------------------------------------------------------------------*/

#define WORD_STRTRIENODE_INIT(word, diff, mask, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_STRTRIENODE); \
    WORD_STRTRIENODE_SET_MASK(word, mask); \
    WORD_STRTRIENODE_DIFF(word) = (diff); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MSTRTRIENODE_INIT
 *
 *	Initializer for mutable string trie nodes.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	diff	- <WORD_STRTRIENODE_DIFF>
 *	mask	- <WORD_STRTRIENODE_SET_MASK>
 *	left	- <WORD_TRIENODE_LEFT>
 *	right	- <WORD_TRIENODE_RIGHT>
 *
 * See also:
 *	<WORD_TYPE_MSTRTRIENODE>, <StringTrieMapFindEntry>
 *---------------------------------------------------------------------------*/

#define WORD_MSTRTRIENODE_INIT(word, diff, mask, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_MSTRTRIENODE); \
    WORD_STRTRIENODE_SET_MASK(word, mask); \
    WORD_STRTRIENODE_DIFF(word) = (diff); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);

/*---------------------------------------------------------------------------
 * Internal Macros: Integer Trie Node Fields
 *
 *	Accessors for integer trie node fields.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                        Unused                         |
 *     +-------+-------------------------------------------------------+
 *   1 |                             Mask                              |
 *     +---------------------------------------------------------------+
 *   2 |                        Left (Generic)                         |
 *     +---------------------------------------------------------------+
 *   3 |                        Right (Generic)                        |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_INTTRIENODE_MASK	- Bitmask where only the critical bit is set.
 *				  I.e. the highest bit where left and right 
 *				  subtries differ.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_MINTTRIENODE_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_INTTRIENODE_MASK(word)	(((intptr_t *)(word))[1])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_INTTRIENODE_INIT
 *
 *	Initializer for immutable integer trie nodes.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	mask	- <WORD_INTTRIENODE_MASK>
 *	left	- <WORD_TRIENODE_LEFT>
 *	right	- <WORD_TRIENODE_RIGHT>
 *
 * See also:
 *	<WORD_TYPE_INTTRIENODE>
 *---------------------------------------------------------------------------*/

#define WORD_INTTRIENODE_INIT(word, mask, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTTRIENODE); \
    WORD_INTTRIENODE_MASK(word) = (mask); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MINTTRIENODE_INIT
 *
 *	Initializer for mutable integer trie nodes.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	mask	- <WORD_INTTRIENODE_MASK>
 *	left	- <WORD_TRIENODE_LEFT>
 *	right	- <WORD_TRIENODE_RIGHT>
 *
 * See also:
 *	<WORD_TYPE_MINTTRIENODE>, <IntTrieMapFindEntry>
 *---------------------------------------------------------------------------*/

#define WORD_MINTTRIENODE_INIT(word, mask, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_MINTTRIENODE); \
    WORD_INTTRIENODE_MASK(word) = (mask); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_TRIELEAF_INIT
 *
 *	Initializer for immutable trie leaf words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_MAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_TYPE_TRIELEAF>
 *---------------------------------------------------------------------------*/

#define WORD_TRIELEAF_INIT(word, key, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_TRIELEAF); \
    WORD_MAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MTRIELEAF_INIT
 *
 *	Initializer for mutable trie leaf words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_MAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_TYPE_MTRIELEAF>, <StringTrieMapFindEntry>
 *---------------------------------------------------------------------------*/

#define WORD_MTRIELEAF_INIT(word, key, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_MTRIELEAF); \
    WORD_MAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_INTTRIELEAF_INIT
 *
 *	Initializer for immutable integer trie leaf words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_INTMAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *
 * See also:
 *	<WORD_TYPE_INTTRIELEAF>
 *---------------------------------------------------------------------------*/

#define WORD_INTTRIELEAF_INIT(word, key, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTTRIELEAF); \
    WORD_INTMAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MINTTRIELEAF_INIT
 *
 *	Initializer for mutable integer trie leaf words.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_INTMAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *
 * See also:
 *	<WORD_TYPE_MINTTRIELEAF>, <IntTrieMapFindEntry>
 *---------------------------------------------------------------------------*/

#define WORD_MINTTRIELEAF_INIT(word, key, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_MINTTRIELEAF); \
    WORD_INTMAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value);

#endif /* _COLIBRI_INTERNAL */
