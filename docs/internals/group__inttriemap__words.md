<a id="group__inttriemap__words"></a>
# Integer Trie Maps



Integer trie maps are [Trie Maps](group__triemap__words.md#group__triemap__words) with integer keys.







**See also**: [WORD\_TYPE\_INTTRIEMAP](col_word_int_8h.md#group__words_1ga9da4310532cf6307f784bd6f33471218)

## Submodules

* [Integer Trie Nodes](group__minttrienode__words.md#group__minttrienode__words)
* [Immutable Integer Trie Nodes](group__inttrienode__words.md#group__inttrienode__words)
* [Integer Trie Leaves](group__minttrieleaf__words.md#group__minttrieleaf__words)
* [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)

## Integer Trie Map Creation

<a id="group__inttriemap__words_1ga26d6d540935055cb89a59be313bb3dc4"></a>
### Macro WORD\_INTTRIEMAP\_INIT

![][public]

```cpp
#define WORD_INTTRIEMAP_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_INTTRIEMAP](col_word_int_8h.md#group__words_1ga9da4310532cf6307f784bd6f33471218)); \
    [WORD\_SYNONYM](col_word_int_8h.md#group__regular__words_1ga19cfddbcf0127f5088803cc68ddb8eaa)(word) = [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065); \
    [WORD\_TRIEMAP\_SIZE](col_trie_int_8h.md#group__triemap__words_1ga9ccb73a2f8e1e4b2e451833d5cb6903c)(word) = 0; \
    [WORD\_TRIEMAP\_ROOT](col_trie_int_8h.md#group__triemap__words_1ga62ce82c870c8e6905dd22b1df72f08f3)(word) = [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065);( word )
```

Integer trie map word initializer.

Integer trie maps are specialized trie maps using built-in bit test and key comparison procs on integer keys.






**Parameters**:

* **word**: Word to initialize.


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_INTTRIEMAP](col_word_int_8h.md#group__words_1ga9da4310532cf6307f784bd6f33471218)



## Integer Trie Map Exceptions

<a id="group__inttriemap__words_1ga355b10b52e63c90e95300c26d5c8b54d"></a>
### Macro TYPECHECK\_INTTRIEMAP

![][public]

```cpp
#define TYPECHECK_INTTRIEMAP( word )
```

Type checking macro for integer trie maps.

**Parameters**:

* **word**: Checked word.


**Exceptions**:

* **[COL\_ERROR\_INTTRIEMAP](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35ad77018799a3606551c4a3a66f6285b49)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **word**: Not an integer-keyed trie map.



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)