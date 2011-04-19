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
 * Definitions:
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

#endif /* _COLIBRI_WIN32PLATFORM */
