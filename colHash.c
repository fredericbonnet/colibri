/*
 * File: colHash.c
 *
 *	This file implements the hash map handling features of Colibri.
 *
 *	Hash maps are an implementation of generic maps that use key hashing 
 *	and flat bucket arrays for string, integer and custom keys.
 *
 *	They are always mutable.
 *
 * See also:
 *	<colHash.h>, <colMap.h>
 */

#include "include/colibri.h"
#include "colInternal.h"

#include "colWordInt.h"
#include "colVectorInt.h"
#include "colMapInt.h"
#include "colHashInt.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>

/*
 * Prototypes for functions used only in this file.
 */

static Col_RopeChunksTraverseProc HashChunkProc;
static Col_HashProc HashString;
static Col_CompareProc CompareStrings;
static void		AllocBuckets(Col_Word map, size_t capacity);
static int		GrowHashMap(Col_Word map, Col_HashProc hashProc);
static int		GrowIntHashMap(Col_Word map);
static Col_Word		HashMapFindEntry(Col_Word map, Col_HashProc hashProc,
			    Col_CompareProc compareProc, Col_Word key,
			    int mutable, int *createPtr, size_t *bucketPtr);
static Col_Word		IntHashMapFindEntry(Col_Word map, intptr_t key,
			    int mutable, int *createPtr, size_t *bucketPtr);
static Col_Word		ConvertEntryToMutable(Col_Word entry, 
			    Col_Word *prevPtr);
static Col_Word		ConvertIntEntryToMutable(Col_Word entry, 
			    Col_Word *prevPtr);


/*
================================================================================
Internal Section: Hash Map Internals
================================================================================
*/

/****************************************************************************
 * Internal Group: Key Hashing
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Algorithm: Hashing Algorithms
 *
 *	Hash value computation.
 *
 *	For each entry in a hash map, an integer hash value is computed from 
 *	the key, and this hash value is used to select a "bucket", i.e. an
 *	insertion point in a dynamic array. As several entries with distinct 
 *	keys can share a single bucket, their keys are compared against the
 *	searched key until the correct entry is found or not.
 *
 *	The choice of a good hashing algorithm is crucial for hash table 
 *	performances. This algorithm must minimize collisions. A collision 
 *	occurs when two distinct keys give the same hash value or end up in the
 *	same bucket.
 *
 *	Integer keys are hashed by multiplying to a large prime number to get
 *	a pseudorandom distribution (see <RANDOMIZE_KEY>).
 *
 *	String keys are hashed using the same algorithm as Tcl, i.e. a 
 *	cumulative shift+add of character codepoints (see <STRING_HASH>,
 *	<HashChunkProc>, <HashString>).
 *
 *	Custom keys are hashed using a custom hash proc as well as a custom key 
 *	comparison proc (see <Col_CustomHashMapType>, <Col_HashProc>, 
 *	<Col_CompareProc>) 
 */

/*---------------------------------------------------------------------------
 * Internal Macro: RANDOMIZE_KEY
 *
 *	Integer key "randomization" by multiplication with a large prime. Given 
 *	that multiplication by an odd number is reversible on 2's complement
 *	integer representations, this guarantees no collision.
 *---------------------------------------------------------------------------*/

