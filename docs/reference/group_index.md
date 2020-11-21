# Index of Modules

## B

* [Basic Types](group__basic__types.md#group__basic__types)

## C

* [Custom Hash Maps](group__customhashmap__words.md#group__customhashmap__words): Custom hash maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Hash Maps](group__hashmap__words.md#group__hashmap__words) with applicative code.
* [Custom Lists](group__customlist__words.md#group__customlist__words): Custom lists are [Custom Words](group__custom__words.md#group__custom__words) implementing [Immutable Lists](group__list__words.md#group__list__words) with applicative code.
* [Custom Maps](group__custommap__words.md#group__custommap__words): Custom maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Maps](group__map__words.md#group__map__words) with applicative code.
* [Custom Ropes](group__customrope__words.md#group__customrope__words): Custom ropes are [Custom Words](group__custom__words.md#group__custom__words) implementing [Ropes](group__rope__words.md#group__rope__words) with applicative code.
* [Custom Trie Maps](group__customtriemap__words.md#group__customtriemap__words): Custom trie maps are [Custom Words](group__custom__words.md#group__custom__words) implementing [Trie Maps](group__triemap__words.md#group__triemap__words) with applicative code.
* [Custom Words](group__custom__words.md#group__custom__words): Custom words are words defined by applicative code.

## E

* [Error Handling & Debugging](group__error.md#group__error)

## G

* [Garbage Collection](group__gc.md#group__gc)

## H

* [Hash Maps](group__hashmap__words.md#group__hashmap__words): Hash maps are an implementation of generic [Maps](group__map__words.md#group__map__words) that use key hashing and flat bucket arrays for string, integer and custom keys.

## I

* [Initialization/Cleanup](group__init.md#group__init)
* [Immutable Lists](group__list__words.md#group__list__words): Immutable lists are constant, arbitrary-sized, linear collections of words.
* [Immutable Vectors](group__vector__words.md#group__vector__words): Immutable vectors are constant, fixed-length arrays of words that are directly accessible through a pointer value.

## M

* [Mutable Lists](group__mlist__words.md#group__mlist__words): Mutable lists are linear collection of words whose content and length can vary over time.
* [Maps](group__map__words.md#group__map__words): Maps are mutable associative arrays whose keys can be integers, strings or generic words.
* [Mutable Vectors](group__mvector__words.md#group__mvector__words): Mutable vectors are arrays of words that are directly accessible and modifiable through a pointer value, and whose length can vary up to a given capacity set at creation time; they can be"frozen" and turned into immutable vectors.

## R

* [Ropes](group__rope__words.md#group__rope__words): Ropes are a string datatype that allows for fast insertion, extraction and composition of strings.

## S

* [Strings](group__strings.md#group__strings)
* [String Buffers](group__strbuf__words.md#group__strbuf__words): String buffers are used to build strings incrementally in an efficient manner, by appending individual characters or whole ropes.

## T

* [Trie Maps](group__triemap__words.md#group__triemap__words): Trie maps are an implementation of generic [Maps](group__map__words.md#group__map__words) that use crit-bit trees for string, integer and custom keys.

## U

* [Utilities](group__utils.md#group__utils)

## W

* [Words](group__words.md#group__words): Words are a generic abstract datatype framework used in conjunction with the exact generational garbage collector and the cell-based allocator.