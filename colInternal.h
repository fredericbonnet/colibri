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
 * remaining cells store rope info. For example, on a 32-bit system with 
 * 1024-byte pages, the first cell is reserved and is formatted as follows:
 *
 *  - bytes 0-3:	link to next page in pool.
 *  - byte 4:		generation.
 *  - byte 5:		lowest child generation from page.
 *  - byte 6:		size of system page containing this page.
 *  - byte 7:		1 if page is last in system page, else 0.
 *  - bytes 8-15:	bitmask for allocated cells (64 bits).
 */

extern const char firstZeroBitSequence[7][256];
extern const char longestLeadingZeroBitSequence[256];
extern const char nbBitsSet[256];

#define RESERVED_CELLS		1
#define AVAILABLE_CELLS		(CELLS_PER_PAGE-RESERVED_CELLS)
#define NB_CELLS(size)		(((size)+CELL_SIZE-1)/CELL_SIZE)

#define PAGE_NEXT(page)		(*(Page **)(page))
#define PAGE_GENERATION(page)	(*((unsigned char *)(page)+sizeof(void *)))
#define PAGE_CHILDGENERATION(page) (*((unsigned char *)(page)+sizeof(void *)+1))
#define PAGE_SYSTEMSIZE(page)	(*((unsigned char *)(page)+sizeof(void *)+2))
#define PAGE_LAST(page)		(*((unsigned char *)(page)+sizeof(void *)+3))
#define PAGE_BITMASK(page)	((unsigned char *)(page)+sizeof(void *)+4)
#define PAGE_CELL(page, index)	((Cell *)(page)+(index))

#define CELL_PAGE(cell)		((Page *)((uintptr_t)(cell) & ~(PAGE_SIZE-1)))
#define CELL_INDEX(cell)	(((uintptr_t)(cell) % PAGE_SIZE) / CELL_SIZE)

/*
 * Memory pools. Pools are a set of pages that store the ropes of a given 
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
    Cell *parents;		/* List of parents in pool. */
    Cell *sweepables;		/* List of cells that need sweeping when 
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
 * Thread-local GC and memory pool related structure.
 */

typedef struct GcMemInfo {
    /* 
     * Memory pools where the new cells are created. 0 is for roots, 1 (Eden) is
     * for regular cells, above is where cells and pages get promoted.
     */

    MemoryPool pools[GC_MAX_GENERATIONS];

    /* 
     * GC-protected section counter, i.e. nb of nested pause calls. When positive,
     * we are in a GC-protected section.
     */

    size_t pauseGC;

    /*
     * Oldest collected generation during GC.
     */

    unsigned int maxCollectedGeneration;

    /* 
     * Roots are stored in a trie indexed by the root source addresses.
     */

    Cell *roots;

    /*
     * Whether to promote individual cells when promoting the oldest collected 
     * generation, instead of promoting whole pages. In practice this performs a 
     * compaction of this pool, which limits fragmentation and memory overhead. 
     */

#ifdef PROMOTE_COMPACT
    unsigned int compactGeneration;
#endif
} GcMemInfo;

/*
 * Initialization.
 */

void			GcInit(void);
void			GcCleanup(void);

/*
 * Cell allocation.
 */

Cell *			AllocCells(size_t number);

/*
 * Custom rope and word handling for cleanup.
 */

void			DeclareCustomRope(Col_Rope rope);
void			DeclareWord(Col_Word word);


/*
 *----------------------------------------------------------------
 * Roots and parents.
 *----------------------------------------------------------------
 */

/*
 * Roots are explicitly preserved ropes or words, using a reference count.
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

#define ROOT_LEAF_GENERATION(cell)	(((size_t *)(cell))[1])
#define ROOT_LEAF_REFCOUNT(cell)	(((size_t *)(cell))[2])
#define ROOT_LEAF_SOURCE(cell)		(((Cell **)(cell))[3])

#define ROOT_LEAF_INIT(cell, parent, generation, refcount, source) \
    ROOT_PARENT(cell) = parent; \
    ROOT_LEAF_GENERATION(cell) = generation; \
    ROOT_LEAF_REFCOUNT(cell) = refcount; \
    ROOT_LEAF_SOURCE(cell) = source;

/*
 * Parents.
 *
 * Parent roots are like automatic roots: they point to pages whose cells 
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
 *   |                                                               |
 *   |                            Unused                             |
 *   |                                                               |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Next : 32-bit rope or word
 *	    Parents are linked together in a singly-linked list for traversal
 *	    during GC.
 *
 *	Page : 32-bit pointer to page
 *
 *	Generation : 32-bit unsigned
 *	    Generation of the page.
 */

#define PARENT_NEXT(cell)	(((Cell **)(cell))[0])
#define PARENT_PAGE(cell)	(((Page **)(cell))[1])

#define PARENT_INIT(cell, next, page) \
    PARENT_NEXT(cell) = next; \
    PARENT_PAGE(cell) = page;


/*
 *----------------------------------------------------------------
 * Rope structures.
 *----------------------------------------------------------------
 */

