<a id="group__strings"></a>
# Strings





## Basic String Types & Constants

<a id="group__strings_1ga125054104f6260ea3902e6e46ebfdfa0"></a>
### Enumeration type Col\_StringFormat

![][public]

**Definition**: `include/colibri.h` (line 277)

```cpp
enum Col_StringFormat {
  COL_UCS1 =0x01,
  COL_UCS2 =0x02,
  COL_UCS4 =0x04,
  COL_UTF8 =0x11,
  COL_UTF16 =0x12
}
```

String formats.

!> **Attention** \
We assume that UTF-8/16 data is always well-formed. It is the caller responsibility to validate and ensure well-formedness of UTF-8/16 data, notably for security reasons.


?> Numeric values are chosen so that the lower 3 bits give the character width in the data chunk.



<a id="group__strings_1gga125054104f6260ea3902e6e46ebfdfa0a18ed97ba951a5d02c0a6e039445235b8"></a>
#### Enumerator COL\_UCS1

Fixed-width array of Col_Char1.



<a id="group__strings_1gga125054104f6260ea3902e6e46ebfdfa0ae43fbe04c500f0868e4b95a669a987e0"></a>
#### Enumerator COL\_UCS2

Fixed-width array of Col_Char2.



<a id="group__strings_1gga125054104f6260ea3902e6e46ebfdfa0af1d4922d36f7509d8936b0af29b0a9e4"></a>
#### Enumerator COL\_UCS4

Fixed-width array of Col_Char4.



<a id="group__strings_1gga125054104f6260ea3902e6e46ebfdfa0ac5f7c75663139add735ce90077cef306"></a>
#### Enumerator COL\_UTF8

UTF-8 variable width encoding.



<a id="group__strings_1gga125054104f6260ea3902e6e46ebfdfa0ad926084d627f96e4d65a353daef34854"></a>
#### Enumerator COL\_UTF16

UTF-16 variable width encoding.



<a id="group__strings_1gab42ee0cd75b78280e412fa5bae5eb862"></a>
### Typedef Col\_Char

![][public]

**Definition**: `include/colibri.h` (line 232)

```cpp
typedef uint32_t Col_Char
```

String characters use the 32-bit Unicode encoding.





**Return type**: uint32_t

<a id="group__strings_1ga961d5ffde8aa1fba42d4b669a6199e76"></a>
### Typedef Col\_Char1

![][public]

**Definition**: `include/colibri.h` (line 243)

```cpp
typedef uint8_t Col_Char1
```

1-byte UCS codepoint truncated to the lower 8 bits, i.e. Latin-1.





**Return type**: uint8_t

<a id="group__strings_1ga8fc4ecea7142d5fecd3adcbcc35fb920"></a>
### Typedef Col\_Char2

![][public]

**Definition**: `include/colibri.h` (line 245)

```cpp
typedef uint16_t Col_Char2
```

2-byte UCS-2 codepoint.





**Return type**: uint16_t

<a id="group__strings_1ga2eafc0c12affd9bdae281629460cfa73"></a>
### Typedef Col\_Char4

![][public]

**Definition**: `include/colibri.h` (line 247)

```cpp
typedef Col_Char Col_Char4
```

4-byte UCS-4/UTF-32 codepoint, same as [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862).





**Return type**: [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862)

<a id="group__strings_1ga7d5dc9bdb8de819c861ee5d4a3300ae1"></a>
### Macro COL\_CHAR\_INVALID

![][public]

```cpp
#define COL_CHAR_INVALID
```

Invalid Unicode character.

Used in error cases.



<a id="group__strings_1gae744cbeec6aaa53c56f5d30b035c2dde"></a>
### Macro COL\_CHAR\_MAX

![][public]

```cpp
#define COL_CHAR_MAX
```

Maximum Universal Character Set (UCS) codepoint.





<a id="group__strings_1gab5bcfc6f2e396458bfca5cc643248427"></a>
### Macro COL\_CHAR1\_MAX