#define RANDOMIZE_KEY(i)	(((uintptr_t) (i))*1610612741)

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
    const char *data = (const char *) chunks->data;
    Col_Char c;
    ASSERT(number == 1);
    for (i = 0; i < length; i++) {
	COL_CHAR_NEXT(chunks->format, data, c);
	STRING_HASH(hash, c);
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
 * Argument:
 *	key	- Key word to generate hash value for.
 *
 * Result:
 *	The given rope's hash value.
 *
 * See also:
 *	<HashChunkProc>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static uintptr_t 
HashString(
    Col_Word key)
{
    uintptr_t hash = 0;
    Col_TraverseRopeChunks(key, 0, SIZE_MAX, 0, HashChunkProc, &hash, NULL);
    return hash;
}

/*---------------------------------------------------------------------------
 * Internal Function: CompareStrings
 *
 *	Compare string keys. Follows <Col_CompareProc> signature.
 *
 * Arguments:
 *	key1, key2	- Key words to compare.
 *
 * Result:
 *	Negative if key1 is less than key2, positive if key2 is greater than
 *	key2, zero if both keys are equal.
 *---------------------------------------------------------------------------*/

static int 
CompareStrings(
    Col_Word key1,
    Col_Word key2)
{
    return Col_CompareRopes(key1, key2);
}


/****************************************************************************
 * Internal Group: Buckets
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Algorithm: Hash Map Bucket Storage
 *
 *	Hash map bucket mananagement.
 *
 * Bucket storage:
 *	Buckets are stored as flat arrays. The bucket index is computed from the
 *	hash value. Bucket size is always a power of 2, so we use the lower bits
 *	of the hash value to select the bucket index.
 *
 * Growth and rehashing:
 *	When two entries end up in the same bucket, there is a collision. When 
 *	the map exceeds a certain size (see <LOAD_FACTOR>), the table is resized
 *	(see <GROW_FACTOR>) and entries are rehashed (all entries are moved from 
 *	the old bucket container to the new one according to their hash value, 
 *	see <GrowHashMap> and <GrowIntHashMap>).
 *
 *	Given that hash entries store high order bits of the hash value (all but
 *	the lower byte), this means that we can get back the full hash value by
 *	combining these high bits with the bucket index when the bucket size is 
 *	at least one byte wide (see <WORD_HASHENTRY_HASH>). This saves having to
 *	recompute the hash value during rehashing.
 *
 * See also:
 *	<Hash Map Word>, <Hash Entry Word>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Constants: Bucket Array Growth Control Constants
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

#define LOAD_FACTOR		1
#define GROW_FACTOR		4

/*---------------------------------------------------------------------------
 * Internal Macro: GET_BUCKETS
 *
 *	Bucket access. Get bucket array regardless of whether it is stored in 
 *	static space or in a separate vector word.
 *
 * Arguments:
 *	map		- Map to get bucket array for.
 *	mutable		- If true, ensure that entry is mutable.
 *
 * Results:
 *	nBuckets	- Size of bucket array.
 *	buckets		- Bucket array.
 *---------------------------------------------------------------------------*/

#define GET_BUCKETS(map, mutable, nbBuckets, buckets) \
    switch (WORD_TYPE(WORD_HASHMAP_BUCKETS(map))) { \
    case WORD_TYPE_VECTOR: \
	if (mutable) { \
	    WORD_HASHMAP_BUCKETS(map) = Col_NewMVector(0, \
		WORD_VECTOR_LENGTH(WORD_HASHMAP_BUCKETS(map)), \
		WORD_VECTOR_ELEMENTS(WORD_HASHMAP_BUCKETS(map))); \
	}; \
	/* continued. */ \
    case WORD_TYPE_MVECTOR: \
	(nbBuckets) = WORD_VECTOR_LENGTH(WORD_HASHMAP_BUCKETS(map)); \
	(buckets) = WORD_VECTOR_ELEMENTS(WORD_HASHMAP_BUCKETS(map)); \
	break; \
    \
    default: \
	ASSERT(!WORD_HASHMAP_BUCKETS(map)); \
	(nbBuckets) = HASHMAP_STATICBUCKETS_SIZE; \
	(buckets) = WORD_HASHMAP_STATICBUCKETS(map); \
    }

/*---------------------------------------------------------------------------
 * Internal Function: GrowHashMap
 *
 *	Resize a hash map bucket container. Size won't grow past a given limit.
 *	As the bucket container is a mutable vector, this limit matches the 
 *	maximum mutable vector length.
 *
 * Arguments:
 *	map		- Hash map to grow.
 *	hashProc	- <Col_HashProc>.
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
    Col_HashProc hashProc)
{
    Col_Word *buckets, *newBuckets, newBucketContainer, entry, next;
    size_t nbBuckets, newNbBuckets;
    uintptr_t hash, index, newIndex;
    Col_Word key;

    GET_BUCKETS(map, 0, nbBuckets, buckets);
    ASSERT(!(nbBuckets & (nbBuckets-1)));
    newNbBuckets = nbBuckets * GROW_FACTOR;
    if (newNbBuckets > VECTOR_MAX_LENGTH(MVECTOR_MAX_SIZE * CELL_SIZE)) {
	/*
	 * Bucket container already too large.
	 */

	return 0;
    }

    /*
     * Create a new bucket container.
     *
     * Note: size is always a power of 2.
     */

    ASSERT(!(newNbBuckets & (newNbBuckets-1)));
    newBucketContainer = Col_NewMVector(newNbBuckets, newNbBuckets, NULL);
    ASSERT(WORD_TYPE(newBucketContainer) == WORD_TYPE_MVECTOR);
    newBuckets = WORD_VECTOR_ELEMENTS(newBucketContainer);

    /*
     * Rehash all entries from old to new bucket array.
     */

    for (index=0; index < nbBuckets; index++) {
	entry = buckets[index];
	while (entry) {
	    /*
	     * Get new index from string key.
	     */

	    key = WORD_MAPENTRY_KEY(entry);
	    if (nbBuckets & ~HASHENTRY_HASH_MASK) {
		/*
		 * Recompute full hash.
		 */

		hash = hashProc(key);
	    } else {
	    /*
		 * Get actual hash by combining entry hash and index.
		 */

		hash = WORD_HASHENTRY_HASH(entry) | index;
		ASSERT(hash == hashProc(key));
	    }
	    newIndex = hash & (newNbBuckets-1);

	    switch (WORD_TYPE(entry)) {
		case WORD_TYPE_HASHENTRY:
		    if (!WORD_HASHENTRY_NEXT(entry) && !newBuckets[newIndex]) {
		    /*
		     * Share immutable entry in new bucket.
			 */

			newBuckets[newIndex] = entry;
			next = WORD_NIL;
			break;
		    }

		/*
		 * Convert to mutable first.
		     */

		    entry = ConvertEntryToMutable(entry, &entry);
		    /* continued. */

		case WORD_TYPE_MHASHENTRY:
		/*
		 * Move entry at head of new bucket.
		     */

		    next = WORD_HASHENTRY_NEXT(entry);
		    WORD_HASHENTRY_NEXT(entry) = newBuckets[newIndex];
		    newBuckets[newIndex] = entry;
	    }

	    entry = next;
	}
}

    WORD_HASHMAP_BUCKETS(map) = newBucketContainer;

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
    Col_Word *buckets, *newBuckets, newBucketContainer, entry, next;
    size_t nbBuckets, newNbBuckets;
    uintptr_t index, newIndex;
    intptr_t key;

    GET_BUCKETS(map, 0, nbBuckets, buckets);
    ASSERT(!(nbBuckets & (nbBuckets-1)));
    newNbBuckets = nbBuckets * GROW_FACTOR;
    if (newNbBuckets > VECTOR_MAX_LENGTH(MVECTOR_MAX_SIZE * CELL_SIZE)) {
	/*
	 * Bucket container already too large.
	 */

	return 0;
    }

    ASSERT(!(newNbBuckets & (newNbBuckets-1)));
    newBucketContainer = Col_NewMVector(newNbBuckets, newNbBuckets, NULL);
    ASSERT(WORD_TYPE(newBucketContainer) == WORD_TYPE_MVECTOR);
    newBuckets = WORD_VECTOR_ELEMENTS(newBucketContainer);

    /*
     * Create a new bucket container.
     *
     * Note: size is always a power of 2.
     */

    ASSERT(!(newNbBuckets & (newNbBuckets-1)));
    newBucketContainer = Col_NewMVector(newNbBuckets, newNbBuckets, NULL);
    ASSERT(WORD_TYPE(newBucketContainer) == WORD_TYPE_MVECTOR);
    newBuckets = WORD_VECTOR_ELEMENTS(newBucketContainer);

    /*
     * Rehash all entries from old to new bucket array.
     */

    for (index=0; index < nbBuckets; index++) {
	entry = buckets[index];
	while (entry) {
	    /*
	     * Get new index from integer key.
	     */

	    key = WORD_INTMAPENTRY_KEY(entry);
	    newIndex = RANDOMIZE_KEY(key) & (newNbBuckets-1);

	    switch (WORD_TYPE(entry)) {
		case WORD_TYPE_INTHASHENTRY:
		    if (!WORD_HASHENTRY_NEXT(entry) && !newBuckets[newIndex]) {
			/*
			 * Share immutable entry in new bucket.
			 */

			newBuckets[newIndex] = entry;
			next = WORD_NIL;
			break;
		    }

		    /*
		     * Convert to mutable first.
		     */

		    entry = ConvertIntEntryToMutable(entry, &entry);
		    /* continued. */

		case WORD_TYPE_MINTHASHENTRY:
		    /*
		     * Move entry at head of new bucket.
		     */

		    next = WORD_HASHENTRY_NEXT(entry);
		    WORD_HASHENTRY_NEXT(entry) = newBuckets[newIndex];
		    newBuckets[newIndex] = entry;
	    }

	    entry = next;
	}
    }

    WORD_HASHMAP_BUCKETS(map) = newBucketContainer;

    return 1;
}


