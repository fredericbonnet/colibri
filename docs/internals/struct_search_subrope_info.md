<a id="struct_search_subrope_info"></a>
# Structure SearchSubropeInfo

![][C++]
![][private]

**Definition**: `src/colRope.c` (line 1636)

Structure used to collect data during subrope search.

**See also**: [SearchSubropeProc](col_rope_8c.md#group__rope__words_1gac469cb18ebe714917347f930c6951f41), [Col\_RopeSearch](col_rope_8h.md#group__rope__words_1gace5992731949e51b03afcb20527c95dd)

## Members

* [first](struct_search_subrope_info.md#struct_search_subrope_info_1aa6ffd54ef7223d4f7153775ef3661094)
* [pos](struct_search_subrope_info.md#struct_search_subrope_info_1a68e716d34eb206c40936245ab774b6cd)
* [reverse](struct_search_subrope_info.md#struct_search_subrope_info_1a6689adef49429bf217e4b7f434bad147)
* [rope](struct_search_subrope_info.md#struct_search_subrope_info_1abfd66a3be58858bf453396deca1d2402)
* [subrope](struct_search_subrope_info.md#struct_search_subrope_info_1a5357a82233680c9f9033b0fb3cf0b4c8)

## Public attributes

<a id="struct_search_subrope_info_1abfd66a3be58858bf453396deca1d2402"></a>
### Variable rope

![][public]

**Definition**: `src/colRope.c` (line 1637)

```cpp
Col_Word SearchSubropeInfo::rope
```

Rope to search subrope into.





**Type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**Referenced by**:

* [SearchSubropeProc](col_rope_8c.md#group__rope__words_1gac469cb18ebe714917347f930c6951f41)
* [Col\_RopeSearch](col_rope_8h.md#group__rope__words_1gace5992731949e51b03afcb20527c95dd)

<a id="struct_search_subrope_info_1a5357a82233680c9f9033b0fb3cf0b4c8"></a>
### Variable subrope

![][public]

**Definition**: `src/colRope.c` (line 1638)

```cpp
Col_Word SearchSubropeInfo::subrope
```

Subrope to search for.





**Type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**Referenced by**:

* [SearchSubropeProc](col_rope_8c.md#group__rope__words_1gac469cb18ebe714917347f930c6951f41)
* [Col\_RopeSearch](col_rope_8h.md#group__rope__words_1gace5992731949e51b03afcb20527c95dd)

<a id="struct_search_subrope_info_1aa6ffd54ef7223d4f7153775ef3661094"></a>
### Variable first

![][public]

**Definition**: `src/colRope.c` (line 1639)

```cpp
Col_Char SearchSubropeInfo::first
```

First character of rope to search for.





**Type**: [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862)

**Referenced by**:

* [SearchSubropeProc](col_rope_8c.md#group__rope__words_1gac469cb18ebe714917347f930c6951f41)
* [Col\_RopeSearch](col_rope_8h.md#group__rope__words_1gace5992731949e51b03afcb20527c95dd)

<a id="struct_search_subrope_info_1a6689adef49429bf217e4b7f434bad147"></a>
### Variable reverse

![][public]

**Definition**: `src/colRope.c` (line 1640)

```cpp
int SearchSubropeInfo::reverse
```

Whether to traverse in reverse order.





**Type**: int

**Referenced by**:

* [SearchSubropeProc](col_rope_8c.md#group__rope__words_1gac469cb18ebe714917347f930c6951f41)
* [Col\_RopeSearch](col_rope_8h.md#group__rope__words_1gace5992731949e51b03afcb20527c95dd)

<a id="struct_search_subrope_info_1a68e716d34eb206c40936245ab774b6cd"></a>
### Variable pos

![][public]

**Definition**: `src/colRope.c` (line 1641)

```cpp
size_t SearchSubropeInfo::pos
```

Upon return, position of character if found.





**Type**: size_t

**Referenced by**:

* [SearchSubropeProc](col_rope_8c.md#group__rope__words_1gac469cb18ebe714917347f930c6951f41)
* [Col\_RopeSearch](col_rope_8h.md#group__rope__words_1gace5992731949e51b03afcb20527c95dd)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)