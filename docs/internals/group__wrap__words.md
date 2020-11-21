<a id="group__wrap__words"></a>
# Wrap Words



Wrap words are generic wrappers for [Words](group__words.md#group__words) lacking specific features.






* Words may have synonyms that can take any accepted word value: [Immediate Words](group__immediate__words.md#group__immediate__words) (e.g. Nil), or cell-based [Regular Words](group__regular__words.md#group__regular__words). Words can thus be part of chains of synonyms having different types, but with semantically identical values. Such chains form a circular linked list using this field. The order of words in a synonym chain has no importance.


Some word types have no synonym field (typically [Immediate Words](group__immediate__words.md#group__immediate__words), but also many [Predefined Words](group__predefined__words.md#group__predefined__words)), in this case they must be wrapped into structures that have one when they are added to a chain of synonyms.

* [Immediate Words](group__immediate__words.md#group__immediate__words) storing native datatypes often cannot represent the whole range of values. For example, [Small Integer Words](group__smallint__words.md#group__smallint__words) lack one bit (the MSB) compared to native integers. This means that larger values cannot be represented as immediate word values. So these datatypes need to use wrap words instead of immediate words to store these values. But both representations remain semantically identical.


For example, [Col\_NewIntWord()](col_word_8h.md#group__words_1gaba67c33e1004d5db691cb5834b77645e) will create either [Small Integer Words](group__smallint__words.md#group__smallint__words) or [Integer Wrappers](group__int__wrappers.md#group__int__wrappers) depending on the integer value.








**Requirements**:


* Wrap words must store wrapped data.

* Wrap words can be part of a synonym chain.

* Wrap words must know the [word type](#group__words_1word_types) of the wrapped data type.


**Todo**:

* Wrap words have a flags field for future purposes.







**Parameters**:

* **WrappedType**: Wrapped value [type](#group__words_1word_types).
* **Synonym**: [Generic word synonym field](col_word_int_8h.md#group__regular__words_1ga19cfddbcf0127f5088803cc68ddb8eaa).
* **Data**: Wrapped value payload.
* **Flags**: Flags.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        wrap_word [shape=none, label=<
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
                <td colspan="7" rowspan="2">Type-specific data</td>
            </tr>
            <tr><td sides="R">3</td></tr>
            </table>
        >]
    }
    








**See also**: [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)

## Submodules

* [Word Wrappers](group__word__wrappers.md#group__word__wrappers)
* [Integer Wrappers](group__int__wrappers.md#group__int__wrappers)
* [Floating Point Wrappers](group__fp__wrappers.md#group__fp__wrappers)

## Wrap Word Constants

<a id="group__wrap__words_1ga43906d6d0d0cd314aa9bd099c6d6b767"></a>
### Macro WRAP\_FLAG\_MUTABLE

![][public]

```cpp
#define WRAP_FLAG_MUTABLE 1
```



**Todo**:



<a id="group__wrap__words_1gad7b0491e125e8750e4cb29016eabc2df"></a>
### Macro WRAP\_FLAG\_VARIANT

![][public]

```cpp
#define WRAP_FLAG_VARIANT 2
```



**Todo**:



## Wrap Word Accessors

<a id="group__wrap__words_1ga3b506672d024f169338c7d5191044f7f"></a>
### Macro WORD\_WRAP\_FLAGS

![][public]

```cpp
#define WORD_WRAP_FLAGS (((uint8_t *)(word))[1])( word )
```

Get/set wrap word flags.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.





**See also**: [WORD\_WRAP\_INIT](col_word_int_8h.md#group__word__wrappers_1ga5176d0b6e39fc4a27effa648b0f88d27), [WORD\_INTWRAP\_INIT](col_word_int_8h.md#group__int__wrappers_1ga9656b5f429e598b3f268d4f9d3821967), [WORD\_FPWRAP\_INIT](col_word_int_8h.md#group__fp__wrappers_1ga66d2954c1e1ea98185f0187e465f0290)



<a id="group__wrap__words_1ga6f350f679e04dbdc05e08f193bad9d1f"></a>
### Macro WORD\_WRAP\_TYPE

![][public]

```cpp
#define WORD_WRAP_TYPE (((uint16_t *)(word))[1])( word )
```

Get/set wrapped value [type](#group__words_1word_types).

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)