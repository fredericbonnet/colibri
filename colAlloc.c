/*
 * Internal File: colAlloc.c
 *
 *	This file implements the memory allocation features of Colibri.
 */

#include "include/colibri.h"
#include "colInternal.h"
#include "colPlatform.h"

#include <stdlib.h>
#include <string.h>

/*
 * Prototypes for functions used only in this file.
 */

typedef struct AddressRange *pAddressRange;
static size_t		FindFreePagesInRange(struct AddressRange *range, 
			    size_t number, size_t index);
static void *		SysPageAlloc(size_t number, int written);
static void		SysPageFree(void * base);
static void		SysPageTrim(void * base);
static Cell *		PageAllocCells(size_t number, Cell *firstCell);
static size_t		FindFreeCells(void *page, size_t number, size_t index);


/****************************************************************************
 * Internal Section: Bit Handling
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Variable: firstZeroBitSequence
 *
 *	Position of the first zero-bit sequence of a given length in byte.
 *	First index is length of zero-bit sequence to look for, minus 1. Second 
 *	index is value of byte in which to search. Result is index of the first 
 *	bit in matching zero-bit sequence, or -1 if none.
 *---------------------------------------------------------------------------*/

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
    /* 8-bit sequences need no table, just test for zero byte */
};

/*---------------------------------------------------------------------------
 * Internal Variable: longestLeadingZeroBitSequence
 *
 *	Longest leading zero-bit sequence in byte. Index is value of byte.
 *	Result is number of consecutive cleared bytes starting at MSB.
 *---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------
 * Internal Variable: nbBitsSet
 *
 *	Number of bits set in byte. Index is value of byte. Result is number of
 *	set bits.
 *---------------------------------------------------------------------------*/

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


/****************************************************************************
 * Internal Section: Memory Pools
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
 *	<SysPageFree>
 *---------------------------------------------------------------------------*/

void
PoolCleanup(
    MemoryPool *pool)
{
    Page *base, *next, *page;

    /*
     * Cleanup sweepable custom ropes (those with a freeProc).
     */

    CleanupSweepables(pool);

    /* 
     * Free all system pages.
     */

    for (base = pool->pages; base; base = next) {
	for (page = base; !PAGE_FLAG(page, PAGE_FLAG_LAST); 
		page = PAGE_NEXT(page));
	next = PAGE_NEXT(page);
	ASSERT(PAGE_FLAG(base, PAGE_FLAG_FIRST));
	SysPageFree(base);
    }
}


/****************************************************************************
 * Internal Section: System Page Allocation
 * 
 *	Granularity-free system page allocation based on address range 
 *	reservation.
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Algorithm: Address Reservation And Allocation
 *
 *	Some systems allow single system page allocations (e.g. POSIX mmap), 
 *	while others require coarser grained schemes (e.g. Windows 
 *	VirtualAlloc). However in both cases we have to keep track of allocated
 *	pages, especially for generational page write monitoring. So address 
 *	ranges are reserved, and individual pages can be allocated and freed 
 *	within these ranges. From a higher level this allows for per-page 
 *	allocation, while at the same time providing enough metadata for memory
 *	introspection.
 *
 *	When the number of pages to allocate exceeds a certain size (defined as 
 *	<LARGE_PAGE_SIZE>), a dedicated address range is reserved and allocated, 
 *	which must be freed all at once. Else, pages are allocated within larger 
 *	address ranges using the following scheme: 
 *	
 *	Address ranges are reserved in geometrically increasing sizes up to a 
 *	maximum size (the first being a multiple of the allocation granularity). 
 *	Ranges form a singly-linked list in allocation order (so that search 
 *	times are geometrically increasing too). A descriptor structure is 
 *	malloc'd along with the range and consists of a pointer to the next 
 *	descriptor, the base address of the range, the total and free numbers of 
 *	pages and the index of the first free page in range, and a page alloc
 *	info table indicating:
 *
 *		- For free pages, zero.
 *		- For first page in group, the negated size of the group.
 *		- For remaining pages, the index in group.
 *
 *	That way, page group info can be known via direct access from the page
 *	index in range:
 *
 *		- When alloc info is zero, the page is free.
 *		- When negative, the page is the first in a group of the given
 *		  negated size.
 *		- When positive, the page is the n-th in a group whose first 
 *		  page is the n-th previous one.
 *	
 *	To allocate a group of pages in an address range, the alloc info table
 *	is scanned until a large enough group of free pages is found. 
 *
 *	To free a group of pages, the containing address range is found by 
 *	scanning all ranges in order (this is fast, as this only involves 
 *	address comparison and the ranges grow geometrically). In either cases 
 *	the descriptor is updated accordingly. If a containing range is not 
 *	found we assume that it was a dedicated range and we attempt to release
 *	it at once. 
 *
 *	Just following this alloc info table is a bitmask table used for write
 *	tracking. With our generational GC, pages of older generations are 
 *	write-protected so that references pointing to younger cells can be
 *	tracked during the mark phase: such modified pages contain potential
 *	parent cells that must be followed along with roots. Regular ranges use
 *	a bitmask array, while dedicated ranges only have to store one value 
 *	for the whole range.
 *	
 *	This allocation scheme may not look optimal at first sight (especially 
 *	the alloc info table scanning step), but keep in mind that the typical 
 *	use case only involves single page allocations. Multiple page 
 *	allocations only occur when allocating large, multiple cell-based 
 *	structures, and most structures are single cell sized. And very large 
 *	pages will end up in their own dedicated range with no group management.
 *	Moreover stress tests have shown that this scheme yielded similar or 
 *	better performances than the previous schemes.
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Variables: System Page Size and Granularity Variables
 *
 *	System-dependent variables.
 *
 *  systemPageSize	- System page size in bytes.
 *  allocGranularity	- Allocation granularity of address ranges.
 *  shiftPage		- Bits to shift to convert between pages and bytes.
 *---------------------------------------------------------------------------*/

