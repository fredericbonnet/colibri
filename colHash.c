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
static void		AllocBuckets(Col_Word map, size_t capacity);
static int		GrowHashMap(Col_Word map, Col_HashProc proc);
static int		GrowIntHashMap(Col_Word map);
static Col_Word		ConvertEntryToMutable(Col_Word entry, 
			    Col_Word *prevPtr);
static Col_Word		ConvertIntEntryToMutable(Col_Word entry, 
			    Col_Word *prevPtr);
static Col_Word		StringHashMapFindEntry(Col_Word map, Col_Word key,
			    int mutable, int *createPtr, size_t *bucketPtr);
static Col_Word		IntHashMapFindEntry(Col_Word map, intptr_t key,
			    int mutable, int *createPtr, size_t *bucketPtr);


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

/****************************************************************************
 * Group: Hash Map Creation
 ****************************************************************************/

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
 *	bucketParent	- Parent word of bucket container. Used during 
 *			  modification (see <Col_WordSetModified>).
 *---------------------------------------------------------------------------*/

#define GET_BUCKETS(map, mutable, nbBuckets, buckets, bucketParent) \
    switch (WORD_TYPE(WORD_HASHMAP_BUCKETS(map))) { \
    case WORD_TYPE_VECTOR: \
	if (mutable) { \
	    WORD_HASHMAP_BUCKETS(map) = Col_NewMVector(0, \
		WORD_VECTOR_LENGTH(WORD_HASHMAP_BUCKETS(map)), \
		WORD_VECTOR_ELEMENTS(WORD_HASHMAP_BUCKETS(map))); \
	    Col_WordSetModified(map); \
	}; \
	/* continued. */ \
    case WORD_TYPE_MVECTOR: \
	(nbBuckets) = WORD_VECTOR_LENGTH(WORD_HASHMAP_BUCKETS(map)); \
	(buckets) = WORD_VECTOR_ELEMENTS(WORD_HASHMAP_BUCKETS(map)); \
	(bucketParent) = WORD_HASHMAP_BUCKETS(map); \
	break; \
    \
    default: \
	ASSERT(!WORD_HASHMAP_BUCKETS(map)); \
	(nbBuckets) = HASHMAP_STATICBUCKETS_SIZE; \
	(buckets) = WORD_HASHMAP_STATICBUCKETS(map); \
	(bucketParent) = map; \
    }

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
    Col_Word *buckets, *newBuckets, newBucketContainer, entry, next, dummy;
    size_t nbBuckets, newNbBuckets;
    uintptr_t hash, index, newIndex;
    Col_Word key;

    GET_BUCKETS(map, 0, nbBuckets, buckets, dummy);
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
	    if (nbBuckets & ~MAPENTRY_HASH_MASK) {
		/*
		 * Recompute full hash.
		 */

		hash = proc(key);
	    } else {
		/*
		 * Get actual hash by combining entry hash and index.
		 */

		hash = WORD_MAPENTRY_HASH(entry) | index;
		ASSERT(hash == proc(key));
	    }
	    newIndex = hash & (newNbBuckets-1);

	    switch (WORD_TYPE(entry)) {
		case WORD_TYPE_MAPENTRY:
		    if (!WORD_MAPENTRY_NEXT(entry) && !newBuckets[newIndex]) {
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

		case WORD_TYPE_MMAPENTRY:
		    /*
		     * Move entry at head of new bucket.
		     */

		    next = WORD_MAPENTRY_NEXT(entry);
		    WORD_MAPENTRY_NEXT(entry) = newBuckets[newIndex];
		    newBuckets[newIndex] = entry;
		    Col_WordSetModified(entry);
	    }

	    entry = next;
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
    Col_Word *buckets, *newBuckets, newBucketContainer, entry, dummy, next;
    size_t nbBuckets, newNbBuckets;
    uintptr_t index, newIndex;
    intptr_t key;

    GET_BUCKETS(map, 0, nbBuckets, buckets, dummy);
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
		case WORD_TYPE_INTMAPENTRY:
		    if (!WORD_MAPENTRY_NEXT(entry) && !newBuckets[newIndex]) {
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

		case WORD_TYPE_MINTMAPENTRY:
		    /*
		     * Move entry at head of new bucket.
		     */

		    next = WORD_MAPENTRY_NEXT(entry);
		    WORD_MAPENTRY_NEXT(entry) = newBuckets[newIndex];
		    newBuckets[newIndex] = entry;
		    Col_WordSetModified(entry);
	    }

	    entry = next;
	}
    }

    WORD_HASHMAP_BUCKETS(map) = newBucketContainer;
    Col_WordSetModified(map);

    return 1;
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
    Col_Word entry, *buckets, dummy;
    size_t nbBuckets, i;

    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRHASHMAP:
	case WORD_TYPE_INTHASHMAP:
	    /*
	     * Copy word first.
	     */

	    newMap = (Col_Word) AllocCells(HASHMAP_NBCELLS);
	    memcpy((void *) newMap, (void *) map, sizeof(Cell) 
		    * HASHMAP_NBCELLS);
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

	    GET_BUCKETS(map, 0, nbBuckets, buckets, dummy);
	    for (i=0; i < nbBuckets; i++) {
		entry = buckets[i];
		while (entry) {
		    ASSERT(!WORD_PINNED(entry));
		    switch (WORD_TYPE(entry)) {
			case WORD_TYPE_MMAPENTRY:
			    WORD_SET_TYPEID(entry, WORD_TYPE_MAPENTRY);
			    entry = WORD_MAPENTRY_NEXT(entry);
			    break;

			case WORD_TYPE_MINTMAPENTRY:
			    WORD_SET_TYPEID(entry, WORD_TYPE_INTMAPENTRY);
			    entry = WORD_MAPENTRY_NEXT(entry);
			    break;

			default:
			    /*
			     * Already frozen, skip remaining entries.
			     */

			    entry = WORD_NIL;
		    }
		}
	    }

	    /*
	     * Both maps now share the same immutable structure.
	     */

	    return newMap;

	default:
	    Col_Error(COL_ERROR, "%x is not a hash map", map);
	    return WORD_NIL;
    }
}


