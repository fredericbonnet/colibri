<a id="struct_compare_chunks_info"></a>
# Structure CompareChunksInfo

![][C++]
![][private]

**Definition**: `src/colRope.c` (line 1809)

Structure used to collect data during rope comparison.

**See also**: [CompareChunksProc](col_rope_8c.md#group__rope__words_1gaa0f078c09e57fc1f7ba7cdf5e80c7f9d), [Col\_CompareRopesL](col_rope_8h.md#group__rope__words_1ga3202ac5414ac22cc7627e0edefbba21c)

## Members

* [c1Ptr](struct_compare_chunks_info.md#struct_compare_chunks_info_1aa4bb2fd92a0fae519ca603730f11eb0f)
* [c2Ptr](struct_compare_chunks_info.md#struct_compare_chunks_info_1a0d2ea5f98cf8a8f19a8e0a3ec8e03a36)
* [posPtr](struct_compare_chunks_info.md#struct_compare_chunks_info_1a51bba1b3643bf5d59cabc426c249d8b8)

## Public attributes

<a id="struct_compare_chunks_info_1a51bba1b3643bf5d59cabc426c249d8b8"></a>
### Variable posPtr

![][public]

**Definition**: `src/colRope.c` (line 1810)

```cpp
size_t* CompareChunksInfo::posPtr
```

If non-NULL, position of the first differing character.





**Type**: size_t *

**Referenced by**:

* [CompareChunksProc](col_rope_8c.md#group__rope__words_1gaa0f078c09e57fc1f7ba7cdf5e80c7f9d)
* [Col\_CompareRopesL](col_rope_8h.md#group__rope__words_1ga3202ac5414ac22cc7627e0edefbba21c)

<a id="struct_compare_chunks_info_1aa4bb2fd92a0fae519ca603730f11eb0f"></a>
### Variable c1Ptr

![][public]

**Definition**: `src/colRope.c` (line 1812)

```cpp
Col_Char* CompareChunksInfo::c1Ptr
```

If non-NULL, codepoint of differing character in first chunk.





**Type**: [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) *

**Referenced by**:

* [CompareChunksProc](col_rope_8c.md#group__rope__words_1gaa0f078c09e57fc1f7ba7cdf5e80c7f9d)
* [Col\_CompareRopesL](col_rope_8h.md#group__rope__words_1ga3202ac5414ac22cc7627e0edefbba21c)

<a id="struct_compare_chunks_info_1a0d2ea5f98cf8a8f19a8e0a3ec8e03a36"></a>
### Variable c2Ptr

![][public]

**Definition**: `src/colRope.c` (line 1814)

```cpp
Col_Char* CompareChunksInfo::c2Ptr
```

If non-NULL, codepoint of differing character in second chunk.





**Type**: [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) *

**Referenced by**:

* [CompareChunksProc](col_rope_8c.md#group__rope__words_1gaa0f078c09e57fc1f7ba7cdf5e80c7f9d)
* [Col\_CompareRopesL](col_rope_8h.md#group__rope__words_1ga3202ac5414ac22cc7627e0edefbba21c)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)