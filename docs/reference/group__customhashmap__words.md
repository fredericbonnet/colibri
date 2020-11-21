<a id="group__customhashmap__words"></a>
# Custom Hash Maps

Custom hash maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Hash Maps](group__hashmap__words.md#group__hashmap__words) with applicative code.

**See also**: [Col\_CustomHashMapType](struct_col___custom_hash_map_type.md#struct_col___custom_hash_map_type)

## Custom Hash Map Type Descriptors

<a id="group__customhashmap__words_1gac6b7003867d2534bcc1848e410c05458"></a>
### Typedef Col\_HashProc

![][public]

**Definition**: `include/colHash.h` (line 105)

```cpp
typedef uintptr_t() Col_HashProc(Col_Word map, Col_Word key)
```

Function signature of custom hash map key hashing function.

**Parameters**:

* **map**: Custom hash map the key belongs to.
* **key**: Key to generate hash value for.


**Returns**:

The key hash value.



**See also**: [Col\_CustomHashMapType](struct_col___custom_hash_map_type.md#struct_col___custom_hash_map_type)



**Return type**: uintptr_t()

<a id="group__customhashmap__words_1gae157152eec296221c971af03b35d39fa"></a>
### Typedef Col\_HashCompareKeysProc

![][public]

**Definition**: `include/colHash.h` (line 119)

```cpp
typedef int() Col_HashCompareKeysProc(Col_Word map, Col_Word key1, Col_Word key2)
```

Function signature of custom hash map key comparison function.

**Parameters**:

* **map**: Custom hash map the keys belong to.
* **key1 key2**: Keys to compare.


**Return values**:

* **negative**: if **key1** is less than **key2**.
* **positive**: if **key1** is greater than **key2**.
* **zero**: if both keys are equal.



**See also**: [Col\_CustomHashMapType](struct_col___custom_hash_map_type.md#struct_col___custom_hash_map_type)



**Return type**: int()

## Custom Hash Map Creation

<a id="group__customhashmap__words_1gad516fa9041eb514e2c5193eb5d958f0e"></a>
### Function Col\_NewCustomHashMap

![][public]

```cpp
Col_Word Col_NewCustomHashMap(Col_CustomHashMapType *type, size_t capacity, size_t size, void **dataPtr)
```

Create a new custom hash map word.

**Returns**:

A new custom hash map word of the given size and capacity.



**Parameters**:

* [Col\_CustomHashMapType](struct_col___custom_hash_map_type.md#struct_col___custom_hash_map_type) * **type**: The hash map word type.
* size_t **capacity**: Initial bucket size. Rounded up to the next power of 2.
* size_t **size**: Size of custom data.
* void ** **dataPtr**: [out] Points to the allocated custom data.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[private]: https://img.shields.io/badge/-private-red (private)
[static]: https://img.shields.io/badge/-static-lightgrey (static)