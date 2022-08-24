<a id="group__list__words"></a>
# Immutable Lists

Immutable lists are constant, arbitrary-sized, linear collections of words.

<a id="group__list__words_1list_tree_balancing"></a>
 Immutable lists can be composed of immutable vectors and lists. Immutable vectors can themselves be used in place of immutable lists.






!> **Warning** \
Mutable vectors, when used in place of immutable lists, may be potentially frozen in the process. To avoid that, they should be properly duplicated to an immutable vector beforehand.







#### List Tree Balancing




Large lists are built by concatenating several sublists, forming a balanced binary tree. A balanced tree is one where the depth of the left and right arms do not differ by more than one level.





List trees are self-balanced by construction: when two lists are concatenated, if their respective depths differ by more than 1, then the tree is recursively rebalanced by splitting and merging subarms. There are four major cases, two if we consider symmetry:






* Deepest subtree is an outer branch (i.e. the left resp. right child of the left resp. right arm). In this case the tree is rotated: the opposite child is moved and concatenated with the opposite arm. For example, with left being deepest:








    digraph {
        fontname="Helvetica";
        node [fontname="Helvetica" fontsize=10 shape="box" style="rounded" height=0 width=0];
        edge [dir="back" arrowtail="odiamond"];

        subgraph cluster_before {
            label="Before rotation";

            concat_before               [label="concat(left,right)\n= (left1 + left2) + right"];
                left_before             [label="left\n= left1 + left2" style="rounded,filled" fillcolor="grey75"];
                    left1_before        [label="left1"];
                        left11_before   [label="?" style="solid,bold"];
                        left12_before   [label="?" style="solid,bold"];
                    left2_before        [label="left2" style="solid,bold"];
                right_before            [label="right" style="solid,bold"];

            concat_before -> left_before;
                left_before -> left1_before;
                    left1_before -> left11_before;
                    left1_before -> left12_before;
                left_before -> left2_before;
            concat_before -> right_before;

            subgraph {
                edge [dir="forward" arrowhead="open"];
                rank = same; left11_before; left12_before, left2_before; right_before;
                left11_before -> left12_before;
                left12_before -> left2_before;
                left2_before -> right_before;
            }
        }
        subgraph cluster_after {
            label="After rotation";

            concat_after                [label="concat(left,right)\n= left1 + (left2 + right)"];
                left1_after             [label="left1"];
                    left11_after        [label="?" style="solid,bold"];
                    left12_after        [label="?" style="solid,bold"];
                concat2_after           [label="left2 + right" style="rounded,filled" fillcolor="grey75"];
                    left2_after         [label="left2" style="solid,bold"];
                    right_after         [label="right" style="solid,bold"];

            concat_after -> left1_after;
                left1_after -> left11_after;
                left1_after -> left12_after;
            concat_after -> concat2_after;
                concat2_after -> left2_after;
                concat2_after -> right_after;

            subgraph {
                edge [dir="forward" arrowhead="open"];
                rank = same; left11_after; left12_after, left2_after; right_after;
                left11_after -> left12_after;
                left12_after -> left2_after;
                left2_after -> right_after;
            }
        }
    }
  







* Deepest subtree is an inner branch (i.e. the right resp. left child of the left resp. right arm). In this case the subtree is split between both arms. For example, with left being deepest:








    digraph {
        fontname="Helvetica";
        node [fontname="Helvetica" fontsize=10 shape="box" style="rounded" height=0 width=0];
        edge [dir="back" arrowtail="odiamond"];

        subgraph cluster_before {
            label="Before splitting";

            concat_before               [label="concat(left,right)\n= (left1 + (left21+left22)) + right"];
                left_before             [label="left\n= left1 + (left21+left22)" style="rounded,filled" fillcolor="grey75"];
                    left1_before        [label="left1" style="solid,bold"];
                    left2_before        [label="left2\n= left21+left22" style="rounded,filled" fillcolor="grey75"];
                        left21_before   [label="left21" style="solid,bold"];
                        left22_before   [label="left22" style="solid,bold"];
                right_before            [label="right" style="solid,bold"];

            concat_before -> left_before;
                left_before -> left1_before;
                left_before -> left2_before;
                    left2_before -> left21_before;
                    left2_before -> left22_before;
            concat_before -> right_before;

            subgraph {
                edge [dir="forward" arrowhead="open"];
                rank = same; left1_before; left21_before, left22_before; right_before;
                left1_before -> left21_before;
                left21_before -> left22_before;
                left22_before -> right_before;
            }
        }
        subgraph cluster_after {
            label="After splitting";

            concat_after                [label="concat(left,right)\n=  (left1 + left21) + (left22 + right)"];
                concat2_after           [label="left1 + left21" style="rounded,filled" fillcolor="grey75"];
                    left1_after         [label="left1" style="solid,bold"];
                    left21_after        [label="left21" style="solid,bold"];
                concat3_after           [label="left22 + right" style="rounded,filled" fillcolor="grey75"];
                    left22_after        [label="left22" style="solid,bold"];
                    right_after         [label="right" style="solid,bold"];

            concat_after -> concat2_after;
                concat2_after -> left1_after;
                concat2_after -> left21_after;
            concat_after -> concat3_after;
                concat3_after -> left22_after;
                concat3_after -> right_after;

            subgraph {
                edge [dir="forward" arrowhead="open"];
                rank = same; left1_after; left21_after, left22_after; right_after;
                left1_after -> left21_after;
                left21_after -> left22_after;
                left22_after -> right_after;
            }
        }
    }
  








**See also**: [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)

## Submodules

