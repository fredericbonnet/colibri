/*                                                                              *//*!   @cond PRIVATE @file \
 * colPlatform.h
 *
 *  This header file defines the generic primitives needing platform-specific
 *  implementations.
 *
 *  @see platform/win32/colWin32Platform.h
 *  @see platform/unix/colUnixPlatform.h
 *
 *  @private
 */

#ifndef _COLIBRI_PLATFORM
#define _COLIBRI_PLATFORM
                                                                                #       ifndef DOXYGEN
#if defined(__WIN32__)
#   include "platform/win32/colWin32Platform.h"
#else /* FIXME */
#   include "platform/unix/colUnixPlatform.h"
#endif
                                                                                #       endif DOXYGEN
                                                                                

/*
================================================================================*//*!   @addtogroup arch \
System and Architecture

  Generic primitives needing platform-specific implementations.                 *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Process & Threads                                                            *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * EnterProtectRoots
 *
 *  Enter protected section around root management structures.
 *
 *  @param data     Group-specific data.
 *
 *  @sideeffect
 *      When model is #COL_SHARED, calls PlatEnterProtectRoots().
 *
 *  @see PlatEnterProtectRoots
 *  @see LeaveProtectRoots
 *//*-----------------------------------------------------------------------*/

#define EnterProtectRoots(data) \
    if ((data)->model >= COL_SHARED) {PlatEnterProtectRoots(data);}

/*---------------------------------------------------------------------------   *//*!   @def \
 * LeaveProtectRoots
 *
 *  Leave protected section around root management structures.
 *
 *  @param data     Group-specific data.
 *
 *  @sideeffect
 *      When model is #COL_SHARED, calls PlatLeaveProtectRoots().
 *//*-----------------------------------------------------------------------*/

#define LeaveProtectRoots(data) \
    if ((data)->model >= COL_SHARED) {PlatLeaveProtectRoots(data);}

/*---------------------------------------------------------------------------   *//*!   @def \
 * SyncPauseGC
 *
 *  Synchronize calls to Col_PauseGC().
 *
 *  @param data     Group-specific data.
 *
 *  @sideeffect
 *      When model isn't #COL_SINGLE, calls PlatSyncPauseGC(), which may block
 *      as long as a GC is underway.
 *
 *  @see Col_PauseGC
 *  @see PlatSyncPauseGC
 *  @see SyncResumeGC
 *//*-----------------------------------------------------------------------*/

#define SyncPauseGC(data) \
    if ((data)->model != COL_SINGLE) {PlatSyncPauseGC(data);}

/*---------------------------------------------------------------------------   *//*!   @def \
 * TrySyncPauseGC
 *
 *  Synchronize calls to Col_TryPauseGC().
 *
 *  @param data     Group-specific data.
 *
 *  @retval 1       if successful
 *  @retval 0       if a GC is underway (this implies the threading model isn't
 *                  #COL_SINGLE).
 *
 *  @sideeffect
 *      When model isn't #COL_SINGLE, calls PlatTrySyncPauseGC().
 *
 *  @see Col_TryPauseGC
 *  @see PlatTrySyncPauseGC
 *  @see SyncResumeGC
 *//*-----------------------------------------------------------------------*/

#define TrySyncPauseGC(data) \
    ((data)->model == COL_SINGLE ? 1 : PlatTrySyncPauseGC(data))

/*---------------------------------------------------------------------------   *//*!   @def \
 * SyncResumeGC
 *
 *  Synchronize calls to Col_ResumeGC().
 *
 *  @param data         Group-specific data.
 *  @param performGc    Whether to perform GC eventually.
 *
 *  @sideeffect
 *      If **performGC** is nonzero, calls PerformGC() eventually: synchronously
 *      if model is #COL_SINGLE, else asynchronously.
 *
 *  @see Col_ResumeGC
 *  @see PlatSyncResumeGC
 *  @see PerformGC
 *  @see SyncPauseGC
 *  @see TrySyncPauseGC
 *//*-----------------------------------------------------------------------*/

#define SyncResumeGC(data, performGc) \
    if ((data)->model != COL_SINGLE) {PlatSyncResumeGC((data), (performGc));} \
    else if (performGc) {PerformGC(data);}

/*
 * Remaining declarations.
 */
                                                                                #       ifndef DOXYGEN
int                     PlatEnter(unsigned int model);
int                     PlatLeave(void);
void                    PlatEnterProtectRoots(GroupData *data);
void                    PlatLeaveProtectRoots(GroupData *data);
void                    PlatSyncPauseGC(GroupData *data);
int                     PlatTrySyncPauseGC(GroupData *data);
void                    PlatSyncResumeGC(GroupData *data, int schedule);
                                                                                #       endif DOXYGEN
                                                                                /*!     @} */
/********************************************************************************//*!   @name \
 * System Page Allocation                                                       *//*!   @{ *//*
 ******************************************************************************/
                                                                                #       ifndef DOXYGEN
extern size_t systemPageSize;
extern size_t allocGranularity;
extern size_t shiftPage;

void *                  PlatReserveRange(size_t size, int alloc);
int                     PlatReleaseRange(void *base, size_t size);
int                     PlatAllocPages(void *addr, size_t number);
int                     PlatFreePages(void *addr, size_t number);
int                     PlatProtectPages(void *addr, size_t number,
                                int protect);
                                                                                #       endif DOXYGEN
                                                                                /*!     @} */
                                                                                /*!     @} */
#endif /* _COLIBRI_PLATFORM */
                                                                                /*!     @endcond */