#ifndef _COLIBRI_INTERNAL
#define _COLIBRI_INTERNAL

/* TODO replace char/short by (u)int(8|16)_t */

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


/*
 *----------------------------------------------------------------
 * System page allocator.
 *----------------------------------------------------------------
 */

/*
 * Pages are divided into cells. On a 32-bit system with 1024-byte logical 
 * pages, each page stores 64 16-byte cells. 
 *
 * Each page has reserved cells that store information about the page. The 
 * remaining cells store word info. For example, on a 32-bit system with 
 * 1024-byte pages, the first cell is reserved and is formatted as follows:
 *
 *  - bytes 0-3:	link to next page in pool.
 *  - byte 4:		generation.
 *  - byte 5:		flags:
 *			 - dirty: such pages potentially contain parents.
		         - last: whether page is last in system page.
 *  - byte 6-7:		size of system page containing this page.
 *  - bytes 8-15:	bitmask for allocated cells (64 bits).
 */

typedef char Page[PAGE_SIZE];
typedef char Cell[CELL_SIZE];

extern const char firstZeroBitSequence[7][256];
extern const char longestLeadingZeroBitSequence[256];
extern const char nbBitsSet[256];

#define RESERVED_CELLS		1
#define AVAILABLE_CELLS		(CELLS_PER_PAGE-RESERVED_CELLS)
#define NB_CELLS(size)		(((size)+CELL_SIZE-1)/CELL_SIZE)

#define PAGE_NEXT(page)		(*(Page **)(page))
#define PAGE_GENERATION(page)	(*((unsigned char *)(page)+sizeof(void *)))
#define PAGE_FLAGS(page)	(*((unsigned char *)(page)+sizeof(void *)+1))
#define PAGE_FLAG_DIRTY		1
#define PAGE_FLAG_LAST		2
#define PAGE_SYSTEMSIZE(page)	(*(unsigned short *)((unsigned char *)(page)+sizeof(void *)+2))
#define PAGE_BITMASK(page)	((unsigned char *)(page)+sizeof(void *)+4)
#define PAGE_CELL(page, index)	((Cell *)(page)+(index))

#define CELL_PAGE(cell)		((Page *)((uintptr_t)(cell) & ~(PAGE_SIZE-1)))
#define CELL_INDEX(cell)	(((uintptr_t)(cell) % PAGE_SIZE) / CELL_SIZE)

/*
 * Memory pools. Pools are a set of pages that store the words of a given 
 * generation.
 */

typedef struct MemoryPool {
    unsigned int generation;	/* Generation level; 0 = younger. */
    Page *pages;		/* Pages form a singly-linked list. */
    Page *lastPage;		/* Last page in pool. */
    Cell *lastFreeCell[AVAILABLE_CELLS]; 
				/* Last cell sequence of a given size found. */
    size_t nbPages;		/* Number of pages in pool. */
    size_t nbAlloc;		/* Number of pages alloc'd since last GC. */
    size_t nbSetCells;		/* Number of set cells in pool. */
    size_t gc;			/* GC counter. Used for generational GC. */
    Col_Word sweepables;	/* List of cells that need sweeping when 
				 * unreachable after a GC. */
    Col_ClientData data;	/* Opaque token for system-specific data. */
} MemoryPool;

/*
 * Pool initialization/cleanup.
 */

void			PoolInit(MemoryPool *pool, unsigned int generation);
void			PoolCleanup(MemoryPool *pool);

/*
 * Allocation/deallocation of pool pages.
 */

void			PoolAllocPages(MemoryPool *pool, size_t number);
void			PoolFreeEmptyPages(MemoryPool *pool);

/*
 * Allocation of cells in pool.
 */

Cell *			PoolAllocCells(MemoryPool *pool, size_t number);

/*
 * Allocation bitmask handling.
 */

void			SetCells(Page *page, size_t first, size_t number);
void			ClearCells(Page *page, size_t first, size_t number);
void			ClearAllCells(Page *page);
int			TestCell(Page *page, size_t index);
size_t			NbSetCells(Page *page);


/*
 *----------------------------------------------------------------
 * Mark-and-sweep, generational, exact GC.
 *----------------------------------------------------------------
 */

/*
 * Group data.
 */

typedef struct GroupData {
    /*
     * Threading model:
     *
     *	 - COL_SINGLE : strict appartment + stop-the-world model. GC is done 
     *	   synchronously when client thread resumes GC (Col_ResumeGC).
     *	 - COL_ASYNC : strict appartment model with asynchronous GC. GC uses a 
     *	   dedicated thread for asynchronous processing, the client thread 
     *	   cannot pause a running GC and is blocked until completion.
     *	 - COL_SHARED : shared multithreaded model with GC-preference. Data can 
     *	   be shared across client threads of the same group (COL_SHARED is the 
     *	   base index value). GC uses a dedicated thread for asynchronous 
     *	   processing.; GC process starts once all client threads get out of 
     *	   pause, no client thread can pause a scheduled GC.
     */

    unsigned int model;

    /* 
     * Root descriptors are stored in a trie indexed by the root cell address. 
     * Dirty page descriptors are stored in a linked list. Both types of cells
     * are managed in a dedicated memory pool (generation 0).
     */

    MemoryPool rootPool;
    Cell *roots;
    Cell *dirties;

    /* 
     * Memory pools for older generations (1 < generation < GC_MAX_GENERATIONS).
     */

    MemoryPool pools[GC_MAX_GENERATIONS-2];

    /*
     * Oldest collected generation during current GC.
     */

    unsigned int maxCollectedGeneration;

    /*
     * Whether to promote individual cells when promoting the oldest collected 
     * generation, instead of promoting whole pages. In practice this performs a 
     * compaction of this pool, which limits fragmentation and memory overhead. 
     */

#ifdef PROMOTE_COMPACT
    unsigned int compactGeneration;
#endif

    /*
     * Group members form a circular list.
     */

    struct ThreadData *first;

} GroupData;

