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
 *	<colTrie.c>, <colMap.h>
 */

#ifndef _COLIBRI_TRIE
#define _COLIBRI_TRIE


/*
================================================================================
Section: Trie Maps
================================================================================
*/

/****************************************************************************
 * Group: Trie Map Creation
 *
 * Declarations:
 *	<Col_NewStringTrieMap>, <Col_NewIntTrieMap>
 ****************************************************************************/

EXTERN Col_Word		Col_NewStringTrieMap();
EXTERN Col_Word		Col_NewIntTrieMap();
EXTERN Col_Word		Col_CopyTrieMap(Col_Word map);


/****************************************************************************
 * Group: Trie Map Accessors
 *
 * Declarations:
 *	<Col_TrieMapGet>, <Col_IntTrieMapGet>, <Col_TrieMapSet>,
 *	<Col_IntTrieMapSet>, <Col_TrieMapUnset>, <Col_IntTrieMapUnset>
 ****************************************************************************/

EXTERN int		Col_TrieMapGet(Col_Word map, Col_Word key,
			    Col_Word *valuePtr);
EXTERN int		Col_IntTrieMapGet(Col_Word map, intptr_t key, 
			    Col_Word *valuePtr);
EXTERN int		Col_TrieMapSet(Col_Word map, Col_Word key, 
			    Col_Word value);
EXTERN int		Col_IntTrieMapSet(Col_Word map, intptr_t key, 
			    Col_Word value);
EXTERN int		Col_TrieMapUnset(Col_Word map, Col_Word key);
EXTERN int		Col_IntTrieMapUnset(Col_Word map, intptr_t key);


/****************************************************************************
 * Group: Trie Map Iteration
 *
 * Declarations:
 *	<Col_TrieMapIterFirst>, <Col_TrieMapIterLast>, 
 *	<Col_TrieMapIterFind>, <Col_IntTrieMapIterFind>, 
 *	<Col_TrieMapIterSetValue>, <Col_TrieMapIterNext>, 
 *	<Col_TrieMapIterPrevious>
 ****************************************************************************/

EXTERN void		Col_TrieMapIterFirst(Col_Word map, 
			    Col_MapIterator *it);
EXTERN void		Col_TrieMapIterLast(Col_Word map, 
			    Col_MapIterator *it);
EXTERN void		Col_TrieMapIterFind(Col_Word map, Col_Word key, 
			    int *createPtr, Col_MapIterator *it);
EXTERN void		Col_IntTrieMapIterFind(Col_Word map, intptr_t key, 
			    int *createPtr, Col_MapIterator *it);
EXTERN void		Col_TrieMapIterSetValue(Col_MapIterator *it, 
			    Col_Word value);
EXTERN void		Col_TrieMapIterNext(Col_MapIterator *it);
EXTERN void		Col_TrieMapIterPrevious(Col_MapIterator *it);


/*
================================================================================
Section: Custom Trie Maps
================================================================================
*/

/*---------------------------------------------------------------------------
 * Typedef: Col_BitSetProc
 *
 *	Function signature of custom trie map's key bit test function.
 *
 * Arguments:
 *	key	- Key to get bit value for.
 *	index	- Index of key element.
 *	bit	- Position of bit to test.
 *
 * Result:
 *	The key bit value.
 *
 * See also: 
 *	<Col_CustomTrieMapType>
 *---------------------------------------------------------------------------*/

typedef int (Col_BitSetProc) (Col_Word key, size_t index, size_t bit);

/*---------------------------------------------------------------------------
 * Typedef: Col_KeyDiffProc
 *
 *	Function signature of custom trie map's key comparison function.
 *
 * Arguments:
 *	key1, key2	- Key words to compare.
 *
 * Result:
 *	Negative if key1 is less than key2, positive if key2 is greater than
 *	key2, zero if both keys are equal. Additionally:
 *
 *	- diffPtr   - Index of first differing key element.
 *	- bitPtr    - Critical bit where key elements differ.
 *
 * See also: 
 *	<Col_CustomHashMapType>
 *---------------------------------------------------------------------------*/

typedef int (Col_KeyDiffProc) (Col_Word key1, Col_Word key2, size_t *diffPtr, 
    size_t *bitPtr);

/*---------------------------------------------------------------------------
 * Typedef: Col_CustomTrieMapType
 *
 *	Custom trie map type descriptor. Inherits from <Col_CustomWordType>.
 *
 * Fields:
 *	type		- Generic word type descriptor. Type field must be equal
 *			  to <COL_TRIEMAP>.
 *	bitSetProc	- Called to test key bits.
 *	keyDiffProc	- Called to compare keys.
 *
 * See also:
 *	<Col_NewCustomWord>, <Col_NewCustomTrieMap>, <Col_CustomWordType>, 
 *	<Col_HashProc>, <Col_KeyDiffProc>
 *---------------------------------------------------------------------------*/

typedef struct Col_CustomTrieMapType {
    Col_CustomWordType type;
    Col_BitSetProc *bitSetProc;
    Col_KeyDiffProc *keyDiffProc;
} Col_CustomTrieMapType;

/****************************************************************************
 * Group: Custom Trie Map Creation
 *
 * Declarations:
 *	<Col_NewCustomTrieMap>
 ****************************************************************************/

EXTERN Col_Word		Col_NewCustomTrieMap(Col_CustomTrieMapType *type, 
			    size_t size, void **dataPtr);

#endif /* _COLIBRI_TRIE */