/****************************************************************************
 * Internal Group: Entries
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Function: HashMapFindEntry
 *
 *	Find or create in hash map the entry mapped to the given key.
 *
 * Arguments:
 *	map		- Hash map to find or create entry into.
 *	hashProc	- <Col_HashProc>.
 *	compareProc	- <Col_CompareProc>.
 *	key		- Entry key.
 *	mutable		- If true, ensure that entry is mutable.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *
 * Results:
 *	The entry if found or created, else nil. Additionally:
 *
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *	bucketPtr	- If non-NULL, bucket containing the entry if found.
 *---------------------------------------------------------------------------*/

static Col_Word
HashMapFindEntry(
    Col_Word map,
    Col_HashProc hashProc,
    Col_CompareProc compareProc, 
    Col_Word key,
    int mutable,
    int *createPtr,
    size_t *bucketPtr)
{
    Col_Word *buckets, entry;
    size_t nbBuckets;
    uintptr_t hash, index;

start:
    /*
     * Search for matching entry.
     *
     * Note: bucket size is always a power of 2.
     */

    GET_BUCKETS(map, 0, nbBuckets, buckets);
    ASSERT(!(nbBuckets & (nbBuckets-1)));
    hash = hashProc(key);
    index = hash & (nbBuckets-1);
    entry = buckets[index];
    while (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_HASHENTRY || WORD_TYPE(entry) == WORD_TYPE_MHASHENTRY);
	if (WORD_HASHENTRY_HASH(entry) == (hash & HASHENTRY_HASH_MASK)
		&& compareProc(WORD_MAPENTRY_KEY(entry), key) == 0) {
	    /*
	     * Found!
	     */

	    if (createPtr) *createPtr = 0;
	    if (bucketPtr) *bucketPtr = index;

	    if (mutable && WORD_TYPE(entry) != WORD_TYPE_MHASHENTRY) {
		/*
		 * Entry is immutable, convert to mutable.
		 */

		ASSERT(WORD_TYPE(entry) == WORD_TYPE_HASHENTRY);
		GET_BUCKETS(map, 1, nbBuckets, buckets);
		entry = ConvertEntryToMutable(entry, &buckets[index]);
		ASSERT(WORD_TYPE(entry) == WORD_TYPE_MHASHENTRY);
	    }
	    return entry;
	}

	entry = WORD_HASHENTRY_NEXT(entry);
    }

    /*
     * Not found, create new if asked for.
     */

    if (!createPtr || !*createPtr) {
	return WORD_NIL;
    }
    *createPtr = 1;

    if (WORD_HASHMAP_SIZE(map) >= nbBuckets * LOAD_FACTOR
	    && GrowHashMap(map, hashProc)) {
	/*
	 * Grow map and insert again.
	 */

	goto start;
    }

    /*
     * Insert a new entry at head so that subsequent lookups are faster. 
     * Moreover this leaves potentially immutable existing entries untouched.
     */

    GET_BUCKETS(map, 1, nbBuckets, buckets);
    entry = (Col_Word) AllocCells(1);
    WORD_MHASHENTRY_INIT(entry, key, WORD_NIL, buckets[index], hash);
    buckets[index] = entry;
    
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
 *	mutable		- If true, ensure that entry is mutable.
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
    int mutable,
    int *createPtr,
    size_t *bucketPtr)
{
    Col_Word *buckets, entry;
    size_t nbBuckets;
    uintptr_t index;

start:
    /*
     * Search for matching entry.
     *
     * Note: bucket size is always a power of 2.
     */

    GET_BUCKETS(map, 0, nbBuckets, buckets);
    ASSERT(!(nbBuckets & (nbBuckets-1)));
    index = RANDOMIZE_KEY(key) & (nbBuckets-1);
    entry = buckets[index];
    while (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTHASHENTRY || WORD_TYPE(entry) == WORD_TYPE_MINTHASHENTRY);
	if (WORD_INTMAPENTRY_KEY(entry) == key) {
	    /*
	     * Found!
	     */

	    if (createPtr) *createPtr = 0;
	    if (bucketPtr) *bucketPtr = index;

	    if (mutable && WORD_TYPE(entry) != WORD_TYPE_MINTHASHENTRY) {
		/*
		 * Entry is immutable, convert to mutable.
		 */

		ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTHASHENTRY);
		GET_BUCKETS(map, 1, nbBuckets, buckets);
		entry = ConvertIntEntryToMutable(entry, &buckets[index]);
		ASSERT(WORD_TYPE(entry) == WORD_TYPE_MINTHASHENTRY);
	    }
	    return entry;
	}

	entry = WORD_HASHENTRY_NEXT(entry);
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

	goto start;
    }

    /*
     * Insert a new entry at head so that subsequent lookups are faster. 
     * Moreover this leaves potentially immutable existing entries untouched.
     */

    GET_BUCKETS(map, 1, nbBuckets, buckets);
    entry = (Col_Word) AllocCells(1);
    WORD_MINTHASHENTRY_INIT(entry, key, WORD_NIL, buckets[index]);
    buckets[index] = entry;

    WORD_HASHMAP_SIZE(map)++;

    if (bucketPtr) *bucketPtr = index;
    return entry;
}


