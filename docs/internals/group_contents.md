# Modules

* [Basic Types](group__basic__types.md#group__basic__types)
* [Strings](group__strings.md#group__strings)
* [Initialization/Cleanup](group__init.md#group__init)
* [Error Handling & Debugging](group__error.md#group__error)
* [Garbage Collection](group__gc.md#group__gc)
  * [Roots](group__gc__roots.md#group__gc__roots)
  * [Parents](group__gc__parents.md#group__gc__parents)
* [Utilities](group__utils.md#group__utils)
* [Words](group__words.md#group__words): Words are a generic abstract datatype framework used in conjunction with the exact generational garbage collector and the cell-based allocator.
  * [Hash Maps](group__hashmap__words.md#group__hashmap__words): Hash maps are an implementation of generic [Maps](group__map__words.md#group__map__words) that use key hashing and flat bucket arrays for string, integer and custom keys.
    * [Custom Hash Maps](group__customhashmap__words.md#group__customhashmap__words): Custom hash maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Hash Maps](group__hashmap__words.md#group__hashmap__words) with applicative code.
    * [String Hash Maps](group__strhashmap__words.md#group__strhashmap__words)
    * [Integer Hash Maps](group__inthashmap__words.md#group__inthashmap__words)
      * [Integer Hash Entries](group__minthashentry__words.md#group__minthashentry__words)
        * [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)
      * [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)
    * [Hash Entries](group__mhashentry__words.md#group__mhashentry__words)
      * [Immutable Hash Entries](group__hashentry__words.md#group__hashentry__words)
    * [Immutable Hash Entries](group__hashentry__words.md#group__hashentry__words)
  * [Immutable Lists](group__list__words.md#group__list__words): Immutable lists are constant, arbitrary-sized, linear collections of words.
    * [Custom Lists](group__customlist__words.md#group__customlist__words): Custom lists are [Custom Words](group__custom__words.md#group__custom__words) implementing [Immutable Lists](group__list__words.md#group__list__words) with applicative code.
    * [Sublists](group__sublist__words.md#group__sublist__words)
    * [Immutable Concat Lists](group__concatlist__words.md#group__concatlist__words)
      * [Mutable Concat Lists](group__mconcatlist__words.md#group__mconcatlist__words)
  * [Mutable Lists](group__mlist__words.md#group__mlist__words): Mutable lists are linear collection of words whose content and length can vary over time.
    * [Mutable Concat Lists](group__mconcatlist__words.md#group__mconcatlist__words)
  * [Maps](group__map__words.md#group__map__words): Maps are mutable associative arrays whose keys can be integers, strings or generic words.
    * [Hash Maps](group__hashmap__words.md#group__hashmap__words): Hash maps are an implementation of generic [Maps](group__map__words.md#group__map__words) that use key hashing and flat bucket arrays for string, integer and custom keys.
      * [Custom Hash Maps](group__customhashmap__words.md#group__customhashmap__words): Custom hash maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Hash Maps](group__hashmap__words.md#group__hashmap__words) with applicative code.
      * [String Hash Maps](group__strhashmap__words.md#group__strhashmap__words)
      * [Integer Hash Maps](group__inthashmap__words.md#group__inthashmap__words)
        * [Integer Hash Entries](group__minthashentry__words.md#group__minthashentry__words)
          * [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)
        * [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)
      * [Hash Entries](group__mhashentry__words.md#group__mhashentry__words)
        * [Immutable Hash Entries](group__hashentry__words.md#group__hashentry__words)
      * [Immutable Hash Entries](group__hashentry__words.md#group__hashentry__words)
    * [Custom Maps](group__custommap__words.md#group__custommap__words): Custom maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Maps](group__map__words.md#group__map__words) with applicative code.
    * [Trie Maps](group__triemap__words.md#group__triemap__words): Trie maps are an implementation of generic [Maps](group__map__words.md#group__map__words) that use crit-bit trees for string, integer and custom keys.
      * [Custom Trie Maps](group__customtriemap__words.md#group__customtriemap__words): Custom trie maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Trie Maps](group__triemap__words.md#group__triemap__words) with applicative code.
      * [String Trie Maps](group__strtriemap__words.md#group__strtriemap__words)
        * [String Trie Nodes](group__mstrtrienode__words.md#group__mstrtrienode__words)
          * [Immutable String Trie Nodes](group__strtrienode__words.md#group__strtrienode__words)
        * [Immutable String Trie Nodes](group__strtrienode__words.md#group__strtrienode__words)
      * [Integer Trie Maps](group__inttriemap__words.md#group__inttriemap__words)
        * [Integer Trie Nodes](group__minttrienode__words.md#group__minttrienode__words)
          * [Immutable Integer Trie Nodes](group__inttrienode__words.md#group__inttrienode__words)
        * [Immutable Integer Trie Nodes](group__inttrienode__words.md#group__inttrienode__words)
        * [Integer Trie Leaves](group__minttrieleaf__words.md#group__minttrieleaf__words)
          * [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)
        * [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)
      * [Trie Nodes](group__mtrienode__words.md#group__mtrienode__words)
        * [Immutable Trie Nodes](group__trienode__words.md#group__trienode__words)
      * [Immutable Trie Nodes](group__trienode__words.md#group__trienode__words)
      * [Trie Leaves](group__mtrieleaf__words.md#group__mtrieleaf__words)
        * [Immutable Trie Leaves](group__trieleaf__words.md#group__trieleaf__words)
      * [Immutable Trie Leaves](group__trieleaf__words.md#group__trieleaf__words)
    * [Map Entries](group__mapentry__words.md#group__mapentry__words)
      * [Hash Entries](group__mhashentry__words.md#group__mhashentry__words)
        * [Immutable Hash Entries](group__hashentry__words.md#group__hashentry__words)
      * [Immutable Hash Entries](group__hashentry__words.md#group__hashentry__words)
      * [Integer Hash Entries](group__minthashentry__words.md#group__minthashentry__words)
        * [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)
      * [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)
      * [Trie Leaves](group__mtrieleaf__words.md#group__mtrieleaf__words)
        * [Immutable Trie Leaves](group__trieleaf__words.md#group__trieleaf__words)
      * [Immutable Trie Leaves](group__trieleaf__words.md#group__trieleaf__words)
    * [Integer Map Entries](group__intmapentry__words.md#group__intmapentry__words)
      * [Integer Trie Leaves](group__minttrieleaf__words.md#group__minttrieleaf__words)
        * [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)
      * [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)
  * [Ropes](group__rope__words.md#group__rope__words): Ropes are a string datatype that allows for fast insertion, extraction and composition of strings.
    * [Custom Ropes](group__customrope__words.md#group__customrope__words): Custom ropes are [Custom Words](group__custom__words.md#group__custom__words) implementing [Ropes](group__rope__words.md#group__rope__words) with applicative code.
    * [Fixed-Width UCS Strings](group__ucsstr__words.md#group__ucsstr__words)
    * [Variable-Width UTF Strings](group__utfstr__words.md#group__utfstr__words)
    * [Subropes](group__subrope__words.md#group__subrope__words)
    * [Concat Ropes](group__concatrope__words.md#group__concatrope__words)
    * [Character Words](group__char__words.md#group__char__words)
    * [Small String Words](group__smallstr__words.md#group__smallstr__words)
  * [Trie Maps](group__triemap__words.md#group__triemap__words): Trie maps are an implementation of generic [Maps](group__map__words.md#group__map__words) that use crit-bit trees for string, integer and custom keys.
    * [Custom Trie Maps](group__customtriemap__words.md#group__customtriemap__words): Custom trie maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Trie Maps](group__triemap__words.md#group__triemap__words) with applicative code.
    * [String Trie Maps](group__strtriemap__words.md#group__strtriemap__words)
      * [String Trie Nodes](group__mstrtrienode__words.md#group__mstrtrienode__words)
        * [Immutable String Trie Nodes](group__strtrienode__words.md#group__strtrienode__words)
      * [Immutable String Trie Nodes](group__strtrienode__words.md#group__strtrienode__words)
    * [Integer Trie Maps](group__inttriemap__words.md#group__inttriemap__words)
      * [Integer Trie Nodes](group__minttrienode__words.md#group__minttrienode__words)
        * [Immutable Integer Trie Nodes](group__inttrienode__words.md#group__inttrienode__words)
      * [Immutable Integer Trie Nodes](group__inttrienode__words.md#group__inttrienode__words)
      * [Integer Trie Leaves](group__minttrieleaf__words.md#group__minttrieleaf__words)
        * [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)
      * [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)
    * [Trie Nodes](group__mtrienode__words.md#group__mtrienode__words)
      * [Immutable Trie Nodes](group__trienode__words.md#group__trienode__words)
    * [Immutable Trie Nodes](group__trienode__words.md#group__trienode__words)
    * [Trie Leaves](group__mtrieleaf__words.md#group__mtrieleaf__words)
      * [Immutable Trie Leaves](group__trieleaf__words.md#group__trieleaf__words)
    * [Immutable Trie Leaves](group__trieleaf__words.md#group__trieleaf__words)
  * [Immutable Vectors](group__vector__words.md#group__vector__words): Immutable vectors are constant, fixed-length arrays of words that are directly accessible through a pointer value.
  * [Mutable Vectors](group__mvector__words.md#group__mvector__words): Mutable vectors are arrays of words that are directly accessible and modifiable through a pointer value, and whose length can vary up to a given capacity set at creation time; they can be"frozen" and turned into immutable vectors.
  * [Custom Words](group__custom__words.md#group__custom__words): Custom words are words defined by applicative code.
    * [Custom Hash Maps](group__customhashmap__words.md#group__customhashmap__words): Custom hash maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Hash Maps](group__hashmap__words.md#group__hashmap__words) with applicative code.
    * [Custom Lists](group__customlist__words.md#group__customlist__words): Custom lists are [Custom Words](group__custom__words.md#group__custom__words) implementing [Immutable Lists](group__list__words.md#group__list__words) with applicative code.
    * [Custom Maps](group__custommap__words.md#group__custommap__words): Custom maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Maps](group__map__words.md#group__map__words) with applicative code.
    * [Custom Ropes](group__customrope__words.md#group__customrope__words): Custom ropes are [Custom Words](group__custom__words.md#group__custom__words) implementing [Ropes](group__rope__words.md#group__rope__words) with applicative code.
    * [Custom Trie Maps](group__customtriemap__words.md#group__customtriemap__words): Custom trie maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Trie Maps](group__triemap__words.md#group__triemap__words) with applicative code.
  * [Immediate Words](group__immediate__words.md#group__immediate__words)
    * [Small Integer Words](group__smallint__words.md#group__smallint__words)
    * [Small Floating Point Words](group__smallfp__words.md#group__smallfp__words)
    * [Boolean Words](group__bool__words.md#group__bool__words)
    * [Character Words](group__char__words.md#group__char__words)
    * [Small String Words](group__smallstr__words.md#group__smallstr__words)
    * [Circular List Words](group__circlist__words.md#group__circlist__words)
    * [Void List Words](group__voidlist__words.md#group__voidlist__words)
  * [Regular Words](group__regular__words.md#group__regular__words)
    * [Predefined Words](group__predefined__words.md#group__predefined__words)
      * [String Hash Maps](group__strhashmap__words.md#group__strhashmap__words)
      * [Integer Hash Maps](group__inthashmap__words.md#group__inthashmap__words)
        * [Integer Hash Entries](group__minthashentry__words.md#group__minthashentry__words)
          * [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)
        * [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)
      * [Hash Entries](group__mhashentry__words.md#group__mhashentry__words)
        * [Immutable Hash Entries](group__hashentry__words.md#group__hashentry__words)
      * [Immutable Hash Entries](group__hashentry__words.md#group__hashentry__words)
      * [Integer Hash Entries](group__minthashentry__words.md#group__minthashentry__words)
        * [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)
      * [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)
      * [Sublists](group__sublist__words.md#group__sublist__words)
      * [Immutable Concat Lists](group__concatlist__words.md#group__concatlist__words)
        * [Mutable Concat Lists](group__mconcatlist__words.md#group__mconcatlist__words)
      * [Mutable Concat Lists](group__mconcatlist__words.md#group__mconcatlist__words)
      * [Map Entries](group__mapentry__words.md#group__mapentry__words)
        * [Hash Entries](group__mhashentry__words.md#group__mhashentry__words)
          * [Immutable Hash Entries](group__hashentry__words.md#group__hashentry__words)
        * [Immutable Hash Entries](group__hashentry__words.md#group__hashentry__words)
        * [Integer Hash Entries](group__minthashentry__words.md#group__minthashentry__words)
          * [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)
        * [Immutable Integer Hash Entries](group__inthashentry__words.md#group__inthashentry__words)
        * [Trie Leaves](group__mtrieleaf__words.md#group__mtrieleaf__words)
          * [Immutable Trie Leaves](group__trieleaf__words.md#group__trieleaf__words)
        * [Immutable Trie Leaves](group__trieleaf__words.md#group__trieleaf__words)
      * [Integer Map Entries](group__intmapentry__words.md#group__intmapentry__words)
        * [Integer Trie Leaves](group__minttrieleaf__words.md#group__minttrieleaf__words)
          * [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)
        * [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)
      * [Fixed-Width UCS Strings](group__ucsstr__words.md#group__ucsstr__words)
      * [Variable-Width UTF Strings](group__utfstr__words.md#group__utfstr__words)
      * [Subropes](group__subrope__words.md#group__subrope__words)
      * [Concat Ropes](group__concatrope__words.md#group__concatrope__words)
      * [String Trie Maps](group__strtriemap__words.md#group__strtriemap__words)
        * [String Trie Nodes](group__mstrtrienode__words.md#group__mstrtrienode__words)
          * [Immutable String Trie Nodes](group__strtrienode__words.md#group__strtrienode__words)
        * [Immutable String Trie Nodes](group__strtrienode__words.md#group__strtrienode__words)
      * [Integer Trie Maps](group__inttriemap__words.md#group__inttriemap__words)
        * [Integer Trie Nodes](group__minttrienode__words.md#group__minttrienode__words)
          * [Immutable Integer Trie Nodes](group__inttrienode__words.md#group__inttrienode__words)
        * [Immutable Integer Trie Nodes](group__inttrienode__words.md#group__inttrienode__words)
        * [Integer Trie Leaves](group__minttrieleaf__words.md#group__minttrieleaf__words)
          * [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)
        * [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)
      * [Trie Nodes](group__mtrienode__words.md#group__mtrienode__words)
        * [Immutable Trie Nodes](group__trienode__words.md#group__trienode__words)
      * [Immutable Trie Nodes](group__trienode__words.md#group__trienode__words)
      * [String Trie Nodes](group__mstrtrienode__words.md#group__mstrtrienode__words)
        * [Immutable String Trie Nodes](group__strtrienode__words.md#group__strtrienode__words)
      * [Immutable String Trie Nodes](group__strtrienode__words.md#group__strtrienode__words)
      * [Integer Trie Nodes](group__minttrienode__words.md#group__minttrienode__words)
        * [Immutable Integer Trie Nodes](group__inttrienode__words.md#group__inttrienode__words)
      * [Immutable Integer Trie Nodes](group__inttrienode__words.md#group__inttrienode__words)
      * [Trie Leaves](group__mtrieleaf__words.md#group__mtrieleaf__words)
        * [Immutable Trie Leaves](group__trieleaf__words.md#group__trieleaf__words)
      * [Immutable Trie Leaves](group__trieleaf__words.md#group__trieleaf__words)
      * [Integer Trie Leaves](group__minttrieleaf__words.md#group__minttrieleaf__words)
        * [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)
      * [Immutable Integer Trie Leaves](group__inttrieleaf__words.md#group__inttrieleaf__words)
    * [Wrap Words](group__wrap__words.md#group__wrap__words)
      * [Word Wrappers](group__word__wrappers.md#group__word__wrappers)
      * [Integer Wrappers](group__int__wrappers.md#group__int__wrappers)
      * [Floating Point Wrappers](group__fp__wrappers.md#group__fp__wrappers)
    * [Redirect Words](group__redirect__words.md#group__redirect__words)
  * [String Buffers](group__strbuf__words.md#group__strbuf__words): String buffers are used to build strings incrementally in an efficient manner, by appending individual characters or whole ropes.
* [Memory Allocation](group__alloc.md#group__alloc)
  * [Pages and Cells](group__pages__cells.md#group__pages__cells)
* [System and Architecture](group__arch.md#group__arch)
  * [System and Architecture (Unix-specific)](group__arch__unix.md#group__arch__unix)
  * [System and Architecture (Win32-specific)](group__arch__win32.md#group__arch__win32)