size_t systemPageSize;
size_t allocGranularity;
size_t shiftPage;

/*---------------------------------------------------------------------------
 * Internal Typedef: AddressRange
 *
 *	Address range descriptor for allocated system pages.
 *
 * Fields:
 *	next		- Next descriptor in list.
 *	base		- Base address.
 *	size		- Size in pages.
 *	free		- Number of free pages in range.
 *	first		- First free page in range.
 *	allocInfo	- Info about allocated pages in range.
 *
 * See also:
 *	<SysPageAlloc>, <SysPageFree>
 *---------------------------------------------------------------------------*/

typedef struct AddressRange {
    struct AddressRange *next;
    void *base;
    size_t size;
    size_t free;
    size_t first;
    char allocInfo[0];
} AddressRange;

/*---------------------------------------------------------------------------
 * Internal Variables: Address Ranges
 *
 *  ranges		- Reserved address ranges for general purpose.
 *  dedicatedRanges	- Dedicated address ranges for large pages.
 *
 * See also:
 *	<AddressRange>, <SysPageAlloc>, <SysPageFree>
 *---------------------------------------------------------------------------*/

static AddressRange *ranges = NULL;
static AddressRange *dedicatedRanges = NULL;

/*---------------------------------------------------------------------------
 * Internal Constant: Address Range Size Constants
 *
 *  FIRST_RANGE_SIZE	- Size of first reserved range.
 *  MAX_RANGE_SIZE	- Maximum size of ranges.
 *---------------------------------------------------------------------------*/

#define FIRST_RANGE_SIZE	256	/* 1 MB */
#define MAX_RANGE_SIZE		32768	/* 128 MB */

/*---------------------------------------------------------------------------
 * Internal Function: FindFreePagesInRange
 *
 *	Find given number of free consecutive pages in alloc info table.
 *
 * Argument:
 *	range	- Address range to look into.
 *	number	- Number of free cnsecutive entries to find.
 *	index	- First entry to consider.
 *
 * Result:
 *	Index of first page of sequence, or -1 if none found.
 *---------------------------------------------------------------------------*/

static size_t 
FindFreePagesInRange(
    AddressRange *range,
    size_t number,
    size_t index)
{
    size_t i, first = (size_t)-1, remaining;

    /* 
     * Iterate over alloc info table to find a chain of <number> zero entries. 
     */

    remaining = number;
    for (i = index; i < range->size; i++) {
	if (range->allocInfo[i] == 0) {
	    if (remaining == number) {
		/* 
		 * Beginning of sequence. 
		 */

		first = i;
	    }

	    remaining--;
	    if (remaining == 0) {
		/*
		 * End of sequence reached.
		 */

		return first;
	    }

	} else {
	    /* 
	     * Sequence interrupted, restart. 
	     */

	    remaining = number;
	}
    }

    /*
     * None found.
     */

    return (size_t) -1;
}

