#include "colibri.h"
#include "colInt.h"

#ifdef WIN32
#   include <windows.h>
#endif

/*
 * System page size. Should be a multiple of PAGE_SIZE on every possible 
 * platform.
 */

static size_t systemPageSize;

#ifdef WIN32
/*
 * Windows-specific memory pool, i.e a MemoryPool augmented with system-specific
 * data.
 *
 * Memory is allocated with VirtualAlloc. As it can't reserve less than 
 * SYSTEM_INFO.dwAllocationGranularity (65536 on IA32) but can commit individual
 * pages with a size of SYSTEM_INFO.dwPageSize (4096 on IA32), this means that 
 * each logical page is part of a system page that is itself part of a larger
 * address range. Allocating whole SYSTEM_INFO.dwAllocationGranularity sized
 * pages would be too heavy.
 *
 * In order to keep track of committed pages in ranges, use the PAGE_RESERVED 
 * field of the first logical page in each system page to store the index of 
 * the next committed system page in range, forming a circular list (last one 
 * points to first). Indices must be consecutive. That way we can scan a range 
 * for holes from any of its page.
 *
 * An alternative was to use VirtualQuery to get the committed and free pages 
 * in a range, but this proved to be too CPU expensive, and VirtualQuery is
 * known to be much slower on Win64.
 */

static SYSTEM_INFO systemInfo;
static size_t pagesPerRange;

#define PHYS_PAGE(range, index)	\
    ((void *)((char *)(range)+systemInfo.dwPageSize*(index)))
#define PHYS_PAGE_RANGE(page)	\
    ((void *)((unsigned int)(page) & ~(systemInfo.dwAllocationGranularity-1)))
#define PHYS_PAGE_INDEX(page)	\
    ((unsigned short)(((unsigned int)(page) % systemInfo.dwAllocationGranularity) / systemInfo.dwPageSize))
#define PHYS_PAGE_NEXT(page)	PAGE_RESERVED(page)

typedef struct WinMemoryPool {
    MemoryPool pool;		/* Must be first for polymorphism. */
    void *lastFreeRange;	/* Last page belonging to a range with free
				 * pages. */
} WinMemoryPool;

#endif /* WIN32 */

/*
 * Logical page after the last one in system page.
 */

#define LAST_PAGE_NEXT(page)	\
    PAGE_NEXT(((char *)(page) + systemPageSize - PAGE_SIZE))


/* 
 *----------------------------------------------------------------
 * Lookup tables for bitmasks. 
 *----------------------------------------------------------------
 */

/* 
 * Position of the first zero bit sequence of a given length in byte.
 */

