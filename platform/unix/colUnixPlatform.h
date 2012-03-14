/*
 * Internal Header: colUnixPlatform.h
 *
 *	This header file defines Unix-specific facilities.
 */

#ifndef _COLIBRI_UNIXPLATFORM
#define _COLIBRI_UNIXPLATFORM

#include <pthread.h>


/****************************************************************************
 * Internal Section: Thread-Local Storage
 *
 * Declarations:
 *	<tsdKey>
 ****************************************************************************/

extern pthread_key_t tsdKey;

/*---------------------------------------------------------------------------
 * Internal Macro: PlatGetThreadData
 *
 *	Get pointer to thread-specific data.
 *
 * See also:
 *	<ThreadData>, <tsdKey>
 *---------------------------------------------------------------------------*/

#define PlatGetThreadData() \
    ((ThreadData *) pthread_getspecific(tsdKey))


/****************************************************************************
 * Internal Section: System Page Allocation
 *
 * Declarations:
 *	<mutexRange>
 ****************************************************************************/

extern pthread_mutex_t mutexRange;

/*---------------------------------------------------------------------------
 * Internal Macro: UNIX_PROTECT_ADDRESS_RANGES_RECURSE
 *
 *	Per-thread recursion count for <PlatEnterProtectAddressRanges> calls. 
 *	As pthread mutexes are non-recursive, and recursive versions are not 
 *	portable, this provides a simple, portable way to allow nested calls. 
 *	Such calls notably occur during <UpdateParents> when allocating new 
 *	pages to store parent descriptor cells, which eventually calls 
 *	<SysPageAlloc>.
 *
 *	We store the recursion counter as an integer just after the <ThreadData>
 *	structure stored in thread-local storage (see allocation in 
 *	<PlatEnter>). This counter prevents calls to pthread_mutex_lock when 
 *	non-zero and to pthread_mutex_unlock when > 1.
 *
 * See also:
 *	<PlatEnter>, <PlatEnterProtectAddressRanges>, 
 *	<PlatLeaveProtectAddressRanges>
 *---------------------------------------------------------------------------*/

#define UNIX_PROTECT_ADDRESS_RANGES_RECURSE(threadData) \
    (*(int *)(threadData+1))

/*---------------------------------------------------------------------------
 * Internal Macro: PlatEnterProtectAddressRanges
 *
 *	Enter protected section around address range management structures.
 *
 * Side effects:
 *	Blocks until no thread owns the section.
 *
 * See also:
 *	<PlatLeaveProtectAddressRanges>, <UNIX_PROTECT_ADDRESS_RANGES_RECURSE>
 *---------------------------------------------------------------------------*/

#define PlatEnterProtectAddressRanges() \
    (UNIX_PROTECT_ADDRESS_RANGES_RECURSE(PlatGetThreadData())++ ? 0 : pthread_mutex_lock(&mutexRange))

/*---------------------------------------------------------------------------
 * Internal Macro: PlatLeaveProtectAddressRanges
 *
 *	Leave protected section around address range management structures.
 *
 * Side effects:
 *	May unblock any thread waiting for the section.
 *
 * See also:
 *	<PlatEnterProtectAddressRanges>, <UNIX_PROTECT_ADDRESS_RANGES_RECURSE>
 *---------------------------------------------------------------------------*/

#define PlatLeaveProtectAddressRanges() \
    (--UNIX_PROTECT_ADDRESS_RANGES_RECURSE(PlatGetThreadData()) ? 0 : pthread_mutex_unlock(&mutexRange))

#endif /* _COLIBRI_UNIXPLATFORM */
