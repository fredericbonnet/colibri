#include "colibri.h"
#include "colInternal.h"
#include "colPlatform.h"

#include <stdio.h>
#include <stdlib.h>


/*
 * Without thread support, use static data instead of thread-local storage.
 */

#ifdef COL_THREADS
#   define GetErrorProcPtr PlatGetErrorProcPtr
#else /* COL_THREADS */
static Col_ErrorProc *errorProc;
#   define GetErrorProcPtr() (&errorProc)
#endif /* COL_THREADS */

/*
 *---------------------------------------------------------------------------
 *
 * Col_Init --
 * Col_Cleanup --
 *
 *	Initialize/cleanup the library. Must be called in every thread.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Initialize/cleanup the garbage collector.
 *
 *---------------------------------------------------------------------------
 */

void 
Col_Init() 
{
    if (PlatEnter()) {
	GcInit();
	Col_SetErrorProc(NULL);
    }
}
void 
Col_Cleanup() 
{
    if (PlatLeave()) {
	GcCleanup();
	PlatCleanup();
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_Error --
 *
 *	Signal an error condition.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Default implementation exits the processus when level is critical.
 *
 *---------------------------------------------------------------------------
 */

void
Col_Error(
    Col_ErrorLevel level,	/* Error level. */
    const char *format,		/* Format string fed to fprintf. */
    ...)			/* Remaining arguments fed to fprintf. */
{
    va_list args;
    Col_ErrorProc *errorProc = *GetErrorProcPtr();

    va_start(args, format);
    if (errorProc) {
	/*
	 * Call custom proc.
	 */

	(*errorProc)(level, format, args);
	return;
    }

    /*
     * Print message and abort process.
     */

    switch (level) {
	case COL_FATAL:
	    fprintf(stderr, "[FATAL] ");
	    break;

	case COL_ERROR:
	    fprintf(stderr, "[ERROR] ");
	    break;
    }
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    fflush(stderr);

    abort();
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_SetErrorProc --
 *
 *	Set a custom error proc (which may be NULL).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Replace current error proc (default is NULL).
 *
 *---------------------------------------------------------------------------
 */

void
Col_SetErrorProc(
    Col_ErrorProc *proc)	/* The new error proc. */
{
    *GetErrorProcPtr() = proc;
}
