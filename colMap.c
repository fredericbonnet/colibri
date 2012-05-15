/*
 * File: colMap.c
 *
 *	This file implements the generic map handling features of Colibri.
 *
 *	Maps are a collection datatype that associates keys to values. Keys can
 *	be integers, strings or generic words.
 *
 *	They are always mutable.
 *
 * See also:
 *	<colMap.h>
 */

#include "include/colibri.h"
#include "colInternal.h"

#include "colWordInt.h"
#include "colMapInt.h"
#include "colHashInt.h"
#include "colTrieInt.h"


/*
================================================================================
Section: Maps
================================================================================
*/

/****************************************************************************
 * Group: Map Accessors
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_MapSize
 *
 *	Get the size of the map. I.e. the number of entries.
 *
 * Argument:
 *	map	- Map to get size for.
 *
 * Type checking:
 *	*map* must be a valid map.
 *
 * Result:
 *	The map size.
 *---------------------------------------------------------------------------*/

size_t
Col_MapSize(
    Col_Word map)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAP(map) return 0;

    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRHASHMAP:
	case WORD_TYPE_INTHASHMAP:
	    return WORD_HASHMAP_SIZE(map);

	case WORD_TYPE_STRTRIEMAP:
	case WORD_TYPE_INTTRIEMAP:
	    return WORD_TRIEMAP_SIZE(map);

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
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
 * Type checking:
 *	*map* must be a valid string or custom map.
 *
 * Results:
 *	Whether the key was found in the map. In this case the value is returned
 *	through valuePtr.
 *---------------------------------------------------------------------------*/

int
Col_MapGet(
    Col_Word map,
    Col_Word key,
    Col_Word *valuePtr)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(map) return 0;

    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRHASHMAP:
	    return Col_HashMapGet(map, key, valuePtr);

	case WORD_TYPE_STRTRIEMAP:
	    return Col_TrieMapGet(map, key, valuePtr);

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
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
 * Type checking:
 *	*map* must be a valid integer map.
 *
 * Results:
 *	Whether the key was found in the map. In this case the value is returned
 *	through valuePtr.
 *---------------------------------------------------------------------------*/