/* 
 * Type field. 
 *
 * We use the 2nd char of the cell to indicate the type, with the 1st char set 
 * to NUL. That way, regular non-empty C strings are valid ropes, and C string 
 * constants can be used everywhere ropes are expected. The only exception 
 * being the empty string "", which is not usable as is, because it is 
 * undistinguishable from ropes. Instead, the STRING_EMPTY macro must be used,
 * which is the string constant "\0", i.e. a byte array of two consecutive NUL
 * chars.
 *
 * The lead bit of the type field is the pinned flag. When set, this means 
 * that the rope isn't movable; its address remains fixed as long as this flag 
 * is set. Ropes can be moved to the upper generation pool during the compaction
 * phase of the GC.
 *
 * ROPE_SET_TYPE also clears the pinned flag.
 *
 * On 32-bit architectures the layout is as follows:
 *
 *  - C string "ABC":
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |      'A'      |      'B'      |      'C'      |       0       |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *
 *  - Empty string:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |       0       |       0       |                               |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *
 *  - Ropes:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |       0       |    Type     |P|                               |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 *   .                                                               .
 *   .                      Type-specific data                       .
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Pinned flag (P) : 1 bit
 *	Type : 7 bits
 */

/* Careful: don't give arguments with side effects! */
#define ROPE_TYPE(rope)	\
    ((rope)? \
	 ((rope)[0]?				ROPE_TYPE_C	\
	:((unsigned char)(rope)[1]&0x7F))	/* Type ID */   \
    :						ROPE_TYPE_NULL)
#define ROPE_SET_TYPE(rope, type) (((unsigned char *)(rope))[0] = 0, ((unsigned char *)(rope))[1] = (type))

#define ROPE_PINNED(rope)	(((unsigned char *)(rope))[1] & 0x80)
#define ROPE_SET_PINNED(rope)	(((unsigned char *)(rope))[1] |= 0x80)
#define ROPE_CLEAR_PINNED(rope)	(((unsigned char *)(rope))[1] &= ~0x80)


/* 
 * ROPE_TYPE_EMPTY is set to 0 so that an empty rope equals a C string with
 * two NUL chars, i.e. "\0".
 */

#define ROPE_TYPE_EMPTY		0
#define ROPE_TYPE_NULL		-1
#define ROPE_TYPE_C		-2

#define ROPE_TYPE_UCS1		1
#define ROPE_TYPE_UCS2		2
#define ROPE_TYPE_UCS4		3
#define ROPE_TYPE_UTF8		4
#define ROPE_TYPE_SUBROPE	5
#define ROPE_TYPE_CONCAT	6
#define ROPE_TYPE_CUSTOM	7

/*
 * ROPE_TYPE_UNKNOWN is used as a tag in the source code to mark places where
 * predefined type specific code is needed. Search for this tag when adding
 * new predefined rope types.
 */

/*
 * Rope fields.
 *
 * Values are for 32-bit systems.
 *
 */

/*
 * Fixed-width UCS ropes.
 *
 * On 32-bit architectures the multi-cell layout is as follows:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |          Rope header          |            Length             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   .                                                               .
 *   .                         Character data                        .
 *   .                                                               .
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Length : 16-bit unsigned
 *	    A rope taking up to AVAILABLE_CELLS, this ensures that the actual
 *	    size fits the length field width.
 */

#define ROPE_UCS_LENGTH(rope)	(((unsigned short *)(rope))[1])
#define ROPE_UCS_HEADER_SIZE	(sizeof(Col_Char4))
#define ROPE_UCS_DATA(rope)	((const Col_Char1 *)(rope)+ROPE_UCS_HEADER_SIZE)
#define ROPE_UCS1_DATA(rope)	((const Col_Char1 *) ROPE_UCS_DATA(rope))
#define ROPE_UCS2_DATA(rope)	((const Col_Char2 *) ROPE_UCS_DATA(rope))
#define ROPE_UCS4_DATA(rope)	((const Col_Char4 *) ROPE_UCS_DATA(rope))

#define UCS_MAX_SIZE		(AVAILABLE_CELLS*CELL_SIZE-ROPE_UCS_HEADER_SIZE)
#define UCS_SIZE(byteLength)	(NB_CELLS(ROPE_UCS_HEADER_SIZE+(byteLength)))

/*
 * Variable-width UTF-8 ropes.
 *
 * On 32-bit architectures the multi-cell layout is as follows:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |          Rope header          |            Length             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |          Byte length          |                               |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 *   .                                                               .
 *   .                         Character data                        .
 *   .                                                               .
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Length : 16-bit unsigned
 *
 *	Byte length : 16-bit unsigned
 *	    A rope taking up to AVAILABLE_CELLS, this ensures that the actual
 *	    size fits the length field width.
 */

#define ROPE_UTF8_LENGTH(rope)		(((unsigned short *)(rope))[1])
#define ROPE_UTF8_BYTELENGTH(rope)	(((unsigned short *)(rope))[2])
#define ROPE_UTF8_HEADER_SIZE		(sizeof(short)*3)
#define ROPE_UTF8_DATA(rope)		((const Col_Char1 *)(rope)+ROPE_UTF8_HEADER_SIZE)

