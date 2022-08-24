<a id="group__gc__roots"></a>
# Roots



Roots are explicitly preserved words, using a reference count.





Roots are stored in a trie indexed by the root source addresses. This forms an ordered collection that is traversed upon GC and is indexable and modifiable in O(log(k)) worst-case, k being the bit size of a cell pointer.





There are two kinds of cells that form the root trie: nodes and leaves. Nodes are intermediate cells pointing to two children. Leaves are terminal cells pointing to the preserved word.






**Common Requirements**:


* Root trie cells use one single cell.

* Root trie cells must know their parent cell for bottom-up traversal.

* There must be a way to tell nodes and leaves apart.


**Parameters**:

* **Parent**: Parent node in trie. NULL for trie root.





**Requirements**:


* Root trie nodes must know their critical bit, which is the highest bit where left and right subtrie keys differ. They store this information as a bitmask with this single bit set.

* Root trie nodes must know their left and right subtries.


**Parameters**:

* **Mask**: Bitmask where only the critical bit is set.
* **Left**: Left subtrie. Keys have their critical bit cleared.
* **Right**: Right subtrie. Keys have their critical bit set.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        root_node [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="160" align="left">0</td><td sides="B" width="160" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref ROOT_NODE_MASK" title="ROOT_NODE_MASK" colspan="2" width="320">Mask</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref ROOT_PARENT" title="ROOT_PARENT" colspan="2">Parent</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref ROOT_NODE_LEFT" title="ROOT_NODE_LEFT" colspan="2">Left</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref ROOT_NODE_RIGHT" title="ROOT_NODE_RIGHT" colspan="4">Right</td>
            </tr>
            </table>
        >]
    }
    






#### Root Trie Leaves





**Requirements**:


* Root trie leaves must know the root's source word.

* Root trie leaves must know the root's reference count. Once it drops to zero the root is removed, however the source may survive if it is referenced elsewhere.


**Parameters**:

* **Generation**: Generation of the source page. Storing it here saves a pointer dereference.
* **Refcount**: Reference count.
* **Source**: Preserved word.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        root_leaf [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="160" align="left">0</td><td sides="B" width="160" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref ROOT_LEAF_GENERATION" title="ROOT_LEAF_GENERATION" colspan="2">Generation</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref ROOT_PARENT" title="ROOT_PARENT" colspan="3">Parent</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref ROOT_LEAF_REFCOUNT" title="ROOT_LEAF_REFCOUNT" colspan="3">Refcount</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref ROOT_LEAF_SOURCE" title="ROOT_LEAF_SOURCE" colspan="3">Source</td>
            </tr>
            </table>
        >]
    }
    









