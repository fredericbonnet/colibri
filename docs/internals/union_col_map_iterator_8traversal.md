<a id="union_col_map_iterator_8traversal"></a>
# Union ColMapIterator.traversal undefined

![][C++]
![][public]

**Definition**: `include/colMap.h` (line 65)

Implementation-specific traversal info.



## Members

* [custom](union_col_map_iterator_8traversal.md#union_col_map_iterator_8traversal_1a963faad63d835bf85f10fa0dbce78d21)
* [hash](union_col_map_iterator_8traversal.md#union_col_map_iterator_8traversal_1a0800fc577294c34e0b28ad2839435945)
* [trie](union_col_map_iterator_8traversal.md#union_col_map_iterator_8traversal_1a2b612faeed66bca483ab46f2dba80d4e)

## Public attributes

<a id="union_col_map_iterator_8traversal_1a0800fc577294c34e0b28ad2839435945"></a>
### Variable hash

![][public]

**Definition**: `include/colMap.h` (line 69)

```cpp

```

Hash-specific fields.





**Type**: struct ColMapIterator::@0::@1

<a id="union_col_map_iterator_8traversal_1a2b612faeed66bca483ab46f2dba80d4e"></a>
### Variable trie

![][public]

**Definition**: `include/colMap.h` (line 77)

```cpp

```

Trie-specific fields.





**Type**: struct ColMapIterator::@0::@2

<a id="union_col_map_iterator_8traversal_1a963faad63d835bf85f10fa0dbce78d21"></a>
### Variable custom

![][public]

**Definition**: `include/colMap.h` (line 80)

```cpp
[2]
```

Custom-specific fields.

Opaque client data. A pair of values is available to custom map types for iteration purpose.



**Type**: [Col\_ClientData](colibri_8h.md#group__basic__types_1ga52e127a5c635bcb88f252efd210ca1a5)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)