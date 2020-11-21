<a id="struct_find_char_info"></a>
# Structure FindCharInfo

![][C++]
![][private]

**Definition**: `src/colRope.c` (line 1527)

Structure used to collect data during character search.

**See also**: [FindCharProc](col_rope_8c.md#group__rope__words_1gafb8374e5f7f739d98741621c4c56ed55), [Col\_RopeFind](col_rope_8h.md#group__rope__words_1ga5cb7658292e0e5ae51fe384ecdc6d6a9)

## Members

* [c](struct_find_char_info.md#struct_find_char_info_1a04dca1b4a32abe160673d959396543d7)
* [pos](struct_find_char_info.md#struct_find_char_info_1aa86fa92344a6765bdfb7a621673ad049)
* [reverse](struct_find_char_info.md#struct_find_char_info_1a94b839a7e7802d92997811a8afc62b12)

## Public attributes

<a id="struct_find_char_info_1a04dca1b4a32abe160673d959396543d7"></a>
### Variable c

![][public]

**Definition**: `src/colRope.c` (line 1528)

```cpp
Col_Char FindCharInfo::c
```

Character to search for.





**Type**: [Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862)

**Referenced by**:

* [FindCharProc](col_rope_8c.md#group__rope__words_1gafb8374e5f7f739d98741621c4c56ed55)
* [Col\_RopeFind](col_rope_8h.md#group__rope__words_1ga5cb7658292e0e5ae51fe384ecdc6d6a9)

<a id="struct_find_char_info_1a94b839a7e7802d92997811a8afc62b12"></a>
### Variable reverse

![][public]

**Definition**: `src/colRope.c` (line 1529)

```cpp
int FindCharInfo::reverse
```

Whether to traverse in reverse order.





**Type**: int

**Referenced by**:

* [FindCharProc](col_rope_8c.md#group__rope__words_1gafb8374e5f7f739d98741621c4c56ed55)
* [Col\_RopeFind](col_rope_8h.md#group__rope__words_1ga5cb7658292e0e5ae51fe384ecdc6d6a9)

<a id="struct_find_char_info_1aa86fa92344a6765bdfb7a621673ad049"></a>
### Variable pos

![][public]

**Definition**: `src/colRope.c` (line 1530)

```cpp
size_t FindCharInfo::pos
```

Upon return, position of character if found.





**Type**: size_t

**Referenced by**:

* [FindCharProc](col_rope_8c.md#group__rope__words_1gafb8374e5f7f739d98741621c4c56ed55)
* [Col\_RopeFind](col_rope_8h.md#group__rope__words_1ga5cb7658292e0e5ae51fe384ecdc6d6a9)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)