<a id="group__fp__wrappers"></a>
# Floating Point Wrappers



Floating point wrappers are [Wrap Words](group__wrap__words.md#group__wrap__words) that wrap a floating point value, either because the value wouldn't fit [Small Floating Point Words](group__smallfp__words.md#group__smallfp__words) or the word would need a synonym field.






**Requirements**:


* Floating point wrappers must store a native floating point value.


**Parameters**:

* **Value**: Wrapped floating point value.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        fp_wrapper [shape=none, label=<
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
                <td href="@ref WORD_FPWRAP_VALUE" title="WORD_FPWRAP_VALUE" sides="LTR" colspan="7">Value</td>
            </tr>
            <tr><td sides="R">3</td>
                <td sides="LBR" colspan="7" bgcolor="grey75">Unused (n &gt; 32)</td>
            </tr>
            </table>
        >]
    }
    








**See also**: [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)

## Floating Point Wrapper Creation

<a id="group__fp__wrappers_1ga66d2954c1e1ea98185f0187e465f0290"></a>
### Macro WORD\_FPWRAP\_INIT

![][public]

```cpp
#define WORD_FPWRAP_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)); \
    [WORD\_WRAP\_FLAGS](col_word_int_8h.md#group__wrap__words_1ga3b506672d024f169338c7d5191044f7f)(word) = (flags); \
    [WORD\_WRAP\_TYPE](col_word_int_8h.md#group__wrap__words_1ga6f350f679e04dbdc05e08f193bad9d1f)(word) = [COL\_FLOAT](col_word_8h.md#group__words_1gaf129d60ea367bc8f58d1a060d0fdba30); \
    [WORD\_SYNONYM](col_word_int_8h.md#group__regular__words_1ga19cfddbcf0127f5088803cc68ddb8eaa)(word) = [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065); \
    [WORD\_FPWRAP\_VALUE](col_word_int_8h.md#group__fp__wrappers_1gaf730548e2147681f5248e8ecfc35ef84)(word) = (value);( word ,flags ,value )
```

Integer wrapper initializer.

**Parameters**:

* **word**: Word to initialize.
* **flags**: [WORD\_WRAP\_FLAGS](col_word_int_8h.md#group__wrap__words_1ga3b506672d024f169338c7d5191044f7f).
* **value**: [WORD\_FPWRAP\_VALUE](col_word_int_8h.md#group__fp__wrappers_1gaf730548e2147681f5248e8ecfc35ef84).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)



## Floating Point Wrapper Accessors

<a id="group__fp__wrappers_1gaf730548e2147681f5248e8ecfc35ef84"></a>
### Macro WORD\_FPWRAP\_VALUE

![][public]

```cpp
#define WORD_FPWRAP_VALUE (*(double *)(((intptr_t *)(word))+2))( word )
```

Get/set wrapped floating point value.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_FPWRAP\_INIT](col_word_int_8h.md#group__fp__wrappers_1ga66d2954c1e1ea98185f0187e465f0290)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)