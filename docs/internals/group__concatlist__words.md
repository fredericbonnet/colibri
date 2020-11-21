<a id="group__concatlist__words"></a>
# Immutable Concat Lists



Immutable concat lists are balanced binary trees concatenating left and right sublists. The tree is balanced by construction (see [List Tree Balancing](#group__list__words_1list_tree_balancing)).






**Requirements**:


* Concat list words use one single cell.

* Concat lists must know their left and right sublist words.

* Concat lists must know their length, which is the sum of their left and right arms. To save full recursive subtree traversal this length is stored at each level in the concat words. We also cache the left arm's length whenever possible to save a pointer dereference (the right length being the total minus left lengths).

* Last, concat lists must know their depth for tree balancing (see [List Tree Balancing](#group__list__words_1list_tree_balancing)). This is the max depth of their left and right arms, plus one.


**Parameters**:

* **Depth**: Depth of concat list. 8 bits will code up to 255 depth levels, which is more than sufficient for balanced binary trees.
* **Left_Length**: Used as shortcut to avoid dereferencing the left arm. Zero if actual length is too large to fit.
* **Length**: List length, which is the sum of both arms'.
* **Left**: Left concatenated list.
* **Right**: Right concatenated list.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        concatlist_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_CONCATLIST_DEPTH" title="WORD_CONCATLIST_DEPTH" colspan="2">Depth</td>
                <td href="@ref WORD_CONCATLIST_LEFT_LENGTH" title="WORD_CONCATLIST_LEFT_LENGTH" colspan="2">Left length</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_CONCATLIST_LENGTH" title="WORD_CONCATLIST_LENGTH" colspan="7">Length</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_CONCATLIST_LEFT" title="WORD_CONCATLIST_LEFT" colspan="7">Left</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_CONCATLIST_RIGHT" title="WORD_CONCATLIST_RIGHT" colspan="7">Right</td>
            </tr>
            </table>
        >]
    }
    








**See also**: [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)

## Submodules

* [Mutable Concat Lists](group__mconcatlist__words.md#group__mconcatlist__words)

## Immutable Concat List Creation

<a id="group__concatlist__words_1ga8cef632de8ecc768d7926efa76a5d550"></a>
### Macro WORD\_CONCATLIST\_INIT

![][public]

```cpp
#define WORD_CONCATLIST_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)); \
    [WORD\_CONCATLIST\_DEPTH](col_list_int_8h.md#group__concatlist__words_1ga6e5dad2a2ae231641e9c8bc89debb338)(word) = (uint8_t) (depth); \
    [WORD\_CONCATLIST\_LENGTH](col_list_int_8h.md#group__concatlist__words_1ga005eedea923258322288f51292f33782)(word) = (length); \
    [WORD\_CONCATLIST\_LEFT\_LENGTH](col_list_int_8h.md#group__concatlist__words_1gab757f4a3117b72ba7ceab9202cf2c2f4)(word) = (uint16_t) ((leftLength)>UINT16_MAX?0:(leftLength)); \
    [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)(word) = (left); \
    [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)(word) = (right);( word ,depth ,length ,leftLength ,left ,right )
```

Immutable concat list word initializer.

**Parameters**:

* **word**: Word to initialize.
* **depth**: [WORD\_CONCATLIST\_DEPTH](col_list_int_8h.md#group__concatlist__words_1ga6e5dad2a2ae231641e9c8bc89debb338).
* **length**: [WORD\_CONCATLIST\_LENGTH](col_list_int_8h.md#group__concatlist__words_1ga005eedea923258322288f51292f33782).
* **leftLength**: [WORD\_CONCATLIST\_LEFT\_LENGTH](col_list_int_8h.md#group__concatlist__words_1gab757f4a3117b72ba7ceab9202cf2c2f4).
* **left**: [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3).
* **right**: [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e).


!> **Warning** \
Arguments **word** and **leftLength** are referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)



## Immutable Concat List Accessors

<a id="group__concatlist__words_1ga6e5dad2a2ae231641e9c8bc89debb338"></a>
### Macro WORD\_CONCATLIST\_DEPTH

![][public]

```cpp
#define WORD_CONCATLIST_DEPTH (((uint8_t *)(word))[1])( word )
```

Get/set depth of concat list.

Used by both mutable and immutable versions.






**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.




**See also**: [WORD\_CONCATLIST\_INIT](col_list_int_8h.md#group__concatlist__words_1ga8cef632de8ecc768d7926efa76a5d550), [WORD\_MCONCATLIST\_INIT](col_list_int_8h.md#group__mconcatlist__words_1ga3c52f1fb2dc05f7d3822519d7be9ee7b)



<a id="group__concatlist__words_1gab757f4a3117b72ba7ceab9202cf2c2f4"></a>
### Macro WORD\_CONCATLIST\_LEFT\_LENGTH

![][public]

```cpp
#define WORD_CONCATLIST_LEFT_LENGTH (((uint16_t *)(word))[1])( word )
```

Get/set left arm's length (zero when too large).

Used by both mutable and immutable versions.






**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.




**See also**: [WORD\_CONCATLIST\_INIT](col_list_int_8h.md#group__concatlist__words_1ga8cef632de8ecc768d7926efa76a5d550), [WORD\_MCONCATLIST\_INIT](col_list_int_8h.md#group__mconcatlist__words_1ga3c52f1fb2dc05f7d3822519d7be9ee7b)



<a id="group__concatlist__words_1ga005eedea923258322288f51292f33782"></a>
### Macro WORD\_CONCATLIST\_LENGTH

![][public]

```cpp
#define WORD_CONCATLIST_LENGTH (((size_t *)(word))[1])( word )
```

Get/set list length.

Used by both mutable and immutable versions.






**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.




**See also**: [WORD\_CONCATLIST\_INIT](col_list_int_8h.md#group__concatlist__words_1ga8cef632de8ecc768d7926efa76a5d550), [WORD\_MCONCATLIST\_INIT](col_list_int_8h.md#group__mconcatlist__words_1ga3c52f1fb2dc05f7d3822519d7be9ee7b)



<a id="group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3"></a>
### Macro WORD\_CONCATLIST\_LEFT

![][public]

```cpp
#define WORD_CONCATLIST_LEFT ((([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(word))[2])( word )
```

Get/set left concatenated list.

**Parameters**:

* **word**: Word to access.

Used by both mutable and immutable versions.






?> Macro is L-Value and suitable for both read/write operations.




**See also**: [WORD\_CONCATLIST\_INIT](col_list_int_8h.md#group__concatlist__words_1ga8cef632de8ecc768d7926efa76a5d550), [WORD\_MCONCATLIST\_INIT](col_list_int_8h.md#group__mconcatlist__words_1ga3c52f1fb2dc05f7d3822519d7be9ee7b)



<a id="group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e"></a>
### Macro WORD\_CONCATLIST\_RIGHT

![][public]

```cpp
#define WORD_CONCATLIST_RIGHT ((([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(word))[3])( word )
```

Get/set right concatenated list.

**Parameters**:

* **word**: Word to access.

Used by both mutable and immutable versions.






?> Macro is L-Value and suitable for both read/write operations.




**See also**: [WORD\_CONCATLIST\_INIT](col_list_int_8h.md#group__concatlist__words_1ga8cef632de8ecc768d7926efa76a5d550), [WORD\_MCONCATLIST\_INIT](col_list_int_8h.md#group__mconcatlist__words_1ga3c52f1fb2dc05f7d3822519d7be9ee7b)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)