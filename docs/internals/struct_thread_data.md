<a id="struct_thread_data"></a>
# Structure ThreadData

![][C++]
![][private]

**Definition**: `src/colInternal.h` (line 568)

Thread-local data.

**See also**: [Threading Model Constants](#group__init_1threading_models), [MemoryPool](struct_memory_pool.md#struct_memory_pool), [GroupData](struct_group_data.md#struct_group_data), [PlatEnter](col_win32_platform_8c.md#group__arch__win32_1gaa42fe97b4b462c9483110a715c1eb1d1), [PlatLeave](col_win32_platform_8c.md#group__arch__win32_1ga445bf6b3cd4afc09367a6d9fce001a2e)

## Members

* [eden](struct_thread_data.md#struct_thread_data_1ac4e14d59d12e49f808e8631fea374297)
* [errorProc](struct_thread_data.md#struct_thread_data_1ad815732e363271be15dce0015abfc8ae)
* [groupData](struct_thread_data.md#struct_thread_data_1aefbdf49c641476274db5326c60853022)
* [nestCount](struct_thread_data.md#struct_thread_data_1a3a471ca0bcac3f93112ed6d3c2fad667)
* [next](struct_thread_data.md#struct_thread_data_1aee56668363c15f17454a3bab5f63b4a4)
* [pauseGC](struct_thread_data.md#struct_thread_data_1a09349f3ddfda58e5a43c3c6962e88da7)

## Public attributes

<a id="struct_thread_data_1aee56668363c15f17454a3bab5f63b4a4"></a>
### Variable next

![][public]

**Definition**: `src/colInternal.h` (line 569)

```cpp
struct ThreadData* ThreadData::next
```

Next in thread group member circular list.





**Type**: struct [ThreadData](struct_thread_data.md#struct_thread_data) *

**Referenced by**:

* [PerformGC](col_gc_8c.md#group__gc_1ga5688ae9d7f658650ca8dfa66f4102f62)
* [PlatEnter](col_unix_platform_8c.md#group__arch__unix_1gaa42fe97b4b462c9483110a715c1eb1d1)
* [PlatLeave](col_unix_platform_8c.md#group__arch__unix_1ga445bf6b3cd4afc09367a6d9fce001a2e)

<a id="struct_thread_data_1aefbdf49c641476274db5326c60853022"></a>
### Variable groupData

![][public]

**Definition**: `src/colInternal.h` (line 571)

```cpp
GroupData* ThreadData::groupData
```

Group-local data.





**Type**: [GroupData](struct_group_data.md#struct_group_data) *

**Referenced by**:

* [PoolAllocPages](col_alloc_8c.md#group__alloc_1ga45316be86459e993b67bf27efbb8bf5f)
* [PoolFreeEmptyPages](col_alloc_8c.md#group__alloc_1ga0c80585c5110f75f1bf723c9b93df073)
* [Col\_PauseGC](colibri_8h.md#group__gc_1gae703ee3215a4724ebed8e5a2824e7a7b)
* [Col\_TryPauseGC](colibri_8h.md#group__gc_1ga54eefaa11ad5a79b8665fef5cc24c26f)
* [Col\_ResumeGC](colibri_8h.md#group__gc_1gaf7d4f0dd1996dde366af3f29e9bcc517)
* [Col\_WordPreserve](col_word_8h.md#group__words_1gab55f452e6b0856f7bd7b34e04fae2aa2)
* [Col\_WordRelease](col_word_8h.md#group__words_1gad93112f81ce6511d6d0ece0db4d38598)

<a id="struct_thread_data_1a3a471ca0bcac3f93112ed6d3c2fad667"></a>
### Variable nestCount

![][public]

**Definition**: `src/colInternal.h` (line 572)

```cpp
size_t ThreadData::nestCount
```

Number of nested calls to [Col\_Init()](colibri_8h.md#group__init_1ga715049d7eb10ff0eeac38b457ef4fce1).

Clear structures once it drops to zero.



**Type**: size_t

<a id="struct_thread_data_1ad815732e363271be15dce0015abfc8ae"></a>
### Variable errorProc

![][public]

**Definition**: `src/colInternal.h` (line 574)

```cpp
Col_ErrorProc* ThreadData::errorProc
```

Error procs are thread-local.





**Type**: [Col\_ErrorProc](colibri_8h.md#group__error_1gac5151fd715c49a363324d700de68e176) *

**Referenced by**:

* [Col\_Error](colibri_8h.md#group__error_1ga9a9a9c96b23c489cf8a19a6248fc77b8)
* [Col\_SetErrorProc](colibri_8h.md#group__error_1ga024ea9d85177427fc412c3e469cc1169)

<a id="struct_thread_data_1a09349f3ddfda58e5a43c3c6962e88da7"></a>
### Variable pauseGC

![][public]

**Definition**: `src/colInternal.h` (line 575)

```cpp
size_t ThreadData::pauseGC
```

GC-protected section counter, i.e. nb of nested pause calls.

When positive, we are in a GC-protected section.



**Type**: size_t

**Referenced by**:

* [Col\_PauseGC](colibri_8h.md#group__gc_1gae703ee3215a4724ebed8e5a2824e7a7b)
* [Col\_TryPauseGC](colibri_8h.md#group__gc_1ga54eefaa11ad5a79b8665fef5cc24c26f)
* [Col\_ResumeGC](colibri_8h.md#group__gc_1gaf7d4f0dd1996dde366af3f29e9bcc517)

<a id="struct_thread_data_1ac4e14d59d12e49f808e8631fea374297"></a>
### Variable eden

![][public]

**Definition**: `src/colInternal.h` (line 578)

```cpp
MemoryPool ThreadData::eden
```

Eden, i.e. memory pool for generation 1.

New cells are always created in thread-local eden pools for minimum contention.



**Type**: [MemoryPool](struct_memory_pool.md#struct_memory_pool)

**Referenced by**:

* [GcInitThread](col_gc_8c.md#group__gc_1gad46e288d322683c434e6efa30e3edc22)
* [GcCleanupThread](col_gc_8c.md#group__gc_1gacbea8d94b2fdf7366fbe4ad8b8298a91)
* [Col\_ResumeGC](colibri_8h.md#group__gc_1gaf7d4f0dd1996dde366af3f29e9bcc517)
* [PerformGC](col_gc_8c.md#group__gc_1ga5688ae9d7f658650ca8dfa66f4102f62)
* [RememberSweepable](col_gc_8c.md#group__gc_1ga23c84fc6b7da85d87751fa5788e3f002)
* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)