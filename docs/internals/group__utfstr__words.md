<a id="group__utfstr__words"></a>
# Variable-Width UTF Strings



Variable-width UTF-8/16 strings are constant arrays of variable-width characters requiring dedicated encoding/decoding procedures for access.






**Requirements**:


* UTF-8/16 string words must know both character and byte lengths, as well as their character format (either [COL\_UTF8](colibri_8h.md#group__strings_1gga125054104f6260ea3902e6e46ebfdfa0ac5f7c75663139add735ce90077cef306) or [COL\_UTF16](colibri_8h.md#group__strings_1gga125054104f6260ea3902e6e46ebfdfa0ad926084d627f96e4d65a353daef34854)).

* Characters are stored within the word cells following the header.

* UTF-8/16 string words use as much cells as needed to store their characters, up to the page limit. Hence, number of cells is at most [AVAILABLE\_CELLS](col_internal_8h.md#group__pages__cells_1ga524e5a52183dcc7088644df29ef766bf). This is a tradeoff between space and performance: larger strings are split into several smaller chunks that get concatenated, but random access is better compared to flat UTF strings thanks to the binary tree indexation, as opposed to per-character linear scanning of UTF strings.


**Parameters**:

* **Format**: Character format.
* **Length**: Number of characters in array.
* **Byte**: Length Byte length of character array.
* **Data**: Array of character code units.


**Cell Layout**:

On all architectures the cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        utfstr_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">31</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_UTFSTR_FORMAT" title="WORD_UTFSTR_FORMAT" colspan="2">Format</td>
                <td href="@ref WORD_UTFSTR_LENGTH" title="WORD_UTFSTR_LENGTH" colspan="2">Length</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_UTFSTR_BYTELENGTH" title="WORD_UTFSTR_BYTELENGTH" colspan="4">Byte Length</td>
                <td href="@ref WORD_UTFSTR_DATA" title="WORD_UTFSTR_DATA" sides="R" colspan="2"></td>
            </tr>
            <tr><td sides="R">.</td>
                <td href="@ref WORD_UTFSTR_DATA" title="WORD_UTFSTR_DATA" colspan="6" rowspan="3" sides="BR" width="320">Character data</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }
    








**See also**: [WORD\_TYPE\_UTFSTR](col_word_int_8h.md#group__words_1gaed71812c790125f3aeb204e6ba4f55b4)

## UTF String Word Constants

<a id="group__utfstr__words_1gaaaafcb7cad000c065f7c5cfcd8d0c7b8"></a>
### Macro UTFSTR\_HEADER\_SIZE

![][public]

```cpp
#define UTFSTR_HEADER_SIZE (sizeof(uint16_t)*3)
```

Byte size of UTF-8/16 string header.





<a id="group__utfstr__words_1ga280e86ac3ab3388ac552f03d2196584c"></a>
### Macro UTFSTR\_MAX\_BYTELENGTH

![][public]

```cpp
#define UTFSTR_MAX_BYTELENGTH ([AVAILABLE\_CELLS](col_internal_8h.md#group__pages__cells_1ga524e5a52183dcc7088644df29ef766bf)*[CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)-[UTFSTR\_HEADER\_SIZE](col_rope_int_8h.md#group__utfstr__words_1gaaaafcb7cad000c065f7c5cfcd8d0c7b8))
```

Maximum byte length of UTF-8/16 strings.





## UTF String Word Utilities

<a id="group__utfstr__words_1ga00f51041c27e8cfcfe2da5e22795cb0c"></a>
### Macro UTFSTR\_SIZE

![][public]

```cpp
#define UTFSTR_SIZE     ([NB\_CELLS](col_internal_8h.md#group__pages__cells_1ga6969cfc3c9b2913a913df84f7842ce74)([UTFSTR\_HEADER\_SIZE](col_rope_int_8h.md#group__utfstr__words_1gaaaafcb7cad000c065f7c5cfcd8d0c7b8)+(byteLength)))( byteLength )
```

Get number of cells taken by an UTF-8/16 string of the given byte length.

**Parameters**:

* **byteLength**: Byte size of string.


**Returns**:

Number of cells taken by word.



## UTF String Word Creation

<a id="group__utfstr__words_1gab6c2e0088ebce6cd847ed1fbd6fe1acb"></a>
### Macro WORD\_UTFSTR\_INIT

![][public]

```cpp
#define WORD_UTFSTR_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_UTFSTR](col_word_int_8h.md#group__words_1gaed71812c790125f3aeb204e6ba4f55b4)); \
    [WORD\_UTFSTR\_FORMAT](col_rope_int_8h.md#group__utfstr__words_1ga3405010b5b366fe5f7f87af019d84536)(word) = (uint8_t) (format); \
    [WORD\_UTFSTR\_LENGTH](col_rope_int_8h.md#group__utfstr__words_1gacd5812fddcc3e5259e89b0a62ac1a238)(word) = (uint16_t) (length); \
    [WORD\_UTFSTR\_BYTELENGTH](col_rope_int_8h.md#group__utfstr__words_1ga837308f56ffc081bf15248ecd7adda9a)(word) = (uint16_t) (byteLength);( word ,format ,length ,byteLength )
```

UTF-8/16 string word initializer.

**Parameters**:

* **word**: Word to initialize.
* **format**: [WORD\_UTFSTR\_FORMAT](col_rope_int_8h.md#group__utfstr__words_1ga3405010b5b366fe5f7f87af019d84536).
* **length**: [WORD\_UTFSTR\_LENGTH](col_rope_int_8h.md#group__utfstr__words_1gacd5812fddcc3e5259e89b0a62ac1a238).
* **byteLength**: [WORD\_UTFSTR\_BYTELENGTH](col_rope_int_8h.md#group__utfstr__words_1ga837308f56ffc081bf15248ecd7adda9a).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_UTFSTR](col_word_int_8h.md#group__words_1gaed71812c790125f3aeb204e6ba4f55b4)



## UTF String Word Accessors

<a id="group__utfstr__words_1ga3405010b5b366fe5f7f87af019d84536"></a>
### Macro WORD\_UTFSTR\_FORMAT

![][public]

```cpp
#define WORD_UTFSTR_FORMAT (((int8_t *)(word))[1])( word )
```

Get/set character format.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_UTFSTR\_INIT](col_rope_int_8h.md#group__utfstr__words_1gab6c2e0088ebce6cd847ed1fbd6fe1acb)



<a id="group__utfstr__words_1gacd5812fddcc3e5259e89b0a62ac1a238"></a>
### Macro WORD\_UTFSTR\_LENGTH

![][public]

```cpp
#define WORD_UTFSTR_LENGTH (((uint16_t *)(word))[1])( word )
```

Get/set character length.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_UTFSTR\_INIT](col_rope_int_8h.md#group__utfstr__words_1gab6c2e0088ebce6cd847ed1fbd6fe1acb)



<a id="group__utfstr__words_1ga837308f56ffc081bf15248ecd7adda9a"></a>
### Macro WORD\_UTFSTR\_BYTELENGTH

![][public]

```cpp
#define WORD_UTFSTR_BYTELENGTH (((uint16_t *)(word))[2])( word )
```

Get/set byte length.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_UTFSTR\_INIT](col_rope_int_8h.md#group__utfstr__words_1gab6c2e0088ebce6cd847ed1fbd6fe1acb)



<a id="group__utfstr__words_1ga54f4a0ffe3636b2e29185ca90c342876"></a>
### Macro WORD\_UTFSTR\_DATA

![][public]

```cpp
#define WORD_UTFSTR_DATA ((const char *)(word)+[UTFSTR\_HEADER\_SIZE](col_rope_int_8h.md#group__utfstr__words_1gaaaafcb7cad000c065f7c5cfcd8d0c7b8))( word )
```

Pointer to beginning of string data.

**Parameters**:

* **word**: Word to access.



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)