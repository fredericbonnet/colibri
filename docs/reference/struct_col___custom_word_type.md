<a id="struct_col___custom_word_type"></a>
# Structure Col\_CustomWordType

![][C++]
![][public]

**Definition**: `include/colWord.h` (line 244)

Custom word type descriptor.

**See also**: [Word Types](#group__words_1word_types)

## Members

* [childrenProc](struct_col___custom_word_type.md#struct_col___custom_word_type_1a5ea9bcfcae2ebadc971e394ba3c2be0f)
* [freeProc](struct_col___custom_word_type.md#struct_col___custom_word_type_1a15e8e2dd2cb2eedf153d89925a359712)
* [name](struct_col___custom_word_type.md#struct_col___custom_word_type_1a6eb4ecb778b2078bf6fd9f522fcdcae6)
* [sizeProc](struct_col___custom_word_type.md#struct_col___custom_word_type_1a19986fa33336cd268de17eba5412605c)
* [type](struct_col___custom_word_type.md#struct_col___custom_word_type_1af9482efe5a6408bc622320619c3ccf9f)

## Public attributes

<a id="struct_col___custom_word_type_1af9482efe5a6408bc622320619c3ccf9f"></a>
### Variable type

![][public]

**Definition**: `include/colWord.h` (line 246)

```cpp
int Col_CustomWordType::type
```

Type identifier.





**Type**: int

<a id="struct_col___custom_word_type_1a6eb4ecb778b2078bf6fd9f522fcdcae6"></a>
### Variable name

![][public]

**Definition**: `include/colWord.h` (line 249)

```cpp
const char* Col_CustomWordType::name
```

Name of the type, e.g. "int".





**Type**: const char *

<a id="struct_col___custom_word_type_1a19986fa33336cd268de17eba5412605c"></a>
### Variable sizeProc

![][public]

**Definition**: `include/colWord.h` (line 253)

```cpp
Col_CustomWordSizeProc* Col_CustomWordType::sizeProc
```

Called to get the size in bytes of the word.

Must be constant during the whole lifetime.



**Type**: [Col\_CustomWordSizeProc](col_word_8h.md#group__custom__words_1ga6f6dd3eac5e1ca1e3fba5878dd698dff) *

<a id="struct_col___custom_word_type_1a15e8e2dd2cb2eedf153d89925a359712"></a>
### Variable freeProc

![][public]

**Definition**: `include/colWord.h` (line 256)

```cpp
Col_CustomWordFreeProc* Col_CustomWordType::freeProc
```

Called once the word gets collected.

NULL if not needed.



**Type**: [Col\_CustomWordFreeProc](col_word_8h.md#group__custom__words_1ga73db405afd10df91bf70e5507fd63584) *

<a id="struct_col___custom_word_type_1a5ea9bcfcae2ebadc971e394ba3c2be0f"></a>
### Variable childrenProc

![][public]

**Definition**: `include/colWord.h` (line 260)

```cpp
Col_CustomWordChildrenProc* Col_CustomWordType::childrenProc
```

Called during the garbage collection to iterate over the words owned by the word, in no special order.

If NULL, do nothing.



**Type**: [Col\_CustomWordChildrenProc](col_word_8h.md#group__custom__words_1ga8806c7822f400375ca69bb879aece079) *

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)