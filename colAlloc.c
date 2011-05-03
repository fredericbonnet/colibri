/*
 * Internal File: colAlloc.c
 *
 *	This file implements the memory allocation features of Colibri.
 */

#include "colibri.h"
#include "colInternal.h"
#include "colPlatform.h"

#include <stdlib.h>
#include <string.h>

/*
 * Prototypes for functions used only in this file.
 */

static Cell *		PageAllocCells(size_t number, Cell *firstCell);
static size_t		FindFreeCells(void *page, size_t number, size_t index);


/****************************************************************************
 * Internal Group: Bit Handling
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Variable: firstZeroBitSequence
 *
 *	Position of the first zero-bit sequence of a given length in byte.
 *	First index is length of zero-bit sequence to look for, minus 1. Second 
 *	index is value of byte in which to search. Result is index of the first 
 *	bit in matching zero-bit sequence, or -1 if none.
 *---------------------------------------------------------------------------*/

const char firstZeroBitSequence[7][256] = {
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
    /* 8-bit sequences need no table, just test for zero byte */
};

/*---------------------------------------------------------------------------
 * Internal Variable: longestLeadingZeroBitSequence
 *
 *	Longest leading zero-bit sequence in byte. Index is value of byte.
 *	Result is number of consecutive cleared bytes starting at MSB.
 *---------------------------------------------------------------------------*/