/*
 * Thread-local data.
 */

typedef struct ThreadData {
    /*
     * Next in thread group member circular list.
     */

    struct ThreadData *next;

    /*
     * Thread group data.
     */

    GroupData *groupData;

    /*
     * Number of nested calls to Col_Init. Clear structures once it drops to 
     * zero.
     */

    size_t nestCount;

    /*
     * Error procs are thread-local.
     */

    Col_ErrorProc *errorProc;

    /* 
     * GC-protected section counter, i.e. nb of nested pause calls. When 
     * positive, we are in a GC-protected section.
     */

    size_t pauseGC;

    /* 
     * Eden, i.e. memory pool for generation 1.
     */

    MemoryPool eden;
} ThreadData;

/*
 * Initialization.
 */

void			GcInitThread(ThreadData *data);
void			GcInitGroup(GroupData *data);
void			GcCleanupThread(ThreadData *data);
void			GcCleanupGroup(GroupData *data);

/*
 * Cell allocation.
 */

Cell *			AllocCells(size_t number);

/*
 * Garbage collection.
 */

void			PerformGC(GroupData *data);

/*
 * Custom word handling for cleanup.
 */

void			DeclareCustomWord(Col_Word word, 
			    Col_CustomWordType *type);

/*
 * Roots are explicitly preserved words, using a reference count.
 *
 * Roots are stored in a trie indexed by the root source addresses. This forms 
 * an ordered collection that is traversed upon GC and is indexable and 
 * modifiable in O(log(k)) worst-case, k being the bit size of a cell pointer.
 *
 * On 32-bit architectures the single-cell layout is as follows:
 *
 * - Nodes:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Parent                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Mask                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Left                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Right                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Parent : 32-bit node
 *
 *	Mask : 32-bit unsigned
 *	    Bitmask where only the critical bit is set, i.e. the highest bit
 *	    where left and right nodes differ.
 *
 *	Left, Right : 32-bit nodes or leaves
 *	    Leave pointers have their LSB set so that they can be distinguished 
 *	    from nodes.
 *
 *
 * - Leaves:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Parent                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                          Generation                           |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                        Reference count                        |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Source                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Parent : 32-bit node
 *
 *	Generation : 32-bit unsigned
 *	    Generation of the source page.
 *
 *	Reference count : 32-bit unsigned
 *	    Once the refcount drops to zero the root is removed, however the
 *	    source may survive if it is referenced elsewhere.
 *
 *	Source : 32-bit word
 */

#define ROOT_PARENT(cell)		(((Cell **)(cell))[0])
#define ROOT_IS_LEAF(cell)		((uintptr_t)(cell) & 1)
#define ROOT_GET_NODE(cell)		((Cell *)((uintptr_t)(cell) & ~1))
#define ROOT_GET_LEAF(cell)		((Cell *)((uintptr_t)(cell) | 1))

#define ROOT_NODE_MASK(cell)		(((uintptr_t *)(cell))[1])
#define ROOT_NODE_LEFT(cell)		(((Cell **)(cell))[2])
#define ROOT_NODE_RIGHT(cell)		(((Cell **)(cell))[3])

#define ROOT_NODE_INIT(cell, parent, mask, left, right) \
    ROOT_PARENT(cell) = parent; \
    ROOT_NODE_MASK(cell) = mask; \
    ROOT_NODE_LEFT(cell) = left; \
    ROOT_NODE_RIGHT(cell) = right;

#define ROOT_LEAF_GENERATION(cell)	(((unsigned int *)(cell))[1])
#define ROOT_LEAF_REFCOUNT(cell)	(((size_t *)(cell))[2])
#define ROOT_LEAF_SOURCE(cell)		(((Col_Word *)(cell))[3])

#define ROOT_LEAF_INIT(cell, parent, generation, refcount, source) \
    ROOT_PARENT(cell) = parent; \
    ROOT_LEAF_GENERATION(cell) = generation; \
    ROOT_LEAF_REFCOUNT(cell) = refcount; \
    ROOT_LEAF_SOURCE(cell) = source;

/*
 * Dirty pages.
 *
 * Parent roots are like automatic roots: they belong to dirty pages whose cells 
 * potentially have children in newer generations.
 *
 * On 32-bit architectures the single-cell layout is as follows:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Next                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Page                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                          Generation                           |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Unused                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Next : 32-bit pointer to cell
 *	    Dirty nodes are linked together in a singly-linked list for 
 *	    traversal during GC.
 *
 *	Page : 32-bit pointer to page
 *
 *	Generation : 32-bit unsigned
 *	    Generation of the source page.
 */

#define DIRTY_NEXT(cell)	(((Cell **)(cell))[0])
#define DIRTY_PAGE(cell)	(((Page **)(cell))[1])
#define DIRTY_GENERATION(cell)	(((unsigned int *)(cell))[2])

#define DIRTY_INIT(cell, next, page, generation) \
    DIRTY_NEXT(cell) = next; \
    DIRTY_PAGE(cell) = page; \
    DIRTY_GENERATION(cell) = generation;


/*
 *----------------------------------------------------------------
 * Word structures.
 *----------------------------------------------------------------
 */

