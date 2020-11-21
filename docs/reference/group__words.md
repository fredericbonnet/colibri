<a id="group__words"></a>
# Words

Words are a generic abstract datatype framework used in conjunction with the exact generational garbage collector and the cell-based allocator.



## Submodules

* [Hash Maps](group__hashmap__words.md#group__hashmap__words)
* [Immutable Lists](group__list__words.md#group__list__words)
* [Mutable Lists](group__mlist__words.md#group__mlist__words)
* [Maps](group__map__words.md#group__map__words)
* [Ropes](group__rope__words.md#group__rope__words)
* [Trie Maps](group__triemap__words.md#group__triemap__words)
* [Immutable Vectors](group__vector__words.md#group__vector__words)
* [Mutable Vectors](group__mvector__words.md#group__mvector__words)
* [Custom Words](group__custom__words.md#group__custom__words)
* [String Buffers](group__strbuf__words.md#group__strbuf__words)

## Basic Word Types & Constants

<a id="group__words_1gadb626f9e195212e4fdfba7df154ad043"></a>
### Typedef Col\_Word

![][public]

**Definition**: `include/colWord.h` (line 30)

```cpp
typedef uintptr_t Col_Word
```

Colibri words are opaque types.





**Return type**: uintptr_t

<a id="group__words_1ga29e370264f4e5659ccc5be4de209f065"></a>
### Macro WORD\_NIL

![][public]

```cpp
#define WORD_NIL
```

Nil word.





## Boolean Values

Boolean singleton words.





Values are immediate and constant, which means that they can be safely compared and stored in static storage.






!> **Warning** \
C and Colibri booleans are not interchangeable. More specifically, [WORD\_FALSE](col_word_8h.md#group__words_1ga886d363141d4b7de508efa17ca191c3e) is not false in the C sense because it is nonzero. They are also distinct from integer words: an integer zero is not a boolean false contrary to C. So it is an error to write e.g. "if (WORD_FALSE)".

<a id="group__words_1ga886d363141d4b7de508efa17ca191c3e"></a>
### Macro WORD\_FALSE

![][public]

```cpp
#define WORD_FALSE
```

False boolean word.





<a id="group__words_1ga675ef8bd2cf0ae2e0f39f8ca02dfb338"></a>
### Macro WORD\_TRUE

![][public]

```cpp
#define WORD_TRUE
```

True boolean word.





## Word Types

<a id="group__words_1word_types"></a>
 Data types recognized by Colibri. Values are OR-able so that a word can match several types (e.g. a custom rope type as returned by [Col\_WordType()](col_word_8h.md#group__words_1gab0f27c794b1e7ed60b537e2ce94b4408) would be [COL\_CUSTOM](col_word_8h.md#group__words_1gae48e0e2183b8910cf7700b847fb87603) | [COL\_ROPE](col_word_8h.md#group__words_1ga64b6f74edaf16829f0083a21dddd4d93) ).








**See also**: [Col\_WordType](col_word_8h.md#group__words_1gab0f27c794b1e7ed60b537e2ce94b4408), [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type)

<a id="group__words_1ga36da6e6dce11922f4b30d9b331cf0f62"></a>
### Macro COL\_NIL

![][public]

```cpp
#define COL_NIL
```

Nil.





<a id="group__words_1gae48e0e2183b8910cf7700b847fb87603"></a>
### Macro COL\_CUSTOM

![][public]

```cpp
#define COL_CUSTOM
```

Custom type.





<a id="group__words_1gaa868d0b25f066a1af86e13823e140307"></a>
### Macro COL\_BOOL

![][public]

```cpp
#define COL_BOOL
```

Boolean.





<a id="group__words_1gaf0ec1d910f6ba19ede429284179b81fd"></a>
### Macro COL\_INT

![][public]

```cpp
#define COL_INT
```

Integer.





<a id="group__words_1gaf129d60ea367bc8f58d1a060d0fdba30"></a>
### Macro COL\_FLOAT

![][public]

```cpp
#define COL_FLOAT
```

Floating point.





<a id="group__words_1gadee59fa07e5d5d1a3be13f0d95679389"></a>
### Macro COL\_CHAR

![][public]

```cpp
#define COL_CHAR
```

Single character.





<a id="group__words_1ga656318950fbb10969668166b3f8d6c1c"></a>
### Macro COL\_STRING

![][public]

```cpp
#define COL_STRING
```

Flat string.





<a id="group__words_1ga64b6f74edaf16829f0083a21dddd4d93"></a>
### Macro COL\_ROPE

![][public]

```cpp
#define COL_ROPE
```

Generic rope.





<a id="group__words_1ga72ce8bcc3a5db0dc98b0740ba8223558"></a>
### Macro COL\_VECTOR

![][public]

```cpp
#define COL_VECTOR
```

Vector.





<a id="group__words_1ga7edc72b3266c04c24c517c84514e908b"></a>
### Macro COL\_MVECTOR

![][public]

```cpp
#define COL_MVECTOR
```

Mutable vector.





<a id="group__words_1gafaaad5bdc900622b1387bcb1f32f61c3"></a>
### Macro COL\_LIST

![][public]

```cpp
#define COL_LIST
```

Generic list.





<a id="group__words_1ga7ee70ee3416cdb3bafa3c752b92f0c98"></a>
### Macro COL\_MLIST

![][public]

```cpp
#define COL_MLIST
```

Mutable list.





<a id="group__words_1ga42912f858f54a3ebfeef2ede9422248c"></a>
### Macro COL\_MAP

![][public]

```cpp
#define COL_MAP
```

Map (word- or string-keyed).





<a id="group__words_1ga0938add7b6f34338e9c7bc847a6b9b2f"></a>
### Macro COL\_INTMAP

![][public]

```cpp
#define COL_INTMAP
```

Integer-keyed map.





<a id="group__words_1gae3509634e52a76014e96c2575b5d8092"></a>
### Macro COL\_HASHMAP

![][public]

```cpp
#define COL_HASHMAP
```

Hash map.





<a id="group__words_1ga7922babbc856f5670805da2267d72ff0"></a>
### Macro COL\_TRIEMAP

![][public]

```cpp
#define COL_TRIEMAP
```

Trie map.





<a id="group__words_1ga747d59c28f0e463cc0818ba691aade31"></a>
### Macro COL\_STRBUF

![][public]

```cpp
#define COL_STRBUF
```

String buffer.





## Word Creation

<a id="group__words_1ga7e2a5d74e73d2e6b9300814f79ea0db6"></a>
### Function Col\_NewBoolWord

![][public]

```cpp
Col_Word Col_NewBoolWord(int value)
```

Create a new boolean word.

**Returns**:

A new boolean word: either [WORD\_TRUE](col_word_8h.md#group__words_1ga675ef8bd2cf0ae2e0f39f8ca02dfb338) or [WORD\_FALSE](col_word_8h.md#group__words_1ga886d363141d4b7de508efa17ca191c3e).



**See also**: [Col\_BoolWordValue](col_word_8h.md#group__words_1ga3237f3b47ae8e74b1169b6ebd3cf6ebe)



**Parameters**:

* int **value**: Boolean value: zero for false, nonzero for true.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__words_1gaba67c33e1004d5db691cb5834b77645e"></a>
### Function Col\_NewIntWord

![][public]

```cpp
Col_Word Col_NewIntWord(intptr_t value)
```

Create a new integer word.

If the integer value is sufficiently small, return an immediate value instead of allocating memory.






?> Allocates memory cells if word is not immediate.


**Returns**:

The new integer word.



**See also**: [Col\_IntWordValue](col_word_8h.md#group__words_1ga5586e52b13811fc363c0a1bb6bc37a06)



**Parameters**:

* intptr_t **value**: Integer value of the word to create.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__words_1gab8a1c82145210cc626b90a3c8dc3b4b7"></a>
### Function Col\_NewFloatWord

![][public]

```cpp
Col_Word Col_NewFloatWord(double value)
```

Create a new floating point word.

If the floating point value fits, return an immediate value instead of allocating memory. This includes IEEE 754 special values such as +/-0, +/-INF and NaN.






?> Allocates memory cells if word is not immediate.


**Returns**:

The new floating point word.



**Parameters**:

* double **value**: Floating point value of the word to create.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

## Word Accessors

<a id="group__words_1gab0f27c794b1e7ed60b537e2ce94b4408"></a>
### Function Col\_WordType

![][public]

```cpp
int Col_WordType(Col_Word word)
```

Get word type.

Actual value may be a combination of known [Word Types](#group__words_1word_types).






**Returns**:

A combination of [Word Types](#group__words_1word_types).



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **word**: The word to get type for.

**Return type**: EXTERN int

<a id="group__words_1ga3237f3b47ae8e74b1169b6ebd3cf6ebe"></a>
### Function Col\_BoolWordValue

![][public]

```cpp
int Col_BoolWordValue(Col_Word word)
```

Get value of boolean word.

**Returns**:

The boolean value: zero for false, nonzero for true.



**See also**: [Col\_NewBoolWord](col_word_8h.md#group__words_1ga7e2a5d74e73d2e6b9300814f79ea0db6)

**Exceptions**:

* **[COL\_ERROR\_BOOL](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aa004747a3d62aadf448c12577cf19c7f)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **word**: Not a boolean word.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **word**: Boolean word to get value for.

**Return type**: EXTERN int

<a id="group__words_1ga5586e52b13811fc363c0a1bb6bc37a06"></a>
### Function Col\_IntWordValue

![][public]

```cpp
intptr_t Col_IntWordValue(Col_Word word)
```

Get value of integer word.

**Returns**:

The integer value.



**See also**: [Col\_NewIntWord](col_word_8h.md#group__words_1gaba67c33e1004d5db691cb5834b77645e)

**Exceptions**:

* **[COL\_ERROR\_INT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a832354f2b8d5ee385ab0a6a89ce0b5d5)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **word**: Not an integer word.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **word**: Integer word to get value for.

**Return type**: EXTERN intptr_t

<a id="group__words_1gad499285384bc620817a33fbdc10b2211"></a>
### Function Col\_FloatWordValue

![][public]

```cpp
double Col_FloatWordValue(Col_Word word)
```

Get value of floating point word.

**Returns**:

The floating point value.



**See also**: [Col\_NewFloatWord](col_word_8h.md#group__words_1gab8a1c82145210cc626b90a3c8dc3b4b7)

**Exceptions**:

* **[COL\_ERROR\_FLOAT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a4388bca7643872d2e7343fee8ed2f616)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **word**: Not a floating point word.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **word**: Floating point word to get value for.

**Return type**: EXTERN double

## Word Synonyms

<a id="group__words_1ga20f25ebcc58b56c911fc2a6db1c81332"></a>
### Function Col\_WordSynonym

![][public]

```cpp
Col_Word Col_WordSynonym(Col_Word word)
```

Get a synonym for the word.

Words may form chains of synonyms, i.e. circular linked lists. To iterate over the chain, simply call this function several times on the intermediary results until it returns nil or the first word.






**Returns**:

The word synonym, which may be nil.



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **word**: The word to get synonym for.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="group__words_1ga065c63fe593a474258ee99f08b752bb4"></a>
### Function Col\_WordAddSynonym

![][public]

```cpp
void Col_WordAddSynonym(Col_Word *wordPtr, Col_Word synonym)
```

Add a synonym to a word.

**Side Effect**:

Modifies the chain of synonyms. May allocate new words.



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **wordPtr**: Point to the word to add synonym to. May be modified in the process (in this case the original word will be part of the returned word's synonym chain).
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **synonym**: The synonym to add.

**Return type**: EXTERN void

<a id="group__words_1ga7464c00091f4eba37aad1b6dbf5912cd"></a>
### Function Col\_WordClearSynonym

![][public]

```cpp
void Col_WordClearSynonym(Col_Word word)
```

Clear a word's synonym.

This removes the word from the synonym chain it belongs to.



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **word**: The word to clear synonym for.

**Return type**: EXTERN void

## Word Lifetime Management

<a id="group__words_1gab55f452e6b0856f7bd7b34e04fae2aa2"></a>
### Function Col\_WordPreserve

![][public]

```cpp
void Col_WordPreserve(Col_Word word)
```

Preserve a persistent reference to a word, making it a root.

This allows words to be safely stored in external structures regardless of memory management cycles. More specifically, they can't be collected and their address remains constant.





Calls can be nested. A reference count is updated accordingly.





Roots are stored in a trie indexed by the root source addresses.






**pre**\
Must be called within a GC-protected section.


**Side Effect**:

May allocate memory cells. Marks word as pinned.



**See also**: [Col\_WordRelease](col_word_8h.md#group__words_1gad93112f81ce6511d6d0ece0db4d38598)

**Exceptions**:

* **[COL\_ERROR\_GCPROTECT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a33f3b4f6762491c50375359e5ffa02f8)**: [[E]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9eae8345daddd8d5e83225f9f88d302f1a0) Outside of a GC-protected section.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **word**: The word to preserve.

**Return type**: EXTERN void

<a id="group__words_1gad93112f81ce6511d6d0ece0db4d38598"></a>
### Function Col\_WordRelease

![][public]

```cpp
void Col_WordRelease(Col_Word word)
```

Release a root word previously made by [Col\_WordPreserve()](col_word_8h.md#group__words_1gab55f452e6b0856f7bd7b34e04fae2aa2).

Calls can be nested. A reference count is updated accordingly. Once the count drops below 1, the root becomes stale.






**pre**\
Must be called within a GC-protected section.


**Side Effect**:

May release memory cells. Unpin word.



**See also**: [Col\_WordPreserve](col_word_8h.md#group__words_1gab55f452e6b0856f7bd7b34e04fae2aa2)

**Exceptions**:

* **[COL\_ERROR\_GCPROTECT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a33f3b4f6762491c50375359e5ffa02f8)**: [[E]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9eae8345daddd8d5e83225f9f88d302f1a0) Outside of a GC-protected section.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **word**: The root word to release.

**Return type**: EXTERN void

## Word Operations

<a id="group__words_1ga45896e7eb4f62148f854d9bd9e220137"></a>
### Typedef Col\_WordCompareProc

![][public]

**Definition**: `include/colWord.h` (line 157)

```cpp
typedef int() Col_WordCompareProc(Col_Word w1, Col_Word w2, Col_ClientData clientData)
```

Function signature of word comparison function.

Used for sorting.






**Parameters**:

* **w1 w2**: Words to compare.
* **clientData**: Opaque client data. Typically passed to the calling proc (e.g. [Col\_SortWords()](col_word_8h.md#group__words_1gaba761f87ccbab03b17efbf6bb232f112)).


**Return values**:

* **negative**: if **w1** is less than **w2**.
* **positive**: if **w1** is greater than **w2**.
* **zero**: if both words are equal.



**See also**: [Col\_SortWords](col_word_8h.md#group__words_1gaba761f87ccbab03b17efbf6bb232f112)



**Return type**: int()

<a id="group__words_1gaba761f87ccbab03b17efbf6bb232f112"></a>
### Function Col\_SortWords

![][public]

```cpp
void Col_SortWords(Col_Word *first, Col_Word *last, Col_WordCompareProc *proc, Col_ClientData clientData)
```

Sort an array of words using the quicksort algorithm with 3-way partitioning given in "Quicksort is optimal" by Robert Sedgewick and Jon Bentley.





**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **first**: First word of array to sort.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **last**: Last word of array to sort.
* [Col\_WordCompareProc](col_word_8h.md#group__words_1ga45896e7eb4f62148f854d9bd9e220137) * **proc**: Comparison proc for sorting.
* [Col\_ClientData](colibri_8h.md#group__basic__types_1ga52e127a5c635bcb88f252efd210ca1a5) **clientData**: Opaque data passed as is to **proc**.

**Return type**: EXTERN void

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[private]: https://img.shields.io/badge/-private-red (private)
[static]: https://img.shields.io/badge/-static-lightgrey (static)