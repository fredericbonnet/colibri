<a id="group__ucsstr__words"></a>
# Fixed-Width UCS Strings



Fixed-width UCS strings are constant arrays of characters that are directly addressable.






**Requirements**:


* UCS string words must know their character length as well as their character format (either [COL\_UCS1](colibri_8h.md#group__strings_1gga125054104f6260ea3902e6e46ebfdfa0a18ed97ba951a5d02c0a6e039445235b8), [COL\_UCS2](colibri_8h.md#group__strings_1gga125054104f6260ea3902e6e46ebfdfa0ae43fbe04c500f0868e4b95a669a987e0) or [COL\_UCS4](colibri_8h.md#group__strings_1gga125054104f6260ea3902e6e46ebfdfa0af1d4922d36f7509d8936b0af29b0a9e4)).

* Byte length is deduced from character length and format.

* Characters are stored within the word cells following the header.

* UCS string words use as much cells as needed to store their characters.


**Parameters**:

* **Format**: Character format.
* **Length**: Size of character array.
* **Data**: Array of character codepoints.


**Cell Layout**:

On all architectures the cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        ucsstr_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">31</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_UCSSTR_FORMAT" title="WORD_UCSSTR_FORMAT" colspan="2">Format</td>
                <td href="@ref WORD_UCSSTR_LENGTH" title="WORD_UCSSTR_LENGTH" colspan="2">Length</td>
            </tr>
            <tr><td sides="R">.</td>
                <td href="@ref WORD_UCSSTR_DATA" title="WORD_UCSSTR_DATA" colspan="6" rowspan="4">Character data</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }
    








**See also**: [WORD\_TYPE\_UCSSTR](col_word_int_8h.md#group__words_1ga3a96c4366162e66944451d0b2ddb9221)

## UCS String Word Constants

<a id="group__ucsstr__words_1ga04d9ab12584a6fc694c0949e52f481c5"></a>
### Macro UCSSTR\_HEADER\_SIZE

![][public]

```cpp
#define UCSSTR_HEADER_SIZE (sizeof(uint16_t)*2)
```

Byte size of UCS string header.





<a id="group__ucsstr__words_1gacf26d3e514c09e967a7120f031f7d184"></a>
### Macro UCSSTR\_MAX\_LENGTH

![][public]

```cpp
#define UCSSTR_MAX_LENGTH UINT16_MAX
```

Maximum char length of UCS strings.





## UCS String Word Utilities

<a id="group__ucsstr__words_1gaaac90786862f40e08eac7734465ec132"></a>
### Macro UCSSTR\_SIZE

![][public]

```cpp
#define UCSSTR_SIZE     ([NB\_CELLS](col_internal_8h.md#group__pages__cells_1ga6969cfc3c9b2913a913df84f7842ce74)([UCSSTR\_HEADER\_SIZE](col_rope_int_8h.md#group__ucsstr__words_1ga04d9ab12584a6fc694c0949e52f481c5)+(byteLength)))( byteLength )
```

Get number of cells taken by an UCS string of the given byte length.

**Parameters**:

* **byteLength**: Byte size of string.


**Returns**:

Number of cells taken by word.



## UCS String Word Creation

<a id="group__ucsstr__words_1gae02e5fdcadce075a6d7891b923633ecf"></a>
### Macro WORD\_UCSSTR\_INIT

![][public]

```cpp
#define WORD_UCSSTR_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_UCSSTR](col_word_int_8h.md#group__words_1ga3a96c4366162e66944451d0b2ddb9221)); \
    [WORD\_UCSSTR\_FORMAT](col_rope_int_8h.md#group__ucsstr__words_1gab71309e620c0b5eb4f3ade931103223f)(word) = (uint8_t) (format); \
    [WORD\_UCSSTR\_LENGTH](col_rope_int_8h.md#group__ucsstr__words_1ga2de5d82b197dbd067fdd323532fc353d)(word) = (uint16_t) (length);( word ,format ,length )
```

UCS string word initializer.

**Parameters**:

* **word**: Word to initialize.
* **format**: [WORD\_UCSSTR\_FORMAT](col_rope_int_8h.md#group__ucsstr__words_1gab71309e620c0b5eb4f3ade931103223f).
* **length**: [WORD\_UCSSTR\_LENGTH](col_rope_int_8h.md#group__ucsstr__words_1ga2de5d82b197dbd067fdd323532fc353d).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_UCSSTR](col_word_int_8h.md#group__words_1ga3a96c4366162e66944451d0b2ddb9221)



## UCS String Word Accessors

<a id="group__ucsstr__words_1gab71309e620c0b5eb4f3ade931103223f"></a>
### Macro WORD\_UCSSTR\_FORMAT

![][public]

```cpp
#define WORD_UCSSTR_FORMAT (((int8_t *)(word))[1])( word )
```

Get/set character format.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_UCSSTR\_INIT](col_rope_int_8h.md#group__ucsstr__words_1gae02e5fdcadce075a6d7891b923633ecf)



<a id="group__ucsstr__words_1ga2de5d82b197dbd067fdd323532fc353d"></a>
### Macro WORD\_UCSSTR\_LENGTH

![][public]

```cpp
#define WORD_UCSSTR_LENGTH (((uint16_t *)(word))[1])( word )
```

Get/set character length.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_UCSSTR\_INIT](col_rope_int_8h.md#group__ucsstr__words_1gae02e5fdcadce075a6d7891b923633ecf)



<a id="group__ucsstr__words_1ga72e52b45c2851e4038089248df5ceddf"></a>
### Macro WORD\_UCSSTR\_DATA

![][public]

```cpp
#define WORD_UCSSTR_DATA ((const char *)(word)+[UCSSTR\_HEADER\_SIZE](col_rope_int_8h.md#group__ucsstr__words_1ga04d9ab12584a6fc694c0949e52f481c5))( word )
```

Pointer to beginning of string data.

**Parameters**:

* **word**: Word to access.



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)