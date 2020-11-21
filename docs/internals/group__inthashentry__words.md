<a id="group__inthashentry__words"></a>
# Immutable Integer Hash Entries



Immutable integer hash entries are [Map Entries](group__mapentry__words.md#group__mapentry__words) with hash-specific data.





They are former [Integer Hash Entries](group__minthashentry__words.md#group__minthashentry__words) that became immutable as a result of a copy (see [Col\_CopyHashMap](col_hash_8h.md#group__hashmap__words_1ga9ebda3b577662e8b1dcf9a227d106f22)): in this case the copy and the original would share most of their internals (rendered immutable in the process). Likewise, mutable operations on the copies would use copy-on-write semantics: immutable structures are copied to new mutable ones before modification.









**See also**: [Map Entries](group__mapentry__words.md#group__mapentry__words), [Integer Hash Entries](group__minthashentry__words.md#group__minthashentry__words), [Hash Maps](group__hashmap__words.md#group__hashmap__words)

## Immutable Integer Hash Entry Creation

<a id="group__inthashentry__words_1ga328cd7bfb2227f0d1dce5d93e8252378"></a>
### Macro WORD\_INTHASHENTRY\_INIT

![][public]

```cpp
#define WORD_INTHASHENTRY_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_INTHASHENTRY](col_word_int_8h.md#group__words_1gab1a5b3b65a05c74cd3973db9dce4a781)); \
    [WORD\_INTMAPENTRY\_KEY](col_map_int_8h.md#group__intmapentry__words_1ga89e26360d76aaad985afd89da56d1539)(word) = (key); \
    [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064)(word) = (value); \
    [WORD\_HASHENTRY\_NEXT](col_hash_int_8h.md#group__mhashentry__words_1ga9f087b8c13513115c5e1b19c86fbe145)(word) = (next);( word ,key ,value ,next )
```

Immutable integer hash entry word initializer.

**Parameters**:

* **word**: Word to initialize.
* **key**: [WORD\_INTMAPENTRY\_KEY](col_map_int_8h.md#group__intmapentry__words_1ga89e26360d76aaad985afd89da56d1539).
* **value**: [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064).
* **next**: [WORD\_HASHENTRY\_NEXT](col_hash_int_8h.md#group__mhashentry__words_1ga9f087b8c13513115c5e1b19c86fbe145).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_INTHASHENTRY](col_word_int_8h.md#group__words_1gab1a5b3b65a05c74cd3973db9dce4a781)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)