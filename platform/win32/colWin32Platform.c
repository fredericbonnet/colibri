/**
 * @file colWin32Platform.c
 *
 * This file provides Win32 implementations of generic primitives needing
 * platform-specific implementations, as well as Win32-specific primitives.
 *
 * @see colPlatform.h
 * @see colWin32Platform.h
 *
 * @beginprivate @cond PRIVATE
 */

#include "../../include/colibri.h"
#include "../../colInternal.h"
#include "../../colPlatform.h"

#include <windows.h>
#include <sys/types.h>

/*
 * Prototypes for functions used only in this file.
 */

/*! \cond IGNORE */
static struct Win32GroupData * AllocGroupData(unsigned int model);
static void             FreeGroupData(struct Win32GroupData *groupData);
static DWORD WINAPI     GcThreadProc(LPVOID lpParameter);
static BOOL             Init(void);
/*! \endcond *//* IGNORE */


/*
===========================================================================*//*!
\internal \weakgroup arch_win32 System and Architecture (Win32-specific)
\{*//*==========================================================================
*/

/** @beginprivate @cond PRIVATE */

/*
 * Bit twiddling hack for computing the log2 of a power of 2.
 * See: http://www-graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
 */

/*! \cond IGNORE */
static const int MultiplyDeBruijnBitPosition2[32] =
{
  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};
#define LOG2(v) MultiplyDeBruijnBitPosition2[(uint32_t)(v * 0x077CB531U) >> 27]
/*! \endcond *//* IGNORE */

/** @endcond @endprivate */


/***************************************************************************//*!
 * \name Thread-Local Storage
 ***************************************************************************\{*/

/** @beginprivate @cond PRIVATE */

/**
 * Thread-local storage identifier. Used to get thread-specific data.
 *
 * @see ThreadData
 * @see Init
 */
DWORD tlsToken;

/**
 * Platform-specific group data.
 *
 * @see ThreadData
 * @see GroupData
 * @see Init
 * @see AllocGroupData
 * @see FreeGroupData
 */
typedef struct Win32GroupData {
    GroupData data;             /*!< Generic #GroupData structure. */
    struct Win32GroupData *next;/*!< Next active group in list. */
    CRITICAL_SECTION csRoots;   /*!< Critical section protecting root
                                     management. */

    CRITICAL_SECTION csGc;      /*!< Critical section protecting GC from worker
                                     threads. */
    HANDLE eventGcScheduled;    /*!< Triggers GC thread. */
    HANDLE eventGcDone;         /*!< Barrier for worker threads. */
    int scheduled;              /*!< Flag for when a GC is scheduled. */
    int terminated;             /*!< Flag for thread group destruction. */
    int nbActive;               /*!< Active worker thread counter. */
    HANDLE threadGc;            /*!< GC thread. */
} Win32GroupData;

/**
 * List of active groups in process.
 *
 * @see Win32GroupData
 * @see AllocGroupData
 * @see FreeGroupData
 */
static Win32GroupData *sharedGroups;

/**
 * Critical section protecting #sharedGroups.
 *
 * @see sharedGroups
 */
static CRITICAL_SECTION csSharedGroups;

/**
 * Allocate and initialize a thread group data structure.
 *
 * @return The newly allocated structure.
 *
 * @sideeffect
 *      Memory allocated and system objects created.
 *
 * @see @ref threading_models "Threading Model Constants"
 * @see Win32GroupData
 * @see FreeGroupData
 */
static Win32GroupData *
AllocGroupData(
    unsigned int model)     /*!< Threading model. */
{
    Win32GroupData *groupData = (Win32GroupData *) malloc(sizeof(Win32GroupData));
    memset(groupData, 0, sizeof(Win32GroupData));
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

/**
 * Free a thread group data structure.
 *
 * @sideeffect
 *      Memory freed and system objects deleted.
 *
 * @see Win32GroupData
 * @see AllocGroupData
 */
static void
FreeGroupData(
    Win32GroupData *groupData)  /*!< Structure to free. */
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

        CloseHandle(groupData->eventGcScheduled);
        CloseHandle(groupData->eventGcDone);
        DeleteCriticalSection(&groupData->csGc);

        DeleteCriticalSection(&groupData->csRoots);
    }

    GcCleanupGroup((GroupData *) groupData);
    free(groupData);
}

/** @endcond @endprivate */

/* End of Thread-Local Storage *//*!\}*/


