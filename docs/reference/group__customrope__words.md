<a id="group__customrope__words"></a>
# Custom Ropes

Custom ropes are [Custom Words](group__custom__words.md#group__custom__words) implementing [Ropes](group__rope__words.md#group__rope__words) with applicative code.



## Custom Rope Type Descriptors

<a id="group__customrope__words_1gafb80b4d5ced2633126855b9b00458730"></a>
### Typedef Col\_RopeLengthProc

![][public]

**Definition**: `include/colRope.h` (line 615)

```cpp
typedef size_t() Col_RopeLengthProc(Col_Word rope)
```

Function signature of custom rope length procs.

**Parameters**:

* **rope**: Custom rope to get length for.


**Returns**:

The custom rope length.




**See also**: [Col\_CustomRopeType](struct_col___custom_rope_type.md#struct_col___custom_rope_type), [Col\_RopeLength](col_rope_8h.md#group__rope__words_1ga19acc7c753b0f7b55287b11360259a16)



**Return type**: size_t()

<a id="group__customrope__words_1ga72262f9fb5f27c2d403a9f6c7f76d791"></a>
### Typedef Col\_RopeCharAtProc

![][public]

**Definition**: `include/colRope.h` (line 632)

```cpp
typedef Col_Char() Col_RopeCharAtProc(Col_Word rope, size_t index)
```

Function signature of custom rope character access procs.

**Parameters**:

* **rope**: Custom rope to get character from.
* **index**: Character index.


?> By construction, **index** is guaranteed to be within valid range, so implementations need not bother with validation.


**Returns**:

The Unicode codepoint of the character at the given index.




**See also**: [Col\_CustomRopeType](struct_col___custom_rope_type.md#struct_col___custom_rope_type), [Col\_RopeAt](col_rope_8h.md#group__rope__words_1ga1dec8dd3d4d8563176a234f5cb9bc5ee)



**Return type**: [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862)()

<a id="group__customrope__words_1ga83788cec4f62ad28636fbc73cf28ab71"></a>
### Typedef Col\_RopeChunkAtProc

![][public]

**Definition**: `include/colRope.h` (line 650)

```cpp
typedef void() Col_RopeChunkAtProc(Col_Word rope, size_t index, Col_RopeChunk *chunkPtr, size_t *firstPtr, size_t *lastPtr)
```

Function signature of custom rope chunk access procs.

**Parameters**:

* **rope**: Custom rope to get chunk from.
* **index**: Index of character to get chunk for.
* **chunkPtr**: Chunk information (see [Col\_RopeChunk](struct_col___rope_chunk.md#struct_col___rope_chunk)).
* **firstPtr lastPtr**: Chunk range of validity.


?> By construction, **index** is guaranteed to be within valid range, so implementations need not bother with validation.



**See also**: [Col\_CustomRopeType](struct_col___custom_rope_type.md#struct_col___custom_rope_type)



**Return type**: void()

<a id="group__customrope__words_1ga8d36e9e3d14cb4d7f24ac0512554c524"></a>
### Typedef Col\_RopeSubropeProc

![][public]

**Definition**: `include/colRope.h` (line 669)

```cpp
typedef Col_Word() Col_RopeSubropeProc(Col_Word rope, size_t first, size_t last)
```

Function signature of custom rope subrope extraction.

**Parameters**:

* **rope**: Custom rope to extract subrope from.
* **first last**: Range of subrope to extract (inclusive).


?> By construction, **first** and **last** are guaranteed to be within valid range, so implementations need not bother with validation.


**Return values**:

* **nil**: to use the generic representation.
* **rope**: representing the subrope otherwise.




**See also**: [Col\_CustomRopeType](struct_col___custom_rope_type.md#struct_col___custom_rope_type), [Col\_Subrope](col_rope_8h.md#group__rope__words_1ga688a99f26c500c1f65f4141e97de0335)



**Return type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)()

<a id="group__customrope__words_1ga25ddec114a580444dbd447c7e4344652"></a>
### Typedef Col\_RopeConcatProc

![][public]

**Definition**: `include/colRope.h` (line 683)

```cpp
typedef Col_Word() Col_RopeConcatProc(Col_Word left, Col_Word right)
```

Function signature of custom rope concatenation.

**Parameters**:

* **left right**: Ropes to concatenate.


**Return values**:

* **nil**: to use the generic representation.
* **rope**: representing the concatenation of both ropes otherwise.




**See also**: [Col\_CustomRopeType](struct_col___custom_rope_type.md#struct_col___custom_rope_type), [Col\_ConcatRopes](col_rope_8h.md#group__rope__words_1gaafab3ef159c0b11402cc50c91fc59700)



**Return type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)()

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[private]: https://img.shields.io/badge/-private-red (private)
[static]: https://img.shields.io/badge/-static-lightgrey (static)