#define UTF8_MAX_SIZE			(AVAILABLE_CELLS*CELL_SIZE-ROPE_UTF8_HEADER_SIZE)
#define UTF8_SIZE(byteLength)		(NB_CELLS(ROPE_UTF8_HEADER_SIZE+(byteLength)))

/*
 * Subropes.
 *
 * On 32-bit architectures the single-cell layout is as follows:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |          Rope header          |     Depth     |    Unused     |
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
 *	Source : 32-bit rope
 *
 *	First, Last : 32-bit unsigned
 */

#define ROPE_SUBROPE_DEPTH(rope)	(((unsigned char *)(rope))[2]) 
#define ROPE_SUBROPE_SOURCE(rope)	(((Col_Rope *)(rope))[1])
#define ROPE_SUBROPE_FIRST(rope)	(((size_t *)(rope))[2])
#define ROPE_SUBROPE_LAST(rope)		(((size_t *)(rope))[3])

#define ROPE_SUBROPE_INIT(rope, depth, source, first, last) \
    ROPE_SET_TYPE((rope), ROPE_TYPE_SUBROPE); \
    ROPE_SUBROPE_DEPTH(rope) = (unsigned char) (depth); \
    ROPE_SUBROPE_SOURCE(rope) = (source); \
    ROPE_SUBROPE_FIRST(rope) = (first); \
    ROPE_SUBROPE_LAST(rope) = (last);

/*
 * Concat ropes.
 *
 * On 32-bit architectures the single-cell layout is as follows:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |          Rope header          |     Depth     |  Left length  |
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
 *	Left length : 8-bit unsigned
 *	    Used as shortcut to avoid dereferencing the left arm. Zero if actual
 *	    length is larger than 255.
 *
 *	Length : 32-bit unsigned
 *
 *	Left, Right : 32-bit ropes
 */

#define ROPE_CONCAT_DEPTH(rope)		(((unsigned char *)(rope))[2])
#define ROPE_CONCAT_LEFT_LENGTH(rope)	(((unsigned char *)(rope))[3])
#define ROPE_CONCAT_LENGTH(rope)	(((size_t *)(rope))[1])
#define ROPE_CONCAT_LEFT(rope)		(((Col_Rope *)(rope))[2])
#define ROPE_CONCAT_RIGHT(rope)		(((Col_Rope *)(rope))[3])

#define ROPE_CONCAT_INIT(rope, depth, length, leftLength, left, right) \
    ROPE_SET_TYPE((rope), ROPE_TYPE_CONCAT); \
    ROPE_CONCAT_DEPTH(rope) = (unsigned char) (depth); \
    ROPE_CONCAT_LENGTH(rope) = (length); \
    ROPE_CONCAT_LEFT_LENGTH(rope) = (unsigned char) ((leftLength)>UCHAR_MAX?0:(leftLength)); \
    ROPE_CONCAT_LEFT(rope) = (left); \
    ROPE_CONCAT_RIGHT(rope) = (right);

/*
 * Custom ropes.
 *
 * On 32-bit architectures the multi-cell layout is as follows:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |          Rope header          |               Size            |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                         Type pointer                         |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   .                                                               .
 *   .                          Custom data                          .
 *   .                                                               .
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                        Next (optional)                        |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Size : 16-bit unsigned
 *	    A rope taking up to AVAILABLE_CELLS, this ensures that the actual
 *	    size fits the length field width.
 *
 *	Type pointer : 32-bit pointer
 *
 *	Next : 32-bit rope or word (optional)
 *	    Custom ropes with a freeProc are singly-linked together using this 
 *	    field, so that unreachable ropes get swept properly upon GC.
 */

#define ROPE_CUSTOM_HEADER_SIZE		(sizeof(void *) + sizeof(Col_RopeCustomType))
#define ROPE_CUSTOM_SIZE(rope)		(((unsigned short *)(rope))[1])
#define ROPE_CUSTOM_TYPE(rope)		(((Col_RopeCustomType **)(rope))[1])
#define ROPE_CUSTOM_DATA(rope)		((void *)((rope)+ROPE_CUSTOM_HEADER_SIZE))
#define ROPE_CUSTOM_TRAILER_SIZE	(sizeof(Col_Rope))
#define ROPE_CUSTOM_NEXT(rope, size)	(*(Cell **)((rope)+NB_CELLS(ROPE_CUSTOM_HEADER_SIZE+(size)+ROPE_CUSTOM_TRAILER_SIZE)*CELL_SIZE-ROPE_CUSTOM_TRAILER_SIZE))

#define CUSTOM_MAX_SIZE			(AVAILABLE_CELLS*CELL_SIZE-ROPE_CUSTOM_HEADER_SIZE)

#define ROPE_CUSTOM_INIT(rope, type, size) \
    ROPE_SET_TYPE((rope), ROPE_TYPE_CUSTOM); \
    ROPE_CUSTOM_TYPE(rope) = (type); \
    ROPE_CUSTOM_SIZE(rope) = (unsigned short) (size);


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
 *	L..			   ..L|100 	Void list (full of nil)
 *	P..			  ..P|1000 	rope pointer (not including C strings)
 */

/* Careful: IS_IMMEDIATE returns false on nil! Nil needs explicit handling. */
#define IS_IMMEDIATE(word)	(((uintptr_t)(word))&7)
#define IS_ROPE(word)		((((uintptr_t)(word))&0xF) == 8)

