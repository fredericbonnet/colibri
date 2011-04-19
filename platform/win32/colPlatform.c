#include "../../colibri.h"
#include "../../colInternal.h"
#include "../../colPlatform.h"

#include <windows.h>

static DWORD tlsToken;
typedef struct {
    size_t nestCount;
    GcMemInfo gcMemInfo; 
    Col_ErrorProc *errorProc;
} ThreadData;


/*
 * Prototypes for functions used only in this file.
 */

static BOOL Init(void);


/*
 *----------------------------------------------------------------
 * System page allocator.
 *----------------------------------------------------------------
 */

/*
 * Memory is allocated with VirtualAlloc. As it can't reserve less than 
 * SYSTEM_INFO.dwAllocationGranularity (65536 on IA32) but can commit individual
 * pages with a size of SYSTEM_INFO.dwPageSize (4096 on IA32), this means that 
 * each logical page is part of a system page that is itself part of a larger
 * address range. Allocating whole SYSTEM_INFO.dwAllocationGranularity sized
 * pages would be too heavy.
 *
 * In order to keep track of committed pages in ranges, use the PAGE_RESERVED 
 * field of the first logical page in each system page to store the index of 
 * the next committed system page in range, forming a circular list (last one 
 * points to first). Indices must be consecutive. That way we can scan a range 
 * for holes from any of its page.
 *
 * An alternative was to use VirtualQuery to get the committed and free pages 
 * in a range, but this proved to be too CPU expensive, and VirtualQuery is
 * known to be much slower on Win64.
 */

static SYSTEM_INFO systemInfo;
static size_t pagesPerRange;

#define PHYS_PAGE(range, index)	\
    ((void *)((char *)(range)+systemInfo.dwPageSize*(index)))
#define PHYS_PAGE_RANGE(page)	\
    ((void *)((uintptr_t)(page) & ~(systemInfo.dwAllocationGranularity-1)))
#define PHYS_PAGE_INDEX(page)	\
    ((unsigned short)(((uintptr_t)(page) % systemInfo.dwAllocationGranularity) / systemInfo.dwPageSize))
#define PHYS_PAGE_NEXT(page)	PAGE_RESERVED(page)

/*
 * Use the data field of the MemoryPool to store the last page belonging to a 
 * range with free pages. 
 */

#define lastFreeRange data

/*
 *---------------------------------------------------------------------------
 *
 * PlatSysPageAlloc --
 *
 *	Allocate a system page.
 *
 * Results:
 *	The new system page.
 *
 * Side effects:
 *	A new page within a new or existing range is allocated.
 *
 *---------------------------------------------------------------------------
 */

void * 
PlatSysPageAlloc(
    MemoryPool * pool)		/* The pool for which to alloc page. */
{
    void *base, *page;

    /* 
     * Look for an address range with a free page. 
     */

    for (page = pool->lastFreeRange; page; page = LAST_PAGE_NEXT(page)) {
	unsigned short index, prev, next;

	/* 
	 * Search hole in page index list. 
	 */

	base = PHYS_PAGE_RANGE(page);
	index = PHYS_PAGE_INDEX(page);
	next = PHYS_PAGE_NEXT(page);
	prev = index;
	if (next == index) {
	    /* 
	     * Page is alone in range, allocate next one. 
	     */

	    next = (prev+1)%pagesPerRange;
	} else {
	    /*
	     * Search next hole if any by comparing indices, which must be
	     * consecutive.
	     */

	    do {
		if ((next-prev)%pagesPerRange > 1) {
		    /* 
		     * Hole found. 
		     */

		    next = (prev+1)%pagesPerRange;
		    break;
		}
		prev = next;
		next = PHYS_PAGE_NEXT(PHYS_PAGE(base, prev));
	    } while (next != index);
	}
	if (next != index) {
	    /* 
	     * Hole found, commit page.
	     */

	    page = PHYS_PAGE(base, next);
	    if (!VirtualAlloc(page, systemInfo.dwPageSize, MEM_COMMIT, 
		    PAGE_READWRITE)) {
		/*
		 * Fatal error!
		 */

		Col_Error(COL_FATAL, "VirtualAlloc: Error %u",
			GetLastError());
		return NULL;
	    }

	    /* 
	     * Insert into list after <index>.
	     */

	    PHYS_PAGE_NEXT(page) = PHYS_PAGE_NEXT(PHYS_PAGE(base, prev));
	    PHYS_PAGE_NEXT(PHYS_PAGE(base, prev)) = next;

	    break;
	}
    }

    if (!page) {
	/* 
	 * No room in existing ranges, reserve new one and allocate first page. 
	 */

	if (!(base = VirtualAlloc(NULL, systemInfo.dwAllocationGranularity, 
		MEM_RESERVE, PAGE_READWRITE)) || !(page = VirtualAlloc(base, 
		systemInfo.dwPageSize, MEM_COMMIT, PAGE_READWRITE))) {
	    /* 
	     * Fatal error!
	     */

	    Col_Error(COL_FATAL, "VirtualAlloc: Error %u",
		    GetLastError());
	    return NULL;
	}

	PHYS_PAGE_NEXT(page) = 0;
    }

    pool->lastFreeRange = page;
    return page;
}

/*
 *---------------------------------------------------------------------------
 *
 * PlatSysPageFree --
 *
 *	Free a system page.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The page is freed, along with its range when empty.
 *
 *---------------------------------------------------------------------------
 */

