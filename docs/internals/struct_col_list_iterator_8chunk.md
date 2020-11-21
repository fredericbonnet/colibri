<a id="struct_col_list_iterator_8chunk"></a>
# Structure ColListIterator.chunk

![][C++]
![][public]

**Definition**: `include/colList.h` (line 200)

Current chunk info.



## Members

* [accessProc](struct_col_list_iterator_8chunk.md#struct_col_list_iterator_8chunk_1a6f739c488e5f16ee1053c535ae055a03)
* [current](struct_col_list_iterator_8chunk.md#struct_col_list_iterator_8chunk_1a43b5c9175984c071f30b873fdce0a000)
* [first](struct_col_list_iterator_8chunk.md#struct_col_list_iterator_8chunk_1a8b04d5e3775d298e78455efc5ca404d5)
* [last](struct_col_list_iterator_8chunk.md#struct_col_list_iterator_8chunk_1a98bd1c45684cf587ac2347a92dd7bb51)

## Public attributes

<a id="struct_col_list_iterator_8chunk_1a8b04d5e3775d298e78455efc5ca404d5"></a>
### Variable first

![][public]

**Definition**: `include/colList.h` (line 201)

```cpp

```

Begining of range of validity for current chunk.





**Type**: size_t

<a id="struct_col_list_iterator_8chunk_1a98bd1c45684cf587ac2347a92dd7bb51"></a>
### Variable last

![][public]

**Definition**: `include/colList.h` (line 202)

```cpp

```

End of range of validity for current chunk.





**Type**: size_t

<a id="struct_col_list_iterator_8chunk_1a6f739c488e5f16ee1053c535ae055a03"></a>
### Variable accessProc

![][public]

**Definition**: `include/colList.h` (line 205)

```cpp

```

If non-NULL, element accessor.

Else, use direct address mode.



**Type**: [ColListIterLeafAtProc](col_list_8h.md#group__list__words_1ga1e84f299b255cb8d8f55d8681d056411) *

<a id="struct_col_list_iterator_8chunk_1a43b5c9175984c071f30b873fdce0a000"></a>
### Variable current

![][public]

**Definition**: `include/colList.h` (line 218)

```cpp

```

Current element information.





**Type**: union ColListIterator::@0::@1

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)