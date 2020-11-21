<a id="group__bool__words"></a>
# Boolean Words



Boolean words are [Immediate Words](group__immediate__words.md#group__immediate__words) taking only two values: true or false.






**Requirements**:


* Boolean words need only one bit to distinguish between true and false.


**Parameters**:

* **Value(V)**: Zero for false, nonzero for true.


**Word Layout**:

On all architectures the _n_-bit word layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        bool_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="10">8</td>
                <td sides="B" width="110" align="left">9</td><td sides="B" width="115" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td colspan="2">00100000</td>
                <td href="@ref WORD_BOOL_VALUE" title="WORD_BOOL_VALUE">V</td>
                <td colspan="2" bgcolor="grey75">Unused</td>
            </tr>
            </table>
        >]
    }
    









**See also**: [WORD\_TYPE\_CHARBOOL](col_word_int_8h.md#group__words_1ga1422c37e0c7d668fad81b0710863dad5), [immediateWordTypes](col_word_int_8h.md#group__immediate__words_1gadc93860da89f6a52a884c10b7835aaac)

## Boolean Word Creation

<a id="group__bool__words_1ga5c2f2b90684edf7db909cd71e88a8a71"></a>
### Macro WORD\_BOOL\_NEW

![][public]

```cpp
#define WORD_BOOL_NEW     ((value)?WORD_TRUE:WORD_FALSE)( value )
```

Boolean word creation.

**Parameters**:

* **value**: Boolean value: zero for false, nonzero for true.


**Returns**:

The new boolean word.



**See also**: [WORD\_BOOL\_VALUE](col_word_int_8h.md#group__bool__words_1ga321a19e253b32f551bdee2e2fa104d37)



## Boolean Word Accessors

<a id="group__bool__words_1ga321a19e253b32f551bdee2e2fa104d37"></a>
### Macro WORD\_BOOL\_VALUE

![][public]

```cpp
#define WORD_BOOL_VALUE (((uintptr_t)(word))&0x100)( word )
```

Get value of boolean word: zero for false, nonzero for true.

**Parameters**:

* **word**: Word to access.



**See also**: [WORD\_BOOL\_NEW](col_word_int_8h.md#group__bool__words_1ga5c2f2b90684edf7db909cd71e88a8a71)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)