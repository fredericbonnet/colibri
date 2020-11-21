<a id="group__mtrienode__words"></a>
# Trie Nodes



Generic trie nodes.





Trie maps store map entries hierarchically, in such a way that all entries in a subtree share a common prefix. This implementation uses crit-bit trees for keys: each node stores a critical bit, which is the index of the bit where both branches differ. Entries where this bit is cleared are stored in the left subtree, and those with this bit set are stored in the right subtree, all in a recursive manner.






**Requirements**:


* Trie node words use one single cell.

* Trie node words must know their critical bit, which is the highest bit of the first differing key element where left and right subtrie keys differ. They store this information as the index of the differing key element as well as the position of the critical bit.

* Crit-bit tries are binary trees and thus must know their left and right subtries.


**Parameters**:

* **Diff**: Index of first differing key element in left and right keys.
* **Bit**: Position of the critical bit in key element. 24-bit should be sufficient for most applications (this handles up to 16777216-bit/2MB-long key elements).
* **Left**: Left subtrie. Keys have their critical bit cleared.
* **Right**: Right subtrie. Keys have their critical bit set.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        mtrienode_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_TRIENODE_BIT" title="WORD_TRIENODE_BIT" colspan="2">Bit</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_TRIENODE_DIFF" title="WORD_TRIENODE_DIFF" colspan="5">Diff</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_TRIENODE_LEFT" title="WORD_TRIENODE_LEFT" colspan="5">Left</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_TRIENODE_RIGHT" title="WORD_TRIENODE_RIGHT" colspan="5">Right</td>
            </tr>
            </table>
        >]
    }
    









