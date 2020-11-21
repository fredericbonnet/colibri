<a id="struct_col_list_iterator"></a>
# Structure ColListIterator

![][C++]
![][private]

**Definition**: `include/colList.h` (line 193)

Internal implementation of list iterators.

**See also**: [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb)

## Members

* [chunk](struct_col_list_iterator.md#struct_col_list_iterator_1a5ab651baa3c1d996362c4002fd794dfb)
* [index](struct_col_list_iterator.md#struct_col_list_iterator_1aa2af90fd6c4555d40bd97cf9643a55a7)
* [length](struct_col_list_iterator.md#struct_col_list_iterator_1aed4f40384c36e99739b843fd488be5fb)
* [list](struct_col_list_iterator.md#struct_col_list_iterator_1a96b5bb57b535f6d91149d079cb1f659b)

## Public attributes

<a id="struct_col_list_iterator_1a96b5bb57b535f6d91149d079cb1f659b"></a>
### Variable list

![][public]

**Definition**: `include/colList.h` (line 194)

```cpp
Col_Word ColListIterator::list
```

List being iterated.

If nil, use array iterator mode.



**Type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**Referenced by**:

* [ColListIterUpdateTraversalInfo](col_list_8h.md#group__list__words_1ga65a0551576955013edca89745886b5dd)

<a id="struct_col_list_iterator_1aed4f40384c36e99739b843fd488be5fb"></a>
### Variable length

![][public]

**Definition**: `include/colList.h` (line 196)

```cpp
size_t ColListIterator::length
```

List length.





**Type**: size_t

**Referenced by**:

* [ColListIterUpdateTraversalInfo](col_list_8h.md#group__list__words_1ga65a0551576955013edca89745886b5dd)

<a id="struct_col_list_iterator_1aa2af90fd6c4555d40bd97cf9643a55a7"></a>
### Variable index

![][public]

**Definition**: `include/colList.h` (line 197)

```cpp
size_t ColListIterator::index
```

Current position.





**Type**: size_t

**Referenced by**:

* [ColListIterUpdateTraversalInfo](col_list_8h.md#group__list__words_1ga65a0551576955013edca89745886b5dd)

<a id="struct_col_list_iterator_1a5ab651baa3c1d996362c4002fd794dfb"></a>
### Variable chunk

![][public]

**Definition**: `include/colList.h` (line 219)

```cpp
struct ColListIterator::@0 ColListIterator::chunk
```

Current chunk info.





**Type**: struct ColListIterator::@0

**Referenced by**:

* [ColListIterUpdateTraversalInfo](col_list_8h.md#group__list__words_1ga65a0551576955013edca89745886b5dd)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)