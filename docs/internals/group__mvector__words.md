<a id="group__mvector__words"></a>
# Mutable Vectors

Mutable vectors are arrays of words that are directly accessible and modifiable through a pointer value, and whose length can vary up to a given capacity set at creation time; they can be"frozen" and turned into immutable vectors.

**Requirements**:


* Mutable vector words use the same basic structure as immutable vector words so that they can be turned immutable in-place ([frozen](col_vector_8h.md#group__mvector__words_1ga1d93d2437dc1066b87d82d49aec779f1)).

* In addition to immutable vector fields, mutable vectors must know their maximum capacity. It is deduced from the word size in cells minus the header. So we just have to know the word cell size. When frozen, unused trailing cells are freed.


**Parameters**:

* **Size**: Number of allocated cells. This determines the maximum length of the element array. Given the storage capacity, the maximum size of a mutable vector is smaller than that of immutable vectors (see [Col\_MaxMVectorLength()](col_vector_8h.md#group__mvector__words_1ga06679e6bd1bf66e8ed03a459c1bdf4fc) vs. [Col\_MaxVectorLength()](col_vector_8h.md#group__vector__words_1ga0f56e839c639b58511831658e6504ed7)).
* **Length**: [Generic immutable vector length field](col_vector_int_8h.md#group__vector__words_1ga926467c6e28cbec0b62107c2d17bb06c).
* **Elements**: [Generic immutable vector elements array](col_vector_int_8h.md#group__vector__words_1ga3a15150382d791225479cfbcad0c0e33).


**Cell Layout**:

On all architectures the cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        mvector_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_MVECTOR_SIZE" title="WORD_MVECTOR_SIZE" colspan="2">Size</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_VECTOR_LENGTH" title="WORD_VECTOR_LENGTH" colspan="4">Length</td>
            </tr>
            <tr><td sides="R">.</td>
                <td href="@ref WORD_VECTOR_ELEMENTS" title="WORD_VECTOR_ELEMENTS" colspan="4" rowspan="4">Elements</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }
    









**See also**: [Immutable Vectors](group__vector__words.md#group__vector__words), [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)

## Mutable Vector Creation

<a id="group__mvector__words_1ga06679e6bd1bf66e8ed03a459c1bdf4fc"></a>
### Function Col\_MaxMVectorLength

![][public]

```cpp
size_t Col_MaxMVectorLength()
```

Get the maximum length of a mutable vector word.

**Returns**:

The max mutable vector length.



**Parameters**:

* void

**Return type**: EXTERN size_t

**References**:

* [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)
* [MVECTOR\_MAX\_SIZE](col_vector_int_8h.md#group__mvector__words_1ga0833fb1bfe926c016c3e00ee41120baf)
* [VECTOR\_MAX\_LENGTH](col_vector_int_8h.md#group__vector__words_1ga2ccb6254308d96069f8187e7b55908d2)

**Referenced by**:

* [AllocBuckets](col_hash_8c.md#group__hashmap__words_1ga19e08b164be41d9b09cd4e10ba33ae27)

<a id="group__mvector__words_1ga5409a9871105f346b35ecd06d857e271"></a>
### Function Col\_NewMVector

![][public]

```cpp
Col_Word Col_NewMVector(size_t capacity, size_t length, const Col_Word *elements)
```

Create a new mutable vector word, and optionally populate with the given elements.

?> The actual capacity will be rounded up to fit allocated cells.


**Returns**:

The new word.

**Exceptions**:

* **[COL\_ERROR\_VECTORLENGTH](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35af25a0023745659d92b3ebd65d7c43bf3)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **capacity < [Col\_MaxMVectorLength()](col_vector_8h.md#group__mvector__words_1ga06679e6bd1bf66e8ed03a459c1bdf4fc)**: Vector too large.

**Parameters**:

* size_t **capacity**: Maximum length of mutable vector.
* size_t **length**: Length of below array.
* const [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **elements**: Array of words to populate vector with, or NULL. In the latter case, elements are initialized to nil.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)
* [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)
* [MVECTOR\_MAX\_SIZE](col_vector_int_8h.md#group__mvector__words_1ga0833fb1bfe926c016c3e00ee41120baf)
* [VALUECHECK\_VECTORLENGTH](col_vector_int_8h.md#group__vector__words_1ga6fdb8e4ad1f9c7819c61753ae41be410)
* [VECTOR\_MAX\_LENGTH](col_vector_int_8h.md#group__vector__words_1ga2ccb6254308d96069f8187e7b55908d2)
* [VECTOR\_SIZE](col_vector_int_8h.md#group__vector__words_1ga3859b99c15ff0bc766748dc0042ea026)
* [WORD\_MVECTOR\_INIT](col_vector_int_8h.md#group__mvector__words_1ga428c7e8ed7fc2dd4bb08a2327b424255)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)
* [WORD\_VECTOR\_ELEMENTS](col_vector_int_8h.md#group__vector__words_1ga3a15150382d791225479cfbcad0c0e33)

**Referenced by**:

* [AllocBuckets](col_hash_8c.md#group__hashmap__words_1ga19e08b164be41d9b09cd4e10ba33ae27)
* [ConvertToMutableAt](col_list_8c.md#group__mlist__words_1gaa3d6a25f3eb845452f465bb66f67ab51)
* [GrowHashMap](col_hash_8c.md#group__hashmap__words_1ga695416e52176dc22863ffc2e36f9d9bb)
* [GrowIntHashMap](col_hash_8c.md#group__hashmap__words_1ga65bb60fab52dac94df55edd80a5300de)
* [MListInsert](col_list_8c.md#group__mlist__words_1gaa62fa49dc7f988e33978dbf3e030ebf8)
* [MListRemove](col_list_8c.md#group__mlist__words_1ga7ebc7b3f5a8c87880fa31e51cde19c04)
* [SplitMutableAt](col_list_8c.md#group__mlist__words_1ga87232eb482742f39972b97a2f1084fed)

## Mutable Vector Accessors

<a id="group__mvector__words_1ga326d17e21d65bda7559f5aa403a443a4"></a>
### Function Col\_MVectorCapacity

![][public]

```cpp
size_t Col_MVectorCapacity(Col_Word mvector)
```

Get the capacity = maximum length of the mutable vector.

**Returns**:

The mutable vector capacity.



**See also**: [Col\_NewMVector](col_vector_8h.md#group__mvector__words_1ga5409a9871105f346b35ecd06d857e271)

**Exceptions**:

* **[COL\_ERROR\_MVECTOR](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35ade4eb60e2eb216ed151689929db31d1c)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **mvector**: Not a mutable vector.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **mvector**: Mutable vector to get capacity for.

**Return type**: EXTERN size_t

**References**:

* [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)
* [TYPECHECK\_MVECTOR](col_vector_int_8h.md#group__mvector__words_1ga4e6d3b341eea2ef09e38e4ea3aa1c51a)
* [VECTOR\_MAX\_LENGTH](col_vector_int_8h.md#group__vector__words_1ga2ccb6254308d96069f8187e7b55908d2)
* [WORD\_MVECTOR\_SIZE](col_vector_int_8h.md#group__mvector__words_1gae997d6e38dc29b20339e997afd28f168)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

<a id="group__mvector__words_1gaf1cc0243830459f8ab39ba6d47c4074d"></a>
### Function Col\_MVectorElements

![][public]

```cpp
Col_Word * Col_MVectorElements(Col_Word mvector)
```

Get the mutable vector element array.

**Returns**:

The mutable vector element array.



**See also**: [Col\_VectorLength](col_vector_8h.md#group__vector__words_1gacd86c23d01fb36cd61e924e7c2f1ce3d)

**Exceptions**:

* **[COL\_ERROR\_MVECTOR](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35ade4eb60e2eb216ed151689929db31d1c)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **mvector**: Not a mutable vector.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **mvector**: Mutable vector to get elements for.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *

**References**:

* [TYPECHECK\_MVECTOR](col_vector_int_8h.md#group__mvector__words_1ga4e6d3b341eea2ef09e38e4ea3aa1c51a)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)
* [WORD\_VECTOR\_ELEMENTS](col_vector_int_8h.md#group__vector__words_1ga3a15150382d791225479cfbcad0c0e33)

## Mutable Vector Operations

<a id="group__mvector__words_1ga549a713508a196e7ed9fb3cceb214e88"></a>
### Function Col\_MVectorSetLength

![][public]

```cpp
void Col_MVectorSetLength(Col_Word mvector, size_t length)
```

Resize the mutable vector.

Newly added elements are set to nil.

**Exceptions**:

* **[COL\_ERROR\_MVECTOR](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35ade4eb60e2eb216ed151689929db31d1c)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **mvector**: Not a mutable vector.
* **[COL\_ERROR\_VECTORLENGTH](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35af25a0023745659d92b3ebd65d7c43bf3)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **length < [Col\_MVectorCapacity(mvector)](col_vector_8h.md#group__mvector__words_1ga326d17e21d65bda7559f5aa403a443a4).**: Vector too large.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **mvector**: Mutable vector to resize.
* size_t **length**: New length. Must not exceed capacity set at creation time.

**Return type**: EXTERN void

**References**:

* [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)
* [TYPECHECK\_MVECTOR](col_vector_int_8h.md#group__mvector__words_1ga4e6d3b341eea2ef09e38e4ea3aa1c51a)
* [VALUECHECK\_VECTORLENGTH](col_vector_int_8h.md#group__vector__words_1ga6fdb8e4ad1f9c7819c61753ae41be410)
* [VECTOR\_MAX\_LENGTH](col_vector_int_8h.md#group__vector__words_1ga2ccb6254308d96069f8187e7b55908d2)
* [WORD\_MVECTOR\_SIZE](col_vector_int_8h.md#group__mvector__words_1gae997d6e38dc29b20339e997afd28f168)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)
* [WORD\_VECTOR\_ELEMENTS](col_vector_int_8h.md#group__vector__words_1ga3a15150382d791225479cfbcad0c0e33)
* [WORD\_VECTOR\_LENGTH](col_vector_int_8h.md#group__vector__words_1ga926467c6e28cbec0b62107c2d17bb06c)

**Referenced by**:

* [Col\_MListSetLength](col_list_8h.md#group__mlist__words_1ga939319b0240b3bd7f0713b09b4dc08dc)
* [MListInsert](col_list_8c.md#group__mlist__words_1gaa62fa49dc7f988e33978dbf3e030ebf8)
* [MListRemove](col_list_8c.md#group__mlist__words_1ga7ebc7b3f5a8c87880fa31e51cde19c04)
* [SplitMutableAt](col_list_8c.md#group__mlist__words_1ga87232eb482742f39972b97a2f1084fed)

<a id="group__mvector__words_1ga1d93d2437dc1066b87d82d49aec779f1"></a>
### Function Col\_MVectorFreeze

![][public]

```cpp
void Col_MVectorFreeze(Col_Word mvector)
```

Turn a mutable vector immutable.

Does nothing on immutable vectors.

**Exceptions**:

* **[COL\_ERROR\_VECTOR](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a887698395dc0643aa2b4c0863ff6d8d4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **mvector**: Not a vector.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **mvector**: Mutable vector to freeze.

**Return type**: EXTERN void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [TYPECHECK\_VECTOR](col_vector_int_8h.md#group__vector__words_1ga6c5b4dc68a4a0843121645728983cf04)
* [WORD\_PINNED](col_word_int_8h.md#group__regular__words_1gad20cf13be09a354418d8615e6f2f2193)
* [WORD\_SET\_PINNED](col_word_int_8h.md#group__regular__words_1ga7ae30ab27827ba70ae1265b5f637101a)
* [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_VECTOR](col_word_int_8h.md#group__words_1gadf6c66e5c2f9fcdf213ae40d253c153f)
* [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb)
* [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)
* [WORD\_WRAP\_SOURCE](col_word_int_8h.md#group__word__wrappers_1ga653feef2b80b11d5c3aae03bddbd422d)

**Referenced by**:

* [Col\_CopyHashMap](col_hash_8h.md#group__hashmap__words_1ga9ebda3b577662e8b1dcf9a227d106f22)

## Mutable Vector Constants

<a id="group__mvector__words_1ga0833fb1bfe926c016c3e00ee41120baf"></a>
### Macro MVECTOR\_MAX\_SIZE

![][public]

```cpp
#define MVECTOR_MAX_SIZE (SIZE_MAX>>CHAR_BIT)
```

Maximum cell size taken by mutable vectors.





## Mutable Vector Creation

<a id="group__mvector__words_1ga428c7e8ed7fc2dd4bb08a2327b424255"></a>
### Macro WORD\_MVECTOR\_INIT

![][public]

```cpp
#define WORD_MVECTOR_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)); \
    [WORD\_MVECTOR\_SET\_SIZE](col_vector_int_8h.md#group__mvector__words_1ga7d6edcf0df783c310c6c3c94962b6dea)((word), (size)); \
    [WORD\_VECTOR\_LENGTH](col_vector_int_8h.md#group__vector__words_1ga926467c6e28cbec0b62107c2d17bb06c)(word) = (length);( word ,size ,length )
```

Mutable vector word initializer.

**Parameters**:

* **word**: Word to initialize.
* **size**: [WORD\_MVECTOR\_SET\_SIZE](col_vector_int_8h.md#group__mvector__words_1ga7d6edcf0df783c310c6c3c94962b6dea).
* **length**: [WORD\_VECTOR\_LENGTH](col_vector_int_8h.md#group__vector__words_1ga926467c6e28cbec0b62107c2d17bb06c).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)



## Mutable Vector Accessors

<a id="group__mvector__words_1ga385524b2a928c4be5f2dacfb67b91461"></a>
### Macro MVECTOR\_SIZE\_MASK

![][public]

```cpp
#define MVECTOR_SIZE_MASK [MVECTOR\_MAX\_SIZE](col_vector_int_8h.md#group__mvector__words_1ga0833fb1bfe926c016c3e00ee41120baf)
```

Bitmask for cell size.





<a id="group__mvector__words_1gae997d6e38dc29b20339e997afd28f168"></a>
### Macro WORD\_MVECTOR\_SIZE

![][public]

```cpp
#define WORD_MVECTOR_SIZE ((((size_t *)(word))[0])&[MVECTOR\_SIZE\_MASK](col_vector_int_8h.md#group__mvector__words_1ga385524b2a928c4be5f2dacfb67b91461))( word )
```

Get the number of allocated cells.

**Parameters**:

* **word**: Word to access.



**See also**: [WORD\_MVECTOR\_SET\_SIZE](col_vector_int_8h.md#group__mvector__words_1ga7d6edcf0df783c310c6c3c94962b6dea)



<a id="group__mvector__words_1ga7d6edcf0df783c310c6c3c94962b6dea"></a>
### Macro WORD\_MVECTOR\_SET\_SIZE

![][public]

```cpp
#define WORD_MVECTOR_SET_SIZE ((((size_t *)(word))[0])&=~[MVECTOR\_SIZE\_MASK](col_vector_int_8h.md#group__mvector__words_1ga385524b2a928c4be5f2dacfb67b91461),(((size_t *)(word))[0])|=((size)&[MVECTOR\_SIZE\_MASK](col_vector_int_8h.md#group__mvector__words_1ga385524b2a928c4be5f2dacfb67b91461)))( word ,size )
```

Set the number of allocated cells.

**Parameters**:

* **word**: Word to access.
* **size**: Size of word in cells.


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.




**See also**: [WORD\_MVECTOR\_SIZE](col_vector_int_8h.md#group__mvector__words_1gae997d6e38dc29b20339e997afd28f168), [WORD\_MVECTOR\_INIT](col_vector_int_8h.md#group__mvector__words_1ga428c7e8ed7fc2dd4bb08a2327b424255)



## Mutable Vector Exceptions

<a id="group__mvector__words_1ga4e6d3b341eea2ef09e38e4ea3aa1c51a"></a>
### Macro TYPECHECK\_MVECTOR

![][public]

```cpp
#define TYPECHECK_MVECTOR( word )
```

Type checking macro for mutable vectors.

**Parameters**:

* **word**: Checked word.


**Exceptions**:

* **[COL\_ERROR\_MVECTOR](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35ade4eb60e2eb216ed151689929db31d1c)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **word**: Not a mutable vector.



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)