<a id="group__pages__cells"></a>
# Pages and Cells



Pages are divided into cells. On a 32-bit system with 1024-byte logical pages, each page stores 64 16-byte cells. On a 64-bit system with 4096-byte logical pages, each page stores 128 32-byte cells.





Each page has reserved cells that store information about the page. The remaining cells store word info.






**First Cell Layout**:

On all architectures, the first cell is reserved and is formatted as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        first_cell [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="20" align="left">0</td><td sides="B" width="20" align="right">3</td>
                <td sides="B" width="20" align="left">4</td><td sides="B" width="20" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref PAGE_GENERATION" title="PAGE_GENERATION" colspan="2">Gen</td>
                <td href="@ref PAGE_FLAGS" title="PAGE_FLAGS" colspan="2">Flags</td>
                <td href="@ref PAGE_NEXT" title="PAGE_NEXT" colspan="2">Next</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref PAGE_GROUPDATA" title="PAGE_GROUPDATA" colspan="6">Group Data</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref PAGE_BITMASK" title="PAGE_BITMASK" colspan="6" rowspan="2">Bitmask</td>
            </tr>
            <tr><td sides="R">3</td></tr>
            </table>
        >]
    }

## Page and Cell Types & Constants

<a id="group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed"></a>
### Typedef Page

![][private]

**Definition**: `src/colInternal.h` (line 206)

```cpp
typedef char Page[PAGE_SIZE]
```

Page-sized byte array.