#define WORD_TYPE_NIL		-1
#define WORD_TYPE_SMALL_INT	-2
#define WORD_TYPE_CHAR		-3
#define WORD_TYPE_SMALL_STRING	-4
#define WORD_TYPE_VOID_LIST	-5
#define WORD_TYPE_ROPE		-6

#define WORD_TYPE_REGULAR	0

#define WORD_TYPE_INT		4
#define WORD_TYPE_STRING	8
#define WORD_TYPE_REFERENCE	12
#define WORD_TYPE_VECTOR	16
#define WORD_TYPE_MVECTOR	20
#define WORD_TYPE_LIST		24
#define WORD_TYPE_MLIST		28
#define WORD_TYPE_SUBLIST	32
#define WORD_TYPE_CONCATLIST	36
#define WORD_TYPE_MCONCATLIST	40
/*#define WORD_TYPE_HASHMAP	44*/
#define WORD_TYPE_INTHASHMAP	48
/*#define WORD_TYPE_HASHENTRY	52*/
#define WORD_TYPE_INTHASHENTRY	56
/*#define WORD_TYPE_TRIEMAP	60*/
#define WORD_TYPE_INTTRIEMAP	64
#define WORD_TYPE_TRIENODE	68
/*#define WORD_TYPE_TRIELEAF	72*/
#define WORD_TYPE_INTTRIELEAF	76

/*
 * WORD_TYPE_UNKNOWN is used as a tag in the source code to mark places where
 * predefined type specific code is needed. Search for this tag when adding
 * new predefined word types.
 */

/* 
 * Type field. 
 *
 * We use the 1st machine word of the cell as the type field. To distinguish 
 * word cells from rope cells, we must ensure that the 1st byte word cells 
 * is always non-zero, contrary to rope cells.
 *
 * The type field is either a numerical ID for predefined types or a pointer 
 * to a Col_WordType structure. As such structures are always word-aligned,
 * this means that the two least significant bits of their pointer value are
 * zero (for architectures with at least 32 bit words) and are free for our 
 * purpose. 
 *
 * To ensure that the first byte is always non-zero, we set bit 1 of type 
 * pointers. On little endian architectures, the LSB is byte 0, so setting bit
 * 1 is sufficient to ensure that byte 0 is non-zero. On big endian 
 * architectures, we rotate the pointer value one byte to the right so that the 
 * original LSB ends up on byte 0.
 *
 * Predefined type IDs are chosen so that their bit 1 is zero to 
 * distinguish them with type pointers and avoid value clashes. The ID only
 * takes byte 0, the rest of the word being free to use. This gives up to 63
 * predefined type IDs (4-252 with steps of 4).
 *
 * We use bit 0 of the type field as the pinned flag for both predefined type 
 * IDs and type pointers. Given the above, this bit is always on byte 0. 
 * When set, this means that the word isn't movable; its address remains fixed 
 * as long as this flag is set. Words can be moved to the upper generation pool 
 * during the compaction phase of the GC.
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
 *   |P|0|   Type    |                                               |
 *   +-+-+-+-+-+-+-+-+                                               +
 *   .                                                               .
 *   .                      Type-specific data                       .
 *   .                                                               .
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Pinned flag (P) : 1 bit
 *	Type : 6 bits
 *
 *
 *  - Regular words:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |P|1|                         Type                              |
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
	     (((uintptr_t)(word))&1)?		WORD_TYPE_SMALL_INT	\
	    :(((uintptr_t)(word))&2)?					\
		 (((uintptr_t)(word))&0x80)?	WORD_TYPE_CHAR		\
		:				WORD_TYPE_SMALL_STRING	\
	    :(((uintptr_t)(word))&4)?		WORD_TYPE_VOID_LIST	\
	    /* Ropes */							\
	    :					WORD_TYPE_ROPE		\
	/* Pointer or predefined */					\
	:(CELL_TYPE(word)&2)?			WORD_TYPE_REGULAR	\
	:(CELL_TYPE(word)&~3)			/* Type ID */		\
    :						WORD_TYPE_NIL)

#ifdef COL_BIGENDIAN
#   define WORD_GET_TYPE_ADDR(word, addr) \
	((addr) = (Col_WordType *)(ROTATE_LEFT(*(uintptr_t *)(word))&~3))
#   define WORD_SET_TYPE_ADDR(word, addr) \
	(*(uintptr_t *)(word) = ROTATE_RIGHT(((uintptr_t)(addr))|2))
#else
#   define WORD_GET_TYPE_ADDR(word, addr) \
	((addr) = (Col_WordType *)(*(uintptr_t *)(word)&~3))
#   define WORD_SET_TYPE_ADDR(word, addr) \
	(*(uintptr_t *)(word) = ((uintptr_t)(addr))|2)
#endif
#define WORD_SET_TYPE_ID(word, type) (((unsigned char *)(word))[0] = (type))

#define WORD_PINNED(word)	(((unsigned char *)(word))[0] & 1)
#define WORD_SET_PINNED(word)	(((unsigned char *)(word))[0] |= 1)
#define WORD_CLEAR_PINNED(word)	(((unsigned char *)(word))[0] &= ~1)