/*---------------------------------------------------------------------------
 * Internal Function: SysPageAlloc
 *
 *	Allocate system pages.
 *
 * Arguments:
 *	number	- Number of system pages to alloc.
 *	written	- Initial write tracking flag value.
 *
 * Result:
 *	The allocated system pages' base address.
 *
 * Side effects:
 *	May reserve new address ranges.
 *
 * See also:
 *	<SysPageFree>
 *---------------------------------------------------------------------------*/

static void * 
SysPageAlloc(
    size_t number,
    int written)
{
    void *addr = NULL;
    AddressRange *range, **prevPtr;
    size_t first, size, i;

    if (number > LARGE_PAGE_SIZE || !(number 
	    & ((allocGranularity >> shiftPage)-1))) {
	/*
	 * Length exceeds a certain threshold or is a multiple of the allocation
	 * granularity, allocate dedicated address range.
	 */

	addr = PlatReserveRange(number, 1);
	if (!addr) {
	    /*
	     * Fatal error!
	     */

	    Col_Error(COL_FATAL, "Address range allocation failed");
	    return NULL;
	}

	/*
	 * Create descriptor without page alloc table and insert at head.
	 */

	PlatEnterProtectAddressRanges();
	{
	    range = (AddressRange *) (malloc(sizeof(AddressRange)+1));
	    range->next = dedicatedRanges;
	    range->base = addr;
	    range->size = number;
	    range->free = 0;
	    range->first = number;
	    range->allocInfo[0] = written;
	    dedicatedRanges = range;
	}
	PlatLeaveProtectAddressRanges();

	return addr;
    }

    /*
     * Try to find address range with enough consecutive pages.
     */

    PlatEnterProtectAddressRanges();
    {
	first = (size_t)-1;
	prevPtr = &ranges;
	range = ranges;
	while (range) {
	    size = range->size;
	    if (range->free >= number) {
		/*
		 * Range has the required number of pages.
		 */

		first = range->first;

		if (number == 1 && !range->allocInfo[first]) {
		    /*
		     * Fast-track the most common case: allocate the first free 
		     * page.
		     */

		    break;
		}
    	    
		/*
		 * Find the first available bit sequence.
		 */

		first = FindFreePagesInRange(range, number, first);
		if (first != (size_t)-1) {
		    break;
		}
	    }

	    /*
	     * Not found, try next range.
	     */

	    prevPtr = &range->next;
	    range = range->next;
	}

	if (!range) {
	    /*
	     * No range was found with available pages. Create a new one.
	     */

	    if (!ranges) {
		size = FIRST_RANGE_SIZE;
	    } else {
		/*
		 * New range size is double that of the previous one.
		 */

		size <<= 1;
		if (size > MAX_RANGE_SIZE) size = MAX_RANGE_SIZE;
	    }
	    ASSERT(number <= size-1);

	    /*
	     * Reserve address range.
	     */

	    addr = PlatReserveRange(size, 0);
	    if (!addr) {
		/*
		 * Fatal error!
		 */

		Col_Error(COL_FATAL, "Address range reservation failed");
		goto end;
	    }

	    /*
	     * Create descriptor.
	     */

	    range = (AddressRange *) (malloc(sizeof(AddressRange) + size 
			+ ((size+7)>>3)));
	    *prevPtr = range;
	    range->base = addr;
	    range->next = NULL;
	    range->size = size;
	    range->free = size;
	    range->first = 0;
	    memset(range->allocInfo, 0, size + ((size+7)>>3));
	    first = 0;
	}

	/*
	 * Allocate pages. 
	 */

	ASSERT(first+number <= size);
	ASSERT(number <= range->free);
	addr = (char *) range->base + (first << shiftPage);
	if (!PlatAllocPages(addr, number)) {
	    /*
	     * Fatal error!
	     */

	    Col_Error(COL_FATAL, "Page allocation failed");
	    goto end;
	}

	/*
	 * Update metadata. Only clear first page's written flag.
	 */

	ASSERT(number < LARGE_PAGE_SIZE && number-1 <= CHAR_MAX && -(char) number >= CHAR_MIN);
	range->allocInfo[first] = -(char) number;
	for (i=1; i < number; i++) {
	    range->allocInfo[first+i] = (char) i;
	}
	if (written) {
	    range->allocInfo[range->size+(first>>3)] |= (1<<(first&7));
	} else {
	    range->allocInfo[range->size+(first>>3)] &= ~(1<<(first&7));
	}
	range->free -= number;
	if (first == range->first) {
	    /* 
	     * Simply increase the first free page index, the actual index will be 
	     * updated on next allocation.
	     */

	    range->first += number;
	}
    }
end:
    PlatLeaveProtectAddressRanges();
    return addr;
}

