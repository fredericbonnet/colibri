<a id="group__error"></a>
# Error Handling & Debugging





## Generic Error Handling

<a id="group__error_1ga6dab009a0b8c4b4fa080cb9ba1859e9e"></a>
### Enumeration type Col\_ErrorLevel

![][public]

**Definition**: `include/colibri.h` (line 523)

```cpp
enum Col_ErrorLevel {
  COL_FATAL,
  COL_ERROR,
  COL_TYPECHECK,
  COL_VALUECHECK
}
```

Error levels.

**See also**: [Col\_Error](colibri_8h.md#group__error_1ga9a9a9c96b23c489cf8a19a6248fc77b8)



<a id="group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27"></a>
#### Enumerator COL\_FATAL

Fatal error within Colibri, forces process termination.



<a id="group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9eae8345daddd8d5e83225f9f88d302f1a0"></a>
#### Enumerator COL\_ERROR

Error typically caused by the application (bad input...) with potential side effects, leaving the application in a potentially unstable state.

Typically when trying to allocate cells outside a GC-protected section. Default implementation forces process termination.

<a id="group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904"></a>
#### Enumerator COL\_TYPECHECK

Idempotent (i.e. without side effect) type-related error.

Typically when passing words of a bad type in debug mode. Default implementation does not force process termination.

<a id="group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9"></a>
#### Enumerator COL\_VALUECHECK

Idempotent (i.e. without side effect) value-related error.

Typically when using lengths, indices or iterators outside of their validity range. Default implementation does not force process termination.

<a id="group__error_1gaf729acbb7ac74baf6d7a95bed1431abf"></a>
### Typedef Col\_ErrorDomain

![][public]

**Definition**: `include/colibri.h` (line 540)

```cpp
typedef const char* const* Col_ErrorDomain
```

Domain into which error codes are defined.

Domains are merely string tables indexed by error code. Each module (library, application...) can define its own domain with specific error codes and associated messages. The pointer to the message table serves as a unique identifier for various coexisting domains.








**See also**: [Col\_SetErrorProc](colibri_8h.md#group__error_1ga024ea9d85177427fc412c3e469cc1169), [Col\_ErrorLevel](colibri_8h.md#group__error_1ga6dab009a0b8c4b4fa080cb9ba1859e9e)



**Return type**: const char *const  *

<a id="group__error_1gac5151fd715c49a363324d700de68e176"></a>
### Typedef Col\_ErrorProc

![][public]

**Definition**: `include/colibri.h` (line 558)

```cpp
typedef int() Col_ErrorProc(Col_ErrorLevel level, Col_ErrorDomain domain, int code, va_list args)
```

Function signature of custom error handlers.

Colibri provides a default error handler, but applications can define their own.






**Parameters**:

* **domain**: Error domain.
* **level**: Error level.
* **code**: Error code.
* **args**: Remaining arguments passed to domain proc.


**Return values**:

* **0**: to stop further error processing.
* **<>0**: to continue error processing.





**See also**: [Col\_SetErrorProc](colibri_8h.md#group__error_1ga024ea9d85177427fc412c3e469cc1169), [Col\_ErrorLevel](colibri_8h.md#group__error_1ga6dab009a0b8c4b4fa080cb9ba1859e9e), [Col\_ErrorDomain](colibri_8h.md#group__error_1gaf729acbb7ac74baf6d7a95bed1431abf)



**Return type**: int()

<a id="group__error_1ga9a9a9c96b23c489cf8a19a6248fc77b8"></a>
### Function Col\_Error

![][public]

```cpp
void Col_Error(Col_ErrorLevel level, Col_ErrorDomain domain, int code, ...)
```

Signal an error condition.

!> **Attention** \
Default implementation exits the processus when level is [COL\_FATAL](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea47572f7e362007f7b266dbe79e778b27) or [COL\_ERROR](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9eae8345daddd8d5e83225f9f88d302f1a0).




**See also**: [Col\_SetErrorProc](colibri_8h.md#group__error_1ga024ea9d85177427fc412c3e469cc1169), [Col\_ErrorDomain](colibri_8h.md#group__error_1gaf729acbb7ac74baf6d7a95bed1431abf)



**Parameters**:

* [Col\_ErrorLevel](colibri_8h.md#group__error_1ga6dab009a0b8c4b4fa080cb9ba1859e9e) **level**: Error level.
* [Col\_ErrorDomain](colibri_8h.md#group__error_1gaf729acbb7ac74baf6d7a95bed1431abf) **domain**: Error domain.
* int **code**: Error code.
* ......: Remaining arguments passed to domain proc.

**Return type**: EXTERN void

<a id="group__error_1ga4decc218875475a0d74c0ed694060157"></a>
### Function Col\_GetErrorProc

![][public]

```cpp
Col_ErrorProc * Col_GetErrorProc()
```

Get the thread's custom error proc.

**Returns**:

The current error proc (may be NULL).




**See also**: [Col\_Error](colibri_8h.md#group__error_1ga9a9a9c96b23c489cf8a19a6248fc77b8), [Col\_SetErrorProc](colibri_8h.md#group__error_1ga024ea9d85177427fc412c3e469cc1169)



**Return type**: EXTERN [Col\_ErrorProc](colibri_8h.md#group__error_1gac5151fd715c49a363324d700de68e176) *

<a id="group__error_1ga024ea9d85177427fc412c3e469cc1169"></a>
### Function Col\_SetErrorProc

![][public]

```cpp
Col_ErrorProc * Col_SetErrorProc(Col_ErrorProc *proc)
```

Set or reset the thread's custom error proc.

**Returns**:

The old error proc (may be NULL).




**See also**: [Col\_Error](colibri_8h.md#group__error_1ga9a9a9c96b23c489cf8a19a6248fc77b8), [Col\_GetErrorProc](colibri_8h.md#group__error_1ga4decc218875475a0d74c0ed694060157)



**Parameters**:

* [Col\_ErrorProc](colibri_8h.md#group__error_1gac5151fd715c49a363324d700de68e176) * **proc**: The new error proc (may be NULL).

**Return type**: EXTERN [Col\_ErrorProc](colibri_8h.md#group__error_1gac5151fd715c49a363324d700de68e176) *

## Colibri Error Domain

<a id="group__error_1ga729084542ed9eae62009a84d3379ef35"></a>
### Enumeration type Col\_ErrorCode

![][public]

**Definition**: `include/colibri.h` (line 582)

```cpp
enum Col_ErrorCode {
  COL_ERROR_GENERIC,
  COL_ERROR_ASSERTION,
  COL_ERROR_MEMORY,
  COL_ERROR_GCPROTECT,
  COL_ERROR_BOOL,
  COL_ERROR_INT,
  COL_ERROR_FLOAT,
  COL_ERROR_CUSTOMWORD,
  COL_ERROR_CHAR,
  COL_ERROR_STRING,
  COL_ERROR_ROPE,
  COL_ERROR_ROPEINDEX,
  COL_ERROR_ROPELENGTH_CONCAT,
  COL_ERROR_ROPELENGTH_REPEAT,
  COL_ERROR_ROPEITER,
  COL_ERROR_ROPEITER_END,
  COL_ERROR_VECTOR,
  COL_ERROR_MVECTOR,
  COL_ERROR_VECTORLENGTH,
  COL_ERROR_LIST,
  COL_ERROR_MLIST,
  COL_ERROR_LISTINDEX,
  COL_ERROR_LISTLENGTH_CONCAT,
  COL_ERROR_LISTLENGTH_REPEAT,
  COL_ERROR_LISTITER,
  COL_ERROR_LISTITER_END,
  COL_ERROR_MAP,
  COL_ERROR_WORDMAP,
  COL_ERROR_INTMAP,
  COL_ERROR_HASHMAP,
  COL_ERROR_WORDHASHMAP,
  COL_ERROR_INTHASHMAP,
  COL_ERROR_TRIEMAP,
  COL_ERROR_WORDTRIEMAP,
  COL_ERROR_INTTRIEMAP,
  COL_ERROR_MAPITER,
  COL_ERROR_MAPITER_END,
  COL_ERROR_STRBUF,
  COL_ERROR_STRBUF_FORMAT
}
```

Error codes defined in the Colibri domain.

**See also**: [Col\_GetErrorDomain](colibri_8h.md#group__error_1gac5f445b6764ff02059fb14b2fe3eec9c)



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a02ae949dee6fd3c78c849d7e7af414e4"></a>
#### Enumerator COL\_ERROR\_GENERIC

Generic error.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35aa1f5bae36323ffa3c0f77bb8de011468"></a>
#### Enumerator COL\_ERROR\_ASSERTION

Assertion failed.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35aaf4bfe66f629e9292b3de0089a891de3"></a>
#### Enumerator COL\_ERROR\_MEMORY

Memory error.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a33f3b4f6762491c50375359e5ffa02f8"></a>
#### Enumerator COL\_ERROR\_GCPROTECT

Outside of a GC-protected section.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35aa004747a3d62aadf448c12577cf19c7f"></a>
#### Enumerator COL\_ERROR\_BOOL

Not a boolean word.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a832354f2b8d5ee385ab0a6a89ce0b5d5"></a>
#### Enumerator COL\_ERROR\_INT

Not an integer word.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a4388bca7643872d2e7343fee8ed2f616"></a>
#### Enumerator COL\_ERROR\_FLOAT

Not a floating point word.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35aeaf92bf356a97fb87088bd24e551411c"></a>
#### Enumerator COL\_ERROR\_CUSTOMWORD

Not a custom word.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a336970cfb68532435a61ff5e30def4e9"></a>
#### Enumerator COL\_ERROR\_CHAR

Not a character word.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a1b27b71d27f64cf37c74e6776d86166f"></a>
#### Enumerator COL\_ERROR\_STRING

Not a string word.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35aea3a7d079cdddc4cc3b6768a83ef47f4"></a>
#### Enumerator COL\_ERROR\_ROPE

Not a rope.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a64cfaeb2d7abb247399c12567348159b"></a>
#### Enumerator COL\_ERROR\_ROPEINDEX

Rope index out of bounds.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a37a3a6924ed998d6803770696cc7d1dc"></a>
#### Enumerator COL\_ERROR\_ROPELENGTH\_CONCAT

Concat rope too large.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35ab86369e9dd70d4605d05d494cc9aa3b9"></a>
#### Enumerator COL\_ERROR\_ROPELENGTH\_REPEAT

Repeat rope too large.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a45d7cbebed5e870c86dbf2dd6c3a56b7"></a>
#### Enumerator COL\_ERROR\_ROPEITER

Invalid rope iterator.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a6ce849d7cc09cc3484477f792c1d0886"></a>
#### Enumerator COL\_ERROR\_ROPEITER\_END

Rope iterator at end.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a887698395dc0643aa2b4c0863ff6d8d4"></a>
#### Enumerator COL\_ERROR\_VECTOR

Not a vector.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35ade4eb60e2eb216ed151689929db31d1c"></a>
#### Enumerator COL\_ERROR\_MVECTOR

Not a mutable vector.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35af25a0023745659d92b3ebd65d7c43bf3"></a>
#### Enumerator COL\_ERROR\_VECTORLENGTH

Vector too large.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a88271295a774232492c1ebbdc68d6958"></a>
#### Enumerator COL\_ERROR\_LIST

Not a list.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a12b82359c99d46f81dda5313e88a5611"></a>
#### Enumerator COL\_ERROR\_MLIST

Not a mutable list.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a53fb84bc77099761ccc326d067022e4b"></a>
#### Enumerator COL\_ERROR\_LISTINDEX

List index out of bounds.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a837fcf987426e821513e2fdaffcdf55e"></a>
#### Enumerator COL\_ERROR\_LISTLENGTH\_CONCAT

Concat list too large.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a40d3bab7c4bdb30d0f50e04735e7c7ef"></a>
#### Enumerator COL\_ERROR\_LISTLENGTH\_REPEAT

Repeat list too large.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35acd609f7a6870d0404ced98da497d3834"></a>
#### Enumerator COL\_ERROR\_LISTITER

Invalid list iterator.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a192c0a7221d1094ac6ac886abb8982c9"></a>
#### Enumerator COL\_ERROR\_LISTITER\_END

List iterator at end.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35aa33916b02406dded5d355b3a24c6ec76"></a>
#### Enumerator COL\_ERROR\_MAP

Not a map.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a892f5f9cfa9a2d2128b9a3035747a111"></a>
#### Enumerator COL\_ERROR\_WORDMAP

Not a string or word-keyed map.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a4b21a1231316f5d1e72f1a4063cd64b4"></a>
#### Enumerator COL\_ERROR\_INTMAP

Not an integer-keyed map.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35aa2ae78a1c25af13ee9ba866d5cb501a1"></a>
#### Enumerator COL\_ERROR\_HASHMAP

Not a hash map.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a5bc15b91b43b981992b031a43e8c3e8d"></a>
#### Enumerator COL\_ERROR\_WORDHASHMAP

Not a string or word-keyed hash map.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a6adf7197e12916d0bdb78d890b43cf3d"></a>
#### Enumerator COL\_ERROR\_INTHASHMAP

Not an integer-keyed hash map.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a0622eefca9ad7bfcd98ef21080611bb3"></a>
#### Enumerator COL\_ERROR\_TRIEMAP

Not a trie map.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35ac284ba5976b713b7a2db37bace5620fb"></a>
#### Enumerator COL\_ERROR\_WORDTRIEMAP

Not a string or word-keyed trie map.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35ad77018799a3606551c4a3a66f6285b49"></a>
#### Enumerator COL\_ERROR\_INTTRIEMAP

Not an integer-keyed trie map.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35aa66f57346b0a9eac571308e75fb1f8ec"></a>
#### Enumerator COL\_ERROR\_MAPITER

Invalid map iterator.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a1a834ed5a623ccf3120ccec5d0d60653"></a>
#### Enumerator COL\_ERROR\_MAPITER\_END

Map iterator at end.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35a24606ae882ebe15eceb13230471fe356"></a>
#### Enumerator COL\_ERROR\_STRBUF

Not a string buffer.



<a id="group__error_1gga729084542ed9eae62009a84d3379ef35af19a91631001d2669a4a068b71162df3"></a>
#### Enumerator COL\_ERROR\_STRBUF\_FORMAT

String format not supported.



<a id="group__error_1gac5f445b6764ff02059fb14b2fe3eec9c"></a>
### Function Col\_GetErrorDomain

![][public]

```cpp
Col_ErrorDomain Col_GetErrorDomain()
```

Get the domain for Colibri error codes.

**Returns**:

The domain.



**See also**: [Col\_Error](colibri_8h.md#group__error_1ga9a9a9c96b23c489cf8a19a6248fc77b8)



**Return type**: EXTERN [Col\_ErrorDomain](colibri_8h.md#group__error_1gaf729acbb7ac74baf6d7a95bed1431abf)

## Runtime Checks

<a id="group__error_1ga600cfacb45fb8cb723df850444b5b87e"></a>
### Macro COL\_RUNTIMECHECK

![][public]

```cpp
#define COL_RUNTIMECHECK( condition ,level ,domain ,code ,... )
```

Check condition at runtime.

If failed, generate an error with the given parameters then execute the code block immediately following the statement.





For example:





```cpp
COL_RUNTIMECHECK(someCondition, COL_ERROR, ColibriDomain,
     COL_ERROR_GENERIC, "some message") return;
```





This code generates a generic Colibri error then exits the current proc if someCondition is false.






**Parameters**:

* **condition**: Boolean condition.
* **level**: Error level.
* **domain**: Error domain.
* **code**: Error code
* **...**: Remaining arguments passed to [Col\_Error()](colibri_8h.md#group__error_1ga9a9a9c96b23c489cf8a19a6248fc77b8).



**See also**: [COL\_DEBUGCHECK](col_utils_8h.md#group__error_1gaca6715f571b2b4b0b6cf658167c7ddc8)



<a id="group__error_1gaca6715f571b2b4b0b6cf658167c7ddc8"></a>
### Macro COL\_DEBUGCHECK

![][public]

```cpp
#define COL_DEBUGCHECK( ... )
```

In debug mode, same as [COL\_RUNTIMECHECK](col_utils_8h.md#group__error_1ga600cfacb45fb8cb723df850444b5b87e).

Else does nothing (and ignore the code block immediately following the statement).







**See also**: [COL\_RUNTIMECHECK](col_utils_8h.md#group__error_1ga600cfacb45fb8cb723df850444b5b87e)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[private]: https://img.shields.io/badge/-private-red (private)
[static]: https://img.shields.io/badge/-static-lightgrey (static)