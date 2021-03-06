<a id="col_win32_platform_8h"></a>
# File colWin32Platform.h

![][C++]

**Location**: `src/platform/win32/colWin32Platform.h`

This header file defines Win32-specific primitives.

**See also**: colPlatform.h

## Source

```cpp
/**
 * @file colWin32Platform.h
 *
 * This header file defines Win32-specific primitives.
 *
 * @see colPlatform.h
 *
 * @beginprivate @cond PRIVATE
 */

#ifndef _COLIBRI_WIN32PLATFORM
#define _COLIBRI_WIN32PLATFORM

#include <windows.h>


/*
===========================================================================*//*!
\internal \defgroup arch_win32 System and Architecture (Win32-specific)
\ingroup arch

Win32-specific implementation of generic primitives.
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
    ((ThreadData *) TlsGetValue(tlsToken))

/*
 * Remaining declarations.
 */

extern DWORD tlsToken;

/* End of Thread-Local Storage *//*!\}*/


/***************************************************************************//*!
 * \name System Page Allocation
 ***************************************************************************\{*/

/**
 * Enter protected section around address range management structures.
 *
 * @sideeffect
 *      Blocks until no thread owns the section.
 *
 * @see PlatLeaveProtectAddressRanges
 */
#define PlatEnterProtectAddressRanges() \
    EnterCriticalSection(&csRange)

/**
 * Leave protected section around address range management structures.
 *
 * @sideeffect
 *      May unblock any thread waiting for the section.
 *
 * @see PlatEnterProtectAddressRanges
 */
#define PlatLeaveProtectAddressRanges() \
    LeaveCriticalSection(&csRange)

/*
 * Remaining declarations.
 */

extern CRITICAL_SECTION csRange;

/* End of System Page Allocation *//*!\}*/

/* End of System and Architecture (Win32-specific) *//*!\}*/

#endif /* _COLIBRI_WIN32PLATFORM */
/*! @endcond @endprivate */
```

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[private]: https://img.shields.io/badge/-private-red (private)
[static]: https://img.shields.io/badge/-static-lightgrey (static)