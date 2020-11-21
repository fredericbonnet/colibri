<a id="group__smallfp__words"></a>
# Small Floating Point Words



Small floating point words are [Immediate Words](group__immediate__words.md#group__immediate__words) storing floating points whose lower 2 bits of the mantissa are zero, so that they are free for the tag bits. This includes IEEE 754 special values such as +/-0, +/-INF and NaN.





Other values are stored in [Floating Point Wrappers](group__fp__wrappers.md#group__fp__wrappers).






**Requirements**:


* Small floating point words need to store the floating point value in the word identifier.


**Parameters**:

* **Value**: Floating point value of word.


**Word Layout**:

On all architectures the _n_-bit word layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        smallfp_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="20">01</td>
                <td sides="B" width="147" align="left">2</td><td sides="B" width="150" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td>01</td>
                <td href="@ref WORD_SMALLFP_VALUE" title="WORD_SMALLFP_VALUE" colspan="2">Value</td>
            </tr>
            </table>
        >]
    }
    









**See also**: [WORD\_TYPE\_SMALLFP](col_word_int_8h.md#group__words_1gaf5bde441039615017c30996b222a55c8), [immediateWordTypes](col_word_int_8h.md#group__immediate__words_1gadc93860da89f6a52a884c10b7835aaac)

## Small Floating Point Word Utilities

<a id="group__smallfp__words_1ga2c853438af5ac989dce08fcbed557051"></a>
### Macro SMALLFP\_TYPE

![][public]

```cpp
#define SMALLFP_TYPE float
```

C Type used by immediate floating point words.

**See also**: [FloatConvert](union_float_convert.md#union_float_convert), [WORD\_SMALLFP\_NEW](col_word_int_8h.md#group__smallfp__words_1ga8b37cdd7a9aa19766a182dbbed963815)



## Small Floating Point Word Creation

<a id="group__smallfp__words_1ga8b37cdd7a9aa19766a182dbbed963815"></a>
### Macro WORD\_SMALLFP\_NEW

![][public]

```cpp
#define WORD_SMALLFP_NEW     ((c).f = ([SMALLFP\_TYPE](col_word_int_8h.md#group__smallfp__words_1ga2c853438af5ac989dce08fcbed557051))(value), (c).i &= ~3, (c).i |= 2, (c).w)( value ,c )
```

Small floating point word creation.

**Parameters**:

* **value**: Floating point value.
* **c**: [FloatConvert](union_float_convert.md#union_float_convert) conversion structure.


!> **Warning** \
Argument **c** is referenced several times by the macro. Make sure to avoid any side effect.


**Returns**:

The new small floating point word.



**See also**: [WORD\_SMALLFP\_VALUE](col_word_int_8h.md#group__smallfp__words_1gae3d867f57b6b1e2bb0d0919aee9a711e)



## Small Floating Point Word Accessors

<a id="group__smallfp__words_1gae3d867f57b6b1e2bb0d0919aee9a711e"></a>
### Macro WORD\_SMALLFP\_VALUE

![][public]

```cpp
#define WORD_SMALLFP_VALUE     ((c).w = (word), (c).i &= ~3, (c).f)( word ,c )
```

Get value of small floating point word.

**Parameters**:

* **word**: Word to access.
* **c**: [FloatConvert](union_float_convert.md#union_float_convert) conversion structure.


**Returns**:

The floating point value.



**See also**: [WORD\_SMALLFP\_NEW](col_word_int_8h.md#group__smallfp__words_1ga8b37cdd7a9aa19766a182dbbed963815)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)