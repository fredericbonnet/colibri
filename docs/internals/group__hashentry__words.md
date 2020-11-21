<a id="group__hashentry__words"></a>
# Immutable Hash Entries



Immutable hash entries are [Map Entries](group__mapentry__words.md#group__mapentry__words) with hash-specific data.





They are former [Hash Entries](group__mhashentry__words.md#group__mhashentry__words) that became immutable as a result of a copy (see [Col\_CopyHashMap](col_hash_8h.md#group__hashmap__words_1ga9ebda3b577662e8b1dcf9a227d106f22)): in this case the copy and the original would share most of their internals (rendered immutable in the process). Likewise, mutable operations on the copies would use copy-on-write semantics: immutable structures are copied to new mutable ones before modification.









**See also**: [Map Entries](group__mapentry__words.md#group__mapentry__words), [Hash Entries](group__mhashentry__words.md#group__mhashentry__words), [Hash Maps](group__hashmap__words.md#group__hashmap__words)

## Immutable Hash Entry Creation

<a id="group__hashentry__words_1ga85cea176e564f2dafee1d93a21e75666"></a>
### Macro WORD\_HASHENTRY\_INIT

![][public]

```cpp
#define WORD_HASHENTRY_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_HASHENTRY](col_word_int_8h.md#group__words_1ga0ccfe6bc407371b3c2cde0a2da83f9fa)); \
    [WORD\_MAPENTRY\_KEY](col_map_int_8h.md#group__mapentry__words_1ga8664d15fae4553b47b658ac7ceb1443a)(word) = (key); \
    [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064)(word) = (value); \
    [WORD\_HASHENTRY\_NEXT](col_hash_int_8h.md#group__mhashentry__words_1ga9f087b8c13513115c5e1b19c86fbe145)(word) = (next); \
    [WORD\_HASHENTRY\_SET\_HASH](col_hash_int_8h.md#group__mhashentry__words_1ga6cba1b5e86476a239bc36a7f2818b1df)(word, hash);( word ,key ,value ,next ,hash )
```

Immutable hash entry word initializer.

**Parameters**:

* **word**: Word to initialize.
* **key**: [WORD\_MAPENTRY\_KEY](col_map_int_8h.md#group__mapentry__words_1ga8664d15fae4553b47b658ac7ceb1443a).
* **value**: [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064).
* **next**: [WORD\_HASHENTRY\_NEXT](col_hash_int_8h.md#group__mhashentry__words_1ga9f087b8c13513115c5e1b19c86fbe145).
* **hash**: [WORD\_HASHENTRY\_SET\_HASH](col_hash_int_8h.md#group__mhashentry__words_1ga6cba1b5e86476a239bc36a7f2818b1df).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_HASHENTRY](col_word_int_8h.md#group__words_1ga0ccfe6bc407371b3c2cde0a2da83f9fa)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)