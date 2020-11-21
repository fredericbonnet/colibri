<a id="struct_col_rope_iterator"></a>
# Structure ColRopeIterator

![][C++]
![][private]

**Definition**: `include/colRope.h` (line 348)

Internal implementation of rope iterators.

**See also**: [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725)

## Members

* [chunk](struct_col_rope_iterator.md#struct_col_rope_iterator_1af41f98123923a570cbe6245b454e3d63)
* [index](struct_col_rope_iterator.md#struct_col_rope_iterator_1ad9c858e2bbbff5fd781a750fdaef9196)
* [length](struct_col_rope_iterator.md#struct_col_rope_iterator_1a3a2e7de5d991452374db32b0c1c53d57)
* [rope](struct_col_rope_iterator.md#struct_col_rope_iterator_1ae2da4eee61fdea0004be46c9c93cd432)

## Public attributes

<a id="struct_col_rope_iterator_1ae2da4eee61fdea0004be46c9c93cd432"></a>
### Variable rope

![][public]

**Definition**: `include/colRope.h` (line 349)

```cpp
Col_Word ColRopeIterator::rope
```

Rope being iterated.

If nil, use string iterator mode.



**Type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**Referenced by**:

* [ColRopeIterUpdateTraversalInfo](col_rope_8h.md#group__rope__words_1ga9c738d41b3d8577f7cdd0fa1880f9143)

<a id="struct_col_rope_iterator_1a3a2e7de5d991452374db32b0c1c53d57"></a>
### Variable length

![][public]

**Definition**: `include/colRope.h` (line 351)

```cpp
size_t ColRopeIterator::length
```

Rope length.





**Type**: size_t

**Referenced by**:

* [ColRopeIterUpdateTraversalInfo](col_rope_8h.md#group__rope__words_1ga9c738d41b3d8577f7cdd0fa1880f9143)

<a id="struct_col_rope_iterator_1ad9c858e2bbbff5fd781a750fdaef9196"></a>
### Variable index

![][public]

**Definition**: `include/colRope.h` (line 352)

```cpp
size_t ColRopeIterator::index
```

Current position.





**Type**: size_t

**Referenced by**:

* [ColRopeIterUpdateTraversalInfo](col_rope_8h.md#group__rope__words_1ga9c738d41b3d8577f7cdd0fa1880f9143)

<a id="struct_col_rope_iterator_1af41f98123923a570cbe6245b454e3d63"></a>
### Variable chunk

![][public]

**Definition**: `include/colRope.h` (line 377)

```cpp
struct ColRopeIterator::@0 ColRopeIterator::chunk
```

Current chunk info.





**Type**: struct ColRopeIterator::@0

**Referenced by**:

* [ColRopeIterUpdateTraversalInfo](col_rope_8h.md#group__rope__words_1ga9c738d41b3d8577f7cdd0fa1880f9143)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)