/*---------------------------------------------------------------------------
 * Internal Function: SysPageFree
 *
 *	Free system pages.
 *
 * Argument:
 *	base	- Base address of the pages to free.
 *
 * Side Effects:
 *	May release address ranges.
 *
 * See also:
 *	<SysPageAlloc>
 *---------------------------------------------------------------------------*/

static void 
SysPageFree(
    void * base)
{
    size_t index, size, i;
    AddressRange * range;

    PlatEnterProtectAddressRanges();
    {
	/*
	 * Get range info for page. 
	 */

	range = ranges;
	while (range) {
	    if (base >= range->base && (char *) base < (char *) range->base 
		    + (range->size << shiftPage)) {
		break;
	    }
	    range = range->next;
	}
	if (!range) {
	    /*
	     * Likely dedicated address range. 
	     */

	    AddressRange **prevPtr = &dedicatedRanges;
	    range = dedicatedRanges;
	    while (range) {
		if (base >= range->base && (char *) base < (char *) range->base 
			+ (range->size << shiftPage)) {
		    break;
		}
		prevPtr = &range->next;
		range = range->next;
	    }
	    if (!range) {
		/*
		 * Not found.
		 */

		Col_Error(COL_FATAL, "Page not found %p", base);
		goto end;
	    }

	    /*
	     * Release whole range.
	     */

	    if (!PlatReleaseRange(range->base, range->size)) {
		/*
		 * Fatal error!
		 */

		Col_Error(COL_FATAL, "Address range release failed");
		goto end;
	    }

	    /*
	     * Remove from dedicated range list.
	     */

	    *prevPtr = range->next;
	    free(range);
	    goto end;
	}

	index = ((char *) base - (char *) range->base) >> shiftPage;
	size = -range->allocInfo[index];
	ASSERT(size > 0);
	ASSERT(index+size <= range->size);

	/*
	 * Free pages.
	 */

	if (!PlatFreePages(base, size)) {
	    /*
	     * Fatal error!
	     */

	    Col_Error(COL_FATAL, "Page deallocation failed");
	    goto end;
	}

	/*
	 * Update metadata.
	 */

	for (i=index; i < index+size; i++) {
	    range->allocInfo[i] = 0;
	}
	range->free += size;
	ASSERT(range->free <= range->size);
	if (range->first > index) {
	    /*
	     * Old first free page is beyond the one we just freed, update.
	     */
    	 
	    range->first = index;
	}
    }
end:
    PlatLeaveProtectAddressRanges();
}

/*---------------------------------------------------------------------------
 * Internal Function: SysPageTrim
 *
 *	Free trailing pages of system page groups, keeping only the first page.
 *
 * Argument:
 *	base	- Base address of the pages to free.
 *
 * See also:
 *	<SysPageFree>
 *---------------------------------------------------------------------------*/

static void 
SysPageTrim(
    void * base)
{
    size_t index, size, i;
    AddressRange * range;

    PlatEnterProtectAddressRanges();
    {
	/*
	 * Get range info for page. 
	 */

	range = ranges;
	while (range) {
	    if (base >= range->base && (char *) base < (char *) range->base 
		    + (range->size << shiftPage)) {
		break;
	    }
	    range = range->next;
	}
	if (!range) {
	    /*
	     * Not found. Cannot trim dedicated ranges.
	     */

	    Col_Error(COL_FATAL, "Page not found %p", base);
	    goto end;
	}

	index = ((char *) base - (char *) range->base) >> shiftPage;
	size = -range->allocInfo[index];
	ASSERT(size > 0);
	ASSERT(index+size <= range->size);

	if (size == 1) {
	    /*
	     * No trailing page.
	     */

	    goto end;
	}

	/*
	 * Free trailing pages.
	 */

	if (!PlatFreePages((char *) base + systemPageSize, size-1)) {
	    /*
	     * Fatal error!
	     */

	    Col_Error(COL_FATAL, "Page deallocation failed");
	    goto end;
	}

	/*
	 * Update metadata.
	 */

	range->allocInfo[index] = -1;
	for (i=index+1; i < index+size; i++) {
	    range->allocInfo[i] = 0;
	}
	range->free += size-1;
	ASSERT(range->free <= range->size);
	if (range->first > index) {
	    /*
	     * Old first free page is beyond the one we just freed, update.
	     */
    	 
	    range->first = index;
	}
    }
end:
    PlatLeaveProtectAddressRanges();
}

