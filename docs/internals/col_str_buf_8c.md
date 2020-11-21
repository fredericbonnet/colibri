<a id="col_str_buf_8c"></a>
# File colStrBuf.c

![][C++]

**Location**: `src/colStrBuf.c`

This file implements the string buffer handling features of Colibri.

String buffers are used to build strings incrementally in an efficient manner, by appending individual characters or whole ropes. The current accumulated rope can be retrieved at any time.







**See also**: [colStrBuf.h](col_str_buf_8h.md#col_str_buf_8h)

## Includes

* [../include/colibri.h](colibri_8h.md#colibri_8h)
* [colInternal.h](col_internal_8h.md#col_internal_8h)
* [colWordInt.h](col_word_int_8h.md#col_word_int_8h)
* [colRopeInt.h](col_rope_int_8h.md#col_rope_int_8h)
* [colStrBufInt.h](col_str_buf_int_8h.md#col_str_buf_int_8h)
* <string.h>

```mermaid
graph LR
6["colRope.h"]
click 6 "col_rope_8h.md#col_rope_8h"
6 --> 5
6 --> 7
6 --> 8

1["src/colStrBuf.c"]
click 1 "col_str_buf_8c.md#col_str_buf_8c"
1 --> 2
1 --> 15
1 --> 18
1 --> 19
1 --> 20
1 --> 8

7["stdarg.h"]

10["colVector.h"]
click 10 "col_vector_8h.md#col_vector_8h"
10 --> 5
10 --> 7

14["colTrie.h"]
click 14 "col_trie_8h.md#col_trie_8h"

15["colInternal.h"]
click 15 "col_internal_8h.md#col_internal_8h"
15 --> 16

13["colHash.h"]
click 13 "col_hash_8h.md#col_hash_8h"
13 --> 5

2["../include/colibri.h"]
click 2 "colibri_8h.md#colibri_8h"
2 --> 3
2 --> 4
2 --> 6
2 --> 9
2 --> 10
2 --> 11
2 --> 12
2 --> 13
2 --> 14

16["colConf.h"]
click 16 "col_conf_8h.md#col_conf_8h"
16 --> 17

5["stddef.h"]

18["colWordInt.h"]
click 18 "col_word_int_8h.md#col_word_int_8h"

17["limits.h"]

11["colList.h"]
click 11 "col_list_8h.md#col_list_8h"
11 --> 5
11 --> 7
11 --> 8

8["string.h"]

12["colMap.h"]
click 12 "col_map_8h.md#col_map_8h"
12 --> 5

4["colWord.h"]
click 4 "col_word_8h.md#col_word_8h"
4 --> 5

3["colUtils.h"]
click 3 "col_utils_8h.md#col_utils_8h"

20["colStrBufInt.h"]
click 20 "col_str_buf_int_8h.md#col_str_buf_int_8h"

9["colStrBuf.h"]
click 9 "col_str_buf_8h.md#col_str_buf_8h"
9 --> 5
9 --> 7

19["colRopeInt.h"]
click 19 "col_rope_int_8h.md#col_rope_int_8h"

```

## String Buffer Creation

<a id="group__strbuf__words_1ga44d7ab2d5a5ccb4a113497f56c297ef4"></a>
### Function Col\_MaxStringBufferLength

![][public]

```cpp
size_t Col_MaxStringBufferLength(Col_StringFormat format)
```

Get the maximum buffer length of string buffers.

**Returns**:

The max string buffer length.



**Parameters**:

* [Col\_StringFormat](colibri_8h.md#group__strings_1ga125054104f6260ea3902e6e46ebfdfa0) **format**: Format policy.

**Return type**: EXTERN size_t

**References**:

* [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)
* [STRBUF\_MAX\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga7484adce06c0538b7285371ac4cd18ce)
* [STRBUF\_MAX\_SIZE](col_str_buf_int_8h.md#group__strbuf__words_1ga1ea7cfcb3f2246521db1f9cc27bb598c)

**Referenced by**:

* [Col\_NewStringBuffer](col_str_buf_8h.md#group__strbuf__words_1ga8c3ba7df2adb643c5da323d7fd013cfb)

<a id="group__strbuf__words_1ga8c3ba7df2adb643c5da323d7fd013cfb"></a>
### Function Col\_NewStringBuffer

![][public]

```cpp
Col_Word Col_NewStringBuffer(size_t capacity, Col_StringFormat format)
```

Create a new string buffer word.

?> The actual capacity will be rounded up to fit an even number of cells, and won't exceed the maximum value given by [Col\_MaxStringBufferLength(format)](col_str_buf_8h.md#group__strbuf__words_1ga44d7ab2d5a5ccb4a113497f56c297ef4).


**Returns**:

The new word.

**Exceptions**:

* **[COL\_ERROR\_STRBUF\_FORMAT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35af19a91631001d2669a4a068b71162df3)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **format**: String format not supported.

**Parameters**:

* size_t **capacity**: Maximum length of string buffer. If zero, use a default value.
* [Col\_StringFormat](colibri_8h.md#group__strings_1ga125054104f6260ea3902e6e46ebfdfa0) **format**: String format.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)
* [CHAR\_WIDTH](col_internal_8h.md#group__strings_1gaf5aa639fca28d7d2fa2ab575d2aa9612)
* [Col\_MaxStringBufferLength](col_str_buf_8h.md#group__strbuf__words_1ga44d7ab2d5a5ccb4a113497f56c297ef4)
* [STRBUF\_DEFAULT\_SIZE](col_str_buf_8c.md#group__strbuf__words_1gac6ad81ef157380a8fdb11a627a5c91e0)
* [STRBUF\_MAX\_SIZE](col_str_buf_int_8h.md#group__strbuf__words_1ga1ea7cfcb3f2246521db1f9cc27bb598c)
* [STRBUF\_SIZE](col_str_buf_int_8h.md#group__strbuf__words_1ga7e3612f406000af77436193770547188)
* [VALUECHECK\_STRBUF\_FORMAT](col_str_buf_int_8h.md#group__strbuf__words_1gafa1b1cc1ddba12fc7d86d66d28bccedd)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)
* [WORD\_STRBUF\_INIT](col_str_buf_int_8h.md#group__strbuf__words_1ga271b07b1cdd01774692ca64687c3f8ab)

## String Buffer Accessors

<a id="group__strbuf__words_1gaee87791f6ba1919cec23d6aedaede4fa"></a>
### Function Col\_StringBufferFormat

![][public]

```cpp
Col_StringFormat Col_StringBufferFormat(Col_Word strbuf)
```

Get the character format used by the internal buffer.

**Returns**:

The string buffer format.



**See also**: [Col\_NewStringBuffer](col_str_buf_8h.md#group__strbuf__words_1ga8c3ba7df2adb643c5da323d7fd013cfb)

**Exceptions**:

* **[COL\_ERROR\_STRBUF](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a24606ae882ebe15eceb13230471fe356)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **strbuf**: Not a string buffer.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **strbuf**: String buffer to get length for.

**Return type**: EXTERN [Col\_StringFormat](colibri_8h.md#group__strings_1ga125054104f6260ea3902e6e46ebfdfa0)

**References**:

* [TYPECHECK\_STRBUF](col_str_buf_int_8h.md#group__strbuf__words_1gac92bccb1906795cc7eac4c28989bd144)
* [WORD\_STRBUF\_FORMAT](col_str_buf_int_8h.md#group__strbuf__words_1gab9e86210fdabc7589bcb4d439ed1ee00)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

<a id="group__strbuf__words_1ga14588604f194ea6b77f21e5550bed561"></a>
### Function Col\_StringBufferCapacity

![][public]

```cpp
size_t Col_StringBufferCapacity(Col_Word strbuf)
```

Get the capacity = maximum length of the string buffer.

**Returns**:

The string buffer capacity.



**See also**: [Col\_NewStringBuffer](col_str_buf_8h.md#group__strbuf__words_1ga8c3ba7df2adb643c5da323d7fd013cfb)

**Exceptions**:

* **[COL\_ERROR\_STRBUF](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a24606ae882ebe15eceb13230471fe356)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **strbuf**: Not a string buffer.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **strbuf**: String buffer to get capacity for.

**Return type**: EXTERN size_t

**References**:

* [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)
* [STRBUF\_MAX\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga7484adce06c0538b7285371ac4cd18ce)
* [TYPECHECK\_STRBUF](col_str_buf_int_8h.md#group__strbuf__words_1gac92bccb1906795cc7eac4c28989bd144)
* [WORD\_STRBUF\_FORMAT](col_str_buf_int_8h.md#group__strbuf__words_1gab9e86210fdabc7589bcb4d439ed1ee00)
* [WORD\_STRBUF\_SIZE](col_str_buf_int_8h.md#group__strbuf__words_1ga9273c95e47384885fc2340c2f6e12bcc)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

<a id="group__strbuf__words_1gaf5e822c1d9b6d2fc959a62600d3edadc"></a>
### Function Col\_StringBufferLength

![][public]

```cpp
size_t Col_StringBufferLength(Col_Word strbuf)
```

Get the current length of the string buffer.

**Returns**:

The string buffer length.

**Exceptions**:

* **[COL\_ERROR\_STRBUF](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a24606ae882ebe15eceb13230471fe356)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **strbuf**: Not a string buffer.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **strbuf**: String buffer to get length for.

**Return type**: EXTERN size_t

**References**:

* [Col\_RopeLength](col_rope_8h.md#group__rope__words_1ga19acc7c753b0f7b55287b11360259a16)
* [TYPECHECK\_STRBUF](col_str_buf_int_8h.md#group__strbuf__words_1gac92bccb1906795cc7eac4c28989bd144)
* [WORD\_STRBUF\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga05ca1e3d8d745145f4d2614f29b1ed0b)
* [WORD\_STRBUF\_ROPE](col_str_buf_int_8h.md#group__strbuf__words_1gab4991b12f32c9e890f71a8dd16701f83)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

<a id="group__strbuf__words_1ga787a56365c2d75a98d67bce1f41b82e4"></a>
### Function Col\_StringBufferValue

![][public]

```cpp
Col_Word Col_StringBufferValue(Col_Word strbuf)
```

Get the current cumulated string.

**Returns**:

The string buffer string value.

**Exceptions**:

* **[COL\_ERROR\_STRBUF](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a24606ae882ebe15eceb13230471fe356)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **strbuf**: Not a string buffer.


**Side Effect**:

Commit buffer into accumulated rope.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **strbuf**: String buffer to get value for.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [CommitBuffer](col_str_buf_8c.md#group__strbuf__words_1ga679d9a4fab2d369567c364325d0e7af5)
* [TYPECHECK\_STRBUF](col_str_buf_int_8h.md#group__strbuf__words_1gac92bccb1906795cc7eac4c28989bd144)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)
* [WORD\_STRBUF\_ROPE](col_str_buf_int_8h.md#group__strbuf__words_1gab4991b12f32c9e890f71a8dd16701f83)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

**Referenced by**:

* [Col\_StringBufferFreeze](col_str_buf_8h.md#group__strbuf__words_1gad40ef93666162a29b1913bec1b3a9715)

## String Buffer Operations

<a id="group__strbuf__words_1ga40425acea88a012954e2c9a3f1981585"></a>
### Function Col\_StringBufferAppendChar

![][public]

```cpp
int Col_StringBufferAppendChar(Col_Word strbuf, Col_Char c)
```

Append the given character to the string buffer.

**Return values**:

* **0**: if character was not appended (may occur when the character codepoint is unrepresentable in the target format).
* **<>0**: if character was appended.




**See also**: [Col\_StringBufferAppendRope](col_str_buf_8h.md#group__strbuf__words_1ga714dd6c258d3b715915d29d1b83d0cfa), [Col\_StringBufferAppendSequence](col_str_buf_8h.md#group__strbuf__words_1ga195c4aa02da672a1ccd60e99bf2f9927)

**Exceptions**:

* **[COL\_ERROR\_STRBUF](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a24606ae882ebe15eceb13230471fe356)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **strbuf**: Not a string buffer.


**Side Effect**:

May commit buffer.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **strbuf**: String buffer to append character to.
* [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) **c**: Character to append.

**Return type**: EXTERN int

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)
* [COL\_CHAR1\_MAX](colibri_8h.md#group__strings_1gab5bcfc6f2e396458bfca5cc643248427)
* [COL\_CHAR2\_MAX](colibri_8h.md#group__strings_1ga20750ccf04ae493b5c1cf9a9c3406157)
* [COL\_UCS1](colibri_8h.md#group__strings_1gga125054104f6260ea3902e6e46ebfdfa0a18ed97ba951a5d02c0a6e039445235b8)
* [COL\_UCS2](colibri_8h.md#group__strings_1gga125054104f6260ea3902e6e46ebfdfa0ae43fbe04c500f0868e4b95a669a987e0)
* [CommitBuffer](col_str_buf_8c.md#group__strbuf__words_1ga679d9a4fab2d369567c364325d0e7af5)
* [STRBUF\_MAX\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga7484adce06c0538b7285371ac4cd18ce)
* [TYPECHECK\_STRBUF](col_str_buf_int_8h.md#group__strbuf__words_1gac92bccb1906795cc7eac4c28989bd144)
* [WORD\_STRBUF\_BUFFER](col_str_buf_int_8h.md#group__strbuf__words_1ga078f2268d4a63773bcce56ee719cb551)
* [WORD\_STRBUF\_FORMAT](col_str_buf_int_8h.md#group__strbuf__words_1gab9e86210fdabc7589bcb4d439ed1ee00)
* [WORD\_STRBUF\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga05ca1e3d8d745145f4d2614f29b1ed0b)
* [WORD\_STRBUF\_SIZE](col_str_buf_int_8h.md#group__strbuf__words_1ga9273c95e47384885fc2340c2f6e12bcc)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

**Referenced by**:

* [Col\_StringBufferAppendRope](col_str_buf_8h.md#group__strbuf__words_1ga714dd6c258d3b715915d29d1b83d0cfa)
* [Col\_StringBufferAppendSequence](col_str_buf_8h.md#group__strbuf__words_1ga195c4aa02da672a1ccd60e99bf2f9927)

<a id="group__strbuf__words_1ga714dd6c258d3b715915d29d1b83d0cfa"></a>
### Function Col\_StringBufferAppendRope

![][public]

```cpp
int Col_StringBufferAppendRope(Col_Word strbuf, Col_Word rope)
```

Append the given rope to the string buffer.

**Return values**:

* **0**: if not all characters were appended (may occur when some character codepoints are unrepresentable in the target format).
* **<>0**: if all characters were appended.




**See also**: [Col\_StringBufferAppendChar](col_str_buf_8h.md#group__strbuf__words_1ga40425acea88a012954e2c9a3f1981585), [Col\_StringBufferAppendSequence](col_str_buf_8h.md#group__strbuf__words_1ga195c4aa02da672a1ccd60e99bf2f9927)

**Exceptions**:

* **[COL\_ERROR\_STRBUF](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a24606ae882ebe15eceb13230471fe356)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **strbuf**: Not a string buffer.


**Side Effect**:

May commit buffer.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **strbuf**: String buffer to append rope to.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: Rope to append.

**Return type**: EXTERN int

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)
* [COL\_CHAR\_INVALID](colibri_8h.md#group__strings_1ga7d5dc9bdb8de819c861ee5d4a3300ae1)
* [Col\_ConcatRopes](col_rope_8h.md#group__rope__words_1gaafab3ef159c0b11402cc50c91fc59700)
* [Col\_NormalizeRope](col_rope_8h.md#group__rope__words_1gad8e0ed73e9d579e9aac9bd5ee7603319)
* [Col\_RopeIterAt](col_rope_8h.md#group__rope__words_1ga4b847f201fcffebbc3edd259608101cb)
* [Col\_RopeIterEnd](col_rope_8h.md#group__rope__words_1ga8abacbcdd87e4b0a3de70a9343a84688)
* [Col\_RopeIterFirst](col_rope_8h.md#group__rope__words_1gab3adca9b65daa62c81801065eaff2bde)
* [Col\_RopeIterNext](col_rope_8h.md#group__rope__words_1ga38f4fb336055b463f32aaa0b99a86830)
* [Col\_RopeLength](col_rope_8h.md#group__rope__words_1ga19acc7c753b0f7b55287b11360259a16)
* [Col\_StringBufferAppendChar](col_str_buf_8h.md#group__strbuf__words_1ga40425acea88a012954e2c9a3f1981585)
* [CommitBuffer](col_str_buf_8c.md#group__strbuf__words_1ga679d9a4fab2d369567c364325d0e7af5)
* [MAX\_SHORT\_ROPE\_LENGTH](col_str_buf_8c.md#group__strbuf__words_1gad6979d05e3506584ca233a947efe9651)
* [STRBUF\_MAX\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga7484adce06c0538b7285371ac4cd18ce)
* [TYPECHECK\_STRBUF](col_str_buf_int_8h.md#group__strbuf__words_1gac92bccb1906795cc7eac4c28989bd144)
* [WORD\_STRBUF\_FORMAT](col_str_buf_int_8h.md#group__strbuf__words_1gab9e86210fdabc7589bcb4d439ed1ee00)
* [WORD\_STRBUF\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga05ca1e3d8d745145f4d2614f29b1ed0b)
* [WORD\_STRBUF\_ROPE](col_str_buf_int_8h.md#group__strbuf__words_1gab4991b12f32c9e890f71a8dd16701f83)
* [WORD\_STRBUF\_SIZE](col_str_buf_int_8h.md#group__strbuf__words_1ga9273c95e47384885fc2340c2f6e12bcc)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

**Referenced by**:

* [Col\_StringBufferAppendSequence](col_str_buf_8h.md#group__strbuf__words_1ga195c4aa02da672a1ccd60e99bf2f9927)

<a id="group__strbuf__words_1ga195c4aa02da672a1ccd60e99bf2f9927"></a>
### Function Col\_StringBufferAppendSequence

![][public]

```cpp
int Col_StringBufferAppendSequence(Col_Word strbuf, const Col_RopeIterator begin, const Col_RopeIterator end)
```

Append the given rope sequence to the string buffer.

**Return values**:

* **0**: if not all characters were appended (may occur when some character codepoints are unrepresentable in the target format).
* **<>0**: if all characters were appended.




**See also**: [Col\_StringBufferAppendChar](col_str_buf_8h.md#group__strbuf__words_1ga40425acea88a012954e2c9a3f1981585), [Col\_StringBufferAppendRope](col_str_buf_8h.md#group__strbuf__words_1ga714dd6c258d3b715915d29d1b83d0cfa)

**Exceptions**:

* **[COL\_ERROR\_STRBUF](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a24606ae882ebe15eceb13230471fe356)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **strbuf**: Not a string buffer.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **strbuf**: String buffer to append character sequence to.
* const [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) **begin**: First character in sequence.
* const [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) **end**: First character past sequence end.

**Return type**: EXTERN int

**References**:

* [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)
* [Col\_RopeIterAt](col_rope_8h.md#group__rope__words_1ga4b847f201fcffebbc3edd259608101cb)
* [Col\_RopeIterCompare](col_rope_8h.md#group__rope__words_1gad2cb7c7b443fdae72cbfe1dfed692904)
* [Col\_RopeIterIndex](col_rope_8h.md#group__rope__words_1ga67fa8efe9abc6a5ffdee793b175a7d68)
* [Col\_RopeIterNext](col_rope_8h.md#group__rope__words_1ga38f4fb336055b463f32aaa0b99a86830)
* [Col\_RopeIterRope](col_rope_8h.md#group__rope__words_1ga2b63ee88bcb7cf112191ddc663f45196)
* [Col\_RopeIterSet](col_rope_8h.md#group__rope__words_1ga3d149921d88aea57c01d62ef3c519862)
* [Col\_StringBufferAppendChar](col_str_buf_8h.md#group__strbuf__words_1ga40425acea88a012954e2c9a3f1981585)
* [Col\_StringBufferAppendRope](col_str_buf_8h.md#group__strbuf__words_1ga714dd6c258d3b715915d29d1b83d0cfa)
* [Col\_Subrope](col_rope_8h.md#group__rope__words_1ga688a99f26c500c1f65f4141e97de0335)
* [STRBUF\_MAX\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga7484adce06c0538b7285371ac4cd18ce)
* [TYPECHECK\_STRBUF](col_str_buf_int_8h.md#group__strbuf__words_1gac92bccb1906795cc7eac4c28989bd144)
* [WORD\_STRBUF\_FORMAT](col_str_buf_int_8h.md#group__strbuf__words_1gab9e86210fdabc7589bcb4d439ed1ee00)
* [WORD\_STRBUF\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga05ca1e3d8d745145f4d2614f29b1ed0b)
* [WORD\_STRBUF\_SIZE](col_str_buf_int_8h.md#group__strbuf__words_1ga9273c95e47384885fc2340c2f6e12bcc)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

<a id="group__strbuf__words_1ga6a94ccd39253236811cdff44ebb5ccca"></a>
### Function Col\_StringBufferReserve

![][public]

```cpp
void * Col_StringBufferReserve(Col_Word strbuf, size_t length)
```

Reserve a number of characters for direct buffer access.

Characters can then be accessed as array elements using the format given at creation time.






**Return values**:

* **NULL**: if the buffer is too small to store the given number of characters.
* **pointer**: to the beginning of the reserved area.

**Exceptions**:

* **[COL\_ERROR\_STRBUF](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a24606ae882ebe15eceb13230471fe356)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **strbuf**: Not a string buffer.


**Side Effect**:

May commit buffer.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **strbuf**: String buffer to reserve into.
* size_t **length**: Number of characters to reserve. Must be <= [Col_StringBufferCapacity(strbuf)]([Col\_StringBufferCapacity](col_str_buf_8h.md#group__strbuf__words_1ga14588604f194ea6b77f21e5550bed561)).

**Return type**: EXTERN void *

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)
* [CHAR\_WIDTH](col_internal_8h.md#group__strings_1gaf5aa639fca28d7d2fa2ab575d2aa9612)
* [CommitBuffer](col_str_buf_8c.md#group__strbuf__words_1ga679d9a4fab2d369567c364325d0e7af5)
* [STRBUF\_MAX\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga7484adce06c0538b7285371ac4cd18ce)
* [TYPECHECK\_STRBUF](col_str_buf_int_8h.md#group__strbuf__words_1gac92bccb1906795cc7eac4c28989bd144)
* [WORD\_STRBUF\_BUFFER](col_str_buf_int_8h.md#group__strbuf__words_1ga078f2268d4a63773bcce56ee719cb551)
* [WORD\_STRBUF\_FORMAT](col_str_buf_int_8h.md#group__strbuf__words_1gab9e86210fdabc7589bcb4d439ed1ee00)
* [WORD\_STRBUF\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga05ca1e3d8d745145f4d2614f29b1ed0b)
* [WORD\_STRBUF\_SIZE](col_str_buf_int_8h.md#group__strbuf__words_1ga9273c95e47384885fc2340c2f6e12bcc)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

<a id="group__strbuf__words_1gac0b7631165e900ef418d0431a424cb51"></a>
### Function Col\_StringBufferRelease

![][public]

```cpp
void Col_StringBufferRelease(Col_Word strbuf, size_t length)
```

Release previously reserved characters.



**Exceptions**:

* **[COL\_ERROR\_STRBUF](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a24606ae882ebe15eceb13230471fe356)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **strbuf**: Not a string buffer.


**Side Effect**:

Decrease current length.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **strbuf**: String buffer to release.
* size_t **length**: Number of characters to release.

**Return type**: EXTERN void

**References**:

* [TYPECHECK\_STRBUF](col_str_buf_int_8h.md#group__strbuf__words_1gac92bccb1906795cc7eac4c28989bd144)
* [WORD\_STRBUF\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga05ca1e3d8d745145f4d2614f29b1ed0b)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

<a id="group__strbuf__words_1ga44bf90a157bbcdaddd00763e608a2a28"></a>
### Function Col\_StringBufferReset

![][public]

```cpp
void Col_StringBufferReset(Col_Word strbuf)
```

Empty the string buffer: reset buffer length & accumulated rope.



**Exceptions**:

* **[COL\_ERROR\_STRBUF](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a24606ae882ebe15eceb13230471fe356)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **strbuf**: Not a string buffer.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **strbuf**: String buffer to reset.

**Return type**: EXTERN void

**References**:

* [TYPECHECK\_STRBUF](col_str_buf_int_8h.md#group__strbuf__words_1gac92bccb1906795cc7eac4c28989bd144)
* [WORD\_SMALLSTR\_EMPTY](col_word_int_8h.md#group__smallstr__words_1gaced4b5ab427a8009e9e3586f60cf487f)
* [WORD\_STRBUF\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga05ca1e3d8d745145f4d2614f29b1ed0b)
* [WORD\_STRBUF\_ROPE](col_str_buf_int_8h.md#group__strbuf__words_1gab4991b12f32c9e890f71a8dd16701f83)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

<a id="group__strbuf__words_1gad40ef93666162a29b1913bec1b3a9715"></a>
### Function Col\_StringBufferFreeze

![][public]

```cpp
Col_Word Col_StringBufferFreeze(Col_Word strbuf)
```

Freeze the string buffer word, turning it into a rope.

**Returns**:

The accumulated rope so far.


**Side Effect**:

As the word may be transmuted into a rope, it can no longer be used as a string buffer.

**Exceptions**:

* **[COL\_ERROR\_STRBUF](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a24606ae882ebe15eceb13230471fe356)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **strbuf**: Not a string buffer.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **strbuf**: String buffer to freeze.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [CHAR\_WIDTH](col_internal_8h.md#group__strings_1gaf5aa639fca28d7d2fa2ab575d2aa9612)
* [COL\_CHAR1\_MAX](colibri_8h.md#group__strings_1gab5bcfc6f2e396458bfca5cc643248427)
* [COL\_CHAR2\_MAX](colibri_8h.md#group__strings_1ga20750ccf04ae493b5c1cf9a9c3406157)
* [Col\_StringBufferValue](col_str_buf_8h.md#group__strbuf__words_1ga787a56365c2d75a98d67bce1f41b82e4)
* [COL\_UCS](col_rope_8h.md#group__rope__words_1ga568cb9253484f41fa8fcbb58e24926d7)
* [COL\_UCS1](colibri_8h.md#group__strings_1gga125054104f6260ea3902e6e46ebfdfa0a18ed97ba951a5d02c0a6e039445235b8)
* [COL\_UCS2](colibri_8h.md#group__strings_1gga125054104f6260ea3902e6e46ebfdfa0ae43fbe04c500f0868e4b95a669a987e0)
* [COL\_UCS4](colibri_8h.md#group__strings_1gga125054104f6260ea3902e6e46ebfdfa0af1d4922d36f7509d8936b0af29b0a9e4)
* [SMALLSTR\_MAX\_LENGTH](col_word_int_8h.md#group__smallstr__words_1ga465e96e908ab71872527b13453fda803)
* [STRBUF\_HEADER\_SIZE](col_str_buf_int_8h.md#group__strbuf__words_1ga00d4433afb72b5ac6983db2a70c02bcf)
* [TYPECHECK\_STRBUF](col_str_buf_int_8h.md#group__strbuf__words_1gac92bccb1906795cc7eac4c28989bd144)
* [UCSSTR\_HEADER\_SIZE](col_rope_int_8h.md#group__ucsstr__words_1ga04d9ab12584a6fc694c0949e52f481c5)
* [UCSSTR\_MAX\_LENGTH](col_rope_int_8h.md#group__ucsstr__words_1gacf26d3e514c09e967a7120f031f7d184)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)
* [WORD\_PINNED](col_word_int_8h.md#group__regular__words_1gad20cf13be09a354418d8615e6f2f2193)
* [WORD\_SET\_PINNED](col_word_int_8h.md#group__regular__words_1ga7ae30ab27827ba70ae1265b5f637101a)
* [WORD\_SMALLSTR\_EMPTY](col_word_int_8h.md#group__smallstr__words_1gaced4b5ab427a8009e9e3586f60cf487f)
* [WORD\_STRBUF\_BUFFER](col_str_buf_int_8h.md#group__strbuf__words_1ga078f2268d4a63773bcce56ee719cb551)
* [WORD\_STRBUF\_FORMAT](col_str_buf_int_8h.md#group__strbuf__words_1gab9e86210fdabc7589bcb4d439ed1ee00)
* [WORD\_STRBUF\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga05ca1e3d8d745145f4d2614f29b1ed0b)
* [WORD\_STRBUF\_ROPE](col_str_buf_int_8h.md#group__strbuf__words_1gab4991b12f32c9e890f71a8dd16701f83)
* [WORD\_UCSSTR\_DATA](col_rope_int_8h.md#group__ucsstr__words_1ga72e52b45c2851e4038089248df5ceddf)
* [WORD\_UCSSTR\_INIT](col_rope_int_8h.md#group__ucsstr__words_1gae02e5fdcadce075a6d7891b923633ecf)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

## Macros

<a id="group__strbuf__words_1gac6ad81ef157380a8fdb11a627a5c91e0"></a>
### Macro STRBUF\_DEFAULT\_SIZE

![][public]

```cpp
#define STRBUF_DEFAULT_SIZE 5
```

Default size for string buffers.

**See also**: [Col\_NewStringBuffer](col_str_buf_8h.md#group__strbuf__words_1ga8c3ba7df2adb643c5da323d7fd013cfb)



<a id="group__strbuf__words_1gad6979d05e3506584ca233a947efe9651"></a>
### Macro MAX\_SHORT\_ROPE\_LENGTH

![][public]

```cpp
#define MAX_SHORT_ROPE_LENGTH         ((size_t)(3*[CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)*[CHAR\_WIDTH](col_internal_8h.md#group__strings_1gaf5aa639fca28d7d2fa2ab575d2aa9612)(format)))( format )
```

Maximum length a rope must have to be appended character-wise into the buffer, instead of being appended as a whole.





## Functions

<a id="group__strbuf__words_1ga679d9a4fab2d369567c364325d0e7af5"></a>
### Function CommitBuffer

![][private]
![][static]

```cpp
static void CommitBuffer(Col_Word strbuf)
```

Commit buffered characters into accumulated rope and clear buffer.

**See also**: [Col\_StringBufferFormat](col_str_buf_8h.md#group__strbuf__words_1gaee87791f6ba1919cec23d6aedaede4fa), [Col\_StringBufferAppendChar](col_str_buf_8h.md#group__strbuf__words_1ga40425acea88a012954e2c9a3f1981585), [Col\_StringBufferAppendRope](col_str_buf_8h.md#group__strbuf__words_1ga714dd6c258d3b715915d29d1b83d0cfa), [Col\_StringBufferAppendSequence](col_str_buf_8h.md#group__strbuf__words_1ga195c4aa02da672a1ccd60e99bf2f9927), [Col\_StringBufferReserve](col_str_buf_8h.md#group__strbuf__words_1ga6a94ccd39253236811cdff44ebb5ccca)



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **strbuf**: String buffer to commit.

**Return type**: void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [CHAR\_WIDTH](col_internal_8h.md#group__strings_1gaf5aa639fca28d7d2fa2ab575d2aa9612)
* [Col\_ConcatRopes](col_rope_8h.md#group__rope__words_1gaafab3ef159c0b11402cc50c91fc59700)
* [Col\_NewRope](col_rope_8h.md#group__rope__words_1gadf89e360729ba5052887cd4897b0167f)
* [COL\_STRBUF](col_word_8h.md#group__words_1ga747d59c28f0e463cc0818ba691aade31)
* [Col\_WordType](col_word_8h.md#group__words_1gab0f27c794b1e7ed60b537e2ce94b4408)
* [WORD\_STRBUF\_BUFFER](col_str_buf_int_8h.md#group__strbuf__words_1ga078f2268d4a63773bcce56ee719cb551)
* [WORD\_STRBUF\_FORMAT](col_str_buf_int_8h.md#group__strbuf__words_1gab9e86210fdabc7589bcb4d439ed1ee00)
* [WORD\_STRBUF\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga05ca1e3d8d745145f4d2614f29b1ed0b)
* [WORD\_STRBUF\_ROPE](col_str_buf_int_8h.md#group__strbuf__words_1gab4991b12f32c9e890f71a8dd16701f83)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)

**Referenced by**:

* [Col\_StringBufferAppendChar](col_str_buf_8h.md#group__strbuf__words_1ga40425acea88a012954e2c9a3f1981585)
* [Col\_StringBufferAppendRope](col_str_buf_8h.md#group__strbuf__words_1ga714dd6c258d3b715915d29d1b83d0cfa)
* [Col\_StringBufferReserve](col_str_buf_8h.md#group__strbuf__words_1ga6a94ccd39253236811cdff44ebb5ccca)
* [Col\_StringBufferValue](col_str_buf_8h.md#group__strbuf__words_1ga787a56365c2d75a98d67bce1f41b82e4)

## Source

```cpp
/**
 * @file colStrBuf.c
 *
 * This file implements the string buffer handling features of Colibri.
 *
 * String buffers are used to build strings incrementally in an efficient
 * manner, by appending individual characters or whole ropes. The current
 * accumulated rope can be retrieved at any time.
 *
 * @see colStrBuf.h
 */

#include "../include/colibri.h"
#include "colInternal.h"

#include "colWordInt.h"
#include "colRopeInt.h"
#include "colStrBufInt.h"

#include <string.h>

/*
 * Prototypes for functions used only in this file.
 */

/*! \cond IGNORE */
static void             CommitBuffer(Col_Word strbuf);
/*! \endcond *//* IGNORE */


/*
===========================================================================*//*!
\weakgroup strbuf_words String Buffers
\{*//*==========================================================================
*/

/*******************************************************************************
 * String Buffer Creation
 ******************************************************************************/

/** @beginprivate @cond PRIVATE */

/**
 * Default size for string buffers.
 *
 * @see Col_NewStringBuffer
 */
#define STRBUF_DEFAULT_SIZE     5

/** @endcond @endprivate */

/**
 * Get the maximum buffer length of string buffers.
 *
 * @return The max string buffer length.
 */
size_t
Col_MaxStringBufferLength(
    Col_StringFormat format)    /*!< Format policy. */
{
    return STRBUF_MAX_LENGTH(STRBUF_MAX_SIZE * CELL_SIZE, format);
}

/**
 * Create a new string buffer word.
 *
 * @note
 *      The actual capacity will be rounded up to fit an even number of cells, 
 *      and won't exceed the maximum value given by
 *      [Col_MaxStringBufferLength(format)](@ref Col_MaxStringBufferLength).
 *
 * @return The new word.
 */
Col_Word
Col_NewStringBuffer(
    size_t capacity,            /*!< Maximum length of string buffer. If zero,
                                     use a default value. */
    Col_StringFormat format)    /*!< String format. */
{
    Col_Word strbuf;            /* Resulting word in the general case. */
    size_t size;                /* Number of allocated cells storing a minimum
                                 * of **capacity** elements. */

    /*
     * Check preconditions.
     */
    
    /*! @valuecheck{COL_ERROR_STRBUF_FORMAT,format} */
    VALUECHECK_STRBUF_FORMAT(format) return WORD_NIL;

    /*
     * Create a new string buffer word.
     */

    if (capacity == 0) {
        size = STRBUF_DEFAULT_SIZE;
    } else {
        size_t max = Col_MaxStringBufferLength(format);
        if (capacity > max) capacity = max;
        size = STRBUF_SIZE(capacity * CHAR_WIDTH(format));
        if (size > STRBUF_MAX_SIZE) size = STRBUF_MAX_SIZE;
    }
    strbuf = (Col_Word) AllocCells(size);
    WORD_STRBUF_INIT(strbuf, size, format);

    return strbuf;
}

/* End of String Buffer Creation */


/*******************************************************************************
 * String Buffer Accessors
 ******************************************************************************/

/**
 * Get the character format used by the internal buffer.
 *
 * @return The string buffer format.
 *
 * @see Col_NewStringBuffer
 */
Col_StringFormat
Col_StringBufferFormat(
    Col_Word strbuf)    /*!< String buffer to get length for. */
{
    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return 0;

    WORD_UNWRAP(strbuf);

    return (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);
}

/**
 * Get the capacity = maximum length of the string buffer.
 *
 * @return The string buffer capacity.
 *
 * @see Col_NewStringBuffer
 */
size_t
Col_StringBufferCapacity(
    Col_Word strbuf)    /*!< String buffer to get capacity for. */
{
    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return 0;

    WORD_UNWRAP(strbuf);

    return STRBUF_MAX_LENGTH(WORD_STRBUF_SIZE(strbuf) * CELL_SIZE,
            WORD_STRBUF_FORMAT(strbuf));
}

/**
 * Get the current length of the string buffer.
 *
 * @return The string buffer length.
 */
size_t
Col_StringBufferLength(
    Col_Word strbuf)    /*!< String buffer to get length for. */
{
    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return 0;

    WORD_UNWRAP(strbuf);

    /*
     * String buffer length is the cumulated string length plus the current
     * length of the character buffer.
     */

    return Col_RopeLength(WORD_STRBUF_ROPE(strbuf))
            + WORD_STRBUF_LENGTH(strbuf);
}

/**
 * Get the current cumulated string.
 *
 * @return The string buffer string value.
 */
Col_Word
Col_StringBufferValue(
    Col_Word strbuf)    /*!< String buffer to get value for. */
{
    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return WORD_NIL;

    WORD_UNWRAP(strbuf);

    /*!
     * @sideeffect
     * Commit buffer into accumulated rope.
     */
    CommitBuffer(strbuf);
    return WORD_STRBUF_ROPE(strbuf);
}

/* End of String Buffer Accessors */


/*******************************************************************************
 * String Buffer Operations
 ******************************************************************************/

/** @beginprivate @cond PRIVATE */

/**
 * Maximum length a rope must have to be appended character-wise into the
 * buffer, instead of being appended as a whole.
 */
#define MAX_SHORT_ROPE_LENGTH(format) \
        ((size_t)(3*CELL_SIZE*CHAR_WIDTH(format)))

/**
 * Commit buffered characters into accumulated rope and clear buffer.
 *
 * @see Col_StringBufferFormat
 * @see Col_StringBufferAppendChar
 * @see Col_StringBufferAppendRope
 * @see Col_StringBufferAppendSequence
 * @see Col_StringBufferReserve
 */
static void
CommitBuffer(
    Col_Word strbuf)    /*!< String buffer to commit. */
{
    Col_Word rope;
    size_t length;
    Col_StringFormat format;

    /*
     * Check preconditions.
     */

    ASSERT(Col_WordType(strbuf) & COL_STRBUF);
    ASSERT(WORD_TYPE(strbuf) != WORD_TYPE_WRAP);

    if ((length = WORD_STRBUF_LENGTH(strbuf)) == 0) {
        /*
         * Nothing to commit.
         */

        return;
    }

    /*
     * Create rope from buffer and append to accumulated rope.
     */

    format = (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);
    rope = Col_NewRope(format, WORD_STRBUF_BUFFER(strbuf),
            length * CHAR_WIDTH(format));
    WORD_STRBUF_ROPE(strbuf) = Col_ConcatRopes(WORD_STRBUF_ROPE(strbuf), rope);
    WORD_STRBUF_LENGTH(strbuf) = 0;
}

/** @endcond @endprivate */

/**
 * Append the given character to the string buffer.
 *
 * @retval 0    if character was not appended (may occur when the character
 *              codepoint is unrepresentable in the target format).
 * @retval <>0  if character was appended.
 *
 * @see Col_StringBufferAppendRope
 * @see Col_StringBufferAppendSequence
 */
int
Col_StringBufferAppendChar(
    Col_Word strbuf,    /*!< String buffer to append character to. */
    Col_Char c)         /*!< Character to append. */
{
    Col_StringFormat format;
    size_t maxLength;
    size_t width;

    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return 0;

    WORD_UNWRAP(strbuf);

    /*
     * Get character width.
     */

    format = (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);
    switch (format) {
    case COL_UCS1:
        width = (c <= COL_CHAR1_MAX) ? 1 : 0;
        break;

    case COL_UCS2:
        width = (c <= COL_CHAR2_MAX) ? 1 : 0;
        break;

    default:
        /* No-loss formats */
        width = 1;
    }
    if (width == 0) {
        /*
         * Lost character.
         */

        return 0;
    }

    maxLength = STRBUF_MAX_LENGTH(WORD_STRBUF_SIZE(strbuf) * CELL_SIZE, format);
    if (WORD_STRBUF_LENGTH(strbuf) + width > maxLength) {
        /*
         * Buffer is full, commit first.
         */

        /*!
         * @sideeffect
         * May commit buffer.
         */
        CommitBuffer(strbuf);
        ASSERT(WORD_STRBUF_LENGTH(strbuf) == 0);
    }

    /*
     * Append character into buffer.
     */

    switch (format) {
    case COL_UCS1:
        ((Col_Char1 *) WORD_STRBUF_BUFFER(strbuf))[WORD_STRBUF_LENGTH(strbuf)]
                = c;
        break;

    case COL_UCS2:
        ((Col_Char2 *) WORD_STRBUF_BUFFER(strbuf))[WORD_STRBUF_LENGTH(strbuf)]
                = c;
        break;

    default:
        ((Col_Char4 *) WORD_STRBUF_BUFFER(strbuf))[WORD_STRBUF_LENGTH(strbuf)]
                = c;
    }
    WORD_STRBUF_LENGTH(strbuf) += width;
    return 1;
}

/**
 * Append the given rope to the string buffer.
 *
 * @retval 0    if not all characters were appended (may occur when some
 *              character codepoints are unrepresentable in the target
 *              format).
 * @retval <>0  if all characters were appended.
 *
 * @see Col_StringBufferAppendChar
 * @see Col_StringBufferAppendSequence
 */
int
Col_StringBufferAppendRope(
    Col_Word strbuf,    /*!< String buffer to append rope to. */
    Col_Word rope)      /*!< Rope to append. */
{
    Col_StringFormat format;
    size_t length, ropeLength;
    Col_Word rope2;

    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return 0;

    if ((ropeLength = Col_RopeLength(rope)) == 0) {
        /*
         * Nothing to do.
         */

        return 1;
    }

    WORD_UNWRAP(strbuf);

    format = (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);
    if ((length = WORD_STRBUF_LENGTH(strbuf)) > 0) {
        /*
         * Buffer is not empty.
         */

        if (ropeLength < MAX_SHORT_ROPE_LENGTH(format)
                && ropeLength <= STRBUF_MAX_LENGTH(WORD_STRBUF_SIZE(strbuf)
                * CELL_SIZE, format) - length) {
            /*
             * Rope is short and fits into buffer, append character-wise.
             */

            Col_RopeIterator it;
            int noLoss = 1;
            for (Col_RopeIterFirst(it, rope); !Col_RopeIterEnd(it);
                    Col_RopeIterNext(it)) {
                noLoss &= Col_StringBufferAppendChar(strbuf,
                        Col_RopeIterAt(it));
            }
            return noLoss;
        }

        /*
         * Commit buffer before appending rope.
         */

        /*!
         * @sideeffect
         * May commit buffer.
         */
        CommitBuffer(strbuf);
    }

    /*
     * Normalize & append rope.
     */

    ASSERT(WORD_STRBUF_LENGTH(strbuf) == 0);
    rope2 = Col_NormalizeRope(rope, format, COL_CHAR_INVALID, 0);
    WORD_STRBUF_ROPE(strbuf) = Col_ConcatRopes(WORD_STRBUF_ROPE(strbuf), rope2);
    return (Col_RopeLength(rope2) == ropeLength);
}

/**
 * Append the given rope sequence to the string buffer.
 *
 * @retval 0    if not all characters were appended (may occur when some
 *              character codepoints are unrepresentable in the target
 *              format).
 * @retval <>0  if all characters were appended.
 *
 * @see Col_StringBufferAppendChar
 * @see Col_StringBufferAppendRope
 */
int
Col_StringBufferAppendSequence(
    Col_Word strbuf,                /*!< String buffer to append character
                                         sequence to. */
    const Col_RopeIterator begin,   /*!< First character in sequence. */
    const Col_RopeIterator end)     /*!< First character past sequence end. */
{
    Col_StringFormat format;
    Col_Word rope;

    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return 0;

    if (Col_RopeIterCompare(begin, end) >= 0) {
        /*
         * Nothing to do.
         */

        return 1;
    }

    WORD_UNWRAP(strbuf);

    rope = Col_RopeIterRope(begin);
    format = (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);
    if (!rope || (Col_RopeIterIndex(end) - Col_RopeIterIndex(begin))
            <= STRBUF_MAX_LENGTH(WORD_STRBUF_SIZE(strbuf)
            * CELL_SIZE, format) - WORD_STRBUF_LENGTH(strbuf)) {
        /*
         * Sequence is in string mode or fits into buffer, append
         * character-wise.
         */

        Col_RopeIterator it;
        int noLoss = 1;
        for (Col_RopeIterSet(it, begin); Col_RopeIterCompare(it, end) < 0;
                Col_RopeIterNext(it)) {
            noLoss &= Col_StringBufferAppendChar(strbuf, Col_RopeIterAt(it));
        }
        return noLoss;
    }

    /*
     * Append subrope.
     */

    return Col_StringBufferAppendRope(strbuf, Col_Subrope(rope,
            Col_RopeIterIndex(begin), Col_RopeIterIndex(end)-1));
}

/**
 * Reserve a number of characters for direct buffer access. Characters can
 * then be accessed as array elements using the format given at creation time.
 *
 *
 * @retval NULL     if the buffer is too small to store the given number of
 *                  characters.
 * @retval pointer  to the beginning of the reserved area.
 */
void *
Col_StringBufferReserve(
    Col_Word strbuf,    /*!< String buffer to reserve into. */
    size_t length)      /*!< Number of characters to reserve. Must be <=
                             [Col_StringBufferCapacity(strbuf)](@ref Col_StringBufferCapacity).
                         */
{
    Col_StringFormat format;
    size_t maxLength;
    const char *data;

    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return NULL;

    WORD_UNWRAP(strbuf);

    if (length == 0) {
        /*
        * No-op.
        */

        return NULL;
    }

    format = (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);
    maxLength = STRBUF_MAX_LENGTH(WORD_STRBUF_SIZE(strbuf) * CELL_SIZE,
            format);
    if (length > maxLength) {
        /*
         * Not enough room.
         */

        return NULL;
    }

    ASSERT(WORD_STRBUF_LENGTH(strbuf) <= maxLength);
    if (length > maxLength - WORD_STRBUF_LENGTH(strbuf)) {
        /*
         * Not enough remaining space, commit buffer first.
         */

        /*!
         * @sideeffect
         * May commit buffer.
         */
        CommitBuffer(strbuf);
        ASSERT(WORD_STRBUF_LENGTH(strbuf) == 0);
    }

    /*
     * Reserve area by shifting current length.
     */

    ASSERT(WORD_STRBUF_LENGTH(strbuf) + length <= maxLength);
    data = WORD_STRBUF_BUFFER(strbuf)
            + CHAR_WIDTH(format) * WORD_STRBUF_LENGTH(strbuf);
    WORD_STRBUF_LENGTH(strbuf) += length;
    return (void *) data;
}

/**
 * Release previously reserved characters.
 */
void
Col_StringBufferRelease(
    Col_Word strbuf,    /*!< String buffer to release. */
    size_t length)      /*!< Number of characters to release. */
{
    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return;

    WORD_UNWRAP(strbuf);

    if (WORD_STRBUF_LENGTH(strbuf) < length) {
        WORD_STRBUF_LENGTH(strbuf) = 0;
    } else {
        /*!
         * @sideeffect
         * Decrease current length.
         */
        WORD_STRBUF_LENGTH(strbuf) -= length;
    }
}

/**
 * Empty the string buffer: reset buffer length & accumulated rope.
 */
void
Col_StringBufferReset(
    Col_Word strbuf)    /*!< String buffer to reset. */
{
    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return;

    WORD_UNWRAP(strbuf);

    WORD_STRBUF_ROPE(strbuf) = WORD_SMALLSTR_EMPTY;
    WORD_STRBUF_LENGTH(strbuf) = 0;
}

/**
 * Freeze the string buffer word, turning it into a rope.
 *
 * @return The accumulated rope so far.
 *
 * @sideeffect
 *      As the word may be transmuted into a rope, it can no longer be used as a
 *      string buffer.
 */
Col_Word
Col_StringBufferFreeze(
    Col_Word strbuf)    /*!< String buffer to freeze. */
{
    Col_StringFormat format;
    size_t length;
    int pinned;

    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return WORD_NIL;

    WORD_UNWRAP(strbuf);

    length = WORD_STRBUF_LENGTH(strbuf);
    if (length == 0 || WORD_STRBUF_ROPE(strbuf) != WORD_SMALLSTR_EMPTY) {
        /*
         * Buffer is empty or accumulated rope is not, return value.
         */

        return Col_StringBufferValue(strbuf);
    }

    /*
     * Turn word into rope.
     */

    pinned = WORD_PINNED(strbuf);
    format = (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);
    switch (format) {
    case COL_UCS1:
    case COL_UCS2:
    case COL_UCS4:
        if (length > (format == COL_UCS1 ? SMALLSTR_MAX_LENGTH : 1)
                && length <= UCSSTR_MAX_LENGTH) {
            /*
             * Convert word in-place.
             */

            ASSERT(UCSSTR_HEADER_SIZE <= STRBUF_HEADER_SIZE);
            WORD_UCSSTR_INIT(strbuf, format, length);
            if (pinned) {
                WORD_SET_PINNED(strbuf);
            }
            memcpy((void *) WORD_UCSSTR_DATA(strbuf),
                    WORD_STRBUF_BUFFER(strbuf),
                    length * CHAR_WIDTH(format));
            return strbuf;
        }
        break;

    case COL_UCS:
        if (length <= UCSSTR_MAX_LENGTH) {
            /*
             * Compute format.
             */

            Col_Char4 *data = (Col_Char4 *) WORD_STRBUF_BUFFER(strbuf);
            size_t i;
            format = COL_UCS1;
            for (i=0; i < length; i++) {
                if (data[i] > COL_CHAR2_MAX) {
                    format = COL_UCS4;
                    break;
                } else if (data[i] > COL_CHAR1_MAX) {
                    format = COL_UCS2;
                }
            }

            if (length <= (format == COL_UCS1 ? SMALLSTR_MAX_LENGTH : 1)) {
                /*
                 * Fits immediate string.
                 */

                break;
            }

            /*
             * Convert word in-place.
             */

            ASSERT(UCSSTR_HEADER_SIZE <= STRBUF_HEADER_SIZE);
            WORD_UCSSTR_INIT(strbuf, format, length);
            if (pinned) {
                WORD_SET_PINNED(strbuf);
            }
            if (format == COL_UCS1) {
                Col_Char1 *data1 = (Col_Char1 *) WORD_UCSSTR_DATA(strbuf);
                for (i=0; i < length; i++) {
                    *data1++ = *data++;
                }
            } else if (format == COL_UCS2) {
                Col_Char2 *data2 = (Col_Char2 *) WORD_UCSSTR_DATA(strbuf);
                for (i=0; i < length; i++) {
                    *data2++ = *data++;
                }
            } else {
                ASSERT(format == COL_UCS4);
                memcpy((void *) WORD_UCSSTR_DATA(strbuf),
                        WORD_STRBUF_BUFFER(strbuf),
                        length * CHAR_WIDTH(format));
            }
            return strbuf;
        }
        break;
    }

    /*
     * Could not convert in-place (either too large or fits immediate word),
     * return value.
     */

    return Col_StringBufferValue(strbuf);
}

/* End of String Buffer Operations */

/* End of String Buffers *//*!\}*/
```

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)