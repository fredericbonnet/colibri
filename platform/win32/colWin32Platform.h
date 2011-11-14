/*
 * Internal File: colWin32Platform.h
 *
 *	This header file defines Win32-specific features.
 */

#ifndef _COLIBRI_WIN32PLATFORM
#define _COLIBRI_WIN32PLATFORM

#include <windows.h>


/****************************************************************************
 * Internal Group: Thread-Local Storage
 *
 * Declarations:
 *	<tlsToken>
 ****************************************************************************/

extern DWORD tlsToken;

/*---------------------------------------------------------------------------
 * Internal Macro: PlatGetThreadData
 *
 *	Get pointer to thread-specific data.
 *
 * See also:
 *	<ThreadData>, <tlsToken>
 *---------------------------------------------------------------------------*/

#define PlatGetThreadData() \
    ((ThreadData *) TlsGetValue(tlsToken))


/****************************************************************************
 * Internal Group: System Page Allocation
 *
 * Declarations:
 *	<csRange>
 ****************************************************************************/

extern CRITICAL_SECTION csRange;

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
    EnterCriticalSection(&csRange)

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
    LeaveCriticalSection(&csRange)

#endif /* _COLIBRI_WIN32PLATFORM */