/*---------------------------------------------------------------------------
 * Internal Function: SysPageProtect
 *
 *	Write-protect system page group.
 *
 * Argument:
 *	page	- Page belonging to page group to protect.
 *	protect	- Whether t protect or unprotect page group.
 *---------------------------------------------------------------------------*/

void 
SysPageProtect(
    void * page,
    int protect)
{
    size_t index, size;
    AddressRange * range;

    PlatEnterProtectAddressRanges();
    {
	/*
	 * Get range info for page. 
	 */

	range = ranges;
	while (range) {
	    if (page >= range->base && (char *) page < (char *) range->base 
		    + (range->size << shiftPage)) {
		break;
	    }
	    range = range->next;
	}
	if (!range) {
	    /*
	     * Likely dedicated address range. 
	     */

	    range = dedicatedRanges;
	    while (range) {
		if (page >= range->base && (char *) page < (char *) range->base 
			+ (range->size << shiftPage)) {
		    break;
		}
		range = range->next;
	    }
	    if (!range) {
		/*
		 * Not found.
		 */

		Col_Error(COL_FATAL, "Page not found %p", page);
		goto end;
	    }

	    /*
	     * Update protection & write tracking flag for whole range.
	     */

	    PlatProtectPages(range->base, range->size, protect);
	    range->allocInfo[0] = !protect;
	    goto end;
	}

	index = ((char *) page - (char *) range->base) >> shiftPage;
	ASSERT(range->allocInfo[index] != 0);
	if (range->allocInfo[index] > 0) index -= range->allocInfo[index];
	size = -range->allocInfo[index];
	ASSERT(size > 0);
	ASSERT(index+size <= range->size);

	/*
	 * Update protection & write tracking flag for page group (only mark 
	 * first one in page group).
	 */

	PlatProtectPages((char *) range->base + (index << shiftPage), size, 
		protect);
	if (protect) {
	    range->allocInfo[range->size+(index>>3)] &= ~(1<<(index&7));
	} else {
	    range->allocInfo[range->size+(index>>3)] |= (1<<(index&7));
	}
    }
end:
    PlatLeaveProtectAddressRanges();
}


/****************************************************************************
 * Internal Section: Roots and Parents
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Function: UpdateParents
 *
 *	Add pages written since the last GC to parent tracking structures. Then 
 *	each page's parent flag is cleared for the mark phase.
 *
 * Argument:
 *	data	- Group-specific data.
 *---------------------------------------------------------------------------*/

void 
UpdateParents(
    GroupData *data)
{
    Page *page;
    size_t i, size;
    AddressRange * range;
    Cell *cell;

    /*
     * First clear parent flags for existing parents.
     */

    for (cell = data->parents; cell; cell = PARENT_NEXT(cell)) {
	ASSERT(TestCell(CELL_PAGE(cell), CELL_INDEX(cell)));
	ASSERT(PAGE_FLAG(PARENT_PAGE(cell), PAGE_FLAG_FIRST));
	for (page = PARENT_PAGE(cell); page; page = PAGE_NEXT(page)) {
	    PAGE_CLEAR_FLAG(page, PAGE_FLAG_PARENT);
	    if (PAGE_FLAG(page, PAGE_FLAG_LAST)) break;
	}
    }

    /*
     * Iterate over ranges and find modified page groups belonging to the given
     * thread group, adding them to its parent list.
     */

    PlatEnterProtectAddressRanges();
    {
	/*
	 * First iterate over regular ranges.
	 */

	for (range = ranges; range ; range = range->next) {
	    /*
	     * Iterate over modified page groups.
	     */

	    for (i = 0; i < range->size; /* increment within loop */) {
		if (!range->allocInfo[i]) {
		    /*
		     * Free page.
		     */

		    i++;
		    continue;
		}

		size = -range->allocInfo[i];
		if (!(range->allocInfo[range->size+(i>>3)] & 1<<(i&7))) {
		    /*
		     * Not modified.
		     */

		    i += size;
		    continue;
		}

		page = (Page *) ((char *) range->base + (i << shiftPage));
		if (PAGE_GROUPDATA(page) != data) {
		    /*
		     * Page belongs to another thread group.
		     */

		    i += size;
		    continue;
		}

		/*
		 * Clear write tracking flag.
		 */

		range->allocInfo[range->size+(i>>3)] &= ~(1<<(i&7));

		/*
		 * Add first page to the thread group's parent list.
		 */

		cell = PoolAllocCells(&data->rootPool, 1);
		PARENT_INIT(cell, data->parents, page);
		data->parents = cell;
		for (; page; page = PAGE_NEXT(page)) {
		    PAGE_CLEAR_FLAG(page, PAGE_FLAG_PARENT);
		    if (PAGE_FLAG(page, PAGE_FLAG_LAST)) break;
		}

		i += size;
	    }
	}

	/*
	 * Now iterate over dedicated ranges.
	 */

	for (range = dedicatedRanges; range; range= range->next) {
	    if (!range->allocInfo[0]) {
		/*
		 * Not modified.
		 */

		continue;
	    }

	    page = (Page *) range->base;
	    if (PAGE_GROUPDATA(page) != data) {
		/*
		 * Page belongs to another thread group.
		 */

		continue;
	    }

	    /*
	     * Clear write tracking flag.
	     */

	    range->allocInfo[0] = 0;

	    /*
	     * Add first page to the thread group's parent list.
	     */

	    cell = PoolAllocCells(&data->rootPool, 1);
	    PARENT_INIT(cell, data->parents, page);
	    data->parents = cell;
	}
    }
    PlatLeaveProtectAddressRanges();
}