/***************************************************************************//*!
 * \name Process & Threads
 ***************************************************************************\{*/

/** @beginprivate @cond PRIVATE */

/**
 * Enter the thread. If this is the first nested call, initialize thread data.
 * If this is the first thread in its group, initialize group data as well.
 *
 * @retval <>0> if this is the first nested call.
 * @retval 0    otherwise.
 *
 * @see @ref threading_models "Threading Model Constants"
 * @see ThreadData
 * @see Win32GroupData
 * @see PlatLeave
 * @see Col_Init
 */
int
PlatEnter(
    unsigned int model) /*!< Threading model. */
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
            Win32GroupData *groupData = sharedGroups;
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

/**
 * Leave the thread. If this is the first nested call, cleanup thread data.
 * If this is the last thread in its group, cleanup group data as well.
 *
 * @retval <>0  if this is the last nested call.
 * @retval 0    otherwise.
 *
 * @see ThreadData
 * @see Win32GroupData
 * @see PlatEnter
 * @see Col_Cleanup
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

        FreeGroupData((Win32GroupData *) data->groupData);
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

                FreeGroupData((Win32GroupData *) data->groupData);
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

/**
 * Thread dedicated to the GC process. Activated when one of the worker threads
 * in the group triggers the GC.
 *
 * @return Always zero.
 *
 * @sideeffect
 *      Calls #PerformGC.
 *
 * @see AllocGroupData
 * @see PerformGC
 */
