/*
 * Header: colHash.h
 *
 *	This header file defines the hash map handling features of Colibri.
 *
 *	Hash maps are an implementation of generic maps that use key hashing 
 *	and flat bucket arrays for string, integer and custom keys.
 *
 *	They are always mutable.
 *
 * See also:
 *	<colHash.c>, <colMap.h>
 */

#ifndef _COLIBRI_HASH
#define _COLIBRI_HASH

#include <stddef.h> /* For size_t */


/*
================================================================================
Section: Hash Maps
================================================================================
*/

/****************************************************************************
 * Group: Hash Map Creation
 *
 * Declarations:
 *	<Col_NewStringHashMap>, <Col_NewIntHashMap>, <Col_CopyHashMap>
 ****************************************************************************/

EXTERN Col_Word		Col_NewStringHashMap(size_t capacity);
EXTERN Col_Word		Col_NewIntHashMap(size_t capacity);
EXTERN Col_Word		Col_CopyHashMap(Col_Word map);


/****************************************************************************
 * Group: Hash Map Accessors
 *
 * Declarations:
 *	<Col_HashMapGet>, <Col_IntHashMapGet>, <Col_HashMapSet>,
 *	<Col_IntHashMapSet>, <Col_HashMapUnset>, <Col_IntHashMapUnset>
 ****************************************************************************/

EXTERN int		Col_HashMapGet(Col_Word map, Col_Word key, 
			    Col_Word *valuePtr);
EXTERN int		Col_IntHashMapGet(Col_Word map, intptr_t key, 
			    Col_Word *valuePtr);
EXTERN int		Col_HashMapSet(Col_Word map, Col_Word key, 
			    Col_Word value);
EXTERN int		Col_IntHashMapSet(Col_Word map, intptr_t key, 
			    Col_Word value);
EXTERN int		Col_HashMapUnset(Col_Word map, Col_Word key);
EXTERN int		Col_IntHashMapUnset(Col_Word map, intptr_t key);


/****************************************************************************
 * Group: Hash Map Iteration
 *
 * Declarations:
 *	<Col_HashMapIterBegin>, <Col_HashMapIterFind>, <Col_IntHashMapIterFind>,
 *	<Col_HashMapIterSetValue>, <Col_HashMapIterNext>
 ****************************************************************************/

EXTERN void		Col_HashMapIterBegin(Col_Word map, Col_MapIterator *it);
EXTERN void		Col_HashMapIterFind(Col_Word map, Col_Word key, 
			    int *createPtr, Col_MapIterator *it);
EXTERN void		Col_IntHashMapIterFind(Col_Word map, intptr_t key, 
			    int *createPtr, Col_MapIterator *it);
EXTERN void		Col_HashMapIterSetValue(Col_MapIterator *it, 
			    Col_Word value);
EXTERN void		Col_HashMapIterNext(Col_MapIterator *it);


/*
================================================================================
Section: Custom Hash Maps
================================================================================
*/

/*---------------------------------------------------------------------------
 * Typedef: Col_HashProc
 *
 *	Function signature of custom hash map's key hashing function.
 *
 * Argument:
 *	key	- Key word to generate hash value for.
 *
 * Result:
 *	The key hash value.
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
 *	<Col_NewCustomWord>, <Col_NewCustomHashMap>, <Col_CustomWordType>, 
 *	<Col_HashProc>, <Col_CompareProc>
 *---------------------------------------------------------------------------*/

typedef struct Col_CustomHashMapType {
    Col_CustomWordType type;
    Col_HashProc *hashProc;
    Col_CompareProc *compareProc;
} Col_CustomHashMapType;

/****************************************************************************
 * Group: Custom Hash Map Creation
 *
 * Declarations:
 *	<Col_NewCustomHashMap>
 ****************************************************************************/

EXTERN Col_Word		Col_NewCustomHashMap(Col_CustomHashMapType *type, 
			    size_t capacity, size_t size, void **dataPtr);

#endif /* _COLIBRI_HASH */
