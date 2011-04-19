#ifndef _COLIBRI_UNIXPLATFORM
#define _COLIBRI_UNIXPLATFORM

#include <pthread.h>

/*
 * Thread-local storage.
 */

extern pthread_key_t tsdKey;

#define PlatGetThreadData() \
    ((ThreadData *) pthread_getspecific(tsdKey))

#endif /* _COLIBRI_UNIXPLATFORM */
