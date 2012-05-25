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

    case WORD_TYPE_CUSTOM: 
	switch (WORD_TYPEINFO(map)->type) {
	case COL_HASHMAP: return WORD_HASHMAP_SIZE                                      (map);
	case COL_TRIEMAP: return WORD_TRIEMAP_SIZE                                      (map);
	case COL_MAP:     return ((Col_CustomMapType *)    WORD_TYPEINFO(map))->sizeProc(map);
	case COL_INTMAP:  return ((Col_CustomIntMapType *) WORD_TYPEINFO(map))->sizeProc(map);
	}
	/* continued. */

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
    Col_MapGetProc *proc;

    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(map) return 0;

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
	proc = Col_HashMapGet;
	break;

    case WORD_TYPE_STRTRIEMAP:
	proc = Col_TrieMapGet;
	break;

    case WORD_TYPE_CUSTOM: {
	Col_CustomMapType *type = (Col_CustomMapType *) WORD_TYPEINFO(map);
	switch (type->type.type) {
	case COL_HASHMAP: proc = Col_HashMapGet; break;
	case COL_TRIEMAP: proc = Col_TrieMapGet; break;
	case COL_MAP:     proc = type->getProc; break;
	}
	break;
	}

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
	return 0;
    }

    return proc(map, key, valuePtr);
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
    Col_IntMapGetProc *proc;

    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(map) return 0;

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_INTHASHMAP:
	proc = Col_IntHashMapGet;
	break;

    case WORD_TYPE_INTTRIEMAP:
	proc = Col_IntTrieMapGet;
	break;

    case WORD_TYPE_CUSTOM: 
	ASSERT(WORD_TYPEINFO(map)->type == COL_INTMAP);
	proc = ((Col_CustomIntMapType *) WORD_TYPEINFO(map))->getProc;
	break;

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
	return 0;
    }

    return proc(map, key, valuePtr);
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
    Col_MapSetProc *proc;

    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(map) return 0;

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
	proc = Col_HashMapSet;
	break;

    case WORD_TYPE_STRTRIEMAP:
	proc = Col_TrieMapSet;
	break;

    case WORD_TYPE_CUSTOM: {
	Col_CustomMapType *type = (Col_CustomMapType *) WORD_TYPEINFO(map);
	switch (type->type.type) {
	case COL_HASHMAP: proc = Col_HashMapSet; break;
	case COL_TRIEMAP: proc = Col_TrieMapSet; break;
	case COL_MAP:     proc = type->setProc; break;
	}
	break;
	}

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
	return 0;
    }

    return proc(map, key, value);
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
    Col_IntMapSetProc *proc;

    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(map) return 0;

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_INTHASHMAP:
	proc = Col_IntHashMapSet;
	break;

    case WORD_TYPE_INTTRIEMAP:
	proc = Col_IntTrieMapSet;
	break;

    case WORD_TYPE_CUSTOM: 
	ASSERT(WORD_TYPEINFO(map)->type == COL_INTMAP);
	proc = ((Col_CustomIntMapType *) WORD_TYPEINFO(map))->setProc;
	break;

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
	return 0;
    }

    return proc(map, key, value);
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
    Col_MapUnsetProc *proc;

    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(map) return 0;

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
	proc = Col_HashMapUnset;
	break;

    case WORD_TYPE_STRTRIEMAP:
	proc = Col_TrieMapUnset;
	break;

    case WORD_TYPE_CUSTOM: {
	Col_CustomMapType *type = (Col_CustomMapType *) WORD_TYPEINFO(map);
	switch (type->type.type) {
	case COL_HASHMAP: proc = Col_HashMapUnset; break;
	case COL_TRIEMAP: proc = Col_TrieMapUnset; break;
	case COL_MAP:     proc = type->unsetProc; break;
	}
	break;
	}

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
	return 0;
    }

    return proc(map, key);
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
    Col_IntMapUnsetProc *proc;

    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(map) return 0;

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_INTHASHMAP:
	proc = Col_IntHashMapUnset;
	break;

    case WORD_TYPE_INTTRIEMAP:
	proc = Col_IntTrieMapUnset;
	break;

    case WORD_TYPE_CUSTOM: 
	ASSERT(WORD_TYPEINFO(map)->type == COL_INTMAP);
	proc = ((Col_CustomIntMapType *) WORD_TYPEINFO(map))->unsetProc;
	break;

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
	return 0;
    }

    return proc(map, key);
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
 *	it	- Iterator to initialize.
 *	map	- Map to iterate over.
 *
 * Type checking:
 *	*map* must be a valid map.
 *---------------------------------------------------------------------------*/