/*
 * Immediate value types.
 *
 * These types store their value directly in the word value, not in the pointed 
 * structure. For example, the value 0 represents nil. So these type IDs are
 * never used in the type field of word structures.
 *
 * Regular words point to structures stored in cells. As cells are aligned on
 * a multiple of their size (16 bytes on 32-bit systems), this means that a 
 * number of low bits are always zero (4 bits on 32-bit systems with 16-byte 
 * cells). Immediate values are distinguished from regular pointers by setting 
 * one of these bits. These bit patterns are called tags.
 *
 * The following tags are recognized:
 *
 *	P..			  ..P|0000	word pointer (aligned on 16 byte boundaries) including nil
 *	I..			     ..I|1 	small signed integer (31 bits on 32-bit systems)
 *	S..		     ..S|L....L|10	small string or char (general format)
 *	U..		     ..U|111111|10 	 - Unicode character (L=-1, 24 bits on 32-bit systems)
 *	0..                    0|000000|10	 - empty string
 *	0..          ..0S..  ..S|000001|10	 - 1-char 8-bit string (L=1)
 *	0..  ..0S..          ..S|000010|10	 - 2-char 8-bit string (L=2)
 *	S..		     ..S|000011|10	 - 3-char 8-bit string (L=3)
 *	L..			   ..L|100 	void list (full of nil)
 *	?..			  ..?|1000 	unused
 */

/* Careful: IS_IMMEDIATE returns false on nil! Nil needs explicit handling. */
#define IS_IMMEDIATE(word)	(((uintptr_t)(word))&0xF)

#define WORD_TYPE_NIL		0

#define WORD_TYPE_CUSTOM	-1

#define WORD_TYPE_SMALLINT	-2
#define WORD_TYPE_CHAR		-3
#define WORD_TYPE_SMALLSTR	-4
#define WORD_TYPE_VOIDLIST	-5

#define WORD_TYPE_WRAP		2

#define WORD_TYPE_UCSSTR	6
#define WORD_TYPE_UTF8STR	10
#define WORD_TYPE_SUBROPE	14
#define WORD_TYPE_CONCATROPE	18

#define WORD_TYPE_INT		22

#define WORD_TYPE_VECTOR	26
#define WORD_TYPE_MVECTOR	30
#define WORD_TYPE_LIST		34
#define WORD_TYPE_MLIST		38
#define WORD_TYPE_SUBLIST	42
#define WORD_TYPE_CONCATLIST	46
#define WORD_TYPE_MCONCATLIST	50
#define WORD_TYPE_MAPENTRY	54
#define WORD_TYPE_INTMAPENTRY	58
#define WORD_TYPE_STRHASHMAP	62
#define WORD_TYPE_INTHASHMAP	66
#define WORD_TYPE_STRTRIEMAP	70
#define WORD_TYPE_INTTRIEMAP	74
#define WORD_TYPE_STRTRIENODE	78
#define WORD_TYPE_INTTRIENODE	82
#ifdef PROMOTE_COMPACT
#   define WORD_TYPE_REDIRECT	254
#endif

/*
 * WORD_TYPE_UNKNOWN is used as a tag in the source code to mark places where
 * predefined type specific code is needed. Search for this tag when adding
 * new predefined word types.
 */

/* 
 * Type field. 
 *
 * We use the 1st machine word of the cell as the type field.
 *
 * The type field is either a numerical ID for predefined types or a pointer to
 * a Col_WordType structure. As such structures are always word-aligned, this 
 * means that the two least significant bits of their pointer value are zero 
 * (for architectures with at least 32 bit words) and are free for our purpose. 
 *
 * We use bit 0 of the type field as the pinned flag for both predefined type 
 * IDs and type pointers. Given the above, this bit is always on byte 0. 
 * When set, this means that the word isn't movable; its address remains fixed 
 * as long as this flag is set. Words can be moved to the upper generation pool 
 * during the compaction phase of the GC.
 *
 * Predefined type IDs are chosen so that their bit 1 is set, to distinguish 
 * them with type pointers and avoid value clashes. The ID only takes byte 0, 
 * the rest of the word being free to use. This gives up to 64 predefined type 
 * IDs (2-254 with steps of 4).
 *
 * On little endian architectures, the LSB of the type pointer is the cell's
 * byte 0. On big endian architectures, we rotate the pointer value one byte to 
 * the right so that the original LSB ends up on byte 0. That way the two 
 * reserved bits are on byte 0 for both predefined type IDs and type pointers.
 *
 * WORD_SET_TYPE_(ID|ADDR) also clears the pinned flag.
 *
 * On 32-bit architectures the cell layout is as follows:
 *
 *  - Predefined types:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |P|    Type     |                                               |
 *   +-+-+-+-+-+-+-+-+                                               |
 *   .                                                               .
 *   .                      Type-specific data                       .
 *   .                                                               .
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Pinned flag (P) : 1 bit
 *	Type : 7-bit value (with bit 1 set)
 *
 *
 *  - Custom words:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |P|0|                         Type                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   .                                                               .
 *   .                      Type-specific data                       .
 *   .                                                               .
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Pinned flag (P) : 1 bit
 *	Type : 32-bit pointer (with 2 lsbs cleared)
 */

#define CELL_TYPE(cell)		(((unsigned char *)(cell))[0])

/* Careful: don't give arguments with side effects! */
#define WORD_TYPE(word) \
    ((word)?								\
	 (((uintptr_t)(word))&0xF)?					\
	    /* Immediate values */					\
	     (((uintptr_t)(word))&1)?		WORD_TYPE_SMALLINT	\
	    :(((uintptr_t)(word))&2)?					\
		 (((uintptr_t)(word))&0x80)?	WORD_TYPE_CHAR		\
		:				WORD_TYPE_SMALLSTR	\
	    :(((uintptr_t)(word))&4)?		WORD_TYPE_VOIDLIST	\
	    /* Unknown */						\
	    :					WORD_TYPE_NIL		\
	/* Pointer or predefined */					\
	:(CELL_TYPE(word)&2)?						\
	     (CELL_TYPE(word)&~1)		/* Type ID */		\
	    :					WORD_TYPE_CUSTOM	\
    :						WORD_TYPE_NIL)

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
#define WORD_SET_TYPEID(word, type) (((unsigned char *)(word))[0] = (type))

#define WORD_PINNED(word)	(((unsigned char *)(word))[0] & 1)
#define WORD_SET_PINNED(word)	(((unsigned char *)(word))[0] |= 1)
#define WORD_CLEAR_PINNED(word)	(((unsigned char *)(word))[0] &= ~1)

