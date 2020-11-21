<a id="group__utils"></a>
# Utilities





## Variadic Macro Utilities

Macro hackery for accessing args passed to variadic macros.







**See also**: [http://groups.google.com/group/comp.std.c/browse_thread/thread/77ee8c8f92e4a3fb/346fc464319b1ee5?pli=1](http://groups.google.com/group/comp.std.c/browse_thread/thread/77ee8c8f92e4a3fb/346fc464319b1ee5?pli=1)

<a id="group__utils_1gabbb0e58841406f54d444d40625a2c4fe"></a>
### Macro COL\_ARGCOUNT

![][public]

```cpp
#define COL_ARGCOUNT( ... )
```

Get the number of args passed to it.

**Parameters**:

* **...**: Arguments passed to the variadic macro.


!> **Warning** \
Argument length must be between 1 and 63. Empty lists return zero due to limitations of the C preprocessor.



<a id="group__utils_1gafc4ddbc34c20a309d7340a87e8f6ffab"></a>
### Macro COL\_FOR\_EACH

![][public]

```cpp
#define COL_FOR_EACH( what ,... )
```

Iterate over the args passed to it.

**Parameters**:

* **what**: Function taking one argument, applied to all remaining arguments.
* **...**: Arguments passed to the variadic macro.


!> **Warning** \
Limited to 63 arguments.



## Macros

<a id="group__utils_1gabca6f10008484ed7d45647264d1c1d32"></a>
### Macro COL\_STRINGIZE

![][public]

```cpp
#define COL_STRINGIZE( arg )
```

Turn argument into a C string.





<a id="group__utils_1ga9f809f159ffd6703fd51d3773b15ade8"></a>
### Macro COL\_CONCATENATE

![][public]

```cpp
#define COL_CONCATENATE( arg1 ,arg2 )
```

Concatenates both arguments.





[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[private]: https://img.shields.io/badge/-private-red (private)
[static]: https://img.shields.io/badge/-static-lightgrey (static)