/****************************************************************************
 * Internal Group: Mutability
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Algorithm: Mutable and Immutable Hash Entries
 *
 *	Mutability and data sharing with hash maps.
 *
 *	From an external point of view, hash maps, like generic maps, are a 
 *	naturally mutable data type. However, internal structures like bucket
 *	container or entries are usually mutable but can become immutable 
 *	through shared copying (see <Col_CopyHashMap>). This means that we have
 *	to turn immutable data mutable on each mutable operation.
 *
 *	Small hash tables store their buckets inline and so don't share them. 
 *	Larger hash tables store their buckets in a vector, which can be shared
 *	when copied. Turning an immutable vector mutable only implies creating
 *	a new mutable copy of this vector. This is done transparently by
 *	<GET_BUCKETS> when passed a true *mutable* parameter.
 *
 *	As entries form linked lists in each bucket, turning an immutable
 *	entry mutable implies that all its predecessors are turned mutable 
 *	before. The trailing entries can remain immutable. This is the role
 *	of <ConvertEntryToMutable> and <ConvertIntEntryToMutable>.
 *
 *	This ensures that modified data is always mutable and that unmodified
 *	data remains shared as long as possible.
 *
 * See also:
 *	<Hash Entry Word>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Function: ConvertEntryToMutable
 *
 *	Convert immutable entry and all all its predecessors to mutable.
 *
 * Arguments:
 *	entry	- Entry to convert (inclusive).
 *	prevPtr	- Points to first entry in chain containing entry.
 *
 * Result:
 *	The converted mutable entry.
 *---------------------------------------------------------------------------*/

