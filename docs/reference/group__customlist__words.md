<a id="group__customlist__words"></a>
# Custom Lists

Custom lists are [Custom Words](group__custom__words.md#group__custom__words) implementing [Immutable Lists](group__list__words.md#group__list__words) with applicative code.



## Custom List Type Descriptors

<a id="group__customlist__words_1ga4e52f301753724d0b03f80e6c2c53f59"></a>
### Typedef Col\_ListLengthProc

![][public]

**Definition**: `include/colList.h` (line 459)

```cpp
typedef size_t() Col_ListLengthProc(Col_Word list)
```

Function signature of custom list length procs.

**Parameters**:

* **list**: Custom list to get length for.


**Returns**:

The custom list length.




**See also**: [Col\_CustomListType](struct_col___custom_list_type.md#struct_col___custom_list_type), [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)



**Return type**: size_t()

<a id="group__customlist__words_1gae4ea34960861f5b333ce546fa728a9ab"></a>
### Typedef Col\_ListElementAtProc

![][public]

**Definition**: `include/colList.h` (line 476)

```cpp
typedef Col_Word() Col_ListElementAtProc(Col_Word list, size_t index)
```

Function signature of custom list element access procs.

**Parameters**:

* **list**: Custom list to get element from.
* **index**: Element index.


?> By construction, **index** is guaranteed to be within valid range, so implementations need not bother with validation.


**Returns**:

The element at the given index.




**See also**: [Col\_CustomListType](struct_col___custom_list_type.md#struct_col___custom_list_type), [Col\_ListAt](col_list_8h.md#group__list__words_1ga7550908ac1ceae597c8283afbaa7f813)



**Return type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)()

<a id="group__customlist__words_1ga20b664371d83c4106431af2f1114c156"></a>
### Typedef Col\_ListChunkAtProc

![][public]

**Definition**: `include/colList.h` (line 495)

```cpp
typedef void() Col_ListChunkAtProc(Col_Word list, size_t index, const Col_Word **chunkPtr, size_t *firstPtr, size_t *lastPtr)
```

Function signature of custom list chunk access procs.

**Parameters**:

* **list**: Custom list to get chunk from.
* **index**: Index of element to get chunk for.
* **chunkPtr**: Chunk. If [COL\_LISTCHUNK\_VOID](col_list_8h.md#group__list__words_1ga8c1b8e76380329e1264c12e77bfbf30d), the chunk is void (i.e. all its elements are nil).
* **firstPtr lastPtr**: Chunk range of validity.


?> By construction, **index** is guaranteed to be within valid range, so implementations need not bother with validation.



**See also**: [Col\_CustomListType](struct_col___custom_list_type.md#struct_col___custom_list_type)



**Return type**: void()

<a id="group__customlist__words_1ga8118b999e36b1bd5ed78d6c4c11ae946"></a>
### Typedef Col\_ListSublistProc

![][public]

**Definition**: `include/colList.h` (line 514)

```cpp
typedef Col_Word() Col_ListSublistProc(Col_Word list, size_t first, size_t last)
```

Function signature of custom list sublist extraction.

**Parameters**:

* **list**: Custom list to extract sublist from.
* **first last**: Range of sublist to extract (inclusive).


?> By construction, **first** and **last** are guaranteed to be within valid range, so implementations need not bother with validation.


**Return values**:

* **nil**: to use the generic representation.
* **list**: representing the sublist otherwise.




**See also**: [Col\_CustomListType](struct_col___custom_list_type.md#struct_col___custom_list_type), [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)



**Return type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)()

<a id="group__customlist__words_1ga9f8dc9244a9b20dda2486f7ae3306576"></a>
### Typedef Col\_ListConcatProc

![][public]

**Definition**: `include/colList.h` (line 528)

```cpp
typedef Col_Word() Col_ListConcatProc(Col_Word left, Col_Word right)
```

Function signature of custom list concatenation.

**Parameters**:

* **left right**: Lists to concatenate.


**Return values**:

* **nil**: to use the generic representation.
* **rope**: representing the concatenation of both lists otherwise.




**See also**: [Col\_CustomListType](struct_col___custom_list_type.md#struct_col___custom_list_type), [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)



**Return type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)()

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[private]: https://img.shields.io/badge/-private-red (private)
[static]: https://img.shields.io/badge/-static-lightgrey (static)