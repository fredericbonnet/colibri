<a id="struct_rope_chunk_traverse_info_8backtracks"></a>
# Structure RopeChunkTraverseInfo.backtracks

![][C++]
![][public]

**Definition**: `src/colRope.c` (line 2999)

Pre-allocated backtracking structure.



## Members

* [max](struct_rope_chunk_traverse_info_8backtracks.md#struct_rope_chunk_traverse_info_8backtracks_1a2ffe4e77325d9a7152f7086ea7aa5114)
* [prevDepth](struct_rope_chunk_traverse_info_8backtracks.md#struct_rope_chunk_traverse_info_8backtracks_1a3d8509eeae39bace4647731037ded04f)
* [rope](struct_rope_chunk_traverse_info_8backtracks.md#struct_rope_chunk_traverse_info_8backtracks_1a379e86ff8a80f711680f9fb5d4b9560e)

## Public attributes

<a id="struct_rope_chunk_traverse_info_8backtracks_1a3d8509eeae39bace4647731037ded04f"></a>
### Variable prevDepth

![][public]

**Definition**: `src/colRope.c` (line 3000)

```cpp

```

Depth of next concat node for backtracking.





**Type**: int

<a id="struct_rope_chunk_traverse_info_8backtracks_1a379e86ff8a80f711680f9fb5d4b9560e"></a>
### Variable rope

![][public]

**Definition**: `src/colRope.c` (line 3001)

```cpp

```

Rope.





**Type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="struct_rope_chunk_traverse_info_8backtracks_1a2ffe4e77325d9a7152f7086ea7aa5114"></a>
### Variable max

![][public]

**Definition**: `src/colRope.c` (line 3002)

```cpp

```

Max number of characters traversed in **rope**.





**Type**: size_t

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)