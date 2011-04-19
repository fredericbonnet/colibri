/*
 * Ensure WORDS_BIGENDIAN is defined correcly:
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
#	     undef WORDS_BIGENDIAN
#	     define WORDS_BIGENDIAN 1
#	 endif
#    endif
#    ifdef LITTLE_ENDIAN
#	 if BYTE_ORDER == LITTLE_ENDIAN
#	     undef WORDS_BIGENDIAN
#	 endif
#    endif
#endif

/*
 * Swap two values using XOR. Don't use when both are the same lvalue.
 */

#define SWAP(a, b)		(((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
#define SWAP_PTR(a, b)		(((*(int *) &(a)) ^= (*(int *) &(b))), ((*(int *) &(b)) ^= (*(int *) &(a))), ((*(int *) &(a)) ^= (*(int *) &(b))))

/*
 *----------------------------------------------------------------
 * Mark-and-sweep, generational, exact GC.
 *----------------------------------------------------------------
 */

/*
 * Initialization.
 */

void			GCInit(void);

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
 * System page allocator.
 *----------------------------------------------------------------
 */

/* 
 * Allocator page size. System page size should be a multiple of it. 
 */

#define PAGE_SIZE	    1024

/*
 * Cell size. 16 bytes on 32-bit systems.
 */

#define CELL_SIZE	    16

/*
 * Pages are divided into cells. On a 32-bit system with 1024-byte logical pages,
 * each page stores 64 16-byte cells. 
 *
 * Each page has reserved cells that store information about the page. The 
 * remaining cells store rope info. For example, on a 32-bit system, the first 
 * cell reserved and is formatted as follows:
 *
 *  - bytes 0-3:	link to next page in pool.
 *  - byte 4:		flags.
 *  - byte 5:		generation.
 *  - bytes 6-7:	reserved.
 *  - bytes 8-15:	bitmask for allocated cells (64 bits).
 */

#define CELLS_PER_PAGE		(PAGE_SIZE/CELL_SIZE) /* 64 */
#if PAGE_SIZE==1024
#   define RESERVED_CELLS	1	/* 2*32 bits + 64 bits */
#elif PAGE_SIZE==4096
#   define RESERVED_CELLS	3	/* 2*32 bits + 256 bits */
#endif
#define AVAILABLE_CELLS		(CELLS_PER_PAGE-RESERVED_CELLS)
#define NB_CELLS(size)		(((size)+CELL_SIZE-1)/CELL_SIZE)

#define IS_PAGE(page)		((((unsigned int)(page)) % PAGE_SIZE) == 0)

#define PAGE_NEXT(page)		(*(void **)(page))
#define PAGE_FLAGS(page)	(*((unsigned char *)(page)+4))
#define PAGE_GENERATION(page)	(*((unsigned char *)(page)+5))
#define PAGE_RESERVED(page)	(*(unsigned short *)((char *)(page)+6))
#define PAGE_BITMASK(page)	((unsigned char *)(page)+8)
#define PAGE_CELL(page, index)	((void *)((char *)(page)+CELL_SIZE*(index)))

#define CELL_PAGE(cell)		((void *)((unsigned int)(cell) & ~(PAGE_SIZE-1)))
#define CELL_INDEX(cell)	(((unsigned int)(cell) % PAGE_SIZE) / CELL_SIZE)

#define PAGE_FLAG_EMPTY		1

/*
 * Memory pools. Pools are a set of pages that store the ropes of a given 
 * generation.
 */

typedef struct MemoryPool {
    void *pages;		/* Pages form a singly-linked list. */
    void *lastFreePage[AVAILABLE_CELLS]; 
				/* Last page where a cell sequence of a given 
				 * size was found. */
    size_t gc;			/* GC counter. Used for generational GC. */
    int alloc;			/* Page alloc counter since last GC. */
    int generation;		/* Generation level; 0 = younger. */
} MemoryPool;

/* 
 * Initialization. 
 */

void			AllocInit(void);

/*
 * Pool creation. Once created, pools cannot be freed.
 */

