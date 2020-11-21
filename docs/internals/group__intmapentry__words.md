<a id="group__intmapentry__words"></a>
# Integer Map Entries



Generic integer map entries are key-value pairs. Type-specific entries extend this generic type with implementation-dependent data.






**Requirements**:


* Map entry words use one single cell.

* Map entries must store at least a key and a value.


**Parameters**:

* **Key**: A word in the general case but can be a native integer for integer maps.
* **Value**: A word.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        mapentry_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td sides="R" colspan="2"></td>
            </tr>
            <tr><td sides="R">1</td>
                <td colspan="4" sides="BR" width="320">Type-specific data</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_INTMAPENTRY_KEY" title="WORD_INTMAPENTRY_KEY" colspan="4">Key</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_MAPENTRY_VALUE" title="WORD_MAPENTRY_VALUE" colspan="4">Value</td>
            </tr>
            </table>
        >]
    }

## Submodules

* [Integer Trie Leaves](group__minttrieleaf__words.md#group__minttrieleaf__words)
* [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)

## Integer Map Entry Accessors

<a id="group__intmapentry__words_1ga89e26360d76aaad985afd89da56d1539"></a>
### Macro WORD\_INTMAPENTRY\_KEY

![][public]

```cpp
#define WORD_INTMAPENTRY_KEY (((intptr_t *)(word))[2])( word )
```

Get/set key of integer map entry.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)