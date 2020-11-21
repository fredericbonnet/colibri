<a id="group__mconcatlist__words"></a>
# Mutable Concat Lists



Mutable concat lists are identical to their immutable counterpart with the following exceptions:






* The content of mutable concat nodes can change over time, whereas immutable ones are fixed at creation time.

* While immutable concat nodes can only contain immutable subnodes, mutable concat nodes can contain both immutable and mutable subnodes; immutable ones are converted to mutable using copy-on-write semantics during mutable operations; mutable ones are frozen and turned immutable when they become shared during immutable operations.

* Balancing rules are identical (see [List Tree Balancing](#group__list__words_1list_tree_balancing)) but while immutable concat nodes must be copied, mutable nodes can be modified in-place.








**Requirements**:


* Mutable concat lists use the same basic structure as immutable concat lists so that they can be turned immutable in-place ([frozen](col_list_8c.md#group__mlist__words_1ga095c6e932019c53fc81e4beaddf0334b)).


**Parameters**:

* **Depth**: [Generic immutable concat list depth field](col_list_int_8h.md#group__concatlist__words_1ga6e5dad2a2ae231641e9c8bc89debb338).
* **Left_Length**: [Generic immutable concat list left length field](col_list_int_8h.md#group__concatlist__words_1gab757f4a3117b72ba7ceab9202cf2c2f4).
* **Length**: [Generic immutable concat list length field](col_list_int_8h.md#group__concatlist__words_1ga005eedea923258322288f51292f33782).
* **Left**: [Generic immutable concat list left field](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3).
* **Right**: [Generic immutable concat list right field](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e).


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
    









**See also**: [Immutable Concat Lists](group__concatlist__words.md#group__concatlist__words), [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)

## Mutable Concat List Creation

<a id="group__mconcatlist__words_1ga3c52f1fb2dc05f7d3822519d7be9ee7b"></a>
### Macro WORD\_MCONCATLIST\_INIT

![][public]

```cpp
#define WORD_MCONCATLIST_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)); \
    [WORD\_CONCATLIST\_DEPTH](col_list_int_8h.md#group__concatlist__words_1ga6e5dad2a2ae231641e9c8bc89debb338)(word) = (uint8_t) (depth); \
    [WORD\_CONCATLIST\_LENGTH](col_list_int_8h.md#group__concatlist__words_1ga005eedea923258322288f51292f33782)(word) = (length); \
    [WORD\_CONCATLIST\_LEFT\_LENGTH](col_list_int_8h.md#group__concatlist__words_1gab757f4a3117b72ba7ceab9202cf2c2f4)(word) = (uint16_t) ((leftLength)>UINT16_MAX?0:(leftLength)); \
    [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)(word) = (left); \
    [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)(word) = (right);( word ,depth ,length ,leftLength ,left ,right )
```

Mutable concat list word initializer.

**Parameters**:

* **word**: Word to initialize.
* **depth**: [WORD\_CONCATLIST\_DEPTH](col_list_int_8h.md#group__concatlist__words_1ga6e5dad2a2ae231641e9c8bc89debb338).
* **length**: [WORD\_CONCATLIST\_LENGTH](col_list_int_8h.md#group__concatlist__words_1ga005eedea923258322288f51292f33782).
* **leftLength**: [WORD\_CONCATLIST\_LEFT\_LENGTH](col_list_int_8h.md#group__concatlist__words_1gab757f4a3117b72ba7ceab9202cf2c2f4).
* **left**: [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3).
* **right**: [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e).


!> **Warning** \
Arguments **word** and **leftLength** are referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)