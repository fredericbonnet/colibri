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

static Col_Word		LeftmostLeaf(Col_Word node);
static Col_Word		RightmostLeaf(Col_Word node);
static Col_Word		StringTrieMapFindEntry(Col_Word map, Col_Word key,
			    int mutable, int *createPtr);
static Col_Word		IntTrieMapFindEntry(Col_Word map, intptr_t key,
			    int mutable, int *createPtr);


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
 * Result:
 *	The leftmost leaf of subtrie.
 *---------------------------------------------------------------------------*/

static Col_Word
LeftmostLeaf(
     Col_Word node) 
{
    for (;;) {
	switch (WORD_TYPE(node)) {
	    case WORD_TYPE_STRTRIENODE:
	    case WORD_TYPE_INTTRIENODE:
		node = WORD_TRIENODE_LEFT(node);
		break;

	    default:
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
 * Result:
 *	The rightmost leaf of subtrie.
 *---------------------------------------------------------------------------*/

static Col_Word
RightmostLeaf(
     Col_Word node) 
{
    for (;;) {
	switch (WORD_TYPE(node)) {
	    case WORD_TYPE_STRTRIENODE:
	    case WORD_TYPE_INTTRIENODE:
		node = WORD_TRIENODE_RIGHT(node);
		break;

	    default:
		return node;
	}
    }
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
 *	The entry if found or created, else nil. Additionally:
 *
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *---------------------------------------------------------------------------*/

static Col_Word
StringTrieMapFindEntry(
    Col_Word map,
    Col_Word key,
    int mutable,
    int *createPtr)
{
    Col_Word node, entry, parent, *nodePtr, rightmost;
    Col_RopeIterator itKey;
    Col_Word entryKey = WORD_NIL;
    size_t diff;
    Col_Char cKey, cEntryKey, mask, newMask;
    
    /*
     * Search for matching entry.
     */

    node = WORD_TRIEMAP_ROOT(map);
    Col_RopeIterBegin(key, 0, &itKey);
    while (node) {
	if (WORD_TYPE(node) == WORD_TYPE_STRTRIENODE) {
	    mask = WORD_STRTRIENODE_MASK(node);
	    cKey = COL_CHAR_INVALID;
	    if (!Col_RopeIterEnd(&itKey)) {
		Col_RopeIterMoveTo(&itKey, WORD_STRTRIENODE_DIFF(node));
		if (!Col_RopeIterEnd(&itKey)) {
		    cKey = Col_RopeIterAt(&itKey);
		}
	    }
	    if (cKey != COL_CHAR_INVALID && (!mask || (cKey & mask))) {
		/*
		 * Recurse on right.
		 */

		node = WORD_TRIENODE_RIGHT(node);
	    } else {
		/*
		 * Recurse on left.
		 */

		node = WORD_TRIENODE_LEFT(node);
	    }
	    continue;
	}

	/*
	 * Leaf node.
	 */

	ASSERT(WORD_TYPE(node) == WORD_TYPE_TRIELEAF);
	entryKey = WORD_MAPENTRY_KEY(node);
	if (Col_CompareRopes(key, entryKey, 0, SIZE_MAX, &diff, &cKey, &cEntryKey) 
		== 0) {
	    /*
	     * Found!
	     */

	    if (createPtr) *createPtr = 0;
	    return node;
	}
	break;
    }

    /*
     * Not found, create new if asked for.
     */

    if (!createPtr || !*createPtr) {
	return WORD_NIL;
    }
    *createPtr = 1;

    WORD_TRIEMAP_SIZE(map)++;

    entry = (Col_Word) AllocCells(1);
    WORD_TRIELEAF_INIT(entry, key, WORD_NIL, WORD_NIL);

    if (!WORD_TRIEMAP_ROOT(map)) {
	/*
	 * First entry.
	 */

	ASSERT(WORD_TRIEMAP_ROOT(map) == WORD_NIL);
	ASSERT(WORD_TRIEMAP_SIZE(map) == 1);
	WORD_TRIEMAP_ROOT(map) = entry;
	Col_WordSetModified(map);

	return entry;
    }
    ASSERT(node);

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
     * Find insertion point.
     */

    nodePtr = &WORD_TRIEMAP_ROOT(map);
    parent = map;
    Col_RopeIterBegin(key, 0, &itKey);
    while (*nodePtr) {
	node = *nodePtr;
	if (WORD_TYPE(node) == WORD_TYPE_STRTRIENODE) {
	    mask = WORD_STRTRIENODE_MASK(node);
	    if (diff < WORD_STRTRIENODE_DIFF(node) 
		    || (diff == WORD_STRTRIENODE_DIFF(node) 
		    && (mask && newMask > mask))) {
		break;
	    }
	    parent = node;
	    cKey = COL_CHAR_INVALID;
	    if (!Col_RopeIterEnd(&itKey)) {
		Col_RopeIterMoveTo(&itKey, WORD_STRTRIENODE_DIFF(node));
		if (!Col_RopeIterEnd(&itKey)) {
		    cKey = Col_RopeIterAt(&itKey);
		}
	    }
	    if (cKey != COL_CHAR_INVALID && (!mask || (cKey & mask))) {
		/*
		 * Recurse on right.
		 */

		nodePtr = &WORD_TRIENODE_RIGHT(node);
	    } else {
		/*
		 * Recurse on left.
		 */

		nodePtr = &WORD_TRIENODE_LEFT(node);
	    }
	    continue;
	}

	break;
    }

    /*
     * Insert node here.
     */

    node = (Col_Word) AllocCells(1);
    cKey = COL_CHAR_INVALID;
    if (!Col_RopeIterEnd(&itKey)) {
	Col_RopeIterMoveTo(&itKey, diff);
	if (!Col_RopeIterEnd(&itKey)) {
	    cKey = Col_RopeIterAt(&itKey);
	}
    }
    if (cKey != COL_CHAR_INVALID && (!newMask || (cKey & newMask))) {
	/*
	 * Entry is right.
	 */

	WORD_STRTRIENODE_INIT(node, diff, newMask, *nodePtr, entry);

	/*
	 * Get rightmost leaf of left subtrie. Entry's ancestor is the current
	 * rightmost's ancestor. New rightmost ancestor's is new node.
	 */

	rightmost = RightmostLeaf(*nodePtr);
	ASSERT(WORD_TYPE(rightmost) == WORD_TYPE_TRIELEAF);
	WORD_TRIELEAF_UP(entry) = WORD_TRIELEAF_UP(rightmost);
	WORD_TRIELEAF_UP(rightmost) = node;
    } else {
	/*
	 * Entry is left.
	 */

	WORD_STRTRIENODE_INIT(node, diff, newMask, entry, *nodePtr);

	/*
	 * Entry ancestor is new node.
	 */

	WORD_TRIELEAF_UP(entry) = node;
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
 *	The entry if found or created, else nil. Additionally:
 *
 *	createPtr	- (out) If non-NULL, whether a new entry was created. 
 *---------------------------------------------------------------------------*/

static Col_Word
IntTrieMapFindEntry(
    Col_Word map,
    intptr_t key,
    int mutable,
    int *createPtr)
{
    Col_Word node, entry, parent, *nodePtr, rightmost;
    intptr_t entryKey = WORD_NIL, mask, newMask;
    
    /*
     * Search for matching entry.
     */

    node = WORD_TRIEMAP_ROOT(map);
    while (node) {
	if (WORD_TYPE(node) == WORD_TYPE_INTTRIENODE) {
	    mask = WORD_INTTRIENODE_MASK(node);
	    if (mask ? (key & mask) : (key >= 0)) {
		/*
		 * Recurse on right.
		 */

		node = WORD_TRIENODE_RIGHT(node);
	    } else {
		/*
		 * Recurse on left.
		 */

		node = WORD_TRIENODE_LEFT(node);
	    }
	    continue;
	}

	/*
	 * Leaf node.
	 */

	ASSERT(WORD_TYPE(node) == WORD_TYPE_INTTRIELEAF);
	entryKey = WORD_INTMAPENTRY_KEY(node);
	if (key == entryKey) {
	    /*
	     * Found!
	     */

	    if (createPtr) *createPtr = 0;
	    return node;
	}
	break;
    }

    /*
     * Not found, create new if asked for.
     */

    if (!createPtr || !*createPtr) {
	return WORD_NIL;
    }
    *createPtr = 1;

    WORD_TRIEMAP_SIZE(map)++;

    entry = (Col_Word) AllocCells(1);
    WORD_INTTRIELEAF_INIT(entry, key, WORD_NIL, WORD_NIL);

    if (!WORD_TRIEMAP_ROOT(map)) {
	/*
	 * First entry.
	 */

	ASSERT(WORD_TRIEMAP_ROOT(map) == WORD_NIL);
	ASSERT(WORD_TRIEMAP_SIZE(map) == 1);
	WORD_TRIEMAP_ROOT(map) = entry;
	Col_WordSetModified(map);

	return entry;
    }
    ASSERT(node);

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
     * Find insertion point.
     */

    nodePtr = &WORD_TRIEMAP_ROOT(map);
    parent = map;
    while (*nodePtr) {
	node = *nodePtr;
	if (WORD_TYPE(node) == WORD_TYPE_INTTRIENODE) {
	    mask = WORD_INTTRIENODE_MASK(node);
	    if (mask && newMask > mask) {
		break;
	    }
	    parent = node;
	    if (mask ? (key & mask) : (key >= 0)) {
		/*
		 * Recurse on right.
		 */

		nodePtr = &WORD_TRIENODE_RIGHT(node);
	    } else {
		/*
		 * Recurse on left.
		 */

		nodePtr = &WORD_TRIENODE_LEFT(node);
	    }
	    continue;
	}

	break;
    }

    /*
     * Insert node here.
     */

    node = (Col_Word) AllocCells(1);
    if (newMask ? (key & newMask) : (key >= 0)) {
	/*
	 * Entry is right.
	 */

	WORD_INTTRIENODE_INIT(node, newMask, *nodePtr, entry);

	/*
	 * Get rightmost leaf of left subtrie. Entry's ancestor is the current
	 * rightmost's ancestor. New rightmost's ancestor is new node.
	 */

	rightmost = RightmostLeaf(*nodePtr);
	ASSERT(WORD_TYPE(rightmost) == WORD_TYPE_INTTRIELEAF);
	WORD_TRIELEAF_UP(entry) = WORD_TRIELEAF_UP(rightmost);
	WORD_TRIELEAF_UP(rightmost) = node;
    } else {
	/*
	 * Entry is left.
	 */

	WORD_INTTRIENODE_INIT(node, newMask, entry, *nodePtr);

	/*
	 * Entry ancestor is new node.
	 */

	WORD_TRIELEAF_UP(entry) = node;
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

    entry = StringTrieMapFindEntry(map, key, 0, NULL);
    if (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_TRIELEAF);
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

    entry = IntTrieMapFindEntry(map, key, 0, NULL);
    if (entry) {
	ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTTRIELEAF);
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

    entry = StringTrieMapFindEntry(map, key, 1, &create);
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_TRIELEAF);
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

    entry = IntTrieMapFindEntry(map, key, 1, &create);
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTTRIELEAF);
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
    Col_Word node, grandParent, parent, sibling, rightmost;
    Col_Char mask;
    Col_RopeIterator itKey;

    if (WORD_TYPE(map) != WORD_TYPE_STRTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not a string trie map", map);
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
	if (WORD_TYPE(node) == WORD_TYPE_STRTRIENODE) {
	    Col_Char c = COL_CHAR_INVALID;

	    grandParent = parent;
	    parent = node;

	    if (!Col_RopeIterEnd(&itKey)) {
		Col_RopeIterMoveTo(&itKey, WORD_STRTRIENODE_DIFF(node));
		if (!Col_RopeIterEnd(&itKey)) {
		    c = Col_RopeIterAt(&itKey);
		}
	    }
	    if (!Col_RopeIterEnd(&itKey)) {
		Col_RopeIterMoveTo(&itKey, WORD_STRTRIENODE_DIFF(node));
	    }
	    mask = WORD_STRTRIENODE_MASK(node);
	    if (c != COL_CHAR_INVALID && (!mask || (c & mask))) {
		/*
		 * Recurse on right.
		 */

		node = WORD_TRIENODE_RIGHT(node);
	    } else {
		/*
		 * Recurse on left.
		 */

		node = WORD_TRIENODE_LEFT(node);
	    }
	    continue;
	}

	/*
	 * Leaf node.
	 */

	ASSERT(WORD_TYPE(node) == WORD_TYPE_TRIELEAF);
	if (Col_CompareRopes(key, WORD_MAPENTRY_KEY(node), 0, SIZE_MAX, NULL,
		NULL, NULL) == 0) {
	    /*
	     * Found!
	     */

	    WORD_TRIEMAP_SIZE(map)--;

	    if (!grandParent) {
		/*
		 * Entry was root.
		 */

		ASSERT(parent == map);
		ASSERT(WORD_TRIEMAP_SIZE(map) == 0);
		WORD_TRIEMAP_ROOT(map) = WORD_NIL;
		Col_WordSetModified(map);
	    } else {
		ASSERT(WORD_TYPE(parent) == WORD_TYPE_STRTRIENODE);
		if (node == WORD_TRIENODE_RIGHT(parent)) {
		    /*
		     * Entry was right.
		     */

		    sibling = WORD_TRIENODE_LEFT(parent);

		    /*
		     * Get rightmost leaf of sibling. Its new ancestor is the
		     * entry's ancestor.
		     */

		    rightmost = RightmostLeaf(sibling);
		    ASSERT(WORD_TYPE(rightmost) == WORD_TYPE_TRIELEAF);
		    WORD_TRIELEAF_UP(rightmost) = WORD_TRIELEAF_UP(node);
		} else {
		    /*
		     * Entry was left.
		     */

		    sibling = WORD_TRIENODE_RIGHT(parent);
		}

		/*
		 * Replace parent by sibling.
		 */

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
	    }
	    return 1;
	}
	break;
    }

    /*
     * Not found.
     */

    return 0;
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
    Col_Word node, grandParent, parent, sibling, rightmost;
    intptr_t mask;

    if (WORD_TYPE(map) != WORD_TYPE_INTTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not an integer trie map", map);
	return 0;
    }

    /*
     * Search for matching entry.
     */

    grandParent = WORD_NIL;
    parent = map;
    node = WORD_TRIEMAP_ROOT(map);
    while (node) {
	if (WORD_TYPE(node) == WORD_TYPE_INTTRIENODE) {
	    grandParent = parent;
	    parent = node;

	    mask = WORD_INTTRIENODE_MASK(node);
	    if (mask ? (key & mask) : (key >= 0)) {
		/*
		 * Recurse on right.
		 */

		node = WORD_TRIENODE_RIGHT(node);
	    } else {
		/*
		 * Recurse on left.
		 */

		node = WORD_TRIENODE_LEFT(node);
	    }
	    continue;
	}

	/*
	 * Leaf node.
	 */

	ASSERT(WORD_TYPE(node) == WORD_TYPE_INTTRIELEAF);
	if (key == WORD_INTMAPENTRY_KEY(node)) {
	    /*
	     * Found!
	     */

	    WORD_TRIEMAP_SIZE(map)--;

	    if (!grandParent) {
		/*
		 * Entry was root.
		 */

		ASSERT(parent == map);
		ASSERT(WORD_TRIEMAP_SIZE(map) == 0);
		WORD_TRIEMAP_ROOT(map) = WORD_NIL;
		Col_WordSetModified(map);
	    } else {
		ASSERT(WORD_TYPE(parent) == WORD_TYPE_INTTRIENODE);
		if (node == WORD_TRIENODE_RIGHT(parent)) {
		    /*
		     * Entry was right.
		     */

		    sibling = WORD_TRIENODE_LEFT(parent);

		    /*
		     * Get rightmost leaf of sibling. Its new ancestor is the
		     * entry's ancestor.
		     */

		    rightmost = RightmostLeaf(sibling);
		    ASSERT(WORD_TYPE(rightmost) == WORD_TYPE_INTTRIELEAF);
		    WORD_TRIELEAF_UP(rightmost) = WORD_TRIELEAF_UP(node);
		} else {
		    /*
		     * Entry was left.
		     */

		    sibling = WORD_TRIENODE_RIGHT(parent);
		}

		/*
		 * Replace parent by sibling.
		 */

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
	    }
	    return 1;
	}
	break;
    }

    /*
     * Not found.
     */

    return 0;
}


/****************************************************************************
 * Group: Trie Map Iterators
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_TrieMapIterBegin
 *
 *	Initialize the trie map iterator so that it points to the first entry
 *	within the map.
 *
 * Arguments:
 *	map	- Trie map to iterate over.
 *	it	- Iterator to initialize.
 *---------------------------------------------------------------------------*/

void
Col_TrieMapIterBegin(
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
    it->entry = LeftmostLeaf(WORD_TRIEMAP_ROOT(map));
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

    it->entry = StringTrieMapFindEntry(map, key, 0, createPtr);
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

    it->entry = IntTrieMapFindEntry(map, key, 0, createPtr);
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

    switch (WORD_TYPE(it->map)) {
	case WORD_TYPE_STRTRIEMAP:
	    //TODO handle immutable subtries.

	    /*
	     * Set entry value.
	     */

	    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_TRIELEAF);
	    WORD_MAPENTRY_VALUE(it->entry) = value;
	    Col_WordSetModified(it->entry);
	    break;

	case WORD_TYPE_INTTRIEMAP:
	    //TODO handle immutable subtries.

	    /*
	     * Set entry value.
	     */

	    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_INTTRIELEAF);
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

    if (!WORD_TRIELEAF_UP(it->entry)) {
	/*
	 * End of map.
	 */

	it->map = WORD_NIL;
	return;
    }

    /*
     * Next entry is leftmost leaf of ancestor's right subtrie.
     */

    ASSERT(WORD_TYPE(WORD_TRIELEAF_UP(it->entry)) == WORD_TYPE_STRTRIENODE
	    || WORD_TYPE(WORD_TRIELEAF_UP(it->entry)) == WORD_TYPE_INTTRIENODE)
    it->entry = LeftmostLeaf(WORD_TRIENODE_RIGHT(
	    WORD_TRIELEAF_UP(it->entry)));
    ASSERT(it->entry);
}
