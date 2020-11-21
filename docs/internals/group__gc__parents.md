<a id="group__gc__parents"></a>
# Parents



Parents are cells pointing to other cells of a newer generation. During GC, parents from uncollected generations are traversed in addition to roots from collected generations, and the parent list is updated.





When a page of an older generation is written, this means that it may contain parents. Such "dirty" pages are added to the parent list at each GC, and each page is checked for potential parents.






**Requirements**:


* Parent cells use one single cell.

* Parent cells are linked together for traversal.

* Parent cells don't point to parent words directly but instead point to the parent's page.


**Parameters**:

* **Next**: Parent nodes are linked together in a singly-linked list for traversal during GC.
* **Page**: Pointer to source page.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        parent_cell [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="160" align="left">0</td><td sides="B" width="160" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref PARENT_NEXT" title="PARENT_NEXT" colspan="2">Next</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref PARENT_PAGE" title="PARENT_PAGE" colspan="2">Page</td>
            </tr>
            <tr><td sides="R">2</td>
                <td colspan="2" rowspan="2" bgcolor="grey75">Unused</td>
            </tr>
            <tr><td sides="R">3</td></tr>
            </table>
        >]
    }

## Parent Cell Creation

<a id="group__gc__parents_1gac7a1ff3c395ce8cce66913868d130367"></a>
### Macro PARENT\_INIT

![][public]

```cpp
#define PARENT_INIT     [PARENT\_NEXT](col_internal_8h.md#group__gc__parents_1gafe78ceffb7ad8783c023adf478a84bf4)(cell) = next; \
    [PARENT\_PAGE](col_internal_8h.md#group__gc__parents_1gafeb8092a81b91dcf8383c6a488dfae2a)(cell) = page;( cell ,next ,page )
```

Parent cell initializer.

**Parameters**:

* **cell**: Cell to initialize.
* **next**: [PARENT\_NEXT](col_internal_8h.md#group__gc__parents_1gafe78ceffb7ad8783c023adf478a84bf4).
* **page**: [PARENT\_PAGE](col_internal_8h.md#group__gc__parents_1gafeb8092a81b91dcf8383c6a488dfae2a).


!> **Warning** \
Argument **cell** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [MarkReachableCellsFromParents](col_gc_8c.md#group__gc_1ga49eb8981c888c90530906952e2869000)



## Parent Cell Accessors

<a id="group__gc__parents_1gafe78ceffb7ad8783c023adf478a84bf4"></a>
### Macro PARENT\_NEXT

![][public]

```cpp
#define PARENT_NEXT ((([Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) **)(cell))[1])( cell )
```

Parent nodes are linked together in a singly-linked list for traversal during GC.

**Parameters**:

* **cell**: Cell to access.



**See also**: [PARENT\_INIT](col_internal_8h.md#group__gc__parents_1gac7a1ff3c395ce8cce66913868d130367)



<a id="group__gc__parents_1gafeb8092a81b91dcf8383c6a488dfae2a"></a>
### Macro PARENT\_PAGE

![][public]

```cpp
#define PARENT_PAGE ((([Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) **)(cell))[2])( cell )
```

Pointer to source page.

**Parameters**:

* **cell**: Cell to access.



**See also**: [PARENT\_INIT](col_internal_8h.md#group__gc__parents_1gac7a1ff3c395ce8cce66913868d130367)



## Functions

<a id="group__gc__parents_1gaa3d85dc993fb1b9831f82c25b8c07d3c"></a>
### Function UpdateParents

![][private]

```cpp
void UpdateParents(GroupData *data)
```

Add pages written since the last GC to parent tracking structures.

Then each page's parent flag is cleared for the mark phase.



**Parameters**:

* [GroupData](struct_group_data.md#struct_group_data) * **data**: Group-specific data.

**Return type**: void

**References**:

* [AddressRange::allocInfo](struct_address_range.md#struct_address_range_1ae789f37dab3d981ccf15a020993bfd2e)
* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [AddressRange::base](struct_address_range.md#struct_address_range_1a08fc12a83b7988aa0cb8fb45c1736076)
* [CELL\_INDEX](col_internal_8h.md#group__pages__cells_1gaa6e93c045bc319412f36118ea1cfbb05)
* [CELL\_PAGE](col_internal_8h.md#group__pages__cells_1gabe4fc1fd7a45bf2858948e3a06710a2b)
* [dedicatedRanges](col_alloc_8c.md#group__alloc_1ga09e3f1c0494d23d9f93481ed4f228a4c)
* [AddressRange::next](struct_address_range.md#struct_address_range_1a29b3c1f7a257bab2afa0a4ee5c63e60d)
* [PAGE\_CLEAR\_FLAG](col_internal_8h.md#group__pages__cells_1gad96556885fb99327d9e9c677043584f5)
* [PAGE\_FLAG](col_internal_8h.md#group__pages__cells_1ga3ccf282a118bd7b9101e38f4228e8853)
* [PAGE\_FLAG\_FIRST](col_internal_8h.md#group__pages__cells_1gace2cf8cd83ae1a8cc646bbd367cd5a86)
* [PAGE\_FLAG\_LAST](col_internal_8h.md#group__pages__cells_1ga011eeac135e2e667ae1356b0abf9c727)
* [PAGE\_FLAG\_PARENT](col_internal_8h.md#group__pages__cells_1gae404cde02792c18aea29d89cd7017e80)
* [PAGE\_GROUPDATA](col_internal_8h.md#group__pages__cells_1gacb4ea8da3119e9c387474afde87569dc)
* [PAGE\_NEXT](col_internal_8h.md#group__pages__cells_1ga0ed08f16e66e86cbed58b86203c7fd38)
* [PARENT\_INIT](col_internal_8h.md#group__gc__parents_1gac7a1ff3c395ce8cce66913868d130367)
* [PARENT\_NEXT](col_internal_8h.md#group__gc__parents_1gafe78ceffb7ad8783c023adf478a84bf4)
* [PARENT\_PAGE](col_internal_8h.md#group__gc__parents_1gafeb8092a81b91dcf8383c6a488dfae2a)
* [GroupData::parents](struct_group_data.md#struct_group_data_1aa17cb04fecebc17f5f9ff130388887ef)
* [PlatEnterProtectAddressRanges](col_unix_platform_8h.md#group__arch__unix_1ga0d52de05fb3a0897f54579ab12519159)
* [PlatLeaveProtectAddressRanges](col_unix_platform_8h.md#group__arch__unix_1gab6be4d622dd8c6bc4c283a1039962e5a)
* [PoolAllocCells](col_alloc_8c.md#group__alloc_1gafd84f35bab195e5e45a3338903dbd837)
* [ranges](col_alloc_8c.md#group__alloc_1ga9ac4c516a0888195d1f2ca4721f633f8)
* [GroupData::rootPool](struct_group_data.md#struct_group_data_1a81941409b9917b41bb20d3572b2b4ec7)
* [shiftPage](col_alloc_8c.md#group__arch_1gacfb643d4c365f92c1ea93d0f1b1b71e5)
* [AddressRange::size](struct_address_range.md#struct_address_range_1a80783f530919686945d93eb7b1e25623)
* [TestCell](col_alloc_8c.md#group__alloc_1gade7bbd62a937c3b2ed2f32c34c6c60a6)

**Referenced by**:

* [PerformGC](col_gc_8c.md#group__gc_1ga5688ae9d7f658650ca8dfa66f4102f62)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)