<a id="struct_memory_pool"></a>
# Structure MemoryPool

![][C++]
![][private]

**Definition**: `src/colInternal.h` (line 451)

Memory pools are a set of pages storing the cells of a given generation.

**See also**: [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed), [Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f)

## Members

* [gc](struct_memory_pool.md#struct_memory_pool_1a2273b16be4a0e84a1d54df9caa74ddd6)
* [generation](struct_memory_pool.md#struct_memory_pool_1a0d3c5d41525ad009c79ca65f07799ce6)
* [lastFreeCell](struct_memory_pool.md#struct_memory_pool_1aea8d38c1c90f937e7819b2ab0222ace1)
* [lastPage](struct_memory_pool.md#struct_memory_pool_1a933a8d62099891367279a8486ae81d3e)
* [nbAlloc](struct_memory_pool.md#struct_memory_pool_1a78090a0d29a65f47f1237fd99ee33316)
* [nbPages](struct_memory_pool.md#struct_memory_pool_1ab4fba4fd762f5a04c124cfafd6577744)
* [nbSetCells](struct_memory_pool.md#struct_memory_pool_1a69b3504fcae96ab4a89e3a0c077ec2ea)
* [pages](struct_memory_pool.md#struct_memory_pool_1afca03b46fe1276b29fdd398ff07dfc41)
* [sweepables](struct_memory_pool.md#struct_memory_pool_1ab449566340991fe03f4b917516116958)

## Public attributes

<a id="struct_memory_pool_1a0d3c5d41525ad009c79ca65f07799ce6"></a>
### Variable generation

![][public]

**Definition**: `src/colInternal.h` (line 452)

```cpp
unsigned int MemoryPool::generation
```

Generation level; 0 = younger, 1 = eden, 2+ = older.





**Type**: unsigned int

**Referenced by**:

* [PoolInit](col_alloc_8c.md#group__alloc_1gab08d29a35a3b57dff2e79468a569ad81)
* [PoolAllocPages](col_alloc_8c.md#group__alloc_1ga45316be86459e993b67bf27efbb8bf5f)
* [PoolFreeEmptyPages](col_alloc_8c.md#group__alloc_1ga0c80585c5110f75f1bf723c9b93df073)
* [ClearPoolBitmasks](col_gc_8c.md#group__gc_1gac78e83c9cbbb1fcbc175aed53353decf)
* [SweepUnreachableCells](col_gc_8c.md#group__gc_1ga911d254f51c4e0b4475330147fbbc545)
* [PromotePages](col_gc_8c.md#group__gc_1ga09e7926c400756068d91140da241934f)

<a id="struct_memory_pool_1afca03b46fe1276b29fdd398ff07dfc41"></a>
### Variable pages

![][public]

**Definition**: `src/colInternal.h` (line 454)

```cpp
Page* MemoryPool::pages
```

Pages form a singly-linked list.





**Type**: [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) *

**Referenced by**:

* [PoolCleanup](col_alloc_8c.md#group__alloc_1ga3ce5b284fd4b0c1f9efa518150268b81)
* [PoolAllocPages](col_alloc_8c.md#group__alloc_1ga45316be86459e993b67bf27efbb8bf5f)
* [PoolFreeEmptyPages](col_alloc_8c.md#group__alloc_1ga0c80585c5110f75f1bf723c9b93df073)
* [PoolAllocCells](col_alloc_8c.md#group__alloc_1gafd84f35bab195e5e45a3338903dbd837)
* [ClearPoolBitmasks](col_gc_8c.md#group__gc_1gac78e83c9cbbb1fcbc175aed53353decf)
* [PromotePages](col_gc_8c.md#group__gc_1ga09e7926c400756068d91140da241934f)
* [ResetPool](col_gc_8c.md#group__gc_1gaa6d1c2ce1d8343c3542c209187b7a47b)

<a id="struct_memory_pool_1a933a8d62099891367279a8486ae81d3e"></a>
### Variable lastPage

![][public]

**Definition**: `src/colInternal.h` (line 455)

```cpp
Page* MemoryPool::lastPage
```

Last page in pool.





**Type**: [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) *

**Referenced by**:

* [PoolAllocPages](col_alloc_8c.md#group__alloc_1ga45316be86459e993b67bf27efbb8bf5f)
* [PoolFreeEmptyPages](col_alloc_8c.md#group__alloc_1ga0c80585c5110f75f1bf723c9b93df073)
* [PoolAllocCells](col_alloc_8c.md#group__alloc_1gafd84f35bab195e5e45a3338903dbd837)
* [PromotePages](col_gc_8c.md#group__gc_1ga09e7926c400756068d91140da241934f)

<a id="struct_memory_pool_1aea8d38c1c90f937e7819b2ab0222ace1"></a>
### Variable lastFreeCell

![][public]

**Definition**: `src/colInternal.h` (line 456)

```cpp
Cell* MemoryPool::lastFreeCell[AVAILABLE_CELLS][AVAILABLE_CELLS]
```

Last cell sequence of a given size found.





**Type**: [Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) *

**Referenced by**:

* [PoolAllocPages](col_alloc_8c.md#group__alloc_1ga45316be86459e993b67bf27efbb8bf5f)
* [PoolAllocCells](col_alloc_8c.md#group__alloc_1gafd84f35bab195e5e45a3338903dbd837)
* [PromotePages](col_gc_8c.md#group__gc_1ga09e7926c400756068d91140da241934f)
* [ResetPool](col_gc_8c.md#group__gc_1gaa6d1c2ce1d8343c3542c209187b7a47b)

<a id="struct_memory_pool_1ab4fba4fd762f5a04c124cfafd6577744"></a>
### Variable nbPages

![][public]

**Definition**: `src/colInternal.h` (line 458)

```cpp
size_t MemoryPool::nbPages
```

Number of pages in pool.





**Type**: size_t

**Referenced by**:

* [PoolAllocPages](col_alloc_8c.md#group__alloc_1ga45316be86459e993b67bf27efbb8bf5f)
* [PoolFreeEmptyPages](col_alloc_8c.md#group__alloc_1ga0c80585c5110f75f1bf723c9b93df073)
* [Col\_ResumeGC](colibri_8h.md#group__gc_1gaf7d4f0dd1996dde366af3f29e9bcc517)
* [PerformGC](col_gc_8c.md#group__gc_1ga5688ae9d7f658650ca8dfa66f4102f62)
* [PromotePages](col_gc_8c.md#group__gc_1ga09e7926c400756068d91140da241934f)

<a id="struct_memory_pool_1a78090a0d29a65f47f1237fd99ee33316"></a>
### Variable nbAlloc

![][public]

**Definition**: `src/colInternal.h` (line 459)

```cpp
size_t MemoryPool::nbAlloc
```

Number of pages alloc'd since last GC.





**Type**: size_t

**Referenced by**:

* [PoolAllocPages](col_alloc_8c.md#group__alloc_1ga45316be86459e993b67bf27efbb8bf5f)
* [Col\_ResumeGC](colibri_8h.md#group__gc_1gaf7d4f0dd1996dde366af3f29e9bcc517)
* [PerformGC](col_gc_8c.md#group__gc_1ga5688ae9d7f658650ca8dfa66f4102f62)
* [PromotePages](col_gc_8c.md#group__gc_1ga09e7926c400756068d91140da241934f)
* [ResetPool](col_gc_8c.md#group__gc_1gaa6d1c2ce1d8343c3542c209187b7a47b)

<a id="struct_memory_pool_1a69b3504fcae96ab4a89e3a0c077ec2ea"></a>
### Variable nbSetCells

![][public]

**Definition**: `src/colInternal.h` (line 461)

```cpp
size_t MemoryPool::nbSetCells
```

Number of set cells in pool.





**Type**: size_t

**Referenced by**:

* [PoolAllocPages](col_alloc_8c.md#group__alloc_1ga45316be86459e993b67bf27efbb8bf5f)
* [PoolFreeEmptyPages](col_alloc_8c.md#group__alloc_1ga0c80585c5110f75f1bf723c9b93df073)
* [PerformGC](col_gc_8c.md#group__gc_1ga5688ae9d7f658650ca8dfa66f4102f62)
* [PromotePages](col_gc_8c.md#group__gc_1ga09e7926c400756068d91140da241934f)

<a id="struct_memory_pool_1a2273b16be4a0e84a1d54df9caa74ddd6"></a>
### Variable gc

![][public]

**Definition**: `src/colInternal.h` (line 462)

```cpp
size_t MemoryPool::gc
```

GC counter.

Used for generational GC.



**Type**: size_t

**Referenced by**:

* [PerformGC](col_gc_8c.md#group__gc_1ga5688ae9d7f658650ca8dfa66f4102f62)
* [ResetPool](col_gc_8c.md#group__gc_1gaa6d1c2ce1d8343c3542c209187b7a47b)

<a id="struct_memory_pool_1ab449566340991fe03f4b917516116958"></a>
### Variable sweepables

![][public]

**Definition**: `src/colInternal.h` (line 464)

```cpp
Col_Word MemoryPool::sweepables
```

List of cells that need sweeping when unreachable after a GC.





**Type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**Referenced by**:

* [RememberSweepable](col_gc_8c.md#group__gc_1ga23c84fc6b7da85d87751fa5788e3f002)
* [SweepUnreachableCells](col_gc_8c.md#group__gc_1ga911d254f51c4e0b4475330147fbbc545)
* [CleanupSweepables](col_gc_8c.md#group__gc_1ga6a8b4c25c3dbf786bfe56975efba86e0)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)