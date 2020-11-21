<a id="group__mapentry__words"></a>
# Map Entries



Generic map entries are key-value pairs. Type-specific entries extend this generic type with implementation-dependent data.






**Requirements**:


* Map entry words use one single cell.

* Map entries must store at least a key and a value.


**Parameters**:

* **Key**: A word in the general case but can be a native integer for integer maps.
* **Value**: A word.


**Cell Layout**:

On all architectures the single-cell layout is as follows:


    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        mapentry_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td sides="R" colspan="2"></td>
            </tr>
            <tr><td sides="R">1</td>
                <td colspan="4" sides="BR" width="320">Type-specific data</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_MAPENTRY_KEY" title="WORD_MAPENTRY_KEY" colspan="4">Key</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_MAPENTRY_VALUE" title="WORD_MAPENTRY_VALUE" colspan="4">Value</td>
            </tr>
            </table>
        >]
    }

## Submodules

* [Hash Entries](group__mhashentry__words.md#group__mhashentry__words)
* [Immutable Hash Entries](group__hashentry__words.md#group__hashentry__words)
* [Integer Hash Entries](group__minthashentry__words.md#group__minthashentry__words)
* [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)
* [Trie Leaves](group__mtrieleaf__words.md#group__mtrieleaf__words)
* [Immutable Trie Leaves](group__trieleaf__words.md#group__trieleaf__words)

## Map Entry Accessors

<a id="group__mapentry__words_1ga8664d15fae4553b47b658ac7ceb1443a"></a>
### Macro WORD\_MAPENTRY\_KEY

![][public]

```cpp
#define WORD_MAPENTRY_KEY ((([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(word))[2])( word )
```

Get/set word key of word-based map entry (string or custom).

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



<a id="group__mapentry__words_1gabad6806f2947f508a9786948c1663064"></a>
### Macro WORD\_MAPENTRY\_VALUE

![][public]

```cpp
#define WORD_MAPENTRY_VALUE ((([Col\_Word](col_word_8h.md#group__words_1gadb626f9e195212e4fdfba7df154ad043) *)(word))[3])( word )
```

Get/set value of map entry.

**Parameters**:

* **word**: Word to access.


?> Macro is L-Value and suitable for both read/write operations.



## Map Entry Exceptions

<a id="group__mapentry__words_1ga1eede95138bba256545ca58e6abcc4b0"></a>
### Macro TYPECHECK\_MAP

![][public]

```cpp
#define TYPECHECK_MAP( word )
```

Type checking macro for maps.

**Parameters**:

* **word**: Checked word.


**Exceptions**:

* **[COL\_ERROR\_MAP](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aa33916b02406dded5d355b3a24c6ec76)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **word**: Not a map.



<a id="group__mapentry__words_1gad6c8914c3cf622521347dfa8a7a3f31c"></a>
### Macro TYPECHECK\_WORDMAP

![][public]

```cpp
#define TYPECHECK_WORDMAP( word )
```

Type checking macro for word-based maps (string or custom).

**Parameters**:

* **word**: Checked word.


**Exceptions**:

* **[COL\_ERROR\_WORDMAP](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a892f5f9cfa9a2d2128b9a3035747a111)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **word**: Not a string or word-keyed map.



<a id="group__mapentry__words_1gabf16d4f023a9f2699c2ef1098d5fb74e"></a>
### Macro TYPECHECK\_INTMAP

![][public]

```cpp
#define TYPECHECK_INTMAP( word )
```

Type checking macro for integer maps.

**Parameters**:

* **word**: Checked word.


**Exceptions**:

* **[COL\_ERROR\_INTMAP](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a4b21a1231316f5d1e72f1a4063cd64b4)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **word**: Not an integer-keyed map.



<a id="group__mapentry__words_1ga1ac9e9b70a28fd5385c1c5fb95494a2b"></a>
### Macro TYPECHECK\_MAPITER

![][public]

```cpp
#define TYPECHECK_MAPITER( it )
```

Type checking macro for map iterators.

**Parameters**:

* **it**: Checked iterator.


**Exceptions**:

* **[COL\_ERROR\_MAPITER](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35aa66f57346b0a9eac571308e75fb1f8ec)**: [[T]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea603a58b9d5bb16fde0708eb0767e4904) **it**: Invalid map iterator.



**See also**: [Col\_MapIterNull](col_map_8h.md#group__map__words_1gaa62e65511e00c06204ab1aac8132ac7c)



<a id="group__mapentry__words_1ga4e477eb4ded0e97fcfa10c0e01eb7ead"></a>
### Macro VALUECHECK\_MAPITER

![][public]

```cpp
#define VALUECHECK_MAPITER( it )
```

Value checking macro for map iterators, ensures that iterator is not at end.

**Parameters**:

* **it**: Checked iterator.


**Exceptions**:

* **[COL\_ERROR\_MAPITER\_END](colibri_8h.md#group__error_1gga729084542ed9eae62009a84d3379ef35a1a834ed5a623ccf3120ccec5d0d60653)**: [[V]](colibri_8h.md#group__error_1gga6dab009a0b8c4b4fa080cb9ba1859e9ea65d5e7232c82ae6972ac56f386a32fc9) **it**: Map iterator at end.



**See also**: [Col\_MapIterEnd](col_map_8h.md#group__map__words_1ga98ec97359170a97141470434b83dcf56)



[public]: https://img.shields.io/badge/-public-brightgreen (public)
[C++]: https://img.shields.io/badge/language-C%2B%2B-blue (C++)
[private]: https://img.shields.io/badge/-private-red (private)
[Markdown]: https://img.shields.io/badge/language-Markdown-blue (Markdown)
[static]: https://img.shields.io/badge/-static-lightgrey (static)