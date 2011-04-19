#ifndef _COLIBRI_PLATFORM
#define _COLIBRI_PLATFORM

/*
 *----------------------------------------------------------------
 * Process & thread.
 *----------------------------------------------------------------
 */

int			PlatEnter(void);
int			PlatLeave(void);
void			PlatCleanup(void);


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


/*
 *----------------------------------------------------------------
 * Mark-and-sweep, generational, exact GC.
 *----------------------------------------------------------------
 */

/*
 * Thread-local storage.
 */

#ifdef COL_THREADS
GcMemInfo *		PlatGetGcMemInfo(void);
Col_ErrorProc **	PlatGetErrorProcPtr(void);
#endif

#endif /* _COLIBRI_PLATFORM */