void
Col_MapIterBegin(
    Col_MapIterator it,
    Col_Word map)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAP(map) {
	Col_MapIterSetNull(it);
	return;
    }

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
    case WORD_TYPE_INTHASHMAP:
	Col_HashMapIterBegin(it, map);
	break;

    case WORD_TYPE_STRTRIEMAP:
    case WORD_TYPE_INTTRIEMAP:
	Col_TrieMapIterFirst(it, map);
	break;

    case WORD_TYPE_CUSTOM: 
	switch (WORD_TYPEINFO(map)->type) {
	case COL_HASHMAP: Col_HashMapIterBegin(it, map); return;
	case COL_TRIEMAP: Col_TrieMapIterFirst(it, map); return;

	/*
	 * Note: for custom maps we set the iterator entry field to a non-nil 
	 * value upon success (i.e. when iterBeginProc returns non-zero) so that
	 * Col_MapIterEnd() is false.
	 */

	case COL_MAP:    it->entry = ((Col_CustomMapType *)    WORD_TYPEINFO(map))->iterBeginProc(map, &it->traversal.custom) ? 1 : WORD_NIL; return;
	case COL_INTMAP: it->entry = ((Col_CustomIntMapType *) WORD_TYPEINFO(map))->iterBeginProc(map, &it->traversal.custom) ? 1 : WORD_NIL; return;
	}
	/* continued. */

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
	Col_MapIterSetNull(it);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_MapIterFind
 *
 *	Initialize the map iterator so that it points to the entry with the
 *	given key within the map.
 *
 * Arguments:
 *	it		- Iterator to initialize.
 *	map		- Map to find or create entry into.
 *	key		- Entry key. Can be any word type, including string,
 *			  however it must match the actual type used by the map.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *
 * Type checking:
 *	*map* must be a valid string or custom map.
 *
 * Result:
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *---------------------------------------------------------------------------*/

void
Col_MapIterFind(
    Col_MapIterator it,
    Col_Word map, 
    Col_Word key, 
    int *createPtr)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(map) {
	Col_MapIterSetNull(it);
	return;
    }

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
	Col_HashMapIterFind(it, map, key, createPtr);
	break;

    case WORD_TYPE_STRTRIEMAP:
	Col_TrieMapIterFind(it, map, key, createPtr);
	break;

    case WORD_TYPE_CUSTOM: 
	switch (WORD_TYPEINFO(map)->type) {
	case COL_HASHMAP: Col_HashMapIterFind(it, map, key, createPtr); return;
	case COL_TRIEMAP: Col_TrieMapIterFind(it, map, key, createPtr); return;

	/*
	 * Note: for custom maps we set the iterator entry field to a non-nil 
	 * value upon success (i.e. when iterFindProc returns non-zero) so that
	 * Col_MapIterEnd() is false.
	 */

	case COL_MAP: it->entry = ((Col_CustomMapType *) WORD_TYPEINFO(map))->iterFindProc(map, key, createPtr, &it->traversal.custom) ? 1 : WORD_NIL; return;
	}
	/* continued. */

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
	Col_MapIterSetNull(it);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_IntMapIterFind
 *
 *	Initialize the map iterator so that it points to the entry with the 
 *	given integer key within the map.
 *
 * Arguments:
 *	it		- Iterator to initialize.
 *	map		- Integer map to find or create entry into.
 *	key		- Integer entry key.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *
 * Type checking:
 *	*map* must be a valid integer map.
 *
 * Result:
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *---------------------------------------------------------------------------*/

