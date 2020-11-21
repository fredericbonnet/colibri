<a id="group__strtrienode__words"></a>
# Immutable String Trie Nodes



String trie nodes are [Immutable Trie Nodes](group__trienode__words.md#group__trienode__words) nodes with string-specific data.





They are former [String Trie Nodes](group__mstrtrienode__words.md#group__mstrtrienode__words) that became immutable as a result of a copy (see [Col\_CopyTrieMap](col_trie_8h.md#group__triemap__words_1ga34e494c0bafde72774a578643bb84a68)): in this case the copy and the original would share most of their internals (rendered immutable in the process). Likewise, mutable operations on the copies would use copy-on-write semantics: immutable structures are copied to new mutable ones before modification.








**See also**: [Immutable String Trie Nodes](group__strtrienode__words.md#group__strtrienode__words), [String Trie Maps](group__strtriemap__words.md#group__strtriemap__words)

## Immutable String Trie Node Creation

<a id="group__strtrienode__words_1gaa99e32f6881a4f95c95f59d5cc1586db"></a>
### Macro WORD\_STRTRIENODE\_INIT

![][public]

```cpp
#define WORD_STRTRIENODE_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_STRTRIENODE](col_word_int_8h.md#group__words_1ga9ff5e124f29796ce3b1dd377f7d6d59f)); \
    [WORD\_STRTRIENODE\_SET\_MASK](col_trie_int_8h.md#group__mstrtrienode__words_1ga8a91d66561940fb9ee33506dec11f4b8)(word, mask); \
    [WORD\_STRTRIENODE\_DIFF](col_trie_int_8h.md#group__mstrtrienode__words_1ga0b7f928792baf650225912ff4f126815)(word) = (diff); \
    [WORD\_TRIENODE\_LEFT](col_trie_int_8h.md#group__mtrienode__words_1ga96a098f14e33a0759a9962e567084a0a)(word) = (left); \
    [WORD\_TRIENODE\_RIGHT](col_trie_int_8h.md#group__mtrienode__words_1gaf58f69b75b276cb0b1dbdc54e94e31ad)(word) = (right);( word ,diff ,mask ,left ,right )
```

Immutable string trie node word initializer.

**Parameters**:

* **word**: Word to initialize.
* **diff**: [WORD\_STRTRIENODE\_DIFF](col_trie_int_8h.md#group__mstrtrienode__words_1ga0b7f928792baf650225912ff4f126815).
* **mask**: [WORD\_STRTRIENODE\_SET\_MASK](col_trie_int_8h.md#group__mstrtrienode__words_1ga8a91d66561940fb9ee33506dec11f4b8).
* **left**: [WORD\_TRIENODE\_LEFT](col_trie_int_8h.md#group__mtrienode__words_1ga96a098f14e33a0759a9962e567084a0a).
* **right**: [WORD\_TRIENODE\_RIGHT](col_trie_int_8h.md#group__mtrienode__words_1gaf58f69b75b276cb0b1dbdc54e94e31ad).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_STRTRIENODE](col_word_int_8h.md#group__words_1ga9ff5e124f29796ce3b1dd377f7d6d59f)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)