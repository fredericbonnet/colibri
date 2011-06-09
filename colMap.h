/*
 * File: colMap.h
 *
 *	This header file defines the map handling features of Colibri.
 *
 *	Maps are a collection datatype that associates keys to values. Keys can
 *	be integers, strings or generic words.
 *
 *	They are always mutable.
 */

#ifndef _COLIBRI_MAP
#define _COLIBRI_MAP

#include <stddef.h> /* For size_t */


/****************************************************************************
 * Group: Map Access
 *
 * Declarations:
 *	<Col_MapSize>, <Col_MapGet>, <Col_IntMapGet>, <Col_MapSet>,
 *	<Col_IntMapSet>, <Col_MapUnset>, <Col_IntMapUnset>
 ****************************************************************************/

EXTERN size_t		Col_MapSize(Col_Word map);
EXTERN int		Col_MapGet(Col_Word map, Col_Word key, 
			    Col_Word *valuePtr);
EXTERN int		Col_IntMapGet(Col_Word map, intptr_t key, 
			    Col_Word *valuePtr);
EXTERN int		Col_MapSet(Col_Word map, Col_Word key, Col_Word value);
EXTERN int		Col_IntMapSet(Col_Word map, intptr_t key, Col_Word value);
EXTERN int		Col_MapUnset(Col_Word map, Col_Word key);
EXTERN int		Col_IntMapUnset(Col_Word map, intptr_t key);


/****************************************************************************
 * Group: Map Iterators
 *
 * Declarations:
 *	<Col_MapIterBegin>, <Col_MapIterFind>, <Col_IntMapIterFind>, 
 *	<Col_MapIterGet>, <Col_IntMapIterGet>, <Col_MapIterGetKey>, 
 *	<Col_IntMapIterGetKey>, <Col_MapIterGetValue>, <Col_MapIterSetValue>, 
 *	<Col_MapIterNext>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Typedef: ColMapIterator
 *
 *	Internal implementation of map iterators.
 *
 * Fields:
 *	map		- Map being iterated.
 *	entry		- Current entry.
 *
 * Hash-specific fields:
 *	hash.bucket	- Current bucket index.
 *
 * Trie-specific fields:
 *	trie.up		- Uplink to deepest common ancestor with next entry. The
 *			  next entry will be the leftmost leaf of the ancestor's 
 *			  right subtrie (current entry being the rightmost leaf 
 *			  of the ancestor's left subtrie). If nil, will be
 *			  recomputed by in-depth traversal.
 *
 * See also:
 *	<Col_MapIterator>
 *---------------------------------------------------------------------------*/

typedef struct ColMapIterator {
    Col_Word map;
    Col_Word entry;
    union {
	struct {
	    size_t bucket;
	} hash;
	struct {
	    Col_Word up;
	} trie;
    };
} ColMapIterator;

/*---------------------------------------------------------------------------
 * Typedef: Col_MapIterator
 *
 *	Map iterator. Encapsulates the necessary info to iterate & access map
 *	data transparently.
 *
 * Note:
 *	Datatype is opaque. Fields should not be accessed by client code.
 *
 *	Each iterator takes 3 words on the stack.
 *---------------------------------------------------------------------------*/

typedef ColMapIterator Col_MapIterator;

/*---------------------------------------------------------------------------
 * Macro: Col_MapIterEnd
 *
 *	Test whether iterator reached end of map.
 *
 * Arguments:
 *	it	- The iterator to test.
 *
 * Result:
 *	Non-zero if iterator is at end.
 *
 * See also: 
 *	<Col_MapIterator>
 *---------------------------------------------------------------------------*/

#define Col_MapIterEnd(it) \
    (!(it)->map)

/*---------------------------------------------------------------------------
 * Macro: Col_MapIterMap
 *
 *	Get map for iterator.
 *
 * Arguments:
 *	it	- The iterator to get map for.
 *
 * Result:
 *	The map, or <WORD_NIL> if at end.
 *
 * See also: 
 *	<Col_MapIterator>, <Col_MapIterEnd>
 *---------------------------------------------------------------------------*/

#define Col_MapIterMap(it) \
    ((it)->map)

EXTERN void		Col_MapIterBegin(Col_Word map, Col_MapIterator *it);
EXTERN void		Col_MapIterFind(Col_Word map, Col_Word key, 
			    int *createPtr, Col_MapIterator *it);
EXTERN void		Col_IntMapIterFind(Col_Word map, intptr_t key, 
			    int *createPtr, Col_MapIterator *it);
EXTERN void		Col_MapIterGet(Col_MapIterator *it, Col_Word *keyPtr,
			    Col_Word *valuePtr);
EXTERN void		Col_IntMapIterGet(Col_MapIterator *it, intptr_t *keyPtr,
			    Col_Word *valuePtr);
EXTERN Col_Word		Col_MapIterGetKey(Col_MapIterator *it);
EXTERN intptr_t		Col_IntMapIterGetKey(Col_MapIterator *it);
EXTERN Col_Word		Col_MapIterGetValue(Col_MapIterator *it);
EXTERN void		Col_MapIterSetValue(Col_MapIterator *it, 
			    Col_Word value);
EXTERN void		Col_MapIterNext(Col_MapIterator *it);

#endif /* _COLIBRI_MAP */
