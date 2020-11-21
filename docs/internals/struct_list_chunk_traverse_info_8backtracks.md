<a id="struct_list_chunk_traverse_info_8backtracks"></a>
# Structure ListChunkTraverseInfo.backtracks

![][C++]
![][public]

**Definition**: `src/colList.c` (line 1691)

Pre-allocated backtracking structure.



## Members

* [list](struct_list_chunk_traverse_info_8backtracks.md#struct_list_chunk_traverse_info_8backtracks_1a10ae9fc7d453b0dd525d0edf2ede7961)
* [max](struct_list_chunk_traverse_info_8backtracks.md#struct_list_chunk_traverse_info_8backtracks_1a2ffe4e77325d9a7152f7086ea7aa5114)
* [prevDepth](struct_list_chunk_traverse_info_8backtracks.md#struct_list_chunk_traverse_info_8backtracks_1a3d8509eeae39bace4647731037ded04f)

## Public attributes

<a id="struct_list_chunk_traverse_info_8backtracks_1a3d8509eeae39bace4647731037ded04f"></a>
### Variable prevDepth

![][public]

**Definition**: `src/colList.c` (line 1692)

```cpp

```

Depth of next concat node for backtracking.





**Type**: int

<a id="struct_list_chunk_traverse_info_8backtracks_1a10ae9fc7d453b0dd525d0edf2ede7961"></a>
### Variable list

![][public]

**Definition**: `src/colList.c` (line 1693)

```cpp

```

List.





**Type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="struct_list_chunk_traverse_info_8backtracks_1a2ffe4e77325d9a7152f7086ea7aa5114"></a>
### Variable max

![][public]

**Definition**: `src/colList.c` (line 1694)

```cpp

```

Max number of elements traversed in **list**.





**Type**: size_t

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)