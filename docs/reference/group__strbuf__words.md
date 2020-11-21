<a id="group__strbuf__words"></a>
# String Buffers

String buffers are used to build strings incrementally in an efficient manner, by appending individual characters or whole ropes.

The current accumulated rope can be retrieved at any time.

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

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[private]: https://img.shields.io/badge/-private-red (private)
[static]: https://img.shields.io/badge/-static-lightgrey (static)