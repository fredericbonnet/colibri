/**
 * @file colUnixPlatform.h
 *
 * This header file defines Unix-specific primitives.
 *
 * @see colPlatform.h
 *
 * @beginprivate @cond PRIVATE
 */

#ifndef _COLIBRI_UNIXPLATFORM
#define _COLIBRI_UNIXPLATFORM

#include <pthread.h>


/*
===========================================================================*//*!
\internal \defgroup arch_unix System and Architecture (Unix-specific)
\ingroup arch

Unix-specific implementation of generic primitives.
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Thread-Local Storage
 ***************************************************************************\{*/

/**
 * Get pointer to thread-specific data.
 *
 * @see ThreadData
 * @see tlsToken
 */
#define PlatGetThreadData() \
    ((ThreadData *) pthread_getspecific(tsdKey))

/*
 * Remaining declarations.
 */

extern pthread_key_t tsdKey;

/* End of Thread-Local Storage *//*!\}*/


/***************************************************************************//*!
 * \name System Page Allocation
 ***************************************************************************\{*/

/**
 * Per-thread recursion count for PlatEnterProtectAddressRanges() calls.
 * As pthread mutexes are non-recursive, and recursive versions are not
 * portable, this provides a simple, portable way to allow nested calls.
 * Such calls notably occur during UpdateParents() when allocating new
 * pages to store parent descriptor cells, which eventually calls
 * SysPageAlloc().
 *
 * We store the recursion counter as an integer just after the #ThreadData
 * structure stored in thread-local storage (see allocation in
 * PlatEnter()). This counter prevents calls to pthread_mutex_lock when
 * nonzero and to pthread_mutex_unlock when > 1.
 *
 * @see PlatEnter
 * @see PlatEnterProtectAddressRanges
 * @see PlatLeaveProtectAddressRanges>
 */
#define UNIX_PROTECT_ADDRESS_RANGES_RECURSE(threadData) \
    (*(int *)(threadData+1))

/**
 * Enter protected section around address range management structures.
 *
 * @sideeffect
 *      Blocks until no thread owns the section.
 *
 * @see PlatLeaveProtectAddressRanges
 * @see UNIX_PROTECT_ADDRESS_RANGES_RECURSE
 */
#define PlatEnterProtectAddressRanges() \
    (UNIX_PROTECT_ADDRESS_RANGES_RECURSE(PlatGetThreadData())++ ? 0 : pthread_mutex_lock(&mutexRange))

/**
 * Leave protected section around address range management structures.
 *
 * @sideeffect
 *      May unblock any thread waiting for the section.
 *
 * @see PlatEnterProtectAddressRanges
 * @see UNIX_PROTECT_ADDRESS_RANGES_RECURSE
 */
#define PlatLeaveProtectAddressRanges() \
    (--UNIX_PROTECT_ADDRESS_RANGES_RECURSE(PlatGetThreadData()) ? 0 : pthread_mutex_unlock(&mutexRange))

/*
 * Remaining declarations.
 */

extern pthread_mutex_t mutexRange;

/* End of System Page Allocation *//*!\}*/

/* End of System and Architecture (Unix-specific) *//*!\}*/

#endif /* _COLIBRI_UNIXPLATFORM */
/*! @endcond @endprivate */