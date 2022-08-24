1. Replace alloca with C99 variable length arrays

> ABANDONED

2. Use recursive mutex in pthread instead of the current hack, there's no point in supporting obsolete systems since they likely won't support other features such as page guards

3. Support platforms with no memory barrier (e.g. Wasm)

4. Add predicates for words e.g. Col_WordIsInt

5. Add Col_NewRopeFromChunk

6. Add Col_RopeIterChunk

7. Allow NULL valuePtr in map get

8. Make iterator accessors readonly (const)

9. Make Col_HashMapGet/Set/Unset typecheck the key with WORD_TYPE_STRHASHMAP

10. Map iterator compare/equals

11. Empty Function signature: (); => (void);

12. Add rope comparison to traversal tests

13. Find closest nodes in trie maps & iterators

14. Test iter setnull for ropes/maps

15. Add debug package `ColDbg_` for e.g. memory pool introspection

16. Add list search?

17. Sorting

    - https://hackernoon.com/timsort-the-fastest-sorting-algorithm-youve-never-heard-of-36b28417f399
    - https://dev.to/brandonbrown4792/a-dive-through-5-sorting-algorithms-155k

18. Make custom words optional

    > Done

19. Replace traverse procs with iterators

20. WASM

    - https://github.com/WebAssembly/gc/issues/123

21. Doc portal

    - https://dev.to/doctave/5-components-of-useful-documentation-546e
