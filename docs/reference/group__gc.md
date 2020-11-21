<a id="group__gc"></a>
# Garbage Collection





## GC-Protected Sections

<a id="group__gc_1gae703ee3215a4724ebed8e5a2824e7a7b"></a>
### Function Col\_PauseGC

![][public]

```cpp
void Col_PauseGC()
```

Pause the automatic garbage collection.

Calls can be nested. Code between the outermost pause and resume calls define a GC-protected section.





When the threading model isn't [COL\_SINGLE](colibri_8h.md#group__init_1gaaecbd3c0ddf9f5684b97db76e7338731), blocks as long as a GC is underway.








**See also**: [Col\_TryPauseGC](colibri_8h.md#group__gc_1ga54eefaa11ad5a79b8665fef5cc24c26f), [Col\_ResumeGC](colibri_8h.md#group__gc_1gaf7d4f0dd1996dde366af3f29e9bcc517)



**Parameters**:

* void

**Return type**: EXTERN void

<a id="group__gc_1ga54eefaa11ad5a79b8665fef5cc24c26f"></a>
### Function Col\_TryPauseGC

![][public]

```cpp
int Col_TryPauseGC()
```

Try to pause the automatic garbage collection.

Calls can be nested.






**Return values**:

* **1**: if successful
* **0**: if a GC is underway (this implies the threading model isn't [COL\_SINGLE](colibri_8h.md#group__init_1gaaecbd3c0ddf9f5684b97db76e7338731)). In this case the caller must try again later or use the blocking version.




**See also**: [Col\_PauseGC](colibri_8h.md#group__gc_1gae703ee3215a4724ebed8e5a2824e7a7b), [Col\_ResumeGC](colibri_8h.md#group__gc_1gaf7d4f0dd1996dde366af3f29e9bcc517)



**Parameters**:

* void

**Return type**: EXTERN int

<a id="group__gc_1gaf7d4f0dd1996dde366af3f29e9bcc517"></a>
### Function Col\_ResumeGC

![][public]

```cpp
void Col_ResumeGC()
```

Resume the automatic garbage collection.

Calls can be nested.





Leaving a GC-protected section potentially triggers a GC.






**pre**\
Must be called within a GC-protected section.


**Side Effect**:

May trigger the garbage collection.




**See also**: [Col\_PauseGC](colibri_8h.md#group__gc_1gae703ee3215a4724ebed8e5a2824e7a7b), [Col\_TryPauseGC](colibri_8h.md#group__gc_1ga54eefaa11ad5a79b8665fef5cc24c26f)

**Exceptions**:

* **[COL\_ERROR\_GCPROTECT](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a33f3b4f6762491c50375359e5ffa02f8)**: [[E]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9eae8345daddd8d5e83225f9f88d302f1a0) Outside of a GC-protected section.

**Parameters**:

* void

**Return type**: EXTERN void

## Functions

<a id="group__gc_1ga23c84fc6b7da85d87751fa5788e3f002"></a>
### Function RememberSweepable

![][private]

```cpp
void RememberSweepable(Col_Word word, Col_CustomWordType *type)
```

Remember custom words needing cleanup upon deletion.

Such words are chained in their order of creation, latest being inserted at the head of the list. This implies that cleanup can stop traversing the list at the first custom word that belongs to a non GC'd pool.









**See also**: [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type), [Col\_CustomWordFreeProc](col_word_8h.md#group__custom__words_1ga73db405afd10df91bf70e5507fd63584), [Col\_NewCustomWord](col_word_8h.md#group__custom__words_1gaf9a6d324967159ae7abeb41a3d59cc79)



**Parameters**:

* [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) **word**: The word to declare.
* [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type) * **type**: The word type.

**Return type**: void

<a id="group__gc_1ga911d254f51c4e0b4475330147fbbc545"></a>
### Function SweepUnreachableCells

![][private]
![][static]

```cpp
static void SweepUnreachableCells(GroupData *data, MemoryPool *pool)
```

Perform cleanup for all collected custom words that need sweeping.

**Side Effect**:

Calls each cleaned word's freeProc.





**See also**: [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type), [Col\_CustomWordFreeProc](col_word_8h.md#group__custom__words_1ga73db405afd10df91bf70e5507fd63584), WORD_CUSTOM_NEXT



**Parameters**:

* GroupData * **data**: Group-specific data.
* MemoryPool * **pool**: The pool to sweep.

**Return type**: void

<a id="group__gc_1ga6a8b4c25c3dbf786bfe56975efba86e0"></a>
### Function CleanupSweepables

![][private]

```cpp
void CleanupSweepables(MemoryPool *pool)
```

Perform cleanup for all custom words that need sweeping.

Called during global cleanup.






**Side Effect**:

Calls each cleaned word's freeProc.






**See also**: [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type), [Col\_CustomWordFreeProc](col_word_8h.md#group__custom__words_1ga73db405afd10df91bf70e5507fd63584), WORD_CUSTOM_NEXT, PoolCleanup



**Parameters**:

* MemoryPool * **pool**: The pool to cleanup.

**Return type**: void

<a id="group__gc_1ga09e7926c400756068d91140da241934f"></a>
### Function PromotePages

![][private]
![][static]

```cpp
static void PromotePages(GroupData *data, MemoryPool *pool)
```

Promote non-empty pages to the next pool.

This simply move the pool's pages to the target pool.



**Parameters**:

* GroupData * **data**: Group-specific data.
* MemoryPool * **pool**: The pool to promote page for.

**Return type**: void

<a id="group__gc_1gaa6d1c2ce1d8343c3542c209187b7a47b"></a>
### Function ResetPool

![][private]
![][static]

```cpp
static void ResetPool(MemoryPool *pool)
```

Reset GC-related info and fast cell pointers.





**Parameters**:

* MemoryPool * **pool**: The pool to reset fields for.

**Return type**: void

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[private]: https://img.shields.io/badge/-private-red (private)
[static]: https://img.shields.io/badge/-static-lightgrey (static)