/****************************************************************************
 * Internal Section: Page Allocation
 ****************************************************************************/

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
 *	<SysPageAlloc>
 *---------------------------------------------------------------------------*/

void 
PoolAllocPages(
    MemoryPool *pool,
    size_t number)
{
    ThreadData *data = PlatGetThreadData();
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
	    /*
	     * Pages are in their own dedicated range.
	     */
	    
	    nbPages = 1;
	} else {
	    nbPages = (nbSysPages * nbPagesPerSysPage) - number + 1;
	}
    }

    /*
     * Allocate system pages. Make sure to mark pages as written for older
     * generations for proper parent tracking.
     */

    base = (Page *) SysPageAlloc(nbSysPages, (pool->generation >= 2));

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
	    PAGE_SET_NEXT(prev, page);
	}
	prev = page;

	PAGE_SET_GENERATION(page, pool->generation);
	PAGE_CLEAR_FLAG(page, PAGE_FLAGS_MASK);
	PAGE_GROUPDATA(page) = data->groupData;

	/* Initialize bit mask for allocated cells. */
	ClearAllCells(page);
    }
    pool->lastPage = prev;
    PAGE_SET_FLAG(base, PAGE_FLAG_FIRST);
    PAGE_SET_NEXT(prev, NULL);
    PAGE_SET_FLAG(prev, PAGE_FLAG_LAST);
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
 *	<SysPageFree>
 *---------------------------------------------------------------------------*/

void 
PoolFreeEmptyPages(
    MemoryPool *pool)
{
    ThreadData *data = PlatGetThreadData();
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
	ASSERT(PAGE_FLAG(base, PAGE_FLAG_FIRST));
	for (page = base; ; page = PAGE_NEXT(page)) {
	    nbPages++;
	    nbSetCells += NbSetCells(page);
	    if (PAGE_FLAG(page, PAGE_FLAG_LAST)) break;
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

		SysPageTrim(base);

		if (nbPages < nbPagesPerSysPage) {
		    /*
		     * Rebuild & relink logical pages that were used by the 
		     * trailing cells.
		     */

		    PAGE_CLEAR_FLAG(page, PAGE_FLAG_LAST);
		    for (i = nbPages; i < nbPagesPerSysPage; i++) {
			PAGE_SET_NEXT(page, page+1);
			page++;

			PAGE_SET_GENERATION(page, pool->generation);
			PAGE_CLEAR_FLAG(page, PAGE_FLAGS_MASK);
			PAGE_GROUPDATA(page) = data->groupData;

			/* Initialize bit mask for allocated cells. */
			ClearAllCells(page);

			nbPages++;
			nbSetCells++;
		    }
		    PAGE_SET_FLAG(page, PAGE_FLAG_LAST);
		    PAGE_SET_NEXT(page, next);
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

		PAGE_SET_NEXT(prev, next);
	    } else {
		/* 
		 * Head of list. 
		 */

		pool->pages = next;
	    }

	    /* 
	     * Free the system page. 
	     */

	    SysPageFree(base);
	}
    }
    ASSERT(!prev||!PAGE_NEXT(prev));
    pool->lastPage = prev;
}


/****************************************************************************
 * Internal Section: Cell Allocation
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
