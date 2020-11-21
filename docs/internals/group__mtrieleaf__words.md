<a id="group__mtrieleaf__words"></a>
# Trie Leaves



Trie leaves are [Map Entries](group__mapentry__words.md#group__mapentry__words) with trie-specific data.






**Requirements**:


* Trie leave words, like generic [Map Entries](group__mapentry__words.md#group__mapentry__words), use one single cell.

* They need no specific info besides generic map entry fields.


**Parameters**:

* **Key**: [Generic map entry key field](col_map_int_8h.md#group__mapentry__words_1ga8664d15fae4553b47b658ac7ceb1443a).
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
                <td href="@ref WORD_MAPENTRY_KEY" title="WORD_MAPENTRY_KEY" colspan="4">Key (Generic)</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_MAPENTRY_VALUE" title="WORD_MAPENTRY_VALUE" colspan="4">Value (Generic)</td>
            </tr>
            </table>
        >]
    }
    









**See also**: [Map Entries](group__mapentry__words.md#group__mapentry__words), [Trie Maps](group__triemap__words.md#group__triemap__words)

## Submodules

* [Immutable Trie Leaves](group__trieleaf__words.md#group__trieleaf__words)

## Trie Leaf Creation

<a id="group__mtrieleaf__words_1ga2b31c29e5458243e6c134031a7d27047"></a>
### Macro WORD\_MTRIELEAF\_INIT

![][public]

```cpp
#define WORD_MTRIELEAF_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_MTRIELEAF](col_word_int_8h.md#group__words_1ga4c7f02b7545a17f527d59a2c66e9d0fa)); \
    [WORD\_MAPENTRY\_KEY](col_map_int_8h.md#group__mapentry__words_1ga8664d15fae4553b47b658ac7ceb1443a)(word) = (key); \
    [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064)(word) = (value);( word ,key ,value )
```

Mutable trie leaf word initializer.

**Parameters**:

* **word**: Word to initialize.
* **key**: [WORD\_MAPENTRY\_KEY](col_map_int_8h.md#group__mapentry__words_1ga8664d15fae4553b47b658ac7ceb1443a).
* **value**: [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_MTRIELEAF](col_word_int_8h.md#group__words_1ga4c7f02b7545a17f527d59a2c66e9d0fa)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)