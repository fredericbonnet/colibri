<a id="group__regular__words"></a>
# Regular Words



Regular words are datatypes that store their structure in cells, and are identified by their cell address.





Regular words store their data in cells whose 1st machine word is used for the word type field, which is either a numerical ID for predefined types or a pointer to a [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type) structure for custom types. As such structures are always word-aligned, this means that the two least significant bits of their pointer value are zero (for architectures with at least 32 bit words) and are free for our purpose.





On little endian architectures, the LSB of the type pointer is the cell's byte 0. On big endian architectures, we rotate the pointer value one byte to the right so that the original LSB ends up on byte 0. That way the two reserved bits are on byte 0 for both predefined type IDs and type pointers.





We use bit 0 of the type field as the pinned flag for both predefined type IDs and type pointers. Given the above, this bit is always on byte 0. When set, this means that the word isn't movable; its address remains fixed as long as this flag is set. Words can normally be moved to the upper generation pool during the compaction phase of the GC.






**Requirements**:


* Regular words must store their type info and a pinned flag.


**Parameters**:

* **Pinned(P)**: Pinned flag.


**Cell Layout**:

On all architectures the cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        regular_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="10">0</td>
                <td sides="B" width="150" align="left">1</td><td sides="B" width="155" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_PINNED" title="WORD_PINNED">P</td>
                <td sides="R" colspan="2"></td>
            </tr>
            <tr><td sides="R">.</td>
                <td colspan="3" rowspan="3" sides="BR" width="320">Type-specific data</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }

## Submodules

* [Predefined Words](group__predefined__words.md#group__predefined__words)
* [Wrap Words](group__wrap__words.md#group__wrap__words)
* [Redirect Words](group__redirect__words.md#group__redirect__words)

## Regular Word Accessors

<a id="group__regular__words_1ga19cfddbcf0127f5088803cc68ddb8eaa"></a>
### Macro WORD\_SYNONYM

![][public]

```cpp
#define WORD_SYNONYM ((([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(word))[1])( word )
```

Get/set synonym word for regular words having a synonym field.

For performance and simplicity all word types with a synonym field use the same location (2nd word of cell).






**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



<a id="group__regular__words_1gad20cf13be09a354418d8615e6f2f2193"></a>
### Macro WORD\_PINNED

![][public]

```cpp
#define WORD_PINNED (((uint8_t *)(word))[0] & 1)( word )
```

Get pinned flag.

**See also**: [WORD\_SET\_PINNED](col_word_int_8h.md#group__regular__words_1ga7ae30ab27827ba70ae1265b5f637101a), [WORD\_CLEAR\_PINNED](col_word_int_8h.md#group__regular__words_1ga04a19fb132382d52fa42d3d3e4237f2f)



<a id="group__regular__words_1ga7ae30ab27827ba70ae1265b5f637101a"></a>
### Macro WORD\_SET\_PINNED

![][public]

```cpp
#define WORD_SET_PINNED (((uint8_t *)(word))[0] |= 1)( word )
```

Set pinned flag.

**See also**: [WORD\_PINNED](col_word_int_8h.md#group__regular__words_1gad20cf13be09a354418d8615e6f2f2193), [WORD\_CLEAR\_PINNED](col_word_int_8h.md#group__regular__words_1ga04a19fb132382d52fa42d3d3e4237f2f)



<a id="group__regular__words_1ga04a19fb132382d52fa42d3d3e4237f2f"></a>
### Macro WORD\_CLEAR\_PINNED

![][public]

```cpp
#define WORD_CLEAR_PINNED (((uint8_t *)(word))[0] &= ~1)( word )
```

Clear pinned flag.

**See also**: [WORD\_PINNED](col_word_int_8h.md#group__regular__words_1gad20cf13be09a354418d8615e6f2f2193), [WORD\_SET\_PINNED](col_word_int_8h.md#group__regular__words_1ga7ae30ab27827ba70ae1265b5f637101a)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)