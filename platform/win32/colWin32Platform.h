#ifndef _COLIBRI_WIN32PLATFORM
#define _COLIBRI_WIN32PLATFORM

#include <windows.h>

/*
 * Thread-local storage.
 */

extern DWORD tlsToken;

#define PlatGetThreadData() \
    ((ThreadData *) TlsGetValue(tlsToken))

#endif /* _COLIBRI_WIN32PLATFORM */