static const char firstZeroBitSequence[7][256] = {
    { /* single bit */
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0,  4, 
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0,  5, 
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0,  4, 
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0,  6, 
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0,  4, 
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0,  5, 
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0,  4, 
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0,  7, 
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0,  4, 
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0,  5, 
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0,  4, 
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0,  6, 
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0,  4, 
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0,  5, 
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0,  4, 
	 0,  1,  0,  2,  0,  1,  0,  3,  0,  1,  0,  2,  0,  1,  0, -1
    },
    { /*  2-bit sequence */
	 0,  1,  2,  2,  0,  3,  3,  3,  0,  1,  4,  4,  0,  4,  4,  4, 
	 0,  1,  2,  2,  0,  5,  5,  5,  0,  1,  5,  5,  0,  5,  5,  5, 
	 0,  1,  2,  2,  0,  3,  3,  3,  0,  1,  6,  6,  0,  6,  6,  6, 
	 0,  1,  2,  2,  0,  6,  6,  6,  0,  1,  6,  6,  0,  6,  6,  6, 
	 0,  1,  2,  2,  0,  3,  3,  3,  0,  1,  4,  4,  0,  4,  4,  4, 
	 0,  1,  2,  2,  0, -1, -1, -1,  0,  1, -1, -1,  0, -1, -1, -1, 
	 0,  1,  2,  2,  0,  3,  3,  3,  0,  1, -1, -1,  0, -1, -1, -1, 
	 0,  1,  2,  2,  0, -1, -1, -1,  0,  1, -1, -1,  0, -1, -1, -1, 
	 0,  1,  2,  2,  0,  3,  3,  3,  0,  1,  4,  4,  0,  4,  4,  4, 
	 0,  1,  2,  2,  0,  5,  5,  5,  0,  1,  5,  5,  0,  5,  5,  5, 
	 0,  1,  2,  2,  0,  3,  3,  3,  0,  1, -1, -1,  0, -1, -1, -1, 
	 0,  1,  2,  2,  0, -1, -1, -1,  0,  1, -1, -1,  0, -1, -1, -1, 
	 0,  1,  2,  2,  0,  3,  3,  3,  0,  1,  4,  4,  0,  4,  4,  4, 
	 0,  1,  2,  2,  0, -1, -1, -1,  0,  1, -1, -1,  0, -1, -1, -1, 
	 0,  1,  2,  2,  0,  3,  3,  3,  0,  1, -1, -1,  0, -1, -1, -1, 
	 0,  1,  2,  2,  0, -1, -1, -1,  0,  1, -1, -1,  0, -1, -1, -1
    },
    { /*  3-bit sequence */
	 0,  1,  2,  2,  3,  3,  3,  3,  0,  4,  4,  4,  4,  4,  4,  4, 
	 0,  1,  5,  5,  5,  5,  5,  5,  0,  5,  5,  5,  5,  5,  5,  5, 
	 0,  1,  2,  2, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1,  2,  2,  3,  3,  3,  3,  0, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1,  2,  2, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1,  2,  2,  3,  3,  3,  3,  0,  4,  4,  4,  4,  4,  4,  4, 
	 0,  1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1,  2,  2, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1,  2,  2,  3,  3,  3,  3,  0, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1,  2,  2, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1
    },
    { /*  4-bit sequence */
	 0,  1,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4, 
	 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1,  2,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1,  2,  2,  3,  3,  3,  3, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1,  2,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    },
    { /*  5-bit sequence */
	 0,  1,  2,  2,  3,  3,  3,  3, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1,  2,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    },
    { /*  6-bit sequence */
	 0,  1,  2,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    },
    { /*  7-bit sequence */
	 0,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    }
    /* 8-bit need no table, just test for zero byte */
};

/* 
 * Longest leading zero bit sequence in byte.
 */

