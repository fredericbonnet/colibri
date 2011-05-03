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

static size_t		FindClearedBits(unsigned char *mask, size_t size, 
			    size_t number, size_t index);
static void		SetBit(unsigned char *mask, size_t i);
static void		SetBits(unsigned char *mask, size_t first, 
			    size_t number);
static void		ClearBits(unsigned char *mask, size_t first, 
			    size_t number);
static size_t		TestBit(unsigned char *mask, size_t index);
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
 *	csDirties		- Critical section protecting dirty page 
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
    CRITICAL_SECTION csDirties;

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
	InitializeCriticalSection(&groupData->csDirties);

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

	DeleteCriticalSection(&groupData->csDirties);
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
 * Internal Macro: PlatEnterProtectRoots
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
 * Internal Macro: PlatLeaveProtectRoots
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
PlatLeaveProtectRoots(GroupData *data)
{
    PlatGroupData *groupData = (PlatGroupData *) data;
    ASSERT(groupData->data.model >= COL_SHARED);
    LeaveCriticalSection(&groupData->csRoots);
}

/*---------------------------------------------------------------------------
 * Internal Macro: PlatEnterProtectDirties
 *
 *	Enter protected section around dirty page management structures.
 *
 * Argument:
 *	data	- Group-specific data.
 *
 * Side effects:
 *	Blocks until no thread owns the section.
 *
 * See also:
 *	<PlatLeaveProtectDirties>, <EnterProtectDirtiess>
 *---------------------------------------------------------------------------*/

void
PlatEnterProtectDirties(GroupData *data)
{
    PlatGroupData *groupData = (PlatGroupData *) data;
    ASSERT(groupData->data.model >= COL_SHARED);
    EnterCriticalSection(&groupData->csDirties);
}

/*---------------------------------------------------------------------------
 * Internal Macro: PlatLeaveProtectDirties
 *
 *	Leave protected section around dirty page management structures.
 *
 * Argument:
 *	data	- Group-specific data.
 *
 * Side effects:
 *	May unblock any thread waiting for the section.
 *
 * See also:
 *	<PlatEnterProtectDirties>, <LeaveProtectDirties>
 *---------------------------------------------------------------------------*/

void
PlatLeaveProtectDirties(GroupData *data)
{
    PlatGroupData *groupData = (PlatGroupData *) data;
    ASSERT(groupData->data.model >= COL_SHARED);
    LeaveCriticalSection(&groupData->csDirties);
}


/****************************************************************************
 * Internal Group: System Page Allocation
 *
 *	Granularity-free system page allocation based on VirtualAlloc.
 *
 *	Contrary to most other platforms where individual pages can be allocated 
 *	and freed (for example using mmap/munmap), the Windows VirtualAlloc API 
 *	restricts address reservation to a granularity of 
 *	SYSTEM_INFO.dwAllocationGranularity (65536 on IA32), although it can 
 *	commit and decommit individual pages with a size of 
 *	SYSTEM_INFO.dwPageSize (4096 on IA32). Besides, behaving as if system 
 *	pages had a size of SYSTEM_INFO.dwAllocationGranularity proved to be too 
 *	CPU and memory intensive. So if we want to keep the system page small, 
 *	this implies that each page is itself part of a larger address range. 
 *	
 *	Achieving single page granularity involves a lot of bookkeeping, as 
 *	Windows doesn't provide an efficient query API (VirtualQuery is very CPU 
 *	intensive and is known to be even slower on Win64). So large address 
 *	ranges are reserved, and individual pages are committed within these 
 *	ranges. 
 *	
 *	When the number of pages to allocate exceeds a certain size (defined as 
 *	<LARGE_PAGE_SIZE>), a dedicated address range is reserved and committed, 
 *	which must be freed all at once. Else,pages are allocated within larger 
 *	address ranges using the following scheme: 
 *	
 *	Address ranges are reserved in geometrically increasing sizes up to a 
 *	maximum size (the first being a multiple of the allocation granularity). 
 *	Ranges form a singly-linked list in allocation order (so that search 
 *	times are geometrically increasing too). A descriptor structure is 
 *	malloc'd along with the range and consists of a pointer to the next 
 *	descrptor, the base address of the range, the total and free numbers of 
 *	pages and the index of the first free page in range, and a bitmask table 
 *	for allocated pages (much alike regular Colibri pages). 
 *	
 *	To allocate a group of pages in an address range, the bitmask is scanned 
 *	until a large enough group of free pages is found. To free a group of 
 *	pages, the containing address range is found by scanning all ranges 
 *	(even VirtualQuery is too slow for this simple purpose!). In either case 
 *	the descriptor is updated accordingly. If a containing range is not 
 *	found we assume that it was a dedicated range and we attempt to release 
 *	it at once. 
 *	
 *	The allocation scheme may not look optimal at first sight (especially 
 *	the bitmask scanning step), but keep in mind that the typical use case 
 *	only involves single page allocations. Multiple page allocations only 
 *	occur when allocating large, multiple cell-based objects, and most 
 *	objects are single cell sized. And very large pages will end up in their 
 *	own dedicated range with no page management. Moreover stress tests have 
 *	shown that this scheme yielded similar or better performances than the 
 *	previous scheme that was limited to granularity-sized address ranges. 
 ****************************************************************************/

