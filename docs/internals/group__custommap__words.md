<a id="group__custommap__words"></a>
# Custom Maps

Custom maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Maps](group__map__words.md#group__map__words) with applicative code.



## Custom Map Type Descriptors

<a id="group__custommap__words_1ga3fa49bef2d9acca19f61e811fd480117"></a>
### Typedef Col\_MapSizeProc

![][public]

**Definition**: `include/colMap.h` (line 232)

```cpp
typedef size_t() Col_MapSizeProc(Col_Word map)
```

Function signature of custom map size procs.

**Parameters**:

* **map**: Custom map to get size for.


**Returns**:

The custom map size.





**See also**: [Col\_CustomMapType](struct_col___custom_map_type.md#struct_col___custom_map_type), [Col\_CustomIntMapType](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type), [Col\_MapSize](col_map_8h.md#group__map__words_1gaae4f3cbf8a5aedc78929bc2364440aac)



**Return type**: size_t()

<a id="group__custommap__words_1gafe7352ecf3f5606bac94ad65c410435b"></a>
### Typedef Col\_MapGetProc

![][public]

**Definition**: `include/colMap.h` (line 251)

```cpp
typedef int() Col_MapGetProc(Col_Word map, Col_Word key, Col_Word *valuePtr)
```

Function signature of custom map get procs.

**Parameters**:

* **map**: Custom map to get entry for.
* **key**: Entry key. Can be any word type, including string, however it must match the actual type used by the map.
* **valuePtr**: Returned entry value, if found.


**Return values**:

* **zero**: if the key wasn't found.
* **non-zero**: if the key was found, in this case the value is returned through **valuePtr**.




**See also**: [Col\_CustomMapType](struct_col___custom_map_type.md#struct_col___custom_map_type), [Col\_MapGet](col_map_8h.md#group__map__words_1gabd075578f35ec7a706654e94aba281d9)



**Return type**: int()

<a id="group__custommap__words_1gae8b01496e5dec9f2d4c5f2d465d84fce"></a>
### Typedef Col\_IntMapGetProc

![][public]

**Definition**: `include/colMap.h` (line 268)

```cpp
typedef int() Col_IntMapGetProc(Col_Word map, intptr_t key, Col_Word *valuePtr)
```

Function signature of custom integer map get procs.

**Parameters**:

* **map**: Custom integer map to get entry for.
* **key**: Integer entry key.
* **valuePtr**: Returned entry value, if found.


**Return values**:

* **zero**: if the key wasn't found.
* **non-zero**: if the key was found, in this case the value is returned through **valuePtr**.




**See also**: [Col\_CustomIntMapType](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type), [Col\_IntMapGet](col_map_8h.md#group__map__words_1ga4f96f7436cc66537b05841c5b088eef2)



**Return type**: int()

<a id="group__custommap__words_1ga115ec8d5572154663268a86a4ab236be"></a>
### Typedef Col\_MapSetProc

![][public]

**Definition**: `include/colMap.h` (line 284)

```cpp
typedef int() Col_MapSetProc(Col_Word map, Col_Word key, Col_Word value)
```

Function signature of custom map set procs.

**Parameters**:

* **map**: Custom map to insert entry into.
* **key**: Entry key. Can be any word type, including string, however it must match the actual type used by the map.
* **value**: Entry value.


**Return values**:

* **zero**: if an existing entry was updated with **value**.
* **non-zero**: if a new entry was created with **key** and **value**.




**See also**: [Col\_CustomMapType](struct_col___custom_map_type.md#struct_col___custom_map_type), [Col\_MapSet](col_map_8h.md#group__map__words_1ga82b31e62df46ff382e18241bdcde49e3)



**Return type**: int()

<a id="group__custommap__words_1ga133f5965501d18c114b195ba153d29f4"></a>
### Typedef Col\_IntMapSetProc

![][public]

**Definition**: `include/colMap.h` (line 299)

```cpp
typedef int() Col_IntMapSetProc(Col_Word map, intptr_t key, Col_Word value)
```

Function signature of custom integer map set procs.

**Parameters**:

* **map**: Custom integer map to insert entry into.
* **key**: Integer entry key.
* **value**: Entry value.


**Return values**:

* **zero**: if the existing entry was updated with **value**.
* **non-zero**: if a new entry was created with **key** and **value**.




**See also**: [Col\_CustomIntMapType](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type), [Col\_IntMapSet](col_map_8h.md#group__map__words_1ga27a694b6de40e1a3af81379a7056754d)



**Return type**: int()

<a id="group__custommap__words_1ga8be5736ce93508cd5be4aa3e33aafee2"></a>
### Typedef Col\_MapUnsetProc

![][public]

**Definition**: `include/colMap.h` (line 314)

```cpp
typedef int() Col_MapUnsetProc(Col_Word map, Col_Word key)
```

Function signature of custom map unset procs.

**Parameters**:

* **map**: Custom map to remove entry from.
* **key**: Entry key. Can be any word type, including string, however it must match the actual type used by the map.


**Return values**:

* **zero**: if no entry matching **key** was found.
* **non-zero**: if the existing entry was removed.




**See also**: [Col\_CustomMapType](struct_col___custom_map_type.md#struct_col___custom_map_type), [Col\_MapUnset](col_map_8h.md#group__map__words_1ga1f48ed3390f9a53cde268533a763e638)



**Return type**: int()

<a id="group__custommap__words_1gaf2fe195205ae8c8f962be73194cc8e5a"></a>
### Typedef Col\_IntMapUnsetProc

![][public]

**Definition**: `include/colMap.h` (line 328)

```cpp
typedef int() Col_IntMapUnsetProc(Col_Word map, intptr_t key)
```

Function signature of custom integer map unset procs.

**Parameters**:

* **map**: Custom integer map to remove entry from.
* **key**: Integer entry key.


**Return values**:

* **zero**: if no entry matching **key** was found.
* **non-zero**: if the existing entry was removed.




**See also**: [Col\_CustomIntMapType](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type), [Col\_IntMapUnset](col_map_8h.md#group__map__words_1gac2ae366ebe1ec3a4495128cb1400d1cf)



**Return type**: int()

<a id="group__custommap__words_1gab5110c2d8072d001d4b407c017a9c376"></a>
### Typedef Col\_MapIterBeginProc

![][public]

**Definition**: `include/colMap.h` (line 342)

```cpp
typedef int() Col_MapIterBeginProc(Col_Word map, Col_ClientData(*clientData)[2])
```

Function signature of custom map iter begin procs.

**Parameters**:

* **map**: Custom map to begin iteration for.
* **clientData**: Pair of opaque values available for iteration purpose.


**Return values**:

* **zero**: if at end.
* **non-zero**: if iteration began.




**See also**: [Col\_CustomMapType](struct_col___custom_map_type.md#struct_col___custom_map_type), [Col\_MapIterBegin](col_map_8h.md#group__map__words_1gab8f4a0de6b264bbe59c332b41a22866a)



**Return type**: int()

<a id="group__custommap__words_1ga9874161b1f347341158556c38ff8284b"></a>
### Typedef Col\_MapIterFindProc

![][public]

**Definition**: `include/colMap.h` (line 364)

```cpp
typedef int() Col_MapIterFindProc(Col_Word map, Col_Word key, int *createPtr, Col_ClientData(*clientData)[2])
```

Function signature of custom map iter find procs.

**Parameters**:

* **map**: Custom map to find or create entry into.
* **key**: Entry key. Can be any word type, including string, however it must match the actual type used by the map.
* **createPtr**: If non-NULL, whether to create entry if absent on input, and whether an entry was created on output.
* **clientData**: Pair of opaque values available for iteration purpose.


**Return values**:

* **zero**: if at end.
* **non-zero**: if iteration began.




**See also**: [Col\_CustomMapType](struct_col___custom_map_type.md#struct_col___custom_map_type), [Col\_MapIterFind](col_map_8h.md#group__map__words_1gaa925d7d32221bc826e9717930c2602e1)



**Return type**: int()

<a id="group__custommap__words_1gadef056ea16828d598f5702a85988da3f"></a>
### Typedef Col\_IntMapIterFindProc

![][public]

**Definition**: `include/colMap.h` (line 384)

```cpp
typedef int() Col_IntMapIterFindProc(Col_Word map, intptr_t key, int *createPtr, Col_ClientData(*clientData)[2])
```

Function signature of custom integer map iter find procs.

**Parameters**:

* **map**: Custom integer map to find or create entry into.
* **key**: Integer entry key.
* **createPtr**: If non-NULL, whether to create entry if absent on input, and whether an entry was created on output.
* **clientData**: Pair of opaque values available for iteration purpose.


**Return values**:

* **zero**: if at end.
* **non-zero**: if iteration began.




**See also**: [Col\_CustomIntMapType](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type), [Col\_IntMapIterFind](col_map_8h.md#group__map__words_1ga8c332381607b0dc828b4aa96fa8f1a12)



**Return type**: int()

<a id="group__custommap__words_1ga0cb64c06139ba1b7db784b169d46c7d1"></a>
### Typedef Col\_MapIterNextProc

![][public]

**Definition**: `include/colMap.h` (line 399)

```cpp
typedef int() Col_MapIterNextProc(Col_Word map, Col_ClientData(*clientData)[2])
```

Function signature of custom map iter next procs.

**Parameters**:

* **map**: Custom map to continue iteration for.
* **clientData**: Pair of opaque values available for iteration purpose.


**Return values**:

* **zero**: if at end.
* **non-zero**: if iteration continued.




**See also**: [Col\_CustomMapType](struct_col___custom_map_type.md#struct_col___custom_map_type), [Col\_MapIterNext](col_map_8h.md#group__map__words_1ga961449849237659a09dbf4cae436e38c)



**Return type**: int()

<a id="group__custommap__words_1ga76b02a84919d214db7c9cf0d0a704cbb"></a>
### Typedef Col\_MapIterGetKeyProc

![][public]

**Definition**: `include/colMap.h` (line 414)

```cpp
typedef Col_Word() Col_MapIterGetKeyProc(Col_Word map, Col_ClientData(*clientData)[2])
```

Function signature of custom map iter key get procs.

**Parameters**:

* **map**: Custom map to get iterator key from.
* **clientData**: Pair of opaque values available for iteration purpose.


**Returns**:

Entry key.





**See also**: [Col\_CustomMapType](struct_col___custom_map_type.md#struct_col___custom_map_type), [Col\_MapIterGet](col_map_8h.md#group__map__words_1ga1ec7db1472cf7bbc5e48b7972f8f5a77), [Col\_MapIterGetKey](col_map_8h.md#group__map__words_1ga2b3488edf027f8f463920b25c9f3323a)



**Return type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)()

<a id="group__custommap__words_1ga3633e15000b5316b94e153b8c57bfb67"></a>
### Typedef Col\_IntMapIterGetKeyProc

![][public]

**Definition**: `include/colMap.h` (line 429)

```cpp
typedef intptr_t() Col_IntMapIterGetKeyProc(Col_Word map, Col_ClientData(*clientData)[2])
```

Function signature of custom integer map iter key get procs.

**Parameters**:

* **map**: Custom integer map to get iterator key from.
* **clientData**: Pair of opaque values available for iteration purpose.


**Returns**:

Integer entry key.





**See also**: [Col\_CustomIntMapType](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type), [Col\_IntMapIterGet](col_map_8h.md#group__map__words_1ga6ac827db7a48d5bee7e40080fef4e27f), [Col\_IntMapIterGetKey](col_map_8h.md#group__map__words_1ga5ef2beb2ba1f4e90a5b0681df2052417)



**Return type**: intptr_t()

<a id="group__custommap__words_1ga579a529afa2ce53d16e6e9ae6e2dab99"></a>
### Typedef Col\_MapIterGetValueProc

![][public]

**Definition**: `include/colMap.h` (line 443)

```cpp
typedef Col_Word() Col_MapIterGetValueProc(Col_Word map, Col_ClientData(*clientData)[2])
```

Function signature of custom map iter value get procs.

**Parameters**:

* **map**: Custom map to get iterator value from.
* **clientData**: Pair of opaque values available for iteration purpose.


**Returns**:

Entry value.




**See also**: [Col\_CustomMapType](struct_col___custom_map_type.md#struct_col___custom_map_type), [Col\_MapIterGetValue](col_map_8h.md#group__map__words_1gacd8a1021ed18330290de0ae3a1a67fa7)



**Return type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)()

<a id="group__custommap__words_1ga7ea9085242a94376ecd5967bcdca7f11"></a>
### Typedef Col\_MapIterSetValueProc

![][public]

**Definition**: `include/colMap.h` (line 456)

```cpp
typedef void() Col_MapIterSetValueProc(Col_Word map, Col_Word value, Col_ClientData(*clientData)[2])
```

Function signature of custom map iter value set procs.

**Parameters**:

* **map**: Custom map to set iterator value from.
* **value**: Value to set.
* **clientData**: Pair of opaque values available for iteration purpose.




**See also**: [Col\_CustomMapType](struct_col___custom_map_type.md#struct_col___custom_map_type), [Col\_MapIterSetValue](col_map_8h.md#group__map__words_1ga8c5d3a82b6cb5b7af6f16ebed863736f)



**Return type**: void()

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)