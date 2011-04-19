/*
 * List data structures implemented as words.
 *
 * Lists are made of vectors assembled in a binary tree structure, in the same 
 * fashion as ropes with string chunks. The present code is actually an 
 * adaptation of the rope implementation.
 *
 * Lists come in mutable and immutable versions.
 */

#include "colibri.h"
#include "colInternal.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <malloc.h> /* For alloca */

/* 
 * Max nb cells/length of short list created with sublist or concat. 
 */

#define MAX_SHORT_LIST_SIZE		3
#define MAX_SHORT_LIST_LENGTH		VECTOR_MAX_LENGTH(MAX_SHORT_LIST_SIZE*CELL_SIZE)

/*
 * Max nb cells/length of mutable vectors during lazy conversions of void lists.
 */

#define MAX_SHORT_MVECTOR_SIZE		10
#define MAX_SHORT_MVECTOR_LENGTH	VECTOR_MAX_LENGTH(MAX_SHORT_MVECTOR_SIZE*CELL_SIZE)

/*
 * Max depth of subnodes in iterators.
 */

#define MAX_ITERATOR_SUBNODE_DEPTH	3

/*
 * Prototypes for functions used only in this file.
 */

static Col_ListChunksTraverseProc MergeChunksProc;
static Col_Word		NewListWord(Col_Word root, size_t loop);
static void		GetArms(Col_Word node, Col_Word * leftPtr, 
			    Col_Word * rightPtr);
static void		FreezeMList(Col_Word mlist);
static void		FreezeMListNode(Col_Word * mlistPtr);
static void		MListSetAt(Col_Word * nodePtr, size_t index, 
			    Col_Word element, Col_Word parent);
static void		ConvertToMutableAt(Col_Word *nodePtr, Col_Word parent, 
				size_t index);
static void		SplitMutableAt(Col_Word *nodePtr, Col_Word parent, 
				size_t index);
static void		ConvertToMConcatNode(Col_Word *nodePtr, 
				Col_Word parent);
static void		UpdateMConcatNode(Col_Word node);
static void		MListInsert(Col_Word * nodePtr, size_t index,
				Col_Word list, Col_Word parent);
static void		MListRemove(Col_Word * nodePtr, size_t first, 
				size_t last, Col_Word parent);
static ColListIterLeafAtProc IterAtVector, IterAtVoid;


/*
 *---------------------------------------------------------------------------
 *
 * NewListWord --
 *
 *	Utility proc used to wrap a list around a list word. This is needed
 *	when resulting list is circular, i.e. the given loop-length is nonzero,
 *	or when it needs a synonym field.
 *
 * Results:
 *	The new word.
 *
 * Side effects:
 *	May allocate memory cells.
 *
 *---------------------------------------------------------------------------
 */

