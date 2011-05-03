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

    if (WORD_TYPE(it->entry) != WORD_TYPE_MAPENTRY) {
	Col_Error(COL_ERROR, "Not a generic or string map iterator");
	return;
    }

    *keyPtr = WORD_MAPENTRY_KEY(it->entry);
    *valuePtr = WORD_MAPENTRY_VALUE(it->entry);
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

    if (WORD_TYPE(it->entry) != WORD_TYPE_INTMAPENTRY) {
	Col_Error(COL_ERROR, "Not an integer map iterator");
	return;
    }

    *keyPtr = WORD_INTMAPENTRY_KEY(it->entry);
    *valuePtr = WORD_MAPENTRY_VALUE(it->entry);
}

/*---------------------------------------------------------------------------
 * Function: Col_MapIterKey
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
Col_MapIterKey(
    Col_MapIterator *it)
{
    if (Col_MapIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid map iterator");
	return WORD_NIL;
    }

    if (WORD_TYPE(it->entry) != WORD_TYPE_MAPENTRY) {
	Col_Error(COL_ERROR, "Not a generic or string map iterator");
	return WORD_NIL;
    }

    return WORD_MAPENTRY_KEY(it->entry);
}

/*---------------------------------------------------------------------------
 * Function: Col_IntMapIterKey
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
Col_IntMapIterKey(
    Col_MapIterator *it)
{
    if (Col_MapIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid map iterator");
	return 0;
    }

    if (WORD_TYPE(it->entry) != WORD_TYPE_INTMAPENTRY) {
	Col_Error(COL_ERROR, "Not an integer map iterator");
	return 0;
    }

    return WORD_INTMAPENTRY_KEY(it->entry);
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

    WORD_MAPENTRY_VALUE(it->entry) = value;
    Col_WordSetModified(it->entry);
}
