/*
 * File: colTrie.c
 *
 *	This file implements the trie map handling features of Colibri.
 *
 *	Trie maps are an implementation of generic maps that use crit-bit trees
 *	for string and integer keys.
 *
 *	They are always mutable.
 *
 * See also:
 *	<colTrie.h>, <colMap.h>
 */

#include "include/colibri.h"
#include "colInternal.h"

#include "colWordInt.h"
#include "colMapInt.h"
#include "colHashInt.h"
#include "colTrieInt.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>


/*
 * Prototypes for functions used only in this file.
 */

static Col_Word		LeftmostLeaf(Col_Word node, Col_Word *rightPtr);
static Col_Word		RightmostLeaf(Col_Word node, Col_Word *leftPtr);
static Col_Word		TrieMapFindNode(Col_Word map, Col_Word key, int closest,
			    int *comparePtr, Col_Word *grandParentPtr, 
			    Col_Word *parentPtr, Col_Word *leftPtr, 
			    Col_Word *rightPtr, size_t *diffPtr, 
			    size_t *bitPtr);
static Col_Word		StringTrieMapFindNode(Col_Word map, Col_Word key,
			    int closest, int *comparePtr, 
			    Col_Word *grandParentPtr, Col_Word *parentPtr,
			    Col_Word *leftPtr, Col_Word *rightPtr, 
			    size_t *diffPtr, Col_Char *maskPtr);
static Col_Word		IntTrieMapFindNode(Col_Word map, intptr_t key,
			    int closest, int *comparePtr, 
			    Col_Word *grandParentPtr, Col_Word *parentPtr,
			    Col_Word *leftPtr, Col_Word *rightPtr, 
			    intptr_t *maskPtr);
static Col_Word		TrieMapFindEntry(Col_Word map, Col_Word key,
			    int mutable, int *createPtr, Col_Word *leftPtr, 
			    Col_Word *rightPtr);
static Col_Word		StringTrieMapFindEntry(Col_Word map, Col_Word key,
			    int mutable, int *createPtr, Col_Word *leftPtr, 
			    Col_Word *rightPtr);
static Col_Word		IntTrieMapFindEntry(Col_Word map, intptr_t key,
			    int mutable, int *createPtr, Col_Word *leftPtr, 
			    Col_Word *rightPtr);
static void		FreezeSubtrie(Col_Word node);
static Col_Word		ConvertNodeToMutable(Col_Word node, Col_Word map, 
				Col_Word prefix);
static Col_Word		ConvertStringNodeToMutable(Col_Word node, Col_Word map, 
				Col_Word prefix);
static Col_Word		ConvertIntNodeToMutable(Col_Word node, Col_Word map, 
				intptr_t prefix);


/*
================================================================================
Internal Section: Trie Map Internals
================================================================================
*/

//TODO algorithms

/****************************************************************************
 * Internal Group: Nodes And Leaves
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
	case WORD_TYPE_TRIENODE:
	case WORD_TYPE_MTRIENODE:
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
 *	leftPtr	- (out) Adjacent left branch.
 *---------------------------------------------------------------------------*/

