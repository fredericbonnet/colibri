<a id="col_vector_8h"></a>
# File colVector.h

![][C++]

**Location**: `include/colVector.h`

This header file defines the vector handling features of Colibri.

Vectors are arrays of words that are directly accessible through a pointer value.





They come in both immutable and mutable forms.

## Includes

* <stddef.h>
* <stdarg.h>

```mermaid
graph LR
3["stdarg.h"]

1["include/colVector.h"]
click 1 "col_vector_8h.md#col_vector_8h"
1 --> 2
1 --> 3

2["stddef.h"]

```

## Included by

* [include/colibri.h](colibri_8h.md#colibri_8h)

```mermaid
graph RL
10["src/colStrBuf.c"]
click 10 "col_str_buf_8c.md#col_str_buf_8c"

13["src/colWord.c"]
click 13 "col_word_8c.md#col_word_8c"

4["src/colGc.c"]
click 4 "col_gc_8c.md#col_gc_8c"

1["include/colVector.h"]
click 1 "col_vector_8h.md#col_vector_8h"
2 --> 1

15["src/platform/win32/colWin32Platform.c"]
click 15 "col_win32_platform_8c.md#col_win32_platform_8c"

7["src/colList.c"]
click 7 "col_list_8c.md#col_list_8c"

5["src/colHash.c"]
click 5 "col_hash_8c.md#col_hash_8c"

8["src/colMap.c"]
click 8 "col_map_8c.md#col_map_8c"

2["include/colibri.h"]
click 2 "colibri_8h.md#colibri_8h"
3 --> 2
4 --> 2
5 --> 2
6 --> 2
7 --> 2
8 --> 2
9 --> 2
10 --> 2
11 --> 2
12 --> 2
13 --> 2
14 --> 2
15 --> 2

11["src/colTrie.c"]
click 11 "col_trie_8c.md#col_trie_8c"

6["src/colibri.c"]
click 6 "colibri_8c.md#colibri_8c"

3["src/colAlloc.c"]
click 3 "col_alloc_8c.md#col_alloc_8c"

12["src/colVector.c"]
click 12 "col_vector_8c.md#col_vector_8c"

14["src/platform/unix/colUnixPlatform.c"]
click 14 "col_unix_platform_8c.md#col_unix_platform_8c"

9["src/colRope.c"]
click 9 "col_rope_8c.md#col_rope_8c"

```

## Immutable Vector Creation

<a id="group__vector__words_1ga35b8a2126b4cddb7f57ae71634fdc61c"></a>
### Macro Col\_NewVectorV

![][public]

```cpp
#define Col_NewVectorV     _Col_NewVectorV(_, first, ##__VA_ARGS__)( first ,... )
```

Variadic macro version of [Col\_NewVectorNV()](col_vector_8h.md#group__vector__words_1gaa56f743590ca8867765f48e31e8a4df9) that deduces its number of arguments automatically.

**Parameters**:

* **first**: First word to add.
* **...**: Next words to add.



**See also**: [COL\_ARGCOUNT](col_utils_8h.md#group__utils_1gabbb0e58841406f54d444d40625a2c4fe)



<a id="group__vector__words_1ga0f56e839c639b58511831658e6504ed7"></a>
### Function Col\_MaxVectorLength

![][public]

```cpp
size_t Col_MaxVectorLength()
```

Get the maximum length of a vector word.

**Returns**:

The max vector length.



**Parameters**:

* void

**Return type**: EXTERN size_t

**References**:

* [VECTOR\_MAX\_LENGTH](col_vector_int_8h.md#group__vector__words_1ga2ccb6254308d96069f8187e7b55908d2)
* [VECTOR\_MAX\_SIZE](col_vector_int_8h.md#group__vector__words_1ga150df031108068f0c8b0a72231999cfa)

<a id="group__vector__words_1ga6ef7d35d75fdc6a6781f0a32e9c7efc1"></a>
### Function Col\_NewVector

![][public]

```cpp
Col_Word Col_NewVector(size_t length, const Col_Word *elements)
```

Create a new vector word.

**Returns**:

The new word.

**Exceptions**:

* **[COL\_ERROR\_VECTORLENGTH](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35af25a0023745659d92b3ebd65d7c43bf3)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **length < [Col\_MaxVectorLength()](col_vector_8h.md#group__vector__words_1ga0f56e839c639b58511831658e6504ed7)**: Vector too large.

**Parameters**:

* size_t **length**: Length of below array.
* const [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **elements**: Array of words to populate vector with, or NULL. In the latter case, elements are initialized to nil.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)
* [VALUECHECK\_VECTORLENGTH](col_vector_int_8h.md#group__vector__words_1ga6fdb8e4ad1f9c7819c61753ae41be410)
* [VECTOR\_MAX\_LENGTH](col_vector_int_8h.md#group__vector__words_1ga2ccb6254308d96069f8187e7b55908d2)
* [VECTOR\_MAX\_SIZE](col_vector_int_8h.md#group__vector__words_1ga150df031108068f0c8b0a72231999cfa)
* [VECTOR\_SIZE](col_vector_int_8h.md#group__vector__words_1ga3859b99c15ff0bc766748dc0042ea026)
* [WORD\_LIST\_EMPTY](col_word_int_8h.md#group__voidlist__words_1ga8b7c20d2cdcdf8f3bc58589d757cf53b)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)
* [WORD\_VECTOR\_ELEMENTS](col_vector_int_8h.md#group__vector__words_1ga3a15150382d791225479cfbcad0c0e33)
* [WORD\_VECTOR\_INIT](col_vector_int_8h.md#group__vector__words_1gace3048c9e9c238e1405c64c19228212d)

**Referenced by**:

* [Col\_NewList](col_list_8h.md#group__list__words_1ga0dc0d4d58c4e694683753211151b0fc7)

<a id="group__vector__words_1gaa56f743590ca8867765f48e31e8a4df9"></a>
### Function Col\_NewVectorNV

![][public]

```cpp
Col_Word Col_NewVectorNV(size_t length, ...)
```

Create a new vector word from a list of arguments.

**Returns**:

The new word.

**Exceptions**:

* **[COL\_ERROR\_VECTORLENGTH](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35af25a0023745659d92b3ebd65d7c43bf3)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **length < [Col\_MaxVectorLength()](col_vector_8h.md#group__vector__words_1ga0f56e839c639b58511831658e6504ed7)**: Vector too large.

**Parameters**:

* size_t **length**: Number of arguments following.
* ......: Words to add in order.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)
* [VALUECHECK\_VECTORLENGTH](col_vector_int_8h.md#group__vector__words_1ga6fdb8e4ad1f9c7819c61753ae41be410)
* [VECTOR\_MAX\_LENGTH](col_vector_int_8h.md#group__vector__words_1ga2ccb6254308d96069f8187e7b55908d2)
* [VECTOR\_MAX\_SIZE](col_vector_int_8h.md#group__vector__words_1ga150df031108068f0c8b0a72231999cfa)
* [VECTOR\_SIZE](col_vector_int_8h.md#group__vector__words_1ga3859b99c15ff0bc766748dc0042ea026)
* [WORD\_LIST\_EMPTY](col_word_int_8h.md#group__voidlist__words_1ga8b7c20d2cdcdf8f3bc58589d757cf53b)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)
* [WORD\_VECTOR\_ELEMENTS](col_vector_int_8h.md#group__vector__words_1ga3a15150382d791225479cfbcad0c0e33)
* [WORD\_VECTOR\_INIT](col_vector_int_8h.md#group__vector__words_1gace3048c9e9c238e1405c64c19228212d)

## Immutable Vector Accessors

<a id="group__vector__words_1gacd86c23d01fb36cd61e924e7c2f1ce3d"></a>
### Function Col\_VectorLength

![][public]

```cpp
size_t Col_VectorLength(Col_Word vector)
```

Get the length of the vector.

**Returns**:

The vector length.




**See also**: [Col\_NewVector](col_vector_8h.md#group__vector__words_1ga6ef7d35d75fdc6a6781f0a32e9c7efc1), [Col\_VectorElements](col_vector_8h.md#group__vector__words_1ga8067c81425c7b94800fd9f64a2fc2ec6)

**Exceptions**:

* **[COL\_ERROR\_VECTOR](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a887698395dc0643aa2b4c0863ff6d8d4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **vector**: Not a vector.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **vector**: Vector to get length for.

**Return type**: EXTERN size_t

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [TYPECHECK\_VECTOR](col_vector_int_8h.md#group__vector__words_1ga6c5b4dc68a4a0843121645728983cf04)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_VECTOR](col_word_int_8h.md#group__words_1gadf6c66e5c2f9fcdf213ae40d253c153f)
* [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)
* [WORD\_VECTOR\_LENGTH](col_vector_int_8h.md#group__vector__words_1ga926467c6e28cbec0b62107c2d17bb06c)
* [WORD\_VOIDLIST\_LENGTH](col_word_int_8h.md#group__voidlist__words_1gaa336b44598700785e9f948fdd3a0da58)

<a id="group__vector__words_1ga8067c81425c7b94800fd9f64a2fc2ec6"></a>
### Function Col\_VectorElements

![][public]

```cpp
const Col_Word * Col_VectorElements(Col_Word vector)
```

Get the vector element array.

**Returns**:

The vector element array.




**See also**: [Col\_NewVector](col_vector_8h.md#group__vector__words_1ga6ef7d35d75fdc6a6781f0a32e9c7efc1), [Col\_VectorLength](col_vector_8h.md#group__vector__words_1gacd86c23d01fb36cd61e924e7c2f1ce3d)

**Exceptions**:

* **[COL\_ERROR\_VECTOR](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a887698395dc0643aa2b4c0863ff6d8d4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **vector**: Not a vector.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **vector**: Vector to get elements for.

**Return type**: EXTERN const [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [TYPECHECK\_VECTOR](col_vector_int_8h.md#group__vector__words_1ga6c5b4dc68a4a0843121645728983cf04)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_VECTOR](col_word_int_8h.md#group__words_1gadf6c66e5c2f9fcdf213ae40d253c153f)
* [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)
* [WORD\_VECTOR\_ELEMENTS](col_vector_int_8h.md#group__vector__words_1ga3a15150382d791225479cfbcad0c0e33)
* [WORD\_VOIDLIST\_LENGTH](col_word_int_8h.md#group__voidlist__words_1gaa336b44598700785e9f948fdd3a0da58)

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

## Source

```cpp
/**
 * @file colVector.h
 *
 * This header file defines the vector handling features of Colibri.
 *
 * Vectors are arrays of words that are directly accessible through a
 * pointer value.
 *
 * They come in both immutable and mutable forms.
 */

#ifndef _COLIBRI_VECTOR
#define _COLIBRI_VECTOR

#include <stddef.h> /* For size_t */
#include <stdarg.h> /* For variadic procs */


/*
===========================================================================*//*!
\defgroup vector_words Immutable Vectors
\ingroup words

Immutable vectors are constant, fixed-length arrays of words that are directly
accessible through a pointer value.
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Immutable Vector Creation
 ***************************************************************************\{*/

/**
 * Variadic macro version of Col_NewVectorNV() that deduces its number
 * of arguments automatically.
 *
 * @param first First word to add.
 * @param ...   Next words to add.
 * 
 * @see COL_ARGCOUNT
 */
#define Col_NewVectorV(first, ...) \
    _Col_NewVectorV(_, first, ##__VA_ARGS__)
/*! \cond IGNORE */
#define _Col_NewVectorV(_, ...) \
    Col_NewVectorNV(COL_ARGCOUNT(__VA_ARGS__),__VA_ARGS__)
/*! \endcond *//* IGNORE */

/*
 * Remaining declarations.
 */

EXTERN size_t           Col_MaxVectorLength(void);
EXTERN Col_Word         Col_NewVector(size_t length,
                            const Col_Word * elements);
EXTERN Col_Word         Col_NewVectorNV(size_t length, ...);

/* End of Immutable Vector Creation *//*!\}*/


/***************************************************************************//*!
 * \name Immutable Vector Accessors
 ***************************************************************************\{*/

EXTERN size_t           Col_VectorLength(Col_Word vector);
EXTERN const Col_Word * Col_VectorElements(Col_Word vector);

/* End of Immutable Vector Accessors *//*!\}*/

/* End of Immutable Vectors *//*!\}*/


/*
===========================================================================*//*!
\defgroup mvector_words Mutable Vectors
\ingroup words

Mutable vectors are arrays of words that are directly accessible and
modifiable through a pointer value, and whose length can vary up to a
given capacity set at creation time; they can be"frozen" and turned into
immutable vectors.
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Mutable Vector Creation
 ***************************************************************************\{*/

EXTERN size_t           Col_MaxMVectorLength(void);
EXTERN Col_Word         Col_NewMVector(size_t capacity, size_t length,
                            const Col_Word * elements);

/* End of Mutable Vector Creation *//*!\}*/


/***************************************************************************//*!
 * \name Mutable Vector Accessors
 ***************************************************************************\{*/

EXTERN size_t           Col_MVectorCapacity(Col_Word mvector);
EXTERN Col_Word *       Col_MVectorElements(Col_Word mvector);

/* End of Mutable Vector Accessors *//*!\}*/


/***************************************************************************//*!
 * \name Mutable Vector Operations
 ***************************************************************************\{*/

EXTERN void             Col_MVectorSetLength(Col_Word mvector, size_t length);
EXTERN void             Col_MVectorFreeze(Col_Word mvector);

/* End of Mutable Vector Operations *//*!\}*/

/* End of Mutable Vectors *//*!\}*/

#endif /* _COLIBRI_VECTOR */
```

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)