static Col_Word
ConvertEntryToMutable(
    Col_Word entry,
    Col_Word *prevPtr)
{
    Col_Word converted;

    ASSERT(WORD_TYPE(entry) == WORD_TYPE_HASHENTRY);
    for (;;) {
	int last = (*prevPtr == entry);

	ASSERT(WORD_TYPE(*prevPtr) == WORD_TYPE_HASHENTRY);

	/*
	 * Convert entry: copy then change type ID.
	 */

	converted = (Col_Word) AllocCells(1);
	memcpy((void *) converted, (void *) *prevPtr, sizeof(Cell));
	ASSERT(!WORD_PINNED(*prevPtr));
	WORD_SET_TYPEID(converted, WORD_TYPE_MHASHENTRY);
	*prevPtr = converted;

	if (last) return converted;

	prevPtr = &WORD_HASHENTRY_NEXT(*prevPtr);
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: ConvertIntEntryToMutable
 *
 *	Convert immutable integer entry and all all its predecessors to mutable.
 *
 * Arguments:
 *	entry	- Entry to convert (inclusive).
 *	prevPtr	- Points to first entry in chain containing entry.
 *
 * Result:
 *	The converted mutable entry.
 *---------------------------------------------------------------------------*/

static Col_Word
ConvertIntEntryToMutable(
    Col_Word entry,
    Col_Word *prevPtr)
{
    Col_Word converted;

    ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTHASHENTRY);
    for (;;) {
	int last = (*prevPtr == entry);

	ASSERT(WORD_TYPE(*prevPtr) == WORD_TYPE_INTHASHENTRY);

	/*
	 * Convert entry: copy then change type ID.
	 */

	converted = (Col_Word) AllocCells(1);
	memcpy((void *) converted, (void *) *prevPtr, sizeof(Cell));
	ASSERT(!WORD_PINNED(*prevPtr));
	WORD_SET_TYPEID(converted, WORD_TYPE_MINTHASHENTRY);
	*prevPtr = converted;

	if (last) return converted;

	prevPtr = &WORD_HASHENTRY_NEXT(*prevPtr);
    }
}


/*
================================================================================
Section: Hash Maps
================================================================================
*/

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

/*---------------------------------------------------------------------------
 * Function: Col_CopyHashMap
 *
 *	Create a new hash map word from an existing one.
 *
 * Argument:
 *	map	- Hash map to copy.
 *
 * Type checking:
 *	*map* must be a valid hash map.
 *
 * Result:
 *	The new word.
 *
 * Side effects:
 *	Source map content is frozen.
 *---------------------------------------------------------------------------*/

Col_Word
Col_CopyHashMap(
    Col_Word map)
{
    Col_Word newMap;
    Col_Word entry, *buckets;
    size_t nbBuckets, i;
    int entryType;

    /*
     * Check preconditions.
     */

    TYPECHECK_HASHMAP(map) return WORD_NIL;

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
    case WORD_TYPE_CUSTOM:
	entryType = WORD_TYPE_HASHENTRY;
	break;

    case WORD_TYPE_INTHASHMAP:
	entryType = WORD_TYPE_INTHASHENTRY;
	break;

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
    }

    /*
     * Copy word first.
     */

    //TODO: provide custom data copy mechanism
    newMap = (Col_Word) AllocCells(HASHMAP_NBCELLS);
    memcpy((void *) newMap, (void *) map, sizeof(Cell) * HASHMAP_NBCELLS);
    WORD_SYNONYM(newMap) = WORD_NIL;
    WORD_CLEAR_PINNED(newMap);

    if (WORD_HASHMAP_BUCKETS(map)) {
	if (WORD_TYPE(WORD_HASHMAP_BUCKETS(map)) == WORD_TYPE_VECTOR) {
	    /*
	     * Already frozen.
	     */

	    return newMap;
	}

	/*
	 * Freeze bucket container.
	 */

	Col_MVectorFreeze(WORD_HASHMAP_BUCKETS(map));
    }

    /*
     * Freeze entries: simply change their type ID.
     */

    GET_BUCKETS(map, 0, nbBuckets, buckets);
    for (i=0; i < nbBuckets; i++) {
	entry = buckets[i];
	while (entry) {
	    if (WORD_TYPE(entry) == entryType) {
		/*
		 * Already frozen, skip remaining entries.
		 */

		break;
	    }

	    ASSERT(!WORD_PINNED(entry));
	    WORD_SET_TYPEID(entry, entryType);
	    entry = WORD_HASHENTRY_NEXT(entry);
	}
    }

    /*
     * Both maps now share the same immutable structure.
     */

    return newMap;
}


/****************************************************************************
 * Group: Hash Map Accessors
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_HashMapGet
 *
 *	Get value mapped to the given key if present.
 *
 * Arguments:
 *	map		- Hash map to get entry from.
 *	key		- Entry key. Can be any word type, including string, 
 *			  however it must match the actual type used by the map.
 *	valuePtr	- Returned entry value, if found.
 *
 * Type checking:
 *	*map* must be a valid string or custom hash map.
 *
 * Results:
 *	Whether the key was found in the map. In this case the value is returned
 *	through valuePtr.
 *---------------------------------------------------------------------------*/

