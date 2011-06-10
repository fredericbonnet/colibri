/*
 * File: colTrie.c
 *
 *	This file implements the trie map handling features of Colibri.
 *
 *	Trie maps are an implementation of generic maps that uses crit-bit trees
 *	for string and integer keys.
 *
 *	They are always mutable.
 *
 * See also:
 *	<colTrie.h>
 */

#include "colibri.h"
#include "colInternal.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>


/*
 * Prototypes for functions used only in this file.
 */

static Col_Word		LeftmostLeaf(Col_Word node, Col_Word *rightPtr);
static Col_Word		RightmostLeaf(Col_Word node, Col_Word *leftPtr);
static Col_Word		StringTrieMapFindNode(Col_Word map, Col_Word key,
			    int closest, int *comparePtr, Col_Word *parentPtr,
			    Col_Word *leftPtr, Col_Word *rightPtr, 
			    size_t *diffPtr, Col_Char *maskPtr);
static Col_Word		IntTrieMapFindNode(Col_Word map, intptr_t key,
			    int closest, int *comparePtr, Col_Word *parentPtr,
			    Col_Word *leftPtr, Col_Word *rightPtr, 
			    intptr_t *maskPtr);
static Col_Word		StringTrieMapFindEntry(Col_Word map, Col_Word key,
			    int mutable, int *createPtr, Col_Word *leftPtr, 
			    Col_Word *rightPtr);
static Col_Word		IntTrieMapFindEntry(Col_Word map, intptr_t key,
			    int mutable, int *createPtr, Col_Word *leftPtr, 
			    Col_Word *rightPtr);


/****************************************************************************
 * Group: Trie Map Creation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_NewStringTrieMap
 *
 *	Create a new string trie map word.
 *
 * Result:
 *	The new word.
 *
 * Side effects:
 *	Allocates memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewStringTrieMap()
{
    Col_Word map;
    
    map = (Col_Word) AllocCells(1);
    WORD_STRTRIEMAP_INIT(map);

    return map;
}

/*---------------------------------------------------------------------------
 * Function: Col_NewIntTrieMap
 *
 *	Create a new integer trie map word.
 *
 * Result:
 *	The new word.
 *
 * Side effects:
 *	Allocates memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewIntTrieMap()
{
    Col_Word map;
    
    map = (Col_Word) AllocCells(1);
    WORD_INTTRIEMAP_INIT(map);

    return map;
}


/****************************************************************************
 * Group: Trie Map Entries
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Function: LeftmostLeaf
 *
 *	Get leftmost leaf in subtrie.
 *
 * Argument:
 *	node	- Root node of subtrie.
 *
 * Results:
 *	The leftmost leaf of subtrie. Additionally (if non-NULL):
 *
 *	rightPtr	- (out) Adjacent right branch.
 *---------------------------------------------------------------------------*/