/*
 * Immediate value fields.
 *
 * Values are for 32-bit systems.
 */

#define SMALLINT_MAX			(INT_MAX>>1)
#define SMALLINT_MIN			(INT_MIN>>1)
#define WORD_SMALLINT_GET(word)		(((int)(intptr_t)(word))>>1)
#define WORD_SMALLINT_NEW(value)	((Col_Word)(intptr_t)((((int)(value))<<1)|1))

#define WORD_CHAR_GET(word)		((Col_Char)(((uintptr_t)(word))>>8))
#define WORD_CHAR_NEW(value)		((Col_Word)((((uintptr_t)(value))<<8)|0xFE))

#define WORD_SMALLSTR_LENGTH(value)	((((uintptr_t)(value))&0xFC)>>2)
#define WORD_SMALLSTR_SET_LENGTH(word, length) (*((uintptr_t *)&(word)) = ((length)<<2)|2)
#ifdef COL_BIGENDIAN
#   define WORD_SMALLSTR_DATA(word)	((Col_Char1  *)&(word))
#else
#   define WORD_SMALLSTR_DATA(word)	(((Col_Char1 *)&(word))+1)
#endif
#define WORD_SMALLSTR_EMPTY		((Col_Word) 2)

#define VOIDLIST_MAX_LENGTH		(SIZE_MAX>>3)
#define WORD_VOIDLIST_LENGTH(word)	(((size_t)(intptr_t)(word))>>3)
#define WORD_VOIDLIST_NEW(length)	((Col_Word)(intptr_t)((((size_t)(length))<<3)|4))

#define WORD_LIST_EMPTY			WORD_VOIDLIST_NEW(0)

/*
 * Custom words.
 *
 * Values are for 32-bit systems.
 *
 * On 32-bit architectures the layout is as follows:
 *
 *  - Predefined types:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                           Type info                           |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                        Next (optional)                        |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   .                                                               .
 *   .                      Type-specific data                       .
 *   .                                                               .
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Next : 32-bit word
 *	    Words with a freeProc are singly-linked together using this field, 
 *	    so that unreachable words get swept properly upon GC.
 */

#define WORD_CUSTOM_NEXT(word)		(((Col_Word *)(word))[1])
#define WORD_CUSTOM_DATA(word, typeInfo) ((void *)(&WORD_CUSTOM_NEXT(word)+((typeInfo)->freeProc?1:0)))
#define WORD_CUSTOM_SIZE(size, typeInfo) NB_CELLS((size)+sizeof(Col_WordType *)+((typeInfo)->freeProc?sizeof(Cell *):0))

#define WORD_CUSTOM_INIT(word, typeInfo) \
    WORD_SET_TYPEINFO((word), (typeInfo));

/*
 * Wrap words.
 *
 * Words may have synonyms that can take any accepted word value: immediate 
 * values (inc. nil), or cell-based words. Words can thus be part of chains of 
 * synonyms having different types, but with semantically identical values. 
 * Such chains form a circular linked list using this field. The order of words 
 * in a synonym chain has no importance. Some word types have no synonym field,
 * in this case they must be wrapped into structures that have one when they
 * are added to a chain of synonyms. For performance reasons, all word types
 * with a synonym field use the same location for this field.
 *
 * This type is a generic wrapper for words needing a synonym field.
 *
 * On 32-bit architectures the single-cell layout is as follows:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |                    Unused                     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Synonym                            |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Source                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Unused                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Synonym : 32-bit word
 *
 *	Source : 32-bit word
 */

#define WORD_SYNONYM(word)	(((Col_Word *)(word))[1])
#define WORD_WRAP_SOURCE(word)	(((Col_Word *)(word))[2])

#define WORD_WRAP_INIT(word, source) \
    WORD_SET_TYPEID((word), WORD_TYPE_WRAP); \
    WORD_WRAP_SOURCE(word) = (source);

/*
 * Integer words.
 *
 * Used to wrap integers into a word structure. Needed when the integer is part 
 * of a synonym chain or when it is too large to fit an immediate small integer.
 *
 * On 32-bit architectures the single-cell layout is as follows:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |                    Unused                     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Synonym                            |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Value                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Unused                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Value : 32-bit integer
 */

#define WORD_INT_VALUE(word)	(*(int *)(&WORD_SYNONYM(word)+1))

#define WORD_INT_INIT(word, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_INT); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_INT_VALUE(word) = (value);


/*
 *----------------------------------------------------------------
 * Redirects.
 *----------------------------------------------------------------
 */

/*
 * Redirects.
 *
 * Redirects replace existing words during promotion. They only exist during
 * the GC.
 *
 * On 32-bit architectures the single-cell layout is as follows:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |                    Unused                     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Source                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                                                               |
 *   +                            Unused                             +
 *   |                                                               |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Source : 32-bit word
 */

#ifdef PROMOTE_COMPACT
#   define WORD_REDIRECT_SOURCE(word)	(((Col_Word *)(word))[1])
#endif /* PROMOTE_COMPACT*/


/*
 *----------------------------------------------------------------
 * Ropes.
 *----------------------------------------------------------------
 */

/*
 * Fixed-width UCS ropes.
 *
 * On 32-bit architectures the multi-cell layout is as follows:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |    Format     |            Length             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   .                                                               .
 *   .                         Character data                        .
 *   .                                                               .
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Format : 8-bit unsigned
 *	    Character format. Numeric value matches the character width.
 *
 *	Length : 16-bit unsigned
 *	    Character length. The actual byte length is a multiple of it.
 */

#define WORD_UCSSTR_HEADER_SIZE		(sizeof(Col_WordType *))

#define WORD_UCSSTR_FORMAT(word)	(((unsigned char *)(word))[1])
#define WORD_UCSSTR_LENGTH(word)	(((unsigned short *)(word))[1])
#define WORD_UCSSTR_DATA(word)		((const char *)(word)+WORD_UCSSTR_HEADER_SIZE)

