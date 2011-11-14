/*
 * Internal File: colUnixPlatform.h
 *
 *	This header file defines Unix-specific facilities.
 */

#ifndef _COLIBRI_UNIXPLATFORM
#define _COLIBRI_UNIXPLATFORM

#include <pthread.h>


/****************************************************************************
 * Internal Group: Thread-Local Storage
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
 * Internal Group: System Page Allocation
 *
 * Declarations:
 *	<mutexRange>
 ****************************************************************************/

extern pthread_mutex_t mutexRange;

/*---------------------------------------------------------------------------
 * Internal Macro: PlatEnterProtectAddressRanges
 *
 *	Enter protected section around address range management structures.
 *
 * Side effects:
 *	Blocks until no thread owns the section.
 *
 * See also:
 *	<PlatLeaveProtectAddressRanges>
 *---------------------------------------------------------------------------*/

#define PlatEnterProtectAddressRanges() \
    pthread_mutex_lock(&mutexRange)

/*---------------------------------------------------------------------------
 * Internal Macro: PlatLeaveProtectAddressRanges
 *
 *	Leave protected section around address range management structures.
 *
 * Side effects:
 *	May unblock any thread waiting for the section.
 *
 * See also:
 *	<PlatEnterProtectAddressRanges>
 *---------------------------------------------------------------------------*/

#define PlatLeaveProtectAddressRanges() \
    pthread_mutex_unlock(&mutexRange)

#endif /* _COLIBRI_UNIXPLATFORM */
