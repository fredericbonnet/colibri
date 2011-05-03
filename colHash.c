/*
 * File: colHash.c
 *
 *	This file implements the hash map handling features of Colibri.
 *
 *	Hash maps are an implementation of generic maps that uses key hashing 
 *	and flat bucket arrays for string and integer keys.
 *
 *	They are always mutable.
 *
 * See also:
 *	<colHash.h>
 */

#include "colibri.h"
#include "colInternal.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>

/*
 * Prototypes for functions used only in this file.
 */

static Col_RopeChunksTraverseProc HashChunkProc;
static Col_HashProc HashString;
static int		GrowHashMap(Col_Word map, Col_HashProc proc);
static int		GrowIntHashMap(Col_Word map);
static Col_Word		StringHashMapFindEntry(Col_Word map, Col_Word key,
			    int *createPtr, size_t *bucketPtr);
static Col_Word		IntHashMapFindEntry(Col_Word map, intptr_t key,
			    int *createPtr, size_t *bucketPtr);
static void		AllocBuckets(Col_Word map, size_t capacity);


/****************************************************************************
 * Internal Group: Internal Definitions
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macro: RANDOMIZE_KEY
 *
 *	Integer key "randomization" by multiplication with a large prime. Given 
 *	that multiplication by an odd number is reversible on 2's complement
 *	integer representations, this guarantees no collision.
 *---------------------------------------------------------------------------*/

#define RANDOMIZE_KEY(i) \
    (((uintptr_t) (i))*1610612741)

/*---------------------------------------------------------------------------
 * Internal Constants: Bucket Array Growth Control
 *
 *	Constants controlling the behavior of hash tables.
 *
 *  LOAD_FACTOR	- Grow bucket container when size exceeds bucket size times
 *		  this load factor.
 *  GROW_FACTOR - When growing bucket container, multiply current size by this
 *		  grow factor.
 *
 * See also:
 *	<GrowHashMap>, <GrowIntHashMap>
 *---------------------------------------------------------------------------*/

#define LOAD_FACTOR \
    1
#define GROW_FACTOR \
    4

/*---------------------------------------------------------------------------
 * Internal Macro: GET_BUCKETS
 *
 *	Bucket access. Get bucket array regardless of whether it is stored in 
 *	static space or in a separate vector word.
 *
 * Argument:
 *	map		- Map to get bucket array for.
 *
 * Results:
 *	nBuckets	- Size of bucket array.
 *	buckets		- Bucket array.
 *	bucketParent	- Parent word of bucket container. Used during 
 *			  modification (see <Col_WordSetModified>).
 *---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------
 * Internal Macro: STRING_HASH
 *
 *	String hash value computation. Uses the same algorithm as Tcl's string 
 *	hash tables (HashStringKey).
 *
 * Arguments:
 *	hash	- Hash value.
 *	c	- Character codepoint.
 *
 * Result:
 *	Hash value through hash argument variable.
 *
 * See also:
 *	<HashChunkProc>
 *---------------------------------------------------------------------------*/

#define STRING_HASH(hash, c) \
    (hash) += ((hash)<<3)+(c)

/*---------------------------------------------------------------------------
 * Internal Function: HashChunkProc
 *
 *	Rope traversal proc that computes its hash value. Called on 
 *	<Col_TraverseRopeChunks> by <HashString>. Follows 
 *	<Col_RopeChunksTraverseProc> signature.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 1.
 *	chunks		- Array of chunks. First element never NULL.
 *	clientData	- Points to hash value.
 *
 * Result:
 *	Always zero, hash value returned through clientData.
 *
 * See also:
 *	<STRING_HASH>, <HashString>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static int 
HashChunkProc(
    size_t index, 
    size_t length, 
    size_t number, 
    const Col_RopeChunk *chunks, 
    Col_ClientData clientData)
{
    size_t i;
    intptr_t hash = *(uintptr_t *) clientData;
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
    *(uintptr_t *) clientData = hash;
    return 0;
}

/*---------------------------------------------------------------------------
 * Internal Function: HashString
 *
 *	Compute a string key hash value. Uses <Col_TraverseRopeChunks> with
 *	traversal proc <HashChunkProc>. Follows <Col_HashProc> signature.
 *
 * Result:
 *	The given rope's hash value.
 *
 * See also:
 *	<HashString>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static uintptr_t 
HashString(
    Col_Word key)
{
    uintptr_t hash = 0;
    Col_TraverseRopeChunks(1, &key, 0, SIZE_MAX, HashChunkProc, &hash, NULL);
    return hash;
}

/*---------------------------------------------------------------------------
 * Internal Function: GrowHashMap
 *
 *	Resize a hash map bucket container. Size won't grow past a given limit.
 *	As the bucket container is a mutable vector, this limit matches the 
 *	maximum mutable vector length.
 *
 * Arguments:
 *	map	- Hash map to grow.
 *	proc	- Hash proc to apply on keys.
 *
 * Result:
 *	Whether the bucket container was resized or not. 
 *
 * Side effects:
 *	Bucket container may be resized.
 *---------------------------------------------------------------------------*/

