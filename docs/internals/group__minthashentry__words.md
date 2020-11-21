<a id="group__minthashentry__words"></a>
# Integer Hash Entries



Integer hash entries are [Integer Map Entries](group__intmapentry__words.md#group__intmapentry__words) with hash-specific data.






**Requirements**:


* Integer hash entry words, like generic [Integer Map Entries](group__intmapentry__words.md#group__intmapentry__words), use one single cell.

* Integer hash entries sharing the same bucket are chained using a linked list, so each entry store must store a pointer to the next entry in chain.


**Parameters**:

* **Key**: [Generic integer map entry key field](col_map_int_8h.md#group__intmapentry__words_1ga89e26360d76aaad985afd89da56d1539).
* **Value**: [Generic map entry value field](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064).
* **Next**: Next entry in bucket. Also used during iteration.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        inthashentry_word [shape=none, label=<
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
                <td href="@ref WORD_HASHENTRY_NEXT" title="WORD_HASHENTRY_NEXT" colspan="4">Next</td>
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
    










**See also**: [Integer Map Entries](group__intmapentry__words.md#group__intmapentry__words), [Integer Hash Maps](group__inthashmap__words.md#group__inthashmap__words), [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)

## Submodules

* [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)

## Integer Hash Entry Creation

<a id="group__minthashentry__words_1ga0d1fde498a4661bd712715552c7fc675"></a>
### Macro WORD\_MINTHASHENTRY\_INIT

![][public]

```cpp
#define WORD_MINTHASHENTRY_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_MINTHASHENTRY](col_word_int_8h.md#group__words_1ga1758f2fa0c44200f5782e548c5b33c7e)); \
    [WORD\_INTMAPENTRY\_KEY](col_map_int_8h.md#group__intmapentry__words_1ga89e26360d76aaad985afd89da56d1539)(word) = (key); \
    [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064)(word) = (value); \
    [WORD\_HASHENTRY\_NEXT](col_hash_int_8h.md#group__mhashentry__words_1ga9f087b8c13513115c5e1b19c86fbe145)(word) = (next);( word ,key ,value ,next )
```

Mutable integer hash entry word initializer.

**Parameters**:

* **word**: Word to initialize.
* **key**: [WORD\_INTMAPENTRY\_KEY](col_map_int_8h.md#group__intmapentry__words_1ga89e26360d76aaad985afd89da56d1539).
* **value**: [WORD\_MAPENTRY\_VALUE](col_map_int_8h.md#group__mapentry__words_1gabad6806f2947f508a9786948c1663064).
* **next**: [WORD\_HASHENTRY\_NEXT](col_hash_int_8h.md#group__mhashentry__words_1ga9f087b8c13513115c5e1b19c86fbe145).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_MINTHASHENTRY](col_word_int_8h.md#group__words_1ga1758f2fa0c44200f5782e548c5b33c7e)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)