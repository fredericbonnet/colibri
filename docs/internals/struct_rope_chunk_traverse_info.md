<a id="struct_rope_chunk_traverse_info"></a>
# Structure RopeChunkTraverseInfo

![][C++]
![][private]

**Definition**: `src/colRope.c` (line 2997)

Structure used during recursive rope chunk traversal.

This avoids recursive procedure calls thanks to a pre-allocated backtracking structure: since the algorithms only recurse on concat nodes and since we know their depth, we can allocate the needed space once and for all.








**See also**: [Col\_TraverseRopeChunksN](col_rope_8h.md#group__rope__words_1ga6cc41b66caa3cbce177085b49ec0139e), [Col\_TraverseRopeChunks](col_rope_8h.md#group__rope__words_1ga1a0bffff5bb042717914fadb3e8501bb)

## Members

* [backtracks](struct_rope_chunk_traverse_info.md#struct_rope_chunk_traverse_info_1a653a39a79146eb8fe0e0988aaf08b219)
* [c](struct_rope_chunk_traverse_info.md#struct_rope_chunk_traverse_info_1aecc47cd8ed42929d0412acbbf9254880)
* [max](struct_rope_chunk_traverse_info.md#struct_rope_chunk_traverse_info_1a8671deec82005e2811f415a9bb591525)
* [maxDepth](struct_rope_chunk_traverse_info.md#struct_rope_chunk_traverse_info_1a379b2b0c765600955094d49ac72f16eb)
* [prevDepth](struct_rope_chunk_traverse_info.md#struct_rope_chunk_traverse_info_1a7601886d7710fa6f6c4fd4c3cdc43b54)
* [rope](struct_rope_chunk_traverse_info.md#struct_rope_chunk_traverse_info_1a80faebac21e5a340b109ab156f2432a4)
* [start](struct_rope_chunk_traverse_info.md#struct_rope_chunk_traverse_info_1ad322e41e9473c8b24d6df29819ed8f16)

## Public attributes

<a id="struct_rope_chunk_traverse_info_1a653a39a79146eb8fe0e0988aaf08b219"></a>
### Variable backtracks

![][public]

**Definition**: `src/colRope.c` (line 3003)

```cpp
struct RopeChunkTraverseInfo::@0 * RopeChunkTraverseInfo::backtracks
```

Pre-allocated backtracking structure.





**Type**: struct RopeChunkTraverseInfo::@0 *

**Referenced by**:

* [GetChunk](col_rope_8c.md#group__rope__words_1gaf91ffc7dc85abe202e6c6946f27f3b06)
* [NextChunk](col_rope_8c.md#group__rope__words_1ga1afce7c7ad5cb705ecb4f0e8368aa9ae)
* [Col\_TraverseRopeChunksN](col_rope_8h.md#group__rope__words_1ga6cc41b66caa3cbce177085b49ec0139e)
* [Col\_TraverseRopeChunks](col_rope_8h.md#group__rope__words_1ga1a0bffff5bb042717914fadb3e8501bb)

<a id="struct_rope_chunk_traverse_info_1a80faebac21e5a340b109ab156f2432a4"></a>
### Variable rope

![][public]

**Definition**: `src/colRope.c` (line 3005)

```cpp
Col_Word RopeChunkTraverseInfo::rope
```

Currently traversed rope.





**Type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**Referenced by**:

* [Col\_NewRope](col_rope_8h.md#group__rope__words_1gadf89e360729ba5052887cd4897b0167f)
* [GetChunk](col_rope_8c.md#group__rope__words_1gaf91ffc7dc85abe202e6c6946f27f3b06)
* [NextChunk](col_rope_8c.md#group__rope__words_1ga1afce7c7ad5cb705ecb4f0e8368aa9ae)
* [Col\_TraverseRopeChunksN](col_rope_8h.md#group__rope__words_1ga6cc41b66caa3cbce177085b49ec0139e)
* [Col\_TraverseRopeChunks](col_rope_8h.md#group__rope__words_1ga1a0bffff5bb042717914fadb3e8501bb)

<a id="struct_rope_chunk_traverse_info_1aecc47cd8ed42929d0412acbbf9254880"></a>
### Variable c

![][public]

**Definition**: `src/colRope.c` (line 3006)

```cpp
Col_Char4 RopeChunkTraverseInfo::c
```

Character used as buffer for single char ropes (e.g.

immediate chars of char-based custom ropes).



**Type**: [Col\_Char4](colibri_8h.md#group__strings_1ga2eafc0c12affd9bdae281629460cfa73)

**Referenced by**:

* [Col\_NewCharWord](col_rope_8h.md#group__rope__words_1ga41d52ca5141a365cf9df75178796b2ea)
* [Ucs1ComputeLengthProc](col_rope_8c.md#group__rope__words_1ga240868fcf7561b8009dde16401747e2d)
* [Ucs2ComputeLengthProc](col_rope_8c.md#group__rope__words_1gaa058721c03751bb068a479c88518b7d2)
* [Utf8ComputeByteLengthProc](col_rope_8c.md#group__rope__words_1ga10418e35f56a3c89469613771512e69d)
* [Utf16ComputeByteLengthProc](col_rope_8c.md#group__rope__words_1gaa0c769655c28d91094e7cedc22940753)
* [UcsComputeFormatProc](col_rope_8c.md#group__rope__words_1ga98e01b4cb68b2d66ca88570ad3181ecb)
* [GetChunk](col_rope_8c.md#group__rope__words_1gaf91ffc7dc85abe202e6c6946f27f3b06)

<a id="struct_rope_chunk_traverse_info_1ad322e41e9473c8b24d6df29819ed8f16"></a>
### Variable start

![][public]

**Definition**: `src/colRope.c` (line 3009)

```cpp
size_t RopeChunkTraverseInfo::start
```

Index of first character traversed in rope.





**Type**: size_t

**Referenced by**:

* [GetChunk](col_rope_8c.md#group__rope__words_1gaf91ffc7dc85abe202e6c6946f27f3b06)
* [NextChunk](col_rope_8c.md#group__rope__words_1ga1afce7c7ad5cb705ecb4f0e8368aa9ae)
* [Col\_TraverseRopeChunksN](col_rope_8h.md#group__rope__words_1ga6cc41b66caa3cbce177085b49ec0139e)
* [Col\_TraverseRopeChunks](col_rope_8h.md#group__rope__words_1ga1a0bffff5bb042717914fadb3e8501bb)

<a id="struct_rope_chunk_traverse_info_1a8671deec82005e2811f415a9bb591525"></a>
### Variable max

![][public]

**Definition**: `src/colRope.c` (line 3010)

```cpp
size_t RopeChunkTraverseInfo::max
```

Max number of characters traversed in rope.





**Type**: size_t

**Referenced by**:

* [GetChunk](col_rope_8c.md#group__rope__words_1gaf91ffc7dc85abe202e6c6946f27f3b06)
* [NextChunk](col_rope_8c.md#group__rope__words_1ga1afce7c7ad5cb705ecb4f0e8368aa9ae)
* [Col\_TraverseRopeChunksN](col_rope_8h.md#group__rope__words_1ga6cc41b66caa3cbce177085b49ec0139e)
* [Col\_TraverseRopeChunks](col_rope_8h.md#group__rope__words_1ga1a0bffff5bb042717914fadb3e8501bb)

<a id="struct_rope_chunk_traverse_info_1a379b2b0c765600955094d49ac72f16eb"></a>
### Variable maxDepth

![][public]

**Definition**: `src/colRope.c` (line 3011)

```cpp
int RopeChunkTraverseInfo::maxDepth
```

Depth of toplevel concat node.





**Type**: int

**Referenced by**:

* [Col\_TraverseRopeChunksN](col_rope_8h.md#group__rope__words_1ga6cc41b66caa3cbce177085b49ec0139e)
* [Col\_TraverseRopeChunks](col_rope_8h.md#group__rope__words_1ga1a0bffff5bb042717914fadb3e8501bb)

<a id="struct_rope_chunk_traverse_info_1a7601886d7710fa6f6c4fd4c3cdc43b54"></a>
### Variable prevDepth

![][public]

**Definition**: `src/colRope.c` (line 3012)

```cpp
int RopeChunkTraverseInfo::prevDepth
```

Depth of next concat node for backtracking.





**Type**: int

**Referenced by**:

* [GetChunk](col_rope_8c.md#group__rope__words_1gaf91ffc7dc85abe202e6c6946f27f3b06)
* [NextChunk](col_rope_8c.md#group__rope__words_1ga1afce7c7ad5cb705ecb4f0e8368aa9ae)
* [Col\_TraverseRopeChunksN](col_rope_8h.md#group__rope__words_1ga6cc41b66caa3cbce177085b49ec0139e)
* [Col\_TraverseRopeChunks](col_rope_8h.md#group__rope__words_1ga1a0bffff5bb042717914fadb3e8501bb)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)