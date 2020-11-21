<a id="group__inttrienode__words"></a>
# Immutable Integer Trie Nodes



Integer trie nodes are [Immutable Trie Nodes](group__trienode__words.md#group__trienode__words) nodes with integer-specific data.





They are former [Integer Trie Nodes](group__minttrienode__words.md#group__minttrienode__words) that became immutable as a result of a copy (see [Col\_CopyTrieMap](col_trie_8h.md#group__triemap__words_1ga34e494c0bafde72774a578643bb84a68)): in this case the copy and the original would share most of their internals (rendered immutable in the process). Likewise, mutable operations on the copies would use copy-on-write semantics: immutable structures are copied to new mutable ones before modification.








**See also**: [Integer Trie Nodes](group__minttrienode__words.md#group__minttrienode__words), [Integer Trie Maps](group__inttriemap__words.md#group__inttriemap__words)

## Immutable Integer Trie Node Creation

<a id="group__inttrienode__words_1ga778e592ea5df7ecfdf2c16954c5e5a59"></a>
### Macro WORD\_INTTRIENODE\_INIT

![][public]

```cpp
#define WORD_INTTRIENODE_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_INTTRIENODE](col_word_int_8h.md#group__words_1gac4adeeed11aa6a07235dfe4099c18074)); \
    [WORD\_INTTRIENODE\_MASK](col_trie_int_8h.md#group__minttrienode__words_1ga010b15b770a0a79b1be365173a693ea3)(word) = (mask); \
    [WORD\_TRIENODE\_LEFT](col_trie_int_8h.md#group__mtrienode__words_1ga96a098f14e33a0759a9962e567084a0a)(word) = (left); \
    [WORD\_TRIENODE\_RIGHT](col_trie_int_8h.md#group__mtrienode__words_1gaf58f69b75b276cb0b1dbdc54e94e31ad)(word) = (right);( word ,mask ,left ,right )
```

Mutable integer trie node word initializer.

**Parameters**:

* **word**: Word to initialize.
* **mask**: [WORD\_INTTRIENODE\_MASK](col_trie_int_8h.md#group__minttrienode__words_1ga010b15b770a0a79b1be365173a693ea3).
* **left**: [WORD\_TRIENODE\_LEFT](col_trie_int_8h.md#group__mtrienode__words_1ga96a098f14e33a0759a9962e567084a0a).
* **right**: [WORD\_TRIENODE\_RIGHT](col_trie_int_8h.md#group__mtrienode__words_1gaf58f69b75b276cb0b1dbdc54e94e31ad).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_INTTRIENODE](col_word_int_8h.md#group__words_1gac4adeeed11aa6a07235dfe4099c18074)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)