/*
 * Immediate & rope value fields.
 *
 * Values are for 32-bit systems.
 */

#define SMALL_INT_MAX			(INT_MAX>>1)
#define SMALL_INT_MIN			(INT_MIN>>1)
#define WORD_SMALL_INT_GET(word)	(((int)(intptr_t)(word))>>1)
#define WORD_SMALL_INT_NEW(value)	((Col_Word)(intptr_t)((((int)(value))<<1)|1))

#define WORD_CHAR_GET(word)		((Col_Char)(((uintptr_t)(word))>>8))
#define WORD_CHAR_NEW(value)		((Col_Word)((((uintptr_t)(value))<<8)|0xFE))

#define WORD_SMALL_STRING_LENGTH(value) ((((uintptr_t)(value))&0xFC)>>2)
#define WORD_SMALL_STRING_SET_LENGTH(word, length) (*((uintptr_t *)&(word)) = ((length)<<2)|2)
#ifdef COL_BIGENDIAN
#   define WORD_SMALL_STRING_DATA(word)	((Col_Char1  *)&(word))
#else
#   define WORD_SMALL_STRING_DATA(word)	(((Col_Char1 *)&(word))+1)
#endif
#define WORD_SMALL_STRING_EMPTY		((Col_Word) 2)

#define VOID_LIST_MAX_LENGTH		(SIZE_MAX>>3)
#define WORD_VOID_LIST_LENGTH(word)	(((size_t)(intptr_t)(word))>>3)
#define WORD_VOID_LIST_NEW(length)	((Col_Word)(intptr_t)((((size_t)(length))<<3)|4))

#define WORD_EMPTY			WORD_VOID_LIST_NEW(0)

#define WORD_ROPE_GET(word)		((Col_Rope)(((uintptr_t)(word))&~0xF))
#define WORD_ROPE_NEW(value)		((Col_Word)(((uintptr_t)(value))|8))

/*
 * Word fields.
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
 *   |                            Synonym                            |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   .                                                               .
 *   .                      Type-specific data                       .
 *   .                                                               .
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                        Next (optional)                        |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Synonym : 32-bit word
 *	    Words may have synonyms that can take any accepted word value:
 *	    immediate values (inc. nil), ropes, or other words. Words can thus 
 *	    be part of chains of synonyms having different types, but with 
 *	    semantically identical values. Such chains form a circular linked 
 *	    list using this field. The order of words in a synonym chain has no 
 *	    importance.
 *
 *	Next : 32-bit rope or word (optional)
 *	    Words with a freeProc are singly-linked together using this field, 
 *	    so that unreachable words get swept properly upon GC.
 */

#define WORD_HEADER_SIZE	(sizeof(Col_WordType *)+sizeof(Col_Word))
#define WORD_SYNONYM(word)	(((Col_Word *)(word))[1])
#define WORD_DATA(word)		((void *)((char *)(word)+WORD_HEADER_SIZE))
#define WORD_TRAILER_SIZE	(sizeof(Col_Word))
#define WORD_NEXT(word, size)	(*(Cell **)((char *)(word)+NB_CELLS(WORD_HEADER_SIZE+(size)+WORD_TRAILER_SIZE)*CELL_SIZE-WORD_TRAILER_SIZE))

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
 *   |                             Data                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Unused                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Data : 32-bit integer
 */

#define WORD_INT_DATA(word)	(*(int *) WORD_DATA(word))

#define WORD_INT_INIT(word, data) \
    WORD_SET_TYPE_ID((word), WORD_TYPE_INT); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_INT_DATA(word) = (data);

/*
 * String words.
 *
 * Used to wrap ropes into a word structure. Needed when the rope is part of
 * a synonym chain.
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
 *   |                             Data                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                           Alternate                           |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Data, Alternate : 32-bit rope
 *	    As its name says, Alternate provides an alternate representation
 *	    of the string, such as a quoted or encoded form.
 */

#define WORD_STRING_DATA(word)	(((Col_Rope *) WORD_DATA(word))[0])
#define WORD_STRING_ALT(word)	(((Col_Rope *) WORD_DATA(word))[1])

#define WORD_STRING_INIT(word, data, alt) \
    WORD_SET_TYPE_ID((word), WORD_TYPE_STRING); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_STRING_DATA(word) = (data); \
    WORD_STRING_ALT(word) = (alt);


/*
 *----------------------------------------------------------------
 * Redirects.
 *----------------------------------------------------------------
 */

/*
 * Redirects.
 *
 * Redirects replace existing ropes during promotion. They only exist during
 * the GC.
 *
 * They use the same fields for both rope and word versions, only the header
 * field and type ID change.
 *
 * On 32-bit architectures the single-cell layout is as follows:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                    Rope or word type info                     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Unused                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Source                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                            Unused                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Source : 32-bit rope or word
 */

#ifdef PROMOTE_COMPACT
#   define ROPE_TYPE_REDIRECT	0x7F
#   define WORD_TYPE_REDIRECT	0xFC

