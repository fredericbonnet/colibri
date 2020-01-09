/**
 * @file colWasiPlatform.c
 *
 * This file provides WASI implementations of generic primitives needing
 * platform-specific implementations, as well as WASI-specific primitives.
 *
 * @see colPlatform.h
 * @see colWasiPlatform.h
 *
 * @beginprivate @cond PRIVATE
 */

#include "../../include/colibri.h"
#include "../../colInternal.h"
#include "../../colPlatform.h"

#include <stdlib.h>

/*
 * Prototypes for functions used only in this file.
 */

/*! \cond IGNORE */
static struct WasiGroupData * AllocGroupData(unsigned int model);
static void             FreeGroupData(struct WasiGroupData *groupData);
static void             Init(void);
/*! \endcond *//* IGNORE */


/*
===========================================================================*//*!
\internal \weakgroup arch_wasi System and Architecture (WASI-specific)
\{*//*==========================================================================
*/

/** @beginprivate @cond PRIVATE */

/*
 * Bit twiddling hack for computing the log2 of a power of 2.
 * See: http://www-graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
 */

/*! \cond IGNORE */
static const int MultiplyDeBruijnBitPosition2[32] =
{
  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};
#define LOG2(v) MultiplyDeBruijnBitPosition2[(uint32_t)(v * 0x077CB531U) >> 27]
/*! \endcond *//* IGNORE */

/** @endcond @endprivate */


/***************************************************************************//*!
 * \name Thread-Local Storage
 ***************************************************************************\{*/

/** @beginprivate @cond PRIVATE */

/**
 * Thread-specific data.
 * 
 * This is just a global static pointer, since WASI doesn't support threads.
 *
 * @see ThreadData
 * @see Init
 * @see WasiGetThreadData
 * @see WasiSetThreadData
 */
static void *tsd = NULL;

/**
 * Get thread-specific data.
 * 
 * @see tsd
 * @see WasiSetThreadData
 */
void * WasiGetThreadData()
{
    return tsd;
}

/**
 * Get thread-specific data.
 * 
 * @see tsd
 * @see WasiGetThreadData
 */
void WasiSetThreadData(
    void *data)             /*!< Data to set. */
{
    tsd = data;
}

/**
 * Platform-specific group data.
 *
 * @see ThreadData
 * @see GroupData
 * @see Init
 * @see AllocGroupData
 * @see FreeGroupData
 */
typedef struct WasiGroupData {
    GroupData data;                 /*!< Generic #GroupData structure. */
} WasiGroupData;

/**
 * Allocate and initialize a thread group data structure.
 *
 * @return The newly allocated structure.
 *
 * @sideeffect
 *      Memory allocated and system objects created.
 *
 * @see @ref threading_models "Threading Model Constants"
 * @see WasiGroupData
 * @see FreeGroupData
 */
static WasiGroupData *
AllocGroupData(
    unsigned int model)     /*!< Threading model. */
{
    WasiGroupData *groupData = (WasiGroupData *) malloc(sizeof(WasiGroupData));
    memset(groupData, 0, sizeof(WasiGroupData));
    groupData->data.model = model;
    GcInitGroup((GroupData *) groupData);

    return groupData;
}

/**
 * Free a thread group data structure.
 *
 * @sideeffect
 *      Memory freed and system objects deleted.
 *
 * @see WasiGroupData
 * @see AllocGroupData
 */
static void
FreeGroupData(
    WasiGroupData *groupData)   /*!< Structure to free. */
{
    GcCleanupGroup((GroupData *) groupData);
    free(groupData);
}

/** @endcond @endprivate */

/* End of Thread-Local Storage *//*!\}*/


/***************************************************************************//*!
 * \name Process & Threads
 ***************************************************************************\{*/

/** @beginprivate @cond PRIVATE */

/**
 * Ensure that per-process initialization only occurs once.
 *
 * @see PlatEnter
 * @see Init
 */
static int once = 0;

/**
 * Enter the thread. If this is the first nested call, initialize thread data.
 * If this is the first thread in its group, initialize group data as well.
 *
 * @retval <>0  if this is the first nested call.
 * @retval 0    otherwise.
 *
 * @see @ref threading_models "Threading Model Constants"
 * @see ThreadData
 * @see WasiGroupData
 * @see PlatLeave
 * @see Col_Init
 */
