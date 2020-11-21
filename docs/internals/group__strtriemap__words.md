<a id="group__strtriemap__words"></a>
# String Trie Maps



String trie maps are [Trie Maps](group__triemap__words.md#group__triemap__words) with string keys.







**See also**: [WORD\_TYPE\_STRTRIEMAP](col_word_int_8h.md#group__words_1gae4ef7e39bd92ee96414ee98c844065ec)

## Submodules

* [String Trie Nodes](group__mstrtrienode__words.md#group__mstrtrienode__words)
* [Immutable String Trie Nodes](group__strtrienode__words.md#group__strtrienode__words)

## String Trie Map Creation

<a id="group__strtriemap__words_1ga973ba8a0c6db8328aedcd73fd303a10d"></a>
### Macro WORD\_STRTRIEMAP\_INIT

![][public]

```cpp
#define WORD_STRTRIEMAP_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_STRTRIEMAP](col_word_int_8h.md#group__words_1gae4ef7e39bd92ee96414ee98c844065ec)); \
    [WORD\_SYNONYM](col_word_int_8h.md#group__regular__words_1ga19cfddbcf0127f5088803cc68ddb8eaa)(word) = [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065); \
    [WORD\_TRIEMAP\_SIZE](col_trie_int_8h.md#group__triemap__words_1ga9ccb73a2f8e1e4b2e451833d5cb6903c)(word) = 0; \
    [WORD\_TRIEMAP\_ROOT](col_trie_int_8h.md#group__triemap__words_1ga62ce82c870c8e6905dd22b1df72f08f3)(word) = [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065);( word )
```

String trie map word initializer.

String trie maps are specialized trie maps using built-in bit test and key comparison procs on string keys.






**Parameters**:

* **word**: Word to initialize.


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_STRTRIEMAP](col_word_int_8h.md#group__words_1gae4ef7e39bd92ee96414ee98c844065ec)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)