<a id="struct_copy_data_info"></a>
# Structure CopyDataInfo

![][C++]
![][private]

**Definition**: `src/colRope.c` (line 729)

Structure used to copy data during the traversal of ropes when normalizing data.

**See also**: [Col\_NormalizeRope](col_rope_8h.md#group__rope__words_1gad8e0ed73e9d579e9aac9bd5ee7603319)

## Members

* [data](struct_copy_data_info.md#struct_copy_data_info_1ad961284b124de229ed1da10962ac9c23)
* [replace](struct_copy_data_info.md#struct_copy_data_info_1aad25196c1265387e0213724f44f24eea)

## Public attributes

<a id="struct_copy_data_info_1ad961284b124de229ed1da10962ac9c23"></a>
### Variable data

![][public]

**Definition**: `src/colRope.c` (line 730)

```cpp
char* CopyDataInfo::data
```

Buffer storing the normalized data.





**Type**: char *

**Referenced by**:

* [Ucs1CopyDataProc](col_rope_8c.md#group__rope__words_1ga10ab42b79a27129c5b2cf5ad0e0b261d)
* [Ucs2CopyDataProc](col_rope_8c.md#group__rope__words_1ga61c8cb8089c0662dec71f99fcbed77e0)
* [Ucs4CopyDataProc](col_rope_8c.md#group__rope__words_1ga5cdbfa1993b834d2fbf3082280be139d)
* [Utf8CopyDataProc](col_rope_8c.md#group__rope__words_1gabea358803d83ed946db99def78568831)
* [Utf16CopyDataProc](col_rope_8c.md#group__rope__words_1gaf089cd3a00689db3e9600b46c21f2867)
* [Col\_NormalizeRope](col_rope_8h.md#group__rope__words_1gad8e0ed73e9d579e9aac9bd5ee7603319)

<a id="struct_copy_data_info_1aad25196c1265387e0213724f44f24eea"></a>
### Variable replace

![][public]

**Definition**: `src/colRope.c` (line 731)

```cpp
Col_Char CopyDataInfo::replace
```

Replacement character for unrepresentable codepoints.





**Type**: [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862)

**Referenced by**:

* [Ucs1CopyDataProc](col_rope_8c.md#group__rope__words_1ga10ab42b79a27129c5b2cf5ad0e0b261d)
* [Ucs2CopyDataProc](col_rope_8c.md#group__rope__words_1ga61c8cb8089c0662dec71f99fcbed77e0)
* [Col\_NormalizeRope](col_rope_8h.md#group__rope__words_1gad8e0ed73e9d579e9aac9bd5ee7603319)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)