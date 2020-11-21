# Structures

* [AddressRange](struct_address_range.md#struct_address_range): Address range descriptor for allocated system pages.
* [Col\_CustomHashMapType](struct_col___custom_hash_map_type.md#struct_col___custom_hash_map_type): Custom hash map type descriptor.
* [Col\_CustomIntMapType](struct_col___custom_int_map_type.md#struct_col___custom_int_map_type): Custom integer map type descriptor.
* [Col\_CustomListType](struct_col___custom_list_type.md#struct_col___custom_list_type): Custom list type descriptor.
* [Col\_CustomMapType](struct_col___custom_map_type.md#struct_col___custom_map_type): Custom map type descriptor.
* [Col\_CustomRopeType](struct_col___custom_rope_type.md#struct_col___custom_rope_type): Custom rope type descriptor.
* [Col\_CustomTrieMapType](struct_col___custom_trie_map_type.md#struct_col___custom_trie_map_type): Custom trie map type descriptor.
* [Col\_CustomWordType](struct_col___custom_word_type.md#struct_col___custom_word_type): Custom word type descriptor.
* [Col\_RopeChunk](struct_col___rope_chunk.md#struct_col___rope_chunk): Describes a rope chunk encountered during traversal.
* [ColListIterator](struct_col_list_iterator.md#struct_col_list_iterator): Internal implementation of list iterators.
* [ColListIterator.chunk](struct_col_list_iterator_8chunk.md#struct_col_list_iterator_8chunk): Current chunk info.
* [ColListIterator.chunk.current.access](struct_col_list_iterator_8chunk_8current_8access.md#struct_col_list_iterator_8chunk_8current_8access): Current element information in accessor mode.
* [ColMapIterator](struct_col_map_iterator.md#struct_col_map_iterator): Internal implementation of map iterators.
* [ColMapIterator.traversal.hash](struct_col_map_iterator_8traversal_8hash.md#struct_col_map_iterator_8traversal_8hash): Hash-specific fields.
* [ColMapIterator.traversal.trie](struct_col_map_iterator_8traversal_8trie.md#struct_col_map_iterator_8traversal_8trie): Trie-specific fields.
* [ColRopeIterator](struct_col_rope_iterator.md#struct_col_rope_iterator): Internal implementation of rope iterators.
* [ColRopeIterator.chunk](struct_col_rope_iterator_8chunk.md#struct_col_rope_iterator_8chunk): Current chunk info.
* [ColRopeIterator.chunk.current.access](struct_col_rope_iterator_8chunk_8current_8access.md#struct_col_rope_iterator_8chunk_8current_8access): Current element information in accessor mode.
* [ColRopeIterator.chunk.current.direct](struct_col_rope_iterator_8chunk_8current_8direct.md#struct_col_rope_iterator_8chunk_8current_8direct): Current element information in direct access mode.
* [CompareChunksInfo](struct_compare_chunks_info.md#struct_compare_chunks_info): Structure used to collect data during rope comparison.
* [CopyDataInfo](struct_copy_data_info.md#struct_copy_data_info): Structure used to copy data during the traversal of ropes when normalizing data.
* [FindCharInfo](struct_find_char_info.md#struct_find_char_info): Structure used to collect data during character search.
* [GroupData](struct_group_data.md#struct_group_data): Group-local data.
* [ListChunkTraverseInfo](struct_list_chunk_traverse_info.md#struct_list_chunk_traverse_info): Structure used during recursive list chunk traversal.
* [ListChunkTraverseInfo.backtracks](struct_list_chunk_traverse_info_8backtracks.md#struct_list_chunk_traverse_info_8backtracks): Pre-allocated backtracking structure.
* [MemoryPool](struct_memory_pool.md#struct_memory_pool): Memory pools are a set of pages storing the cells of a given generation.
* [MergeListChunksInfo](struct_merge_list_chunks_info.md#struct_merge_list_chunks_info): Structure used to collect data during the traversal of lists when merged into one vector.
* [MergeRopeChunksInfo](struct_merge_rope_chunks_info.md#struct_merge_rope_chunks_info): Structure used to collect data during the traversal of ropes when merged into one string.
* [RopeChunkTraverseInfo](struct_rope_chunk_traverse_info.md#struct_rope_chunk_traverse_info): Structure used during recursive rope chunk traversal.
* [RopeChunkTraverseInfo.backtracks](struct_rope_chunk_traverse_info_8backtracks.md#struct_rope_chunk_traverse_info_8backtracks): Pre-allocated backtracking structure.
* [SearchSubropeInfo](struct_search_subrope_info.md#struct_search_subrope_info): Structure used to collect data during subrope search.
* [ThreadData](struct_thread_data.md#struct_thread_data): Thread-local data.
* [UnixGroupData](struct_unix_group_data.md#struct_unix_group_data): Platform-specific group data.
* [Win32GroupData](struct_win32_group_data.md#struct_win32_group_data): Platform-specific group data.