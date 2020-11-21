<a id="struct_col___custom_word_type"></a>
# Structure Col\_CustomWordType

![][C++]
![][public]

**Definition**: `include/colWord.h` (line 244)

Custom word type descriptor.

**See also**: [Word Types](#group__words_1word_types)

## Members

* [childrenProc](struct_col___custom_word_type.md#struct_col___custom_word_type_1a5ea9bcfcae2ebadc971e394ba3c2be0f)
* [freeProc](struct_col___custom_word_type.md#struct_col___custom_word_type_1a15e8e2dd2cb2eedf153d89925a359712)
* [name](struct_col___custom_word_type.md#struct_col___custom_word_type_1a6eb4ecb778b2078bf6fd9f522fcdcae6)
* [sizeProc](struct_col___custom_word_type.md#struct_col___custom_word_type_1a19986fa33336cd268de17eba5412605c)
* [type](struct_col___custom_word_type.md#struct_col___custom_word_type_1af9482efe5a6408bc622320619c3ccf9f)

## Public attributes

<a id="struct_col___custom_word_type_1af9482efe5a6408bc622320619c3ccf9f"></a>
### Variable type

![][public]

**Definition**: `include/colWord.h` (line 246)

```cpp
int Col_CustomWordType::type
```

Type identifier.





**Type**: int

**Referenced by**:

* [GetNbCells](col_gc_8c.md#group__gc_1ga0c15f058185926a533dc21cea927e384)
* [MarkWord](col_gc_8c.md#group__gc_1gaf54093bb37e6a4aaaf718fb1a791d56c)
* [RememberSweepable](col_gc_8c.md#group__gc_1ga23c84fc6b7da85d87751fa5788e3f002)
* [SweepUnreachableCells](col_gc_8c.md#group__gc_1ga911d254f51c4e0b4475330147fbbc545)
* [CleanupSweepables](col_gc_8c.md#group__gc_1ga6a8b4c25c3dbf786bfe56975efba86e0)
* [Col\_HashMapGet](col_hash_8h.md#group__hashmap__words_1ga7fe5b8f4de905e324ada5177527d483e)
* [Col\_HashMapSet](col_hash_8h.md#group__hashmap__words_1ga5290a8ca2aeccdb481e46ca161dbafdf)
* [Col\_HashMapUnset](col_hash_8h.md#group__hashmap__words_1ga4319b874a1524fcd008125db503a7f9c)
* [Col\_HashMapIterFind](col_hash_8h.md#group__hashmap__words_1ga7f4cdf033cec55efd5d6c7704176dfc5)
* [Col\_NewCustomHashMap](col_hash_8h.md#group__customhashmap__words_1gad516fa9041eb514e2c5193eb5d958f0e)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [GetChunk](col_list_8c.md#group__list__words_1ga58a993e823bf134e615c862a546d6d87)
* [ColListIterUpdateTraversalInfo](col_list_8h.md#group__list__words_1ga65a0551576955013edca89745886b5dd)
* [Col\_MapGet](col_map_8h.md#group__map__words_1gabd075578f35ec7a706654e94aba281d9)
* [Col\_MapSet](col_map_8h.md#group__map__words_1ga82b31e62df46ff382e18241bdcde49e3)
* [Col\_MapUnset](col_map_8h.md#group__map__words_1ga1f48ed3390f9a53cde268533a763e638)
* [Col\_Subrope](col_rope_8h.md#group__rope__words_1ga688a99f26c500c1f65f4141e97de0335)
* [Col\_ConcatRopes](col_rope_8h.md#group__rope__words_1gaafab3ef159c0b11402cc50c91fc59700)
* [ColRopeIterUpdateTraversalInfo](col_rope_8h.md#group__rope__words_1ga9c738d41b3d8577f7cdd0fa1880f9143)
* [Col\_NewCustomTrieMap](col_trie_8h.md#group__customtriemap__words_1ga18de761037e23e723d8d62aef7d6246c)
* [Col\_NewCustomWord](col_word_8h.md#group__custom__words_1gaf9a6d324967159ae7abeb41a3d59cc79)
* [Col\_CustomWordInfo](col_word_8h.md#group__custom__words_1gaaa55265553c564df20fc33dfe9153527)

<a id="struct_col___custom_word_type_1a6eb4ecb778b2078bf6fd9f522fcdcae6"></a>
### Variable name

![][public]

**Definition**: `include/colWord.h` (line 249)

```cpp
const char* Col_CustomWordType::name
```

Name of the type, e.g. "int".





**Type**: const char *

<a id="struct_col___custom_word_type_1a19986fa33336cd268de17eba5412605c"></a>
### Variable sizeProc

![][public]

**Definition**: `include/colWord.h` (line 253)

```cpp
Col_CustomWordSizeProc* Col_CustomWordType::sizeProc
```

Called to get the size in bytes of the word.

Must be constant during the whole lifetime.



**Type**: [Col\_CustomWordSizeProc](col_word_8h.md#group__custom__words_1ga6f6dd3eac5e1ca1e3fba5878dd698dff) *

**Referenced by**:

* [GetNbCells](col_gc_8c.md#group__gc_1ga0c15f058185926a533dc21cea927e384)

<a id="struct_col___custom_word_type_1a15e8e2dd2cb2eedf153d89925a359712"></a>
### Variable freeProc

![][public]

**Definition**: `include/colWord.h` (line 256)

```cpp
Col_CustomWordFreeProc* Col_CustomWordType::freeProc
```

Called once the word gets collected.

NULL if not needed.



**Type**: [Col\_CustomWordFreeProc](col_word_8h.md#group__custom__words_1ga73db405afd10df91bf70e5507fd63584) *

**Referenced by**:

* [RememberSweepable](col_gc_8c.md#group__gc_1ga23c84fc6b7da85d87751fa5788e3f002)
* [SweepUnreachableCells](col_gc_8c.md#group__gc_1ga911d254f51c4e0b4475330147fbbc545)
* [CleanupSweepables](col_gc_8c.md#group__gc_1ga6a8b4c25c3dbf786bfe56975efba86e0)
* [Col\_NewCustomWord](col_word_8h.md#group__custom__words_1gaf9a6d324967159ae7abeb41a3d59cc79)

<a id="struct_col___custom_word_type_1a5ea9bcfcae2ebadc971e394ba3c2be0f"></a>
### Variable childrenProc

![][public]

**Definition**: `include/colWord.h` (line 260)

```cpp
Col_CustomWordChildrenProc* Col_CustomWordType::childrenProc
```

Called during the garbage collection to iterate over the words owned by the word, in no special order.

If NULL, do nothing.



**Type**: [Col\_CustomWordChildrenProc](col_word_8h.md#group__custom__words_1ga8806c7822f400375ca69bb879aece079) *

**Referenced by**:

* [MarkWord](col_gc_8c.md#group__gc_1gaf54093bb37e6a4aaaf718fb1a791d56c)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)