static const char longestLeadingZeroBitSequence[256] = {
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 *  Number of bits set in byte.
 */

static const char nbBitsSet[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

/*
 * Prototypes for functions used only in this file.
 */

static void *		SysPageAlloc(MemoryPool * pool);
static void		SysPageFree(MemoryPool * pool, void * page);
static void		SysPageCleanup(MemoryPool * pool);
static size_t		FindFreeCells(void *page, size_t number);


/*
 *---------------------------------------------------------------------------
 *
 * AllocInit --
 *
 *	Initialize the system-specific memory allocator.
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
AllocInit() {
#ifdef WIN32
    GetSystemInfo(&systemInfo);
    systemPageSize = systemInfo.dwPageSize;
    pagesPerRange = systemInfo.dwAllocationGranularity/systemInfo.dwPageSize;
#endif /* WIN32 */
}

/*
 *---------------------------------------------------------------------------
 *
 * SysPageAlloc --
 *
 *	Allocate a system page.
 *
 * Results:
 *	The new system page.
 *
 * Side effects:
 *	See code (system-dependent).
 *
 *---------------------------------------------------------------------------
 */

static void * 
SysPageAlloc(
    MemoryPool * pool)		/* The page for which to alloc page. */
{
#ifdef WIN32
    WinMemoryPool * winPool = (WinMemoryPool *)pool;
    void *base, *page;

    /* 
     * Look for an address range with a free page. 
     */

    for (page = winPool->lastFreeRange; page; page = LAST_PAGE_NEXT(page)) {
	unsigned short index, prev, next;

	/* 
	 * Search hole in page index list. 
	 */

	base = PHYS_PAGE_RANGE(page);
	index = PHYS_PAGE_INDEX(page);
	next = PHYS_PAGE_NEXT(page);
	prev = index;
	if (next == index) {
	    /* 
	     * Page is alone in range, allocate next one. 
	     */

	    next = (prev+1)%pagesPerRange;
	} else {
	    /*
	     * Search next hole if any by comparing indices, which must be
	     * consecutive.
	     */

	    do {
		if ((next-prev)%pagesPerRange > 1) {
		    /* 
		     * Hole found. 
		     */

		    next = (prev+1)%pagesPerRange;
		    break;
		}
		prev = next;
		next = PHYS_PAGE_NEXT(PHYS_PAGE(base, prev));
	    } while (next != index);
	}
	if (next != index) {
	    /* 
	     * Hole found, commit page.
	     */

	    page = PHYS_PAGE(base, next);
	    VirtualAlloc(page, systemInfo.dwPageSize, MEM_COMMIT, 
		    PAGE_READWRITE);

	    /* 
	     * Insert into list after <index>.
	     */

	    PHYS_PAGE_NEXT(page) = PHYS_PAGE_NEXT(PHYS_PAGE(base, prev));
	    PHYS_PAGE_NEXT(PHYS_PAGE(base, prev)) = next;

	    break;
	}
    }

    if (!page) {
	/* 
	 * No room in existing ranges, reserve new one and allocate first page. 
	 */

	base = VirtualAlloc(NULL, systemInfo.dwAllocationGranularity, 
		MEM_RESERVE, PAGE_READWRITE);
	page = VirtualAlloc(base, systemInfo.dwPageSize, MEM_COMMIT, 
		PAGE_READWRITE);
	PHYS_PAGE_NEXT(page) = 0;
    }

    winPool->lastFreeRange = page;
    return page;
#endif /* WIN32 */
}

/*
 *---------------------------------------------------------------------------
 *
 * SysPageFree --
 *
 *	Free a system page.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	See code (system-dependent).
 *
 *---------------------------------------------------------------------------
 */

static void 
SysPageFree(
    MemoryPool * pool,		/* The pool the page belongs to. */
    void * page)		/* The page to free. */
{
#ifdef WIN32
    unsigned short index = PHYS_PAGE_INDEX(page);
    if (PHYS_PAGE_NEXT(page) == index) {
	/* 
	 * Last and only page in range, release whole range. 
	 */

	VirtualFree(PHYS_PAGE_RANGE(page), 0, MEM_RELEASE);
    } else {
	/* 
	 * Unlink and decommit first page. 
	 */

	void *base = PHYS_PAGE_RANGE(page);
	unsigned short prev, next;
	for (prev = index, next = PHYS_PAGE_NEXT(PHYS_PAGE(base, prev)); 
		next != index; prev = next, 
		next = PHYS_PAGE_NEXT(PHYS_PAGE(base, next)));
	PHYS_PAGE_NEXT(PHYS_PAGE(base, prev)) = PHYS_PAGE_NEXT(page);
	VirtualFree(page, systemInfo.dwPageSize, MEM_DECOMMIT);
    }
#endif
}

/*
 *---------------------------------------------------------------------------
 *
 * SysPageCleanup --
 *
 *	Final cleanup page after pages have been freed in pool.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	See code (system-dependent).
 *
 *---------------------------------------------------------------------------
 */

static void 
SysPageCleanup(MemoryPool * pool) {
#ifdef WIN32
    WinMemoryPool * winPool = (WinMemoryPool *)pool;

    winPool->lastFreeRange = pool->pages;
#endif /* WIN32 */
}

/*
 *---------------------------------------------------------------------------
 *
 * PoolNew --
 *
 *	Allocate memory pool.
 *
 * Results:
 *	The newly allocated pool.
 *
 * Side effects:
 *	Memory allocated and initialized.
 *
 *---------------------------------------------------------------------------
 */

MemoryPool * 
PoolNew(
    unsigned int generation)	/* Generation number; 0 = youngest. */
{
#ifdef WIN32
    WinMemoryPool * winPool = (WinMemoryPool *) malloc(sizeof(*winPool));
    MemoryPool * pool = (MemoryPool *) winPool;

    memset(winPool, 0, sizeof(*winPool));
#endif

    pool->generation = generation;

    return pool;
}

/*
 *---------------------------------------------------------------------------
 *
 * PoolAllocPages --
 *
 *	Allocate pages in pool. Pages are inserted after the given page. This
 *	guarantees better performances by avoiding the traversal of previous
 *	pages.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory allocated and initialized.
 *
 *---------------------------------------------------------------------------
 */

void 
PoolAllocPages(
    MemoryPool *pool,		/* The pool to alloc page into. */
    void **nextPtr)		/* The page after which to insert the new one. */
{
    char *base, *page, *next;

    /* 
     * Allocate system page. 
     */

    base = SysPageAlloc(pool);

    pool->nbPages += systemPageSize/PAGE_SIZE;
    pool->nbAlloc += systemPageSize/PAGE_SIZE;
    pool->nbSetCells += RESERVED_CELLS*systemPageSize/PAGE_SIZE;

    /*
     * Insert in list.
     */

    *nextPtr = base;

    /* 
     * Initialize pages. 
     */

    for (page = base; page < base + systemPageSize; page += PAGE_SIZE) {
	/* 
	 * Pages are linked in order.
	 */

	next = page + PAGE_SIZE;
	if (next == base + systemPageSize) {
	    next = NULL;
	}
	PAGE_NEXT(page) = next;

	/* Flags. */
	PAGE_FLAGS(page) = 0;
	PAGE_GENERATION(page) = (unsigned char) pool->generation;

	/* Initialize bit mask for allocated cells. */
	ClearAllCells(page);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * PoolFreePages --
 *
 *	Free system pages in pool. Refresh page count in the process.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory deallocated.
 *
 *---------------------------------------------------------------------------
 */

void 
PoolFreePages(
    MemoryPool *pool)		/* The pool with pages to free. */
{
    char *page, *base, *prev, *next;
    size_t nbSetCells;

    pool->nbPages = 0;
    pool->nbSetCells = 0;

    /* 
     * Iterate over system pages... 
     */

    for (prev = NULL, base = pool->pages; base; base = next) {
	next = LAST_PAGE_NEXT(base);

	/* 
	 * ... then over logical pages within the system page. 
	 */

	nbSetCells = 0;
	for (page = base; page < base + systemPageSize; page += PAGE_SIZE) {
	    nbSetCells += NbSetCells(page);
	}
	if (nbSetCells > RESERVED_CELLS * (systemPageSize/PAGE_SIZE)) {
	    /*
	     * At least one page contains allocated cells.
	     */

	    prev = base;
	    pool->nbPages += systemPageSize/PAGE_SIZE;
	    pool->nbSetCells += nbSetCells;
	} else {
	    /* 
	     * All logical pages in system page are empty. 
	     */

	    if (prev) {
		/* 
		 * Middle of list. 
		 */

		LAST_PAGE_NEXT(prev) = next;
	    } else {
		/* 
		 * Head of list. 
		 */

		pool->pages = next;
	    }

	    /* 
	     * Free the system page. 
	     */

	    SysPageFree(pool, base);
	}
    }

    /* 
     * Cleanup the pool. 
     */

    SysPageCleanup(pool);
}

/*
 *---------------------------------------------------------------------------
 *
 * PageAllocCells --
 *
 *	Allocate cells in page or in next pages.
 *
 * Results:
 *	First allocated cell, or NULL if failure. In the latter case, tailPtr
 *	will point to the list tail.
 *
 * Side effects:
 *	Page bitmask may be modified.
 *
 *---------------------------------------------------------------------------
 */

void * 
PageAllocCells(
    size_t number,		/* Number of cells to allocate. */
    void *firstPage,		/* First page to traverse. */
    void **tailPtr)		/* Upon return, pointer to tail for fast 
				 * insertion of new pages. */
{
    void *page;
    size_t first;

    for (page = firstPage; page; page = PAGE_NEXT(page)) {
	first = FindFreeCells(page, number);
	if (first != (size_t)-1) {
	    /* 
	     * Mark cells. 
	     */

	    SetCells(page, first, number);

	    /* 
	     * Return address of first cell. 
	     */

	    return (char * ) page + (first * CELL_SIZE);
	}
	if (PAGE_NEXT(page) == NULL) {
	    *tailPtr = page;
	}
    }

    /* 
     * No room found. 
     */

    return NULL;
}

/*
 *---------------------------------------------------------------------------
 *
 * FindFreeCells --
 *
 *	Find sequence of free cells in page. 
 *
 * Results:
 *	Index of first cell of sequence, or -1 if none found.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

static size_t 
FindFreeCells(
    void *page,			/* Page to look into. */
    size_t number)		/* Number of cells to look for. */
{
    size_t i, first = (size_t)-1, remaining;
    char seq;
    unsigned char *mask = PAGE_BITMASK(page);

    /* 
     * Sanity checks. 
     */

    if (number > AVAILABLE_CELLS) {
	/*TODO: exception*/ 
	return 0;
    }

    /* 
     * Iterate over bytes of the bitmask to find a chain of <number> zero bits. 
     */

    remaining = number;
    for (i = 0; i < CELLS_PER_PAGE>>3; i++) {
	if (remaining <= 7) {
	    /* 
	     * End of sequence, or whole sequence for number < 8. 
	     */

	    /* 
	     * Find sequence in byte. 
	     */

	    seq = firstZeroBitSequence[remaining-1][mask[i]];
	    if (seq !=-1) {
		if (remaining == number) {
		    /* 
		     * Sequence fits into one byte. 
		     */

		    return (i<<3) + seq;
		}

		/* 
		 * Sequence spans over several bytes. 
		 */

		if (seq == 0) {
		    /* 
		     * Sequence is contiguous. 
		     */

		    return first;
		}
	    }

	    /* 
	     * Sequence interrupted, restart. 
	     */

	    remaining = number;
	}

	/* 
	 * Sequence may span over several bytes. 
	 */

	if (remaining == number) {
	    /* 
	     * Beginning of sequence. 
	     *
	     * Note: leading bits actually denote trailing cells: bit 0 (LSB) is
	     * first cell, bit 7 (MSB) is last.
	     */

	    seq = longestLeadingZeroBitSequence[mask[i]];
	    if (seq) {
		first = (i<<3) + (8-seq);
		remaining -= seq;
	    }
	} else {
	    /* 
	     * Middle of sequence, look for 8 bits cleared = zero byte. 
	     */

	    if (mask[i] == 0) {
		/* 
		 * Continue sequence. 
		 */

		remaining -= 8;
	    } else {
		/* 
		 * Sequence interrupted, restart. 
		 */

		remaining = number;
	    }
	}
	if (remaining == 0) {
	    /*
	     * End of sequence reached.
	     */

	    return first;
	}
    }

    /*
     * None found.
     */

    return (size_t)-1;
}

/*
 *---------------------------------------------------------------------------
 *
 * SetCells --
 * ClearCells --
 * ClearAllCells --
 *
 *	Set or clear a page bitmask.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Bitmask altered.
 *
 *---------------------------------------------------------------------------
 */

void 
SetCells(
    void *page,			/* The page. */
    size_t first,		/* Index of first cell. */
    size_t number)		/* Number of cells in sequence. */
{
    unsigned char *mask = PAGE_BITMASK(page);
    size_t i;
    for (i=first; i < first+number; i++) {
	mask[i>>3] |= 1<<(i&7);
    }
}

void 
ClearCells(
    void *page,			/* The page. */
    size_t first,		/* Index of first cell. */
    size_t number)		/* Number of cells in sequence. */
{
    unsigned char *mask = PAGE_BITMASK(page);
    size_t i;
    for (i=first; i < first+number; i++) {
	mask[i>>3] &= ~(1<<(i&7));
    }
}

void 
ClearAllCells(
    void *page)			/* The page for which to clear all cells. */
{
    unsigned char *mask = PAGE_BITMASK(page);
    size_t i;
    for (i=0; i < (CELLS_PER_PAGE>>3); i++) {
	mask[i] = 0;
    }

    /*
     * Don't clear reserved cells.
     */

    SetCells(page, 0, RESERVED_CELLS);
}

/*
 *---------------------------------------------------------------------------
 *
 * TestCell --
 *
 *	Test a cell page bitmask.
 *
 * Results:
 *	Whether the cell is set.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

int 
TestCell(
    void *page,			/* The page. */
    size_t index)		/* Index of cell to set. */
{
    unsigned char *mask = PAGE_BITMASK(page);
    return mask[index>>3] & (1<<(index&7));
}

/*
 *---------------------------------------------------------------------------
 *
 * NbSetCells --
 *
 *	Get the number of cells set in a page.
 *
 * Results:
 *	Number of set cells.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

size_t
NbSetCells(
    void *page)			/* The page. */
{
    unsigned char *mask = PAGE_BITMASK(page);
    size_t i, nb = 0;
    for (i=0; i < (CELLS_PER_PAGE>>3); i++) {
	nb += nbBitsSet[mask[i]];
    }
    return nb;
}