void 
PlatSysPageFree(
    MemoryPool * pool,		/* The pool the page belongs to. */
    void * page)		/* The page to free. */
{
    unsigned short index = PHYS_PAGE_INDEX(page);
    if (PHYS_PAGE_NEXT(page) == index) {
	/* 
	 * Last and only page in range, release whole range. 
	 */

	VirtualFree(PHYS_PAGE_RANGE(page), 0, MEM_RELEASE);
    } else {
	/* 
	 * Unlink and decommit first page. 
	 */

	void *base = PHYS_PAGE_RANGE(page);
	unsigned short prev, next;
	for (prev = index, next = PHYS_PAGE_NEXT(PHYS_PAGE(base, prev)); 
		next != index; prev = next, 
		next = PHYS_PAGE_NEXT(PHYS_PAGE(base, next)));
	PHYS_PAGE_NEXT(PHYS_PAGE(base, prev)) = PHYS_PAGE_NEXT(page);
	VirtualFree(page, systemInfo.dwPageSize, MEM_DECOMMIT);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * PlatSysPageCleanup --
 *
 *	Final cleanup page after pages have been freed in pool.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Reset range pointer.
 *
 *---------------------------------------------------------------------------
 */

void 
PlatSysPageCleanup(MemoryPool * pool) {
    pool->lastFreeRange = pool->pages;
}


/*
 *----------------------------------------------------------------
 * Mark-and-sweep, generational, exact GC.
 *----------------------------------------------------------------
 */

#ifdef COL_THREADS

/*
 *---------------------------------------------------------------------------
 *
 * PlatGetGcMemInfo --
 * PlatGetErrorProcPtr --
 *
 *	Get thread-local info.
 *
 * Results:
 *	Pointer to thread-local info.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

GcMemInfo *
PlatGetGcMemInfo() {
    return &((ThreadData *) TlsGetValue(tlsToken))->gcMemInfo;
}

Col_ErrorProc **
PlatGetErrorProcPtr() {
    return &((ThreadData *) TlsGetValue(tlsToken))->errorProc;
}

#endif /* COL_THREADS */


/*
 *----------------------------------------------------------------
 * Process & threads.
 *----------------------------------------------------------------
 */

/*
 *---------------------------------------------------------------------------
 *
 * DllMain --
 *
 *	Windows DLL entry point.
 *
 * Results:
 *	True.
 *
 * Side effects:
 *	Create thread-local storage token.
 *
 *---------------------------------------------------------------------------
 */

BOOL APIENTRY 
DllMain( 
    HMODULE hModule,
    DWORD dwReason,
    LPVOID lpReserved)
{
    switch (dwReason) {
	case DLL_PROCESS_ATTACH:
	    return Init();

	case DLL_PROCESS_DETACH:
	    TlsFree(tlsToken);
	    break;
    }
    return TRUE;
}

/*
 *---------------------------------------------------------------------------
 *
 * Init --
 *
 *	Initialization routine. Called through DllMain.
 *
 * Results:
 *	True.
 *
 * Side effects:
 *	Create thread-local storage key. Note: key is never freed.
 *
 *---------------------------------------------------------------------------
 */

static BOOL
Init()
{
    if ((tlsToken = TlsAlloc()) == TLS_OUT_OF_INDEXES) {
	return FALSE;
    }

    GetSystemInfo(&systemInfo);
    systemPageSize = systemInfo.dwPageSize;
    pagesPerRange = systemInfo.dwAllocationGranularity/systemInfo.dwPageSize;

    return TRUE;
}

/*
 *---------------------------------------------------------------------------
 *
 * PlatEnter --
 *
 *	Enter the thread.
 *
 * Results:
 *	Non-zero if this is the first nested call, else 0.
 *
 * Side effects:
 *	Global data is initialized; decrement nesting count.
 *
 *---------------------------------------------------------------------------
 */

int
PlatEnter()
{
    ThreadData *info = (ThreadData *) TlsGetValue(tlsToken);
    if (!info) {
	/*
	 * Not yet initialized.
	*/

	info = (ThreadData *) malloc(sizeof(ThreadData));
	info->nestCount = 0;
	TlsSetValue(tlsToken, info);
    }

    /* 
     * Increment nest count. 
     */

    return ((++info->nestCount) == 1);
}

/*
 *---------------------------------------------------------------------------
 *
 * PlatLeave --
 *
 *	Leave the thread.
 *
 * Results:
 *	Non-zero if this is the last nested call, else 0.
 *
 * Side effects:
 *	Decrement nesting count.
 *
 *---------------------------------------------------------------------------
 */

int
PlatLeave()
{
    ThreadData *info = (ThreadData *) TlsGetValue(tlsToken);
    if (!info) {
	/* TODO: exception ? */
	return 0;
    }

    /*
     * Decrement nest count.
     */

    return ((--info->nestCount) == 0);
}

/*
 *---------------------------------------------------------------------------
 *
 * PlatCleanup --
 *
 *	Final cleanup.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Cleanup global structures.
 *
 *---------------------------------------------------------------------------
 */

void
PlatCleanup()
{
    ThreadData *info = (ThreadData *) TlsGetValue(tlsToken);
    if (!info || info->nestCount != 0) {
	/* TODO: exception ? */
	return;
    }

    free(info);
    TlsSetValue(tlsToken, 0);
}
