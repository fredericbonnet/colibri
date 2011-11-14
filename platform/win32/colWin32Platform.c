/*
 * Internal File: colWin32Platform.c
 *
 *	This Win32-specific file implements the generic features needing 
 *	platform-specific implementations, as well as the Win32-specific
 *	features.
 *
 * See also:
 *	<colPlatform.h>, <colWin32Platform.h>
 */

#include "../../colibri.h"
#include "../../colInternal.h"
#include "../../colPlatform.h"

#include <windows.h>
#include <sys/types.h>

/*
 * Bit twiddling hack for computing the log2 of a power of 2.
 * See: http://www-graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
 */

static const int MultiplyDeBruijnBitPosition2[32] = 
{
  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};
#define LOG2(v) MultiplyDeBruijnBitPosition2[(uint32_t)(v * 0x077CB531U) >> 27]

/*
 * Prototypes for functions used only in this file.
 */

static struct PlatGroupData * AllocGroupData(unsigned int model);
static void		FreeGroupData(struct PlatGroupData *groupData);
static DWORD WINAPI	GcThreadProc(LPVOID lpParameter);
static BOOL		Init(void);


/****************************************************************************
 * Internal Group: Thread-Local Storage
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Variable: tlsToken
 *
 *	Thread-local storage identifier. Used to get thread-specific data.
 *
 * See also:
 *	<ThreadData>, <Init>
 *---------------------------------------------------------------------------*/

DWORD tlsToken;

/*---------------------------------------------------------------------------
 * Internal Typedef: PlatGroupData
 *
 *	Platform-specific group data.
 *
 * Fields:
 *	data			- Generic <GroupData> structure.
 *	next			- Next active group in list.
 *	csRoots			- Critical section protecting root management.
 *				  management.
 *	csGc			- Critical section protecting GC from worker 
 *				  threads.
 *	eventGcScheduled	- Triggers GC thread.
 *	eventGcDone		- Barrier for worker threads.
 *	scheduled		- Flag for when a GC is scheduled.
 *	terminated		- Flag for thread group destruction.
 *	nbActive		- Active worker thread counter.
 *	threadGc		- GC thread.
 *
 * See also:
 *	<ThreadData>, <GroupData>, <Init>, <AllocGroupData>, <FreeGroupData>
 *---------------------------------------------------------------------------*/

typedef struct PlatGroupData {
    GroupData data;
    struct PlatGroupData *next;
    CRITICAL_SECTION csRoots;
    
    CRITICAL_SECTION csGc;
    HANDLE eventGcScheduled;
    HANDLE eventGcDone;
    int scheduled;
    int terminated;
    int nbActive;
    HANDLE threadGc;
} PlatGroupData;

/*---------------------------------------------------------------------------
 * Internal Variable: sharedGroups
 *
 *	List of active groups in process.
 *
 * See also:
 *	<PlatGroupData>, <AllocGroupData>, <FreeGroupData>
 *---------------------------------------------------------------------------*/

static PlatGroupData *sharedGroups;

/*---------------------------------------------------------------------------
 * Internal Variable: csSharedGroups
 *
 *	Critical section protecting <sharedGroups>.
 *
 * See also:
 *	<sharedGroups>
 *---------------------------------------------------------------------------*/

static CRITICAL_SECTION csSharedGroups;

/*---------------------------------------------------------------------------
 * Internal Function: AllocGroupData
 *
 *	Allocate and initialize a thread group data structure.
 *
 * Argument:
 *	model	- Threading model.
 *
 * Result:
 *	The newly allocated structure.
 *
 * Side effects:
 *	Memory allocated and system objects created.
 *
 * See also:
 *	<Threading Models>, <PlatGroupData>, <FreeGroupData>
 *---------------------------------------------------------------------------*/

