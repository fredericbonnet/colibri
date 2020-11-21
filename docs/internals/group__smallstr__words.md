<a id="group__smallstr__words"></a>
# Small String Words



Small string words are [Immediate Words](group__immediate__words.md#group__immediate__words) storing short 8-bit strings. Maximum capacity is the machine word size minus one (i.e. 3 on 32-bit, 7 on 64-bit). Larger strings are cell-based.






**Requirements**:


* Small string words need to store the characters and the length.


**Parameters**:

* **Length(L)**: Number of characters [0,7].
* **Data**: Character data.


**Word Layout**:

On all architectures the _n_-bit word layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        smallstr_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="25" align="left">0</td><td sides="B" width="25" align="right">4</td>
                <td sides="B" width="15" align="left">5</td><td sides="B" width="15" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td colspan="2">00110</td>
                <td href="@ref WORD_SMALLSTR_LENGTH" title="WORD_SMALLSTR_LENGTH" colspan="2">L</td>
                <td href="@ref WORD_SMALLSTR_DATA" title="WORD_SMALLSTR_DATA" colspan="2">Data</td>
            </tr>
            </table>
        >]
    }
    









**See also**: [WORD\_TYPE\_SMALLSTR](col_word_int_8h.md#group__words_1ga4148e4b4a03bff2e7715274ae4b126eb), [immediateWordTypes](col_word_int_8h.md#group__immediate__words_1gadc93860da89f6a52a884c10b7835aaac)

## Small String Word Constants

<a id="group__smallstr__words_1ga465e96e908ab71872527b13453fda803"></a>
### Macro SMALLSTR\_MAX\_LENGTH

![][public]

```cpp
#define SMALLSTR_MAX_LENGTH (sizeof([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043))-1)
```

Maximum length of small string words.





<a id="group__smallstr__words_1gaced4b5ab427a8009e9e3586f60cf487f"></a>
### Macro WORD\_SMALLSTR\_EMPTY

![][public]

```cpp
#define WORD_SMALLSTR_EMPTY (([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)) 12)
```

Empty string immediate singleton.

This is simply a small string of zero length.



## Small String Word Accessors

<a id="group__smallstr__words_1gad314e3ed08cf8450986fec6ac88a0e11"></a>
### Macro WORD\_SMALLSTR\_LENGTH

![][public]

```cpp
#define WORD_SMALLSTR_LENGTH ((((uintptr_t)(word))&0xFF)>>5)( word )
```

Get small string length.

**Parameters**:

* **word**: Word to access.



**See also**: [WORD\_SMALLSTR\_SET\_LENGTH](col_word_int_8h.md#group__smallstr__words_1gadf6c0a3a5c0d10bdff25627332aa16a6)



<a id="group__smallstr__words_1gadf6c0a3a5c0d10bdff25627332aa16a6"></a>
### Macro WORD\_SMALLSTR\_SET\_LENGTH

![][public]

```cpp
#define WORD_SMALLSTR_SET_LENGTH (*((uintptr_t *)&(word)) = ((length)<<5)|12)( word ,length )
```

Set small string length.

**Parameters**:

* **word**: Word to access.
* **length**: Length value.



**See also**: [WORD\_SMALLSTR\_LENGTH](col_word_int_8h.md#group__smallstr__words_1gad314e3ed08cf8450986fec6ac88a0e11)



<a id="group__smallstr__words_1ga0895416dca7181737047c535c2f6edee"></a>
### Macro WORD\_SMALLSTR\_DATA

![][public]

```cpp
#define WORD_SMALLSTR_DATA (([Col\_Char1](colibri_8h.md#group__strings_1ga961d5ffde8aa1fba42d4b669a6199e76)  *)&(word))( word )
```

Pointer to beginning of small string data.

**Parameters**:

* **word**: Word to access.



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)