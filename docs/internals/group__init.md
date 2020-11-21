<a id="group__init"></a>
# Initialization/Cleanup





## Threading Model Constants

<a id="group__init_1threading_models"></a>
 Threading models chosen at initialization time.

<a id="group__init_1gaaecbd3c0ddf9f5684b97db76e7338731"></a>
### Macro COL\_SINGLE

![][public]

```cpp
#define COL_SINGLE 0
```

Strict appartment + stop-the-world model.

GC is done synchronously when client thread resumes GC.









**See also**: [Col\_Init](colibri_8h.md#group__init_1ga715049d7eb10ff0eeac38b457ef4fce1), [Col\_PauseGC](colibri_8h.md#group__gc_1gae703ee3215a4724ebed8e5a2824e7a7b), [Col\_ResumeGC](colibri_8h.md#group__gc_1gaf7d4f0dd1996dde366af3f29e9bcc517)



<a id="group__init_1ga74df35d99d8e8408a563ae5a23022b06"></a>
### Macro COL\_ASYNC

![][public]

```cpp
#define COL_ASYNC 1
```

Strict appartment model with asynchronous GC.

GC uses a dedicated thread for asynchronous processing, the client thread cannot pause a running GC and is blocked until completion.









**See also**: [Col\_Init](colibri_8h.md#group__init_1ga715049d7eb10ff0eeac38b457ef4fce1), [Col\_PauseGC](colibri_8h.md#group__gc_1gae703ee3215a4724ebed8e5a2824e7a7b), [Col\_ResumeGC](colibri_8h.md#group__gc_1gaf7d4f0dd1996dde366af3f29e9bcc517)



<a id="group__init_1ga9df51b794beca985147399feb68a8734"></a>
### Macro COL\_SHARED

![][public]

```cpp
#define COL_SHARED 2
```

Shared multithreaded model with GC-preference.

Data can be shared across client threads of the same group (COL_SHARED is the base index value). GC uses a dedicated thread for asynchronous processing.; GC process starts once all client threads get out of pause, no client thread can pause a scheduled GC.









**See also**: [Col\_Init](colibri_8h.md#group__init_1ga715049d7eb10ff0eeac38b457ef4fce1), [Col\_PauseGC](colibri_8h.md#group__gc_1gae703ee3215a4724ebed8e5a2824e7a7b), [Col\_ResumeGC](colibri_8h.md#group__gc_1gaf7d4f0dd1996dde366af3f29e9bcc517)



## Initialization/Cleanup Functions

<a id="group__init_1ga715049d7eb10ff0eeac38b457ef4fce1"></a>
### Function Col\_Init

![][public]

```cpp
void Col_Init(unsigned int model)
```

Initialize the library.

Must be called in every thread.






**Side Effect**:

Initialize the memory allocator & garbage collector.



**See also**: [Threading Model Constants](#group__init_1threading_models)



**Parameters**:

* unsigned int **model**: Threading model.

**Return type**: EXTERN void

**References**:

* [PlatEnter](col_platform_8h.md#group__arch_1gaa42fe97b4b462c9483110a715c1eb1d1)

<a id="group__init_1gaaa7addef72eaac5fb58f6edb449fda75"></a>
### Function Col\_Cleanup

![][public]

```cpp
void Col_Cleanup()
```

Cleanup the library.

Must be called in every thread.






**Side Effect**:

Cleanup the memory allocator & garbage collector.



**Parameters**:

* void

**Return type**: EXTERN void

**References**:

* [PlatLeave](col_platform_8h.md#group__arch_1gaf70aa9db460d666ff9a09610dda183ef)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)