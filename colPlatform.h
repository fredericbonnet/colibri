/**
 * @file colPlatform.h
 *
 * This header file defines the generic primitives needing platform-specific
 * implementations.
 *
 * @see platform/win32/colWin32Platform.h
 * @see platform/unix/colUnixPlatform.h
 *
 * @beginprivate @cond PRIVATE
 */

#ifndef _COLIBRI_PLATFORM
#define _COLIBRI_PLATFORM

#ifndef DOXYGEN

#if defined(__WIN32__)
#   include "platform/win32/colWin32Platform.h"
#elif defined(WASI)
#   include "platform/wasi/colWasiPlatform.h"
#else /* FIXME */
#   include "platform/unix/colUnixPlatform.h"
#endif

#endif /* DOXYGEN */


/*
===========================================================================*//*!
\internal \defgroup arch System and Architecture

Generic primitives needing platform-specific implementations.
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Process & Threads
 ***************************************************************************\{*/

/**
 * Enter protected section around root management structures.
 *
 * @param data  Group-specific data.
 *
 * @sideeffect
 *      When model is #COL_SHARED, calls PlatEnterProtectRoots().
 *
 * @see PlatEnterProtectRoots
 * @see LeaveProtectRoots
 */
#ifdef COL_USE_THREADS
#   define EnterProtectRoots(data) \
        if ((data)->model >= COL_SHARED) {PlatEnterProtectRoots(data);}
#else
#   define EnterProtectRoots(data) /* NOOP */
#endif /* COL_USE_THREADS */

/**
 * Leave protected section around root management structures.
 *
 * @param data  Group-specific data.
 *
 * @sideeffect
 *      When model is #COL_SHARED, calls PlatLeaveProtectRoots().
 */
#ifdef COL_USE_THREADS
#   define LeaveProtectRoots(data) \
        if ((data)->model >= COL_SHARED) {PlatLeaveProtectRoots(data);}
#else
#   define LeaveProtectRoots(data) /* NOOP */
#endif /* COL_USE_THREADS */

/**
 * Synchronize calls to Col_PauseGC().
 *
 * @param data  Group-specific data.
 *
 * @sideeffect
 *      When model isn't #COL_SINGLE, calls PlatSyncPauseGC(), which may block
 *      as long as a GC is underway.
 *
 * @see Col_PauseGC
 * @see PlatSyncPauseGC
 * @see SyncResumeGC
 */
#ifdef COL_USE_THREADS
#   define SyncPauseGC(data) \
        if ((data)->model != COL_SINGLE) {PlatSyncPauseGC(data);}
#else
#   define SyncPauseGC(data) /* NOOP */
#endif /* COL_USE_THREADS */

/**
 * Synchronize calls to Col_TryPauseGC().
 *
 * @param data  Group-specific data.
 *
 * @retval 1    if successful
 * @retval 0    if a GC is underway (this implies the threading model isn't
 *              #COL_SINGLE).
 *
 * @sideeffect
 *      When model isn't #COL_SINGLE, calls PlatTrySyncPauseGC().
 *
 * @see Col_TryPauseGC
 * @see PlatTrySyncPauseGC
 * @see SyncResumeGC
 */
#ifdef COL_USE_THREADS
#   define TrySyncPauseGC(data) \
        ((data)->model == COL_SINGLE ? 1 : PlatTrySyncPauseGC(data))
#else
#   define TrySyncPauseGC(data) 1 /* NOOP */
#endif /* COL_USE_THREADS */

/**
 * Synchronize calls to Col_ResumeGC().
 *
 * @param data          Group-specific data.
 * @param performGc     Whether to perform GC eventually.
 *
 * @sideeffect
 *      If **performGC** is nonzero, calls PerformGC() eventually: synchronously
 *      if model is #COL_SINGLE, else asynchronously.
 *
 * @see Col_ResumeGC
 * @see PlatSyncResumeGC
 * @see PerformGC
 * @see SyncPauseGC
 * @see TrySyncPauseGC
 */
#ifdef COL_USE_THREADS
#   define SyncResumeGC(data, performGc) \
        if ((data)->model != COL_SINGLE) {PlatSyncResumeGC((data), (performGc));} \
        else if (performGc) {PerformGC(data);}
#else
#   define SyncResumeGC(data, performGc) \
        if (performGc) {PerformGC(data);}
#endif /* COL_USE_THREADS */

/*
 * Remaining declarations.
 */

int                     PlatEnter(unsigned int model);
int                     PlatLeave(void);
#ifdef COL_USE_THREADS
void                    PlatEnterProtectRoots(GroupData *data);
void                    PlatLeaveProtectRoots(GroupData *data);
void                    PlatSyncPauseGC(GroupData *data);
int                     PlatTrySyncPauseGC(GroupData *data);
void                    PlatSyncResumeGC(GroupData *data, int schedule);
#endif /* COL_USE_THREADS */

/* End of Process & Threads *//*!\}*/


/***************************************************************************//*!
 * \name System Page Allocation
 ***************************************************************************\{*/

extern size_t systemPageSize;
extern size_t allocGranularity;
extern size_t shiftPage;

void *                  PlatReserveRange(size_t size, int alloc);
int                     PlatReleaseRange(void *base, size_t size);
int                     PlatAllocPages(void *addr, size_t number);
int                     PlatFreePages(void *addr, size_t number);
int                     PlatProtectPages(void *addr, size_t number,
                                int protect);

/* End of System Page Allocation *//*!\}*/

/* End of System and Architecture *//*!\}*/

#endif /* _COLIBRI_PLATFORM */
/*! @endcond @endprivate */