**See also**: [PAGE\_SIZE](col_conf_8h.md#group__alloc_1ga7d467c1d283fdfa1f2081ba1e0d01b6e)



**Return type**: char

<a id="group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f"></a>
### Typedef Cell

![][private]

**Definition**: `src/colInternal.h` (line 213)

```cpp
typedef char Cell[CELL_SIZE]
```

Cell-sized byte array.

**See also**: [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)



**Return type**: char

<a id="group__pages__cells_1ga2e2387471157b525133bb3d9ddc02bde"></a>
### Macro RESERVED\_CELLS

![][public]

```cpp
#define RESERVED_CELLS 1
```

Number of reserved cells in page.

**See also**: [AVAILABLE\_CELLS](col_internal_8h.md#group__pages__cells_1ga524e5a52183dcc7088644df29ef766bf)



<a id="group__pages__cells_1ga524e5a52183dcc7088644df29ef766bf"></a>
### Macro AVAILABLE\_CELLS

![][public]

```cpp
#define AVAILABLE_CELLS ([CELLS\_PER\_PAGE](col_conf_8h.md#group__alloc_1gae8f0d88b8c73d3fb1bc64a0e2ef68faa)-[RESERVED\_CELLS](col_internal_8h.md#group__pages__cells_1ga2e2387471157b525133bb3d9ddc02bde))
```

Number of available cells in page, i.e. number of cells in page minus number of reserved cells.

**See also**: [CELLS\_PER\_PAGE](col_conf_8h.md#group__alloc_1gae8f0d88b8c73d3fb1bc64a0e2ef68faa), [RESERVED\_CELLS](col_internal_8h.md#group__pages__cells_1ga2e2387471157b525133bb3d9ddc02bde)



<a id="group__pages__cells_1ga6969cfc3c9b2913a913df84f7842ce74"></a>
### Macro NB\_CELLS

![][public]

```cpp
#define NB_CELLS     (((size)+[CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)-1)/[CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f))( size )
```

Number of cells needed to store a given number of bytes.

**Parameters**:

* **size**: Number of raw bytes to store.



**See also**: [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)



<a id="group__pages__cells_1gace2cf8cd83ae1a8cc646bbd367cd5a86"></a>
### Macro PAGE\_FLAG\_FIRST

![][public]

```cpp
#define PAGE_FLAG_FIRST 0x10
```

PAGE_FLAG_FIRST Marks first page in group.

**See also**: [PAGE\_FLAG](col_internal_8h.md#group__pages__cells_1ga3ccf282a118bd7b9101e38f4228e8853), [PAGE\_SET\_FLAG](col_internal_8h.md#group__pages__cells_1ga6bfd878f32448ca9487ecfedfabdaed6), [PAGE\_CLEAR\_FLAG](col_internal_8h.md#group__pages__cells_1gad96556885fb99327d9e9c677043584f5)



<a id="group__pages__cells_1ga011eeac135e2e667ae1356b0abf9c727"></a>
### Macro PAGE\_FLAG\_LAST

![][public]

```cpp
#define PAGE_FLAG_LAST 0x20
```

Marks last page in group.

**See also**: [PAGE\_FLAG](col_internal_8h.md#group__pages__cells_1ga3ccf282a118bd7b9101e38f4228e8853), [PAGE\_SET\_FLAG](col_internal_8h.md#group__pages__cells_1ga6bfd878f32448ca9487ecfedfabdaed6), [PAGE\_CLEAR\_FLAG](col_internal_8h.md#group__pages__cells_1gad96556885fb99327d9e9c677043584f5)



<a id="group__pages__cells_1gae404cde02792c18aea29d89cd7017e80"></a>
### Macro PAGE\_FLAG\_PARENT

![][public]

```cpp
#define PAGE_FLAG_PARENT 0x40
```

Marks pages having parent cells.

**See also**: [PAGE\_FLAG](col_internal_8h.md#group__pages__cells_1ga3ccf282a118bd7b9101e38f4228e8853), [PAGE\_SET\_FLAG](col_internal_8h.md#group__pages__cells_1ga6bfd878f32448ca9487ecfedfabdaed6), [PAGE\_CLEAR\_FLAG](col_internal_8h.md#group__pages__cells_1gad96556885fb99327d9e9c677043584f5)



## Page and Cell Accessors

<a id="group__pages__cells_1ga3d14770b4646c4481b42169342e807e4"></a>
### Macro PAGE\_NEXT\_MASK

![][public]

```cpp
#define PAGE_NEXT_MASK (~([PAGE\_GENERATION\_MASK](col_internal_8h.md#group__pages__cells_1gaaf0eb5eed9f520b4828c5c11401e8d08)|[PAGE\_FLAGS\_MASK](col_internal_8h.md#group__pages__cells_1gaf5f12024480e802312d785501f55b443)))
```

Bitmask for next page.





<a id="group__pages__cells_1gaaf0eb5eed9f520b4828c5c11401e8d08"></a>
### Macro PAGE\_GENERATION\_MASK

![][public]

```cpp
#define PAGE_GENERATION_MASK 0x0F
```

Bitmask for generation.





<a id="group__pages__cells_1gaf5f12024480e802312d785501f55b443"></a>
### Macro PAGE\_FLAGS\_MASK

![][public]

```cpp
#define PAGE_FLAGS_MASK 0xF0
```

Bitmask for flags.





<a id="group__pages__cells_1ga0ed08f16e66e86cbed58b86203c7fd38"></a>
### Macro PAGE\_NEXT

![][public]

```cpp
#define PAGE_NEXT (([Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) *)((*(uintptr_t *)(page)) & [PAGE\_NEXT\_MASK](col_internal_8h.md#group__pages__cells_1ga3d14770b4646c4481b42169342e807e4)))( page )
```

Get next page in pool.

**Parameters**:

* **page**: [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) to access.


**Returns**:

Next page in pool.



**See also**: [PAGE\_SET\_NEXT](col_internal_8h.md#group__pages__cells_1ga8ecae0f26c64c6971edbe344eb117665)



<a id="group__pages__cells_1ga8ecae0f26c64c6971edbe344eb117665"></a>
### Macro PAGE\_SET\_NEXT

![][public]

```cpp
#define PAGE_SET_NEXT (*(uintptr_t *)(page) &= ~[PAGE\_NEXT\_MASK](col_internal_8h.md#group__pages__cells_1ga3d14770b4646c4481b42169342e807e4), *(uintptr_t *)(page) |= ((uintptr_t)(next)) & [PAGE\_NEXT\_MASK](col_internal_8h.md#group__pages__cells_1ga3d14770b4646c4481b42169342e807e4))( page ,next )
```

Set next page in pool.

**Parameters**:

* **page**: [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) to access.
* **next**: Next [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) to link to.



**See also**: [PAGE\_NEXT](col_internal_8h.md#group__pages__cells_1ga0ed08f16e66e86cbed58b86203c7fd38)



<a id="group__pages__cells_1gaa7215b15e159db56ed2bc30d66488fd8"></a>
### Macro PAGE\_GENERATION

![][public]

```cpp
#define PAGE_GENERATION ((*(uintptr_t *)(page)) & [PAGE\_GENERATION\_MASK](col_internal_8h.md#group__pages__cells_1gaaf0eb5eed9f520b4828c5c11401e8d08))( page )
```

Get generation of pool containing this page.

**Parameters**:

* **page**: [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) to access.


**Returns**:

The page generation.



**See also**: [PAGE\_SET\_GENERATION](col_internal_8h.md#group__pages__cells_1ga29860aee271faab1c7c7c4b16d8a3da1)



<a id="group__pages__cells_1ga29860aee271faab1c7c7c4b16d8a3da1"></a>
### Macro PAGE\_SET\_GENERATION

![][public]

```cpp
#define PAGE_SET_GENERATION (*(uintptr_t *)(page) &= ~[PAGE\_GENERATION\_MASK](col_internal_8h.md#group__pages__cells_1gaaf0eb5eed9f520b4828c5c11401e8d08), *(uintptr_t *)(page) |= (gen) & [PAGE\_GENERATION\_MASK](col_internal_8h.md#group__pages__cells_1gaaf0eb5eed9f520b4828c5c11401e8d08))( page ,gen )
```

Set generation of pool containing this page.

**Parameters**:

* **page**: [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) to access.
* **gen**: Generation.



**See also**: [PAGE\_GENERATION](col_internal_8h.md#group__pages__cells_1gaa7215b15e159db56ed2bc30d66488fd8)



<a id="group__pages__cells_1gae3787089618c0a4f84e4cb8db4290b66"></a>
### Macro PAGE\_FLAGS

![][public]

```cpp
#define PAGE_FLAGS ((*(uintptr_t *)(page)) & [PAGE\_FLAGS\_MASK](col_internal_8h.md#group__pages__cells_1gaf5f12024480e802312d785501f55b443))( page )
```

Get page flags.

**Parameters**:

* **page**: [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) to access.





**See also**: [PAGE\_FLAG](col_internal_8h.md#group__pages__cells_1ga3ccf282a118bd7b9101e38f4228e8853), [PAGE\_SET\_FLAG](col_internal_8h.md#group__pages__cells_1ga6bfd878f32448ca9487ecfedfabdaed6), [PAGE\_CLEAR\_FLAG](col_internal_8h.md#group__pages__cells_1gad96556885fb99327d9e9c677043584f5)



<a id="group__pages__cells_1ga3ccf282a118bd7b9101e38f4228e8853"></a>
### Macro PAGE\_FLAG

![][public]

```cpp
#define PAGE_FLAG ((*(uintptr_t *)(page)) & (flag))( page ,flag )
```

Get page flag.

**Parameters**:

* **page**: [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) to access.
* **flag**: Flag to get.



<a id="group__pages__cells_1ga6bfd878f32448ca9487ecfedfabdaed6"></a>
### Macro PAGE\_SET\_FLAG

![][public]

```cpp
#define PAGE_SET_FLAG ((*(uintptr_t *)(page)) |= (flag))( page ,flag )
```

Set page flag.

**Parameters**:

* **page**: [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) to access.
* **flag**: Flag to set.



**See also**: [PAGE\_FLAG](col_internal_8h.md#group__pages__cells_1ga3ccf282a118bd7b9101e38f4228e8853)



<a id="group__pages__cells_1gad96556885fb99327d9e9c677043584f5"></a>
### Macro PAGE\_CLEAR\_FLAG

![][public]

```cpp
#define PAGE_CLEAR_FLAG ((*(uintptr_t *)(page)) &= ~(flag))( page ,flag )
```

Clear page flag.

**Parameters**:

* **page**: [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) to access.
* **flag**: Flag to clear.




**See also**: [PAGE\_FLAG](col_internal_8h.md#group__pages__cells_1ga3ccf282a118bd7b9101e38f4228e8853), [PAGE\_SET\_FLAG](col_internal_8h.md#group__pages__cells_1ga6bfd878f32448ca9487ecfedfabdaed6)



<a id="group__pages__cells_1gacb4ea8da3119e9c387474afde87569dc"></a>
### Macro PAGE\_GROUPDATA

![][public]

```cpp
#define PAGE_GROUPDATA (*(([GroupData](struct_group_data.md#struct_group_data) **)(page)+1))( page )
```

Get/set data for the thread group the page belongs to.

**Parameters**:

* **page**: [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) to access.


?> Macro is L-Value and suitable for both read/write operations.



<a id="group__pages__cells_1ga00782a3620010f5d871ea505e6ca3abe"></a>
### Macro PAGE\_BITMASK

![][public]

```cpp
#define PAGE_BITMASK ((uint8_t *)(page)+sizeof([Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) *)*2)( page )
```

Get/set bitmask for allocated cells in page.

**Parameters**:

* **page**: [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) to access.


?> Macro is L-Value and suitable for both read/write operations.



<a id="group__pages__cells_1ga2fb20c83455d53df390ae692b500cb5f"></a>
### Macro PAGE\_CELL

![][public]

```cpp
#define PAGE_CELL     (([Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) *)(page)+(index))( page ,index )
```

Get **index**-th cell in page.

**Parameters**:

* **page**: [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) containing cell.
* **index**: Index of cell in **page**.


**Returns**:

**index**-th [Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) in page.



**See also**: [Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f)



<a id="group__pages__cells_1gabe4fc1fd7a45bf2858948e3a06710a2b"></a>
### Macro CELL\_PAGE

![][public]

```cpp
#define CELL_PAGE     (([Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) *)((uintptr_t)(cell) & ~([PAGE\_SIZE](col_conf_8h.md#group__alloc_1ga7d467c1d283fdfa1f2081ba1e0d01b6e)-1)))( cell )
```

Get page containing the cell.

**Parameters**:

* **cell**: Cell to get page for.


**Returns**:

[Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed) containing **cell**.




**See also**: [Page](col_internal_8h.md#group__pages__cells_1ga876f63acb28a01cd6d79f2a23b5a9bed), [PAGE\_SIZE](col_conf_8h.md#group__alloc_1ga7d467c1d283fdfa1f2081ba1e0d01b6e)



<a id="group__pages__cells_1gaa6e93c045bc319412f36118ea1cfbb05"></a>
### Macro CELL\_INDEX

![][public]

```cpp
#define CELL_INDEX     (((uintptr_t)(cell) % [PAGE\_SIZE](col_conf_8h.md#group__alloc_1ga7d467c1d283fdfa1f2081ba1e0d01b6e)) / [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f))( cell )
```

Get index of cell in page.

**Parameters**:

* **cell**: Cell to get index for.


**Returns**:

Index of **cell** in its containing page.




**See also**: [PAGE\_SIZE](col_conf_8h.md#group__alloc_1ga7d467c1d283fdfa1f2081ba1e0d01b6e), [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)