#include "colibri.h"
#include "colInternal.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>


/*
 * Integer key "randomization" by multiplication with a large prime. Given that 
 * multiplication by an odd number is reversible, this guarantees no collision.
 */

#define RANDOMIZE_KEY(i) (((unsigned int) (i))*1610612741)

/*
 * Constants controlling the behavior of hash tables.
 */

#define LOAD_FACTOR 1
#define GROW_FACTOR 4

/*
 * Bucket access.
 */

#define GET_BUCKETS(map, nbBuckets, buckets, bucketParent) \
    if (WORD_HASHMAP_BUCKETS(map)) { \
	ASSERT(WORD_TYPE(WORD_HASHMAP_BUCKETS(map)) == WORD_TYPE_MVECTOR); \
	(nbBuckets) = WORD_VECTOR_LENGTH(WORD_HASHMAP_BUCKETS(map)); \
	(buckets) = WORD_VECTOR_ELEMENTS(WORD_HASHMAP_BUCKETS(map)); \
	(bucketParent) = WORD_HASHMAP_BUCKETS(map); \
    } else { \
	(nbBuckets) = HASHMAP_STATICBUCKETS_SIZE; \
	(buckets) = WORD_HASHMAP_STATICBUCKETS(map); \
	(bucketParent) = map; \
    }

/*
 * Prototypes for functions used only in this file.
 */

static Col_RopeChunksTraverseProc HashChunkProc;
static Col_HashProc HashString;
static void		AllocBuckets(Col_Word map, size_t capacity);
static int		GrowStringHashMap(Col_Word map);
static int		GrowIntHashMap(Col_Word map);


/*
 *---------------------------------------------------------------------------
 *
 * HashChunkProc --
 *
 *	Rope traversal proc that computes its hash value.
 *
 *	Uses the same algorithm as Tcl's string hash tables (HashStringKey).
 *
 * Results:
 *	Always zero.
 *
 * Side effects:
 *	Modifies unsigned int value pointed to by clientData.
 *
 *---------------------------------------------------------------------------
 */

#define STRING_HASH(hash, c) \
    (hash) += ((hash)<<3)+(c)

static int 
HashChunkProc(
    size_t index, 
    size_t length, 
    size_t number, 
    const Col_RopeChunk *chunks, 
    Col_ClientData clientData)
{
    size_t i;
    unsigned int hash = *(unsigned int *) clientData;
    ASSERT(number == 1);
    switch (chunks->format) {
	case COL_UCS1: {
	    const Col_Char1 *p = (const Col_Char1 *) chunks->data;
	    for (i = 0; i < length; i++) {
		STRING_HASH(hash, p[i]);
	    }
	    break;
	}

	case COL_UCS2: {
	    const Col_Char2 *p = (const Col_Char2 *) chunks->data;
	    for (i = 0; i < length; i++) {
		STRING_HASH(hash, p[i]);
	    }
	    break;
	}

	case COL_UCS4: {
	    const Col_Char4 *p = (const Col_Char4 *) chunks->data;
	    for (i = 0; i < length; i++) {
		STRING_HASH(hash, p[i]);
	    }
	    break;
	}

	case COL_UTF8: {
	    const char *p = (const char *) chunks->data;
	    for (i = 0; i < length; i++) {
		STRING_HASH(hash, Col_Utf8CharAt(p));
		COL_UTF8_NEXT(p);
	    }
	    break;
	}
    }
    *(unsigned int *) clientData = hash;
    return 0;
}

/*
 *---------------------------------------------------------------------------
 *
 * HashString --
 *
 *	Compute a string key hash value.
 *
 * Results:
 *	The given rope's hash value.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

static unsigned int 
HashString(
    Col_Word key)
{
    unsigned int hash = 0;
    Col_TraverseRopeChunks(1, &key, 0, SIZE_MAX, HashChunkProc, &hash, NULL);
    return hash;
}

/*
 *---------------------------------------------------------------------------
 *
 * AllocBuckets --
 *
 *	Allocate bucket container having the given minimum capacity, rounded
 *	up to a power of 2.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	May allocate memory cells, modifies given cells.
 *
 *---------------------------------------------------------------------------
 */

