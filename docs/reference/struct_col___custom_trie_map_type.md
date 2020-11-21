<a id="struct_col___custom_trie_map_type"></a>
# Structure Col\_CustomTrieMapType

![][C++]
![][public]

**Definition**: `include/colTrie.h` (line 136)

Custom trie map type descriptor.

Inherits from [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type).







**See also**: [Col\_NewCustomTrieMap](col_trie_8h.md#group__customtriemap__words_1ga18de761037e23e723d8d62aef7d6246c)

## Members

* [bitTestProc](struct_col___custom_trie_map_type.md#struct_col___custom_trie_map_type_1a6a4b77d4f98512af17e9a2017fdabd40)
* [keyDiffProc](struct_col___custom_trie_map_type.md#struct_col___custom_trie_map_type_1a9938752c43cc037848a4f3b62542bb08)
* [type](struct_col___custom_trie_map_type.md#struct_col___custom_trie_map_type_1a5840c121ff181e25d59fb5ebccc5a03a)

## Public attributes

<a id="struct_col___custom_trie_map_type_1a5840c121ff181e25d59fb5ebccc5a03a"></a>
### Variable type

![][public]

**Definition**: `include/colTrie.h` (line 139)

```cpp
Col_CustomWordType Col_CustomTrieMapType::type
```

Generic word type descriptor.

Type field must be equal to [COL\_TRIEMAP](col_word_8h.md#group__words_1ga7922babbc856f5670805da2267d72ff0).



**Type**: [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type)

<a id="struct_col___custom_trie_map_type_1a6a4b77d4f98512af17e9a2017fdabd40"></a>
### Variable bitTestProc

![][public]

**Definition**: `include/colTrie.h` (line 142)

```cpp
Col_TrieBitTestProc* Col_CustomTrieMapType::bitTestProc
```

Called to test key bits.





**Type**: [Col\_TrieBitTestProc](col_trie_8h.md#group__customtriemap__words_1gac7ba97b901a90c7f2532fb233d01a883) *

<a id="struct_col___custom_trie_map_type_1a9938752c43cc037848a4f3b62542bb08"></a>
### Variable keyDiffProc

![][public]

**Definition**: `include/colTrie.h` (line 145)

```cpp
Col_TrieKeyDiffProc* Col_CustomTrieMapType::keyDiffProc
```

Called to compare keys.





**Type**: [Col\_TrieKeyDiffProc](col_trie_8h.md#group__customtriemap__words_1ga2277ebb3a3d4bee177de042ad194198d) *

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)