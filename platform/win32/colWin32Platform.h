/*                                                                              *//*!   @cond PRIVATE @file \
 * colWin32Platform.h
 *
 *  This header file defines Win32-specific primitives.
 *
 *  @see colPlatform.h
 *
 *  @private
 */

#ifndef _COLIBRI_WIN32PLATFORM
#define _COLIBRI_WIN32PLATFORM

#include <windows.h>


/*
================================================================================*//*!   @addtogroup arch_win32 \
System and Architecture (Win32-specific)
                                                                                        @ingroup arch
  Win32-specific implementation of generic primitives.                          *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Thread-Local Storage                                                         *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * PlatGetThreadData
 *
 *  Get pointer to thread-specific data.
 *
 *  @see ThreadData
 *  @see tlsToken
 *//*-----------------------------------------------------------------------*/

#define PlatGetThreadData() \
    ((ThreadData *) TlsGetValue(tlsToken))

/*
 * Remaining declarations.
 */
                                                                                #       ifndef DOXYGEN
extern DWORD tlsToken;
                                                                                #       endif DOXYGEN
                                                                                /*!     @} */
/********************************************************************************//*!   @name \
 * System Page Allocation                                                       *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * PlatEnterProtectAddressRanges
 *
 *  Enter protected section around address range management structures.
 *
 *  @sideeffect
 *      Blocks until no thread owns the section.
 *
 *  @see PlatLeaveProtectAddressRanges
 *//*-----------------------------------------------------------------------*/

#define PlatEnterProtectAddressRanges() \
    EnterCriticalSection(&csRange)

/*---------------------------------------------------------------------------   *//*!   @def \
 * PlatLeaveProtectAddressRanges
 *
 *  Leave protected section around address range management structures.
 *
 *  @sideeffect
 *      May unblock any thread waiting for the section.
 *
 *  @see PlatEnterProtectAddressRanges
 *//*-----------------------------------------------------------------------*/

#define PlatLeaveProtectAddressRanges() \
    LeaveCriticalSection(&csRange)

/*
 * Remaining declarations.
 */
                                                                                #       ifndef DOXYGEN
extern CRITICAL_SECTION csRange;
                                                                                #       endif DOXYGEN
                                                                                /*!     @} */
                                                                                /*!     @} */
#endif /* _COLIBRI_WIN32PLATFORM */
                                                                                /*!     @endcond */