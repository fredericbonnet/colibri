#include "colibri.h"
#include "colInternal.h"
#include "colPlatform.h"

#include <stdlib.h>
#include <string.h>

/*
 * System page size. Should be a multiple of PAGE_SIZE on every possible 
 * platform.
 */

size_t systemPageSize;


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

static size_t		FindFreeCells(void *page, size_t number);


/*
 *---------------------------------------------------------------------------
 *
 * AllocInit --
 *
 *	Initialize the memory allocator.
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
AllocInit() {
    PlatAllocInit();
}

/*
 *---------------------------------------------------------------------------
 *
 * PoolInit --
 * PoolCleanup --
 *
 *	Initialize/cleanup memory pool.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Structure is initialized or cleaned up.
 *
 *---------------------------------------------------------------------------
 */

void
PoolInit(
    MemoryPool *pool,		/* The pool to initialize. */
    unsigned int generation)	/* Generation number; 0 = youngest. */
{
    memset(pool, 0, sizeof(pool));
    pool->generation = generation;
}

void
PoolCleanup(
    MemoryPool *pool)		/* The pool to cleanup. */
{
    Page *base, *next;

    /* 
     * Free all system pages.
     */

    for (base = pool->pages; base; base = next) {
	next = LAST_PAGE_NEXT(base);
	PlatSysPageFree(pool, base);
    }
    PlatSysPageCleanup(pool);
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
    Page **nextPtr)		/* The page after which to insert the new one. */
{
    Page *base, *page;
    const size_t nbPagesPerSysPage = systemPageSize/PAGE_SIZE;
    size_t i;

    /* 
     * Allocate NB_SYSPAGE_ALLOC system pages. 
     */

    for (i = 0; i < NB_SYSPAGE_ALLOC; i++) {
	base = (Page *) PlatSysPageAlloc(pool);

	pool->nbPages += nbPagesPerSysPage;
	pool->nbAlloc += nbPagesPerSysPage;
	pool->nbSetCells += RESERVED_CELLS*nbPagesPerSysPage;

	/* 
	 * Initialize pages. 
	 */

	for (i = 0, page = base; i < nbPagesPerSysPage; i++, page++) {
	    /* 
	     * Pages are linked in order.
	     */

	    *nextPtr = page;
	    nextPtr = &PAGE_NEXT(page);

	    /* Flags. */
	    PAGE_FLAGS(page) = 0;
	    PAGE_GENERATION(page) = (unsigned char) pool->generation;

	    /* Initialize bit mask for allocated cells. */
	    ClearAllCells(page);
	}

	*nextPtr = NULL;
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
    Page *page, *base, *prev, *next;
    const size_t nbPagesPerSysPage = systemPageSize/PAGE_SIZE;
    size_t nbSetCells;
    size_t i;

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
	for (i = 0, page = base; i < nbPagesPerSysPage; i++, page++) {
	    nbSetCells += NbSetCells(page);
	}
	if (nbSetCells > RESERVED_CELLS * (systemPageSize/PAGE_SIZE)) {
	    /*
	     * At least one page contains allocated cells.
	     */

	    prev = base;
	    pool->nbPages += nbPagesPerSysPage;
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

	    PlatSysPageFree(pool, base);
	}
    }

    /* 
     * Cleanup the pool. 
     */

    PlatSysPageCleanup(pool);
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

Cell * 
PageAllocCells(
    size_t number,		/* Number of cells to allocate. */
    Page *firstPage,		/* First page to traverse. */
    Page **tailPtr)		/* Upon return, pointer to tail for fast 
				 * insertion of new pages. */
{
    Page *page;
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

	    return (Cell *) page + first;
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
	/*
	 * Too large.
	 */

	Col_Error(COL_ERROR, 
	    "Cannot allocate more than %u cells (asking for %d)", 
	    AVAILABLE_CELLS, number);
	return (size_t)-1;
    }

    /* 
     * Iterate over bytes of the bitmask to find a chain of <number> zero bits. 
     */

    remaining = number;
    for (i = 0; i < (CELLS_PER_PAGE>>3); i++) {
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
    Page *page,			/* The page. */
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
    Page *page,			/* The page. */
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
    Page *page)			/* The page for which to clear all cells. */
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
    Page *page,			/* The page. */
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
    Page *page)			/* The page. */
{
    unsigned char *mask = PAGE_BITMASK(page);
    size_t i, nb = 0;
    for (i=0; i < (CELLS_PER_PAGE>>3); i++) {
	nb += nbBitsSet[mask[i]];
    }
    return nb;
}
