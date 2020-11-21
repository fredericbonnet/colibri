<a id="struct_col___custom_map_type"></a>
# Structure Col\_CustomMapType

![][C++]
![][public]

**Definition**: `include/colMap.h` (line 462)

Custom map type descriptor.

Inherits from [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type).

## Members

* [getProc](struct_col___custom_map_type.md#struct_col___custom_map_type_1a2d74b10090dfda067455420b3b66ba55)
* [iterBeginProc](struct_col___custom_map_type.md#struct_col___custom_map_type_1a55fb7df54aee17e43f4300aee820504f)
* [iterFindProc](struct_col___custom_map_type.md#struct_col___custom_map_type_1a7284c90da9812b36a4bc22a7291e457e)
* [iterGetKeyProc](struct_col___custom_map_type.md#struct_col___custom_map_type_1ab1b9c1f350b6eae05efc248dc4fed4e0)
* [iterGetValueProc](struct_col___custom_map_type.md#struct_col___custom_map_type_1ad5683dc97da63b51a4038253f7416274)
* [iterNextProc](struct_col___custom_map_type.md#struct_col___custom_map_type_1aad9b00aadb7d6f5531108b70c80054c0)
* [iterSetValueProc](struct_col___custom_map_type.md#struct_col___custom_map_type_1a54875c361728a10c54de45016f8dc666)
* [setProc](struct_col___custom_map_type.md#struct_col___custom_map_type_1a9b7315ab84fd55ba3f2f9199b180481e)
* [sizeProc](struct_col___custom_map_type.md#struct_col___custom_map_type_1ad938ed04f757ff8710f92f806c6ab684)
* [type](struct_col___custom_map_type.md#struct_col___custom_map_type_1aaf00fcf33d181f450a55261f2814eb77)
* [unsetProc](struct_col___custom_map_type.md#struct_col___custom_map_type_1ae0ee2e1af0ac42e9914cc3f6b8cd842f)

## Public attributes

<a id="struct_col___custom_map_type_1aaf00fcf33d181f450a55261f2814eb77"></a>
### Variable type

![][public]

**Definition**: `include/colMap.h` (line 464)

```cpp
Col_CustomWordType Col_CustomMapType::type
```

Generic word type descriptor.

Type field must be equal to [COL\_MAP](col_word_8h.md#group__words_1ga42912f858f54a3ebfeef2ede9422248c).



**Type**: [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type)

<a id="struct_col___custom_map_type_1ad938ed04f757ff8710f92f806c6ab684"></a>
### Variable sizeProc

![][public]

**Definition**: `include/colMap.h` (line 467)

```cpp
Col_MapSizeProc* Col_CustomMapType::sizeProc
```

Called to get map size.





**Type**: [Col\_MapSizeProc](col_map_8h.md#group__custommap__words_1ga3fa49bef2d9acca19f61e811fd480117) *

<a id="struct_col___custom_map_type_1a2d74b10090dfda067455420b3b66ba55"></a>
### Variable getProc

![][public]

**Definition**: `include/colMap.h` (line 470)

```cpp
Col_MapGetProc* Col_CustomMapType::getProc
```

Called to get entry value.





**Type**: [Col\_MapGetProc](col_map_8h.md#group__custommap__words_1gafe7352ecf3f5606bac94ad65c410435b) *

<a id="struct_col___custom_map_type_1a9b7315ab84fd55ba3f2f9199b180481e"></a>
### Variable setProc

![][public]

**Definition**: `include/colMap.h` (line 473)

```cpp
Col_MapSetProc* Col_CustomMapType::setProc
```

Called to set entry value.





**Type**: [Col\_MapSetProc](col_map_8h.md#group__custommap__words_1ga115ec8d5572154663268a86a4ab236be) *

<a id="struct_col___custom_map_type_1ae0ee2e1af0ac42e9914cc3f6b8cd842f"></a>
### Variable unsetProc

![][public]

**Definition**: `include/colMap.h` (line 476)

```cpp
Col_MapUnsetProc* Col_CustomMapType::unsetProc
```

Called to unset entry.





**Type**: [Col\_MapUnsetProc](col_map_8h.md#group__custommap__words_1ga8be5736ce93508cd5be4aa3e33aafee2) *

<a id="struct_col___custom_map_type_1a55fb7df54aee17e43f4300aee820504f"></a>
### Variable iterBeginProc

![][public]

**Definition**: `include/colMap.h` (line 479)

```cpp
Col_MapIterBeginProc* Col_CustomMapType::iterBeginProc
```

Called to begin iteration.





**Type**: [Col\_MapIterBeginProc](col_map_8h.md#group__custommap__words_1gab5110c2d8072d001d4b407c017a9c376) *

<a id="struct_col___custom_map_type_1a7284c90da9812b36a4bc22a7291e457e"></a>
### Variable iterFindProc

![][public]

**Definition**: `include/colMap.h` (line 482)

```cpp
Col_MapIterFindProc* Col_CustomMapType::iterFindProc
```

Called to begin iteration at given key.





**Type**: [Col\_MapIterFindProc](col_map_8h.md#group__custommap__words_1ga9874161b1f347341158556c38ff8284b) *

<a id="struct_col___custom_map_type_1aad9b00aadb7d6f5531108b70c80054c0"></a>
### Variable iterNextProc

![][public]

**Definition**: `include/colMap.h` (line 485)

```cpp
Col_MapIterNextProc* Col_CustomMapType::iterNextProc
```

Called to continue iteration.

<br/>



**Type**: [Col\_MapIterNextProc](col_map_8h.md#group__custommap__words_1ga0cb64c06139ba1b7db784b169d46c7d1) *

<a id="struct_col___custom_map_type_1ab1b9c1f350b6eae05efc248dc4fed4e0"></a>
### Variable iterGetKeyProc

![][public]

**Definition**: `include/colMap.h` (line 488)

```cpp
Col_MapIterGetKeyProc* Col_CustomMapType::iterGetKeyProc
```

Called to get iterator key.





**Type**: [Col\_MapIterGetKeyProc](col_map_8h.md#group__custommap__words_1ga76b02a84919d214db7c9cf0d0a704cbb) *

<a id="struct_col___custom_map_type_1ad5683dc97da63b51a4038253f7416274"></a>
### Variable iterGetValueProc

![][public]

**Definition**: `include/colMap.h` (line 491)

```cpp
Col_MapIterGetValueProc* Col_CustomMapType::iterGetValueProc
```

Called to get iterator value.





**Type**: [Col\_MapIterGetValueProc](col_map_8h.md#group__custommap__words_1ga579a529afa2ce53d16e6e9ae6e2dab99) *

<a id="struct_col___custom_map_type_1a54875c361728a10c54de45016f8dc666"></a>
### Variable iterSetValueProc

![][public]

**Definition**: `include/colMap.h` (line 494)

```cpp
Col_MapIterSetValueProc* Col_CustomMapType::iterSetValueProc
```

Called to set iterator value.





**Type**: [Col\_MapIterSetValueProc](col_map_8h.md#group__custommap__words_1ga7ea9085242a94376ecd5967bcdca7f11) *

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)