**See also**: [Col\_WordPreserve](col_word_8h.md#group__words_1gab55f452e6b0856f7bd7b34e04fae2aa2), [Col\_WordRelease](col_word_8h.md#group__words_1gad93112f81ce6511d6d0ece0db4d38598)

## Root Trie Creation

<a id="group__gc__roots_1ga9e5b5f33b34e01b54b6addaaaa9ace5e"></a>
### Macro ROOT\_NODE\_INIT

![][public]

```cpp
#define ROOT_NODE_INIT     [ROOT\_PARENT](col_internal_8h.md#group__gc__roots_1gadd16c38bcc5016a0c43a17442c232ffa)(cell) = parent; \
    [ROOT\_NODE\_MASK](col_internal_8h.md#group__gc__roots_1gaa1f0481c45b7c14cf933b91d9f27a541)(cell) = mask; \
    [ROOT\_NODE\_LEFT](col_internal_8h.md#group__gc__roots_1ga27676041bc270c4dfc8c7caea4e64274)(cell) = left; \
    [ROOT\_NODE\_RIGHT](col_internal_8h.md#group__gc__roots_1gaf86512b7113a6afaea849d480070dd33)(cell) = right;( cell ,parent ,mask ,left ,right )
```

Root trie node initializer.

**Parameters**:

* **cell**: Cell to initialize.
* **parent**: [ROOT\_PARENT](col_internal_8h.md#group__gc__roots_1gadd16c38bcc5016a0c43a17442c232ffa).
* **mask**: [ROOT\_NODE\_MASK](col_internal_8h.md#group__gc__roots_1gaa1f0481c45b7c14cf933b91d9f27a541).
* **left**: [ROOT\_NODE\_LEFT](col_internal_8h.md#group__gc__roots_1ga27676041bc270c4dfc8c7caea4e64274).
* **right**: [ROOT\_NODE\_RIGHT](col_internal_8h.md#group__gc__roots_1gaf86512b7113a6afaea849d480070dd33).


!> **Warning** \
Argument **cell** is referenced several times by the macro. Make sure to avoid any side effect.



<a id="group__gc__roots_1ga283423b988b4f6979e2f6e8d2b8a8ba9"></a>
### Macro ROOT\_LEAF\_INIT

![][public]

```cpp
#define ROOT_LEAF_INIT     [ROOT\_PARENT](col_internal_8h.md#group__gc__roots_1gadd16c38bcc5016a0c43a17442c232ffa)(cell) = parent; \
    [ROOT\_LEAF\_GENERATION](col_internal_8h.md#group__gc__roots_1gae5cec2c280c8e0dd0beb3b5c8ba5f0da)(cell) = generation; \
    [ROOT\_LEAF\_REFCOUNT](col_internal_8h.md#group__gc__roots_1gaef6c67ed97c9ceace9b0818bed0110b2)(cell) = refcount; \
    [ROOT\_LEAF\_SOURCE](col_internal_8h.md#group__gc__roots_1gae634c20d504ac8bd99ab26f4ddc3ee12)(cell) = source;( cell ,parent ,generation ,refcount ,source )
```

Root trie leaf initializer.

**Parameters**:

* **cell**: Cell to initialize.
* **parent**: [ROOT\_PARENT](col_internal_8h.md#group__gc__roots_1gadd16c38bcc5016a0c43a17442c232ffa).
* **generation**: [ROOT\_LEAF\_GENERATION](col_internal_8h.md#group__gc__roots_1gae5cec2c280c8e0dd0beb3b5c8ba5f0da).
* **refcount**: [ROOT\_LEAF\_REFCOUNT](col_internal_8h.md#group__gc__roots_1gaef6c67ed97c9ceace9b0818bed0110b2).
* **source**: [ROOT\_LEAF\_SOURCE](col_internal_8h.md#group__gc__roots_1gae634c20d504ac8bd99ab26f4ddc3ee12).


!> **Warning** \
Argument **cell** is referenced several times by the macro. Make sure to avoid any side effect.



## Root Trie Accessors

<a id="group__gc__roots_1gaa8c5f8c118fe2e0c3c0bcfb0d8be9a08"></a>
### Macro ROOT\_IS\_LEAF

![][public]

```cpp
#define ROOT_IS_LEAF ((uintptr_t)(cell) & 1)( cell )
```

Whether pointed cell is a root trie node or leaf.

To distinguish between both types the least significant bit of leaf pointers is set.






**Parameters**:

* **cell**: Cell to access.




**See also**: [ROOT\_GET\_NODE](col_internal_8h.md#group__gc__roots_1ga33fa98f95a7bab38652b351e60d60bae), [ROOT\_GET\_LEAF](col_internal_8h.md#group__gc__roots_1gaf093eb2c5ec8150f06a69bd10df6be6a)



<a id="group__gc__roots_1ga33fa98f95a7bab38652b351e60d60bae"></a>
### Macro ROOT\_GET\_NODE

![][public]

```cpp
#define ROOT_GET_NODE (([Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) *)((uintptr_t)(cell) & ~1))( cell )
```

Get actual pointer to node.

**Parameters**:

* **cell**: Cell to access.



**See also**: [ROOT\_IS\_LEAF](col_internal_8h.md#group__gc__roots_1gaa8c5f8c118fe2e0c3c0bcfb0d8be9a08)



<a id="group__gc__roots_1gaf093eb2c5ec8150f06a69bd10df6be6a"></a>
### Macro ROOT\_GET\_LEAF

![][public]

```cpp
#define ROOT_GET_LEAF (([Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) *)((uintptr_t)(cell) | 1))( cell )
```

Get actual pointer to leaf.

**Parameters**:

* **cell**: Cell to access.



**See also**: [ROOT\_IS\_LEAF](col_internal_8h.md#group__gc__roots_1gaa8c5f8c118fe2e0c3c0bcfb0d8be9a08)



<a id="group__gc__roots_1gadd16c38bcc5016a0c43a17442c232ffa"></a>
### Macro ROOT\_PARENT

![][public]

```cpp
#define ROOT_PARENT ((([Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) **)(cell))[1])( cell )
```

Get/set parent node in trie.

**Parameters**:

* **cell**: Cell to access.


?> Macro is L-Value and suitable for both read/write operations.




**See also**: [ROOT\_NODE\_INIT](col_internal_8h.md#group__gc__roots_1ga9e5b5f33b34e01b54b6addaaaa9ace5e), [ROOT\_LEAF\_INIT](col_internal_8h.md#group__gc__roots_1ga283423b988b4f6979e2f6e8d2b8a8ba9)



<a id="group__gc__roots_1gaa1f0481c45b7c14cf933b91d9f27a541"></a>
### Macro ROOT\_NODE\_MASK

![][public]

```cpp
#define ROOT_NODE_MASK (((uintptr_t *)(cell))[0])( cell )
```

Get/set bitmask where only the critical bit is set, i.e. the highest bit where left and right subtries differ.

**Parameters**:

* **cell**: Cell to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [ROOT\_NODE\_INIT](col_internal_8h.md#group__gc__roots_1ga9e5b5f33b34e01b54b6addaaaa9ace5e)



<a id="group__gc__roots_1ga27676041bc270c4dfc8c7caea4e64274"></a>
### Macro ROOT\_NODE\_LEFT

![][public]

```cpp
#define ROOT_NODE_LEFT ((([Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) **)(cell))[2])( cell )
```

Get/set left subtrie.

**Parameters**:

* **cell**: Cell to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [ROOT\_NODE\_INIT](col_internal_8h.md#group__gc__roots_1ga9e5b5f33b34e01b54b6addaaaa9ace5e)



<a id="group__gc__roots_1gaf86512b7113a6afaea849d480070dd33"></a>
### Macro ROOT\_NODE\_RIGHT

![][public]

```cpp
#define ROOT_NODE_RIGHT ((([Cell](col_internal_8h.md#group__pages__cells_1ga4eabbd6c7c650aaf998aefac3c78448f) **)(cell))[3])( cell )
```

Get/set right subtrie.

**Parameters**:

* **cell**: Cell to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [ROOT\_NODE\_INIT](col_internal_8h.md#group__gc__roots_1ga9e5b5f33b34e01b54b6addaaaa9ace5e)



<a id="group__gc__roots_1gae5cec2c280c8e0dd0beb3b5c8ba5f0da"></a>
### Macro ROOT\_LEAF\_GENERATION

![][public]

```cpp
#define ROOT_LEAF_GENERATION (((unsigned int *)(cell))[0])( cell )
```

Get/set generation of the source page.

Storing it here saves a pointer dereference.






**Parameters**:

* **cell**: Cell to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [ROOT\_LEAF\_INIT](col_internal_8h.md#group__gc__roots_1ga283423b988b4f6979e2f6e8d2b8a8ba9)



<a id="group__gc__roots_1gaef6c67ed97c9ceace9b0818bed0110b2"></a>
### Macro ROOT\_LEAF\_REFCOUNT

![][public]

```cpp
#define ROOT_LEAF_REFCOUNT (((size_t *)(cell))[2])( cell )
```

Get/set the root's reference count.

**Parameters**:

* **cell**: Cell to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [ROOT\_LEAF\_INIT](col_internal_8h.md#group__gc__roots_1ga283423b988b4f6979e2f6e8d2b8a8ba9)



<a id="group__gc__roots_1gae634c20d504ac8bd99ab26f4ddc3ee12"></a>
### Macro ROOT\_LEAF\_SOURCE

![][public]

```cpp
#define ROOT_LEAF_SOURCE ((([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(cell))[3])( cell )
```

Get/set the root's preserved word.

**Parameters**:

* **cell**: Cell to access.


?> Macro is L-Value and suitable for both read/write operations.



**See also**: [ROOT\_LEAF\_INIT](col_internal_8h.md#group__gc__roots_1ga283423b988b4f6979e2f6e8d2b8a8ba9)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)