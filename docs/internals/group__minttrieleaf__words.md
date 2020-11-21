<a id="group__minttrieleaf__words"></a>
# Integer Trie Leaves



Trie leaves are [Integer Map Entries](group__intmapentry__words.md#group__intmapentry__words) with trie-specific data.






**Requirements**:


* Trie leave words, like generic [Integer Map Entries](group__intmapentry__words.md#group__intmapentry__words), use one single cell.

* They need no specific info besides generic map entry fields.


**Parameters**:

* **Key**: [Generic integer map entry key field](col_map_int_8h.md#group__intmapentry__words_1ga89e26360d76aaad985afd89da56d1539).
* **Value**: [Generic map entry value field](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064).


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        trieleaf_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td sides="TR" colspan="2" bgcolor="grey75"></td>
            </tr>
            <tr><td sides="R">1</td>
                <td colspan="4" sides="LBR" width="320" bgcolor="grey75">Unused</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_INTMAPENTRY_KEY" title="WORD_INTMAPENTRY_KEY" colspan="4">Key (Generic)</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_MAPENTRY_VALUE" title="WORD_MAPENTRY_VALUE" colspan="4">Value (Generic)</td>
            </tr>
            </table>
        >]
    }
    









**See also**: [Integer Map Entries](group__intmapentry__words.md#group__intmapentry__words), [Integer Trie Maps](group__inttriemap__words.md#group__inttriemap__words)

## Submodules

* [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)

## Integer Trie Leaf Creation

<a id="group__minttrieleaf__words_1ga7d5079b13b1bda7ec6bfc72c38a708aa"></a>
### Macro WORD\_MINTTRIELEAF\_INIT

![][public]

```cpp
#define WORD_MINTTRIELEAF_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_MINTTRIELEAF](col_word_int_8h.md#group__words_1ga81a397c929cd0fa5f89c5a01ce2a1487)); \
    [WORD\_INTMAPENTRY\_KEY](col_map_int_8h.md#group__intmapentry__words_1ga89e26360d76aaad985afd89da56d1539)(word) = (key); \
    [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064)(word) = (value);( word ,key ,value )
```

Mutable integer leaf word initializer.

**Parameters**:

* **word**: Word to initialize.
* **key**: [WORD\_INTMAPENTRY\_KEY](col_map_int_8h.md#group__intmapentry__words_1ga89e26360d76aaad985afd89da56d1539).
* **value**: [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_MINTTRIELEAF](col_word_int_8h.md#group__words_1ga81a397c929cd0fa5f89c5a01ce2a1487)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)