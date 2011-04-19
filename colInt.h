#include "colConf.h"

/*
 * Ensure COL_BIGENDIAN is defined correcly:
 * Needs to happen here in addition to configure to work with fat compiles on
 * Darwin (where configure runs only once for multiple architectures).
 */

#ifdef HAVE_SYS_TYPES_H
#    include <sys/types.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#    include <sys/param.h>
#endif
#ifdef BYTE_ORDER
#    ifdef BIG_ENDIAN
#	 if BYTE_ORDER == BIG_ENDIAN
#	     undef COL_BIGENDIAN
#	     define COL_BIGENDIAN 1
#	 endif
#    endif
#    ifdef LITTLE_ENDIAN
#	 if BYTE_ORDER == LITTLE_ENDIAN
#	     undef COL_BIGENDIAN
#	 endif
#    endif
#endif

#ifdef _DEBUG
#define DO_TRACE
#endif

#ifdef DO_TRACE
#   include <stdio.h>
#   define TRACE(format, ...) (fprintf(stderr, format, __VA_ARGS__), fflush(stderr))
#else
#   define TRACE
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
 * Pages are divided into cells. On a 32-bit system with 1024-byte logical pages,
 * each page stores 64 16-byte cells. 
 *
 * Each page has reserved cells that store information about the page. The 
 * remaining cells store rope info. For example, on a 32-bit system with 
 * 1024-byte pages, the first cell is reserved and is formatted as follows:
 *
 *  - bytes 0-3:	link to next page in pool.
 *  - byte 4:		generation.
 *  - byte 5:		flags.
 *  - bytes 6-7:	reserved.
 *  - bytes 8-15:	bitmask for allocated cells (64 bits).
 */

#define CELLS_PER_PAGE		(PAGE_SIZE/CELL_SIZE)
#if PAGE_SIZE==1024
#   define RESERVED_CELLS	1	/* 2*32 bits + 64 bits */
#elif PAGE_SIZE==4096
#   define RESERVED_CELLS	3	/* 2*32 bits + 256 bits */
#endif
#define AVAILABLE_CELLS		(CELLS_PER_PAGE-RESERVED_CELLS)
#define NB_CELLS(size)		(((size)+CELL_SIZE-1)/CELL_SIZE)

#define PAGE_NEXT(page)		(*(void **)(page))
#define PAGE_GENERATION(page)	(*((unsigned char *)(page)+sizeof(void *)))
#define PAGE_FLAGS(page)	(*((unsigned char *)(page)+sizeof(void *)+1))
#define PAGE_RESERVED(page)	(*(unsigned short *)((char *)(page)+sizeof(void *)+2))
#define PAGE_BITMASK(page)	((unsigned char *)(page)+sizeof(void *)+2+sizeof(unsigned short))
#define PAGE_CELL(page, index)	((void *)((char *)(page)+CELL_SIZE*(index)))

#define CELL_PAGE(cell)		((void *)((uintptr_t)(cell) & ~(PAGE_SIZE-1)))
#define CELL_INDEX(cell)	(((uintptr_t)(cell) % PAGE_SIZE) / CELL_SIZE)

/*
 * Memory pools. Pools are a set of pages that store the ropes of a given 
 * generation.
 */

typedef struct MemoryPool {
    unsigned int generation;	/* Generation level; 0 = younger. */
    void *pages;		/* Pages form a singly-linked list. */
    void *lastFreePage[AVAILABLE_CELLS]; 
				/* Last page where a cell sequence of a given 
				 * size was found. */
    size_t nbPages;		/* Number of pages in pool. */
    size_t nbAlloc;		/* Number of pages alloc'd since last GC. */
    size_t nbSetCells;		/* Number of set cells in pool. */
    size_t gc;			/* GC counter. Used for generational GC. */
    const void *roots, *parents;/* List of roots for each source or parent in
				 * pool. */
    const void *sweepables;	/* List of ropes or words that need sweeping
				 * when unreachable after a GC. */
    Col_ClientData data;	/* Opaque token for system-specific data. */
} MemoryPool;

/* 
 * Initialization. 
 */

void			AllocInit(void);

/*
 * Pool initialization/cleanup.
 */