* [Custom Lists](group__customlist__words.md#group__customlist__words)
* [Sublists](group__sublist__words.md#group__sublist__words)
* [Immutable Concat Lists](group__concatlist__words.md#group__concatlist__words)

## Immutable List Creation

<a id="group__list__words_1gafc53d9d08fb0368e412785b161194b7f"></a>
### Function Col\_EmptyList

![][public]

```cpp
Col_Word Col_EmptyList()
```

Return an empty list.

The returned word is immediate and constant, which means that it consumes no memory and its value can be safely compared and stored in static storage.






**Returns**:

The empty list.



**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [WORD\_LIST\_EMPTY](col_word_int_8h.md#group__voidlist__words_1ga8b7c20d2cdcdf8f3bc58589d757cf53b)

<a id="group__list__words_1ga0dc0d4d58c4e694683753211151b0fc7"></a>
### Function Col\_NewList

![][public]

```cpp
Col_Word Col_NewList(size_t length, const Col_Word *elements)
```

Create a new list word.

Short lists are created as vectors. Larger lists are recursively split in half and assembled in a tree.






**Returns**:

A new list.



**Parameters**:

* size_t **length**: Length of below array.
* const [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **elements**: Array of words to populate list with. If NULL, build a void list (i.e. whose elements are all nil).

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [Col\_NewVector](col_vector_8h.md#group__vector__words_1ga6ef7d35d75fdc6a6781f0a32e9c7efc1)
* [VOIDLIST\_MAX\_LENGTH](col_word_int_8h.md#group__voidlist__words_1gabf4f6876ac71e3f3368c711756459baf)
* [WORD\_LIST\_EMPTY](col_word_int_8h.md#group__voidlist__words_1ga8b7c20d2cdcdf8f3bc58589d757cf53b)
* [WORD\_VOIDLIST\_NEW](col_word_int_8h.md#group__voidlist__words_1ga76a437807f61cfb3d4066867ae82d8d4)

**Referenced by**:

* [Col\_MListSetLength](col_list_8h.md#group__mlist__words_1ga939319b0240b3bd7f0713b09b4dc08dc)

## Immutable List Accessors

?> Works with mutable or immutable lists and vectors.

<a id="group__list__words_1ga5eb1c9ddea940171e18817b90301cc03"></a>
### Function Col\_ListLength

![][public]

```cpp
size_t Col_ListLength(Col_Word list)
```

Get the length of the list.

Also works with vectors. If the list is cyclic, only give the core length.






**Returns**:

The list length.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **list**: Not a list.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: List to get length for.

**Return type**: EXTERN size_t

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [ListChunkTraverseInfo::list](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a20400ffc5ef99dfd4ffd684870cf51f0)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)
* [WORD\_CONCATLIST\_LENGTH](col_list_int_8h.md#group__concatlist__words_1ga005eedea923258322288f51292f33782)
* [WORD\_SUBLIST\_FIRST](col_list_int_8h.md#group__sublist__words_1ga18e2c99a420ebeabe9f94b4166c44033)
* [WORD\_SUBLIST\_LAST](col_list_int_8h.md#group__sublist__words_1ga4e00a849390732cdf829ce42d9c42417)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_CUSTOM](col_word_int_8h.md#group__words_1ga8babfbc77291680db519873c91efdd4c)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)
* [WORD\_TYPE\_VECTOR](col_word_int_8h.md#group__words_1gadf6c66e5c2f9fcdf213ae40d253c153f)
* [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb)
* [WORD\_TYPEINFO](col_word_int_8h.md#group__custom__words_1gafc962791c45a5dd5bb034050444084be)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)
* [WORD\_VECTOR\_LENGTH](col_vector_int_8h.md#group__vector__words_1ga926467c6e28cbec0b62107c2d17bb06c)
* [WORD\_VOIDLIST\_LENGTH](col_word_int_8h.md#group__voidlist__words_1gaa336b44598700785e9f948fdd3a0da58)

**Referenced by**:

* [Col\_CircularList](col_list_8h.md#group__list__words_1ga327fd0dc9444ab63bdc30d5f62eee4ea)
* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [Col\_CopyMList](col_list_8h.md#group__mlist__words_1gaff1c8024a9ca1e6a6f33de4fe7344e81)
* [Col\_ListInsert](col_list_8h.md#group__list__words_1ga8ab7ea672c85e35028769758674e26f1)
* [Col\_ListIterBegin](col_list_8h.md#group__list__words_1gab82314e6a85f5b68b646efe5c9fe0200)
* [Col\_ListIterFirst](col_list_8h.md#group__list__words_1ga0f1d7cb546b01d0a346e40a49671d0ec)
* [Col\_ListIterLast](col_list_8h.md#group__list__words_1ga39ef512be590fc88aca2f6d9668c6ddb)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [Col\_ListRemove](col_list_8h.md#group__list__words_1gad3a369cfb544159866851398efcf50a8)
* [Col\_ListReplace](col_list_8h.md#group__list__words_1ga7286de8fbf801b507b4dd9ea422153c0)
* [Col\_MListInsert](col_list_8h.md#group__mlist__words_1ga453cc8131623ff7f2981ffe3cca75797)
* [Col\_MListLoop](col_list_8h.md#group__mlist__words_1ga717d5a5fa372406df49ae460436b923e)
* [Col\_MListRemove](col_list_8h.md#group__mlist__words_1ga0ff728019a9d46b0a7c0096007203915)
* [Col\_MListReplace](col_list_8h.md#group__mlist__words_1gaecb4269ea8b60e6f9bb4d0a5eb62fc2a)
* [Col\_MListSetAt](col_list_8h.md#group__mlist__words_1ga5dc6434e4a1ba966bba2c87a58f76cc5)
* [Col\_MListSetLength](col_list_8h.md#group__mlist__words_1ga939319b0240b3bd7f0713b09b4dc08dc)
* [Col\_RepeatList](col_list_8h.md#group__list__words_1ga8eaa742be1b1c2b87f7c850ad71a77bd)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)
* [Col\_TraverseListChunksN](col_list_8h.md#group__list__words_1ga4c20735036c60715f55cfd4e8d96825f)
* [ColListIterUpdateTraversalInfo](col_list_8h.md#group__list__words_1ga65a0551576955013edca89745886b5dd)
* [ConvertToMutableAt](col_list_8c.md#group__mlist__words_1gaa3d6a25f3eb845452f465bb66f67ab51)
* [GetArms](col_list_8c.md#group__list__words_1gae54e7f540a28fbe6f26002e43c9374c7)
* [GetChunk](col_list_8c.md#group__list__words_1ga58a993e823bf134e615c862a546d6d87)
* [MergeListChunksProc](col_list_8c.md#group__list__words_1ga2daef27844161dd9c5b16f571bb2e01a)
* [MListInsert](col_list_8c.md#group__mlist__words_1gaa62fa49dc7f988e33978dbf3e030ebf8)
* [MListRemove](col_list_8c.md#group__mlist__words_1ga7ebc7b3f5a8c87880fa31e51cde19c04)
* [MListSetAt](col_list_8c.md#group__mlist__words_1ga417914bde37aaaa2b6948efe8f157046)
* [NewMConcatList](col_list_8c.md#group__mlist__words_1ga0cf94fe0b5f417772755b0bd02851e5a)
* [NextChunk](col_list_8c.md#group__list__words_1gab2a98e33d9052b36916698a4aa2e276e)
* [SplitMutableAt](col_list_8c.md#group__mlist__words_1ga87232eb482742f39972b97a2f1084fed)
* [UpdateMConcatNode](col_list_8c.md#group__mlist__words_1ga0bd3ec88c86ba531f946c9e1cd271793)

<a id="group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6"></a>
### Function Col\_ListLoopLength

![][public]

```cpp
size_t Col_ListLoopLength(Col_Word list)
```

Get the loop length of the list.

**Return values**:

* **0**: for non-cyclic lists
* **<>0**: length of terminal loop for cyclic lists.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **list**: Not a list.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: List to get loop length for.

**Return type**: EXTERN size_t

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [ListChunkTraverseInfo::list](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a20400ffc5ef99dfd4ffd684870cf51f0)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)
* [WORD\_TYPE\_VECTOR](col_word_int_8h.md#group__words_1gadf6c66e5c2f9fcdf213ae40d253c153f)
* [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

**Referenced by**:

* [Col\_CircularList](col_list_8h.md#group__list__words_1ga327fd0dc9444ab63bdc30d5f62eee4ea)
* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [Col\_ConcatListsA](col_list_8h.md#group__list__words_1ga964905c5c77b7ccb72a1fe3805cd70c7)
* [Col\_ListInsert](col_list_8h.md#group__list__words_1ga8ab7ea672c85e35028769758674e26f1)
* [Col\_ListIterBegin](col_list_8h.md#group__list__words_1gab82314e6a85f5b68b646efe5c9fe0200)
* [Col\_ListIterForward](col_list_8h.md#group__list__words_1ga12a0d04173010d29d1bf59492b829e7f)
* [Col\_ListRemove](col_list_8h.md#group__list__words_1gad3a369cfb544159866851398efcf50a8)
* [Col\_ListReplace](col_list_8h.md#group__list__words_1ga7286de8fbf801b507b4dd9ea422153c0)
* [Col\_MListInsert](col_list_8h.md#group__mlist__words_1ga453cc8131623ff7f2981ffe3cca75797)
* [Col\_MListLoop](col_list_8h.md#group__mlist__words_1ga717d5a5fa372406df49ae460436b923e)
* [Col\_MListRemove](col_list_8h.md#group__mlist__words_1ga0ff728019a9d46b0a7c0096007203915)
* [Col\_MListReplace](col_list_8h.md#group__mlist__words_1gaecb4269ea8b60e6f9bb4d0a5eb62fc2a)
* [Col\_MListSetAt](col_list_8h.md#group__mlist__words_1ga5dc6434e4a1ba966bba2c87a58f76cc5)
* [Col\_MListSetLength](col_list_8h.md#group__mlist__words_1ga939319b0240b3bd7f0713b09b4dc08dc)
* [Col\_RepeatList](col_list_8h.md#group__list__words_1ga8eaa742be1b1c2b87f7c850ad71a77bd)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)

<a id="group__list__words_1ga825ecbc9aa5b7e650e45b347d8aab253"></a>
### Function Col\_ListDepth

![][public]

```cpp
unsigned char Col_ListDepth(Col_Word list)
```

Get the depth of the list.

**Returns**:

The list depth.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **list**: Not a list.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: List to get depth for.

**Return type**: EXTERN unsigned char

**References**:

* [GetDepth](col_list_8c.md#group__list__words_1ga789c836420f8a8885fa25abd032c4b10)
* [ListChunkTraverseInfo::list](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a20400ffc5ef99dfd4ffd684870cf51f0)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

<a id="group__list__words_1ga7550908ac1ceae597c8283afbaa7f813"></a>
### Function Col\_ListAt

![][public]

```cpp
Col_Word Col_ListAt(Col_Word list, size_t index)
```

Get the element of a list at a given position.

**Return values**:

* **nil**: if **index** past end of **list**.
* **element**: at given index otherwise.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **list**: Not a list.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: List to get element from.
* size_t **index**: Element index.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [Col\_ListIterAt](col_list_8h.md#group__list__words_1gab3ec4df728a607ea0dd60cf85e8dde1a)
* [Col\_ListIterBegin](col_list_8h.md#group__list__words_1gab82314e6a85f5b68b646efe5c9fe0200)
* [Col\_ListIterEnd](col_list_8h.md#group__list__words_1ga0025ae0a8a58c10285c5c131acd1d9e9)
* [ListChunkTraverseInfo::list](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a20400ffc5ef99dfd4ffd684870cf51f0)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)

## Immutable List Operations

!> **Warning** \
Works with mutable or immutable lists and vectors, however mutable words may be frozen in the process.

<a id="group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1"></a>
### Function Col\_Sublist

![][public]

```cpp
Col_Word Col_Sublist(Col_Word list, size_t first, size_t last)
```

Create a new list that is a sublist of another.

We try to minimize the overhead as much as possible, such as:
* identity.

* create vectors for small sublists.

* sublists of sublists point to original list.

* sublists of concats point to the deepest superset sublist.








**Returns**:

When first is past the end of the (acyclic) list, or **last** is before **first**, an empty list. Else, a list representing the sublist.

In any case the resulting sublist is never cyclic.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **list**: Not a list.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: The list to extract the sublist from.
* size_t **first**: Index of first character in sublist.
* size_t **last**: Index of last character in sublist.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)
* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [Col\_ConcatListsV](col_list_8h.md#group__list__words_1ga77aca8145dd519155802b77a89788576)
* [COL\_LIST](col_word_8h.md#group__words_1gafaaad5bdc900622b1387bcb1f32f61c3)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [Col\_RepeatList](col_list_8h.md#group__list__words_1ga8eaa742be1b1c2b87f7c850ad71a77bd)
* [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)
* [FreezeMList](col_list_8c.md#group__mlist__words_1ga095c6e932019c53fc81e4beaddf0334b)
* [MergeListChunksInfo::length](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info_1a1dfa080c13a4eee1fce0bb2c0a2624c8)
* [MAX\_SHORT\_LIST\_LENGTH](col_list_8c.md#group__list__words_1gabe73f76756546f55d2b797a323ab79d5)
* [MAX\_SHORT\_MVECTOR\_LENGTH](col_list_8c.md#group__list__words_1gacd4b42724e38ce4618b6afe3363a3f4c)
* [MergeListChunksProc](col_list_8c.md#group__list__words_1ga2daef27844161dd9c5b16f571bb2e01a)
* [Col\_CustomListType::sublistProc](struct_col___custom_list_type.md#struct_col___custom_list_type_1a431969013f0382f5d3c9d8f9902d07c3)
* [Col\_CustomListType::type](struct_col___custom_list_type.md#struct_col___custom_list_type_1aa5adf5f0d567cbd47a4fe6090e7e88cb)
* [Col\_CustomWordType::type](struct_col___custom_word_type.md#struct_col___custom_word_type_1af9482efe5a6408bc622320619c3ccf9f)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [MergeListChunksInfo::vector](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info_1ae40e462380046ec92b2bb08c9172ecd6)
* [VOIDLIST\_MAX\_LENGTH](col_word_int_8h.md#group__voidlist__words_1gabf4f6876ac71e3f3368c711756459baf)
* [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)
* [WORD\_CONCATLIST\_DEPTH](col_list_int_8h.md#group__concatlist__words_1ga6e5dad2a2ae231641e9c8bc89debb338)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_LEFT\_LENGTH](col_list_int_8h.md#group__concatlist__words_1gab757f4a3117b72ba7ceab9202cf2c2f4)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_LIST\_EMPTY](col_word_int_8h.md#group__voidlist__words_1ga8b7c20d2cdcdf8f3bc58589d757cf53b)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)
* [WORD\_SUBLIST\_FIRST](col_list_int_8h.md#group__sublist__words_1ga18e2c99a420ebeabe9f94b4166c44033)
* [WORD\_SUBLIST\_INIT](col_list_int_8h.md#group__sublist__words_1ga0e3fe32fa3ae20489dc8d38db3148c1b)
* [WORD\_SUBLIST\_SOURCE](col_list_int_8h.md#group__sublist__words_1ga448ba72120af1a5d655107b05479424c)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_CUSTOM](col_word_int_8h.md#group__words_1ga8babfbc77291680db519873c91efdd4c)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)
* [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb)
* [WORD\_TYPEINFO](col_word_int_8h.md#group__custom__words_1gafc962791c45a5dd5bb034050444084be)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)
* [WORD\_VOIDLIST\_NEW](col_word_int_8h.md#group__voidlist__words_1ga76a437807f61cfb3d4066867ae82d8d4)

**Referenced by**:

* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [Col\_ListInsert](col_list_8h.md#group__list__words_1ga8ab7ea672c85e35028769758674e26f1)
* [Col\_ListRemove](col_list_8h.md#group__list__words_1gad3a369cfb544159866851398efcf50a8)
* [Col\_MListRemove](col_list_8h.md#group__mlist__words_1ga0ff728019a9d46b0a7c0096007203915)
* [Col\_MListSetLength](col_list_8h.md#group__mlist__words_1ga939319b0240b3bd7f0713b09b4dc08dc)
* [ConvertToMutableAt](col_list_8c.md#group__mlist__words_1gaa3d6a25f3eb845452f465bb66f67ab51)
* [GetArms](col_list_8c.md#group__list__words_1gae54e7f540a28fbe6f26002e43c9374c7)
* [MListRemove](col_list_8c.md#group__mlist__words_1ga7ebc7b3f5a8c87880fa31e51cde19c04)
* [SplitMutableAt](col_list_8c.md#group__mlist__words_1ga87232eb482742f39972b97a2f1084fed)

<a id="group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b"></a>
### Function Col\_ConcatLists

![][public]

```cpp
Col_Word Col_ConcatLists(Col_Word left, Col_Word right)
```

Concatenate lists.

Concatenation forms self-balanced binary trees. See [List Tree Balancing](#group__list__words_1list_tree_balancing) for more information.






**Returns**:

A list representing the concatenation of both lists.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **left**: Not a list.
* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **right**: Not a list.
* **[COL\_ERROR\_LISTLENGTH\_CONCAT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a837fcf987426e821513e2fdaffcdf55e)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **length(left+right)**: Concat list too large.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **left**: Left part.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **right**: Right part.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)
* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_CircularList](col_list_8h.md#group__list__words_1ga327fd0dc9444ab63bdc30d5f62eee4ea)
* [COL\_LIST](col_word_8h.md#group__words_1gafaaad5bdc900622b1387bcb1f32f61c3)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)
* [Col\_CustomListType::concatProc](struct_col___custom_list_type.md#struct_col___custom_list_type_1a00a0cd040234db58920ca192bdcc077c)
* [FreezeMList](col_list_8c.md#group__mlist__words_1ga095c6e932019c53fc81e4beaddf0334b)
* [GetArms](col_list_8c.md#group__list__words_1gae54e7f540a28fbe6f26002e43c9374c7)
* [GetDepth](col_list_8c.md#group__list__words_1ga789c836420f8a8885fa25abd032c4b10)
* [MergeListChunksInfo::length](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info_1a1dfa080c13a4eee1fce0bb2c0a2624c8)
* [MAX\_SHORT\_LIST\_LENGTH](col_list_8c.md#group__list__words_1gabe73f76756546f55d2b797a323ab79d5)
* [MAX\_SHORT\_MVECTOR\_LENGTH](col_list_8c.md#group__list__words_1gacd4b42724e38ce4618b6afe3363a3f4c)
* [MergeListChunksProc](col_list_8c.md#group__list__words_1ga2daef27844161dd9c5b16f571bb2e01a)
* [Col\_CustomListType::type](struct_col___custom_list_type.md#struct_col___custom_list_type_1aa5adf5f0d567cbd47a4fe6090e7e88cb)
* [Col\_CustomWordType::type](struct_col___custom_word_type.md#struct_col___custom_word_type_1af9482efe5a6408bc622320619c3ccf9f)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [VALUECHECK\_LISTLENGTH\_CONCAT](col_list_int_8h.md#group__list__words_1gaaa7ad7fed7ada4b019dd93db3db402a4)
* [MergeListChunksInfo::vector](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info_1ae40e462380046ec92b2bb08c9172ecd6)
* [VOIDLIST\_MAX\_LENGTH](col_word_int_8h.md#group__voidlist__words_1gabf4f6876ac71e3f3368c711756459baf)
* [WORD\_CONCATLIST\_INIT](col_list_int_8h.md#group__concatlist__words_1ga8cef632de8ecc768d7926efa76a5d550)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)
* [WORD\_SUBLIST\_FIRST](col_list_int_8h.md#group__sublist__words_1ga18e2c99a420ebeabe9f94b4166c44033)
* [WORD\_SUBLIST\_LAST](col_list_int_8h.md#group__sublist__words_1ga4e00a849390732cdf829ce42d9c42417)
* [WORD\_SUBLIST\_SOURCE](col_list_int_8h.md#group__sublist__words_1ga448ba72120af1a5d655107b05479424c)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_CUSTOM](col_word_int_8h.md#group__words_1ga8babfbc77291680db519873c91efdd4c)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)
* [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb)
* [WORD\_TYPEINFO](col_word_int_8h.md#group__custom__words_1gafc962791c45a5dd5bb034050444084be)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)
* [WORD\_VOIDLIST\_NEW](col_word_int_8h.md#group__voidlist__words_1ga76a437807f61cfb3d4066867ae82d8d4)

**Referenced by**:

* [Col\_ConcatListsA](col_list_8h.md#group__list__words_1ga964905c5c77b7ccb72a1fe3805cd70c7)
* [Col\_ListInsert](col_list_8h.md#group__list__words_1ga8ab7ea672c85e35028769758674e26f1)
* [Col\_ListRemove](col_list_8h.md#group__list__words_1gad3a369cfb544159866851398efcf50a8)
* [Col\_NewList](col_list_8h.md#group__list__words_1ga0dc0d4d58c4e694683753211151b0fc7)
* [Col\_RepeatList](col_list_8h.md#group__list__words_1ga8eaa742be1b1c2b87f7c850ad71a77bd)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)

<a id="group__list__words_1ga964905c5c77b7ccb72a1fe3805cd70c7"></a>
### Function Col\_ConcatListsA

![][public]

```cpp
Col_Word Col_ConcatListsA(size_t number, const Col_Word *lists)
```

Concatenate several lists given in an array.

Concatenation is done recursively, by halving the array until it contains one or two elements, at this point we respectively return the element or use [Col\_ConcatLists()](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b).






**Returns**:

A list representing the concatenation of all lists.

**Exceptions**:

* **[COL\_ERROR\_GENERIC](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a02ae949dee6fd3c78c849d7e7af414e4)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **number == 0**: Generic error.

**Parameters**:

* size_t **number**: Size of **lists** array.
* listsconst [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **words**: Array of lists to concatenate in order.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [Col\_CopyMList](col_list_8h.md#group__mlist__words_1gaff1c8024a9ca1e6a6f33de4fe7344e81)
* [COL\_ERROR\_GENERIC](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a02ae949dee6fd3c78c849d7e7af414e4)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [VALUECHECK](col_internal_8h.md#group__error_1ga711949fdb4e6c4bf5218075c1db5439b)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)

**Referenced by**:

* [Col\_ConcatListsNV](col_list_8h.md#group__list__words_1ga291672360f258f14e6a92fe0a4232a19)

<a id="group__list__words_1ga291672360f258f14e6a92fe0a4232a19"></a>
### Function Col\_ConcatListsNV

![][public]

```cpp
Col_Word Col_ConcatListsNV(size_t number, ...)
```

Concatenate lists given as arguments.

The argument list is first copied into a stack-allocated array then passed to [Col\_ConcatListsA()](col_list_8h.md#group__list__words_1ga964905c5c77b7ccb72a1fe3805cd70c7).






**Returns**:

A list representing the concatenation of all lists.




**See also**: [Col\_ConcatListsA](col_list_8h.md#group__list__words_1ga964905c5c77b7ccb72a1fe3805cd70c7), [Col\_ConcatListsV](col_list_8h.md#group__list__words_1ga77aca8145dd519155802b77a89788576)

**Exceptions**:

* **[COL\_ERROR\_GENERIC](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a02ae949dee6fd3c78c849d7e7af414e4)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **number == 0**: Generic error.

**Parameters**:

* size_t **number**: Number of arguments following.
* ......: Lists to concatenate in order.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [Col\_ConcatListsA](col_list_8h.md#group__list__words_1ga964905c5c77b7ccb72a1fe3805cd70c7)
* [COL\_ERROR\_GENERIC](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a02ae949dee6fd3c78c849d7e7af414e4)
* [VALUECHECK](col_internal_8h.md#group__error_1ga711949fdb4e6c4bf5218075c1db5439b)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)

<a id="group__list__words_1ga8eaa742be1b1c2b87f7c850ad71a77bd"></a>
### Function Col\_RepeatList

![][public]

```cpp
Col_Word Col_RepeatList(Col_Word list, size_t count)
```

Create a list formed by the repetition of a source list.

This method is based on recursive concatenations of the list following the bit pattern of the count factor. Doubling a list simply consists of a concat with itself. In the end the resulting tree is very compact, and only a minimal number of extraneous cells are allocated during the balancing process (and will be eventually collected).






**Returns**:

A list representing the repetition of the source list. A list repeated zero times is empty. A cyclic list repeated more than once is identity.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **list**: Not a list.
* **[COL\_ERROR\_LISTLENGTH\_REPEAT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a40d3bab7c4bdb30d0f50e04735e7c7ef)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **length(list)*count**: Repeat list too large.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: The list to repeat.
* size_t **count**: Repetition factor.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [Col\_CopyMList](col_list_8h.md#group__mlist__words_1gaff1c8024a9ca1e6a6f33de4fe7344e81)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [VALUECHECK\_LISTLENGTH\_REPEAT](col_list_int_8h.md#group__list__words_1gaae9920f9fe6d6eb79ed243523cbf3321)
* [WORD\_LIST\_EMPTY](col_word_int_8h.md#group__voidlist__words_1ga8b7c20d2cdcdf8f3bc58589d757cf53b)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)

**Referenced by**:

* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)

<a id="group__list__words_1ga327fd0dc9444ab63bdc30d5f62eee4ea"></a>
### Function Col\_CircularList

![][public]

```cpp
Col_Word Col_CircularList(Col_Word core)
```

Create a circular list from a regular list.

**Returns**:

If the list is empty or cyclic, identity. Else a new circular list from the given core.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **list**: Not a list.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **core**: The core list.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [Col\_CopyMList](col_list_8h.md#group__mlist__words_1gaff1c8024a9ca1e6a6f33de4fe7344e81)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [WORD\_CIRCLIST\_NEW](col_word_int_8h.md#group__circlist__words_1ga51827e999fb41c560624c93dd2a8770b)
* [WORD\_LIST\_EMPTY](col_word_int_8h.md#group__voidlist__words_1ga8b7c20d2cdcdf8f3bc58589d757cf53b)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)

**Referenced by**:

* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [Col\_ListInsert](col_list_8h.md#group__list__words_1ga8ab7ea672c85e35028769758674e26f1)
* [Col\_ListRemove](col_list_8h.md#group__list__words_1gad3a369cfb544159866851398efcf50a8)

<a id="group__list__words_1ga8ab7ea672c85e35028769758674e26f1"></a>
### Function Col\_ListInsert

![][public]

```cpp
Col_Word Col_ListInsert(Col_Word into, size_t index, Col_Word list)
```

Insert a list into another one, just before the given insertion point, taking cyclicity into account.

As target list is immutable, this results in a new list.





Insertion past the end of the list results in a concatenation.






?> Only perform minimal tests to prevent overflow, basic ops should perform further optimizations anyway.


**Returns**:

The resulting list.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **into**: Not a list.
* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **list**: Not a list.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **into**: Target list to insert into.
* size_t **index**: Index of insertion point.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: List to insert.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_CircularList](col_list_8h.md#group__list__words_1ga327fd0dc9444ab63bdc30d5f62eee4ea)
* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [Col\_CopyMList](col_list_8h.md#group__mlist__words_1gaff1c8024a9ca1e6a6f33de4fe7344e81)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)

**Referenced by**:

* [Col\_ListReplace](col_list_8h.md#group__list__words_1ga7286de8fbf801b507b4dd9ea422153c0)

<a id="group__list__words_1gad3a369cfb544159866851398efcf50a8"></a>
### Function Col\_ListRemove

![][public]

```cpp
Col_Word Col_ListRemove(Col_Word list, size_t first, size_t last)
```

Remove a range of elements from a list, taking cyclicity into account.

As target list is immutable, this results in a new list.






?> Only perform minimal tests to prevent overflow, basic ops should perform further optimizations anyway.


**Returns**:

The resulting list.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **list**: Not a list.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: List to remove sequence from.
* size_t **first**: Index of first character in range to remove.
* size_t **last**: Index of last character in range to remove.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_CircularList](col_list_8h.md#group__list__words_1ga327fd0dc9444ab63bdc30d5f62eee4ea)
* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [Col\_CopyMList](col_list_8h.md#group__mlist__words_1gaff1c8024a9ca1e6a6f33de4fe7344e81)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_LIST\_EMPTY](col_word_int_8h.md#group__voidlist__words_1ga8b7c20d2cdcdf8f3bc58589d757cf53b)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

**Referenced by**:

* [Col\_ListReplace](col_list_8h.md#group__list__words_1ga7286de8fbf801b507b4dd9ea422153c0)

<a id="group__list__words_1ga7286de8fbf801b507b4dd9ea422153c0"></a>
### Function Col\_ListReplace

![][public]

```cpp
Col_Word Col_ListReplace(Col_Word list, size_t first, size_t last, Col_Word with)
```

Replace a range of elements in a list with another, taking cyclicity into account.

As target list is immutable, this results in a new list.





Replacement is a combination of [Col\_ListRemove()](col_list_8h.md#group__list__words_1gad3a369cfb544159866851398efcf50a8) and [Col\_ListInsert()](col_list_8h.md#group__list__words_1ga8ab7ea672c85e35028769758674e26f1).






?> Only perform minimal tests to prevent overflow, basic ops should perform further optimizations anyway.


**Returns**:

The resulting list.




**See also**: [Col\_ListInsert](col_list_8h.md#group__list__words_1ga8ab7ea672c85e35028769758674e26f1), [Col\_ListRemove](col_list_8h.md#group__list__words_1gad3a369cfb544159866851398efcf50a8)

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **list**: Not a list.
* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **with**: Not a list.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: Original list.
* size_t **first**: Index of first element in range to remove.
* size_t **last**: Index of last element in range to replace.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **with**: Replacement list.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [Col\_CopyMList](col_list_8h.md#group__mlist__words_1gaff1c8024a9ca1e6a6f33de4fe7344e81)
* [Col\_ListInsert](col_list_8h.md#group__list__words_1ga8ab7ea672c85e35028769758674e26f1)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [Col\_ListRemove](col_list_8h.md#group__list__words_1gad3a369cfb544159866851398efcf50a8)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)

<a id="group__list__words_1ga77aca8145dd519155802b77a89788576"></a>
### Macro Col\_ConcatListsV

![][public]

```cpp
#define Col_ConcatListsV     _Col_ConcatListsV(_, first, ##__VA_ARGS__)( first ,... )
```

Variadic macro version of [Col\_ConcatListsNV()](col_list_8h.md#group__list__words_1ga291672360f258f14e6a92fe0a4232a19) that deduces its number of arguments automatically.

**Parameters**:

* **first**: First list to concatenate.
* **...**: Next lists to concatenate.



**See also**: [COL\_ARGCOUNT](col_utils_8h.md#group__utils_1gabbb0e58841406f54d444d40625a2c4fe)



## Immutable List Traversal

?> Works with mutable or immutable lists and vectors.

<a id="group__list__words_1gae4fff149231b539311b16e990f1f53a0"></a>
### Typedef Col\_ListChunksTraverseProc

![][public]

**Definition**: `include/colList.h` (line 138)

```cpp
typedef int() Col_ListChunksTraverseProc(size_t index, size_t length, size_t number, const Col_Word **chunks, Col_ClientData clientData)
```

Function signature of list traversal procs.

**Parameters**:

* **index**: List-relative index where chunks begin.
* **length**: Length of chunks.
* **number**: Number of chunks.
* **chunks**: Array of chunks. When chunk is NULL, means the index is past the end of the traversed list. When chunk is [COL\_LISTCHUNK\_VOID](col_list_8h.md#group__list__words_1ga8c1b8e76380329e1264c12e77bfbf30d), means the traversed list is a void list.
* **clientData**: Opaque client data. Same value as passed to [Col\_TraverseListChunks()](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861) procedure family.


**Return values**:

* **zero**: to continue traversal.
* **non-zero**: to stop traversal. Value is returned as result of [Col\_TraverseListChunks()](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861) and related procs.



**Return type**: int()

<a id="group__list__words_1ga4c20735036c60715f55cfd4e8d96825f"></a>
### Function Col\_TraverseListChunksN

![][public]

```cpp
int Col_TraverseListChunksN(size_t number, Col_Word *lists, size_t start, size_t max, Col_ListChunksTraverseProc *proc, Col_ClientData clientData, size_t *lengthPtr)
```

Iterate over the chunks of a number of lists.

For each traversed chunk, **proc** is called back with the opaque data as well as the position within the lists. If it returns a nonzero result then the iteration ends.






?> The algorithm is naturally recursive but this implementation avoids recursive calls thanks to a stack-allocated backtracking structure.


**Return values**:

* **-1**: if no traversal was performed.
* **int**: last returned value of **proc** otherwise.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **lists[i]**: Not a list.
* **[COL\_ERROR\_GENERIC](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a02ae949dee6fd3c78c849d7e7af414e4)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **proc == NULL**: Generic error.

**Parameters**:

* size_t **number**: Number of lists to traverse.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **lists**: Array of lists to traverse.
* size_t **start**: Index of first character.
* size_t **max**: Max number of characters.
* [Col\_ListChunksTraverseProc](col_list_8h.md#group__list__words_1gae4fff149231b539311b16e990f1f53a0) * **proc**: Callback proc called on each chunk.
* [Col\_ClientData](colibri_8h.md#group__basic__types_1ga52e127a5c635bcb88f252efd210ca1a5) **clientData**: Opaque data passed as is to above **proc**.
* size_t * **lengthPtr**: [in,out] If non-NULL, incremented by the total number of characters traversed upon completion.

**Return type**: EXTERN int

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [ListChunkTraverseInfo::backtracks](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a144abdb4fe0550b8ba692e8b55783ba9)
* [COL\_ERROR\_GENERIC](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a02ae949dee6fd3c78c849d7e7af414e4)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [GetChunk](col_list_8c.md#group__list__words_1ga58a993e823bf134e615c862a546d6d87)
* [GetDepth](col_list_8c.md#group__list__words_1ga789c836420f8a8885fa25abd032c4b10)
* [ListChunkTraverseInfo::list](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a20400ffc5ef99dfd4ffd684870cf51f0)
* [ListChunkTraverseInfo::max](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a35fa386ec4ea893f6ffe08686b25f886)
* [ListChunkTraverseInfo::maxDepth](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a85956991f5f276c5c79469ff07e45d29)
* [NextChunk](col_list_8c.md#group__list__words_1gab2a98e33d9052b36916698a4aa2e276e)
* [ListChunkTraverseInfo::prevDepth](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a22b5d4d14134f49bc4cdc2cf5d930e0d)
* [ListChunkTraverseInfo::start](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a4a0b34787f1830365a9d6d060f464075)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [VALUECHECK](col_internal_8h.md#group__error_1ga711949fdb4e6c4bf5218075c1db5439b)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

<a id="group__list__words_1ga95d08cdde802b1411fbbb812412a8861"></a>
### Function Col\_TraverseListChunks

![][public]

```cpp
int Col_TraverseListChunks(Col_Word list, size_t start, size_t max, int reverse, Col_ListChunksTraverseProc *proc, Col_ClientData clientData, size_t *lengthPtr)
```

Iterate over the chunks of a list.

For each traversed chunk, **proc** is called back with the opaque data as well as the position within the list. If it returns a nonzero result then the iteration ends.






?> The algorithm is naturally recursive but this implementation avoids recursive calls thanks to a stack-allocated backtracking structure. This procedure is an optimized version of [Col\_TraverseListChunksN()](col_list_8h.md#group__list__words_1ga4c20735036c60715f55cfd4e8d96825f) that also supports reverse traversal.


**Return values**:

* **-1**: if no traversal was performed.
* **int**: last returned value of **proc** otherwise.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **list**: Not a list.
* **[COL\_ERROR\_GENERIC](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a02ae949dee6fd3c78c849d7e7af414e4)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **proc == NULL**: Generic error.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: List to traverse.
* size_t **start**: Index of first character.
* size_t **max**: Max number of characters.
* int **reverse**: Whether to traverse in reverse order.
* [Col\_ListChunksTraverseProc](col_list_8h.md#group__list__words_1gae4fff149231b539311b16e990f1f53a0) * **proc**: Callback proc called on each chunk.
* [Col\_ClientData](colibri_8h.md#group__basic__types_1ga52e127a5c635bcb88f252efd210ca1a5) **clientData**: Opaque data passed as is to above **proc**.
* size_t * **lengthPtr**: [in,out] If non-NULL, incremented by the total number of characters traversed upon completion.

**Return type**: EXTERN int

**References**:

* [ListChunkTraverseInfo::backtracks](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a144abdb4fe0550b8ba692e8b55783ba9)
* [COL\_ERROR\_GENERIC](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a02ae949dee6fd3c78c849d7e7af414e4)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [GetChunk](col_list_8c.md#group__list__words_1ga58a993e823bf134e615c862a546d6d87)
* [GetDepth](col_list_8c.md#group__list__words_1ga789c836420f8a8885fa25abd032c4b10)
* [ListChunkTraverseInfo::list](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a20400ffc5ef99dfd4ffd684870cf51f0)
* [ListChunkTraverseInfo::max](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a35fa386ec4ea893f6ffe08686b25f886)
* [ListChunkTraverseInfo::maxDepth](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a85956991f5f276c5c79469ff07e45d29)
* [NextChunk](col_list_8c.md#group__list__words_1gab2a98e33d9052b36916698a4aa2e276e)
* [ListChunkTraverseInfo::prevDepth](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a22b5d4d14134f49bc4cdc2cf5d930e0d)
* [ListChunkTraverseInfo::start](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a4a0b34787f1830365a9d6d060f464075)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [VALUECHECK](col_internal_8h.md#group__error_1ga711949fdb4e6c4bf5218075c1db5439b)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

**Referenced by**:

* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [ConvertToMutableAt](col_list_8c.md#group__mlist__words_1gaa3d6a25f3eb845452f465bb66f67ab51)
* [MListInsert](col_list_8c.md#group__mlist__words_1gaa62fa49dc7f988e33978dbf3e030ebf8)
* [MListRemove](col_list_8c.md#group__mlist__words_1ga7ebc7b3f5a8c87880fa31e51cde19c04)

<a id="group__list__words_1ga8c1b8e76380329e1264c12e77bfbf30d"></a>
### Macro COL\_LISTCHUNK\_VOID

![][public]

```cpp
#define COL_LISTCHUNK_VOID     (([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)-1)
```

Value passed as chunk pointer to list traversal procs when traversing void lists.

**See also**: [Col\_ListChunksTraverseProc](col_list_8h.md#group__list__words_1gae4fff149231b539311b16e990f1f53a0)



## Immutable List Iteration

!> **Warning** \
Works with mutable or immutable lists and vectors, however modifying a mutable list during iteration results in undefined behavior.

<a id="group__list__words_1ga1e84f299b255cb8d8f55d8681d056411"></a>
### Typedef ColListIterLeafAtProc

![][private]

**Definition**: `include/colList.h` (line 186)

```cpp
typedef Col_Word() ColListIterLeafAtProc(Col_Word leaf, size_t index)
```

Helper for list iterators to access elements in leaves.

**Parameters**:

* **leaf**: Leaf node.
* **index**: Leaf-relative index of element.


**Returns**:

Element at given index.



**See also**: [ColListIterator](struct_col_list_iterator.md#struct_col_list_iterator)



**Return type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)()

<a id="group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb"></a>
### Typedef Col\_ListIterator

![][public]

**Definition**: `include/colList.h` (line 240)

```cpp
typedef ColListIterator Col_ListIterator[1]
```

List iterator.

Encapsulates the necessary info to iterate & access list data transparently.






?> Datatype is opaque. Fields should not be accessed by client code.
\
Each iterator takes 8 words on the stack.
\
The type is defined as a single-element array of the internal datatype:
\

* declared variables allocate the right amount of space on the stack,

* calls use pass-by-reference (i.e. pointer) and not pass-by-value,

* forbidden as return type.



**Return type**: [ColListIterator](struct_col_list_iterator.md#struct_col_list_iterator)

<a id="group__list__words_1gab82314e6a85f5b68b646efe5c9fe0200"></a>
### Function Col\_ListIterBegin

![][public]

```cpp
int Col_ListIterBegin(Col_ListIterator it, Col_Word list, size_t index)
```

Initialize the list iterator so that it points to the **index**-th element within the list.

If **index** points past the end of the list, the iterator is initialized to the end iterator.






**Return values**:

* **<>0**: if the iterator looped over the cyclic list.
* **0**: in all other cases.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **list**: Not a list.

**Parameters**:

* [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) **it**: Iterator to initialize.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: List to iterate over.
* size_t **index**: Index of first element to iterate.

**Return type**: EXTERN int

**References**:

* [Col\_ListIterSetNull](col_list_8h.md#group__list__words_1ga11978e75247d90159d4e7a7da8f4615e)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)

**Referenced by**:

* [Col\_ListAt](col_list_8h.md#group__list__words_1ga7550908ac1ceae597c8283afbaa7f813)

<a id="group__list__words_1ga0f1d7cb546b01d0a346e40a49671d0ec"></a>
### Function Col\_ListIterFirst

![][public]

```cpp
void Col_ListIterFirst(Col_ListIterator it, Col_Word list)
```

Initialize the list iterator so that it points to the first character within the list.

If list is empty, the iterator is initialized to the end iterator.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **list**: Not a list.

**Parameters**:

* [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) **it**: Iterator to initialize.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: List to iterate over.

**Return type**: EXTERN void

**References**:

* [Col\_ListIterSetNull](col_list_8h.md#group__list__words_1ga11978e75247d90159d4e7a7da8f4615e)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)

<a id="group__list__words_1ga39ef512be590fc88aca2f6d9668c6ddb"></a>
### Function Col\_ListIterLast

![][public]

```cpp
void Col_ListIterLast(Col_ListIterator it, Col_Word list)
```

Initialize the list iterator so that it points to the last character within the list.

If list is empty, the iterator is initialized to the end iterator.






?> If the list is cyclic, will point to the last character of the core.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **list**: Not a list.

**Parameters**:

* [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) **it**: Iterator to initialize.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: List to iterate over.

**Return type**: EXTERN void

**References**:

* [Col\_ListIterSetNull](col_list_8h.md#group__list__words_1ga11978e75247d90159d4e7a7da8f4615e)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)

<a id="group__list__words_1ga16e66915236c82fa9eb9695e6d899686"></a>
### Function Col\_ListIterArray

![][public]

```cpp
void Col_ListIterArray(Col_ListIterator it, size_t length, const Col_Word *elements)
```

Initialize the list iterator so that it points to the first element in an array.





**Parameters**:

* [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) **it**: Iterator to initialize.
* size_t **length**: Number of elements in array.
* const [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **elements**: Array of elements.

**Return type**: EXTERN void

**References**:

* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)

<a id="group__list__words_1gaa199f0b545340f4ee41512f5d9d1bcd0"></a>
### Function Col\_ListIterCompare

![][public]

```cpp
int Col_ListIterCompare(const Col_ListIterator it1, const Col_ListIterator it2)
```

Compare two iterators by their respective positions.



**Exceptions**:

* **[COL\_ERROR\_LISTITER](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35acd609f7a6870d0404ced98da497d3834)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **it1**: Invalid list iterator.
* **[COL\_ERROR\_LISTITER](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35acd609f7a6870d0404ced98da497d3834)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **it2**: Invalid list iterator.


**Return values**:

* **-1**: if **it1** before **it2**.
* **1**: if **it1** after **it2**.
* **0**: if **it1** and **it2** are equal.

**Parameters**:

* const [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) **it1**: First iterator.
* const [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) **it2**: Second iterator.

**Return type**: EXTERN int

**References**:

* [TYPECHECK\_LISTITER](col_list_int_8h.md#group__list__words_1ga4c1fffdf0457591c87c4fc4efbc18f0f)

<a id="group__list__words_1ga86a67cbc053b8ac15627484285aea972"></a>
### Function Col\_ListIterMoveTo

![][public]

```cpp
int Col_ListIterMoveTo(Col_ListIterator it, size_t index)
```

Move the iterator to the given absolute position.

**Return values**:

* **<>0**: if the iterator looped over the cyclic list.
* **0**: in all other cases.



**Parameters**:

* [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) **it**: The iterator to move.
* size_t **index**: Position to move to.

**Return type**: EXTERN int

**References**:

* [Col\_ListIterBackward](col_list_8h.md#group__list__words_1ga76affcec789dc4314c2f666779c59837)
* [Col\_ListIterForward](col_list_8h.md#group__list__words_1ga12a0d04173010d29d1bf59492b829e7f)

**Referenced by**:

* [Col\_ListIterBackward](col_list_8h.md#group__list__words_1ga76affcec789dc4314c2f666779c59837)

<a id="group__list__words_1ga12a0d04173010d29d1bf59492b829e7f"></a>
### Function Col\_ListIterForward

![][public]

```cpp
int Col_ListIterForward(Col_ListIterator it, size_t nb)
```

Move the iterator to the **nb**-th next element.

**Return values**:

* **<>0**: if the iterator looped over the cyclic list.
* **0**: in all other cases.

**Exceptions**:

* **[COL\_ERROR\_LISTITER](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35acd609f7a6870d0404ced98da497d3834)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **it**: Invalid list iterator.
* **[COL\_ERROR\_LISTITER\_END](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a192c0a7221d1094ac6ac886abb8982c9)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **it**: List iterator at end. (only when **nb** != 0)

**Parameters**:

* [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) **it**: The iterator to move.
* size_t **nb**: Offset to move forward.

**Return type**: EXTERN int

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_ListIterBackward](col_list_8h.md#group__list__words_1ga76affcec789dc4314c2f666779c59837)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [TYPECHECK\_LISTITER](col_list_int_8h.md#group__list__words_1ga4c1fffdf0457591c87c4fc4efbc18f0f)
* [VALUECHECK\_LISTITER](col_list_int_8h.md#group__list__words_1gac14cf510ccb681127bed16e1433f2b51)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)

**Referenced by**:

* [Col\_ListIterMoveTo](col_list_8h.md#group__list__words_1ga86a67cbc053b8ac15627484285aea972)

<a id="group__list__words_1ga76affcec789dc4314c2f666779c59837"></a>
### Function Col\_ListIterBackward

![][public]

```cpp
void Col_ListIterBackward(Col_ListIterator it, size_t nb)
```

Move the iterator backward to the **nb**-th previous element.

?> If moved before the beginning of list, **it** is set at end. This means that backward iterators will loop forever if unchecked against [Col\_ListIterEnd()](col_list_8h.md#group__list__words_1ga0025ae0a8a58c10285c5c131acd1d9e9).

**Exceptions**:

* **[COL\_ERROR\_LISTITER](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35acd609f7a6870d0404ced98da497d3834)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **it**: Invalid list iterator.

**Parameters**:

* [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) **it**: The iterator to move.
* size_t **nb**: Offset to move backward.

**Return type**: EXTERN void

**References**:

* [Col\_ListIterEnd](col_list_8h.md#group__list__words_1ga0025ae0a8a58c10285c5c131acd1d9e9)
* [Col\_ListIterMoveTo](col_list_8h.md#group__list__words_1ga86a67cbc053b8ac15627484285aea972)
* [TYPECHECK\_LISTITER](col_list_int_8h.md#group__list__words_1ga4c1fffdf0457591c87c4fc4efbc18f0f)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)

**Referenced by**:

* [Col\_ListIterForward](col_list_8h.md#group__list__words_1ga12a0d04173010d29d1bf59492b829e7f)
* [Col\_ListIterMoveTo](col_list_8h.md#group__list__words_1ga86a67cbc053b8ac15627484285aea972)

<a id="group__list__words_1ga65a0551576955013edca89745886b5dd"></a>
### Function ColListIterUpdateTraversalInfo

![][private]

```cpp
Col_Word ColListIterUpdateTraversalInfo(ColListIterator *it)
```

Get the chunk containing the element at the current iterator position.

Traversal info is updated lazily, each time element data needs to be retrieved. This means that a blind iteration over an arbitrarily complex list is on average no more computationally intensive than over a flat array (chunk retrieval is O(log n)).






**Returns**:

Current element.

**Exceptions**:

* **[COL\_ERROR\_LISTITER\_END](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a192c0a7221d1094ac6ac886abb8982c9)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **it**: List iterator at end.

**Parameters**:

* [ColListIterator](struct_col_list_iterator.md#struct_col_list_iterator) * **it**: The iterator to update.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [ColListIterator::chunk](struct_col_list_iterator.md#struct_col_list_iterator_1a5ab651baa3c1d996362c4002fd794dfb)
* [Col\_CustomListType::chunkAtProc](struct_col___custom_list_type.md#struct_col___custom_list_type_1a71964a691bc5b9ff15a6441cecb9825c)
* [COL\_LIST](col_word_8h.md#group__words_1gafaaad5bdc900622b1387bcb1f32f61c3)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [COL\_NIL](col_word_8h.md#group__words_1ga36da6e6dce11922f4b30d9b331cf0f62)
* [Col\_CustomListType::elementAtProc](struct_col___custom_list_type.md#struct_col___custom_list_type_1a70aab7ac3b4b86103df2f9e49c1cadfc)
* [ColListIterator::index](struct_col_list_iterator.md#struct_col_list_iterator_1aa2af90fd6c4555d40bd97cf9643a55a7)
* [ColListIterator::length](struct_col_list_iterator.md#struct_col_list_iterator_1aed4f40384c36e99739b843fd488be5fb)
* [ColListIterator::list](struct_col_list_iterator.md#struct_col_list_iterator_1a96b5bb57b535f6d91149d079cb1f659b)
* [Col\_CustomListType::type](struct_col___custom_list_type.md#struct_col___custom_list_type_1aa5adf5f0d567cbd47a4fe6090e7e88cb)
* [Col\_CustomWordType::type](struct_col___custom_word_type.md#struct_col___custom_word_type_1af9482efe5a6408bc622320619c3ccf9f)
* [VALUECHECK\_LISTITER](col_list_int_8h.md#group__list__words_1gac14cf510ccb681127bed16e1433f2b51)
* [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_LEFT\_LENGTH](col_list_int_8h.md#group__concatlist__words_1gab757f4a3117b72ba7ceab9202cf2c2f4)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)
* [WORD\_SUBLIST\_FIRST](col_list_int_8h.md#group__sublist__words_1ga18e2c99a420ebeabe9f94b4166c44033)
* [WORD\_SUBLIST\_SOURCE](col_list_int_8h.md#group__sublist__words_1ga448ba72120af1a5d655107b05479424c)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_CUSTOM](col_word_int_8h.md#group__words_1ga8babfbc77291680db519873c91efdd4c)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)
* [WORD\_TYPE\_VECTOR](col_word_int_8h.md#group__words_1gadf6c66e5c2f9fcdf213ae40d253c153f)
* [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb)
* [WORD\_TYPEINFO](col_word_int_8h.md#group__custom__words_1gafc962791c45a5dd5bb034050444084be)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)
* [WORD\_VECTOR\_ELEMENTS](col_vector_int_8h.md#group__vector__words_1ga3a15150382d791225479cfbcad0c0e33)

<a id="group__list__words_1ga0de33ba7a6b1d3f780a544f40abda4fe"></a>
### Macro COL\_LISTITER\_NULL

![][public]

```cpp
#define COL_LISTITER_NULL
```

Static initializer for null list iterators.

**See also**: [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb), [Col\_ListIterNull](col_list_8h.md#group__list__words_1ga875bb73d6d35111b84e38f139a6846ba)



<a id="group__list__words_1ga875bb73d6d35111b84e38f139a6846ba"></a>
### Macro Col\_ListIterNull

![][public]

```cpp
#define Col_ListIterNull( it )
```

Test whether iterator is null (e.g. it has been set to [COL\_LISTITER\_NULL](col_list_8h.md#group__list__words_1ga0de33ba7a6b1d3f780a544f40abda4fe) or [Col\_ListIterSetNull()](col_list_8h.md#group__list__words_1ga11978e75247d90159d4e7a7da8f4615e)).

!> **Warning** \
This uninitialized state renders it unusable for any call. Use with caution.


**Parameters**:

* **it**: The [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) to test.


!> **Warning** \
Argument **it** is referenced several times by the macro. Make sure to avoid any side effect.


**Return values**:

* **zero**: if iterator if not null.
* **non-zero**: if iterator is null.





**See also**: [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb), [COL\_LISTITER\_NULL](col_list_8h.md#group__list__words_1ga0de33ba7a6b1d3f780a544f40abda4fe), [Col\_ListIterSetNull](col_list_8h.md#group__list__words_1ga11978e75247d90159d4e7a7da8f4615e)



<a id="group__list__words_1ga11978e75247d90159d4e7a7da8f4615e"></a>
### Macro Col\_ListIterSetNull

![][public]

```cpp
#define Col_ListIterSetNull     memset((it), 0, sizeof(*(it)))( it )
```

Set an iterator to null.

**Parameters**:

* **it**: The [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) to initialize.


!> **Warning** \
Argument **it** is referenced several times by the macro. Make sure to avoid any side effect.



<a id="group__list__words_1ga3eaa6c9abbfa40ebc794a6bf061d5dd0"></a>
### Macro Col\_ListIterList

![][public]

```cpp
#define Col_ListIterList     ((it)->list)( it )
```

Get list for iterator.

**Parameters**:

* **it**: The [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) to access.


**Return values**:

* **WORD_NIL**: if iterating over array (see [Col\_ListIterArray()](col_list_8h.md#group__list__words_1ga16e66915236c82fa9eb9695e6d899686)).
* **list**: if iterating over list.



<a id="group__list__words_1ga8b9420e166c0a3f55c8615cdee8621ef"></a>
### Macro Col\_ListIterLength

![][public]

```cpp
#define Col_ListIterLength     ((it)->length)( it )
```

Get length of the iterated sequence.

**Parameters**:

* **it**: The [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) to access.


**Returns**:

Length of iterated sequence.



<a id="group__list__words_1ga5735491b3c5ae48e35965368a7f56ee2"></a>
### Macro Col\_ListIterIndex

![][public]

```cpp
#define Col_ListIterIndex     ((it)->index)( it )
```

Get current index within list for iterator.

**Parameters**:

* **it**: The [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) to access.


**Returns**:

Current index.



<a id="group__list__words_1gab3ec4df728a607ea0dd60cf85e8dde1a"></a>
### Macro Col\_ListIterAt

![][public]

```cpp
#define Col_ListIterAt( it )
```

Get current list element for iterator.

**Parameters**:

* **it**: The [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) to access.


!> **Warning** \
Argument **it** is referenced several times by the macro. Make sure to avoid any side effect.


**Returns**:

Current element.


**Exceptions**:

* **[COL\_ERROR\_LISTITER\_END](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a192c0a7221d1094ac6ac886abb8982c9)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **it**: List iterator at end.



<a id="group__list__words_1ga7dfc8bab191efc9de9ed8730f8a32d33"></a>
### Macro Col\_ListIterNext

![][public]

```cpp
#define Col_ListIterNext( it )
```

Move the iterator to the next element.

**Parameters**:

* **it**: The [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) to move.


!> **Warning** \
Argument **it** is referenced several times by the macro. Make sure to avoid any side effect.


**Return values**:

* **non-zero**: if the iterator looped over the cyclic list.
* **zero**: in all other cases.



**Exceptions**:

* **[COL\_ERROR\_LISTITER](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35acd609f7a6870d0404ced98da497d3834)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **it**: Invalid list iterator.
* **[COL\_ERROR\_LISTITER\_END](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a192c0a7221d1094ac6ac886abb8982c9)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **it**: List iterator at end.


**See also**: [Col\_ListIterForward](col_list_8h.md#group__list__words_1ga12a0d04173010d29d1bf59492b829e7f)



<a id="group__list__words_1ga446651e0e15e138183d8045660465e3b"></a>
### Macro Col\_ListIterPrevious

![][public]

```cpp
#define Col_ListIterPrevious( it )
```

Move the iterator to the previous element.

**Parameters**:

* **it**: The [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) to move.


!> **Warning** \
Argument **it** is referenced several times by the macro. Make sure to avoid any side effect.



**Exceptions**:

* **[COL\_ERROR\_LISTITER](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35acd609f7a6870d0404ced98da497d3834)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **it**: Invalid list iterator.


**See also**: [Col\_ListIterBackward](col_list_8h.md#group__list__words_1ga76affcec789dc4314c2f666779c59837)



<a id="group__list__words_1ga0025ae0a8a58c10285c5c131acd1d9e9"></a>
### Macro Col\_ListIterEnd

![][public]

```cpp
#define Col_ListIterEnd     ((it)->index >= (it)->length)( it )
```

Test whether iterator reached end of list.

**Parameters**:

* **it**: The [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) to test.


!> **Warning** \
Argument **it** is referenced several times by the macro. Make sure to avoid any side effect.


**Return values**:

* **zero**: if iterator if not at end.
* **non-zero**: if iterator is at end.



**See also**: [Col\_ListIterBegin](col_list_8h.md#group__list__words_1gab82314e6a85f5b68b646efe5c9fe0200)



<a id="group__list__words_1gad04f9168a054bd67a6b790c964b2c174"></a>
### Macro Col\_ListIterSet

![][public]

```cpp
#define Col_ListIterSet     (*(it) = *(value))( it ,value )
```

Initialize an iterator with another one's value.

**Parameters**:

* **it**: The [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) to initialize.
* **value**: The [Col\_ListIterator](col_list_8h.md#group__list__words_1ga2793b6b86a3ff97de81eb67f79b46eeb) to copy.



## Immutable List Exceptions

<a id="group__list__words_1ga50e2794d537901134d3d60e509469fa3"></a>
### Macro TYPECHECK\_LIST

![][public]

```cpp
#define TYPECHECK_LIST( word )
```

Type checking macro for lists.

**Parameters**:

* **word**: Checked word.


**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **word**: Not a list.



<a id="group__list__words_1ga1a41d86dcdb925ca6ee2a938f54fb206"></a>
### Macro VALUECHECK\_BOUNDS

![][public]

```cpp
#define VALUECHECK_BOUNDS( index ,length )
```

Value checking macro for lists, ensures that index is within bounds.

**Parameters**:

* **index**: Checked index.
* **length**: List length.


**Exceptions**:

* **[COL\_ERROR\_LISTINDEX](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a53fb84bc77099761ccc326d067022e4b)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **index < length**: List index out of bounds.



<a id="group__list__words_1gaaa7ad7fed7ada4b019dd93db3db402a4"></a>
### Macro VALUECHECK\_LISTLENGTH\_CONCAT

![][public]

```cpp
#define VALUECHECK_LISTLENGTH_CONCAT( length1 ,length2 )
```

Value checking macro for lists, ensures that combined lengths of two concatenated lists don't exceed the maximum value.

**Parameters**:

* **length1 length2**: Checked lengths.


**Exceptions**:

* **[COL\_ERROR\_LISTLENGTH\_CONCAT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a837fcf987426e821513e2fdaffcdf55e)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **length1+length2**: Concat list too large.



<a id="group__list__words_1gaae9920f9fe6d6eb79ed243523cbf3321"></a>
### Macro VALUECHECK\_LISTLENGTH\_REPEAT

![][public]

```cpp
#define VALUECHECK_LISTLENGTH_REPEAT( length ,count )
```

Value checking macro for lists, ensures that length of a repeated list doesn't exceed the maximum value.

**Parameters**:

* **length count**: Checked length and repetition factor.


**Exceptions**:

* **[COL\_ERROR\_LISTLENGTH\_CONCAT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a837fcf987426e821513e2fdaffcdf55e)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **length*count**: Concat list too large.



## List Iterator Exceptions

<a id="group__list__words_1ga4c1fffdf0457591c87c4fc4efbc18f0f"></a>
### Macro TYPECHECK\_LISTITER

![][public]

```cpp
#define TYPECHECK_LISTITER( it )
```

Type checking macro for list iterators.

**Parameters**:

* **it**: Checked iterator.


**Exceptions**:

* **[COL\_ERROR\_LISTITER](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35acd609f7a6870d0404ced98da497d3834)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **it**: Invalid list iterator.



**See also**: [Col\_ListIterNull](col_list_8h.md#group__list__words_1ga875bb73d6d35111b84e38f139a6846ba)



<a id="group__list__words_1gac14cf510ccb681127bed16e1433f2b51"></a>
### Macro VALUECHECK\_LISTITER

![][public]

```cpp
#define VALUECHECK_LISTITER( it )
```

Value checking macro for list iterators, ensures that iterator is not at end.

**Parameters**:

* **it**: Checked iterator.


**Exceptions**:

* **[COL\_ERROR\_LISTITER\_END](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a192c0a7221d1094ac6ac886abb8982c9)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **it**: List iterator at end.



**See also**: [Col\_ListIterEnd](col_list_8h.md#group__list__words_1ga0025ae0a8a58c10285c5c131acd1d9e9)



## Functions

<a id="group__list__words_1ga2daef27844161dd9c5b16f571bb2e01a"></a>
### Function MergeListChunksProc

![][private]
![][static]

```cpp
static int MergeListChunksProc(size_t index, size_t length, size_t number, const Col_Word **chunks, Col_ClientData clientData)
```

List traversal procedure used to concatenate all portions of lists into one to fit wthin one vector word.

Follows [Col\_ListChunksTraverseProc()](col_list_8h.md#group__list__words_1gae4fff149231b539311b16e990f1f53a0) signature.






**Returns**:

Always zero, info returned through **clientData**.



**See also**: [MergeListChunksInfo](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info)



**Parameters**:

* size_t **index**: List-relative index where chunks begin.
* size_t **length**: Length of chunks.
* size_t **number**: Number of chunks. Always 1.
* const [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) ** **chunks**: Array of chunks. First element never NULL.
* [Col\_ClientData](colibri_8h.md#group__basic__types_1ga52e127a5c635bcb88f252efd210ca1a5) **clientData**: [in,out] Points to [MergeListChunksInfo](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info).

**Return type**: int

**References**:

* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)
* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [COL\_LISTCHUNK\_VOID](col_list_8h.md#group__list__words_1ga8c1b8e76380329e1264c12e77bfbf30d)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [MergeListChunksInfo::length](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info_1a1dfa080c13a4eee1fce0bb2c0a2624c8)
* [MergeListChunksInfo::vector](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info_1ae40e462380046ec92b2bb08c9172ecd6)
* [VECTOR\_SIZE](col_vector_int_8h.md#group__vector__words_1ga3859b99c15ff0bc766748dc0042ea026)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb)
* [WORD\_VECTOR\_ELEMENTS](col_vector_int_8h.md#group__vector__words_1ga3a15150382d791225479cfbcad0c0e33)
* [WORD\_VECTOR\_INIT](col_vector_int_8h.md#group__vector__words_1gace3048c9e9c238e1405c64c19228212d)
* [WORD\_VOIDLIST\_LENGTH](col_word_int_8h.md#group__voidlist__words_1gaa336b44598700785e9f948fdd3a0da58)

**Referenced by**:

* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [ConvertToMutableAt](col_list_8c.md#group__mlist__words_1gaa3d6a25f3eb845452f465bb66f67ab51)
* [MListInsert](col_list_8c.md#group__mlist__words_1gaa62fa49dc7f988e33978dbf3e030ebf8)
* [MListRemove](col_list_8c.md#group__mlist__words_1ga7ebc7b3f5a8c87880fa31e51cde19c04)

<a id="group__list__words_1ga789c836420f8a8885fa25abd032c4b10"></a>
### Function GetDepth

![][private]
![][static]

```cpp
static unsigned char GetDepth(Col_Word list)
```

Get the depth of the list.

**Returns**:

Depth of list.



**See also**: [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: List node to get depth from.

**Return type**: unsigned char

**References**:

* [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)
* [WORD\_CONCATLIST\_DEPTH](col_list_int_8h.md#group__concatlist__words_1ga6e5dad2a2ae231641e9c8bc89debb338)
* [WORD\_SUBLIST\_DEPTH](col_list_int_8h.md#group__sublist__words_1ga6e3c10b7e63b6f6cabd36f24b69a6ecf)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)

**Referenced by**:

* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [Col\_ListDepth](col_list_8h.md#group__list__words_1ga825ecbc9aa5b7e650e45b347d8aab253)
* [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)
* [Col\_TraverseListChunksN](col_list_8h.md#group__list__words_1ga4c20735036c60715f55cfd4e8d96825f)
* [MListInsert](col_list_8c.md#group__mlist__words_1gaa62fa49dc7f988e33978dbf3e030ebf8)
* [NewMConcatList](col_list_8c.md#group__mlist__words_1ga0cf94fe0b5f417772755b0bd02851e5a)
* [UpdateMConcatNode](col_list_8c.md#group__mlist__words_1ga0bd3ec88c86ba531f946c9e1cd271793)

<a id="group__list__words_1gae54e7f540a28fbe6f26002e43c9374c7"></a>
### Function GetArms

![][private]
![][static]

```cpp
static void GetArms(Col_Word node, Col_Word *leftPtr, Col_Word *rightPtr)
```

Get the left and right arms of a list, i.e. a concat or one of its sublists.

**See also**: [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **node**: List node to extract arms from. Either a sublist or concat list.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **leftPtr**: [out] Left arm.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **rightPtr**: [out] Right arm.

**Return type**: void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [WORD\_CONCATLIST\_DEPTH](col_list_int_8h.md#group__concatlist__words_1ga6e5dad2a2ae231641e9c8bc89debb338)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_LEFT\_LENGTH](col_list_int_8h.md#group__concatlist__words_1gab757f4a3117b72ba7ceab9202cf2c2f4)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_SUBLIST\_DEPTH](col_list_int_8h.md#group__sublist__words_1ga6e3c10b7e63b6f6cabd36f24b69a6ecf)
* [WORD\_SUBLIST\_FIRST](col_list_int_8h.md#group__sublist__words_1ga18e2c99a420ebeabe9f94b4166c44033)
* [WORD\_SUBLIST\_LAST](col_list_int_8h.md#group__sublist__words_1ga4e00a849390732cdf829ce42d9c42417)
* [WORD\_SUBLIST\_SOURCE](col_list_int_8h.md#group__sublist__words_1ga448ba72120af1a5d655107b05479424c)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)

**Referenced by**:

* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [ConvertToMConcatNode](col_list_8c.md#group__mlist__words_1gaa5f68e20c39d5dee85063c39b1310cff)

<a id="group__list__words_1ga58a993e823bf134e615c862a546d6d87"></a>
### Function GetChunk

![][private]
![][static]

```cpp
static void GetChunk(ListChunkTraverseInfo *info, const Col_Word **chunkPtr, int reverse)
```

Get chunk from given traversal info.

**See also**: [ListChunkTraverseInfo](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info), [Col\_TraverseListChunksN](col_list_8h.md#group__list__words_1ga4c20735036c60715f55cfd4e8d96825f), [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)



**Parameters**:

* [ListChunkTraverseInfo](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info) * **info**: Traversal info.
* const [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) ** **chunkPtr**: Chunk info for leaf.
* int **reverse**: Whether to traverse in reverse order.

**Return type**: void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [ListChunkTraverseInfo::backtracks](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a144abdb4fe0550b8ba692e8b55783ba9)
* [Col\_CustomListType::chunkAtProc](struct_col___custom_list_type.md#struct_col___custom_list_type_1a71964a691bc5b9ff15a6441cecb9825c)
* [COL\_LIST](col_word_8h.md#group__words_1gafaaad5bdc900622b1387bcb1f32f61c3)
* [COL\_LISTCHUNK\_VOID](col_list_8h.md#group__list__words_1ga8c1b8e76380329e1264c12e77bfbf30d)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [ListChunkTraverseInfo::e](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a61175342ec174b4fccce7b3842a3bc7e)
* [Col\_CustomListType::elementAtProc](struct_col___custom_list_type.md#struct_col___custom_list_type_1a70aab7ac3b4b86103df2f9e49c1cadfc)
* [ListChunkTraverseInfo::list](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a20400ffc5ef99dfd4ffd684870cf51f0)
* [ListChunkTraverseInfo::max](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a35fa386ec4ea893f6ffe08686b25f886)
* [ListChunkTraverseInfo::prevDepth](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a22b5d4d14134f49bc4cdc2cf5d930e0d)
* [ListChunkTraverseInfo::start](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a4a0b34787f1830365a9d6d060f464075)
* [Col\_CustomListType::type](struct_col___custom_list_type.md#struct_col___custom_list_type_1aa5adf5f0d567cbd47a4fe6090e7e88cb)
* [Col\_CustomWordType::type](struct_col___custom_word_type.md#struct_col___custom_word_type_1af9482efe5a6408bc622320619c3ccf9f)
* [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)
* [WORD\_CONCATLIST\_DEPTH](col_list_int_8h.md#group__concatlist__words_1ga6e5dad2a2ae231641e9c8bc89debb338)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_LEFT\_LENGTH](col_list_int_8h.md#group__concatlist__words_1gab757f4a3117b72ba7ceab9202cf2c2f4)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_SUBLIST\_FIRST](col_list_int_8h.md#group__sublist__words_1ga18e2c99a420ebeabe9f94b4166c44033)
* [WORD\_SUBLIST\_SOURCE](col_list_int_8h.md#group__sublist__words_1ga448ba72120af1a5d655107b05479424c)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_CUSTOM](col_word_int_8h.md#group__words_1ga8babfbc77291680db519873c91efdd4c)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)
* [WORD\_TYPE\_VECTOR](col_word_int_8h.md#group__words_1gadf6c66e5c2f9fcdf213ae40d253c153f)
* [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb)
* [WORD\_TYPEINFO](col_word_int_8h.md#group__custom__words_1gafc962791c45a5dd5bb034050444084be)
* [WORD\_VECTOR\_ELEMENTS](col_vector_int_8h.md#group__vector__words_1ga3a15150382d791225479cfbcad0c0e33)

**Referenced by**:

* [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)
* [Col\_TraverseListChunksN](col_list_8h.md#group__list__words_1ga4c20735036c60715f55cfd4e8d96825f)

<a id="group__list__words_1gab2a98e33d9052b36916698a4aa2e276e"></a>
### Function NextChunk

![][private]
![][static]

```cpp
static void NextChunk(ListChunkTraverseInfo *info, size_t nb, int reverse)
```

Get next chunk in traversal order.

**See also**: [ListChunkTraverseInfo](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info), [Col\_TraverseListChunksN](col_list_8h.md#group__list__words_1ga4c20735036c60715f55cfd4e8d96825f), [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)



**Parameters**:

* [ListChunkTraverseInfo](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info) * **info**: Traversal info.
* size_t **nb**: Number of elements to skip.
* int **reverse**: Whether to traverse in reverse order.

**Return type**: void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [ListChunkTraverseInfo::backtracks](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a144abdb4fe0550b8ba692e8b55783ba9)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [ListChunkTraverseInfo::list](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a20400ffc5ef99dfd4ffd684870cf51f0)
* [ListChunkTraverseInfo::max](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a35fa386ec4ea893f6ffe08686b25f886)
* [ListChunkTraverseInfo::prevDepth](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a22b5d4d14134f49bc4cdc2cf5d930e0d)
* [ListChunkTraverseInfo::start](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info_1a4a0b34787f1830365a9d6d060f464075)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)

**Referenced by**:

* [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)
* [Col\_TraverseListChunksN](col_list_8h.md#group__list__words_1ga4c20735036c60715f55cfd4e8d96825f)

## Macros

<a id="group__list__words_1ga9fa86c89f399dde6c7afb35f6979ade0"></a>
### Macro MAX\_SHORT\_LIST\_SIZE

![][public]

```cpp
#define MAX_SHORT_LIST_SIZE 3
```

Maximum number of cells a short list can take.

This constant controls the creation of short leaves during sublist/concatenation. Lists built this way normally use sublist and concat nodes, but to avoid fragmentation, multiple short lists are flattened into a single vector.



<a id="group__list__words_1gabe73f76756546f55d2b797a323ab79d5"></a>
### Macro MAX\_SHORT\_LIST\_LENGTH

![][public]

```cpp
#define MAX_SHORT_LIST_LENGTH [VECTOR\_MAX\_LENGTH](col_vector_int_8h.md#group__vector__words_1ga2ccb6254308d96069f8187e7b55908d2)([MAX\_SHORT\_LIST\_SIZE](col_list_8c.md#group__list__words_1ga9fa86c89f399dde6c7afb35f6979ade0)*[CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f))
```

Maximum number of elements a short list can take.

**See also**: [MAX\_SHORT\_LIST\_SIZE](col_list_8c.md#group__list__words_1ga9fa86c89f399dde6c7afb35f6979ade0)



<a id="group__list__words_1gaa70449e4e6c12997947763a85f504512"></a>
### Macro MAX\_SHORT\_MVECTOR\_SIZE

![][public]

```cpp
#define MAX_SHORT_MVECTOR_SIZE 10
```

Maximum number of cells a short mutable vector can take.

This constant controls the creation of short mutable vectors during immutable list conversions. When setting an element of a mutable list, if the leaf node containing this element is immutable we convert it to a mutable node of this size in case nearby elements get modified later. This amortizes the conversion overhead over time.



<a id="group__list__words_1gacd4b42724e38ce4618b6afe3363a3f4c"></a>
### Macro MAX\_SHORT\_MVECTOR\_LENGTH

![][public]

```cpp
#define MAX_SHORT_MVECTOR_LENGTH [VECTOR\_MAX\_LENGTH](col_vector_int_8h.md#group__vector__words_1ga2ccb6254308d96069f8187e7b55908d2)([MAX\_SHORT\_MVECTOR\_SIZE](col_list_8c.md#group__list__words_1gaa70449e4e6c12997947763a85f504512)*[CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f))
```

Maximum number of elements a short mutable vector can take.

**See also**: [MAX\_SHORT\_MVECTOR\_SIZE](col_list_8c.md#group__list__words_1gaa70449e4e6c12997947763a85f504512)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)