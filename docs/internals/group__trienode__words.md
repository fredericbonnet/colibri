<a id="group__trienode__words"></a>
# Immutable Trie Nodes



Immutable trie nodes are former [Trie Nodes](group__mtrienode__words.md#group__mtrienode__words) that became immutable as a result of a copy (see [Col\_CopyTrieMap](col_trie_8h.md#group__triemap__words_1ga34e494c0bafde72774a578643bb84a68)): in this case the copy and the original would share most of their internals (rendered immutable in the process). Likewise, mutable operations on the copies would use copy-on-write semantics: immutable structures are copied to new mutable ones before modification.








**See also**: [Immutable Trie Nodes](group__trienode__words.md#group__trienode__words), [Trie Maps](group__triemap__words.md#group__triemap__words)

## Immutable Trie Node Creation

<a id="group__trienode__words_1gaa2668e0b8ee0e48846bd5816f8b817d6"></a>
### Macro WORD\_TRIENODE\_INIT

![][public]

```cpp
#define WORD_TRIENODE_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_TRIENODE](col_word_int_8h.md#group__words_1gae52162432efdceb0ac49e6332b213401)); \
    [WORD\_TRIENODE\_SET\_BIT](col_trie_int_8h.md#group__mtrienode__words_1ga4b49795cbbe35d01e5dd00f97eb98004)((word), (bit)); \
    [WORD\_TRIENODE\_DIFF](col_trie_int_8h.md#group__mtrienode__words_1ga4e2885eb03b3f4575cb80e7e84ed1b7b)(word) = (diff); \
    [WORD\_TRIENODE\_LEFT](col_trie_int_8h.md#group__mtrienode__words_1ga96a098f14e33a0759a9962e567084a0a)(word) = (left); \
    [WORD\_TRIENODE\_RIGHT](col_trie_int_8h.md#group__mtrienode__words_1gaf58f69b75b276cb0b1dbdc54e94e31ad)(word) = (right);( word ,diff ,bit ,left ,right )
```

Immutable trie node initializer.

**Parameters**:

* **word**: Word to initialize.
* **diff**: [WORD\_TRIENODE\_DIFF](col_trie_int_8h.md#group__mtrienode__words_1ga4e2885eb03b3f4575cb80e7e84ed1b7b).
* **bit**: [WORD\_TRIENODE\_SET\_BIT](col_trie_int_8h.md#group__mtrienode__words_1ga4b49795cbbe35d01e5dd00f97eb98004).
* **left**: [WORD\_TRIENODE\_LEFT](col_trie_int_8h.md#group__mtrienode__words_1ga96a098f14e33a0759a9962e567084a0a).
* **right**: [WORD\_TRIENODE\_RIGHT](col_trie_int_8h.md#group__mtrienode__words_1gaf58f69b75b276cb0b1dbdc54e94e31ad).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_TRIENODE](col_word_int_8h.md#group__words_1gae52162432efdceb0ac49e6332b213401)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)