# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.15.0] - 2020-11-21

### Added

- CMake support for build.
- `Col_EmptyList` for empty list singleton.
- Boolean type.
- Custom list type (based on existing custom ropes design).
- Custom hash map type.
- Custom trie map type.
- Custom map type (word- and integer-keyed).
- Word comparison procs and array sorting.
- User-level, non-fatal `COL_TYPECHECK` and `COL_RANGECHECK` error codes.
- Type and range checking macros.
- Synonym field for custom words.
- Error domains. Domains are message string tables. Error codes are defined as numeric values within a domain; this avoid value clashes.

### Changed

- Iterator is now the first argument of all procs for consistency.
- Iterator type declaration. Public type is now a 1-element array of private type (idea stolen from MPIR). This improves type safety.
- `Col_RopeIterForward` and `Col_ListIterForward` now accept iterators at end when nb=0.
- List iterators can now iterate on flat arrays as well.
- Rope and list iterator performance improvements. Iterators no longer store intermediary subropes/lists. This greatly simplifies algorithms and compensates for the overhead of descending into the whole structure systematically.
- Merged generic- and string-keyed maps.
- Replaced `Col_WordType` enum by OR-able constants.
- Replaced `Col_GetWordInfo` proc and `Col_WordData` struct by `Col_WordType` proc and type-specific accessors.
- Renamed `Col\_(Preserve|Release)Word` to `Col_Word(Preserve|Release)` for consistency.
- Replaced `WORD_TYPE_MLIST`, `WORD_TYPE_INT` and `WORD_TYPE_FP` internal types by improved `WORD_TYPE_WRAP`.
- Error handling & runtime check improvements.
- Unit testing overhaul.
- Major documentation improvements.
- Replaced Natural Docs with [Doxygen](http://www.doxygen.nl).

### Removed

- Removed Visual Studio and Unix Makefiles, build now uses CMake.
- Moved unit testing framework to own project [PicoTest](https://github.com/fredericbonnet/picotest).
- Removed `Col_FindWordInfo` proc, client code must now iterate over synonym chain and check types manually.

### Fixed

- Initializing vectors with a `NULL` element array no longer raises an error.
- Fixed case when adding a synonym that is already part of the synonym chain.
- Incorrect immediate float tag handling.
- GCC fixes.

## [0.14] - 2012-04-19

### Added

- Direct value accessors for integer and floating point words; calling `Col_GetWordInfo` is no longer required.
- Support for UTF-16.
- Rope normalization functions (character format conversion & rope tree flattening) for easier interoperability (notably system calls).
- String buffer for efficient string writing and interchange.
- Added null initializer constant and predicate for iterators; useful for telling between uninitialized and end conditions.

### Changed

- Improved support for UTF-8.
- Rope & list iterator can now move backwards from end.
- Rope iterators now work on flat character arrays (e.g. static C strings) in addition to ropes.
- Source tree and documentation improvements.
- Windows build environment is now Visual Studio 2010.

## [0.13] - 2011-12-02

### Fixed

- There was a deadlock in the Unix version where a pthread mutex was entered recursively, which is forbidden.

## [0.12] - 2011-11-30

### Added

- `Col_RopeFind` (similar to `strchr`/`strrchr`).
- `Col_RopeSearch` (similar to `strstr`).
- `Col_NewChar` for easier single-char strings. Such strings use immutable words and thus take up no allocated space.

### Changed

- Improved iterator & traversal interfaces. Ropes, maps and lists can now be iterated and traversed in reverse.
- `Col_CompareRopes` now have several versions for simpler usage (similar to
  `strcmp`/`strncmp`).
- Implemented page write protection-based parent tracking. This is a major internal change in the generational GC. In previous versions, parents (i.e. cells having children in younger generations) had to be tracked explicitly by client code calls to `Col_WordSetModified` each time a mutable cell was modified. This caused a significant performance penalty and added complexity in client code. Now parents are automatically tracked thanks to memory page write protection: pages of older generations are write-protected, and write attempts are caught (using exceptions on Windows and SIGSEGV signal handling on Unix). When such a page write is detected, the write protection is removed and the page marked as modified, and the client code resumes. All this happens transparently, so that client code no longer have to track modifications. Upon GC, cells belonging to modified pages are followed as potential parents so that younger reachable cells are marked as well. As a result, client code is much simpler to write (this is notable for all mutable structures such as maps or vectors) and less error prone (failure to track parents can result in catastrophic memory corruption), but runs faster too due to the absence of book-keeping on the client side. Some benchmarks show a speedup of 25%.

### Fixed

- Some bug fixes.

## [0.11] - 2011-06-23

### Added

- Floating point word type, in both immediate and regular versions.

### Changed

- Some immediate word binary tag changes to make room for floating points.
- Reworked cyclic list handling. Circular lists are now immediate words that wrap regular lists; cyclic lists are built by concatenating a regular and a circular list.
- Added immutable internal map structures (both hash and trie maps); maps are still mutable datatypes but can now share data with copy-on-write semantics.
- Improved trie map access: tries can now be iterated in reverse and use key prefix matching.

### Removed

- `COL_CHAR` type, single-char words now use `COL_STRING`.

## [0.10] - 2011-04-19

### Added

- 64-bit support on Linux.
- Public & internal documentation. Documentation is automatically generated from source files thanks to [Natural Docs](http://www.naturaldocs.org/).

## [0.9] - 2011-03-10

### Added

- Improved threading model. Previous versions used a strict appartment model: each thread had its own memory pools & GC process. Consequently data could not be shared across threads, which is suboptimal when using immutable structures (especially strings and ropes). Individual threads can now choose which model to follow at initialization time.
- `COL_SINGLE` model: strict appartment, synchronous GC, no data sharing, deterministic behavior.
- `COL_ASYNC` model: strict appartment, asynchronous GC process in a dedicated thread thread.
- `COL_SHARED` model: allows several threads to share data. There can be several distinct "groups" of threads in the same process, each having shared memory pools and a single background GC thread. For example, `COL_SHARED` and `COL_SHARED+1` define two distinct shared groups. Immutable data can be shared safely within a group without specific handling, however mutable data needs explicit, external synchronization depending on the application's needs. Contention is minimal: memory allocation is thread-local and as fast as with single-threaded models, older structures are merged in the shared pool during the GC process. Blocking only occurs when a GC is ongoing. Benchmarks show that this model scales very well.

## [0.8] - 2011-02-21

### Added

- Generic map iterator interface for map types with the same key type (integer or string).
- String map interface.
- String hash map type. Similar to Tcl hash tables, unordered collection with O(1) access, but O(k) hash computation (k being the key length).
- String crit-bit trie map type. Ordered collection, binary tree with O(log(n)) access, but better mean access than hash maps with long key strings.

### Changed

- Merged ropes and words. Ropes are now regular words. Custom rope types are now custom words.
- Rope and list traversal improvement. An arbitrary number of ropes or lists can be traversed in parallel, à la Tcl's `[foreach]`. This is useful for e.g. string comparisons.
- Completely reworked parent handling (i.e. cells of older generations having children in younger generations). The previous implementation used one cell per parent, now parents are handled on a per-page basis. This paves the way for transparent support of parent detection through page guards.
- Rework of word synonym handling. Immutable word types no longer have a synonym field internally, but use a special wrapper word when needed. The goal is to distinguish immutable vs. mutable data for better memory management (and possibly memory protection and inter-thread sharing in the future).

### Removed

- As a consequence of the rope-word merger, raw C string are no longer valid ropes (however conversion procs are provided).
- Removed old string word types, ropes can now take advantage of words' "immediate" representation (e.g. a 3-byte string takes up no allocated space and is stored within the pointer value)

## [0.7] - 2010-12-20

### Added

- Generic map interface.
- Generic integer map interface.
- Integer hash map type. Similar to Tcl hash tables, unordered collection with O(1) access.
- Integer crit-bit trie map type. Ordered collection, binary tree with O(log(n)) access.
- Cells can be pinned, i.e. marked as unmovable when compacting GC is performed.
- Benchmarking framework based on Tcl. Specific tests are exposed as Tcl commands, and stress tests are written as Tcl scripts. Used to compare Colibri datatype performances to Tcl, STL and others

### Changed

- System page allocator now supports arbitrarily sized pages.
- Data structures are no longer limited in size (previous version were constrained by the page size, 1KB on 32-bit systems).
- Roots no longer wrap the preserved cells but instead are stored in a dedicated trie indexed by the cell address; cells with positive refcount are pinned so that their address doesn't change.
- Parents (i.e. cells from older generations pointing to newer ones) are now managed on a per page basis; this improves the locality of reference during GC, and simplifies declaration: instead of calling `Col_DeclareChild` with parent and child cells, client code simply calls `Col_SetModified` on the parent.
- A lot of performance optimizations.

## [0.6] - 2010-10-25

### Added

- Mutable list type. Efficient sparse representation of very large lists thanks to "void lists". Copy on write model, can be mixed with immutable types while preserving respective semantics. Can be "frozen" and downgraded to immutable lists at no cost.
- Test suite. Follows the xUnit model. Implemented as a set of macros in a small, single C header file.

### Fixed

- Lots of bug fixes thanks to the test suite.

## [0.5] - 2010-10-08

### Added

- Cyclic lists. Each list has a loop length field that, when non-zero, gives the length of the terminating loop. Cyclic loops can also be easily detected during iteration thanks to the iterator index.
- Sparse lists. Lists can now contain void parts (i.e. whose elements are all nil) of arbitrary lengths at the cost of a single machine word.
- Mutable vector word type. Can grow and shrink arbitrarily within a reserved size set at creation time, Can be "frozen" and downgraded to immutable vectors at no cost.
- Basic exception handling. Catches critical conditions and validation errors with meaningful message. Default handler calls abort(), can be overridden by user proc.
- Preliminary work for mutable list type. Will use the new sparse list facility, and downgrade to immutable form easily.

### Removed

- Sequence word type. Awkward, hard to use, didn't quite fit the purpose, and doing a mutable version was too complicated.

## [0.4] - 2010-08-19

### Added

- Reference word type.
- Sequence word type. Potentially unlimited and self-referencing collections with efficient cycle detection.
- Improved source code documentation on the internal data structures.

### Changed

- Type- and nil/`NULL`- related cleanup.

## [0.3] - 2009-09-22

### Added

- Multithreading support. Appartment model, each thread has its own memory pool.
- Linux port. Linux version uses `mmap` and `pthreads`.
- Multithreading support in tests.

### Changed

- Now uses C99 types such as `int8_t` and `uintptr_t`.
- Moved platform-specific code to platform/\- subtree.
- Each test now has its own C file.

## [0.2] - 2009-04-16

### Added

- Vector word type. Vectors are flat collections of words. Their size is limited, as they must fit within one single page. They are directly addressable.
- List word type. Lists are linear collections of words, made by assembling vectors into balanced binary trees. They are to vectors what ropes are to strings. Like ropes, lists are immutable.
- Iterator and traversal APIs.

### Changed

- Roots are now sorted by generation for better performances with large number of instances.
- Parent-child relationships now use one cell per parent instead of one per parent-child pair.
- Roots and parent-child relationships now use cells allocated in a dedicated memory pool.
- Promotion is done per page rather than per cell: whole pages are moved up one generation at each GC. CPU overhead is much smaller.
- The first generation is always emptied at each GC, this speeds up further allocations.
- The last collected generation is by default merged with the first uncollected one, but may be relocated if fragmentation reaches a certain threshold; this is done by copying individual cells instead of moving pages.
- Relocation and redirection are now performed inline during the mark phase, versus during later steps in previous versions. The resulting code is much simpler, and avoids cache-intensive traversal of whole memory pools.
- Overall performance improvements.

## [0.1] - 2009-02-14

### Added

- Generic word system.
- Immediate word types: integer, single characters, small strings.
- Ropes.
- Cell and page allocator.
- Garbage collector.

[unreleased]: https://github.com/fredericbonnet/colibri/compare/v0.15.0...HEAD
[0.15.0]: https://github.com/fredericbonnet/colibri/compare/v0.14...v0.15.0
[0.14]: https://github.com/fredericbonnet/colibri/compare/v0.13...v0.14
[0.13]: https://github.com/fredericbonnet/colibri/compare/v0.12...v0.13
[0.12]: https://github.com/fredericbonnet/colibri/compare/v0.11...v0.12
[0.11]: https://github.com/fredericbonnet/colibri/compare/v0.10...v0.11
[0.10]: https://github.com/fredericbonnet/colibri/compare/v0.9...v0.10
[0.9]: https://github.com/fredericbonnet/colibri/compare/v0.8...v0.9
[0.8]: https://github.com/fredericbonnet/colibri/compare/v0.7...v0.8
[0.7]: https://github.com/fredericbonnet/colibri/compare/v0.6...v0.7
[0.6]: https://github.com/fredericbonnet/colibri/compare/v0.5...v0.6
[0.5]: https://github.com/fredericbonnet/colibri/compare/v0.4...v0.5
[0.4]: https://github.com/fredericbonnet/colibri/compare/v0.3...v0.4
[0.3]: https://github.com/fredericbonnet/colibri/compare/v0.2...v0.3
[0.2]: https://github.com/fredericbonnet/colibri/compare/v0.1...v0.2
[0.1]: https://github.com/fredericbonnet/colibri/releases/tag/v0.1
