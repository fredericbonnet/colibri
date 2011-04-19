/*
 * File: colTrie.h
 *
 *	This header file defines the trie map handling features of Colibri.
 *
 *	Trie maps are an implementation of generic maps that uses crit-bit trees
 *	for string and integer keys.
 *
 *	They are always mutable.
 *
 * See also:
 *	<colMap.h>
 */

#ifndef _COLIBRI_TRIE
#define _COLIBRI_TRIE


/****************************************************************************
 * Group: Trie Map Creation
 *
 * Declarations:
 *	<Col_NewStringTrieMap>, <Col_NewIntTrieMap>
 ****************************************************************************/

EXTERN Col_Word		Col_NewStringTrieMap();
EXTERN Col_Word		Col_NewIntTrieMap();


/****************************************************************************
 * Group: Trie Map Access
 *
 * Declarations:
 *	<Col_StringTrieMapGet>, <Col_IntTrieMapGet>, <Col_StringTrieMapSet>,
 *	<Col_IntTrieMapSet>, <Col_StringTrieMapUnset>, <Col_IntTrieMapUnset>
 ****************************************************************************/

/*
 * Crit-bit trie maps (string based).
 */

EXTERN int		Col_StringTrieMapGet(Col_Word map, Col_Word key,
			    Col_Word *valuePtr);
EXTERN int		Col_IntTrieMapGet(Col_Word map, intptr_t key, 
			    Col_Word *valuePtr);
EXTERN int		Col_StringTrieMapSet(Col_Word map, Col_Word key, 
			    Col_Word value);
EXTERN int		Col_IntTrieMapSet(Col_Word map, intptr_t key, 
			    Col_Word value);
EXTERN int		Col_StringTrieMapUnset(Col_Word map, Col_Word key);
EXTERN int		Col_IntTrieMapUnset(Col_Word map, intptr_t key);


/****************************************************************************
 * Group: Trie Map Entries
 *
 * Declarations:
 *	<Col_StringTrieMapFindEntry>, <Col_IntTrieMapFindEntry>
 ****************************************************************************/

EXTERN Col_Word		Col_StringTrieMapFindEntry(Col_Word map, Col_Word key, 
			    int *createPtr);
EXTERN Col_Word		Col_IntTrieMapFindEntry(Col_Word map, intptr_t key, 
			    int *createPtr);


/****************************************************************************
 * Group: Trie Map Iterators
 *
 * Declarations:
 *	<Col_TrieMapIterBegin>, <Col_TrieMapIterNext>
 ****************************************************************************/

EXTERN void		Col_TrieMapIterBegin(Col_Word map, 
			    Col_MapIterator *it);
EXTERN void		Col_TrieMapIterNext(Col_MapIterator *it);

#endif /* _COLIBRI_TRIE */
