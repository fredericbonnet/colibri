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
	case WORD_TYPE_INTHASHMAP:
	    return WORD_INTHASHMAP_SIZE(map);

	case WORD_TYPE_INTTRIEMAP:
	    return WORD_TRIEMAP_SIZE(map);

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
 $
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
    Col_Word map, 		/* Integer hash map to remove entry from. */
    int key) 			/* Entry key. */
{
    switch (WORD_TYPE(map)) {
	case WORD_TYPE_INTHASHMAP:
	    return Col_IntHashMapUnset(map, key);

	case WORD_TYPE_INTTRIEMAP:
	    return Col_IntTrieMapUnset(map, key);

	default:
	    Col_Error(COL_ERROR, "%x is not an integer map", map);
	    return 0;
    }
}
