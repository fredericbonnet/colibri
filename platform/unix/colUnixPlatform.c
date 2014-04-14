/*                                                                              *//*!   @cond PRIVATE @file \
 * colUnixPlatform.c
 *
 *  This file provides Unix implementations of generic primitives needing
 *  platform-specific implementations, as well as Unix-specific primitives.
 *
 *  @see colPlatform.h
 *  @see colUnixPlatform.h
 *
 *  @private
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
                                                                                #       ifndef DOXYGEN
/*
 * Prototypes for functions used only in this file.
 */

static struct UnixGroupData * AllocGroupData(unsigned int model);
static void             FreeGroupData(struct UnixGroupData *groupData);
static void *           GcThreadProc(void *arg);
static void             Init(void);
                                                                                #       endif DOXYGEN

/*
================================================================================*//*!   @addtogroup arch_unix \
System and Architecture (Unix-specific)                                         *//*!   @{ *//*
================================================================================
*/
                                                                                #       ifndef DOXYGEN
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
                                                                                #       endif DOXYGEN

/********************************************************************************//*!   @name \
 * Thread-Local Storage                                                         *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * tsdKey
 *                                                                              *//*!
 *      Thread-speficic data identifier. Used to get thread-specific data.
 *
 *  @see ThreadData
 *  @see Init
 *//*-----------------------------------------------------------------------*/

pthread_key_t tsdKey;

/*---------------------------------------------------------------------------
 * UnixGroupData
 *                                                                              *//*!
 *  Platform-specific group data.
 *
 *  @see ThreadData
 *  @see GroupData
 *  @see Init
 *  @see AllocGroupData
 *  @see FreeGroupData
 *//*-----------------------------------------------------------------------*/

typedef struct UnixGroupData {
    GroupData data;                 /*!< Generic #GroupData structure. */
    struct UnixGroupData *next;     /*!< Next active group in list. */
    pthread_mutex_t mutexRoots;     /*!< Mutex protecting root management. */

    pthread_mutex_t mutexGc;        /*!< Mutex protecting GC from worker
                                         threads. */
    pthread_cond_t condGcScheduled; /*!< Triggers GC thread. */
    pthread_cond_t condGcDone;      /*!< Barrier for worker threads. */
    int scheduled;                  /*!< Flag for when a GC is scheduled. */
    int terminated;                 /*!< Flag for thread group destruction. */
    int nbActive;                   /*!< Active worker thread counter. */
    pthread_t threadGc;             /*!< GC thread. */
} UnixGroupData;

/*---------------------------------------------------------------------------
 * sharedGroups
 *                                                                              *//*!
 *  List of active groups in process.
 *
 *  @see UnixGroupData
 *  @see AllocGroupData
 *  @see FreeGroupData
 *//*-----------------------------------------------------------------------*/

static UnixGroupData *sharedGroups;

/*---------------------------------------------------------------------------
 * mutexSharedGroups
 *                                                                              *//*!
 *  Mutex protecting #sharedGroups.
 *
 *  @see sharedGroups
 *//*-----------------------------------------------------------------------*/

static pthread_mutex_t mutexSharedGroups = PTHREAD_MUTEX_INITIALIZER;

/*---------------------------------------------------------------------------
 * AllocGroupData
 *                                                                              *//*!
 *  Allocate and initialize a thread group data structure.
 *
 *  @return
 *      The newly allocated structure.
 *
 *  @sideeffect
 *      Memory allocated and system objects created.
 *
 *  @see @ref threading_models "Threading Model Constants"
 *  @see UnixGroupData
 *  @see FreeGroupData
 *//*-----------------------------------------------------------------------*/

