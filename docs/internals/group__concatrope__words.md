<a id="group__concatrope__words"></a>
# Concat Ropes



Concat ropes are balanced binary trees concatenating left and right subropes. The tree is balanced by construction (see [Rope Tree Balancing](#group__rope__words_1rope_tree_balancing)).






**Requirements**:


* Concat rope words use one single cell.

* Concat ropes must know their left and right subrope words.

* Concat ropes must know their length, which is the sum of their left and right arms. To save full recursive subtree traversal this length is stored at each level in the concat words. We also cache the left arm's length whenever possible to save a pointer dereference (the right length being the total minus left lengths).

* Last, concat ropes must know their depth for tree balancing (see [Rope Tree Balancing](#group__rope__words_1rope_tree_balancing)). This is the max depth of their left and right arms, plus one.


**Parameters**:

* **Depth**: Depth of concat rope. 8 bits will code up to 255 depth levels, which is more than sufficient for balanced binary trees.
* **Left**: Used as shortcut to avoid dereferencing the left arm. Zero if actual length is too large to fit.
* **Length**: Rope length, which is the sum of both arms'.
* **Left**: Left concatenated rope.
* **Right**: Right concatenated rope.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        concatrope_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_CONCATROPE_DEPTH" title="WORD_CONCATROPE_DEPTH" colspan="2">Depth</td>
                <td href="@ref WORD_CONCATROPE_LEFT_LENGTH" title="WORD_CONCATROPE_LEFT_LENGTH" colspan="2">Left length</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_CONCATROPE_LENGTH" title="WORD_CONCATROPE_LENGTH" colspan="7">Length</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_CONCATROPE_LEFT" title="WORD_CONCATROPE_LEFT" colspan="7">Left</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_CONCATROPE_RIGHT" title="WORD_CONCATROPE_RIGHT" colspan="7">Right</td>
            </tr>
            </table>
        >]
    }
    








**See also**: [WORD\_TYPE\_CONCATROPE](col_word_int_8h.md#group__words_1ga677525993a1e4a934e98042c53021c3a)

## Concat Rope Creation

<a id="group__concatrope__words_1ga6f8d139b77fb5ab7a1e4f4f3afc6ceec"></a>
### Macro WORD\_CONCATROPE\_INIT

![][public]

```cpp
#define WORD_CONCATROPE_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_CONCATROPE](col_word_int_8h.md#group__words_1ga677525993a1e4a934e98042c53021c3a)); \
    [WORD\_CONCATROPE\_DEPTH](col_rope_int_8h.md#group__concatrope__words_1gaf74129b430f17d52c86ee7d4e317fa5b)(word) = (uint8_t) (depth); \
    [WORD\_CONCATROPE\_LENGTH](col_rope_int_8h.md#group__concatrope__words_1ga32f67b8d198ec4a90b371f0c51723bf5)(word) = (length); \
    [WORD\_CONCATROPE\_LEFT\_LENGTH](col_rope_int_8h.md#group__concatrope__words_1ga1a1e048ea8091a88ced0ead88242b5b4)(word) = (uint16_t) ((leftLength)>UINT16_MAX?0:(leftLength)); \
    [WORD\_CONCATROPE\_LEFT](col_rope_int_8h.md#group__concatrope__words_1ga2af5901b1cf501c31b3f5593332bb68f)(word) = (left); \
    [WORD\_CONCATROPE\_RIGHT](col_rope_int_8h.md#group__concatrope__words_1ga587823d297e401d584898411bc44d8d3)(word) = (right);( word ,depth ,length ,leftLength ,left ,right )
```

Concat rope word initializer.

**Parameters**:

* **word**: Word to initialize.
* **depth**: [WORD\_CONCATROPE\_DEPTH](col_rope_int_8h.md#group__concatrope__words_1gaf74129b430f17d52c86ee7d4e317fa5b).
* **length**: [WORD\_CONCATROPE\_LENGTH](col_rope_int_8h.md#group__concatrope__words_1ga32f67b8d198ec4a90b371f0c51723bf5).
* **leftLength**: [WORD\_CONCATROPE\_LEFT\_LENGTH](col_rope_int_8h.md#group__concatrope__words_1ga1a1e048ea8091a88ced0ead88242b5b4).
* **left**: [WORD\_CONCATROPE\_LEFT](col_rope_int_8h.md#group__concatrope__words_1ga2af5901b1cf501c31b3f5593332bb68f).
* **right**: [WORD\_CONCATROPE\_RIGHT](col_rope_int_8h.md#group__concatrope__words_1ga587823d297e401d584898411bc44d8d3).


!> **Warning** \
Arguments **word** and **leftLength** are referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_CONCATROPE](col_word_int_8h.md#group__words_1ga677525993a1e4a934e98042c53021c3a)



## Concat Rope Accessors

<a id="group__concatrope__words_1gaf74129b430f17d52c86ee7d4e317fa5b"></a>
### Macro WORD\_CONCATROPE\_DEPTH

![][public]

```cpp
#define WORD_CONCATROPE_DEPTH (((uint8_t *)(word))[1])( word )
```

Get/set depth of concat rope.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_CONCATROPE\_INIT](col_rope_int_8h.md#group__concatrope__words_1ga6f8d139b77fb5ab7a1e4f4f3afc6ceec)



<a id="group__concatrope__words_1ga1a1e048ea8091a88ced0ead88242b5b4"></a>
### Macro WORD\_CONCATROPE\_LEFT\_LENGTH

![][public]

```cpp
#define WORD_CONCATROPE_LEFT_LENGTH (((uint16_t *)(word))[1])( word )
```

Get/set left arm's length (zero when too large).

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_CONCATROPE\_INIT](col_rope_int_8h.md#group__concatrope__words_1ga6f8d139b77fb5ab7a1e4f4f3afc6ceec)



<a id="group__concatrope__words_1ga32f67b8d198ec4a90b371f0c51723bf5"></a>
### Macro WORD\_CONCATROPE\_LENGTH

![][public]

```cpp
#define WORD_CONCATROPE_LENGTH (((size_t *)(word))[1])( word )
```

Get/set rope length.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_CONCATROPE\_INIT](col_rope_int_8h.md#group__concatrope__words_1ga6f8d139b77fb5ab7a1e4f4f3afc6ceec)



<a id="group__concatrope__words_1ga2af5901b1cf501c31b3f5593332bb68f"></a>
### Macro WORD\_CONCATROPE\_LEFT

![][public]

```cpp
#define WORD_CONCATROPE_LEFT ((([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(word))[2])( word )
```

Get/set left concatenated rope.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_CONCATROPE\_INIT](col_rope_int_8h.md#group__concatrope__words_1ga6f8d139b77fb5ab7a1e4f4f3afc6ceec)



<a id="group__concatrope__words_1ga587823d297e401d584898411bc44d8d3"></a>
### Macro WORD\_CONCATROPE\_RIGHT

![][public]

```cpp
#define WORD_CONCATROPE_RIGHT ((([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(word))[3])( word )
```

Get/set right concatenated rope.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_CONCATROPE\_INIT](col_rope_int_8h.md#group__concatrope__words_1ga6f8d139b77fb5ab7a1e4f4f3afc6ceec)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)