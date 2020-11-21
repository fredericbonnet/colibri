<a id="struct_col___custom_hash_map_type"></a>
# Structure Col\_CustomHashMapType

![][C++]
![][public]

**Definition**: `include/colHash.h` (line 127)

Custom hash map type descriptor.

Inherits from [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type).







**See also**: [Col\_NewCustomHashMap](col_hash_8h.md#group__customhashmap__words_1gad516fa9041eb514e2c5193eb5d958f0e)

## Members

* [compareKeysProc](struct_col___custom_hash_map_type.md#struct_col___custom_hash_map_type_1adecb961bbf29ef0ff4b37ed97309d59d)
* [hashProc](struct_col___custom_hash_map_type.md#struct_col___custom_hash_map_type_1a571294a9fa0b5e0241986efaf50f5b37)
* [type](struct_col___custom_hash_map_type.md#struct_col___custom_hash_map_type_1aea81519da7ec0622ec1a598637c4e488)

## Public attributes

<a id="struct_col___custom_hash_map_type_1aea81519da7ec0622ec1a598637c4e488"></a>
### Variable type

![][public]

**Definition**: `include/colHash.h` (line 130)

```cpp
Col_CustomWordType Col_CustomHashMapType::type
```

Generic word type descriptor.

Type field must be equal to [COL\_HASHMAP](col_word_8h.md#group__words_1gae3509634e52a76014e96c2575b5d8092).



**Type**: [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type)

<a id="struct_col___custom_hash_map_type_1a571294a9fa0b5e0241986efaf50f5b37"></a>
### Variable hashProc

![][public]

**Definition**: `include/colHash.h` (line 133)

```cpp
Col_HashProc* Col_CustomHashMapType::hashProc
```

Called to compute the hash value of a key.





**Type**: [Col\_HashProc](col_hash_8h.md#group__customhashmap__words_1gac6b7003867d2534bcc1848e410c05458) *

<a id="struct_col___custom_hash_map_type_1adecb961bbf29ef0ff4b37ed97309d59d"></a>
### Variable compareKeysProc

![][public]

**Definition**: `include/colHash.h` (line 136)

```cpp
Col_HashCompareKeysProc* Col_CustomHashMapType::compareKeysProc
```

Called to compare keys.





**Type**: [Col\_HashCompareKeysProc](col_hash_8h.md#group__customhashmap__words_1gae157152eec296221c971af03b35d39fa) *

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)