void
Col_IntMapIterFind(
    Col_MapIterator it,
    Col_Word map, 
    intptr_t key, 
    int *createPtr)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(map) {
	Col_MapIterSetNull(it);
	return;
    }

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_INTHASHMAP:
	Col_IntHashMapIterFind(it, map, key, createPtr);
	break;

    case WORD_TYPE_INTTRIEMAP:
	Col_IntTrieMapIterFind(it, map, key, createPtr);
	break;

    case WORD_TYPE_CUSTOM: 
	ASSERT(WORD_TYPEINFO(map)->type == COL_INTMAP);
	/*
	 * Note: for custom maps we set the iterator entry field to a non-nil 
	 * value upon success (i.e. when iterFindProc returns non-zero) so that
	 * Col_MapIterEnd() is false.
	 */

	it->entry = ((Col_CustomIntMapType *) WORD_TYPEINFO(map))->iterFindProc(map, key, createPtr, &it->traversal.custom) ? 1 : WORD_NIL; return;
	break;

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
	Col_MapIterSetNull(it);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_MapIterGet
 *
 *	Get key & value from map iterator.
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
 * Results:
 *	keyPtr		- Entry key.
 *	valuePtr	- Entry value.
 *---------------------------------------------------------------------------*/

void
Col_MapIterGet(
    Col_MapIterator it,
    Col_Word *keyPtr,
    Col_Word *valuePtr)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(it->map) return;
    RANGECHECK_MAPITER(it) return;

    ASSERT(it->entry);

    if (WORD_TYPE(it->map) == WORD_TYPE_CUSTOM) {
	switch (WORD_TYPEINFO(it->map)->type) {
	case COL_MAP: {
	    Col_CustomMapType *type 
		    = (Col_CustomMapType *) WORD_TYPEINFO(it->map);
	    *keyPtr   = type->iterGetKeyProc  (it->map, &it->traversal.custom);
	    *valuePtr = type->iterGetValueProc(it->map, &it->traversal.custom);
	    return;
	    }
	}

	/*
	 * Other custom maps are hash or trie maps and use regular entries.
	 */
    }

    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_HASHENTRY 
	    || WORD_TYPE(it->entry) == WORD_TYPE_MHASHENTRY
	    || WORD_TYPE(it->entry) == WORD_TYPE_TRIELEAF
	    || WORD_TYPE(it->entry) == WORD_TYPE_MTRIELEAF);

    *keyPtr   = WORD_MAPENTRY_KEY  (it->entry);
    *valuePtr = WORD_MAPENTRY_VALUE(it->entry);
}

