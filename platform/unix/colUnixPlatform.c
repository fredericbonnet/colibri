/*
 * Internal File: colUnixPlatform.c
 *
 *	This Unix-specific file implements the generic features needing 
 *	platform-specific implementations, as well as the Unix-specific
 *	features.
 *
 * See also:
 *	<colPlatform.h>, <colUnixPlatform.h>
 */

#include "../../include/colibri.h"
#include "../../colInternal.h"
#include "../../colPlatform.h"

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

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
static void *		GcThreadProc(void *arg);
static void		Init(void);


/****************************************************************************
 * Internal Section: Thread-Specific Data
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Variable: tsdKey
 *
 *	Thread-speficic data identifier. Used to get thread-specific data.
 *
 * See also:
 *	<ThreadData>, <Init>
 *---------------------------------------------------------------------------*/

pthread_key_t tsdKey;

/*---------------------------------------------------------------------------
 * Internal Typedef: PlatGroupData
 *
 *	Platform-specific group data.
 *
 * Fields:
 *	data			- Generic <GroupData> structure.
 *	next			- Next active group in list.
 *	mutexRoots		- Mutex protecting root management.
 *	mutexGc			- Mutex protecting GC from worker 
 *				  threads.
 *	condGcScheduled		- Triggers GC thread.
 *	condGcDone		- Barrier for worker threads.
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
    pthread_mutex_t mutexRoots;

    pthread_mutex_t mutexGc;
    pthread_cond_t condGcScheduled;
    pthread_cond_t condGcDone;
    int scheduled;
    int terminated;
    int nbActive;
    pthread_t threadGc;
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
 * Internal Variable: mutexSharedGroups
 *
 *	Mutex protecting <sharedGroups>.
 *
 * See also:
 *	<sharedGroups>
 *---------------------------------------------------------------------------*/

static pthread_mutex_t mutexSharedGroups = PTHREAD_MUTEX_INITIALIZER;

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
 *	<Threading Model Constants>, <PlatGroupData>, <FreeGroupData>
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
	pthread_mutex_init(&groupData->mutexRoots, NULL);

	pthread_mutex_init(&groupData->mutexGc, NULL);
	pthread_cond_init(&groupData->condGcDone, NULL);
	pthread_cond_init(&groupData->condGcScheduled, NULL);

	/*
	 * Create GC thread.
	 */

	pthread_create(&groupData->threadGc, NULL, GcThreadProc, groupData);
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
	pthread_mutex_lock(&groupData->mutexGc);
	{
	    pthread_cond_signal(&groupData->condGcScheduled);
	}
	pthread_mutex_unlock(&groupData->mutexGc);
	pthread_join(groupData->threadGc, NULL);

	/*
	 * Destroy synchronization objects.
	 */

	pthread_cond_destroy(&groupData->condGcScheduled);
	pthread_cond_destroy(&groupData->condGcDone);
	pthread_mutex_destroy(&groupData->mutexGc);

	pthread_mutex_destroy(&groupData->mutexRoots);
    }

    GcCleanupGroup((GroupData *) groupData);
    free(groupData);
}


/****************************************************************************
 * Internal Section: Process & Threads
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Variable: once
 *
 *	Ensure that per-process initialization only occurs once.
 *
 * See also:
 *	<PlatEnter>, <Init>
 *---------------------------------------------------------------------------*/

static pthread_once_t once = PTHREAD_ONCE_INIT;

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
 *	Nonzero if this is the first nested call, else 0.
 *
 * See also:
 *	<Threading Model Constants>, <ThreadData>, <PlatGroupData>, <PlatLeave>,
 *	<Col_Init>
 *---------------------------------------------------------------------------*/

int
PlatEnter(
    unsigned int model)
{
    ThreadData *data;

    /*
     * Ensures that the TLS key is created once.
     */

    pthread_once(&once, Init);

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

    data = (ThreadData *) malloc(sizeof(*data)
	    + sizeof(UNIX_PROTECT_ADDRESS_RANGES_RECURSE(data)));
    memset(data, 0, sizeof(*data));
    data->nestCount = 1;
    UNIX_PROTECT_ADDRESS_RANGES_RECURSE(data) = 0;
    GcInitThread(data);
    pthread_setspecific(tsdKey, data);

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

	pthread_mutex_lock(&mutexSharedGroups);
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
	pthread_mutex_unlock(&mutexSharedGroups);
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
 *	Nonzero if this is the last nested call, else 0.
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

	pthread_mutex_lock(&mutexSharedGroups);
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
	pthread_mutex_unlock(&mutexSharedGroups);
    }

    GcCleanupThread(data);
    free(data);
    pthread_setspecific(tsdKey, 0);

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

