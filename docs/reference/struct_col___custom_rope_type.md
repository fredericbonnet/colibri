<a id="struct_col___custom_rope_type"></a>
# Structure Col\_CustomRopeType

![][C++]
![][public]

**Definition**: `include/colRope.h` (line 688)

Custom rope type descriptor.

Inherits from [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type).

## Members

* [charAtProc](struct_col___custom_rope_type.md#struct_col___custom_rope_type_1a60b6e82ac9aaa2363582f8a2db09f5a1)
* [chunkAtProc](struct_col___custom_rope_type.md#struct_col___custom_rope_type_1a3dc0ea7157c31ced24784c666f794dd3)
* [concatProc](struct_col___custom_rope_type.md#struct_col___custom_rope_type_1a6c5065d5970b033657bfeae20457bf82)
* [lengthProc](struct_col___custom_rope_type.md#struct_col___custom_rope_type_1ad3b816bb21a13de5ec142c0ae7d50a0b)
* [subropeProc](struct_col___custom_rope_type.md#struct_col___custom_rope_type_1a69f30a52f68d9df783c3ec876a19c5bf)
* [type](struct_col___custom_rope_type.md#struct_col___custom_rope_type_1ab1a54b63c4a00889da68065932c1f5df)

## Public attributes

<a id="struct_col___custom_rope_type_1ab1a54b63c4a00889da68065932c1f5df"></a>
### Variable type

![][public]

**Definition**: `include/colRope.h` (line 690)

```cpp
Col_CustomWordType Col_CustomRopeType::type
```

Generic word type descriptor.

Type field must be equal to [COL\_ROPE](col_word_8h.md#group__words_1ga64b6f74edaf16829f0083a21dddd4d93).



**Type**: [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type)

<a id="struct_col___custom_rope_type_1ad3b816bb21a13de5ec142c0ae7d50a0b"></a>
### Variable lengthProc

![][public]

**Definition**: `include/colRope.h` (line 694)

```cpp
Col_RopeLengthProc* Col_CustomRopeType::lengthProc
```

Called to get the length of the rope.

Must be constant during the whole lifetime.



**Type**: [Col\_RopeLengthProc](col_rope_8h.md#group__customrope__words_1gafb80b4d5ced2633126855b9b00458730) *

<a id="struct_col___custom_rope_type_1a60b6e82ac9aaa2363582f8a2db09f5a1"></a>
### Variable charAtProc

![][public]

**Definition**: `include/colRope.h` (line 698)

```cpp
Col_RopeCharAtProc* Col_CustomRopeType::charAtProc
```

Called to get the character at a given position.

Must be constant during the whole lifetime.



**Type**: [Col\_RopeCharAtProc](col_rope_8h.md#group__customrope__words_1ga72262f9fb5f27c2d403a9f6c7f76d791) *

<a id="struct_col___custom_rope_type_1a3dc0ea7157c31ced24784c666f794dd3"></a>
### Variable chunkAtProc

![][public]

**Definition**: `include/colRope.h` (line 702)

```cpp
Col_RopeChunkAtProc* Col_CustomRopeType::chunkAtProc
```

Called during traversal.

If NULL, traversal is done per character using [charAtProc](struct_col___custom_rope_type.md#struct_col___custom_rope_type_1a60b6e82ac9aaa2363582f8a2db09f5a1).



**Type**: [Col\_RopeChunkAtProc](col_rope_8h.md#group__customrope__words_1ga83788cec4f62ad28636fbc73cf28ab71) *

<a id="struct_col___custom_rope_type_1a69f30a52f68d9df783c3ec876a19c5bf"></a>
### Variable subropeProc

![][public]

**Definition**: `include/colRope.h` (line 706)

```cpp
Col_RopeSubropeProc* Col_CustomRopeType::subropeProc
```

Called to extract subrope.

If NULL, or if it returns nil, use the standard procedure.



**Type**: [Col\_RopeSubropeProc](col_rope_8h.md#group__customrope__words_1ga8d36e9e3d14cb4d7f24ac0512554c524) *

<a id="struct_col___custom_rope_type_1a6c5065d5970b033657bfeae20457bf82"></a>
### Variable concatProc

![][public]

**Definition**: `include/colRope.h` (line 710)

```cpp
Col_RopeConcatProc* Col_CustomRopeType::concatProc
```

Called to concat ropes.

If NULL, or if it returns nil, use the standard procedure.



**Type**: [Col\_RopeConcatProc](col_rope_8h.md#group__customrope__words_1ga25ddec114a580444dbd447c7e4344652) *

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)