<a id="group__mhashentry__words"></a>
# Hash Entries



Hash entries are [Map Entries](group__mapentry__words.md#group__mapentry__words) with hash-specific data.






**Requirements**:


* Hash entry words, like generic [Map Entries](group__mapentry__words.md#group__mapentry__words), use one single cell.

* Hash entries sharing the same bucket are chained using a linked list, so each entry store must store a pointer to the next entry in chain.

* In addition we cache part of the hash value in the remaining bytes to avoid recomputing in many cases.


**Parameters**:

* **Key**: [Generic map entry key field](col_map_int_8h.md#group__mapentry__words_1ga8664d15fae4553b47b658ac7ceb1443a).
* **Value**: [Generic map entry value field](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064).
* **Next**: Next entry in bucket. Also used during iteration.
* **Hash**: Higher order bits of the hash value for fast negative test and rehashing. The full value is computed by combining these high order bits and the bucket index whenever possible, else the value is recomputed from the key.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        hashentry_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_HASHENTRY_HASH" title="WORD_HASHENTRY_HASH" colspan="2">Hash</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_HASHENTRY_NEXT" title="WORD_HASHENTRY_NEXT" colspan="4">Next</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_MAPENTRY_KEY" title="WORD_MAPENTRY_KEY" colspan="4">Key (Generic)</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_MAPENTRY_VALUE" title="WORD_MAPENTRY_VALUE" colspan="4">Value (Generic)</td>
            </tr>
            </table>
        >]
    }
    









**See also**: [Map Entries](group__mapentry__words.md#group__mapentry__words), [Hash Maps](group__hashmap__words.md#group__hashmap__words)

## Submodules

* [Immutable Hash Entries](group__hashentry__words.md#group__hashentry__words)

## Hash Entry Creation

<a id="group__mhashentry__words_1ga467a83e27fa5bfd5eca35e6312d5fe5e"></a>
### Macro WORD\_MHASHENTRY\_INIT

![][public]

```cpp
#define WORD_MHASHENTRY_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_MHASHENTRY](col_word_int_8h.md#group__words_1ga4c79463f98f0ec9296451862e5d0b76c)); \
    [WORD\_MAPENTRY\_KEY](col_map_int_8h.md#group__mapentry__words_1ga8664d15fae4553b47b658ac7ceb1443a)(word) = (key); \
    [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064)(word) = (value); \
    [WORD\_HASHENTRY\_NEXT](col_hash_int_8h.md#group__mhashentry__words_1ga9f087b8c13513115c5e1b19c86fbe145)(word) = (next); \
    [WORD\_HASHENTRY\_SET\_HASH](col_hash_int_8h.md#group__mhashentry__words_1ga6cba1b5e86476a239bc36a7f2818b1df)(word, hash);( word ,key ,value ,next ,hash )
```

Mutable hash entry word initializer.

**Parameters**:

* **word**: Word to initialize.
* **key**: [WORD\_MAPENTRY\_KEY](col_map_int_8h.md#group__mapentry__words_1ga8664d15fae4553b47b658ac7ceb1443a).
* **value**: [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064).
* **next**: [WORD\_HASHENTRY\_NEXT](col_hash_int_8h.md#group__mhashentry__words_1ga9f087b8c13513115c5e1b19c86fbe145).
* **hash**: [WORD\_HASHENTRY\_SET\_HASH](col_hash_int_8h.md#group__mhashentry__words_1ga6cba1b5e86476a239bc36a7f2818b1df).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_MHASHENTRY](col_word_int_8h.md#group__words_1ga4c79463f98f0ec9296451862e5d0b76c)



## Hash Entry Accessors

<a id="group__mhashentry__words_1ga1f31f4326456a1511d32accb096baa63"></a>
### Macro HASHENTRY\_HASH\_MASK

![][public]

```cpp
#define HASHENTRY_HASH_MASK (UINTPTR_MAX^UCHAR_MAX)
```

Bitmask for hash value.





<a id="group__mhashentry__words_1ga54cafd113b5788e58294047e3e3cbe7c"></a>
### Macro WORD\_HASHENTRY\_HASH

![][public]

```cpp
#define WORD_HASHENTRY_HASH ((((uintptr_t *)(word))[0]<<CHAR_BIT)&[HASHENTRY\_HASH\_MASK](col_hash_int_8h.md#group__mhashentry__words_1ga1f31f4326456a1511d32accb096baa63))( word )
```

Get higher order bits of the hash value.

**Parameters**:

* **word**: Word to access.



**See also**: [WORD\_HASHENTRY\_SET\_HASH](col_hash_int_8h.md#group__mhashentry__words_1ga6cba1b5e86476a239bc36a7f2818b1df)



<a id="group__mhashentry__words_1ga6cba1b5e86476a239bc36a7f2818b1df"></a>
### Macro WORD\_HASHENTRY\_SET\_HASH

![][public]

```cpp
#define WORD_HASHENTRY_SET_HASH ((((uintptr_t *)(word))[0])&=~([HASHENTRY\_HASH\_MASK](col_hash_int_8h.md#group__mhashentry__words_1ga1f31f4326456a1511d32accb096baa63)>>CHAR_BIT),(((uintptr_t *)(word))[0])|=(((hash)&[HASHENTRY\_HASH\_MASK](col_hash_int_8h.md#group__mhashentry__words_1ga1f31f4326456a1511d32accb096baa63))>>CHAR_BIT))( word ,hash )
```

Set hash value.

**Parameters**:

* **word**: Word to access.
* **hash**: Hash value. Only retains the high order bits.


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.





**See also**: [WORD\_HASHENTRY\_HASH](col_hash_int_8h.md#group__mhashentry__words_1ga54cafd113b5788e58294047e3e3cbe7c), [WORD\_HASHENTRY\_INIT](col_hash_int_8h.md#group__hashentry__words_1ga85cea176e564f2dafee1d93a21e75666), [WORD\_MHASHENTRY\_INIT](col_hash_int_8h.md#group__mhashentry__words_1ga467a83e27fa5bfd5eca35e6312d5fe5e)



<a id="group__mhashentry__words_1ga9f087b8c13513115c5e1b19c86fbe145"></a>
### Macro WORD\_HASHENTRY\_NEXT

![][public]

```cpp
#define WORD_HASHENTRY_NEXT ((([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(word))[1])( word )
```

Get/set pointer to next entry in bucket.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.






**See also**: [WORD\_HASHENTRY\_INIT](col_hash_int_8h.md#group__hashentry__words_1ga85cea176e564f2dafee1d93a21e75666), [WORD\_MHASHENTRY\_INIT](col_hash_int_8h.md#group__mhashentry__words_1ga467a83e27fa5bfd5eca35e6312d5fe5e), [WORD\_INTHASHENTRY\_INIT](col_hash_int_8h.md#group__inthashentry__words_1ga328cd7bfb2227f0d1dce5d93e8252378), [WORD\_MINTHASHENTRY\_INIT](col_hash_int_8h.md#group__minthashentry__words_1ga0d1fde498a4661bd712715552c7fc675)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)