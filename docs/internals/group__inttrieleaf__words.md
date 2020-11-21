<a id="group__inttrieleaf__words"></a>
# Immutable Integer Trie Leaves



Immutable integer trie leaves are [Integer Map Entries](group__intmapentry__words.md#group__intmapentry__words) with trie-specific data.





They are former [Integer Trie Leaves](group__minttrieleaf__words.md#group__minttrieleaf__words) that became immutable as a result of a copy (see [Col\_CopyTrieMap](col_trie_8h.md#group__triemap__words_1ga34e494c0bafde72774a578643bb84a68)): in this case the copy and the original would share most of their internals (rendered immutable in the process). Likewise, mutable operations on the copies would use copy-on-write semantics: immutable structures are copied to new mutable ones before modification.









**See also**: [Integer Map Entries](group__intmapentry__words.md#group__intmapentry__words), [Integer Trie Leaves](group__minttrieleaf__words.md#group__minttrieleaf__words), [Integer Trie Maps](group__inttriemap__words.md#group__inttriemap__words)

## Immutable Integer Trie Leaf Creation

<a id="group__inttrieleaf__words_1ga2d95f188d06bdcff5ccc89cef8674204"></a>
### Macro WORD\_INTTRIELEAF\_INIT

![][public]

```cpp
#define WORD_INTTRIELEAF_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_INTTRIELEAF](col_word_int_8h.md#group__words_1ga896310a96176f87d4ec0bd06eabf55f7)); \
    [WORD\_INTMAPENTRY\_KEY](col_map_int_8h.md#group__intmapentry__words_1ga89e26360d76aaad985afd89da56d1539)(word) = (key); \
    [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064)(word) = (value);( word ,key ,value )
```

Immutable integer trie leaf word initializer.

**Parameters**:

* **word**: Word to initialize.
* **key**: [WORD\_INTMAPENTRY\_KEY](col_map_int_8h.md#group__intmapentry__words_1ga89e26360d76aaad985afd89da56d1539).
* **value**: [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_INTTRIELEAF](col_word_int_8h.md#group__words_1ga896310a96176f87d4ec0bd06eabf55f7)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)