<a id="group__redirect__words"></a>
# Redirect Words



Redirect words are temporary words used during GC compaction phase (see [PROMOTE\_COMPACT](col_conf_8h.md#group__gc_1gabfe8006c7310448a1d33027d30a59fe4)).





When a word is relocated, the original cell is overwritten by a redirect word that points to the new cell, so that other words pointing to this word can update their pointer.






**Requirements**:


* Redirect words must store the relocated source word.


**Parameters**:

* **Source**: New location of the word.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        redirect_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td colspan="2" bgcolor="grey75">Unused</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_REDIRECT_SOURCE" title="WORD_REDIRECT_SOURCE" colspan="4">Source</td>
            </tr>
            <tr><td sides="R">2</td>
                <td colspan="4" rowspan="2" bgcolor="grey75">Unused</td>
            </tr>
            <tr><td sides="R">3</td></tr>
            </table>
        >]
    }

## Redirect Word Creation

<a id="group__redirect__words_1gae3fd7eacc3ffa9eb2362d23144b9cb86"></a>
### Macro WORD\_REDIRECT\_INIT

![][public]

```cpp
#define WORD_REDIRECT_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_REDIRECT](col_word_int_8h.md#group__words_1ga2c7094c849fb96cd773345ef34fa3e34)); \
    [WORD\_REDIRECT\_SOURCE](col_word_int_8h.md#group__redirect__words_1ga674cd2f9b410ff99a526a720bb20c981)(word) = (source);( word ,source )
```

Redirect word initializer.

**Parameters**:

* **word**: Word to initialize.
* **source**: [WORD\_REDIRECT\_SOURCE](col_word_int_8h.md#group__redirect__words_1ga674cd2f9b410ff99a526a720bb20c981).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_REDIRECT](col_word_int_8h.md#group__words_1ga2c7094c849fb96cd773345ef34fa3e34)



## Redirect Word Accessors

<a id="group__redirect__words_1ga674cd2f9b410ff99a526a720bb20c981"></a>
### Macro WORD\_REDIRECT\_SOURCE

![][public]

```cpp
#define WORD_REDIRECT_SOURCE ((([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(word))[1])( word )
```

Get/set new location of the word.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_REDIRECT\_INIT](col_word_int_8h.md#group__redirect__words_1gae3fd7eacc3ffa9eb2362d23144b9cb86)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)