static PlatGroupData *
AllocGroupData(
    unsigned int model)
{
    PlatGroupData *groupData = (PlatGroupData *) malloc(sizeof(PlatGroupData));
    memset(groupData, 0, sizeof(PlatGroupData));
    groupData->data.model = model;
    GcInitGroup((GroupData *) groupData);

    if (model != COL_SINGLE) {
	/*
	 * Create synchronization objects.
	 */

	//TODO error handling.
	InitializeCriticalSection(&groupData->csRoots);
	
	InitializeCriticalSection(&groupData->csGc);
	groupData->eventGcDone = CreateEvent(NULL, TRUE, TRUE, NULL);
	groupData->eventGcScheduled = CreateEvent(NULL, FALSE, FALSE, NULL);

	/*
	 * Create GC thread.
	 */

	groupData->threadGc = CreateThread(NULL, 0, GcThreadProc, groupData,
		0, NULL);
    }

    return groupData;
}

/*---------------------------------------------------------------------------
 * Internal Function: FreeGroupData
 *
 *	Free a thread group data structure.
 *
 * Argument:
 *	groupData	- Structure to free.
 *
 * Side effects:
 *	Memory freed and system objects deleted.
 *
 * See also:
 *	<PlatGroupData>, <AllocGroupData>
 *---------------------------------------------------------------------------*/

static void
FreeGroupData(
    PlatGroupData *groupData)
{
    if (groupData->data.model != COL_SINGLE) {
	/*
	 * Signal and wait for termination of GC thread.
	 */

	//TODO error handling.
	groupData->terminated = 1;
	SignalObjectAndWait(groupData->eventGcScheduled, groupData->threadGc,
	    INFINITE, FALSE);

	/*
	 * Destroy synchronization objects.
	 */

	DeleteObject(groupData->eventGcScheduled);
	DeleteObject(groupData->eventGcDone);
	DeleteCriticalSection(&groupData->csGc);

	DeleteCriticalSection(&groupData->csRoots);
    }

    GcCleanupGroup((GroupData *) groupData);
    free(groupData);
}


/****************************************************************************
 * Internal Group: Process & Threads
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Function: PlatEnter
 *
 *	Enter the thread. If this is the first nested call, initialize thread
 *	data. If this is the first thread in its group, initialize group data
 *	as well.
 *
 * Argument:
 *	model	- Threading model.
 *
 * Result:
 *	Non-zero if this is the first nested call, else 0.
 *
 * See also:
 *	<Threading Models>, <ThreadData>, <PlatGroupData>, <PlatLeave>, 
 *	<Col_Init>
 *---------------------------------------------------------------------------*/

int
PlatEnter(
    unsigned int model)
{
    ThreadData *data = PlatGetThreadData();
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
    TlsSetValue(tlsToken, data);

    if (model == COL_SINGLE || model == COL_ASYNC) {
	/*
	 * Allocate dedicated group.
	 */

	data->groupData = (GroupData *) AllocGroupData(model);
	data->groupData->first = data;
	data->next = data;
    } else {
	/*
	 * Try to find shared group with same model value.
	 */

	EnterCriticalSection(&csSharedGroups);
	{
	    PlatGroupData *groupData = sharedGroups;
	    while (groupData && groupData->data.model != model) {
		groupData = groupData->next;
	    }
	    if (!groupData) {
		/*
		 * Allocate new group and insert at head.
		 */

		groupData = AllocGroupData(model);
		groupData->next = sharedGroups;
		sharedGroups = groupData;
	    }

	    /*
	     * Add thread to group.
	     */

	    data->groupData = (GroupData *) groupData;
	    if (data->groupData->first) {
		data->next = data->groupData->first->next;
		data->groupData->first->next = data;
	    } else {
		data->groupData->first = data;
		data->next = data;
	    }
	}
	LeaveCriticalSection(&csSharedGroups);
    }

    return 1;
}

