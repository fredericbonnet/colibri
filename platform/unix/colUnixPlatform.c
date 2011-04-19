#include "../../colibri.h"
#include "../../colInternal.h"
#include "../../colPlatform.h"

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>

static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t tsdKey;

/*
 * Thread-local data.
 */

typedef struct {
    size_t nestCount;
    GcMemInfo gcMemInfo; 
    Col_ErrorProc *errorProc;
} ThreadData;

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

static void Init(void);


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
    return &((ThreadData *) pthread_getspecific(tsdKey))->gcMemInfo;
}

Col_ErrorProc **
PlatGetErrorProcPtr() {
    return &((ThreadData *) pthread_getspecific(tsdKey))->errorProc;
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
    ThreadData *info;

    /*
     * Ensures that the TLS key is created once.
     */

    pthread_once(&once, Init);
    info = (ThreadData *) pthread_getspecific(tsdKey);
    if (!info) {
	/*
	 * Not yet initialized.
	*/

	info = (ThreadData *) malloc(sizeof(ThreadData));
	info->nestCount = 0;
	pthread_setspecific(tsdKey, info);
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
    ThreadData *info = (ThreadData *) pthread_getspecific(tsdKey);
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
    ThreadData *info = (ThreadData *) pthread_getspecific(tsdKey);
    if (!info || info->nestCount != 0) {
	/* TODO: exception ? */
	return;
    }

    free(info);
    pthread_setspecific(tsdKey, 0);
}
