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
 * Logical page after the last one in system page.
 */

#define LAST_PAGE_NEXT(page)	\
    PAGE_NEXT(((char *)(page) + systemPageSize - PAGE_SIZE))

/*
 * Allocation/deallocation of system pages.
 */

void *			PlatSysPageAlloc(MemoryPool * pool);
void			PlatSysPageFree(MemoryPool * pool, void * page);
void			PlatSysPageCleanup(MemoryPool * pool);


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