int
Col_HashMapGet(
    Col_Word map,
    Col_Word key,
    Col_Word *valuePtr)
{
    Col_HashProc *hashProc;
    Col_CompareProc *compareProc;
    Col_Word entry;

    /*
     * Check preconditions.
     */

    TYPECHECK_WORDHASHMAP(map) return 0;

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
	hashProc = HashString;
	compareProc = CompareStrings;
	break;
	
    case WORD_TYPE_CUSTOM: {
	Col_CustomHashMapType *typeInfo 
		= (Col_CustomHashMapType *) WORD_TYPEINFO(map);
	ASSERT(typeInfo->type.type == COL_HASHMAP);
	hashProc = typeInfo->hashProc;
	compareProc = typeInfo->compareProc;
	break;
	}

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
    }

    entry = HashMapFindEntry(map, hashProc, compareProc, key, 0, NULL, NULL);
    if (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_MHASHENTRY);
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
 * Type checking:
 *	*map* must be a valid integer hash map.
 *
 * Results:
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
    
    /*
     * Check preconditions.
     */

    TYPECHECK_INTHASHMAP(map) return 0;

    entry = IntHashMapFindEntry(map, key, 0, NULL, NULL);
    if (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_MINTHASHENTRY);
	*valuePtr = WORD_MAPENTRY_VALUE(entry);
	return 1;
    } else {
	return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_HashMapSet
 *
 *	Map the value to the key, replacing any existing.
 *
 * Arguments:
 *	map	- Hash map to insert entry into.
 *	key	- Entry key. Can be any word type, including string, however it
 *		  must match the actual type used by the map.
 *	value	- Entry value.
 *
 * Type checking:
 *	*map* must be a valid string or custom hash map.
 *
 * Result:
 *	Whether a new entry was created.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

int
Col_HashMapSet(
    Col_Word map,
    Col_Word key,
    Col_Word value)
{
    Col_HashProc *hashProc;
    Col_CompareProc *compareProc;
    int create = 1;
    Col_Word entry;

    /*
     * Check preconditions.
     */

    TYPECHECK_WORDHASHMAP(map) return 0;

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
	hashProc = HashString;
	compareProc = CompareStrings;
	break;
	
    case WORD_TYPE_CUSTOM: {
	Col_CustomHashMapType *typeInfo 
		= (Col_CustomHashMapType *) WORD_TYPEINFO(map);
	ASSERT(typeInfo->type.type == COL_HASHMAP);
	hashProc = typeInfo->hashProc;
	compareProc = typeInfo->compareProc;
	break;
	}

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
    }

    entry = HashMapFindEntry(map, hashProc, compareProc, key, 1, &create, NULL);
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_MHASHENTRY);
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
 * Type checking:
 *	*map* must be a valid integer hash map.
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
    
    /*
     * Check preconditions.
     */

    TYPECHECK_INTHASHMAP(map) return 0;

    entry = IntHashMapFindEntry(map, key, 1, &create, NULL);
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_MINTHASHENTRY);
    WORD_MAPENTRY_VALUE(entry) = value;
    return create;
}

/*---------------------------------------------------------------------------
 * Function: Col_HashMapUnset
 *
 *	Remove any value mapped to the given key.
 *
 * Arguments:
 *	map	- Hash map to remove entry from.
 *	key	- Entry key. Can be any word type, including string, however it
 *		  must match the actual type used by the map.
 *
 * Type checking:
 *	*map* must be a valid string or custom hash map.
 *
 * Result:
 *	Whether an existing entry was removed.
 *---------------------------------------------------------------------------*/