static int
GrowHashMap(
    Col_Word map,
    Col_HashProc proc)
{
    Col_Word *buckets, *newBuckets, newBucketContainer, entry, dummy;
    size_t nbBuckets, newNbBuckets;
    uintptr_t hash, index, newIndex;
    Col_Word key;

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

	    key = WORD_MAPENTRY_KEY(entry);
	    //TODO: optimize hash computation when possible by combining masked & modulo values?
	    hash = proc(key);
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

/*---------------------------------------------------------------------------
 * Internal Function: GrowIntHashMap
 *
 *	Resize an integer hash map bucket container. Size won't grow past a 
 *	given limit. As the bucket container is a mutable vector, this limit 
 *	matches the maximum mutable vector length.
 *
 * Arguments:
 *	map	- Integer hash map to grow.
 *
 * Result:
 *	Whether the bucket container was resized or not. 
 *
 * Side effects:
 *	Bucket container may be resized.
 *---------------------------------------------------------------------------*/

static int
GrowIntHashMap(
    Col_Word map)
{
    Col_Word *buckets, *newBuckets, newBucketContainer, entry, dummy;
    size_t nbBuckets, newNbBuckets;
    uintptr_t index, newIndex;
    intptr_t key;

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


/****************************************************************************
 * Group: Hash Map Creation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Function: AllocBuckets
 *
 *	Allocate bucket container having the given minimum capacity, rounded
 *	up to a power of 2.
 *
 * Arguments:
 *	map		- Map to allocate buckets for.
 *	capacity	- Initial bucket size.
 *
 * Side effects:
 *	May allocate memory cells.
 *
 * See also:
 *	<Col_NewStringHashMap>, <Col_NewIntHashMap>
 *---------------------------------------------------------------------------*/

static void
AllocBuckets(
    Col_Word map,
    size_t capacity)
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

/*---------------------------------------------------------------------------
 * Function: Col_NewStringHashMap
 *
 *	Create a new string hash map word.
 *
 * Argument:
 *	capacity	- Initial bucket size. Rounded up to the next power of
 *			  2.
 *
 * Result:
 *	The new word.
 *
 * Side effects:
 *	Allocates memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewStringHashMap(
    size_t capacity)
{
    Col_Word map;
    
    map = (Col_Word) AllocCells(HASHMAP_NBCELLS);
    WORD_STRHASHMAP_INIT(map);
    AllocBuckets(map, capacity);

    return map;
}

/*---------------------------------------------------------------------------
 * Function: Col_NewIntHashMap
 *
 *	Create a new integer hash map word.
 *
 * Argument:
 *	capacity	- Initial bucket size. Rounded up to the next power of
 *			  2.
 *
 * Result:
 *	The new word.
 *
 * Side effects:
 *	Allocates memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewIntHashMap(
    size_t capacity)
{
    Col_Word map;
    
    map = (Col_Word) AllocCells(HASHMAP_NBCELLS);
    WORD_INTHASHMAP_INIT(map);
    AllocBuckets(map, capacity);

    return map;
}


/****************************************************************************
 * Internal Group: Hash Map Entries
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Function: StringHashMapFindEntry
 *
 *	Find or create in string hash map the entry mapped to the given key.
 *
 * Arguments:
 *	map		- String hash map to find or create entry into.
 *	key		- String entry key.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *
 * Results:
 *	The entry if found or created, else nil. Additionally:
 *
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *	bucketPtr	- If non-NULL, bucket containing the entry if found.
 *---------------------------------------------------------------------------*/

static Col_Word
StringHashMapFindEntry(
    Col_Word map,
    Col_Word key,
    int *createPtr,
    size_t *bucketPtr)
{
    Col_Word *buckets, bucketParent, entry;
    size_t nbBuckets;
    uintptr_t hash, index;

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
	    if (bucketPtr) *bucketPtr = index;
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
	    && GrowHashMap(map, HashString)) {
	/*
	 * Grow map and insert again.
	 */

	return StringHashMapFindEntry(map, key, createPtr, bucketPtr);
    }

    /*
     * Insert a new entry at head so that subsequent lookups are faster.
     */

    entry = (Col_Word) AllocCells(1);
    WORD_MAPENTRY_INIT(entry, buckets[index], key, WORD_NIL, hash);
    buckets[index] = entry;
    Col_WordSetModified(bucketParent);

    WORD_HASHMAP_SIZE(map)++;

    if (bucketPtr) *bucketPtr = index;
    return entry;
}

/*---------------------------------------------------------------------------
 * Internal Function: IntHashMapFindEntry
 *
 *	Find or create in integer hash map the entry mapped to the given key.
 *
 * Arguments:
 *	map		- Integer hash map to find or create entry into.
 *	key		- Integer entry key.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *
 * Results:
 *	The entry if found or created, else nil. Additionally:
 *
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *	bucketPtr	- If found, bucket containing the entry.
 *---------------------------------------------------------------------------*/

static Col_Word
IntHashMapFindEntry(
    Col_Word map,
    intptr_t key,
    int *createPtr,
    size_t *bucketPtr)
{
    Col_Word *buckets, bucketParent, entry;
    size_t nbBuckets;
    uintptr_t index;

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
	    if (bucketPtr) *bucketPtr = index;
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

	return IntHashMapFindEntry(map, key, createPtr, bucketPtr);
    }

    /*
     * Insert a new entry at head so that subsequent lookups are faster.
     */

    entry = (Col_Word) AllocCells(1);
    WORD_INTMAPENTRY_INIT(entry, buckets[index], key, WORD_NIL);
    buckets[index] = entry;
    Col_WordSetModified(bucketParent);

    WORD_HASHMAP_SIZE(map)++;

    if (bucketPtr) *bucketPtr = index;
    return entry;
}