#define UCSSTR_MAX_LENGTH		USHRT_MAX
#define UCSSTR_SIZE(byteLength)		(NB_CELLS(WORD_UCSSTR_HEADER_SIZE+(byteLength)))

#define WORD_UCSSTR_INIT(word, format, length) \
    WORD_SET_TYPEID((word), WORD_TYPE_UCSSTR); \
    WORD_UCSSTR_FORMAT(word) = format; \
    WORD_UCSSTR_LENGTH(word) = (unsigned short) (length);

/*
 * Variable-width UTF-8 ropes.
 *
 * Contrary to fixed-width versions, UTF-8 ropes are limited in size to one
 * page, so that access performances are better.
 *
 * On 32-bit architectures the multi-cell layout is as follows:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |    Unused     |            Length             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |          Byte length          |                               |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 *   .                                                               .
 *   .                         Character data                        .
 *   .                                                               .
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Length : 16-bit unsigned
 *	    Character length.
 *
 *	Byte length : 16-bit unsigned
 *	    An UTF-8 rope taking up to AVAILABLE_CELLS, this ensures that the 
 *	    actual size fits the length field width.
 */

#define WORD_UTF8STR_HEADER_SIZE	(sizeof(Col_WordType *)+sizeof(unsigned short))

#define WORD_UTF8STR_LENGTH(word)	(((unsigned short *)(word))[1])
#define WORD_UTF8STR_BYTELENGTH(word)	(((unsigned short *)(word))[2])
#define WORD_UTF8STR_DATA(word)		((const char *)(word)+WORD_UTF8STR_HEADER_SIZE)

#define UTF8STR_MAX_BYTELENGTH		(AVAILABLE_CELLS*CELL_SIZE-WORD_UTF8STR_HEADER_SIZE)
#define UTF8STR_SIZE(byteLength)	(NB_CELLS(WORD_UTF8STR_HEADER_SIZE+(byteLength)))

#define WORD_UTF8STR_INIT(word, length, byteLength) \
    WORD_SET_TYPEID((word), WORD_TYPE_UTF8STR); \
    WORD_UTF8STR_LENGTH(word) = (unsigned short) (length); \
    WORD_UTF8STR_BYTELENGTH(word) = (unsigned short) (byteLength); \

/*
 * Subropes.
 *
 * On 32-bit architectures the single-cell layout is as follows:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |     Depth     |            Unused             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Source                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             First                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Last                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Depth : 8-bit unsigned
 *	    8 bits will code up to 255 depth levels, which is more than 
 *	    sufficient for balanced binary trees. 
 *
 *	Source : 32-bit word
 *
 *	First, Last : 32-bit unsigned
 */

#define WORD_SUBROPE_DEPTH(word)	(((unsigned char *)(word))[1])
#define WORD_SUBROPE_SOURCE(word)	(((Col_Word *)(word))[1])
#define WORD_SUBROPE_FIRST(word)	(((size_t *)(word))[2])
#define WORD_SUBROPE_LAST(word)		(((size_t *)(word))[3])

#define WORD_SUBROPE_INIT(word, depth, source, first, last) \
    WORD_SET_TYPEID((word), WORD_TYPE_SUBROPE); \
    WORD_SUBROPE_DEPTH(word) = (unsigned char) (depth); \
    WORD_SUBROPE_SOURCE(word) = (source); \
    WORD_SUBROPE_FIRST(word) = (first); \
    WORD_SUBROPE_LAST(word) = (last);

/*
 * Concat ropes.
 *
 * On 32-bit architectures the single-cell layout is as follows:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |     Depth     |          Left length          |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Length                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Left                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Right                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Depth : 8-bit unsigned
 *	    8 bits will code up to 255 depth levels, which is more than 
 *	    sufficient for balanced binary trees. 
 *
 *	Left length : 16-bit unsigned
 *	    Used as shortcut to avoid dereferencing the left arm. Zero if actual 
 *	    length is larger than 65536.
 *
 *	Length : 32-bit unsigned
 *
 *	Left, Right : 32-bit words
 */

#define WORD_CONCATROPE_DEPTH(word)	(((unsigned char *)(word))[1])
#define WORD_CONCATROPE_LEFT_LENGTH(word) (((unsigned short *)(word))[1])
#define WORD_CONCATROPE_LENGTH(word)	(((size_t *)(word))[1])
#define WORD_CONCATROPE_LEFT(word)	(((Col_Word *)(word))[2])
#define WORD_CONCATROPE_RIGHT(word)	(((Col_Word *)(word))[3])

#define WORD_CONCATROPE_INIT(word, depth, length, leftLength, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_CONCATROPE); \
    WORD_CONCATROPE_DEPTH(word) = (unsigned char) (depth); \
    WORD_CONCATROPE_LENGTH(word) = (length); \
    WORD_CONCATROPE_LEFT_LENGTH(word) = (unsigned short) ((leftLength)>USHRT_MAX?0:(leftLength)); \
    WORD_CONCATROPE_LEFT(word) = (left); \
    WORD_CONCATROPE_RIGHT(word) = (right);

/*
 *----------------------------------------------------------------
 * Lists.
 *----------------------------------------------------------------
 */

/*
 * Vector words.
 *
 * Vectors are arrays of word elements that fit in a single word structure. 
 * Elements are directly addressable.
 *
 * Immutable vectors' content and length are fixed.
 * Mutable vectors can grow up to the word's reserved size.
 *
 * On 32-bit architectures the multi-cell layout is as follows:
 *
 *  - Immutable vector words:
 * 
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |                    Unused                     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Length                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   .                                                               .
 *   .                           Elements                            .
 *   .                                                               .
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Length : 32-bit unsigned
 *
 *	Elements : array of 32-bit words
 *
 *
 *  - Mutable vector words:
 * 
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |                     Size                      |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Length                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   .                                                               .
 *   .                           Elements                            .
 *   .                                                               .
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Size : 24-bit unsigned
 *	    The number of allocated cells. 24 bits should be sufficient to 
 *	    represent the maximum allocatable size.
 *
 *	Length : 32-bit unsigned
 *
 *	Elements : array of 32-bit words
 */