int
Col_HashMapUnset(
    Col_Word map,
    Col_Word key)
{
    Col_HashProc *hashProc;
    Col_CompareProc *compareProc;
    Col_Word *buckets, prev, entry;
    size_t nbBuckets;
    uintptr_t hash, index;

    /*
     * Check preconditions.
     */

    TYPECHECK_WORDHASHMAP(map) return 0;

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
	hashProc = HashString;
	compareProc = CompareStrings;
	break;
	
    case WORD_TYPE_CUSTOM: {
	Col_CustomHashMapType *typeInfo 
		= (Col_CustomHashMapType *) WORD_TYPEINFO(map);
	ASSERT(typeInfo->type.type == COL_HASHMAP);
	hashProc = typeInfo->hashProc;
	compareProc = typeInfo->compareProc;
	break;
	}

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
    }

    /*
     * Search for matching entry.
     *
     * Note: bucket size is always a power of 2.
     */

    GET_BUCKETS(map, 0, nbBuckets, buckets);
    ASSERT(!(nbBuckets & (nbBuckets-1)));
    hash = hashProc(key);
    index = hash & (nbBuckets-1);
    prev = WORD_NIL;
    entry = buckets[index];
    while (entry) {
	if (WORD_HASHENTRY_HASH(entry) == (hash & HASHENTRY_HASH_MASK)
		&& compareProc(WORD_MAPENTRY_KEY(entry), key) == 0) {
	    /*
	     * Found! Unlink & remove entry.
	     */

	    switch (WORD_TYPE(prev)) {
	    case WORD_TYPE_NIL:
		/*
		 * Removed entry is first in bucket, ensure it is mutable 
		 * before pointing to next entry.
		 */

		GET_BUCKETS(map, 1, nbBuckets, buckets);
		buckets[index] = WORD_HASHENTRY_NEXT(entry);
		break;

	    case WORD_TYPE_HASHENTRY:
		/*
		 * Previous entry is immutable, convert first.
		 */

		GET_BUCKETS(map, 1, nbBuckets, buckets);
		prev = ConvertEntryToMutable(prev, &buckets[index]);
		ASSERT(WORD_TYPE(prev) == WORD_TYPE_MHASHENTRY);
		/* continued. */

	    case WORD_TYPE_MHASHENTRY:
		/*
		 * Skip removed entry.
		 */

		WORD_HASHENTRY_NEXT(prev) = WORD_HASHENTRY_NEXT(entry);
	    }
	    WORD_HASHMAP_SIZE(map)--;
	    return 1;
	}

	prev = entry;
	entry = WORD_HASHENTRY_NEXT(entry);
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
 * Type checking:
 *	*map* must be a valid integer hash map.
 *
 * Result:
 *	Whether an existing entry was removed.
 *---------------------------------------------------------------------------*/

int
Col_IntHashMapUnset(
    Col_Word map,
    intptr_t key)
{
    Col_Word *buckets, prev, entry;
    size_t nbBuckets;
    uintptr_t index;

    /*
     * Check preconditions.
     */

    TYPECHECK_INTHASHMAP(map) return 0;

    /*
     * Search for matching entry.
     *
     * Note: bucket size is always a power of 2.
     */

    GET_BUCKETS(map, 1, nbBuckets, buckets);
    ASSERT(!(nbBuckets & (nbBuckets-1)));
    index = RANDOMIZE_KEY(key) & (nbBuckets-1);
    prev = WORD_NIL;
    entry = buckets[index];
    while (entry) {
	if (WORD_INTMAPENTRY_KEY(entry) == key) {
	    /*
	     * Found! Unlink & remove entry.
	     */

	    switch (WORD_TYPE(prev)) {
	    case WORD_TYPE_NIL:
		/*
		 * Removed entry is first in bucket, ensure it is mutable 
		 * before pointing to next entry.
		 */

		GET_BUCKETS(map, 1, nbBuckets, buckets);
		buckets[index] = WORD_HASHENTRY_NEXT(entry);
		break;

	    case WORD_TYPE_INTHASHENTRY:
		/*
		 * Previous entry is immutable, convert first.
		 */

		GET_BUCKETS(map, 1, nbBuckets, buckets);
		prev = ConvertIntEntryToMutable(prev, &buckets[index]);
		ASSERT(WORD_TYPE(prev) == WORD_TYPE_MHASHENTRY);
		/* continued. */

	    case WORD_TYPE_MINTHASHENTRY:
		/*
		 * Skip removed entry.
		 */

		WORD_HASHENTRY_NEXT(prev) = WORD_HASHENTRY_NEXT(entry);
	    }
	    WORD_HASHMAP_SIZE(map)--;
	    return 1;
	}

	prev = entry;
	entry = WORD_HASHENTRY_NEXT(entry);
    }

    /*
     * Not found.
     */

    return 0;
}


/****************************************************************************
 * Group: Hash Map Iteration
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_HashMapIterBegin
 *
 *	Initialize the hash map iterator so that it points to the first entry
 *	within the map.
 *
 * Arguments:
 *	it	- Iterator to initialize.
 *	map	- Hash map to iterate over.
 *
 * Type checking:
 *	*map* must be a valid hash map.
 *---------------------------------------------------------------------------*/

void
Col_HashMapIterBegin(
    Col_MapIterator it,
    Col_Word map)
{
    Col_Word *buckets;
    size_t nbBuckets, i;

    /*
     * Check preconditions.
     */

    TYPECHECK_HASHMAP(map) {
	Col_MapIterSetNull(it);
	return;
    }

    it->map = map;
    if (Col_MapSize(map) == 0) {
	/*
	 * Map is empty.
	 */

	it->entry = WORD_NIL;
	return;
    }

    /*
     * Get first entry in first nonempty bucket.
     */

    GET_BUCKETS(map, 0, nbBuckets, buckets);
    for (i=0; i < nbBuckets; i++) {
	if (buckets[i]) {
	    it->entry = buckets[i];
	    it->traversal.hash.bucket = i;
	    return;
	}
    }

    /* CANTHAPPEN */
    ASSERT(0);
    Col_MapIterSetNull(it);
}

/*---------------------------------------------------------------------------
 * Function: Col_HashMapIterFind
 *
 *	Initialize the hash map iterator so that it points to the entry with
 *	the given key within the map.
 *
 * Arguments:
 *	it		- Iterator to initialize.
 *	map		- Hash map to find or create entry into.
 *	key		- Entry key. Can be any word type, including string,
 *			  however it must match the actual type used by the map.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *
 * Type checking:
 *	*map* must be a valid string or custom hash map.
 *
 * Result:
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *---------------------------------------------------------------------------*/

void
Col_HashMapIterFind(
    Col_MapIterator it,
    Col_Word map, 
    Col_Word key, 
    int *createPtr)
{
    Col_HashProc *hashProc;
    Col_CompareProc *compareProc;

    /*
     * Check preconditions.
     */

    TYPECHECK_WORDHASHMAP(map) {
	Col_MapIterSetNull(it);
	return;
    }

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
	hashProc = HashString;
	compareProc = CompareStrings;
	break;
	
    case WORD_TYPE_CUSTOM: {
	Col_CustomHashMapType *typeInfo 
		= (Col_CustomHashMapType *) WORD_TYPEINFO(map);
	ASSERT(typeInfo->type.type == COL_HASHMAP);
	hashProc = typeInfo->hashProc;
	compareProc = typeInfo->compareProc;
	break;
	}
    }

    it->map = map;
    it->entry = HashMapFindEntry(map, hashProc, compareProc, key, 0, createPtr,
	    &it->traversal.hash.bucket);
}

/*---------------------------------------------------------------------------
 * Function: Col_IntHashMapIterFind
 *
 *	Initialize the hash map iterator so that it points to the entry with
 *	the given integer key within the map.
 *
 * Arguments:
 *	it		- Iterator to initialize.
 *	map		- Integer hash map to find or create entry into.
 *	key		- Integer entry key.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *
 * Type checking:
 *	*map* must be a valid integer hash map.
 *
 * Result:
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *---------------------------------------------------------------------------*/

