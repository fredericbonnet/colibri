<a id="group__customtriemap__words"></a>
# Custom Trie Maps

Custom trie maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Trie Maps](group__triemap__words.md#group__triemap__words) with applicative code.

**See also**: [Col\_CustomTrieMapType](struct_col___custom_trie_map_type.md#struct_col___custom_trie_map_type)

## Custom Trie Map Type Descriptors

<a id="group__customtriemap__words_1gac7ba97b901a90c7f2532fb233d01a883"></a>
### Typedef Col\_TrieBitTestProc

![][public]

**Definition**: `include/colTrie.h` (line 108)

```cpp
typedef int() Col_TrieBitTestProc(Col_Word map, Col_Word key, size_t index, size_t bit)
```

Function signature of custom trie map's key bit test function.

**Parameters**:

* **map**: Custom trie map the key belongs to.
* **key**: Key to get bit value for.
* **index**: Index of key element.
* **bit**: Position of bit to test.


**Returns**:

The key bit value.



**See also**: [Col\_CustomTrieMapType](struct_col___custom_trie_map_type.md#struct_col___custom_trie_map_type)



**Return type**: int()

<a id="group__customtriemap__words_1ga2277ebb3a3d4bee177de042ad194198d"></a>
### Typedef Col\_TrieKeyDiffProc

![][public]

**Definition**: `include/colTrie.h` (line 128)

```cpp
typedef int() Col_TrieKeyDiffProc(Col_Word map, Col_Word key1, Col_Word key2, size_t *diffPtr, size_t *bitPtr)
```

Function signature of custom trie map's key comparison function.

**Parameters**:

* **map**: Custom trie map the key belongs to.
* **key1 key2**: Keys to compare.
* **diffPtr**: If non-NULL and keys are different, index of first differing key element.
* **bitPtr**: If non-NULL and keys are different, critical bit where key elements differ.


**Return values**:

* **negative**: if **key1** is less than **key2**.
* **positive**: if **key1** is greater than **key2**.
* **zero**: if both keys are equal.



**See also**: [Col\_CustomTrieMapType](struct_col___custom_trie_map_type.md#struct_col___custom_trie_map_type)



**Return type**: int()

## Custom Trie Map Creation

<a id="group__customtriemap__words_1ga18de761037e23e723d8d62aef7d6246c"></a>
### Function Col\_NewCustomTrieMap

![][public]

```cpp
Col_Word Col_NewCustomTrieMap(Col_CustomTrieMapType *type, size_t size, void **dataPtr)
```

Create a new custom trie map word.

**Returns**:

A new custom trie map word of the given size.



**Parameters**:

* [Col\_CustomTrieMapType](struct_col___custom_trie_map_type.md#struct_col___custom_trie_map_type) * **type**: The trie map word type.
* size_t **size**: Size of custom data.
* void ** **dataPtr**: [out] Points to the allocated custom data.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)
* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [COL\_TRIEMAP](col_word_8h.md#group__words_1ga7922babbc856f5670805da2267d72ff0)
* [CUSTOMTRIEMAP\_HEADER\_SIZE](col_trie_int_8h.md#group__customtriemap__words_1gab1b1757562f39ce72387cd26b4ae8f2a)
* [Col\_CustomTrieMapType::type](struct_col___custom_trie_map_type.md#struct_col___custom_trie_map_type_1a5840c121ff181e25d59fb5ebccc5a03a)
* [Col\_CustomWordType::type](struct_col___custom_word_type.md#struct_col___custom_word_type_1af9482efe5a6408bc622320619c3ccf9f)
* [WORD\_CUSTOM\_SIZE](col_word_int_8h.md#group__custom__words_1ga2610704afbab6a5ec38e561f17dde6ea)
* [WORD\_TRIEMAP\_INIT](col_trie_int_8h.md#group__customtriemap__words_1ga83f9a9931b75d0b6d16e739195d17536)

**Referenced by**:

* [Col\_NewCustomWord](col_word_8h.md#group__custom__words_1gaf9a6d324967159ae7abeb41a3d59cc79)

## Custom Trie Map Constants

<a id="group__customtriemap__words_1gab1b1757562f39ce72387cd26b4ae8f2a"></a>
### Macro CUSTOMTRIEMAP\_HEADER\_SIZE

![][public]

```cpp
#define CUSTOMTRIEMAP_HEADER_SIZE [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)
```

Byte size of custom trie map word header.





## Custom Trie Map Creation

<a id="group__customtriemap__words_1ga83f9a9931b75d0b6d16e739195d17536"></a>
### Macro WORD\_TRIEMAP\_INIT

![][public]

```cpp
#define WORD_TRIEMAP_INIT     [WORD\_SET\_TYPEINFO](col_word_int_8h.md#group__custom__words_1gad327193f2befdd20bebdd2ee74023310)((word), (typeInfo)); \
    [WORD\_SYNONYM](col_word_int_8h.md#group__regular__words_1ga19cfddbcf0127f5088803cc68ddb8eaa)(word) = [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065); \
    [WORD\_TRIEMAP\_SIZE](col_trie_int_8h.md#group__triemap__words_1ga9ccb73a2f8e1e4b2e451833d5cb6903c)(word) = 0; \
    [WORD\_TRIEMAP\_ROOT](col_trie_int_8h.md#group__triemap__words_1ga62ce82c870c8e6905dd22b1df72f08f3)(word) = [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065);( word ,typeInfo )
```

Custom trie map word initializer.

Custom trie maps are custom word types of type [COL\_TRIEMAP](col_word_8h.md#group__words_1ga7922babbc856f5670805da2267d72ff0).






**Parameters**:

* **word**: Word to initialize.
* **typeInfo**: [WORD\_SET\_TYPEINFO](col_word_int_8h.md#group__custom__words_1gad327193f2befdd20bebdd2ee74023310).



**See also**: [Col\_CustomTrieMapType](struct_col___custom_trie_map_type.md#struct_col___custom_trie_map_type)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)