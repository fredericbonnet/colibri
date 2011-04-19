#include "colibri.h"
#include "colInternal.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>


/*
 * Integer key "randomization" by multiplication with a large prime. Given that 
 * multiplication by an odd number is reversible, this guarantees no collision.
 */

#define RANDOMIZE_KEY(i) (((unsigned long) (i))*1610612741)

/*
 * Constants controlling the behavior of hash tables.
 */

#define LOAD_FACTOR 1
#define GROW_FACTOR 4

/*
 * Prototypes for functions used only in this file.
 */

static int		GrowIntHashMap(Col_Word map);


/*
 *---------------------------------------------------------------------------
 *
 * Col_NewHashMap --
 *
 *	Create a new hash map word.
 *
 * Results:
 *	The new word.
 *
 * Side effects:
 *	Allocates memory cells.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_NewHashMap(
    Col_HashKeyProc *haskKeyProc,
				/* TODO */
    Col_CompareKeysProc *compareKeysProc)
				/* TODO */
{
    /* TODO */
    return WORD_NIL;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewIntHashMap --
 *
 *	Create a new integer hash map word.
 *
 * Results:
 *	The new word.
 *
 * Side effects:
 *	Allocates memory cells.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_NewIntHashMap()
{
    Col_Word map, *buckets;
    
    map = (Col_Word) AllocCells(WORD_INTHASHMAP_NBCELLS(NULL));
    WORD_INTHASHMAP_INIT(map);
    buckets = WORD_INTHASHMAP_STATICBUCKETS(map);
    memset(buckets, 0, sizeof(*buckets) 
	    * WORD_INTHASHMAP_STATICBUCKETS_SIZE(map));

    return map;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_IntHashMapGet --
 *
 *	Get value mapped to the given integer key if present.
 *
 * Results:
 *	Whether the key was found in the map.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

int
Col_IntHashMapGet(
    Col_Word map,		/* Integer hash map to get entry from. */ 
    int key, 			/* Entry key. */
    Col_Word *valuePtr)		/* Pointer to entry value, if found. */
{
    Col_Word *buckets, entry;
    size_t nbBuckets, index;

    if (WORD_TYPE(map) != WORD_TYPE_INTHASHMAP) {
	Col_Error(COL_ERROR, "%x is not an integer hash map", map);
	return 0;
    }

    /*
     * Get buckets.
     */

    if (WORD_INTHASHMAP_BUCKETS(map)) {
	ASSERT(WORD_TYPE(WORD_INTHASHMAP_BUCKETS(map)) == WORD_TYPE_MVECTOR);
	nbBuckets = WORD_VECTOR_LENGTH(WORD_INTHASHMAP_BUCKETS(map));
	buckets = WORD_VECTOR_ELEMENTS(WORD_INTHASHMAP_BUCKETS(map));
    } else {
	nbBuckets = WORD_INTHASHMAP_STATICBUCKETS_SIZE(map);
	buckets = WORD_INTHASHMAP_STATICBUCKETS(map);
    }

    /*
     * Get index from integer key.
     */

    index = RANDOMIZE_KEY(key) % nbBuckets;

    /*
     * Search for matching entry.
     */

    entry = buckets[index];
    while (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTHASHENTRY);
	if (WORD_INTHASHENTRY_KEY(entry) == key) {
	    /*
	     * Found!
	     */

	    *valuePtr = WORD_INTHASHENTRY_VALUE(entry);
	    return 1;
	}

	entry = WORD_INTHASHENTRY_NEXT(entry);
    }

    /*
     * Not found.
     */

    return 0;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_IntHashMapSet --
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
Col_IntHashMapSet(
    Col_Word map, 		/* Integer hash map to insert entry into. */
    int key, 			/* Entry key. */
    Col_Word value)		/* Entry value. */
{
    Col_Word *buckets, bucketParent, entry;
    size_t nbBuckets, index;

    if (WORD_TYPE(map) != WORD_TYPE_INTHASHMAP) {
	Col_Error(COL_ERROR, "%x is not an integer hash map", map);
	return 0;
    }

    /*
     * Get buckets.
     */

    if (WORD_INTHASHMAP_BUCKETS(map)) {
	bucketParent = WORD_INTHASHMAP_BUCKETS(map);
	ASSERT(WORD_TYPE(bucketParent) == WORD_TYPE_MVECTOR);
	nbBuckets = WORD_VECTOR_LENGTH(bucketParent);
	buckets = WORD_VECTOR_ELEMENTS(bucketParent);
    } else {
	nbBuckets = WORD_INTHASHMAP_STATICBUCKETS_SIZE(map);
	buckets = WORD_INTHASHMAP_STATICBUCKETS(map);
	bucketParent = map;
    }

    /*
     * Get index from integer key.
     */

    index = RANDOMIZE_KEY(key) % nbBuckets;

    /*
     * Search for matching entry.
     */

    entry = buckets[index];
    while (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTHASHENTRY);
	if (WORD_INTHASHENTRY_KEY(entry) == key) {
	    /*
	     * Found!
	     */

	    WORD_INTHASHENTRY_VALUE(entry) = value;
	    Col_SetModified((void *) entry);
	    return 0;
	}

	entry = WORD_INTHASHENTRY_NEXT(entry);
    }

    /*
     * Not found.
     */

    if (WORD_INTHASHMAP_SIZE(map) >= nbBuckets * LOAD_FACTOR
	    && GrowIntHashMap(map)) {
	/*
	 * Grow map and insert again.
	 */

	return Col_IntHashMapSet(map, key, value);
    }

    /*
     * Insert a new entry at head so that subsequent lookups are faster.
     */

    entry = (Col_Word) AllocCells(1);
    WORD_INTHASHENTRY_INIT(entry, buckets[index], key, value);
    buckets[index] = entry;
    Col_SetModified((void *) bucketParent);

    WORD_INTHASHMAP_SIZE(map)++;

    return 1;
}

/*
 *---------------------------------------------------------------------------
 *
 * GrowIntHashMap --
 *
 *	Resize an integer hash map bucket container. Size won't grow past a
 *	given limit. As the bucket container is a mutable vector, this limit 
 *	matches the maximum mutable vector length.
 *
 * Results:
 *	Whether the bucket container was resized or not. 
 *
 * Side effects:
 *	Bucket container may be resized.
 *
 *---------------------------------------------------------------------------
 */

static int
GrowIntHashMap(
    Col_Word map)		/* Integer hash map to grow. */
{
    Col_Word *buckets, *newBuckets, newBucketContainer, entry;
    size_t nbBuckets, newNbBuckets, index, newIndex;
    int key;

    /*
     * Get existing buckets.
     */

    if (WORD_INTHASHMAP_BUCKETS(map)) {
	Col_Word bucketContainer = WORD_INTHASHMAP_BUCKETS(map);
	ASSERT(WORD_TYPE(bucketContainer) == WORD_TYPE_MVECTOR);
	nbBuckets = WORD_VECTOR_LENGTH(bucketContainer);
	buckets = WORD_VECTOR_ELEMENTS(bucketContainer);
    } else {
	nbBuckets = WORD_INTHASHMAP_STATICBUCKETS_SIZE(map);
	buckets = WORD_INTHASHMAP_STATICBUCKETS(map);
    }

    /*
     * Create a new bucket container.
     */

    newNbBuckets = nbBuckets * GROW_FACTOR;
    if (newNbBuckets > MVECTOR_MAX_LENGTH(MVECTOR_MAX_SIZE)) {
	/*
	 * Already too large.
	 */

	return 0;
    }

    /*
     * Rehash all entries from old to new bucket array.
     */

    newBucketContainer = Col_NewMVector(newNbBuckets, newNbBuckets, NULL);
    ASSERT(WORD_TYPE(newBucketContainer) == WORD_TYPE_MVECTOR);
    newBuckets = WORD_VECTOR_ELEMENTS(newBucketContainer);

    for (index=0; index < nbBuckets; index++) {
	entry = buckets[index];
	while (entry) {
	    ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTHASHENTRY);

	    /*
	     * Get new index from integer key and move at head of new bucket.
	     */

	    buckets[index] = WORD_INTHASHENTRY_NEXT(entry);

	    key = WORD_INTHASHENTRY_KEY(entry);
	    newIndex = RANDOMIZE_KEY(key) % newNbBuckets;
	    WORD_INTHASHENTRY_NEXT(entry) = newBuckets[newIndex];
	    newBuckets[newIndex] = entry;
	    Col_SetModified((void *) entry);

	    entry = buckets[index];
	}
    }

    WORD_INTHASHMAP_BUCKETS(map) = newBucketContainer;
    Col_SetModified((void *) map);

    return 1;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_IntHashMapUnset --
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
Col_IntHashMapUnset(
    Col_Word map, 		/* Integer hash map to remove entry from. */
    int key) 			/* Entry key. */
{
    Col_Word *buckets, parent, *prevPtr, entry;
    size_t nbBuckets, index;

    if (WORD_TYPE(map) != WORD_TYPE_INTHASHMAP) {
	Col_Error(COL_ERROR, "%x is not an integer hash map", map);
	return 0;
    }

    /*
     * Get buckets.
     */

    if (WORD_INTHASHMAP_BUCKETS(map)) {
	ASSERT(WORD_TYPE(WORD_INTHASHMAP_BUCKETS(map)) == WORD_TYPE_MVECTOR);
	nbBuckets = WORD_VECTOR_LENGTH(WORD_INTHASHMAP_BUCKETS(map));
	buckets = WORD_VECTOR_ELEMENTS(WORD_INTHASHMAP_BUCKETS(map));
	parent = WORD_INTHASHMAP_BUCKETS(map);
    } else {
	nbBuckets = WORD_INTHASHMAP_STATICBUCKETS_SIZE(map);
	buckets = WORD_INTHASHMAP_STATICBUCKETS(map);
	parent = map;
    }

    /*
     * Get index from integer key.
     */

    index = RANDOMIZE_KEY(key) % nbBuckets;

    /*
     * Search for matching entry.
     */

    prevPtr = &buckets[index];
    while (*prevPtr) {
	entry = *prevPtr;
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTHASHENTRY);

	if (WORD_INTHASHENTRY_KEY(entry) == key) {
	    /*
	     * Found! Unlink & remove entry.
	     */

	    *prevPtr = WORD_INTHASHENTRY_NEXT(entry);
	    Col_SetModified((void *) parent);
	    WORD_INTHASHMAP_SIZE(map)--;
	    return 1;
	}

	parent = entry;
	prevPtr = &WORD_INTHASHENTRY_NEXT(entry);
    }

    /*
     * Not found.
     */

    return 0;
}