void
Col_IntHashMapIterFind(
    Col_MapIterator it,
    Col_Word map, 
    intptr_t key, 
    int *createPtr)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_INTHASHMAP(map) {
	Col_MapIterSetNull(it);
	return;
    }

    it->map = map;
    it->entry = IntHashMapFindEntry(map, key, 0, createPtr, 
	    &it->traversal.hash.bucket);
}

/*---------------------------------------------------------------------------
 * Function: Col_HashMapIterSetValue
 *
 *	Set value of hash map iterator.
 *
 * Arguments:
 *	it	- Map iterator to set value for.
 *	value	- Value to set.
 *
 * Type checking:
 *	*it* must be a valid hash map iterator.
 *
 * Range checking:
 *	*it* must not be at end.
 *---------------------------------------------------------------------------*/

void
Col_HashMapIterSetValue(
    Col_MapIterator it,
    Col_Word value)
{
    Col_Word *buckets;
    size_t nbBuckets;
    int type = WORD_TYPE(it->map);

    /*
     * Check preconditions.
     */

    TYPECHECK_HASHMAP(it->map) return;
    RANGECHECK_MAPITER(it) return;

    ASSERT(it->entry);

    switch (WORD_TYPE(it->map)) {
    case WORD_TYPE_STRHASHMAP:
    case WORD_TYPE_CUSTOM:
	if (WORD_TYPE(it->entry) != WORD_TYPE_MHASHENTRY) {
	    /*
	     * Entry is immutable, convert to mutable.
	     */

	    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_HASHENTRY);
	    GET_BUCKETS(it->map, 1, nbBuckets, buckets);
	    it->entry = ConvertEntryToMutable(it->entry, 
		    &buckets[it->traversal.hash.bucket]);
	}

	/*
	 * Set entry value.
	 */

	ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_MHASHENTRY);
	WORD_MAPENTRY_VALUE(it->entry) = value;
	break;

    case WORD_TYPE_INTHASHMAP:
	if (WORD_TYPE(it->entry) != WORD_TYPE_MINTHASHENTRY) {
	    /*
	     * Entry is immutable, convert to mutable.
	     */

	    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_INTHASHENTRY);
	    GET_BUCKETS(it->map, 1, nbBuckets, buckets);
	    it->entry = ConvertIntEntryToMutable(it->entry, 
		    &buckets[it->traversal.hash.bucket]);
	}

	/*
	 * Set entry value.
	 */

	ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_MINTHASHENTRY);
	WORD_MAPENTRY_VALUE(it->entry) = value;
	break;

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_HashMapIterNext
 *
 *	Move the iterator to the next element.
 *
 * Argument:
 *	it	- The iterator to move.
 *
 * Type checking:
 *	*it* must be a valid hash map iterator.
 *
 * Range checking:
 *	*it* must not be at end.
 *---------------------------------------------------------------------------*/

void
Col_HashMapIterNext(
    Col_MapIterator it)
{
    Col_Word *buckets;
    size_t nbBuckets, i;

    /*
     * Check preconditions.
     */

    TYPECHECK_HASHMAP(it->map) return;
    RANGECHECK_MAPITER(it) return;

    ASSERT(it->entry);

    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_HASHENTRY 
	    || WORD_TYPE(it->entry) == WORD_TYPE_MHASHENTRY
	    || WORD_TYPE(it->entry) == WORD_TYPE_INTHASHENTRY
	    || WORD_TYPE(it->entry) == WORD_TYPE_MINTHASHENTRY);

    /*
     * Get next entry in bucket.
     */

    it->entry = WORD_HASHENTRY_NEXT(it->entry);
    if (it->entry) {
	return;
    }

    /*
     * End of bucket, get first entry in next nonempty bucket.
     */

    GET_BUCKETS(it->map, 0, nbBuckets, buckets);
    for (i=it->traversal.hash.bucket+1; i < nbBuckets; i++) {
	if (buckets[i]) {
	    it->entry = buckets[i];
	    it->traversal.hash.bucket = i;
	    return;
	}
    }

    /*
     * End of map.
     */

    it->entry = WORD_NIL;
}


/*
================================================================================
Section: Custom Hash Maps
================================================================================
*/

/****************************************************************************
 * Group: Custom Hash Map Creation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_NewCustomHashMap
 *
 *	Create a new custom hash map word.
 *
 * Arguments:
 *	type		- The hash map word type.
 *	capacity	- Initial bucket size. Rounded up to the next power of
 *			  2.
 *	size		- Size of custom data.
 *	dataPtr		- Will hold a pointer to the allocated data.
 *
 * Result:
 *	A new custom hash map word of the given size. Additionally:
 *
 *	dataPtr	- Points to the allocated custom data.
 *
 * Side effects:
 *	Allocates memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewCustomHashMap(
    Col_CustomHashMapType *type,
    size_t capacity,
    size_t size,
    void **dataPtr)
{
    Col_Word map;
    
    ASSERT(type->type.type == COL_HASHMAP);

    map = (Col_Word) AllocCells(WORD_CUSTOM_SIZE(&type->type, 
	    CUSTOMHASHMAP_HEADER_SIZE, size));
    WORD_HASHMAP_INIT(map, type);
    AllocBuckets(map, capacity);

    return map;
}