int
PlatEnter(
    unsigned int model) /*!< Threading model. */
{
    ThreadData *data;

    /*
     * Ensures that global data is initialized once.
     */

    if (once == 0) {
        Init();
        once = 1;
    }

    data = PlatGetThreadData();
    if (data) {
        /*
         * Increment nest count.
         */

        return ((++data->nestCount) == 1);
    }

    /*
     * Initialize thread data.
     */

    data = (ThreadData *) malloc(sizeof(ThreadData));
    memset(data, 0, sizeof(*data));
    data->nestCount = 1;
    GcInitThread(data);
    WasiSetThreadData(data);

    /*
     * Allocate dedicated group.
     */

    data->groupData = (GroupData *) AllocGroupData(model);
    data->groupData->first = data;
    data->next = data;

    return 1;
}

/**
 * Leave the thread. If this is the first nested call, cleanup thread data.
 * If this is the last thread in its group, cleanup group data as well.
 *
 * @retval <>0  if this is the last nested call.
 * @retval 0    otherwise.
 *
 * @see ThreadData
 * @see WasiGroupData
 * @see PlatEnter
 * @see Col_Cleanup
 */
int
PlatLeave()
{
    ThreadData *data = PlatGetThreadData();
    if (!data) {
        /* TODO: exception ? */
        return 0;
    }

    /*
     * Decrement nest count.
     */

    if (--data->nestCount) {
        return 0;
    }

    /*
     * This is the last nested call, free structures.
     */

    FreeGroupData((WasiGroupData *) data->groupData);

    GcCleanupThread(data);
    free(data);
    WasiSetThreadData(NULL);

    return 1;
}

/** @endcond @endprivate */

/* End of Process & Threads *//*!\}*/


/***************************************************************************//*!
 * \name System Page Allocation
 ***************************************************************************\{*/

/** @beginprivate @cond PRIVATE */

/**
 * Pseudo-page size.
 */
#define WASI_PAGE_SIZE      1024

/**
 * Alignment constant for memory ranges.
 */
#define WASI_RANGE_ALIGNMENT 1024

/**
 * Reserve an address range.
 *
 * @return The reserved range's base address, or NULL if failure.
 */
void *
PlatReserveRange(
    size_t size,    /*!< Number of pages to reserve. */
    int alloc)      /*!< Whether to allocate the range pages as well. */
{
    void *addr;
    posix_memalign(&addr, WASI_RANGE_ALIGNMENT, size << shiftPage);
    return addr;
}

/**
 * Release an address range.
 *
 * @retval <>0  for success.
 * @retval 0    for failure.
 */
int
PlatReleaseRange(
    void *base,     /*!< Base address of range to release. */
    size_t size)    /*!< Number of pages in range. */
{
    free(base);
    return 1;
}

/**
 * Allocate pages in reserved range.
 *
 * @retval <>0  for success.
 * @retval 0    for failure.
 */
int
PlatAllocPages(
    void *addr,     /*!< Address of first page to allocate. */
    size_t number)  /*!< Number of pages to allocate. */
{
    /* Unsupported */
    return 1;
}

/**
 * Free pages in reserved range.
 *
 * @retval <>0  for success.
 * @retval 0    for failure.
 */
int
PlatFreePages(
    void *addr,     /*!< Address of first page to free. */
    size_t number)  /*!< Number of pages to free. */
{
    /* Unsupported */
    return 1;
}

/**
 * Protect/unprotect pages in reserved range.
 *
 * @retval <>0  for success.
 * @retval 0    for failure.
 */
int
PlatProtectPages(
    void *addr,     /*!< Address of first page to protect/unprotect */
    size_t number,  /*!< Number of pages to protect/unprotect. */
    int protect)    /*!< Whether to protect or unprotect pages. */
{
    /* Unsupported */
    return 1;
}

/** @endcond @endprivate */

/* End of System Page Allocation *//*!\}*/


/***************************************************************************//*!
 * \name Initialization/Cleanup
 ***************************************************************************\{*/

/** @beginprivate @cond PRIVATE */

/**
 * Initialization routine.
 *
 * @see PlatEnter
 */
static void
Init()
{
    systemPageSize = WASI_PAGE_SIZE;
    allocGranularity = systemPageSize * 16;
    shiftPage = LOG2(systemPageSize);
}

/** @endcond @endprivate */

/* End of Initialization/Cleanup *//*!\}*/

/* End of System and Architecture (WASI-specific) *//*!\}*/
/*! @endcond @endprivate */