**See also**: [Trie Nodes](group__mtrienode__words.md#group__mtrienode__words), [Trie Maps](group__triemap__words.md#group__triemap__words)

## Submodules

* [Immutable Trie Nodes](group__trienode__words.md#group__trienode__words)

## Trie Node Creation

<a id="group__mtrienode__words_1ga2824a0cd8faf2335bb228fdfd43be612"></a>
### Macro WORD\_MTRIENODE\_INIT

![][public]

```cpp
#define WORD_MTRIENODE_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_MTRIENODE](col_word_int_8h.md#group__words_1ga20d128dd2702743f2027be54817f2275)); \
    [WORD\_TRIENODE\_SET\_BIT](col_trie_int_8h.md#group__mtrienode__words_1ga4b49795cbbe35d01e5dd00f97eb98004)((word), (bit)); \
    [WORD\_TRIENODE\_DIFF](col_trie_int_8h.md#group__mtrienode__words_1ga4e2885eb03b3f4575cb80e7e84ed1b7b)(word) = (diff); \
    [WORD\_TRIENODE\_LEFT](col_trie_int_8h.md#group__mtrienode__words_1ga96a098f14e33a0759a9962e567084a0a)(word) = (left); \
    [WORD\_TRIENODE\_RIGHT](col_trie_int_8h.md#group__mtrienode__words_1gaf58f69b75b276cb0b1dbdc54e94e31ad)(word) = (right);( word ,diff ,bit ,left ,right )
```

Mutable trie node initializer.

**Parameters**:

* **word**: Word to initialize.
* **diff**: [WORD\_TRIENODE\_DIFF](col_trie_int_8h.md#group__mtrienode__words_1ga4e2885eb03b3f4575cb80e7e84ed1b7b).
* **bit**: [WORD\_TRIENODE\_SET\_BIT](col_trie_int_8h.md#group__mtrienode__words_1ga4b49795cbbe35d01e5dd00f97eb98004).
* **left**: [WORD\_TRIENODE\_LEFT](col_trie_int_8h.md#group__mtrienode__words_1ga96a098f14e33a0759a9962e567084a0a).
* **right**: [WORD\_TRIENODE\_RIGHT](col_trie_int_8h.md#group__mtrienode__words_1gaf58f69b75b276cb0b1dbdc54e94e31ad).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_MTRIENODE](col_word_int_8h.md#group__words_1ga20d128dd2702743f2027be54817f2275)



## Trie Node Accessors

<a id="group__mtrienode__words_1ga0f92a981e564bc1571f652022029528e"></a>
### Macro TRIENODE\_MASK

![][public]

```cpp
#define TRIENODE_MASK (UINT_MAX>>CHAR_BIT)
```

Bitmask for critical bit.





<a id="group__mtrienode__words_1gaf6b550343f8cf97aaf951a68640695ec"></a>
### Macro WORD\_TRIENODE\_BIT

![][public]

```cpp
#define WORD_TRIENODE_BIT (((([Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) *)(word))[0])&[TRIENODE\_MASK](col_trie_int_8h.md#group__mtrienode__words_1ga0f92a981e564bc1571f652022029528e))( word )
```

Get critical bit position.

**Parameters**:

* **word**: Word to access.



**See also**: [WORD\_TRIENODE\_SET\_BIT](col_trie_int_8h.md#group__mtrienode__words_1ga4b49795cbbe35d01e5dd00f97eb98004)



<a id="group__mtrienode__words_1ga4b49795cbbe35d01e5dd00f97eb98004"></a>
### Macro WORD\_TRIENODE\_SET\_BIT

![][public]

```cpp
#define WORD_TRIENODE_SET_BIT (((([Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) *)(word))[0])&=~[TRIENODE\_MASK](col_trie_int_8h.md#group__mtrienode__words_1ga0f92a981e564bc1571f652022029528e),((([Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) *)(word))[0])|=((bit)&[TRIENODE\_MASK](col_trie_int_8h.md#group__mtrienode__words_1ga0f92a981e564bc1571f652022029528e)))( word ,bit )
```

Set critical bit position.

**Parameters**:

* **word**: Word to access.
* **bit**: Bit value. Only retains the low order bits.


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.





**See also**: [WORD\_TRIENODE\_BIT](col_trie_int_8h.md#group__mtrienode__words_1gaf6b550343f8cf97aaf951a68640695ec), [WORD\_TRIENODE\_INIT](col_trie_int_8h.md#group__trienode__words_1gaa2668e0b8ee0e48846bd5816f8b817d6), [WORD\_MTRIENODE\_INIT](col_trie_int_8h.md#group__mtrienode__words_1ga2824a0cd8faf2335bb228fdfd43be612)



<a id="group__mtrienode__words_1ga4e2885eb03b3f4575cb80e7e84ed1b7b"></a>
### Macro WORD\_TRIENODE\_DIFF

![][public]

```cpp
#define WORD_TRIENODE_DIFF (((size_t *)(word))[1])( word )
```

Get/set index of first differing key element in left and right keys.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.




**See also**: [WORD\_TRIENODE\_INIT](col_trie_int_8h.md#group__trienode__words_1gaa2668e0b8ee0e48846bd5816f8b817d6), [WORD\_MTRIENODE\_INIT](col_trie_int_8h.md#group__mtrienode__words_1ga2824a0cd8faf2335bb228fdfd43be612)



<a id="group__mtrienode__words_1ga96a098f14e33a0759a9962e567084a0a"></a>
### Macro WORD\_TRIENODE\_LEFT

![][public]

```cpp
#define WORD_TRIENODE_LEFT ((([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(word))[2])( word )
```

Get/set left subtrie.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.




**See also**: [WORD\_TRIENODE\_INIT](col_trie_int_8h.md#group__trienode__words_1gaa2668e0b8ee0e48846bd5816f8b817d6), [WORD\_MTRIENODE\_INIT](col_trie_int_8h.md#group__mtrienode__words_1ga2824a0cd8faf2335bb228fdfd43be612)



<a id="group__mtrienode__words_1gaf58f69b75b276cb0b1dbdc54e94e31ad"></a>
### Macro WORD\_TRIENODE\_RIGHT

![][public]

```cpp
#define WORD_TRIENODE_RIGHT ((([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(word))[3])( word )
```

Get/set right subtrie.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.




**See also**: [WORD\_TRIENODE\_INIT](col_trie_int_8h.md#group__trienode__words_1gaa2668e0b8ee0e48846bd5816f8b817d6), [WORD\_MTRIENODE\_INIT](col_trie_int_8h.md#group__mtrienode__words_1ga2824a0cd8faf2335bb228fdfd43be612)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)