static SYSTEM_INFO systemInfo;
static size_t shiftPage;

typedef struct AddressRange {
    struct AddressRange *next;
    void *base;
    size_t size;
    size_t free;
    size_t first;
    unsigned char bitmask[0];
} AddressRange;

static AddressRange *ranges;
static AddressRange *dedicatedRanges;
static CRITICAL_SECTION csPageAlloc;

#define FIRST_RANGE_SIZE	256	/* 1 MB */
#define MAX_RANGE_SIZE		32768	/* 128 MB */


/*---------------------------------------------------------------------------
 * Internal Functions: Bit Handling
 *
 *	Bit manipulation routines. Adapted from <colAlloc.c>.
 *
 *  FindClearedBits	- See <FindFreeCells>.
 *  SetBit		- See <SetCells>.
 *  SetBits		- See <SetCells>.
 *  ClearBits		- Set <ClearCells>.
 *  TestBit		- Set <TestCell>.
 *---------------------------------------------------------------------------*/

static size_t 
FindClearedBits(
    unsigned char *mask,	/* Bitmask table. */
    size_t size,		/* Size of bitmask in bits. */
    size_t number,		/* Number of consecutive bits to find. */
    size_t index)		/* First bit to consider. */
{
    size_t i, first = (size_t)-1, remaining;
    char seq;

    /* 
     * Iterate over bytes of the bitmask to find a chain of <number> zero bits. 
     */

    remaining = number;
    for (i = (index>>3); i < (size>>3); i++) {
	if (remaining <= 7) {
	    /* 
	     * End of sequence, or whole sequence for number < 8. 
	     * Find sequence in byte. 
	     */

	    seq = firstZeroBitSequence[remaining-1][mask[i]];
	    if (seq != -1) {
		if (remaining == number) {
		    /* 
		     * Sequence fits into one byte. 
		     */

		    return (i<<3) + seq;
		}

		/* 
		 * Sequence spans over several bytes. 
		 */

		if (seq == 0) {
		    /* 
		     * Sequence is contiguous. 
		     */

		    return first;
		}
	    }

	    /* 
	     * Sequence interrupted, restart. 
	     */

	    remaining = number;
	}

	/* 
	 * Sequence may span over several bytes. 
	 */

	if (remaining == number) {
	    /* 
	     * Beginning of sequence. 
	     *
	     * Note: leading bits actually denote trailing cells: bit 0 (LSB) is
	     * first cell, bit 7 (MSB) is last.
	     */

	    seq = longestLeadingZeroBitSequence[mask[i]];
	    if (seq) {
		first = (i<<3) + (8-seq);
		remaining -= seq;
	    }
	} else {
	    /* 
	     * Middle of sequence, look for 8 bits cleared = zero byte. 
	     */

	    if (mask[i] == 0) {
		/* 
		 * Continue sequence. 
		 */

		remaining -= 8;
	    } else {
		/* 
		 * Sequence interrupted, restart. 
		 */

		remaining = number;
	    }
	}
	if (remaining == 0) {
	    /*
	     * End of sequence reached.
	     */

	    return first;
	}
    }

    /*
     * None found.
     */

    return (size_t) -1;
}

static void
SetBit(
    unsigned char *mask,	/* Bitmask table. */
    size_t i)			/* Index of first bit to set. */
{
    mask[i>>3] |= 1<<(i&7);
}