/*---------------------------------------------------------------------------
 * Internal Function: PlatLeave
 *
 *	Leave the thread. If this is the first nested call, cleanup thread
 *	data. If this is the last thread in its group, cleanup group data
 *	as well.
 *
 * Result:
 *	Non-zero if this is the last nested call, else 0.
 *
 * See also:
 *	<ThreadData>, <PlatGroupData>, <PlatEnter>, <Col_Cleanup>
 *---------------------------------------------------------------------------*/

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

    if (data->groupData->model == COL_SINGLE || data->groupData->model 
	    == COL_ASYNC) {
	/*
	 * Free dedicated group as well.
	 */

	FreeGroupData((PlatGroupData *) data->groupData);
    } else {
	/*
	 * Remove from shared group.
	 */

	EnterCriticalSection(&csSharedGroups);
	{
	    if (data->next == data) {
		/*
		 * Free group as well.
		 */

		FreeGroupData((PlatGroupData *) data->groupData);
	    } else {
		/*
		 * Unlink.
		 */

		ThreadData *prev = data->next;
		while (prev->next != data) {
		    prev = prev->next;
		}
		prev->next = data->next;
		data->groupData->first = prev;
	    }
	}
	LeaveCriticalSection(&csSharedGroups);
    }

    GcCleanupThread(data);
    free(data);
    TlsSetValue(tlsToken, 0);

    return 1;
}

/*---------------------------------------------------------------------------
 * Internal Function: GcThreadProc
 *
 *	Thread dedicated to the GC process. Activated when one of the worker
 *	threads in the group triggers the GC.
 *
 * Argument:
 *	lpParameter	- <PlatGroupData>.
 *
 * Result:
 *	Always zero.
 *
 * Side effects:
 *	Calls <PerformGC>.
 *
 * See also:
 *	<AllocGroupData>, <PerformGC>
 *---------------------------------------------------------------------------*/