MemoryPool *		PoolNew(int generation);

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
 * The lead bit of the type field indicates the rope generation. A 1-bit 
 * generation counter means that ropes get promoted when they survive 2 GCs.
 *
 * ROPE_SET_TYPE also clears the generation counter.
 */

/* Careful: don't give arguments with side effects! */
#define ROPE_TYPE(rope)	\
    ((rope)? \
	 ((rope)[0]?		ROPE_TYPE_C	\
	:((rope)[1]&0x7F))	/* Type ID */   \
    :				ROPE_TYPE_NULL)
#define ROPE_SET_TYPE(rope, type)	(((unsigned char *)(rope))[0] = 0, ((unsigned char *)(rope))[1] = (type))

#define ROPE_GENERATION(rope)		(((unsigned char *)(rope))[1] & 0x80)
#define ROPE_INCR_GENERATION(rope)	(((unsigned char *)(rope))[1] |= 0x80)
#define ROPE_CLEAR_GENERATION(rope)	(((unsigned char *)(rope))[1] &= ~0x80)


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
 */

#define ROPE_UCS_LENGTH(rope)		(*(unsigned short *)((rope)+2))
#define ROPE_UCS_HEADER_SIZE		4
#define ROPE_UCS_DATA(rope)		((const unsigned char *)((rope)+4))
#define ROPE_UCS1_DATA(rope)		((const unsigned char *)((rope)+4))
#define ROPE_UCS2_DATA(rope)		((const unsigned short *)((rope)+4))
#define ROPE_UCS4_DATA(rope)		((const unsigned int *)((rope)+4))

#define ROPE_UTF8_LENGTH(rope)		(*(unsigned short *)((rope)+2))
#define ROPE_UTF8_BYTELENGTH(rope)	(*(unsigned short *)((rope)+4))
#define ROPE_UTF8_HEADER_SIZE		6
#define ROPE_UTF8_DATA(rope)		((const unsigned char *)((rope)+6))

#define ROPE_SUBROPE_DEPTH(rope)	(*(unsigned char *)((rope)+2)) 
#define ROPE_SUBROPE_SOURCE(rope)	(*(Col_Rope *)((rope)+4))
#define ROPE_SUBROPE_FIRST(rope)	(*(size_t *)((rope)+8))
#define ROPE_SUBROPE_LAST(rope)		(*(size_t *)((rope)+12))

#define ROPE_CONCAT_DEPTH(rope)		(*(unsigned char *)((rope)+2))
#define ROPE_CONCAT_LEFT_LENGTH(rope)	(*(unsigned char *)((rope)+3))
#define ROPE_CONCAT_LENGTH(rope)	(*(size_t *)((rope)+4))
#define ROPE_CONCAT_LEFT(rope)		(*(Col_Rope *)((rope)+8))
#define ROPE_CONCAT_RIGHT(rope)		(*(Col_Rope *)((rope)+12))

#define ROPE_CUSTOM_HEADER_SIZE		8
#define ROPE_CUSTOM_SIZE(rope)		(*(unsigned short *)((rope)+2))
#define ROPE_CUSTOM_TYPE(rope)		(*(Col_RopeCustomType **)((rope)+4))
#define ROPE_CUSTOM_DATA(rope)		((void *)((rope)+8))
#define ROPE_CUSTOM_TRAILER_SIZE	4
#define ROPE_CUSTOM_NEXT(rope, size)	(*(Col_Rope *)((rope)+NB_CELLS(ROPE_CUSTOM_HEADER_SIZE+(size)+ROPE_CUSTOM_TRAILER_SIZE)*CELL_SIZE-ROPE_CUSTOM_TRAILER_SIZE))


/*
 *----------------------------------------------------------------
 * Word structures.
 *----------------------------------------------------------------
 */

