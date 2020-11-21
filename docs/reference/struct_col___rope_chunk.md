<a id="struct_col___rope_chunk"></a>
# Structure Col\_RopeChunk

![][C++]
![][public]

**Definition**: `include/colRope.h` (line 285)

Describes a rope chunk encountered during traversal.

A chunk is a contiguous portion of rope with a given format.








**See also**: [Col\_TraverseRopeChunks](col_rope_8h.md#group__rope__words_1ga1a0bffff5bb042717914fadb3e8501bb), [Col\_RopeChunksTraverseProc](col_rope_8h.md#group__rope__words_1ga8a4ef8b6e6f6aa8d863dad85c2f1b2bd)

## Members

* [byteLength](struct_col___rope_chunk.md#struct_col___rope_chunk_1a91a659379d76867d98cdd975a50ce333)
* [data](struct_col___rope_chunk.md#struct_col___rope_chunk_1a625062993f256bd911a0341dbb4c495f)
* [format](struct_col___rope_chunk.md#struct_col___rope_chunk_1ac5170cd8ca17351bbe1b3e5c70317084)

## Public attributes

<a id="struct_col___rope_chunk_1ac5170cd8ca17351bbe1b3e5c70317084"></a>
### Variable format

![][public]

**Definition**: `include/colRope.h` (line 286)

```cpp
Col_StringFormat Col_RopeChunk::format
```

Format of the traversed chunk.





**Type**: [Col\_StringFormat](colibri_8h.md#group__strings_1ga125054104f6260ea3902e6e46ebfdfa0)

<a id="struct_col___rope_chunk_1a625062993f256bd911a0341dbb4c495f"></a>
### Variable data

![][public]

**Definition**: `include/colRope.h` (line 287)

```cpp
const void* Col_RopeChunk::data
```

Pointer to format-specific data.





**Type**: const void *

<a id="struct_col___rope_chunk_1a91a659379d76867d98cdd975a50ce333"></a>
### Variable byteLength

![][public]

**Definition**: `include/colRope.h` (line 288)

```cpp
size_t Col_RopeChunk::byteLength
```

Data length in bytes.





**Type**: size_t

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)