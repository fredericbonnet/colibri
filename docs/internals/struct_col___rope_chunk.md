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

**Referenced by**:

* [HashChunkProc](col_hash_8c.md#group__hashmap__words_1gad45bb2e3e346da8b00d4f4c2ac852877)
* [Ucs1ComputeLengthProc](col_rope_8c.md#group__rope__words_1ga240868fcf7561b8009dde16401747e2d)
* [Ucs2ComputeLengthProc](col_rope_8c.md#group__rope__words_1gaa058721c03751bb068a479c88518b7d2)
* [Utf8ComputeByteLengthProc](col_rope_8c.md#group__rope__words_1ga10418e35f56a3c89469613771512e69d)
* [Utf16ComputeByteLengthProc](col_rope_8c.md#group__rope__words_1gaa0c769655c28d91094e7cedc22940753)
* [UcsComputeFormatProc](col_rope_8c.md#group__rope__words_1ga98e01b4cb68b2d66ca88570ad3181ecb)
* [Ucs1CopyDataProc](col_rope_8c.md#group__rope__words_1ga10ab42b79a27129c5b2cf5ad0e0b261d)
* [Ucs2CopyDataProc](col_rope_8c.md#group__rope__words_1ga61c8cb8089c0662dec71f99fcbed77e0)
* [Ucs4CopyDataProc](col_rope_8c.md#group__rope__words_1ga5cdbfa1993b834d2fbf3082280be139d)
* [Utf8CopyDataProc](col_rope_8c.md#group__rope__words_1gabea358803d83ed946db99def78568831)
* [Utf16CopyDataProc](col_rope_8c.md#group__rope__words_1gaf089cd3a00689db3e9600b46c21f2867)
* [FindCharProc](col_rope_8c.md#group__rope__words_1gafb8374e5f7f739d98741621c4c56ed55)
* [SearchSubropeProc](col_rope_8c.md#group__rope__words_1gac469cb18ebe714917347f930c6951f41)
* [MergeRopeChunksProc](col_rope_8c.md#group__rope__words_1ga33ffefe10088e23594b6f76a1d65e184)
* [GetChunk](col_rope_8c.md#group__rope__words_1gaf91ffc7dc85abe202e6c6946f27f3b06)
* [Col\_TraverseRopeChunks](col_rope_8h.md#group__rope__words_1ga1a0bffff5bb042717914fadb3e8501bb)
* [ColRopeIterUpdateTraversalInfo](col_rope_8h.md#group__rope__words_1ga9c738d41b3d8577f7cdd0fa1880f9143)

<a id="struct_col___rope_chunk_1a625062993f256bd911a0341dbb4c495f"></a>
### Variable data

![][public]

**Definition**: `include/colRope.h` (line 287)

```cpp
const void* Col_RopeChunk::data
```

Pointer to format-specific data.





**Type**: const void *

**Referenced by**:

* [HashChunkProc](col_hash_8c.md#group__hashmap__words_1gad45bb2e3e346da8b00d4f4c2ac852877)
* [Ucs1ComputeLengthProc](col_rope_8c.md#group__rope__words_1ga240868fcf7561b8009dde16401747e2d)
* [Ucs2ComputeLengthProc](col_rope_8c.md#group__rope__words_1gaa058721c03751bb068a479c88518b7d2)
* [Utf8ComputeByteLengthProc](col_rope_8c.md#group__rope__words_1ga10418e35f56a3c89469613771512e69d)
* [Utf16ComputeByteLengthProc](col_rope_8c.md#group__rope__words_1gaa0c769655c28d91094e7cedc22940753)
* [UcsComputeFormatProc](col_rope_8c.md#group__rope__words_1ga98e01b4cb68b2d66ca88570ad3181ecb)
* [Ucs1CopyDataProc](col_rope_8c.md#group__rope__words_1ga10ab42b79a27129c5b2cf5ad0e0b261d)
* [Ucs2CopyDataProc](col_rope_8c.md#group__rope__words_1ga61c8cb8089c0662dec71f99fcbed77e0)
* [Ucs4CopyDataProc](col_rope_8c.md#group__rope__words_1ga5cdbfa1993b834d2fbf3082280be139d)
* [Utf8CopyDataProc](col_rope_8c.md#group__rope__words_1gabea358803d83ed946db99def78568831)
* [Utf16CopyDataProc](col_rope_8c.md#group__rope__words_1gaf089cd3a00689db3e9600b46c21f2867)
* [FindCharProc](col_rope_8c.md#group__rope__words_1gafb8374e5f7f739d98741621c4c56ed55)
* [SearchSubropeProc](col_rope_8c.md#group__rope__words_1gac469cb18ebe714917347f930c6951f41)
* [CompareChunksProc](col_rope_8c.md#group__rope__words_1gaa0f078c09e57fc1f7ba7cdf5e80c7f9d)
* [MergeRopeChunksProc](col_rope_8c.md#group__rope__words_1ga33ffefe10088e23594b6f76a1d65e184)
* [GetChunk](col_rope_8c.md#group__rope__words_1gaf91ffc7dc85abe202e6c6946f27f3b06)
* [Col\_TraverseRopeChunksN](col_rope_8h.md#group__rope__words_1ga6cc41b66caa3cbce177085b49ec0139e)
* [Col\_TraverseRopeChunks](col_rope_8h.md#group__rope__words_1ga1a0bffff5bb042717914fadb3e8501bb)
* [ColRopeIterUpdateTraversalInfo](col_rope_8h.md#group__rope__words_1ga9c738d41b3d8577f7cdd0fa1880f9143)

<a id="struct_col___rope_chunk_1a91a659379d76867d98cdd975a50ce333"></a>
### Variable byteLength

![][public]

**Definition**: `include/colRope.h` (line 288)

```cpp
size_t Col_RopeChunk::byteLength
```

Data length in bytes.





**Type**: size_t

**Referenced by**:

* [FindCharProc](col_rope_8c.md#group__rope__words_1gafb8374e5f7f739d98741621c4c56ed55)
* [SearchSubropeProc](col_rope_8c.md#group__rope__words_1gac469cb18ebe714917347f930c6951f41)
* [MergeRopeChunksProc](col_rope_8c.md#group__rope__words_1ga33ffefe10088e23594b6f76a1d65e184)
* [GetChunk](col_rope_8c.md#group__rope__words_1gaf91ffc7dc85abe202e6c6946f27f3b06)
* [Col\_TraverseRopeChunksN](col_rope_8h.md#group__rope__words_1ga6cc41b66caa3cbce177085b49ec0139e)
* [Col\_TraverseRopeChunks](col_rope_8h.md#group__rope__words_1ga1a0bffff5bb042717914fadb3e8501bb)
* [ColRopeIterUpdateTraversalInfo](col_rope_8h.md#group__rope__words_1ga9c738d41b3d8577f7cdd0fa1880f9143)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)