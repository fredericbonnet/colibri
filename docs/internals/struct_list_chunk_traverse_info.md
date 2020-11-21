<a id="struct_list_chunk_traverse_info"></a>
# Structure ListChunkTraverseInfo

![][C++]
![][private]

**Definition**: `src/colList.c` (line 1689)

Structure used during recursive list chunk traversal.

This avoids recursive procedure calls thanks to a pre-allocated backtracking structure: since the algorithms only recurse on concat nodes and since we know their depth, we can allocate the needed space once for all.








**See also**: [Col\_TraverseListChunksN](col_list_8h.md#group__list__words_1ga4c20735036c60715f55cfd4e8d96825f), [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)

## Members

* [backtracks](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a144abdb4fe0550b8ba692e8b55783ba9)
* [e](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a61175342ec174b4fccce7b3842a3bc7e)
* [list](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a20400ffc5ef99dfd4ffd684870cf51f0)
* [max](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a35fa386ec4ea893f6ffe08686b25f886)
* [maxDepth](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a85956991f5f276c5c79469ff07e45d29)
* [prevDepth](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a22b5d4d14134f49bc4cdc2cf5d930e0d)
* [start](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a4a0b34787f1830365a9d6d060f464075)

## Public attributes

<a id="struct_list_chunk_traverse_info_1a144abdb4fe0550b8ba692e8b55783ba9"></a>
### Variable backtracks

![][public]

**Definition**: `src/colList.c` (line 1695)

```cpp
struct ListChunkTraverseInfo::@0 * ListChunkTraverseInfo::backtracks
```

Pre-allocated backtracking structure.





**Type**: struct ListChunkTraverseInfo::@0 *

**Referenced by**:

* [GetChunk](col_list_8c.md#group__list__words_1ga58a993e823bf134e615c862a546d6d87)
* [NextChunk](col_list_8c.md#group__list__words_1gab2a98e33d9052b36916698a4aa2e276e)
* [Col\_TraverseListChunksN](col_list_8h.md#group__list__words_1ga4c20735036c60715f55cfd4e8d96825f)
* [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)

<a id="struct_list_chunk_traverse_info_1a20400ffc5ef99dfd4ffd684870cf51f0"></a>
### Variable list

![][public]

**Definition**: `src/colList.c` (line 1697)

```cpp
Col_Word ListChunkTraverseInfo::list
```

Currently traversed list.





**Type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**Referenced by**:

* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [Col\_ListDepth](col_list_8h.md#group__list__words_1ga825ecbc9aa5b7e650e45b347d8aab253)
* [Col\_ListAt](col_list_8h.md#group__list__words_1ga7550908ac1ceae597c8283afbaa7f813)
* [GetChunk](col_list_8c.md#group__list__words_1ga58a993e823bf134e615c862a546d6d87)
* [NextChunk](col_list_8c.md#group__list__words_1gab2a98e33d9052b36916698a4aa2e276e)
* [Col\_TraverseListChunksN](col_list_8h.md#group__list__words_1ga4c20735036c60715f55cfd4e8d96825f)
* [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)

<a id="struct_list_chunk_traverse_info_1a61175342ec174b4fccce7b3842a3bc7e"></a>
### Variable e

![][public]

**Definition**: `src/colList.c` (line 1698)

```cpp
Col_Word ListChunkTraverseInfo::e
```

Element used as buffer for single element list (e.g.

element-based custom lists) .



**Type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**Referenced by**:

* [GetChunk](col_list_8c.md#group__list__words_1ga58a993e823bf134e615c862a546d6d87)

<a id="struct_list_chunk_traverse_info_1a4a0b34787f1830365a9d6d060f464075"></a>
### Variable start

![][public]

**Definition**: `src/colList.c` (line 1700)

```cpp
size_t ListChunkTraverseInfo::start
```

Index of first element traversed in list.





**Type**: size_t

**Referenced by**:

* [GetChunk](col_list_8c.md#group__list__words_1ga58a993e823bf134e615c862a546d6d87)
* [NextChunk](col_list_8c.md#group__list__words_1gab2a98e33d9052b36916698a4aa2e276e)
* [Col\_TraverseListChunksN](col_list_8h.md#group__list__words_1ga4c20735036c60715f55cfd4e8d96825f)
* [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)

<a id="struct_list_chunk_traverse_info_1a35fa386ec4ea893f6ffe08686b25f886"></a>
### Variable max

![][public]

**Definition**: `src/colList.c` (line 1701)

```cpp
size_t ListChunkTraverseInfo::max
```

Max number of elements traversed in list.





**Type**: size_t

**Referenced by**:

* [GetChunk](col_list_8c.md#group__list__words_1ga58a993e823bf134e615c862a546d6d87)
* [NextChunk](col_list_8c.md#group__list__words_1gab2a98e33d9052b36916698a4aa2e276e)
* [Col\_TraverseListChunksN](col_list_8h.md#group__list__words_1ga4c20735036c60715f55cfd4e8d96825f)
* [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)

<a id="struct_list_chunk_traverse_info_1a85956991f5f276c5c79469ff07e45d29"></a>
### Variable maxDepth

![][public]

**Definition**: `src/colList.c` (line 1702)

```cpp
int ListChunkTraverseInfo::maxDepth
```

Depth of toplevel concat node.





**Type**: int

**Referenced by**:

* [Col\_TraverseListChunksN](col_list_8h.md#group__list__words_1ga4c20735036c60715f55cfd4e8d96825f)
* [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)

<a id="struct_list_chunk_traverse_info_1a22b5d4d14134f49bc4cdc2cf5d930e0d"></a>
### Variable prevDepth

![][public]

**Definition**: `src/colList.c` (line 1703)

```cpp
int ListChunkTraverseInfo::prevDepth
```

Depth of next concat node for backtracking.





**Type**: int

**Referenced by**:

* [GetChunk](col_list_8c.md#group__list__words_1ga58a993e823bf134e615c862a546d6d87)
* [NextChunk](col_list_8c.md#group__list__words_1gab2a98e33d9052b36916698a4aa2e276e)
* [Col\_TraverseListChunksN](col_list_8h.md#group__list__words_1ga4c20735036c60715f55cfd4e8d96825f)
* [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)