![][public]

```cpp
#define COL_CHAR1_MAX
```

Maximum UCS-1 codepoint.





<a id="group__strings_1ga20750ccf04ae493b5c1cf9a9c3406157"></a>
### Macro COL\_CHAR2\_MAX

![][public]

```cpp
#define COL_CHAR2_MAX
```

Maximum UCS-2 codepoint.





<a id="group__strings_1ga20e78e7ab54a3612eb276410ae8fb541"></a>
### Macro COL\_CHAR4\_MAX

![][public]

```cpp
#define COL_CHAR4_MAX
```

Maximum UCS-4 codepoint, same as [COL\_CHAR\_MAX](colibri_8h.md#group__strings_1gae744cbeec6aaa53c56f5d30b035c2dde).





<a id="group__strings_1ga31d95cca0dbddad6e0494c7f1ab11271"></a>
### Macro COL\_UTF8\_MAX\_WIDTH

![][public]

```cpp
#define COL_UTF8_MAX_WIDTH
```

Maximum width of character in UTF-8 code units.





<a id="group__strings_1gaf681f77d3737704a4283d1c6b20aaada"></a>
### Macro COL\_UTF8\_WIDTH

![][public]

```cpp
#define COL_UTF8_WIDTH( c )
```

Get width of character in UTF-8 code units.

**Parameters**:

* **c**: The character.


**Returns**:

Number of UTF-8 codepoints needed for the character (0 if invalid).



<a id="group__strings_1ga076f9d371bec86c5e4d2a9745bd8ff85"></a>
### Macro COL\_UTF16\_MAX\_WIDTH

![][public]

```cpp
#define COL_UTF16_MAX_WIDTH
```

Maximum width of character in UTF-16 code units.





<a id="group__strings_1gadfa1181197464d4368c9c70cd427586c"></a>
### Macro COL\_UTF16\_WIDTH

![][public]

```cpp
#define COL_UTF16_WIDTH( c )
```

Get width of character in UTF-16 code units.

**Parameters**:

* **c**: The character.


**Returns**:

Number of UTF-16 codepoints needed for the character (0 if invalid).



## Character Data Chunk Access

<a id="group__strings_1ga7307a6191008261c48032847bd7795db"></a>
### Function Col\_Utf8Addr

![][public]

```cpp
const Col_Char1 * Col_Utf8Addr(const Col_Char1 *data, size_t index, size_t length, size_t byteLength)
```

Find the index-th char in a UTF-8 sequence.

Iterate over char boundaries from the beginning or end of the string, whichever is closest, until the char is reached.






!> **Attention** \
We assume that UTF-8 data is always well-formed. It is the caller responsibility to validate and ensure well-formedness of UTF-8 data, notably for security reasons.


**Returns**:

Pointer to the character.



**Parameters**:

* const [Col\_Char1](colibri_8h.md#group__strings_1ga961d5ffde8aa1fba42d4b669a6199e76) * **data**: UTF-8 code unit sequence.
* size_t **index**: Index of char to find.
* size_t **length**: Char length of sequence.
* size_t **byteLength**: Byte length of sequence.

**Return type**: EXTERN const [Col\_Char1](colibri_8h.md#group__strings_1ga961d5ffde8aa1fba42d4b669a6199e76) *

<a id="group__strings_1ga4676e48a2cf0edaa609475081f9c33bd"></a>
### Function Col\_Utf8Get

![][public]

```cpp
Col_Char Col_Utf8Get(const Col_Char1 *data)
```

Get the first character codepoint of a UTF-8 sequence.

**Returns**:

32-bit Unicode codepoint of the char.



**Parameters**:

* const [Col\_Char1](colibri_8h.md#group__strings_1ga961d5ffde8aa1fba42d4b669a6199e76) * **data**: UTF-8 code unit sequence.

**Return type**: EXTERN [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862)

<a id="group__strings_1gafcfc9b9efa5d66494625e7575a175032"></a>
### Function Col\_Utf8Set

![][public]

```cpp
Col_Char1 * Col_Utf8Set(Col_Char1 *data, Col_Char c)
```

Append character in a UTF-8 sequence.

**Returns**:

Position just past the newly added character in sequence.



**Parameters**:

* [Col\_Char1](colibri_8h.md#group__strings_1ga961d5ffde8aa1fba42d4b669a6199e76) * **data**: UTF-8 code unit sequence.
* [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) **c**: Character to write.

**Return type**: EXTERN [Col\_Char1](colibri_8h.md#group__strings_1ga961d5ffde8aa1fba42d4b669a6199e76) *

<a id="group__strings_1ga2fa6a9667e4b8e840c3a787f57754f87"></a>
### Function Col\_Utf8Next

![][public]

```cpp
const Col_Char1 * Col_Utf8Next(const Col_Char1 *data)
```

Get next character in a UTF-8 sequence.

**Returns**:

Position just past the first character in sequence.



**Parameters**:

* const [Col\_Char1](colibri_8h.md#group__strings_1ga961d5ffde8aa1fba42d4b669a6199e76) * **data**: UTF-8 code unit sequence.

**Return type**: EXTERN const [Col\_Char1](colibri_8h.md#group__strings_1ga961d5ffde8aa1fba42d4b669a6199e76) *

<a id="group__strings_1ga84eeec1507e04c1bce7743bde002b660"></a>
### Function Col\_Utf8Prev

![][public]

```cpp
const Col_Char1 * Col_Utf8Prev(const Col_Char1 *data)
```

Get previous character in a UTF-8 sequence.

This is done by skipping all continuation code units.






**Returns**:

Position of the previous character in sequence.



**Parameters**:

* const [Col\_Char1](colibri_8h.md#group__strings_1ga961d5ffde8aa1fba42d4b669a6199e76) * **data**: UTF-8 code unit sequence.

**Return type**: EXTERN const [Col\_Char1](colibri_8h.md#group__strings_1ga961d5ffde8aa1fba42d4b669a6199e76) *

<a id="group__strings_1ga54bd05a8b6bb3ee30245b51a8e3849f3"></a>
### Function Col\_Utf16Addr

![][public]

```cpp
const Col_Char2 * Col_Utf16Addr(const Col_Char2 *data, size_t index, size_t length, size_t byteLength)
```

Find the index-th char in a UTF-16 code unit sequence.

Iterate over char boundaries from the beginning or end of the string, whichever is closest, until the char is reached.






!> **Attention** \
We assume that UTF-16 data is always well-formed. It is the caller responsibility to validate and ensure well-formedness of UTF-16 data, notably for security reasons.


**Returns**:

Pointer to the character.



**Parameters**:

* const [Col\_Char2](colibri_8h.md#group__strings_1ga8fc4ecea7142d5fecd3adcbcc35fb920) * **data**: UTF-16 code unit sequence.
* size_t **index**: Index of char to find.
* size_t **length**: Char length of sequence.
* size_t **byteLength**: Byte length of sequence.

**Return type**: EXTERN const [Col\_Char2](colibri_8h.md#group__strings_1ga8fc4ecea7142d5fecd3adcbcc35fb920) *

<a id="group__strings_1ga4011f8814d933eb2aa977fbcfae64bcc"></a>
### Function Col\_Utf16Get

![][public]

```cpp
Col_Char Col_Utf16Get(const Col_Char2 *data)
```

Get the first character codepoint of a UTF-16 sequence.

**Returns**:

Unicode codepoint of the char.



**Parameters**:

* const [Col\_Char2](colibri_8h.md#group__strings_1ga8fc4ecea7142d5fecd3adcbcc35fb920) * **data**: UTF-16 code unit sequence.

**Return type**: EXTERN [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862)

<a id="group__strings_1ga89b26b7e1327b5b96c088c3ed4263abe"></a>
### Function Col\_Utf16Set

![][public]

```cpp
Col_Char2 * Col_Utf16Set(Col_Char2 *data, Col_Char c)
```

Append character in a UTF-16 sequence.

**Returns**:

Position just past the newly added character in sequence.



**Parameters**:

* [Col\_Char2](colibri_8h.md#group__strings_1ga8fc4ecea7142d5fecd3adcbcc35fb920) * **data**: UTF-16 code unit sequence.
* [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) **c**: Character to write.

**Return type**: EXTERN [Col\_Char2](colibri_8h.md#group__strings_1ga8fc4ecea7142d5fecd3adcbcc35fb920) *

<a id="group__strings_1ga8996e8aad9616731b75cd6b66c9f57f7"></a>
### Function Col\_Utf16Next

![][public]

```cpp
const Col_Char2 * Col_Utf16Next(const Col_Char2 *data)
```

Get next character in a UTF-16 sequence.

**Returns**:

Position just past the first character in sequence.



**Parameters**:

* const [Col\_Char2](colibri_8h.md#group__strings_1ga8fc4ecea7142d5fecd3adcbcc35fb920) * **data**: UTF-16 code unit sequence.

**Return type**: EXTERN const [Col\_Char2](colibri_8h.md#group__strings_1ga8fc4ecea7142d5fecd3adcbcc35fb920) *

<a id="group__strings_1ga674a0094a0990b74f91717c8970903bd"></a>
### Function Col\_Utf16Prev

![][public]

```cpp
const Col_Char2 * Col_Utf16Prev(const Col_Char2 *data)
```

Get previous character in a UTF-16 sequence.

This is done by skipping all low surrogate code units.






**Returns**:

Position of the previous character in sequence.



**Parameters**:

* const [Col\_Char2](colibri_8h.md#group__strings_1ga8fc4ecea7142d5fecd3adcbcc35fb920) * **data**: UTF-16 code unit sequence.

**Return type**: EXTERN const [Col\_Char2](colibri_8h.md#group__strings_1ga8fc4ecea7142d5fecd3adcbcc35fb920) *

<a id="group__strings_1ga9ac1e66190e6ee8fe700d14e0f47e45c"></a>
### Macro COL\_CHAR\_GET

![][public]

```cpp
#define COL_CHAR_GET( format ,data )
```

Get current character in data chunk.

**Parameters**:

* **format**: Data format (see [Col\_StringFormat](colibri_8h.md#group__strings_1ga125054104f6260ea3902e6e46ebfdfa0)).
* **data**: Pointer to current character.


!> **Warning** \
Arguments are referenced several times by the macro. Make sure to avoid any side effect.


**Returns**:

Character codepoint.



<a id="group__strings_1gaf2cdba8d2bfb4a403ef3acc98da01b51"></a>
### Macro COL\_CHAR\_NEXT

![][public]

```cpp
#define COL_CHAR_NEXT( format ,data )
```

Move to next character in data chunk.

**Parameters**:

* **format**: Data format (see [Col\_StringFormat](colibri_8h.md#group__strings_1ga125054104f6260ea3902e6e46ebfdfa0)).
* **data**: Pointer to current character.


**Side Effect**:

The data pointer is moved just past the current character



<a id="group__strings_1ga6ef225d4a90b1cc4e716bfc324f6a446"></a>
### Macro COL\_CHAR\_PREVIOUS

![][public]

```cpp
#define COL_CHAR_PREVIOUS( format ,data )
```

Move to previous character in data chunk.

**Parameters**:

* **format**: Data format (see [Col\_StringFormat](colibri_8h.md#group__strings_1ga125054104f6260ea3902e6e46ebfdfa0)).
* **data**: Pointer to current character.


**Side Effect**:

The data pointer is moved to the previous character



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[private]: https://img.shields.io/badge/-private-red (private)
[static]: https://img.shields.io/badge/-static-lightgrey (static)