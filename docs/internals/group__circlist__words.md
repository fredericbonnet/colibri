<a id="group__circlist__words"></a>
# Circular List Words



Circular lists words are [Immediate Words](group__immediate__words.md#group__immediate__words) made of a core list that repeats infinitely. Core lists can be either regular [Immutable Lists](group__list__words.md#group__list__words) or [Void List Words](group__voidlist__words.md#group__voidlist__words).






**Requirements**:


* Circular list words need to store their core list word.


**Parameters**:

* **Core**: Core list word.


**Word Layout**:

On all architectures the _n_-bit word layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        circlist_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="20" align="left">0</td><td sides="B" width="20" align="right">3</td>
                <td sides="B" width="140" align="left">4</td><td sides="B" width="140" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td colspan="2">0001</td>
                <td href="@ref WORD_CIRCLIST_CORE" title="WORD_CIRCLIST_CORE" colspan="2">Core (regular list)</td>
            </tr>
            </table>
        >]
    }
    
 
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        circvoidlist_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="25" align="left">0</td><td sides="B" width="25" align="right">4</td>
                <td sides="B" width="135" align="left">5</td><td sides="B" width="135" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td colspan="2">00111</td>
                <td href="@ref WORD_VOIDLIST_LENGTH" title="WORD_VOIDLIST_LENGTH" colspan="2">Core length (void list)</td>
            </tr>
            </table>
        >]
    }
    











**See also**: [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650), [Immutable Lists](group__list__words.md#group__list__words), [Void List Words](group__voidlist__words.md#group__voidlist__words), [immediateWordTypes](col_word_int_8h.md#group__immediate__words_1gadc93860da89f6a52a884c10b7835aaac)

## Core List Word Creation

<a id="group__circlist__words_1ga51827e999fb41c560624c93dd2a8770b"></a>
### Macro WORD\_CIRCLIST\_NEW

![][public]

```cpp
#define WORD_CIRCLIST_NEW     (([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043))(((uintptr_t)(core))|8))( core )
```

Circular list word creation.

**Parameters**:

* **core**: Core list. Must be acyclic.


**Returns**:

The new circular list word.



**See also**: [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)



## Core List Word Accessors

<a id="group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36"></a>
### Macro WORD\_CIRCLIST\_CORE

![][public]

```cpp
#define WORD_CIRCLIST_CORE (([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043))(((uintptr_t)(word))&~8))( word )
```

Get core list.

**Parameters**:

* **word**: Word to access.



**See also**: [WORD\_CIRCLIST\_NEW](col_word_int_8h.md#group__circlist__words_1ga51827e999fb41c560624c93dd2a8770b)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)