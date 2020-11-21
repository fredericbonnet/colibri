<a id="group__predefined__words"></a>
# Predefined Words



Predefined words are [Regular Words](group__regular__words.md#group__regular__words) that are identified by a fixed ID and managed by hardwired internal code.





[Predefined type IDs](#group__words_1word_type_ids) for regular word types are chosen so that their bit 0 is cleared and bit 1 is set, to distinguish them with type pointers and avoid value clashes.






**Requirements**:


* Regular words must store their type ID.


**Parameters**:

* **Type**: Type identifier.


**Cell Layout**:

On all architectures the cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        predefined_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="10">0</td>
                <td sides="B" width="10">1</td>
                <td sides="B" width="20" align="left">2</td><td sides="B" width="30" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_PINNED" title="WORD_PINNED">P</td>
                <td>1</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td sides="R" colspan="2"></td>
            </tr>
            <tr><td sides="R">.</td>
                <td colspan="6" rowspan="3" sides="BR" width="320">Type-specific data</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }
    








**See also**: [Word Type Identifiers](#group__words_1word_type_ids)

## Submodules

* [String Hash Maps](group__strhashmap__words.md#group__strhashmap__words)
* [Integer Hash Maps](group__inthashmap__words.md#group__inthashmap__words)
* [Hash Entries](group__mhashentry__words.md#group__mhashentry__words)
* [Immutable Hash Entries](group__hashentry__words.md#group__hashentry__words)
* [Integer Hash Entries](group__minthashentry__words.md#group__minthashentry__words)
* [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)
* [Sublists](group__sublist__words.md#group__sublist__words)
* [Immutable Concat Lists](group__concatlist__words.md#group__concatlist__words)
* [Mutable Concat Lists](group__mconcatlist__words.md#group__mconcatlist__words)
* [Map Entries](group__mapentry__words.md#group__mapentry__words)
* [Integer Map Entries](group__intmapentry__words.md#group__intmapentry__words)
* [Fixed-Width UCS Strings](group__ucsstr__words.md#group__ucsstr__words)
* [Variable-Width UTF Strings](group__utfstr__words.md#group__utfstr__words)
* [Subropes](group__subrope__words.md#group__subrope__words)
* [Concat Ropes](group__concatrope__words.md#group__concatrope__words)
* [String Trie Maps](group__strtriemap__words.md#group__strtriemap__words)
* [Integer Trie Maps](group__inttriemap__words.md#group__inttriemap__words)
* [Trie Nodes](group__mtrienode__words.md#group__mtrienode__words)
* [Immutable Trie Nodes](group__trienode__words.md#group__trienode__words)
* [String Trie Nodes](group__mstrtrienode__words.md#group__mstrtrienode__words)
* [Immutable String Trie Nodes](group__strtrienode__words.md#group__strtrienode__words)
* [Integer Trie Nodes](group__minttrienode__words.md#group__minttrienode__words)
* [Immutable Integer Trie Nodes](group__inttrienode__words.md#group__inttrienode__words)
* [Trie Leaves](group__mtrieleaf__words.md#group__mtrieleaf__words)
* [Immutable Trie Leaves](group__trieleaf__words.md#group__trieleaf__words)
* [Integer Trie Leaves](group__minttrieleaf__words.md#group__minttrieleaf__words)
* [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)

## Predefined Word Accessors

<a id="group__predefined__words_1ga0ce63a12a32f50c4fada9a19f40356d0"></a>
### Macro WORD\_TYPEID

![][public]

```cpp
#define WORD_TYPEID ((((uint8_t *)(word))[0])&~1)( word )
```

Get type ID for predefined word type.

**Parameters**:

* **word**: Word to access.


**Returns**:

Word type ID.




**See also**: [Word Type Identifiers](#group__words_1word_type_ids), [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)



<a id="group__predefined__words_1ga52822cf424704829e60b112fe03614b6"></a>
### Macro WORD\_SET\_TYPEID

![][public]

```cpp
#define WORD_SET_TYPEID (((uint8_t *)(word))[0] = (type))( word ,type )
```

Set type ID for predefined word type.

**Parameters**:

* **word**: Word to access.
* **type**: Type ID.


**Side Effect**:

This also clears the pinned flag.




**See also**: [WORD\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga0ce63a12a32f50c4fada9a19f40356d0), [WORD\_CLEAR\_PINNED](col_word_int_8h.md#group__regular__words_1ga04a19fb132382d52fa42d3d3e4237f2f)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)