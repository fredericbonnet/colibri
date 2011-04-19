#include "colibri.h"
#include "colInternal.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>


/*
 *---------------------------------------------------------------------------
 *
 * Col_NewStringTrieMap --
 *
 *	Create a new trie map word.
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
Col_NewStringTrieMap()
{
    Col_Word map;
    
    map = (Col_Word) AllocCells(1);
    WORD_STRTRIEMAP_INIT(map);

    return map;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_StringTrieMapGet --
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
Col_StringTrieMapGet(
    Col_Word map,		/* Trie map to get entry from. */ 
    Col_Word key, 		/* Entry key. */
    Col_Word *valuePtr)		/* Pointer to entry value, if found. */
{
    Col_Word entry = Col_StringTrieMapFindEntry(map, key, NULL);
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
 * Col_StringTrieMapSet --
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
Col_StringTrieMapSet(
    Col_Word map, 		/* Trie map to insert entry into. */
    Col_Word key, 		/* Entry key. */
    Col_Word value)		/* Entry value. */
{
    int create = 1;
    Col_Word entry = Col_StringTrieMapFindEntry(map, key, &create);
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_MAPENTRY);
    Col_WordSetModified(entry);
    WORD_MAPENTRY_VALUE(entry) = value;
    return create;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_StringTrieMapUnset --
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
Col_StringTrieMapUnset(
    Col_Word map, 		/* Trie map to remove entry from. */
    Col_Word key) 		/* Entry key. */
{
    Col_Word node, *nodePtr, sibling, grandParent, *parentPtr, prev;
    Col_Char mask;
    Col_RopeIterator itKey;

    if (WORD_TYPE(map) != WORD_TYPE_STRTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not a string trie map", map);
	return 0;
    }

    /*
     * Search for matching entry.
     */

    parentPtr = NULL;
    nodePtr = &WORD_TRIEMAP_ROOT(map);
    prev = WORD_NIL;
    Col_RopeIterBegin(key, 0, &itKey);
    while (*nodePtr) {
	node = *nodePtr;
	if (WORD_TYPE(node) == WORD_TYPE_STRTRIENODE) {
	    Col_Char c = COL_CHAR_INVALID;
	    if (!Col_RopeIterEnd(&itKey)) {
		Col_RopeIterMoveTo(&itKey, WORD_STRTRIENODE_DIFF(node));
		if (!Col_RopeIterEnd(&itKey)) {
		    c = Col_RopeIterAt(&itKey);
		}
	    }
	    if (parentPtr) {
		grandParent = *parentPtr;
	    } else {
		grandParent = map;
	    }
	    parentPtr = nodePtr;
	    if (!Col_RopeIterEnd(&itKey)) {
		Col_RopeIterMoveTo(&itKey, WORD_STRTRIENODE_DIFF(node));
	    }
	    mask = WORD_STRTRIENODE_MASK(node);
	    if (c != COL_CHAR_INVALID && (!mask || (c & mask))) {
		/*
		 * Recurse on right, remember left for later deletion from 
		 * chain.
		 */

		prev = WORD_TRIENODE_LEFT(node);
		nodePtr = &WORD_TRIENODE_RIGHT(node);
		sibling = WORD_TRIENODE_LEFT(node);
	    } else {
		/*
		 * Recurse on left.
		 */

		nodePtr = &WORD_TRIENODE_LEFT(node);
		sibling = WORD_TRIENODE_RIGHT(node);
	    }
	    continue;
	}

	ASSERT(WORD_TYPE(node) == WORD_TYPE_MAPENTRY);
	if (Col_CompareRopes(key, WORD_MAPENTRY_KEY(node), 0, SIZE_MAX, NULL,
		NULL, NULL) == 0) {
	    /*
	     * Found!
	     */

	    if (parentPtr) {
		/*
		 * Replace parent by sibling.
		 */

		ASSERT(grandParent);
		*parentPtr = sibling;
		Col_WordSetModified(grandParent);
	    } else {
		/*
		 * Entry was root.
		 */

		WORD_TRIEMAP_ROOT(map) = WORD_NIL;
	    }

	    /*
	     * Remove from chain.
	     */

	    if (prev) {
		/*
		 * Remove after rightmost adjacent entry.
		 */

		while (WORD_TYPE(prev) == WORD_TYPE_STRTRIENODE) {
		    prev = WORD_TRIENODE_RIGHT(prev);
		}
		ASSERT(WORD_TYPE(prev) == WORD_TYPE_MAPENTRY);
		WORD_MAPENTRY_NEXT(prev) = WORD_MAPENTRY_NEXT(node);
		Col_WordSetModified(prev);
	    }

	    WORD_TRIEMAP_SIZE(map)--;
	    return 1;
	}
	break;
    }

    /*
     * Not found.
     */

    return 0;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_StringTrieMapFindEntry --
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
Col_StringTrieMapFindEntry(
    Col_Word map, 		/* Integer trie map to find or create entry 
				 * into. */
    Col_Word key, 		/* Entry key. */
    int *createPtr)		/* In: if non-NULL, whether to create entry if 
				 * absent.
				 * Out: if non-NULL, whether a new entry was 
				 * created. */
{
    Col_Word node, entry, parent, *nodePtr, prev, next;
    Col_RopeIterator itKey;
    Col_Word entryKey = WORD_NIL;
    size_t diff;
    Col_Char cKey, cEntryKey, mask, newMask;
    
    if (WORD_TYPE(map) != WORD_TYPE_STRTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not a string trie map", map);
	return WORD_NIL;
    }

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
	 * Entry node.
	 */

	ASSERT(WORD_TYPE(node) == WORD_TYPE_MAPENTRY);
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

    entry = (Col_Word) AllocCells(1);
    WORD_MAPENTRY_INIT(entry, WORD_NIL, key, WORD_NIL, 0);

    if (!WORD_TRIEMAP_ROOT(map)) {
	/*
	 * First entry.
	 */

	ASSERT(WORD_TRIEMAP_ROOT(map) == WORD_NIL);
	ASSERT(WORD_TRIEMAP_SIZE(map) == 0);
	WORD_TRIEMAP_ROOT(map) = entry;
	Col_WordSetModified(map);

	WORD_TRIEMAP_SIZE(map)++;

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
    prev = WORD_NIL;
    next = WORD_NIL;
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
		 * Recurse on right, remember left for later insertion in chain.
		 */

		prev = WORD_TRIENODE_LEFT(node);
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
     * Insert entry here.
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

	prev = *nodePtr;
	WORD_STRTRIENODE_INIT(node, diff, newMask, *nodePtr, entry);
    } else {
	/*
	 * Entry is left.
	 */

	next = *nodePtr;
	WORD_STRTRIENODE_INIT(node, diff, newMask, entry, *nodePtr);
    }
    *nodePtr = node;
    Col_WordSetModified(parent);

    /*
     * Insert in chain.
     */

    if (prev) {
	/*
	 * Insert after rightmost adjacent entry.
	 */

	while (WORD_TYPE(prev) == WORD_TYPE_STRTRIENODE) {
	    prev = WORD_TRIENODE_RIGHT(prev);
	}
	ASSERT(WORD_TYPE(prev) == WORD_TYPE_MAPENTRY);
	WORD_MAPENTRY_NEXT(entry) = WORD_MAPENTRY_NEXT(prev);
	WORD_MAPENTRY_NEXT(prev) = entry;
	Col_WordSetModified(prev);
    } else {
	/*
	 * Entry is first, insert before leftmost adjacent entry.
	 */

	ASSERT(next);
	while (WORD_TYPE(next) == WORD_TYPE_STRTRIENODE) {
	    next = WORD_TRIENODE_LEFT(next);
	}
	ASSERT(WORD_TYPE(next) == WORD_TYPE_MAPENTRY);
	WORD_MAPENTRY_NEXT(entry) = next;
    }

    WORD_TRIEMAP_SIZE(map)++;

    return entry;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewIntTrieMap --
 *
 *	Create a new integer trie map word.
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
Col_NewIntTrieMap()
{
    Col_Word map;
    
    map = (Col_Word) AllocCells(1);
    WORD_INTTRIEMAP_INIT(map);

    return map;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_IntTrieMapGet --
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
Col_IntTrieMapGet(
    Col_Word map,		/* Integer trie map to get entry from. */ 
    int key, 			/* Entry key. */
    Col_Word *valuePtr)		/* Pointer to entry value, if found. */
{
    Col_Word entry = Col_IntTrieMapFindEntry(map, key, NULL);
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
 * Col_IntTrieMapSet --
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
Col_IntTrieMapSet(
    Col_Word map, 		/* Integer trie map to insert entry into. */
    int key, 			/* Entry key. */
    Col_Word value)		/* Entry value. */
{
    int create = 1;
    Col_Word entry = Col_IntTrieMapFindEntry(map, key, &create);
    ASSERT(entry);
    ASSERT(WORD_TYPE(entry) == WORD_TYPE_INTMAPENTRY);
    Col_WordSetModified(entry);
    WORD_MAPENTRY_VALUE(entry) = value;
    return create;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_IntTrieMapUnset --
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
Col_IntTrieMapUnset(
    Col_Word map, 		/* Integer trie map to remove entry from. */
    int key) 			/* Entry key. */
{
    Col_Word node, *nodePtr, sibling, grandParent, *parentPtr, prev;
    int mask;

    if (WORD_TYPE(map) != WORD_TYPE_INTTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not an integer trie map", map);
	return 0;
    }

    /*
     * Search for matching entry.
     */

    parentPtr = NULL;
    nodePtr = &WORD_TRIEMAP_ROOT(map);
    prev = WORD_NIL;
    while (*nodePtr) {
	node = *nodePtr;
	if (WORD_TYPE(node) == WORD_TYPE_INTTRIENODE) {
	    if (parentPtr) {
		grandParent = *parentPtr;
	    } else {
		grandParent = map;
	    }
	    parentPtr = nodePtr;
	    mask = WORD_INTTRIENODE_MASK(node);
	    if (mask ? (key & mask) : (key >= 0)) {
		/*
		 * Recurse on right, remember left for later deletion from 
		 * chain.
		 */

		prev = WORD_TRIENODE_LEFT(node);
		nodePtr = &WORD_TRIENODE_RIGHT(node);
		sibling = WORD_TRIENODE_LEFT(node);
	    } else {
		/*
		 * Recurse on left.
		 */

		nodePtr = &WORD_TRIENODE_LEFT(node);
		sibling = WORD_TRIENODE_RIGHT(node);
	    }
	    continue;
	}

	ASSERT(WORD_TYPE(node) == WORD_TYPE_INTMAPENTRY);
	if (key == WORD_INTMAPENTRY_KEY(node)) {
	    /*
	     * Found!
	     */

	    if (parentPtr) {
		/*
		 * Replace parent by sibling.
		 */

		ASSERT(grandParent);
		*parentPtr = sibling;
		Col_WordSetModified(grandParent);
	    } else {
		/*
		 * Entry was root.
		 */

		WORD_TRIEMAP_ROOT(map) = WORD_NIL;
	    }

	    /*
	     * Remove from chain.
	     */

	    if (prev) {
		/*
		 * Remove after rightmost adjacent entry.
		 */

		while (WORD_TYPE(prev) == WORD_TYPE_INTTRIENODE) {
		    prev = WORD_TRIENODE_RIGHT(prev);
		}
		ASSERT(WORD_TYPE(prev) == WORD_TYPE_INTMAPENTRY);
		WORD_MAPENTRY_NEXT(prev) = WORD_MAPENTRY_NEXT(node);
		Col_WordSetModified(prev);
	    }

	    WORD_TRIEMAP_SIZE(map)--;
	    return 1;
	}
	break;
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
Col_IntTrieMapFindEntry(
    Col_Word map, 		/* Integer trie map to find or create entry 
				 * into. */
    int key, 			/* Entry key. */
    int *createPtr)		/* In: if non-NULL, whether to create entry if 
				 * absent.
				 * Out: if non-NULL, whether a new entry was 
				 * created. */
{
    Col_Word node, entry, parent, *nodePtr, prev, next;
    int entryKey = WORD_NIL, mask, newMask;
    
    if (WORD_TYPE(map) != WORD_TYPE_INTTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not an integer trie map", map);
	return WORD_NIL;
    }

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
	 * Entry node.
	 */

	ASSERT(WORD_TYPE(node) == WORD_TYPE_INTMAPENTRY);
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

    entry = (Col_Word) AllocCells(1);
    WORD_INTMAPENTRY_INIT(entry, WORD_NIL, key, WORD_NIL);

    if (!WORD_TRIEMAP_ROOT(map)) {
	/*
	 * First entry.
	 */

	ASSERT(WORD_TRIEMAP_ROOT(map) == WORD_NIL);
	ASSERT(WORD_TRIEMAP_SIZE(map) == 0);
	WORD_TRIEMAP_ROOT(map) = entry;
	Col_WordSetModified(map);

	WORD_TRIEMAP_SIZE(map)++;

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
    prev = WORD_NIL;
    next = WORD_NIL;
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
		 * Recurse on right, remember left for later insertion in chain.
		 */

		prev = WORD_TRIENODE_LEFT(node);
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
     * Insert entry here.
     */

    node = (Col_Word) AllocCells(1);
    if (newMask ? (key & newMask) : (key >= 0)) {
	/*
	 * Entry is right.
	 */

	prev = *nodePtr;
	WORD_INTTRIENODE_INIT(node, newMask, *nodePtr, entry);
    } else {
	/*
	 * Entry is left.
	 */

	next = *nodePtr;
	WORD_INTTRIENODE_INIT(node, newMask, entry, *nodePtr);
    }
    *nodePtr = node;
    Col_WordSetModified(parent);

    /*
     * Insert in chain.
     */

    if (prev) {
	/*
	 * Insert after rightmost adjacent entry.
	 */

	while (WORD_TYPE(prev) == WORD_TYPE_INTTRIENODE) {
	    prev = WORD_TRIENODE_RIGHT(prev);
	}
	ASSERT(WORD_TYPE(prev) == WORD_TYPE_INTMAPENTRY);
	WORD_MAPENTRY_NEXT(entry) = WORD_MAPENTRY_NEXT(prev);
	WORD_MAPENTRY_NEXT(prev) = entry;
	Col_WordSetModified(prev);
    } else {
	/*
	 * Entry is first, insert before leftmost adjacent entry.
	 */

	ASSERT(next);
	while (WORD_TYPE(next) == WORD_TYPE_INTTRIENODE) {
	    next = WORD_TRIENODE_LEFT(next);
	}
	ASSERT(WORD_TYPE(next) == WORD_TYPE_INTMAPENTRY);
	WORD_MAPENTRY_NEXT(entry) = next;
    }

    WORD_TRIEMAP_SIZE(map)++;

    return entry;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_TrieMapIterBegin --
 *
 *	Initialize the trie map iterator so that it points to the first entry
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
Col_TrieMapIterBegin(
    Col_Word map,		/* Map to iterate over. */
    Col_MapIterator *it)	/* Iterator to initialize. */
{
    Col_Word node;

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

    /*
     * Get leftmost entry.
     */

    it->map = map;
    node = WORD_TRIEMAP_ROOT(map);
    ASSERT(node);
    for (;;) {
	switch (WORD_TYPE(node)) {
	    case WORD_TYPE_STRTRIENODE:
	    case WORD_TYPE_INTTRIENODE:
		node = WORD_TRIENODE_LEFT(node);
		break;

	    default:
		it->entry = node;
		return;
	}
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_TrieMapIterNext --
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
Col_TrieMapIterNext(
    Col_MapIterator *it)	/* The iterator to move. */
{
    if (Col_MapIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid map iterator");
	return;
    }

    /*
     * Get next entry in chain.
     */

    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_MAPENTRY 
	    || WORD_TYPE(it->entry) == WORD_TYPE_INTMAPENTRY);
    it->entry = WORD_MAPENTRY_NEXT(it->entry);
    if (!it->entry) {
	/*
	 * End of map.
	 */

	it->map = WORD_NIL;
    }
}
