<a id="group__minttrienode__words"></a>
# Integer Trie Nodes



Integer trie nodes are [Trie Nodes](group__mtrienode__words.md#group__mtrienode__words) nodes with integer-specific data.






**Requirements**:


* Integer trie node words, like generic [Trie Nodes](group__mtrienode__words.md#group__mtrienode__words), use one single cell and know their left and right subtries.

* Integer trie node words must know their critical bit, which is the highest bit where left and right subtrie keys differ. They store this information as a bitmask with this single bit set.


**Parameters**:

* **Mask**: Bitmask where only the critical bit is set.
* **Left**: [Generic trie node left subtrie field](col_trie_int_8h.md#group__mtrienode__words_1ga96a098f14e33a0759a9962e567084a0a).
* **Right**: [Generic trie node right subtrie field](col_trie_int_8h.md#group__mtrienode__words_1gaf58f69b75b276cb0b1dbdc54e94e31ad).


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        minttrienode_word [shape=none, label=<
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
                <td href="@ref WORD_INTTRIENODE_MASK" title="WORD_INTTRIENODE_MASK" colspan="4">Mask</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_TRIENODE_LEFT" title="WORD_TRIENODE_LEFT" colspan="4">Left (Generic)</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_TRIENODE_RIGHT" title="WORD_TRIENODE_RIGHT" colspan="4">Right (Generic)</td>
            </tr>
            </table>
        >]
    }
    









**See also**: [Immutable Integer Trie Nodes](group__inttrienode__words.md#group__inttrienode__words), [Integer Trie Maps](group__inttriemap__words.md#group__inttriemap__words)

## Submodules

* [Immutable Integer Trie Nodes](group__inttrienode__words.md#group__inttrienode__words)

## Integer Trie Node Creation

<a id="group__minttrienode__words_1gacb33bcfbca4f286c7a68fde0682b5804"></a>
### Macro WORD\_MINTTRIENODE\_INIT

![][public]

```cpp
#define WORD_MINTTRIENODE_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_MINTTRIENODE](col_word_int_8h.md#group__words_1gac0b62fa5b054dc6713a5fc7fd69298da)); \
    [WORD\_INTTRIENODE\_MASK](col_trie_int_8h.md#group__minttrienode__words_1ga010b15b770a0a79b1be365173a693ea3)(word) = (mask); \
    [WORD\_TRIENODE\_LEFT](col_trie_int_8h.md#group__mtrienode__words_1ga96a098f14e33a0759a9962e567084a0a)(word) = (left); \
    [WORD\_TRIENODE\_RIGHT](col_trie_int_8h.md#group__mtrienode__words_1gaf58f69b75b276cb0b1dbdc54e94e31ad)(word) = (right);( word ,mask ,left ,right )
```

Mutable integer trie node word initializer.

**Parameters**:

* **word**: Word to initialize.
* **mask**: [WORD\_INTTRIENODE\_MASK](col_trie_int_8h.md#group__minttrienode__words_1ga010b15b770a0a79b1be365173a693ea3).
* **left**: [WORD\_TRIENODE\_LEFT](col_trie_int_8h.md#group__mtrienode__words_1ga96a098f14e33a0759a9962e567084a0a).
* **right**: [WORD\_TRIENODE\_RIGHT](col_trie_int_8h.md#group__mtrienode__words_1gaf58f69b75b276cb0b1dbdc54e94e31ad).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_MINTTRIENODE](col_word_int_8h.md#group__words_1gac0b62fa5b054dc6713a5fc7fd69298da)



## Integer Trie Node Accessors

<a id="group__minttrienode__words_1ga010b15b770a0a79b1be365173a693ea3"></a>
### Macro WORD\_INTTRIENODE\_MASK

![][public]

```cpp
#define WORD_INTTRIENODE_MASK (((intptr_t *)(word))[1])( word )
```

Get/set bitmask where only the critical bit is set.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.




**See also**: [WORD\_INTTRIENODE\_INIT](col_trie_int_8h.md#group__inttrienode__words_1ga778e592ea5df7ecfdf2c16954c5e5a59), [WORD\_MINTTRIENODE\_INIT](col_trie_int_8h.md#group__minttrienode__words_1gacb33bcfbca4f286c7a68fde0682b5804)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)