static void
AllocBuckets(
    Col_Word map,		/* Map to allocate buckets for. */
    size_t capacity)		/* Initial bucket size. */
{
    if (capacity <= HASHMAP_STATICBUCKETS_SIZE) {
	/*
	 * Use static buckets.
	 */

	Col_Word *buckets = WORD_HASHMAP_STATICBUCKETS(map);
	memset(buckets, 0, sizeof(*buckets) * HASHMAP_STATICBUCKETS_SIZE);
    } else {
	/*
	 * Store buckets in mutable vector (round capacity to the next power of 
	 * 2).
	 * See: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2 
	 */

	capacity--;
	capacity |= capacity >> 1;
	capacity |= capacity >> 2;
	capacity |= capacity >> 4;
	capacity |= capacity >> 8;
	capacity |= capacity >> 16;
	capacity++;

	WORD_HASHMAP_BUCKETS(map) = Col_NewMVector(capacity, capacity, NULL);
	ASSERT(WORD_TYPE(WORD_HASHMAP_BUCKETS(map)) == WORD_TYPE_MVECTOR);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * GrowStringHashMap --
 * GrowIntHashMap --
 *
 *	Resize a string hash map bucket container. Size won't grow past a
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
GrowStringHashMap(
    Col_Word map)		/* String hash map to grow. */
{
    Col_Word *buckets, *newBuckets, newBucketContainer, entry, dummy;
    size_t nbBuckets, newNbBuckets;
    unsigned int hash, index, newIndex;
    int key;

    /*
     * Create a new bucket container.
     */

    GET_BUCKETS(map, nbBuckets, buckets, dummy);
    ASSERT(!(nbBuckets & (nbBuckets-1)));
    newNbBuckets = nbBuckets * GROW_FACTOR;
    if (newNbBuckets > VECTOR_MAX_LENGTH(MVECTOR_MAX_SIZE * CELL_SIZE)) {
	/*
	 * Already too large.
	 */

	return 0;
    }

    /*
     * Rehash all entries from old to new bucket array.
     *
     * Note: bucket size is always a power of 2.
     */

    ASSERT(!(newNbBuckets & (newNbBuckets-1)));
    newBucketContainer = Col_NewMVector(newNbBuckets, newNbBuckets, NULL);
    ASSERT(WORD_TYPE(newBucketContainer) == WORD_TYPE_MVECTOR);
    newBuckets = WORD_VECTOR_ELEMENTS(newBucketContainer);

    for (index=0; index < nbBuckets; index++) {
	entry = buckets[index];
	while (entry) {
	    ASSERT(WORD_TYPE(entry) == WORD_TYPE_MAPENTRY);

	    /*
	     * Get new index from string key and move at head of new bucket.
	     */

	    buckets[index] = WORD_MAPENTRY_NEXT(entry);

	    key = WORD_INTMAPENTRY_KEY(entry);
	    //TODO: optimize hash computation when possible by combining masked & modulo values?
	    hash = HashString(key);
	    newIndex = hash & (newNbBuckets-1);
	    WORD_MAPENTRY_NEXT(entry) = newBuckets[newIndex];
	    newBuckets[newIndex] = entry;
	    Col_WordSetModified(entry);

	    entry = buckets[index];
	}
    }

    WORD_HASHMAP_BUCKETS(map) = newBucketContainer;
    Col_WordSetModified(map);

    return 1;
}

static int
GrowIntHashMap(
    Col_Word map)		/* Integer hash map to grow. */
{
    Col_Word *buckets, *newBuckets, newBucketContainer, entry, dummy;
    size_t nbBuckets, newNbBuckets;
    unsigned int index, newIndex;
    int key;

    /*
     * Create a new bucket container.
     */

    GET_BUCKETS(map, nbBuckets, buckets, dummy);
    ASSERT(!(nbBuckets & (nbBuckets-1)));
    newNbBuckets = nbBuckets * GROW_FACTOR;
    if (newNbBuckets > VECTOR_MAX_LENGTH(MVECTOR_MAX_SIZE * CELL_SIZE)) {
	/*
	 * Already too large.
	 */

	return 0;
    }

    /*
     * Rehash all entries from old to new bucket array.
     *
     * Note: bucket size is always a power of 2.
     */

    ASSERT(!(newNbBuckets & (newNbBuckets-1)));
    newBucketContainer = Col_NewMVector(newNbBuckets, newNbBuckets, NULL);
    ASSERT(WORD_TYPE(newBucketContainer) == WORD_TYPE_MVECTOR);
    newBuckets = WORD_VECTOR_ELEMENTS(newBucketContainer);

    for (index=0; index < nbBuckets; index++) {
	entry = buckets[index];
	while (entry) {
	    ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTMAPENTRY);

	    /*
	     * Get new index from integer key and move at head of new bucket.
	     */

	    buckets[index] = WORD_MAPENTRY_NEXT(entry);

	    key = WORD_INTMAPENTRY_KEY(entry);
	    newIndex = RANDOMIZE_KEY(key) & (newNbBuckets-1);
	    WORD_MAPENTRY_NEXT(entry) = newBuckets[newIndex];
	    newBuckets[newIndex] = entry;
	    Col_WordSetModified(entry);

	    entry = buckets[index];
	}
    }

    WORD_HASHMAP_BUCKETS(map) = newBucketContainer;
    Col_WordSetModified(map);

    return 1;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewStringHashMap --
 *
 *	Create a new string hash map word.
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
Col_NewStringHashMap(
    size_t capacity)		/* Initial bucket size. */
{
    Col_Word map;
    
    map = (Col_Word) AllocCells(HASHMAP_NBCELLS);
    WORD_STRHASHMAP_INIT(map);
    AllocBuckets(map, capacity);

    return map;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_StringHashMapGet --
 *
 *	Get value mapped to the given string key if present.
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
Col_StringHashMapGet(
    Col_Word map,		/* String hash map to get entry from. */ 
    Col_Word key, 		/* Entry key. */
    Col_Word *valuePtr)		/* Pointer to entry value, if found. */
{
    Col_Word entry = Col_StringHashMapFindEntry(map, key, NULL);
    if (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_MAPENTRY);
	*valuePtr = WORD_MAPENTRY_VALUE(entry);
	return 1;
    } else {
	return 0;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_StringHashMapSet --
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
Col_StringHashMapSet(
    Col_Word map, 		/* String hash map to insert entry into. */
    Col_Word key, 		/* Entry key. */
    Col_Word value)		/* Entry value. */
{
    int create = 1;
    Col_Word entry = Col_StringHashMapFindEntry(map, key, &create);
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_MAPENTRY);
    Col_WordSetModified(entry);
    WORD_MAPENTRY_VALUE(entry) = value;
    return create;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_StringHashMapUnset --
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
Col_StringHashMapUnset(
    Col_Word map, 		/* String hash map to remove entry from. */
    Col_Word key) 		/* Entry key. */
{
    Col_Word *buckets, parent, *prevPtr, entry;
    size_t nbBuckets;
    unsigned int hash, index;

    if (WORD_TYPE(map) != WORD_TYPE_STRHASHMAP) {
	Col_Error(COL_ERROR, "%x is not a string hash map", map);
	return 0;
    }

    /*
     * Search for matching entry.
     *
     * Note: bucket size is always a power of 2.
     */

    GET_BUCKETS(map, nbBuckets, buckets, parent);
    ASSERT(!(nbBuckets & (nbBuckets-1)));
    hash = HashString(key);
    index = hash & (nbBuckets-1);
    prevPtr = &buckets[index];
    while (*prevPtr) {
	entry = *prevPtr;
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_MAPENTRY);

	if (WORD_MAPENTRY_HASH(entry) == (hash & MAPENTRY_HASH_MASK)
		&& Col_CompareRopes(WORD_MAPENTRY_KEY(entry), key, 0, SIZE_MAX,
		NULL, NULL, NULL) == 0) {
	    /*
	     * Found! Unlink & remove entry.
	     */

	    *prevPtr = WORD_MAPENTRY_NEXT(entry);
	    Col_WordSetModified(parent);
	    WORD_HASHMAP_SIZE(map)--;
	    return 1;
	}

	parent = entry;
	prevPtr = &WORD_MAPENTRY_NEXT(entry);
    }

    /*
     * Not found.
     */

    return 0;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_StringHashMapFindEntry --
 *
 *	Find or create the entry mapped to the given key.
 *
 * Results:
 *	The entry if found or created, else nil.
 *
 * Side effects:
 *	If createPtr is non-NULL and the pointed value is non-zero, set to 1 
 *	if a new entry was created, else zero.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_StringHashMapFindEntry(
    Col_Word map, 		/* Integer hash map to find or create entry 
				 * into. */
    Col_Word key, 		/* Entry key. */
    int *createPtr)		/* In: if non-NULL, whether to create entry if 
				 * absent.
				 * Out: if non-NULL, whether a new entry was 
				 * created. */
{
    Col_Word *buckets, bucketParent, entry;
    size_t nbBuckets;
    unsigned int hash, index;

    if (WORD_TYPE(map) != WORD_TYPE_STRHASHMAP) {
	Col_Error(COL_ERROR, "%x is not a string hash map", map);
	return WORD_NIL;
    }

    /*
     * Search for matching entry.
     *
     * Note: bucket size is always a power of 2.
     */

    GET_BUCKETS(map, nbBuckets, buckets, bucketParent);
    ASSERT(!(nbBuckets & (nbBuckets-1)));
    hash = HashString(key);
    index = hash & (nbBuckets-1);
    entry = buckets[index];
    while (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_MAPENTRY);
	if (WORD_MAPENTRY_HASH(entry) == (hash & MAPENTRY_HASH_MASK)
		&& Col_CompareRopes(WORD_MAPENTRY_KEY(entry), key, 0, SIZE_MAX,
		NULL, NULL, NULL) == 0) {
	    /*
	     * Found!
	     */

	    if (createPtr) *createPtr = 0;
	    return entry;
	}

	entry = WORD_MAPENTRY_NEXT(entry);
    }

    /*
     * Not found, create new if asked for.
     */

    if (!createPtr || !*createPtr) {
	return WORD_NIL;
    }
    *createPtr = 1;

    if (WORD_HASHMAP_SIZE(map) >= nbBuckets * LOAD_FACTOR
	    && GrowStringHashMap(map)) {
	/*
	 * Grow map and insert again.
	 */

	return Col_StringHashMapFindEntry(map, key, createPtr);
    }

    /*
     * Insert a new entry at head so that subsequent lookups are faster.
     */

    entry = (Col_Word) AllocCells(1);
    WORD_MAPENTRY_INIT(entry, buckets[index], key, WORD_NIL, hash);
    buckets[index] = entry;
    Col_WordSetModified(bucketParent);

    WORD_HASHMAP_SIZE(map)++;

    return entry;
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
Col_NewIntHashMap(
    size_t capacity)		/* Initial bucket size. */
{
    Col_Word map;
    
    map = (Col_Word) AllocCells(HASHMAP_NBCELLS);
    WORD_INTHASHMAP_INIT(map);
    AllocBuckets(map, capacity);

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
    Col_Word entry = Col_IntHashMapFindEntry(map, key, NULL);
    if (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTMAPENTRY);
	*valuePtr = WORD_MAPENTRY_VALUE(entry);
	return 1;
    } else {
	return 0;
    }
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
    int create = 1;
    Col_Word entry = Col_IntHashMapFindEntry(map, key, &create);
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTMAPENTRY);
    Col_WordSetModified(entry);
    WORD_MAPENTRY_VALUE(entry) = value;
    return create;
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
    size_t nbBuckets;
    unsigned int index;

    if (WORD_TYPE(map) != WORD_TYPE_INTHASHMAP) {
	Col_Error(COL_ERROR, "%x is not an integer hash map", map);
	return 0;
    }

    /*
     * Search for matching entry.
     *
     * Note: bucket size is always a power of 2.
     */

    GET_BUCKETS(map, nbBuckets, buckets, parent);
    ASSERT(!(nbBuckets & (nbBuckets-1)));
    index = RANDOMIZE_KEY(key) & (nbBuckets-1);
    prevPtr = &buckets[index];
    while (*prevPtr) {
	entry = *prevPtr;
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTMAPENTRY);

	if (WORD_INTMAPENTRY_KEY(entry) == key) {
	    /*
	     * Found! Unlink & remove entry.
	     */

	    *prevPtr = WORD_MAPENTRY_NEXT(entry);
	    Col_WordSetModified(parent);
	    WORD_HASHMAP_SIZE(map)--;
	    return 1;
	}

	parent = entry;
	prevPtr = &WORD_MAPENTRY_NEXT(entry);
    }

    /*
     * Not found.
     */

    return 0;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_IntTrieMapFindEntry --
 *
 *	Find or create the entry mapped to the given key.
 *
 * Results:
 *	The entry if found or created, else nil.
 *
 * Side effects:
 *	If createPtr is non-NULL and the pointed value is non-zero, set to 1 
 *	if a new entry was created, else zero.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_IntHashMapFindEntry(
    Col_Word map, 		/* Integer hash map to find or create entry 
				 * into. */
    int key, 			/* Entry key. */
    int *createPtr)		/* In: if non-NULL, whether to create entry if 
				 * absent.
				 * Out: if non-NULL, whether a new entry was 
				 * created. */
{
    Col_Word *buckets, bucketParent, entry;
    size_t nbBuckets;
    unsigned int index;

    if (WORD_TYPE(map) != WORD_TYPE_INTHASHMAP) {
	Col_Error(COL_ERROR, "%x is not an integer hash map", map);
	return WORD_NIL;
    }

    /*
     * Search for matching entry.
     *
     * Note: bucket size is always a power of 2.
     */

    GET_BUCKETS(map, nbBuckets, buckets, bucketParent);
    ASSERT(!(nbBuckets & (nbBuckets-1)));
    index = RANDOMIZE_KEY(key) & (nbBuckets-1);
    entry = buckets[index];
    while (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTMAPENTRY);
	if (WORD_INTMAPENTRY_KEY(entry) == key) {
	    /*
	     * Found!
	     */

	    if (createPtr) *createPtr = 0;
	    return entry;
	}

	entry = WORD_MAPENTRY_NEXT(entry);
    }

    /*
     * Not found, create new if asked for.
     */

    if (!createPtr || !*createPtr) {
	return WORD_NIL;
    }
    *createPtr = 1;

    if (WORD_HASHMAP_SIZE(map) >= nbBuckets * LOAD_FACTOR
	    && GrowIntHashMap(map)) {
	/*
	 * Grow map and insert again.
	 */

	return Col_IntHashMapFindEntry(map, key, createPtr);
    }

    /*
     * Insert a new entry at head so that subsequent lookups are faster.
     */

    entry = (Col_Word) AllocCells(1);
    WORD_INTMAPENTRY_INIT(entry, buckets[index], key, WORD_NIL);
    buckets[index] = entry;
    Col_WordSetModified(bucketParent);

    WORD_HASHMAP_SIZE(map)++;

    return entry;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_HashMapIterBegin --
 *
 *	Initialize the hash map iterator so that it points to the first entry
 *	within the map.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Iterator will point to the first entry in map.
 *
 *---------------------------------------------------------------------------
 */

void
Col_HashMapIterBegin(
    Col_Word map,		/* Map to iterate over. */
    Col_MapIterator *it)	/* Iterator to initialize. */
{
    Col_Word *buckets, dummy;
    size_t nbBuckets, i;

    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRHASHMAP:
	case WORD_TYPE_INTHASHMAP:
	    break;

	default:
	    Col_Error(COL_ERROR, "%x is not a hash map", map);
	    return;
    }

    if (Col_MapSize(map) == 0) {
	/*
	 * Map is empty anyway.
	 */

	it->map = WORD_NIL;
	it->entry = WORD_NIL;
	return;
    }

    /*
     * Get first entry in first nonempty bucket.
     */

    it->map = map;
    GET_BUCKETS(map, nbBuckets, buckets, dummy);
    for (i=0; i < nbBuckets; i++) {
	if (buckets[i]) {
	    it->entry = buckets[i];
	    it->bucket = i;
	    return;
	}
    }

    /* CANTHAPPEN */
    ASSERT(0);
    it->map = WORD_NIL;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_HashMapIterNext --
 *
 *	Move the iterator to the next element.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Update the iterator.
 *
 *---------------------------------------------------------------------------
 */

void
Col_HashMapIterNext(
    Col_MapIterator *it)	/* The iterator to move. */
{
    Col_Word *buckets, dummy;
    size_t nbBuckets, i;

    if (Col_MapIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid map iterator");
	return;
    }

    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_MAPENTRY 
	    || WORD_TYPE(it->entry) == WORD_TYPE_INTMAPENTRY);

    /*
     * Get next entry in bucket.
     */

    it->entry = WORD_MAPENTRY_NEXT(it->entry);
    if (it->entry) {
	return;
    }

    /*
     * End of bucket, get first entry in next nonempty bucket.
     */

    GET_BUCKETS(it->map, nbBuckets, buckets, dummy);
    for (i=it->bucket+1; i < nbBuckets; i++) {
	if (buckets[i]) {
	    it->entry = buckets[i];
	    it->bucket = i;
	    return;
	}
    }

    /*
     * End of map.
     */

    it->map = WORD_NIL;
}