/* Immutable vectors: */
#define WORD_VECTOR_HEADER_SIZE		(sizeof(Col_WordType *)+sizeof(size_t))

#define WORD_VECTOR_LENGTH(word)	(((size_t *)(word))[1])
#define WORD_VECTOR_ELEMENTS(word)	((Col_Word *)(word)+2)
#define VECTOR_MAX_LENGTH(byteSize)	(((byteSize)-WORD_VECTOR_HEADER_SIZE)/sizeof(Col_Word))
#define VECTOR_SIZE(length)		(NB_CELLS(WORD_VECTOR_HEADER_SIZE+(length)*sizeof(Col_Word)))

#define WORD_VECTOR_INIT(word, length) \
    WORD_SET_TYPEID((word), WORD_TYPE_VECTOR); \
    WORD_VECTOR_LENGTH(word) = (length);

/* Mutable vectors: */
#define MVECTOR_MAX_SIZE		(UINT_MAX>>CHAR_BIT)
#define MVECTOR_SIZE_MASK		MVECTOR_MAX_SIZE

#ifdef COL_BIGENDIAN
#   define WORD_MVECTOR_SIZE(word)	((((size_t *)(word))[0])&MVECTOR_SIZE_MASK)
#   define WORD_MVECTOR_SET_SIZE(word, size) ((((size_t *)(word))[0])&=~MVECTOR_SIZE_MASK,(((size_t *)(word))[0])|=((size)&MVECTOR_SIZE_MASK))
#else
#   define WORD_MVECTOR_SIZE(word)	((((size_t *)(word))[0]>>CHAR_BIT)&MVECTOR_SIZE_MASK)
#   define WORD_MVECTOR_SET_SIZE(word, size) ((((size_t *)(word))[0])&=~(MVECTOR_SIZE_MASK<<CHAR_BIT),(((size_t *)(word))[0])|=(((size)&MVECTOR_SIZE_MASK)<<CHAR_BIT))
#endif

#define WORD_MVECTOR_INIT(word, size, length) \
    WORD_SET_TYPEID((word), WORD_TYPE_MVECTOR); \
    WORD_MVECTOR_SET_SIZE((word), (size)); \
    WORD_VECTOR_LENGTH(word) = (length);


/*
 * List words.
 *
 * Lists are collections of randomly accessed words elements. They are similar 
 * to vectors but have no size limit (bar the platform-specific integer limits). 
 * They are implemented as self-balanced binary trees whose leaves are vectors
 * and nodes are concat or sublist words, and are very similar to ropes.
 *
 * Immutable lists and nodes are made of immutable nodes and vectors.
 * Mutable lists and nodes can use both mutable or immutable nodes and vectors 
 * with a copy-on-write semantics. For this reason sublist nodes are never 
 * mutable.
 *
 * List words are used to wrap other nodes when the list is part of a synonym 
 * chain or when it is cyclic.
 *
 * On 32-bit architectures the single-cell layout is as follows:
 *
 *  - Sublist words:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |     Depth     |            Unused             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Source                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             First                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Last                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Depth : 8-bit unsigned
 *	    8 bits will code up to 255 depth levels, which is more than 
 *	    sufficient for balanced binary trees. 
 *
 *	Source : 32-bit word
 *
 *	First, Last : 32-bit unsigned
 *
 *
 *  - Concat words:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |     Depth     |          Left length          |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Length                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Left                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Right                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Depth : 8-bit unsigned
 *	    8 bits will code up to 255 depth levels, which is more than 
 *	    sufficient for balanced binary trees. 
 *
 *	Left length : 16-bit unsigned
 *	    Used as shortcut to avoid dereferencing the left arm. Zero if actual 
 *	    length is larger than 65536.
 *
 *	Length : 32-bit unsigned
 *
 *	Left, Right : 32-bit words
 *
 *
 *  - List words:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |                    Unused                     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Synonym                            |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Root                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Loop                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Root : 32-bit word
 *	    The root node, may be a vector, sublist or concat list node. The 
 *	    list length is given by its root node.
 *
 *	Loop : 32-bit unsigned
 *	    Terminal loop length for cyclic lists, else zero.
 */

/* Sublist words: */
#define WORD_SUBLIST_DEPTH(word)	(((unsigned char *)(word))[1])
#define WORD_SUBLIST_SOURCE(word)	(((Col_Word *)(word))[1])
#define WORD_SUBLIST_FIRST(word)	(((size_t *)(word))[2])
#define WORD_SUBLIST_LAST(word)		(((size_t *)(word))[3])

#define WORD_SUBLIST_INIT(word, depth, source, first, last) \
    WORD_SET_TYPEID((word), WORD_TYPE_SUBLIST); \
    WORD_SUBLIST_DEPTH(word) = (unsigned char) (depth); \
    WORD_SUBLIST_SOURCE(word) = (source); \
    WORD_SUBLIST_FIRST(word) = (first); \
    WORD_SUBLIST_LAST(word) = (last);

/* Concat words: */
#define WORD_CONCATLIST_DEPTH(word)	(((unsigned char *)(word))[1])
#define WORD_CONCATLIST_LEFT_LENGTH(word) (((unsigned short *)(word))[1])
#define WORD_CONCATLIST_LENGTH(word)	(((size_t *)(word))[1])
#define WORD_CONCATLIST_LEFT(word)	(((Col_Word *)(word))[2])
#define WORD_CONCATLIST_RIGHT(word)	(((Col_Word *)(word))[3])

