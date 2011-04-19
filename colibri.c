#include <windows.h>

#include "colibri.h"
#include "colInt.h"

BOOL APIENTRY 
DllMain( 
    HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	    break;
    }
    return TRUE;
}


/*
 *---------------------------------------------------------------------------
 *
 * Col_Init --
 *
 *	Initialize the rope engine.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Initialize the allocator and garbage collector. See respective procs
 *	for infos.
 *
 *---------------------------------------------------------------------------
 */

void 
Col_Init() {
    AllocInit();
    GCInit();
}
