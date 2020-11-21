<a id="struct_col_rope_iterator_8chunk"></a>
# Structure ColRopeIterator.chunk

![][C++]
![][public]

**Definition**: `include/colRope.h` (line 355)

Current chunk info.



## Members

* [accessProc](struct_col_rope_iterator_8chunk.md#struct_col_rope_iterator_8chunk_1a6f739c488e5f16ee1053c535ae055a03)
* [current](struct_col_rope_iterator_8chunk.md#struct_col_rope_iterator_8chunk_1a43b5c9175984c071f30b873fdce0a000)
* [first](struct_col_rope_iterator_8chunk.md#struct_col_rope_iterator_8chunk_1a8b04d5e3775d298e78455efc5ca404d5)
* [last](struct_col_rope_iterator_8chunk.md#struct_col_rope_iterator_8chunk_1a98bd1c45684cf587ac2347a92dd7bb51)

## Public attributes

<a id="struct_col_rope_iterator_8chunk_1a8b04d5e3775d298e78455efc5ca404d5"></a>
### Variable first

![][public]

**Definition**: `include/colRope.h` (line 356)

```cpp

```

Begining of range of validity for current chunk.





**Type**: size_t

<a id="struct_col_rope_iterator_8chunk_1a98bd1c45684cf587ac2347a92dd7bb51"></a>
### Variable last

![][public]

**Definition**: `include/colRope.h` (line 357)

```cpp

```

End of range of validity for current chunk.





**Type**: size_t

<a id="struct_col_rope_iterator_8chunk_1a6f739c488e5f16ee1053c535ae055a03"></a>
### Variable accessProc

![][public]

**Definition**: `include/colRope.h` (line 360)

```cpp

```

If non-NULL, element accessor.

Else, use direct address mode.



**Type**: [ColRopeIterLeafAtProc](col_rope_8h.md#group__rope__words_1gae708f12f954e4776ed28eb37f1e99d4d) *

<a id="struct_col_rope_iterator_8chunk_1a43b5c9175984c071f30b873fdce0a000"></a>
### Variable current

![][public]

**Definition**: `include/colRope.h` (line 376)

```cpp

```

Current element information.





**Type**: union ColRopeIterator::@0::@1

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)