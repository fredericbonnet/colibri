<a id="group__strbuf__words"></a>
# String Buffers

String buffers are used to build strings incrementally in an efficient manner, by appending individual characters or whole ropes.

The current accumulated rope can be retrieved at any time.





**Requirements**:


* String buffer words can use different character formats for internal storage. So they need to store this information.

* Characters can be appended individually or in bulks (ropes or sequences as iterator ranges). When adding single or multiple characters, they are appended to an internal buffer stored within the word, up to its maximum capacity. When the buffer is full, a rope is appended, or the accumulated value is retrieved, then the buffer content is appended to the accumulated rope so far and the buffer is cleared before performing the operation.

* String buffers have a capacity set at creation time, which is the guaranteed number of characters the buffer can store. This plus the format determines the maximum byte size of the buffer and thus the number of cells the word takes.

* Characters are stored in fixed-width arrays. For this reason, variable-width formats are not supported.


**Parameters**:

* **Format**: Preferred format for string building.
* **Size**: Number of allocated cells.
* **Rope**: Cumulated rope so far.
* **Length**: Current length of character buffer.
* **Buffer**: Character buffer data (array of characters).


**Cell Layout**:

On all architectures the cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        strbuf_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_STRBUF_FORMAT" title="WORD_STRBUF_FORMAT" colspan="2">Format</td>
                <td href="@ref WORD_STRBUF_SIZE" title="WORD_STRBUF_SIZE" colspan="2">Size</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_STRBUF_ROPE" title="WORD_STRBUF_ROPE" colspan="7">Rope</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_STRBUF_LENGTH" title="WORD_STRBUF_LENGTH" colspan="7">Length</td>
            </tr>
            <tr><td sides="R">.</td>
                <td href="@ref WORD_STRBUF_BUFFER" title="WORD_STRBUF_BUFFER" colspan="7" rowspan="3">Buffer</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }
    








