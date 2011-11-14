/*
 * Internal File: colConf.h
 *
 *	This header file defines configuration macros.
 */

#ifndef _COLIBRI_CONFIGURATION
#define _COLIBRI_CONFIGURATION


/****************************************************************************
 * Internal Group: System and Architecture
 *
 *	Machine-specific stuff.
 *
 * Note:
 *	This code was take from Tcl.
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macro: BIG_ENDIAN
 *
 *	Machine endianness. When defined, we are on a big-endian machine 
 *	(PowerPC, ...), else we are on a little-endian machine (Intel, ...).
 *---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------
 * Internal Macro: SIZE_BIT
 *
 *	Machine word size. 
 *
 * Note:
 *	Comparing UINT_MAX and SIZE_MAX is a reasonable heuristics when 
 *	discriminating 32-bit systems vs. 64-bit.
 *---------------------------------------------------------------------------*/

#include <limits.h>
#if UINT_MAX == SIZE_MAX
#   define SIZE_BIT		32
#else
#   define SIZE_BIT		64
#endif


/****************************************************************************
 * Internal Group: Garbage Collection
 *
 *	GC-related configuration settings.
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Constant: GC_MAX_GENERATIONS
 *
 *	Maximum number of generations. With a generational factor of 10, and 6 
 *	generations, the oldest generation would be collected 10^5 = 1,000,000 
 *	times less frequently than the youngest generation. With one GC every 
 *	1/10th of second, that would mean about 1 major GC a day.
 *
 * Note:
 *	Value should not exceed 15 (see <PAGE_GENERATION>).
 *
 * See also:
 *	<GC_GEN_FACTOR>
 *---------------------------------------------------------------------------*/

#define GC_MAX_GENERATIONS \
    6

/*---------------------------------------------------------------------------
 * Internal Constant: GC_GEN_FACTOR
 *
 *	Generational factor. I.e. number of GCs between generations. 
 *---------------------------------------------------------------------------*/

#define GC_GEN_FACTOR \
    10

/*---------------------------------------------------------------------------
 * Internal Constants: Promotion/Compaction Control
 *
 *	Control how cells are promoted to the next generation during GC.
 *	Promotion is generally done by moving whole pages across generations,
 *	which is fast and non-moving (it preserves cell addresses). However it
 *	can generate fragmentation if pages contain a mix of reachable and
 *	collected cells. To avoid that, we can promote cells of the last
 *	collected generation individually by moving them to the next generation,
 *	i.e. the first uncollected generation. This is slightly slower and
 *	causes the address of cells to change (unless they are pinned), however
 *	it lowers overall memory consumption and may improve locality of 
 *	reference over time.
 *
 *  PROMOTE_COMPACT		- When defined, enables promotion/compaction
 *				  during GC. Else, whole pages are promoted 
 *				  across generations.
 *  PROMOTE_PAGE_FILL_RATIO	- Threshold value on a pool's fill ratio to 
 *				  decide whether to activate compaction.
 *
 * See also:
 *	<PerformGC>, <MarkWord>
 *---------------------------------------------------------------------------*/

#define PROMOTE_COMPACT
#define PROMOTE_PAGE_FILL_RATIO \
    0.90

/*---------------------------------------------------------------------------
 * Internal Macros: GC Trigger Criteria
 *
 *	Control when garbage collection are performed.
 *
 *  GC_MIN_PAGE_ALLOC	- Minimum number of page allocations in a pool since
 *			  last GC before triggering a new one.
 *  GC_MAX_PAGE_ALLOC	- Number of page allocations in a pool since last GC
 *			  above which to always trigger a new one.
 *  GC_THRESHOLD	- Utility macro that combines both criteria.
 *
 * See also:
 *	<Col_ResumeGC>, <PerformGC>
 *---------------------------------------------------------------------------*/

#define GC_MIN_PAGE_ALLOC \
    64
#define GC_MAX_PAGE_ALLOC \
    1024
#define GC_THRESHOLD(threshold)	\
    ((threshold)<GC_MIN_PAGE_ALLOC ?	GC_MIN_PAGE_ALLOC \
    :(threshold)>GC_MAX_PAGE_ALLOC ?	GC_MAX_PAGE_ALLOC \
    :					(threshold))


/****************************************************************************
 * Internal Group: Memory Allocation
 *
 *	Page and cell allocation-related configuration settings.
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Constant: LARGE_PAGE_SIZE
 *
 *	Large page size. This is the number of pages above which cell groups are
 *	allocated in their own dedicated page range, i.e. no free cell remain
 *	after allocation.
 *
 * Note:
 *	Value should not exceed 128 (see <AddressRange>).
 *
 * See also:
 *	<PoolAllocPages>
 *---------------------------------------------------------------------------*/

#define LARGE_PAGE_SIZE \
    128 /* 512 KB */

/*---------------------------------------------------------------------------
 * Internal Constants: Page and Cell Sizes
 *
 *	Actual sizes of pages and cells depending on the system architecture.
 *
 *  PAGE_SIZE		- Page size in bytes.
 *  CELL_SIZE		- Cell size in bytes.
 *  CELLS_PER_PAGE	- Number of cells per page.
 *---------------------------------------------------------------------------*/

#if SIZE_BIT == 32
#   define PAGE_SIZE		1024
#   define CELL_SIZE		16 /* = 4*4 */
#   define CELLS_PER_PAGE	64 /* PAGE_SIZE/CELLS_PER_PAGE */
#elif SIZE_BIT == 64
#   define PAGE_SIZE		4096
#   define CELL_SIZE		32 /* = 4*8 */
#   define CELLS_PER_PAGE	128 /* PAGE_SIZE/CELLS_PER_PAGE */
#endif

#endif /* _COLIBRI_CONFIGURATION */