static Col_Word
LeftmostLeaf(
     Col_Word node,
     Col_Word *rightPtr) 
{
    Col_Word right = WORD_NIL;

    for (;;) {
	switch (WORD_TYPE(node)) {
	    case WORD_TYPE_STRTRIENODE:
	    case WORD_TYPE_MSTRTRIENODE:
	    case WORD_TYPE_INTTRIENODE:
	    case WORD_TYPE_MINTTRIENODE:
		right = WORD_TRIENODE_RIGHT(node);
		node = WORD_TRIENODE_LEFT(node);
		break;

	    default:
		if (rightPtr) *rightPtr = right;
		return node;
	}
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: RightmostLeaf
 *
 *	Get rightmost leaf in subtrie.
 *
 * Argument:
 *	node	- Root node of subtrie.
 *
 * Results:
 *	The rightmost leaf of subtrie. Additionally (if non-NULL):
 *
 *	leftPtr		- (out) Adjacent left branch.
 *---------------------------------------------------------------------------*/

static Col_Word
RightmostLeaf(
     Col_Word node,
     Col_Word *leftPtr) 
{
    Col_Word left = WORD_NIL;

    for (;;) {
	switch (WORD_TYPE(node)) {
	    case WORD_TYPE_STRTRIENODE:
	    case WORD_TYPE_MSTRTRIENODE:
	    case WORD_TYPE_INTTRIENODE:
	    case WORD_TYPE_MINTTRIENODE:
		node = WORD_TRIENODE_RIGHT(node);
		break;

	    default:
		if (leftPtr) *leftPtr = left;
		return node;
	}
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: StringTrieMapFindNode
 *
 *	Find node equal or closest to the given key.
 *
 * Arguments:
 *	map	- Integer trie map to find or create entry into.
 *	key	- String entry key.
 *	closest	- If true, find closest if key doesn't match.
 *
 * Results:
 *	If map is empty, nil.
 *
 *	If key is present, the matching entry (i.e. leaf). Else, if closest is
 *	true, the node with the longest common prefix. Else nil. Additionally 
 *	(if non-NULL):
 *
 *	comparePtr	- (out) Key comparison result: 0 if node matches key, 1
 *			  if key is after node key, -1 if before.
 *	parentPtr	- (out) The node's parent word. May be the map or 
 *			  another node.
 *	leftPtr		- (out) Adjacent left branch.
 *	rightPtr	- (out) Adjacent right branch.
 *
 *	Moreover (if non-NULL), if key doesn't match and closest is true:
 *
 *	diffPtr		- (out) Index of first differing character.
 *	maskPtr		- (out) Crit-bit mask.
 *---------------------------------------------------------------------------*/

static Col_Word
StringTrieMapFindNode(
    Col_Word map,
    Col_Word key,
    int closest,
    int *comparePtr,
    Col_Word *parentPtr,
    Col_Word *leftPtr,
    Col_Word *rightPtr,
    size_t *diffPtr,
    Col_Char *maskPtr)
{
    int compare;
    Col_Word node, parent, left, right;
    Col_Word entryKey;
    Col_RopeIterator itKey;
    size_t diff;
    Col_Char mask, newMask;
    Col_Char cKey, cEntryKey;
    
    if (!WORD_TRIEMAP_ROOT(map)) {
	/*
	 * Map is empty.
	 */

	ASSERT(WORD_TRIEMAP_SIZE(map) == 0);
	return WORD_NIL;
    }

    /*
     * Search for matching entry.
     */

    node = WORD_TRIEMAP_ROOT(map);
    parent = map;
    left = WORD_NIL;
    right = WORD_NIL;
    Col_RopeIterBegin(key, 0, &itKey);
    while (node) {
	if (WORD_TYPE(node) == WORD_TYPE_MTRIELEAF) break;

	ASSERT(WORD_TYPE(node) == WORD_TYPE_MSTRTRIENODE);
	mask = WORD_STRTRIENODE_MASK(node);
	cKey = COL_CHAR_INVALID;
	if (!Col_RopeIterEnd(&itKey)) {
	    Col_RopeIterMoveTo(&itKey, WORD_STRTRIENODE_DIFF(node));
	    if (!Col_RopeIterEnd(&itKey)) {
		cKey = Col_RopeIterAt(&itKey);
	    }
	}
	parent = node;
	if (cKey != COL_CHAR_INVALID && (!mask || (cKey & mask))) {
	    left = WORD_TRIENODE_LEFT(node);
	    node = WORD_TRIENODE_RIGHT(node);
	} else {
	    right = WORD_TRIENODE_RIGHT(node);
	    node = WORD_TRIENODE_LEFT(node);
	}
    }

    ASSERT(WORD_TYPE(node) == WORD_TYPE_MTRIELEAF);
    entryKey = WORD_MAPENTRY_KEY(node);
    compare = Col_CompareRopes(key, entryKey, 0, SIZE_MAX, &diff, &cKey, 
	    &cEntryKey);
    if (compare == 0 || !closest) {
	/*
	 * Exact match found or required.
	 */

	if (comparePtr) *comparePtr = compare;
	if (parentPtr) *parentPtr = parent;
	if (leftPtr) *leftPtr = left;
	if (rightPtr) *rightPtr = right;
	return (compare == 0 ? node : WORD_NIL);
    }

    /*
     * Get diff mask between inserted key and existing entry key, i.e. only keep 
     * the highest bit set.
     * See: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2 
     *
     * Note: when either char is COL_CHAR_INVALID, they differ on the highest
     * bit, so the penultimate step overflows and the mask is zero. This
     * happens when one of the keys is shorter. We use this property when 
     * choosing the right arm to follow.
     */

    newMask = cKey ^ cEntryKey;
    newMask |= newMask >> 1;
    newMask |= newMask >> 2;
    newMask |= newMask >> 4;
    newMask |= newMask >> 8;
    newMask |= newMask >> 16;
    newMask++;
    newMask >>= 1;

    /*
     * Find longest common prefix.
     */

    node = WORD_TRIEMAP_ROOT(map);
    parent = map;
    left = WORD_NIL;
    right = WORD_NIL;
    Col_RopeIterBegin(key, 0, &itKey);
    while (node) {
	if (WORD_TYPE(node) == WORD_TYPE_MTRIELEAF) break;

	ASSERT(WORD_TYPE(node) == WORD_TYPE_MSTRTRIENODE);
	mask = WORD_STRTRIENODE_MASK(node);
	if (diff < WORD_STRTRIENODE_DIFF(node) 
		|| (diff == WORD_STRTRIENODE_DIFF(node) 
		&& (mask && newMask > mask))) {
	    /*
	     * Prefixes diverge.
	     */

	    break;
	}
	cKey = COL_CHAR_INVALID;
	if (!Col_RopeIterEnd(&itKey)) {
	    Col_RopeIterMoveTo(&itKey, WORD_STRTRIENODE_DIFF(node));
	    if (!Col_RopeIterEnd(&itKey)) {
		cKey = Col_RopeIterAt(&itKey);
	    }
	}

	parent = node;
	if (cKey != COL_CHAR_INVALID && (!mask || (cKey & mask))) {
	    left = WORD_TRIENODE_LEFT(node);
	    node = WORD_TRIENODE_RIGHT(node);
	} else {
	    right = WORD_TRIENODE_RIGHT(node);
	    node = WORD_TRIENODE_LEFT(node);
	}
    }
    if (comparePtr) {
	cKey = COL_CHAR_INVALID;
	if (!Col_RopeIterEnd(&itKey)) {
	    Col_RopeIterMoveTo(&itKey, diff);
	    if (!Col_RopeIterEnd(&itKey)) {
		cKey = Col_RopeIterAt(&itKey);
	    }
	}
	*comparePtr = ((cKey != COL_CHAR_INVALID && (!newMask 
		|| (cKey & newMask))) ? 1 : -1);
    }
    if (parentPtr) *parentPtr = parent;
    if (leftPtr) *leftPtr = left;
    if (rightPtr) *rightPtr = right;
    if (diffPtr) *diffPtr = diff;
    if (maskPtr) *maskPtr = newMask;
    return node;
}

/*---------------------------------------------------------------------------
 * Internal Function: IntTrieMapFindNode
 *
 *	Find node closest to the given key.
 *
 * Arguments:
 *	map	- Integer trie map to find or create entry into.
 *	key	- Integer entry key.
 *	closest	- If true, find closest if key doesn't match.
 *
 * Results:
 *	If map is empty, nil.
 *
 *	If key is present, the matching entry (i.e. leaf). Else, if closest is
 *	true, the node with the longest common prefix. Else nil. Additionally 
 *	(if non-NULL):
 *
 *	comparePtr	- (out) Key comparison result: 0 if node matches key, 1
 *			  if key is after node key, -1 if before.
 *	parentPtr	- (out) The node's parent word. May be the map or 
 *			  another node.
 *	leftPtr		- (out) Adjacent left branch.
 *	rightPtr	- (out) Adjacent right branch.
 *
 *	Moreover (if non-NULL), if key doesn't match and closest is true:
 *
 *	maskPtr		- (out) Crit-bit mask.
 *---------------------------------------------------------------------------*/

static Col_Word
IntTrieMapFindNode(
    Col_Word map,
    intptr_t key,
    int closest,
    int *comparePtr,
    Col_Word *parentPtr,
    Col_Word *leftPtr,
    Col_Word *rightPtr,
    intptr_t *maskPtr)
{
    int compare;
    Col_Word node, parent, left, right;
    intptr_t entryKey;
    intptr_t mask, newMask;
    
    if (!WORD_TRIEMAP_ROOT(map)) {
	/*
	 * Map is empty.
	 */

	ASSERT(WORD_TRIEMAP_SIZE(map) == 0);
	return WORD_NIL;
    }

    /*
     * Search for matching entry.
     */

    node = WORD_TRIEMAP_ROOT(map);
    parent = map;
    left = WORD_NIL;
    right = WORD_NIL;
    while (node) {
	if (WORD_TYPE(node) == WORD_TYPE_MINTTRIELEAF) break;

	ASSERT(WORD_TYPE(node) == WORD_TYPE_MINTTRIENODE);
	mask = WORD_INTTRIENODE_MASK(node);
	parent = node;
	if (mask ? (key & mask) : (key >= 0)) {
	    left = WORD_TRIENODE_LEFT(node);
	    node = WORD_TRIENODE_RIGHT(node);
	} else {
	    right = WORD_TRIENODE_RIGHT(node);
	    node = WORD_TRIENODE_LEFT(node);
	}
    }

    ASSERT(WORD_TYPE(node) == WORD_TYPE_MINTTRIELEAF);
    entryKey = WORD_INTMAPENTRY_KEY(node);
    compare = (key == entryKey ? 0 : key > entryKey ? 1 : -1);
    if (compare == 0 || !closest) {
	/*
	 * Exact match found or required.
	 */

	if (comparePtr) *comparePtr = compare;
	if (parentPtr) *parentPtr = parent;
	if (leftPtr) *leftPtr = left;
	if (rightPtr) *rightPtr = right;
	return (compare == 0 ? node : WORD_NIL);
    }

    /*
     * Get diff mask between inserted key and existing entry key, i.e. only keep 
     * the highest bit set.
     * See: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2 
     *
     * Note: when keys are of opposite signs, they differ on the highest bit,
     * so the penultimate step overflows and the mask is zero. We use this
     * property when choosing the right arm to follow.
     */

    newMask = key ^ entryKey;
    ASSERT(newMask);
    newMask |= newMask >> 1;
    newMask |= newMask >> 2;
    newMask |= newMask >> 4;
    newMask |= newMask >> 8;
    newMask |= newMask >> 16;
    newMask++;
    newMask >>= 1;

    /*
     * Find longest common prefix.
     */

    node = WORD_TRIEMAP_ROOT(map);
    parent = map;
    left = WORD_NIL;
    right = WORD_NIL;
    while (node) {
	if (WORD_TYPE(node) == WORD_TYPE_MINTTRIELEAF) break;

	ASSERT(WORD_TYPE(node) == WORD_TYPE_MINTTRIENODE);
	mask = WORD_INTTRIENODE_MASK(node);
	if (mask && newMask > mask) {
	    /*
	     * Prefixes diverge.
	     */

	    break;
	}

	parent = node;
	if (mask ? (key & mask) : (key >= 0)) {
	    left = WORD_TRIENODE_LEFT(node);
	    node = WORD_TRIENODE_RIGHT(node);
	} else {
	    right = WORD_TRIENODE_RIGHT(node);
	    node = WORD_TRIENODE_LEFT(node);
	}
    }
    if (comparePtr) *comparePtr = ((newMask ? (key & newMask) : (key >= 0)) ? 1
	    : -1);
    if (parentPtr) *parentPtr = parent;
    if (leftPtr) *leftPtr = left;
    if (rightPtr) *rightPtr = right;
    if (maskPtr) *maskPtr = newMask;
    return node;
}

/*---------------------------------------------------------------------------
 * Internal Function: StringTrieMapFindEntry
 *
 *	Find or create in string trie map the entry mapped to the given key.
 *
 * Arguments:
 *	map		- String trie map to find or create entry into.
 *	key		- String entry key.
 *	mutable		- If true, ensure that entry is mutable.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *
 * Results:
 *	The entry if found or created, else nil. Additionally (if non-NULL):
 *
 *	createPtr	- (out) Whether a new entry was created. 
 *	leftPtr		- (out) Adjacent left branch.
 *	rightPtr	- (out) Adjacent right branch.
 *---------------------------------------------------------------------------*/

static Col_Word
StringTrieMapFindEntry(
    Col_Word map,
    Col_Word key,
    int mutable,
    int *createPtr,
    Col_Word *leftPtr,
    Col_Word *rightPtr)
{
    Col_Word node, entry, parent, *nodePtr;
    int compare;
    size_t diff;
    Col_Char mask;
    
    node = StringTrieMapFindNode(map, key, (createPtr && *createPtr), &compare, 
	    &parent, leftPtr, rightPtr, &diff, &mask);
    if (node && compare == 0) {
	/*
	 * Found!
	 */

	if (createPtr) *createPtr = 0;
	return node;
    }

    /*
     * Not found, create new if asked for.
     */

    if (!createPtr || !*createPtr) {
	ASSERT(!node);
	return WORD_NIL;
    }
    *createPtr = 1;

    WORD_TRIEMAP_SIZE(map)++;

    entry = (Col_Word) AllocCells(1);
    WORD_MTRIELEAF_INIT(entry, key, WORD_NIL);

    if (!node) {
	/*
	 * First entry.
	 */

	ASSERT(!WORD_TRIEMAP_ROOT(map));
	ASSERT(WORD_TRIEMAP_SIZE(map) == 1);
	ASSERT(!leftPtr || !*leftPtr);
	ASSERT(!rightPtr || !*rightPtr);
	WORD_TRIEMAP_ROOT(map) = entry;
	Col_WordSetModified(map);

	return entry;
    }
    ASSERT(node);

    /*
     * Insert node here.
     */

    if (parent == map) {
	ASSERT(node == WORD_TRIEMAP_ROOT(map));
	ASSERT(!leftPtr || !*leftPtr);
	ASSERT(!rightPtr || !*rightPtr);
	nodePtr = &WORD_TRIEMAP_ROOT(map);
    } else {
	ASSERT(WORD_TYPE(parent) == WORD_TYPE_MSTRTRIENODE);
	if (node == WORD_TRIENODE_LEFT(parent)) {
	    if (rightPtr) *rightPtr = WORD_TRIENODE_RIGHT(parent);
	    nodePtr = &WORD_TRIENODE_LEFT(parent);
	} else {
	    if (leftPtr) *leftPtr = WORD_TRIENODE_LEFT(parent);
	    nodePtr = &WORD_TRIENODE_RIGHT(parent);
	}
    }

    node = (Col_Word) AllocCells(1);
    if (compare > 0) {
	/*
	 * Entry is right.
	 */

	if (leftPtr) *leftPtr = *nodePtr;
	WORD_MSTRTRIENODE_INIT(node, diff, mask, *nodePtr, entry);
    } else {
	/*
	 * Entry is left.
	 */

	if (rightPtr) *rightPtr = *nodePtr;
	WORD_MSTRTRIENODE_INIT(node, diff, mask, entry, *nodePtr);
    }
    *nodePtr = node;
    Col_WordSetModified(parent);

    return entry;
}

/*---------------------------------------------------------------------------
 * Internal Function: IntTrieMapFindEntry
 *
 *	Find or create in integer trie map the entry mapped to the given key.
 *
 * Arguments:
 *	map		- Integer trie map to find or create entry into.
 *	key		- Integer entry key.
 *	mutable		- If true, ensure that entry is mutable.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *
 * Results:
 *	The entry if found or created, else nil. Additionally (if non-NULL):
 *
 *	createPtr	- (out) Whether a new entry was created. 
 *	leftPtr		- (out) Adjacent left branch.
 *	rightPtr	- (out) Adjacent right branch.
 *---------------------------------------------------------------------------*/

static Col_Word
IntTrieMapFindEntry(
    Col_Word map,
    intptr_t key,
    int mutable,
    int *createPtr,
    Col_Word *leftPtr,
    Col_Word *rightPtr)
{
    Col_Word node, entry, parent, *nodePtr;
    int compare;
    intptr_t mask;
    
    node = IntTrieMapFindNode(map, key, (createPtr && *createPtr), &compare, 
	    &parent, leftPtr, rightPtr, &mask);
    if (node && compare == 0) {
	/*
	 * Found!
	 */

	if (createPtr) *createPtr = 0;
	return node;
    }

    /*
     * Not found, create new if asked for.
     */

    if (!createPtr || !*createPtr) {
	ASSERT(!node);
	return WORD_NIL;
    }
    *createPtr = 1;

    WORD_TRIEMAP_SIZE(map)++;

    entry = (Col_Word) AllocCells(1);
    WORD_MINTTRIELEAF_INIT(entry, key, WORD_NIL);

    if (!node) {
	/*
	 * First entry.
	 */

	ASSERT(!WORD_TRIEMAP_ROOT(map));
	ASSERT(WORD_TRIEMAP_SIZE(map) == 1);
	ASSERT(!leftPtr || !*leftPtr);
	ASSERT(!rightPtr || !*rightPtr);
	WORD_TRIEMAP_ROOT(map) = entry;
	Col_WordSetModified(map);

	return entry;
    }

    /*
     * Insert node here.
     */

    if (parent == map) {
	ASSERT(node == WORD_TRIEMAP_ROOT(map));
	ASSERT(!leftPtr || !*leftPtr);
	ASSERT(!rightPtr || !*rightPtr);
	nodePtr = &WORD_TRIEMAP_ROOT(map);
    } else {
	ASSERT(WORD_TYPE(parent) == WORD_TYPE_MINTTRIENODE);
	if (node == WORD_TRIENODE_LEFT(parent)) {
	    if (rightPtr) *rightPtr = WORD_TRIENODE_RIGHT(parent);
	    nodePtr = &WORD_TRIENODE_LEFT(parent);
	} else {
	    if (leftPtr) *leftPtr = WORD_TRIENODE_LEFT(parent);
	    nodePtr = &WORD_TRIENODE_RIGHT(parent);
	}
    }

    node = (Col_Word) AllocCells(1);
    if (compare > 0) {
	/*
	 * Entry is right.
	 */

	if (leftPtr) *leftPtr = *nodePtr;
	WORD_MINTTRIENODE_INIT(node, mask, *nodePtr, entry);
    } else {
	/*
	 * Entry is left.
	 */

	if (rightPtr) *rightPtr = *nodePtr;
	WORD_MINTTRIENODE_INIT(node, mask, entry, *nodePtr);
    }
    *nodePtr = node;
    Col_WordSetModified(parent);

    return entry;
}


/****************************************************************************
 * Group: Trie Map Access
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_StringTrieMapGet
 *
 *	Get value mapped to the given string key if present.
 *
 * Arguments:
 *	map		- String trie map to get entry from.
 *	key		- String entry key.
 *	valuePtr	- Returned entry value, if found.
 *
 * Result:
 *	Whether the key was found in the map. In this case the value is returned
 *	through valuePtr.
 *---------------------------------------------------------------------------*/

int
Col_StringTrieMapGet(
    Col_Word map,
    Col_Word key,
    Col_Word *valuePtr)
{
    Col_Word entry;

    if (WORD_TYPE(map) != WORD_TYPE_STRTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not a string trie map", map);
	return WORD_NIL;
    }

    entry = StringTrieMapFindEntry(map, key, 0, NULL, NULL, NULL);
    if (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_MTRIELEAF);
	*valuePtr = WORD_MAPENTRY_VALUE(entry);
	return 1;
    } else {
	return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_IntTrieMapGet
 *
 *	Get value mapped to the given integer key if present.
 *
 * Arguments:
 *	map		- Integer trie map to get entry from.
 *	key		- Integer entry key.
 *	valuePtr	- Returned entry value, if found.
 *
 * Result:
 *	Whether the key was found in the map. In this case the value is returned
 *	through valuePtr.
 *---------------------------------------------------------------------------*/

int
Col_IntTrieMapGet(
    Col_Word map,
    intptr_t key,
    Col_Word *valuePtr)
{
    Col_Word entry;

    if (WORD_TYPE(map) != WORD_TYPE_INTTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not an integer trie map", map);
	return WORD_NIL;
    }

    entry = IntTrieMapFindEntry(map, key, 0, NULL, NULL, NULL);
    if (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_MINTTRIELEAF);
	*valuePtr = WORD_MAPENTRY_VALUE(entry);
	return 1;
    } else {
	return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_StringTrieMapSet
 *
 *	Map the value to the string key, replacing any existing.
 *
 * Arguments:
 *	map	- String trie map to insert entry into.
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
Col_StringTrieMapSet(
    Col_Word map,
    Col_Word key,
    Col_Word value)
{
    int create = 1;
    Col_Word entry;

    if (WORD_TYPE(map) != WORD_TYPE_STRTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not a string trie map", map);
	return WORD_NIL;
    }

    entry = StringTrieMapFindEntry(map, key, 1, &create, NULL, NULL);
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_MTRIELEAF);
    Col_WordSetModified(entry);
    WORD_MAPENTRY_VALUE(entry) = value;
    return create;
}

/*---------------------------------------------------------------------------
 * Function: Col_IntTrieMapSet
 *
 *	Get value mapped to the given integer key if present.
 *
 * Arguments:
 *	map		- Integer trie map to get entry from.
 *	key		- Integer entry key.
 *	valuePtr	- Returned entry value, if found.
 *
 * Result:
 *	Whether the key was found in the map. In this case the value is returned
 *	through valuePtr.
 *---------------------------------------------------------------------------*/

int
Col_IntTrieMapSet(
    Col_Word map,
    intptr_t key,
    Col_Word value)
{
    int create = 1;
    Col_Word entry;

    if (WORD_TYPE(map) != WORD_TYPE_INTTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not an integer trie map", map);
	return WORD_NIL;
    }

    entry = IntTrieMapFindEntry(map, key, 1, &create, NULL, NULL);
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_MINTTRIELEAF);
    Col_WordSetModified(entry);
    WORD_MAPENTRY_VALUE(entry) = value;
    return create;
}

/*---------------------------------------------------------------------------
 * Function: Col_StringTrieMapUnset
 *
 *	Remove any value mapped to the given string key.
 *
 * Arguments:
 *	map	- String trie map to remove entry from.
 *	key	- String entry key.
 *
 * Result:
 *	Whether an existing entry was removed.
 *---------------------------------------------------------------------------*/

int
Col_StringTrieMapUnset(
    Col_Word map,
    Col_Word key)
{
    Col_Word node, grandParent, parent, sibling;
    Col_RopeIterator itKey;
    Col_Char mask;
    Col_Char cKey;

    if (WORD_TYPE(map) != WORD_TYPE_STRTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not a string trie map", map);
	return 0;
    }

    if (!WORD_TRIEMAP_ROOT(map)) {
	/*
	 * Map is empty.
	 */

	ASSERT(WORD_TRIEMAP_SIZE(map) == 0);
	return 0;
    }

    /*
     * Search for matching entry.
     */

    grandParent = WORD_NIL;
    parent = map;
    node = WORD_TRIEMAP_ROOT(map);
    Col_RopeIterBegin(key, 0, &itKey);
    while (node) {
	if (WORD_TYPE(node) == WORD_TYPE_MTRIELEAF) break;

	ASSERT(WORD_TYPE(node) == WORD_TYPE_MSTRTRIENODE);
	mask = WORD_STRTRIENODE_MASK(node);
	cKey = COL_CHAR_INVALID;
	if (!Col_RopeIterEnd(&itKey)) {
	    Col_RopeIterMoveTo(&itKey, WORD_STRTRIENODE_DIFF(node));
	    if (!Col_RopeIterEnd(&itKey)) {
		cKey = Col_RopeIterAt(&itKey);
	    }
	}
	grandParent = parent;
	parent = node;
	if (cKey != COL_CHAR_INVALID && (!mask || (cKey & mask))) {
	    node = WORD_TRIENODE_RIGHT(node);
	} else {
	    node = WORD_TRIENODE_LEFT(node);
	}
    }

    ASSERT(WORD_TYPE(node) == WORD_TYPE_MTRIELEAF);
    if (Col_CompareRopes(key, WORD_MAPENTRY_KEY(node), 0, SIZE_MAX, NULL, NULL, 
	    NULL) != 0) {
	/*
	 * Not found.
	 */

	return 0;
    }

    WORD_TRIEMAP_SIZE(map)--;

    if (!grandParent) {
	/*
	 * Last entry.
	 */

	ASSERT(parent == map);
	ASSERT(WORD_TRIEMAP_ROOT(map) == node);
	ASSERT(WORD_TRIEMAP_SIZE(map) == 0);
	WORD_TRIEMAP_ROOT(map) = WORD_NIL;
	Col_WordSetModified(map);

	return 1;
    }

    /*
     * Replace parent by sibling.
     */

    ASSERT(WORD_TYPE(parent) == WORD_TYPE_MSTRTRIENODE);
    if (node == WORD_TRIENODE_RIGHT(parent)) {
	sibling = WORD_TRIENODE_LEFT(parent);
    } else {
	sibling = WORD_TRIENODE_RIGHT(parent);
    }
    if (grandParent == map) {
	/*
	 * Parent was root.
	 */

	ASSERT(WORD_TRIEMAP_ROOT(map) == parent);
	WORD_TRIEMAP_ROOT(map) = sibling;
    } else if (WORD_TRIENODE_LEFT(grandParent) == parent) {
	WORD_TRIENODE_LEFT(grandParent) = sibling;
    } else {
	WORD_TRIENODE_RIGHT(grandParent) = sibling;
    }
    Col_WordSetModified(grandParent);

    return 1;
}

/*---------------------------------------------------------------------------
 * Function: Col_IntTrieMapUnset
 *
 *	Remove any value mapped to the given integer key.
 *
 * Arguments:
 *	map	- Integer trie map to remove entry from.
 *	key	- Integer entry key.
 *
 * Result:
 *	Whether an existing entry was removed.
 *---------------------------------------------------------------------------*/

int
Col_IntTrieMapUnset(
    Col_Word map,
    intptr_t key)
{
    Col_Word node, grandParent, parent, sibling;
    intptr_t mask;

    if (WORD_TYPE(map) != WORD_TYPE_INTTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not an integer trie map", map);
	return 0;
    }

    if (!WORD_TRIEMAP_ROOT(map)) {
	/*
	 * Map is empty.
	 */

	ASSERT(WORD_TRIEMAP_SIZE(map) == 0);
	return 0;
    }

    /*
     * Search for matching entry.
     */

    grandParent = WORD_NIL;
    parent = map;
    node = WORD_TRIEMAP_ROOT(map);
    while (node) {
	if (WORD_TYPE(node) == WORD_TYPE_MINTTRIELEAF) break;

	ASSERT(WORD_TYPE(node) == WORD_TYPE_MINTTRIENODE);
	mask = WORD_INTTRIENODE_MASK(node);
	grandParent = parent;
	parent = node;
	if (mask ? (key & mask) : (key >= 0)) {
	    node = WORD_TRIENODE_RIGHT(node);
	} else {
	    node = WORD_TRIENODE_LEFT(node);
	}
    }

    ASSERT(WORD_TYPE(node) == WORD_TYPE_MINTTRIELEAF);
    if (key != WORD_INTMAPENTRY_KEY(node)) {
	/*
	 * Not found.
	 */

	return 0;
    }

    WORD_TRIEMAP_SIZE(map)--;

    if (!grandParent) {
	/*
	 * Last entry.
	 */

	ASSERT(parent == map);
	ASSERT(WORD_TRIEMAP_ROOT(map) == node);
	ASSERT(WORD_TRIEMAP_SIZE(map) == 0);
	WORD_TRIEMAP_ROOT(map) = WORD_NIL;
	Col_WordSetModified(map);

	return 1;
    }

    /*
     * Replace parent by sibling.
     */

    ASSERT(WORD_TYPE(parent) == WORD_TYPE_MINTTRIENODE);
    if (node == WORD_TRIENODE_RIGHT(parent)) {
	sibling = WORD_TRIENODE_LEFT(parent);
    } else {
	sibling = WORD_TRIENODE_RIGHT(parent);
    }
    if (grandParent == map) {
	/*
	 * Parent was root.
	 */

	ASSERT(WORD_TRIEMAP_ROOT(map) == parent);
	WORD_TRIEMAP_ROOT(map) = sibling;
    } else if (WORD_TRIENODE_LEFT(grandParent) == parent) {
	WORD_TRIENODE_LEFT(grandParent) = sibling;
    } else {
	WORD_TRIENODE_RIGHT(grandParent) = sibling;
    }
    Col_WordSetModified(grandParent);

    return 1;
}


/****************************************************************************
 * Group: Trie Map Iterators
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_TrieMapIterFirst
 *
 *	Initialize the trie map iterator so that it points to the first entry
 *	within the map.
 *
 * Arguments:
 *	map	- Trie map to iterate over.
 *	it	- Iterator to initialize.
 *---------------------------------------------------------------------------*/

void
Col_TrieMapIterFirst(
    Col_Word map,
    Col_MapIterator *it)
{
    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRTRIEMAP:
	case WORD_TYPE_INTTRIEMAP:
	    break;

	default:
	    Col_Error(COL_ERROR, "%x is not a trie map", map);
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

    ASSERT(WORD_TRIEMAP_ROOT(map));
    it->map = map;
    it->entry = LeftmostLeaf(WORD_TRIEMAP_ROOT(map), &it->trie.next);
    it->trie.prev = WORD_NIL;
    ASSERT(it->entry);
}

/*---------------------------------------------------------------------------
 * Function: Col_TrieMapIterLast
 *
 *	Initialize the trie map iterator so that it points to the last entry
 *	within the map.
 *
 * Arguments:
 *	map	- Trie map to iterate over.
 *	it	- Iterator to initialize.
 *---------------------------------------------------------------------------*/

void
Col_TrieMapIterLast(
    Col_Word map,
    Col_MapIterator *it)
{
    switch (WORD_TYPE(map)) {
	case WORD_TYPE_STRTRIEMAP:
	case WORD_TYPE_INTTRIEMAP:
	    break;

	default:
	    Col_Error(COL_ERROR, "%x is not a trie map", map);
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

    ASSERT(WORD_TRIEMAP_ROOT(map));
    it->map = map;
    it->entry = RightmostLeaf(WORD_TRIEMAP_ROOT(map), &it->trie.prev);
    it->trie.next = WORD_NIL;
    ASSERT(it->entry);
}

/*---------------------------------------------------------------------------
 * Function: Col_StringTrieMapIterFind
 *
 *	Initialize the trie map iterator so that it points to the entry with
 *	the given string key within the map.
 *
 * Arguments:
 *	map		- String trie map to find or create entry into.
 *	key		- String entry key.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *	it		- Iterator to initialize.
 *
 * Results:
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *---------------------------------------------------------------------------*/

void
Col_StringTrieMapIterFind(
    Col_Word map, 
    Col_Word key, 
    int *createPtr, 
    Col_MapIterator *it)
{
    if (WORD_TYPE(map) != WORD_TYPE_STRTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not a string trie map", map);
	it->map = WORD_NIL;
	return;
    }

    it->entry = StringTrieMapFindEntry(map, key, 0, createPtr, &it->trie.prev,
	    &it->trie.next);
    if (!it->entry) {
	/*
	 * Not found.
	 */

	it->map = WORD_NIL;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_IntTrieMapIterFind
 *
 *	Initialize the trie map iterator so that it points to the entry with
 *	the given integer key within the map.
 *
 * Arguments:
 *	map		- Integer trie map to find or create entry into.
 *	key		- Integer entry key.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *	it		- Iterator to initialize.
 *
 * Results:
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *---------------------------------------------------------------------------*/

void
Col_IntTrieMapIterFind(
    Col_Word map, 
    intptr_t key, 
    int *createPtr, 
    Col_MapIterator *it)
{
    if (WORD_TYPE(map) != WORD_TYPE_INTTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not an integer trie map", map);
	it->map = WORD_NIL;
	return;
    }

    it->entry = IntTrieMapFindEntry(map, key, 0, createPtr, &it->trie.prev,
	    &it->trie.next);
    if (!it->entry) {
	/*
	 * Not found.
	 */

	it->map = WORD_NIL;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_TrieMapIterSetValue
 *
 *	Set value of trie map iterator.
 *
 * Argument:
 *	it	- Map iterator to set value for.
 *	value	- Value to set.
 *---------------------------------------------------------------------------*/

void
Col_TrieMapIterSetValue(
    Col_MapIterator *it,
    Col_Word value)
{
    if (Col_MapIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid map iterator");
	return;
    }

    ASSERT(it->entry);

    switch (WORD_TYPE(it->map)) {
	case WORD_TYPE_STRTRIEMAP:
	    /*
	     * Set entry value.
	     */

	    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_MTRIELEAF);
	    WORD_MAPENTRY_VALUE(it->entry) = value;
	    Col_WordSetModified(it->entry);
	    break;

	case WORD_TYPE_INTTRIEMAP:
	    /*
	     * Set entry value.
	     */

	    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_MINTTRIELEAF);
	    WORD_MAPENTRY_VALUE(it->entry) = value;
	    Col_WordSetModified(it->entry);
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    Col_Error(COL_ERROR, "%x is not a trie map", it->map);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_TrieMapIterNext
 *
 *	Move the iterator to the next element.
 *
 * Argument:
 *	it	- The iterator to move.
 *---------------------------------------------------------------------------*/

void
Col_TrieMapIterNext(
    Col_MapIterator *it)
{
    if (Col_MapIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid map iterator");
	return;
    }

    ASSERT(it->entry);

    if (!it->trie.next) {
	/*
	 * Refresh shortcuts.
	 */

	switch (WORD_TYPE(it->map)) {
	    case WORD_TYPE_STRTRIEMAP:
		StringTrieMapFindNode(it->map, WORD_MAPENTRY_KEY(it->entry), 0,
			NULL, NULL, NULL, &it->trie.next, NULL, NULL);
		break;

	    case WORD_TYPE_INTTRIEMAP:
		IntTrieMapFindNode(it->map, WORD_INTMAPENTRY_KEY(it->entry), 0,
			NULL, NULL, NULL, &it->trie.next, NULL);
		break;
	}
    }

    if (it->trie.next) {
	/*
	 * Next is leftmost leaf of adjacent right branch.
	 */

	it->trie.prev = it->entry;
	it->entry = LeftmostLeaf(it->trie.next, &it->trie.next);
    } else {
	/*
	 * End of map.
	 */

	it->map = WORD_NIL;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_TrieMapIterPrevious
 *
 *	Move the iterator to the previous element.
 *
 * Argument:
 *	it	- The iterator to move.
 *---------------------------------------------------------------------------*/

void
Col_TrieMapIterPrevious(
    Col_MapIterator *it)
{
    if (Col_MapIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid map iterator");
	return;
    }

    ASSERT(it->entry);

    if (!it->trie.prev) {
	/*
	 * Refresh shortcuts.
	 */

	switch (WORD_TYPE(it->map)) {
	    case WORD_TYPE_STRTRIEMAP:
		StringTrieMapFindNode(it->map, WORD_MAPENTRY_KEY(it->entry), 0,
			NULL, NULL, &it->trie.prev, NULL, NULL, NULL);
		break;

	    case WORD_TYPE_INTTRIEMAP:
		IntTrieMapFindNode(it->map, WORD_INTMAPENTRY_KEY(it->entry), 0,
			NULL, NULL, &it->trie.prev, NULL, NULL);
		break;
	}
    }

    if (it->trie.prev) {
	/*
	 * Previous is rightmost leaf of adjacent left branch.
	 */

	it->trie.next  = it->entry;
	it->entry = RightmostLeaf(it->trie.prev, &it->trie.prev);
    } else {
	/*
	 * End of map.
	 */

	it->map = WORD_NIL;
    }
}
