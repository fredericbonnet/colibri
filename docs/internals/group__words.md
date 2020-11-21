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
* [Immediate Words](group__immediate__words.md#group__immediate__words)
* [Regular Words](group__regular__words.md#group__regular__words)
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
#define WORD_NIL     (([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)) 0)
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
#define WORD_FALSE     (([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)) 0x004)
```

False boolean word.





<a id="group__words_1ga675ef8bd2cf0ae2e0f39f8ca02dfb338"></a>
### Macro WORD\_TRUE

![][public]

```cpp
#define WORD_TRUE     (([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)) 0x104)
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
#define COL_NIL 0x0000
```

Nil.





<a id="group__words_1gae48e0e2183b8910cf7700b847fb87603"></a>
### Macro COL\_CUSTOM

![][public]

```cpp
#define COL_CUSTOM 0x0001
```

Custom type.





<a id="group__words_1gaa868d0b25f066a1af86e13823e140307"></a>
### Macro COL\_BOOL

![][public]

```cpp
#define COL_BOOL 0x0002
```

Boolean.





<a id="group__words_1gaf0ec1d910f6ba19ede429284179b81fd"></a>
### Macro COL\_INT

![][public]

```cpp
#define COL_INT 0x0004
```

Integer.





<a id="group__words_1gaf129d60ea367bc8f58d1a060d0fdba30"></a>
### Macro COL\_FLOAT

![][public]

```cpp
#define COL_FLOAT 0x0008
```

Floating point.





<a id="group__words_1gadee59fa07e5d5d1a3be13f0d95679389"></a>
### Macro COL\_CHAR

![][public]

```cpp
#define COL_CHAR 0x0010
```

Single character.





<a id="group__words_1ga656318950fbb10969668166b3f8d6c1c"></a>
### Macro COL\_STRING

![][public]

```cpp
#define COL_STRING 0x0020
```

Flat string.





<a id="group__words_1ga64b6f74edaf16829f0083a21dddd4d93"></a>
### Macro COL\_ROPE

![][public]

```cpp
#define COL_ROPE 0x0040
```

Generic rope.





<a id="group__words_1ga72ce8bcc3a5db0dc98b0740ba8223558"></a>
### Macro COL\_VECTOR

![][public]

```cpp
#define COL_VECTOR 0x0080
```

Vector.





<a id="group__words_1ga7edc72b3266c04c24c517c84514e908b"></a>
### Macro COL\_MVECTOR

![][public]

```cpp
#define COL_MVECTOR 0x0100
```

Mutable vector.





<a id="group__words_1gafaaad5bdc900622b1387bcb1f32f61c3"></a>
### Macro COL\_LIST

![][public]

```cpp
#define COL_LIST 0x0200
```

Generic list.





<a id="group__words_1ga7ee70ee3416cdb3bafa3c752b92f0c98"></a>
### Macro COL\_MLIST

![][public]

```cpp
#define COL_MLIST 0x0400
```

Mutable list.





<a id="group__words_1ga42912f858f54a3ebfeef2ede9422248c"></a>
### Macro COL\_MAP

![][public]

```cpp
#define COL_MAP 0x0800
```

Map (word- or string-keyed).





<a id="group__words_1ga0938add7b6f34338e9c7bc847a6b9b2f"></a>
### Macro COL\_INTMAP

![][public]

```cpp
#define COL_INTMAP 0x1000
```

Integer-keyed map.





<a id="group__words_1gae3509634e52a76014e96c2575b5d8092"></a>
### Macro COL\_HASHMAP

![][public]

```cpp
#define COL_HASHMAP 0x2000
```

Hash map.





<a id="group__words_1ga7922babbc856f5670805da2267d72ff0"></a>
### Macro COL\_TRIEMAP

![][public]

```cpp
#define COL_TRIEMAP 0x4000
```

Trie map.





<a id="group__words_1ga747d59c28f0e463cc0818ba691aade31"></a>
### Macro COL\_STRBUF

![][public]

```cpp
#define COL_STRBUF 0x8000
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

**References**:

* [WORD\_FALSE](col_word_8h.md#group__words_1ga886d363141d4b7de508efa17ca191c3e)
* [WORD\_TRUE](col_word_8h.md#group__words_1ga675ef8bd2cf0ae2e0f39f8ca02dfb338)

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

**References**:

* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)
* [SMALLINT\_MIN](col_word_int_8h.md#group__smallint__words_1ga183cb9a571b40494df308dc41c2bdd20)
* [WORD\_INTWRAP\_INIT](col_word_int_8h.md#group__int__wrappers_1ga9656b5f429e598b3f268d4f9d3821967)
* [WORD\_SMALLINT\_NEW](col_word_int_8h.md#group__smallint__words_1gaa5463ce1fc2e50bd64e0dbc951cbb510)

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

**References**:

* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)
* [WORD\_FPWRAP\_INIT](col_word_int_8h.md#group__fp__wrappers_1ga66d2954c1e1ea98185f0187e465f0290)
* [WORD\_SMALLFP\_NEW](col_word_int_8h.md#group__smallfp__words_1ga8b37cdd7a9aa19766a182dbbed963815)
* [WORD\_SMALLFP\_VALUE](col_word_int_8h.md#group__smallfp__words_1gae3d867f57b6b1e2bb0d0919aee9a711e)

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

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [COL\_BOOL](col_word_8h.md#group__words_1gaa868d0b25f066a1af86e13823e140307)
* [COL\_CHAR](col_word_8h.md#group__words_1gadee59fa07e5d5d1a3be13f0d95679389)
* [COL\_CUSTOM](col_word_8h.md#group__words_1gae48e0e2183b8910cf7700b847fb87603)
* [COL\_FLOAT](col_word_8h.md#group__words_1gaf129d60ea367bc8f58d1a060d0fdba30)
* [COL\_HASHMAP](col_word_8h.md#group__words_1gae3509634e52a76014e96c2575b5d8092)
* [COL\_INT](col_word_8h.md#group__words_1gaf0ec1d910f6ba19ede429284179b81fd)
* [COL\_INTMAP](col_word_8h.md#group__words_1ga0938add7b6f34338e9c7bc847a6b9b2f)
* [COL\_LIST](col_word_8h.md#group__words_1gafaaad5bdc900622b1387bcb1f32f61c3)
* [COL\_MAP](col_word_8h.md#group__words_1ga42912f858f54a3ebfeef2ede9422248c)
* [COL\_MLIST](col_word_8h.md#group__words_1ga7ee70ee3416cdb3bafa3c752b92f0c98)
* [COL\_MVECTOR](col_word_8h.md#group__words_1ga7edc72b3266c04c24c517c84514e908b)
* [COL\_NIL](col_word_8h.md#group__words_1ga36da6e6dce11922f4b30d9b331cf0f62)
* [COL\_ROPE](col_word_8h.md#group__words_1ga64b6f74edaf16829f0083a21dddd4d93)
* [COL\_STRBUF](col_word_8h.md#group__words_1ga747d59c28f0e463cc0818ba691aade31)
* [COL\_STRING](col_word_8h.md#group__words_1ga656318950fbb10969668166b3f8d6c1c)
* [COL\_TRIEMAP](col_word_8h.md#group__words_1ga7922babbc856f5670805da2267d72ff0)
* [COL\_VECTOR](col_word_8h.md#group__words_1ga72ce8bcc3a5db0dc98b0740ba8223558)
* [WORD\_CHAR\_WIDTH](col_word_int_8h.md#group__char__words_1ga92aceea828352d3433d76c193f259a1c)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CHARBOOL](col_word_int_8h.md#group__words_1ga1422c37e0c7d668fad81b0710863dad5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_CONCATROPE](col_word_int_8h.md#group__words_1ga677525993a1e4a934e98042c53021c3a)
* [WORD\_TYPE\_CUSTOM](col_word_int_8h.md#group__words_1ga8babfbc77291680db519873c91efdd4c)
* [WORD\_TYPE\_INTHASHMAP](col_word_int_8h.md#group__words_1ga230c3d50685afa970c1e0da69feb5811)
* [WORD\_TYPE\_INTTRIEMAP](col_word_int_8h.md#group__words_1ga9da4310532cf6307f784bd6f33471218)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_NIL](col_word_int_8h.md#group__words_1ga1f4d1db7619649bb51aeddd67c8b242f)
* [WORD\_TYPE\_SMALLFP](col_word_int_8h.md#group__words_1gaf5bde441039615017c30996b222a55c8)
* [WORD\_TYPE\_SMALLINT](col_word_int_8h.md#group__words_1gacaa5b57dfa5151a5852a1616bad80d3d)
* [WORD\_TYPE\_SMALLSTR](col_word_int_8h.md#group__words_1ga4148e4b4a03bff2e7715274ae4b126eb)
* [WORD\_TYPE\_STRBUF](col_word_int_8h.md#group__words_1ga31e3979e5f5419111f813131cd508768)
* [WORD\_TYPE\_STRHASHMAP](col_word_int_8h.md#group__words_1ga4b4fdf9a2320675d8dd1dc29d0007564)
* [WORD\_TYPE\_STRTRIEMAP](col_word_int_8h.md#group__words_1gae4ef7e39bd92ee96414ee98c844065ec)
* [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)
* [WORD\_TYPE\_SUBROPE](col_word_int_8h.md#group__words_1gaadd34ba690f1e2711ee1b6ae965fd8e7)
* [WORD\_TYPE\_UCSSTR](col_word_int_8h.md#group__words_1ga3a96c4366162e66944451d0b2ddb9221)
* [WORD\_TYPE\_UTFSTR](col_word_int_8h.md#group__words_1gaed71812c790125f3aeb204e6ba4f55b4)
* [WORD\_TYPE\_VECTOR](col_word_int_8h.md#group__words_1gadf6c66e5c2f9fcdf213ae40d253c153f)
* [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb)
* [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)
* [WORD\_TYPEINFO](col_word_int_8h.md#group__custom__words_1gafc962791c45a5dd5bb034050444084be)
* [WORD\_VOIDLIST\_LENGTH](col_word_int_8h.md#group__voidlist__words_1gaa336b44598700785e9f948fdd3a0da58)
* [WORD\_WRAP\_TYPE](col_word_int_8h.md#group__wrap__words_1ga6f350f679e04dbdc05e08f193bad9d1f)

**Referenced by**:

* [AddSynonymField](col_word_8c.md#group__words_1ga058a6e96bd5370d04936b59a3d3c48c9)
* [Col\_BoolWordValue](col_word_8h.md#group__words_1ga3237f3b47ae8e74b1169b6ebd3cf6ebe)
* [CommitBuffer](col_str_buf_8c.md#group__strbuf__words_1ga679d9a4fab2d369567c364325d0e7af5)
* [CompareStrings](col_hash_8c.md#group__hashmap__words_1gacd46dc7644f9fb2e0a56e8a0b85c6763)
* [HashString](col_hash_8c.md#group__hashmap__words_1ga5a354bdba1e95d6747bc07725902275c)

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

**References**:

* [COL\_BOOL](col_word_8h.md#group__words_1gaa868d0b25f066a1af86e13823e140307)
* [COL\_ERROR\_BOOL](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aa004747a3d62aadf448c12577cf19c7f)
* [Col\_WordType](col_word_8h.md#group__words_1gab0f27c794b1e7ed60b537e2ce94b4408)
* [TYPECHECK](col_internal_8h.md#group__error_1gaa780a70ef44d8ae2fb023777a35ade9a)
* [WORD\_BOOL\_VALUE](col_word_int_8h.md#group__bool__words_1ga321a19e253b32f551bdee2e2fa104d37)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

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

**References**:

* [COL\_ERROR\_INT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a832354f2b8d5ee385ab0a6a89ce0b5d5)
* [COL\_INT](col_word_8h.md#group__words_1gaf0ec1d910f6ba19ede429284179b81fd)
* [TYPECHECK](col_internal_8h.md#group__error_1gaa780a70ef44d8ae2fb023777a35ade9a)
* [WORD\_INTWRAP\_VALUE](col_word_int_8h.md#group__int__wrappers_1ga1110d15dc1187fc3d3f1f7c47305200e)
* [WORD\_SMALLINT\_VALUE](col_word_int_8h.md#group__smallint__words_1ga9d8dacbb3b5ccd6780c031ce039aeefb)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_SMALLINT](col_word_int_8h.md#group__words_1gacaa5b57dfa5151a5852a1616bad80d3d)
* [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)
* [WORD\_WRAP\_TYPE](col_word_int_8h.md#group__wrap__words_1ga6f350f679e04dbdc05e08f193bad9d1f)

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

**References**:

* [COL\_ERROR\_FLOAT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a4388bca7643872d2e7343fee8ed2f616)
* [COL\_FLOAT](col_word_8h.md#group__words_1gaf129d60ea367bc8f58d1a060d0fdba30)
* [TYPECHECK](col_internal_8h.md#group__error_1gaa780a70ef44d8ae2fb023777a35ade9a)
* [WORD\_FPWRAP\_VALUE](col_word_int_8h.md#group__fp__wrappers_1gaf730548e2147681f5248e8ecfc35ef84)
* [WORD\_SMALLFP\_VALUE](col_word_int_8h.md#group__smallfp__words_1gae3d867f57b6b1e2bb0d0919aee9a711e)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_SMALLFP](col_word_int_8h.md#group__words_1gaf5bde441039615017c30996b222a55c8)
* [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)
* [WORD\_WRAP\_TYPE](col_word_int_8h.md#group__wrap__words_1ga6f350f679e04dbdc05e08f193bad9d1f)

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

**References**:

* [HasSynonymField](col_word_8c.md#group__words_1ga7cd9782c127cd58eea6ab39c05f54eee)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)
* [WORD\_SYNONYM](col_word_int_8h.md#group__regular__words_1ga19cfddbcf0127f5088803cc68ddb8eaa)

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

**References**:

* [AddSynonymField](col_word_8c.md#group__words_1ga058a6e96bd5370d04936b59a3d3c48c9)
* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [HasSynonymField](col_word_8c.md#group__words_1ga7cd9782c127cd58eea6ab39c05f54eee)
* [WORD\_SYNONYM](col_word_int_8h.md#group__regular__words_1ga19cfddbcf0127f5088803cc68ddb8eaa)

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

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [HasSynonymField](col_word_8c.md#group__words_1ga7cd9782c127cd58eea6ab39c05f54eee)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)
* [WORD\_SYNONYM](col_word_int_8h.md#group__regular__words_1ga19cfddbcf0127f5088803cc68ddb8eaa)

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

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [CELL\_PAGE](col_internal_8h.md#group__pages__cells_1gabe4fc1fd7a45bf2858948e3a06710a2b)
* [EnterProtectRoots](col_platform_8h.md#group__arch_1ga037203dea1294535ac25be8712d708d6)
* [ThreadData::groupData](struct_thread_data.md#struct_thread_data_1aefbdf49c641476274db5326c60853022)
* [LeaveProtectRoots](col_platform_8h.md#group__arch_1gaee9721bdb7cc0ecca4a8ee295eadd446)
* [PAGE\_GENERATION](col_internal_8h.md#group__pages__cells_1gaa7215b15e159db56ed2bc30d66488fd8)
* [PlatGetThreadData](col_unix_platform_8h.md#group__arch__unix_1ga6964b3c4d4787a9defb7aae57825d92c)
* [PoolAllocCells](col_alloc_8c.md#group__alloc_1gafd84f35bab195e5e45a3338903dbd837)
* [PRECONDITION\_GCPROTECTED](col_gc_8c.md#group__gc_1ga1ce49cbb2ef788b5e3d501b57b903289)
* [ROOT\_GET\_LEAF](col_internal_8h.md#group__gc__roots_1gaf093eb2c5ec8150f06a69bd10df6be6a)
* [ROOT\_GET\_NODE](col_internal_8h.md#group__gc__roots_1ga33fa98f95a7bab38652b351e60d60bae)
* [ROOT\_IS\_LEAF](col_internal_8h.md#group__gc__roots_1gaa8c5f8c118fe2e0c3c0bcfb0d8be9a08)
* [ROOT\_LEAF\_INIT](col_internal_8h.md#group__gc__roots_1ga283423b988b4f6979e2f6e8d2b8a8ba9)
* [ROOT\_LEAF\_REFCOUNT](col_internal_8h.md#group__gc__roots_1gaef6c67ed97c9ceace9b0818bed0110b2)
* [ROOT\_LEAF\_SOURCE](col_internal_8h.md#group__gc__roots_1gae634c20d504ac8bd99ab26f4ddc3ee12)
* [ROOT\_NODE\_INIT](col_internal_8h.md#group__gc__roots_1ga9e5b5f33b34e01b54b6addaaaa9ace5e)
* [ROOT\_NODE\_LEFT](col_internal_8h.md#group__gc__roots_1ga27676041bc270c4dfc8c7caea4e64274)
* [ROOT\_NODE\_MASK](col_internal_8h.md#group__gc__roots_1gaa1f0481c45b7c14cf933b91d9f27a541)
* [ROOT\_NODE\_RIGHT](col_internal_8h.md#group__gc__roots_1gaf86512b7113a6afaea849d480070dd33)
* [ROOT\_PARENT](col_internal_8h.md#group__gc__roots_1gadd16c38bcc5016a0c43a17442c232ffa)
* [GroupData::rootPool](struct_group_data.md#struct_group_data_1a81941409b9917b41bb20d3572b2b4ec7)
* [GroupData::roots](struct_group_data.md#struct_group_data_1af753ea77322dc01776c856b584b219e7)
* [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)
* [WORD\_PINNED](col_word_int_8h.md#group__regular__words_1gad20cf13be09a354418d8615e6f2f2193)
* [WORD\_SET\_PINNED](col_word_int_8h.md#group__regular__words_1ga7ae30ab27827ba70ae1265b5f637101a)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CHARBOOL](col_word_int_8h.md#group__words_1ga1422c37e0c7d668fad81b0710863dad5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_NIL](col_word_int_8h.md#group__words_1ga1f4d1db7619649bb51aeddd67c8b242f)
* [WORD\_TYPE\_SMALLFP](col_word_int_8h.md#group__words_1gaf5bde441039615017c30996b222a55c8)
* [WORD\_TYPE\_SMALLINT](col_word_int_8h.md#group__words_1gacaa5b57dfa5151a5852a1616bad80d3d)
* [WORD\_TYPE\_SMALLSTR](col_word_int_8h.md#group__words_1ga4148e4b4a03bff2e7715274ae4b126eb)
* [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb)

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

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [CELL\_INDEX](col_internal_8h.md#group__pages__cells_1gaa6e93c045bc319412f36118ea1cfbb05)
* [CELL\_PAGE](col_internal_8h.md#group__pages__cells_1gabe4fc1fd7a45bf2858948e3a06710a2b)
* [ClearCells](col_alloc_8c.md#group__alloc_1ga5d95195ed024066e939d0564549e865d)
* [EnterProtectRoots](col_platform_8h.md#group__arch_1ga037203dea1294535ac25be8712d708d6)
* [ThreadData::groupData](struct_thread_data.md#struct_thread_data_1aefbdf49c641476274db5326c60853022)
* [LeaveProtectRoots](col_platform_8h.md#group__arch_1gaee9721bdb7cc0ecca4a8ee295eadd446)
* [PlatGetThreadData](col_unix_platform_8h.md#group__arch__unix_1ga6964b3c4d4787a9defb7aae57825d92c)
* [PRECONDITION\_GCPROTECTED](col_gc_8c.md#group__gc_1ga1ce49cbb2ef788b5e3d501b57b903289)
* [ROOT\_GET\_NODE](col_internal_8h.md#group__gc__roots_1ga33fa98f95a7bab38652b351e60d60bae)
* [ROOT\_IS\_LEAF](col_internal_8h.md#group__gc__roots_1gaa8c5f8c118fe2e0c3c0bcfb0d8be9a08)
* [ROOT\_LEAF\_REFCOUNT](col_internal_8h.md#group__gc__roots_1gaef6c67ed97c9ceace9b0818bed0110b2)
* [ROOT\_LEAF\_SOURCE](col_internal_8h.md#group__gc__roots_1gae634c20d504ac8bd99ab26f4ddc3ee12)
* [ROOT\_NODE\_LEFT](col_internal_8h.md#group__gc__roots_1ga27676041bc270c4dfc8c7caea4e64274)
* [ROOT\_NODE\_MASK](col_internal_8h.md#group__gc__roots_1gaa1f0481c45b7c14cf933b91d9f27a541)
* [ROOT\_NODE\_RIGHT](col_internal_8h.md#group__gc__roots_1gaf86512b7113a6afaea849d480070dd33)
* [ROOT\_PARENT](col_internal_8h.md#group__gc__roots_1gadd16c38bcc5016a0c43a17442c232ffa)
* [GroupData::roots](struct_group_data.md#struct_group_data_1af753ea77322dc01776c856b584b219e7)
* [TestCell](col_alloc_8c.md#group__alloc_1gade7bbd62a937c3b2ed2f32c34c6c60a6)
* [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)
* [WORD\_CLEAR\_PINNED](col_word_int_8h.md#group__regular__words_1ga04a19fb132382d52fa42d3d3e4237f2f)
* [WORD\_PINNED](col_word_int_8h.md#group__regular__words_1gad20cf13be09a354418d8615e6f2f2193)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CHARBOOL](col_word_int_8h.md#group__words_1ga1422c37e0c7d668fad81b0710863dad5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_NIL](col_word_int_8h.md#group__words_1ga1f4d1db7619649bb51aeddd67c8b242f)
* [WORD\_TYPE\_SMALLFP](col_word_int_8h.md#group__words_1gaf5bde441039615017c30996b222a55c8)
* [WORD\_TYPE\_SMALLINT](col_word_int_8h.md#group__words_1gacaa5b57dfa5151a5852a1616bad80d3d)
* [WORD\_TYPE\_SMALLSTR](col_word_int_8h.md#group__words_1ga4148e4b4a03bff2e7715274ae4b126eb)
* [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb)

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

## Word Type Identifiers

<a id="group__words_1word_type_ids"></a>
 Internal word type identifiers.





Predefined type IDs for regular word types are chosen so that their bit 0 is cleared, their bit 1 is set, and their value fit into a byte. This gives up to 64 predefined type IDs (2-254 with steps of 4).





Immediate word type IDs use negative values to avoid clashes with regular word type IDs.

<a id="group__words_1ga1f4d1db7619649bb51aeddd67c8b242f"></a>
### Macro WORD\_TYPE\_NIL

![][public]

```cpp
#define WORD_TYPE_NIL 0
```

Nil singleton.





<a id="group__words_1ga8babfbc77291680db519873c91efdd4c"></a>
### Macro WORD\_TYPE\_CUSTOM

![][public]

```cpp
#define WORD_TYPE_CUSTOM -1
```

[Custom Words](group__custom__words.md#group__custom__words).





<a id="group__words_1gacaa5b57dfa5151a5852a1616bad80d3d"></a>
### Macro WORD\_TYPE\_SMALLINT

![][public]

```cpp
#define WORD_TYPE_SMALLINT -2
```

[Small Integer Words](group__smallint__words.md#group__smallint__words).





<a id="group__words_1gaf5bde441039615017c30996b222a55c8"></a>
### Macro WORD\_TYPE\_SMALLFP

![][public]

```cpp
#define WORD_TYPE_SMALLFP -3
```

[Small Floating Point Words](group__smallfp__words.md#group__smallfp__words).





<a id="group__words_1ga1422c37e0c7d668fad81b0710863dad5"></a>
### Macro WORD\_TYPE\_CHARBOOL

![][public]

```cpp
#define WORD_TYPE_CHARBOOL -4
```

[Character Words](group__char__words.md#group__char__words) _or_ [Boolean Words](group__bool__words.md#group__bool__words).





<a id="group__words_1ga4148e4b4a03bff2e7715274ae4b126eb"></a>
### Macro WORD\_TYPE\_SMALLSTR

![][public]

```cpp
#define WORD_TYPE_SMALLSTR -5
```

[Small String Words](group__smallstr__words.md#group__smallstr__words).





<a id="group__words_1ga5986ba88af901948fd9a78f422001650"></a>
### Macro WORD\_TYPE\_CIRCLIST

![][public]

```cpp
#define WORD_TYPE_CIRCLIST -6
```

[Circular List Words](group__circlist__words.md#group__circlist__words).





<a id="group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb"></a>
### Macro WORD\_TYPE\_VOIDLIST

![][public]

```cpp
#define WORD_TYPE_VOIDLIST -7
```

[Void List Words](group__voidlist__words.md#group__voidlist__words).





<a id="group__words_1ga3c604da44ba72f4661d0ac28f6718cac"></a>
### Macro WORD\_TYPE\_WRAP

![][public]

```cpp
#define WORD_TYPE_WRAP 2
```

[Wrap Words](group__wrap__words.md#group__wrap__words).





<a id="group__words_1ga3a96c4366162e66944451d0b2ddb9221"></a>
### Macro WORD\_TYPE\_UCSSTR

![][public]

```cpp
#define WORD_TYPE_UCSSTR 6
```

[Fixed-Width UCS Strings](group__ucsstr__words.md#group__ucsstr__words).





<a id="group__words_1gaed71812c790125f3aeb204e6ba4f55b4"></a>
### Macro WORD\_TYPE\_UTFSTR

![][public]

```cpp
#define WORD_TYPE_UTFSTR 10
```

[Variable-Width UTF Strings](group__utfstr__words.md#group__utfstr__words).





<a id="group__words_1gaadd34ba690f1e2711ee1b6ae965fd8e7"></a>
### Macro WORD\_TYPE\_SUBROPE

![][public]

```cpp
#define WORD_TYPE_SUBROPE 14
```

[Subropes](group__subrope__words.md#group__subrope__words).





<a id="group__words_1ga677525993a1e4a934e98042c53021c3a"></a>
### Macro WORD\_TYPE\_CONCATROPE

![][public]

```cpp
#define WORD_TYPE_CONCATROPE 18
```

[Concat Ropes](group__concatrope__words.md#group__concatrope__words).





<a id="group__words_1gadf6c66e5c2f9fcdf213ae40d253c153f"></a>
### Macro WORD\_TYPE\_VECTOR

![][public]

```cpp
#define WORD_TYPE_VECTOR 22
```

[Immutable Vectors](group__vector__words.md#group__vector__words).





<a id="group__words_1ga22d76782e9dfd28846b6eeac3547280f"></a>
### Macro WORD\_TYPE\_MVECTOR

![][public]

```cpp
#define WORD_TYPE_MVECTOR 26
```

[Mutable Vectors](group__mvector__words.md#group__mvector__words).





<a id="group__words_1gab019a30aca48483424886bf08f7b7cac"></a>
### Macro WORD\_TYPE\_SUBLIST

![][public]

```cpp
#define WORD_TYPE_SUBLIST 30
```

[Sublists](group__sublist__words.md#group__sublist__words).





<a id="group__words_1ga8f0a60698d7b383460fe868b1c043f19"></a>
### Macro WORD\_TYPE\_CONCATLIST

![][public]

```cpp
#define WORD_TYPE_CONCATLIST 34
```

[Immutable Concat Lists](group__concatlist__words.md#group__concatlist__words).





<a id="group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee"></a>
### Macro WORD\_TYPE\_MCONCATLIST

![][public]

```cpp
#define WORD_TYPE_MCONCATLIST 38
```

[Mutable Concat Lists](group__mconcatlist__words.md#group__mconcatlist__words).





<a id="group__words_1ga4b4fdf9a2320675d8dd1dc29d0007564"></a>
### Macro WORD\_TYPE\_STRHASHMAP

![][public]

```cpp
#define WORD_TYPE_STRHASHMAP 42
```

[String Hash Maps](group__strhashmap__words.md#group__strhashmap__words).





<a id="group__words_1ga230c3d50685afa970c1e0da69feb5811"></a>
### Macro WORD\_TYPE\_INTHASHMAP

![][public]

```cpp
#define WORD_TYPE_INTHASHMAP 46
```

[Integer Hash Maps](group__inthashmap__words.md#group__inthashmap__words).





<a id="group__words_1ga0ccfe6bc407371b3c2cde0a2da83f9fa"></a>
### Macro WORD\_TYPE\_HASHENTRY

![][public]

```cpp
#define WORD_TYPE_HASHENTRY 50
```

[Immutable Hash Entries](group__hashentry__words.md#group__hashentry__words).





<a id="group__words_1ga4c79463f98f0ec9296451862e5d0b76c"></a>
### Macro WORD\_TYPE\_MHASHENTRY

![][public]

```cpp
#define WORD_TYPE_MHASHENTRY 54
```

[Hash Entries](group__mhashentry__words.md#group__mhashentry__words).





<a id="group__words_1gab1a5b3b65a05c74cd3973db9dce4a781"></a>
### Macro WORD\_TYPE\_INTHASHENTRY

![][public]

```cpp
#define WORD_TYPE_INTHASHENTRY 58
```

[Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words).





<a id="group__words_1ga1758f2fa0c44200f5782e548c5b33c7e"></a>
### Macro WORD\_TYPE\_MINTHASHENTRY

![][public]

```cpp
#define WORD_TYPE_MINTHASHENTRY 62
```

[Integer Hash Entries](group__minthashentry__words.md#group__minthashentry__words).





<a id="group__words_1gae4ef7e39bd92ee96414ee98c844065ec"></a>
### Macro WORD\_TYPE\_STRTRIEMAP

![][public]

```cpp
#define WORD_TYPE_STRTRIEMAP 66
```

[String Trie Maps](group__strtriemap__words.md#group__strtriemap__words).





<a id="group__words_1ga9da4310532cf6307f784bd6f33471218"></a>
### Macro WORD\_TYPE\_INTTRIEMAP

![][public]

```cpp
#define WORD_TYPE_INTTRIEMAP 70
```

[Integer Trie Maps](group__inttriemap__words.md#group__inttriemap__words).





<a id="group__words_1gae52162432efdceb0ac49e6332b213401"></a>
### Macro WORD\_TYPE\_TRIENODE

![][public]

```cpp
#define WORD_TYPE_TRIENODE 74
```

[Immutable Trie Nodes](group__trienode__words.md#group__trienode__words).





<a id="group__words_1ga20d128dd2702743f2027be54817f2275"></a>
### Macro WORD\_TYPE\_MTRIENODE

![][public]

```cpp
#define WORD_TYPE_MTRIENODE 78
```

[Trie Nodes](group__mtrienode__words.md#group__mtrienode__words).





<a id="group__words_1ga9ff5e124f29796ce3b1dd377f7d6d59f"></a>
### Macro WORD\_TYPE\_STRTRIENODE

![][public]

```cpp
#define WORD_TYPE_STRTRIENODE 82
```

[Immutable String Trie Nodes](group__strtrienode__words.md#group__strtrienode__words).





<a id="group__words_1ga82e8b6ff4abcd49f5ffa67cc45b17358"></a>
### Macro WORD\_TYPE\_MSTRTRIENODE

![][public]

```cpp
#define WORD_TYPE_MSTRTRIENODE 86
```

[String Trie Nodes](group__mstrtrienode__words.md#group__mstrtrienode__words).





<a id="group__words_1gac4adeeed11aa6a07235dfe4099c18074"></a>
### Macro WORD\_TYPE\_INTTRIENODE

![][public]

```cpp
#define WORD_TYPE_INTTRIENODE 90
```

[Immutable Integer Trie Nodes](group__inttrienode__words.md#group__inttrienode__words).





<a id="group__words_1gac0b62fa5b054dc6713a5fc7fd69298da"></a>
### Macro WORD\_TYPE\_MINTTRIENODE

![][public]

```cpp
#define WORD_TYPE_MINTTRIENODE 94
```

[Integer Trie Nodes](group__minttrienode__words.md#group__minttrienode__words).





<a id="group__words_1ga2a17ea1e39ad925fc0057cd928cdd49c"></a>
### Macro WORD\_TYPE\_TRIELEAF

![][public]

```cpp
#define WORD_TYPE_TRIELEAF 98
```

[Immutable Trie Leaves](group__trieleaf__words.md#group__trieleaf__words).





<a id="group__words_1ga4c7f02b7545a17f527d59a2c66e9d0fa"></a>
### Macro WORD\_TYPE\_MTRIELEAF

![][public]

```cpp
#define WORD_TYPE_MTRIELEAF 102
```

[Trie Leaves](group__mtrieleaf__words.md#group__mtrieleaf__words).





<a id="group__words_1ga896310a96176f87d4ec0bd06eabf55f7"></a>
### Macro WORD\_TYPE\_INTTRIELEAF

![][public]

```cpp
#define WORD_TYPE_INTTRIELEAF 106
```

[Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words).





<a id="group__words_1ga81a397c929cd0fa5f89c5a01ce2a1487"></a>
### Macro WORD\_TYPE\_MINTTRIELEAF

![][public]

```cpp
#define WORD_TYPE_MINTTRIELEAF 110
```

[Integer Trie Leaves](group__minttrieleaf__words.md#group__minttrieleaf__words).





<a id="group__words_1ga31e3979e5f5419111f813131cd508768"></a>
### Macro WORD\_TYPE\_STRBUF

![][public]

```cpp
#define WORD_TYPE_STRBUF 114
```

[String Buffers](group__strbuf__words.md#group__strbuf__words).





<a id="group__words_1ga2c7094c849fb96cd773345ef34fa3e34"></a>
### Macro WORD\_TYPE\_REDIRECT

![][public]

```cpp
#define WORD_TYPE_REDIRECT 254
```

[Redirect Words](group__redirect__words.md#group__redirect__words).





<a id="group__words_1ga2dbd46684921f49af69a8b0032d372b5"></a>
### Macro WORD\_TYPE\_UNKNOWN

![][public]

```cpp
#define WORD_TYPE_UNKNOWN
```

Used as a tag in the source code to mark places where predefined type specific code is needed.

Search for this tag when adding new predefined word types.



## Functions

<a id="group__words_1ga7cd9782c127cd58eea6ab39c05f54eee"></a>
### Function HasSynonymField

![][private]
![][static]

```cpp
static int HasSynonymField(Col_Word word)
```

Test whether the word has a synonym field.

**Returns**:

Whether the word has a synonym field.



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **word**: The word to test.

**Return type**: int

**References**:

* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CUSTOM](col_word_int_8h.md#group__words_1ga8babfbc77291680db519873c91efdd4c)
* [WORD\_TYPE\_INTHASHMAP](col_word_int_8h.md#group__words_1ga230c3d50685afa970c1e0da69feb5811)
* [WORD\_TYPE\_INTTRIEMAP](col_word_int_8h.md#group__words_1ga9da4310532cf6307f784bd6f33471218)
* [WORD\_TYPE\_STRHASHMAP](col_word_int_8h.md#group__words_1ga4b4fdf9a2320675d8dd1dc29d0007564)
* [WORD\_TYPE\_STRTRIEMAP](col_word_int_8h.md#group__words_1gae4ef7e39bd92ee96414ee98c844065ec)
* [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)

**Referenced by**:

* [AddSynonymField](col_word_8c.md#group__words_1ga058a6e96bd5370d04936b59a3d3c48c9)
* [Col\_WordAddSynonym](col_word_8h.md#group__words_1ga065c63fe593a474258ee99f08b752bb4)
* [Col\_WordClearSynonym](col_word_8h.md#group__words_1ga7464c00091f4eba37aad1b6dbf5912cd)
* [Col\_WordSynonym](col_word_8h.md#group__words_1ga20f25ebcc58b56c911fc2a6db1c81332)

<a id="group__words_1ga058a6e96bd5370d04936b59a3d3c48c9"></a>
### Function AddSynonymField

![][private]
![][static]

```cpp
static void AddSynonymField(Col_Word *wordPtr)
```

Return a word that is semantically identical to the given one and has a synonym field.

**Returns**:

A word with a synonym field.

**Side Effect**:

Allocates memory cells.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **wordPtr**: Points to the word to convert.

**Return type**: void

**References**:

* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)
* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_WordType](col_word_8h.md#group__words_1gab0f27c794b1e7ed60b537e2ce94b4408)
* [HasSynonymField](col_word_8c.md#group__words_1ga7cd9782c127cd58eea6ab39c05f54eee)
* [WORD\_FPWRAP\_INIT](col_word_int_8h.md#group__fp__wrappers_1ga66d2954c1e1ea98185f0187e465f0290)
* [WORD\_INTWRAP\_INIT](col_word_int_8h.md#group__int__wrappers_1ga9656b5f429e598b3f268d4f9d3821967)
* [WORD\_SMALLFP\_VALUE](col_word_int_8h.md#group__smallfp__words_1gae3d867f57b6b1e2bb0d0919aee9a711e)
* [WORD\_SMALLINT\_VALUE](col_word_int_8h.md#group__smallint__words_1ga9d8dacbb3b5ccd6780c031ce039aeefb)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_SMALLFP](col_word_int_8h.md#group__words_1gaf5bde441039615017c30996b222a55c8)
* [WORD\_TYPE\_SMALLINT](col_word_int_8h.md#group__words_1gacaa5b57dfa5151a5852a1616bad80d3d)
* [WORD\_WRAP\_INIT](col_word_int_8h.md#group__word__wrappers_1ga5176d0b6e39fc4a27effa648b0f88d27)

**Referenced by**:

* [Col\_WordAddSynonym](col_word_8h.md#group__words_1ga065c63fe593a474258ee99f08b752bb4)

## Macros

<a id="group__words_1ga014e27ea4160eb3845ac495a22c232f5"></a>
### Macro WORD\_TYPE

![][public]

```cpp
#define WORD_TYPE     /* Nil? */                                                  \
    ((!(word))?                         [WORD\_TYPE\_NIL](col_word_int_8h.md#group__words_1ga1f4d1db7619649bb51aeddd67c8b242f)           \
    /* Immediate Word? */                                       \
    :(((uintptr_t)(word))&15)?          [immediateWordTypes](col_word_int_8h.md#group__immediate__words_1gadc93860da89f6a52a884c10b7835aaac)[(((uintptr_t)(word))&31)] \
    /* Predefined Type ID? */                                   \
    :((((uint8_t *)(word))[0])&2)?      [WORD\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga0ce63a12a32f50c4fada9a19f40356d0)(word)       \
    /* Custom Type */                                           \
    :                                   [WORD\_TYPE\_CUSTOM](col_word_int_8h.md#group__words_1ga8babfbc77291680db519873c91efdd4c))( word )
```

Get word type identifier.

**Parameters**:

* **word**: Word to get type for.


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.


**Returns**:

Word type identifier.






**See also**: [Word Type Identifiers](#group__words_1word_type_ids), [immediateWordTypes](col_word_int_8h.md#group__immediate__words_1gadc93860da89f6a52a884c10b7835aaac), [Immediate Words](group__immediate__words.md#group__immediate__words), [Regular Words](group__regular__words.md#group__regular__words)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)