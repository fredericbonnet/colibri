<a id="group__immediate__words"></a>
# Immediate Words



Immediate words are immutable datatypes that store their value directly in the word identifier, contrary to [Regular Words](group__regular__words.md#group__regular__words) whose identifier is a pointer to a cell-based structure. As cells are aligned on a multiple of their size (16 bytes on 32-bit systems), this means that a number of low bits are always zero in regular word identifiers (4 bits on 32-bit systems with 16-byte cells). Immediate values are distinguished from regular pointers by setting one of these bits. Immediate word types are identified by these bit patterns, called tags.

## Submodules

* [Small Integer Words](group__smallint__words.md#group__smallint__words)
* [Small Floating Point Words](group__smallfp__words.md#group__smallfp__words)
* [Boolean Words](group__bool__words.md#group__bool__words)
* [Character Words](group__char__words.md#group__char__words)
* [Small String Words](group__smallstr__words.md#group__smallstr__words)
* [Circular List Words](group__circlist__words.md#group__circlist__words)
* [Void List Words](group__voidlist__words.md#group__voidlist__words)

## Variables

<a id="group__immediate__words_1gadc93860da89f6a52a884c10b7835aaac"></a>
### Variable immediateWordTypes

![][private]
![][static]

**Definition**: `src/colWordInt.h` (line 150)

```cpp
const int immediateWordTypes[32][32]
```

Lookup table for immediate word types, gives the word type ID from the first 5 bits of the word value (= the tag).

?> Defined as static so that all modules use their own instance for potential compiler optimizations.



**See also**: [WORD\_TYPE](col_word_int_8h.md#group__words_1ga014e27ea4160eb3845ac495a22c232f5)



**Type**: const int

[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)