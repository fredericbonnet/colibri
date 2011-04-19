#ifndef _COLIBRI_CONFIGURATION
#define _COLIBRI_CONFIGURATION

/*
 * Machine-specific stuff.
 *
 * Note: this code was take from Tcl.
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

/*
 * Machine word size. Comparing UINT_MAX and SIZE_MAX is a reasonable heuristics
 * when discriminating 32-bit systems vs. 64-bit.
 */

#include <limits.h>
#if UINT_MAX == SIZE_MAX
#   define SIZE_BIT		32
#else
#   define SIZE_BIT		64
#endif

/*
 * Max number of generations. With a generational factor of 10, and 6 
 * generations, the oldest generation would be collected 10^5 = 1,000,000 times 
 * less frequently than the youngest generation. With one GC every 1/10th of
 * second, that would mean about 1 major GC a day.
 */

#define GC_MAX_GENERATIONS	6

/* 
 * Generational factor, i.e. nb of GCs between generations. 
 */

#define GC_GEN_FACTOR		10

/* 
 * Number of page allocations on a pool before triggering a GC.
 */

#define GC_MIN_PAGE_ALLOC	64
#define GC_MAX_PAGE_ALLOC	1024
#define GC_THRESHOLD(threshold)	\
    ((threshold)<GC_MIN_PAGE_ALLOC ?	GC_MIN_PAGE_ALLOC \
    :(threshold)>GC_MAX_PAGE_ALLOC ?	GC_MAX_PAGE_ALLOC \
    :					(threshold))

/*
 * Large page size, i.e. number of pages above which cell groups are allocated
 * in their own dedicated page range.
 */

#define LARGE_PAGE_SIZE		128 /* 512 KB */

/*
 * Threshold value on a pool's fragmentation to decide whether to promote 
 * individual cells vs. whole pages. In practice this performs a compaction of 
 * this pool, which limits fragmentation and memory overhead. 
 */

#define PROMOTE_COMPACT
#define PROMOTE_PAGE_FILL_RATIO 0.90

/*
 * Page and cell sizes, and number of cells per page.
 */

#if SIZE_BIT == 32
#   define PAGE_SIZE		1024
#   define CELL_SIZE		16 /* = 4*4 */
#   define CELLS_PER_PAGE	64 /* PAGE_SIZE/CELLS_PER_PAGE */
#endif

#endif /* _COLIBRI_CONFIGURATION */
