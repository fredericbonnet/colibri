<a id="group__alloc"></a>
# Memory Allocation





## Submodules

* [Pages and Cells](group__pages__cells.md#group__pages__cells)

## Bit Handling

<a id="group__alloc_1gaee6b78438aa7f935582f7eefb28e63bf"></a>
### Variable firstZeroBitSequence

![][private]
![][static]

**Definition**: `src/colAlloc.c` (line 84)

```cpp
const char firstZeroBitSequence[7][256][7][256]
```

Position of the first zero-bit sequence of a given length in byte.

First index is length of zero-bit sequence to look for, minus 1. Second index is value of byte in which to search. Result is index of the first bit in matching zero-bit sequence, or -1 if none.



**Type**: const char

**Referenced by**:

* [FindFreeCells](col_alloc_8c.md#group__alloc_1ga36d583de9ab3c9da67c3da5bef92d135)

<a id="group__alloc_1gab2440050425db6a793ac535b9c4ef432"></a>
### Variable longestLeadingZeroBitSequence

![][private]
![][static]

**Definition**: `src/colAlloc.c` (line 218)

```cpp
const char longestLeadingZeroBitSequence[256][256] = {
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
}
```

Longest leading zero-bit sequence in byte.

Index is value of byte. Result is number of consecutive cleared bytes starting at MSB.



**Type**: const char

**Referenced by**:

* [FindFreeCells](col_alloc_8c.md#group__alloc_1ga36d583de9ab3c9da67c3da5bef92d135)

<a id="group__alloc_1ga8c45e7d296e78baadc741c63954c38cc"></a>
### Variable nbBitsSet

![][private]
![][static]

**Definition**: `src/colAlloc.c` (line 241)

```cpp
const char nbBitsSet[256][256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
}
```

Number of bits set in byte.

Index is value of byte. Result is number of set bits.



**Type**: const char

**Referenced by**:

* [NbSetCells](col_alloc_8c.md#group__alloc_1ga356ad642b33dca80bcf2865b1f93c039)

## Address Reservation And Allocation

Granularity-free system page allocation based on address range reservation.





Some systems allow single system page allocations (e.g. POSIX mmap), while others require coarser grained schemes (e.g. Windows VirtualAlloc). However in both cases we have to keep track of allocated pages, especially for generational page write monitoring. So address ranges are reserved, and individual pages can be allocated and freed within these ranges. From a higher level this allows for per-page allocation, while at the same time providing enough metadata for memory introspection.





When the number of pages to allocate exceeds a certain size (defined as [LARGE\_PAGE\_SIZE](col_conf_8h.md#group__alloc_1ga5e794872b12686cc064052cdf6b38f9f)), a dedicated address range is reserved and allocated, which must be freed all at once. Else, pages are allocated within larger address ranges using the following scheme:





Address ranges are reserved in geometrically increasing sizes up to a maximum size (the first being a multiple of the allocation granularity). Ranges form a singly-linked list in allocation order (so that search times are geometrically increasing too). A descriptor structure is malloc'd along with the range and consists of a pointer to the next descriptor, the base address of the range, the total and free numbers of pages and the index of the first free page in range, and a page alloc info table indicating:






* For free pages, zero.

* For first page in group, the negated size of the group.

* For remaining pages, the index in group.







That way, page group info can be known via direct access from the page index in range:






* When alloc info is zero, the page is free.

* When negative, the page is the first in a group of the given negated size.

* When positive, the page is the n-th in a group whose first page is the n-th previous one.







To allocate a group of pages in an address range, the alloc info table is scanned until a large enough group of free pages is found.





To free a group of pages, the containing address range is found by scanning all ranges in order (this is fast, as this only involves address comparison and the ranges grow geometrically). In either cases the descriptor is updated accordingly. If a containing range is not found we assume that it was a dedicated range and we attempt to release it at once.





Just following this alloc info table is a bitmask table used for write tracking. With our generational GC, pages of older generations are write-protected so that references pointing to younger cells can be tracked during the mark phase: such modified pages contain potential parent cells that must be followed along with roots. Regular ranges use a bitmask array, while dedicated ranges only have to store one value for the whole range.





This allocation scheme may not look optimal at first sight (especially the alloc info table scanning step), but keep in mind that the typical use case only involves single page allocations. Multiple page allocations only occur when allocating large, multiple cell-based structures, and most structures are single cell sized. And very large pages will end up in their own dedicated range with no group management. Moreover stress tests have shown that this scheme yielded similar or better performances than the previous schemes.

<a id="group__alloc_1ga9ac4c516a0888195d1f2ca4721f633f8"></a>
### Variable ranges

![][private]
![][static]

**Definition**: `src/colAlloc.c` (line 411)

```cpp
AddressRange* ranges = NULL
```

Reserved address ranges for general purpose.

**See also**: [SysPageAlloc](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f), [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)



**Type**: [AddressRange](struct_address_range.md#struct_address_range) *

**Referenced by**:

* [SysPageAlloc](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f)
* [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)
* [SysPageProtect](col_alloc_8c.md#group__alloc_1ga35a37fb9d22a879405b65f3e90d09358)
* [SysPageTrim](col_alloc_8c.md#group__alloc_1ga16a9361484b90e5202862b29a03cb958)
* [UpdateParents](col_alloc_8c.md#group__gc__parents_1gaa3d85dc993fb1b9831f82c25b8c07d3c)

<a id="group__alloc_1ga09e3f1c0494d23d9f93481ed4f228a4c"></a>
### Variable dedicatedRanges

![][private]
![][static]

**Definition**: `src/colAlloc.c` (line 419)

```cpp
AddressRange* dedicatedRanges = NULL
```

Dedicated address ranges for large pages.

**See also**: [SysPageAlloc](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f), [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)



**Type**: [AddressRange](struct_address_range.md#struct_address_range) *

**Referenced by**:

* [SysPageAlloc](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f)
* [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)
* [SysPageProtect](col_alloc_8c.md#group__alloc_1ga35a37fb9d22a879405b65f3e90d09358)
* [UpdateParents](col_alloc_8c.md#group__gc__parents_1gaa3d85dc993fb1b9831f82c25b8c07d3c)

<a id="group__alloc_1ga36b4e5e8a0a88ada5fa956cadbe83460"></a>
### Function FindFreePagesInRange

![][private]
![][static]

```cpp
static size_t FindFreePagesInRange(AddressRange *range, size_t number, size_t index)
```

Find given number of free consecutive pages in alloc info table.

**Return values**:

* **index**: of first page of sequence if found.
* **-1**: otherwise.



**Parameters**:

* [AddressRange](struct_address_range.md#struct_address_range) * **range**: Address range to look into.
* size_t **number**: Number of free consecutive entries to find.
* size_t **index**: First entry to consider.

**Return type**: size_t

**References**:

* [AddressRange::allocInfo](struct_address_range.md#struct_address_range_1ae789f37dab3d981ccf15a020993bfd2e)
* [AddressRange::first](struct_address_range.md#struct_address_range_1aa8ee3a687b3f1fc91257feb0bd8704cf)
* [AddressRange::size](struct_address_range.md#struct_address_range_1a80783f530919686945d93eb7b1e25623)

**Referenced by**:

* [SysPageAlloc](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f)

<a id="group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f"></a>
### Function SysPageAlloc

![][private]
![][static]

```cpp
static void* SysPageAlloc(size_t number, int written)
```

Allocate system pages.

**Returns**:

The allocated system pages' base address.


**Side Effect**:

May reserve new address ranges.



**See also**: [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)

**Exceptions**:

* **[COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)**: [[F]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27) Memory error. (_Address range allocation failed_)
* **[COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)**: [[F]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27) Memory error. (_Address range reservation failed_)
* **[COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)**: [[F]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27) Memory error. (_Page allocation failed_)

**Parameters**:

* size_t **number**: Number of system pages to alloc.
* int **written**: Initial write tracking flag value.

**Return type**: void *

**References**:

* [allocGranularity](col_alloc_8c.md#group__arch_1ga9fdf03be3e1742b0fbc71f3445d83c48)
* [AddressRange::allocInfo](struct_address_range.md#struct_address_range_1ae789f37dab3d981ccf15a020993bfd2e)
* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [AddressRange::base](struct_address_range.md#struct_address_range_1a08fc12a83b7988aa0cb8fb45c1736076)
* [Col\_Error](colibri_8h.md#group__error_1ga9a9a9c96b23c489cf8a19a6248fc77b8)
* [COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)
* [COL\_FATAL](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27)
* [ColibriDomain](colibri_8c.md#group__error_1gadf9c5202f89dd2ecc2aeee560f04ee4d)
* [dedicatedRanges](col_alloc_8c.md#group__alloc_1ga09e3f1c0494d23d9f93481ed4f228a4c)
* [FindFreePagesInRange](col_alloc_8c.md#group__alloc_1ga36b4e5e8a0a88ada5fa956cadbe83460)
* [AddressRange::first](struct_address_range.md#struct_address_range_1aa8ee3a687b3f1fc91257feb0bd8704cf)
* [FIRST\_RANGE\_SIZE](col_alloc_8c.md#group__alloc_1gaf947156938f941bb8251459e8bfc1021)
* [AddressRange::free](struct_address_range.md#struct_address_range_1ac7a3b5515ce98c3274f41566e880647e)
* [LARGE\_PAGE\_SIZE](col_conf_8h.md#group__alloc_1ga5e794872b12686cc064052cdf6b38f9f)
* [MAX\_RANGE\_SIZE](col_alloc_8c.md#group__alloc_1gab4949f1d9f3dae07dd371a5b01f46c26)
* [AddressRange::next](struct_address_range.md#struct_address_range_1a29b3c1f7a257bab2afa0a4ee5c63e60d)
* [PlatAllocPages](col_platform_8h.md#group__arch_1gab258be1bc40313f7c0a4c8bcab840243)
* [PlatEnterProtectAddressRanges](col_unix_platform_8h.md#group__arch__unix_1ga0d52de05fb3a0897f54579ab12519159)
* [PlatLeaveProtectAddressRanges](col_unix_platform_8h.md#group__arch__unix_1gab6be4d622dd8c6bc4c283a1039962e5a)
* [PlatReserveRange](col_platform_8h.md#group__arch_1ga7d63e9c0de11a4c0954aab0efbc09c09)
* [ranges](col_alloc_8c.md#group__alloc_1ga9ac4c516a0888195d1f2ca4721f633f8)
* [shiftPage](col_alloc_8c.md#group__arch_1gacfb643d4c365f92c1ea93d0f1b1b71e5)
* [AddressRange::size](struct_address_range.md#struct_address_range_1a80783f530919686945d93eb7b1e25623)

**Referenced by**:

* [PoolAllocPages](col_alloc_8c.md#group__alloc_1ga45316be86459e993b67bf27efbb8bf5f)

<a id="group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f"></a>
### Function SysPageFree

![][private]
![][static]

```cpp
static void SysPageFree(void *base)
```

Free system pages.

**Side Effect**:

May release address ranges.



**See also**: [SysPageAlloc](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f)

**Exceptions**:

* **[COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)**: [[F]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27) Memory error. (_Page not found_)
* **[COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)**: [[F]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27) Memory error. (_Address range release failed_)
* **[COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)**: [[F]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27) Memory error. (_Page deallocation failed_)

**Parameters**:

* void * **base**: Base address of the pages to free.

**Return type**: void

**References**:

* [AddressRange::allocInfo](struct_address_range.md#struct_address_range_1ae789f37dab3d981ccf15a020993bfd2e)
* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [AddressRange::base](struct_address_range.md#struct_address_range_1a08fc12a83b7988aa0cb8fb45c1736076)
* [Col\_Error](colibri_8h.md#group__error_1ga9a9a9c96b23c489cf8a19a6248fc77b8)
* [COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)
* [COL\_FATAL](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27)
* [ColibriDomain](colibri_8c.md#group__error_1gadf9c5202f89dd2ecc2aeee560f04ee4d)
* [dedicatedRanges](col_alloc_8c.md#group__alloc_1ga09e3f1c0494d23d9f93481ed4f228a4c)
* [AddressRange::first](struct_address_range.md#struct_address_range_1aa8ee3a687b3f1fc91257feb0bd8704cf)
* [AddressRange::free](struct_address_range.md#struct_address_range_1ac7a3b5515ce98c3274f41566e880647e)
* [AddressRange::next](struct_address_range.md#struct_address_range_1a29b3c1f7a257bab2afa0a4ee5c63e60d)
* [PlatEnterProtectAddressRanges](col_unix_platform_8h.md#group__arch__unix_1ga0d52de05fb3a0897f54579ab12519159)
* [PlatFreePages](col_platform_8h.md#group__arch_1ga3af1cd9d798457114f4bf563c10f9875)
* [PlatLeaveProtectAddressRanges](col_unix_platform_8h.md#group__arch__unix_1gab6be4d622dd8c6bc4c283a1039962e5a)
* [PlatReleaseRange](col_platform_8h.md#group__arch_1ga7ca830366dd1fb4e114a2f912ef5ed6c)
* [ranges](col_alloc_8c.md#group__alloc_1ga9ac4c516a0888195d1f2ca4721f633f8)
* [shiftPage](col_alloc_8c.md#group__arch_1gacfb643d4c365f92c1ea93d0f1b1b71e5)
* [AddressRange::size](struct_address_range.md#struct_address_range_1a80783f530919686945d93eb7b1e25623)

**Referenced by**:

* [PoolCleanup](col_alloc_8c.md#group__alloc_1ga3ce5b284fd4b0c1f9efa518150268b81)
* [PoolFreeEmptyPages](col_alloc_8c.md#group__alloc_1ga0c80585c5110f75f1bf723c9b93df073)

<a id="group__alloc_1ga16a9361484b90e5202862b29a03cb958"></a>
### Function SysPageTrim

![][private]
![][static]

```cpp
static void SysPageTrim(void *base)
```

Free trailing pages of system page groups, keeping only the first page.

**See also**: [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)

**Exceptions**:

* **[COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)**: [[F]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27) Memory error. (_Page not found_)
* **[COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)**: [[F]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27) Memory error. (_Page deallocation failed_)

**Parameters**:

* void * **base**: Base address of the pages to free.

**Return type**: void

**References**:

* [AddressRange::allocInfo](struct_address_range.md#struct_address_range_1ae789f37dab3d981ccf15a020993bfd2e)
* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [AddressRange::base](struct_address_range.md#struct_address_range_1a08fc12a83b7988aa0cb8fb45c1736076)
* [Col\_Error](colibri_8h.md#group__error_1ga9a9a9c96b23c489cf8a19a6248fc77b8)
* [COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)
* [COL\_FATAL](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27)
* [ColibriDomain](colibri_8c.md#group__error_1gadf9c5202f89dd2ecc2aeee560f04ee4d)
* [AddressRange::first](struct_address_range.md#struct_address_range_1aa8ee3a687b3f1fc91257feb0bd8704cf)
* [AddressRange::free](struct_address_range.md#struct_address_range_1ac7a3b5515ce98c3274f41566e880647e)
* [AddressRange::next](struct_address_range.md#struct_address_range_1a29b3c1f7a257bab2afa0a4ee5c63e60d)
* [PlatEnterProtectAddressRanges](col_unix_platform_8h.md#group__arch__unix_1ga0d52de05fb3a0897f54579ab12519159)
* [PlatFreePages](col_platform_8h.md#group__arch_1ga3af1cd9d798457114f4bf563c10f9875)
* [PlatLeaveProtectAddressRanges](col_unix_platform_8h.md#group__arch__unix_1gab6be4d622dd8c6bc4c283a1039962e5a)
* [ranges](col_alloc_8c.md#group__alloc_1ga9ac4c516a0888195d1f2ca4721f633f8)
* [shiftPage](col_alloc_8c.md#group__arch_1gacfb643d4c365f92c1ea93d0f1b1b71e5)
* [AddressRange::size](struct_address_range.md#struct_address_range_1a80783f530919686945d93eb7b1e25623)
* [systemPageSize](col_alloc_8c.md#group__arch_1gacfca316efccddeee528c309c490c3f90)

**Referenced by**:

* [PoolFreeEmptyPages](col_alloc_8c.md#group__alloc_1ga0c80585c5110f75f1bf723c9b93df073)

<a id="group__alloc_1gaf947156938f941bb8251459e8bfc1021"></a>
### Macro FIRST\_RANGE\_SIZE

![][public]

```cpp
#define FIRST_RANGE_SIZE 256     /* 1 MB */
```

Size of first reserved range.





<a id="group__alloc_1gab4949f1d9f3dae07dd371a5b01f46c26"></a>
### Macro MAX\_RANGE\_SIZE

![][public]

```cpp
#define MAX_RANGE_SIZE 32768   /* 128 MB */
```

Maximum size of ranges.





## Page and Cell Allocation-Related Configuration Settings

<a id="group__alloc_1ga5e794872b12686cc064052cdf6b38f9f"></a>
### Macro LARGE\_PAGE\_SIZE

![][public]

```cpp
#define LARGE_PAGE_SIZE 128 /* 512 KB on 64-bit */
```

Large page size.

This is the number of pages above which cell groups are allocated in their own dedicated page range, i.e. no free cell remains after allocation.






?> Value should not exceed 128 (see [AddressRange](struct_address_range.md#struct_address_range)).



**See also**: [PoolAllocPages](col_alloc_8c.md#group__alloc_1ga45316be86459e993b67bf27efbb8bf5f)



<a id="group__alloc_1ga7d467c1d283fdfa1f2081ba1e0d01b6e"></a>
### Macro PAGE\_SIZE

![][public]

```cpp
#define PAGE_SIZE 1024
```

Page size in bytes.





<a id="group__alloc_1ga7a4127f14f16563da90eb3c836bc404f"></a>
### Macro CELL\_SIZE

![][public]

```cpp
#define CELL_SIZE 16 /* 4*4 */
```

Cell size in bytes.





<a id="group__alloc_1gae8f0d88b8c73d3fb1bc64a0e2ef68faa"></a>
### Macro CELLS\_PER\_PAGE

![][public]

```cpp
#define CELLS_PER_PAGE 64 /* [PAGE\_SIZE](col_conf_8h.md#group__alloc_1ga7d467c1d283fdfa1f2081ba1e0d01b6e)/[CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f) */
```

Number of cells per page.





## Memory Pools

<a id="group__alloc_1gab08d29a35a3b57dff2e79468a569ad81"></a>
### Function PoolInit

![][private]

```cpp
void PoolInit(MemoryPool *pool, unsigned int generation)
```

Initialize memory pool.





**Parameters**:

* [MemoryPool](struct_memory_pool.md#struct_memory_pool) * **pool**: Pool to initialize.
* unsigned int **generation**: Generation number; 0 = youngest.

**Return type**: void

**References**:

* [MemoryPool::generation](struct_memory_pool.md#struct_memory_pool_1a0d3c5d41525ad009c79ca65f07799ce6)

**Referenced by**:

* [GcInitGroup](col_gc_8c.md#group__gc_1ga4183a56fec06c1ff8176f90c31248d68)
* [GcInitThread](col_gc_8c.md#group__gc_1gad46e288d322683c434e6efa30e3edc22)

<a id="group__alloc_1ga3ce5b284fd4b0c1f9efa518150268b81"></a>
### Function PoolCleanup

![][private]

```cpp
void PoolCleanup(MemoryPool *pool)
```

Cleanup memory pool.

**See also**: [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)



**Parameters**:

* [MemoryPool](struct_memory_pool.md#struct_memory_pool) * **pool**: Pool to cleanup.

**Return type**: void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [AddressRange::base](struct_address_range.md#struct_address_range_1a08fc12a83b7988aa0cb8fb45c1736076)
* [CleanupSweepables](col_gc_8c.md#group__gc_1ga6a8b4c25c3dbf786bfe56975efba86e0)
* [AddressRange::next](struct_address_range.md#struct_address_range_1a29b3c1f7a257bab2afa0a4ee5c63e60d)
* [PAGE\_FLAG](col_internal_8h.md#group__pages__cells_1ga3ccf282a118bd7b9101e38f4228e8853)
* [PAGE\_FLAG\_FIRST](col_internal_8h.md#group__pages__cells_1gace2cf8cd83ae1a8cc646bbd367cd5a86)
* [PAGE\_FLAG\_LAST](col_internal_8h.md#group__pages__cells_1ga011eeac135e2e667ae1356b0abf9c727)
* [PAGE\_NEXT](col_internal_8h.md#group__pages__cells_1ga0ed08f16e66e86cbed58b86203c7fd38)
* [MemoryPool::pages](struct_memory_pool.md#struct_memory_pool_1afca03b46fe1276b29fdd398ff07dfc41)
* [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)

**Referenced by**:

* [GcCleanupGroup](col_gc_8c.md#group__gc_1ga8e6fd33d04cb870d9d4b64eb34ab2ebc)
* [GcCleanupThread](col_gc_8c.md#group__gc_1gacbea8d94b2fdf7366fbe4ad8b8298a91)

## System Page Allocation

<a id="group__alloc_1ga35a37fb9d22a879405b65f3e90d09358"></a>
### Function SysPageProtect

![][private]

```cpp
void SysPageProtect(void *page, int protect)
```

Write-protect system page group.



**Exceptions**:

* **[COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)**: [[F]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27) Memory error. (_Page not found_)

**Parameters**:

* void * **page**: Page belonging to page group to protect.
* int **protect**: Whether to protect or unprotect page group.

**Return type**: void

**References**:

* [AddressRange::allocInfo](struct_address_range.md#struct_address_range_1ae789f37dab3d981ccf15a020993bfd2e)
* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [AddressRange::base](struct_address_range.md#struct_address_range_1a08fc12a83b7988aa0cb8fb45c1736076)
* [Col\_Error](colibri_8h.md#group__error_1ga9a9a9c96b23c489cf8a19a6248fc77b8)
* [COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)
* [COL\_FATAL](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27)
* [ColibriDomain](colibri_8c.md#group__error_1gadf9c5202f89dd2ecc2aeee560f04ee4d)
* [dedicatedRanges](col_alloc_8c.md#group__alloc_1ga09e3f1c0494d23d9f93481ed4f228a4c)
* [AddressRange::next](struct_address_range.md#struct_address_range_1a29b3c1f7a257bab2afa0a4ee5c63e60d)
* [PlatEnterProtectAddressRanges](col_unix_platform_8h.md#group__arch__unix_1ga0d52de05fb3a0897f54579ab12519159)
* [PlatLeaveProtectAddressRanges](col_unix_platform_8h.md#group__arch__unix_1gab6be4d622dd8c6bc4c283a1039962e5a)
* [PlatProtectPages](col_platform_8h.md#group__arch_1gaafe939448070a0b389fc64dcd009ac9e)
* [ranges](col_alloc_8c.md#group__alloc_1ga9ac4c516a0888195d1f2ca4721f633f8)
* [shiftPage](col_alloc_8c.md#group__arch_1gacfb643d4c365f92c1ea93d0f1b1b71e5)
* [AddressRange::size](struct_address_range.md#struct_address_range_1a80783f530919686945d93eb7b1e25623)

**Referenced by**:

* [ClearPoolBitmasks](col_gc_8c.md#group__gc_1gac78e83c9cbbb1fcbc175aed53353decf)
* [PageProtectSigAction](col_unix_platform_8c.md#group__arch__unix_1ga4e30b30c96c05d605c05e40613e6dc5a)
* [PageProtectVectoredHandler](col_win32_platform_8c.md#group__arch__win32_1ga1a9939ca3576553f6bd29f80af62c862)
* [PromotePages](col_gc_8c.md#group__gc_1ga09e7926c400756068d91140da241934f)
* [PurgeParents](col_gc_8c.md#group__gc_1gae851dc24a0065a16f7149b2f10147b52)

## Page Allocation

<a id="group__alloc_1ga45316be86459e993b67bf27efbb8bf5f"></a>
### Function PoolAllocPages

![][private]

```cpp
void PoolAllocPages(MemoryPool *pool, size_t number)
```

Allocate pages in pool.

Pages are inserted after the given page. This guarantees better performances by avoiding the traversal of previous pages.







**See also**: [SysPageAlloc](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f)



**Parameters**:

* [MemoryPool](struct_memory_pool.md#struct_memory_pool) * **pool**: Pool to allocate pages into.
* size_t **number**: Number of pages to allocate.

**Return type**: void

**References**:

* [AVAILABLE\_CELLS](col_internal_8h.md#group__pages__cells_1ga524e5a52183dcc7088644df29ef766bf)
* [AddressRange::base](struct_address_range.md#struct_address_range_1a08fc12a83b7988aa0cb8fb45c1736076)
* [ClearAllCells](col_alloc_8c.md#group__alloc_1gaad7a90e68f1bfd00a40c626c7bfe5c5f)
* [MemoryPool::generation](struct_memory_pool.md#struct_memory_pool_1a0d3c5d41525ad009c79ca65f07799ce6)
* [ThreadData::groupData](struct_thread_data.md#struct_thread_data_1aefbdf49c641476274db5326c60853022)
* [LARGE\_PAGE\_SIZE](col_conf_8h.md#group__alloc_1ga5e794872b12686cc064052cdf6b38f9f)
* [MemoryPool::lastFreeCell](struct_memory_pool.md#struct_memory_pool_1aea8d38c1c90f937e7819b2ab0222ace1)
* [MemoryPool::lastPage](struct_memory_pool.md#struct_memory_pool_1a933a8d62099891367279a8486ae81d3e)
* [MemoryPool::nbAlloc](struct_memory_pool.md#struct_memory_pool_1a78090a0d29a65f47f1237fd99ee33316)
* [MemoryPool::nbPages](struct_memory_pool.md#struct_memory_pool_1ab4fba4fd762f5a04c124cfafd6577744)
* [MemoryPool::nbSetCells](struct_memory_pool.md#struct_memory_pool_1a69b3504fcae96ab4a89e3a0c077ec2ea)
* [PAGE\_CELL](col_internal_8h.md#group__pages__cells_1ga2fb20c83455d53df390ae692b500cb5f)
* [PAGE\_CLEAR\_FLAG](col_internal_8h.md#group__pages__cells_1gad96556885fb99327d9e9c677043584f5)
* [PAGE\_FLAG\_FIRST](col_internal_8h.md#group__pages__cells_1gace2cf8cd83ae1a8cc646bbd367cd5a86)
* [PAGE\_FLAG\_LAST](col_internal_8h.md#group__pages__cells_1ga011eeac135e2e667ae1356b0abf9c727)
* [PAGE\_FLAGS\_MASK](col_internal_8h.md#group__pages__cells_1gaf5f12024480e802312d785501f55b443)
* [PAGE\_GROUPDATA](col_internal_8h.md#group__pages__cells_1gacb4ea8da3119e9c387474afde87569dc)
* [PAGE\_SET\_FLAG](col_internal_8h.md#group__pages__cells_1ga6bfd878f32448ca9487ecfedfabdaed6)
* [PAGE\_SET\_GENERATION](col_internal_8h.md#group__pages__cells_1ga29860aee271faab1c7c7c4b16d8a3da1)
* [PAGE\_SET\_NEXT](col_internal_8h.md#group__pages__cells_1ga8ecae0f26c64c6971edbe344eb117665)
* [PAGE\_SIZE](col_conf_8h.md#group__alloc_1ga7d467c1d283fdfa1f2081ba1e0d01b6e)
* [MemoryPool::pages](struct_memory_pool.md#struct_memory_pool_1afca03b46fe1276b29fdd398ff07dfc41)
* [PlatGetThreadData](col_unix_platform_8h.md#group__arch__unix_1ga6964b3c4d4787a9defb7aae57825d92c)
* [RESERVED\_CELLS](col_internal_8h.md#group__pages__cells_1ga2e2387471157b525133bb3d9ddc02bde)
* [SysPageAlloc](col_alloc_8c.md#group__alloc_1ga9318fd94abe19ee6d962cacb9d08830f)
* [systemPageSize](col_alloc_8c.md#group__arch_1gacfca316efccddeee528c309c490c3f90)

**Referenced by**:

* [PoolAllocCells](col_alloc_8c.md#group__alloc_1gafd84f35bab195e5e45a3338903dbd837)

<a id="group__alloc_1ga0c80585c5110f75f1bf723c9b93df073"></a>
### Function PoolFreeEmptyPages

![][private]

```cpp
void PoolFreeEmptyPages(MemoryPool *pool)
```

Free empty system pages in pool.

Refresh page count in the process.







**See also**: [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)



**Parameters**:

* [MemoryPool](struct_memory_pool.md#struct_memory_pool) * **pool**: Pool with pages to free.

**Return type**: void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [AddressRange::base](struct_address_range.md#struct_address_range_1a08fc12a83b7988aa0cb8fb45c1736076)
* [CELLS\_PER\_PAGE](col_conf_8h.md#group__alloc_1gae8f0d88b8c73d3fb1bc64a0e2ef68faa)
* [ClearAllCells](col_alloc_8c.md#group__alloc_1gaad7a90e68f1bfd00a40c626c7bfe5c5f)
* [MemoryPool::generation](struct_memory_pool.md#struct_memory_pool_1a0d3c5d41525ad009c79ca65f07799ce6)
* [ThreadData::groupData](struct_thread_data.md#struct_thread_data_1aefbdf49c641476274db5326c60853022)
* [MemoryPool::lastPage](struct_memory_pool.md#struct_memory_pool_1a933a8d62099891367279a8486ae81d3e)
* [MemoryPool::nbPages](struct_memory_pool.md#struct_memory_pool_1ab4fba4fd762f5a04c124cfafd6577744)
* [NbSetCells](col_alloc_8c.md#group__alloc_1ga356ad642b33dca80bcf2865b1f93c039)
* [MemoryPool::nbSetCells](struct_memory_pool.md#struct_memory_pool_1a69b3504fcae96ab4a89e3a0c077ec2ea)
* [AddressRange::next](struct_address_range.md#struct_address_range_1a29b3c1f7a257bab2afa0a4ee5c63e60d)
* [PAGE\_CLEAR\_FLAG](col_internal_8h.md#group__pages__cells_1gad96556885fb99327d9e9c677043584f5)
* [PAGE\_FLAG](col_internal_8h.md#group__pages__cells_1ga3ccf282a118bd7b9101e38f4228e8853)
* [PAGE\_FLAG\_FIRST](col_internal_8h.md#group__pages__cells_1gace2cf8cd83ae1a8cc646bbd367cd5a86)
* [PAGE\_FLAG\_LAST](col_internal_8h.md#group__pages__cells_1ga011eeac135e2e667ae1356b0abf9c727)
* [PAGE\_FLAGS\_MASK](col_internal_8h.md#group__pages__cells_1gaf5f12024480e802312d785501f55b443)
* [PAGE\_GROUPDATA](col_internal_8h.md#group__pages__cells_1gacb4ea8da3119e9c387474afde87569dc)
* [PAGE\_NEXT](col_internal_8h.md#group__pages__cells_1ga0ed08f16e66e86cbed58b86203c7fd38)
* [PAGE\_SET\_FLAG](col_internal_8h.md#group__pages__cells_1ga6bfd878f32448ca9487ecfedfabdaed6)
* [PAGE\_SET\_GENERATION](col_internal_8h.md#group__pages__cells_1ga29860aee271faab1c7c7c4b16d8a3da1)
* [PAGE\_SET\_NEXT](col_internal_8h.md#group__pages__cells_1ga8ecae0f26c64c6971edbe344eb117665)
* [PAGE\_SIZE](col_conf_8h.md#group__alloc_1ga7d467c1d283fdfa1f2081ba1e0d01b6e)
* [MemoryPool::pages](struct_memory_pool.md#struct_memory_pool_1afca03b46fe1276b29fdd398ff07dfc41)
* [PlatGetThreadData](col_unix_platform_8h.md#group__arch__unix_1ga6964b3c4d4787a9defb7aae57825d92c)
* [RESERVED\_CELLS](col_internal_8h.md#group__pages__cells_1ga2e2387471157b525133bb3d9ddc02bde)
* [SysPageFree](col_alloc_8c.md#group__alloc_1gae87fa6ec29c10f180f511dfd1213fe5f)
* [SysPageTrim](col_alloc_8c.md#group__alloc_1ga16a9361484b90e5202862b29a03cb958)
* [systemPageSize](col_alloc_8c.md#group__arch_1gacfca316efccddeee528c309c490c3f90)
* [TestCell](col_alloc_8c.md#group__alloc_1gade7bbd62a937c3b2ed2f32c34c6c60a6)

**Referenced by**:

* [PerformGC](col_gc_8c.md#group__gc_1ga5688ae9d7f658650ca8dfa66f4102f62)

## Cell Allocation

<a id="group__alloc_1gafd84f35bab195e5e45a3338903dbd837"></a>
### Function PoolAllocCells

![][private]

```cpp
Cell * PoolAllocCells(MemoryPool *pool, size_t number)
```

Allocate cells in a pool, allocating new pages if needed.

Traverse and search all existing pages for a free cell sequence of the given length, and if none is found, allocate new pages.






**Return values**:

* **pointer**: to the first allocated cell if successful
* **NULL**: otherwise


**Side Effect**:

Memory pages may be allocated. This may trigger the GC later once we leave the GC-protected section.




**See also**: [PageAllocCells](col_alloc_8c.md#group__alloc_1ga40aaa69883691c7f291a07375ece72ec), [PoolAllocPages](col_alloc_8c.md#group__alloc_1ga45316be86459e993b67bf27efbb8bf5f)

**Exceptions**:

* **[COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)**: [[F]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27) Memory error. (_Page allocation failed_)
* **[COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)**: [[F]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27) Memory error. (_Page allocation failed_)

**Parameters**:

* [MemoryPool](struct_memory_pool.md#struct_memory_pool) * **pool**: Pool to allocate cells into.
* size_t **number**: Number of cells to allocate.

**Return type**: [Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) *

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [AVAILABLE\_CELLS](col_internal_8h.md#group__pages__cells_1ga524e5a52183dcc7088644df29ef766bf)
* [CELLS\_PER\_PAGE](col_conf_8h.md#group__alloc_1gae8f0d88b8c73d3fb1bc64a0e2ef68faa)
* [Col\_Error](colibri_8h.md#group__error_1ga9a9a9c96b23c489cf8a19a6248fc77b8)
* [COL\_ERROR\_MEMORY](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3)
* [COL\_FATAL](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27)
* [ColibriDomain](colibri_8c.md#group__error_1gadf9c5202f89dd2ecc2aeee560f04ee4d)
* [AddressRange::first](struct_address_range.md#struct_address_range_1aa8ee3a687b3f1fc91257feb0bd8704cf)
* [LARGE\_PAGE\_SIZE](col_conf_8h.md#group__alloc_1ga5e794872b12686cc064052cdf6b38f9f)
* [MemoryPool::lastFreeCell](struct_memory_pool.md#struct_memory_pool_1aea8d38c1c90f937e7819b2ab0222ace1)
* [MemoryPool::lastPage](struct_memory_pool.md#struct_memory_pool_1a933a8d62099891367279a8486ae81d3e)
* [PAGE\_CELL](col_internal_8h.md#group__pages__cells_1ga2fb20c83455d53df390ae692b500cb5f)
* [PAGE\_NEXT](col_internal_8h.md#group__pages__cells_1ga0ed08f16e66e86cbed58b86203c7fd38)
* [PageAllocCells](col_alloc_8c.md#group__alloc_1ga40aaa69883691c7f291a07375ece72ec)
* [MemoryPool::pages](struct_memory_pool.md#struct_memory_pool_1afca03b46fe1276b29fdd398ff07dfc41)
* [PoolAllocPages](col_alloc_8c.md#group__alloc_1ga45316be86459e993b67bf27efbb8bf5f)
* [SetCells](col_alloc_8c.md#group__alloc_1ga9eee2c912dd9c4dc461b8fa760204b5b)

**Referenced by**:

* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)
* [Col\_WordPreserve](col_word_8h.md#group__words_1gab55f452e6b0856f7bd7b34e04fae2aa2)
* [MarkWord](col_gc_8c.md#group__gc_1gaf54093bb37e6a4aaaf718fb1a791d56c)
* [UpdateParents](col_alloc_8c.md#group__gc__parents_1gaa3d85dc993fb1b9831f82c25b8c07d3c)

<a id="group__alloc_1ga9eee2c912dd9c4dc461b8fa760204b5b"></a>
### Function SetCells

![][private]

```cpp
void SetCells(Page *page, size_t first, size_t number)
```

Set the page bitmask for a given sequence of cells.





**Parameters**:

* [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) * **page**: The page.
* size_t **first**: Index of first cell.
* size_t **number**: Number of cells in sequence.

**Return type**: void

**References**:

* [AddressRange::first](struct_address_range.md#struct_address_range_1aa8ee3a687b3f1fc91257feb0bd8704cf)
* [PAGE\_BITMASK](col_internal_8h.md#group__pages__cells_1ga00782a3620010f5d871ea505e6ca3abe)

**Referenced by**:

* [ClearAllCells](col_alloc_8c.md#group__alloc_1gaad7a90e68f1bfd00a40c626c7bfe5c5f)
* [MarkWord](col_gc_8c.md#group__gc_1gaf54093bb37e6a4aaaf718fb1a791d56c)
* [PageAllocCells](col_alloc_8c.md#group__alloc_1ga40aaa69883691c7f291a07375ece72ec)
* [PoolAllocCells](col_alloc_8c.md#group__alloc_1gafd84f35bab195e5e45a3338903dbd837)

<a id="group__alloc_1ga5d95195ed024066e939d0564549e865d"></a>
### Function ClearCells

![][private]

```cpp
void ClearCells(Page *page, size_t first, size_t number)
```

Clear the page bitmask for a given sequence of cells.





**Parameters**:

* [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) * **page**: The page.
* size_t **first**: Index of first cell.
* size_t **number**: Number of cells in sequence.

**Return type**: void

**References**:

* [AddressRange::first](struct_address_range.md#struct_address_range_1aa8ee3a687b3f1fc91257feb0bd8704cf)
* [PAGE\_BITMASK](col_internal_8h.md#group__pages__cells_1ga00782a3620010f5d871ea505e6ca3abe)

**Referenced by**:

* [Col\_WordRelease](col_word_8h.md#group__words_1gad93112f81ce6511d6d0ece0db4d38598)
* [MarkReachableCellsFromParents](col_gc_8c.md#group__gc_1ga49eb8981c888c90530906952e2869000)
* [MarkWord](col_gc_8c.md#group__gc_1gaf54093bb37e6a4aaaf718fb1a791d56c)
* [PurgeParents](col_gc_8c.md#group__gc_1gae851dc24a0065a16f7149b2f10147b52)

<a id="group__alloc_1gaad7a90e68f1bfd00a40c626c7bfe5c5f"></a>
### Function ClearAllCells

![][private]

```cpp
void ClearAllCells(Page *page)
```

Clear the page bitmask.





**Parameters**:

* [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) * **page**: The page.

**Return type**: void

**References**:

* [CELLS\_PER\_PAGE](col_conf_8h.md#group__alloc_1gae8f0d88b8c73d3fb1bc64a0e2ef68faa)
* [PAGE\_BITMASK](col_internal_8h.md#group__pages__cells_1ga00782a3620010f5d871ea505e6ca3abe)
* [RESERVED\_CELLS](col_internal_8h.md#group__pages__cells_1ga2e2387471157b525133bb3d9ddc02bde)
* [SetCells](col_alloc_8c.md#group__alloc_1ga9eee2c912dd9c4dc461b8fa760204b5b)

**Referenced by**:

* [ClearPoolBitmasks](col_gc_8c.md#group__gc_1gac78e83c9cbbb1fcbc175aed53353decf)
* [PoolAllocPages](col_alloc_8c.md#group__alloc_1ga45316be86459e993b67bf27efbb8bf5f)
* [PoolFreeEmptyPages](col_alloc_8c.md#group__alloc_1ga0c80585c5110f75f1bf723c9b93df073)

<a id="group__alloc_1gade7bbd62a937c3b2ed2f32c34c6c60a6"></a>
### Function TestCell

![][private]

```cpp
int TestCell(Page *page, size_t index)
```

Test the page bitmap for a given cell.

**Returns**:

Whether the cell is set.



**Parameters**:

* [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) * **page**: The page.
* size_t **index**: Index of cell.

**Return type**: int

**References**:

* [PAGE\_BITMASK](col_internal_8h.md#group__pages__cells_1ga00782a3620010f5d871ea505e6ca3abe)

**Referenced by**:

* [Col\_WordRelease](col_word_8h.md#group__words_1gad93112f81ce6511d6d0ece0db4d38598)
* [MarkReachableCellsFromParents](col_gc_8c.md#group__gc_1ga49eb8981c888c90530906952e2869000)
* [MarkReachableCellsFromRoots](col_gc_8c.md#group__gc_1ga246db5ae65938d3efa2b04b2b5cb2021)
* [MarkWord](col_gc_8c.md#group__gc_1gaf54093bb37e6a4aaaf718fb1a791d56c)
* [PoolFreeEmptyPages](col_alloc_8c.md#group__alloc_1ga0c80585c5110f75f1bf723c9b93df073)
* [PurgeParents](col_gc_8c.md#group__gc_1gae851dc24a0065a16f7149b2f10147b52)
* [SweepUnreachableCells](col_gc_8c.md#group__gc_1ga911d254f51c4e0b4475330147fbbc545)
* [UpdateParents](col_alloc_8c.md#group__gc__parents_1gaa3d85dc993fb1b9831f82c25b8c07d3c)

<a id="group__alloc_1ga356ad642b33dca80bcf2865b1f93c039"></a>
### Function NbSetCells

![][private]

```cpp
size_t NbSetCells(Page *page)
```

Get the number of cells set in a page.

**Returns**:

Number of set cells.



**Parameters**:

* [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) * **page**: The page.

**Return type**: size_t

**References**:

* [CELLS\_PER\_PAGE](col_conf_8h.md#group__alloc_1gae8f0d88b8c73d3fb1bc64a0e2ef68faa)
* [nbBitsSet](col_alloc_8c.md#group__alloc_1ga8c45e7d296e78baadc741c63954c38cc)
* [PAGE\_BITMASK](col_internal_8h.md#group__pages__cells_1ga00782a3620010f5d871ea505e6ca3abe)

**Referenced by**:

* [PoolFreeEmptyPages](col_alloc_8c.md#group__alloc_1ga0c80585c5110f75f1bf723c9b93df073)

<a id="group__alloc_1gaeec69115deeb3321bdfbb4e42119f806"></a>
### Function AllocCells

![][private]

```cpp
Cell * AllocCells(size_t number)
```

Allocate cells in the eden pool.

**pre**\
Must be called within a GC-protected section.


**Return values**:

* **pointer**: to the first allocated cell if successful.
* **NULL**: otherwise.



**See also**: [PoolAllocCells](col_alloc_8c.md#group__alloc_1gafd84f35bab195e5e45a3338903dbd837)

**Exceptions**:

* **[COL\_ERROR\_GCPROTECT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a33f3b4f6762491c50375359e5ffa02f8)**: [[E]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9eae8345daddd8d5e83225f9f88d302f1a0) Outside of a GC-protected section.

**Parameters**:

* size_t **number**: Number of cells to allocate.

**Return type**: [Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) *

**References**:

* [ThreadData::eden](struct_thread_data.md#struct_thread_data_1ac4e14d59d12e49f808e8631fea374297)
* [PlatGetThreadData](col_unix_platform_8h.md#group__arch__unix_1ga6964b3c4d4787a9defb7aae57825d92c)
* [PoolAllocCells](col_alloc_8c.md#group__alloc_1gafd84f35bab195e5e45a3338903dbd837)
* [PRECONDITION\_GCPROTECTED](col_gc_8c.md#group__gc_1ga1ce49cbb2ef788b5e3d501b57b903289)

**Referenced by**:

* [AddSynonymField](col_word_8c.md#group__words_1ga058a6e96bd5370d04936b59a3d3c48c9)
* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [Col\_ConcatRopes](col_rope_8h.md#group__rope__words_1gaafab3ef159c0b11402cc50c91fc59700)
* [Col\_CopyHashMap](col_hash_8h.md#group__hashmap__words_1ga9ebda3b577662e8b1dcf9a227d106f22)
* [Col\_CopyTrieMap](col_trie_8h.md#group__triemap__words_1ga34e494c0bafde72774a578643bb84a68)
* [Col\_NewCustomHashMap](col_hash_8h.md#group__customhashmap__words_1gad516fa9041eb514e2c5193eb5d958f0e)
* [Col\_NewCustomTrieMap](col_trie_8h.md#group__customtriemap__words_1ga18de761037e23e723d8d62aef7d6246c)
* [Col\_NewCustomWord](col_word_8h.md#group__custom__words_1gaf9a6d324967159ae7abeb41a3d59cc79)
* [Col\_NewFloatWord](col_word_8h.md#group__words_1gab8a1c82145210cc626b90a3c8dc3b4b7)
* [Col\_NewIntHashMap](col_hash_8h.md#group__hashmap__words_1ga21868cc2f614fe73e31690d5d233e0c9)
* [Col\_NewIntTrieMap](col_trie_8h.md#group__triemap__words_1ga774d1c17ace439ef92703934652ccec0)
* [Col\_NewIntWord](col_word_8h.md#group__words_1gaba67c33e1004d5db691cb5834b77645e)
* [Col\_NewMList](col_list_8h.md#group__mlist__words_1ga3b048f22f88eb07685a0d6e12960ca91)
* [Col\_NewMVector](col_vector_8h.md#group__mvector__words_1ga5409a9871105f346b35ecd06d857e271)
* [Col\_NewRope](col_rope_8h.md#group__rope__words_1gadf89e360729ba5052887cd4897b0167f)
* [Col\_NewStringBuffer](col_str_buf_8h.md#group__strbuf__words_1ga8c3ba7df2adb643c5da323d7fd013cfb)
* [Col\_NewStringHashMap](col_hash_8h.md#group__hashmap__words_1ga83815df8c509dbf24974ed447ed5ad75)
* [Col\_NewStringTrieMap](col_trie_8h.md#group__triemap__words_1ga41fcb3cc5b729930b9a9c405f4fabc25)
* [Col\_NewVector](col_vector_8h.md#group__vector__words_1ga6ef7d35d75fdc6a6781f0a32e9c7efc1)
* [Col\_NewVectorNV](col_vector_8h.md#group__vector__words_1gaa56f743590ca8867765f48e31e8a4df9)
* [Col\_NormalizeRope](col_rope_8h.md#group__rope__words_1gad8e0ed73e9d579e9aac9bd5ee7603319)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [Col\_Subrope](col_rope_8h.md#group__rope__words_1ga688a99f26c500c1f65f4141e97de0335)
* [ConvertEntryToMutable](col_hash_8c.md#group__hashmap__words_1ga12e2f90ca22da78efd1d3e42d442a3e3)
* [ConvertIntEntryToMutable](col_hash_8c.md#group__hashmap__words_1gace9fa4c1e18e28528ecbd57dc858cdda)
* [ConvertIntNodeToMutable](col_trie_8c.md#group__triemap__words_1gaaa5e678151930128e95296f7d2abf23c)
* [ConvertNodeToMutable](col_trie_8c.md#group__triemap__words_1ga875d493095b92897563a377cf810aaa2)
* [ConvertStringNodeToMutable](col_trie_8c.md#group__triemap__words_1ga99dfbb25b22d51753f941a244aad50b1)
* [HashMapFindEntry](col_hash_8c.md#group__hashmap__words_1ga0c88b84075dfbde40bfc894ea158bdc8)
* [IntHashMapFindEntry](col_hash_8c.md#group__hashmap__words_1ga1ddc6ccf9196e70ad14895d5bc32ad2d)
* [IntTrieMapFindEntry](col_trie_8c.md#group__triemap__words_1ga15570e9b8e1e48b2ea26b6ea369a95fb)
* [MergeListChunksProc](col_list_8c.md#group__list__words_1ga2daef27844161dd9c5b16f571bb2e01a)
* [NewMConcatList](col_list_8c.md#group__mlist__words_1ga0cf94fe0b5f417772755b0bd02851e5a)
* [StringTrieMapFindEntry](col_trie_8c.md#group__triemap__words_1ga22228c8cc05e205425dd3f6bba64c759)
* [TrieMapFindEntry](col_trie_8c.md#group__triemap__words_1ga927988459b3b6759f775b0c4af8f3c5e)

## Functions

<a id="group__alloc_1ga40aaa69883691c7f291a07375ece72ec"></a>
### Function PageAllocCells

![][private]
![][static]

```cpp
static Cell* PageAllocCells(size_t number, Cell *firstCell)
```

Allocate cells in existing pages.

Traverse and search all existing pages for a free cell sequence of the given length, and if found, set cells.






**Return values**:

* **pointer**: to the first allocated cell if successful
* **NULL**: otherwise



**See also**: [FindFreeCells](col_alloc_8c.md#group__alloc_1ga36d583de9ab3c9da67c3da5bef92d135)



**Parameters**:

* size_t **number**: Number of cells to allocate.
* [Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) * **firstCell**: First cell to consider.

**Return type**: [Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) *

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [CELL\_INDEX](col_internal_8h.md#group__pages__cells_1gaa6e93c045bc319412f36118ea1cfbb05)
* [CELL\_PAGE](col_internal_8h.md#group__pages__cells_1gabe4fc1fd7a45bf2858948e3a06710a2b)
* [FindFreeCells](col_alloc_8c.md#group__alloc_1ga36d583de9ab3c9da67c3da5bef92d135)
* [AddressRange::first](struct_address_range.md#struct_address_range_1aa8ee3a687b3f1fc91257feb0bd8704cf)
* [PAGE\_NEXT](col_internal_8h.md#group__pages__cells_1ga0ed08f16e66e86cbed58b86203c7fd38)
* [SetCells](col_alloc_8c.md#group__alloc_1ga9eee2c912dd9c4dc461b8fa760204b5b)

**Referenced by**:

* [PoolAllocCells](col_alloc_8c.md#group__alloc_1gafd84f35bab195e5e45a3338903dbd837)

<a id="group__alloc_1ga36d583de9ab3c9da67c3da5bef92d135"></a>
### Function FindFreeCells

![][private]
![][static]

```cpp
static size_t FindFreeCells(void *page, size_t number, size_t index)
```

Find sequence of free cells in page.

**Return values**:

* **index**: of first cell of sequence if found
* **-1**: if none found



**Parameters**:

* void * **page**: Page to look into.
* size_t **number**: Number of cells to look for.
* size_t **index**: First cell to consider.

**Return type**: size_t

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [AVAILABLE\_CELLS](col_internal_8h.md#group__pages__cells_1ga524e5a52183dcc7088644df29ef766bf)
* [CELLS\_PER\_PAGE](col_conf_8h.md#group__alloc_1gae8f0d88b8c73d3fb1bc64a0e2ef68faa)
* [AddressRange::first](struct_address_range.md#struct_address_range_1aa8ee3a687b3f1fc91257feb0bd8704cf)
* [firstZeroBitSequence](col_alloc_8c.md#group__alloc_1gaee6b78438aa7f935582f7eefb28e63bf)
* [longestLeadingZeroBitSequence](col_alloc_8c.md#group__alloc_1gab2440050425db6a793ac535b9c4ef432)
* [PAGE\_BITMASK](col_internal_8h.md#group__pages__cells_1ga00782a3620010f5d871ea505e6ca3abe)

**Referenced by**:

* [PageAllocCells](col_alloc_8c.md#group__alloc_1ga40aaa69883691c7f291a07375ece72ec)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)