<a id="struct_merge_list_chunks_info"></a>
# Structure MergeListChunksInfo

![][C++]
![][private]

**Definition**: `src/colList.c` (line 514)

Structure used to collect data during the traversal of lists when merged into one vector.

**See also**: [MergeListChunksProc](col_list_8c.md#group__list__words_1ga2daef27844161dd9c5b16f571bb2e01a)

## Members

* [length](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info_1a1dfa080c13a4eee1fce0bb2c0a2624c8)
* [vector](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info_1ae40e462380046ec92b2bb08c9172ecd6)

## Public attributes

<a id="struct_merge_list_chunks_info_1a1dfa080c13a4eee1fce0bb2c0a2624c8"></a>
### Variable length

![][public]

**Definition**: `src/colList.c` (line 515)

```cpp
size_t MergeListChunksInfo::length
```

Length so far.





**Type**: size_t

**Referenced by**:

* [MergeListChunksProc](col_list_8c.md#group__list__words_1ga2daef27844161dd9c5b16f571bb2e01a)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [ConvertToMutableAt](col_list_8c.md#group__mlist__words_1gaa3d6a25f3eb845452f465bb66f67ab51)
* [MListInsert](col_list_8c.md#group__mlist__words_1gaa62fa49dc7f988e33978dbf3e030ebf8)
* [MListRemove](col_list_8c.md#group__mlist__words_1ga7ebc7b3f5a8c87880fa31e51cde19c04)

<a id="struct_merge_list_chunks_info_1ae40e462380046ec92b2bb08c9172ecd6"></a>
### Variable vector

![][public]

**Definition**: `src/colList.c` (line 516)

```cpp
Col_Word MergeListChunksInfo::vector
```

Vector word to copy elements into.





**Type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**Referenced by**:

* [MergeListChunksProc](col_list_8c.md#group__list__words_1ga2daef27844161dd9c5b16f571bb2e01a)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [ConvertToMutableAt](col_list_8c.md#group__mlist__words_1gaa3d6a25f3eb845452f465bb66f67ab51)
* [MListInsert](col_list_8c.md#group__mlist__words_1gaa62fa49dc7f988e33978dbf3e030ebf8)
* [MListRemove](col_list_8c.md#group__mlist__words_1ga7ebc7b3f5a8c87880fa31e51cde19c04)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)