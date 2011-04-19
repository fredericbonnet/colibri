#include "colibri.h"
#include "colInternal.h"
#include "colPlatform.h"

#include <stdio.h>
#include <stdlib.h>


/*
 * Thread-local storage.
 */

#define GetErrorProcPtr PlatGetErrorProcPtr

/*
 *---------------------------------------------------------------------------
 *
 * Col_Init --
 * Col_Cleanup --
 *
 *	Initialize/cleanup the library. Must be called in every thread.
 *
 *	Several threading models are possible:
 *
 *	 - COL_SINGLE : strict appartment + stop-the-world model. GC is done 
 *	   synchronously when client thread resumes GC (Col_ResumeGC).
 *	 - COL_ASYNC : strict appartment model with asynchronous GC. GC uses a 
 *	   dedicated thread for asynchronous processing, the client thread 
 *	   cannot pause a running GC and is blocked until completion.
 *	 - COL_SHARED : shared multithreaded model with GC-preference. Data can 
 *	   be shared across client threads of the same group (COL_SHARED is the 
 *	   base index value). GC uses a dedicated thread for asynchronous 
 *	   processing.; GC process starts once all client threads get out of 
 *	   pause, no client thread can pause a scheduled GC.
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
Col_Init(
    unsigned int model)		/* Threading model. */
{
    PlatEnter(model);
}
void 
Col_Cleanup() 
{
    PlatLeave();
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
    ThreadData *data = PlatGetThreadData();

    va_start(args, format);
    if (data->errorProc) {
	/*
	 * Call custom proc.
	 */

	(data->errorProc)(level, format, args);
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
    PlatGetThreadData()->errorProc = proc;
}
