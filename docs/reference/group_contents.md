# Modules

* [Basic Types](group__basic__types.md#group__basic__types)
* [Strings](group__strings.md#group__strings)
* [Initialization/Cleanup](group__init.md#group__init)
* [Error Handling & Debugging](group__error.md#group__error)
* [Garbage Collection](group__gc.md#group__gc)
* [Utilities](group__utils.md#group__utils)
* [Words](group__words.md#group__words): Words are a generic abstract datatype framework used in conjunction with the exact generational garbage collector and the cell-based allocator.
  * [Hash Maps](group__hashmap__words.md#group__hashmap__words): Hash maps are an implementation of generic [Maps](group__map__words.md#group__map__words) that use key hashing and flat bucket arrays for string, integer and custom keys.
    * [Custom Hash Maps](group__customhashmap__words.md#group__customhashmap__words): Custom hash maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Hash Maps](group__hashmap__words.md#group__hashmap__words) with applicative code.
  * [Immutable Lists](group__list__words.md#group__list__words): Immutable lists are constant, arbitrary-sized, linear collections of words.
    * [Custom Lists](group__customlist__words.md#group__customlist__words): Custom lists are [Custom Words](group__custom__words.md#group__custom__words) implementing [Immutable Lists](group__list__words.md#group__list__words) with applicative code.
  * [Mutable Lists](group__mlist__words.md#group__mlist__words): Mutable lists are linear collection of words whose content and length can vary over time.
  * [Maps](group__map__words.md#group__map__words): Maps are mutable associative arrays whose keys can be integers, strings or generic words.
    * [Hash Maps](group__hashmap__words.md#group__hashmap__words): Hash maps are an implementation of generic [Maps](group__map__words.md#group__map__words) that use key hashing and flat bucket arrays for string, integer and custom keys.
      * [Custom Hash Maps](group__customhashmap__words.md#group__customhashmap__words): Custom hash maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Hash Maps](group__hashmap__words.md#group__hashmap__words) with applicative code.
    * [Custom Maps](group__custommap__words.md#group__custommap__words): Custom maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Maps](group__map__words.md#group__map__words) with applicative code.
    * [Trie Maps](group__triemap__words.md#group__triemap__words): Trie maps are an implementation of generic [Maps](group__map__words.md#group__map__words) that use crit-bit trees for string, integer and custom keys.
      * [Custom Trie Maps](group__customtriemap__words.md#group__customtriemap__words): Custom trie maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Trie Maps](group__triemap__words.md#group__triemap__words) with applicative code.
  * [Ropes](group__rope__words.md#group__rope__words): Ropes are a string datatype that allows for fast insertion, extraction and composition of strings.
    * [Custom Ropes](group__customrope__words.md#group__customrope__words): Custom ropes are [Custom Words](group__custom__words.md#group__custom__words) implementing [Ropes](group__rope__words.md#group__rope__words) with applicative code.
  * [Trie Maps](group__triemap__words.md#group__triemap__words): Trie maps are an implementation of generic [Maps](group__map__words.md#group__map__words) that use crit-bit trees for string, integer and custom keys.
    * [Custom Trie Maps](group__customtriemap__words.md#group__customtriemap__words): Custom trie maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Trie Maps](group__triemap__words.md#group__triemap__words) with applicative code.
  * [Immutable Vectors](group__vector__words.md#group__vector__words): Immutable vectors are constant, fixed-length arrays of words that are directly accessible through a pointer value.
  * [Mutable Vectors](group__mvector__words.md#group__mvector__words): Mutable vectors are arrays of words that are directly accessible and modifiable through a pointer value, and whose length can vary up to a given capacity set at creation time; they can be"frozen" and turned into immutable vectors.
  * [Custom Words](group__custom__words.md#group__custom__words): Custom words are words defined by applicative code.
    * [Custom Hash Maps](group__customhashmap__words.md#group__customhashmap__words): Custom hash maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Hash Maps](group__hashmap__words.md#group__hashmap__words) with applicative code.
    * [Custom Lists](group__customlist__words.md#group__customlist__words): Custom lists are [Custom Words](group__custom__words.md#group__custom__words) implementing [Immutable Lists](group__list__words.md#group__list__words) with applicative code.
    * [Custom Maps](group__custommap__words.md#group__custommap__words): Custom maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Maps](group__map__words.md#group__map__words) with applicative code.
    * [Custom Ropes](group__customrope__words.md#group__customrope__words): Custom ropes are [Custom Words](group__custom__words.md#group__custom__words) implementing [Ropes](group__rope__words.md#group__rope__words) with applicative code.
    * [Custom Trie Maps](group__customtriemap__words.md#group__customtriemap__words): Custom trie maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Trie Maps](group__triemap__words.md#group__triemap__words) with applicative code.
  * [String Buffers](group__strbuf__words.md#group__strbuf__words): String buffers are used to build strings incrementally in an efficient manner, by appending individual characters or whole ropes.