<a id="struct_col___custom_list_type"></a>
# Structure Col\_CustomListType

![][C++]
![][public]

**Definition**: `include/colList.h` (line 533)

Custom list type descriptor.

Inherits from [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type).

## Members

* [chunkAtProc](struct_col___custom_list_type.md#struct_col___custom_list_type_1a71964a691bc5b9ff15a6441cecb9825c)
* [concatProc](struct_col___custom_list_type.md#struct_col___custom_list_type_1a00a0cd040234db58920ca192bdcc077c)
* [elementAtProc](struct_col___custom_list_type.md#struct_col___custom_list_type_1a70aab7ac3b4b86103df2f9e49c1cadfc)
* [lengthProc](struct_col___custom_list_type.md#struct_col___custom_list_type_1ac6b28eb281be246b4f3427fbb2941413)
* [sublistProc](struct_col___custom_list_type.md#struct_col___custom_list_type_1a431969013f0382f5d3c9d8f9902d07c3)
* [type](struct_col___custom_list_type.md#struct_col___custom_list_type_1aa5adf5f0d567cbd47a4fe6090e7e88cb)

## Public attributes

<a id="struct_col___custom_list_type_1aa5adf5f0d567cbd47a4fe6090e7e88cb"></a>
### Variable type

![][public]

**Definition**: `include/colList.h` (line 535)

```cpp
Col_CustomWordType Col_CustomListType::type
```

Generic word type descriptor.

Type field must be equal to [COL\_LIST](col_word_8h.md#group__words_1gafaaad5bdc900622b1387bcb1f32f61c3).



**Type**: [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type)

<a id="struct_col___custom_list_type_1ac6b28eb281be246b4f3427fbb2941413"></a>
### Variable lengthProc

![][public]

**Definition**: `include/colList.h` (line 539)

```cpp
Col_ListLengthProc* Col_CustomListType::lengthProc
```

Called to get the length of the list.

Must be constant during the whole lifetime.



**Type**: [Col\_ListLengthProc](col_list_8h.md#group__customlist__words_1ga4e52f301753724d0b03f80e6c2c53f59) *

<a id="struct_col___custom_list_type_1a70aab7ac3b4b86103df2f9e49c1cadfc"></a>
### Variable elementAtProc

![][public]

**Definition**: `include/colList.h` (line 543)

```cpp
Col_ListElementAtProc* Col_CustomListType::elementAtProc
```

Called to get the element at a given position.

Must be constant during the whole lifetime.



**Type**: [Col\_ListElementAtProc](col_list_8h.md#group__customlist__words_1gae4ea34960861f5b333ce546fa728a9ab) *

<a id="struct_col___custom_list_type_1a71964a691bc5b9ff15a6441cecb9825c"></a>
### Variable chunkAtProc

![][public]

**Definition**: `include/colList.h` (line 547)

```cpp
Col_ListChunkAtProc* Col_CustomListType::chunkAtProc
```

Called during traversal.

If NULL, traversal is done per character using [elementAtProc](struct_col___custom_list_type.md#struct_col___custom_list_type_1a70aab7ac3b4b86103df2f9e49c1cadfc).



**Type**: [Col\_ListChunkAtProc](col_list_8h.md#group__customlist__words_1ga20b664371d83c4106431af2f1114c156) *

<a id="struct_col___custom_list_type_1a431969013f0382f5d3c9d8f9902d07c3"></a>
### Variable sublistProc

![][public]

**Definition**: `include/colList.h` (line 551)

```cpp
Col_ListSublistProc* Col_CustomListType::sublistProc
```

Called to extract sublist.

If NULL, or if it returns nil, use the standard procedure.



**Type**: [Col\_ListSublistProc](col_list_8h.md#group__customlist__words_1ga8118b999e36b1bd5ed78d6c4c11ae946) *

<a id="struct_col___custom_list_type_1a00a0cd040234db58920ca192bdcc077c"></a>
### Variable concatProc

![][public]

**Definition**: `include/colList.h` (line 555)

```cpp
Col_ListConcatProc* Col_CustomListType::concatProc
```

Called to concat lists.

If NULL, or if it returns nil, use the standard procedure.



**Type**: [Col\_ListConcatProc](col_list_8h.md#group__customlist__words_1ga9f8dc9244a9b20dda2486f7ae3306576) *

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)