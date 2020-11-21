<a id="group__mlist__words"></a>
# Mutable Lists

Mutable lists are linear collection of words whose content and length can vary over time.

Mutable lists can be composed of either mutable or immutable lists and vectors. They can be "frozen" and turned into immutable versions.






!> **Warning** \
Mutable vectors _cannot_ be used in place of mutable lists, as the latter can grow indefinitely whereas the former have a maximum length set at creation time.

## Submodules

* [Mutable Concat Lists](group__mconcatlist__words.md#group__mconcatlist__words)

## Mutable List Creation

<a id="group__mlist__words_1ga3b048f22f88eb07685a0d6e12960ca91"></a>
### Function Col\_NewMList

![][public]

```cpp
Col_Word Col_NewMList()
```

Create a new mutable list word.

**Returns**:

The new word.



**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)
* [COL\_LIST](col_word_8h.md#group__words_1gafaaad5bdc900622b1387bcb1f32f61c3)
* [COL\_MLIST](col_word_8h.md#group__words_1ga7ee70ee3416cdb3bafa3c752b92f0c98)
* [WORD\_LIST\_EMPTY](col_word_int_8h.md#group__voidlist__words_1ga8b7c20d2cdcdf8f3bc58589d757cf53b)
* [WORD\_WRAP\_INIT](col_word_int_8h.md#group__word__wrappers_1ga5176d0b6e39fc4a27effa648b0f88d27)
* [WRAP\_FLAG\_MUTABLE](col_word_int_8h.md#group__wrap__words_1ga43906d6d0d0cd314aa9bd099c6d6b767)

<a id="group__mlist__words_1gaff1c8024a9ca1e6a6f33de4fe7344e81"></a>
### Function Col\_CopyMList

![][public]

```cpp
Col_Word Col_CopyMList(Col_Word mlist)
```

Create an immutable list from a mutable list.

If an immutable list is given, it is simply returned. If a mutable list is given, its content is frozen and shared with the new one. If a non-empty mutable vector is given, it is frozen.






**Returns**:

Immutable copy of the mutable list.

**Exceptions**:

* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **mlist**: Not a list.


**Side Effect**:

Freeze mutable list content and turn mutable vectors immutable.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **mlist**: Mutable list to copy.

**Return type**: EXTERN [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [FreezeMList](col_list_8c.md#group__mlist__words_1ga095c6e932019c53fc81e4beaddf0334b)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [WORD\_LIST\_EMPTY](col_word_int_8h.md#group__voidlist__words_1ga8b7c20d2cdcdf8f3bc58589d757cf53b)
* [WORD\_NIL](col_word_8h.md#group__words_1ga29e370264f4e5659ccc5be4de209f065)
* [WORD\_UNWRAP](col_word_int_8h.md#group__word__wrappers_1ga5278e42908e256bb743954bf7745d06c)

**Referenced by**:

* [Col\_CircularList](col_list_8h.md#group__list__words_1ga327fd0dc9444ab63bdc30d5f62eee4ea)
* [Col\_ConcatListsA](col_list_8h.md#group__list__words_1ga964905c5c77b7ccb72a1fe3805cd70c7)
* [Col\_ListInsert](col_list_8h.md#group__list__words_1ga8ab7ea672c85e35028769758674e26f1)
* [Col\_ListRemove](col_list_8h.md#group__list__words_1gad3a369cfb544159866851398efcf50a8)
* [Col\_ListReplace](col_list_8h.md#group__list__words_1ga7286de8fbf801b507b4dd9ea422153c0)
* [Col\_MListInsert](col_list_8h.md#group__mlist__words_1ga453cc8131623ff7f2981ffe3cca75797)
* [Col\_RepeatList](col_list_8h.md#group__list__words_1ga8eaa742be1b1c2b87f7c850ad71a77bd)
* [MListInsert](col_list_8c.md#group__mlist__words_1gaa62fa49dc7f988e33978dbf3e030ebf8)

## Mutable List Operations

<a id="group__mlist__words_1ga939319b0240b3bd7f0713b09b4dc08dc"></a>
### Function Col\_MListSetLength

![][public]

```cpp
void Col_MListSetLength(Col_Word mlist, size_t length)
```

Resize the mutable list.

Newly added elements are set to nil. Removed elements are simply discarded.

**Exceptions**:

* **[COL\_ERROR\_MLIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a12b82359c99d46f81dda5313e88a5611)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **mlist**: Not a mutable list.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **mlist**: Mutable list to resize.
* size_t **length**: New length.

**Return type**: EXTERN void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [Col\_MListInsert](col_list_8h.md#group__mlist__words_1ga453cc8131623ff7f2981ffe3cca75797)
* [Col\_MListRemove](col_list_8h.md#group__mlist__words_1ga0ff728019a9d46b0a7c0096007203915)
* [Col\_MVectorSetLength](col_vector_8h.md#group__mvector__words_1ga549a713508a196e7ed9fb3cceb214e88)
* [Col\_NewList](col_list_8h.md#group__list__words_1ga0dc0d4d58c4e694683753211151b0fc7)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [ConvertToMConcatNode](col_list_8c.md#group__mlist__words_1gaa5f68e20c39d5dee85063c39b1310cff)
* [TYPECHECK\_MLIST](col_list_int_8h.md#group__mlist__words_1ga026e161ab67cc1825a33e750a1ae7af0)
* [UpdateMConcatNode](col_list_8c.md#group__mlist__words_1ga0bd3ec88c86ba531f946c9e1cd271793)
* [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_LIST\_EMPTY](col_word_int_8h.md#group__voidlist__words_1ga8b7c20d2cdcdf8f3bc58589d757cf53b)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)
* [WORD\_WRAP\_SOURCE](col_word_int_8h.md#group__word__wrappers_1ga653feef2b80b11d5c3aae03bddbd422d)

**Referenced by**:

* [Col\_MListInsert](col_list_8h.md#group__mlist__words_1ga453cc8131623ff7f2981ffe3cca75797)

<a id="group__mlist__words_1ga717d5a5fa372406df49ae460436b923e"></a>
### Function Col\_MListLoop

![][public]

```cpp
void Col_MListLoop(Col_Word mlist)
```

Make a mutable list circular.

Do nothing if cyclic (not necessarily circular).

**Exceptions**:

* **[COL\_ERROR\_MLIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a12b82359c99d46f81dda5313e88a5611)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **mlist**: Not a mutable list.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **mlist**: Mutable list to make circular.

**Return type**: EXTERN void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [TYPECHECK\_MLIST](col_list_int_8h.md#group__mlist__words_1ga026e161ab67cc1825a33e750a1ae7af0)
* [WORD\_CIRCLIST\_NEW](col_word_int_8h.md#group__circlist__words_1ga51827e999fb41c560624c93dd2a8770b)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)
* [WORD\_WRAP\_SOURCE](col_word_int_8h.md#group__word__wrappers_1ga653feef2b80b11d5c3aae03bddbd422d)

<a id="group__mlist__words_1ga5dc6434e4a1ba966bba2c87a58f76cc5"></a>
### Function Col\_MListSetAt

![][public]

```cpp
void Col_MListSetAt(Col_Word mlist, size_t index, Col_Word element)
```

Set the element of a mutable list at a given position.



**Exceptions**:

* **[COL\_ERROR\_MLIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a12b82359c99d46f81dda5313e88a5611)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **mlist**: Not a mutable list.
* **[COL\_ERROR\_LISTINDEX](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a53fb84bc77099761ccc326d067022e4b)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **index < [Col\_ListLength(mlist)](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)**: List index out of bounds.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **mlist**: Mutable list to modify.
* size_t **index**: Element index.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **element**: New element.

**Return type**: EXTERN void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [MListSetAt](col_list_8c.md#group__mlist__words_1ga417914bde37aaaa2b6948efe8f157046)
* [TYPECHECK\_MLIST](col_list_int_8h.md#group__mlist__words_1ga026e161ab67cc1825a33e750a1ae7af0)
* [VALUECHECK\_BOUNDS](col_list_int_8h.md#group__list__words_1ga1a41d86dcdb925ca6ee2a938f54fb206)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)
* [WORD\_WRAP\_SOURCE](col_word_int_8h.md#group__word__wrappers_1ga653feef2b80b11d5c3aae03bddbd422d)

<a id="group__mlist__words_1ga453cc8131623ff7f2981ffe3cca75797"></a>
### Function Col\_MListInsert

![][public]

```cpp
void Col_MListInsert(Col_Word into, size_t index, Col_Word list)
```

Insert a list into a mutable list, just before the given insertion point, taking cyclicity into account.



**Exceptions**:

* **[COL\_ERROR\_MLIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a12b82359c99d46f81dda5313e88a5611)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **into**: Not a mutable list.
* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **list**: Not a list.
* **[COL\_ERROR\_LISTLENGTH\_CONCAT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a837fcf987426e821513e2fdaffcdf55e)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **length(into+list)**: Concat list too large.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **into**: Target mutable list to insert into.
* size_t **index**: Index of insertion point.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: List to insert.

**Return type**: EXTERN void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_CopyMList](col_list_8h.md#group__mlist__words_1gaff1c8024a9ca1e6a6f33de4fe7344e81)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [Col\_MListSetLength](col_list_8h.md#group__mlist__words_1ga939319b0240b3bd7f0713b09b4dc08dc)
* [ConvertToMConcatNode](col_list_8c.md#group__mlist__words_1gaa5f68e20c39d5dee85063c39b1310cff)
* [MListInsert](col_list_8c.md#group__mlist__words_1gaa62fa49dc7f988e33978dbf3e030ebf8)
* [NewMConcatList](col_list_8c.md#group__mlist__words_1ga0cf94fe0b5f417772755b0bd02851e5a)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [TYPECHECK\_MLIST](col_list_int_8h.md#group__mlist__words_1ga026e161ab67cc1825a33e750a1ae7af0)
* [UpdateMConcatNode](col_list_8c.md#group__mlist__words_1ga0bd3ec88c86ba531f946c9e1cd271793)
* [VALUECHECK\_LISTLENGTH\_CONCAT](col_list_int_8h.md#group__list__words_1gaaa7ad7fed7ada4b019dd93db3db402a4)
* [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)
* [WORD\_CIRCLIST\_NEW](col_word_int_8h.md#group__circlist__words_1ga51827e999fb41c560624c93dd2a8770b)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)
* [WORD\_WRAP\_SOURCE](col_word_int_8h.md#group__word__wrappers_1ga653feef2b80b11d5c3aae03bddbd422d)

**Referenced by**:

* [Col\_MListReplace](col_list_8h.md#group__mlist__words_1gaecb4269ea8b60e6f9bb4d0a5eb62fc2a)
* [Col\_MListSetLength](col_list_8h.md#group__mlist__words_1ga939319b0240b3bd7f0713b09b4dc08dc)

<a id="group__mlist__words_1ga0ff728019a9d46b0a7c0096007203915"></a>
### Function Col\_MListRemove

![][public]

```cpp
void Col_MListRemove(Col_Word mlist, size_t first, size_t last)
```

Remove a range of elements from a mutable list.



**Exceptions**:

* **[COL\_ERROR\_MLIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a12b82359c99d46f81dda5313e88a5611)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **mlist**: Not a mutable list.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **mlist**: Mutable list to remove sequence from.
* size_t **first**: Index of first element in range to remove.
* size_t **last**: Index of last element in range to remove.

**Return type**: EXTERN void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [ConvertToMConcatNode](col_list_8c.md#group__mlist__words_1gaa5f68e20c39d5dee85063c39b1310cff)
* [MListRemove](col_list_8c.md#group__mlist__words_1ga7ebc7b3f5a8c87880fa31e51cde19c04)
* [NewMConcatList](col_list_8c.md#group__mlist__words_1ga0cf94fe0b5f417772755b0bd02851e5a)
* [TYPECHECK\_MLIST](col_list_int_8h.md#group__mlist__words_1ga026e161ab67cc1825a33e750a1ae7af0)
* [UpdateMConcatNode](col_list_8c.md#group__mlist__words_1ga0bd3ec88c86ba531f946c9e1cd271793)
* [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)
* [WORD\_CIRCLIST\_NEW](col_word_int_8h.md#group__circlist__words_1ga51827e999fb41c560624c93dd2a8770b)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_WRAP](col_word_int_8h.md#group__words_1ga3c604da44ba72f4661d0ac28f6718cac)
* [WORD\_WRAP\_SOURCE](col_word_int_8h.md#group__word__wrappers_1ga653feef2b80b11d5c3aae03bddbd422d)

**Referenced by**:

* [Col\_MListReplace](col_list_8h.md#group__mlist__words_1gaecb4269ea8b60e6f9bb4d0a5eb62fc2a)
* [Col\_MListSetLength](col_list_8h.md#group__mlist__words_1ga939319b0240b3bd7f0713b09b4dc08dc)

<a id="group__mlist__words_1gaecb4269ea8b60e6f9bb4d0a5eb62fc2a"></a>
### Function Col\_MListReplace

![][public]

```cpp
void Col_MListReplace(Col_Word mlist, size_t first, size_t last, Col_Word with)
```

Replace a range of elements in a mutable list with another, taking cyclicity into account.

Replacement is a combination of [Col\_MListRemove()](col_list_8h.md#group__mlist__words_1ga0ff728019a9d46b0a7c0096007203915) and [Col\_MListInsert()](col_list_8h.md#group__mlist__words_1ga453cc8131623ff7f2981ffe3cca75797).






?> Only perform minimal tests to prevent overflow, basic ops should perform further optimizations anyway.




**See also**: [Col\_MListInsert](col_list_8h.md#group__mlist__words_1ga453cc8131623ff7f2981ffe3cca75797), [Col\_MListRemove](col_list_8h.md#group__mlist__words_1ga0ff728019a9d46b0a7c0096007203915)

**Exceptions**:

* **[COL\_ERROR\_MLIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a12b82359c99d46f81dda5313e88a5611)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **mlist**: Not a mutable list.
* **[COL\_ERROR\_LIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **with**: Not a list.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **mlist**: Mutable list to modify.
* size_t **first**: Index of first element in range to replace.
* size_t **last**: Index of last element in range to replace.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **with**: Replacement list.

**Return type**: EXTERN void

**References**:

* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_ListLoopLength](col_list_8h.md#group__list__words_1gadf33dcf3fbbb192c17ae521920a14db6)
* [Col\_MListInsert](col_list_8h.md#group__mlist__words_1ga453cc8131623ff7f2981ffe3cca75797)
* [Col\_MListRemove](col_list_8h.md#group__mlist__words_1ga0ff728019a9d46b0a7c0096007203915)
* [TYPECHECK\_LIST](col_list_int_8h.md#group__list__words_1ga50e2794d537901134d3d60e509469fa3)
* [TYPECHECK\_MLIST](col_list_int_8h.md#group__mlist__words_1ga026e161ab67cc1825a33e750a1ae7af0)

## Mutable List Exceptions

<a id="group__mlist__words_1ga026e161ab67cc1825a33e750a1ae7af0"></a>
### Macro TYPECHECK\_MLIST

![][public]

```cpp
#define TYPECHECK_MLIST( word )
```

Type checking macro for mutable lists.

**Parameters**:

* **word**: Checked word.


**Exceptions**:

* **[COL\_ERROR\_MLIST](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a12b82359c99d46f81dda5313e88a5611)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **word**: Not a mutable list.



## Functions

<a id="group__mlist__words_1ga095c6e932019c53fc81e4beaddf0334b"></a>
### Function FreezeMList

![][private]
![][static]

```cpp
static void FreezeMList(Col_Word mlist)
```

Freeze mutable list recursively.

This changes mutable words to immutable versions inplace.







**See also**: [Col\_CopyMList](col_list_8h.md#group__mlist__words_1gaff1c8024a9ca1e6a6f33de4fe7344e81)

**Side Effect**:

Turn mutable vectors immutable. This is done by simply changing the type ID.


**Side Effect**:

Freeze mutable concat list node. This is done by changing the type ID and recursing on child nodes.

**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **mlist**: Mutable list to freeze.

**Return type**: void

**References**:

* [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_PINNED](col_word_int_8h.md#group__regular__words_1gad20cf13be09a354418d8615e6f2f2193)
* [WORD\_SET\_PINNED](col_word_int_8h.md#group__regular__words_1ga7ae30ab27827ba70ae1265b5f637101a)
* [WORD\_SET\_TYPEID](col_word_int_8h.md#group__predefined__words_1ga52822cf424704829e60b112fe03614b6)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_VECTOR](col_word_int_8h.md#group__words_1gadf6c66e5c2f9fcdf213ae40d253c153f)

**Referenced by**:

* [Col\_ConcatLists](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)
* [Col\_CopyMList](col_list_8h.md#group__mlist__words_1gaff1c8024a9ca1e6a6f33de4fe7344e81)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)

<a id="group__mlist__words_1ga417914bde37aaaa2b6948efe8f157046"></a>
### Function MListSetAt

![][private]
![][static]

```cpp
static void MListSetAt(Col_Word *nodePtr, size_t index, Col_Word element)
```

Set the element of a mutable list at a given position, converting nodes to mutable forms in the process.

?> Bounds checking is done upfront.



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **nodePtr**: [in,out] Mutable list node to modify. May be overwritten if converted to mutable.
* size_t **index**: Element index.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **element**: New element.

**Return type**: void

**References**:

* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [ConvertToMConcatNode](col_list_8c.md#group__mlist__words_1gaa5f68e20c39d5dee85063c39b1310cff)
* [ConvertToMutableAt](col_list_8c.md#group__mlist__words_1gaa3d6a25f3eb845452f465bb66f67ab51)
* [UpdateMConcatNode](col_list_8c.md#group__mlist__words_1ga0bd3ec88c86ba531f946c9e1cd271793)
* [WORD\_CIRCLIST\_CORE](col_word_int_8h.md#group__circlist__words_1ga736b51bba4c6bf3ca55f43d942390d36)
* [WORD\_CIRCLIST\_NEW](col_word_int_8h.md#group__circlist__words_1ga51827e999fb41c560624c93dd2a8770b)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_LEFT\_LENGTH](col_list_int_8h.md#group__concatlist__words_1gab757f4a3117b72ba7ceab9202cf2c2f4)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_SUBLIST\_DEPTH](col_list_int_8h.md#group__sublist__words_1ga6e3c10b7e63b6f6cabd36f24b69a6ecf)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)
* [WORD\_VECTOR\_ELEMENTS](col_vector_int_8h.md#group__vector__words_1ga3a15150382d791225479cfbcad0c0e33)

**Referenced by**:

* [Col\_MListSetAt](col_list_8h.md#group__mlist__words_1ga5dc6434e4a1ba966bba2c87a58f76cc5)

<a id="group__mlist__words_1ga0cf94fe0b5f417772755b0bd02851e5a"></a>
### Function NewMConcatList

![][private]
![][static]

```cpp
static Col_Word NewMConcatList(Col_Word left, Col_Word right)
```

Create a new mutable concat list word.

**Returns**:

The new word.



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **left**: Left part.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **right**: Right part.

**Return type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

**References**:

* [AllocCells](col_gc_8c.md#group__alloc_1gaeec69115deeb3321bdfbb4e42119f806)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [GetDepth](col_list_8c.md#group__list__words_1ga789c836420f8a8885fa25abd032c4b10)
* [WORD\_MCONCATLIST\_INIT](col_list_int_8h.md#group__mconcatlist__words_1ga3c52f1fb2dc05f7d3822519d7be9ee7b)

**Referenced by**:

* [Col\_MListInsert](col_list_8h.md#group__mlist__words_1ga453cc8131623ff7f2981ffe3cca75797)
* [Col\_MListRemove](col_list_8h.md#group__mlist__words_1ga0ff728019a9d46b0a7c0096007203915)
* [ConvertToMConcatNode](col_list_8c.md#group__mlist__words_1gaa5f68e20c39d5dee85063c39b1310cff)
* [ConvertToMutableAt](col_list_8c.md#group__mlist__words_1gaa3d6a25f3eb845452f465bb66f67ab51)
* [MListInsert](col_list_8c.md#group__mlist__words_1gaa62fa49dc7f988e33978dbf3e030ebf8)
* [MListRemove](col_list_8c.md#group__mlist__words_1ga7ebc7b3f5a8c87880fa31e51cde19c04)
* [SplitMutableAt](col_list_8c.md#group__mlist__words_1ga87232eb482742f39972b97a2f1084fed)

<a id="group__mlist__words_1gaa3d6a25f3eb845452f465bb66f67ab51"></a>
### Function ConvertToMutableAt

![][private]
![][static]

```cpp
static void ConvertToMutableAt(Col_Word *nodePtr, size_t index)
```

Convert an immutable node (vector, void list or sublist) to mutable around a given index.

?> The original is not modified, a mutable copy is made.



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **nodePtr**: [in,out] Mutable list node to convert. Gets overwritten upon return.
* size_t **index**: Element index.

**Return type**: void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_NewMVector](col_vector_8h.md#group__mvector__words_1ga5409a9871105f346b35ecd06d857e271)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)
* [MergeListChunksInfo::length](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info_1a1dfa080c13a4eee1fce0bb2c0a2624c8)
* [MAX\_SHORT\_MVECTOR\_LENGTH](col_list_8c.md#group__list__words_1gacd4b42724e38ce4618b6afe3363a3f4c)
* [MergeListChunksProc](col_list_8c.md#group__list__words_1ga2daef27844161dd9c5b16f571bb2e01a)
* [NewMConcatList](col_list_8c.md#group__mlist__words_1ga0cf94fe0b5f417772755b0bd02851e5a)
* [MergeListChunksInfo::vector](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info_1ae40e462380046ec92b2bb08c9172ecd6)

**Referenced by**:

* [MListSetAt](col_list_8c.md#group__mlist__words_1ga417914bde37aaaa2b6948efe8f157046)

<a id="group__mlist__words_1gaa5f68e20c39d5dee85063c39b1310cff"></a>
### Function ConvertToMConcatNode

![][private]
![][static]

```cpp
static void ConvertToMConcatNode(Col_Word *nodePtr)
```

Convert an immutable node (concat or sublist) to a mutable concat list.

?> The original is not modified, a mutable copy is made.



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **nodePtr**: [in,out] Mutable list node to convert. Gets overwritten upon return.

**Return type**: void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [GetArms](col_list_8c.md#group__list__words_1gae54e7f540a28fbe6f26002e43c9374c7)
* [NewMConcatList](col_list_8c.md#group__mlist__words_1ga0cf94fe0b5f417772755b0bd02851e5a)
* [WORD\_CONCATLIST\_DEPTH](col_list_int_8h.md#group__concatlist__words_1ga6e5dad2a2ae231641e9c8bc89debb338)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_SUBLIST\_DEPTH](col_list_int_8h.md#group__sublist__words_1ga6e3c10b7e63b6f6cabd36f24b69a6ecf)
* [WORD\_SUBLIST\_SOURCE](col_list_int_8h.md#group__sublist__words_1ga448ba72120af1a5d655107b05479424c)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)

**Referenced by**:

* [Col\_MListInsert](col_list_8h.md#group__mlist__words_1ga453cc8131623ff7f2981ffe3cca75797)
* [Col\_MListRemove](col_list_8h.md#group__mlist__words_1ga0ff728019a9d46b0a7c0096007203915)
* [Col\_MListSetLength](col_list_8h.md#group__mlist__words_1ga939319b0240b3bd7f0713b09b4dc08dc)
* [MListInsert](col_list_8c.md#group__mlist__words_1gaa62fa49dc7f988e33978dbf3e030ebf8)
* [MListRemove](col_list_8c.md#group__mlist__words_1ga7ebc7b3f5a8c87880fa31e51cde19c04)
* [MListSetAt](col_list_8c.md#group__mlist__words_1ga417914bde37aaaa2b6948efe8f157046)
* [UpdateMConcatNode](col_list_8c.md#group__mlist__words_1ga0bd3ec88c86ba531f946c9e1cd271793)

<a id="group__mlist__words_1ga0bd3ec88c86ba531f946c9e1cd271793"></a>
### Function UpdateMConcatNode

![][private]
![][static]

```cpp
static void UpdateMConcatNode(Col_Word node)
```

Update (depth, left length...) & rebalance mutable concat list node.

?> Rebalancing of mutable lists works as for immutable lists (see [Col\_ConcatLists()](col_list_8h.md#group__list__words_1ga73c0f71ee367af68bbad4a4738dfac3b)) except that it is done in-place, whereas for the latter new concat nodes are created in the process. Consequently no extraneous word is created here.



**See also**: [List Tree Balancing](#group__list__words_1list_tree_balancing)



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **node**: Mutable concat list node.

**Return type**: void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [ConvertToMConcatNode](col_list_8c.md#group__mlist__words_1gaa5f68e20c39d5dee85063c39b1310cff)
* [GetDepth](col_list_8c.md#group__list__words_1ga789c836420f8a8885fa25abd032c4b10)
* [WORD\_CONCATLIST\_DEPTH](col_list_int_8h.md#group__concatlist__words_1ga6e5dad2a2ae231641e9c8bc89debb338)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_MCONCATLIST\_INIT](col_list_int_8h.md#group__mconcatlist__words_1ga3c52f1fb2dc05f7d3822519d7be9ee7b)
* [WORD\_PINNED](col_word_int_8h.md#group__regular__words_1gad20cf13be09a354418d8615e6f2f2193)
* [WORD\_SET\_PINNED](col_word_int_8h.md#group__regular__words_1ga7ae30ab27827ba70ae1265b5f637101a)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CIRCLIST](col_word_int_8h.md#group__words_1ga5986ba88af901948fd9a78f422001650)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)

**Referenced by**:

* [Col\_MListInsert](col_list_8h.md#group__mlist__words_1ga453cc8131623ff7f2981ffe3cca75797)
* [Col\_MListRemove](col_list_8h.md#group__mlist__words_1ga0ff728019a9d46b0a7c0096007203915)
* [Col\_MListSetLength](col_list_8h.md#group__mlist__words_1ga939319b0240b3bd7f0713b09b4dc08dc)
* [MListInsert](col_list_8c.md#group__mlist__words_1gaa62fa49dc7f988e33978dbf3e030ebf8)
* [MListRemove](col_list_8c.md#group__mlist__words_1ga7ebc7b3f5a8c87880fa31e51cde19c04)
* [MListSetAt](col_list_8c.md#group__mlist__words_1ga417914bde37aaaa2b6948efe8f157046)

<a id="group__mlist__words_1gaa62fa49dc7f988e33978dbf3e030ebf8"></a>
### Function MListInsert

![][private]
![][static]

```cpp
static void MListInsert(Col_Word *nodePtr, size_t index, Col_Word list)
```

Insert a list into a mutable list node, just before the given insertion point.

Recursively descends into containing node.







**See also**: [Col\_MListInsert](col_list_8h.md#group__mlist__words_1ga453cc8131623ff7f2981ffe3cca75797)



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **nodePtr**: [in,out] Mutable list node to insert into. May be overwritten in the process (e.g. if it gets converted to mutable).
* size_t **index**: Index of insertion point.
* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **list**: List to insert.

**Return type**: void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [CELL\_SIZE](col_conf_8h.md#group__alloc_1ga7a4127f14f16563da90eb3c836bc404f)
* [Col\_CopyMList](col_list_8h.md#group__mlist__words_1gaff1c8024a9ca1e6a6f33de4fe7344e81)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_MVectorSetLength](col_vector_8h.md#group__mvector__words_1ga549a713508a196e7ed9fb3cceb214e88)
* [Col\_NewMVector](col_vector_8h.md#group__mvector__words_1ga5409a9871105f346b35ecd06d857e271)
* [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)
* [ConvertToMConcatNode](col_list_8c.md#group__mlist__words_1gaa5f68e20c39d5dee85063c39b1310cff)
* [GetDepth](col_list_8c.md#group__list__words_1ga789c836420f8a8885fa25abd032c4b10)
* [MergeListChunksInfo::length](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info_1a1dfa080c13a4eee1fce0bb2c0a2624c8)
* [MAX\_SHORT\_MVECTOR\_LENGTH](col_list_8c.md#group__list__words_1gacd4b42724e38ce4618b6afe3363a3f4c)
* [MergeListChunksProc](col_list_8c.md#group__list__words_1ga2daef27844161dd9c5b16f571bb2e01a)
* [NewMConcatList](col_list_8c.md#group__mlist__words_1ga0cf94fe0b5f417772755b0bd02851e5a)
* [SplitMutableAt](col_list_8c.md#group__mlist__words_1ga87232eb482742f39972b97a2f1084fed)
* [UpdateMConcatNode](col_list_8c.md#group__mlist__words_1ga0bd3ec88c86ba531f946c9e1cd271793)
* [MergeListChunksInfo::vector](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info_1ae40e462380046ec92b2bb08c9172ecd6)
* [VECTOR\_MAX\_LENGTH](col_vector_int_8h.md#group__vector__words_1ga2ccb6254308d96069f8187e7b55908d2)
* [WORD\_CONCATLIST\_DEPTH](col_list_int_8h.md#group__concatlist__words_1ga6e5dad2a2ae231641e9c8bc89debb338)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_LEFT\_LENGTH](col_list_int_8h.md#group__concatlist__words_1gab757f4a3117b72ba7ceab9202cf2c2f4)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_MVECTOR\_SIZE](col_vector_int_8h.md#group__mvector__words_1gae997d6e38dc29b20339e997afd28f168)
* [WORD\_SUBLIST\_DEPTH](col_list_int_8h.md#group__sublist__words_1ga6e3c10b7e63b6f6cabd36f24b69a6ecf)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)
* [WORD\_VECTOR\_ELEMENTS](col_vector_int_8h.md#group__vector__words_1ga3a15150382d791225479cfbcad0c0e33)

**Referenced by**:

* [Col\_MListInsert](col_list_8h.md#group__mlist__words_1ga453cc8131623ff7f2981ffe3cca75797)

<a id="group__mlist__words_1ga87232eb482742f39972b97a2f1084fed"></a>
### Function SplitMutableAt

![][private]
![][static]

```cpp
static void SplitMutableAt(Col_Word *nodePtr, size_t index)
```

Split a mutable list node at a given index.





**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **nodePtr**: [in,out] Mutable list node to split. Gets overwritten upon return.
* size_t **index**: Index of split point.

**Return type**: void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_MVectorSetLength](col_vector_8h.md#group__mvector__words_1ga549a713508a196e7ed9fb3cceb214e88)
* [Col\_NewMVector](col_vector_8h.md#group__mvector__words_1ga5409a9871105f346b35ecd06d857e271)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [NewMConcatList](col_list_8c.md#group__mlist__words_1ga0cf94fe0b5f417772755b0bd02851e5a)
* [WORD\_SUBLIST\_DEPTH](col_list_int_8h.md#group__sublist__words_1ga6e3c10b7e63b6f6cabd36f24b69a6ecf)
* [WORD\_SUBLIST\_FIRST](col_list_int_8h.md#group__sublist__words_1ga18e2c99a420ebeabe9f94b4166c44033)
* [WORD\_SUBLIST\_SOURCE](col_list_int_8h.md#group__sublist__words_1ga448ba72120af1a5d655107b05479424c)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CUSTOM](col_word_int_8h.md#group__words_1ga8babfbc77291680db519873c91efdd4c)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)
* [WORD\_TYPE\_VECTOR](col_word_int_8h.md#group__words_1gadf6c66e5c2f9fcdf213ae40d253c153f)
* [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb)
* [WORD\_VECTOR\_ELEMENTS](col_vector_int_8h.md#group__vector__words_1ga3a15150382d791225479cfbcad0c0e33)
* [WORD\_VOIDLIST\_NEW](col_word_int_8h.md#group__voidlist__words_1ga76a437807f61cfb3d4066867ae82d8d4)

**Referenced by**:

* [MListInsert](col_list_8c.md#group__mlist__words_1gaa62fa49dc7f988e33978dbf3e030ebf8)

<a id="group__mlist__words_1ga7ebc7b3f5a8c87880fa31e51cde19c04"></a>
### Function MListRemove

![][private]
![][static]

```cpp
static void MListRemove(Col_Word *nodePtr, size_t first, size_t last)
```

Remove a range of elements from a mutable list.

Recursively descends into containing nodes.







**See also**: [Col\_MListRemove](col_list_8h.md#group__mlist__words_1ga0ff728019a9d46b0a7c0096007203915)



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) * **nodePtr**: [in,out] Mutable list node to modify. May be overwritten in the process (e.g. if it gets converted to mutable).
* size_t **first**: Index of first element in range to remove.
* size_t **last**: Index of last element in range to remove.

**Return type**: void

**References**:

* [ASSERT](col_internal_8h.md#group__error_1gac22830a985e1daed0c9eadba8c6f606e)
* [Col\_ListLength](col_list_8h.md#group__list__words_1ga5eb1c9ddea940171e18817b90301cc03)
* [Col\_MVectorSetLength](col_vector_8h.md#group__mvector__words_1ga549a713508a196e7ed9fb3cceb214e88)
* [Col\_NewMVector](col_vector_8h.md#group__mvector__words_1ga5409a9871105f346b35ecd06d857e271)
* [Col\_Sublist](col_list_8h.md#group__list__words_1gaa26702b61fabf55805c9ef1b2783e7f1)
* [Col\_TraverseListChunks](col_list_8h.md#group__list__words_1ga95d08cdde802b1411fbbb812412a8861)
* [ConvertToMConcatNode](col_list_8c.md#group__mlist__words_1gaa5f68e20c39d5dee85063c39b1310cff)
* [MergeListChunksInfo::length](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info_1a1dfa080c13a4eee1fce0bb2c0a2624c8)
* [MAX\_SHORT\_MVECTOR\_LENGTH](col_list_8c.md#group__list__words_1gacd4b42724e38ce4618b6afe3363a3f4c)
* [MergeListChunksProc](col_list_8c.md#group__list__words_1ga2daef27844161dd9c5b16f571bb2e01a)
* [NewMConcatList](col_list_8c.md#group__mlist__words_1ga0cf94fe0b5f417772755b0bd02851e5a)
* [UpdateMConcatNode](col_list_8c.md#group__mlist__words_1ga0bd3ec88c86ba531f946c9e1cd271793)
* [MergeListChunksInfo::vector](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info_1ae40e462380046ec92b2bb08c9172ecd6)
* [WORD\_CONCATLIST\_LEFT](col_list_int_8h.md#group__concatlist__words_1ga6b758463af55b736f2585d4ebc9d57f3)
* [WORD\_CONCATLIST\_LEFT\_LENGTH](col_list_int_8h.md#group__concatlist__words_1gab757f4a3117b72ba7ceab9202cf2c2f4)
* [WORD\_CONCATLIST\_RIGHT](col_list_int_8h.md#group__concatlist__words_1gaaec42f5b15639059d8422083c596af4e)
* [WORD\_LIST\_EMPTY](col_word_int_8h.md#group__voidlist__words_1ga8b7c20d2cdcdf8f3bc58589d757cf53b)
* [WORD\_SUBLIST\_DEPTH](col_list_int_8h.md#group__sublist__words_1ga6e3c10b7e63b6f6cabd36f24b69a6ecf)
* [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)
* [WORD\_TYPE\_CONCATLIST](col_word_int_8h.md#group__words_1ga8f0a60698d7b383460fe868b1c043f19)
* [WORD\_TYPE\_MCONCATLIST](col_word_int_8h.md#group__words_1ga0c4f44385c099ed03aec5db8ff98c4ee)
* [WORD\_TYPE\_MVECTOR](col_word_int_8h.md#group__words_1ga22d76782e9dfd28846b6eeac3547280f)
* [WORD\_TYPE\_SUBLIST](col_word_int_8h.md#group__words_1gab019a30aca48483424886bf08f7b7cac)
* [WORD\_TYPE\_VOIDLIST](col_word_int_8h.md#group__words_1gad13a9d2efd54cfe0f381fb9c85c4bebb)
* [WORD\_VECTOR\_ELEMENTS](col_vector_int_8h.md#group__vector__words_1ga3a15150382d791225479cfbcad0c0e33)
* [WORD\_VOIDLIST\_NEW](col_word_int_8h.md#group__voidlist__words_1ga76a437807f61cfb3d4066867ae82d8d4)

**Referenced by**:

* [Col\_MListRemove](col_list_8h.md#group__mlist__words_1ga0ff728019a9d46b0a7c0096007203915)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)