#define WORD_TYPE_REGULAR		-1

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
 *	P..			  ..P|0000	object pointer (aligned on 16 byte boundaries) including nil and ropes
 *	I..			     ..I|1 	small signed integer (31 bits on 32-bit systems)
 *	S..		     ..S|L....L|10	small string or char (general format)
 *	U..		     ..U|111111|10 	 - Unicode character (L=-1, 24 bits on 32-bit systems)
 *	0..                    0|000000|10	 - empty string
 *	0..            0S    ..S|000001|10	 - 1-char 8-bit string (L=1)
 *	0..  ..0S..          ..S|000010|10	 - 2-char 8-bit string (L=2)
 *	S..		     ..S|000011|10	 - 3-char 8-bit string (L=3)
 *	?..			   .?|0100 	unused
 *	?..			  ..?|1000 	unused
 *	?..			  ..?|1100 	unused
 */

#define WORD_TYPE_NULL			-2
#define WORD_TYPE_SMALL_INT		-3
#define WORD_TYPE_CHAR			-4
#define WORD_TYPE_SMALL_STRING		-5

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
 * architectures, we swap the LSB and MSB to get the same result.
 *
 * Predefined type IDs are chosen so that their bit 1 is zero to 
 * distinguish them with type pointers and avoid value clashes. The ID only
 * takes byte 0, the rest of the word being free to use. Ropes can be seen as
 * a special word type with a zero type ID.
 *
 * We use bit 0 of the type field as a generation indicator for both predefined
 * type IDs and type pointers. Given the above, this bit is always on byte 0. 
 * A 1-bit generation counter means that words get promoted when they survive 
 * 2 GCs.
 *
 * WORD_SET_TYPE_(ID|ADDR) also clears the generation counter.
 */

#define IS_IMMEDIATE(word)	(((unsigned int)(word))&0xF)
#define IS_WORD(ropeOrWord)	(((unsigned char *)(ropeOrWord))[0])

/* Careful: don't give arguments with side effects! */
#define WORD_TYPE(word) \
    ((word)?										\
	IS_IMMEDIATE(word)?								\
	    /* Immediate values */							\
	     (((unsigned int)(word))&1)?		WORD_TYPE_SMALL_INT		\
	    :(((unsigned int)(word))&2)?						\
		 (((int)(word))&0x80)?			WORD_TYPE_CHAR			\
		:					WORD_TYPE_SMALL_STRING		\
	    /* Unknown format */							\
	    :						WORD_TYPE_NULL			\
	/* Pointer or predefined */							\
	:(((unsigned char *)(word))[0]&2)?		WORD_TYPE_REGULAR		\
	:(((unsigned char *)(word))[0]&~3)		/* Type ID (inc. rope) */	\
    :							WORD_TYPE_NULL)

#ifdef WORDS_BIGENDIAN
#   define WORD_GET_TYPE_ADDR(word) \
	((addr) = ((Col_WordType *)(*(unsigned int *)(word)&~3)), SWAP(((char *)&(addr))[0], ((char *)&(addr))[3]))
#   define WORD_SET_TYPE_ADDR(word, addr) \
	(*(unsigned int *)(word) = ((unsigned int)(addr))|2, SWAP(((char *)(word))[0], ((char *)(word))[3]))
#else
#   define WORD_GET_TYPE_ADDR(word, addr) \
	((addr) = ((Col_WordType *)(*(unsigned int *)(word)&~3)))
#   define WORD_SET_TYPE_ADDR(word, addr) \
	(*(unsigned int *)(word) = ((unsigned int)(addr))|2)
#endif
#define WORD_SET_TYPE_ID(word, type)	(((unsigned char *)(word))[0] = (type))
#define WORD_GENERATION(word)		(((unsigned char *)(word))[0] & 1)
#define WORD_INCR_GENERATION(word)	(((unsigned char *)(word))[0] |= 1)
#define WORD_CLEAR_GENERATION(word)	(((unsigned char *)(word))[0] &= ~1)

/*
 * Predefined word types.
 *
 * To respect the aforementioned constraints, values are chosen so that the MSB
 * and LSB are identical, and that their bit 0 and 1 are zero.
 *
 * As ropes have their byte 0 set to zero, a zero type ID indicates that the
 * word is a rope.
 */

