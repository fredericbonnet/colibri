<a id="union_float_convert"></a>
# Union FloatConvert undefined

![][C++]
![][private]

**Definition**: `src/colWordInt.h` (line 362)

Utility union type for immediate floating point words.

Because of C language restrictions (bitwise operations on floating points are forbidden), we have to use this intermediary union type for conversions.








**See also**: [WORD\_SMALLFP\_VALUE](col_word_int_8h.md#group__smallfp__words_1gae3d867f57b6b1e2bb0d0919aee9a711e), [WORD\_SMALLFP\_NEW](col_word_int_8h.md#group__smallfp__words_1ga8b37cdd7a9aa19766a182dbbed963815)

## Members

* [f](union_float_convert.md#union_float_convert_1a82845b672892c8b16527dae8d55ed807)
* [i](union_float_convert.md#union_float_convert_1ab323f54476bd89aefe2b6686189857e0)
* [w](union_float_convert.md#union_float_convert_1a3e1be772c1d9d002cd7033ae6719a7b3)

## Public attributes

<a id="union_float_convert_1a3e1be772c1d9d002cd7033ae6719a7b3"></a>
### Variable w

![][public]

**Definition**: `src/colWordInt.h` (line 363)

```cpp
Col_Word FloatConvert::w
```

Word value.

<br/>



**Type**: [Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043)

<a id="union_float_convert_1ab323f54476bd89aefe2b6686189857e0"></a>
### Variable i

![][public]

**Definition**: `src/colWordInt.h` (line 364)

```cpp
uintptr_t FloatConvert::i
```

Integer value used for bitwise operations.





**Type**: uintptr_t

<a id="union_float_convert_1a82845b672892c8b16527dae8d55ed807"></a>
### Variable f

![][public]

**Definition**: `src/colWordInt.h` (line 365)

```cpp
SMALLFP_TYPE FloatConvert::f
```

Floating point value.





**Type**: [SMALLFP\_TYPE](col_word_int_8h.md#group__smallfp__words_1ga2c853438af5ac989dce08fcbed557051)

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)