/*
 * Header: colTrie.h
 *
 *	This header file defines the trie map handling features of Colibri.
 *
 *	Trie maps are an implementation of generic maps that use crit-bit trees
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
 * Section: Trie Map Creation
 *
 * Declarations:
 *	<Col_NewStringTrieMap>, <Col_NewIntTrieMap>
 ****************************************************************************/

EXTERN Col_Word		Col_NewStringTrieMap();
EXTERN Col_Word		Col_NewIntTrieMap();
EXTERN Col_Word		Col_CopyTrieMap(Col_Word map);


/****************************************************************************
 * Section: Trie Map Access
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
 * Section: Trie Map Iterators
 *
 * Declarations:
 *	<Col_TrieMapIterFirst>, <Col_TrieMapIterLast>, 
 *	<Col_StringTrieMapIterFind>, <Col_IntTrieMapIterFind>, 
 *	<Col_TrieMapIterSetValue>, <Col_TrieMapIterNext>, 
 *	<Col_TrieMapIterPrevious>
 ****************************************************************************/

EXTERN void		Col_TrieMapIterFirst(Col_Word map, 
			    Col_MapIterator *it);
EXTERN void		Col_TrieMapIterLast(Col_Word map, 
			    Col_MapIterator *it);
EXTERN void		Col_StringTrieMapIterFind(Col_Word map, Col_Word key, 
			    int *createPtr, Col_MapIterator *it);
EXTERN void		Col_IntTrieMapIterFind(Col_Word map, intptr_t key, 
			    int *createPtr, Col_MapIterator *it);
EXTERN void		Col_TrieMapIterSetValue(Col_MapIterator *it, 
			    Col_Word value);
EXTERN void		Col_TrieMapIterNext(Col_MapIterator *it);
EXTERN void		Col_TrieMapIterPrevious(Col_MapIterator *it);

#endif /* _COLIBRI_TRIE */