#define WORD_TYPE_ROPE			0
#define WORD_TYPE_INT			4
#define WORD_TYPE_STRING		8

/*
 * Immediate value fields.
 *
 * Values are for 32-bit systems.
 */

#define WORD_SMALL_INT_GET(word)	(((int)(word))>>1)
#define WORD_SMALL_INT_NEW(value)	((Col_Word)(((value)<<1)|1))

#define WORD_CHAR_GET(word)		(((unsigned int)(word))>>8)
#define WORD_CHAR_NEW(value)		((Col_Word)(((value)<<8)|0xFE))

#define WORD_SMALL_STRING_GET_LENGTH(value) \
    ((((unsigned int)(value))&0xFC)>>2)
#define WORD_SMALL_STRING_SET_LENGTH(word, length) \
    (*((unsigned int *)&(word)) = ((length)<<2)|2)
#ifdef WORDS_BIGENDIAN
#   define WORD_SMALL_STRING_DATA(word) \
	((unsigned char *)&(word))
#else
#   define WORD_SMALL_STRING_DATA(word) \
	(((unsigned char *)&(word))+1)
#endif
#define WORD_SMALL_STRING_EMPTY		((Col_Word) 2)

/*
 * Word fields.
 *
 * Words have a value, stored in the 2nd machine word of the cell, that can 
 * take any accepted word value: immediate values (inc. NULL), ropes, or other 
 * words. Words can thus form chains of values having different types. All 
 * words in a value chain ultimately have the same value (e.g. a string 
 * representation or an interchange format). This implies that the order of 
 * words in a value chain has no importance.
 *
 * Values are for 32-bit systems.
 */

#define WORD_HEADER_SIZE		8
#define WORD_VALUE(word)		(((Col_Word *)(word))[1])
#define WORD_DATA(word)			((void *)((char *)(word)+8))
#define WORD_TRAILER_SIZE		4
#define WORD_NEXT(word, size)		(*(Col_Word *)((char *)(word)+NB_CELLS(WORD_HEADER_SIZE+(size)+WORD_TRAILER_SIZE)*CELL_SIZE-WORD_TRAILER_SIZE))

#define WORD_INT_DATA(word)		(*(int *) WORD_DATA(word))
#define WORD_STRING_DATA(word)		(*(Col_Rope *) WORD_DATA(word))

/*
 * Roots, references and redirects.
 *
 * They use the same fields for both rope and word versions, only the header
 * field and type ID change.
 *
 * Redirects replace existing ropes during promotion.
 *
 * References are like automatic roots: they are followed from parents in
 * uncollected polls pointing to children in newer pools.
 */

#define ROPE_TYPE_ROOT		0x7F
#define ROPE_TYPE_REDIRECT	0x7E
#define ROPE_TYPE_REF		0x7D

#define WORD_TYPE_ROOT		0xFC
#define WORD_TYPE_REDIRECT	0xF8
#define WORD_TYPE_REF		0xF4

/* Careful: don't give arguments with side effects!  */
#define RESOLVE_ROPE(rope)	\
	{if (ROPE_TYPE((rope)) == ROPE_TYPE_ROOT) {(rope) = ROOT_SOURCE((rope));}}
#define RESOLVE_WORD(word)	\
	{if (WORD_TYPE((word)) == WORD_TYPE_ROOT) {(word) = ROOT_SOURCE((word));}}

#define ROOT_REFCOUNT(ropeOrWord)	(((size_t *)(ropeOrWord))[1])
#define ROOT_SOURCE(ropeOrWord)		(((const void **)(ropeOrWord))[2])
#define ROOT_NEXT(ropeOrWord)		(((const void **)(ropeOrWord))[3])

#define REDIRECT_SOURCE(rope)		ROOT_SOURCE(rope)

#define REF_PARENT(ropeOrWord)		(((const void **)(ropeOrWord))[1])
#define REF_CHILD(ropeOrWord)		(((const void **)(ropeOrWord))[2])
#define REF_NEXT(ropeOrWord)		(((const void **)(ropeOrWord))[3])

