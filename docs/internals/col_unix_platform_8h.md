<a id="col_unix_platform_8h"></a>
# File colUnixPlatform.h

![][C++]

**Location**: `src/platform/unix/colUnixPlatform.h`

This header file defines Unix-specific primitives.

**See also**: [colPlatform.h](col_platform_8h.md#col_platform_8h)

## Includes

* <pthread.h>

```mermaid
graph LR
2["pthread.h"]

1["src/platform/unix/colUnixPlatform.h"]
click 1 "col_unix_platform_8h.md#col_unix_platform_8h"
1 --> 2

```

## Thread-Local Storage

<a id="group__arch__unix_1ga6964b3c4d4787a9defb7aae57825d92c"></a>
### Macro PlatGetThreadData

![][public]

```cpp
#define PlatGetThreadData     (([ThreadData](struct_thread_data.md#struct_thread_data) *) pthread_getspecific([tsdKey](col_unix_platform_8c.md#group__arch__unix_1gacfa4b2395a4c29a432e682bfbb407a35)))(  )
```

Get pointer to thread-specific data.

**See also**: [ThreadData](struct_thread_data.md#struct_thread_data), [tlsToken](col_win32_platform_8c.md#group__arch__win32_1ga338377fedfe1571e9dd2118048a516c7)



<a id="group__arch__unix_1gacfa4b2395a4c29a432e682bfbb407a35"></a>
### Variable tsdKey

![][private]

**Definition**: `src/platform/unix/colUnixPlatform.h` (line 43)

```cpp
pthread_key_t tsdKey
```

Thread-speficic data identifier.

Used to get thread-specific data.








**See also**: [ThreadData](struct_thread_data.md#struct_thread_data), [Init](col_unix_platform_8c.md#group__arch__unix_1ga1716946dffd7f7f6a9c8f46406ab0732)



**Type**: pthread_key_t

**Referenced by**:

* [Init](col_unix_platform_8c.md#group__arch__unix_1ga1716946dffd7f7f6a9c8f46406ab0732)
* [PlatEnter](col_unix_platform_8c.md#group__arch__unix_1gaa42fe97b4b462c9483110a715c1eb1d1)
* [PlatLeave](col_unix_platform_8c.md#group__arch__unix_1ga445bf6b3cd4afc09367a6d9fce001a2e)

## System Page Allocation

<a id="group__arch__unix_1ga9e2c8984ce04b1ef7c69111e31c508cd"></a>
### Macro UNIX\_PROTECT\_ADDRESS\_RANGES\_RECURSE

![][public]

```cpp
#define UNIX_PROTECT_ADDRESS_RANGES_RECURSE     (*(int *)(threadData+1))( threadData )
```

Per-thread recursion count for [PlatEnterProtectAddressRanges()](col_unix_platform_8h.md#group__arch__unix_1ga0d52de05fb3a0897f54579ab12519159) calls.

As pthread mutexes are non-recursive, and recursive versions are not portable, this provides a simple, portable way to allow nested calls. Such calls notably occur during [UpdateParents()](col_alloc_8c.md#group__gc__parents_1gaa3d85dc993fb1b9831f82c25b8c07d3c) when allocating new pages to store parent descriptor cells, which eventually calls [SysPageAlloc()](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f).





We store the recursion counter as an integer just after the [ThreadData](struct_thread_data.md#struct_thread_data) structure stored in thread-local storage (see allocation in [PlatEnter()](col_platform_8h.md#group__arch_1gaa42fe97b4b462c9483110a715c1eb1d1)). This counter prevents calls to pthread_mutex_lock when nonzero and to pthread_mutex_unlock when > 1.









**See also**: [PlatEnter](col_win32_platform_8c.md#group__arch__win32_1gaa42fe97b4b462c9483110a715c1eb1d1), [PlatEnterProtectAddressRanges](col_unix_platform_8h.md#group__arch__unix_1ga0d52de05fb3a0897f54579ab12519159), [PlatLeaveProtectAddressRanges](col_unix_platform_8h.md#group__arch__unix_1gab6be4d622dd8c6bc4c283a1039962e5a)>



<a id="group__arch__unix_1ga0d52de05fb3a0897f54579ab12519159"></a>
### Macro PlatEnterProtectAddressRanges

![][public]

```cpp
#define PlatEnterProtectAddressRanges     ([UNIX\_PROTECT\_ADDRESS\_RANGES\_RECURSE](col_unix_platform_8h.md#group__arch__unix_1ga9e2c8984ce04b1ef7c69111e31c508cd)([PlatGetThreadData](col_win32_platform_8h.md#group__arch__win32_1ga6964b3c4d4787a9defb7aae57825d92c)())++ ? 0 : pthread_mutex_lock(&[mutexRange](col_unix_platform_8c.md#group__arch__unix_1ga61c38d216e226603ecd1e143f8757970)))(  )
```

Enter protected section around address range management structures.

**Side Effect**:

Blocks until no thread owns the section.




**See also**: [PlatLeaveProtectAddressRanges](col_unix_platform_8h.md#group__arch__unix_1gab6be4d622dd8c6bc4c283a1039962e5a), [UNIX\_PROTECT\_ADDRESS\_RANGES\_RECURSE](col_unix_platform_8h.md#group__arch__unix_1ga9e2c8984ce04b1ef7c69111e31c508cd)



<a id="group__arch__unix_1gab6be4d622dd8c6bc4c283a1039962e5a"></a>
### Macro PlatLeaveProtectAddressRanges

![][public]

```cpp
#define PlatLeaveProtectAddressRanges     (--[UNIX\_PROTECT\_ADDRESS\_RANGES\_RECURSE](col_unix_platform_8h.md#group__arch__unix_1ga9e2c8984ce04b1ef7c69111e31c508cd)([PlatGetThreadData](col_win32_platform_8h.md#group__arch__win32_1ga6964b3c4d4787a9defb7aae57825d92c)()) ? 0 : pthread_mutex_unlock(&[mutexRange](col_unix_platform_8c.md#group__arch__unix_1ga61c38d216e226603ecd1e143f8757970)))(  )
```

Leave protected section around address range management structures.

**Side Effect**:

May unblock any thread waiting for the section.




**See also**: [PlatEnterProtectAddressRanges](col_unix_platform_8h.md#group__arch__unix_1ga0d52de05fb3a0897f54579ab12519159), [UNIX\_PROTECT\_ADDRESS\_RANGES\_RECURSE](col_unix_platform_8h.md#group__arch__unix_1ga9e2c8984ce04b1ef7c69111e31c508cd)



<a id="group__arch__unix_1ga61c38d216e226603ecd1e143f8757970"></a>
### Variable mutexRange

![][private]

**Definition**: `src/platform/unix/colUnixPlatform.h` (line 100)

```cpp
pthread_mutex_t mutexRange
```

Mutex protecting address range management.

* [ranges](col_alloc_8c.md#group__alloc_1ga9ac4c516a0888195d1f2ca4721f633f8): Reserved address ranges for general purpose.

* [dedicatedRanges](col_alloc_8c.md#group__alloc_1ga09e3f1c0494d23d9f93481ed4f228a4c): Dedicated address ranges for large pages.










**See also**: [PlatEnterProtectAddressRanges](col_win32_platform_8h.md#group__arch__win32_1ga0d52de05fb3a0897f54579ab12519159), [PlatLeaveProtectAddressRanges](col_win32_platform_8h.md#group__arch__win32_1gab6be4d622dd8c6bc4c283a1039962e5a)



**Type**: pthread_mutex_t

## Source

```cpp
/**
 * @file colUnixPlatform.h
 *
 * This header file defines Unix-specific primitives.
 *
 * @see colPlatform.h
 *
 * @beginprivate @cond PRIVATE
 */

#ifndef _COLIBRI_UNIXPLATFORM
#define _COLIBRI_UNIXPLATFORM

#include <pthread.h>


/*
===========================================================================*//*!
\internal \defgroup arch_unix System and Architecture (Unix-specific)
\ingroup arch

Unix-specific implementation of generic primitives.
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
    ((ThreadData *) pthread_getspecific(tsdKey))

/*
 * Remaining declarations.
 */

extern pthread_key_t tsdKey;

/* End of Thread-Local Storage *//*!\}*/


/***************************************************************************//*!
 * \name System Page Allocation
 ***************************************************************************\{*/

/**
 * Per-thread recursion count for PlatEnterProtectAddressRanges() calls.
 * As pthread mutexes are non-recursive, and recursive versions are not
 * portable, this provides a simple, portable way to allow nested calls.
 * Such calls notably occur during UpdateParents() when allocating new
 * pages to store parent descriptor cells, which eventually calls
 * SysPageAlloc().
 *
 * We store the recursion counter as an integer just after the #ThreadData
 * structure stored in thread-local storage (see allocation in
 * PlatEnter()). This counter prevents calls to pthread_mutex_lock when
 * nonzero and to pthread_mutex_unlock when > 1.
 *
 * @see PlatEnter
 * @see PlatEnterProtectAddressRanges
 * @see PlatLeaveProtectAddressRanges>
 */
#define UNIX_PROTECT_ADDRESS_RANGES_RECURSE(threadData) \
    (*(int *)(threadData+1))

/**
 * Enter protected section around address range management structures.
 *
 * @sideeffect
 *      Blocks until no thread owns the section.
 *
 * @see PlatLeaveProtectAddressRanges
 * @see UNIX_PROTECT_ADDRESS_RANGES_RECURSE
 */
#define PlatEnterProtectAddressRanges() \
    (UNIX_PROTECT_ADDRESS_RANGES_RECURSE(PlatGetThreadData())++ ? 0 : pthread_mutex_lock(&mutexRange))

/**
 * Leave protected section around address range management structures.
 *
 * @sideeffect
 *      May unblock any thread waiting for the section.
 *
 * @see PlatEnterProtectAddressRanges
 * @see UNIX_PROTECT_ADDRESS_RANGES_RECURSE
 */
#define PlatLeaveProtectAddressRanges() \
    (--UNIX_PROTECT_ADDRESS_RANGES_RECURSE(PlatGetThreadData()) ? 0 : pthread_mutex_unlock(&mutexRange))

/*
 * Remaining declarations.
 */

extern pthread_mutex_t mutexRange;

/* End of System Page Allocation *//*!\}*/

/* End of System and Architecture (Unix-specific) *//*!\}*/

#endif /* _COLIBRI_UNIXPLATFORM */
/*! @endcond @endprivate */
```

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)