void			PoolInit(MemoryPool *pool, unsigned int generation);
void			PoolCleanup(MemoryPool *pool);

/*
 * Allocation/deallocation of pool pages.
 */

void			PoolAllocPages(MemoryPool *pool, void **nextPtr);
void			PoolFreePages(MemoryPool *pool);

/*
 * Allocation of cells in pages.
 */

void *			PageAllocCells(size_t number, void *firstPage, 
			    void **tailPtr);

/*
 * Allocation bitmask handling.
 */

void			SetCells(void *page, size_t first, size_t number);
void			ClearCells(void *page, size_t first, size_t number);
void			ClearAllCells(void *page);
int			TestCell(void *page, size_t index);
size_t			NbSetCells(void *page);


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

void *			AllocCells(size_t number);

/*
 * Custom rope and word handling for cleanup.
 */

void			DeclareCustomRope(Col_Rope rope);
void			DeclareWord(Col_Word word);


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
 * The lead bit of the type field indicates parenthood. When set, this means 
 * that the rope has children in youngest generations.
 *
 * ROPE_SET_TYPE also clears the parent flag.
 */

/* Careful: don't give arguments with side effects! */
#define ROPE_TYPE(rope)	\
    ((rope)? \
	 ((rope)[0]?				ROPE_TYPE_C	\
	:((unsigned char)(rope)[1]&0x7F))	/* Type ID */   \
    :						ROPE_TYPE_NULL)
#define ROPE_SET_TYPE(rope, type) (((unsigned char *)(rope))[0] = 0, ((unsigned char *)(rope))[1] = (type))

#define ROPE_PARENT(rope)	(((unsigned char *)(rope))[1] & 0x80)
#define ROPE_SET_PARENT(rope)	(((unsigned char *)(rope))[1] |= 0x80)
#define ROPE_CLEAR_PARENT(rope)	(((unsigned char *)(rope))[1] &= ~0x80)


/* 
 * Rope types. 
 *
 * ROPE_TYPE_EMPTY is set to 0 so that an empty rope equals a C string with
 * two NUL chars, i.e. "\0".
 */

#define ROPE_TYPE_EMPTY		0
#define ROPE_TYPE_UCS1		1
#define ROPE_TYPE_UCS2		2
#define ROPE_TYPE_UCS4		3
#define ROPE_TYPE_UTF8		4
#define ROPE_TYPE_SUBROPE	5
#define ROPE_TYPE_CONCAT	6
#define ROPE_TYPE_CUSTOM	7

#define ROPE_TYPE_NULL		-1
#define ROPE_TYPE_C		-2

/*
 * Rope fields.
 *
 * Values are for 32-bit systems.
 *
 * ROPE_SUBROPE_DEPTH, ROPE_CONCAT_DEPTH: 8 bits will code up to 255 depth 
 * levels, which is more than sufficient for balanced binary trees. 
 *
 * ROPE_CUSTOM_NEXT: custom ropes with a freeProc are singly-linked together
 * using this field, so that unreachable ropes get swept properly upon GC.
 */

#define ROPE_UCS_LENGTH(rope)		(((unsigned short *)(rope))[1])
#define ROPE_UCS_HEADER_SIZE		(sizeof(Col_Char4))
#define ROPE_UCS_DATA(rope)		((const Col_Char1 *)(rope)+ROPE_UCS_HEADER_SIZE)
#define ROPE_UCS1_DATA(rope)		((const Col_Char1 *) ROPE_UCS_DATA(rope))
#define ROPE_UCS2_DATA(rope)		((const Col_Char2 *) ROPE_UCS_DATA(rope))
#define ROPE_UCS4_DATA(rope)		((const Col_Char4 *) ROPE_UCS_DATA(rope))

#define ROPE_UTF8_LENGTH(rope)		(((unsigned short *)(rope))[1])
#define ROPE_UTF8_BYTELENGTH(rope)	(((unsigned short *)(rope))[2])
#define ROPE_UTF8_HEADER_SIZE		(sizeof(short)*3)
#define ROPE_UTF8_DATA(rope)		((const Col_Char1 *)(rope)+ROPE_UTF8_HEADER_SIZE)

