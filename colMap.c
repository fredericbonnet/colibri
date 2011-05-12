/*
 * File: colMap.c
 *
 *	This file implements the map handling features of Colibri.
 *
 *	Maps are a collection datatype that associates keys to values. Keys can
 *	be integers, strings or generic words.
 *
 *	They are always mutable.
 *
 * See also:
 *	<colMap.h>
 */

#include "colibri.h"
#include "colInternal.h"


/****************************************************************************
 * Group: Map Access
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_MapSize
 *
 *	Get the size of the map. I.e. the number of entries.
 *
 * Argument:
 *	map	- Map to get size for.
 *
 * Result:
 *	The map size.
 *---------------------------------------------------------------------------*/

size_t
Col_MapSize(
    Col_Word map)
{
    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRHASHMAP:
	case WORD_TYPE_INTHASHMAP:
	    return WORD_HASHMAP_SIZE(map);

	case WORD_TYPE_STRTRIEMAP:
	case WORD_TYPE_INTTRIEMAP:
	    return WORD_TRIEMAP_SIZE(map);

	/* WORD_TYPE_UNKNOWN */

	default:
	    /*
	     * Invalid type.
	     */

	    Col_Error(COL_ERROR, "%x is not a map", map);
	    return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_MapGet
 *
 *	Get value mapped to the given key if present.
 *
 *	Generic interface to map get, calls actual function depending on type.
 *
 * Arguments:
 *	map		- Map to get entry from.
 *	key		- Entry key. Can be any word type, including string, 
 *			  however it must match the actual type used by the map.
 *	valuePtr	- Returned entry value, if found.
 *
 * Result:
 *	Whether the key was found in the map. In this case the value is returned
 *	through valuePtr.
 *---------------------------------------------------------------------------*/

int
Col_MapGet(
    Col_Word map,
    Col_Word key,
    Col_Word *valuePtr)
{
    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRHASHMAP:
	    return Col_StringHashMapGet(map, key, valuePtr);

	case WORD_TYPE_STRTRIEMAP:
	    return Col_StringTrieMapGet(map, key, valuePtr);

	/* WORD_TYPE_UNKNOWN */

	default:
	    Col_Error(COL_ERROR, "%x is not a string map", map);
	    return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_IntMapGet
 *
 *	Get value mapped to the given integer key if present.
 *
 *	Generic interface to integer map get, calls actual function depending on
 *	type.
 *
 * Arguments:
 *	map		- Integer map to get entry from.
 *	key		- Integer entry key.
 *	valuePtr	- Returned entry value, if found.
 *
 * Result:
 *	Whether the key was found in the map. In this case the value is returned
 *	through valuePtr.
 *---------------------------------------------------------------------------*/

int
Col_IntMapGet(
    Col_Word map,
    intptr_t key,
    Col_Word *valuePtr)
{
    switch (WORD_TYPE(map)) {
	case WORD_TYPE_INTHASHMAP:
	    return Col_IntHashMapGet(map, key, valuePtr);

	case WORD_TYPE_INTTRIEMAP:
	    return Col_IntTrieMapGet(map, key, valuePtr);

	/* WORD_TYPE_UNKNOWN */

	default:
	    Col_Error(COL_ERROR, "%x is not an integer map", map);
	    return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_MapSet
 *
 *	Map the value to the key, replacing any existing.
 *
 *	Generic interface to map set, calls actual function depending on type.
 *
 * Arguments:
 *	map	- Map to insert entry into.
 *	key	- Entry key. Can be any word type, including string, however it
 *		  must match the actual type used by the map.
 *	value	- Entry value.
 *
 * Result:
 *	Whether a new entry was created.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

int
Col_MapSet(
    Col_Word map,
    Col_Word key,
    Col_Word value)
{
    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRHASHMAP:
	    return Col_StringHashMapSet(map, key, value);

	case WORD_TYPE_STRTRIEMAP:
	    return Col_StringTrieMapSet(map, key, value);

	/* WORD_TYPE_UNKNOWN */

	default:
	    Col_Error(COL_ERROR, "%x is not a string map", map);
	    return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_IntMapSet
 *
 *	Map the value to the integer key, replacing any existing.
 *
 *	Generic interface to integer map set, calls actual function depending on
 *	type.
 *
 * Arguments:
 *	map	- Integer map to insert entry into.
 *	key	- Integer entry key.
 *	value	- Entry value.
 *
 * Result:
 *	Whether a new entry was created.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

int
Col_IntMapSet(
    Col_Word map,
    intptr_t key,
    Col_Word value)
{
    switch (WORD_TYPE(map)) {
	case WORD_TYPE_INTHASHMAP:
	    return Col_IntHashMapSet(map, key, value);

	case WORD_TYPE_INTTRIEMAP:
	    return Col_IntTrieMapSet(map, key, value);

	/* WORD_TYPE_UNKNOWN */

	default:
	    Col_Error(COL_ERROR, "%x is not an integer map", map);
	    return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_MapUnset
 *
 *	Remove any value mapped to the given key.
 *
 *	Generic interface to map unset, calls actual function depending on type.
 *
 * Arguments:
 *	map	- Map to remove entry from.
 *	key	- Entry key. Can be any word type, including string, however it
 *		  must match the actual type used by the map.
 *
 * Result:
 *	Whether an existing entry was removed.
 *---------------------------------------------------------------------------*/

int
Col_MapUnset(
    Col_Word map,
    Col_Word key)
{
    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRHASHMAP:
	    return Col_StringHashMapUnset(map, key);

	case WORD_TYPE_STRTRIEMAP:
	    return Col_StringTrieMapUnset(map, key);

	/* WORD_TYPE_UNKNOWN */

	default:
	    Col_Error(COL_ERROR, "%x is not a string map", map);
	    return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_IntMapUnset
 *
 *	Remove any value mapped to the given integer key.
 *
 *	Generic interface to integer map unset, calls actual function depending 
 *	on type.
 *
 * Arguments:
 *	map	- Integer map to remove entry from.
 *	key	- Integer entry key.
 *
 * Result:
 *	Whether an existing entry was removed.
 *---------------------------------------------------------------------------*/

int
Col_IntMapUnset(
    Col_Word map,
    intptr_t key)
{
    switch (WORD_TYPE(map)) {
	case WORD_TYPE_INTHASHMAP:
	    return Col_IntHashMapUnset(map, key);

	case WORD_TYPE_INTTRIEMAP:
	    return Col_IntTrieMapUnset(map, key);

	/* WORD_TYPE_UNKNOWN */

	default:
	    Col_Error(COL_ERROR, "%x is not an integer map", map);
	    return 0;
    }
}


/****************************************************************************
 * Group: Map Iterators
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_MapIterBegin
 *
 *	Initialize the map iterator so that it points to the first entry within
 *	the map.
 *
 * Arguments:
 *	map	- Map to iterate over.
 *	it	- Iterator to initialize.
 *---------------------------------------------------------------------------*/

void
Col_MapIterBegin(
    Col_Word map,
    Col_MapIterator *it)
{
    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRHASHMAP:
	case WORD_TYPE_INTHASHMAP:
	    Col_HashMapIterBegin(map, it);
	    break;

	case WORD_TYPE_STRTRIEMAP:
	case WORD_TYPE_INTTRIEMAP:
	    Col_TrieMapIterBegin(map, it);
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /*
	     * Invalid type.
	     */

	    Col_Error(COL_ERROR, "%x is not a map", map);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_MapIterFind
 *
 *	Initialize the map iterator so that it points to the entry with the
 *	given key within the map.
 *
 * Arguments:
 *	map		- Map to find or create entry into.
 *	key		- Entry key. Can be any word type, including string,
 *			  however it must match the actual type used by the map.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *	it		- Iterator to initialize.
 *
 * Results:
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *---------------------------------------------------------------------------*/

void
Col_MapIterFind(
    Col_Word map, 
    Col_Word key, 
    int *createPtr, 
    Col_MapIterator *it)
{
    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRHASHMAP:
	    Col_StringHashMapIterFind(map, key, createPtr, it);
	    break;

	case WORD_TYPE_STRTRIEMAP:
	    Col_StringTrieMapIterFind(map, key, createPtr, it);
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /*
	     * Invalid type.
	     */

	    Col_Error(COL_ERROR, "%x is not a generic or string map", map);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_IntMapIterFind
 *
 *	Initialize the map iterator so that it points to the entry with the 
 *	given integer key within the map.
 *
 * Arguments:
 *	map		- Integer hash map to find or create entry into.
 *	key		- Integer entry key.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *	it		- Iterator to initialize.
 *
 * Results:
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *---------------------------------------------------------------------------*/

void
Col_IntMapIterFind(
    Col_Word map, 
    intptr_t key, 
    int *createPtr, 
    Col_MapIterator *it)
{
    switch (WORD_TYPE(map)) {
	case WORD_TYPE_INTHASHMAP:
	    Col_IntHashMapIterFind(map, key, createPtr, it);
	    break;

	case WORD_TYPE_INTTRIEMAP:
	    Col_IntTrieMapIterFind(map, key, createPtr, it);
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /*
	     * Invalid type.
	     */

	    Col_Error(COL_ERROR, "%x is not an integer map", map);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_MapIterGet
 *
 *	Get key & value from map iterator.
 *
 * Argument:
 *	it		- Map iterator to get key & value from.
 *
 * Results:
 *	keyPtr		- Entry key.
 *	valuePtr	- Entry value.
 *---------------------------------------------------------------------------*/

void
Col_MapIterGet(
    Col_MapIterator *it,
    Col_Word *keyPtr,
    Col_Word *valuePtr)
{
    if (Col_MapIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid map iterator");
	return;
    }

    switch (WORD_TYPE(it->entry)) {
	case WORD_TYPE_HASHENTRY:
	case WORD_TYPE_MHASHENTRY:
	case WORD_TYPE_TRIELEAF:
	    *keyPtr = WORD_MAPENTRY_KEY(it->entry);
	    *valuePtr = WORD_MAPENTRY_VALUE(it->entry);
	    break;

	default:
	    Col_Error(COL_ERROR, "Not a generic or string map iterator");
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_IntMapIterGet
 *
 *	Get key & value from integer map iterator.
 *
 * Argument:
 *	it		- Integer map iterator to get key & value from.
 *
 * Results:
 *	keyPtr		- Integer entry key.
 *	valuePtr	- Entry value.
 *---------------------------------------------------------------------------*/

void
Col_IntMapIterGet(
    Col_MapIterator *it,
    intptr_t *keyPtr,
    Col_Word *valuePtr)
{
    if (Col_MapIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid map iterator");
	return;
    }

    switch (WORD_TYPE(it->entry)) {
	case WORD_TYPE_INTHASHENTRY:
	case WORD_TYPE_MINTHASHENTRY:
	case WORD_TYPE_INTTRIELEAF:
	    *keyPtr = WORD_INTMAPENTRY_KEY(it->entry);
	    *valuePtr = WORD_MAPENTRY_VALUE(it->entry);
	    break;

	default:
	    Col_Error(COL_ERROR, "Not an integer map iterator");
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_MapIterGetKey
 *
 *	Get key from map iterator.
 *
 * Argument:
 *	it	- Map iterator to get key & value from.
 *
 * Result:
 *	Key word.
 *---------------------------------------------------------------------------*/

Col_Word
Col_MapIterGetKey(
    Col_MapIterator *it)
{
    if (Col_MapIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid map iterator");
	return WORD_NIL;
    }

    switch (WORD_TYPE(it->entry)) {
	case WORD_TYPE_HASHENTRY:
	case WORD_TYPE_MHASHENTRY:
	case WORD_TYPE_TRIELEAF:
	    return WORD_MAPENTRY_KEY(it->entry);

	default:
	    Col_Error(COL_ERROR, "Not a generic or string map iterator");
	    return WORD_NIL;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_IntMapIterGetKey
 *
 *	Get integer key from integer map iterator.
 *
 * Argument:
 *	it	- Integer map iterator to get integer key from.
 *
 * Result:
 *	Integer key.
 *---------------------------------------------------------------------------*/

intptr_t
Col_IntMapIterGetKey(
    Col_MapIterator *it)
{
    if (Col_MapIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid map iterator");
	return 0;
    }

    switch (WORD_TYPE(it->entry)) {
	case WORD_TYPE_INTHASHENTRY:
	case WORD_TYPE_MINTHASHENTRY:
	case WORD_TYPE_INTTRIELEAF:
	    return WORD_INTMAPENTRY_KEY(it->entry);

	default:
	    Col_Error(COL_ERROR, "Not an integer map iterator");
	    return WORD_NIL;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_MapIterGetValue
 *
 *	Get value from map iterator.
 *
 * Argument:
 *	it	- Map iterator to get value from.
 *
 * Result:
 *	Value word.
 *---------------------------------------------------------------------------*/

Col_Word
Col_MapIterGetValue(
    Col_MapIterator *it)
{
    if (Col_MapIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid map iterator");
	return WORD_NIL;
    }

    return WORD_MAPENTRY_VALUE(it->entry);
}

/*---------------------------------------------------------------------------
 * Function: Col_MapIterSetValue
 *
 *	Set value of map iterator.
 *
 * Argument:
 *	it	- Map iterator to set value for.
 *	value	- Value to set.
 *---------------------------------------------------------------------------*/

void
Col_MapIterSetValue(
    Col_MapIterator *it,
    Col_Word value)
{
    if (Col_MapIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid map iterator");
	return;
    }

    switch (WORD_TYPE(it->map)) {
	case WORD_TYPE_STRHASHMAP:
	case WORD_TYPE_INTHASHMAP:
	    Col_HashMapIterSetValue(it, value);
	    break;

	case WORD_TYPE_STRTRIEMAP:
	case WORD_TYPE_INTTRIEMAP:
	    Col_TrieMapIterSetValue(it, value);
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_MapIterNext
 *
 *	Move the iterator to the next element.
 *
 * Argument:
 *	it	- The iterator to move.
 *---------------------------------------------------------------------------*/

void
Col_MapIterNext(
    Col_MapIterator *it)
{
    if (Col_MapIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid map iterator");
	return;
    }

    switch (WORD_TYPE(it->map)) {
	case WORD_TYPE_STRHASHMAP:
	case WORD_TYPE_INTHASHMAP:
	    Col_HashMapIterNext(it);
	    break;

	case WORD_TYPE_STRTRIEMAP:
	case WORD_TYPE_INTTRIEMAP:
	    Col_TrieMapIterNext(it);
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
    }
}
