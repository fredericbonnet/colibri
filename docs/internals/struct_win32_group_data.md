<a id="struct_win32_group_data"></a>
# Structure Win32GroupData

![][C++]
![][private]

**Definition**: `src/platform/win32/colWin32Platform.c` (line 82)

Platform-specific group data.

**See also**: [ThreadData](struct_thread_data.md#struct_thread_data), [GroupData](struct_group_data.md#struct_group_data), [Init](col_win32_platform_8c.md#group__arch__win32_1ga02edfe3159fd71b280177c0879667da5), [AllocGroupData](col_win32_platform_8c.md#group__arch__win32_1ga1d941e385e9fad8e3eccbdb0c865cc25), [FreeGroupData](col_win32_platform_8c.md#group__arch__win32_1ga747d41ae9054ca8d8cb09c1ecc374ae0)

## Members

* [csGc](struct_win32_group_data.md#struct_win32_group_data_1af28904a028484c08cacab8c50e4705f8)
* [csRoots](struct_win32_group_data.md#struct_win32_group_data_1a6944a9302c4b18dfaa54a0a2d95e5f4d)
* [data](struct_win32_group_data.md#struct_win32_group_data_1a11d7fb8755fb30bd47bbefd8098ec489)
* [eventGcDone](struct_win32_group_data.md#struct_win32_group_data_1a98a56c55ed514fe43a741dcc71d06d72)
* [eventGcScheduled](struct_win32_group_data.md#struct_win32_group_data_1aad968230554371a9d6be7012ef4ef5e8)
* [nbActive](struct_win32_group_data.md#struct_win32_group_data_1a6d5f37daf4e64c6cf6586066b90fb987)
* [next](struct_win32_group_data.md#struct_win32_group_data_1a11ee4dca830fbf843b3f7acabf3e54be)
* [scheduled](struct_win32_group_data.md#struct_win32_group_data_1af4560732f756493b5fd37bff90926c76)
* [terminated](struct_win32_group_data.md#struct_win32_group_data_1a04c084e20364f5cf14d9e0bebfeb11e3)
* [threadGc](struct_win32_group_data.md#struct_win32_group_data_1afd8eb1f08058dc558a6034d3357850dd)

## Public attributes

<a id="struct_win32_group_data_1a11d7fb8755fb30bd47bbefd8098ec489"></a>
### Variable data

![][public]

**Definition**: `src/platform/win32/colWin32Platform.c` (line 83)

```cpp
GroupData Win32GroupData::data
```

Generic [GroupData](struct_group_data.md#struct_group_data) structure.





**Type**: [GroupData](struct_group_data.md#struct_group_data)

**Referenced by**:

* [AllocGroupData](col_win32_platform_8c.md#group__arch__win32_1ga1d941e385e9fad8e3eccbdb0c865cc25)
* [FreeGroupData](col_win32_platform_8c.md#group__arch__win32_1ga747d41ae9054ca8d8cb09c1ecc374ae0)
* [PlatEnter](col_win32_platform_8c.md#group__arch__win32_1gaa42fe97b4b462c9483110a715c1eb1d1)
* [PlatLeave](col_win32_platform_8c.md#group__arch__win32_1ga445bf6b3cd4afc09367a6d9fce001a2e)
* [PlatSyncPauseGC](col_win32_platform_8c.md#group__arch__win32_1gaa19923dd2972ca8919b05b935cc5592f)
* [PlatTrySyncPauseGC](col_win32_platform_8c.md#group__arch__win32_1ga52acd19d06324536559d9f142b832025)
* [PlatSyncResumeGC](col_win32_platform_8c.md#group__arch__win32_1ga234974987c437283a255cad2e6847585)
* [PlatEnterProtectRoots](col_win32_platform_8c.md#group__arch__win32_1gaeb2b83a6cb4f1d38f1993fe221556bfa)
* [PlatLeaveProtectRoots](col_win32_platform_8c.md#group__arch__win32_1gac35163168872326ce29c15c6df13d76a)

<a id="struct_win32_group_data_1a11ee4dca830fbf843b3f7acabf3e54be"></a>
### Variable next

![][public]

**Definition**: `src/platform/win32/colWin32Platform.c` (line 85)

```cpp
struct Win32GroupData* Win32GroupData::next
```

Next active group in list.





**Type**: struct [Win32GroupData](struct_win32_group_data.md#struct_win32_group_data) *

**Referenced by**:

* [PlatEnter](col_win32_platform_8c.md#group__arch__win32_1gaa42fe97b4b462c9483110a715c1eb1d1)

<a id="struct_win32_group_data_1a6944a9302c4b18dfaa54a0a2d95e5f4d"></a>
### Variable csRoots

![][public]

**Definition**: `src/platform/win32/colWin32Platform.c` (line 86)

```cpp
CRITICAL_SECTION Win32GroupData::csRoots
```

Critical section protecting root management.





**Type**: CRITICAL_SECTION

**Referenced by**:

* [FreeGroupData](col_win32_platform_8c.md#group__arch__win32_1ga747d41ae9054ca8d8cb09c1ecc374ae0)
* [PlatEnterProtectRoots](col_win32_platform_8c.md#group__arch__win32_1gaeb2b83a6cb4f1d38f1993fe221556bfa)
* [PlatLeaveProtectRoots](col_win32_platform_8c.md#group__arch__win32_1gac35163168872326ce29c15c6df13d76a)

<a id="struct_win32_group_data_1af28904a028484c08cacab8c50e4705f8"></a>
### Variable csGc

![][public]

**Definition**: `src/platform/win32/colWin32Platform.c` (line 89)

```cpp
CRITICAL_SECTION Win32GroupData::csGc
```

Critical section protecting GC from worker threads.





**Type**: CRITICAL_SECTION

**Referenced by**:

* [FreeGroupData](col_win32_platform_8c.md#group__arch__win32_1ga747d41ae9054ca8d8cb09c1ecc374ae0)
* [GcThreadProc](col_win32_platform_8c.md#group__arch__win32_1ga5195d9f3490f89e1e761ab0bd6e369b0)
* [PlatSyncPauseGC](col_win32_platform_8c.md#group__arch__win32_1gaa19923dd2972ca8919b05b935cc5592f)
* [PlatTrySyncPauseGC](col_win32_platform_8c.md#group__arch__win32_1ga52acd19d06324536559d9f142b832025)
* [PlatSyncResumeGC](col_win32_platform_8c.md#group__arch__win32_1ga234974987c437283a255cad2e6847585)

<a id="struct_win32_group_data_1aad968230554371a9d6be7012ef4ef5e8"></a>
### Variable eventGcScheduled

![][public]

**Definition**: `src/platform/win32/colWin32Platform.c` (line 91)

```cpp
HANDLE Win32GroupData::eventGcScheduled
```

Triggers GC thread.





**Type**: HANDLE

**Referenced by**:

* [FreeGroupData](col_win32_platform_8c.md#group__arch__win32_1ga747d41ae9054ca8d8cb09c1ecc374ae0)
* [GcThreadProc](col_win32_platform_8c.md#group__arch__win32_1ga5195d9f3490f89e1e761ab0bd6e369b0)
* [PlatSyncResumeGC](col_win32_platform_8c.md#group__arch__win32_1ga234974987c437283a255cad2e6847585)

<a id="struct_win32_group_data_1a98a56c55ed514fe43a741dcc71d06d72"></a>
### Variable eventGcDone

![][public]

**Definition**: `src/platform/win32/colWin32Platform.c` (line 92)

```cpp
HANDLE Win32GroupData::eventGcDone
```

Barrier for worker threads.





**Type**: HANDLE

**Referenced by**:

* [FreeGroupData](col_win32_platform_8c.md#group__arch__win32_1ga747d41ae9054ca8d8cb09c1ecc374ae0)
* [GcThreadProc](col_win32_platform_8c.md#group__arch__win32_1ga5195d9f3490f89e1e761ab0bd6e369b0)
* [PlatSyncPauseGC](col_win32_platform_8c.md#group__arch__win32_1gaa19923dd2972ca8919b05b935cc5592f)
* [PlatTrySyncPauseGC](col_win32_platform_8c.md#group__arch__win32_1ga52acd19d06324536559d9f142b832025)
* [PlatSyncResumeGC](col_win32_platform_8c.md#group__arch__win32_1ga234974987c437283a255cad2e6847585)

<a id="struct_win32_group_data_1af4560732f756493b5fd37bff90926c76"></a>
### Variable scheduled

![][public]

**Definition**: `src/platform/win32/colWin32Platform.c` (line 93)

```cpp
int Win32GroupData::scheduled
```

Flag for when a GC is scheduled.





**Type**: int

**Referenced by**:

* [GcThreadProc](col_win32_platform_8c.md#group__arch__win32_1ga5195d9f3490f89e1e761ab0bd6e369b0)
* [PlatSyncResumeGC](col_win32_platform_8c.md#group__arch__win32_1ga234974987c437283a255cad2e6847585)

<a id="struct_win32_group_data_1a04c084e20364f5cf14d9e0bebfeb11e3"></a>
### Variable terminated

![][public]

**Definition**: `src/platform/win32/colWin32Platform.c` (line 94)

```cpp
int Win32GroupData::terminated
```

Flag for thread group destruction.





**Type**: int

**Referenced by**:

* [FreeGroupData](col_win32_platform_8c.md#group__arch__win32_1ga747d41ae9054ca8d8cb09c1ecc374ae0)
* [GcThreadProc](col_win32_platform_8c.md#group__arch__win32_1ga5195d9f3490f89e1e761ab0bd6e369b0)

<a id="struct_win32_group_data_1a6d5f37daf4e64c6cf6586066b90fb987"></a>
### Variable nbActive

![][public]

**Definition**: `src/platform/win32/colWin32Platform.c` (line 95)

```cpp
int Win32GroupData::nbActive
```

Active worker thread counter.





**Type**: int

**Referenced by**:

* [PlatSyncPauseGC](col_win32_platform_8c.md#group__arch__win32_1gaa19923dd2972ca8919b05b935cc5592f)
* [PlatTrySyncPauseGC](col_win32_platform_8c.md#group__arch__win32_1ga52acd19d06324536559d9f142b832025)
* [PlatSyncResumeGC](col_win32_platform_8c.md#group__arch__win32_1ga234974987c437283a255cad2e6847585)

<a id="struct_win32_group_data_1afd8eb1f08058dc558a6034d3357850dd"></a>
### Variable threadGc

![][public]

**Definition**: `src/platform/win32/colWin32Platform.c` (line 96)

```cpp
HANDLE Win32GroupData::threadGc
```

GC thread.





**Type**: HANDLE

**Referenced by**:

* [FreeGroupData](col_win32_platform_8c.md#group__arch__win32_1ga747d41ae9054ca8d8cb09c1ecc374ae0)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)