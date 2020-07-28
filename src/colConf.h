/**
 * @file colConf.h
 *
 * This header file defines configuration macros.
 *
 * @beginprivate @cond PRIVATE
 */

#ifndef _COLIBRI_CONFIGURATION
#define _COLIBRI_CONFIGURATION

/*
===========================================================================*//*!
\internal \addtogroup arch System and Architecture
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Machine-Dependent Constants
 ***************************************************************************\{*/

/**
 * \def COL_BIGENDIAN
 *      Machine endianness. When defined, we are on a big-endian machine
 *      (PowerPC, ...), else we are on a little-endian machine (Intel, ...).
 *
 * @remark
 *      This code was taken from Tcl.
 */
#ifdef HAVE_SYS_TYPES_H
#    include <sys/types.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#    include <sys/param.h>
#endif
#ifdef BYTE_ORDER
#    ifdef BIG_ENDIAN
#        if BYTE_ORDER == BIG_ENDIAN
#            undef COL_BIGENDIAN
#            define COL_BIGENDIAN 1
#        endif
#    endif
#    ifdef LITTLE_ENDIAN
#        if BYTE_ORDER == LITTLE_ENDIAN
#            undef COL_BIGENDIAN
#        endif
#    endif
#endif

/**
 * Machine word size.
 *
 * @note
 *      Comparing UINT_MAX and SIZE_MAX is a reasonable heuristics when
 *      discriminating 32-bit systems vs. 64-bit.
 */
#include <limits.h>
#if UINT_MAX == SIZE_MAX
#   define SIZE_BIT             32
#else
#   define SIZE_BIT             64
#endif

/* End of Machine-Dependent Constants *//*!\}*/

/* End of System and Architecture *//*!\}*/


/*
===========================================================================*//*!
\internal \addtogroup gc Garbage Collection
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name GC-Related Configuration Settings
 ***************************************************************************\{*/

/**
 * Maximum number of generations. With a generational factor of 10, and 6
 * generations, the oldest generation would be collected 10^5 = 1,000,000
 * times less frequently than the youngest generation. With one GC every
 * 1/10th of second, that would mean about 1 major GC a day.
 *
 * @attention
 *      Value should not exceed 15 (see #PAGE_GENERATION).
 *
 * @see GC_GEN_FACTOR
 */
#define GC_MAX_GENERATIONS      6

/**
 * Generational factor, i.e.\ number of GCs between generations.
 */
#define GC_GEN_FACTOR           10

/**
 * Control how cells are promoted to the next generation during GC.
 * Promotion is generally done by moving whole pages across generations,
 * which is fast and non-moving (it preserves cell addresses). However it
 * can generate fragmentation if pages contain a mix of reachable and
 * collected cells. To avoid that, we can promote cells of the last
 * collected generation individually by moving them to the next generation,
 * i.e. the first uncollected generation. This is slightly slower and
 * causes the address of cells to change (unless they are pinned), however
 * it lowers overall memory consumption and may improve locality of
 * reference over time.
 *
 * When defined, enables promotion/compaction during GC. Else, whole pages
 * are promoted across generations.
 *
 * @see PerformGC
 * @see MarkWord
 * @see PROMOTE_PAGE_FILL_RATIO
 */
#define PROMOTE_COMPACT

/**
 * Threshold value on a pool's fill ratio to decide whether to activate
 * compaction.
 *
 * @see PerformGC
 * @see MarkWord
 * @see PROMOTE_COMPACT
 */
#define PROMOTE_PAGE_FILL_RATIO 0.90

/*---------------------------------------------------------------------------
 * Control when garbage collection are performed.
 *--------------------------------------------------------------------------*/

/**
 * Minimum number of page allocations in a pool since last GC before
 * triggering a new one.
 *
 * @see Col_ResumeGC
 * @see PerformGC
 * @see GC_THRESHOLD
 */
#define GC_MIN_PAGE_ALLOC       64

/**
 * Number of page allocations in a pool since last GC above which to always
 * trigger a new one.
 *
 * @see Col_ResumeGC
 * @see PerformGC
 * @see GC_THRESHOLD
 */
#define GC_MAX_PAGE_ALLOC       1024

/**
 * Utility macro that combines both min and max page allocation numbers
 * for GC trigger control.
 *
 * @param threshold     Threshold value.
 *
 * @return Actual threshold value compared to number of allocations.
 *
 * @see Col_ResumeGC
 * @see PerformGC
 * @see GC_MIN_PAGE_ALLOC
 * @see GC_MAX_PAGE_ALLOC
 */
#define GC_THRESHOLD(threshold) \
    ((threshold)<GC_MIN_PAGE_ALLOC ?    GC_MIN_PAGE_ALLOC \
    :(threshold)>GC_MAX_PAGE_ALLOC ?    GC_MAX_PAGE_ALLOC \
    :                                   (threshold))

/* End of GC-Related Configuration Settings *//*!\}*/

/* End of Garbage Collection *//*!\}*/


/*
===========================================================================*//*!
\internal \addtogroup alloc Memory Allocation
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Page and Cell Allocation-Related Configuration Settings
 ***************************************************************************\{*/

/**
 * Large page size. This is the number of pages above which cell groups are
 * allocated in their own dedicated page range, i.e. no free cell remains
 * after allocation.
 *
 * @note
 *      Value should not exceed 128 (see #AddressRange).
 *
 * @see PoolAllocPages
 */
#define LARGE_PAGE_SIZE         128 /* 512 KB on 64-bit */

/*---------------------------------------------------------------------------
 * Page and Cell Size Constants
 *--------------------------------------------------------------------------*/

/**
 * \def PAGE_SIZE
 *      Page size in bytes.
 *
 * \def CELL_SIZE
 *      Cell size in bytes.
 *
 * \def CELLS_PER_PAGE
 *      Number of cells per page.
 */
#if SIZE_BIT == 32
#   define PAGE_SIZE            1024
#   define CELL_SIZE            16 /* 4*4 */
#   define CELLS_PER_PAGE       64 /* PAGE_SIZE/CELL_SIZE */
#elif SIZE_BIT == 64
#   define PAGE_SIZE            4096
#   define CELL_SIZE            32 /* 4*8 */
#   define CELLS_PER_PAGE       128 /* PAGE_SIZE/CELL_SIZE */
#endif

/* End of Page and Cell Allocation-Related Configuration Settings *//*!\}*/

/* End of Memory Allocation *//*!\}*/

#endif /* _COLIBRI_CONFIGURATION */
/*! @endcond @endprivate */