/****************************************************************************
 * Internal Group: Hash Map Entries
 ****************************************************************************/

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

    ASSERT(WORD_TYPE(entry) == WORD_TYPE_MAPENTRY);
    for (;;) {
	int last = (*prevPtr == entry);

	ASSERT(WORD_TYPE(*prevPtr) == WORD_TYPE_MAPENTRY);

	/*
	 * Convert entry: copy then change type ID.
	 */

	converted = (Col_Word) AllocCells(1);
	memcpy((void *) converted, (void *) *prevPtr, sizeof(Cell));
	WORD_SET_TYPEID(converted, WORD_TYPE_MMAPENTRY);
	*prevPtr = converted;

	if (last) return converted;

	prevPtr = &WORD_MAPENTRY_NEXT(*prevPtr);
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

    ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTMAPENTRY);
    for (;;) {
	int last = (*prevPtr == entry);

	ASSERT(WORD_TYPE(*prevPtr) == WORD_TYPE_INTMAPENTRY);

	/*
	 * Convert entry: copy then change type ID.
	 */

	converted = (Col_Word) AllocCells(1);
	memcpy((void *) converted, (void *) *prevPtr, sizeof(Cell));
	WORD_SET_TYPEID(converted, WORD_TYPE_MINTMAPENTRY);
	*prevPtr = converted;

	if (last) return converted;

	prevPtr = &WORD_MAPENTRY_NEXT(*prevPtr);
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: StringHashMapFindEntry
 *
 *	Find or create in string hash map the entry mapped to the given key.
 *
 * Arguments:
 *	map		- String hash map to find or create entry into.
 *	key		- String entry key.
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
StringHashMapFindEntry(
    Col_Word map,
    Col_Word key,
    int mutable,
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

    GET_BUCKETS(map, 0, nbBuckets, buckets, bucketParent);
    ASSERT(!(nbBuckets & (nbBuckets-1)));
    hash = HashString(key);
    index = hash & (nbBuckets-1);
    entry = buckets[index];
    while (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_MAPENTRY || WORD_TYPE(entry) == WORD_TYPE_MMAPENTRY);
	if (WORD_MAPENTRY_HASH(entry) == (hash & MAPENTRY_HASH_MASK)
		&& Col_CompareRopes(WORD_MAPENTRY_KEY(entry), key, 0, SIZE_MAX,
		NULL, NULL, NULL) == 0) {
	    /*
	     * Found!
	     */

	    if (createPtr) *createPtr = 0;
	    if (bucketPtr) *bucketPtr = index;

	    if (mutable && WORD_TYPE(entry) != WORD_TYPE_MMAPENTRY) {
		/*
		 * Entry is immutable, convert.
		 */

		GET_BUCKETS(map, 1, nbBuckets, buckets, bucketParent);
		entry = ConvertEntryToMutable(entry, &buckets[index]);
		ASSERT(WORD_TYPE(entry) == WORD_TYPE_MMAPENTRY);
	    }
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
    ASSERT(mutable);
    *createPtr = 1;

    if (WORD_HASHMAP_SIZE(map) >= nbBuckets * LOAD_FACTOR
	    && GrowHashMap(map, HashString)) {
	/*
	 * Grow map and insert again.
	 */

	return StringHashMapFindEntry(map, key, mutable, createPtr, bucketPtr);
    }

    /*
     * Insert a new entry at head so that subsequent lookups are faster. 
     * Moreover this leaves potentially immutable existing entries untouched.
     */

    GET_BUCKETS(map, 1, nbBuckets, buckets, bucketParent);
    entry = (Col_Word) AllocCells(1);
    WORD_MMAPENTRY_INIT(entry, buckets[index], key, WORD_NIL, hash);
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
    Col_Word *buckets, bucketParent, entry;
    size_t nbBuckets;
    uintptr_t index;

    /*
     * Search for matching entry.
     *
     * Note: bucket size is always a power of 2.
     */

    GET_BUCKETS(map, 0, nbBuckets, buckets, bucketParent);
    ASSERT(!(nbBuckets & (nbBuckets-1)));
    index = RANDOMIZE_KEY(key) & (nbBuckets-1);
    entry = buckets[index];
    while (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTMAPENTRY || WORD_TYPE(entry) == WORD_TYPE_MINTMAPENTRY);
	if (WORD_INTMAPENTRY_KEY(entry) == key) {
	    /*
	     * Found!
	     */

	    if (createPtr) *createPtr = 0;
	    if (bucketPtr) *bucketPtr = index;

	    if (mutable && WORD_TYPE(entry) != WORD_TYPE_MINTMAPENTRY) {
		/*
		 * Entry is immutable, convert.
		 */

		GET_BUCKETS(map, 1, nbBuckets, buckets, bucketParent);
		entry = ConvertIntEntryToMutable(entry, &buckets[index]);
		ASSERT(WORD_TYPE(entry) == WORD_TYPE_MINTMAPENTRY);
	    }
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
    ASSERT(mutable);
    *createPtr = 1;

    if (WORD_HASHMAP_SIZE(map) >= nbBuckets * LOAD_FACTOR
	    && GrowIntHashMap(map)) {
	/*
	 * Grow map and insert again.
	 */

	return IntHashMapFindEntry(map, key, mutable, createPtr, bucketPtr);
    }

    /*
     * Insert a new entry at head so that subsequent lookups are faster. 
     * Moreover this leaves potentially immutable existing entries untouched.
     */

    GET_BUCKETS(map, 1, nbBuckets, buckets, bucketParent);
    entry = (Col_Word) AllocCells(1);
    WORD_MINTMAPENTRY_INIT(entry, buckets[index], key, WORD_NIL);
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

    entry = StringHashMapFindEntry(map, key, 0, NULL, NULL);

    if (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_MMAPENTRY);
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

    entry = IntHashMapFindEntry(map, key, 0, NULL, NULL);
    if (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_MINTMAPENTRY);
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

    entry = StringHashMapFindEntry(map, key, 1, &create, NULL);
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_MMAPENTRY);
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

    entry = IntHashMapFindEntry(map, key, 1, &create, NULL);
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_MINTMAPENTRY);
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
    Col_Word *buckets, parent, prev, entry;
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

    GET_BUCKETS(map, 0, nbBuckets, buckets, parent);
    ASSERT(!(nbBuckets & (nbBuckets-1)));
    hash = HashString(key);
    index = hash & (nbBuckets-1);
    prev = WORD_NIL;
    entry = buckets[index];
    while (entry) {
	if (WORD_MAPENTRY_HASH(entry) == (hash & MAPENTRY_HASH_MASK)
		&& Col_CompareRopes(WORD_MAPENTRY_KEY(entry), key, 0, SIZE_MAX,
		NULL, NULL, NULL) == 0) {
	    /*
	     * Found! Unlink & remove entry.
	     */

	    switch (WORD_TYPE(prev)) {
		case WORD_TYPE_NIL:
		    /*
		     * Removed entry is first in bucket, ensure it is mutable 
		     * before pointing to next entry.
		     */

		    GET_BUCKETS(map, 1, nbBuckets, buckets, parent);
		    buckets[index] = WORD_MAPENTRY_NEXT(entry);
		    Col_WordSetModified(parent);
		    break;

		case WORD_TYPE_MAPENTRY:
		    /*
		     * Previous entry is immutable, convert first.
		     */

		    GET_BUCKETS(map, 1, nbBuckets, buckets, parent);
		    prev = ConvertEntryToMutable(prev, &buckets[index]);
		    ASSERT(WORD_TYPE(prev) == WORD_TYPE_MMAPENTRY);
		    /* continued. */

		case WORD_TYPE_MMAPENTRY:
		    /*
		     * Skip removed entry.
		     */

		    WORD_MAPENTRY_NEXT(prev) = WORD_MAPENTRY_NEXT(entry);
		    Col_WordSetModified(prev);
	    }
	    WORD_HASHMAP_SIZE(map)--;
	    return 1;
	}

	prev = entry;
	entry = WORD_MAPENTRY_NEXT(entry);
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
    Col_Word *buckets, parent, prev, entry;
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

    GET_BUCKETS(map, 1, nbBuckets, buckets, parent);
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

		    GET_BUCKETS(map, 1, nbBuckets, buckets, parent);
		    buckets[index] = WORD_MAPENTRY_NEXT(entry);
		    Col_WordSetModified(parent);
		    break;

		case WORD_TYPE_INTMAPENTRY:
		    /*
		     * Previous entry is immutable, convert first.
		     */

		    GET_BUCKETS(map, 1, nbBuckets, buckets, parent);
		    prev = ConvertIntEntryToMutable(prev, &buckets[index]);
		    ASSERT(WORD_TYPE(prev) == WORD_TYPE_MMAPENTRY);
		    /* continued. */

		case WORD_TYPE_MINTMAPENTRY:
		    /*
		     * Skip removed entry.
		     */

		    WORD_MAPENTRY_NEXT(prev) = WORD_MAPENTRY_NEXT(entry);
		    Col_WordSetModified(prev);
	    }
	    WORD_HASHMAP_SIZE(map)--;
	    return 1;
	}

	prev = entry;
	entry = WORD_MAPENTRY_NEXT(entry);
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
    GET_BUCKETS(map, 0, nbBuckets, buckets, dummy);
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

    it->entry = StringHashMapFindEntry(map, key, 0, createPtr, &it->bucket);
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
 *	the given integer key within the map.
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

    it->entry = IntHashMapFindEntry(map, key, 0, createPtr, &it->bucket);
    if (!it->entry) {
	/*
	 * Not found.
	 */

	it->map = WORD_NIL;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_HashMapIterSetValue
 *
 *	Set value of hash map iterator.
 *
 * Argument:
 *	it	- Map iterator to set value for.
 *	value	- Value to set.
 *---------------------------------------------------------------------------*/

void
Col_HashMapIterSetValue(
    Col_MapIterator *it,
    Col_Word value)
{
    Col_Word *buckets, dummy;
    size_t nbBuckets;

    if (Col_MapIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid map iterator");
	return;
    }

    switch (WORD_TYPE(it->map)) {
	case WORD_TYPE_STRHASHMAP:
	    if (WORD_TYPE(it->entry) != WORD_TYPE_MMAPENTRY) {
		/*
		 * Entry is immutable, convert to mutable.
		 */

		ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_MAPENTRY);
		GET_BUCKETS(it->map, 1, nbBuckets, buckets, dummy);
		it->entry = ConvertEntryToMutable(it->entry, 
			&buckets[it->bucket]);
	    }

	    /*
	     * Set entry value.
	     */

	    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_MMAPENTRY);
	    WORD_MAPENTRY_VALUE(it->entry) = value;
	    Col_WordSetModified(it->entry);
	    break;

	case WORD_TYPE_INTHASHMAP:
	    if (WORD_TYPE(it->entry) != WORD_TYPE_MINTMAPENTRY) {
		/*
		 * Entry is immutable, convert to mutable.
		 */

		ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_INTMAPENTRY);
		GET_BUCKETS(it->map, 1, nbBuckets, buckets, dummy);
		it->entry = ConvertIntEntryToMutable(it->entry, 
			&buckets[it->bucket]);
	    }

	    /*
	     * Set entry value.
	     */

	    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_MINTMAPENTRY);
	    WORD_MAPENTRY_VALUE(it->entry) = value;
	    Col_WordSetModified(it->entry);
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    Col_Error(COL_ERROR, "%x is not a hash map", it->map);
    }
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
	    || WORD_TYPE(it->entry) == WORD_TYPE_MMAPENTRY
	    || WORD_TYPE(it->entry) == WORD_TYPE_INTMAPENTRY
	    || WORD_TYPE(it->entry) == WORD_TYPE_MINTMAPENTRY);

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

    GET_BUCKETS(it->map, 0, nbBuckets, buckets, dummy);
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