static void 
SetBits(
    unsigned char *mask,	/* Bitmask table. */
    size_t first,		/* Index of first bit to set. */
    size_t number)		/* Number of bits to set. */
{
    size_t i;
    for (i=first; i < first+number; i++) {
	mask[i>>3] |= 1<<(i&7);
    }
}

static void 
ClearBits(
    unsigned char *mask,	/* Bitmask table. */
    size_t first,		/* Index of first bit to clear. */
    size_t number)		/* Number of bits to clear. */
{
    size_t i;
    for (i=first; i < first+number; i++) {
	mask[i>>3] &= ~(1<<(i&7));
    }
}

static size_t 
TestBit(
    unsigned char *mask,	/* Bitmask table. */
    size_t index)		/* Index of bit to set. */
{
    return mask[index>>3] & (1<<(index&7));
}

/*---------------------------------------------------------------------------
 * Internal Function: PlatSysPageAlloc
 *
 *	Allocate system pages.
 *
 * Argument:
 *	number	- Number of system pages to alloc.
 *
 * Result:
 *	The allocated system pages' base address.
 *
 * Side effects:
 *	May reserve new virtual ranges.
 *
 * See also:
 *	<PlatSysPageFree>
 *---------------------------------------------------------------------------*/

void * 
PlatSysPageAlloc(
    size_t number)
{
    void *addr = NULL;
    AddressRange *range, **prevPtr;
    size_t first, size;

    if (number > LARGE_PAGE_SIZE || !(number 
	    & ((systemInfo.dwAllocationGranularity >> shiftPage)-1))) {
	/*
	 * Length exceeds a certain threshold or is a multiple of the allocation
	 * granularity, allocate dedicated address range.
	 */

	addr = VirtualAlloc(NULL, number << shiftPage, MEM_RESERVE|MEM_COMMIT, 
		PAGE_READWRITE);
	if (!addr) {
	    /*
	     * Fatal error!
	     */

	    Col_Error(COL_FATAL, "VirtualAlloc: Error %u", GetLastError());
	}

	/*
	 * Create descriptor without bitmask and insert at head.
	 */

	EnterCriticalSection(&csPageAlloc);
	{
	    range = (AddressRange *) (malloc(sizeof(AddressRange)));
	    range->next = dedicatedRanges;
	    range->base = addr;
	    range->size = number;
	    range->free = 0;
	    range->first = number;
	    dedicatedRanges = range;
	}
	LeaveCriticalSection(&csPageAlloc);

	return addr;
    }

    /*
     * Try to find address range with enough consecutive pages.
     */

    EnterCriticalSection(&csPageAlloc);
    {
	first = (size_t)-1;
	prevPtr = &ranges;
	range = ranges;
	while (range) {
	    size = range->size;
	    if (range->free >= number) {
		/*
		 * Range has the required number of pages.
		 */

		unsigned char *mask = range->bitmask;
		first = range->first;

		if (number == 1 && !TestBit(mask, first)) {
		    /*
		     * Fast-track the most common case: allocate the first free 
		     * page.
		     */

		    break;
		}
    	    
		/*
		 * Find the first available bit sequence.
		 */

		first = FindClearedBits(mask, size, number, first);
		if (first != (size_t)-1) {
		    break;
		}
	    }

	    /*
	     * Not found, try next range.
	     */

	    prevPtr = &range->next;
	    range = range->next;
	}

	if (!range) {
	    /*
	     * No range was found with available pages. Create a new one.
	     */

	    if (!ranges) {
		size = FIRST_RANGE_SIZE;
	    } else {
		/*
		 * New range size is double that of the previous one.
		 */

		size <<= 1;
		if (size > MAX_RANGE_SIZE) size = MAX_RANGE_SIZE;
	    }
	    ASSERT(number <= size-1);

	    /*
	     * Reserve address range.
	     */

	    addr = VirtualAlloc(NULL, size << shiftPage, MEM_RESERVE, 
		    PAGE_READWRITE);
	    if (!addr) {
		/*
		 * Fatal error!
		 */

		Col_Error(COL_FATAL, "VirtualAlloc: Error %u", GetLastError());
		goto end;
	    }

	    /*
	     * Create descriptor.
	     */

	    range = (AddressRange *) (malloc(sizeof(AddressRange) + (size>>3)));
	    *prevPtr = range;
	    range->base = addr;
	    range->next = NULL;
	    range->size = size;
	    range->free = size;
	    range->first = 0;
	    memset(range->bitmask, 0, (size >> 3));
	    first = 0;
	}

	/*
	 * Commit pages. 
	 */

	ASSERT(first+number <= size);
	ASSERT(number <= range->free);
	addr = VirtualAlloc((char *) range->base + (first << shiftPage), 
		number << shiftPage, MEM_COMMIT, PAGE_READWRITE);
	if (!addr) {
	    /*
	     * Fatal error!
	     */

	    Col_Error(COL_FATAL, "VirtualAlloc: Error %u", GetLastError());
	    goto end;
	}

	/*
	 * Update metadata and return.
	 */

	range->free -= number;
	if (first == range->first) {
	    /* 
	     * Simply increase the first free page index, the actual index will be 
	     * updated on next allocation.
	     */

	    range->first += number;
	}
	SetBits(range->bitmask, first, number);
    }
end:
    LeaveCriticalSection(&csPageAlloc);
    return addr;
}

