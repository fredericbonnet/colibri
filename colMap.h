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
 * Group: Map Entries
 *
 * Declarations:
 *	<Col_MapEntryGet>, <Col_IntMapEntryGet>, <Col_MapEntryGetKey>,
 *	<Col_IntMapEntryGetKey>, <Col_MapEntryGetValue>, <Col_MapEntrySetValue>
 ****************************************************************************/

EXTERN void		Col_MapEntryGet(Col_Word entry, Col_Word *keyPtr,
			    Col_Word *valuePtr);
EXTERN void		Col_IntMapEntryGet(Col_Word entry, intptr_t *keyPtr,
			    Col_Word *valuePtr);
EXTERN Col_Word		Col_MapEntryGetKey(Col_Word entry);
EXTERN intptr_t		Col_IntMapEntryGetKey(Col_Word entry);
EXTERN Col_Word		Col_MapEntryGetValue(Col_Word entry);
EXTERN void		Col_MapEntrySetValue(Col_Word entry, Col_Word value);


/****************************************************************************
 * Group: Map Iterators
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Typedef: ColMapIterator
 *
 *	Internal implementation of map iterators.
 *
 * Fields:
 *	map	- Map being iterated.
 *	index	- Current entry.
 *	bucket	- Current bucket (for hash maps).
 *
 * See also:
 *	<Col_MapIterator>
 *---------------------------------------------------------------------------*/

typedef struct ColMapIterator {
    Col_Word map;
    Col_Word entry;
    size_t bucket;
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

/*---------------------------------------------------------------------------
 * Macro: Col_MapIterEntry
 *
 *	Get current entry within map for iterator.
 *
 * Arguments:
 *	it	- The iterator to get entry for.
 *
 * Result:
 *	Current entry. Undefined if at end.
 *
 * See also: 
 *	<Col_MapIterator>
 *---------------------------------------------------------------------------*/

#define Col_MapIterEntry(it) \
    ((it)->entry)

#endif /* _COLIBRI_MAP */
