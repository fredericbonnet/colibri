<a id="group__sublist__words"></a>
# Sublists



Sublists are immutable lists pointing to a slice of a larger list.





Extraction of a sublist from a large list can be expensive. For example, removing one element from a string of several thousands implies copying the whole data minus this element. Likewise, extracting a range of elements from a complex binary tree of list nodes implies traversing and copying large parts of this subtree.





To limit the memory and processing costs of list slicing, the sublist word simply points to the original list and the index range of elements in the slice. This representation is preferred over raw copy when the source list is large.






**Requirements**:


* Sublist words use one single cell.

* Sublists must know their source list word. This source must be immutable.

* Sublists must also know the indices of the first and last elements in the source list that belongs to the slice. These indices are inclusive. Difference plus one gives the sublist length.

* Last, sublists must know their depth for tree balancing (see [List Tree Balancing](#group__list__words_1list_tree_balancing)). This is the same as the source depth, but to avoid one pointer dereference we cache the value in the word.


**Parameters**:

* **Depth**: Depth of sublist. 8 bits will code up to 255 depth levels, which is more than sufficient for balanced binary trees.
* **Source**: List of which this one is a sublist.
* **First**: First element in source.
* **Last**: Last element in source.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        sublist_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_SUBLIST_DEPTH" title="WORD_SUBLIST_DEPTH" colspan="2">Depth</td>
                <td colspan="2" bgcolor="grey75">Unused</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_SUBLIST_SOURCE" title="WORD_SUBLIST_SOURCE" colspan="6">Source</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_SUBLIST_FIRST" title="WORD_SUBLIST_FIRST" colspan="6">First</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_SUBLIST_LAST" title="WORD_SUBLIST_LAST" colspan="6">Last</td>
            </tr>
            </table>
        >]
    }
    








**See also**: [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)

## Sublist Creation

<a id="group__sublist__words_1ga0e3fe32fa3ae20489dc8d38db3148c1b"></a>
### Macro WORD\_SUBLIST\_INIT

![][public]

```cpp
#define WORD_SUBLIST_INIT     [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)((word), [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)); \
    [WORD\_SUBLIST\_DEPTH](col_list_int_8h.md#group__sublist__words_1ga6e3c10b7e63b6f6cabd36f24b69a6ecf)(word) = (uint8_t) (depth); \
    [WORD\_SUBLIST\_SOURCE](col_list_int_8h.md#group__sublist__words_1ga448ba72120af1a5d655107b05479424c)(word) = (source); \
    [WORD\_SUBLIST\_FIRST](col_list_int_8h.md#group__sublist__words_1ga18e2c99a420ebeabe9f94b4166c44033)(word) = (first); \
    [WORD\_SUBLIST\_LAST](col_list_int_8h.md#group__sublist__words_1ga4e00a849390732cdf829ce42d9c42417)(word) = (last);( word ,depth ,source ,first ,last )
```

Sublist word initializer.

**Parameters**:

* **word**: Word to initialize.
* **depth**: [WORD\_SUBLIST\_DEPTH](col_list_int_8h.md#group__sublist__words_1ga6e3c10b7e63b6f6cabd36f24b69a6ecf).
* **source**: [WORD\_SUBLIST\_SOURCE](col_list_int_8h.md#group__sublist__words_1ga448ba72120af1a5d655107b05479424c).
* **first**: [WORD\_SUBLIST\_FIRST](col_list_int_8h.md#group__sublist__words_1ga18e2c99a420ebeabe9f94b4166c44033).
* **last**: [WORD\_SUBLIST\_LAST](col_list_int_8h.md#group__sublist__words_1ga4e00a849390732cdf829ce42d9c42417).


!> **Warning** \
Argument **word** is referenced several times by the macro. Make sure to avoid any side effect.



**See also**: [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)



## Sublist Accessors

<a id="group__sublist__words_1ga6e3c10b7e63b6f6cabd36f24b69a6ecf"></a>
### Macro WORD\_SUBLIST\_DEPTH

![][public]

```cpp
#define WORD_SUBLIST_DEPTH (((uint8_t *)(word))[1])( word )
```

Get/set depth of sublist.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_SUBLIST\_INIT](col_list_int_8h.md#group__sublist__words_1ga0e3fe32fa3ae20489dc8d38db3148c1b)



<a id="group__sublist__words_1ga448ba72120af1a5d655107b05479424c"></a>
### Macro WORD\_SUBLIST\_SOURCE

![][public]

```cpp
#define WORD_SUBLIST_SOURCE ((([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(word))[1])( word )
```

Get/set list from which this one is extracted.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_SUBLIST\_INIT](col_list_int_8h.md#group__sublist__words_1ga0e3fe32fa3ae20489dc8d38db3148c1b)



<a id="group__sublist__words_1ga18e2c99a420ebeabe9f94b4166c44033"></a>
### Macro WORD\_SUBLIST\_FIRST

![][public]

```cpp
#define WORD_SUBLIST_FIRST (((size_t *)(word))[2])( word )
```

Get/set index of first element in source.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_SUBLIST\_INIT](col_list_int_8h.md#group__sublist__words_1ga0e3fe32fa3ae20489dc8d38db3148c1b)



<a id="group__sublist__words_1ga4e00a849390732cdf829ce42d9c42417"></a>
### Macro WORD\_SUBLIST\_LAST

![][public]

```cpp
#define WORD_SUBLIST_LAST (((size_t *)(word))[3])( word )
```

Get/set index of last element in source.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [WORD\_SUBLIST\_INIT](col_list_int_8h.md#group__sublist__words_1ga0e3fe32fa3ae20489dc8d38db3148c1b)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)