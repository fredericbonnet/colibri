/*
 * File: colHash.h
 *
 *	This header file defines the hash map handling features of Colibri.
 *
 *	Hash maps are an implementation of generic maps that uses key hashing 
 *	and flat bucket arrays for string and integer keys.
 *
 *	They are always mutable.
 *
 * See also:
 *	<colMap.h>
 */

#ifndef _COLIBRI_HASH
#define _COLIBRI_HASH


/****************************************************************************
 * Group: Hash Map Creation
 *
 * Declarations:
 *	<Col_NewStringHashMap>, <Col_NewIntHashMap>
 ****************************************************************************/

EXTERN Col_Word		Col_NewStringHashMap(size_t capacity);
EXTERN Col_Word		Col_NewIntHashMap(size_t capacity);


/****************************************************************************
 * Group: Hash Map Access
 *
 * Declarations:
 *	<Col_StringHashMapGet>, <Col_IntHashMapGet>, <Col_StringHashMapSet>,
 *	<Col_IntHashMapSet>, <Col_StringHashMapUnset>, <Col_IntHashMapUnset>
 ****************************************************************************/

EXTERN int		Col_StringHashMapGet(Col_Word map, Col_Word key, 
			    Col_Word *valuePtr);
EXTERN int		Col_IntHashMapGet(Col_Word map, intptr_t key, 
			    Col_Word *valuePtr);
EXTERN int		Col_StringHashMapSet(Col_Word map, Col_Word key, 
			    Col_Word value);
EXTERN int		Col_IntHashMapSet(Col_Word map, intptr_t key, 
			    Col_Word value);
EXTERN int		Col_StringHashMapUnset(Col_Word map, Col_Word key);
EXTERN int		Col_IntHashMapUnset(Col_Word map, intptr_t key);


/****************************************************************************
 * Group: Hash Map Iterators
 *
 * Declarations:
 *	<Col_HashMapIterBegin>, <Col_HashMapIterNext>,
 *	<Col_StringHashMapIterFind>, <Col_IntHashMapIterFind>
 ****************************************************************************/

EXTERN void		Col_HashMapIterBegin(Col_Word map, 
			    Col_MapIterator *it);
EXTERN void		Col_HashMapIterNext(Col_MapIterator *it);
EXTERN void		Col_StringHashMapIterFind(Col_Word map, Col_Word key, 
			    int *createPtr, Col_MapIterator *it);
EXTERN void		Col_IntHashMapIterFind(Col_Word map, intptr_t key, 
			    int *createPtr, Col_MapIterator *it);


/****************************************************************************
 * Group: Custom Hash Maps
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Typedef: Col_HashProc
 *
 *	Function signature of custom hash map's key hashing function.
 *
 * Arguments:
 *	key		- Key word to generate hash value for.
 *
 * Result:
 *	The hash hash value.
 *
 * See also: 
 *	<Col_CustomHashMapType>
 *---------------------------------------------------------------------------*/

typedef uintptr_t (Col_HashProc) (Col_Word key);

/*---------------------------------------------------------------------------
 * Typedef: Col_CompareProc
 *
 *	Function signature of custom hash map's key comparison function.
 *
 * Arguments:
 *	key1, key2	- Key words to compare.
 *
 * Result:
 *	Negative if key1 is less than key2, positive if key2 is greater than
 *	key2, zero if both keys are equal.
 *
 * See also: 
 *	<Col_CustomHashMapType>
 *---------------------------------------------------------------------------*/

typedef int (Col_CompareProc) (Col_Word key1, Col_Word key2);

/*---------------------------------------------------------------------------
 * Typedef: Col_CustomHashMapType
 *
 *	Custom hash map type descriptor. Inherits from <Col_CustomWordType>.
 *
 * Fields:
 *	type		- Generic word type descriptor. Type field must be equal
 *			  to <COL_HASHMAP>.
 *	hashProc	- Called to compute the hash value of a key.
 *	compareProc	- Called to compare keys.
 *
 * See also:
 *	<Col_CustomWordType>, <Col_HashProc>, <Col_CompareProc>
 *---------------------------------------------------------------------------*/

typedef struct Col_CustomHashMapType {
    Col_CustomWordType type;
    Col_HashProc *hashProc;
    Col_CompareProc *compareProc;
} Col_CustomHashMapType;

#endif /* _COLIBRI_HASH */