#define ROPE_SUBROPE_DEPTH(rope)	(((unsigned char *)(rope))[2]) 
#define ROPE_SUBROPE_SOURCE(rope)	(((Col_Rope *)(rope))[1])
#define ROPE_SUBROPE_FIRST(rope)	(((size_t *)(rope))[2])
#define ROPE_SUBROPE_LAST(rope)		(((size_t *)(rope))[3])

#define ROPE_CONCAT_DEPTH(rope)		(((unsigned char *)(rope))[2])
#define ROPE_CONCAT_LEFT_LENGTH(rope)	(((unsigned char *)(rope))[3])
#define ROPE_CONCAT_LENGTH(rope)	(((size_t *)(rope))[1])
#define ROPE_CONCAT_LEFT(rope)		(((Col_Rope *)(rope))[2])
#define ROPE_CONCAT_RIGHT(rope)		(((Col_Rope *)(rope))[3])

#define ROPE_CUSTOM_HEADER_SIZE		(sizeof(void *)*2)
#define ROPE_CUSTOM_SIZE(rope)		(((unsigned short *)(rope))[1])
#define ROPE_CUSTOM_TYPE(rope)		(((Col_RopeCustomType **)(rope))[1])
#define ROPE_CUSTOM_DATA(rope)		((void *)((rope)+ROPE_CUSTOM_HEADER_SIZE))
#define ROPE_CUSTOM_TRAILER_SIZE	(sizeof(Col_Rope))
#define ROPE_CUSTOM_NEXT(rope, size)	(*(Col_Rope *)((rope)+NB_CELLS(ROPE_CUSTOM_HEADER_SIZE+(size)+ROPE_CUSTOM_TRAILER_SIZE)*CELL_SIZE-ROPE_CUSTOM_TRAILER_SIZE))

/* 
 * Max byte size of leaf ropes. A leaf rope can take no more than the available
 * size of a page. Larger ropes must be built by concatenation of smaller ones. 
 */

#define UCS_MAX_SIZE		(AVAILABLE_CELLS*CELL_SIZE-ROPE_UCS_HEADER_SIZE)
#define UTF8_MAX_SIZE		(AVAILABLE_CELLS*CELL_SIZE-ROPE_UTF8_HEADER_SIZE)
#define CUSTOM_MAX_SIZE		(AVAILABLE_CELLS*CELL_SIZE-ROPE_CUSTOM_HEADER_SIZE)


/*
 *----------------------------------------------------------------
 * Word structures.
 *----------------------------------------------------------------
 */

/*
 * Immediate value types.
 *
 * These types store their value in the pointer value, not in a structure. For
 * example, the NULL pointer represents a nil value. So these type IDs are
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
 *	P..			  ..P|1000 	rope pointer (not including C strings)
 *	I..			     ..I|1 	small signed integer (31 bits on 32-bit systems)
 *	S..		     ..S|L....L|10	small string or char (general format)
 *	U..		     ..U|111111|10 	 - Unicode character (L=-1, 24 bits on 32-bit systems)
 *	0..                    0|000000|10	 - empty string
 *	0..          ..0S..  ..S|000001|10	 - 1-char 8-bit string (L=1)
 *	0..  ..0S..          ..S|000010|10	 - 2-char 8-bit string (L=2)
 *	S..		     ..S|000011|10	 - 3-char 8-bit string (L=3)
 *	?..			  ..?|0100 	unused
 *	?..			  ..?|1100 	unused
 */

#define WORD_TYPE_REGULAR	0

#define IS_ROPE(word)		((((uintptr_t)(word))&0xF) == 8)
#define WORD_TYPE_ROPE		-1

#define IS_IMMEDIATE(word)	(((uintptr_t)(word))&7)
#define WORD_TYPE_NULL		-2
#define WORD_TYPE_SMALL_INT	-3
#define WORD_TYPE_CHAR		-4
#define WORD_TYPE_SMALL_STRING	-5

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
 * takes byte 0, the rest of the word being free to use.
 *
 * We use bit 0 of the type field as a parenthood indicator for both predefined
 * type IDs and type pointers. Given the above, this bit is always on byte 0. 
 * When set, this means that the word has children in youngest generations.
 *
 * WORD_SET_TYPE_(ID|ADDR) also clears the parent flag.
 */

