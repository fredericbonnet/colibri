#include "colibri.h"
#include "colInt.h"
#include "colPlat.h"


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
Col_Init() {
    if (PlatEnter()) {
	AllocInit();
	GcInit();
    }
}
void 
Col_Cleanup() {
    if (PlatLeave()) {
	GcCleanup();
	PlatCleanup();
    }
}
