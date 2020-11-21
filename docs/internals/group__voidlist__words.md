<a id="group__voidlist__words"></a>
# Void List Words



Void list words are [Immediate Words](group__immediate__words.md#group__immediate__words) representing lists whose elements are all nil.






**Requirements**:


* Void list words need to know their length. Length width is the machine word width minus 4 bits, so the maximum length is about 1/16th of the theoretical maximum. Larger void lists are built by concatenating several shorter immediate void lists.

* Void list tag bit comes after the circular list tag so that void lists can be made circular. Void circular lists thus combine both tag bits.


**Parameters**:

* **Length**: List length.


**Word Layout**:

On all architectures the _n_-bit word layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        voidlist_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="25" align="left">0</td><td sides="B" width="25" align="right">4</td>
                <td sides="B" width="135" align="left">5</td><td sides="B" width="135" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td colspan="2">00101</td>
                <td href="@ref WORD_VOIDLIST_LENGTH" title="WORD_VOIDLIST_LENGTH" colspan="2">Length</td>
            </tr>
            </table>
        >]
    }
    









**See also**: [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb), [immediateWordTypes](col_word_int_8h.md#group__immediate__words_1gadc93860da89f6a52a884c10b7835aaac)

## Void List Word Constants

<a id="group__voidlist__words_1gabf4f6876ac71e3f3368c711756459baf"></a>
### Macro VOIDLIST\_MAX\_LENGTH

![][public]

```cpp
#define VOIDLIST_MAX_LENGTH (SIZE_MAX>>5)
```

Maximum length of void list words.





<a id="group__voidlist__words_1ga8b7c20d2cdcdf8f3bc58589d757cf53b"></a>
### Macro WORD\_LIST\_EMPTY

![][public]

```cpp
#define WORD_LIST_EMPTY [WORD\_VOIDLIST\_NEW](col_word_int_8h.md#group__voidlist__words_1ga76a437807f61cfb3d4066867ae82d8d4)(0)
```

Empty list immediate singleton.

This is simply a zero-length void list.



## Void List Word Creation

<a id="group__voidlist__words_1ga76a437807f61cfb3d4066867ae82d8d4"></a>
### Macro WORD\_VOIDLIST\_NEW

![][public]

```cpp
#define WORD_VOIDLIST_NEW     (([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043))(intptr_t)((((size_t)(length))<<5)|20))( length )
```

Void list word creation.

**Parameters**:

* **length**: Void list length. Must be less than [VOIDLIST\_MAX\_LENGTH](col_word_int_8h.md#group__voidlist__words_1gabf4f6876ac71e3f3368c711756459baf).


**Returns**:

The new void list word.



**See also**: [WORD\_VOIDLIST\_LENGTH](col_word_int_8h.md#group__voidlist__words_1gaa336b44598700785e9f948fdd3a0da58)



## Void List Word Accessors

<a id="group__voidlist__words_1gaa336b44598700785e9f948fdd3a0da58"></a>
### Macro WORD\_VOIDLIST\_LENGTH

![][public]

```cpp
#define WORD_VOIDLIST_LENGTH (((size_t)(intptr_t)(word))>>5)( word )
```

Get void list length.

**Parameters**:

* **word**: Word to access.



**See also**: [WORD\_VOIDLIST\_NEW](col_word_int_8h.md#group__voidlist__words_1ga76a437807f61cfb3d4066867ae82d8d4)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)