#   define ROPE_REDIRECT_SOURCE(rope)	(((Col_Rope *)(rope))[2])
#   define WORD_REDIRECT_SOURCE(word)	(((Col_Word *)(word))[2])
#endif /* PROMOTE_COMPACT*/


/*
 *----------------------------------------------------------------
 * References.
 *----------------------------------------------------------------
 */

/*
 * Reference words.
 *
 * References can be used to build self-referential structures, especially 
 * immutable ones such as lists. References can be resolved after their 
 * creation, allowing for example a list or one of its children to include a 
 * reference to an element that is created at a later time. 
 *
 * References may also help identify words having the same value in immutable
 * structures where words are naturally shared.
 *
 * References are synonymous with the word they refer to. Thus, unbound
 * references are semantically identical to nil. So an unbound reference is 
 * not an error condition at this level of abstraction, although it may be one 
 * if the upper abstraction layer expects non-nil values. In other words, 
 * references are outside the domain of values.
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
 *	Source : 32-bit word.
 *	    Points to the referenced word.
 */

#define WORD_REFERENCE_SOURCE(word)	(((Col_Word *)(word))[2])

#define WORD_REFERENCE_INIT(word, source) \
    WORD_SET_TYPE_ID((word), WORD_TYPE_REFERENCE); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_REFERENCE_SOURCE(word) = (source);


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
 *   |                            Synonym                            |
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
 *   |                            Synonym                            |
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

/* Immutable vector: */
#define WORD_VECTOR_HEADER_SIZE		(sizeof(size_t))

#define WORD_VECTOR_LENGTH(word)	(((size_t *)(word))[2])
#define WORD_VECTOR_ELEMENTS(word)	((Col_Word *)(word)+3)
#define VECTOR_MAX_LENGTH		((SIZE_MAX-(WORD_HEADER_SIZE+WORD_VECTOR_HEADER_SIZE))/sizeof(Col_Word))
#define VECTOR_SIZE(length)		(NB_CELLS(WORD_HEADER_SIZE+WORD_VECTOR_HEADER_SIZE+(length)*sizeof(Col_Word)))

#define WORD_VECTOR_INIT(word, length) \
    WORD_SET_TYPE_ID((word), WORD_TYPE_VECTOR); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_VECTOR_LENGTH(word) = (length);

/* Mutable vector: */
#define MVECTOR_MAX_SIZE		(UINT_MAX>>CHAR_BIT)
#define MVECTOR_SIZE_MASK		MVECTOR_MAX_SIZE
#define MVECTOR_MAX_LENGTH(size)	(((size)*CELL_SIZE-(WORD_HEADER_SIZE+WORD_VECTOR_HEADER_SIZE))/sizeof(Col_Word))

#ifdef COL_BIGENDIAN
#   define WORD_MVECTOR_SIZE(word)	((((size_t *)(word))[0])&MVECTOR_SIZE_MASK)
#   define WORD_MVECTOR_SET_SIZE(word, size) ((((size_t *)(word))[0])&=~MVECTOR_SIZE_MASK,(((size_t *)(word))[0])|=((size)&MVECTOR_SIZE_MASK))
#else
#   define WORD_MVECTOR_SIZE(word)	((((size_t *)(word))[0]>>CHAR_BIT)&MVECTOR_SIZE_MASK)
#   define WORD_MVECTOR_SET_SIZE(word, size) ((((size_t *)(word))[0])&=~(MVECTOR_SIZE_MASK<<CHAR_BIT),(((size_t *)(word))[0])|=(((size)&MVECTOR_SIZE_MASK)<<CHAR_BIT))
#endif

#define WORD_MVECTOR_INIT(word, size, length) \
    WORD_SET_TYPE_ID((word), WORD_TYPE_MVECTOR); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_MVECTOR_SET_SIZE((word), (size)); \
    WORD_VECTOR_LENGTH(word) = (length);


/*
 * List words.
 *
 * Lists are collections of randomly accessed words elements. They are similar 
 * to vectors but have no size limit (bar the platform-specific integer limits). 
 * They are implemented as self-balanced binary trees whose leaves are vectors, 
 * and are very similar to ropes.
 *
 * Concat and sublist nodes are not regular words in the sense that they have no 
 * synonym field. Instead they store their size. They are similar to concat 
 * resp. substring ropes and use the same format. They are never used 
 * independently from list words.
 *
 * Immutable lists and nodes are made of immutable nodes and vectors.
 * Mutable lists and nodes can use both mutable or immutable nodes and vectors 
 * with a copy-on-write semantics. For this reason sublist nodes are never 
 * mutable.
 *
 * On 32-bit architectures the single-cell layout is as follows:
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
 *	    The root is either a sublist or concat list node. The list length
 *	    is given by its root node.
 *
 *	Loop : 32-bit unsigned
 *	    Terminal loop length for cyclic lists, else zero.
 *
 *
 *  - Sublist nodes:
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
 *  - Concat nodes:
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
 *	Left, Right : 32-bit words or list nodes
 */

/* List words: */
#define WORD_LIST_ROOT(word)		(((Col_Word *)(word))[2])
#define WORD_LIST_LOOP(word)		(((size_t *)(word))[3])

