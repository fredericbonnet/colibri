<a id="group__mstrtrienode__words"></a>
# String Trie Nodes



String trie nodes are [Trie Nodes](group__mtrienode__words.md#group__mtrienode__words) nodes with string-specific data.






**Requirements**:


* String trie node words, like generic [Trie Nodes](group__mtrienode__words.md#group__mtrienode__words), use one single cell and know their left and right subtries.

* String trie node words must know their critical bit, which is the highest bit of the first differing character where left and right subtrie keys differ. They store this information as the index of the differing character as well as a bit mask with this single bit set.


**Parameters**:

* **Diff**: Index of first differing character in left and right keys.
* **Mask**: Bitmask where only the critical bit is set. 24-bit is sufficient for Unicode character codepoints.
* **Left**: [Generic trie node left subtrie field](col_trie_int_8h.md#group__mtrienode__words_1ga96a098f14e33a0759a9962e567084a0a).
* **Right**: [Generic trie node right subtrie field](col_trie_int_8h.md#group__mtrienode__words_1gaf58f69b75b276cb0b1dbdc54e94e31ad).


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
                <td href="@ref WORD_STRTRIENODE_MASK" title="WORD_STRTRIENODE_MASK" colspan="2">Mask</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_STRTRIENODE_DIFF" title="WORD_STRTRIENODE_DIFF" colspan="5">Diff</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_TRIENODE_LEFT" title="WORD_TRIENODE_LEFT" colspan="5">Left (Generic)</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_TRIENODE_RIGHT" title="WORD_TRIENODE_RIGHT" colspan="5">Right (Generic)</td>
            </tr>
            </table>
        >]
    }
    









**See also**: [String Trie Nodes](group__mstrtrienode__words.md#group__mstrtrienode__words), [String Trie Maps](group__strtriemap__words.md#group__strtriemap__words)

## Submodules

* [Immutable String Trie Nodes](group__strtrienode__words.md#group__strtrienode__words)

## String Trie Node Creation

<a id="group__mstrtrienode__words_1gadb6e244e2369d6711272317f897e0418"></a>
### Macro WORD\_MSTRTRIENODE\_INIT

![][public]

```cpp
#define WORD_MSTRTRIENODE_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_MSTRTRIENODE](col_word_int_8h.md#group__words_1ga82e8b6ff4abcd49f5ffa67cc45b17358)); \
    [WORD\_STRTRIENODE\_SET\_MASK](col_trie_int_8h.md#group__mstrtrienode__words_1ga8a91d66561940fb9ee33506dec11f4b8)((word), (mask)); \
    [WORD\_STRTRIENODE\_DIFF](col_trie_int_8h.md#group__mstrtrienode__words_1ga0b7f928792baf650225912ff4f126815)(word) = (diff); \
    [WORD\_TRIENODE\_LEFT](col_trie_int_8h.md#group__mtrienode__words_1ga96a098f14e33a0759a9962e567084a0a)(word) = (left); \
    [WORD\_TRIENODE\_RIGHT](col_trie_int_8h.md#group__mtrienode__words_1gaf58f69b75b276cb0b1dbdc54e94e31ad)(word) = (right);( word ,diff ,mask ,left ,right )
```

Mutable string trie node word initializer.

**Parameters**:

* **word**: Word to initialize.
* **diff**: [WORD\_STRTRIENODE\_DIFF](col_trie_int_8h.md#group__mstrtrienode__words_1ga0b7f928792baf650225912ff4f126815).
* **mask**: [WORD\_STRTRIENODE\_SET\_MASK](col_trie_int_8h.md#group__mstrtrienode__words_1ga8a91d66561940fb9ee33506dec11f4b8).
* **left**: [WORD\_TRIENODE\_LEFT](col_trie_int_8h.md#group__mtrienode__words_1ga96a098f14e33a0759a9962e567084a0a).
* **right**: [WORD\_TRIENODE\_RIGHT](col_trie_int_8h.md#group__mtrienode__words_1gaf58f69b75b276cb0b1dbdc54e94e31ad).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_MSTRTRIENODE](col_word_int_8h.md#group__words_1ga82e8b6ff4abcd49f5ffa67cc45b17358)



## String Trie Node Accessors

<a id="group__mstrtrienode__words_1gac85bca7104d1e247f4271262079067ce"></a>
### Macro STRTRIENODE\_MASK

![][public]

```cpp
#define STRTRIENODE_MASK (UINT_MAX>>CHAR_BIT)
```

Bitmask for critical bit.





<a id="group__mstrtrienode__words_1ga2b0f59fcf11f10eba52074596a0fd8c9"></a>
### Macro WORD\_STRTRIENODE\_MASK

![][public]

```cpp
#define WORD_STRTRIENODE_MASK (((([Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) *)(word))[0])&[STRTRIENODE\_MASK](col_trie_int_8h.md#group__mstrtrienode__words_1gac85bca7104d1e247f4271262079067ce))( word )
```

Get bitmask where only the critical bit is set.

**Parameters**:

* **word**: Word to access.



**See also**: [WORD\_STRTRIENODE\_SET\_MASK](col_trie_int_8h.md#group__mstrtrienode__words_1ga8a91d66561940fb9ee33506dec11f4b8)



<a id="group__mstrtrienode__words_1ga8a91d66561940fb9ee33506dec11f4b8"></a>
### Macro WORD\_STRTRIENODE\_SET\_MASK

![][public]

```cpp
#define WORD_STRTRIENODE_SET_MASK (((([Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) *)(word))[0])&=~[STRTRIENODE\_MASK](col_trie_int_8h.md#group__mstrtrienode__words_1gac85bca7104d1e247f4271262079067ce),((([Col\_Char](colibri_8h.md#group__strings_1gab42ee0cd75b78280e412fa5bae5eb862) *)(word))[0])|=((size)&[STRTRIENODE\_MASK](col_trie_int_8h.md#group__mstrtrienode__words_1gac85bca7104d1e247f4271262079067ce)))( word ,mask )
```

Set bitmask to the critical bit.

**Parameters**:

* **word**: Word to access.
* **mask**: Mask value.


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.





**See also**: [WORD\_STRTRIENODE\_MASK](col_trie_int_8h.md#group__mstrtrienode__words_1ga2b0f59fcf11f10eba52074596a0fd8c9), [WORD\_STRTRIENODE\_INIT](col_trie_int_8h.md#group__strtrienode__words_1gaa99e32f6881a4f95c95f59d5cc1586db), [WORD\_MSTRTRIENODE\_INIT](col_trie_int_8h.md#group__mstrtrienode__words_1gadb6e244e2369d6711272317f897e0418)



<a id="group__mstrtrienode__words_1ga0b7f928792baf650225912ff4f126815"></a>
### Macro WORD\_STRTRIENODE\_DIFF

![][public]

```cpp
#define WORD_STRTRIENODE_DIFF (((size_t *)(word))[1])( word )
```

Get/set index of first differing character in left and right keys.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.




**See also**: [WORD\_TRIENODE\_INIT](col_trie_int_8h.md#group__trienode__words_1gaa2668e0b8ee0e48846bd5816f8b817d6), [WORD\_MTRIENODE\_INIT](col_trie_int_8h.md#group__mtrienode__words_1ga2824a0cd8faf2335bb228fdfd43be612)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)