#define WORD_CONCATLIST_INIT(word, depth, length, leftLength, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_CONCATLIST); \
    WORD_CONCATLIST_DEPTH(word) = (unsigned char) (depth); \
    WORD_CONCATLIST_LENGTH(word) = (length); \
    WORD_CONCATLIST_LEFT_LENGTH(word) = (unsigned short) ((leftLength)>USHRT_MAX?0:(leftLength)); \
    WORD_CONCATLIST_LEFT(word) = (left); \
    WORD_CONCATLIST_RIGHT(word) = (right);

#define WORD_MCONCATLIST_INIT(word, depth, length, leftLength, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_MCONCATLIST); \
    WORD_CONCATLIST_DEPTH(word) = (unsigned char) (depth); \
    WORD_CONCATLIST_LENGTH(word) = (length); \
    WORD_CONCATLIST_LEFT_LENGTH(word) = (unsigned short) ((leftLength)>USHRT_MAX?0:(leftLength)); \
    WORD_CONCATLIST_LEFT(word) = (left); \
    WORD_CONCATLIST_RIGHT(word) = (right);

/* List words: */
#define WORD_LIST_ROOT(word)		(((Col_Word *)(word))[2])
#define WORD_LIST_LOOP(word)		(((size_t *)(word))[3])

#define WORD_LIST_INIT(word, root, loop) \
    WORD_SET_TYPEID((word), WORD_TYPE_LIST); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_LIST_ROOT(word) = (root); \
    WORD_LIST_LOOP(word) = (loop);

#define WORD_MLIST_INIT(word, root, loop) \
    WORD_SET_TYPEID((word), WORD_TYPE_MLIST); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_LIST_ROOT(word) = (root); \
    WORD_LIST_LOOP(word) = (loop);


/*
 *----------------------------------------------------------------
 * Maps.
 *----------------------------------------------------------------
 */

/*
 * Map entries.
 *
 * Maps associate a key to a word value. The key is usually a word (including 
 * ropes) but specialized subtypes use integer values.
 *
 * All map types use the same entry word structure. Some fields have different
 * usage depending on their container (for example, the hash field is only used
 * by hash maps and ignored by tries).
 * 
 * On 32-bit architectures the single-cell layout is as follows:
 *
 *  - Generic map entry words:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |                     Hash                      |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Next                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                              Key                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Value                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Hash : 24-bit unsigned
 *	    Higher order bits of the 32-bit hash value for fast negative test.
 *	    Only used by hash maps.
 *
 *	Next : 32-bit word
 *	    Pointer to next entry in chain. The exact nature of this chain
 *	    depends on the container (for example, on hash maps this is the 
 *	    chain of all entries in a bucket).
 *
 *	Key : 32-bit word
 *	    Entry key.
 *
 *	Value : 32-bit word
 *	    Entry value word.
 *
 *
 *  - Integer map entry words:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |                    Unused                     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Next                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                              Key                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Value                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Next : 32-bit word
 *	    Pointer to next entry in chain. The exact nature of this chain
 *	    depends on the container (for example, on hash map this is the chain
 *	    of all entries in a bucket).
 *
 *	Key : 32-bit integer
 *	    Entry key.
 *
 *	Value : 32-bit word
 *	    Entry value word.
 */

/* Map entry words: */
#define WORD_MAPENTRY_NEXT(word)	(((Col_Word *)(word))[1])
#define WORD_MAPENTRY_KEY(word)		(((Col_Word *)(word))[2])
#define WORD_MAPENTRY_VALUE(word)	(((Col_Word *)(word))[3])
#define MAPENTRY_HASH_MASK		(UINT_MAX^UCHAR_MAX)
#ifdef COL_BIGENDIAN
#   define WORD_MAPENTRY_HASH(word)	((((unsigned int*)(word))[0]<<CHAR_BIT)&MAPENTRY_HASH_MASK)
#   define WORD_MAPENTRY_SET_HASH(word, hash) ((((unsigned int*)(word))[0])&=~(MAPENTRY_HASH_MASK>>CHAR_BIT),(((unsigned int*)(word))[0])|=(((hash)&MAPENTRY_HASH_MASK)>>CHAR_BIT))
#else
#   define WORD_MAPENTRY_HASH(word)	((((unsigned int *)(word))[0])&MAPENTRY_HASH_MASK)
#   define WORD_MAPENTRY_SET_HASH(word, hash) ((((unsigned int*)(word))[0])&=~MAPENTRY_HASH_MASK,(((unsigned int*)(word))[0])|=((hash)&MAPENTRY_HASH_MASK))
#endif

#define WORD_MAPENTRY_INIT(word, next, key, value, hash) \
    WORD_SET_TYPEID((word), WORD_TYPE_MAPENTRY); \
    WORD_MAPENTRY_NEXT(word) = (next); \
    WORD_MAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value); \
    WORD_MAPENTRY_SET_HASH(word, hash);

/* Integer map entry words: */
#define WORD_INTMAPENTRY_KEY(word)	(((int *)(word))[2])

#define WORD_INTMAPENTRY_INIT(word, next, key, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTMAPENTRY); \
    WORD_MAPENTRY_NEXT(word) = (next); \
    WORD_INTMAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value);

