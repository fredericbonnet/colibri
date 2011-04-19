#include "colibri.h"
#include "colInternal.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>


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
    Col_Word node;

    if (WORD_TYPE(map) != WORD_TYPE_INTTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not an integer trie map", map);
	return 0;
    }

    /*
     * Search for matching entry.
     */

    node = WORD_TRIEMAP_ROOT(map);
    while (node) {
	if (WORD_TYPE(node) == WORD_TYPE_TRIENODE) {
	    if (key & WORD_TRIENODE_MASK(node)) {
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
	if (key == WORD_INTTRIELEAF_KEY(node)) {
	    /*
	     * Found!
	     */

	    *valuePtr = WORD_INTTRIELEAF_VALUE(node);
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
    Col_Word node, leaf, parent, *nodePtr;
    int leafKey;
    uint32_t mask;

    if (WORD_TYPE(map) != WORD_TYPE_INTTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not an integer trie map", map);
	return 0;
    }

    /*
     * Search for matching entry.
     */

    node = WORD_TRIEMAP_ROOT(map);
    while (node) {
	if (WORD_TYPE(node) == WORD_TYPE_TRIENODE) {
	    if (key & WORD_TRIENODE_MASK(node)) {
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
	leafKey = WORD_INTTRIELEAF_KEY(node);
	if (key == leafKey) {
	    /*
	     * Found!
	     */

	    WORD_INTTRIELEAF_VALUE(node) = value;
	    Col_SetModified((void *) node);
	    return 0;
	}
	break;
    }

    /*
     * Not found, insert.
     */

    leaf = (Col_Word) AllocCells(1);
    WORD_INTTRIELEAF_INIT(leaf, key, value);

    if (!WORD_TRIEMAP_ROOT(map)) {
	/*
	 * First leaf.
	 */

	ASSERT(WORD_TRIEMAP_ROOT(map) == WORD_NIL);
	ASSERT(WORD_TRIEMAP_SIZE(map) == 0);
	WORD_TRIEMAP_ROOT(map) = leaf;
	Col_SetModified((void *) map);

	WORD_TRIEMAP_SIZE(map)++;

	return 1;
    }

    /*
     * Get diff mask between inserted key and existing leaf key, i.e. only keep 
     * the highest bit set.
     * See: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2 
     */

    mask = key ^ leafKey;
    mask |= mask >> 1;
    mask |= mask >> 2;
    mask |= mask >> 4;
    mask |= mask >> 8;
    mask |= mask >> 16;
    mask++;
    mask >>= 1;

    /*
     * Find insertion point.
     */

    nodePtr = &WORD_TRIEMAP_ROOT(map);
    parent = map;
    while (*nodePtr) {
	node = *nodePtr;
	if (WORD_TYPE(node) == WORD_TYPE_TRIENODE && mask
		< WORD_TRIENODE_MASK(node)) {
	    parent = node;
	    if (key & WORD_TRIENODE_MASK(node)) {
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
     * Insert leaf here.
     */

    node = (Col_Word) AllocCells(1);
    if (key & mask) {
	/*
	 * Leaf is right.
	 */

	WORD_TRIENODE_INIT(node, mask, *nodePtr, leaf);
    } else {
	/*
	 * Leaf is left.
	 */

	WORD_TRIENODE_INIT(node, mask, leaf, *nodePtr);
    }
    *nodePtr = node;
    Col_SetModified((void *) parent);

    WORD_TRIEMAP_SIZE(map)++;

    return 1;
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
    Col_Word map, 		/* Integer hash map to remove entry from. */
    int key) 			/* Entry key. */
{
    Col_Word node, *nodePtr, sibling, grandParent, *parentPtr;

    if (WORD_TYPE(map) != WORD_TYPE_INTTRIEMAP) {
	Col_Error(COL_ERROR, "%x is not an integer trie map", map);
	return 0;
    }

    /*
     * Search for matching entry.
     */

    parentPtr = NULL;
    nodePtr = &WORD_TRIEMAP_ROOT(map);
    while (*nodePtr) {
	node = *nodePtr;
	if (WORD_TYPE(node) == WORD_TYPE_TRIENODE) {
	    if (parentPtr) {
		grandParent = *parentPtr;
	    } else {
		grandParent = map;
	    }
	    parentPtr = nodePtr;
	    if (key & WORD_TRIENODE_MASK(node)) {
		/*
		 * Recurse on right.
		 */

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

	ASSERT(WORD_TYPE(node) == WORD_TYPE_INTTRIELEAF);
	if (key == WORD_INTTRIELEAF_KEY(node)) {
	    /*
	     * Found!
	     */

	    if (parentPtr) {
		/*
		 * Replace parent by sibling.
		 */

		ASSERT(grandParent);
		*parentPtr = sibling;
		Col_SetModified((void *) grandParent);
	    } else {
		/*
		 * Leaf was root.
		 */

		WORD_TRIEMAP_ROOT(map) = WORD_NIL;
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
