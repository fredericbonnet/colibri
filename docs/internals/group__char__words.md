<a id="group__char__words"></a>
# Character Words



Character words are [Immediate Words](group__immediate__words.md#group__immediate__words) storing one Unicode character codepoint.






**Requirements**:


* Character words need to store Unicode character codepoints up to [COL\_CHAR\_MAX](colibri_8h.md#group__strings_1gae744cbeec6aaa53c56f5d30b035c2dde), i.e. at least 21 bits.

* Character words also need to know the character format for string normalization issues. For that the codepoint width is sufficient (between 1 and 4, i.e. 3 bits).

* As character and boolean words share the same tag, we distinguish both types with boolean words having a zero width field.


**Parameters**:

* **Codepoint**: Unicode codepoint of character word.
* **Width(W)**: Character width set at creation time.


**Word Layout**:

On all architectures the _n_-bit word layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        char_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="25" align="left">0</td><td sides="B" width="25" align="right">4</td>
                <td sides="B" width="15" align="left">5</td><td sides="B" width="15" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td colspan="2">00100</td>
                <td href="@ref WORD_CHAR_WIDTH" title="WORD_CHAR_WIDTH" colspan="2">W</td>
                <td href="@ref WORD_CHAR_CP" title="WORD_CHAR_CP" colspan="2">Codepoint</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            </table>
        >]
    }
    









**See also**: [WORD\_TYPE\_CHARBOOL](col_word_int_8h.md#group__words_1ga1422c37e0c7d668fad81b0710863dad5), [immediateWordTypes](col_word_int_8h.md#group__immediate__words_1gadc93860da89f6a52a884c10b7835aaac)

## Character Word Creation

<a id="group__char__words_1ga3e1012ee747ef755581f098b55cde3c7"></a>
### Macro WORD\_CHAR\_NEW

![][public]

```cpp
#define WORD_CHAR_NEW     (([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043))((((uintptr_t)(value))<<8)|(((uintptr_t)(width))<<5)|4))( value ,width )
```

Unicode char word creation.

**Parameters**:

* **value**: Unicode codepoint.
* **width**: Character width.


**Returns**:

The new character word.




**See also**: [WORD\_CHAR\_CP](col_word_int_8h.md#group__char__words_1ga56cddd2dee0e6afab1ee29c2f83f33ab), [WORD\_CHAR\_WIDTH](col_word_int_8h.md#group__char__words_1ga92aceea828352d3433d76c193f259a1c)



## Character Word Accessors

<a id="group__char__words_1ga56cddd2dee0e6afab1ee29c2f83f33ab"></a>
### Macro WORD\_CHAR\_CP

![][public]

```cpp
#define WORD_CHAR_CP (([Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862))(((uintptr_t)(word))>>8))( word )
```

Get Unicode codepoint of char word.

**Parameters**:

* **word**: Word to access.



**See also**: [WORD\_CHAR\_NEW](col_word_int_8h.md#group__char__words_1ga3e1012ee747ef755581f098b55cde3c7)



<a id="group__char__words_1ga92aceea828352d3433d76c193f259a1c"></a>
### Macro WORD\_CHAR\_WIDTH

![][public]

```cpp
#define WORD_CHAR_WIDTH ((((uintptr_t)(word))>>5)&7)( word )
```

Get char width used at creation time.

**Parameters**:

* **word**: Word to access.



**See also**: [WORD\_CHAR\_NEW](col_word_int_8h.md#group__char__words_1ga3e1012ee747ef755581f098b55cde3c7)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)