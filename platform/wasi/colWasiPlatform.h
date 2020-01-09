/**
 * @file colWasiPlatform.h
 *
 * This header file defines WASI-specific primitives.
 *
 * @see colPlatform.h
 *
 * @beginprivate @cond PRIVATE
 */

#ifdef COL_USE_THREADS
#   error threads are not supported on WASI platform, rebuild with -DUSE_THREADS=OFF
#endif

#ifndef _COLIBRI_WASIPLATFORM
#define _COLIBRI_WASIPLATFORM


/*
===========================================================================*//*!
\internal \defgroup arch_wasi System and Architecture (WASI-specific)
\ingroup arch

WASI-specific implementation of generic primitives.
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Thread-Local Storage
 ***************************************************************************\{*/

/**
 * Get pointer to thread-specific data.
 *
 * @see ThreadData
 */
#define PlatGetThreadData() \
    ((ThreadData *) WasiGetThreadData())

/*
 * Remaining declarations.
 */

void *                  WasiGetThreadData();
void                    WasiSetThreadData(void *data);

/* End of Thread-Local Storage *//*!\}*/


/***************************************************************************//*!
 * \name System Page Allocation
 ***************************************************************************\{*/

/**
 * Enter protected section around address range management structures.
 *
 * @see PlatLeaveProtectAddressRanges
 */
#define PlatEnterProtectAddressRanges() /* NOOP */

/**
 * Leave protected section around address range management structures.
 *
 * @see PlatEnterProtectAddressRanges
 */
#define PlatLeaveProtectAddressRanges() /* NOOP */

/* End of System Page Allocation *//*!\}*/

/* End of System and Architecture (WASI-specific) *//*!\}*/

#endif /* _COLIBRI_WASIPLATFORM */
/*! @endcond @endprivate */