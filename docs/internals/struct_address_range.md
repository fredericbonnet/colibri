<a id="struct_address_range"></a>
# Structure AddressRange

![][C++]
![][private]

**Definition**: `src/colAlloc.c` (line 396)

Address range descriptor for allocated system pages.

**See also**: [SysPageAlloc](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f), [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)

## Members

* [allocInfo](struct_address_range.md#struct_address_range_1ae789f37dab3d981ccf15a020993bfd2e)
* [base](struct_address_range.md#struct_address_range_1a08fc12a83b7988aa0cb8fb45c1736076)
* [first](struct_address_range.md#struct_address_range_1aa8ee3a687b3f1fc91257feb0bd8704cf)
* [free](struct_address_range.md#struct_address_range_1ac7a3b5515ce98c3274f41566e880647e)
* [next](struct_address_range.md#struct_address_range_1a29b3c1f7a257bab2afa0a4ee5c63e60d)
* [size](struct_address_range.md#struct_address_range_1a80783f530919686945d93eb7b1e25623)

## Public attributes

<a id="struct_address_range_1a29b3c1f7a257bab2afa0a4ee5c63e60d"></a>
### Variable next

![][public]

**Definition**: `src/colAlloc.c` (line 397)

```cpp
struct AddressRange* AddressRange::next
```

Next descriptor in list.





**Type**: struct [AddressRange](struct_address_range.md#struct_address_range) *

**Referenced by**:

* [PoolCleanup](col_alloc_8c.md#group__alloc_1ga3ce5b284fd4b0c1f9efa518150268b81)
* [SysPageAlloc](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f)
* [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)
* [SysPageTrim](col_alloc_8c.md#group__alloc_1ga16a9361484b90e5202862b29a03cb958)
* [SysPageProtect](col_alloc_8c.md#group__alloc_1ga35a37fb9d22a879405b65f3e90d09358)
* [PoolFreeEmptyPages](col_alloc_8c.md#group__alloc_1ga0c80585c5110f75f1bf723c9b93df073)
* [UpdateParents](col_alloc_8c.md#group__gc__parents_1gaa3d85dc993fb1b9831f82c25b8c07d3c)
* [PurgeParents](col_gc_8c.md#group__gc_1gae851dc24a0065a16f7149b2f10147b52)
* [GrowHashMap](col_hash_8c.md#group__hashmap__words_1ga695416e52176dc22863ffc2e36f9d9bb)
* [GrowIntHashMap](col_hash_8c.md#group__hashmap__words_1ga65bb60fab52dac94df55edd80a5300de)

<a id="struct_address_range_1a08fc12a83b7988aa0cb8fb45c1736076"></a>
### Variable base

![][public]

**Definition**: `src/colAlloc.c` (line 398)

```cpp
void* AddressRange::base
```

Base address.





**Type**: void *

**Referenced by**:

* [PoolCleanup](col_alloc_8c.md#group__alloc_1ga3ce5b284fd4b0c1f9efa518150268b81)
* [SysPageAlloc](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f)
* [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)
* [SysPageTrim](col_alloc_8c.md#group__alloc_1ga16a9361484b90e5202862b29a03cb958)
* [SysPageProtect](col_alloc_8c.md#group__alloc_1ga35a37fb9d22a879405b65f3e90d09358)
* [PoolAllocPages](col_alloc_8c.md#group__alloc_1ga45316be86459e993b67bf27efbb8bf5f)
* [PoolFreeEmptyPages](col_alloc_8c.md#group__alloc_1ga0c80585c5110f75f1bf723c9b93df073)
* [UpdateParents](col_alloc_8c.md#group__gc__parents_1gaa3d85dc993fb1b9831f82c25b8c07d3c)

<a id="struct_address_range_1a80783f530919686945d93eb7b1e25623"></a>
### Variable size

![][public]

**Definition**: `src/colAlloc.c` (line 399)

```cpp
size_t AddressRange::size
```

Size in pages.





**Type**: size_t

**Referenced by**:

* [FindFreePagesInRange](col_alloc_8c.md#group__alloc_1ga36b4e5e8a0a88ada5fa956cadbe83460)
* [SysPageAlloc](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f)
* [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)
* [SysPageTrim](col_alloc_8c.md#group__alloc_1ga16a9361484b90e5202862b29a03cb958)
* [SysPageProtect](col_alloc_8c.md#group__alloc_1ga35a37fb9d22a879405b65f3e90d09358)
* [UpdateParents](col_alloc_8c.md#group__gc__parents_1gaa3d85dc993fb1b9831f82c25b8c07d3c)
* [Col\_NewCustomHashMap](col_hash_8h.md#group__customhashmap__words_1gad516fa9041eb514e2c5193eb5d958f0e)

<a id="struct_address_range_1ac7a3b5515ce98c3274f41566e880647e"></a>
### Variable free

![][public]

**Definition**: `src/colAlloc.c` (line 400)

```cpp
size_t AddressRange::free
```

Number of free pages in range.





**Type**: size_t

**Referenced by**:

* [SysPageAlloc](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f)
* [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)
* [SysPageTrim](col_alloc_8c.md#group__alloc_1ga16a9361484b90e5202862b29a03cb958)

<a id="struct_address_range_1aa8ee3a687b3f1fc91257feb0bd8704cf"></a>
### Variable first

![][public]

**Definition**: `src/colAlloc.c` (line 401)

```cpp
size_t AddressRange::first
```

First free page in range.





**Type**: size_t

**Referenced by**:

* [FindFreePagesInRange](col_alloc_8c.md#group__alloc_1ga36b4e5e8a0a88ada5fa956cadbe83460)
* [SysPageAlloc](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f)
* [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)
* [SysPageTrim](col_alloc_8c.md#group__alloc_1ga16a9361484b90e5202862b29a03cb958)
* [PoolAllocCells](col_alloc_8c.md#group__alloc_1gafd84f35bab195e5e45a3338903dbd837)
* [PageAllocCells](col_alloc_8c.md#group__alloc_1ga40aaa69883691c7f291a07375ece72ec)
* [FindFreeCells](col_alloc_8c.md#group__alloc_1ga36d583de9ab3c9da67c3da5bef92d135)
* [SetCells](col_alloc_8c.md#group__alloc_1ga9eee2c912dd9c4dc461b8fa760204b5b)
* [ClearCells](col_alloc_8c.md#group__alloc_1ga5d95195ed024066e939d0564549e865d)

<a id="struct_address_range_1ae789f37dab3d981ccf15a020993bfd2e"></a>
### Variable allocInfo

![][public]

**Definition**: `src/colAlloc.c` (line 402)

```cpp
char AddressRange::allocInfo[0][0]
```

Info about allocated pages in range.





**Type**: char

**Referenced by**:

* [FindFreePagesInRange](col_alloc_8c.md#group__alloc_1ga36b4e5e8a0a88ada5fa956cadbe83460)
* [SysPageAlloc](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f)
* [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)
* [SysPageTrim](col_alloc_8c.md#group__alloc_1ga16a9361484b90e5202862b29a03cb958)
* [SysPageProtect](col_alloc_8c.md#group__alloc_1ga35a37fb9d22a879405b65f3e90d09358)
* [UpdateParents](col_alloc_8c.md#group__gc__parents_1gaa3d85dc993fb1b9831f82c25b8c07d3c)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)