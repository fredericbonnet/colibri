#include "../../colibri.h"
#include "../../colInt.h"
#include "../../colPlat.h"

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>

static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t tsdKey;
typedef struct {
    size_t nestCount;
    GcMemInfo info; 
} ThreadData;

/*
 * Prototypes for functions used only in this file.
 */

static void Init(void);


/*
 *----------------------------------------------------------------
 * Process & thread.
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


/*
 *----------------------------------------------------------------
 * System page allocator.
 *----------------------------------------------------------------
 */

/*
 * Memory is allocated with mmap in anonymous mode, which manages single pages.
 */

/*
 *---------------------------------------------------------------------------
 *
 * PlatAllocInitAllocInit --
 *
 *	Initialize the platform-specific memory allocator stuff.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Global data is initialized.
 *
 *---------------------------------------------------------------------------
 */

void
PlatAllocInit() {
    systemPageSize = sysconf(_SC_PAGESIZE);
}

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
 *	A new page is allocated.
 *
 *---------------------------------------------------------------------------
 */

void * 
PlatSysPageAlloc(
    MemoryPool * pool)		/* The pool for which to alloc page. */
{
    void * page = mmap(NULL, systemPageSize, PROT_READ | PROT_WRITE, 
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
 *	Free a system page.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The page is freed.
 *
 *---------------------------------------------------------------------------
 */

void 
PlatSysPageFree(
    MemoryPool * pool,		/* The pool the page belongs to. */
    void * page)		/* The page to free. */
{
    munmap(page, systemPageSize);
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
 *	None.
 *
 *---------------------------------------------------------------------------
 */

void 
PlatSysPageCleanup(MemoryPool * pool) {
    /* Nothing to do. */
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
    return &((ThreadData *) pthread_getspecific(tsdKey))->info;
}

#endif /* COL_THREADS */
