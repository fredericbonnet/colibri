#include "../../colibri.h"
#include "../../colInternal.h"
#include "../../colPlatform.h"

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>

/*
 * Thread-local data.
 */

static pthread_once_t once = PTHREAD_ONCE_INIT;
pthread_key_t tsdKey;

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


/*
 *----------------------------------------------------------------
 * System page allocator.
 *----------------------------------------------------------------
 */

/*
 * Memory is allocated with mmap in anonymous mode, which manages single pages.
 */

static size_t shiftPage;

/*
 *---------------------------------------------------------------------------
 *
 * PlatSysPageAlloc --
 *
 *	Allocate system pages.
 *
 * Results:
 *	The allocated system pages' base address.
 *
 * Side effects:
 *	New pages are allocated.
 *
 *---------------------------------------------------------------------------
 */

void * 
PlatSysPageAlloc(
    size_t number)		/* Number of pages to alloc. */
{
    void * page = mmap(NULL, number << shiftPage, PROT_READ | PROT_WRITE, 
	    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page == MAP_FAILED) {
	/* TODO: exception handling. */
	return NULL;
    }

    return page;
}

/*
 *---------------------------------------------------------------------------
 *
 * PlatSysPageFree --
 *
 *	Free system pages.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The pages are freed.
 *
 *---------------------------------------------------------------------------
 */

void 
PlatSysPageFree(
    void * page,		/* Base address of the pages to free. */
    size_t number)		/* Number of pages to free. */
{
    munmap(page, number << shiftPage);
}


/*
 *----------------------------------------------------------------
 * Process & threads.
 *----------------------------------------------------------------
 */

typedef struct PlatGroupData {
    GroupData data;		/* Generic structure. */
    struct PlatGroupData *next;	/* Next active group in list. */

    pthread_mutex_t mutexRoots;	/* Protect root management. */
    pthread_mutex_t mutexDirties;
				/* Protect dirty page management. */

    pthread_mutex_t mutexGc;	/* Protect GC from worker threads. */
    pthread_cond_t condGcScheduled;	
				/* Triggers GC thread. */
    pthread_cond_t condGcDone;	/* Barrier for worker threads. */
    int scheduled;		/* Flag for when a GC is scheduled. */
    int terminated;		/* Flag for thread group destruction. */
    int nbActive;		/* Active worker thread counter. */
    pthread_t threadGc;		/* GC thread. */
} PlatGroupData;
static PlatGroupData *sharedGroups;
static pthread_mutex_t mutexSharedGroups = PTHREAD_MUTEX_INITIALIZER;

/*
 *---------------------------------------------------------------------------
 *
 * AllocGroupData --
 *
 *	Allocate and initialize a thread group data structure.
 *
 * Results:
 *	The newly allocated structure.
 *
 * Side effects:
 *	Memory allocated and system objects created.
 *
 *---------------------------------------------------------------------------
 */

static PlatGroupData *
AllocGroupData(
    unsigned int model)		/* Threading model. */
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
	pthread_mutex_init(&groupData->mutexDirties, NULL);

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

/*
 *---------------------------------------------------------------------------
 *
 * FreeGroupData --
 *
 *	Free a thread group data structure.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory freed and system objects deleted.
 *
 *---------------------------------------------------------------------------
 */

static void
FreeGroupData(
    PlatGroupData *groupData)	/* Structure to free. */
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

	pthread_mutex_destroy(&groupData->mutexDirties);
	pthread_mutex_destroy(&groupData->mutexRoots);
    }

    GcCleanupGroup((GroupData *) groupData);
    free(groupData);
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
PlatEnter(
    unsigned int model)		/* Threading model. */
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

    data = (ThreadData *) malloc(sizeof(ThreadData));
    memset(data, 0, sizeof(*data));
    data->nestCount = 1;
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

/*
 *---------------------------------------------------------------------------
 *
 * GcThreadProc --
 *
 *	Thread dedicated to the GC process. Activated when one of the worker
 *	threads in the group triggers the GC.
 *
 * Results:
 *	None (always zero).
 *
 * Side effects:
 *	Calls PerformGC.
 *
 *---------------------------------------------------------------------------
 */

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

/*
 *---------------------------------------------------------------------------
 *
 * PlatSyncPauseGC --
 *
 *	Called when a worker thread calls the outermost Col_PauseGC.
 *	May block as long as a GC is underway (see GcThreadProc).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Update internal structures.
 *
 *---------------------------------------------------------------------------
 */

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

/*
 *---------------------------------------------------------------------------
 *
 * PlatSyncResumeGC --
 *
 *	Called when a worker thread calls the outermost Col_ResumeGC.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If last thread in group, may trigger the GC if previously scheduled.
 *	This will block further calls to Col_PauseGC/PlatSyncPauseGC.
 *
 *---------------------------------------------------------------------------
 */

void
PlatSyncResumeGC(
    GroupData *data,
    int schedule) 
{
    PlatGroupData *groupData = (PlatGroupData *) data;
    pthread_mutex_lock(&groupData->mutexGc);
    {
	if (schedule) {
	    groupData->scheduled = 1;
	}
	--groupData->nbActive;
	if (!groupData->nbActive && groupData->scheduled) {
	    pthread_cond_signal(&groupData->condGcScheduled);
	}
    }
    pthread_mutex_unlock(&groupData->mutexGc);
}

/*
 *---------------------------------------------------------------------------
 *
 * PlatEnterProtectRoots --
 *
 *	Enter protected section for root management.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Blocks until no thread owns the section.
 *
 *---------------------------------------------------------------------------
 */

void
PlatEnterProtectRoots(
    GroupData *data)
{
    PlatGroupData *groupData = (PlatGroupData *) data;
    pthread_mutex_lock(&groupData->mutexRoots);
}

/*
 *---------------------------------------------------------------------------
 *
 * PlatLeaveProtectRoots --
 *
 *	Leave protected section for root management.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	May unblock any thread waiting for the section.
 *
 *---------------------------------------------------------------------------
 */

void
PlatLeaveProtectRoots(GroupData *data)
{
    PlatGroupData *groupData = (PlatGroupData *) data;
    pthread_mutex_unlock(&groupData->mutexRoots);
}

/*
 *---------------------------------------------------------------------------
 *
 * PlatEnterProtectDirties --
 *
 *	Enter protected section for dirty page management.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Blocks until no thread owns the section.
 *
 *---------------------------------------------------------------------------
 */

void
PlatEnterProtectDirties(GroupData *data)
{
    PlatGroupData *groupData = (PlatGroupData *) data;
    pthread_mutex_lock(&groupData->mutexDirties);
}

/*
 *---------------------------------------------------------------------------
 *
 * PlatLeaveProtectRoots --
 *
 *	Leave protected section for root management.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	May unblock any thread waiting for the section.
 *
 *---------------------------------------------------------------------------
 */

void
PlatLeaveProtectDirties(GroupData *data)
{
    PlatGroupData *groupData = (PlatGroupData *) data;
    pthread_mutex_unlock(&groupData->mutexDirties);
}

/*
 *---------------------------------------------------------------------------
 *
 * Init --
 *
 *	Initialization routine. Called through pthread_once.
 *
 * Results:
 *	True.
 *
 * Side effects:
 *	Create thread-local storage key. Note: key is never freed.
 *
 *---------------------------------------------------------------------------
 */

static void
Init()
{
    if (pthread_key_create(&tsdKey, NULL)) {
	/* TODO: exception */
	return;
    }
    systemPageSize = sysconf(_SC_PAGESIZE);
    shiftPage = LOG2(systemPageSize);
}

