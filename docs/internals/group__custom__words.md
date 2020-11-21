<a id="group__custom__words"></a>
# Custom Words

Custom words are words defined by applicative code.

They can extend existing word types like ropes or maps, or define application-specific data types.







**See also**: [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type)





**Requirements**:


* Custom words must store a pointer to their [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type).

* Custom words can be part of a synonym chain.

* Some custom word types define cleanup procedures ([Col\_CustomWordType.freeProc](struct_col___custom_word_type.md#struct_col___custom_word_type_1a15e8e2dd2cb2eedf153d89925a359712)), such words need to be traversed during the sweep phase of the GC process. To do so, they are singly-linked together and so need to store the next word in chain.


**Parameters**:

* **Type**: Type descriptor.
* **Synonym**: [Generic word synonym field](col_word_int_8h.md#group__regular__words_1ga19cfddbcf0127f5088803cc68ddb8eaa).
* **Next**: Next word in custom cleanup chain (only when [Col\_CustomWordType.freeProc](struct_col___custom_word_type.md#struct_col___custom_word_type_1a15e8e2dd2cb2eedf153d89925a359712) is non-NULL).
* **Data**: Custom type payload.


**Cell Layout**:

On all architectures the cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        custom_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="10">0</td>
                <td sides="B" width="10">1</td>
                <td sides="B" width="140" align="left">2</td><td sides="B" width="150" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_PINNED" title="WORD_PINNED">P</td>
                <td>0</td>
                <td href="@ref WORD_TYPEINFO" title="WORD_TYPEINFO" colspan="2">Type</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_SYNONYM" title="WORD_SYNONYM" colspan="4" width="320">Synonym</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_CUSTOM_NEXT" title="WORD_CUSTOM_NEXT" colspan="4">Next (optional)</td>
            </tr>
            <tr><td sides="R">.</td>
                <td href="@ref WORD_CUSTOM_DATA" title="WORD_CUSTOM_DATA" colspan="4" rowspan="3">Type-specific data</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }
    







?> Custom words are described by a [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type) type descriptor. As such structures are machine word-aligned, this means that the four lower bits of their address are cleared.




**See also**: [WORD\_TYPE\_CUSTOM](col_word_int_8h.md#group__words_1ga8babfbc77291680db519873c91efdd4c), [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type)

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

**References**:

* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)
* [COL\_HASHMAP](col_word_8h.md#group__words_1gae3509634e52a76014e96c2575b5d8092)
* [Col\_NewCustomHashMap](col_hash_8h.md#group__customhashmap__words_1gad516fa9041eb514e2c5193eb5d958f0e)
* [Col\_NewCustomTrieMap](col_trie_8h.md#group__customtriemap__words_1ga18de761037e23e723d8d62aef7d6246c)
* [COL\_TRIEMAP](col_word_8h.md#group__words_1ga7922babbc856f5670805da2267d72ff0)
* [CUSTOM\_HEADER\_SIZE](col_word_int_8h.md#group__custom__words_1gafc60bf09c25a9eaed4d5271ebc675b80)
* [Col\_CustomWordType::freeProc](struct_col___custom_word_type.md#struct_col___custom_word_type_1a15e8e2dd2cb2eedf153d89925a359712)
* [RememberSweepable](col_gc_8c.md#group__gc_1ga23c84fc6b7da85d87751fa5788e3f002)
* [Col\_CustomWordType::type](struct_col___custom_word_type.md#struct_col___custom_word_type_1af9482efe5a6408bc622320619c3ccf9f)
* [WORD\_CUSTOM\_DATA](col_word_int_8h.md#group__custom__words_1ga5a4c487ea8aec133b0a2e18d07793191)
* [WORD\_CUSTOM\_INIT](col_word_int_8h.md#group__custom__words_1gaf513110792f0f9dcf424e5d20060a328)
* [WORD\_CUSTOM\_SIZE](col_word_int_8h.md#group__custom__words_1ga2610704afbab6a5ec38e561f17dde6ea)

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

**References**:

* [COL\_ERROR\_CUSTOMWORD](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aeaf92bf356a97fb87088bd24e551411c)
* [COL\_HASHMAP](col_word_8h.md#group__words_1gae3509634e52a76014e96c2575b5d8092)
* [COL\_TRIEMAP](col_word_8h.md#group__words_1ga7922babbc856f5670805da2267d72ff0)
* [CUSTOM\_HEADER\_SIZE](col_word_int_8h.md#group__custom__words_1gafc60bf09c25a9eaed4d5271ebc675b80)
* [CUSTOMHASHMAP\_HEADER\_SIZE](col_hash_int_8h.md#group__customhashmap__words_1ga983e7c0095b8a45a118d43878c885814)
* [CUSTOMTRIEMAP\_HEADER\_SIZE](col_trie_int_8h.md#group__customtriemap__words_1gab1b1757562f39ce72387cd26b4ae8f2a)
* [Col\_CustomWordType::type](struct_col___custom_word_type.md#struct_col___custom_word_type_1af9482efe5a6408bc622320619c3ccf9f)
* [TYPECHECK](col_internal_8h.md#group__error_1gaa780a70ef44d8ae2fb023777a35ade9a)
* [WORD\_CUSTOM\_DATA](col_word_int_8h.md#group__custom__words_1ga5a4c487ea8aec133b0a2e18d07793191)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CUSTOM](col_word_int_8h.md#group__words_1ga8babfbc77291680db519873c91efdd4c)
* [WORD\_TYPEINFO](col_word_int_8h.md#group__custom__words_1gafc962791c45a5dd5bb034050444084be)

## Custom Word Constants

<a id="group__custom__words_1gafc60bf09c25a9eaed4d5271ebc675b80"></a>
### Macro CUSTOM\_HEADER\_SIZE

![][public]

```cpp
#define CUSTOM_HEADER_SIZE (sizeof([Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type) *)+sizeof([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *))
```

Byte size of custom word header.





## Custom Word Creation

<a id="group__custom__words_1gaf513110792f0f9dcf424e5d20060a328"></a>
### Macro WORD\_CUSTOM\_INIT

![][public]

```cpp
#define WORD_CUSTOM_INIT     [WORD\_SET\_TYPEINFO](col_word_int_8h.md#group__custom__words_1gad327193f2befdd20bebdd2ee74023310)((word), (typeInfo)); \
    [WORD\_SYNONYM](col_word_int_8h.md#group__regular__words_1ga19cfddbcf0127f5088803cc68ddb8eaa)(word) = [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065);( word ,typeInfo )
```

Custom word initializer.

**Parameters**:

* **word**: Word to initialize.
* **typeInfo**: [WORD\_SET\_TYPEINFO](col_word_int_8h.md#group__custom__words_1gad327193f2befdd20bebdd2ee74023310).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPEINFO](col_word_int_8h.md#group__custom__words_1gafc962791c45a5dd5bb034050444084be)



## Custom Word Accessors

<a id="group__custom__words_1ga3f6badbfe1336dd485dd92d3787f465f"></a>
### Macro ROTATE\_LEFT

![][public]

```cpp
#define ROTATE_LEFT     (((value)>>((sizeof(value)-1)*8)) | ((value)<<8))( value )
```

Rotate value left by one byte.

**Parameters**:

* **value**: Value to rotate.


!> **Warning** \
Argument **Value** is referenced several times by the macro. Make sure to avoid any side effect.



<a id="group__custom__words_1ga60667a36b002f88176aba33daaf467bc"></a>
### Macro ROTATE\_RIGHT

![][public]

```cpp
#define ROTATE_RIGHT     (((value)<<((sizeof(value)-1)*8)) | ((value)>>8))( value )
```

Rotate value right by one byte.

**Parameters**:

* **value**: Value to rotate.


!> **Warning** \
Argument **Value** is referenced several times by the macro. Make sure to avoid any side effect.



<a id="group__custom__words_1gafc962791c45a5dd5bb034050444084be"></a>
### Macro WORD\_TYPEINFO

![][public]

```cpp
#define WORD_TYPEINFO         (([Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type) *)([ROTATE\_LEFT](col_word_int_8h.md#group__custom__words_1ga3f6badbfe1336dd485dd92d3787f465f)(*(uintptr_t *)(word))&~1))( word )
```

Get [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type) custom type descriptor.

**Parameters**:

* **word**: Word to access.


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_SET\_TYPEINFO](col_word_int_8h.md#group__custom__words_1gad327193f2befdd20bebdd2ee74023310)



<a id="group__custom__words_1gad327193f2befdd20bebdd2ee74023310"></a>
### Macro WORD\_SET\_TYPEINFO

![][public]

```cpp
#define WORD_SET_TYPEINFO         (*(uintptr_t *)(word) = [ROTATE\_RIGHT](col_word_int_8h.md#group__custom__words_1ga60667a36b002f88176aba33daaf467bc)(((uintptr_t)(addr))))( word ,addr )
```

Set [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type) custom type descriptor.

**Parameters**:

* **word**: Word to access.
* **addr**: Address of [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type) descriptor.


!> **Warning** \
Arguments are referenced several times by the macro. Make sure to avoid any side effect.


**Side Effect**:

This also clears the pinned flag.



**See also**: [WORD\_TYPEINFO](col_word_int_8h.md#group__custom__words_1gafc962791c45a5dd5bb034050444084be)



<a id="group__custom__words_1ga0d40b7c193abd59149d3c4f56b8c343e"></a>
### Macro WORD\_CUSTOM\_NEXT

![][public]

```cpp
#define WORD_CUSTOM_NEXT (*([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(((char *)(word))+headerSize))( word ,typeInfo ,headerSize )
```

Get/set next word in custom cleanup chain.

!> **Warning** \
Only when [Col\_CustomWordType.freeProc](struct_col___custom_word_type.md#struct_col___custom_word_type_1a15e8e2dd2cb2eedf153d89925a359712) is non-NULL.


?> Macro is L-Value and suitable for both read/write operations.



<a id="group__custom__words_1ga5a4c487ea8aec133b0a2e18d07793191"></a>
### Macro WORD\_CUSTOM\_DATA

![][public]

```cpp
#define WORD_CUSTOM_DATA ((void *)(&[WORD\_CUSTOM\_NEXT](col_word_int_8h.md#group__custom__words_1ga0d40b7c193abd59149d3c4f56b8c343e)((word), (typeInfo), (headerSize))+((typeInfo)->freeProc?1:0)))( word ,typeInfo ,headerSize )
```

Pointer to beginning of custom word data.





<a id="group__custom__words_1ga2610704afbab6a5ec38e561f17dde6ea"></a>
### Macro WORD\_CUSTOM\_SIZE

![][public]

```cpp
#define WORD_CUSTOM_SIZE     ([NB\_CELLS](col_internal_8h.md#group__pages__cells_1ga6969cfc3c9b2913a913df84f7842ce74)((headerSize)+((typeInfo)->freeProc?sizeof([Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) *):0)+(size)))( typeInfo ,headerSize ,size )
```

Get number of cells taken by the custom word.

**Parameters**:

* **typeInfo**: WORD_TYPEINFO
* **headerSize**: Byte size of predefined header.
* **size**: Byte size of custom word data.


**Returns**:

Number of cells taken by word.



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)