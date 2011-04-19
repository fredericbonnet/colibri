#ifndef _COLIBRI_PLATFORM
#define _COLIBRI_PLATFORM

#if defined(__WIN32__)
#   include "platform/win32/colWin32Platform.h"
#else /* FIXME */
#   include "platform/unix/colUnixPlatform.h"
#endif

/*
 *----------------------------------------------------------------
 * Process & thread.
 *----------------------------------------------------------------
 */

int			PlatEnter(unsigned int model);
int			PlatLeave(void);
void			PlatEnterProtectRoots(GroupData *data);
void			PlatLeaveProtectRoots(GroupData *data);
void			PlatEnterProtectDirties(GroupData *data);
void			PlatLeaveProtectDirties(GroupData *data);
void			PlatSyncPauseGC(GroupData *data);
int			PlatTrySyncPauseGC(GroupData *data);
void			PlatSyncResumeGC(GroupData *data, int schedule);

#define EnterProtectRoots(groupData) \
    if ((groupData)->model >= COL_SHARED) {PlatEnterProtectRoots(groupData);}
#define LeaveProtectRoots(groupData) \
    if ((groupData)->model >= COL_SHARED) {PlatLeaveProtectRoots(groupData);}
#define EnterProtectDirties(groupData) \
    if ((groupData)->model >= COL_SHARED) {PlatEnterProtectDirties(groupData);}
#define LeaveProtectDirties(groupData) \
    if ((groupData)->model >= COL_SHARED) {PlatLeaveProtectDirties(groupData);}

#define SyncPauseGC(groupData) \
    if ((groupData)->model != COL_SINGLE) {PlatSyncPauseGC(groupData);}
#define TrySyncPauseGC(groupData) \
    ((groupData)->model == COL_SINGLE ? 1 : PlatTrySyncPauseGC(groupData))
#define SyncResumeGC(groupData, schedule) \
    if ((groupData)->model != COL_SINGLE) {PlatSyncResumeGC((groupData), (schedule));} \
    else if (schedule) {PerformGC(groupData);} \


/*
 *----------------------------------------------------------------
 * System page allocator.
 *----------------------------------------------------------------
 */

/*
 * System page size. Should be a multiple of PAGE_SIZE on every possible 
 * platform.
 */

extern size_t systemPageSize;

/*
 * Allocation/deallocation of system pages.
 */

void *			PlatSysPageAlloc(size_t number);
void			PlatSysPageFree(void * page, size_t number);

#endif /* _COLIBRI_PLATFORM */