static DWORD WINAPI
GcThreadProc(
    LPVOID lpParameter)
{
    PlatGroupData *groupData = (PlatGroupData *) lpParameter;
    for (;;) {
	WaitForSingleObject(groupData->eventGcScheduled, INFINITE);
	EnterCriticalSection(&groupData->csGc);
	{
	    if (groupData->scheduled) {
		groupData->scheduled = 0;
		PerformGC((GroupData *) groupData);
	    }
	    SetEvent(groupData->eventGcDone);
	}
	LeaveCriticalSection(&groupData->csGc);
	if (groupData->terminated) {
	    ExitThread(0);
	}
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: PlatSyncPauseGC
 *
 *	Called when a worker thread calls the outermost <Col_PauseGC>.
 *
 * Argument:
 *	data	- Group-specific data.
 *
 * Side effects:
 *	May block as long as a GC is underway.
 * 
 * See also:
 *	<GcThreadProc>, <SyncPauseGC>, <Col_PauseGC>
 *---------------------------------------------------------------------------*/

void
PlatSyncPauseGC(
    GroupData *data) 
{
    PlatGroupData *groupData = (PlatGroupData *) data;
    ASSERT(groupData->data.model != COL_SINGLE);
    WaitForSingleObject(groupData->eventGcDone, INFINITE);
    EnterCriticalSection(&groupData->csGc);
    {
	groupData->nbActive++;
    }
    LeaveCriticalSection(&groupData->csGc);
}

/*---------------------------------------------------------------------------
 * Internal Function: PlatTrySyncPauseGC
 *
 *	Called when a worker thread calls the outermost <Col_TryPauseGC>.
 *
 * Argument:
 *	data	- Group-specific data.
 *
 * Result:
 *	Non-zero if successful.
 * 
 * See also:
 *	<GcThreadProc>, <TrySyncPauseGC>, <Col_TryPauseGC>
 *---------------------------------------------------------------------------*/

int
PlatTrySyncPauseGC(
    GroupData *data) 
{
    PlatGroupData *groupData = (PlatGroupData *) data;
    ASSERT(groupData->data.model != COL_SINGLE);
    if (WaitForSingleObject(groupData->eventGcDone, 0) 
	    != WAIT_OBJECT_0) {
	return 0;
    }
    EnterCriticalSection(&groupData->csGc);
    {
	groupData->nbActive++;
    }
    LeaveCriticalSection(&groupData->csGc);
    return 1;
}

/*---------------------------------------------------------------------------
 * Internal Function: PlatSyncResumeGC
 *
 *	Called when a worker thread calls the outermost <Col_ResumeGC>.
 *
 * Arguments:
 *	data		- Group-specific data.
 *	performGc	- Whether to perform GC. 
 *
 * Side effects:
 *	If last thread in group, may trigger the GC in the dedicated thread if 
 *	previously scheduled. This will block further calls to 
 *	<Col_PauseGC>/<PlatSyncPauseGC>.
 *
 * See also:
 *	<GcThreadProc>, <SyncResumeGC>, <Col_ResumeGC>
 *---------------------------------------------------------------------------*/

void
PlatSyncResumeGC(
    GroupData *data,
    int performGc) 
{
    PlatGroupData *groupData = (PlatGroupData *) data;
    ASSERT(groupData->data.model != COL_SINGLE);
    EnterCriticalSection(&groupData->csGc);
    {
	if (performGc && !groupData->scheduled) {
	    ResetEvent(groupData->eventGcDone);
	    groupData->scheduled = 1;
	}
	--groupData->nbActive;
	if (!groupData->nbActive && groupData->scheduled) {
	    SetEvent(groupData->eventGcScheduled);
	}
    }
    LeaveCriticalSection(&groupData->csGc);
}

/*---------------------------------------------------------------------------
 * Internal Function: PlatEnterProtectRoots
 *
 *	Enter protected section around root management structures.
 *
 * Argument:
 *	data	- Group-specific data.
 *
 * Side effects:
 *	Blocks until no thread owns the section.
 *
 * See also:
 *	<PlatLeaveProtectRoots>, <EnterProtectRoots>
 *---------------------------------------------------------------------------*/

void
PlatEnterProtectRoots(
    GroupData *data)
{
    PlatGroupData *groupData = (PlatGroupData *) data;
    ASSERT(groupData->data.model >= COL_SHARED);
    EnterCriticalSection(&groupData->csRoots);
}

/*---------------------------------------------------------------------------
 * Internal Function: PlatLeaveProtectRoots
 *
 *	Leave protected section around root management structures.
 *
 * Argument:
 *	data	- Group-specific data.
 *
 * Side effects:
 *	May unblock any thread waiting for the section.
 *
 * See also:
 *	<PlatEnterProtectRoots>, <LeaveProtectRoots>
 *---------------------------------------------------------------------------*/

void
PlatLeaveProtectRoots(
    GroupData *data)
{
    PlatGroupData *groupData = (PlatGroupData *) data;
    ASSERT(groupData->data.model >= COL_SHARED);
    LeaveCriticalSection(&groupData->csRoots);
}


/****************************************************************************
 * Internal Group: System Page Allocation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Variable: csRange
 *
 *	Critical section protecting address range management.
 *
 *  ranges		- Reserved address ranges for general purpose.
 *  dedicatedRanges	- Dedicated address ranges for large pages.
 *
 * See also:
 *	<PlatEnterProtectAddressRanges>, <PlatLeaveProtectAddressRanges>
 *---------------------------------------------------------------------------*/

CRITICAL_SECTION csRange;

/*---------------------------------------------------------------------------
 * Internal Function: PlatReserveRange
 *
 *	Reserve an address range.
 *
 * Arguments:
 *	size	- Number of pages to reserve.
 *	alloc	- Whether to allocate the range pages as well.
 *
 * Result:
 *	The reserved range's base address, or NULL if failure.
 *---------------------------------------------------------------------------*/

void *
PlatReserveRange(
    size_t size,
    int alloc)
{
    return VirtualAlloc(NULL, size << shiftPage, 
	MEM_RESERVE | (alloc ? MEM_COMMIT : 0), PAGE_READWRITE);
}

/*---------------------------------------------------------------------------
 * Internal Function: PlatReleaseRange
 *
 *	Release an address range.
 *
 * Arguments:
 *	base	- Base address of range to release.
 *	size	- Number of pages in range.
 *
 * Result:
 *	Nonzero for success.
 *---------------------------------------------------------------------------*/

int
PlatReleaseRange(
    void *base,
    size_t size)
{
    return VirtualFree(base, 0, MEM_RELEASE);
}

/*---------------------------------------------------------------------------
 * Internal Function: PlatAllocPages
 *
 *	Allocate pages in reserved range.
 *
 * Arguments:
 *	addr	- Address of first page to allocate.
 *	number	- Number of pages to allocate.
 *
 * Result:
 *	Nonzero for success.
 *---------------------------------------------------------------------------*/

int
PlatAllocPages(
    void *addr,
    size_t number)
{
    return !!VirtualAlloc(addr, number << shiftPage, MEM_COMMIT, 
	    PAGE_READWRITE);
}

/*---------------------------------------------------------------------------
 * Internal Function: PlatFreePages
 *
 *	Free pages in reserved range.
 *
 * Arguments:
 *	addr	- Address of first page to free.
 *	number	- Number of pages to free.
 *
 * Result:
 *	Nonzero for success.
 *---------------------------------------------------------------------------*/

int
PlatFreePages(
    void *addr,
    size_t number)
{
    return VirtualFree(addr, number << shiftPage, MEM_DECOMMIT);
}

/*---------------------------------------------------------------------------
 * Internal Function: PlatProtectPages
 *
 *	Protect/unprotect pages in reserved range.
 *
 * Arguments:
 *	addr	- Address of first page to protect/unprotect
 *	number	- Number of pages to protect/unprotect.
 *	protect	- Whether to protect or unprotect pages.
 *
 * Result:
 *	Nonzero for success.
 *---------------------------------------------------------------------------*/

int
PlatProtectPages(
    void *addr,
    size_t number,
    int protect)
{
    DWORD old;
    return VirtualProtect(addr, number << shiftPage, 
	    (protect ? PAGE_READONLY : PAGE_READWRITE), &old);
}

/*---------------------------------------------------------------------------
 * Internal Function: PageProtectVectoredHandler
 *
 *	Called upon exception.
 *
 * Argument:
 *	exceptionInfo	- Info about caught exception.
 *
 * Result:
 *	EXCEPTION_CONTINUE_SEARCH for unhandled exceptions, will pass exception
 *	to other handlers. 
 *	EXCEPTION_CONTINUE_EXECUTION for handled exceptions, will resume
 *	execution of calling code.
 *
 * See also:
 *	<SysPageProtect>
 *---------------------------------------------------------------------------*/

static LONG CALLBACK 
PageProtectVectoredHandler(
    PEXCEPTION_POINTERS exceptionInfo) 
{
    if (exceptionInfo->ExceptionRecord->ExceptionCode 
	    != EXCEPTION_ACCESS_VIOLATION 
	    || exceptionInfo->ExceptionRecord->ExceptionInformation[0] != 1) {
	/*
	 * Not a memory write exception.
	 */

	return EXCEPTION_CONTINUE_SEARCH;
    }

    /*
     * Remove write protection and remember page for parent tracking.
     */

    SysPageProtect(
	    (void *) exceptionInfo->ExceptionRecord->ExceptionInformation[1],
	    0);
    return EXCEPTION_CONTINUE_EXECUTION;
}


/****************************************************************************
 * Internal Group: Initialization/Cleanup
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Function: DllMain
 *
 *	Windows DLL entry point.
 *
 * Result:
 *	Always true.
 *
 * See also:
 *	<Init>
 *---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------
 * Internal Function: Init
 *
 *	Initialization routine. Called through <DllMain>.
 *
 * Result:
 *	Always true.
 *
 * Side effects:
 *	Create thread-local storage key <tlsToken> (freed upon 
 *	DLL_PROCESS_DETACH in <DllMain>).
 *	Install memory protection exception handler for parent tracking.
 *
 * See also:
 *	<DllMain>, <systemPageSize>, <allocGranularity>, <shiftPage>
 *---------------------------------------------------------------------------*/

static BOOL
Init()
{
    SYSTEM_INFO systemInfo;

    if ((tlsToken = TlsAlloc()) == TLS_OUT_OF_INDEXES) {
	return FALSE;
    }

    GetSystemInfo(&systemInfo);
    systemPageSize = systemInfo.dwPageSize;
    allocGranularity = systemInfo.dwAllocationGranularity;
    shiftPage = LOG2(systemPageSize);

    InitializeCriticalSection(&csRange);

    sharedGroups = NULL;
    InitializeCriticalSection(&csSharedGroups);

    AddVectoredExceptionHandler(1, PageProtectVectoredHandler);

    return TRUE;
}