static UnixGroupData *
AllocGroupData(
    unsigned int model)     /*!< Threading model. */
{
    UnixGroupData *groupData = (UnixGroupData *) malloc(sizeof(UnixGroupData));
    memset(groupData, 0, sizeof(UnixGroupData));
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
 * FreeGroupData
 *                                                                              *//*!
 *  Free a thread group data structure.
 *
 *  @sideeffect
 *      Memory freed and system objects deleted.
 *
 *  @see UnixGroupData
 *  @see AllocGroupData
 *//*-----------------------------------------------------------------------*/

static void
FreeGroupData(
    UnixGroupData *groupData)   /*!< Structure to free. */
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
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Process & Threads                                                            *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * once
 *                                                                              *//*!
 *  Ensure that per-process initialization only occurs once.
 *
 *  @see PlatEnter
 *  @see Init
 *//*-----------------------------------------------------------------------*/

static pthread_once_t once = PTHREAD_ONCE_INIT;

/*---------------------------------------------------------------------------
 * PlatEnter
 *                                                                              *//*!
 *  Enter the thread. If this is the first nested call, initialize thread data.
 *  If this is the first thread in its group, initialize group data as well.
 *
 *  @retval nonzero     if this is the first nested call.
 *  @retval zero        otherwise.
 *
 *  @see @ref threading_models "Threading Model Constants"
 *  @see ThreadData
 *  @see UnixGroupData
 *  @see PlatLeave
 *  @see Col_Init
 *//*-----------------------------------------------------------------------*/

int
PlatEnter(
    unsigned int model) /*!< Threading model. */
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
            UnixGroupData *groupData = sharedGroups;
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
 * PlatLeave
 *                                                                              *//*!
 *  Leave the thread. If this is the first nested call, cleanup thread data.
 *  If this is the last thread in its group, cleanup group data as well.
 *
 *  @retval nonzero     if this is the last nested call.
 *  @retval zero        otherwise.
 *
 *  @see ThreadData
 *  @see UnixGroupData
 *  @see PlatEnter
 *  @see Col_Cleanup
 *//*-----------------------------------------------------------------------*/

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

        FreeGroupData((UnixGroupData *) data->groupData);
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

                FreeGroupData((UnixGroupData *) data->groupData);
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
 * GcThreadProc
 *                                                                              *//*!
 *  Thread dedicated to the GC process. Activated when one of the worker threads
 *  in the group triggers the GC.
 *
 *  @return
 *      Always zero.
 *
 *  @sideeffect
 *      Calls #PerformGC.
 *
 *  @see AllocGroupData
 *  @see PerformGC
 *//*-----------------------------------------------------------------------*/

static void *
GcThreadProc(
    void *arg)  /*!< #UnixGroupData. */
{
    UnixGroupData *groupData = (UnixGroupData *) arg;
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
 * PlatSyncPauseGC
 *                                                                              *//*!
 *  Called when a worker thread calls the outermost Col_PauseGC().
 *
 *  @sideeffect
 *      May block as long as a GC is underway.
 *
 *  @see GcThreadProc
 *  @see SyncPauseGC
 *  @see Col_PauseGC
 *//*-----------------------------------------------------------------------*/

void
PlatSyncPauseGC(
    GroupData *data)    /*!< Group-specific data. */
{
    UnixGroupData *groupData = (UnixGroupData *) data;
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
 * PlatTrySyncPauseGC
 *                                                                              *//*!
 *  Called when a worker thread calls the outermost Col_TryPauseGC().
 *
 *  @return
 *      Nonzero if successful.
 *
 *  @see GcThreadProc
 *  @see TrySyncPauseGC
 *  @see Col_TryPauseGC
 *//*-----------------------------------------------------------------------*/

int
PlatTrySyncPauseGC(
    GroupData *data)    /*!< Group-specific data. */
{
    UnixGroupData *groupData = (UnixGroupData *) data;
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
 * PlatSyncResumeGC
 *                                                                              *//*!
 *  Called when a worker thread calls the outermost Col_ResumeGC().
 *
 *  @sideeffect
 *      If last thread in group, may trigger the GC in the dedicated thread if
 *      previously scheduled. This will block further calls to
 *      Col_PauseGC() / PlatSyncPauseGC().
 *
 *  @see GcThreadProc
 *  @see SyncResumeGC
 *  @see Col_ResumeGC
 *//*-----------------------------------------------------------------------*/

void
PlatSyncResumeGC(
    GroupData *data,    /*!< Group-specific data. */
    int performGc)      /*!< Whether to perform GC. */
{
    UnixGroupData *groupData = (UnixGroupData *) data;
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
 * PlatEnterProtectRoots
 *                                                                              *//*!
 *  Enter protected section around root management structures.
 *
 *  @sideeffect
 *      Blocks until no thread owns the section.
 *
 *  @see PlatLeaveProtectRoots
 *  @see EnterProtectRoots
 *//*-----------------------------------------------------------------------*/

void
PlatEnterProtectRoots(
    GroupData *data)    /*!< Group-specific data. */
{
    UnixGroupData *groupData = (UnixGroupData *) data;
    pthread_mutex_lock(&groupData->mutexRoots);
}

/*---------------------------------------------------------------------------
 * PlatLeaveProtectRoots
 *                                                                              *//*!
 *  Leave protected section around root management structures.
 *
 *  @sideeffect
 *      May unblock any thread waiting for the section.
 *
 *  @see PlatEnterProtectRoots
 *  @see LeaveProtectRoots
 *//*-----------------------------------------------------------------------*/

void
PlatLeaveProtectRoots(
    GroupData *data)    /*!< Group-specific data. */
{
    UnixGroupData *groupData = (UnixGroupData *) data;
    pthread_mutex_unlock(&groupData->mutexRoots);
}
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * System Page Allocation                                                       *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * mutexRange
 *                                                                              *//*!
 *  Mutex protecting address range management.
 *
 *  - #ranges:          Reserved address ranges for general purpose.
 *  - #dedicatedRanges: Dedicated address ranges for large pages.
 *
 *  @see PlatEnterProtectAddressRanges
 *  @see PlatLeaveProtectAddressRanges
 *//*-----------------------------------------------------------------------*/

pthread_mutex_t mutexRange = PTHREAD_MUTEX_INITIALIZER;

/*---------------------------------------------------------------------------
 * PlatReserveRange
 *                                                                              *//*!
 *  Reserve an address range.
 *
 *  @return
 *      The reserved range's base address, or NULL if failure.
 *//*-----------------------------------------------------------------------*/

void *
PlatReserveRange(
    size_t size,    /*!< Number of pages to reserve. */
    int alloc)      /*!< Whether to allocate the range pages as well. */
{
    void *addr = mmap(NULL, size << shiftPage,
            (alloc ? PROT_READ | PROT_WRITE : PROT_NONE),
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return (addr == MAP_FAILED ? NULL : addr);
}

/*---------------------------------------------------------------------------
 * PlatReleaseRange
 *                                                                              *//*!
 *  Release an address range.
 *
 *  @return
 *      Nonzero for success.
 *//*-----------------------------------------------------------------------*/

int
PlatReleaseRange(
    void *base,     /*!< Base address of range to release. */
    size_t size)    /*!< Number of pages in range. */
{
    return !munmap(base, size << shiftPage);
}

/*---------------------------------------------------------------------------
 * PlatAllocPages
 *                                                                              *//*!
 *  Allocate pages in reserved range.
 *
 *  @return
 *      Nonzero for success.
 *//*-----------------------------------------------------------------------*/

int
PlatAllocPages(
    void *addr,     /*!< Address of first page to allocate. */
    size_t number)  /*!< Number of pages to allocate. */
{
    return !mprotect(addr, number << shiftPage, PROT_READ | PROT_WRITE);
}

/*---------------------------------------------------------------------------
 * PlatFreePages
 *                                                                              *//*!
 *  Free pages in reserved range.
 *
 *  @return
 *      Nonzero for success.
 *//*-----------------------------------------------------------------------*/

int
PlatFreePages(
    void *addr,     /*!< Address of first page to free. */
    size_t number)  /*!< Number of pages to free. */
{
    return !mprotect(addr, number << shiftPage, PROT_NONE);
}

/*---------------------------------------------------------------------------
 * PlatProtectPages
 *                                                                              *//*!
 *  Protect/unprotect pages in reserved range.
 *
 *  @return
 *      Nonzero for success.
 *//*-----------------------------------------------------------------------*/

int
PlatProtectPages(
    void *addr,     /*!< Address of first page to protect/unprotect */
    size_t number,  /*!< Number of pages to protect/unprotect. */
    int protect)    /*!< Whether to protect or unprotect pages. */
{
    return !mprotect(addr, number << shiftPage,
            PROT_READ | (protect ? 0 : PROT_WRITE));
}

/*---------------------------------------------------------------------------
 * PageProtectSigAction
 *                                                                              *//*!
 *  Called upon memory protection signal (SIGSEGV).
 *
 *  @see SysPageProtect
 *//*-----------------------------------------------------------------------*/

static void
PageProtectSigAction(
    int signo,          /*!< Signal number caught. */
    siginfo_t *info,    /*!< Signal information. */
    void *dummy)        /*!< Unused. */
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
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Initialization/Cleanup                                                       *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * Init
 *                                                                              *//*!
 *  Initialization routine. Called through DllMain().
 *
 *  @sideeffect
 *      - Create thread-specific data key #tsdKey (never freed).
 *      - Install memory protection signal handler PageProtectSigAction() for
 *        parent tracking.
 *
 *  @see PlatEnter
 *//*-----------------------------------------------------------------------*/

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
                                                                                /*!     @} */
                                                                                /*!     @} */
                                                                                /*!     @endcond */