const char longestLeadingZeroBitSequence[256] = {
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

/*---------------------------------------------------------------------------
 * Internal Variable: nbBitsSet
 *
 *	Number of bits set in byte. Index is value of byte. Result is number of
 *	set bits.
 *---------------------------------------------------------------------------*/

const char nbBitsSet[256] = {
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


/****************************************************************************
 * Internal Group: Memory Pools
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Function: PoolInit
 *
 *	Initialize memory pool.
 *
 * Arguments:
 *	pool		- Pool to initialize.
 *	generation	- Generation number; 0 = youngest.
 *---------------------------------------------------------------------------*/

void
PoolInit(
    MemoryPool *pool,
    unsigned int generation)
{
    memset(pool, 0, sizeof(*pool));
    pool->generation = generation;
}

/*---------------------------------------------------------------------------
 * Internal Function: PoolCleanup
 *
 *	Cleanup memory pool.
 *
 * Argument:
 *	pool		- Pool to clenup.
 *
 * See also:
 *	<PlatSysPageFree>
 *---------------------------------------------------------------------------*/

void
PoolCleanup(
    MemoryPool *pool)
{
    Page *base, *next, *page;

    /* 
     * Free all system pages.
     */

    for (base = pool->pages; base; base = next) {
	for (page = base; !(PAGE_FLAGS(page) & PAGE_FLAG_LAST); 
		page = PAGE_NEXT(page));
	next = PAGE_NEXT(page);
	PlatSysPageFree(base, PAGE_SYSTEMSIZE(base));
    }
}


/****************************************************************************
 * Internal Group: Page Allocation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Variable: systemPageSize
 *
 *	System page size in bytes. Allocated by platform specific code.
 *---------------------------------------------------------------------------*/

size_t systemPageSize;

/*---------------------------------------------------------------------------
 * Internal Function: PoolAllocPages
 *
 *	Allocate pages in pool. Pages are inserted after the given page. This
 *	guarantees better performances by avoiding the traversal of previous
 *	pages.
 *
 * Arguments:
 *	pool	- Pool to allocate pages into.
 *	number	- Number of pages to allocate.
 *
 * See also:
 *	<PlatSysPageAlloc>
 *---------------------------------------------------------------------------*/

void 
PoolAllocPages(
    MemoryPool *pool,
    size_t number)
{
    Page *base, *page, *prev;
    const size_t nbPagesPerSysPage = systemPageSize/PAGE_SIZE;
    size_t nbSysPages, nbPages;
    size_t i;

    if (number == 1) {
	/*
	 * Regular case: allocate one physical page divided into equally sized
	 * logical pages.
	 */

	nbSysPages = 1;
	nbPages = nbPagesPerSysPage;
    } else {
	/*
	 * Allocate enough physical pages for the given number of logical pages.
	 */

	div_t d = div((int) number, (int) nbPagesPerSysPage);
	nbSysPages = d.quot + (d.rem?1:0);
	if (nbSysPages >= LARGE_PAGE_SIZE) {
	    nbPages = 1;
	} else {
	    nbPages = (nbSysPages * nbPagesPerSysPage) - number + 1;
	}
    }

    base = (Page *) PlatSysPageAlloc(nbSysPages);

    if (!pool->pages) {
	pool->pages = base;
	for (i = 0; i < AVAILABLE_CELLS; i++) {
	    pool->lastFreeCell[i] = PAGE_CELL(base, 0);
	}
    }

    pool->nbPages += nbPages;
    pool->nbAlloc += nbPages;
    pool->nbSetCells += RESERVED_CELLS*nbPages;

    /* 
     * Initialize pages. 
     */

    prev = pool->lastPage;
    for (i = 0, page = base; i < nbPages; i++, page++) {
	/* 
	 * Pages are linked in order.
	 */

	if (prev) {
	    PAGE_NEXT(prev) = page;
	}
	prev = page;

	/* Flags. */
	PAGE_GENERATION(page) = (unsigned char) pool->generation;
	PAGE_FLAGS(page) = 0;
	PAGE_SYSTEMSIZE(page) = (unsigned short) nbSysPages;//FIXME: gets truncated with large page groups

	/* Initialize bit mask for allocated cells. */
	ClearAllCells(page);
    }
    pool->lastPage = prev;
    PAGE_NEXT(prev) = NULL;
    PAGE_FLAGS(prev) |= PAGE_FLAG_LAST;
}

/*---------------------------------------------------------------------------
 * Internal Function: PoolFreeEmptyPages
 *
 *	Free empty system pages in pool. Refresh page count in the process.
 *
 * Argument:
 *	pool	- Pool with pages to free.
 *
 * See also:
 *	<PlatSysPageFree>
 *---------------------------------------------------------------------------*/

void 
PoolFreeEmptyPages(
    MemoryPool *pool)
{
    Page *page, *base, *prev, *next;
    const size_t nbPagesPerSysPage = systemPageSize/PAGE_SIZE;
    size_t nbSetCells, nbPages, i;

    pool->nbPages = 0;
    pool->nbSetCells = 0;

    /* 
     * Iterate over system pages... 
     */

    for (prev = NULL, base = pool->pages; base; base = next) {
	/* 
	 * ... then over logical pages within the system page. 
	 */

	nbPages = 0;
	nbSetCells = 0;
	for (page = base; ; page = PAGE_NEXT(page)) {
	    nbPages++;
	    nbSetCells += NbSetCells(page);
	    if (PAGE_FLAGS(page) & PAGE_FLAG_LAST) break;
	}
	next = PAGE_NEXT(page);
	if (nbSetCells > RESERVED_CELLS * nbPages) {
	    /*
	     * At least one page contains allocated cells.
	     */

	    if (!TestCell(page, CELLS_PER_PAGE-1)) {
		/*
		 * If there was a trailing large cell group, it is unused now.
		 */

		if (PAGE_SYSTEMSIZE(base) > 1) {
		    /*
		     * Free extra system pages used by trailing cells.
		     */

		    PlatSysPageFree(base + nbPagesPerSysPage, 
			    PAGE_SYSTEMSIZE(base) - 1);
		    for (i = 0; i < nbPages; i++) {
			PAGE_SYSTEMSIZE(base+i) = 1;
		    }
		}

		if (nbPages < nbPagesPerSysPage) {
		    /*
		     * Rebuild & relink logical pages that were used by the 
		     * trailing cells.
		     */

		    PAGE_FLAGS(page) &= ~PAGE_FLAG_LAST;
		    for (i = nbPages; i < nbPagesPerSysPage; i++) {
			PAGE_NEXT(page) = page+1;
			page++;

			/* Flags. */
			PAGE_GENERATION(page) 
				= (unsigned char) pool->generation;
			PAGE_FLAGS(page) = 0;
			PAGE_SYSTEMSIZE(page) = 1;

			/* Initialize bit mask for allocated cells. */
			ClearAllCells(page);

			nbPages++;
			nbSetCells++;
		    }
		    PAGE_FLAGS(page) |= PAGE_FLAG_LAST;
		    PAGE_NEXT(page) = next;
		}
	    }
	    prev = page;
	    pool->nbPages += nbPages;
	    pool->nbSetCells += nbSetCells;
	} else {
	    /* 
	     * All logical pages in system page are empty. 
	     */

	    if (prev) {
		/* 
		 * Middle of list. 
		 */

		PAGE_NEXT(prev) = next;
	    } else {
		/* 
		 * Head of list. 
		 */

		pool->pages = next;
	    }

	    /* 
	     * Free the system page. 
	     */

	    PlatSysPageFree(base, PAGE_SYSTEMSIZE(base));
	}
    }
    ASSERT(!prev||!PAGE_NEXT(prev));
    pool->lastPage = prev;
}


/****************************************************************************
 * Internal Group: Cell Allocation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Function: PoolAllocCells
 *
 *	Allocate cells in a pool, allocating new pages if needed. Traverse and
 *	search all existing pages for a free cell sequence of the given length, 
 *	and if none is found, allocate new pages.
 *
 * Arguments:
 *	pool	- Pool to allocate cells into.
 *	number	- Number of cells to allocate.
 *
 * Result:
 *	If successful, a pointer to the first allocated cell. Else NULL.
 *	
 * Side effects:
 *	Memory pages may be allocated. This may trigger the GC later once we
 *	leave the GC-protected section.
 *
 * See also:
 *	<PageAllocCells>, <PoolAllocPages>
 *---------------------------------------------------------------------------*/

Cell * 
PoolAllocCells(
    MemoryPool *pool,
    size_t number)
{
    Cell *cells;
    Page *tail;
    size_t i;

    if (number > AVAILABLE_CELLS) {
	/*
	 * Cells span several pages. Always allocate new pages, and allocate
	 * cells in the last page.
	 */

	div_t d;
	size_t nbPages, nbFirst, first;

	/*
	 * Allocate needed pages.
	 */

	d = div((int) number-AVAILABLE_CELLS, CELLS_PER_PAGE);
	nbPages = 1 + d.quot + (d.rem?1:0);
	PoolAllocPages(pool, nbPages);
	if (!pool->lastPage) {
	    /*
	     * Fatal error!
	     */

	    Col_Error(COL_FATAL, "Page allocation failed");
	    return NULL;
	}

	if (nbPages >= LARGE_PAGE_SIZE) {
	    /*
	     * Allocate cells in their own dedicated range, leaving no free 
	     * cell, so that freeing this cell group end up in freeing the
	     * whole page range.
	     */

	    nbFirst = AVAILABLE_CELLS;
	} else {
	    /*
	     * Allocate cells at the end of the last page, leaving first cells
	     * at the beginning of the first page.
	     */

	    nbFirst = number - ((nbPages-1) * CELLS_PER_PAGE);
	    if (nbFirst == 0) {
		/*
		 * Allocate at least one cell in the last page.
		 */

		nbFirst = 1;
	    }
	}
	first = CELLS_PER_PAGE - nbFirst;
	SetCells(pool->lastPage, first, nbFirst);
	return (Cell *) pool->lastPage + first;
    }
    
    if (!pool->pages) {
	/*
	 * Alloc first pages in pool.
	 */

	PoolAllocPages(pool, 1);
    }
    cells = PageAllocCells(number, pool->lastFreeCell[number-1]);
    if (cells) {
	/* 
	 * Remember the cell page as the last one where a sequence of <number>
	 * cells was found. This avoids having to search into previous pages. 
	 */

	pool->lastFreeCell[number-1] = cells;
	return cells;
    }

    /* 
     * Allocate new pages then retry. 
     */

    tail = pool->lastPage;
    PoolAllocPages(pool, 1);
    if (!PAGE_NEXT(tail)) {
	/*
	 * Fatal error!
	 */

	Col_Error(COL_FATAL, "Page allocation failed");
	return NULL;
    }
    cells = PageAllocCells(number, PAGE_CELL(PAGE_NEXT(tail), 0));
    ASSERT(cells);

    /* 
     * Cell sequences equal to or larger than <number> cannot be found before 
     * this one. 
     */

    for (i = number-1; i < AVAILABLE_CELLS; i++) {
	pool->lastFreeCell[i] = cells;
    }

    return cells;
}

/*---------------------------------------------------------------------------
 * Internal Function: PageAllocCells
 *
 *	Allocate cells in existing pages. Traverse and search all existing pages
 *	for a free cell sequence of the given length, and if found, set cells.
 *
 * Arguments:
 *	number		- Number of cells to allocate.
 *	firstCell	- First cell to consider.
 *
 * Result:
 *	First allocated cell, or NULL if none found.
 *
 * See also:
 *	<FindFreeCells>
 *---------------------------------------------------------------------------*/

static Cell * 
PageAllocCells(
    size_t number,
    Cell *firstCell)
{
    Page *page;
    size_t first;

    /*
     * First search in cell's page.
     */

    page = CELL_PAGE(firstCell);
    first = FindFreeCells(page, number, CELL_INDEX(firstCell));
    if (first == (size_t)-1) {
	/*
	 * Not found, search in next pages.
	 */

	for (;;) {
	    page = PAGE_NEXT(page);
	    if (!page) {
		/*
		 * No room available.
		 */

		return NULL;
	    }
	    first = FindFreeCells(page, number, 0);
	    if (first != (size_t)-1) {
		/*
		 * Found!
		 */

		break;
	    }
	}
    }

    /* 
     * Mark cells. 
     */

    ASSERT(first != (size_t)-1);
    SetCells(page, first, number);

    /* 
     * Return address of first cell. 
     */

    return (Cell *) page + first;
}

/*---------------------------------------------------------------------------
 * Internal Function: FindFreeCells
 *
 *	Find sequence of free cells in page. 
 *
 * Arguments:
 *	page	- Page to look into.
 *	number	- Number of cells to look for.
 *	index	- First cell to consider.
 *
 * Result:
 *	Index of first cell of sequence, or -1 if none found.
 *---------------------------------------------------------------------------*/

static size_t 
FindFreeCells(
    void *page,
    size_t number,
    size_t index)
{
    size_t i, first = (size_t)-1, remaining;
    char seq;
    unsigned char *mask = PAGE_BITMASK(page);

    /* 
     * Iterate over bytes of the bitmask to find a chain of <number> zero bits. 
     */

    ASSERT(number <= AVAILABLE_CELLS);
    remaining = number;
    for (i = (index>>3); i < (CELLS_PER_PAGE>>3); i++) {
	if (remaining <= 7) {
	    /* 
	     * End of sequence, or whole sequence for number < 8. 
	     * Find sequence in byte. 
	     */

	    seq = firstZeroBitSequence[remaining-1][mask[i]];
	    if (seq != -1) {
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

/*---------------------------------------------------------------------------
 * Internal Function: SetCells
 *
 *	Set the page bitmask for a given sequence of cells.
 *
 * Arguments:
 *	page	- The page.
 *	first	- Index of first cell.
 *	number	- Number of cells in sequence.
 *---------------------------------------------------------------------------*/

void 
SetCells(
    Page *page,
    size_t first,
    size_t number)
{
#if CELLS_PER_PAGE == 64
#   ifdef COL_BIGENDIAN
	*(uint64_t *) PAGE_BITMASK(page) |= (((((uint64_t)1)<<number)-1)<<(64-number-first);
#   else
	*(uint64_t *) PAGE_BITMASK(page) |= (((((uint64_t)1)<<number)-1)<<first);
#   endif
#else
    unsigned char *mask = PAGE_BITMASK(page);
    size_t i;
    for (i=first; i < first+number; i++) {
	mask[i>>3] |= 1<<(i&7);
    }
#endif
}

/*---------------------------------------------------------------------------
 * Internal Function: ClearCells
 *
 *	Clear the page bitmask for a given sequence of cells.
 *
 * Arguments:
 *	page	- The page.
 *	first	- Index of first cell.
 *	number	- Number of cells in sequence.
 *---------------------------------------------------------------------------*/

void 
ClearCells(
    Page *page,
    size_t first,
    size_t number)
{
#if CELLS_PER_PAGE == 64
#   ifdef COL_BIGENDIAN
	*(uint64_t*) PAGE_BITMASK(page) &= ~(((((uint64_t)1)<<number)-1)<<(64-number-first));
#   else
	*(uint64_t*) PAGE_BITMASK(page) &= ~(((((uint64_t)1)<<number)-1)<<first);
#   endif
#else
    unsigned char *mask = PAGE_BITMASK(page);
    size_t i;
    for (i=first; i < first+number; i++) {
	mask[i>>3] &= ~(1<<(i&7));
    }
#endif
}

/*---------------------------------------------------------------------------
 * Internal Function: ClearAllCells
 *
 *	Clear the page bitmask.
 *
 * Argument:
 *	page	- The page.
 *---------------------------------------------------------------------------*/

void 
ClearAllCells(
    Page *page)
{
#if CELLS_PER_PAGE == 64
#   ifdef COL_BIGENDIAN
	*(uint64_t*) PAGE_BITMASK(page) = ((((uint64_t)1)<<RESERVED_CELLS)-1)<<(64-RESERVED_CELLS);
#   else
	*(uint64_t*) PAGE_BITMASK(page) = (((uint64_t)1)<<RESERVED_CELLS)-1;
#   endif
#else
    unsigned char *mask = PAGE_BITMASK(page);
    size_t i;
    for (i=0; i < (CELLS_PER_PAGE>>3); i++) {
	mask[i] = 0;
    }

    /*
     * Don't clear reserved cells.
     */

    SetCells(page, 0, RESERVED_CELLS);
#endif
}

/*---------------------------------------------------------------------------
 * Internal Function: TestCell
 *
 *	Test the page bitmap for a given cell.
 *
 * Arguments:
 *	page	- The page.
 *	index	- Index of cell.
 *
 * Result:
 *	Whether the cell is set.
 *---------------------------------------------------------------------------*/

int 
TestCell(
    Page *page,
    size_t index)
{
#if CELLS_PER_PAGE == 64
#   ifdef COL_BIGENDIAN
	return (*(uint64_t*) PAGE_BITMASK(page) & (((uint64_t)1)<<(63-index))?1:0;
#   else
	return (*(uint64_t*) PAGE_BITMASK(page) & (((uint64_t)1)<<index))?1:0;
#   endif
#else
    unsigned char *mask = PAGE_BITMASK(page);
    return mask[index>>3] & (1<<(index&7));
#endif
}

/*---------------------------------------------------------------------------
 * Internal Function: NbSetCells
 *
 *	Get the number of cells set in a page.
 *
 * Argument:
 *	page	- The page.
 *
 * Result:
 *	Number of set cells.
 *---------------------------------------------------------------------------*/

size_t
NbSetCells(
    Page *page)
{
#if 0 && CELLS_PER_PAGE == 64 //FIXME?
    /*
     * Hamming weight on bitstring.
     *
     * See http://en.wikipedia.org/wiki/Hamming_weight
     *
     * " This uses fewer arithmetic operations than any other known  
     * implementation on machines with fast multiplication.
     * It uses 12 arithmetic operations, one of which is a multiply. "
     */

    const uint64_t m1  = 0x5555555555555555; /* binary: 0101... */
    const uint64_t m2  = 0x3333333333333333; /* binary: 00110011..  */
    const uint64_t m4  = 0x0f0f0f0f0f0f0f0f; /* binary:  4 zeros,  4 ones ...  */
    const uint64_t m8  = 0x00ff00ff00ff00ff; /* binary:  8 zeros,  8 ones ...  */
    const uint64_t m16 = 0x0000ffff0000ffff; /* binary: 16 zeros, 16 ones ...  */
    const uint64_t m32 = 0x00000000ffffffff; /* binary: 32 zeros, 32 ones */
    const uint64_t hff = 0xffffffffffffffff; /* binary: all ones */
    const uint64_t h01 = 0x0101010101010101; /* the sum of 256 to the power of 0,1,2,3...  */
    uint64_t x = PAGE_BITMASK(page);
    x -= (x >> 1) & m1;             /* put count of each 2 bits into those 2 bits */
    x = (x & m2) + ((x >> 2) & m2); /* put count of each 4 bits into those 4 bits */
    x = (x + (x >> 4)) & m4;        /* put count of each 8 bits into those 8 bits */
    return (x * h01)>>56;  /* returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...  */
#else
    unsigned char *mask = PAGE_BITMASK(page);
    size_t i, nb = 0;
    for (i=0; i < (CELLS_PER_PAGE>>3); i++) {
	nb += nbBitsSet[mask[i]];
    }
    return nb;
#endif
}