static Col_Word
RightmostLeaf(
     Col_Word node,
     Col_Word *leftPtr) 
{
    Col_Word left = WORD_NIL;

    for (;;) {
	switch (WORD_TYPE(node)) {
	case WORD_TYPE_TRIENODE:
	case WORD_TYPE_MTRIENODE:
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
 * Internal Function: TrieMapFindNode
 *
 *	Find node equal or closest to the given key.
 *
 * Arguments:
 *	map	- Trie map to find or create entry into.
 *	key	- Entry key.
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
 *	grandParentPtr	- (out) The node's grandparent word. May be nil, the map
 *			  or another node.
 *	parentPtr	- (out) The node's parent word. May be the map or 
 *			  another node.
 *	leftPtr		- (out) Adjacent left branch.
 *	rightPtr	- (out) Adjacent right branch.
 *
 *	Moreover (if non-NULL), if key doesn't match and closest is true:
 *
 *	diffPtr		- (out) Index of first differing key element.
 *	bitPtr		- (out) Crit-bit position.
 *
 * See also:
 *	<Col_CompareRopesL>
 *---------------------------------------------------------------------------*/

static Col_Word
TrieMapFindNode(
    Col_Word map,
    Col_Word key,
    int closest,
    int *comparePtr,
    Col_Word *grandParentPtr,
    Col_Word *parentPtr,
    Col_Word *leftPtr,
    Col_Word *rightPtr,
    size_t *diffPtr,
    size_t *bitPtr)
{
    int compare;
    Col_Word node, grandParent, parent, left, right;
    Col_Word entryKey;
    size_t diff, bit;
    Col_CustomTrieMapType *typeInfo = (Col_CustomTrieMapType *) WORD_TYPEINFO(map);

    ASSERT(WORD_TYPE(map) == WORD_TYPE_CUSTOM);
    
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
    grandParent = WORD_NIL;
    parent = map;
    left = WORD_NIL;
    right = WORD_NIL;
    while (node) {
	switch (WORD_TYPE(node)) {
	case WORD_TYPE_TRIENODE:
	case WORD_TYPE_MTRIENODE:
	    diff = WORD_TRIENODE_DIFF(node);
	    bit = WORD_TRIENODE_BIT(node);

	    /*
	     * Descend.
	     */

	    grandParent = parent;
	    parent = node;
	    if (typeInfo->bitSetProc(key, diff, bit)) {
		left = WORD_TRIENODE_LEFT(node);
		node = WORD_TRIENODE_RIGHT(node);
	    } else {
		right = WORD_TRIENODE_RIGHT(node);
		node = WORD_TRIENODE_LEFT(node);
	    }
	    continue;
	}

	/*
	 * Leaf, exit loop.
	 */

	break;
    }

    ASSERT(WORD_TYPE(node) == WORD_TYPE_TRIELEAF || WORD_TYPE(node) == WORD_TYPE_MTRIELEAF);
    entryKey = WORD_MAPENTRY_KEY(node);
    compare = typeInfo->keyDiffProc(key, entryKey, &diff, &bit);
    if (compare == 0 || !closest) {
	/*
	 * Exact match found or required.
	 */

	if (comparePtr) *comparePtr = compare;
	if (grandParentPtr) *grandParentPtr = grandParent;
	if (parentPtr) *parentPtr = parent;
	if (leftPtr) *leftPtr = left;
	if (rightPtr) *rightPtr = right;
	return (compare == 0 ? node : WORD_NIL);
    }

    /*
     * Find longest common prefix.
     */

    node = WORD_TRIEMAP_ROOT(map);
    grandParent = WORD_NIL;
    parent = map;
    left = WORD_NIL;
    right = WORD_NIL;
    while (node) {
	switch (WORD_TYPE(node)) {
	case WORD_TYPE_TRIENODE:
	case WORD_TYPE_MTRIENODE:
	    if (diff < WORD_TRIENODE_DIFF(node) 
		    || (diff == WORD_TRIENODE_DIFF(node) 
		    && bit < WORD_TRIENODE_BIT(node))) {
		/*
		 * Prefixes diverge.
		 */

		break;
	    }

	    /*
	     * Descend.
	     */

	    grandParent = parent;
	    parent = node;
	    if (typeInfo->bitSetProc(key, WORD_TRIENODE_DIFF(node), 
		    WORD_TRIENODE_BIT(node))) {
		left = WORD_TRIENODE_LEFT(node);
		node = WORD_TRIENODE_RIGHT(node);
	    } else {
		right = WORD_TRIENODE_RIGHT(node);
		node = WORD_TRIENODE_LEFT(node);
	    }
	    continue;
	}

	/*
	 * Leaf or branch, exit loop.
	 */

	break;
    }
    if (comparePtr) *comparePtr = (typeInfo->bitSetProc(key, diff, bit) ? 1 
	    : -1);
    if (grandParentPtr) *grandParentPtr = grandParent;
    if (parentPtr) *parentPtr = parent;
    if (leftPtr) *leftPtr = left;
    if (rightPtr) *rightPtr = right;
    if (diffPtr) *diffPtr = diff;
    if (bitPtr) *bitPtr = bit;
    return node;
}

/*---------------------------------------------------------------------------
 * Internal Function: StringTrieMapFindNode
 *
 *	Find node equal or closest to the given key.
 *
 * Arguments:
 *	map	- String trie map to find or create entry into.
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
 *	grandParentPtr	- (out) The node's grandparent word. May be nil, the map
 *			  or another node.
 *	parentPtr	- (out) The node's parent word. May be the map or 
 *			  another node.
 *	leftPtr		- (out) Adjacent left branch.
 *	rightPtr	- (out) Adjacent right branch.
 *
 *	Moreover (if non-NULL), if key doesn't match and closest is true:
 *
 *	diffPtr		- (out) Index of first differing character.
 *	maskPtr		- (out) Crit-bit mask.
 *
 * See also:
 *	<Col_CompareRopesL>
 *---------------------------------------------------------------------------*/

static Col_Word
StringTrieMapFindNode(
    Col_Word map,
    Col_Word key,
    int closest,
    int *comparePtr,
    Col_Word *grandParentPtr,
    Col_Word *parentPtr,
    Col_Word *leftPtr,
    Col_Word *rightPtr,
    size_t *diffPtr,
    Col_Char *maskPtr)
{
    int compare;
    Col_Word node, grandParent, parent, left, right;
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
    grandParent = WORD_NIL;
    parent = map;
    left = WORD_NIL;
    right = WORD_NIL;
    Col_RopeIterBegin(key, 0, &itKey);
    while (node) {
	switch (WORD_TYPE(node)) {
	case WORD_TYPE_STRTRIENODE:
	case WORD_TYPE_MSTRTRIENODE:
	    mask = WORD_STRTRIENODE_MASK(node);
	    cKey = COL_CHAR_INVALID;
	    if (!Col_RopeIterEnd(&itKey)) {
		Col_RopeIterMoveTo(&itKey, WORD_STRTRIENODE_DIFF(node));
		if (!Col_RopeIterEnd(&itKey)) {
		    cKey = Col_RopeIterAt(&itKey);
		}
	    }

	    /*
	     * Descend.
	     */

	    grandParent = parent;
	    parent = node;
	    if (cKey != COL_CHAR_INVALID && (!mask || (cKey & mask))) {
		left = WORD_TRIENODE_LEFT(node);
		node = WORD_TRIENODE_RIGHT(node);
	    } else {
		right = WORD_TRIENODE_RIGHT(node);
		node = WORD_TRIENODE_LEFT(node);
	    }
	    continue;
	}

	/*
	 * Leaf, exit loop.
	 */

	break;
    }

    ASSERT(WORD_TYPE(node) == WORD_TYPE_TRIELEAF || WORD_TYPE(node) == WORD_TYPE_MTRIELEAF);
    entryKey = WORD_MAPENTRY_KEY(node);
    compare = Col_CompareRopesL(key, entryKey, 0, SIZE_MAX, &diff, &cKey, 
	    &cEntryKey);
    if (compare == 0 || !closest) {
	/*
	 * Exact match found or required.
	 */

	if (comparePtr) *comparePtr = compare;
	if (grandParentPtr) *grandParentPtr = grandParent;
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
    grandParent = WORD_NIL;
    parent = map;
    left = WORD_NIL;
    right = WORD_NIL;
    Col_RopeIterBegin(key, 0, &itKey);
    while (node) {
	switch (WORD_TYPE(node)) {
	case WORD_TYPE_STRTRIENODE:
	case WORD_TYPE_MSTRTRIENODE:
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

	    /*
	     * Descend.
	     */

	    grandParent = parent;
	    parent = node;
	    if (cKey != COL_CHAR_INVALID && (!mask || (cKey & mask))) {
		left = WORD_TRIENODE_LEFT(node);
		node = WORD_TRIENODE_RIGHT(node);
	    } else {
		right = WORD_TRIENODE_RIGHT(node);
		node = WORD_TRIENODE_LEFT(node);
	    }
	    continue;
	}

	/*
	 * Leaf or branch, exit loop.
	 */

	break;
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
    if (grandParentPtr) *grandParentPtr = grandParent;
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
 *	Find node equal or closest to the given key.
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
 *	grandParentPtr	- (out) The node's grandparent word. May be nil, the map
 *			  or another node.
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
    Col_Word *grandParentPtr,
    Col_Word *parentPtr,
    Col_Word *leftPtr,
    Col_Word *rightPtr,
    intptr_t *maskPtr)
{
    int compare;
    Col_Word node, grandParent, parent, left, right;
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
    grandParent = WORD_NIL;
    parent = map;
    left = WORD_NIL;
    right = WORD_NIL;
    while (node) {
	switch (WORD_TYPE(node)) {
	case WORD_TYPE_INTTRIENODE:
	case WORD_TYPE_MINTTRIENODE:
	    mask = WORD_INTTRIENODE_MASK(node);

	    /*
	     * Descend.
	     */

	    grandParent = parent;
	    parent = node;
	    if (mask ? (key & mask) : (key >= 0)) {
		left = WORD_TRIENODE_LEFT(node);
		node = WORD_TRIENODE_RIGHT(node);
	    } else {
		right = WORD_TRIENODE_RIGHT(node);
		node = WORD_TRIENODE_LEFT(node);
	    }
	    continue;
	}

	/*
	 * Leaf, exit loop.
	 */

	break;
    }

    ASSERT(WORD_TYPE(node) == WORD_TYPE_INTTRIELEAF || WORD_TYPE(node) == WORD_TYPE_MINTTRIELEAF);
    entryKey = WORD_INTMAPENTRY_KEY(node);
    compare = (key == entryKey ? 0 : key > entryKey ? 1 : -1);
    if (compare == 0 || !closest) {
	/*
	 * Exact match found or required.
	 */

	if (comparePtr) *comparePtr = compare;
	if (grandParentPtr) *grandParentPtr = grandParent;
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
    grandParent = WORD_NIL;
    parent = map;
    left = WORD_NIL;
    right = WORD_NIL;
    while (node) {
	switch (WORD_TYPE(node)) {
	case WORD_TYPE_INTTRIENODE:
	case WORD_TYPE_MINTTRIENODE:
	    mask = WORD_INTTRIENODE_MASK(node);
	    if (mask && newMask > mask) {
		/*
		 * Prefixes diverge.
		 */

		break;
	    }

	    /*
	     * Descend.
	     */

	    grandParent = parent;
	    parent = node;
	    if (mask ? (key & mask) : (key >= 0)) {
		left = WORD_TRIENODE_LEFT(node);
		node = WORD_TRIENODE_RIGHT(node);
	    } else {
		right = WORD_TRIENODE_RIGHT(node);
		node = WORD_TRIENODE_LEFT(node);
	    }
	    continue;
	}

	/*
	 * Leaf or branch, exit loop.
	 */

	break;
    }
    if (comparePtr) *comparePtr = ((newMask ? (key & newMask) : (key >= 0)) ? 1
	    : -1);
    if (grandParentPtr) *grandParentPtr = grandParent;
    if (parentPtr) *parentPtr = parent;
    if (leftPtr) *leftPtr = left;
    if (rightPtr) *rightPtr = right;
    if (maskPtr) *maskPtr = newMask;
    return node;
}

/*---------------------------------------------------------------------------
 * Internal Function: TrieMapFindEntry
 *
 *	Find or create in trie map the entry mapped to the given key.
 *
 * Arguments:
 *	map		- Trie map to find or create entry into.
 *	key		- Entry key.
 *	mutable		- If true, ensure that entry is mutable.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *
 * Results:
 *	The entry if found or created, else nil. Additionally (if non-NULL):
 *
 *	createPtr	- (out) Whether a new entry was created. 
 *	leftPtr		- (out) Adjacent left branch.
 *	rightPtr	- (out) Adjacent right branch.
 *
 * See also:
 *	<TrieMapFindNode>
 *---------------------------------------------------------------------------*/

static Col_Word
TrieMapFindEntry(
    Col_Word map,
    Col_Word key,
    int mutable,
    int *createPtr,
    Col_Word *leftPtr,
    Col_Word *rightPtr)
{
    Col_Word node, entry, parent, *nodePtr;
    int compare;
    size_t diff, bit;
    
    node = TrieMapFindNode(map, key, (createPtr && *createPtr), &compare, NULL,
	    &parent, leftPtr, rightPtr, &diff, &bit);
    if (node && compare == 0) {
	/*
	 * Found!
	 */

	if (createPtr) *createPtr = 0;

	if (mutable && WORD_TYPE(node) != WORD_TYPE_MTRIELEAF) {
	    /*
	     * Entry is immutable, convert to mutable.
	     */

	    ASSERT(WORD_TYPE(node) == WORD_TYPE_TRIELEAF);
	    node = ConvertNodeToMutable(node, map, key);
	}
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
	if (WORD_TYPE(parent) != WORD_TYPE_MTRIENODE) {
	    /*
	     * Parent is immutable, convert first.
	     */

	    ASSERT(WORD_TYPE(parent) == WORD_TYPE_TRIENODE);
	    parent = ConvertStringNodeToMutable(parent, map, key);
	}
	ASSERT(WORD_TYPE(parent) == WORD_TYPE_MTRIENODE);
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
	WORD_MTRIENODE_INIT(node, diff, bit, *nodePtr, entry);
    } else {
	/*
	 * Entry is left.
	 */

	if (rightPtr) *rightPtr = *nodePtr;
	WORD_MTRIENODE_INIT(node, diff, bit, entry, *nodePtr);
    }
    *nodePtr = node;

    return entry;
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
 *
 * See also:
 *	<StringTrieMapFindNode>
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
	    NULL, &parent, leftPtr, rightPtr, &diff, &mask);
    if (node && compare == 0) {
	/*
	 * Found!
	 */

	if (createPtr) *createPtr = 0;

	if (mutable && WORD_TYPE(node) != WORD_TYPE_MTRIELEAF) {
	    /*
	     * Entry is immutable, convert to mutable.
	     */

	    ASSERT(WORD_TYPE(node) == WORD_TYPE_TRIELEAF);
	    node = ConvertStringNodeToMutable(node, map, key);
	}
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
	if (WORD_TYPE(parent) != WORD_TYPE_MSTRTRIENODE) {
	    /*
	     * Parent is immutable, convert first.
	     */

	    ASSERT(WORD_TYPE(parent) == WORD_TYPE_STRTRIENODE);
	    parent = ConvertStringNodeToMutable(parent, map, key);
	}
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
 *
 * See also:
 *	<IntTrieMapFindNode>
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
	    NULL, &parent, leftPtr, rightPtr, &mask);
    if (node && compare == 0) {
	/*
	 * Found!
	 */

	if (createPtr) *createPtr = 0;

	if (mutable && WORD_TYPE(node) != WORD_TYPE_MINTTRIELEAF) {
	    /*
	     * Entry is immutable, convert to mutable.
	     */

	    ASSERT(WORD_TYPE(node) == WORD_TYPE_INTTRIELEAF);
	    node = ConvertIntNodeToMutable(node, map, key);
	}
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
	if (WORD_TYPE(parent) != WORD_TYPE_MINTTRIENODE) {
	    /*
	     * Parent is immutable, convert first.
	     */

	    ASSERT(WORD_TYPE(parent) == WORD_TYPE_INTTRIENODE);
	    parent = ConvertIntNodeToMutable(parent, map, key);
	}
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

    return entry;
}


/****************************************************************************
 * Internal Group: Mutability
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Function: FreezeSubtrie
 *
 *	Turn trie nodes into immutable
 *
 * Argument:
 *	node	- Root node of subtrie.
 *
 * Side effects:
 *	Subtrie content is frozen.
 *---------------------------------------------------------------------------*/

static void
    FreezeSubtrie(
    Col_Word node)
{
    /*
     * Entry point for tail recursive calls.
     */

start:
    switch (WORD_TYPE(node)) {
    case WORD_TYPE_MTRIENODE:
	/*
	 * Freeze node: simply change type ID.
	 */

	ASSERT(!WORD_PINNED(node));
	WORD_SET_TYPEID(node, WORD_TYPE_TRIENODE);

	/*
	 * Recurse on left and tail recurse on right.
	 */

	FreezeSubtrie(WORD_TRIENODE_LEFT(node));
	node = WORD_TRIENODE_RIGHT(node);
	goto start;

    case WORD_TYPE_MSTRTRIENODE:
	/*
	 * Freeze node: simply change type ID.
	 */

	ASSERT(!WORD_PINNED(node));
	WORD_SET_TYPEID(node, WORD_TYPE_STRTRIENODE);

	/*
	 * Recurse on left and tail recurse on right.
	 */

	FreezeSubtrie(WORD_TRIENODE_LEFT(node));
	node = WORD_TRIENODE_RIGHT(node);
	goto start;

    case WORD_TYPE_MINTTRIENODE:
	/*
	 * Freeze node: simply change type ID.
	 */

	ASSERT(!WORD_PINNED(node));
	WORD_SET_TYPEID(node, WORD_TYPE_INTTRIENODE);

	/*
	 * Recurse on left and tail recurse on right.
	 */

	FreezeSubtrie(WORD_TRIENODE_LEFT(node));
	node = WORD_TRIENODE_RIGHT(node);
	goto start;

    case WORD_TYPE_MTRIELEAF:
	/*
	 * Freeze node: simply change type ID.
	 */

	ASSERT(!WORD_PINNED(node));
	WORD_SET_TYPEID(node, WORD_TYPE_TRIELEAF);
	break;

    case WORD_TYPE_MINTTRIELEAF:
	/*
	 * Freeze node: simply change type ID.
	 */

	ASSERT(!WORD_PINNED(node));
	WORD_SET_TYPEID(node, WORD_TYPE_INTTRIELEAF);
	break;

    default:
	/*
	 * Already immutable.
	 */

	return;
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: ConvertNodeToMutable
 *
 *	Convert immutable node and all all its ancestors to mutable.
 *
 * Arguments:
 *	node	- Node to convert.
 *	map	- Trie map the node belongs to.
 *	prefix	- Key prefix.
 *
 * Result:
 *	The converted mutable entry.
 *---------------------------------------------------------------------------*/

static Col_Word
ConvertNodeToMutable(
    Col_Word node, 
    Col_Word map, 
    Col_Word prefix)
{
    Col_Word *nodePtr, existing, converted;
    Col_CustomTrieMapType *typeInfo = (Col_CustomTrieMapType *) WORD_TYPEINFO(map);

    ASSERT(WORD_TYPE(map) == WORD_TYPE_CUSTOM);

    ASSERT(WORD_TYPE(node) == WORD_TYPE_TRIENODE || WORD_TYPE(node) == WORD_TYPE_TRIELEAF);
    nodePtr = &WORD_TRIEMAP_ROOT(map);
    for (;;) {
	existing = *nodePtr;
	switch (WORD_TYPE(existing)) {
	case WORD_TYPE_TRIENODE:
	    /*
	     * Convert to mutable.
	     */

	    converted = (Col_Word) AllocCells(1);
	    memcpy((void *) converted, (void *) existing, sizeof(Cell));
	    ASSERT(!WORD_PINNED(existing));
	    WORD_SET_TYPEID(converted, WORD_TYPE_MTRIENODE);
	    *nodePtr = converted;
	    if (existing == node) {
		return converted;
	    }
	    existing = converted;
	    /* continued */
	case WORD_TYPE_MTRIENODE:
	    ASSERT(existing != node);

	    /*
	     * Descend.
	     */

	    if (typeInfo->bitSetProc(prefix, WORD_TRIENODE_DIFF(existing), 
		    WORD_TRIENODE_BIT(existing))) {
		existing = WORD_TRIENODE_RIGHT(existing);
	    } else {
		existing = WORD_TRIENODE_LEFT(existing);
	    }
	    continue;

	case WORD_TYPE_TRIELEAF:
	    ASSERT(existing == node);
	    /*
	     * Convert to mutable.
	     */

	    converted = (Col_Word) AllocCells(1);
	    memcpy((void *) converted, (void *) existing, sizeof(Cell));
	    ASSERT(!WORD_PINNED(existing));
	    WORD_SET_TYPEID(converted, WORD_TYPE_MTRIELEAF);
	    *nodePtr = converted;
	    return converted;
	}
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: ConvertStringNodeToMutable
 *
 *	Convert immutable string node and all all its ancestors to mutable.
 *
 * Arguments:
 *	node	- Node to convert.
 *	map	- Trie map the node belongs to.
 *	prefix	- Key prefix.
 *
 * Result:
 *	The converted mutable entry.
 *---------------------------------------------------------------------------*/

static Col_Word
ConvertStringNodeToMutable(
    Col_Word node, 
    Col_Word map, 
    Col_Word prefix)
{
    Col_Word *nodePtr, existing, converted;
    Col_RopeIterator itKey;
    Col_Char mask;
    Col_Char cKey;

    ASSERT(WORD_TYPE(node) == WORD_TYPE_STRTRIENODE || WORD_TYPE(node) == WORD_TYPE_TRIELEAF);
    nodePtr = &WORD_TRIEMAP_ROOT(map);
    Col_RopeIterBegin(prefix, 0, &itKey);
    for (;;) {
	existing = *nodePtr;
	switch (WORD_TYPE(existing)) {
	case WORD_TYPE_STRTRIENODE:
	    /*
	     * Convert to mutable.
	     */

	    converted = (Col_Word) AllocCells(1);
	    memcpy((void *) converted, (void *) existing, sizeof(Cell));
	    ASSERT(!WORD_PINNED(existing));
	    WORD_SET_TYPEID(converted, WORD_TYPE_MSTRTRIENODE);
	    *nodePtr = converted;
	    if (existing == node) {
		return converted;
	    }
	    existing = converted;
	    /* continued */
	case WORD_TYPE_MSTRTRIENODE:
	    ASSERT(existing != node);
	    mask = WORD_STRTRIENODE_MASK(existing);
	    cKey = COL_CHAR_INVALID;
	    if (!Col_RopeIterEnd(&itKey)) {
		Col_RopeIterMoveTo(&itKey, WORD_STRTRIENODE_DIFF(existing));
		if (!Col_RopeIterEnd(&itKey)) {
		    cKey = Col_RopeIterAt(&itKey);
		}
	    }

	    /*
	     * Descend.
	     */

	    if (cKey != COL_CHAR_INVALID && (!mask || (cKey & mask))) {
		existing = WORD_TRIENODE_RIGHT(existing);
	    } else {
		existing = WORD_TRIENODE_LEFT(existing);
	    }
	    continue;

	case WORD_TYPE_TRIELEAF:
	    ASSERT(existing == node);
	    /*
	     * Convert to mutable.
	     */

	    converted = (Col_Word) AllocCells(1);
	    memcpy((void *) converted, (void *) existing, sizeof(Cell));
	    ASSERT(!WORD_PINNED(existing));
	    WORD_SET_TYPEID(converted, WORD_TYPE_MTRIELEAF);
	    *nodePtr = converted;
	    return converted;
	}
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: ConvertIntNodeToMutable
 *
 *	Convert immutable intege node and all all its ancestors to mutable.
 *
 * Arguments:
 *	node	- Node to convert.
 *	map	- Trie map the node belongs to.
 *	prefix	- Key prefix.
 *
 * Result:
 *	The converted mutable entry.
 *---------------------------------------------------------------------------*/

static Col_Word
ConvertIntNodeToMutable(
    Col_Word node, 
    Col_Word map, 
    intptr_t prefix)
{
    Col_Word *nodePtr, existing, converted;
    intptr_t mask;

    ASSERT(WORD_TYPE(node) == WORD_TYPE_INTTRIENODE || WORD_TYPE(node) == WORD_TYPE_INTTRIELEAF);
    nodePtr = &WORD_TRIEMAP_ROOT(map);
    for (;;) {
	existing = *nodePtr;
	switch (WORD_TYPE(existing)) {
	case WORD_TYPE_INTTRIENODE:
	    /*
	     * Convert to mutable.
	     */

	    converted = (Col_Word) AllocCells(1);
	    memcpy((void *) converted, (void *) existing, sizeof(Cell));
	    ASSERT(!WORD_PINNED(existing));
	    WORD_SET_TYPEID(converted, WORD_TYPE_MINTTRIENODE);
	    *nodePtr = converted;
	    if (existing == node) {
		return converted;
	    }
	    existing = converted;
	    /* continued */
	case WORD_TYPE_MINTTRIENODE:
	    ASSERT(existing != node);
	    mask = WORD_INTTRIENODE_MASK(existing);

	    /*
	     * Descend.
	     */

	    if (mask ? (prefix & mask) : (prefix >= 0)) {
		existing = WORD_TRIENODE_RIGHT(existing);
	    } else {
		existing = WORD_TRIENODE_LEFT(existing);
	    }
	    continue;

	case WORD_TYPE_INTTRIELEAF:
	    ASSERT(existing == node);
	    /*
	     * Convert to mutable.
	     */

	    converted = (Col_Word) AllocCells(1);
	    memcpy((void *) converted, (void *) existing, sizeof(Cell));
	    ASSERT(!WORD_PINNED(existing));
	    WORD_SET_TYPEID(converted, WORD_TYPE_MINTTRIELEAF);
	    *nodePtr = converted;
	    return converted;
	}
    }
}


/*
================================================================================
Section: Trie Maps
================================================================================
*/

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

/*---------------------------------------------------------------------------
 * Function: Col_CopyTrieMap
 *
 *	Create a new trie map word from an existing one.
 *
 * Argument:
 *	map	- Trie map to copy.
 *
 * Type checking:
 *	*map* must be a valid trie map.
 *
 * Result:
 *	The new word.
 *
 * Side effects:
 *	Source map content is frozen.
 *---------------------------------------------------------------------------*/

Col_Word
Col_CopyTrieMap(
    Col_Word map)
{
    Col_Word newMap;

    /*
     * Check preconditions.
     */

    TYPECHECK_TRIEMAP(map) return WORD_NIL;

    /*
     * Copy word first.
     */

    //TODO: provide custom data copy mechanism
    newMap = (Col_Word) AllocCells(1);
    memcpy((void *) newMap, (void *) map, sizeof(Cell));
    WORD_SYNONYM(newMap) = WORD_NIL;
    WORD_CLEAR_PINNED(newMap);

    /*
     * Freeze nodes recursively.
     */

    FreezeSubtrie(WORD_TRIEMAP_ROOT(map));

    /*
     * Both maps now share the same immutable structure.
     */

    return newMap;
}


/****************************************************************************
 * Group: Trie Map Accessors
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_TrieMapGet
 *
 *	Get value mapped to the given key if present.
 *
 * Arguments:
 *	map		- Trie map to get entry from.
 *	key		- Entry key. Can be any word type, including string, 
 *			  however it must match the actual type used by the map.
 *	valuePtr	- Returned entry value, if found.
 *
 * Type checking:
 *	*map* must be a valid string or custom trie map.
 *
 * Results:
 *	Whether the key was found in the map. In this case the value is returned
 *	through valuePtr.
 *---------------------------------------------------------------------------*/

int
Col_TrieMapGet(
    Col_Word map,
    Col_Word key,
    Col_Word *valuePtr)
{
    Col_Word entry;

    /*
     * Check preconditions.
     */

    TYPECHECK_WORDTRIEMAP(map) return 0;

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRTRIEMAP:
	entry = StringTrieMapFindEntry(map, key, 0, NULL, NULL, NULL);
	break;
	
    case WORD_TYPE_CUSTOM:
	entry = TrieMapFindEntry(map, key, 0, NULL, NULL, NULL);
	break;

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
    }
    
    if (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_TRIELEAF || WORD_TYPE(entry) == WORD_TYPE_MTRIELEAF);
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
 * Type checking:
 *	*map* must be a valid integer trie map.
 *
 * Results:
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

    /*
     * Check preconditions.
     */

    TYPECHECK_INTTRIEMAP(map) return 0;

    entry = IntTrieMapFindEntry(map, key, 0, NULL, NULL, NULL);
    if (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTTRIELEAF || WORD_TYPE(entry) == WORD_TYPE_MINTTRIELEAF);
	*valuePtr = WORD_MAPENTRY_VALUE(entry);
	return 1;
    } else {
	return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_TrieMapSet
 *
 *	Map the value to the key, replacing any existing.
 *
 * Arguments:
 *	map	- Trie map to insert entry into.
 *	key	- Entry key. Can be any word type, including string, however it
 *		  must match the actual type used by the map.
 *	value	- Entry value.
 *
 * Type checking:
 *	*map* must be a valid string or custom trie map.
 *
 * Result:
 *	Whether a new entry was created.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

int
Col_TrieMapSet(
    Col_Word map,
    Col_Word key,
    Col_Word value)
{
    int create = 1;
    Col_Word entry;

    /*
     * Check preconditions.
     */

    TYPECHECK_WORDTRIEMAP(map) return 0;

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRTRIEMAP:
	entry = StringTrieMapFindEntry(map, key, 1, &create, NULL, NULL);
	break;
	
    case WORD_TYPE_CUSTOM:
	entry = TrieMapFindEntry(map, key, 1, &create, NULL, NULL);
	break;

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
    }
    
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_MTRIELEAF);
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
 * Type checking:
 *	*map* must be a valid integer trie map.
 *
 * Results:
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

    /*
     * Check preconditions.
     */

    TYPECHECK_INTTRIEMAP(map) return 0;

    entry = IntTrieMapFindEntry(map, key, 1, &create, NULL, NULL);
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_MINTTRIELEAF);
    WORD_MAPENTRY_VALUE(entry) = value;
    return create;
}

/*---------------------------------------------------------------------------
 * Function: Col_TrieMapUnset
 *
 *	Remove any value mapped to the given key.
 *
 * Arguments:
 *	map	- Trie map to remove entry from.
 *	key	- Entry key. Can be any word type, including string, however it
 *		  must match the actual type used by the map.
 *
 * Type checking:
 *	*map* must be a valid string or custom trie map.
 *
 * Result:
 *	Whether an existing entry was removed.
 *---------------------------------------------------------------------------*/

int
Col_TrieMapUnset(
    Col_Word map,
    Col_Word key)
{
    Col_Word node, grandParent, parent, sibling;

    /*
     * Check preconditions.
     */

    TYPECHECK_WORDTRIEMAP(map) return 0;

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRTRIEMAP:
	node = StringTrieMapFindNode(map, key, 0, NULL, &grandParent, &parent, 
		NULL, NULL, NULL, NULL);
	break;
	
    case WORD_TYPE_CUSTOM:
	node = TrieMapFindNode(map, key, 0, NULL, &grandParent, &parent, NULL, 
		NULL, NULL, NULL);
	break;

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
    }
    
    if (!node) {
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

	return 1;
    }

    /*
     * Replace parent by sibling.
     */

    ASSERT(WORD_TYPE(parent) == WORD_TYPE_STRTRIENODE || WORD_TYPE(parent) == WORD_TYPE_MSTRTRIENODE);
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
    } else {
	if (WORD_TYPE(grandParent) != WORD_TYPE_MSTRTRIENODE) {
	    /*
	     * Grandparent is immutable, convert first.
	     */

	    ASSERT(WORD_TYPE(grandParent) == WORD_TYPE_STRTRIENODE);
	    grandParent = ConvertStringNodeToMutable(grandParent, map, key);
	}
	ASSERT(WORD_TYPE(grandParent) == WORD_TYPE_MSTRTRIENODE);
	if (WORD_TRIENODE_LEFT(grandParent) == parent) {
	    WORD_TRIENODE_LEFT(grandParent) = sibling;
	} else {
	    WORD_TRIENODE_RIGHT(grandParent) = sibling;
	}
    }

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
 * Type checking:
 *	*map* must be a valid integer trie map.
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

    /*
     * Check preconditions.
     */

    TYPECHECK_INTTRIEMAP(map) return 0;

    node = IntTrieMapFindNode(map, key, 0, NULL, &grandParent, &parent, NULL, 
	    NULL, NULL);
    if (!node) {
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

	return 1;
    }

    /*
     * Replace parent by sibling.
     */

    ASSERT(WORD_TYPE(parent) == WORD_TYPE_INTTRIENODE || WORD_TYPE(parent) == WORD_TYPE_MINTTRIENODE);
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
    } else {
	if (WORD_TYPE(grandParent) != WORD_TYPE_MINTTRIENODE) {
	    /*
	     * Grandparent is immutable, convert first.
	     */

	    ASSERT(WORD_TYPE(grandParent) == WORD_TYPE_INTTRIENODE);
	    grandParent = ConvertIntNodeToMutable(grandParent, map, key);
	}
	ASSERT(WORD_TYPE(grandParent) == WORD_TYPE_MINTTRIENODE);
	if (WORD_TRIENODE_LEFT(grandParent) == parent) {
	    WORD_TRIENODE_LEFT(grandParent) = sibling;
	} else {
	    WORD_TRIENODE_RIGHT(grandParent) = sibling;
	}
    }

    return 1;
}


/****************************************************************************
 * Group: Trie Map Iteration
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
 *
 * Type checking:
 *	*map* must be a valid trie map.
 *---------------------------------------------------------------------------*/

void
Col_TrieMapIterFirst(
    Col_Word map,
    Col_MapIterator *it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_TRIEMAP(map) {
	*it = COL_MAPITER_NULL;
	return;
    }

    it->map = map;
    it->entry = LeftmostLeaf(WORD_TRIEMAP_ROOT(map), &it->traversal.trie.next);
    it->traversal.trie.prev = WORD_NIL;
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
 *
 * Type checking:
 *	*map* must be a valid trie map.
 *---------------------------------------------------------------------------*/

void
Col_TrieMapIterLast(
    Col_Word map,
    Col_MapIterator *it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_TRIEMAP(map) {
	*it = COL_MAPITER_NULL;
	return;
    }

    it->map = map;
    it->entry = RightmostLeaf(WORD_TRIEMAP_ROOT(map), &it->traversal.trie.prev);
    it->traversal.trie.next = WORD_NIL;
}

/*---------------------------------------------------------------------------
 * Function: Col_TrieMapIterFind
 *
 *	Initialize the trie map iterator so that it points to the entry with
 *	the given key within the map.
 *
 * Arguments:
 *	map		- Trie map to find or create entry into.
 *	key		- Entry key. Can be any word type, including string,
 *			  however it must match the actual type used by the map.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *	it		- Iterator to initialize.
 *
 * Type checking:
 *	*map* must be a valid string or custom trie map.
 *
 * Result:
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *---------------------------------------------------------------------------*/

void
Col_TrieMapIterFind(
    Col_Word map, 
    Col_Word key, 
    int *createPtr, 
    Col_MapIterator *it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_WORDTRIEMAP(map) {
	*it = COL_MAPITER_NULL;
	return;
    }

    it->map = map;
    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRTRIEMAP:
	it->entry = StringTrieMapFindEntry(map, key, 0, createPtr, 
		&it->traversal.trie.prev, &it->traversal.trie.next);
	break;
	
    case WORD_TYPE_CUSTOM:
	it->entry = TrieMapFindEntry(map, key, 0, createPtr, 
		&it->traversal.trie.prev, &it->traversal.trie.next);
	break;

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
    }
    
    ASSERT(!it->entry || WORD_TRIEMAP_ROOT(map));
    ASSERT(!it->entry || WORD_TYPE(it->entry) == WORD_TYPE_TRIELEAF || WORD_TYPE(it->entry) == WORD_TYPE_MTRIELEAF);
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
 * Type checking:
 *	*map* must be a valid integer trie map.
 *
 * Result:
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *---------------------------------------------------------------------------*/

void
Col_IntTrieMapIterFind(
    Col_Word map, 
    intptr_t key, 
    int *createPtr, 
    Col_MapIterator *it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_INTTRIEMAP(map) {
	*it = COL_MAPITER_NULL;
	return;
    }

    it->map = map;
    it->entry = IntTrieMapFindEntry(map, key, 0, createPtr, 
	    &it->traversal.trie.prev, &it->traversal.trie.next);
    ASSERT(!it->entry || WORD_TRIEMAP_ROOT(map));
    ASSERT(!it->entry || WORD_TYPE(it->entry) == WORD_TYPE_INTTRIELEAF || WORD_TYPE(it->entry) == WORD_TYPE_MINTTRIELEAF);
}

/*---------------------------------------------------------------------------
 * Function: Col_TrieMapIterSetValue
 *
 *	Set value of trie map iterator.
 *
 * Arguments:
 *	it	- Map iterator to set value for.
 *	value	- Value to set.
 *
 * Type checking:
 *	*it* must be a valid trie map iterator.
 *
 * Range checking:
 *	*it* must not be at end.
 *---------------------------------------------------------------------------*/

void
Col_TrieMapIterSetValue(
    Col_MapIterator *it,
    Col_Word value)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_TRIEMAP(it->map) return;
    RANGECHECK_MAPITER(it) return;

    switch (WORD_TYPE(it->map)) {
    case WORD_TYPE_STRTRIEMAP:
	if (WORD_TYPE(it->entry) != WORD_TYPE_MTRIELEAF) {
	    /*
	     * Entry is immutable, convert to mutable.
	     */

	    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_TRIELEAF);
	    it->entry = ConvertStringNodeToMutable(it->entry, it->map, 
		    WORD_MAPENTRY_KEY(it->entry));
	}

	/*
	 * Set entry value.
	 */

	ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_MTRIELEAF);
	WORD_MAPENTRY_VALUE(it->entry) = value;
	break;

    case WORD_TYPE_INTTRIEMAP:
	if (WORD_TYPE(it->entry) != WORD_TYPE_MINTTRIELEAF) {
	    /*
	     * Entry is immutable, convert to mutable.
	     */

	    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_INTTRIELEAF);
	    it->entry = ConvertIntNodeToMutable(it->entry, it->map, 
		    WORD_INTMAPENTRY_KEY(it->entry));
	}

	/*
	 * Set entry value.
	 */

	ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_MINTTRIELEAF);
	WORD_MAPENTRY_VALUE(it->entry) = value;
	break;

    case WORD_TYPE_CUSTOM:
	if (WORD_TYPE(it->entry) != WORD_TYPE_MTRIELEAF) {
	    /*
	     * Entry is immutable, convert to mutable.
	     */

	    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_TRIELEAF);
	    it->entry = ConvertNodeToMutable(it->entry, it->map, 
		    WORD_MAPENTRY_KEY(it->entry));
	}

	/*
	 * Set entry value.
	 */

	ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_MTRIELEAF);
	WORD_MAPENTRY_VALUE(it->entry) = value;
	break;

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_TrieMapIterNext
 *
 *	Move the iterator to the next element.
 *
 * Argument:
 *	it	- The iterator to move.
 *
 * Type checking:
 *	*it* must be a valid trie map iterator.
 *
 * Range checking:
 *	*it* must not be at end.
 *---------------------------------------------------------------------------*/

void
Col_TrieMapIterNext(
    Col_MapIterator *it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_TRIEMAP(it->map) return;
    RANGECHECK_MAPITER(it) return;

    ASSERT(it->entry);

    if (!it->traversal.trie.next) {
	/*
	 * Refresh shortcuts.
	 */

	switch (WORD_TYPE(it->map)) {
	case WORD_TYPE_STRTRIEMAP:
	    StringTrieMapFindNode(it->map, WORD_MAPENTRY_KEY(it->entry), 0,
		    NULL, NULL, NULL, NULL, &it->traversal.trie.next, NULL,
		    NULL);
	    break;

	case WORD_TYPE_INTTRIEMAP:
	    IntTrieMapFindNode(it->map, WORD_INTMAPENTRY_KEY(it->entry), 0,
		    NULL, NULL, NULL, NULL, &it->traversal.trie.next, NULL);
	    break;

	case WORD_TYPE_CUSTOM:
	    TrieMapFindNode(it->map, WORD_MAPENTRY_KEY(it->entry), 0, NULL, 
		    NULL, NULL, NULL, &it->traversal.trie.next, NULL, NULL);
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
	}
    }

    it->traversal.trie.prev = it->entry;
    it->entry = LeftmostLeaf(it->traversal.trie.next, &it->traversal.trie.next);
}

/*---------------------------------------------------------------------------
 * Function: Col_TrieMapIterPrevious
 *
 *	Move the iterator to the previous element.
 *
 * Argument:
 *	it	- The iterator to move.
 *
 * Type checking:
 *	*it* must be a valid trie map iterator.
 *---------------------------------------------------------------------------*/

void
Col_TrieMapIterPrevious(
    Col_MapIterator *it)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_TRIEMAP(it->map) return;

    if (Col_MapIterEnd(it)) {
	/*
	 * Allow iterators at end to go back.
	 */

	Col_TrieMapIterLast(it->map, it);
	return;
    }

    if (!it->traversal.trie.prev) {
	/*
	 * Refresh shortcuts.
	 */

	switch (WORD_TYPE(it->map)) {
	case WORD_TYPE_STRTRIEMAP:
	    StringTrieMapFindNode(it->map, WORD_MAPENTRY_KEY(it->entry), 0,
		    NULL, NULL, NULL, &it->traversal.trie.prev, NULL, NULL,
		    NULL);
	    break;

	case WORD_TYPE_INTTRIEMAP:
	    IntTrieMapFindNode(it->map, WORD_INTMAPENTRY_KEY(it->entry), 0,
		    NULL, NULL, NULL, &it->traversal.trie.prev, NULL, NULL);
	    break;

	case WORD_TYPE_CUSTOM:
	    TrieMapFindNode(it->map, WORD_MAPENTRY_KEY(it->entry), 0, NULL, 
		    NULL, NULL, &it->traversal.trie.prev, NULL, NULL, NULL);
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
	}
    }

    it->traversal.trie.next  = it->entry;
    it->entry = RightmostLeaf(it->traversal.trie.prev, 
	    &it->traversal.trie.prev);
}


/*
================================================================================
Section: Custom Trie Maps
================================================================================
*/

/****************************************************************************
 * Group: Custom Trie Map Creation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_NewCustomTrieMap
 *
 *	Create a new custom trie map word.
 *
 * Arguments:
 *	type	- The trie map word type.
 *	size	- Size of custom data.
 *	dataPtr	- Will hold a pointer to the allocated data.
 *
 * Result:
  *	A new custom trie map word of the given size. Additionally:
 *
 *	dataPtr	- Points to the allocated custom data.
*
 * Side effects:
 *	Allocates memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewCustomTrieMap(
    Col_CustomTrieMapType *type,
    size_t size,
    void **dataPtr)
{
    Col_Word map;
    
    ASSERT(type->type.type == COL_TRIEMAP);

    map = (Col_Word) AllocCells(WORD_CUSTOM_SIZE(&type->type, size));
    WORD_TRIEMAP_INIT(map, type);

    return map;
}