**See also**: [WORD\_TYPE\_STRBUF](col_word_int_8h.md#group__words_1ga31e3979e5f5419111f813131cd508768)

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

## String Buffer Constants

<a id="group__strbuf__words_1ga00d4433afb72b5ac6983db2a70c02bcf"></a>
### Macro STRBUF\_HEADER\_SIZE

![][public]

```cpp
#define STRBUF_HEADER_SIZE (sizeof(size_t)*3)
```

Byte size of string buffer header.





<a id="group__strbuf__words_1ga1ea7cfcb3f2246521db1f9cc27bb598c"></a>
### Macro STRBUF\_MAX\_SIZE

![][public]

```cpp
#define STRBUF_MAX_SIZE UINT16_MAX
```

Maximum cell size taken by string buffers.





## String Buffer Utilities

<a id="group__strbuf__words_1ga7e3612f406000af77436193770547188"></a>
### Macro STRBUF\_SIZE

![][public]

```cpp
#define STRBUF_SIZE     ([NB\_CELLS](col_internal_8h.md#group__pages__cells_1ga6969cfc3c9b2913a913df84f7842ce74)([STRBUF\_HEADER\_SIZE](col_str_buf_int_8h.md#group__strbuf__words_1ga00d4433afb72b5ac6983db2a70c02bcf)+(byteLength)))( byteLength )
```

Get number of cells for a string buffer of a given length.

**Parameters**:

* **byteLength**: Buffer length in bytes.


**Returns**:

Number of cells taken by word.



<a id="group__strbuf__words_1ga7484adce06c0538b7285371ac4cd18ce"></a>
### Macro STRBUF\_MAX\_LENGTH

![][public]

```cpp
#define STRBUF_MAX_LENGTH     (((byteSize)-[STRBUF\_HEADER\_SIZE](col_str_buf_int_8h.md#group__strbuf__words_1ga00d4433afb72b5ac6983db2a70c02bcf))/[CHAR\_WIDTH](col_internal_8h.md#group__strings_1gaf5aa639fca28d7d2fa2ab575d2aa9612)(format))( byteSize ,format )
```

Get maximum string buffer length for a given byte size.

**Parameters**:

* **byteSize**: Available size.
* **format**: Character format.


**Returns**:

String buffer length fitting the given size.



## String Buffer Creation

<a id="group__strbuf__words_1ga271b07b1cdd01774692ca64687c3f8ab"></a>
### Macro WORD\_STRBUF\_INIT

![][public]

```cpp
#define WORD_STRBUF_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_STRBUF](col_word_int_8h.md#group__words_1ga31e3979e5f5419111f813131cd508768)); \
    [WORD\_STRBUF\_FORMAT](col_str_buf_int_8h.md#group__strbuf__words_1gab9e86210fdabc7589bcb4d439ed1ee00)(word) = (format); \
    [WORD\_STRBUF\_SIZE](col_str_buf_int_8h.md#group__strbuf__words_1ga9273c95e47384885fc2340c2f6e12bcc)(word) = (uint16_t) (size); \
    [WORD\_STRBUF\_ROPE](col_str_buf_int_8h.md#group__strbuf__words_1gab4991b12f32c9e890f71a8dd16701f83)(word) = [WORD\_SMALLSTR\_EMPTY](col_word_int_8h.md#group__smallstr__words_1gaced4b5ab427a8009e9e3586f60cf487f); \
    [WORD\_STRBUF\_LENGTH](col_str_buf_int_8h.md#group__strbuf__words_1ga05ca1e3d8d745145f4d2614f29b1ed0b)(word) = 0;( word ,size ,format )
```

String buffer word initializer.

**Parameters**:

* **word**: Word to initialize.
* **size**: [WORD\_STRBUF\_SIZE](col_str_buf_int_8h.md#group__strbuf__words_1ga9273c95e47384885fc2340c2f6e12bcc).
* **format**: [WORD\_STRBUF\_FORMAT](col_str_buf_int_8h.md#group__strbuf__words_1gab9e86210fdabc7589bcb4d439ed1ee00).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_STRBUF](col_word_int_8h.md#group__words_1ga31e3979e5f5419111f813131cd508768)



## String Buffer Accessors

<a id="group__strbuf__words_1gab9e86210fdabc7589bcb4d439ed1ee00"></a>
### Macro WORD\_STRBUF\_FORMAT

![][public]

```cpp
#define WORD_STRBUF_FORMAT (((int8_t *)(word))[1])( word )
```

Get/set preferred format for string building.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_STRBUF\_INIT](col_str_buf_int_8h.md#group__strbuf__words_1ga271b07b1cdd01774692ca64687c3f8ab)



<a id="group__strbuf__words_1ga9273c95e47384885fc2340c2f6e12bcc"></a>
### Macro WORD\_STRBUF\_SIZE

![][public]

```cpp
#define WORD_STRBUF_SIZE (((uint16_t *)(word))[1])( word )
```

Get/set number of allocated cells.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_STRBUF\_INIT](col_str_buf_int_8h.md#group__strbuf__words_1ga271b07b1cdd01774692ca64687c3f8ab)



<a id="group__strbuf__words_1gab4991b12f32c9e890f71a8dd16701f83"></a>
### Macro WORD\_STRBUF\_ROPE

![][public]

```cpp
#define WORD_STRBUF_ROPE ((([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(word))[1])( word )
```

Get/set cumulated rope so far.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_STRBUF\_INIT](col_str_buf_int_8h.md#group__strbuf__words_1ga271b07b1cdd01774692ca64687c3f8ab)



<a id="group__strbuf__words_1ga05ca1e3d8d745145f4d2614f29b1ed0b"></a>
### Macro WORD\_STRBUF\_LENGTH

![][public]

```cpp
#define WORD_STRBUF_LENGTH (((size_t *)(word))[2])( word )
```

Get/set current length of character buffer.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_STRBUF\_INIT](col_str_buf_int_8h.md#group__strbuf__words_1ga271b07b1cdd01774692ca64687c3f8ab)



<a id="group__strbuf__words_1ga078f2268d4a63773bcce56ee719cb551"></a>
### Macro WORD\_STRBUF\_BUFFER

![][public]

```cpp
#define WORD_STRBUF_BUFFER ((const char *)(word)+[STRBUF\_HEADER\_SIZE](col_str_buf_int_8h.md#group__strbuf__words_1ga00d4433afb72b5ac6983db2a70c02bcf))( word )
```

Pointer to character buffer data (array of characters in the format given by [WORD\_STRBUF\_FORMAT](col_str_buf_int_8h.md#group__strbuf__words_1gab9e86210fdabc7589bcb4d439ed1ee00)).

**Parameters**:

* **word**: Word to access.



## String Buffer Exceptions

<a id="group__strbuf__words_1gac92bccb1906795cc7eac4c28989bd144"></a>
### Macro TYPECHECK\_STRBUF

![][public]

```cpp
#define TYPECHECK_STRBUF     [TYPECHECK](col_internal_8h.md#group__error_1gaa780a70ef44d8ae2fb023777a35ade9a)(([Col\_WordType](col_word_8h.md#group__words_1gab0f27c794b1e7ed60b537e2ce94b4408)(word) & [COL\_STRBUF](col_word_8h.md#group__words_1ga747d59c28f0e463cc0818ba691aade31)), [COL\_ERROR\_STRBUF](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a24606ae882ebe15eceb13230471fe356), (word))( word )
```

Type checking macro for string buffers.

**Parameters**:

* **word**: Checked word.


**Exceptions**:

* **[COL\_ERROR\_STRBUF](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a24606ae882ebe15eceb13230471fe356)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **word**: Not a string buffer.



<a id="group__strbuf__words_1gafa1b1cc1ddba12fc7d86d66d28bccedd"></a>
### Macro VALUECHECK\_STRBUF\_FORMAT

![][public]

```cpp
#define VALUECHECK_STRBUF_FORMAT( format )
```

Value checking macro for string buffers, ensures that format is supported.

**Parameters**:

* **format**: Checked format.


**Exceptions**:

* **[COL\_ERROR\_STRBUF\_FORMAT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35af19a91631001d2669a4a068b71162df3)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **format is not supported**: String format not supported.



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





[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)