static Col_Word	
NewListWord(
    Col_Word root,		/* List root. */
    size_t loop)		/* Loop length (zero for non-cyclic). */
{
    Col_Word list;

    if (!loop) {
	return root;
    }

    if (WORD_TYPE(root) == WORD_TYPE_LIST) {
	root = WORD_LIST_ROOT(root);
    } else if (WORD_TYPE(root) == WORD_TYPE_WRAP) {
	//FIXME: is it needed?
	root = WORD_WRAP_SOURCE(root);
    }
    list = (Col_Word) AllocCells(1);
    WORD_LIST_INIT(list, root, loop);

    return list;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewList --
 *
 *	Create a new list word.
 *
 *	Short lists are created as vectors. Larger lists are recursively split
 *	in half and assembled in a tree.
 *
 *	If elements pointer is NULL, build a void list (ie. whose elements are
 *	all nil).
 *
 * Results:
 *	The new word, or empty when length is zero.
 *
 * Side effects:
 *	May allocate memory cells.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_NewList(
    size_t length,		/* Length of below array. */
    const Col_Word * elements)	/* Array of words to populate list with. */
{
    size_t half=0;		/* Index of the split point. */

    if (length == 0) {
	/*
	 * Empty list.
	 */

	return WORD_LIST_EMPTY;
    }

    if (!elements) {
	/*
	 * Void list.
	 */

	if (length <= VOIDLIST_MAX_LENGTH) {
	    return WORD_VOIDLIST_NEW(length);
	}

	/*
	 * Length is too long to fit within an immediate value. Split.
	 */

	half = length/2;
	return Col_ConcatLists(Col_NewList(half, NULL), Col_NewList(length-half,
		NULL));
    }

    return Col_NewVector(length, elements);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ListLength --
 *
 *	Get or compute the length of the list.
 *
 * Results:
 *	The length.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

size_t 
Col_ListLength(
    Col_Word list)		/* List to get length for. */
{
    switch (WORD_TYPE(list)) {
	case WORD_TYPE_WRAP:
	    return Col_ListLength(WORD_WRAP_SOURCE(list));

	case WORD_TYPE_VOIDLIST:
	    return WORD_VOIDLIST_LENGTH(list);

	case WORD_TYPE_VECTOR:
	case WORD_TYPE_MVECTOR:
	    return WORD_VECTOR_LENGTH(list);

	case WORD_TYPE_LIST:
	case WORD_TYPE_MLIST:
	    return Col_ListLength(WORD_LIST_ROOT(list));

	case WORD_TYPE_CONCATLIST:
	case WORD_TYPE_MCONCATLIST:
	    return WORD_CONCATLIST_LENGTH(list);

	case WORD_TYPE_SUBLIST:
	    /* 
	     * Sublist length is the range width.
	     */

	    return WORD_SUBLIST_LAST(list)-WORD_SUBLIST_FIRST(list)+1;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /*
	     * Invalid type.
	     */

	    Col_Error(COL_ERROR, "%x is not a list", list);
	    return 0;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * MergeChunksProc --
 *
 *	Traversal procedure used to concatenate all portions of lists into
 *	one to fit wthin one vector word.
 *
 * Results:
 *	0.
 *
 * Side effects:
 *	Copy the vector data into the associated MergeChunksInfo fields.
 *
 *---------------------------------------------------------------------------
 */

/* 
 * Structure used to collect data during the traversal. 
 */

typedef struct MergeChunksInfo {
    size_t length;		/* Length so far. */
    Col_Word vector;		/* Vector word. */
} MergeChunksInfo;

static int 
MergeChunksProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_Word **elements, 
    Col_ClientData clientData) 
{
    MergeChunksInfo *info = (MergeChunksInfo *) clientData;

    ASSERT(number == 1);
    if (*elements != COL_LISTCHUNK_VOID
	    && WORD_TYPE(info->vector) == WORD_TYPE_VOIDLIST) {
	/*
	 * Allocate vector and initialize existing elements to nil.
	 */

	size_t vectorLength = WORD_VOIDLIST_LENGTH(info->vector);

	info->vector = (Col_Word) AllocCells(VECTOR_SIZE(vectorLength));
	WORD_VECTOR_INIT(info->vector, vectorLength);
	memset(WORD_VECTOR_ELEMENTS(info->vector), 0, sizeof(Col_Word) 
		* vectorLength);
    }
    if (*elements != COL_LISTCHUNK_VOID) {
	/* 
	 * Set elements. 
	 */

	memcpy(WORD_VECTOR_ELEMENTS(info->vector)+info->length, *elements, 
		length * sizeof(**elements));
    } else if (WORD_TYPE(info->vector) != WORD_TYPE_VOIDLIST) {
	/* 
	 * Initialize elements to nil. 
	 */

	memset(WORD_VECTOR_ELEMENTS(info->vector)+info->length, 0, 
		length * sizeof(**elements));
    }
    info->length += length;
    ASSERT(info->length <= Col_ListLength(info->vector));

    return 0;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_Sublist --
 *
 *	Create a new list which is a sublist of another.
 *
 *	We try to minimize the overhead as much as possible, such as: 
 *	 - identity.
 *       - create vectors for small sublists.
 *	 - sublists of sublists point to original list.
 *	 - sublists of concats point to the deepest superset sublist.
 *
 * Results:
 *	When first is past the end of the (non-cyclic) list, or last is before 
 *	first, empty.
 *	Else, a list representing the sublist.
 *
 * Side effects:
 *	Memory cells may be allocated.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_Sublist(
    Col_Word list,		/* The list to extract the sublist from. */
    size_t first, size_t last)	/* Range of sublist. */
{
    Col_Word sublist;		/* Resulting list node in the general case. */
    size_t listLength;		/* Length of source list. */
    size_t loop;		/* Loop length of source list. */
    unsigned char depth=0;	/* Depth of source list. */
    size_t length;		/* Length of resulting sublist. */

    /*
     * Note: getting length will perfom type checking as well.
     */

    listLength = Col_ListLength(list);
    loop = Col_ListLoopLength(list);

    if (last < first) {
	/*
	 * Invalid range.
	 */

	return WORD_LIST_EMPTY;
    }

    if (first >= listLength) {
	/*
	 * Beginning is past the end of the list.
	 */

	if (loop) {
	    /*
	     * List is cyclic. The resulting sublist will be a concatenation of 
	     * slices and repetitions of the loop.
	     *
	     * First, normalize indices.
	     */

	    size_t offset = (first - (listLength-loop)) % loop 
		    + (listLength-loop);
	    last -= first - offset;
	    first = offset;

	    if (last < listLength) {
		/*
		 * Simple loop slice.
		 */

		return Col_Sublist(list, first, last);
	    } else if (first > listLength-loop) {
		/* 
		 * Leading slice plus remainder.
		 */

		return Col_ConcatLists(Col_Sublist(list, first, listLength-1), 
			Col_Sublist(list, listLength, last));
	    } else {
		/*
		 * Repeat loop.
		 */

		size_t count = (last - first) / loop;

		ASSERT(first == listLength-loop);
		ASSERT(count > 0);

		if (last >= first + (loop * count)) {
		    /*
		     * Loop repeat + trailing slice.
		     */

		    ASSERT(last - (loop * count) < listLength);

		    return Col_ConcatLists(Col_RepeatList(Col_Sublist(list, 
			    first, listLength-1), count), Col_Sublist(list, 
			    first, last - (loop * count)));
		} else {
		    /*
		     * Simple loop repeat.
		     */

		    return Col_RepeatList(Col_Sublist(list, first, 
			    listLength-1), count);
		}
	    }
	}

	/*
	 * This handles empty as well.
	 */

	return WORD_LIST_EMPTY;
    }

    if (last >= listLength) {
	if (loop) {
	    /*
	     * Append loop part.
	     */

	    return Col_ConcatLists(Col_Sublist(list, first, listLength-1), 
		    Col_Sublist(list, listLength, last));
	}

	/* 
	 * Truncate overlong sublists. (Note: at this point listLength > 1)
	 */

	last = listLength-1;
    }

    length = last-first+1;

    if (first == 0 && length == listLength && !loop) {
	/* 
	 * Identity. 
	 */

	FreezeMListNode(&list);
	return list;
    }

    if (WORD_TYPE(list) == WORD_TYPE_LIST || WORD_TYPE(list) 
	    == WORD_TYPE_MLIST) {
	list = WORD_LIST_ROOT(list);
    } else if (WORD_TYPE(list) == WORD_TYPE_WRAP) {
	list = WORD_WRAP_SOURCE(list);
    }
    switch (WORD_TYPE(list)) {
	case WORD_TYPE_VOIDLIST:
	    /*
	     * Use immediate value.
	     */

	    ASSERT(length <= VOIDLIST_MAX_LENGTH);

	    return WORD_VOIDLIST_NEW(length);

	case WORD_TYPE_SUBLIST:
	    /* 
	     * Point to original source. 
	     */

	    return Col_Sublist(WORD_SUBLIST_SOURCE(list), 
		    WORD_SUBLIST_FIRST(list)+first, 
		    WORD_SUBLIST_FIRST(list)+last);

	case WORD_TYPE_CONCATLIST:
	case WORD_TYPE_MCONCATLIST: {
	    /* 
	     * Try to find the deepest superset of the sublist. 
	     */

	    size_t leftLength = WORD_CONCATLIST_LEFT_LENGTH(list);
	    if (leftLength == 0) {
		leftLength = Col_ListLength(WORD_CONCATLIST_LEFT(list));
	    }
	    if (last < leftLength) {
		/* 
		 * Left arm is a superset of sublist. 
		 */

		return Col_Sublist(WORD_CONCATLIST_LEFT(list), first, last);
	    } else if (first >= leftLength) {
		/*
		 * Right arm is a superset of sublist. 
		 */

		return Col_Sublist(WORD_CONCATLIST_RIGHT(list), 
			first-leftLength, last-leftLength);
	    }
	    depth = WORD_CONCATLIST_DEPTH(list);
	    break;
	}
    }

    if (length <= (WORD_TYPE(list) == WORD_TYPE_MVECTOR 
	    ? MAX_SHORT_MVECTOR_LENGTH : MAX_SHORT_LIST_LENGTH)) {
	/* 
	 * Build a vector for short lists or mutable vectors.
	 */

	MergeChunksInfo info; 

	info.length = 0;
	info.vector = WORD_VOIDLIST_NEW(length);
	Col_TraverseListChunks(1, &list, first, length, MergeChunksProc, &info, 
		NULL);
	ASSERT(info.length == length);

	return info.vector;
    }

    /* 
     * General case: build a sublist node.
     */

    FreezeMListNode(&list);
    sublist = (Col_Word) AllocCells(1);
    WORD_SUBLIST_INIT(sublist, depth, list, first, last);

    return sublist;
}

/*
 *---------------------------------------------------------------------------
 *
 * GetArms --
 *
 *	Get the left and right arms of a list node, i.e. a concat or one of 
 *	its sublists.
 *
 * Results:
 *	leftPtr and rightPtr will hold the left and right arms.
 *
 * Side effects:
 *	New words may be created.
 *
 *---------------------------------------------------------------------------
 */

static void 
GetArms(
    Col_Word node,		/* List node to extract arms from. */
    Col_Word * leftPtr,		/* Returned left arm. */
    Col_Word * rightPtr)	/* Returned right arm. */
{
    if (WORD_TYPE(node) == WORD_TYPE_SUBLIST) {
	/* Create one sublist for each list node arm. */
	Col_Word source = WORD_SUBLIST_SOURCE(node);
	size_t leftLength;

	ASSERT(WORD_SUBLIST_DEPTH(node) >= 1);
	ASSERT(WORD_TYPE(source) == WORD_TYPE_CONCATLIST);
	ASSERT(WORD_CONCATLIST_DEPTH(source) == WORD_SUBLIST_DEPTH(node));

	leftLength = WORD_CONCATLIST_LEFT_LENGTH(source);
	if (leftLength == 0) {
	    leftLength = Col_ListLength(WORD_CONCATLIST_LEFT(source));
	}
	*leftPtr = Col_Sublist(WORD_CONCATLIST_LEFT(source), 
		WORD_SUBLIST_FIRST(node), leftLength-1);
	*rightPtr = Col_Sublist(WORD_CONCATLIST_RIGHT(source), 0, 
		WORD_SUBLIST_LAST(node)-leftLength);
    } else {
	ASSERT(WORD_CONCATLIST_DEPTH(node) >= 1);
	ASSERT(WORD_TYPE(node) == WORD_TYPE_CONCATLIST
		|| WORD_TYPE(node) == WORD_TYPE_MCONCATLIST);

	*leftPtr  = WORD_CONCATLIST_LEFT(node);
	*rightPtr = WORD_CONCATLIST_RIGHT(node);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ConcatLists --
 * Col_ConcatListsA --
 * Col_ConcatListsNV --
 * Col_ConcatListsL --
 *
 *	Concatenate lists.
 *	Col_ConcatLists concatenates two lists.
 *	Col_ConcatListsA concatenates several lists given in an array, by 
 *	recursive halvings until it contains one or two elements. 
 *	Col_ConcatListsNV is a variadic version of Col_ConcatListsA.
 *	Col_ConcatListsL takes a list of lists as input.
 *
 *	Concatenation forms self-balanced binary trees. Each node has a depth 
 *	level. Concat nodes have a depth > 1. Sublist nodes have a depth equal
 *	to that of their source node. Other nodes have a depth of 0. 
 *
 *	The resulting concat will have a depth equal to the largest of both 
 *	left and right arms' depth plus 1. If they differ by more than 1, then 
 *	the tree is rebalanced.
 *
 * Results:
 *	If the resulting list would exceed the maximum length, nil. Else, a 
 *	list representing the concatenation of both lists.
 *	Array and variadic versions return nil when concatenating zero lists,
 *	and the source list when concatenating a single one.
 *
 * Side effects:
 *	Memory cells may be allocated.
 *
 *---------------------------------------------------------------------------
 */

/* - Binary version. */
Col_Word
Col_ConcatLists(
    Col_Word left,		/* Left part. */
    Col_Word right)		/* Right part. */
{
    Col_Word concatNode;	/* Resulting list node in the general case. */
    unsigned char leftDepth=0, rightDepth=0; 
				/* Respective depths of left and right lists. */
    size_t leftLength, rightLength;
				/* Respective lengths. */
    size_t loop = 0;		/* Loop length of resulting list (same as 
				 * right). */

    /*
     * Note: getting lengths will perfom type checking as well.
     */

    leftLength = Col_ListLength(left);
    rightLength = Col_ListLength(right);

    switch (WORD_TYPE(left)) {
	case WORD_TYPE_LIST:
	    if (WORD_LIST_LOOP(left)) {
		/*
		 * Left is cyclic, discard right.
		 */

		return left;
	    }
	    break;

	case WORD_TYPE_MLIST:
	    if (WORD_LIST_LOOP(left)) {
		/*
		 * Left is mutable and cyclic, freeze root and discard right.
		 */

		FreezeMList(WORD_LIST_ROOT(left));
		return NewListWord(WORD_LIST_ROOT(left), WORD_LIST_LOOP(left));
	    }
	    break;
    }

    switch (WORD_TYPE(right)) {
	case WORD_TYPE_MLIST:
	case WORD_TYPE_LIST:
	    /*
	     * Final list loop length will be the same as the right arm's.
	     */

	    loop = WORD_LIST_LOOP(right);
	    break;
    }

    if (SIZE_MAX-leftLength < rightLength) {
	/*
	 * Concatenated list would be too long.
	 */

	Col_Error(COL_ERROR, 
		"Combined length %u+%u exceeds the maximum allowed value for lists (%u)", 
		leftLength, rightLength, SIZE_MAX);
	return WORD_NIL;
    }

    if (leftLength == 0) {
	/* 
	 * Concat is a no-op on right. 
	 */

	FreezeMListNode(&right);
	return right;
    } else if (rightLength == 0) {
	/* 
	 * Concat is a no-op on left.
	 */

	FreezeMListNode(&left);
	return left;
    }

    /* 
     * Handle quick cases and get input node depths. 
     */
    
    if (WORD_TYPE(left) == WORD_TYPE_LIST || WORD_TYPE(left) 
	    == WORD_TYPE_MLIST) {
	left = WORD_LIST_ROOT(left);
    } else if (WORD_TYPE(left) == WORD_TYPE_WRAP) {
	left = WORD_WRAP_SOURCE(left);
    }
    if (WORD_TYPE(right) == WORD_TYPE_LIST || WORD_TYPE(right) 
	    == WORD_TYPE_MLIST) {
	right = WORD_LIST_ROOT(right);
    } else if (WORD_TYPE(right) == WORD_TYPE_WRAP) {
	right = WORD_WRAP_SOURCE(right);
    }

    switch (WORD_TYPE(left)) {
	case WORD_TYPE_VOIDLIST:
	    if (WORD_TYPE(right) == WORD_TYPE_VOIDLIST 
		    && leftLength + rightLength <= VOIDLIST_MAX_LENGTH) {
		/*
		 * Merge void lists.
		 */

		return WORD_VOIDLIST_NEW(leftLength + rightLength);
	    }
	    break;

	case WORD_TYPE_SUBLIST:
	    if (WORD_TYPE(right) == WORD_TYPE_SUBLIST
		    && WORD_SUBLIST_SOURCE(left) == WORD_SUBLIST_SOURCE(right) 
		    && WORD_SUBLIST_LAST(left)+1 == WORD_SUBLIST_FIRST(right)) {
		/* 
		 * Merge if left and right are adjacent sublists. This allows
		 * for fast consecutive insertions/removals at a given index.
		 */

		concatNode = Col_Sublist(WORD_SUBLIST_SOURCE(left), 
			WORD_SUBLIST_FIRST(left), WORD_SUBLIST_LAST(right));
		return NewListWord(concatNode, loop);
	    }
	    leftDepth = WORD_SUBLIST_DEPTH(left);
	    break;

	case WORD_TYPE_CONCATLIST:
	case WORD_TYPE_MCONCATLIST:
	    leftDepth = WORD_CONCATLIST_DEPTH(left);
	    break;

	/* WORD_TYPE_UNKNOWN */
    }
    switch (WORD_TYPE(right)) {
	case WORD_TYPE_SUBLIST:
	    rightDepth = WORD_SUBLIST_DEPTH(right);
	    break;

	case WORD_TYPE_CONCATLIST:
	case WORD_TYPE_MCONCATLIST:
	    rightDepth = WORD_CONCATLIST_DEPTH(right);
	    break;

	/* WORD_TYPE_UNKNOWN */
    }

    if (leftLength + rightLength <= ((WORD_TYPE(left) == WORD_TYPE_MVECTOR 
	    || WORD_TYPE(right) == WORD_TYPE_MVECTOR) 
	    ? MAX_SHORT_MVECTOR_LENGTH : MAX_SHORT_LIST_LENGTH)) {
	/* 
	 * Build a vector for short lists or mutable vectors.
	 */

	MergeChunksInfo info; 

	info.length = 0;
	info.vector = WORD_VOIDLIST_NEW(leftLength + rightLength);
	Col_TraverseListChunks(1, &left, 0, leftLength, MergeChunksProc, &info, 
		NULL);
	Col_TraverseListChunks(1, &right, 0, rightLength, MergeChunksProc, 
		&info, NULL);
	ASSERT(info.length == leftLength+rightLength);

	return NewListWord(info.vector, loop);
    }

    /* 
     * Assume the input lists are well balanced by construction. Build a list 
     * that is balanced as well, i.e. where left and right depths don't differ 
     * by more that 1 level.

    /* Note that a sublist with depth >= 1 always points to a concat node, as 
     * by construction it cannot point to another sublist (see Col_Sublist). 
     * Likewise, the arms cannot be list root words.
     */

    if (leftDepth > rightDepth+1) {
	/* 
	 * Left is deeper by more than 1 level, rebalance.
	 */

	unsigned char left1Depth=0, left2Depth=0;
	Col_Word left1, left2;

	ASSERT(leftDepth >= 2);

	GetArms(left, &left1, &left2);
	switch (WORD_TYPE(left1)) {
	    case WORD_TYPE_SUBLIST: left1Depth = WORD_SUBLIST_DEPTH(left1); break;
	    case WORD_TYPE_CONCATLIST:
	    case WORD_TYPE_MCONCATLIST: left1Depth = WORD_CONCATLIST_DEPTH(left1); break;
	}
	switch (WORD_TYPE(left2)) {
	    case WORD_TYPE_SUBLIST: left2Depth = WORD_SUBLIST_DEPTH(left2); break;
	    case WORD_TYPE_CONCATLIST:
	    case WORD_TYPE_MCONCATLIST: left2Depth = WORD_CONCATLIST_DEPTH(left2); break;
	}
	if (left1Depth < left2Depth) {
	    /* 
	     * Left2 is deeper, split it between both arms. 
	     *
	     * Before:
	     *          concat = (left1 + (left21+left22)) + right
	     *         /      \
	     *       left    right
	     *      /    \
	     *   left1  left2
	     *         /     \
	     *      left21  left22
	     *
	     * After:
	     *              concat = (left1 + left21) + (left22 + right)
	     *             /      \
	     *       concat        concat
	     *      /      \      /      \
	     *   left1  left21  left22  right
	     */

	    Col_Word left21, left22;
	    GetArms(left2, &left21, &left22);
	    concatNode = Col_ConcatLists(Col_ConcatLists(left1, left21), 
		    Col_ConcatLists(left22, right));
	    return NewListWord(concatNode, loop);
	} else {
	    /* 
	     * Left1 is deeper or at the same level, rotate to right.
	     *
	     * Before:
	     *            concat = (left1 + left2) + right
	     *           /      \
	     *         left    right
	     *        /    \
	     *     left1  left2
	     *    /     \
	     *   ?       ?
	     *
	     * After:
	     *           concat = left1 + (left2 + right)
	     *          /      \
	     *     left1        concat
	     *    /     \      /      \
	     *   ?       ?   left2   right
	     */

	    concatNode = Col_ConcatLists(left1, Col_ConcatLists(left2, right));
	    return NewListWord(concatNode, loop);
	}
    } else if (leftDepth+1 < rightDepth) {
	/* 
	 * Right is deeper by more than 1 level, rebalance. 
	 */

	unsigned char right1Depth=0, right2Depth=0;
	Col_Word right1, right2;

	ASSERT(rightDepth >= 2);

	GetArms(right, &right1, &right2);
	switch (WORD_TYPE(right1)) {
	    case WORD_TYPE_SUBLIST: right1Depth = WORD_SUBLIST_DEPTH(right1); break;
	    case WORD_TYPE_CONCATLIST:
	    case WORD_TYPE_MCONCATLIST: right1Depth = WORD_CONCATLIST_DEPTH(right1); break;
	}
	switch (WORD_TYPE(right2)) {
	    case WORD_TYPE_SUBLIST: right2Depth = WORD_SUBLIST_DEPTH(right2); break;
	    case WORD_TYPE_CONCATLIST:
	    case WORD_TYPE_MCONCATLIST: right2Depth = WORD_CONCATLIST_DEPTH(right2); break;
	}
	if (right2Depth < right1Depth) {
	    /* 
	     * Right1 is deeper, split it between both arms. 
	     *
	     * Before:
	     *          concat = left + ((right11+right12) + right2)
	     *         /      \
	     *       left    right
	     *              /     \
	     *           right1  right2
	     *          /      \
	     *      right11  right12
	     *
	     * After:
	     *               concat = (left + right11) + (right12 + right2)
	     *             /        \
	     *       concat          concat
	     *      /      \        /      \
	     *    left  right11  right12  right2
	     */

	    Col_Word right11, right12;
	    GetArms(right1, &right11, &right12);
	    concatNode = Col_ConcatLists(Col_ConcatLists(left, right11),
		    Col_ConcatLists(right12, right2));
	    return NewListWord(concatNode, loop);
	} else {
	    /* 
	     * Right2 is deeper or at the same level, rotate to left.
	     *
	     * Before:
	     *          concat = left + (right1 + right2)
	     *         /      \
	     *       left    right
	     *              /     \
	     *           right1  right2
	     *                  /      \
	     *                 ?        ?
	     *
	     * After:
	     *            concat = (left + right1) + right2
	     *           /      \
	     *     concat        right2
	     *    /      \      /      \
	     *  left   right1  ?        ?
	     */

	    concatNode = Col_ConcatLists(Col_ConcatLists(left, right1), right2);
	    return NewListWord(concatNode, loop);
	}
    }

    /* 
     * General case: build a concat node.
     */

    FreezeMListNode(&left);
    FreezeMListNode(&right);
    concatNode = (Col_Word) AllocCells(1);
    WORD_CONCATLIST_INIT(concatNode, (leftDepth>rightDepth?leftDepth:rightDepth)
	    + 1, leftLength + rightLength, leftLength, left, right);

    return NewListWord(concatNode, loop);
}

/* - Array version. */
Col_Word
Col_ConcatListsA(
    size_t number,		/* Number of lists in below array. */
    const Col_Word * lists)	/* Lists to concatenate in order. */
{
    size_t half;
    Col_Word l1, l2;

    /* 
     * Quick cases.
     */

    if (number == 0) {return WORD_NIL;}
    if (number == 1) {return lists[0];}
    if (number == 2) {
	if (Col_ListLoopLength(lists[0])) {
	    return lists[0];
	}
	return Col_ConcatLists(lists[0], lists[1]);
    }

    /* 
     * Split array and concatenate both halves. This should result in a well 
     * balanced tree. 
     */

    half = number/2;
    l1 = Col_ConcatListsA(half, lists);
    if (Col_ListLoopLength(l1)) {
	/*
	 * Skip remaining args.
	 */

	return l1;
    }
    l2 = Col_ConcatListsA(number-half, lists+half);
    return Col_ConcatLists(l1, l2);
}


/* - Variadic version. */
Col_Word
Col_ConcatListsNV(
    size_t number,		/* Number of arguments. */
    ...)			/* Remaining arguments, i.e. lists to 
				 * concatenate in order. */
{
    size_t i;
    va_list args;
    Col_Word *lists;
    
    /* 
     * Convert vararg list to array. Use alloca since a vararg list is 
     * typically small. 
     */

    lists = alloca(number*sizeof(Col_Word));
    va_start(args, number);
    for (i=0; i < number; i++) {
	lists[i] = va_arg(args, Col_Word);
    }
    return Col_ConcatListsA(number, lists);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_LoopList --
 *
 *	Create a cyclic list from a regular list.
 *
 * Results:
 *	If the list is not already cyclic, a new cyclic list.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_LoopList(
    Col_Word list)		/* The list to loop. */
{
    size_t length;

    switch (WORD_TYPE(list)) {
	case WORD_TYPE_WRAP:
	    return Col_LoopList(WORD_WRAP_SOURCE(list));

	case WORD_TYPE_VOIDLIST:
	    length = WORD_VOIDLIST_LENGTH(list);
	    if (length == 0) {
		/*
		 * No-op.
		 */

		return WORD_LIST_EMPTY;
	    }
	    return NewListWord(list, length);

	case WORD_TYPE_VECTOR:
	case WORD_TYPE_MVECTOR:
	case WORD_TYPE_SUBLIST:
	case WORD_TYPE_CONCATLIST:
	    length = Col_ListLength(list);
	    if (length == 0) {
		/*
		 * No-op.
		 */

		return WORD_LIST_EMPTY;
	    }
	    FreezeMListNode(&list);
	    return NewListWord(list, length);

	case WORD_TYPE_LIST:
	case WORD_TYPE_MLIST:
	    length = Col_ListLength(list);
	    if (length == 0) {
		/*
		 * No-op.
		 */

		return WORD_LIST_EMPTY;
	    }
	    if (WORD_LIST_LOOP(list) == length) {
		/*
		 * No-op.
		 */

		return list;
	    }
	    FreezeMListNode(&list);
	    return NewListWord(list, length);

	/* WORD_TYPE_UNKNOWN */

	default:
	    /*
	     * Invalid type.
	     */

	    Col_Error(COL_ERROR, "%x is not an immutable list", list);
	    return WORD_NIL;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ListLoopLength --
 *
 *	Get the loop length of the list.
 *
 * Results:
 *	If the list is cyclic, the terminal loop length. Else zero.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

size_t 
Col_ListLoopLength(
    Col_Word list)		/* List to get length for. */
{
    switch (WORD_TYPE(list)) {
	case WORD_TYPE_WRAP:
	    return Col_ListLoopLength(WORD_WRAP_SOURCE(list));

	case WORD_TYPE_VOIDLIST:
	case WORD_TYPE_VECTOR:
	case WORD_TYPE_MVECTOR:
	case WORD_TYPE_CONCATLIST:
	case WORD_TYPE_MCONCATLIST:
	case WORD_TYPE_SUBLIST:
	    return 0;

	case WORD_TYPE_LIST:
	case WORD_TYPE_MLIST:
	    return WORD_LIST_LOOP(list);

	/* WORD_TYPE_UNKNOWN */

	default:
	    /*
	     * Invalid type.
	     */

	    Col_Error(COL_ERROR, "%x is not a list", list);
	    return 0;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewMList --
 *
 *	Create a new mutable list word.
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
Col_NewMList()
{
    Col_Word mlist;

    mlist = (Col_Word) AllocCells(1);
    WORD_MLIST_INIT(mlist, WORD_LIST_EMPTY, 0);

    return mlist;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_FreezeMList --
 *
 *	Turn a mutable list immutable. If an immutable list is given,
 *	does nothing.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The mutable list becomes immutable.
 *
 *---------------------------------------------------------------------------
 */

void
Col_FreezeMList(
    Col_Word mlist)		/* The mutable list to freeze. */
{
    switch (WORD_TYPE(mlist)) {
	case WORD_TYPE_LIST:
	    /*
	     * No-op.
	     */

	    break;

	case WORD_TYPE_MLIST: {
	    /*
	     * Change type ID and recursively freeze subnodes.
	     */

	    int pinned = WORD_PINNED(mlist);
	    WORD_SET_TYPEID(mlist, WORD_TYPE_MLIST);
	    if (pinned) {
		WORD_SET_PINNED(mlist);
	    }
	    FreezeMList(WORD_LIST_ROOT(mlist));
	    break;
	}

	default:
	    Col_Error(COL_ERROR, "%x is not a mutable list", mlist);
    }
}

static void
FreezeMList(
    Col_Word mlist)
{
    /*
     * Entry point for tail recursive calls.
     */

start:
    switch (WORD_TYPE(mlist)) {
	case WORD_TYPE_MVECTOR: {
	    /*
	     * Simply change type ID. Don't mark extraneous cells, they will
	     * be collected during GC.
	     */

	    int pinned = WORD_PINNED(mlist);
	    WORD_SET_TYPEID(mlist, WORD_TYPE_VECTOR);
	    if (pinned) {
		WORD_SET_PINNED(mlist);
	    }
	    return;
	}

	case WORD_TYPE_MCONCATLIST: {
	    /*
	     * Change type ID and recursively freeze subnodes (tail recurse on
	     * right node).
	     */

	    int pinned = WORD_PINNED(mlist);
	    WORD_SET_TYPEID(mlist, WORD_TYPE_CONCATLIST);
	    if (pinned) {
		WORD_SET_PINNED(mlist);
	    }
	    FreezeMList(WORD_CONCATLIST_LEFT(mlist));
	    mlist = WORD_CONCATLIST_RIGHT(mlist);
	    break;
	}

	default:
	    /*
	     * Already immutable, stop there.
	     */

	    return;
    }

    /*
     * Tail recurse.
     */

    goto start;
}

static void
FreezeMListNode(
    Col_Word *mlistPtr)
{
    switch (WORD_TYPE(*mlistPtr)) {
	case WORD_TYPE_MVECTOR:
	    /*
	     * Copy to large immutable vector.
	     */

	    *mlistPtr = Col_NewVector(WORD_VECTOR_LENGTH(*mlistPtr), 
		    WORD_VECTOR_ELEMENTS(*mlistPtr));
	    break;

	case WORD_TYPE_MLIST:
	    /*
	     * Freeze root.
	     */

	    *mlistPtr = WORD_LIST_ROOT(*mlistPtr);
	    FreezeMList(*mlistPtr);
	    break;

	case WORD_TYPE_MCONCATLIST:
	    /*
	     * Freeze node.
	     */

	    FreezeMList(*mlistPtr);
	    break;

	case WORD_TYPE_LIST:
	    /*
	     * Get root.
	     */

	    *mlistPtr = WORD_LIST_ROOT(*mlistPtr);
	    break;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_MListSetLength --
 *
 *	Resize the mutable list.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The actual length is modified. Newly added elements are set to nil.
 *
 *---------------------------------------------------------------------------
 */

void
Col_MListSetLength(
    Col_Word mlist,		/* Mutable list to resize. */
    size_t length)		/* New length. */
{
    size_t listLength = Col_ListLength(mlist);
    if (listLength < length) {
	/*
	 * Append void list.
	 */

	Col_MListInsert(mlist, listLength, Col_NewList(length - listLength, 
		NULL));
    } else if (listLength > length) {
	/*
	 * Remove tail.
	 */

	Col_MListRemove(mlist, length, listLength-1);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_MListSetLoop --
 *
 *	Make a mutable list cyclic, or break cycle if given length is zero.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The mutable list is looped or unlooped.
 *
 *---------------------------------------------------------------------------
 */

void
Col_MListSetLoop(
    Col_Word mlist,		/* The mutable list to loop. */
    size_t loopLength)		/* Length of the terminal loop. */

{
    size_t length;

    switch (WORD_TYPE(mlist)) {
	case WORD_TYPE_MLIST:
	    length = Col_ListLength(mlist);
	    if (loopLength > length) {loopLength = length;}

	    WORD_LIST_LOOP(mlist) = loopLength;
	    break;

	default:
	    Col_Error(COL_ERROR, "%x is not a mutable list", mlist);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_MListSetAt --
 *
 *	Set the element of a mutable list at a given position. 
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The list is modified.
 *
 *---------------------------------------------------------------------------
 */

void
Col_MListSetAt(
    Col_Word mlist,		/* Mutable list to modify. */
    size_t index,		/* Element index. */
    Col_Word element)		/* New element. */
{
    size_t length;

    if (WORD_TYPE(mlist) != WORD_TYPE_MLIST) {
	Col_Error(COL_ERROR, "%x is not a mutable list", mlist);
	return;
    }

    length = Col_ListLength(mlist);
    if (index >= length) {
	size_t loop = Col_ListLoopLength(mlist);
	if (!loop) {
	    /*
	     * Out of bounds.
	     */

	    Col_Error(COL_ERROR, 
		    "Index %u out of bounds for mutable list %x", index,
		    mlist);
	    return;
	}

	/*
	 * Cyclic list. Normalize index.
	 */

	index = (index - (length-loop)) % loop + (length-loop);
    }
    MListSetAt(&WORD_LIST_ROOT(mlist), index, element, mlist);
}

static void
MListSetAt(
    Col_Word * nodePtr, 
    size_t index,
    Col_Word element,
    Col_Word parent)
{
    /*
     * Note: bounds checking is done upfront.
     */

    /*
     * Entry point for tail recursive calls.
     */

start:
    switch (WORD_TYPE(*nodePtr)) {
	/*
	 * First mutable types...
	 */

	case WORD_TYPE_MCONCATLIST: {
	    size_t leftLength = WORD_CONCATLIST_LEFT_LENGTH(*nodePtr);
	    if (leftLength == 0) {
		leftLength = Col_ListLength(WORD_CONCATLIST_LEFT(*nodePtr));
	    }
	    if (index < leftLength) {
		/*
		 * Recurse on left.
		 */

		MListSetAt(&WORD_CONCATLIST_LEFT(*nodePtr), index, element, 
			*nodePtr);
	    } else {
		/*
		 * Recurse on right.
		 */

		MListSetAt(&WORD_CONCATLIST_RIGHT(*nodePtr), index - leftLength,
			element, *nodePtr);
	    }

	    /*
	     * Update & rebalance tree.
	     */

	    UpdateMConcatNode(*nodePtr);
	    return;
	}

	case WORD_TYPE_MVECTOR:
	    /*
	     * Set element.
	     */

	    WORD_VECTOR_ELEMENTS(*nodePtr)[index] = element;
	    Col_WordSetModified(*nodePtr);
	    return;

	/*
	 * ... then immediate and immutable types.
	 */

	case WORD_TYPE_CONCATLIST:
	    /*
	     * Convert to mutable concat then retry.
	     */

	    ConvertToMConcatNode(nodePtr, parent);
	    goto start;

	case WORD_TYPE_SUBLIST:
	    if (WORD_SUBLIST_DEPTH(*nodePtr) >= 1) {
		/*
		 * Source is concat. Convert to mutable then retry.
		 */

		ConvertToMConcatNode(nodePtr, parent);
		goto start;
	    }
	    /* continued. */
	default:
	    /*
	     * Convert to mutable around index then retry.
	     */

	    ConvertToMutableAt(nodePtr, parent, index);
	    goto start;
    }
}

static void
ConvertToMutableAt(
    Col_Word *nodePtr,
    Col_Word parent,
    size_t index)
{
    size_t length;
    Col_Word *elements;
    Col_Word converted;

    length = Col_ListLength(*nodePtr);
    switch (WORD_TYPE(*nodePtr)) {
	case WORD_TYPE_VECTOR:
	    elements = WORD_VECTOR_ELEMENTS(*nodePtr);
	    break;

	case WORD_TYPE_VOIDLIST:
	    elements = NULL;
	    break;

	case WORD_TYPE_SUBLIST:
	    ASSERT(WORD_SUBLIST_DEPTH(*nodePtr) == 0);
	    ASSERT(WORD_TYPE(WORD_SUBLIST_SOURCE(*nodePtr)) == WORD_TYPE_VECTOR);
	    elements = WORD_VECTOR_ELEMENTS(WORD_SUBLIST_SOURCE(*nodePtr))
		    + WORD_SUBLIST_FIRST(*nodePtr);
	    break;

	default: 
	    /* CANTHAPPEN */
	    ASSERT(0);
    }

    if (length <= MAX_SHORT_MVECTOR_LENGTH) {
	/*
	 * Convert whole.
	 */

	converted = Col_NewMVector(0, length, elements);
    } else if (index < MAX_SHORT_MVECTOR_LENGTH) {
	/*
	 * Convert head.
	 */

	Col_Word mvector = Col_NewMVector(0, MAX_SHORT_MVECTOR_LENGTH, 
		elements);

	converted = (Col_Word) AllocCells(1);
	WORD_MCONCATLIST_INIT(converted, 1, length, MAX_SHORT_MVECTOR_LENGTH, 
		mvector, Col_Sublist(*nodePtr, MAX_SHORT_MVECTOR_LENGTH, 
		length-1));
    } else if (index >= WORD_VOIDLIST_LENGTH(*nodePtr)
	    - MAX_SHORT_MVECTOR_LENGTH) {
	/*
	 * Convert tail.
	 */

	size_t leftLength = length - MAX_SHORT_MVECTOR_LENGTH;
	Col_Word mvector = Col_NewMVector(0, MAX_SHORT_MVECTOR_LENGTH, 
		(elements ? elements + leftLength : NULL));

	ASSERT(leftLength >= 1);

	converted = (Col_Word) AllocCells(1);
	WORD_MCONCATLIST_INIT(converted, 1, length, leftLength,
		Col_Sublist(*nodePtr, 0, leftLength-1), mvector);
    } else {
	/*
	 * Convert central part starting at index.
	 */

	Col_Word mvector = Col_NewMVector(0, MAX_SHORT_MVECTOR_LENGTH,
		(elements ? elements + index : NULL));
	size_t leftLength = index + MAX_SHORT_MVECTOR_LENGTH;
	Col_Word left = (Col_Word) AllocCells(1);
	WORD_MCONCATLIST_INIT(left, 1, leftLength, index, 
		Col_Sublist(*nodePtr, 0, index-1), mvector);

	converted = (Col_Word) AllocCells(1);
	WORD_MCONCATLIST_INIT(converted, 2, length, leftLength, left, 
		Col_Sublist(*nodePtr, leftLength, length-1));
    }

    *nodePtr = converted;
    Col_WordSetModified(parent);
}

static void
ConvertToMConcatNode(
    Col_Word *nodePtr,
    Col_Word parent)
{
    Col_Word converted;

    switch (WORD_TYPE(*nodePtr)) {
	case WORD_TYPE_SUBLIST: {
	    /*
	     * Split sublist into mutable concat node.
	     */

	    Col_Word left, right;
	    unsigned char leftDepth=0, rightDepth=0; 
	    size_t leftLength;

	    ASSERT(WORD_SUBLIST_DEPTH(*nodePtr) >= 1);
	    ASSERT(WORD_TYPE(WORD_SUBLIST_SOURCE(*nodePtr)) == WORD_TYPE_CONCATLIST);

	    GetArms(*nodePtr, &left, &right);
	    leftLength = Col_ListLength(left);
	    switch (WORD_TYPE(left)) {
		case WORD_TYPE_SUBLIST: leftDepth = WORD_SUBLIST_DEPTH(left); break;
		case WORD_TYPE_CONCATLIST:
		case WORD_TYPE_MCONCATLIST: leftDepth = WORD_CONCATLIST_DEPTH(left); break;
	    }
	    switch (WORD_TYPE(right)) {
		case WORD_TYPE_SUBLIST: rightDepth = WORD_SUBLIST_DEPTH(right); break;
		case WORD_TYPE_CONCATLIST:
		case WORD_TYPE_MCONCATLIST: rightDepth = WORD_CONCATLIST_DEPTH(right); break;
	    }

	    converted = (Col_Word) AllocCells(1);
	    WORD_MCONCATLIST_INIT(converted,
		    (leftDepth>rightDepth?leftDepth:rightDepth) + 1,
		    Col_ListLength(*nodePtr), leftLength, left, right);
	    break;
	}

	case WORD_TYPE_CONCATLIST: {
	    /*
	     * Convert to mutable concat node.
	     */

	    size_t leftLength = WORD_CONCATLIST_LEFT_LENGTH(*nodePtr);

	    ASSERT(WORD_CONCATLIST_DEPTH(*nodePtr) >= 1);

	    converted = (Col_Word) AllocCells(1);
	    WORD_MCONCATLIST_INIT(converted, WORD_CONCATLIST_DEPTH(*nodePtr), 
		    WORD_CONCATLIST_LENGTH(*nodePtr), leftLength, 
		    WORD_CONCATLIST_LEFT(*nodePtr),
		    WORD_CONCATLIST_RIGHT(*nodePtr))
	    break;
	}

	default: 
	    /* CANTHAPPEN */
	    ASSERT(0);
    }

    *nodePtr = converted;
    Col_WordSetModified(parent);
}

static void
UpdateMConcatNode(
    Col_Word node)
{
    Col_Word left, right;
    unsigned char leftDepth=0, rightDepth=0; 
    size_t leftLength;

    ASSERT(WORD_TYPE(node) == WORD_TYPE_MCONCATLIST);

    /*
     * Entry point for loop.
     */

start:

    left = WORD_CONCATLIST_LEFT(node);
    switch (WORD_TYPE(left)) {
	case WORD_TYPE_SUBLIST: leftDepth = WORD_SUBLIST_DEPTH(left); break;
	case WORD_TYPE_CONCATLIST:
	case WORD_TYPE_MCONCATLIST: leftDepth = WORD_CONCATLIST_DEPTH(left); break;
    }
    right = WORD_CONCATLIST_RIGHT(node);
    switch (WORD_TYPE(right)) {
	case WORD_TYPE_SUBLIST: rightDepth = WORD_SUBLIST_DEPTH(right); break;
	case WORD_TYPE_CONCATLIST:
	case WORD_TYPE_MCONCATLIST: rightDepth = WORD_CONCATLIST_DEPTH(right); break;
    }

    if (leftDepth > rightDepth+1) {
	/* 
	 * Left is deeper by more than 1 level, rebalance.
	 */

	unsigned char left1Depth=0, left2Depth=0;
	Col_Word left1, left2;

	ASSERT(leftDepth >= 2);
	if (WORD_TYPE(left) != WORD_TYPE_MCONCATLIST) {
	    ConvertToMConcatNode(&left, node);
	}
	ASSERT(WORD_TYPE(left) == WORD_TYPE_MCONCATLIST);

	left1 = WORD_CONCATLIST_LEFT(left);
	left2 = WORD_CONCATLIST_RIGHT(left);

	switch (WORD_TYPE(left1)) {
	    case WORD_TYPE_SUBLIST: left1Depth = WORD_SUBLIST_DEPTH(left1); break;
	    case WORD_TYPE_CONCATLIST:
	    case WORD_TYPE_MCONCATLIST: left1Depth = WORD_CONCATLIST_DEPTH(left1); break;
	}
	switch (WORD_TYPE(left2)) {
	    case WORD_TYPE_SUBLIST: left2Depth = WORD_SUBLIST_DEPTH(left2); break;
	    case WORD_TYPE_CONCATLIST:
	    case WORD_TYPE_MCONCATLIST: left2Depth = WORD_CONCATLIST_DEPTH(left2); break;
	}
	if (left1Depth < left2Depth) {
	    /* 
	     * Left2 is deeper, split it between both arms. 
	     *
	     * Before:
	     *          node = (left1 + (left21+left22)) + right
	     *         /    \
	     *       left  right
	     *      /    \
	     *   left1  left2
	     *         /     \
	     *      left21  left22
	     *
	     * After:
	     *              node = (left1 + left21) + (left22 + right)
	     *             /    \
	     *         left      left2
	     *        /    \    /     \
	     *    left1 left21 left22 right
	     */

	    Col_Word left21, left22;
	    unsigned char left21Depth=0, left22Depth=0;

	    if (WORD_TYPE(left2) != WORD_TYPE_MCONCATLIST) {
		ConvertToMConcatNode(&left2, node);
	    }
	    ASSERT(WORD_TYPE(left2) == WORD_TYPE_MCONCATLIST);
	    ASSERT(WORD_CONCATLIST_DEPTH(left2) >= 1);

	    left21 = WORD_CONCATLIST_LEFT(left2);
	    left22 = WORD_CONCATLIST_RIGHT(left2);

	    switch (WORD_TYPE(left21)) {
		case WORD_TYPE_SUBLIST: left21Depth = WORD_SUBLIST_DEPTH(left21); break;
		case WORD_TYPE_CONCATLIST:
		case WORD_TYPE_MCONCATLIST: left21Depth = WORD_CONCATLIST_DEPTH(left21); break;
	    }
	    leftDepth = (left1Depth>left21Depth?left1Depth:left21Depth) + 1;
	    switch (WORD_TYPE(left22)) {
		case WORD_TYPE_SUBLIST: left22Depth = WORD_SUBLIST_DEPTH(left22); break;
		case WORD_TYPE_CONCATLIST:
		case WORD_TYPE_MCONCATLIST: left22Depth = WORD_CONCATLIST_DEPTH(left22); break;
	    }
	    rightDepth = (left22Depth>rightDepth?left22Depth:rightDepth) + 1;

	    /*
	     * Update left node.
	     */

	    leftLength = Col_ListLength(left1);
	    WORD_MCONCATLIST_INIT(left, leftDepth, 
		    leftLength+Col_ListLength(left21), leftLength, left1, 
		    left21);
	    Col_WordSetModified(left);

	    /*
	     * Update right node.
	     */

	    leftLength = Col_ListLength(left22);
	    WORD_MCONCATLIST_INIT(left2, rightDepth, 
		leftLength+Col_ListLength(right), leftLength, left22, right);
	    Col_WordSetModified(left2);

	    /*
	     * Update node.
	     */

	    leftLength = WORD_CONCATLIST_LENGTH(left);
	    WORD_MCONCATLIST_INIT(node, 
		    (leftDepth>rightDepth?leftDepth:rightDepth) + 1, 
		    leftLength+WORD_CONCATLIST_LENGTH(left2), leftLength, left, 
		    left2);
	    Col_WordSetModified(node);
	} else {
	    /* 
	     * Left1 is deeper or at the same level, rotate to right.
	     *
	     * Before:
	     *            node = (left1 + left2) + right
	     *           /    \
	     *         left   right
	     *        /    \
	     *     left1  left2
	     *    /     \
	     *   ?       ?
	     *
	     * After:
	     *           node = left1 + (left2 + right)
	     *          /    \
	     *     left1      left
	     *    /     \    /    \
	     *   ?       ? left2 right
	     */

	    unsigned char left2Depth=0;

	    ASSERT(left1Depth >= 1);

	    switch (WORD_TYPE(left2)) {
		case WORD_TYPE_SUBLIST: left2Depth = WORD_SUBLIST_DEPTH(left2); break;
		case WORD_TYPE_CONCATLIST:
		case WORD_TYPE_MCONCATLIST: left2Depth = WORD_CONCATLIST_DEPTH(left2); break;
	    }
	    rightDepth = (left2Depth>rightDepth?left2Depth:rightDepth) + 1;

	    /*
	     * Update right node.
	     */

	    leftLength = Col_ListLength(left2);
	    WORD_MCONCATLIST_INIT(left, rightDepth, 
		    leftLength+Col_ListLength(right), leftLength, left2, right);
	    Col_WordSetModified(left);

	    /*
	     * Update node.
	     */

	    leftLength = Col_ListLength(left1);
	    WORD_MCONCATLIST_INIT(node, 
		    (left1Depth>rightDepth?left1Depth:rightDepth) + 1, 
		    leftLength+WORD_CONCATLIST_LENGTH(left), leftLength, left1, 
		    left);
	    Col_WordSetModified(node);
	}
    } else if (leftDepth+1 < rightDepth) {
	/* 
	 * Right is deeper by more than 1 level, rebalance. 
	 */

	unsigned char right2Depth=0;
	Col_Word right1, right2;

	ASSERT(rightDepth >= 2);
	if (WORD_TYPE(right) != WORD_TYPE_MCONCATLIST) {
	    ConvertToMConcatNode(&right, node);
	}
	ASSERT(WORD_TYPE(right) == WORD_TYPE_MCONCATLIST);

	right1 = WORD_CONCATLIST_LEFT(right);
	right2 = WORD_CONCATLIST_RIGHT(right);

	switch (WORD_TYPE(right2)) {
	    case WORD_TYPE_SUBLIST: right2Depth = WORD_SUBLIST_DEPTH(right2); break;
	    case WORD_TYPE_CONCATLIST:
	    case WORD_TYPE_MCONCATLIST: right2Depth = WORD_CONCATLIST_DEPTH(right2); break;
	}
	if (right2Depth < rightDepth-1) {
	    /* 
	     * Right1 is deeper, split it between both arms. 
	     *
	     * Before:
	     *          node = left + ((right11+right12) + right2)
	     *         /    \
	     *       left  right
	     *            /     \
	     *         right1  right2
	     *        /      \
	     *    right11  right12
	     *
	     * After:
	     *               node = (left + right11) + (right12 + right2)
	     *             /      \
	     *       right1        right
	     *      /      \      /     \
	     *    left right11 right12 right2
	     */

	    Col_Word right11, right12;
	    unsigned char right11Depth=0, right12Depth=0;

	    if (WORD_TYPE(right1) != WORD_TYPE_MCONCATLIST) {
		ConvertToMConcatNode(&right1, node);
	    }
	    ASSERT(WORD_TYPE(right1) == WORD_TYPE_MCONCATLIST);
	    ASSERT(WORD_CONCATLIST_DEPTH(right1) >= 1);

	    right11 = WORD_CONCATLIST_LEFT(right1);
	    right12 = WORD_CONCATLIST_RIGHT(right1);

	    switch (WORD_TYPE(right11)) {
		case WORD_TYPE_SUBLIST: right11Depth = WORD_SUBLIST_DEPTH(right11); break;
		case WORD_TYPE_CONCATLIST:
		case WORD_TYPE_MCONCATLIST: right11Depth = WORD_CONCATLIST_DEPTH(right11); break;
	    }
	    leftDepth = (leftDepth>right11Depth?leftDepth:right11Depth) + 1;
	    switch (WORD_TYPE(right12)) {
		case WORD_TYPE_SUBLIST: right12Depth = WORD_SUBLIST_DEPTH(right12); break;
		case WORD_TYPE_CONCATLIST:
		case WORD_TYPE_MCONCATLIST: right12Depth = WORD_CONCATLIST_DEPTH(right12); break;
	    }
	    rightDepth = (right12Depth>right2Depth?right12Depth:right2Depth) + 1;

	    /*
	     * Update left node.
	     */

	    leftLength = Col_ListLength(left);
	    WORD_MCONCATLIST_INIT(right1, leftDepth, 
		    leftLength+Col_ListLength(right11), leftLength, left, 
		    right11);
	    Col_WordSetModified(right1);

	    /*
	     * Update right node.
	     */

	    leftLength = Col_ListLength(right12);
	    WORD_MCONCATLIST_INIT(right, rightDepth, 
		    leftLength+Col_ListLength(right2), leftLength, right12, 
		    right2);
	    Col_WordSetModified(right);

	    /*
	     * Update node.
	     */

	    leftLength = WORD_CONCATLIST_LENGTH(right1);
	    WORD_MCONCATLIST_INIT(node, 
		    (leftDepth>rightDepth?leftDepth:rightDepth) + 1, 
		    leftLength+WORD_CONCATLIST_LENGTH(right), leftLength, 
		    right1, right);
	    Col_WordSetModified(node);
	} else {
	    /* 
	     * Right2 is deeper or at the same level, rotate to left.
	     *
	     * Before:
	     *          node = left + (right1 + right2)
	     *         /    \
	     *       left  right
	     *            /     \
	     *         right1  right2
	     *                /      \
	     *               ?        ?
	     *
	     * After:
	     *            node = (left + right1) + right2
	     *           /    \
	     *      right      right2
	     *     /     \    /      \
	     *   left right1 ?        ?
	     */

	    unsigned char right1Depth=0;

	    ASSERT(right2Depth >= 1);
	    switch (WORD_TYPE(right1)) {
		case WORD_TYPE_SUBLIST: right1Depth = WORD_SUBLIST_DEPTH(right1); break;
		case WORD_TYPE_CONCATLIST:
		case WORD_TYPE_MCONCATLIST: right1Depth = WORD_CONCATLIST_DEPTH(right1); break;
	    }
	    leftDepth = (leftDepth>right1Depth?leftDepth:right1Depth) + 1;

	    /*
	     * Update left node.
	     */

	    leftLength = Col_ListLength(left);
	    WORD_MCONCATLIST_INIT(right, rightDepth,
		    leftLength+Col_ListLength(right1), leftLength, left, 
		    right1);
	    Col_WordSetModified(right);

	    /*
	     * Update node.
	     */

	    leftLength = WORD_CONCATLIST_LENGTH(right);
	    WORD_MCONCATLIST_INIT(node, 
		    (leftDepth>right2Depth?leftDepth:right2Depth) + 1, 
		    leftLength+Col_ListLength(right2), leftLength, right, 
		    right2);
	    Col_WordSetModified(node);
	}
    } else {
	/*
	 * Tree is balanced, update and stop there.
	 */

	int pinned = WORD_PINNED(node);
	leftLength = Col_ListLength(left);
	WORD_MCONCATLIST_INIT(node, 
		(leftDepth>rightDepth?leftDepth:rightDepth) + 1,
		leftLength+Col_ListLength(right), leftLength, left, right);
	if (pinned) {
	    WORD_SET_PINNED(node);
	}
	return;
    }

    /*
     * Loop until tree is balanced.
     */

     goto start;
}


/*
 *---------------------------------------------------------------------------
 *
 * Col_MListInsert --
 *
 *	Insert a list into a mutable list, just before the given insertion 
 *	point.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The target list is modified.
 *
 *---------------------------------------------------------------------------
 */

void
Col_MListInsert(
    Col_Word into,		/* Mutable list to insert into. */
    size_t index,		/* Index of insertion point. */
    Col_Word list)		/* List to insert. */
{
    size_t length, loop, listLength, listLoop;

    if (WORD_TYPE(into) != WORD_TYPE_MLIST) {
	Col_Error(COL_ERROR, "%x is not a mutable list", into);
	return;
    }

    length = Col_ListLength(into);
    listLength = Col_ListLength(list);

    if (SIZE_MAX-length < listLength) {
	/*
	 * Concatenated list would be too long.
	 */

	Col_Error(COL_ERROR, 
		"Combined length %u+%u exceeds the maximum allowed value for lists (%u)", 
		length, listLength, SIZE_MAX);
	return;
    }

    if (listLength == 0) {
	/*
	 * No-op.
	 */

	return;
    }

    listLoop = Col_ListLoopLength(list);

    if (length == 0) {
	/*
	 * Target is empty, replace content with input.
	 */

	FreezeMListNode(&list);
	WORD_LIST_ROOT(into) = list;
	WORD_LIST_LOOP(into) = listLoop;
	return;
    }

    loop = Col_ListLoopLength(into);
    if (loop == 0 && index >= length) {
	/*
	 * Insertion past the end of non-cyclic list is concatenation.
	 */

	index = length;
    }

    if (loop > 0 && index > length-loop) {
	/*
	 * Insertion within loop. Normalize index and increase loop length.
	 */

	index = (index - (length-loop)) % loop + (length-loop);
	if (index == length-loop) index = length;
	loop += listLength;
    }

    if (listLoop) {
	/*
	 * Inserted list is cyclic, its loop closes the list. Trim end.
	 */

	if (index < length) {
	    MListRemove(&WORD_LIST_ROOT(into), index, length-1, into);
	}
	loop = listLoop;
    }

    /*
     * Insert list.
     */

    MListInsert(&WORD_LIST_ROOT(into), index, list, into);
    WORD_LIST_LOOP(into) = loop;
}

static void
MListInsert(
    Col_Word * nodePtr, 
    size_t index,
    Col_Word list,
    Col_Word parent)
{
    size_t length = Col_ListLength(*nodePtr);
    size_t listLength = Col_ListLength(list);
    Col_Word node;
    int type;

    ASSERT(index <= length);
    ASSERT(SIZE_MAX-length >= listLength);
    ASSERT(listLength > 0);

    if (length == 0) {
	/*
	 * Replace content.
	 */

	if (WORD_TYPE(list) == WORD_TYPE_LIST) {
	    list = WORD_LIST_ROOT(list);
	} else if (WORD_TYPE(list) == WORD_TYPE_WRAP) {
	    list = WORD_WRAP_SOURCE(list);
	}
	*nodePtr = list;
	Col_WordSetModified(parent);
	return;
    }

    /*
     * Entry point for tail recursive calls.
     */

start:

    /*
     * First try to alter mutable types if possible.
     */

    type = WORD_TYPE(*nodePtr);
    switch (type) {
	case WORD_TYPE_MCONCATLIST: {
	    size_t leftLength = WORD_CONCATLIST_LEFT_LENGTH(*nodePtr);
	    Col_Word left = WORD_CONCATLIST_LEFT(*nodePtr);
	    if (leftLength == 0) {
		leftLength = Col_ListLength(left);
	    }

	    if (index == leftLength && WORD_CONCATLIST_DEPTH(*nodePtr) > 1) {
		/*
		 * Insertion point is just between the two arms. Insert into
		 * the shallowest one to balance tree.
		 */

		unsigned char leftDepth=0;
		switch (WORD_TYPE(left)) {
		    case WORD_TYPE_SUBLIST: leftDepth = WORD_SUBLIST_DEPTH(left); break;
		    case WORD_TYPE_CONCATLIST:
		    case WORD_TYPE_MCONCATLIST: leftDepth = WORD_CONCATLIST_DEPTH(left); break;
		}
		if (leftDepth < WORD_CONCATLIST_DEPTH(*nodePtr)-1) {
		    /*
		     * Right is deeper, insert into left.
		     */

		    MListInsert(&WORD_CONCATLIST_LEFT(*nodePtr), index, 
			    list, *nodePtr);
		    UpdateMConcatNode(*nodePtr);
		    return;
		}
	    }

	    if (index <= leftLength) {
		/*
		 * Insert into left.
		 */

		MListInsert(&WORD_CONCATLIST_LEFT(*nodePtr), index, list, 
			*nodePtr);
	    } else {
		/*
		 * Insert into right.
		 */

		MListInsert(&WORD_CONCATLIST_RIGHT(*nodePtr), 
			index-leftLength, list, *nodePtr);
	    }
	    UpdateMConcatNode(*nodePtr);
	    return;
	}

	case WORD_TYPE_MVECTOR: {
	    size_t maxLength = VECTOR_MAX_LENGTH(WORD_MVECTOR_SIZE(*nodePtr)
		    * CELL_SIZE);
	    Col_Word *elements = WORD_VECTOR_ELEMENTS(*nodePtr);
	    if (listLength <= maxLength-length) {
		/*
		 * Insert data directly into available space.
		 */

		MergeChunksInfo info;

		Col_MVectorSetLength(*nodePtr, length+listLength);

		/*
		 * Move trailing elements.
		 */

		memmove(elements+index+listLength, elements+index,
			(length-index) * sizeof(*elements));

		/*
		 * Copy elements from list.
		 */

		info.length = index;
		info.vector = *nodePtr;
		Col_TraverseListChunks(1, &list, 0, listLength, MergeChunksProc, 
			&info, NULL);
		ASSERT(info.length == index+listLength);
		Col_WordSetModified(*nodePtr);
		return;
	    }

	    /*
	     * Try other methods below.
	     */

	    break;
	}

    }

    if (length+listLength <= MAX_SHORT_MVECTOR_LENGTH) {
	/*
	 * Merge all chunks into one mutable vector for short lists.
	 * TODO: geometric growth?
	 */

	MergeChunksInfo info;
	
	node = Col_NewMVector(0, length+listLength, NULL);

	info.length = 0;
	info.vector = node;
	Col_TraverseListChunks(1, nodePtr, 0, index, MergeChunksProc, 
		&info, NULL);
	Col_TraverseListChunks(1, &list, 0, listLength, MergeChunksProc, 
		&info, NULL);
	Col_TraverseListChunks(1, nodePtr, index, length-index, 
		MergeChunksProc, &info, NULL);
	ASSERT(info.length == length+listLength);

	*nodePtr = node;
	Col_WordSetModified(parent);
	return;
    }

    /*
     * General case with immutable types and mutable with no available space.
     */

    switch (type) {
	case WORD_TYPE_CONCATLIST:
	    /*
	     * Convert to mutable concat then retry.
	     */

	    ConvertToMConcatNode(nodePtr, parent);
	    goto start;

	case WORD_TYPE_SUBLIST:
	    if (WORD_SUBLIST_DEPTH(*nodePtr) >= 1) {
		/*
		 * Source is concat. Convert to mutable concat then retry.
		 */

		ConvertToMConcatNode(nodePtr, parent);
		goto start;
	    }
	    /* continued. */
	default: 
	    if (index > 0 && index < length) {
		/*
		 * Split at insertion point then retry.
		 */

		SplitMutableAt(nodePtr, parent, index);
		goto start;
	    }

	    /*
	     * Build a mutable concat node with current node and list to insert.
	     */

	    node = (Col_Word) AllocCells(1);
	    if (index == 0) {
		/*
		 * Insert before.
		 */

		WORD_MCONCATLIST_INIT(node, 1, length+listLength, 
			listLength, list, *nodePtr);
	    } else {
		/*
		 * Insert after.
		 */

		ASSERT(index >= length);
		WORD_MCONCATLIST_INIT(node, 1, length+listLength, length, 
			*nodePtr, list);
	    }
	    UpdateMConcatNode(node);
	    *nodePtr = node;
	    Col_WordSetModified(parent);
	    return;
    }
}

static void
SplitMutableAt(
    Col_Word *nodePtr, 
    Col_Word parent, 
    size_t index)
{
    size_t length;
    Col_Word *elements;
    Col_Word converted;
    int type;

    length = Col_ListLength(*nodePtr);
    ASSERT(index != 0 && index < length);
    type = WORD_TYPE(*nodePtr);
    switch (type) {
	case WORD_TYPE_VECTOR:
	case WORD_TYPE_MVECTOR:
	    elements = WORD_VECTOR_ELEMENTS(*nodePtr);
	    break;

	case WORD_TYPE_VOIDLIST:
	    elements = NULL;
	    break;

	case WORD_TYPE_SUBLIST: {
	    ASSERT(WORD_SUBLIST_DEPTH(*nodePtr) == 0);
	    elements = WORD_VECTOR_ELEMENTS(WORD_SUBLIST_SOURCE(*nodePtr))
		    + WORD_SUBLIST_FIRST(*nodePtr);
	    break;
	}

	default: 
	    /* CANTHAPPEN */
	    ASSERT(0);
    }

    /*
     * Build a mutable concat node.
     */

    converted = (Col_Word) AllocCells(1);
    if (!elements) {
	ASSERT(type == WORD_TYPE_VOIDLIST);
	WORD_MCONCATLIST_INIT(converted, 1, length, index, 
		WORD_VOIDLIST_NEW(index), 
		WORD_VOIDLIST_NEW(length-index));
    } else {
	Col_Word right = Col_NewMVector(0, length-index, elements+index);

	if (type == WORD_TYPE_MVECTOR) {
	    /*
	     * Shrink existing vector and use as left arm.
	     */

	    Col_MVectorSetLength(*nodePtr, index);
	    WORD_MCONCATLIST_INIT(converted, 1, length, index, *nodePtr, 
		    right);
	} else {
	    /*
	     * Build new vector for left arm.
	     */

	    WORD_MCONCATLIST_INIT(converted, 1, length, index, 
		    Col_NewMVector(0, index, elements), right);
	}
    }

    *nodePtr = converted;
    Col_WordSetModified(parent);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_MListRemove --
 *
 *	Remove a range of elements from a mutable list.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The target list is modified.
 *
 *---------------------------------------------------------------------------
 */

void
Col_MListRemove(
    Col_Word mlist,		/* Mutable list to modify. */
    size_t first, size_t last)	/* Range of chars to remove. */
{
    size_t length, loop;

    if (first > last) {
	/* 
	 * No-op. 
	 */

	return;
    }

    length = Col_ListLength(mlist);
    if (length == 0) {
	/*
	 * No-op.
	 */

	return;
    }

    loop = Col_ListLoopLength(mlist);
    if (loop == 0) {
	if (first >= length) {
	    /*
	     * Removal past the end of non-cyclic list is no-op.
	     */

	    return;
	}
	if (last >= length) {
	    /*
	     * Normalize index.
	     */

	    last = length-1;
	}
    } else {
	/*
	 * Normalize indices.
	 */

	if (first > length-loop) {
	    first = (first - (length-loop)) % loop + (length-loop);
	    if (first == length-loop) {
		/* 
		 * Keep within loop.
		 */

		first = length;
	    }
	}
	if (last > length-loop) {
	    last = (last - (length-loop)) % loop + (length-loop);
	}

	if (first > length-loop && last-first+1 == 0) {
	    /*
	     * Removal of a multiple of loop is no-op.
	     */

	    return;
	}
    }

    ASSERT(last < length);

    if (last < first) {
	/*
	 * Keep inner part of loop.
	 */

	ASSERT(loop > 0);
	ASSERT(first > length-loop);
	ASSERT(last >= length-loop);

	loop = first-last-1;
	if (first < length) {
	    MListRemove(&WORD_LIST_ROOT(mlist), first, length-1, mlist);
	}
    } else if (first > length-loop) {
	/*
	 * Shorten loop.
	 */

	ASSERT(loop > 0);
	ASSERT(last > length-loop);

	loop -= (last-first+1);
	ASSERT(loop > 0);

	MListRemove(&WORD_LIST_ROOT(mlist), first, last, mlist);
    } else if (last >= length-loop) {
	/*
	 * Keep loop length, only change loop start: append beginning of loop 
	 * after removal.
	 */

	Col_Word loopBegin = Col_Sublist(mlist, length-loop, last);

	ASSERT(loop > 0);

	MListRemove(&WORD_LIST_ROOT(mlist), first, last, mlist);
	MListInsert(&WORD_LIST_ROOT(mlist), length-(last-first+1), loopBegin,
		mlist);
    } else {
	/*
	 * Removal outside of loop.
	 */

	MListRemove(&WORD_LIST_ROOT(mlist), first, last, mlist);
    }
    WORD_LIST_LOOP(mlist) = loop;
}

static void
MListRemove(
    Col_Word * nodePtr, 
    size_t first, 
    size_t last,
    Col_Word parent)
{
    size_t length = Col_ListLength(*nodePtr);
    Col_Word node;
    int type;

    ASSERT(first <= last);
    ASSERT(last < length);

    if (first == 0 && last == length-1) {
	/*
	 * Delete all content.
	 */

	*nodePtr = WORD_LIST_EMPTY;
	return;
    }

    /*
     * Entry point for tail recursive calls.
     */

start:

    /*
     * First alter mutable types.
     */

    type = WORD_TYPE(*nodePtr);
    switch (type) {
	case WORD_TYPE_MCONCATLIST: {
	    size_t leftLength = WORD_CONCATLIST_LEFT_LENGTH(*nodePtr);
	    if (leftLength == 0) {
		leftLength = Col_ListLength(WORD_CONCATLIST_LEFT(*nodePtr));
	    }

	    if (last >= leftLength) {
		/*
		 * Remove part on right arm.
		 */

		MListRemove(&WORD_CONCATLIST_RIGHT(*nodePtr), 
			(first<leftLength?0:first-leftLength), 
			last-leftLength, *nodePtr);
	    }
	    if (first < leftLength) {
		/*
		 * Remove part on left arm.
		 */

		MListRemove(&WORD_CONCATLIST_LEFT(*nodePtr), first, 
			(last<leftLength?last:leftLength-1), *nodePtr);
		leftLength = Col_ListLength(WORD_CONCATLIST_LEFT(*nodePtr));
	    }

	    if (leftLength == 0) {
		/*
		 * Whole left arm removed, replace node by right.
		 */

		*nodePtr = WORD_CONCATLIST_RIGHT(*nodePtr);
		Col_WordSetModified(parent);
	    } else if (Col_ListLength(WORD_CONCATLIST_RIGHT(*nodePtr)) 
		    == 0) {
		/*
		 * Whole right arm removed, replace node by left.
		 */

		*nodePtr = WORD_CONCATLIST_LEFT(*nodePtr);
		Col_WordSetModified(parent);
	    } else {
		/*
		 * Update & rebalance node.
		 */

		UpdateMConcatNode(*nodePtr);
	    }
	    return;
	}

	case WORD_TYPE_MVECTOR:
	    if (last < length-1) {
		/*
		 * Move trailing elements.
		 */

		Col_Word *elements = WORD_VECTOR_ELEMENTS(*nodePtr);
		memmove(elements+first, elements+last+1, (length-last-1)
			* sizeof(*elements));
	    }

	    /*
	     * Shorten mutable vector, keeping reserved size.
	     */

	    Col_MVectorSetLength(*nodePtr, length - (last-first+1));
	    return;

	case WORD_TYPE_VOIDLIST:
	    /*
	     * Shorten void list.
	     */

	    *nodePtr = WORD_VOIDLIST_NEW(length - (last-first+1));
	    return;
    }

    if (length - (last-first+1) <= MAX_SHORT_MVECTOR_LENGTH) {
	/*
	 * Merge all chunks into one mutable vector for short lists.
	 * TODO: geometric growth?
	 */

	MergeChunksInfo info; 

	node = Col_NewMVector(0, length - (last-first+1), NULL);

	info.length = 0;
	info.vector = node;
	Col_TraverseListChunks(1, nodePtr, 0, first, 
		MergeChunksProc, &info, NULL);
	Col_TraverseListChunks(1, nodePtr, last+1, length-last, 
		MergeChunksProc, &info, NULL);
	ASSERT(info.length == length-(last-first+1));

	*nodePtr = node;
	Col_WordSetModified(parent);
	return;
    }

    /*
     * General case with immutable types.
     */

    switch (type) {
	case WORD_TYPE_CONCATLIST:
	    /*
	     * Convert to mutable concat then retry.
	     */

	    ConvertToMConcatNode(nodePtr, parent);
	    goto start;

	case WORD_TYPE_SUBLIST: 
	    if (WORD_SUBLIST_DEPTH(*nodePtr) >= 1) {
		/*
		 * Source is concat. Convert to mutable concat then retry.
		 */

		ConvertToMConcatNode(nodePtr, parent);
		goto start;
	    }
	    /* continued. */
	default: 
	    if (first == 0) {
		/*
		 * Remove head: keep tail.
		 */

		node = Col_Sublist(*nodePtr, last+1, length-1);
	    } else if (last >= length-1) {
		/*
		 * Remove tail: keep head.
		 */

		node = Col_Sublist(*nodePtr, 0, first-1);
	    } else {
		/*
		 * Merge head & tail into a mutable concat.
		 */

		node = (Col_Word) AllocCells(1);
		WORD_MCONCATLIST_INIT(node, 1, length-(last-first+1), first, 
			Col_Sublist(*nodePtr, 0, first-1), Col_Sublist(*nodePtr,
			last+1, length-1));
	    }
	    *nodePtr = node;
	    Col_WordSetModified(parent);
	    return;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ListAt --
 *
 *	Get the element of a list at a given position. 
 *
 * Results:
 *	If the index is past the end of the list, nil, else the element.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_ListAt(
    Col_Word list,		/* List to get element from. */
    size_t index)		/* Element index. */
{
    Col_ListIterator it;
    
    Col_ListIterBegin(list, index, &it);
    return Col_ListIterAt(&it);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_RepeatList --
 *
 *	Create a list formed by the repetition of a source list.
 *
 *	This method is based on recursive concatenations of the list
 *	following the bit pattern of the count factor. Doubling a list simply 
 *	consists of a concat with itself. In the end the resulting tree is 
 *	very compact, and only a minimal number of extraneous cells are 
 *	allocated during the balancing process (and will be eventually 
 *	collected).
 *
 * Results:
 *	Nil if count is zero.
 *	Else, the repetition of the source list.
 *
 * Side effects:
 *	New lists may be created.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_RepeatList(
    Col_Word list,		/* The list to repeat. */
    size_t count)		/* Repetition factor. */
{
    Col_Word listRoot;		/* Actual root node. */

    switch (WORD_TYPE(list)) {
	case WORD_TYPE_WRAP:
	    return Col_RepeatList(WORD_WRAP_SOURCE(list), count);

	case WORD_TYPE_VOIDLIST:
	case WORD_TYPE_VECTOR:
	case WORD_TYPE_MVECTOR:
	case WORD_TYPE_CONCATLIST:
	case WORD_TYPE_SUBLIST:
	    listRoot = list;
	    break;

	case WORD_TYPE_LIST:
	case WORD_TYPE_MLIST:
	    if (WORD_LIST_LOOP(list)) {
		/*
		 * List is cyclic, won't repeat.
		 */

		return list;
	    }
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /*
	     * Invalid type.
	     */

	    Col_Error(COL_ERROR, "%x is not an immutable list", list);
	    return WORD_NIL;
    }

    if (Col_ListLength(listRoot) == 0) {
	/*
	 * No-op.
	 */

	return WORD_LIST_EMPTY;
    }

    /* 
     * Quick cases. 
     */

    if (count == 0) {return WORD_NIL;}
    if (count == 1) {return list;}
    if (count == 2) {return Col_ConcatLists(list, list);}

    if (count & 1) {
	/* Odd.*/
	return Col_ConcatLists(list, Col_RepeatList(list, count-1));
    } else {
	/* Even. */
	return Col_RepeatList(Col_ConcatLists(list, list), count>>1);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ListInsert --
 *
 *	Insert a list into another one, just before the given insertion point.
 *
 *	Only perform minimal tests to prevent overflow, basic ops should 
 *	perform further optimizations anyway.
 *
 *	As list are immutable, the result is a new list.
 *
 *	Insertion past the end of the list results in a concatenation.
 *
 * Results:
 *	The resulting list.
 *
 * Side effects:
 *	New lists may be created.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_ListInsert(
    Col_Word into,		/* Where to insert. */
    size_t index,		/* Index of insertion point. */
    Col_Word list)		/* List to insert. */
{
    size_t length, loop, listLength, listLoop;
    Col_Word listRoot;

    listLength = Col_ListLength(list);
    if (listLength == 0) {
	/*
	 * No-op.
	 */

	return into;
    }

    if (index == 0) {
	/*
	 * Insert at beginning.
	 */

	return Col_ConcatLists(list, into);
    }

    length = Col_ListLength(into);
    loop = Col_ListLoopLength(into);
    if (loop == 0 && index >= length) {
	/*
	 * Insertion past the end of non-cyclic list is concatenation.
	 */

	return Col_ConcatLists(into, list);
    }

    if (loop > 0 && index > length-loop) {
	/*
	 * Insertion within loop. Normalize index and increase loop length.
	 */

	index = (index - (length-loop)) % loop + (length-loop);
	if (index == length-loop) index = length;
	loop += listLength;
    }

    listRoot = Col_ConcatLists(Col_Sublist(into, 0, index-1), list);

    listLoop = Col_ListLoopLength(list);
    if (listLoop) {
	/*
	 * Inserted list is cyclic, its loop closes the resulting list.
	 */

	return NewListWord(listRoot, listLoop);
    }
    
    if (index <= length-1) {
	/*
	 * Append trailing sublist.
	 */

	listRoot = Col_ConcatLists(listRoot, Col_Sublist(into, index, 
		length-1));
    }
    return NewListWord(listRoot, loop);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ListRemove --
 *
 *	Remove a range of elements from a list.
 *
 *	Only perform minimal tests to prevent overflow, basic ops should 
 *	perform further optimizations anyway.
 *
 *	As lists are immutable, the result is a new list.
 *
 * Results:
 *	The resulting list.
 *
 * Side effects:
 *	New lists may be created.
 *
 *---------------------------------------------------------------------------
 */

Col_Word 
Col_ListRemove(
    Col_Word list,		/* List to remove sequence from. */
    size_t first, size_t last)	/* Range of chars to remove. */
{
    size_t length, loop;
    Col_Word listRoot;

    if (first > last) {
	/* 
	 * No-op. 
	 */

	return list;
    }

    length = Col_ListLength(list);
    if (length == 0) {
	/*
	 * No-op.
	 */

	return list;
    }

    loop = Col_ListLoopLength(list);
    if (loop == 0 && first >= length) {
	/*
	 * Removal past the end of non-cyclic list is no-op.
	 */

	return list;
    }

    if (loop > 0) {
	/*
	 * Normalize indices.
	 */

	if (first > length-loop) {
	    first = (first - (length-loop)) % loop + (length-loop);
	    if (first == length-loop) {
		/* 
		 * Keep within loop.
		 */

		first = length;
	    }
	}
	if (last > length-loop) {
	    last = (last - (length-loop)) % loop + (length-loop);
	}

	if (first > length-loop && last-first+1 == 0) {
	    /*
	     * Removal of a multiple of loop is no-op.
	     */

	    return list;
	}
    }

    listRoot = WORD_LIST_EMPTY;
    if (first > 0) {
	/*
	 * Keep beginning of list.
	 */

	listRoot = Col_Sublist(list, 0, first-1);
    }

    if (last < first) {
	/*
	 * Keep inner part of loop (shorten it as it's already part of listRoot,
	 * see above).
	 */

	ASSERT(loop > 0);
	ASSERT(first > length-loop);
	ASSERT(last >= length-loop);

	loop = first-last-1;
    } else {
	if (last < length-1) {
	    /* 
	     * Keep end of list.
	     */

	    listRoot = Col_ConcatLists(listRoot, Col_Sublist(list, last+1, 
		    length-1));
	}
	if (first > length-loop) {
	    /*
	     * Shorten loop.
	     */

	    loop -= (last-first+1);
	} else if (last >= length-loop) {
	    /*
	     * Append beginning of loop.
	     */

	    listRoot = Col_ConcatLists(listRoot, Col_Sublist(list, length-loop, 
		    last));
	}
    }

    return NewListWord(listRoot, loop);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ListReplace --
 *
 *	Replace a range of characters in a list with another.
 *
 *	Only perform minimal tests to prevent overflow, basic ops should 
 *	perform further optimizations anyway.
 *
 *	As lists are immutable, the result is a new list.
 *
 * Results:
 *	The resulting list.
 *
 * Side effects:
 *	New lists may be created.
 *
 *---------------------------------------------------------------------------
 */

Col_Word 
Col_ListReplace(
    Col_Word list,		/* Original list. */
    size_t first, size_t last,	/* Inclusive range of chars to replace. */
    Col_Word with)		/* Replacement list. */
{
    size_t length, loop;
    Col_Word result;

    if (first > last) {
	/* 
	 * No-op. 
	 */

	return list;
    }

    length = Col_ListLength(list);
    loop = Col_ListLoopLength(list);

    /*
     * First remove range to replace...
     */

    result = Col_ListRemove(list, first, last);

    if (loop > 0 && first > length) {
	/*
	 * Insertion point may have moved. Normalize indices.
	 */

	first = (first - (length-loop)) % loop + (length-loop);
	if (first == length-loop) {
	    /* 
	     * Keep within loop.
	     */

	    first = length;
	}
    }

    /* 
     * .. then insert list at beginning of range.
     */

    return Col_ListInsert(result, first, with);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_MListReplace --
 *
 *	Replace a range of characters in a mutable list with another.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The target list is modified.
 *
 *---------------------------------------------------------------------------
 */

void
Col_MListReplace(
    Col_Word mlist,		/* Mutable list to modify. */
    size_t first, size_t last,	/* Inclusive range of chars to replace. */
    Col_Word with)		/* Replacement list. */
{
    size_t length, loop;

    if (WORD_TYPE(mlist) != WORD_TYPE_MLIST) {
	Col_Error(COL_ERROR, "%x is not a mutable list", mlist);
	return;
    }

    if (first > last) {
	/* 
	 * No-op. 
	 */

	return;
    }

    length = Col_ListLength(mlist);
    loop = Col_ListLoopLength(mlist);

    /*
     * First remove range to replace...
     */

    Col_MListRemove(mlist, first, last);

    if (loop > 0 && first > length) {
	/*
	 * Insertion point may have moved. Normalize indices.
	 */

	first = (first - (length-loop)) % loop + (length-loop);
	if (first == length-loop) {
	    /* 
	     * Keep within loop.
	     */

	    first = length;
	}
    }

    /* 
     * .. then insert list at beginning of range.
     */

    Col_MListInsert(mlist, first, with);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_TraverseListChunks --
 *
 *	Iterate over the chunks of a number of lists.
 *
 *	For each traversed chunk, proc is called back with the opaque data as
 *	well as the position within the lists. If it returns a non-zero result 
 *	then the iteration ends. 
 *
 *	The algorithm is naturally recursive but this implementation avoids
 *	recursive calls thanks to a stack-allocated backtracking structure.
 *
 * Results:
 *	The return value of the last called proc, or -1 if no traversal was
 *	performed.
 *	If non-NULL, the value pointed by lengthPtr is incremented by the total
 *	length of the traversed lists, i.e. the number of elements.
 *
 * Side effects:
 *	If non-NULL, the value pointed to by lengthPtr is updated.
 *
 *---------------------------------------------------------------------------
 */

int 
Col_TraverseListChunks(
    size_t number,		/* Number of lists to traverse. */
    Col_Word *lists,		/* Lists to traverse. */
    size_t start,		/* Index of first element. */
    size_t max,			/* Max number of elements. */
    Col_ListChunksTraverseProc *proc,	
				/* Callback proc called on each chunk. */
    Col_ClientData clientData,	/* Opaque data passed as is to above proc. */
    size_t *lengthPtr)		/* If non-NULL, incremented by the total number 
				 * of elements traversed upon completion. */
{
    struct {
	/*
	 * Non-recursive backtracking structure. Since the algorithm only 
	 * recurses on concat nodes and since we know their depth, we can 
	 * allocate the needed space on the stack.
	 */

	struct {
	    int prevDepth;
	    Col_Word list;
	    size_t max;
	} *backtracks;

	Col_Word list;
	size_t start, max;
	int maxDepth, prevDepth;
    } *info;
    size_t i;
    const Col_Word **elements;
    int type;
    int result;

    info = alloca(sizeof(*info) * number);
    elements = alloca(sizeof(*elements) * number);

    for (i=0; i < number; i++) {
	size_t listLength = Col_ListLength(lists[i]);
	if (start > listLength) {
	    /* 
	     * Start is past the end of the list.
	     */

	    info[i].max = 0;
	} else if (max > listLength-start) {
	    /* 
	     * Adjust max to the remaining length. 
	     */

	    info[i].max = listLength-start;
	} else {
	    info[i].max = max;
	}
    }

    max = 0;
    for (i=0; i < number; i++) {
	if (max < info[i].max) {
	    max = info[i].max;
	}
    }
    if (max == 0) {
	/*
	 * Nothing to traverse.
	 */

	return -1;
    }

    for (i=0; i < number; i++) {
	ASSERT(info[i].max <= max);
	info[i].list = lists[i];
	info[i].backtracks = NULL;
	info[i].start = start;
	info[i].maxDepth = 0;
	info[i].prevDepth = INT_MAX;
    }

    for (;;) {
	for (i=0; i < number; i++) {
	    if (!info[i].list) {
		/*
		 * Past end of list.
		 */

		elements[i] = NULL;
		continue;
	    }

	    for (;;) {
		/*
		 * Descend into structure until we find a suitable leaf.
		 */

		type = WORD_TYPE(info[i].list);
		switch (type) {
		    case WORD_TYPE_SUBLIST:
			/* 
			 * Sublist: recurse on source list.
			 */

			info[i].start += WORD_SUBLIST_FIRST(info[i].list);
			info[i].list = WORD_SUBLIST_SOURCE(info[i].list);
			continue;
            	    
		    case WORD_TYPE_CONCATLIST:
		    case WORD_TYPE_MCONCATLIST: {
			/* 
			 * Concat: descend into covered arms.
			 */

			int depth;
			size_t leftLength = WORD_CONCATLIST_LEFT_LENGTH(
				info[i].list);
			if (leftLength == 0) {
			    leftLength = Col_ListLength(
				    WORD_CONCATLIST_LEFT(info[i].list));
			}
			if (info[i].start + info[i].max <= leftLength) {
			    /* 
			     * Recurse on left arm only. 
			     */

			    info[i].list = WORD_CONCATLIST_LEFT(info[i].list);
			    continue;
			}
			if (info[i].start >= leftLength) {
			    /* 
			     * Recurse on right arm only. 
			     */

			    info[i].start -= leftLength;
			    info[i].list = WORD_CONCATLIST_RIGHT(info[i].list);
			    continue;
			} 

			/*
			 * Push right onto stack and recurse on left.
			 */

			depth = WORD_CONCATLIST_DEPTH(info[i].list);
			if (!info[i].backtracks) {
			    ASSERT(info[i].maxDepth == 0);
			    ASSERT(info[i].prevDepth == INT_MAX);
			    info[i].maxDepth = depth;
			    info[i].backtracks = alloca(
				    sizeof(*info[i].backtracks) * depth);
			} else {
			    ASSERT(depth <= info[i].maxDepth);
			}
			info[i].backtracks[depth-1].prevDepth 
				= info[i].prevDepth;
			info[i].backtracks[depth-1].list 
				= WORD_CONCATLIST_RIGHT(info[i].list);
			info[i].backtracks[depth-1].max = info[i].max
				- (leftLength-info[i].start);
			info[i].prevDepth = depth;

			info[i].max = leftLength-info[i].start;
			info[i].list = WORD_CONCATLIST_LEFT(info[i].list);
			continue;
		    }

		    case WORD_TYPE_LIST:
		    case WORD_TYPE_MLIST:
			/* 
			 * Recurse on list root.
			 */

			info[i].list = WORD_LIST_ROOT(info[i].list);
			continue;

		    case WORD_TYPE_WRAP:
			/* 
			 * Recurse on source.
			 */

			info[i].list = WORD_WRAP_SOURCE(info[i].list);
			continue;
		}
		break;
	    }

	    /*
	     * Get leaf data.
	     */

	    switch (type) {
		case WORD_TYPE_VECTOR:
		case WORD_TYPE_MVECTOR:
		    elements[i] = WORD_VECTOR_ELEMENTS(info[i].list)
			    + info[i].start;
		    break;

		case WORD_TYPE_VOIDLIST:
		    elements[i] = COL_LISTCHUNK_VOID;
		    break;

		/* WORD_TYPE_UNKNOWN */

		default:
		    /* CANTHAPPEN */
		    ASSERT(0);
		    return 0;
	    }
	}

	/*
	 * Limit chunk lengths to the shortest one.
	 */

	max = SIZE_MAX;
	for (i=0; i < number; i++) {
	    if (info[i].list && max > info[i].max) {
		max = info[i].max;
	    }
	}

	/*
	 * Call proc on leaves' data.
	 */

	if (lengthPtr) *lengthPtr += max;
	result = proc(start, max, number, elements, clientData);
	start += max;
	if (result != 0) {
	    /*
	     * Stop there.
	     */

	    return result;
	} else {
	    /*
	     * Continue iteration.
	     */

	    size_t nbEnd = 0;
	    for (i=0; i < number; i++) {
		ASSERT(info[i].max >= max);
		info[i].max -= max;
		if (info[i].max > 0) {
		    info[i].start += max;
		} else if (info[i].prevDepth == INT_MAX) {
		    /*
		     * Already at toplevel => end of list.
		     */

		    info[i].list = WORD_NIL;
		    nbEnd++;
		} else {
		    /*
		     * Reached end of leaf, backtrack.
		     */

		    ASSERT(info[i].backtracks);
		    info[i].list = info[i].backtracks[info[i].prevDepth-1].list;
		    info[i].max = info[i].backtracks[info[i].prevDepth-1].max;
		    info[i].start = 0;
		    info[i].prevDepth 
			    = info[i].backtracks[info[i].prevDepth-1].prevDepth;
		}
	    }
	    if (nbEnd == number) {
		/*
		 * Reached end of all lists, stop there.
		 */

		return 0;
	    }
	}
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ListIterBegin --
 *
 *	Initialize the list iterator so that it points to the index-th
 *	element within the list.
 *
 * Results:
 *	Whether the iterator looped or not.
 *
 * Side effects:
 *	If index points past the end of the list, the iterator is initialized 
 *	to the end iterator (i.e. whose list field is nil), else it points 
 *	to the element within the list.
 *
 *---------------------------------------------------------------------------
 */

int
Col_ListIterBegin(
    Col_Word list,		/* List to iterate over. */
    size_t index,		/* Index of element. */
    Col_ListIterator *it)	/* Iterator to initialize. */
{
    size_t length;
    int looped=0;

    length = Col_ListLength(list);
    if (index >= length) {
	size_t loop = Col_ListLoopLength(list);
	if (!loop) {
	    /*
	     * End of list.
	     */

	    it->list = WORD_NIL;
	    return looped;
	}

	/*
	 * Cyclic list. Normalize index.
	 */

	looped = 1;
	index = (index - (length-loop)) % loop + (length-loop);
    }

    it->list = list;
    it->index = index;

    /*
     * Traversal info will be lazily computed.
     */

    it->traversal.subnode = WORD_NIL;
    it->traversal.leaf = WORD_NIL;

    return looped;
}

/*
 *---------------------------------------------------------------------------
 *
 * IterAtVector --
 * IterAtVoid --
 *
 *	Element accessors for list iterators.
 *
 * Results:
 *	The element at the given position in the leaf.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

static Col_Word IterAtVector(
    Col_Word leaf, 
    size_t index
)
{
    return WORD_VECTOR_ELEMENTS(leaf)[index];
}
static Col_Word IterAtVoid(
    Col_Word leaf, 
    size_t index
)
{
    return WORD_NIL;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColListIterUpdateTraversalInfo --
 *
 *	Get the deepest subnodes needed to access the current element 
 *	designated by the iterator.
 *
 *	Iterators point to the leaf containing the current element. To avoid 
 *	rescanning the whole tree when leaving this leaf, it also stores a
 *	higher level subnode containing this leaf, so that traversing all this
 *	subnode's children is fast. The subnode is the leaf's highest ancestor
 *	with a maximum depth of MAX_ITERATOR_SUBNODE_DEPTH. Some indices are
 *	stored along with this sublist in a way that traversal can be resumed
 *	quickly: If the current index is withing the sublist's range of 
 *	validity, then traversal starts at the sublist, else it restarts from
 *	the root.
 *
 *	Traversal info is updated lazily, each time element data needs to
 *	be retrieved. This means that a blind iteration over an arbitrarily 
 *	complex list is no more computationally intensive than over a flat 
 *	array.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Iterator is updated.
 *
 *---------------------------------------------------------------------------
 */

void
ColListIterUpdateTraversalInfo(
    Col_ListIterator *it)	/* The iterator to update. */
{
    Col_Word node;
    size_t first, last, offset;

    if (it->traversal.subnode && (it->index < it->traversal.first 
		    || it->index > it->traversal.last)) {
	/*
	 * Out of range.
	 */

	it->traversal.subnode = WORD_NIL;
    }

    /*
     * Search for leaf node, remember containing subnode in the process.
     */

    if (it->traversal.subnode) {
	node = it->traversal.subnode;
    } else {
	node = it->list;
	it->traversal.first = 0;
	it->traversal.last = SIZE_MAX;
	it->traversal.offset = 0;
    }
    first = it->traversal.first;
    last = it->traversal.last;
    offset = it->traversal.offset;

    it->traversal.leaf = WORD_NIL;
    while (!it->traversal.leaf) {
	size_t subFirst=first, subLast=last;

	switch (WORD_TYPE(node)) {
	    case WORD_TYPE_VOIDLIST:
		it->traversal.leaf = node;
		it->traversal.index = it->index - offset;
		it->traversal.proc = IterAtVoid;
		ASSERT(it->traversal.index < WORD_VOIDLIST_LENGTH(node));
		break;

	    case WORD_TYPE_VECTOR:
	    case WORD_TYPE_MVECTOR:
		it->traversal.leaf = node;
		it->traversal.index = it->index - offset;
		it->traversal.proc = IterAtVector;
		ASSERT(it->traversal.index < WORD_VECTOR_LENGTH(node));
		break;

	    case WORD_TYPE_LIST:
	    case WORD_TYPE_MLIST:
		/*
		 * Recurse into root.
		 */

		node = WORD_LIST_ROOT(node);
		break;

	    case WORD_TYPE_SUBLIST: 
		/*
		 * Always remember as subnode.
		 */

		it->traversal.subnode = node;
		it->traversal.first = first;
		it->traversal.last = last;
		it->traversal.offset = offset;

		/*
		 * Recurse into source.
		 * Note: offset may become negative (in 2's complement) but it 
		 * doesn't matter.
		 */

		offset -= WORD_SUBLIST_FIRST(node);
		subLast = first - WORD_SUBLIST_FIRST(node) 
			+ WORD_SUBLIST_LAST(node);
		node = WORD_SUBLIST_SOURCE(node);
		break;

	    case WORD_TYPE_CONCATLIST:
	    case WORD_TYPE_MCONCATLIST: {
		size_t leftLength = WORD_CONCATLIST_LEFT_LENGTH(node);
		if (leftLength == 0) {
		    leftLength = Col_ListLength(WORD_CONCATLIST_LEFT(node));
		}
		if (WORD_CONCATLIST_DEPTH(node) == MAX_ITERATOR_SUBNODE_DEPTH
			|| !it->traversal.subnode) {
		    /*
		     * Remember as subnode.
		     */

		    it->traversal.subnode = node;
		    it->traversal.first = first;
		    it->traversal.last = last;
		    it->traversal.offset = offset;
		}

		if (it->index - offset < leftLength) {
		    /*
		     * Recurse into left arm.
		     */

		    subLast = offset + leftLength-1;
		    node = WORD_CONCATLIST_LEFT(node);
		} else {
		    /*
		     * Recurse into right arm.
		     */

		    subFirst = offset + leftLength;
		    offset += leftLength;
		    node = WORD_CONCATLIST_RIGHT(node);
		}
		break;
	    }

	    /* WORD_TYPE_UNKNOWN */

	    default:
		/* CANTHAPPEN */
		ASSERT(0);
		return;
	}

	/*
	 * Shorten validity range.
	 */

	if (subFirst > first) first = subFirst;
	if (subLast < last) last = subLast;
    }
    if (!it->traversal.subnode) {
	it->traversal.subnode = it->traversal.leaf;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ListIterCompare --
 *
 *	Compare two iterators by their respective positions.
 *
 * Results:
 *	-  < 0 if it1 is before it2
 *	-  > 0 if it1 is after it2
 *	-  0 if they are equal
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

int
Col_ListIterCompare(
    Col_ListIterator *it1,	/* First iterator. */
    Col_ListIterator *it2)	/* Second iterator. */
{
    if (Col_ListIterEnd(it1)) {
	if (Col_ListIterEnd(it2)) {
	    return 0;
	} else {
	    return 1;
	}
    } else if (Col_ListIterEnd(it2)) {
	return -1;
    } else if (it1->index < it2->index) {
	return -1;
    } else if (it1->index > it2->index) {
	return 1;
    } else {
	return 0;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ListIterForward --
 *
 *	Move the iterator to the nb-th next element.
 *
 * Results:
 *	Whether the iterator looped or not.
 *
 * Side effects:
 *	Update the iterator.
 *
 *---------------------------------------------------------------------------
 */

int
Col_ListIterForward(
    Col_ListIterator *it,	/* The iterator to move. */
    size_t nb)			/* Offset. */
{
    size_t length;
    int looped=0;

    if (Col_ListIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid list iterator");
	return looped;
    }

    if (nb == 0) {
	/*
	 * No-op.
	 */

	return looped;
    }

    /*
     * Check for end of list.
     */
    
    length = Col_ListLength(it->list);
    if (nb >= length - it->index) {
	size_t loop = Col_ListLoopLength(it->list);
	if (!loop) {
	    /*
	     * End of list.
	     */

	    it->list = WORD_NIL;
	    return looped;
	}

	/*
	 * Cyclic list.
	 */

	looped = 1;
	if (it->index < length-loop) {
	    /*
	     * Currently before loop, forward into loop.
	     */

	    size_t nb1 = (length-loop) - it->index;
	    ASSERT(nb >= nb1);
	    nb = nb1 + (nb-nb1) % loop;
	    it->index += nb;
	} else {
	    /*
	     * Currently within loop.
	     */

	    nb %= loop;
	    if (it->index >= length-nb) {
		/*
		 * Loop backward.
		 */

		Col_ListIterBackward(it, loop-nb);
		return looped;
	    } else {
		/* 
		 * Forward.
		 */

		it->index += nb;
	    }
	}
    } else {
	it->index += nb;
    }

    if (!it->traversal.subnode || !it->traversal.leaf) {
	/*
	 * No traversal info.
	 */

	return looped;
    }
    if (it->index > it->traversal.last) {
	/*
	 * Traversal info out of range. Discard leaf node info, but not 
	 * subnode, as it may be used again should the iteration go back.
	 */

	it->traversal.leaf = WORD_NIL;
	return looped;
    }

    /*
     * Update traversal info.
     */

    switch (WORD_TYPE(it->traversal.leaf)) {
	case WORD_TYPE_VECTOR:
	case WORD_TYPE_MVECTOR:
	    if (nb >= WORD_VECTOR_LENGTH(it->traversal.leaf) 
		    - it->traversal.index) {
		/*
		 * Reached end of leaf.
		 */

		it->traversal.leaf = WORD_NIL;
	    } else {
		it->traversal.index += nb;
	    }
	    break;

	case WORD_TYPE_VOIDLIST:
	    if (nb >= WORD_VOIDLIST_LENGTH(it->traversal.leaf) 
		    - it->traversal.index) {
		/*
		 * Reached end of leaf.
		 */

		it->traversal.leaf = WORD_NIL;
	    } else {
		it->traversal.index += nb;
	    }
	    break;
    }
    return looped;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ListIterBackward --
 *
 *	Move the iterator to the nb-th previous element.
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
Col_ListIterBackward(
    Col_ListIterator *it,	/* The iterator to move. */
    size_t nb)			/* Offset. */
{
    if (Col_ListIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid list iterator");
	return;
    }

    if (nb == 0) {
	/*
	 * No-op.
	 */

	return;
    }

    /*
     * Check for beginning of list.
     */
    
    if (it->index < nb) {
	it->list = WORD_NIL;
	return;
    }

    it->index -= nb;

    if (!it->traversal.subnode || !it->traversal.leaf) {
	/*
	 * No traversal info.
	 */

	return;
    }
    if (it->index < it->traversal.first) {
	/*
	 * Traversal info out of range. Discard leaf node info, but not 
	 * subnode, as it may be used again should the iteration go back.
	 */

	it->traversal.leaf = WORD_NIL;
	return;
    }

    /*
     * Update traversal info.
     */

    if (it->traversal.index < nb) {
	/*
	 * Reached beginning of leaf. 
	 */

	it->traversal.leaf = WORD_NIL;
	return;
    }

    /*
     * Go backward.
     */

    it->traversal.index -= nb;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ListIterMoveTo --
 *
 *	Move the iterator to the given absolute position.
 *
 * Results:
 *	Whether the iterator looped or not.
 *
 * Side effects:
 *	Update the iterator.
 *
 *---------------------------------------------------------------------------
 */

int
Col_ListIterMoveTo(
    Col_ListIterator *it,	/* The iterator to move. */
    size_t index)		/* Position. */
{
    if (index > it->index) {
	return Col_ListIterForward(it, index - it->index);
    } else if (index < it->index) {
	Col_ListIterBackward(it, it->index - index);
	return 0;
    } else {
	return 0;
    }
}