static void *
GcThreadProc(
    void *arg)
{
    PlatGroupData *groupData = (PlatGroupData *) arg;
    for (;;) {
	pthread_mutex_lock(&groupData->mutexGc);
	{
	    pthread_cond_wait(&groupData->condGcScheduled, &groupData->mutexGc);
	    if (groupData->scheduled) {
		groupData->scheduled = 0;
		PerformGC((GroupData *) groupData);
		pthread_cond_broadcast(&groupData->condGcDone);
	    }
	}
	pthread_mutex_unlock(&groupData->mutexGc);
	if (groupData->terminated) {
	    pthread_exit(NULL);
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
    pthread_mutex_lock(&groupData->mutexGc);
    {
	if (groupData->scheduled) {
	    pthread_cond_wait(&groupData->condGcDone, &groupData->mutexGc);
	}
	groupData->nbActive++;
    }
    pthread_mutex_unlock(&groupData->mutexGc);
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
 *	Nonzero if successful.
 * 
 * See also:
 *	<GcThreadProc>, <TrySyncPauseGC>, <Col_TryPauseGC>
 *---------------------------------------------------------------------------*/

int
PlatTrySyncPauseGC(
    GroupData *data) 
{
    PlatGroupData *groupData = (PlatGroupData *) data;
    if (!pthread_mutex_trylock(&groupData->mutexGc)) {
	return 0;
    } else {
	if (groupData->scheduled) {
	    pthread_cond_wait(&groupData->condGcDone, &groupData->mutexGc);
	}
	groupData->nbActive++;
    }
    pthread_mutex_unlock(&groupData->mutexGc);
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
    pthread_mutex_lock(&groupData->mutexGc);
    {
	if (performGc) {
	    groupData->scheduled = 1;
	}
	--groupData->nbActive;
	if (!groupData->nbActive && groupData->scheduled) {
	    pthread_cond_signal(&groupData->condGcScheduled);
	}
    }
    pthread_mutex_unlock(&groupData->mutexGc);
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
    pthread_mutex_lock(&groupData->mutexRoots);
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
    pthread_mutex_unlock(&groupData->mutexRoots);
}


/****************************************************************************
 * Internal Section: System Page Allocation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Variable: mutexRange
 *
 *	Mutex protecting address range management.
 *
 *  ranges		- Reserved address ranges for general purpose.
 *  dedicatedRanges	- Dedicated address ranges for large pages.
 *
 * See also:
 *	<PlatEnterProtectAddressRanges>, <PlatLeaveProtectAddressRanges>
 *---------------------------------------------------------------------------*/

pthread_mutex_t mutexRange = PTHREAD_MUTEX_INITIALIZER;

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
    void *addr = mmap(NULL, size << shiftPage,
	    (alloc ? PROT_READ | PROT_WRITE : PROT_NONE), 
	    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return (addr == MAP_FAILED ? NULL : addr);
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
    return !munmap(base, size << shiftPage);
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
    return !mprotect(addr, number << shiftPage, PROT_READ | PROT_WRITE);
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
    return !mprotect(addr, number << shiftPage, PROT_NONE);
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
    return !mprotect(addr, number << shiftPage, 
	    PROT_READ | (protect ? 0 : PROT_WRITE));
}

/*---------------------------------------------------------------------------
 * Internal Function: PageProtectSigAction
 *
 *	Called upon memory protection signal (SIGSEGV).
 *
 * Argument:
 *	signo	- Signal numbercaught.
 *	info	- Signal information.
 *
 * See also:
 *	<SysPageProtect>
 *---------------------------------------------------------------------------*/

static void 
PageProtectSigAction(
    int signo,
    siginfo_t *info,
    void *dummy) 
{
    if (signo != SIGSEGV || info->si_code != SEGV_ACCERR) {
	/*
	 * Not a memory protection signal.
	 */

	return;
    }

    /*
     * Remove write protection and remember page for parent tracking.
     */

    SysPageProtect(info->si_addr, 0);
}


/****************************************************************************
 * Internal Section: Initialization/Cleanup
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Function: Init
 *
 *	Initialization routine. Called through pthread_once on <once>.
 *
 * Side effects:
 *	Create thread-specific data key <tsdKey> (never freed).
 *	Install memory protection signal handler for parent tracking.
 *
 * See also:
 *	<PlatEnter>
 *---------------------------------------------------------------------------*/

static void
Init()
{
    struct sigaction sa;
    if (pthread_key_create(&tsdKey, NULL)) {
	/* TODO: exception */
	return;
    }
    systemPageSize = sysconf(_SC_PAGESIZE);
    allocGranularity = systemPageSize * 16;
    shiftPage = LOG2(systemPageSize);

    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = PageProtectSigAction;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sigaction(SIGSEGV, &sa, NULL);
}
