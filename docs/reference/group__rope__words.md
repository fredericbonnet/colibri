<a id="group__rope__words"></a>
# Ropes

Ropes are a string datatype that allows for fast insertion, extraction and composition of strings.

<a id="group__rope__words_1rope_tree_balancing"></a>

## Submodules

* [Custom Ropes](group__customrope__words.md#group__customrope__words)

## Rope Creation

<a id="group__rope__words_1gac89e756ecb28842e1eb50327270408f3"></a>
### Function Col\_EmptyRope

![][public]

```cpp
Col_Word Col_EmptyRope()
```

Return an empty rope.

The returned word is immediate and constant, which means that it consumes no memory and its value can be safely compared and stored in static storage.






**Returns**:

The empty rope.



**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__rope__words_1ga6904ba10bcedc80ff75dd6d775679bdc"></a>
### Function Col\_NewRopeFromString

![][public]

```cpp
Col_Word Col_NewRopeFromString(const char *string)
```

Create a new rope from a C string.

The string is treated as an UCS1 character buffer whose length is computed with **strlen()** and is passed to Col_NewRope.






**Returns**:

A new rope containing the character data.



**See also**: [Col\_NewRope](col_rope_8h.md#group__rope__words_1gadf89e360729ba5052887cd4897b0167f)



**Parameters**:

* const char * **string**: C string to build rope from.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__rope__words_1ga41d52ca5141a365cf9df75178796b2ea"></a>
### Function Col\_NewCharWord

![][public]

```cpp
Col_Word Col_NewCharWord(Col_Char c)
```

Create a new rope from a single character.

Use adaptive format.






**Returns**:

A new rope made of the single character.




**See also**: [Col\_NewRope](col_rope_8h.md#group__rope__words_1gadf89e360729ba5052887cd4897b0167f), [Col\_CharWordValue](col_rope_8h.md#group__rope__words_1ga340bac62f22ac611e3c3bb27e5b7b56a)



**Parameters**:

* [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) **c**: Character.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__rope__words_1gadf89e360729ba5052887cd4897b0167f"></a>
### Function Col\_NewRope

![][public]

```cpp
Col_Word Col_NewRope(Col_StringFormat format, const void *data, size_t byteLength)
```

Create a new rope from flat character data.

This can either be a single leaf rope containing the whole data, or a concatenation of leaves if data is too large.





If the string contains a single Unicode char, or if the string is 8-bit clean and is sufficiently small, return an immediate value instead of allocating memory.





If the original string is too large, data may span several multi-cell leaf ropes. In this case we recursively split the data in half and build a concat tree.






**Returns**:

A new rope containing the character data.



**Parameters**:

* [Col\_StringFormat](colibri_8h.md#group__strings_1ga125054104f6260ea3902e6e46ebfdfa0) **format**: Format of data in buffer. if [COL\_UCS](col_rope_8h.md#group__rope__words_1ga568cb9253484f41fa8fcbb58e24926d7), data is provided as with [COL\_UCS4](colibri_8h.md#group__strings_1gga125054104f6260ea3902e6e46ebfdfa0af1d4922d36f7509d8936b0af29b0a9e4) but will use [COL\_UCS1](colibri_8h.md#group__strings_1gga125054104f6260ea3902e6e46ebfdfa0a18ed97ba951a5d02c0a6e039445235b8) or [COL\_UCS2](colibri_8h.md#group__strings_1gga125054104f6260ea3902e6e46ebfdfa0ae43fbe04c500f0868e4b95a669a987e0) if data fits.
* const void * **data**: Buffer containing flat data.
* size_t **byteLength**: Length of data in bytes.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__rope__words_1gad8e0ed73e9d579e9aac9bd5ee7603319"></a>
### Function Col\_NormalizeRope

![][public]

```cpp
Col_Word Col_NormalizeRope(Col_Word rope, Col_StringFormat format, Col_Char replace, int flatten)
```

Create a copy of a rope using a given target format.

Unrepresentable characters (i.e. whose codepoint is too large to fit the target representation) can be skipped or replaced by a replacement character. The rope is converted chunk-wise (i.e. subropes that have the right format remain unchanged) but can optionally be flattened to form a single chunk (or several concatenated chunks for larger strings).





Immediate representations are used as long as they match the format.





Custom ropes are always converted whatever their format.






**Returns**:

A new rope containing the character data in the target format.



**See also**: [Col\_NewRope](col_rope_8h.md#group__rope__words_1gadf89e360729ba5052887cd4897b0167f)

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope**: Not a rope.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: Rope to copy.
* [Col\_StringFormat](colibri_8h.md#group__strings_1ga125054104f6260ea3902e6e46ebfdfa0) **format**: Target format (see [Col\_StringFormat](colibri_8h.md#group__strings_1ga125054104f6260ea3902e6e46ebfdfa0) and [COL\_UCS](col_rope_8h.md#group__rope__words_1ga568cb9253484f41fa8fcbb58e24926d7)).
* [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) **replace**: Replacement characters for unrepresentable codepoints, or [COL\_CHAR\_INVALID](colibri_8h.md#group__strings_1ga7d5dc9bdb8de819c861ee5d4a3300ae1) to skip.
* int **flatten**: If true, flatten ropes into large string arrays, else keep ropes fitting the target format whatever their structure.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__rope__words_1ga568cb9253484f41fa8fcbb58e24926d7"></a>
### Macro COL\_UCS

![][public]

```cpp
#define COL_UCS
```

When passed to [Col\_NewRope()](col_rope_8h.md#group__rope__words_1gadf89e360729ba5052887cd4897b0167f) or [Col\_NormalizeRope()](col_rope_8h.md#group__rope__words_1gad8e0ed73e9d579e9aac9bd5ee7603319), use the shortest possible fixed-width format.

Input format is always [COL\_UCS4](colibri_8h.md#group__strings_1gga125054104f6260ea3902e6e46ebfdfa0af1d4922d36f7509d8936b0af29b0a9e4).






?> Numeric value is chosen so that the lower 3 bits give the character width in the data chunk.



## Rope Accessors

<a id="group__rope__words_1ga340bac62f22ac611e3c3bb27e5b7b56a"></a>
### Function Col\_CharWordValue

![][public]

```cpp
Col_Char Col_CharWordValue(Col_Word ch)
```

Get codepoint value of character word.

**Returns**:

The codepoint.



**See also**: [Col\_NewCharWord](col_rope_8h.md#group__rope__words_1ga41d52ca5141a365cf9df75178796b2ea)

**Exceptions**:

* **[COL\_ERROR\_CHAR](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a336970cfb68532435a61ff5e30def4e9)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **ch**: Not a character word.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **ch**: Character word to get value for.

**Return type**: EXTERN [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862)

<a id="group__rope__words_1ga615c33c7debc250deb16457ee8821e5d"></a>
### Function Col\_StringWordFormat

![][public]

```cpp
Col_StringFormat Col_StringWordFormat(Col_Word string)
```

Get format of string word.

A string word is a rope made of a single leaf, i.e. a flat character array and not a binary tree of subropes and concat nodes.






**Returns**:

The format.

**Exceptions**:

* **[COL\_ERROR\_STRING](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a1b27b71d27f64cf37c74e6776d86166f)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **string**: Not a string word.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **string**: String word to get format for.

**Return type**: EXTERN [Col\_StringFormat](colibri_8h.md#group__strings_1ga125054104f6260ea3902e6e46ebfdfa0)

<a id="group__rope__words_1ga19acc7c753b0f7b55287b11360259a16"></a>
### Function Col\_RopeLength

![][public]

```cpp
size_t Col_RopeLength(Col_Word rope)
```

Get the length of the rope.

**Returns**:

The rope length.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope**: Not a rope.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: Rope to get length for.

**Return type**: EXTERN size_t

<a id="group__rope__words_1gab39b3e09b4d6c285c4cf0a894e2cd238"></a>
### Function Col\_RopeDepth

![][public]

```cpp
unsigned char Col_RopeDepth(Col_Word rope)
```

Get the depth of the rope.

**Returns**:

The rope depth.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope**: Not a rope.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: Rope to get depth for.

**Return type**: EXTERN unsigned char

<a id="group__rope__words_1ga1dec8dd3d4d8563176a234f5cb9bc5ee"></a>
### Function Col\_RopeAt

![][public]

```cpp
Col_Char Col_RopeAt(Col_Word rope, size_t index)
```

Get the character codepoint of a rope at a given position.

**Return values**:

* **COL_CHAR_INVALID**: if **index** past end of **rope**.
* **code**: Unicode codepoint of the character otherwise.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope**: Not a rope.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: Rope to get character from.
* size_t **index**: Character index.

**Return type**: EXTERN [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862)

## Rope Search and Comparison

<a id="group__rope__words_1ga5cb7658292e0e5ae51fe384ecdc6d6a9"></a>
### Function Col\_RopeFind

![][public]

```cpp
size_t Col_RopeFind(Col_Word rope, Col_Char c, size_t start, size_t max, int reverse)
```

Find first occurrence of a character in a rope.

**Return values**:

* **SIZE_MAX**: if not found (which is an invalid character index since this is the maximum rope length, and indices are zero-based)
* **index**: position of **c** in **rope**.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope**: Not a rope.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: Rope to search character into.
* [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) **c**: Character to search for.
* size_t **start**: Starting index.
* size_t **max**: Maximum number of characters to search.
* int **reverse**: Whether to traverse in reverse order.

**Return type**: EXTERN size_t

<a id="group__rope__words_1gace5992731949e51b03afcb20527c95dd"></a>
### Function Col\_RopeSearch

![][public]

```cpp
size_t Col_RopeSearch(Col_Word rope, Col_Word subrope, size_t start, size_t max, int reverse)
```

Find first occurrence of a subrope in a rope.

**Return values**:

* **SIZE_MAX**: if not found (which is an invalid character index since this is the maximum rope length, and indices are zero-based)
* **index**: position of **subrope** in **rope**.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope**: Not a rope.
* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **subrope**: Not a rope.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: Rope to search subrope into.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **subrope**: Subrope to search for.
* size_t **start**: Starting index.
* size_t **max**: Maximum number of characters to search.
* int **reverse**: Whether to traverse in reverse order.

**Return type**: EXTERN size_t

<a id="group__rope__words_1ga3202ac5414ac22cc7627e0edefbba21c"></a>
### Function Col\_CompareRopesL

![][public]

```cpp
int Col_CompareRopesL(Col_Word rope1, Col_Word rope2, size_t start, size_t max, size_t *posPtr, Col_Char *c1Ptr, Col_Char *c2Ptr)
```

Compare two ropes and find the first differing characters if any.

This is the rope counterpart to C's **strncmp** with extra features.






**Return values**:

* **0**: if both ropes are identical.
* **<0**: if first rope is lexically before second rope.
* **>0**: if first rope is lexically after second rope.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope1**: Not a rope.
* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope2**: Not a rope.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope1**: First rope to compare.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope2**: Second rope to compare.
* size_t **start**: Starting index.
* size_t **max**: Maximum number of characters to compare.
* size_t * **posPtr**: [out] If non-NULL, position of the first differing character.
* [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) * **c1Ptr**: [out] If non-NULL, codepoint of differing character in first chunk.
* [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) * **c2Ptr**: [out] If non-NULL, codepoint of differing character in second chunk.

**Return type**: EXTERN int

<a id="group__rope__words_1ga549a61e9622fc532f08819ccf60639e6"></a>
### Macro Col\_RopeFindFirst

![][public]

```cpp
#define Col_RopeFindFirst( rope ,c )
```

Simple version of [Col\_RopeFind()](col_rope_8h.md#group__rope__words_1ga5cb7658292e0e5ae51fe384ecdc6d6a9), find first occurrence of a character in whole rope from its beginning.

This is the rope counterpart to C's **strchr**.






**Parameters**:

* **rope**: Rope to search character into.
* **c**: Character to search for.


**Return values**:

* **SIZE_MAX**: if not found (which is an invalid character index since this is the maximum rope length, and indices are zero-based)
* **index**: position of **subrope** in **rope**.



**See also**: [Col\_RopeFind](col_rope_8h.md#group__rope__words_1ga5cb7658292e0e5ae51fe384ecdc6d6a9)



<a id="group__rope__words_1gacc02819f3e67e558b5b02c797e48f90e"></a>
### Macro Col\_RopeFindFirstN

![][public]

```cpp
#define Col_RopeFindFirstN( rope ,c ,max )
```

Simple version of [Col\_RopeFind()](col_rope_8h.md#group__rope__words_1ga5cb7658292e0e5ae51fe384ecdc6d6a9), find first occurrence of a character in rope from its beginning up to a given number of characters.

This is the rope counterpart to C's **strnchr**.






**Parameters**:

* **rope**: Rope to search character into.
* **c**: Character to search for.
* **max**: Maximum number of characters to search.


**Return values**:

* **SIZE_MAX**: if not found (which is an invalid character index since this is the maximum rope length, and indices are zero-based)
* **index**: position of **c** in **rope**.



**See also**: [Col\_RopeFind](col_rope_8h.md#group__rope__words_1ga5cb7658292e0e5ae51fe384ecdc6d6a9)



<a id="group__rope__words_1gadb4b4e279e6b488efc8413434192617e"></a>
### Macro Col\_RopeFindLast

![][public]

```cpp
#define Col_RopeFindLast( rope ,c )
```

Simple version of [Col\_RopeFind()](col_rope_8h.md#group__rope__words_1ga5cb7658292e0e5ae51fe384ecdc6d6a9), find last occurrence of a character in whole rope from its end.

This is the rope counterpart to C's **strrchr**.






**Parameters**:

* **rope**: Rope to search character into.
* **c**: Character to search for.


**Return values**:

* **SIZE_MAX**: if not found (which is an invalid character index since this is the maximum rope length, and indices are zero-based)
* **index**: position of **c** in **rope**.



**See also**: [Col\_RopeFind](col_rope_8h.md#group__rope__words_1ga5cb7658292e0e5ae51fe384ecdc6d6a9)



<a id="group__rope__words_1ga14746f33c4059ac87b4e074ca0ee49db"></a>
### Macro Col\_RopeFindLastN

![][public]

```cpp
#define Col_RopeFindLastN( rope ,c ,max )
```

Simple version of [Col\_RopeFind()](col_rope_8h.md#group__rope__words_1ga5cb7658292e0e5ae51fe384ecdc6d6a9), find last occurrence of a character in rope from its end up to a given number of characters.

This function has no C counterpart and is provided for symmetry.






**Parameters**:

* **rope**: Rope to search character into.
* **c**: Character to search for.
* **max**: Maximum number of characters to search.


**Return values**:

* **SIZE_MAX**: if not found (which is an invalid character index since this is the maximum rope length, and indices are zero-based)
* **index**: position of **c** in **rope**.



**See also**: [Col\_RopeFind](col_rope_8h.md#group__rope__words_1ga5cb7658292e0e5ae51fe384ecdc6d6a9)



<a id="group__rope__words_1ga370b92b19777daa2f4a0ed7802fd9c46"></a>
### Macro Col\_RopeSearchFirst

![][public]

```cpp
#define Col_RopeSearchFirst( rope ,subrope )
```

Simple version of [Col\_RopeSearch()](col_rope_8h.md#group__rope__words_1gace5992731949e51b03afcb20527c95dd), find first occurrence of a subrope in whole rope from its beginning.

This is the rope counterpart to C's **strstr**.






**Parameters**:

* **rope**: Rope to search subrope into.
* **subrope**: Subrope to search for.


**Return values**:

* **SIZE_MAX**: if not found (which is an invalid character index since this is the maximum rope length, and indices are zero-based)
* **index**: position of **subrope** in **rope**.



**See also**: [Col\_RopeSearch](col_rope_8h.md#group__rope__words_1gace5992731949e51b03afcb20527c95dd)



<a id="group__rope__words_1gae8f42ebc3f995422f72dda136a0371f1"></a>
### Macro Col\_RopeSearchLast

![][public]

```cpp
#define Col_RopeSearchLast( rope ,subrope )
```

Simple version of [Col\_RopeSearch()](col_rope_8h.md#group__rope__words_1gace5992731949e51b03afcb20527c95dd), find last occurrence of a subrope in whole rope from its end.

This function has no C counterpart and is provided for symmetry.






**Parameters**:

* **rope**: Rope to search subrope into.
* **subrope**: Subrope to search for.


**Return values**:

* **SIZE_MAX**: if not found (which is an invalid character index since this is the maximum rope length, and indices are zero-based)
* **index**: position of **subrope** in **rope**.



**See also**: [Col\_RopeSearch](col_rope_8h.md#group__rope__words_1gace5992731949e51b03afcb20527c95dd)



<a id="group__rope__words_1ga48f2eb5b9cc86ff959c7b85827c3e1b7"></a>
### Macro Col\_CompareRopes

![][public]

```cpp
#define Col_CompareRopes( rope1 ,rope2 )
```

Simple version of [Col\_CompareRopesL()](col_rope_8h.md#group__rope__words_1ga3202ac5414ac22cc7627e0edefbba21c), compare two ropes.

This is the rope counterpart to C's **strcmp**.






**Parameters**:

* **rope1**: First rope to compare.
* **rope2**: Second rope to compare.


**Return values**:

* **zero**: if both ropes are identical
* **negative**: if **rope1** is lexically before **rope2**.
* **positive**: if **rope1** is lexically after **rope2**.



**See also**: [Col\_CompareRopesL](col_rope_8h.md#group__rope__words_1ga3202ac5414ac22cc7627e0edefbba21c)



<a id="group__rope__words_1ga340955d6d7b37bd6bd91216233907d9a"></a>
### Macro Col\_CompareRopesN

![][public]

```cpp
#define Col_CompareRopesN( rope1 ,rope2 ,max )
```

Simple version of [Col\_CompareRopesL()](col_rope_8h.md#group__rope__words_1ga3202ac5414ac22cc7627e0edefbba21c), compare two ropes up to a given number of characters.

This is the rope counterpart to C's **strncmp**.






**Parameters**:

* **rope1**: First rope to compare.
* **rope2**: Second rope to compare.
* **max**: Maximum number of characters to compare.


**Return values**:

* **zero**: if both ropes are identical
* **negative**: if **rope1** is lexically before **rope2**.
* **positive**: if **rope1** is lexically after **rope2**.



**See also**: [Col\_CompareRopesL](col_rope_8h.md#group__rope__words_1ga3202ac5414ac22cc7627e0edefbba21c)



## Rope Operations

<a id="group__rope__words_1ga688a99f26c500c1f65f4141e97de0335"></a>
### Function Col\_Subrope

![][public]

```cpp
Col_Word Col_Subrope(Col_Word rope, size_t first, size_t last)
```

Create a new rope that is a subrope of another.

We try to minimize the overhead as much as possible, such as:
* identity.

* create leaf ropes for small subropes.

* subropes of subropes point to original data.

* subropes of concats point to the deepest superset subrope.








**Returns**:

When **first** is past the end of the rope, or **last** is before **first**, an empty rope. Else, a rope representing the subrope.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope**: Not a rope.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: The rope to extract the subrope from.
* size_t **first**: Index of first character in subrope.
* size_t **last**: Index of last character in subrope.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__rope__words_1gaafab3ef159c0b11402cc50c91fc59700"></a>
### Function Col\_ConcatRopes

![][public]

```cpp
Col_Word Col_ConcatRopes(Col_Word left, Col_Word right)
```

Concatenate ropes.

Concatenation forms self-balanced binary trees. See [Rope Tree Balancing](#group__rope__words_1rope_tree_balancing) for more information.






**Returns**:

A rope representing the concatenation of both ropes.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **left**: Not a rope.
* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **right**: Not a rope.
* **[COL\_ERROR\_ROPELENGTH\_CONCAT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a37a3a6924ed998d6803770696cc7d1dc)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **length(left+right)**: Concat rope too large.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **left**: Left part.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **right**: Right part.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__rope__words_1gaedea8f7a253aa3a5fa1f028ca7586a2e"></a>
### Function Col\_ConcatRopesA

![][public]

```cpp
Col_Word Col_ConcatRopesA(size_t number, const Col_Word *ropes)
```

Concatenate several ropes given in an array.

Concatenation is done recursively, by halving the array until it contains one or two elements, at this point we respectively return the element or use [Col\_ConcatRopes()](col_rope_8h.md#group__rope__words_1gaafab3ef159c0b11402cc50c91fc59700).






**Returns**:

A rope representing the concatenation of all ropes.

**Exceptions**:

* **[COL\_ERROR\_GENERIC](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a02ae949dee6fd3c78c849d7e7af414e4)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **number == 0**: Generic error.

**Parameters**:

* size_t **number**: Size of **ropes** array.
* const [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **ropes**: Array of ropes to concatenate in order.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__rope__words_1ga913fe33c3a479d4f3de008bae5b1bf74"></a>
### Function Col\_ConcatRopesNV

![][public]

```cpp
Col_Word Col_ConcatRopesNV(size_t number, ...)
```

Concatenate ropes given as arguments.

The argument list is first copied into a stack-allocated array then passed to [Col\_ConcatRopesA()](col_rope_8h.md#group__rope__words_1gaedea8f7a253aa3a5fa1f028ca7586a2e).






**Returns**:

A rope representing the concatenation of all ropes.




**See also**: [Col\_ConcatRopesA](col_rope_8h.md#group__rope__words_1gaedea8f7a253aa3a5fa1f028ca7586a2e), [Col\_ConcatRopesV](col_rope_8h.md#group__rope__words_1gabf17771a646dc7eb24990072cd499ca6)

**Exceptions**:

* **[COL\_ERROR\_GENERIC](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a02ae949dee6fd3c78c849d7e7af414e4)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **number == 0**: Generic error.

**Parameters**:

* size_t **number**: Number of arguments following.
* ......: Ropes to concatenate in order.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__rope__words_1ga934627ff7dbfa6ba4a9f7824e9a83895"></a>
### Function Col\_RepeatRope

![][public]

```cpp
Col_Word Col_RepeatRope(Col_Word rope, size_t count)
```

Create a rope formed by the repetition of a source rope.

This method is based on recursive concatenations of the rope following the bit pattern of the count factor. Doubling a rope simply consists of a concat with itself. In the end the resulting tree is very compact, and only a minimal number of extraneous cells are allocated during the balancing process (and will be eventually collected).






**Returns**:

A rope representing the repetition of the source rope. A rope repeated zero times is empty.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope**: Not a rope.
* **[COL\_ERROR\_ROPELENGTH\_REPEAT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35ab86369e9dd70d4605d05d494cc9aa3b9)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **length(rope)*count**: Repeat rope too large.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: The rope to repeat.
* size_t **count**: Repetition factor.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__rope__words_1ga0bf2c4a83475bbc3fd36ab46f4a7cf96"></a>
### Function Col\_RopeInsert

![][public]

```cpp
Col_Word Col_RopeInsert(Col_Word into, size_t index, Col_Word rope)
```

Insert a rope into another one, just before the given insertion point.

As target rope is immutable, this results in a new rope.





Insertion past the end of the rope results in a concatenation.






?> Only perform minimal tests to prevent overflow, basic ops should perform further optimizations anyway.


**Returns**:

The resulting rope.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **into**: Not a rope.
* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope**: Not a rope.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **into**: Target rope to insert into.
* size_t **index**: Index of insertion point.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: Rope to insert.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__rope__words_1ga107a45f2c36ba772229b3dff1e73e44a"></a>
### Function Col\_RopeRemove

![][public]

```cpp
Col_Word Col_RopeRemove(Col_Word rope, size_t first, size_t last)
```

Remove a range of characters from a rope.

As target rope is immutable, this results in a new rope.






?> Only perform minimal tests to prevent overflow, basic ops should perform further optimizations anyway.


**Returns**:

The resulting rope.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope**: Not a rope.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: Rope to remove sequence from.
* size_t **first**: Index of first character in range to remove.
* size_t **last**: Index of last character in range to remove.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__rope__words_1gabb2853a9c71b59cc1c9e649c73a60ff0"></a>
### Function Col\_RopeReplace

![][public]

```cpp
Col_Word Col_RopeReplace(Col_Word rope, size_t first, size_t last, Col_Word with)
```

Replace a range of characters in a rope with another.

As target rope is immutable, this results in a new rope.





Replacement is a combination of [Col\_RopeRemove()](col_rope_8h.md#group__rope__words_1ga107a45f2c36ba772229b3dff1e73e44a) and [Col\_RopeInsert()](col_rope_8h.md#group__rope__words_1ga0bf2c4a83475bbc3fd36ab46f4a7cf96).






?> Only perform minimal tests to prevent overflow, basic ops should perform further optimizations anyway.


**Returns**:

The resulting rope.




**See also**: [Col\_RopeInsert](col_rope_8h.md#group__rope__words_1ga0bf2c4a83475bbc3fd36ab46f4a7cf96), [Col\_RopeRemove](col_rope_8h.md#group__rope__words_1ga107a45f2c36ba772229b3dff1e73e44a)

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope**: Not a rope.
* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **with**: Not a rope.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: Original rope.
* size_t **first**: Index of first character in range to replace.
* size_t **last**: Index of last character in range to replace.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **with**: Replacement rope.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__rope__words_1gabf17771a646dc7eb24990072cd499ca6"></a>
### Macro Col\_ConcatRopesV

![][public]

```cpp
#define Col_ConcatRopesV( first ,... )
```

Variadic macro version of [Col\_ConcatRopesNV()](col_rope_8h.md#group__rope__words_1ga913fe33c3a479d4f3de008bae5b1bf74) that deduces its number of arguments automatically.

**Parameters**:

* **first**: First rope to concatenate.
* **...**: Next ropes to concatenate.



**See also**: [COL\_ARGCOUNT](col_utils_8h.md#group__utils_1gabbb0e58841406f54d444d40625a2c4fe)



## Rope Traversal

<a id="group__rope__words_1ga8a4ef8b6e6f6aa8d863dad85c2f1b2bd"></a>
### Typedef Col\_RopeChunksTraverseProc

![][public]

**Definition**: `include/colRope.h` (line 306)

```cpp
typedef int() Col_RopeChunksTraverseProc(size_t index, size_t length, size_t number, const Col_RopeChunk *chunks, Col_ClientData clientData)
```

Function signature of rope traversal procs.

**Parameters**:

* **index**: Rope-relative index where chunks begin.
* **length**: Length of chunks.
* **number**: Number of chunks.
* **chunks**: Array of chunks. When chunk is NULL, means the index is past the end of the traversed rope.
* **clientData**: Opaque client data. Same value as passed to [Col\_TraverseRopeChunks()](col_rope_8h.md#group__rope__words_1ga1a0bffff5bb042717914fadb3e8501bb) procedure family.


**Return values**:

* **zero**: to continue traversal.
* **non-zero**: to stop traversal. Value is returned as result of [Col\_TraverseRopeChunks()](col_rope_8h.md#group__rope__words_1ga1a0bffff5bb042717914fadb3e8501bb) and related procs.



**Return type**: int()

<a id="group__rope__words_1ga6cc41b66caa3cbce177085b49ec0139e"></a>
### Function Col\_TraverseRopeChunksN

![][public]

```cpp
int Col_TraverseRopeChunksN(size_t number, Col_Word *ropes, size_t start, size_t max, Col_RopeChunksTraverseProc *proc, Col_ClientData clientData, size_t *lengthPtr)
```

Iterate over the chunks of a number of ropes.

For each traversed chunk, **proc** is called back with the opaque data as well as the position within the ropes. If it returns a nonzero result then the iteration ends.






?> The algorithm is naturally recursive but this implementation avoids recursive calls thanks to a stack-allocated backtracking structure.


**Return values**:

* **-1**: if no traversal was performed.
* **int**: last returned value of **proc** otherwise.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **ropes[i]**: Not a rope.
* **[COL\_ERROR\_GENERIC](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a02ae949dee6fd3c78c849d7e7af414e4)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **proc == NULL**: Generic error.

**Parameters**:

* size_t **number**: Number of ropes to traverse.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **ropes**: Array of ropes to traverse.
* size_t **start**: Index of first character.
* size_t **max**: Max number of characters.
* [Col\_RopeChunksTraverseProc](col_rope_8h.md#group__rope__words_1ga8a4ef8b6e6f6aa8d863dad85c2f1b2bd) * **proc**: Callback proc called on each chunk.
* [Col\_ClientData](colibri_8h.md#group__basic__types_1ga52e127a5c635bcb88f252efd210ca1a5) **clientData**: Opaque data passed as is to above **proc**.
* size_t * **lengthPtr**: [in,out] If non-NULL, incremented by the total number of characters traversed upon completion.

**Return type**: EXTERN int

<a id="group__rope__words_1ga1a0bffff5bb042717914fadb3e8501bb"></a>
### Function Col\_TraverseRopeChunks

![][public]

```cpp
int Col_TraverseRopeChunks(Col_Word rope, size_t start, size_t max, int reverse, Col_RopeChunksTraverseProc *proc, Col_ClientData clientData, size_t *lengthPtr)
```

Iterate over the chunks of a rope.

For each traversed chunk, **proc** is called back with the opaque data as well as the position within the rope. If it returns a nonzero result then the iteration ends.






?> The algorithm is naturally recursive but this implementation avoids recursive calls thanks to a stack-allocated backtracking structure. This procedure is an optimized version of [Col\_TraverseRopeChunksN()](col_rope_8h.md#group__rope__words_1ga6cc41b66caa3cbce177085b49ec0139e) that also supports reverse traversal.


**Return values**:

* **-1**: if no traversal was performed.
* **int**: last returned value of **proc** otherwise.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope**: Not a rope.
* **[COL\_ERROR\_GENERIC](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a02ae949dee6fd3c78c849d7e7af414e4)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **proc == NULL**: Generic error.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: Rope to traverse.
* size_t **start**: Index of first character.
* size_t **max**: Max number of characters.
* int **reverse**: Whether to traverse in reverse order.
* [Col\_RopeChunksTraverseProc](col_rope_8h.md#group__rope__words_1ga8a4ef8b6e6f6aa8d863dad85c2f1b2bd) * **proc**: Callback proc called on each chunk.
* [Col\_ClientData](colibri_8h.md#group__basic__types_1ga52e127a5c635bcb88f252efd210ca1a5) **clientData**: Opaque data passed as is to above **proc**.
* size_t * **lengthPtr**: [in,out] If non-NULL, incremented by the total number of characters traversed upon completion.

**Return type**: EXTERN int

## Rope Iteration

<a id="group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725"></a>
### Typedef Col\_RopeIterator

![][public]

**Definition**: `include/colRope.h` (line 398)

```cpp
typedef ColRopeIterator Col_RopeIterator[1]
```

Rope iterator.

Encapsulates the necessary info to iterate & access rope data transparently.






?> Datatype is opaque. Fields should not be accessed by client code.
\
Each iterator takes 8 words on the stack.
\
The type is defined as a single-element array of the internal datatype:
\

* declared variables allocate the right amount of space on the stack,

* calls use pass-by-reference (i.e. pointer) and not pass-by-value,

* forbidden as return type.



**Return type**: ColRopeIterator

<a id="group__rope__words_1ga3b8de67b364b05c3ba6e92d3f79ffeb1"></a>
### Function Col\_RopeIterBegin

![][public]

```cpp
void Col_RopeIterBegin(Col_RopeIterator it, Col_Word rope, size_t index)
```

Initialize the rope iterator so that it points to the **index**-th character within the rope.

If **index** points past the end of the rope, the iterator is initialized to the end iterator.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope**: Not a rope.

**Parameters**:

* [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) **it**: Iterator to initialize.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: Rope to iterate over.
* size_t **index**: Index of first character to iterate.

**Return type**: EXTERN void

<a id="group__rope__words_1gab3adca9b65daa62c81801065eaff2bde"></a>
### Function Col\_RopeIterFirst

![][public]

```cpp
void Col_RopeIterFirst(Col_RopeIterator it, Col_Word rope)
```

Initialize the rope iterator so that it points to the first character within the rope.

If rope is empty, the iterator is initialized to the end iterator.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope**: Not a rope.

**Parameters**:

* [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) **it**: Iterator to initialize.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: Rope to iterate over.

**Return type**: EXTERN void

<a id="group__rope__words_1ga734fc68a33601edefa73fc93ba6c7189"></a>
### Function Col\_RopeIterLast

![][public]

```cpp
void Col_RopeIterLast(Col_RopeIterator it, Col_Word rope)
```

Initialize the rope iterator so that it points to the last character within the rope.

If rope is empty, the iterator is initialized to the end iterator.

**Exceptions**:

* **[COL\_ERROR\_ROPE](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **rope**: Not a rope.

**Parameters**:

* [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) **it**: Iterator to initialize.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **rope**: Rope to iterate over.

**Return type**: EXTERN void

<a id="group__rope__words_1ga087a784890990cffb9d63ece02bcac30"></a>
### Function Col\_RopeIterString

![][public]

```cpp
void Col_RopeIterString(Col_RopeIterator it, Col_StringFormat format, const void *data, size_t length)
```

Initialize the rope iterator so that it points to the first character in a string.





**Parameters**:

* [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) **it**: Iterator to initialize.
* [Col\_StringFormat](colibri_8h.md#group__strings_1ga125054104f6260ea3902e6e46ebfdfa0) **format**: Format of data in string (see [Col\_StringFormat](colibri_8h.md#group__strings_1ga125054104f6260ea3902e6e46ebfdfa0)).
* const void * **data**: Buffer containing flat data.
* size_t **length**: Character length of string.

**Return type**: EXTERN void

<a id="group__rope__words_1gad2cb7c7b443fdae72cbfe1dfed692904"></a>
### Function Col\_RopeIterCompare

![][public]

```cpp
int Col_RopeIterCompare(const Col_RopeIterator it1, const Col_RopeIterator it2)
```

Compare two iterators by their respective positions.



**Exceptions**:

* **[COL\_ERROR\_ROPEITER](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a45d7cbebed5e870c86dbf2dd6c3a56b7)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **it1**: Invalid rope iterator.
* **[COL\_ERROR\_ROPEITER](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a45d7cbebed5e870c86dbf2dd6c3a56b7)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **it2**: Invalid rope iterator.


**Return values**:

* **-1**: if **it1** before **it2**.
* **1**: if **it1** after **it2**.
* **0**: if **it1** and **it2** are equal.

**Parameters**:

* const [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) **it1**: First iterator.
* const [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) **it2**: Second iterator.

**Return type**: EXTERN int

<a id="group__rope__words_1ga038ea096ffa506cb68a0ca8177b2ff13"></a>
### Function Col\_RopeIterMoveTo

![][public]

```cpp
void Col_RopeIterMoveTo(Col_RopeIterator it, size_t index)
```

Move the iterator to the given absolute position.





**Parameters**:

* [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) **it**: The iterator to move.
* size_t **index**: Position to move to.

**Return type**: EXTERN void

<a id="group__rope__words_1ga93ee7c122f7d88a9188503820870923b"></a>
### Function Col\_RopeIterForward

![][public]

```cpp
void Col_RopeIterForward(Col_RopeIterator it, size_t nb)
```

Move the iterator forward to the **nb**-th next character.

?> If moved past the end of rope, **it** is set at end.

**Exceptions**:

* **[COL\_ERROR\_ROPEITER](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a45d7cbebed5e870c86dbf2dd6c3a56b7)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **it**: Invalid rope iterator.
* **[COL\_ERROR\_ROPEITER\_END](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a6ce849d7cc09cc3484477f792c1d0886)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **it**: Rope iterator at end. (only when **nb** != 0)

**Parameters**:

* [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) **it**: The iterator to move.
* size_t **nb**: Offset to move forward.

**Return type**: EXTERN void

<a id="group__rope__words_1ga9e142ef15460061aa676e800bcb9030b"></a>
### Function Col\_RopeIterBackward

![][public]

```cpp
void Col_RopeIterBackward(Col_RopeIterator it, size_t nb)
```

Move the iterator backward to the **nb**-th previous character.

?> If moved before the beginning of rope, **it** is set at end. This means that backward iterators will loop forever if unchecked against [Col\_RopeIterEnd()](col_rope_8h.md#group__rope__words_1ga8abacbcdd87e4b0a3de70a9343a84688).

**Exceptions**:

* **[COL\_ERROR\_ROPEITER](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a45d7cbebed5e870c86dbf2dd6c3a56b7)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **it**: Invalid rope iterator.

**Parameters**:

* [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) **it**: The iterator to move.
* size_t **nb**: Offset to move backward.

**Return type**: EXTERN void

<a id="group__rope__words_1ga833d5c6a140c9da06d9c8ab510a4fddb"></a>
### Macro COL\_ROPEITER\_NULL

![][public]

```cpp
#define COL_ROPEITER_NULL
```

Static initializer for null rope iterators.

**See also**: [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725), [Col\_RopeIterNull](col_rope_8h.md#group__rope__words_1ga3de44fb926a2657b092ff5ca2ca832a4)



<a id="group__rope__words_1ga3de44fb926a2657b092ff5ca2ca832a4"></a>
### Macro Col\_RopeIterNull

![][public]

```cpp
#define Col_RopeIterNull( it )
```

Test whether iterator is null (e.g. it has been set to [COL\_ROPEITER\_NULL](col_rope_8h.md#group__rope__words_1ga833d5c6a140c9da06d9c8ab510a4fddb) or [Col\_RopeIterSetNull()](col_rope_8h.md#group__rope__words_1ga283f3c374aa901fd1aa7bad043c706fe)).

!> **Warning** \
This uninitialized state renders it unusable for any call. Use with caution.


**Parameters**:

* **it**: The [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) to test.


!> **Warning** \
Argument **it** is referenced several times by the macro. Make sure to avoid any side effect.


**Return values**:

* **zero**: if iterator if not null.
* **non-zero**: if iterator is null.





**See also**: [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725), [COL\_ROPEITER\_NULL](col_rope_8h.md#group__rope__words_1ga833d5c6a140c9da06d9c8ab510a4fddb), [Col\_RopeIterSetNull](col_rope_8h.md#group__rope__words_1ga283f3c374aa901fd1aa7bad043c706fe)



<a id="group__rope__words_1ga283f3c374aa901fd1aa7bad043c706fe"></a>
### Macro Col\_RopeIterSetNull

![][public]

```cpp
#define Col_RopeIterSetNull( it )
```

Set an iterator to null.

**Parameters**:

* **it**: The [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) to initialize.


!> **Warning** \
Argument **it** is referenced several times by the macro. Make sure to avoid any side effect.



<a id="group__rope__words_1ga2b63ee88bcb7cf112191ddc663f45196"></a>
### Macro Col\_RopeIterRope

![][public]

```cpp
#define Col_RopeIterRope( it )
```

Get rope for iterator.

**Parameters**:

* **it**: The [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) to access.


**Return values**:

* **WORD_NIL**: if iterating over string (see [Col\_RopeIterString()](col_rope_8h.md#group__rope__words_1ga087a784890990cffb9d63ece02bcac30)).
* **rope**: if iterating over rope.



<a id="group__rope__words_1ga4699d18d2e51050dfbfab245e3376030"></a>
### Macro Col\_RopeIterLength

![][public]

```cpp
#define Col_RopeIterLength( it )
```

Get length of the iterated sequence.

**Parameters**:

* **it**: The [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) to access.


**Returns**:

Length of iterated sequence.



<a id="group__rope__words_1ga67fa8efe9abc6a5ffdee793b175a7d68"></a>
### Macro Col\_RopeIterIndex

![][public]

```cpp
#define Col_RopeIterIndex( it )
```

Get current index within rope for iterator.

**Parameters**:

* **it**: The [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) to access.


**Returns**:

Current index.



<a id="group__rope__words_1ga4b847f201fcffebbc3edd259608101cb"></a>
### Macro Col\_RopeIterAt

![][public]

```cpp
#define Col_RopeIterAt( it )
```

Get current rope character for iterator.

**Parameters**:

* **it**: The [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) to access.


!> **Warning** \
Argument **it** is referenced several times by the macro. Make sure to avoid any side effect.


**Returns**:

Unicode codepoint of current character.


**Exceptions**:

* **[COL\_ERROR\_ROPEITER\_END](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a6ce849d7cc09cc3484477f792c1d0886)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **it**: Rope iterator at end.



<a id="group__rope__words_1ga38f4fb336055b463f32aaa0b99a86830"></a>
### Macro Col\_RopeIterNext

![][public]

```cpp
#define Col_RopeIterNext( it )
```

Move the iterator to the next rope character.

**Parameters**:

* **it**: The [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) to move.


!> **Warning** \
Argument **it** is referenced several times by the macro. Make sure to avoid any side effect.



**Exceptions**:

* **[COL\_ERROR\_ROPEITER](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a45d7cbebed5e870c86dbf2dd6c3a56b7)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **it**: Invalid rope iterator.
* **[COL\_ERROR\_ROPEITER\_END](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a6ce849d7cc09cc3484477f792c1d0886)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **it**: Rope iterator at end.


**See also**: [Col\_RopeIterForward](col_rope_8h.md#group__rope__words_1ga93ee7c122f7d88a9188503820870923b)



<a id="group__rope__words_1gaf2043573abace87c00b13a8c05324a82"></a>
### Macro Col\_RopeIterPrevious

![][public]

```cpp
#define Col_RopeIterPrevious( it )
```

Move the iterator to the previous rope character.

**Parameters**:

* **it**: The [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) to move.


!> **Warning** \
Argument **it** is referenced several times by the macro. Make sure to avoid any side effect.



**Exceptions**:

* **[COL\_ERROR\_ROPEITER](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a45d7cbebed5e870c86dbf2dd6c3a56b7)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **it**: Invalid rope iterator.


**See also**: [Col\_RopeIterBackward](col_rope_8h.md#group__rope__words_1ga9e142ef15460061aa676e800bcb9030b)



<a id="group__rope__words_1ga8abacbcdd87e4b0a3de70a9343a84688"></a>
### Macro Col\_RopeIterEnd

![][public]

```cpp
#define Col_RopeIterEnd( it )
```

Test whether iterator reached end of rope.

**Parameters**:

* **it**: The [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) to test.


!> **Warning** \
Argument **it** is referenced several times by the macro. Make sure to avoid any side effect.


**Return values**:

* **zero**: if iterator if not at end.
* **non-zero**: if iterator is at end.



**See also**: [Col\_RopeIterBegin](col_rope_8h.md#group__rope__words_1ga3b8de67b364b05c3ba6e92d3f79ffeb1)



<a id="group__rope__words_1ga3d149921d88aea57c01d62ef3c519862"></a>
### Macro Col\_RopeIterSet

![][public]

```cpp
#define Col_RopeIterSet( it ,value )
```

Initialize an iterator with another one's value.

**Parameters**:

* **it**: The [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) to initialize.
* **value**: The [Col\_RopeIterator](col_rope_8h.md#group__rope__words_1ga1faf03f532034b5c89e1209d55c9f725) to copy.



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[private]: https://img.shields.io/badge/-private-red (private)
[static]: https://img.shields.io/badge/-static-lightgrey (static)