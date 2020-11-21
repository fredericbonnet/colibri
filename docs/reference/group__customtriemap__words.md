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

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[private]: https://img.shields.io/badge/-private-red (private)
[static]: https://img.shields.io/badge/-static-lightgrey (static)