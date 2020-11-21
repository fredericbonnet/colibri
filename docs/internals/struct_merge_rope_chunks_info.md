<a id="struct_merge_rope_chunks_info"></a>
# Structure MergeRopeChunksInfo

![][C++]
![][private]

**Definition**: `src/colRope.c` (line 1996)

Structure used to collect data during the traversal of ropes when merged into one string.

**See also**: [MergeRopeChunksProc](col_rope_8c.md#group__rope__words_1ga33ffefe10088e23594b6f76a1d65e184)

## Members

* [byteLength](struct_merge_rope_chunks_info.md#struct_merge_rope_chunks_info_1a889f1160b5a99df78a69be735160ebc0)
* [data](struct_merge_rope_chunks_info.md#struct_merge_rope_chunks_info_1a6076ccd35f62ed7fc5331cee6e250f77)
* [format](struct_merge_rope_chunks_info.md#struct_merge_rope_chunks_info_1a625645c02156390018fde6af588ec907)
* [length](struct_merge_rope_chunks_info.md#struct_merge_rope_chunks_info_1ac2003de1c200f18c3bf5ab9a1bd12bd4)

## Public attributes

<a id="struct_merge_rope_chunks_info_1a625645c02156390018fde6af588ec907"></a>
### Variable format

![][public]

**Definition**: `src/colRope.c` (line 1997)

```cpp
Col_StringFormat MergeRopeChunksInfo::format
```

Character format in buffer.





**Type**: [Col\_StringFormat](colibri_8h.md#group__strings_1ga125054104f6260ea3902e6e46ebfdfa0)

**Referenced by**:

* [MergeRopeChunksProc](col_rope_8c.md#group__rope__words_1ga33ffefe10088e23594b6f76a1d65e184)
* [Col\_Subrope](col_rope_8h.md#group__rope__words_1ga688a99f26c500c1f65f4141e97de0335)
* [Col\_ConcatRopes](col_rope_8h.md#group__rope__words_1gaafab3ef159c0b11402cc50c91fc59700)

<a id="struct_merge_rope_chunks_info_1a6076ccd35f62ed7fc5331cee6e250f77"></a>
### Variable data

![][public]

**Definition**: `src/colRope.c` (line 1998)

```cpp
char MergeRopeChunksInfo::data[MAX_SHORT_LEAF_SIZE][MAX_SHORT_LEAF_SIZE]
```

Buffer storing the flattened data.





**Type**: char

**Referenced by**:

* [MergeRopeChunksProc](col_rope_8c.md#group__rope__words_1ga33ffefe10088e23594b6f76a1d65e184)
* [Col\_Subrope](col_rope_8h.md#group__rope__words_1ga688a99f26c500c1f65f4141e97de0335)
* [Col\_ConcatRopes](col_rope_8h.md#group__rope__words_1gaafab3ef159c0b11402cc50c91fc59700)

<a id="struct_merge_rope_chunks_info_1ac2003de1c200f18c3bf5ab9a1bd12bd4"></a>
### Variable length

![][public]

**Definition**: `src/colRope.c` (line 1999)

```cpp
size_t MergeRopeChunksInfo::length
```

Character length so far.





**Type**: size_t

**Referenced by**:

* [MergeRopeChunksProc](col_rope_8c.md#group__rope__words_1ga33ffefe10088e23594b6f76a1d65e184)
* [Col\_Subrope](col_rope_8h.md#group__rope__words_1ga688a99f26c500c1f65f4141e97de0335)
* [Col\_ConcatRopes](col_rope_8h.md#group__rope__words_1gaafab3ef159c0b11402cc50c91fc59700)

<a id="struct_merge_rope_chunks_info_1a889f1160b5a99df78a69be735160ebc0"></a>
### Variable byteLength

![][public]

**Definition**: `src/colRope.c` (line 2000)

```cpp
size_t MergeRopeChunksInfo::byteLength
```

Byte length so far.





**Type**: size_t

**Referenced by**:

* [MergeRopeChunksProc](col_rope_8c.md#group__rope__words_1ga33ffefe10088e23594b6f76a1d65e184)
* [Col\_Subrope](col_rope_8h.md#group__rope__words_1ga688a99f26c500c1f65f4141e97de0335)
* [Col\_ConcatRopes](col_rope_8h.md#group__rope__words_1gaafab3ef159c0b11402cc50c91fc59700)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)