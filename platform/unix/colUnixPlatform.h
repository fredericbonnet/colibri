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
 * Definitions:
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

#endif /* _COLIBRI_UNIXPLATFORM */
