<a id="struct_unix_group_data"></a>
# Structure UnixGroupData

![][C++]
![][private]

**Definition**: `src/platform/unix/colUnixPlatform.c` (line 86)

Platform-specific group data.

**See also**: [ThreadData](struct_thread_data.md#struct_thread_data), [GroupData](struct_group_data.md#struct_group_data), [Init](col_win32_platform_8c.md#group__arch__win32_1ga02edfe3159fd71b280177c0879667da5), [AllocGroupData](col_win32_platform_8c.md#group__arch__win32_1ga1d941e385e9fad8e3eccbdb0c865cc25), [FreeGroupData](col_win32_platform_8c.md#group__arch__win32_1ga747d41ae9054ca8d8cb09c1ecc374ae0)

## Members

* [condGcDone](struct_unix_group_data.md#struct_unix_group_data_1a8e596ff91f9b7bccc2e03ba8a3904646)
* [condGcScheduled](struct_unix_group_data.md#struct_unix_group_data_1a965c8fab20818cadc1ed6149560be9bc)
* [data](struct_unix_group_data.md#struct_unix_group_data_1ad7c37ae9b2b049edee188d6123ed1d02)
* [mutexGc](struct_unix_group_data.md#struct_unix_group_data_1a3755b74c0ab89d8c45ece97e0f1391bc)
* [mutexRoots](struct_unix_group_data.md#struct_unix_group_data_1a38ea1e8080b000b915c65824cbed5692)
* [nbActive](struct_unix_group_data.md#struct_unix_group_data_1aea3a82f31d27f81fd27582e8afebda97)
* [next](struct_unix_group_data.md#struct_unix_group_data_1ac4503fc576f6bc99549cf25866b45304)
* [scheduled](struct_unix_group_data.md#struct_unix_group_data_1a8b8e85e9f5c851a84d34a399373bf12a)
* [terminated](struct_unix_group_data.md#struct_unix_group_data_1a6cbd01f65b68f69a64417339bffd4536)
* [threadGc](struct_unix_group_data.md#struct_unix_group_data_1aa293192519610d88e0c7bbb4678f31ba)

## Public attributes

<a id="struct_unix_group_data_1ad7c37ae9b2b049edee188d6123ed1d02"></a>
### Variable data

![][public]

**Definition**: `src/platform/unix/colUnixPlatform.c` (line 87)

```cpp
GroupData UnixGroupData::data
```

Generic [GroupData](struct_group_data.md#struct_group_data) structure.





**Type**: [GroupData](struct_group_data.md#struct_group_data)

**Referenced by**:

* [AllocGroupData](col_unix_platform_8c.md#group__arch__unix_1gada67280be3d6df8cf250ba65d46d176e)
* [FreeGroupData](col_unix_platform_8c.md#group__arch__unix_1ga721b5c30cbc3d79ced480bf39efcf5aa)
* [PlatEnter](col_unix_platform_8c.md#group__arch__unix_1gaa42fe97b4b462c9483110a715c1eb1d1)
* [PlatLeave](col_unix_platform_8c.md#group__arch__unix_1ga445bf6b3cd4afc09367a6d9fce001a2e)
* [PlatSyncPauseGC](col_unix_platform_8c.md#group__arch__unix_1gaa19923dd2972ca8919b05b935cc5592f)
* [PlatTrySyncPauseGC](col_unix_platform_8c.md#group__arch__unix_1ga52acd19d06324536559d9f142b832025)
* [PlatSyncResumeGC](col_unix_platform_8c.md#group__arch__unix_1ga234974987c437283a255cad2e6847585)
* [PlatEnterProtectRoots](col_unix_platform_8c.md#group__arch__unix_1gaeb2b83a6cb4f1d38f1993fe221556bfa)
* [PlatLeaveProtectRoots](col_unix_platform_8c.md#group__arch__unix_1gac35163168872326ce29c15c6df13d76a)

<a id="struct_unix_group_data_1ac4503fc576f6bc99549cf25866b45304"></a>
### Variable next

![][public]

**Definition**: `src/platform/unix/colUnixPlatform.c` (line 89)

```cpp
struct UnixGroupData* UnixGroupData::next
```

Next active group in list.





**Type**: struct [UnixGroupData](struct_unix_group_data.md#struct_unix_group_data) *

**Referenced by**:

* [PlatEnter](col_unix_platform_8c.md#group__arch__unix_1gaa42fe97b4b462c9483110a715c1eb1d1)

<a id="struct_unix_group_data_1a38ea1e8080b000b915c65824cbed5692"></a>
### Variable mutexRoots

![][public]

**Definition**: `src/platform/unix/colUnixPlatform.c` (line 90)

```cpp
pthread_mutex_t UnixGroupData::mutexRoots
```

Mutex protecting root management.





**Type**: pthread_mutex_t

**Referenced by**:

* [FreeGroupData](col_unix_platform_8c.md#group__arch__unix_1ga721b5c30cbc3d79ced480bf39efcf5aa)
* [PlatEnterProtectRoots](col_unix_platform_8c.md#group__arch__unix_1gaeb2b83a6cb4f1d38f1993fe221556bfa)
* [PlatLeaveProtectRoots](col_unix_platform_8c.md#group__arch__unix_1gac35163168872326ce29c15c6df13d76a)

<a id="struct_unix_group_data_1a3755b74c0ab89d8c45ece97e0f1391bc"></a>
### Variable mutexGc

![][public]

**Definition**: `src/platform/unix/colUnixPlatform.c` (line 92)

```cpp
pthread_mutex_t UnixGroupData::mutexGc
```

Mutex protecting GC from worker threads.





**Type**: pthread_mutex_t

**Referenced by**:

* [FreeGroupData](col_unix_platform_8c.md#group__arch__unix_1ga721b5c30cbc3d79ced480bf39efcf5aa)
* [GcThreadProc](col_unix_platform_8c.md#group__arch__unix_1ga74665cfa1a2c8827d87315985197ee41)
* [PlatSyncPauseGC](col_unix_platform_8c.md#group__arch__unix_1gaa19923dd2972ca8919b05b935cc5592f)
* [PlatTrySyncPauseGC](col_unix_platform_8c.md#group__arch__unix_1ga52acd19d06324536559d9f142b832025)
* [PlatSyncResumeGC](col_unix_platform_8c.md#group__arch__unix_1ga234974987c437283a255cad2e6847585)

<a id="struct_unix_group_data_1a965c8fab20818cadc1ed6149560be9bc"></a>
### Variable condGcScheduled

![][public]

**Definition**: `src/platform/unix/colUnixPlatform.c` (line 94)

```cpp
pthread_cond_t UnixGroupData::condGcScheduled
```

Triggers GC thread.





**Type**: pthread_cond_t

**Referenced by**:

* [FreeGroupData](col_unix_platform_8c.md#group__arch__unix_1ga721b5c30cbc3d79ced480bf39efcf5aa)
* [GcThreadProc](col_unix_platform_8c.md#group__arch__unix_1ga74665cfa1a2c8827d87315985197ee41)
* [PlatSyncResumeGC](col_unix_platform_8c.md#group__arch__unix_1ga234974987c437283a255cad2e6847585)

<a id="struct_unix_group_data_1a8e596ff91f9b7bccc2e03ba8a3904646"></a>
### Variable condGcDone

![][public]

**Definition**: `src/platform/unix/colUnixPlatform.c` (line 95)

```cpp
pthread_cond_t UnixGroupData::condGcDone
```

Barrier for worker threads.





**Type**: pthread_cond_t

**Referenced by**:

* [FreeGroupData](col_unix_platform_8c.md#group__arch__unix_1ga721b5c30cbc3d79ced480bf39efcf5aa)
* [PlatSyncPauseGC](col_unix_platform_8c.md#group__arch__unix_1gaa19923dd2972ca8919b05b935cc5592f)
* [PlatTrySyncPauseGC](col_unix_platform_8c.md#group__arch__unix_1ga52acd19d06324536559d9f142b832025)

<a id="struct_unix_group_data_1a8b8e85e9f5c851a84d34a399373bf12a"></a>
### Variable scheduled

![][public]

**Definition**: `src/platform/unix/colUnixPlatform.c` (line 96)

```cpp
int UnixGroupData::scheduled
```

Flag for when a GC is scheduled.





**Type**: int

**Referenced by**:

* [GcThreadProc](col_unix_platform_8c.md#group__arch__unix_1ga74665cfa1a2c8827d87315985197ee41)
* [PlatSyncPauseGC](col_unix_platform_8c.md#group__arch__unix_1gaa19923dd2972ca8919b05b935cc5592f)
* [PlatTrySyncPauseGC](col_unix_platform_8c.md#group__arch__unix_1ga52acd19d06324536559d9f142b832025)
* [PlatSyncResumeGC](col_unix_platform_8c.md#group__arch__unix_1ga234974987c437283a255cad2e6847585)

<a id="struct_unix_group_data_1a6cbd01f65b68f69a64417339bffd4536"></a>
### Variable terminated

![][public]

**Definition**: `src/platform/unix/colUnixPlatform.c` (line 97)

```cpp
int UnixGroupData::terminated
```

Flag for thread group destruction.





**Type**: int

**Referenced by**:

* [FreeGroupData](col_unix_platform_8c.md#group__arch__unix_1ga721b5c30cbc3d79ced480bf39efcf5aa)
* [GcThreadProc](col_unix_platform_8c.md#group__arch__unix_1ga74665cfa1a2c8827d87315985197ee41)

<a id="struct_unix_group_data_1aea3a82f31d27f81fd27582e8afebda97"></a>
### Variable nbActive

![][public]

**Definition**: `src/platform/unix/colUnixPlatform.c` (line 98)

```cpp
int UnixGroupData::nbActive
```

Active worker thread counter.





**Type**: int

**Referenced by**:

* [PlatSyncPauseGC](col_unix_platform_8c.md#group__arch__unix_1gaa19923dd2972ca8919b05b935cc5592f)
* [PlatTrySyncPauseGC](col_unix_platform_8c.md#group__arch__unix_1ga52acd19d06324536559d9f142b832025)
* [PlatSyncResumeGC](col_unix_platform_8c.md#group__arch__unix_1ga234974987c437283a255cad2e6847585)

<a id="struct_unix_group_data_1aa293192519610d88e0c7bbb4678f31ba"></a>
### Variable threadGc

![][public]

**Definition**: `src/platform/unix/colUnixPlatform.c` (line 99)

```cpp
pthread_t UnixGroupData::threadGc
```

GC thread.





**Type**: pthread_t

**Referenced by**:

* [FreeGroupData](col_unix_platform_8c.md#group__arch__unix_1ga721b5c30cbc3d79ced480bf39efcf5aa)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)