static DWORD WINAPI
GcThreadProc(
    LPVOID lpParameter) /*!< #Win32GroupData. */
{
    Win32GroupData *groupData = (Win32GroupData *) lpParameter;
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

/**
 * Called when a worker thread calls the outermost Col_PauseGC().
 *
 * @sideeffect
 *      May block as long as a GC is underway.
 *
 * @see GcThreadProc
 * @see SyncPauseGC
 * @see Col_PauseGC
 */
void
PlatSyncPauseGC(
    GroupData *data)    /*!< Group-specific data. */
{
    Win32GroupData *groupData = (Win32GroupData *) data;
    ASSERT(groupData->data.model != COL_SINGLE);
    WaitForSingleObject(groupData->eventGcDone, INFINITE);
    EnterCriticalSection(&groupData->csGc);
    {
        groupData->nbActive++;
    }
    LeaveCriticalSection(&groupData->csGc);
}

/**
 * Called when a worker thread calls the outermost Col_TryPauseGC().
 *
 * @retval <>0  if successful.
 * @retval 0    if call would block.
 *
 * @see GcThreadProc
 * @see TrySyncPauseGC
 * @see Col_TryPauseGC
 */
int
PlatTrySyncPauseGC(
    GroupData *data)    /*!< Group-specific data. */
{
    Win32GroupData *groupData = (Win32GroupData *) data;
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

/**
 * Called when a worker thread calls the outermost Col_ResumeGC().
 *
 * @sideeffect
 *      If last thread in group, may trigger the GC in the dedicated thread if
 *      previously scheduled. This will block further calls to
 *      Col_PauseGC() / PlatSyncPauseGC().
 *
 * @see GcThreadProc
 * @see SyncResumeGC
 * @see Col_ResumeGC
 */
void
PlatSyncResumeGC(
    GroupData *data,    /*!< Group-specific data. */
    int performGc)      /*!< Whether to perform GC. */
{
    Win32GroupData *groupData = (Win32GroupData *) data;
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

/**
 * Enter protected section around root management structures.
 *
 * @sideeffect
 *      Blocks until no thread owns the section.
 *
 * @see PlatLeaveProtectRoots
 * @see EnterProtectRoots
 */
void
PlatEnterProtectRoots(
    GroupData *data)    /*!< Group-specific data. */
{
    Win32GroupData *groupData = (Win32GroupData *) data;
    ASSERT(groupData->data.model >= COL_SHARED);
    EnterCriticalSection(&groupData->csRoots);
}

/**
 * Leave protected section around root management structures.
 *
 * @sideeffect
 *      May unblock any thread waiting for the section.
 *
 * @see PlatEnterProtectRoots
 * @see LeaveProtectRoots
 */
void
PlatLeaveProtectRoots(
    GroupData *data)    /*!< Group-specific data. */
{
    Win32GroupData *groupData = (Win32GroupData *) data;
    ASSERT(groupData->data.model >= COL_SHARED);
    LeaveCriticalSection(&groupData->csRoots);
}

/** @endcond @endprivate */

/* End of Process & Threads *//*!\}*/


/***************************************************************************//*!
 * \name System Page Allocation
 ***************************************************************************\{*/

/** @beginprivate @cond PRIVATE */

/**
 * Critical section protecting address range management.
 *
 * - #ranges:          Reserved address ranges for general purpose.
 * - #dedicatedRanges: Dedicated address ranges for large pages.
 *
 * @see PlatEnterProtectAddressRanges
 * @see PlatLeaveProtectAddressRanges
 */
CRITICAL_SECTION csRange;

/**
 * Reserve an address range.
 *
 * @return The reserved range's base address, or NULL if failure.
 */
void *
PlatReserveRange(
    size_t size,    /*!< Number of pages to reserve. */
    int alloc)      /*!< Whether to allocate the range pages as well. */
{
    return VirtualAlloc(NULL, size << shiftPage,
        MEM_RESERVE | (alloc ? MEM_COMMIT : 0), PAGE_READWRITE);
}

/**
 * Release an address range.
 *
 * @retval <>0  for success.
 * @retval 0    for failure.
 */
int
PlatReleaseRange(
    void *base,     /*!< Base address of range to release. */
    size_t size)    /*!< Number of pages in range. */
{
    return VirtualFree(base, 0, MEM_RELEASE);
}

/**
 * Allocate pages in reserved range.
 *
 * @retval <>0  for success.
 * @retval 0    for failure.
 */
int
PlatAllocPages(
    void *addr,     /*!< Address of first page to allocate. */
    size_t number)  /*!< Number of pages to allocate. */
{
    return !!VirtualAlloc(addr, number << shiftPage, MEM_COMMIT,
            PAGE_READWRITE);
}

/**
 * Free pages in reserved range.
 *
 * @retval <>0  for success.
 * @retval 0    for failure.
 */
int
PlatFreePages(
    void *addr,     /*!< Address of first page to free. */
    size_t number)  /*!< Number of pages to free. */
{
    return VirtualFree(addr, number << shiftPage, MEM_DECOMMIT);
}

/**
 * Protect/unprotect pages in reserved range.
 *
 * @retval <>0  for success.
 * @retval 0    for failure.
 */
int
PlatProtectPages(
    void *addr,     /*!< Address of first page to protect/unprotect */
    size_t number,  /*!< Number of pages to protect/unprotect. */
    int protect)    /*!< Whether to protect or unprotect pages. */
{
    DWORD old;
    return VirtualProtect(addr, number << shiftPage,
            (protect ? PAGE_READONLY : PAGE_READWRITE), &old);
}

/**
 * Called upon exception.
 *
 * @retval EXCEPTION_CONTINUE_SEARCH        for unhandled exceptions, will pass
 *                                          exception to other handlers.
 * @retval EXCEPTION_CONTINUE_EXECUTION     for handled exceptions, will resume
 *                                          execution of calling code.
 *
 * @see SysPageProtect
 */
static LONG CALLBACK
PageProtectVectoredHandler(
    PEXCEPTION_POINTERS exceptionInfo)  /*!< Info about caught exception. */
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

/** @endcond @endprivate */

/* End of System Page Allocation *//*!\}*/


/***************************************************************************//*!
 * \name Initialization/Cleanup
 ***************************************************************************\{*/

/** @beginprivate @cond PRIVATE */

/**
 * Windows DLL entry point.
 *
 * @return Always true.
 *
 * @see Init
 */
BOOL APIENTRY
DllMain(
    HMODULE hModule,    /*!< A handle to the DLL module. */
    DWORD dwReason,     /*!< The reason code that indicates why the DLL
                             entry-point function is being called. */
    LPVOID lpReserved)  /*!< Unused. */
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

/**
 * Initialization routine. Called through DllMain().
 *
 * @return Always true.
 *
 * @sideeffect
 *      - Create thread-local storage key #tlsToken (freed upon
 *        DLL_PROCESS_DETACH in #DllMain).
 *      - Install memory protection exception handler
 *        PageProtectVectoredHandler()for parent tracking.
 *
 * @see DllMain
 * @see systemPageSize
 * @see allocGranularity
 * @see shiftPage
 */
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

/** @endcond @endprivate */

/* End of Initialization/Cleanup *//*!\}*/

/* End of System and Architecture (Win32-specific) *//*!\}*/
/*! @endcond @endprivate */