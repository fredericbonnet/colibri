<a id="struct_col___custom_int_map_type"></a>
# Structure Col\_CustomIntMapType

![][C++]
![][public]

**Definition**: `include/colMap.h` (line 500)

Custom integer map type descriptor.

Inherits from [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type).

## Members

* [getProc](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type_1a1182d33461048ba4798b717b4785f44c)
* [iterBeginProc](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type_1a2d9505a1ba4092d56be0f1d5aa9330d9)
* [iterFindProc](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type_1a324b123a07214cdf626ce6e09e9aecef)
* [iterGetKeyProc](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type_1a15d5cc8018de195af5728bb60a89614a)
* [iterGetValueProc](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type_1a58a82c69b042d896adc4f3aac21efca3)
* [iterNextProc](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type_1af1d11a892a1b6cb3af437aaee1ce31a3)
* [iterSetValueProc](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type_1a0d6d7e37ad765759c2b48e8d5e347805)
* [setProc](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type_1ac99aea50115d944d3ad83554b0a34bf0)
* [sizeProc](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type_1a4a60028002e0dae0dc3d43e7bb75f5d6)
* [type](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type_1a930e99e2a9f936817aeb46a2de0d34cc)
* [unsetProc](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type_1ac50a20eb9c006e88fa88f173f44bcddd)

## Public attributes

<a id="struct_col___custom_int_map_type_1a930e99e2a9f936817aeb46a2de0d34cc"></a>
### Variable type

![][public]

**Definition**: `include/colMap.h` (line 503)

```cpp
Col_CustomWordType Col_CustomIntMapType::type
```

Generic word type descriptor.

Type field must be equal to [COL\_INTMAP](col_word_8h.md#group__words_1ga0938add7b6f34338e9c7bc847a6b9b2f).



**Type**: [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type)

<a id="struct_col___custom_int_map_type_1a4a60028002e0dae0dc3d43e7bb75f5d6"></a>
### Variable sizeProc

![][public]

**Definition**: `include/colMap.h` (line 506)

```cpp
Col_MapSizeProc* Col_CustomIntMapType::sizeProc
```

Called to get map size.





**Type**: [Col\_MapSizeProc](col_map_8h.md#group__custommap__words_1ga3fa49bef2d9acca19f61e811fd480117) *

<a id="struct_col___custom_int_map_type_1a1182d33461048ba4798b717b4785f44c"></a>
### Variable getProc

![][public]

**Definition**: `include/colMap.h` (line 509)

```cpp
Col_IntMapGetProc* Col_CustomIntMapType::getProc
```

Called to get entry value.





**Type**: [Col\_IntMapGetProc](col_map_8h.md#group__custommap__words_1gae8b01496e5dec9f2d4c5f2d465d84fce) *

<a id="struct_col___custom_int_map_type_1ac99aea50115d944d3ad83554b0a34bf0"></a>
### Variable setProc

![][public]

**Definition**: `include/colMap.h` (line 512)

```cpp
Col_IntMapSetProc* Col_CustomIntMapType::setProc
```

Called to set entry value.





**Type**: [Col\_IntMapSetProc](col_map_8h.md#group__custommap__words_1ga133f5965501d18c114b195ba153d29f4) *

<a id="struct_col___custom_int_map_type_1ac50a20eb9c006e88fa88f173f44bcddd"></a>
### Variable unsetProc

![][public]

**Definition**: `include/colMap.h` (line 515)

```cpp
Col_IntMapUnsetProc* Col_CustomIntMapType::unsetProc
```

Called to unset entry.





**Type**: [Col\_IntMapUnsetProc](col_map_8h.md#group__custommap__words_1gaf2fe195205ae8c8f962be73194cc8e5a) *

<a id="struct_col___custom_int_map_type_1a2d9505a1ba4092d56be0f1d5aa9330d9"></a>
### Variable iterBeginProc

![][public]

**Definition**: `include/colMap.h` (line 518)

```cpp
Col_MapIterBeginProc* Col_CustomIntMapType::iterBeginProc
```

Called to begin iteration.





**Type**: [Col\_MapIterBeginProc](col_map_8h.md#group__custommap__words_1gab5110c2d8072d001d4b407c017a9c376) *

<a id="struct_col___custom_int_map_type_1a324b123a07214cdf626ce6e09e9aecef"></a>
### Variable iterFindProc

![][public]

**Definition**: `include/colMap.h` (line 521)

```cpp
Col_IntMapIterFindProc* Col_CustomIntMapType::iterFindProc
```

Called to begin iteration at given key.





**Type**: [Col\_IntMapIterFindProc](col_map_8h.md#group__custommap__words_1gadef056ea16828d598f5702a85988da3f) *

<a id="struct_col___custom_int_map_type_1af1d11a892a1b6cb3af437aaee1ce31a3"></a>
### Variable iterNextProc

![][public]

**Definition**: `include/colMap.h` (line 524)

```cpp
Col_MapIterNextProc* Col_CustomIntMapType::iterNextProc
```

Called to continue iteration.





**Type**: [Col\_MapIterNextProc](col_map_8h.md#group__custommap__words_1ga0cb64c06139ba1b7db784b169d46c7d1) *

<a id="struct_col___custom_int_map_type_1a15d5cc8018de195af5728bb60a89614a"></a>
### Variable iterGetKeyProc

![][public]

**Definition**: `include/colMap.h` (line 527)

```cpp
Col_IntMapIterGetKeyProc* Col_CustomIntMapType::iterGetKeyProc
```

Called to get iterator key.





**Type**: [Col\_IntMapIterGetKeyProc](col_map_8h.md#group__custommap__words_1ga3633e15000b5316b94e153b8c57bfb67) *

<a id="struct_col___custom_int_map_type_1a58a82c69b042d896adc4f3aac21efca3"></a>
### Variable iterGetValueProc

![][public]

**Definition**: `include/colMap.h` (line 530)

```cpp
Col_MapIterGetValueProc* Col_CustomIntMapType::iterGetValueProc
```

Called to get iterator value.





**Type**: [Col\_MapIterGetValueProc](col_map_8h.md#group__custommap__words_1ga579a529afa2ce53d16e6e9ae6e2dab99) *

<a id="struct_col___custom_int_map_type_1a0d6d7e37ad765759c2b48e8d5e347805"></a>
### Variable iterSetValueProc

![][public]

**Definition**: `include/colMap.h` (line 533)

```cpp
Col_MapIterSetValueProc* Col_CustomIntMapType::iterSetValueProc
```

Called to set iterator value.





**Type**: [Col\_MapIterSetValueProc](col_map_8h.md#group__custommap__words_1ga7ea9085242a94376ecd5967bcdca7f11) *

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)