/*---------------------------------------------------------------------------
 * Internal Function: PlatSysPageFree
 *
 *	Free system pages.
 *
 * Arguments:
 *	page	- Base address of the pages to free.
 *	number	- Number of pages to free.
 *
 * Side Effects:
 *	May release virtual ranges.
 *
 * See also:
 *	<PlatSysPageAlloc>
 *---------------------------------------------------------------------------*/

void 
PlatSysPageFree(
    void * page,
    size_t number)
{
    size_t index;
    AddressRange * range;
    unsigned char *mask;

    EnterCriticalSection(&csPageAlloc);
    {
	/*
	 * Get range info for page. 
	 */

	range = ranges;
	while (range) {
	    if (page >= range->base && (char *) page < (char *) range->base 
		    + (range->size << shiftPage)) {
		break;
	    }
	    range = range->next;
	}
	if (!range) {
	    /*
	     * Likely dedicated address range. 
	     */

	    AddressRange **prevPtr = &dedicatedRanges;
	    range = dedicatedRanges;
	    while (range) {
		if (page >= range->base && (char *) page < (char *) range->base 
			+ (range->size << shiftPage)) {
		    break;
		}
		prevPtr = &range->next;
		range = range->next;
	    }
	    if (!range) {
		/*
		 * Not found.
		 */

		Col_Error(COL_FATAL, "Page not found %p", page);
		goto end;
	    }

	    /*
	     * Release whole range.
	     */

	    if (!VirtualFree(range->base, 0, MEM_RELEASE)) {
		/*
		 * Fatal error!
		 */

		Col_Error(COL_FATAL, "VirtualFree: Error %u", GetLastError());
		goto end;
	    }

	    /*
	     * Remove from dedicated range list.
	     */

	    *prevPtr = range->next;
	    free(range);
	    goto end;
	}

	mask = range->bitmask;
	index = ((char *) page - (char *) range->base) >> shiftPage;
	ASSERT(index+number <= range->size);
	ASSERT(TestBit(mask, index));

	/*
	 * Decommit pages.
	 */

	if (!VirtualFree(page, number << shiftPage, MEM_DECOMMIT)) {
	    /*
	     * Fatal error!
	     */

	    Col_Error(COL_FATAL, "VirtualFree: Error %u", GetLastError());
	    goto end;
	}
	ClearBits(mask, index, number);
	range->free += number;
	ASSERT(range->free <= range->size);
	if (range->first > index) {
	    /*
	     * Old first free page is beyond the one we just freed, update.
	     */
    	 
	    range->first = index;
	}
    }
end:
    LeaveCriticalSection(&csPageAlloc);
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
 *
 * See also:
 *	<DllMain>
 *---------------------------------------------------------------------------*/

static BOOL
Init()
{
    if ((tlsToken = TlsAlloc()) == TLS_OUT_OF_INDEXES) {
	return FALSE;
    }

    GetSystemInfo(&systemInfo);
    systemPageSize = systemInfo.dwPageSize;
    shiftPage = LOG2(systemPageSize);

    ranges = NULL;
    dedicatedRanges = NULL;
    InitializeCriticalSection(&csPageAlloc);

    sharedGroups = NULL;
    InitializeCriticalSection(&csSharedGroups);

    return TRUE;
}
