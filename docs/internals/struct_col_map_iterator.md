<a id="struct_col_map_iterator"></a>
# Structure ColMapIterator

![][C++]
![][private]

**Definition**: `include/colMap.h` (line 60)

Internal implementation of map iterators.

**See also**: [Col\_MapIterator](col_map_8h.md#group__map__words_1ga33d331116aff3f3d03a231ccbbce40c2)

## Members

* [entry](struct_col_map_iterator.md#struct_col_map_iterator_1ab4b5223ec48c1b7aebcdd9463ebdf0a9)
* [map](struct_col_map_iterator.md#struct_col_map_iterator_1ae68e5f11baec349c4697a085d0d1039f)
* [traversal](struct_col_map_iterator.md#struct_col_map_iterator_1a39d62683f701b2cfed37955b3180ed89)

## Public attributes

<a id="struct_col_map_iterator_1ae68e5f11baec349c4697a085d0d1039f"></a>
### Variable map

![][public]

**Definition**: `include/colMap.h` (line 61)

```cpp
Col_Word ColMapIterator::map
```

Map being iterated.





**Type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="struct_col_map_iterator_1ab4b5223ec48c1b7aebcdd9463ebdf0a9"></a>
### Variable entry

![][public]

**Definition**: `include/colMap.h` (line 62)

```cpp
Col_Word ColMapIterator::entry
```

Current entry.





**Type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="struct_col_map_iterator_1a39d62683f701b2cfed37955b3180ed89"></a>
### Variable traversal

![][public]

**Definition**: `include/colMap.h` (line 83)

```cpp
union ColMapIterator::@0 ColMapIterator::traversal
```

Implementation-specific traversal info.





**Type**: union ColMapIterator::@0

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)