/*
 * Hash maps.
 *
 * Hash maps are mutable datatypes used to implement maps, AKA associative 
 * arrays. For each entry, an integer hash value is computed from the key, and 
 * this hash value is used to select a "bucket", i.e. an insertion point in a 
 * dynamic array. When two entries end up in the same bucket, there is a 
 * collision. When the map exceeds a certain size or the number of collisions 
 * exceed a certain threshold, the table is resized and entries are rehashed.
 *
 * For each bucket, hash entries are stored as linked lists. Each entry stores 
 * its hash, key and value, as well as a pointer to the next entry.
 *
 * Generic hash maps are custom word types of type COL_MAP, whose compare and
 * hash procs are not null.
 *
 * String hash maps are specialized hash maps using built-in compare and hash
 * procs on string keys.

 * Integer hash maps are specialized hash maps where the hash value is the 
 * integer key.
 * 
 * On 32-bit architectures the single-cell layout is as follows:
 *
 *  - Hash map words:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                           Type info                           |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Synonym                            |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Size                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Buckets                            |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   .                                                               .
 *   .                        Static buckets                         .
 *   .                                                               .
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Size : 32-bit unsigned
 *	    Number of elements in map.
 *
 *	Buckets : 32-bit word
 *	    Bucket container. If nil, buckets are stored in the inline static 
 *	    bucket array. Else it also uses hierarchic mutable vectors.
 *
 *	Static buckets : array of 32-bit words
 *	    Inline bucket array for small sized maps.
 */

/* Hash maps words: */
#define HASHMAP_STATICBUCKETS_NBCELLS	2
#define HASHMAP_STATICBUCKETS_SIZE	(HASHMAP_STATICBUCKETS_NBCELLS*CELL_SIZE/sizeof(Col_Word))
#define HASHMAP_NBCELLS			(HASHMAP_STATICBUCKETS_NBCELLS+1)

#define WORD_HASHMAP_HEADER_SIZE	CELL_SIZE
#define WORD_HASHMAP_SIZE(word)		(((size_t *)(word))[2])
#define WORD_HASHMAP_BUCKETS(word)	(((Col_Word *)(word))[3])
#define WORD_HASHMAP_STATICBUCKETS(word) ((Col_Word *)((char *)(word)+WORD_HASHMAP_HEADER_SIZE))

#define WORD_HASHMAP_INIT(word, typeInfo) \
    WORD_SET_TYPEINFO((word), (typeInfo)); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_HASHMAP_SIZE(word) = 0; \
    WORD_HASHMAP_BUCKETS(word) = WORD_NIL;

/* String hash map words: */
#define WORD_STRHASHMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_STRHASHMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_HASHMAP_SIZE(word) = 0; \
    WORD_HASHMAP_BUCKETS(word) = WORD_NIL;

/* Integer hash map words: */
#define WORD_INTHASHMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTHASHMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_HASHMAP_SIZE(word) = 0; \
    WORD_HASHMAP_BUCKETS(word) = WORD_NIL;

/*
 * Trie maps.
 *
 * TODO
 * 
 * On 32-bit architectures the single-cell layout is as follows:
 *
 *  - Trie map words:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |                    Unused                     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Synonym                            |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Size                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Root                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Size : 32-bit unsigned
 *	    Number of elements in map.
 *
 *	Root : 32-bit node
 *	    Root node of the trie.
 *
 *
 *  - String trie nodes:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |                     Mask                      |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Diff                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Left                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Right                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Mask : 24-bit unsigned
 *	    Bitmask where only the critical bit is set, i.e. the highest bit
 *	    where left and right nodes differ. 24-bit is sufficient for
 *	    Unicode chars.
 *
 *	Diff : 32-bit unsigned
 *	    Index of differing character in string.
 *
 *	Left, Right : 32-bit trie nodes
 *
 *
 *  - Integer trie nodes:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |                    Unused                     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Mask                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Left                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Right                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Mask : 32-bit unsigned
 *	    Bitmask where only the critical bit is set, i.e. the highest bit
 *	    where left and right nodes differ.
 *
 *	Left, Right : 32-bit trie nodes
 */

/* Trie map words: */
#define WORD_TRIEMAP_SIZE(word)		(((size_t *)(word))[2])
#define WORD_TRIEMAP_ROOT(word)		(((Col_Word *)(word))[3])

/* String trie map words: */
#define WORD_STRTRIEMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_STRTRIEMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_TRIEMAP_SIZE(word) = 0; \
    WORD_TRIEMAP_ROOT(word) = WORD_NIL;

/* Integer trie map words: */
#define WORD_INTTRIEMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTTRIEMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_TRIEMAP_SIZE(word) = 0; \
    WORD_TRIEMAP_ROOT(word) = WORD_NIL;

/* Trie nodes: */
#define WORD_TRIENODE_LEFT(word)	(((Col_Word *)(word))[2])
#define WORD_TRIENODE_RIGHT(word)	(((Col_Word *)(word))[3])

/* String trie nodes: */
#define STRTRIENODE_MASK		(UINT_MAX>>CHAR_BIT)
#ifdef COL_BIGENDIAN
#   define WORD_STRTRIENODE_MASK(word)	((((Col_Char *)(word))[0])&STRTRIENODE_MASK)
#   define WORD_STRTRIENODE_SET_MASK(word, mask) ((((Col_Char *)(word))[0])&=~STRTRIENODE_MASK,(((Col_Char *)(word))[0])|=((size)&STRTRIENODE_MASK))
#else
#   define WORD_STRTRIENODE_MASK(word)	((((Col_Char *)(word))[0]>>CHAR_BIT)&STRTRIENODE_MASK)
#   define WORD_STRTRIENODE_SET_MASK(word, mask) ((((Col_Char *)(word))[0])&=~(STRTRIENODE_MASK<<CHAR_BIT),(((Col_Char *)(word))[0])|=(((mask)&STRTRIENODE_MASK)<<CHAR_BIT))
#endif
#define WORD_STRTRIENODE_DIFF(word)	(((size_t *)(word))[1])

#define WORD_STRTRIENODE_INIT(word, diff, mask, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_STRTRIENODE); \
    WORD_STRTRIENODE_SET_MASK(word, mask); \
    WORD_STRTRIENODE_DIFF(word) = (diff); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);

/* Integer trie nodes: */
#define WORD_INTTRIENODE_MASK(word)	(((int *)(word))[1])

#define WORD_INTTRIENODE_INIT(word, mask, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTTRIENODE); \
    WORD_INTTRIENODE_MASK(word) = (mask); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);

#endif /* _COLIBRI_INTERNAL */