int
Col_IntMapGet(
    Col_Word map,
    intptr_t key,
    Col_Word *valuePtr)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(map) return 0;

    switch (WORD_TYPE(map)) {
	case WORD_TYPE_INTHASHMAP:
	    return Col_IntHashMapGet(map, key, valuePtr);

	case WORD_TYPE_INTTRIEMAP:
	    return Col_IntTrieMapGet(map, key, valuePtr);

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
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
 * Type checking:
 *	*map* must be a valid string or custom map.
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
    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(map) return 0;

    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRHASHMAP:
	    return Col_HashMapSet(map, key, value);

	case WORD_TYPE_STRTRIEMAP:
	    return Col_TrieMapSet(map, key, value);

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
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
 * Type checking:
 *	*map* must be a valid integer map.
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
    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(map) return 0;

    switch (WORD_TYPE(map)) {
	case WORD_TYPE_INTHASHMAP:
	    return Col_IntHashMapSet(map, key, value);

	case WORD_TYPE_INTTRIEMAP:
	    return Col_IntTrieMapSet(map, key, value);

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
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
 * Type checking:
 *	*map* must be a valid string or custom map.
 *
 * Result:
 *	Whether an existing entry was removed.
 *---------------------------------------------------------------------------*/

int
Col_MapUnset(
    Col_Word map,
    Col_Word key)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(map) return 0;

    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRHASHMAP:
	    return Col_HashMapUnset(map, key);

	case WORD_TYPE_STRTRIEMAP:
	    return Col_TrieMapUnset(map, key);

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
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
 * Type checking:
 *	*map* must be a valid integer map.
 *
 * Result:
 *	Whether an existing entry was removed.
 *---------------------------------------------------------------------------*/

int
Col_IntMapUnset(
    Col_Word map,
    intptr_t key)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(map) return 0;

    switch (WORD_TYPE(map)) {
	case WORD_TYPE_INTHASHMAP:
	    return Col_IntHashMapUnset(map, key);

	case WORD_TYPE_INTTRIEMAP:
	    return Col_IntTrieMapUnset(map, key);

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
	    return 0;
    }
}


/****************************************************************************
 * Group: Map Iteration
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
 *
 * Type checking:
 *	*map* must be a valid map.
 *---------------------------------------------------------------------------*/

void
Col_MapIterBegin(
    Col_Word map,
    Col_MapIterator *it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAP(map) {
	*it = COL_MAPITER_NULL;
	return;
    }

    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRHASHMAP:
	case WORD_TYPE_INTHASHMAP:
	    Col_HashMapIterBegin(map, it);
	    break;

	case WORD_TYPE_STRTRIEMAP:
	case WORD_TYPE_INTTRIEMAP:
	    Col_TrieMapIterFirst(map, it);
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
	    *it = COL_MAPITER_NULL;
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
 * Type checking:
 *	*map* must be a valid string or custom map.
 *
 * Result:
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *---------------------------------------------------------------------------*/

void
Col_MapIterFind(
    Col_Word map, 
    Col_Word key, 
    int *createPtr, 
    Col_MapIterator *it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(map) {
	*it = COL_MAPITER_NULL;
	return;
    }

    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRHASHMAP:
	    Col_HashMapIterFind(map, key, createPtr, it);
	    break;

	case WORD_TYPE_STRTRIEMAP:
	    Col_TrieMapIterFind(map, key, createPtr, it);
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
	    *it = COL_MAPITER_NULL;
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
 * Type checking:
 *	*map* must be a valid integer map.
 *
 * Result:
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *---------------------------------------------------------------------------*/

void
Col_IntMapIterFind(
    Col_Word map, 
    intptr_t key, 
    int *createPtr, 
    Col_MapIterator *it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(map) {
	*it = COL_MAPITER_NULL;
	return;
    }

    switch (WORD_TYPE(map)) {
	case WORD_TYPE_INTHASHMAP:
	    Col_IntHashMapIterFind(map, key, createPtr, it);
	    break;

	case WORD_TYPE_INTTRIEMAP:
	    Col_IntTrieMapIterFind(map, key, createPtr, it);
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
	    *it = COL_MAPITER_NULL;
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
 * Type checking:
 *	*it* must be a valid string or custom map iterator.
 *
 * Range checking:
 *	*it* must not be at end.
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
    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(it->map) return;
    RANGECHECK_MAPITER(it) return;

    ASSERT(it->entry);

    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_HASHENTRY 
	    || WORD_TYPE(it->entry) == WORD_TYPE_MHASHENTRY
	    || WORD_TYPE(it->entry) == WORD_TYPE_TRIELEAF
	    || WORD_TYPE(it->entry) == WORD_TYPE_MTRIELEAF);

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
 * Type checking:
 *	*it* must be a valid integer map iterator.
 *
 * Range checking:
 *	*it* must not be at end.
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
    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(it->map) return;
    RANGECHECK_MAPITER(it) return;

    ASSERT(it->entry);

    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_INTHASHENTRY 
	    || WORD_TYPE(it->entry) == WORD_TYPE_MINTHASHENTRY
	    || WORD_TYPE(it->entry) == WORD_TYPE_INTTRIELEAF
	    || WORD_TYPE(it->entry) == WORD_TYPE_MINTTRIELEAF);

    *keyPtr = WORD_INTMAPENTRY_KEY(it->entry);
    *valuePtr = WORD_MAPENTRY_VALUE(it->entry);
}

/*---------------------------------------------------------------------------
 * Function: Col_MapIterGetKey
 *
 *	Get key from map iterator.
 *
 * Argument:
 *	it	- Map iterator to get key & value from.
 *
 * Type checking:
 *	*it* must be a valid string or custom map iterator.
 *
 * Range checking:
 *	*it* must not be at end.
 *
 * Result:
 *	Key word.
 *---------------------------------------------------------------------------*/

Col_Word
Col_MapIterGetKey(
    Col_MapIterator *it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(it->map) return WORD_NIL;
    RANGECHECK_MAPITER(it) return WORD_NIL;

    ASSERT(it->entry);

    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_HASHENTRY 
	    || WORD_TYPE(it->entry) == WORD_TYPE_MHASHENTRY
	    || WORD_TYPE(it->entry) == WORD_TYPE_TRIELEAF
	    || WORD_TYPE(it->entry) == WORD_TYPE_MTRIELEAF);

    return WORD_MAPENTRY_KEY(it->entry);
}

/*---------------------------------------------------------------------------
 * Function: Col_IntMapIterGetKey
 *
 *	Get integer key from integer map iterator.
 *
 * Argument:
 *	it	- Integer map iterator to get integer key from.
 *
 * Type checking:
 *	*it* must be a valid integer map iterator.
 *
 * Range checking:
 *	*it* must not be at end.
 *
 * Result:
 *	Integer key.
 *---------------------------------------------------------------------------*/

intptr_t
Col_IntMapIterGetKey(
    Col_MapIterator *it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(it->map) return 0;
    RANGECHECK_MAPITER(it) return 0;

    ASSERT(it->entry);

    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_INTHASHENTRY 
	    || WORD_TYPE(it->entry) == WORD_TYPE_MINTHASHENTRY
	    || WORD_TYPE(it->entry) == WORD_TYPE_INTTRIELEAF
	    || WORD_TYPE(it->entry) == WORD_TYPE_MINTTRIELEAF);

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
 * Type checking:
 *	*it* must be a valid map iterator.
 *
 * Range checking:
 *	*it* must not be at end.
 *
 * Result:
 *	Value word.
 *---------------------------------------------------------------------------*/

Col_Word
Col_MapIterGetValue(
    Col_MapIterator *it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAPITER(it) return WORD_NIL;
    RANGECHECK_MAPITER(it) return WORD_NIL;

    ASSERT(it->entry);

    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_HASHENTRY 
	    || WORD_TYPE(it->entry) == WORD_TYPE_MHASHENTRY
	    || WORD_TYPE(it->entry) == WORD_TYPE_INTHASHENTRY 
	    || WORD_TYPE(it->entry) == WORD_TYPE_MINTHASHENTRY
	    || WORD_TYPE(it->entry) == WORD_TYPE_TRIELEAF
	    || WORD_TYPE(it->entry) == WORD_TYPE_MTRIELEAF
	    || WORD_TYPE(it->entry) == WORD_TYPE_INTTRIELEAF
	    || WORD_TYPE(it->entry) == WORD_TYPE_MINTTRIELEAF);

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
 *
 * Type checking:
 *	*it* must be a valid map iterator.
 *
 * Range checking:
 *	*it* must not be at end.
 *---------------------------------------------------------------------------*/

void
Col_MapIterSetValue(
    Col_MapIterator *it,
    Col_Word value)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAPITER(it) return;
    RANGECHECK_MAPITER(it) return;

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
 *
 * Type checking:
 *	*it* must be a valid map iterator.
 *
 * Range checking:
 *	*it* must not be at end.
 *---------------------------------------------------------------------------*/

void
Col_MapIterNext(
    Col_MapIterator *it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAPITER(it) return;
    RANGECHECK_MAPITER(it) return;

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
