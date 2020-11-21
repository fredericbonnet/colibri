<a id="group__inthashmap__words"></a>
# Integer Hash Maps



Integer hash maps are [Hash Maps](group__hashmap__words.md#group__hashmap__words) with integer keys.







**See also**: [WORD\_TYPE\_INTHASHMAP](col_word_int_8h.md#group__words_1ga230c3d50685afa970c1e0da69feb5811)

## Submodules

* [Integer Hash Entries](group__minthashentry__words.md#group__minthashentry__words)
* [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)

## Integer Hash Map Creation

<a id="group__inthashmap__words_1ga53503676dc5f75eb7894087cf53d8252"></a>
### Macro WORD\_INTHASHMAP\_INIT

![][public]

```cpp
#define WORD_INTHASHMAP_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_INTHASHMAP](col_word_int_8h.md#group__words_1ga230c3d50685afa970c1e0da69feb5811)); \
    [WORD\_SYNONYM](col_word_int_8h.md#group__regular__words_1ga19cfddbcf0127f5088803cc68ddb8eaa)(word) = [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065); \
    [WORD\_HASHMAP\_SIZE](col_hash_int_8h.md#group__hashmap__words_1gac639d1878d96d8bb8d825822bc104b8c)(word) = 0; \
    [WORD\_HASHMAP\_BUCKETS](col_hash_int_8h.md#group__hashmap__words_1gaa3913885cb3625fe8ba8582eb3323315)(word) = [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065);( word )
```

Integer hash map word initializer.

Integer hash maps are specialized hash maps where the hash value is the randomized integer key.






**Parameters**:

* **word**: Word to initialize.


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_INTHASHMAP](col_word_int_8h.md#group__words_1ga230c3d50685afa970c1e0da69feb5811)



## Integer Hash Map Exceptions

<a id="group__inthashmap__words_1gab451eefab69078045d4b5e92c07350f1"></a>
### Macro TYPECHECK\_INTHASHMAP

![][public]

```cpp
#define TYPECHECK_INTHASHMAP( word )
```

Type checking macro for integer hash maps.

**Parameters**:

* **word**: Checked word.


**Exceptions**:

* **[COL\_ERROR\_INTHASHMAP](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a6adf7197e12916d0bdb78d890b43cf3d)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **word**: Not an integer-keyed hash map.



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)