#define WORD_LIST_INIT(word, root, loop) \
    WORD_SET_TYPE_ID((word), WORD_TYPE_LIST); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_LIST_ROOT(word) = (root); \
    WORD_LIST_LOOP(word) = (loop);

#define WORD_MLIST_INIT(word, root, loop) \
    WORD_SET_TYPE_ID((word), WORD_TYPE_MLIST); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_LIST_ROOT(word) = (root); \
    WORD_LIST_LOOP(word) = (loop);

#define WORD_LISTNODE_DEPTH(node)	(((unsigned char *)(node))[1])

/* Sublist nodes: */
#define WORD_SUBLIST_SOURCE(node)	(((Col_Word *)(node))[1])
#define WORD_SUBLIST_FIRST(node)	(((size_t *)(node))[2])
#define WORD_SUBLIST_LAST(node)		(((size_t *)(node))[3])

#define WORD_SUBLIST_INIT(node, depth, source, first, last) \
    WORD_SET_TYPE_ID((node), WORD_TYPE_SUBLIST); \
    WORD_LISTNODE_DEPTH(node) = (unsigned char) (depth); \
    WORD_SUBLIST_SOURCE(node) = (source); \
    WORD_SUBLIST_FIRST(node) = (first); \
    WORD_SUBLIST_LAST(node) = (last);

/* Concat node: */
#define WORD_CONCATLIST_LEFT_LENGTH(node) (((unsigned short *)(node))[1])
#define WORD_CONCATLIST_LENGTH(node)	(((size_t *)(node))[1])
#define WORD_CONCATLIST_LEFT(node)	(((Col_Word *)(node))[2])
#define WORD_CONCATLIST_RIGHT(node)	(((Col_Word *)(node))[3])

#define WORD_CONCATLIST_INIT(node, depth, length, leftLength, left, right) \
    WORD_SET_TYPE_ID((node), WORD_TYPE_CONCATLIST); \
    WORD_LISTNODE_DEPTH(node) = (unsigned char) (depth); \
    WORD_CONCATLIST_LENGTH(node) = (length); \
    WORD_CONCATLIST_LEFT_LENGTH(node) = (unsigned short) ((leftLength)>USHRT_MAX?0:(leftLength)); \
    WORD_CONCATLIST_LEFT(node) = (left); \
    WORD_CONCATLIST_RIGHT(node) = (right);

#define WORD_MCONCATLIST_INIT(node, depth, length, leftLength, left, right) \
    WORD_SET_TYPE_ID((node), WORD_TYPE_MCONCATLIST); \
    WORD_LISTNODE_DEPTH(node) = (unsigned char) (depth); \
    WORD_CONCATLIST_LENGTH(node) = (length); \
    WORD_CONCATLIST_LEFT_LENGTH(node) = (unsigned short) ((leftLength)>USHRT_MAX?0:(leftLength)); \
    WORD_CONCATLIST_LEFT(node) = (left); \
    WORD_CONCATLIST_RIGHT(node) = (right);


/*
 *----------------------------------------------------------------
 * Maps.
 *----------------------------------------------------------------
 */

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
 * Integer hash maps are specialized hash maps where the hash value is the 
 * integer key. The hash values thus don't need to be stored in map entries,
 * which saves space & CPU.
 * 
 *  - Hash map words:
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
 *   |                       Hash key function                       |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                     Compare keys function                     |
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
 *	Hash key function : 32-bit pointer
 *	    Pointer to function for computing the hash value from a key.
 *	    
 *	Key compare function : 32-bit pointer
 *	    Pointer to function for comparing two keys.
 *
 *	Buckets : 32-bit word
 *	    Mutable vector/list node of buckets. If nil, buckets are stored in
 *	    the inline static bucket array.
 *
 *	Static buckets : array of 32-bit words
 *	    Inline bucket array for small sized maps.
 *
 *
 *  - Integer hash map words:
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
 *
 *
 *  - Hash entry nodes:
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
 *	    Unused for integer hash maps.
 *
 *	Next : 32-bit word
 *	    Pointer to next entry in chain.
 *
 *	Key : 32-bit word
 *	    Entry key word.
 *
 *	Value : 32-bit word
 *	    Entry value word.
 *
 *
 *  - Integer hash entry nodes:
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
 *	Hash : 24-bit unsigned
 *	    Higher order bits of the 32-bit hash value for fast negative test.
 *	    Unused for integer hash maps.
 *
 *	Next : 32-bit word
 *	    Pointer to next entry in chain.
 *
 *	Key : 32-bit integer
 *	    Entry key.
 *
 *	Value : 32-bit word
 *	    Entry value word.
 */

/* Hash maps: */
/*
#define WORD_HASHMAP_NBCELLS(word)	3 /* TODO make field?*//*
#define WORD_HASHMAP_HEADER_SIZE	(sizeof(size_t)+sizeof(Col_HashKeyProc *)+sizeof(Col_CompareKeysProc *)+sizeof(Col_Word))
#define WORD_HASHMAP_SIZE(word)		(((size_t *)(word))[2])
#define WORD_HASHMAP_HASHKEY(word)	(((Col_HashKeyProc *)(word))[3])
#define WORD_HASHMAP_COMPAREKEYS(word)	(((Col_CompareKeysProc *)(word))[4])
#define WORD_HASHMAP_BUCKETS(word)	(((Col_Word *)(word))[5])
#define WORD_HASHMAP_STATICBUCKETS_SIZE(word) ((WORD_HASHMAP_NBCELLS(word)*CELL_SIZE-(WORD_HEADER_SIZE+WORD_HASHMAP_HEADER_SIZE))/sizeof(Col_Word))
#define WORD_HASHMAP_STATICBUCKETS(word) ((Col_Word *)((char *)WORD_DATA(word)+WORD_HASHMAP_HEADER_SIZE))
*/