#define CELL_TYPE(cell)		(((unsigned char *)(cell))[0])

/* Careful: don't give arguments with side effects! */
#define WORD_TYPE(word) \
    ((word)?								\
	IS_IMMEDIATE(word)?						\
	    /* Immediate values */					\
	     (((uintptr_t)(word))&1)?		WORD_TYPE_SMALL_INT	\
	    :(((uintptr_t)(word))&2)?					\
		 (((uintptr_t)(word))&0x80)?	WORD_TYPE_CHAR		\
		:				WORD_TYPE_SMALL_STRING	\
	    /* Unknown format */					\
	    :					WORD_TYPE_NULL		\
	:IS_ROPE(word)?				WORD_TYPE_ROPE		\
	/* Pointer or predefined */					\
	:(CELL_TYPE(word)&2)?			WORD_TYPE_REGULAR	\
	:(CELL_TYPE(word)&~3)			/* Type ID */		\
    :						WORD_TYPE_NULL)

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

#define WORD_PARENT(word)	(((unsigned char *)(word))[0] & 1)
#define WORD_SET_PARENT(word)	(((unsigned char *)(word))[0] |= 1)
#define WORD_CLEAR_PARENT(word)	(((unsigned char *)(word))[0] &= ~1)

/*
 * Predefined word types.
 *
 * To respect the aforementioned constraints, values are chosen so that their 
 * 2 lsbs are zero.
 */

#define WORD_TYPE_INT		4
#define WORD_TYPE_STRING	8
#define WORD_TYPE_VECTOR	12
#define WORD_TYPE_LIST		16
#define WORD_TYPE_SUBLIST	20
#define WORD_TYPE_CONCATLIST	24

/*
 * Immediate & rope value fields.
 *
 * Values are for 32-bit systems.
 */

#define WORD_ROPE_GET(word)		((Col_Rope)(((uintptr_t)(word))&~0xF))
#define WORD_ROPE_NEW(value)		((Col_Word)(((uintptr_t)(value))|8))

#define WORD_SMALL_INT_GET(word)	(((int)(intptr_t)(word))>>1)
#define WORD_SMALL_INT_NEW(value)	((Col_Word)(intptr_t)((((int)(value))<<1)|1))

#define WORD_CHAR_GET(word)		((Col_Char)(((uintptr_t)(word))>>8))
#define WORD_CHAR_NEW(value)		((Col_Word)((((uintptr_t)(value))<<8)|0xFE))

#define WORD_SMALL_STRING_GET_LENGTH(value) ((((uintptr_t)(value))&0xFC)>>2)
#define WORD_SMALL_STRING_SET_LENGTH(word, length) (*((uintptr_t *)&(word)) = ((length)<<2)|2)
#ifdef COL_BIGENDIAN
#   define WORD_SMALL_STRING_DATA(word)	((Col_Char1  *)&(word))
#else
#   define WORD_SMALL_STRING_DATA(word)	(((Col_Char1 *)&(word))+1)
#endif
#define WORD_SMALL_STRING_EMPTY		((Col_Word) 2)

/*
 * Word fields.
 *
 * Values are for 32-bit systems.
 *
 * WORD_SYNONYM: words may have synonyms that can take any accepted word value:
 * immediate values (inc. NULL), ropes, or other words. Words can thus be part 
 * of chains of synonyms having different types, but with semantically identical
 * values. Such chains form a circular linked list using this field. The order 
 * of words in a synonym chain has no importance.
 *
 * WORD_NEXT: words with a freeProc are singly-linked together using this field,
 * so that unreachable words get swept properly upon GC.
 */

#define WORD_HEADER_SIZE	(sizeof(void *)*2)
#define WORD_SYNONYM(word)	(((Col_Word *)(word))[1])
#define WORD_DATA(word)		((void *)((char *)(word)+WORD_HEADER_SIZE))
#define WORD_TRAILER_SIZE	(sizeof(Col_Word))
#define WORD_NEXT(word, size)	(*(Col_Word *)((char *)(word)+NB_CELLS(WORD_HEADER_SIZE+(size)+WORD_TRAILER_SIZE)*CELL_SIZE-WORD_TRAILER_SIZE))

