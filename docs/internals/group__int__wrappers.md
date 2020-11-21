<a id="group__int__wrappers"></a>
# Integer Wrappers



Integer wrappers are [Wrap Words](group__wrap__words.md#group__wrap__words) that wrap an integer value, either because the value wouldn't fit [Small Integer Words](group__smallint__words.md#group__smallint__words) or the word would need a synonym field.






**Requirements**:


* Integer wrappers must store a native integer value.


**Parameters**:

* **Value**: Wrapped integer value.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        int_wrapper [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_WRAP_FLAGS" title="WORD_WRAP_FLAGS" colspan="2">Flags</td>
                <td href="@ref WORD_WRAP_TYPE" title="WORD_WRAP_TYPE" colspan="2">Wrapped type</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_SYNONYM" title="WORD_SYNONYM" colspan="7">Synonym</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_INTWRAP_VALUE" title="WORD_INTWRAP_VALUE" colspan="7">Value</td>
            </tr>
            <tr><td sides="R">3</td>
                <td colspan="7" bgcolor="grey75">Unused</td>
            </tr>
            </table>
        >]
    }
    








**See also**: [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)

## Integer Wrapper Creation

<a id="group__int__wrappers_1ga9656b5f429e598b3f268d4f9d3821967"></a>
### Macro WORD\_INTWRAP\_INIT

![][public]

```cpp
#define WORD_INTWRAP_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)); \
    [WORD\_WRAP\_FLAGS](col_word_int_8h.md#group__wrap__words_1ga3b506672d024f169338c7d5191044f7f)(word) = (flags); \
    [WORD\_WRAP\_TYPE](col_word_int_8h.md#group__wrap__words_1ga6f350f679e04dbdc05e08f193bad9d1f)(word) = [COL\_INT](col_word_8h.md#group__words_1gaf0ec1d910f6ba19ede429284179b81fd); \
    [WORD\_SYNONYM](col_word_int_8h.md#group__regular__words_1ga19cfddbcf0127f5088803cc68ddb8eaa)(word) = [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065); \
    [WORD\_INTWRAP\_VALUE](col_word_int_8h.md#group__int__wrappers_1ga1110d15dc1187fc3d3f1f7c47305200e)(word) = (value);( word ,flags ,value )
```

Integer wrapper initializer.

**Parameters**:

* **word**: Word to initialize.
* **flags**: [WORD\_WRAP\_FLAGS](col_word_int_8h.md#group__wrap__words_1ga3b506672d024f169338c7d5191044f7f).
* **value**: [WORD\_INTWRAP\_VALUE](col_word_int_8h.md#group__int__wrappers_1ga1110d15dc1187fc3d3f1f7c47305200e).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)



## Integer Wrapper Accessors

<a id="group__int__wrappers_1ga1110d15dc1187fc3d3f1f7c47305200e"></a>
### Macro WORD\_INTWRAP\_VALUE

![][public]

```cpp
#define WORD_INTWRAP_VALUE (((intptr_t *)(word))[2])( word )
```

Get/set wrapped integer value.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_INTWRAP\_INIT](col_word_int_8h.md#group__int__wrappers_1ga9656b5f429e598b3f268d4f9d3821967)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)