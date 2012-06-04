/*
 * Internal Header: colPlatform.h
 *
 *	This header file defines the generic features needing platform-specific 
 *	implementations.
 */

#ifndef _COLIBRI_PLATFORM
#define _COLIBRI_PLATFORM

#if defined(__WIN32__)
#   include "platform/win32/colWin32Platform.h"
#else /* FIXME */
#   include "platform/unix/colUnixPlatform.h"
#endif


/****************************************************************************
 * Internal Section: Process & Threads
 *
 * Declarations:
 *	<PlatEnter>, <PlatLeave>, <PlatEnterProtectRoots>, 
 *	<PlatLeaveProtectRoots>, <PlatSyncPauseGC>, <PlatTrySyncPauseGC>, 
 *	<PlatSyncResumeGC>
 ****************************************************************************/

int			PlatEnter(unsigned int model);
int			PlatLeave(void);
void			PlatEnterProtectRoots(GroupData *data);
void			PlatLeaveProtectRoots(GroupData *data);
void			PlatSyncPauseGC(GroupData *data);
int			PlatTrySyncPauseGC(GroupData *data);
void			PlatSyncResumeGC(GroupData *data, int schedule);

/*---------------------------------------------------------------------------
 * Internal Macro: EnterProtectRoots
 *
 *	Enter protected section around root management structures.
 *
 * Argument:
 *	data	- Group-specific data.
 *
 * Side effects:
 *	When model is <COL_SHARED>, calls <PlatEnterProtectRoots>.
 *
 * See also:
 *	<PlatEnterProtectRoots>, <LeaveProtectRoots>
 *---------------------------------------------------------------------------*/

#define EnterProtectRoots(data) \
    if ((data)->model >= COL_SHARED) {PlatEnterProtectRoots(data);}

/*---------------------------------------------------------------------------
 * Internal Macro: LeaveProtectRoots
 *
 *	Leave protected section around root management structures.
 *
 * Argument:
 *	data	- Group-specific data.
 *
 * Side effects:
 *	When model is <COL_SHARED>, calls <PlatLeaveProtectRoots>.
 *
 * See also:
 *	<PlatLeaveProtectRoots>, <EnterProtectRoots>
 *---------------------------------------------------------------------------*/

#define LeaveProtectRoots(data) \
    if ((data)->model >= COL_SHARED) {PlatLeaveProtectRoots(data);}

/*---------------------------------------------------------------------------
 * Internal Macro: SyncPauseGC
 *
 *	Synchronize calls to <Col_PauseGC>.
 *
 * Argument:
 *	data	- Group-specific data.
 *
 * Side effects:
 *	When model isn't <COL_SINGLE>, calls <PlatSyncPauseGC>, which may block
 *	as long as a GC is underway.
 *
 * See also:
 *	<Col_PauseGC>, <PlatSyncPauseGC>, <SyncResumeGC>
 *---------------------------------------------------------------------------*/

#define SyncPauseGC(data) \
    if ((data)->model != COL_SINGLE) {PlatSyncPauseGC(data);}

/*---------------------------------------------------------------------------
 * Internal Macro: TrySyncPauseGC
 *
 *	Synchronize calls to <Col_TryPauseGC>.
 *
 * Argument:
 *	data	- Group-specific data.
 *
 * Result:
 *	1 if successful, 0 if a GC is underway (this implies the threading model
 *	isn't <COL_SINGLE>).
 *
 * Side effects:
 *	When model isn't <COL_SINGLE>, calls <PlatTrySyncPauseGC>.
 *
 * See also:
 *	<Col_TryPauseGC>, <PlatTrySyncPauseGC>, <SyncResumeGC>
 *---------------------------------------------------------------------------*/

#define TrySyncPauseGC(data) \
    ((data)->model == COL_SINGLE ? 1 : PlatTrySyncPauseGC(data))

/*---------------------------------------------------------------------------
 * Internal Macro: SyncResumeGC
 *
 *	Synchronize calls to <Col_ResumeGC>.
 *
 * Arguments:
 *	data		- Group-specific data.
 *	performGc	- Whether to perform GC eventually.
 *
 * Side effects:
 *	If performGC is nonzero, calls <PerformGC> eventually: synchronously
 *	if model is <COL_SINGLE>, else asynchronously.
 *
 * See also:
 *	<Col_ResumeGC>, <PlatSyncResumeGC>, <PerformGC>, <SyncPauseGC>, 
 *	<TrySyncPauseGC>
 *---------------------------------------------------------------------------*/

#define SyncResumeGC(data, performGc) \
    if ((data)->model != COL_SINGLE) {PlatSyncResumeGC((data), (performGc));} \
    else if (performGc) {PerformGC(data);} \


/****************************************************************************
 * Internal Section: System Page Allocation
 *
 * Declarations:
 *	<systemPageSize>, <allocGranularity>, <shiftPage>, 
 *	<PlatReserveRange>, <PlatReleaseRange>, <PlatAllocPages>,
 *	<PlatFreePages>
 ****************************************************************************/

extern size_t systemPageSize;
extern size_t allocGranularity;
extern size_t shiftPage;

void *			PlatReserveRange(size_t size, int alloc);
int			PlatReleaseRange(void *base, size_t size);
int			PlatAllocPages(void *addr, size_t number);
int			PlatFreePages(void *addr, size_t number);
int			PlatProtectPages(void *addr, size_t number, 
				int protect);

#endif /* _COLIBRI_PLATFORM */