#define WORD_INT_DATA(word)	(*(int *) WORD_DATA(word))
#define WORD_STRING_DATA(word)	(*(Col_Rope *) WORD_DATA(word))

/* 
 * Max byte size of word data. A word can take no more than the available size 
 * of a page. Larger words must be built by concatenation of smaller ones. 
 */

#define WORD_MAX_SIZE		(AVAILABLE_CELLS*CELL_SIZE-WORD_HEADER_SIZE)

/*
 *----------------------------------------------------------------
 * Roots and redirects.
 *----------------------------------------------------------------
 */

/*
 * They use the same fields for both rope and word versions, only the header
 * field and type ID change.
 *
 * Roots are explicitly preserved ropes or words, using a reference count.
 *
 * Parent roots are like automatic roots: they point to parents with children 
 * in newer generations.
 *
 * Redirects replace existing ropes during promotion.
 *
 */

#define ROPE_TYPE_ROOT		0x7F
#define ROPE_TYPE_PARENT	0x7E
#define ROPE_TYPE_REDIRECT	0x7D

#define WORD_TYPE_ROOT		0xFC
#define WORD_TYPE_PARENT	0xF8
#define WORD_TYPE_REDIRECT	0xF4

/* Careful: don't give arguments with side effects!  */
#define RESOLVE_ROPE(rope) \
    {if (ROPE_TYPE((rope)) == ROPE_TYPE_ROOT) {(rope) = ROOT_SOURCE((rope));}}
#define RESOLVE_WORD(word) \
    {if (WORD_TYPE((word)) == WORD_TYPE_ROOT) {(word) = ROOT_SOURCE((word));}}

#define ROOT_REFCOUNT(cell)	(((size_t *)(cell))[1])
#define ROOT_SOURCE(cell)	(((const void **)(cell))[2])
#define ROOT_NEXT(cell)		(((const void **)(cell))[3])

#define PARENT_CELL(cell)	(((const void **)(cell))[1])
#define PARENT_NEXT(cell)	(((const void **)(cell))[3])

#define REDIRECT_SOURCE(rope)	ROOT_SOURCE(rope)


/*
 *----------------------------------------------------------------
 * Vectors and lists.
 *----------------------------------------------------------------
 */

/* 
 * Max length of vectors words. A vector word can take no more than the 
 * available size of a page. Larger datasets must use tree-based lists.
 */

#define VECTOR_MAX_LENGTH	(WORD_MAX_SIZE/sizeof(Col_Word))

/*
 * Vector fields.
 */

#define WORD_VECTOR_FLAGS(word)		(((unsigned char *)(word))[1])
#define WORD_VECTOR_LENGTH(word)	(((unsigned short *)(word))[1])
#define WORD_VECTOR_ELEMENTS(word)	((Col_Word *) WORD_DATA(word))

/*
 * List fields.
 *
 * List nodes and sublists are not regular words in the sense that they have no 
 * synonym field. Instead they store their size. They are similar to concat 
 * resp. substring ropes and use the same format. They are never used 
 * independently from list words.
 */

#define WORD_LIST_LENGTH(word)		(((size_t *)(word))[2])
#define WORD_LIST_ROOT(word)		(((Col_Word *)(word))[3])

#define WORD_SUBLIST_DEPTH(word)	(((unsigned char *)(word))[1]) 
#define WORD_SUBLIST_SOURCE(word)	(((Col_Word *)(word))[1])
#define WORD_SUBLIST_FIRST(word)	(((size_t *)(word))[2])
#define WORD_SUBLIST_LAST(word)		(((size_t *)(word))[3])

#define WORD_CONCATLIST_DEPTH(word)	(((unsigned char *)(word))[1])
#define WORD_CONCATLIST_LEFT_LENGTH(word) (((unsigned short *)(word))[1])
#define WORD_CONCATLIST_LENGTH(word)	(((size_t *)(word))[1])
#define WORD_CONCATLIST_LEFT(word)	(((Col_Word *)(word))[2])
#define WORD_CONCATLIST_RIGHT(word)	(((Col_Word *)(word))[3])
