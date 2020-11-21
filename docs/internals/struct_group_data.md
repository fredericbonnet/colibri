<a id="struct_group_data"></a>
# Structure GroupData

![][C++]
![][private]

**Definition**: `src/colInternal.h` (line 535)

Group-local data.

**See also**: [Threading Model Constants](#group__init_1threading_models), [MemoryPool](struct_memory_pool.md#struct_memory_pool), [ThreadData](struct_thread_data.md#struct_thread_data), [PlatEnter](col_win32_platform_8c.md#group__arch__win32_1gaa42fe97b4b462c9483110a715c1eb1d1), [PlatLeave](col_win32_platform_8c.md#group__arch__win32_1ga445bf6b3cd4afc09367a6d9fce001a2e), [AllocGroupData](col_win32_platform_8c.md#group__arch__win32_1ga1d941e385e9fad8e3eccbdb0c865cc25), [FreeGroupData](col_win32_platform_8c.md#group__arch__win32_1ga747d41ae9054ca8d8cb09c1ecc374ae0)

## Members

* [compactGeneration](struct_group_data.md#struct_group_data_1a19fe0bfacbbb85c6d4545b83d10e4cb7)
* [first](struct_group_data.md#struct_group_data_1af58eb628d2c2bb11f4bfa161c7e93318)
* [maxCollectedGeneration](struct_group_data.md#struct_group_data_1a23653b17e9904b4b5441c2e95b2d7a99)
* [model](struct_group_data.md#struct_group_data_1a02daeab5df802e492019f1f17d3efde2)
* [parents](struct_group_data.md#struct_group_data_1aa17cb04fecebc17f5f9ff130388887ef)
* [pools](struct_group_data.md#struct_group_data_1a1db77277f7db550bc190771614fe2dbc)
* [rootPool](struct_group_data.md#struct_group_data_1a81941409b9917b41bb20d3572b2b4ec7)
* [roots](struct_group_data.md#struct_group_data_1af753ea77322dc01776c856b584b219e7)

## Public attributes

<a id="struct_group_data_1a02daeab5df802e492019f1f17d3efde2"></a>
### Variable model

![][public]

**Definition**: `src/colInternal.h` (line 536)

```cpp
unsigned int GroupData::model
```

Threading model.





**Type**: unsigned int

**Referenced by**:

* [AllocGroupData](col_unix_platform_8c.md#group__arch__unix_1gada67280be3d6df8cf250ba65d46d176e)
* [FreeGroupData](col_unix_platform_8c.md#group__arch__unix_1ga721b5c30cbc3d79ced480bf39efcf5aa)
* [PlatEnter](col_unix_platform_8c.md#group__arch__unix_1gaa42fe97b4b462c9483110a715c1eb1d1)
* [PlatLeave](col_unix_platform_8c.md#group__arch__unix_1ga445bf6b3cd4afc09367a6d9fce001a2e)
* [PlatSyncPauseGC](col_win32_platform_8c.md#group__arch__win32_1gaa19923dd2972ca8919b05b935cc5592f)
* [PlatTrySyncPauseGC](col_win32_platform_8c.md#group__arch__win32_1ga52acd19d06324536559d9f142b832025)
* [PlatSyncResumeGC](col_win32_platform_8c.md#group__arch__win32_1ga234974987c437283a255cad2e6847585)
* [PlatEnterProtectRoots](col_win32_platform_8c.md#group__arch__win32_1gaeb2b83a6cb4f1d38f1993fe221556bfa)
* [PlatLeaveProtectRoots](col_win32_platform_8c.md#group__arch__win32_1gac35163168872326ce29c15c6df13d76a)

<a id="struct_group_data_1a81941409b9917b41bb20d3572b2b4ec7"></a>
### Variable rootPool

![][public]

**Definition**: `src/colInternal.h` (line 537)

```cpp
MemoryPool GroupData::rootPool
```

Memory pool used to store root and parent page descriptor cells.





**Type**: [MemoryPool](struct_memory_pool.md#struct_memory_pool)

**Referenced by**:

* [UpdateParents](col_alloc_8c.md#group__gc__parents_1gaa3d85dc993fb1b9831f82c25b8c07d3c)
* [GcInitGroup](col_gc_8c.md#group__gc_1ga4183a56fec06c1ff8176f90c31248d68)
* [GcCleanupGroup](col_gc_8c.md#group__gc_1ga8e6fd33d04cb870d9d4b64eb34ab2ebc)
* [PerformGC](col_gc_8c.md#group__gc_1ga5688ae9d7f658650ca8dfa66f4102f62)
* [Col\_WordPreserve](col_word_8h.md#group__words_1gab55f452e6b0856f7bd7b34e04fae2aa2)

<a id="struct_group_data_1af753ea77322dc01776c856b584b219e7"></a>
### Variable roots

![][public]

**Definition**: `src/colInternal.h` (line 539)

```cpp
Cell* GroupData::roots
```

Root descriptors are stored in a trie indexed by the root cell address.





**Type**: [Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) *

**Referenced by**:

* [MarkReachableCellsFromRoots](col_gc_8c.md#group__gc_1ga246db5ae65938d3efa2b04b2b5cb2021)
* [Col\_WordPreserve](col_word_8h.md#group__words_1gab55f452e6b0856f7bd7b34e04fae2aa2)
* [Col\_WordRelease](col_word_8h.md#group__words_1gad93112f81ce6511d6d0ece0db4d38598)

<a id="struct_group_data_1aa17cb04fecebc17f5f9ff130388887ef"></a>
### Variable parents

![][public]

**Definition**: `src/colInternal.h` (line 541)

```cpp
Cell* GroupData::parents
```

Parent descriptors are stored in a linked list.





**Type**: [Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) *

**Referenced by**:

* [UpdateParents](col_alloc_8c.md#group__gc__parents_1gaa3d85dc993fb1b9831f82c25b8c07d3c)
* [MarkReachableCellsFromParents](col_gc_8c.md#group__gc_1ga49eb8981c888c90530906952e2869000)
* [PurgeParents](col_gc_8c.md#group__gc_1gae851dc24a0065a16f7149b2f10147b52)

<a id="struct_group_data_1a1db77277f7db550bc190771614fe2dbc"></a>
### Variable pools

![][public]

**Definition**: `src/colInternal.h` (line 544)

```cpp
MemoryPool GroupData::pools[GC_MAX_GENERATIONS-2][GC_MAX_GENERATIONS-2]
```

Memory pools used to store words for generations older than eden (1 < generation < [GC\_MAX\_GENERATIONS](col_conf_8h.md#group__gc_1gab203ff01512f39769443cf23c24c1234)).





**Type**: [MemoryPool](struct_memory_pool.md#struct_memory_pool)

**Referenced by**:

* [GcInitGroup](col_gc_8c.md#group__gc_1ga4183a56fec06c1ff8176f90c31248d68)
* [GcCleanupGroup](col_gc_8c.md#group__gc_1ga8e6fd33d04cb870d9d4b64eb34ab2ebc)
* [Col\_ResumeGC](colibri_8h.md#group__gc_1gaf7d4f0dd1996dde366af3f29e9bcc517)
* [PerformGC](col_gc_8c.md#group__gc_1ga5688ae9d7f658650ca8dfa66f4102f62)
* [MarkWord](col_gc_8c.md#group__gc_1gaf54093bb37e6a4aaaf718fb1a791d56c)
* [SweepUnreachableCells](col_gc_8c.md#group__gc_1ga911d254f51c4e0b4475330147fbbc545)
* [PromotePages](col_gc_8c.md#group__gc_1ga09e7926c400756068d91140da241934f)

<a id="struct_group_data_1a23653b17e9904b4b5441c2e95b2d7a99"></a>
### Variable maxCollectedGeneration

![][public]

**Definition**: `src/colInternal.h` (line 548)

```cpp
unsigned int GroupData::maxCollectedGeneration
```

Oldest collected generation during current GC.





**Type**: unsigned int

**Referenced by**:

* [PerformGC](col_gc_8c.md#group__gc_1ga5688ae9d7f658650ca8dfa66f4102f62)
* [MarkReachableCellsFromRoots](col_gc_8c.md#group__gc_1ga246db5ae65938d3efa2b04b2b5cb2021)
* [MarkReachableCellsFromParents](col_gc_8c.md#group__gc_1ga49eb8981c888c90530906952e2869000)
* [PurgeParents](col_gc_8c.md#group__gc_1gae851dc24a0065a16f7149b2f10147b52)
* [SweepUnreachableCells](col_gc_8c.md#group__gc_1ga911d254f51c4e0b4475330147fbbc545)
* [PromotePages](col_gc_8c.md#group__gc_1ga09e7926c400756068d91140da241934f)

<a id="struct_group_data_1a19fe0bfacbbb85c6d4545b83d10e4cb7"></a>
### Variable compactGeneration

![][public]

**Definition**: `src/colInternal.h` (line 551)

```cpp
unsigned int GroupData::compactGeneration
```

Generation on which to perform compaction during promotion (see [PROMOTE\_COMPACT](col_conf_8h.md#group__gc_1gabfe8006c7310448a1d33027d30a59fe4)).





**Type**: unsigned int

**Referenced by**:

* [PerformGC](col_gc_8c.md#group__gc_1ga5688ae9d7f658650ca8dfa66f4102f62)
* [MarkWord](col_gc_8c.md#group__gc_1gaf54093bb37e6a4aaaf718fb1a791d56c)
* [SweepUnreachableCells](col_gc_8c.md#group__gc_1ga911d254f51c4e0b4475330147fbbc545)

<a id="struct_group_data_1af58eb628d2c2bb11f4bfa161c7e93318"></a>
### Variable first

![][public]

**Definition**: `src/colInternal.h` (line 555)

```cpp
struct ThreadData* GroupData::first
```

Group member threads form a circular list.





**Type**: struct [ThreadData](struct_thread_data.md#struct_thread_data) *

**Referenced by**:

* [PerformGC](col_gc_8c.md#group__gc_1ga5688ae9d7f658650ca8dfa66f4102f62)
* [PlatEnter](col_unix_platform_8c.md#group__arch__unix_1gaa42fe97b4b462c9483110a715c1eb1d1)
* [PlatLeave](col_unix_platform_8c.md#group__arch__unix_1ga445bf6b3cd4afc09367a6d9fce001a2e)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)