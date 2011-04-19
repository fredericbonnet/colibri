#include "colibri.h"
#include "colInternal.h"


/*
 *---------------------------------------------------------------------------
 *
 * Col_MapSize --
 *
 *	Get the size of the map, i.e. the number of entries.
 *
 * Results:
 *	The size.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

size_t
Col_MapSize(
    Col_Word map)		/* Map to get size for. */
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_MapEntryGet --
 *
 *	Get key & value from map entry.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Overwrite pointed values.
 *
 *---------------------------------------------------------------------------
 */

void
Col_MapEntryGet(
    Col_Word entry,		/* Map entry to get key & value from. */
    Col_Word *keyPtr,		/* Pointer to entry key. */
    Col_Word *valuePtr)		/* Pointer to value key. */
{
    if (WORD_TYPE(entry) != WORD_TYPE_MAPENTRY) {
	Col_Error(COL_ERROR, "%x is not a map entry", entry);
	return;
    }

    *keyPtr = WORD_MAPENTRY_KEY(entry);
    *valuePtr = WORD_MAPENTRY_VALUE(entry);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_MapEntryGetKey --
 * Col_MapEntryGetValue --
 *
 *	Get key/value from map entry.
 *
 * Results:
 *	Key/value word.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_MapEntryGetKey(
    Col_Word entry)		/* Map entry to get key from. */
{
    if (WORD_TYPE(entry) != WORD_TYPE_MAPENTRY) {
	Col_Error(COL_ERROR, "%x is not a map entry", entry);
	return WORD_NIL;
    }

    return WORD_MAPENTRY_KEY(entry);
}

Col_Word
Col_MapEntryGetValue(
    Col_Word entry)		/* Map entry to get value from. */
{
    switch (WORD_TYPE(entry)) {
	case WORD_TYPE_MAPENTRY:
	case WORD_TYPE_INTMAPENTRY:
	    return WORD_MAPENTRY_VALUE(entry);

	default:
	    Col_Error(COL_ERROR, "%x is not a map entry", entry);
	    return WORD_NIL;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_MapEntrySetValue --
 *
 *	Set value of map entry.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Alter entry.
 *
 *---------------------------------------------------------------------------
 */

void
Col_MapEntrySetValue(
    Col_Word entry,		/* Map entry to set value for. */
    Col_Word value)		/* Value to set. */
{
    switch (WORD_TYPE(entry)) {
	case WORD_TYPE_MAPENTRY:
	case WORD_TYPE_INTMAPENTRY:
	    WORD_MAPENTRY_VALUE(entry) = value;
	    Col_WordSetModified(entry);
	    return;

	default:
	    Col_Error(COL_ERROR, "%x is not a map entry", entry);
	    return;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_StringMapGet --
 *
 *	Generic interface to string map get.
 *
 *	Get value mapped to the given string key if present.
 *
 * Results:
 *	TODO
 *
 * Side effects:
 *	TODO
 *
 *---------------------------------------------------------------------------
 */

int
Col_StringMapGet(
    Col_Word map,		/* String map to get entry from. */ 
    Col_Word key, 		/* Entry key. */
    Col_Word *valuePtr)		/* Pointer to entry value, if found. */
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_StringMapSet --
 *
 *	Generic interface to string map set.
 *
 *	Map the value to the key, replacing any existing.
 *
 * Results:
 *	Whether a new entry was created.
 *
 * Side effects:
 *	May allocate cells or replace value in the map.
 *
 *---------------------------------------------------------------------------
 */

int
Col_StringMapSet(
    Col_Word map, 		/* String map to insert entry into. */
    Col_Word key, 		/* Entry key. */
    Col_Word value)		/* Entry value. */
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_StringMapUnset --
 *
 *	Generic interface to string map unset.
 *
 *	Remove any value mapped to the given key.
 *
 * Results:
 *	Whether an existing entry was removed.
 *
 * Side effects:
 *	May remove value in the map.
 *
 *---------------------------------------------------------------------------
 */

int
Col_StringMapUnset(
    Col_Word map, 		/* String map to remove entry from. */
    Col_Word key) 		/* Entry key. */
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_IntMapGet --
 *
 *	Generic interface to integer map get.
 *
 *	Get value mapped to the given integer key if present.
 *
 * Results:
 *	TODO
 *
 * Side effects:
 *	TODO
 *
 *---------------------------------------------------------------------------
 */

int
Col_IntMapGet(
    Col_Word map,		/* Integer map to get entry from. */ 
    int key, 			/* Entry key. */
    Col_Word *valuePtr)		/* Pointer to entry value, if found. */
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_IntMapSet --
 *
 *	Generic interface to integer map set.
 *
 *	Map the value to the key, replacing any existing.
 *
 * Results:
 *	Whether a new entry was created.
 *
 * Side effects:
 *	May allocate cells or replace value in the map.
 *
 *---------------------------------------------------------------------------
 */

int
Col_IntMapSet(
    Col_Word map, 		/* Integer map to insert entry into. */
    int key, 			/* Entry key. */
    Col_Word value)		/* Entry value. */
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_IntMapUnset --
 *
 *	Generic interface to integer map unset.
 *
 *	Remove any value mapped to the given key.
 *
 * Results:
 *	Whether an existing entry was removed.
 *
 * Side effects:
 *	May remove value in the map.
 *
 *---------------------------------------------------------------------------
 */

int
Col_IntMapUnset(
    Col_Word map, 		/* Integer map to remove entry from. */
    int key) 			/* Entry key. */
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_IntMapEntryGet --
 *
 *	Get key & value from integer map entry.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Overwrite pointed values.
 *
 *---------------------------------------------------------------------------
 */

void
Col_IntMapEntryGet(
    Col_Word entry,		/* Map entry to get key & value from. */
    int *keyPtr,		/* Pointer to entry key. */
    Col_Word *valuePtr)		/* Pointer to value key. */
{
    if (WORD_TYPE(entry) != WORD_TYPE_INTMAPENTRY) {
	Col_Error(COL_ERROR, "%x is not an integer map entry", entry);
	return;
    }

    *keyPtr = WORD_INTMAPENTRY_KEY(entry);
    *valuePtr = WORD_MAPENTRY_VALUE(entry);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_IntMapEntryGetKey --
 *
 *	Get key from integer map entry.
 *
 * Results:
 *	Key/value word.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

int
Col_IntMapEntryGetKey(
    Col_Word entry)		/* Map entry to get key from. */
{
    if (WORD_TYPE(entry) != WORD_TYPE_INTMAPENTRY) {
	Col_Error(COL_ERROR, "%x is not an integer map entry", entry);
	return WORD_NIL;
    }

    return WORD_INTMAPENTRY_KEY(entry);
}
