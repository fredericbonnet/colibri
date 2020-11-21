<a id="group__trieleaf__words"></a>
# Immutable Trie Leaves



Immutable trie leaves are [Map Entries](group__mapentry__words.md#group__mapentry__words) with trie-specific data.





They are former [Trie Leaves](group__mtrieleaf__words.md#group__mtrieleaf__words) that became immutable as a result of a copy (see [Col\_CopyTrieMap](col_trie_8h.md#group__triemap__words_1ga34e494c0bafde72774a578643bb84a68)): in this case the copy and the original would share most of their internals (rendered immutable in the process). Likewise, mutable operations on the copies would use copy-on-write semantics: immutable structures are copied to new mutable ones before modification.









**See also**: [Map Entries](group__mapentry__words.md#group__mapentry__words), [Trie Leaves](group__mtrieleaf__words.md#group__mtrieleaf__words), [Trie Maps](group__triemap__words.md#group__triemap__words)

## Immutable Trie Leaf Creation

<a id="group__trieleaf__words_1ga2e55cc85a82dae0bc17423d5b12d00d8"></a>
### Macro WORD\_TRIELEAF\_INIT

![][public]

```cpp
#define WORD_TRIELEAF_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_TRIELEAF](col_word_int_8h.md#group__words_1ga2a17ea1e39ad925fc0057cd928cdd49c)); \
    [WORD\_MAPENTRY\_KEY](col_map_int_8h.md#group__mapentry__words_1ga8664d15fae4553b47b658ac7ceb1443a)(word) = (key); \
    [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064)(word) = (value);( word ,key ,value )
```

Immutable trie leaf word initializer.

**Parameters**:

* **word**: Word to initialize.
* **key**: [WORD\_MAPENTRY\_KEY](col_map_int_8h.md#group__mapentry__words_1ga8664d15fae4553b47b658ac7ceb1443a).
* **value**: [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_TRIELEAF](col_word_int_8h.md#group__words_1ga2a17ea1e39ad925fc0057cd928cdd49c)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)