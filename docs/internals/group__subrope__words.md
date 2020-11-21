<a id="group__subrope__words"></a>
# Subropes



Subropes are immutable ropes pointing to a slice of a larger rope.





Extraction of a subrope from a large rope can be expensive. For example, removing one character from a string of several thousands implies copying the whole data minus this element. Likewise, extracting a range of characters from a complex binary tree of rope nodes implies traversing and copying large parts of this subtree.





To limit the memory and processing costs of rope slicing, the subrope word simply points to the original rope and the index range of characters in the slice. This representation is preferred over raw copy when the source rope is large.






**Requirements**:


* Subrope words use one single cell.

* Subropes must know their source rope word.

* Subropes must also know the indices of the first and last characters in the source rope that belongs to the slice. These indices are inclusive. Difference plus one gives the subrope length.

* Last, subropes must know their depth for tree balancing (see [Rope Tree Balancing](#group__rope__words_1rope_tree_balancing)). This is the same as the source depth, but to avoid one pointer dereference we cache the value in the word.


**Parameters**:

* **Depth**: Depth of subrope. 8 bits will code up to 255 depth levels, which is more than sufficient for balanced binary trees.
* **Source**: Rope of which this one is a subrope.
* **First**: First character in source.
* **Last**: Last character in source.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        subrope_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_SUBROPE_DEPTH" title="WORD_SUBROPE_DEPTH" colspan="2">Depth</td>
                <td colspan="2" bgcolor="grey75">Unused</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_SUBROPE_SOURCE" title="WORD_SUBROPE_SOURCE" colspan="6">Source</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_SUBROPE_FIRST" title="WORD_SUBROPE_FIRST" colspan="6">First</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_SUBROPE_LAST" title="WORD_SUBROPE_LAST" colspan="6">Last</td>
            </tr>
            </table>
        >]
    }
    








**See also**: [WORD\_TYPE\_SUBROPE](col_word_int_8h.md#group__words_1gaadd34ba690f1e2711ee1b6ae965fd8e7)

## Subrope Creation

<a id="group__subrope__words_1ga7515ed32b8448be528c6f71c73ad7b8d"></a>
### Macro WORD\_SUBROPE\_INIT

![][public]

```cpp
#define WORD_SUBROPE_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_SUBROPE](col_word_int_8h.md#group__words_1gaadd34ba690f1e2711ee1b6ae965fd8e7)); \
    [WORD\_SUBROPE\_DEPTH](col_rope_int_8h.md#group__subrope__words_1ga79860a542687768ee7965f5c8bd86b2a)(word) = (uint8_t) (depth); \
    [WORD\_SUBROPE\_SOURCE](col_rope_int_8h.md#group__subrope__words_1ga78e40e3bc8f132eb513198bffd78b644)(word) = (source); \
    [WORD\_SUBROPE\_FIRST](col_rope_int_8h.md#group__subrope__words_1ga6db22689c9aec5c74206bddd98abad70)(word) = (first); \
    [WORD\_SUBROPE\_LAST](col_rope_int_8h.md#group__subrope__words_1ga72a89875578dffdc0ce1029b078ae2c9)(word) = (last);( word ,depth ,source ,first ,last )
```

Subrope word initializer.

**Parameters**:

* **word**: Word to initialize.
* **depth**: [WORD\_SUBROPE\_DEPTH](col_rope_int_8h.md#group__subrope__words_1ga79860a542687768ee7965f5c8bd86b2a).
* **source**: [WORD\_SUBROPE\_SOURCE](col_rope_int_8h.md#group__subrope__words_1ga78e40e3bc8f132eb513198bffd78b644).
* **first**: [WORD\_SUBROPE\_FIRST](col_rope_int_8h.md#group__subrope__words_1ga6db22689c9aec5c74206bddd98abad70).
* **last**: [WORD\_SUBROPE\_LAST](col_rope_int_8h.md#group__subrope__words_1ga72a89875578dffdc0ce1029b078ae2c9).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_SUBROPE](col_word_int_8h.md#group__words_1gaadd34ba690f1e2711ee1b6ae965fd8e7)



## Subrope Accessors

<a id="group__subrope__words_1ga79860a542687768ee7965f5c8bd86b2a"></a>
### Macro WORD\_SUBROPE\_DEPTH

![][public]

```cpp
#define WORD_SUBROPE_DEPTH (((uint8_t *)(word))[1])( word )
```

Get/set depth of subrope.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_SUBROPE\_INIT](col_rope_int_8h.md#group__subrope__words_1ga7515ed32b8448be528c6f71c73ad7b8d)



<a id="group__subrope__words_1ga78e40e3bc8f132eb513198bffd78b644"></a>
### Macro WORD\_SUBROPE\_SOURCE

![][public]

```cpp
#define WORD_SUBROPE_SOURCE ((([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(word))[1])( word )
```

Get/set rope from which this one is extracted.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_SUBROPE\_INIT](col_rope_int_8h.md#group__subrope__words_1ga7515ed32b8448be528c6f71c73ad7b8d)



<a id="group__subrope__words_1ga6db22689c9aec5c74206bddd98abad70"></a>
### Macro WORD\_SUBROPE\_FIRST

![][public]

```cpp
#define WORD_SUBROPE_FIRST (((size_t *)(word))[2])( word )
```

Get/set index of first character in source.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_SUBROPE\_INIT](col_rope_int_8h.md#group__subrope__words_1ga7515ed32b8448be528c6f71c73ad7b8d)



<a id="group__subrope__words_1ga72a89875578dffdc0ce1029b078ae2c9"></a>
### Macro WORD\_SUBROPE\_LAST

![][public]

```cpp
#define WORD_SUBROPE_LAST (((size_t *)(word))[3])( word )
```

Get/set index of last character in source.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_SUBROPE\_INIT](col_rope_int_8h.md#group__subrope__words_1ga7515ed32b8448be528c6f71c73ad7b8d)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)