/* Integer hash maps: */
#define WORD_INTHASHMAP_NBCELLS(word)	3 /* TODO make field?*/
#define WORD_INTHASHMAP_HEADER_SIZE	(sizeof(size_t)+sizeof(Col_Word))
#define WORD_INTHASHMAP_SIZE(word)	(((size_t *)(word))[2])
#define WORD_INTHASHMAP_BUCKETS(word)	(((Col_Word *)(word))[3])
#define WORD_INTHASHMAP_STATICBUCKETS_SIZE(word) ((WORD_INTHASHMAP_NBCELLS(word)*CELL_SIZE-(WORD_HEADER_SIZE+WORD_INTHASHMAP_HEADER_SIZE))/sizeof(Col_Word))
#define WORD_INTHASHMAP_STATICBUCKETS(word) ((Col_Word *)((char *)WORD_DATA(word)+WORD_INTHASHMAP_HEADER_SIZE))

#define WORD_INTHASHMAP_INIT(word) \
    WORD_SET_TYPE_ID((word), WORD_TYPE_INTHASHMAP); \
    WORD_INTHASHMAP_SIZE(word) = 0; \
    WORD_INTHASHMAP_BUCKETS(word) = WORD_NIL;

/* Integer hash entries: */
#define WORD_INTHASHENTRY_NEXT(node)	(((Col_Word *)(node))[1])
#define WORD_INTHASHENTRY_KEY(node)	(((int *)(node))[2])
#define WORD_INTHASHENTRY_VALUE(node)	(((Col_Word *)(node))[3])

#define WORD_INTHASHENTRY_INIT(node, next, key, value) \
    WORD_SET_TYPE_ID((node), WORD_TYPE_INTHASHENTRY); \
    WORD_INTHASHENTRY_NEXT(node) = (next); \
    WORD_INTHASHENTRY_KEY(node) = (key); \
    WORD_INTHASHENTRY_VALUE(node) = (value);

/*
 * Trie maps.
 *
 * TODO
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
 *  - Trie map nodes:
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
 * 
 *  - Trie map leaves:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |                    Unused                     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Unused                            |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                              Key                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Value                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Key : 32-bit word
 *	    Key word.
 *
 *	Value : 32-bit word
 *	    Value word.
 * 
 *  - Int trie map leaves:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   Type info   |                    Unused                     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Unused                            |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                              Key                              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             Value                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *	Key : 32-bit integer
 *	    Entry key.
 *
 *	Value : 32-bit word
 *	    Entry value word.
 */

#define WORD_TRIEMAP_SIZE(word)		(((size_t *)(word))[2])
#define WORD_TRIEMAP_ROOT(word)		(((Col_Word *)(word))[3])

#define WORD_INTTRIEMAP_INIT(word) \
    WORD_SET_TYPE_ID((word), WORD_TYPE_INTTRIEMAP); \
    WORD_TRIEMAP_SIZE(word) = 0; \
    WORD_TRIEMAP_ROOT(word) = WORD_NIL;

#define WORD_TRIENODE_MASK(node)	(((uint32_t *)(node))[1])
#define WORD_TRIENODE_LEFT(node)	(((Col_Word *)(node))[2])
#define WORD_TRIENODE_RIGHT(node)	(((Col_Word *)(node))[3])

#define WORD_TRIENODE_INIT(node, mask, left, right) \
    WORD_SET_TYPE_ID((node), WORD_TYPE_TRIENODE); \
    WORD_TRIENODE_MASK(node) = (mask); \
    WORD_TRIENODE_LEFT(node) = (left); \
    WORD_TRIENODE_RIGHT(node) = (right);

/*
#define node_TRIELEAF_KEY(node)		(((Col_node *)(node))[2])
#define node_TRIELEAF_VALUE(node)	(((Col_node *)(node))[3])

#define node_TRIELEAF_INIT(node, key, value) \
    node_SET_TYPE_ID((node), node_TYPE_TRIELEAF); \
    node_TRIELEAF_KEY(node) = (key); \
    node_TRIELEAF_VALUE(node) = (value);
*/

#define WORD_INTTRIELEAF_KEY(node)	(((int *)(node))[2])
#define WORD_INTTRIELEAF_VALUE(node)	(((Col_Word *)(node))[3])

#define WORD_INTTRIELEAF_INIT(node, key, value) \
    WORD_SET_TYPE_ID((node), WORD_TYPE_INTTRIELEAF); \
    WORD_INTTRIELEAF_KEY(node) = (key); \
    WORD_INTTRIELEAF_VALUE(node) = (value);

#endif /* _COLIBRI_INTERNAL */