/*---------------------------------------------------------------------------
 * Function: Col_IntMapIterGet
 *
 *	Get key & value from integer map iterator.
 *
 * Argument:
 *	it	- Integer map iterator to get key & value from.
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
    Col_MapIterator it,
    intptr_t *keyPtr,
    Col_Word *valuePtr)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(it->map) return;
    RANGECHECK_MAPITER(it) return;

    ASSERT(it->entry);

    if (WORD_TYPE(it->map) == WORD_TYPE_CUSTOM) {
	Col_CustomIntMapType *type 
		= (Col_CustomIntMapType *) WORD_TYPEINFO(it->map);
	ASSERT(WORD_TYPEINFO(it->map)->type == COL_INTMAP);
	*keyPtr   = type->iterGetKeyProc  (it->map, &it->traversal.custom);
	*valuePtr = type->iterGetValueProc(it->map, &it->traversal.custom);
	return;
    }

    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_INTHASHENTRY 
	    || WORD_TYPE(it->entry) == WORD_TYPE_MINTHASHENTRY
	    || WORD_TYPE(it->entry) == WORD_TYPE_INTTRIELEAF
	    || WORD_TYPE(it->entry) == WORD_TYPE_MINTTRIELEAF);

    *keyPtr   = WORD_INTMAPENTRY_KEY(it->entry);
    *valuePtr = WORD_MAPENTRY_VALUE (it->entry);
}

/*---------------------------------------------------------------------------
 * Function: Col_MapIterGetKey
 *
 *	Get key from map iterator.
 *
 * Argument:
 *	it	- Map iterator to get key from.
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
    Col_MapIterator it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(it->map) return WORD_NIL;
    RANGECHECK_MAPITER(it) return WORD_NIL;

    ASSERT(it->entry);

    if (WORD_TYPE(it->map) == WORD_TYPE_CUSTOM) {
	switch (WORD_TYPEINFO(it->map)->type) {
	case COL_MAP: return ((Col_CustomMapType *) WORD_TYPEINFO(it->map))->iterGetKeyProc(it->map, &it->traversal.custom);
	}

	/*
	 * Other custom maps are hash or trie maps and use regular entries.
	 */
    }

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
    Col_MapIterator it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(it->map) return 0;
    RANGECHECK_MAPITER(it) return 0;

    ASSERT(it->entry);

    if (WORD_TYPE(it->map) == WORD_TYPE_CUSTOM) {
	ASSERT(WORD_TYPEINFO(it->map)->type == COL_INTMAP);
	return ((Col_CustomIntMapType *) WORD_TYPEINFO(it->map))->iterGetKeyProc(it->map, &it->traversal.custom);
    }

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
    Col_MapIterator it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAPITER(it) return WORD_NIL;
    RANGECHECK_MAPITER(it) return WORD_NIL;

    ASSERT(it->entry);

    if (WORD_TYPE(it->map) == WORD_TYPE_CUSTOM) {
	switch (WORD_TYPEINFO(it->map)->type) {
	case COL_MAP:    return ((Col_CustomMapType *)    WORD_TYPEINFO(it->map))->iterGetValueProc(it->map, &it->traversal.custom);
	case COL_INTMAP: return ((Col_CustomIntMapType *) WORD_TYPEINFO(it->map))->iterGetValueProc(it->map, &it->traversal.custom);
	}

	/*
	 * Other custom maps are hash or trie maps and use regular entries.
	 */
    }

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
    Col_MapIterator it,
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

    case WORD_TYPE_CUSTOM: 
	switch (WORD_TYPEINFO(it->map)->type) {
	case COL_HASHMAP: Col_HashMapIterSetValue(it, value); return;
	case COL_TRIEMAP: Col_TrieMapIterSetValue(it, value); return;
	case COL_MAP:     ((Col_CustomMapType *)    WORD_TYPEINFO(it->map))->iterSetValueProc(it->map, value, &it->traversal.custom); return;
	case COL_INTMAP:  ((Col_CustomIntMapType *) WORD_TYPEINFO(it->map))->iterSetValueProc(it->map, value, &it->traversal.custom); return;
	}
	/* continued. */

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
    Col_MapIterator it)
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

    case WORD_TYPE_CUSTOM: 
	switch (WORD_TYPEINFO(it->map)->type) {
	case COL_HASHMAP: Col_HashMapIterNext(it); return;
	case COL_TRIEMAP: Col_TrieMapIterNext(it); return;

	/*
	 * Note: for custom maps we set the iterator entry field to a non-nil 
	 * value upon success (i.e. when iterNextProc returns non-zero) so that
	 * Col_MapIterEnd() is false.
	 */

	case COL_MAP:    it->entry = ((Col_CustomMapType *)    WORD_TYPEINFO(it->map))->iterNextProc(it->map, &it->traversal.custom) ? 1 : WORD_NIL; return;
	case COL_INTMAP: it->entry = ((Col_CustomIntMapType *) WORD_TYPEINFO(it->map))->iterNextProc(it->map, &it->traversal.custom) ? 1 : WORD_NIL; return;
	}
	/* continued. */

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
    }
}