/****************************************************************************
 * Group: Hash Map Access
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_StringHashMapGet
 *
 *	Get value mapped to the given string key if present.
 *
 * Arguments:
 *	map		- String hash map to get entry from.
 *	key		- String entry key.
 *	valuePtr	- Returned entry value, if found.
 *
 * Result:
 *	Whether the key was found in the map. In this case the value is returned
 *	through valuePtr.
 *---------------------------------------------------------------------------*/

int
Col_StringHashMapGet(
    Col_Word map,
    Col_Word key,
    Col_Word *valuePtr)
{
    Col_Word entry;

    if (WORD_TYPE(map) != WORD_TYPE_STRHASHMAP) {
	Col_Error(COL_ERROR, "%x is not a string hash map", map);
	return WORD_NIL;
    }

    entry = StringHashMapFindEntry(map, key, NULL, NULL);

    if (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_MAPENTRY);
	*valuePtr = WORD_MAPENTRY_VALUE(entry);
	return 1;
    } else {
	return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_IntHashMapGet
 *
 *	Get value mapped to the given integer key if present.
 *
 * Arguments:
 *	map		- Integer hash map to get entry from.
 *	key		- Integer entry key.
 *	valuePtr	- Returned entry value, if found.
 *
 * Result:
 *	Whether the key was found in the map. In this case the value is returned
 *	through valuePtr.
 *---------------------------------------------------------------------------*/

int
Col_IntHashMapGet(
    Col_Word map,
    intptr_t key,
    Col_Word *valuePtr)
{
    Col_Word entry;
    
    if (WORD_TYPE(map) != WORD_TYPE_INTHASHMAP) {
	Col_Error(COL_ERROR, "%x is not an integer hash map", map);
	return WORD_NIL;
    }

    entry = IntHashMapFindEntry(map, key, NULL, NULL);
    if (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTMAPENTRY);
	*valuePtr = WORD_MAPENTRY_VALUE(entry);
	return 1;
    } else {
	return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_StringHashMapSet
 *
 *	Map the value to the string key, replacing any existing.
 *
 * Arguments:
 *	map	- String hash map to insert entry into.
 *	key	- String entry key.
 *	value	- Entry value.
 *
 * Result:
 *	Whether a new entry was created.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

int
Col_StringHashMapSet(
    Col_Word map,
    Col_Word key,
    Col_Word value)
{
    int create = 1;
    Col_Word entry;
    
    if (WORD_TYPE(map) != WORD_TYPE_STRHASHMAP) {
	Col_Error(COL_ERROR, "%x is not a string hash map", map);
	return WORD_NIL;
    }

    entry = StringHashMapFindEntry(map, key, &create, NULL);
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_MAPENTRY);
    Col_WordSetModified(entry);
    WORD_MAPENTRY_VALUE(entry) = value;
    return create;
}

/*---------------------------------------------------------------------------
 * Function: Col_IntHashMapSet
 *
 *	Map the value to the integer key, replacing any existing.
 *
 * Arguments:
 *	map	- Integer hash map to insert entry into.
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
Col_IntHashMapSet(
    Col_Word map,
    intptr_t key,
    Col_Word value)
{
    int create = 1;
    Col_Word entry;
    
    if (WORD_TYPE(map) != WORD_TYPE_INTHASHMAP) {
	Col_Error(COL_ERROR, "%x is not an integer hash map", map);
	return WORD_NIL;
    }

    entry = IntHashMapFindEntry(map, key, &create, NULL);
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTMAPENTRY);
    Col_WordSetModified(entry);
    WORD_MAPENTRY_VALUE(entry) = value;
    return create;
}

/*---------------------------------------------------------------------------
 * Function: Col_StringHashMapUnset
 *
 *	Remove any value mapped to the given string key.
 *
 * Arguments:
 *	map	- String hash map to remove entry from.
 *	key	- String entry key.
 *
 * Result:
 *	Whether an existing entry was removed.
 *---------------------------------------------------------------------------*/

int
Col_StringHashMapUnset(
    Col_Word map,
    Col_Word key)
{
    Col_Word *buckets, parent, *prevPtr, entry;
    size_t nbBuckets;
    uintptr_t hash, index;

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

/*---------------------------------------------------------------------------
 * Function: Col_IntHashMapUnset
 *
 *	Remove any value mapped to the given integer key.
 *
 * Arguments:
 *	map	- Integer hash map to remove entry from.
 *	key	- Integer entry key.
 *
 * Result:
 *	Whether an existing entry was removed.
 *---------------------------------------------------------------------------*/

int
Col_IntHashMapUnset(
    Col_Word map,
    intptr_t key)
{
    Col_Word *buckets, parent, *prevPtr, entry;
    size_t nbBuckets;
    uintptr_t index;

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


/****************************************************************************
 * Group: Hash Map Iterators
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_HashMapIterBegin
 *
 *	Initialize the hash map iterator so that it points to the first entry
 *	within the map.
 *
 * Arguments:
 *	map	- Hash map to iterate over.
 *	it	- Iterator to initialize.
 *---------------------------------------------------------------------------*/

void
Col_HashMapIterBegin(
    Col_Word map,
    Col_MapIterator *it)
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

/*---------------------------------------------------------------------------
 * Function: Col_HashMapIterNext
 *
 *	Move the iterator to the next element.
 *
 * Argument:
 *	it	- The iterator to move.
 *---------------------------------------------------------------------------*/

void
Col_HashMapIterNext(
    Col_MapIterator *it)
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

/*---------------------------------------------------------------------------
 * Function: Col_StringHashMapIterFind
 *
 *	Initialize the hash map iterator so that it points to the entry with
 *	the given string key within the map.
 *
 * Arguments:
 *	map		- String hash map to find or create entry into.
 *	key		- String entry key.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *	it		- Iterator to initialize.
 *
 * Results:
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *---------------------------------------------------------------------------*/

void
Col_StringHashMapIterFind(
    Col_Word map, 
    Col_Word key, 
    int *createPtr, 
    Col_MapIterator *it)
{
    if (WORD_TYPE(map) != WORD_TYPE_STRHASHMAP) {
	Col_Error(COL_ERROR, "%x is not a string hash map", map);
	it->map = WORD_NIL;
	return;
    }

    it->entry = StringHashMapFindEntry(map, key, createPtr, &it->bucket);
    if (!it->entry) {
	/*
	 * Not found.
	 */

	it->map = WORD_NIL;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_IntHashMapIterFind
 *
 *	Initialize the hash map iterator so that it points to the entry with
 *	the given string key within the map.
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
Col_IntHashMapIterFind(
    Col_Word map, 
    intptr_t key, 
    int *createPtr, 
    Col_MapIterator *it)
{
    if (WORD_TYPE(map) != WORD_TYPE_INTHASHMAP) {
	Col_Error(COL_ERROR, "%x is not an integer hash map", map);
	it->map = WORD_NIL;
	return;
    }

    it->entry = IntHashMapFindEntry(map, key, createPtr, &it->bucket);
    if (!it->entry) {
	/*
	 * Not found.
	 */

	it->map = WORD_NIL;
    }
}
