<a id="group__custom__words"></a>
# Custom Words

Custom words are words defined by applicative code.

They can extend existing word types like ropes or maps, or define application-specific data types.







**See also**: [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type)

## Submodules

* [Custom Hash Maps](group__customhashmap__words.md#group__customhashmap__words)
* [Custom Lists](group__customlist__words.md#group__customlist__words)
* [Custom Maps](group__custommap__words.md#group__custommap__words)
* [Custom Ropes](group__customrope__words.md#group__customrope__words)
* [Custom Trie Maps](group__customtriemap__words.md#group__customtriemap__words)

## Custom Word Type Descriptors

<a id="group__custom__words_1ga6f6dd3eac5e1ca1e3fba5878dd698dff"></a>
### Typedef Col\_CustomWordSizeProc

![][public]

**Definition**: `include/colWord.h` (line 198)

```cpp
typedef size_t() Col_CustomWordSizeProc(Col_Word word)
```

Function signature of custom word size procs.

**Parameters**:

* **word**: Custom word to get size for.


**Returns**:

The custom word size in bytes.



**See also**: [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type)



**Return type**: size_t()

<a id="group__custom__words_1ga73db405afd10df91bf70e5507fd63584"></a>
### Typedef Col\_CustomWordFreeProc

![][public]

**Definition**: `include/colWord.h` (line 208)

```cpp
typedef void() Col_CustomWordFreeProc(Col_Word word)
```

Function signature of custom word cleanup procs.

Called on collected words during the sweep phase of the garbage collection.






**Parameters**:

* **word**: Custom word to cleanup.



**See also**: [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type)



**Return type**: void()

<a id="group__custom__words_1ga571dc0dc48421f8e7c6578c6f5f0e827"></a>
### Typedef Col\_CustomWordChildEnumProc

![][public]

**Definition**: `include/colWord.h` (line 223)

```cpp
typedef void() Col_CustomWordChildEnumProc(Col_Word word, Col_Word *childPtr, Col_ClientData clientData)
```

Function signature of custom word child enumeration procs.

Called during the mark phase of the garbage collection. Words are movable, so pointer values may be modified in the process.






**Parameters**:

* **word**: Custom word whose child is being followed.
* **childPtr**: Pointer to child, may be overwritten if moved.
* **clientData**: Opaque client data. Same value as passed to [Col\_CustomWordChildrenProc()](col_word_8h.md#group__custom__words_1ga8806c7822f400375ca69bb879aece079).




**See also**: [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type), [Col\_CustomWordChildrenProc](col_word_8h.md#group__custom__words_1ga8806c7822f400375ca69bb879aece079)



**Return type**: void()

<a id="group__custom__words_1ga8806c7822f400375ca69bb879aece079"></a>
### Typedef Col\_CustomWordChildrenProc

![][public]

**Definition**: `include/colWord.h` (line 236)

```cpp
typedef void() Col_CustomWordChildrenProc(Col_Word word, Col_CustomWordChildEnumProc *proc, Col_ClientData clientData)
```

Function signature of custom word child enumeration procs.

Called during the mark phase of the garbage collection.






**Parameters**:

* **word**: Custom word to follow children for.
* **proc**: Callback proc called at each child.
* **clientData**: Opaque data passed as is to above proc.




**See also**: [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type), [Col\_CustomWordChildEnumProc](col_word_8h.md#group__custom__words_1ga571dc0dc48421f8e7c6578c6f5f0e827)



**Return type**: void()

## Custom Word Creation

<a id="group__custom__words_1gaf9a6d324967159ae7abeb41a3d59cc79"></a>
### Function Col\_NewCustomWord

![][public]

```cpp
Col_Word Col_NewCustomWord(Col_CustomWordType *type, size_t size, void **dataPtr)
```

Create a new custom word.

**Returns**:

A new word of the given size.



**Parameters**:

* [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type) * **type**: The word type descriptor.
* size_t **size**: Size of custom data.
* void ** **dataPtr**: [out] Points to the allocated custom data.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

## Custom Word Accessors

<a id="group__custom__words_1gaaa55265553c564df20fc33dfe9153527"></a>
### Function Col\_CustomWordInfo

![][public]

```cpp
Col_CustomWordType * Col_CustomWordInfo(Col_Word word, void **dataPtr)
```

Get custom word type and data.

**Returns**:

The custom word type descriptor.



**See also**: [Col\_NewCustomWord](col_word_8h.md#group__custom__words_1gaf9a6d324967159ae7abeb41a3d59cc79)

**Exceptions**:

* **[COL\_ERROR\_CUSTOMWORD](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aeaf92bf356a97fb87088bd24e551411c)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **word**: Not a custom word.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **word**: The word to get data for.
* void ** **dataPtr**: [out] Points to the word's custom data.

**Return type**: EXTERN [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type) *

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[private]: https://img.shields.io/badge/-private-red (private)
[static]: https://img.shields.io/badge/-static-lightgrey (static)