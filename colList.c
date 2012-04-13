/*
 * File: colList.c
 *
 *	This file implements the list handling features of Colibri.
 *
 *	Lists are a collection datatype that allows for fast insertion, 
 *	extraction and composition of other lists. Internally they use 
 *	self-balanced binary trees, like ropes, except that they use vectors
 *	as basic containers instead of character arrays.
 *
 *	They come in both immutable and mutable forms : 
 *	
 *	- Immutable lists can be composed of immutable vectors and lists. 
 *	Immutable vectors can themselves be used in place of immutable lists. 
 *
 *	- Mutable lists can be composed of either mutable or immutable lists and
 *	vectors. They can be "frozen" and turned into immutable versions.
 *	Mutable vectors *cannot* be used in place of mutable lists, as the 
 *	latter can grow indefinitely whereas the former have a maximum length 
 *	set at creation time.
 *
 * Note:
 *	The present code is largely adapted from the rope implementation.
 *
 * See also: 
 *	<colList.h>, <colVector.h>, <colRope.c>
 */

#include "include/colibri.h"
#include "colInternal.h"

#include "colWordInt.h"
#include "colVectorInt.h"
#include "colListInt.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <malloc.h> /* For alloca */

/*
 * Prototypes for functions used only in this file.
 */

typedef struct ListChunkTraverseInfo *pListChunkTraverseInfo;
static Col_ListChunksTraverseProc MergeChunksProc;
static unsigned char	GetDepth(Col_Word list);
static Col_Word		GetRoot(Col_Word list);
static void		GetArms(Col_Word node, Col_Word * leftPtr, 
			    Col_Word * rightPtr);
static void		FreezeMList(Col_Word mlist);
static void		MListSetAt(Col_Word * nodePtr, size_t index, 
			    Col_Word element);
static Col_Word		NewMConcatList(Col_Word left, Col_Word right);
static void		ConvertToMutableAt(Col_Word *nodePtr, size_t index);
static void		SplitMutableAt(Col_Word *nodePtr, size_t index);
static void		ConvertToMConcatNode(Col_Word *nodePtr);
static void		UpdateMConcatNode(Col_Word node);
static void		MListInsert(Col_Word * nodePtr, size_t index,
			    Col_Word list);
static void		MListRemove(Col_Word * nodePtr, size_t first, 
			    size_t last);
static void		GetChunk(struct ListChunkTraverseInfo *info, 
			    const Col_Word **elements, int reverse);
static void		NextChunk(struct ListChunkTraverseInfo *info, 
			    size_t nb, int reverse);
static ColListIterLeafAtProc IterAtVector, IterAtVoid;


/*
================================================================================
Internal Section: List Internals
================================================================================
*/

/*---------------------------------------------------------------------------
 * Algorithm: List Tree Balancing
 *
 *	Large lists are built by concatenating several sublists, forming a
 *	balanced binary tree. A balanced tree is one where the depth of the
 *	left and right arms do not differ by more than one level.
 *
 *	List trees are self-balanced by construction: when two lists are 
 *	concatenated, if their respective depths differ by more than 1, then 
 *	the tree is recursively rebalanced by splitting and merging subarms. 
 *	There are four major cases, two if we consider symmetry:
 *
 *	- Deepest subtree is an outer branch (i.e. the left resp. right child of
 *	  the left resp. right arm). In this case the tree is rotated: the
 *	  opposite child is moved and concatenated with the opposite arm.
 *	  For example, with left being deepest:
 *
 * (start diagram)
 *  Before:
 *            concat = (left1 + left2) + right
 *           /      \
 *         left    right
 *        /    \
 *     left1  left2
 *    /     \
 *   ?       ?
 *
 *
 * After:
 *           concat = left1 + (left2 + right)
 *          /      \
 *     left1        concat
 *    /     \      /      \
 *   ?       ?   left2   right
 * (end)
 *
 *	- Deepest subtree is an inner branch (i.e. the right resp. left child of
 *	  the left resp. right arm). In this case the subtree is split 
 *	  between both arms. For example, with left being deepest:
 *
 * (start diagram)
 *  Before:
 *          concat = (left1 + (left21+left22)) + right
 *         /      \
 *       left    right
 *      /    \
 *   left1  left2
 *         /     \
 *      left21  left22
 *
 *
 * After:
 *              concat = (left1 + left21) + (left22 + right)
 *             /      \
 *       concat        concat
 *      /      \      /      \
 *   left1  left21  left22  right
 * (end)
 *---------------------------------------------------------------------------*/


/*
================================================================================
Section: Immutable Lists
================================================================================
*/

/****************************************************************************
 * Group: Immutable List Creation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_NewList
 *
 *	Create a new list word.
 *
 *	Short lists are created as vectors. Larger lists are recursively split
 *	in half and assembled in a tree.
 *
 * Arguments:
 *	length		- Length of below array.
 *	elements	- Array of words to populate list with. If NULL, build
 *			  a void list (i.e. whose elements are all nil).
 *
 * Result:
 *	The new word, or empty when length is zero.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewList(
    size_t length,
    const Col_Word * elements)
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


/****************************************************************************
 * Group: Immutable List Access
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_ListLength
 *
 *	Get the length of the list. Also works with vectors. If the list is
 *	cyclic, only give the core length.
 *
 * Argument:
 *	list	- List to get length for.
 *
 * Result:
 *	The list length.
 *---------------------------------------------------------------------------*/

size_t 
Col_ListLength(
    Col_Word list)
{
    switch (WORD_TYPE(list)) {
	case WORD_TYPE_WRAP:
	    return Col_ListLength(WORD_WRAP_SOURCE(list));

	case WORD_TYPE_CIRCLIST:
	    return Col_ListLength(WORD_CIRCLIST_CORE(list));

	case WORD_TYPE_VOIDLIST:
	    return WORD_VOIDLIST_LENGTH(list);

	case WORD_TYPE_VECTOR:
	case WORD_TYPE_MVECTOR:
	    return WORD_VECTOR_LENGTH(list);

	case WORD_TYPE_MLIST:
	    return Col_ListLength(WORD_MLIST_ROOT(list));

	case WORD_TYPE_SUBLIST:
	    /* 
	     * Sublist length is the range width.
	     */

	    return WORD_SUBLIST_LAST(list)-WORD_SUBLIST_FIRST(list)+1;

	case WORD_TYPE_CONCATLIST:
	case WORD_TYPE_MCONCATLIST:
	    return WORD_CONCATLIST_LENGTH(list);

	/* WORD_TYPE_UNKNOWN */

	default:
	    /*
	     * Invalid type.
	     */

	    Col_Error(COL_ERROR, "%x is not a list", list);
	    return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_ListLoopLength
 *
 *	Get the loop length of the list.
 *
 * Argument:
 *	list	- List to get loop length for.
 *
 * Result:
 *	If the list is cyclic, the terminal loop length. Else zero.
 *---------------------------------------------------------------------------*/

size_t 
Col_ListLoopLength(
    Col_Word list)
{
    switch (WORD_TYPE(list)) {
	case WORD_TYPE_WRAP:
	    return Col_ListLoopLength(WORD_WRAP_SOURCE(list));

	case WORD_TYPE_VOIDLIST:
	case WORD_TYPE_VECTOR:
	case WORD_TYPE_MVECTOR:
	case WORD_TYPE_SUBLIST:
	    return 0;

	case WORD_TYPE_CIRCLIST:
	    return Col_ListLength(WORD_CIRCLIST_CORE(list));

	case WORD_TYPE_CONCATLIST:
	case WORD_TYPE_MCONCATLIST:
	    if (WORD_TYPE(WORD_CONCATLIST_RIGHT(list)) == WORD_TYPE_CIRCLIST) {
		/*
		 * Cyclic list length is given by right arm length.
		 */

		return Col_ListLength(WORD_CONCATLIST_RIGHT(list));
	    }
	    return 0;

	case WORD_TYPE_MLIST:
	    return Col_ListLoopLength(WORD_MLIST_ROOT(list));

	/* WORD_TYPE_UNKNOWN */

	default:
	    /*
	     * Invalid type.
	     */

	    Col_Error(COL_ERROR, "%x is not a list", list);
	    return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_ListAt
 *
 *	Get the element of a list at a given position. 
 *
 * Argument:
 *	list	- List to get element from.
 *	index	- Element index.
 *
 * Result:
 *	If the index is past the end of the list, nil, else the element.
 *---------------------------------------------------------------------------*/

Col_Word
Col_ListAt(
    Col_Word list,
    size_t index)
{
    Col_ListIterator it;
    
    Col_ListIterBegin(list, index, &it);
    return Col_ListIterEnd(&it) ? WORD_NIL : Col_ListIterAt(&it);
}


/****************************************************************************
 * Group: Immutable List Operations
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Constant: MAX_SHORT_LIST_SIZE
 *
 *	Maximum number of cells a short list can take. This constant controls
 *	the creation of short leaves during sublist/concatenation. Lists built
 *	this way normally use sublist and concat nodes, but to avoid 
 *	fragmentation, multiple short lists are flattened into a single vector.
 *---------------------------------------------------------------------------*/

#define MAX_SHORT_LIST_SIZE	3

/*---------------------------------------------------------------------------
 * Internal Constant: MAX_SHORT_LIST_LENGTH
 *
 *	Maximum number of elements a short list can take.
 *
 * See also:
 *	<MAX_SHORT_LIST_SIZE>
 *---------------------------------------------------------------------------*/

#define MAX_SHORT_LIST_LENGTH	VECTOR_MAX_LENGTH(MAX_SHORT_LIST_SIZE*CELL_SIZE)

/*---------------------------------------------------------------------------
 * Internal Constant: MAX_SHORT_MVECTOR_SIZE
 *
 *	Maximum number of cells a short mutable vector can take. This constant
 *	controls the creation of short mutable vectors during immutable list 
 *	conversions. When setting an element of a mutable list, if the leaf node
 *	containing this element is immutable we convert it to a mutable node of
 *	this size in case nearby elements get modified later. This amortizes the
 *	conversion overhead over time.
 *
 *  MAX_SHORT_MVECTOR_SIZE	- 
 *  MAX_SHORT_MVECTOR_LENGTH	- Maximum number of elements a short mutable
 *				  vector can take.
 *---------------------------------------------------------------------------*/

#define MAX_SHORT_MVECTOR_SIZE		10

/*---------------------------------------------------------------------------
 * Internal Constant: MAX_SHORT_MVECTOR_LENGTH
 *
 *	Maximum number of elements a short mutable vector can take.
 *
 * See also:
 *	<MAX_SHORT_MVECTOR_SIZE>
 *---------------------------------------------------------------------------*/

#define MAX_SHORT_MVECTOR_LENGTH	VECTOR_MAX_LENGTH(MAX_SHORT_MVECTOR_SIZE*CELL_SIZE)

/*---------------------------------------------------------------------------
 * Internal Typedef: MergeChunksInfo
 *
 *	Structure used to collect data during the traversal of lists when merged
 *	into one vector.
 *
 * Fields:
 *	length	- Length so far.
 *	vector	- Vector word to copy elements into.
 *
 * See also:
 *	<MergeChunksProc>
 *---------------------------------------------------------------------------*/

typedef struct MergeChunksInfo {
    size_t length;
    Col_Word vector;
} MergeChunksInfo;

/*---------------------------------------------------------------------------
 * Internal Function: MergeChunksProc
 *
 *	List traversal procedure used to concatenate all portions of lists into
 *	one to fit wthin one vector word. Follows <Col_ListChunksTraverseProc> 
 *	signature.
 *
 * Arguments:
 *	index		- List-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 1.
 *	chunks		- Array of chunks. First element never NULL.
 *	clientData	- Points to <MergeChunksInfo>.
 *
 * Result:
 *	Always zero, info returned through clientData.
 *
 * Side effects:
 *	Copy the vector data into the associated <MergeChunksInfo> fields.
 *
 * See also:
 *	<MergeChunksInfo>, <Col_TraverseListChunks>
 *---------------------------------------------------------------------------*/

static int 
MergeChunksProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_Word **chunks, 
    Col_ClientData clientData) 
{
    MergeChunksInfo *info = (MergeChunksInfo *) clientData;

    ASSERT(number == 1);
    if (*chunks != COL_LISTCHUNK_VOID
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
    if (*chunks != COL_LISTCHUNK_VOID) {
	/* 
	 * Set elements. 
	 */

	memcpy(WORD_VECTOR_ELEMENTS(info->vector)+info->length, *chunks, 
		length * sizeof(**chunks));
    } else if (WORD_TYPE(info->vector) != WORD_TYPE_VOIDLIST) {
	/* 
	 * Initialize elements to nil. 
	 */

	memset(WORD_VECTOR_ELEMENTS(info->vector)+info->length, 0, 
		length * sizeof(**chunks));
    }
    info->length += length;
    ASSERT(info->length <= Col_ListLength(info->vector));

    return 0;
}

/*---------------------------------------------------------------------------
 * Internal Function: GetDepth
 *
 *	Get the depth of the list.
 *
 * Argument:
 *	node	- List node to get depth from.
 *
 * Result:
 *	Depth of list.
 *
 * See also:
 *	<Col_ConcatLists>
 *---------------------------------------------------------------------------*/

static unsigned char
GetDepth(
    Col_Word list)
{
    for (;;) {
	switch (WORD_TYPE(list)) {
	    case WORD_TYPE_WRAP:
		list = WORD_WRAP_SOURCE(list);
		break;

	    case WORD_TYPE_CIRCLIST:
		list = WORD_CIRCLIST_CORE(list);
		break;

	    case WORD_TYPE_SUBLIST:
		return WORD_SUBLIST_DEPTH(list);

	    case WORD_TYPE_CONCATLIST:
	    case WORD_TYPE_MCONCATLIST:
		return WORD_CONCATLIST_DEPTH(list);

	    case WORD_TYPE_MLIST:
		list = WORD_MLIST_ROOT(list);
		break;

	    /* WORD_TYPE_UNKNOWN */

	    default:
		return 0;
	}
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: GetRoot
 *
 *	Get the root of the list.
 *
 * Argument:
 *	node	- List node to get depth from.
 *
 * Result:
 *	Depth of list.
 *
 * See also:
 *	<Col_ConcatLists>
 *---------------------------------------------------------------------------*/

static Col_Word
GetRoot(
    Col_Word list)
{
    switch (WORD_TYPE(list)) {
	case WORD_TYPE_WRAP:
	    return WORD_WRAP_SOURCE(list);

	case WORD_TYPE_MLIST:
	    return WORD_MLIST_ROOT(list);

	default:
	    return list;
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: GetArms
 *
 *	Get the left and right arms of a list node (a concat or one of its 
 *	sublists).
 *
 * Argument:
 *	node		- List node to extract arms from. Either a sublist or
 *			  concat node.
 *
 * Results:
 *	leftPtr		- Left arm.
 *	rightPtr	- Right arm.
 *
 * Side effects:
 *	May create new words.
 *
 * See also:
 *	<Col_ConcatLists>
 *---------------------------------------------------------------------------*/

static void 
GetArms(
    Col_Word node,
    Col_Word * leftPtr,
    Col_Word * rightPtr)
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
	ASSERT(WORD_TYPE(node) == WORD_TYPE_CONCATLIST
		|| WORD_TYPE(node) == WORD_TYPE_MCONCATLIST);
	ASSERT(WORD_CONCATLIST_DEPTH(node) >= 1);

	*leftPtr  = WORD_CONCATLIST_LEFT(node);
	*rightPtr = WORD_CONCATLIST_RIGHT(node);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_Sublist
 *
 *	Create a new list which is a sublist of another.
 *
 *	We try to minimize the overhead as much as possible, such as: 
 *	 - identity.
 *       - create vectors for small sublists.
 *	 - sublists of sublists point to original list.
 *	 - sublists of concats point to the deepest superset sublist.
 *
 * Arguments:
 *	list		- The list to extract the sublist from.
 *	first, last	- Range of sublist.
 *
 * Result:
 *	When first is past the end of the (acyclic) list, or last is before 
 *	first, an empty list.
 *	Else, a list representing the sublist.
 *
 *	In any case the resulting sublist is never cyclic.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_Sublist(
    Col_Word list,
    size_t first, 
    size_t last)
{
    Col_Word sublist;		/* Resulting list node in the general case. */
    size_t listLength;		/* Length of source list. */
    size_t loop;		/* Loop length of source list. */
    unsigned char depth=0;	/* Depth of source list. */
    size_t length;		/* Length of resulting sublist. */

    list = GetRoot(list);

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

    if (listLength == loop) {
	/*
	 * List is circular.
	 */

	size_t offset;

	ASSERT(WORD_TYPE(list) == WORD_TYPE_CIRCLIST);

	/*
	 * Sublist of loop repetition.
	 */

	offset = first % loop;
	last -= first - offset;
	first = offset;
	return Col_Sublist(Col_RepeatList(WORD_CIRCLIST_CORE(list), 
		last/loop + 1), first, last);
   } else if (loop) {
	/*
	 * List is cyclic.
	 */

	ASSERT(WORD_TYPE(list) == WORD_TYPE_CONCATLIST || WORD_TYPE(list) == WORD_TYPE_MCONCATLIST);
	ASSERT(WORD_TYPE(WORD_CONCATLIST_RIGHT(list)) == WORD_TYPE_CIRCLIST);

	if (last < listLength-loop) {
	    /*
	     * Sublist of head.
	     */

	    return Col_Sublist(WORD_CONCATLIST_LEFT(list), first, last);
	} else if (first >= listLength-loop) {
	    /*
	     * Sublist of tail.
	     */

	    first -= listLength-loop;
	    last -= listLength-loop;
	    return Col_Sublist(WORD_CONCATLIST_RIGHT(list), first, last);
	} else {
	    /*
	     * Concat end of head + beginning of tail.
	     */

	    return Col_ConcatLists(Col_Sublist(WORD_CONCATLIST_LEFT(list),
		    first, listLength-loop-1), Col_Sublist(
		    WORD_CONCATLIST_RIGHT(list), 0, last - (listLength-loop)));
	}
    }

    if (first >= listLength) {
	/*
	 * Beginning is past the end of the list.
	 */

	return WORD_LIST_EMPTY;
    }

    if (last >= listLength) {
	/* 
	 * Truncate overlong sublists. (Note: at this point listLength > 1)
	 */

	last = listLength-1;
    }

    length = last-first+1;

    if (first == 0 && length == listLength) {
	/* 
	 * Identity. 
	 */

	FreezeMList(list);
	return list;
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
	Col_TraverseListChunks(list, first, length, 0, MergeChunksProc, &info, 
		NULL);
	ASSERT(info.length == length);

	return info.vector;
    }

    /* 
     * General case: build a sublist node.
     */

    FreezeMList(list);
    sublist = (Col_Word) AllocCells(1);
    WORD_SUBLIST_INIT(sublist, depth, list, first, last);

    return sublist;
}

/*---------------------------------------------------------------------------
 * Function: Col_ConcatLists
 *
 *	Concatenate lists.
 *
 *	Concatenation forms self-balanced binary trees. Each node has a depth 
 *	level. Concat nodes have a depth > 1. Sublist nodes have a depth equal
 *	to that of their source node. Other nodes have a depth of 0. 
 *
 * Arguments:
 *	left	- Left part.
 *	right	- Right part.
 *
 * Result:
 *	If the resulting list would exceed the maximum length, nil. Else, a 
 *	list representing the concatenation of both lists.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_ConcatLists(
    Col_Word left,
    Col_Word right)
{
    Col_Word concatNode;	/* Resulting list node in the general case. */
    unsigned char leftDepth, rightDepth; 
				/* Respective depths of left and right lists. */
    size_t leftLength, rightLength;
				/* Respective lengths. */
    size_t rightLoop;		/* Loop length of right. */

    left = GetRoot(left);
    right = GetRoot(right);

    /*
     * Note: getting lengths will perfom type checking as well.
     */

    leftLength = Col_ListLength(left);
    if (leftLength == 0) {
	/* 
	 * Concat is a no-op on right. 
	 */

	FreezeMList(right);
	return right;
    }
    
    rightLength = Col_ListLength(right);
    if (rightLength == 0) {
	/* 
	 * Concat is a no-op on left.
	 */

	FreezeMList(left);
	return left;
    }

    if (Col_ListLoopLength(left)) {
	/*
	 * Left is cyclic, discard right.
	 */

	FreezeMList(left);
	return left;
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

    rightLoop = Col_ListLoopLength(right);
    if (rightLoop) {
	/*
	 * Right is cyclic.
	 */

	Col_Word head, tail;

	if (rightLoop == rightLength) {
	    /*
	     * Right is circular, use as tail.
	     */

	    ASSERT(WORD_TYPE(right) == WORD_TYPE_CIRCLIST);

	    head = left;
	    tail = right;
	} else {
	    /*
	     * Concat left and right head to form head.
	     */

	    ASSERT(WORD_TYPE(right) == WORD_TYPE_CONCATLIST || WORD_TYPE(right) == WORD_TYPE_MCONCATLIST);
	    ASSERT(WORD_TYPE(WORD_CONCATLIST_RIGHT(right)) == WORD_TYPE_CIRCLIST);
	    head = Col_ConcatLists(left, WORD_CONCATLIST_LEFT(right));
	    tail = WORD_CONCATLIST_RIGHT(right);
	}

	/*
	 * Concat head and tail without rebalancing.
	 */

	leftLength = Col_ListLength(head);
	leftDepth = GetDepth(head);
	rightLength = Col_ListLength(tail);
	rightDepth = GetDepth(tail);
	FreezeMList(head);
	FreezeMList(tail);
	concatNode = (Col_Word) AllocCells(1);
	WORD_CONCATLIST_INIT(concatNode, 
		(leftDepth>rightDepth?leftDepth:rightDepth) + 1, leftLength 
		+ rightLength, leftLength, head, Col_CircularList(tail));
	return concatNode;
    }

    /* 
     * Handle quick cases. 
     */
    
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

		return Col_Sublist(WORD_SUBLIST_SOURCE(left), 
			WORD_SUBLIST_FIRST(left), WORD_SUBLIST_LAST(right));
	    }
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
	Col_TraverseListChunks(left, 0, leftLength, 0, MergeChunksProc, &info, 
		NULL);
	Col_TraverseListChunks(right, 0, rightLength, 0, MergeChunksProc, &info,
		NULL);
	ASSERT(info.length == leftLength+rightLength);

	return info.vector;
    }

    /* 
     * Assume the input lists are well balanced by construction. Build a list 
     * that is balanced as well, i.e. where left and right depths don't differ 
     * by more that 1 level.
     *
     * Note that a sublist with depth >= 1 always points to a concat node, as 
     * by construction it cannot point to another sublist (see Col_Sublist). 
     * Likewise, the arms cannot be list root words.
     */

    leftDepth = GetDepth(left);
    rightDepth = GetDepth(right);
    if (leftDepth > rightDepth+1) {
	/* 
	 * Left is deeper by more than 1 level, rebalance.
	 */

	Col_Word left1, left2;

	ASSERT(leftDepth >= 2);

	GetArms(left, &left1, &left2);
	if (GetDepth(left2) > GetDepth(left1)) {
	    /* 
	     * Left2 is deeper, split it between both arms. 
	     */

	    Col_Word left21, left22;
	    GetArms(left2, &left21, &left22);
	    return Col_ConcatLists(Col_ConcatLists(left1, left21), 
		    Col_ConcatLists(left22, right));
	} else {
	    /* 
	     * Left1 is deeper or at the same level, rotate to right.
	     */

	    return Col_ConcatLists(left1, Col_ConcatLists(left2, right));
	}
    } else if (leftDepth+1 < rightDepth) {
	/* 
	 * Right is deeper by more than 1 level, rebalance. 
	 */

	Col_Word right1, right2;

	ASSERT(rightDepth >= 2);

	GetArms(right, &right1, &right2);
	if (GetDepth(right1) > GetDepth(right2)) {
	    /* 
	     * Right1 is deeper, split it between both arms. 
	     */

	    Col_Word right11, right12;
	    GetArms(right1, &right11, &right12);
	    return Col_ConcatLists(Col_ConcatLists(left, right11),
		    Col_ConcatLists(right12, right2));
	} else {
	    /* 
	     * Right2 is deeper or at the same level, rotate to left.
	     */

	    return Col_ConcatLists(Col_ConcatLists(left, right1), right2);
	}
    }

    /* 
     * General case: build a concat node.
     */

    FreezeMList(left);
    FreezeMList(right);
    concatNode = (Col_Word) AllocCells(1);
    WORD_CONCATLIST_INIT(concatNode, (leftDepth>rightDepth?leftDepth:rightDepth)
	    + 1, leftLength + rightLength, leftLength, left, right);

    return concatNode;
}

/*---------------------------------------------------------------------------
 * Function: Col_ConcatListsA
 *
 *	Concatenate several lists given in an array. Concatenation is done 
 *	recursively, by halving the array until it contains one or two elements,
 *	at this point we respectively return the element or use 
 *	<Col_ConcatLists>.
 *
 * Arguments:
 *	number	- Number of lists in below array.
 *	lists	- Lists to concatenate in order.
 *
 * Result:
 *	If the array is empty or the resulting list would exceed the maximum 
 *	length, nil. Else, a list representing the concatenation of all lists.
 *
 * Side effects:
 *	See <Col_ConcatLists>.
 *---------------------------------------------------------------------------*/

Col_Word
Col_ConcatListsA(
    size_t number,
    const Col_Word * lists)
{
    size_t half;
    Col_Word l1, l2;

    /* 
     * Quick cases.
     */

    if (number == 0) {return WORD_NIL;}
    if (number == 1) {return Col_CopyMList(lists[0]);}
    if (number == 2) {return Col_ConcatLists(lists[0], lists[1]);}

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


/*---------------------------------------------------------------------------
 * Function: Col_ConcatListsNV
 *
 *	Concatenate lists given as arguments. The argument list is first 
 *	copied into a stack-allocated array then passed to <Col_ConcatListsA>.
 *
 * Arguments:
 *	number	- Number of arguments.
 *	...	- Remaining arguments, i.e. lists to concatenate in order.
 *
 * Result:
 *	If the number of arguments is zero or the resulting list would exceed 
 *	the maximum length, nil. Else, a list representing the concatenation of
 *	all lists.
 *
 * Side effects:
 *	See <Col_ConcatListsA>.
 *---------------------------------------------------------------------------*/

Col_Word
Col_ConcatListsNV(
    size_t number,
    ...)
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

/*---------------------------------------------------------------------------
 * Function: Col_RepeatList
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
 * Arguments:
 *	list	- The list to repeat.
 *	count	- Repetition factor.
 *
 * Result:
 *	If count is zero or the resulting list would exceed the maximum length,
 *	nil. Else, a list representing the repetition of the source list.
 *---------------------------------------------------------------------------*/

Col_Word
Col_RepeatList(
    Col_Word list,
    size_t count)
{
    if (Col_ListLength(list) == 0) {
	/*
	 * No-op.
	 */

	return WORD_LIST_EMPTY;
    }

    if (Col_ListLoopLength(list)) {
	/*
	 * List is cyclic, won't repeat.
	 */

	return list;
    }

    /* 
     * Quick cases. 
     */

    if (count == 0) {return WORD_NIL;}
    if (count == 1) {return Col_CopyMList(list);}
    if (count == 2) {return Col_ConcatLists(list, list);}

    if (count & 1) {
	/* Odd.*/
	return Col_ConcatLists(list, Col_RepeatList(list, count-1));
    } else {
	/* Even. */
	return Col_RepeatList(Col_ConcatLists(list, list), count>>1);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_CircularList
 *
 *	Create a circular list from a regular list.
 *
 * Argument:
 *	core	- The core list.
 *
 * Result:
 *	If the list is empty or cyclic, the original list. Else a new circular 
 *	list from the given core.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_CircularList(
    Col_Word core)
{
    if (Col_ListLength(core) == 0) {
	/*
	 * No-op.
	 */

	return WORD_LIST_EMPTY;
    }

    core = Col_CopyMList(core);
    if (Col_ListLoopLength(core)) {
	/*
	 * List is cyclic, no-op.
	 */

	return core;
    }

    return WORD_CIRCLIST_NEW(core);
}

/*---------------------------------------------------------------------------
 * Function: Col_ListInsert
 *
 *	Insert a list into another one, just before the given insertion point, 
 *	taking cyclicity into account. As target list is immutable, this results
 *	in a new list.
 *
 *	Insertion past the end of the list results in a concatenation.
 *
 * Arguments:
 *	into	- Where to insert.
 *	index	- Index of insertion point.
 *	list	- List to insert.
 *
 * Note:
 *	Only perform minimal tests to prevent overflow, basic ops should 
 *	perform further optimizations anyway.
 *
 * Result:
 *	The resulting list.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_ListInsert(
    Col_Word into,
    size_t index,
    Col_Word list)
{
    size_t length, loop;

    into = Col_CopyMList(into);
    if (Col_ListLength(list) == 0) {
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
    if (length == loop) {
	/*
	 * Destination is circular.
	 */

	ASSERT(WORD_TYPE(into) == WORD_TYPE_CIRCLIST);
	index %= loop;
	if (index == 0) index = loop;
	return Col_CircularList(Col_ListInsert(WORD_CIRCLIST_CORE(into), index, 
		list));
    } else if (loop) {
	/*
	 * Destination is cyclic.
	 */

	ASSERT(WORD_TYPE(into) == WORD_TYPE_CONCATLIST);
	ASSERT(WORD_TYPE(WORD_CONCATLIST_RIGHT(into)) == WORD_TYPE_CIRCLIST);
	if (index <= length-loop) {
	    /*
	     * Insert within head.
	     */

	    return Col_ConcatLists(Col_ListInsert(WORD_CONCATLIST_LEFT(into),
		    index, list), WORD_CONCATLIST_RIGHT(into));
	} else {
	    /*
	     * Insert within tail.
	     */

	    return Col_ConcatLists(WORD_CONCATLIST_LEFT(into), Col_ListInsert(
		    WORD_CONCATLIST_RIGHT(into), index - (length-loop), list));
	}
    }

    /*
     * Destination is acyclic.
     */

    if (index >= length) {
	/*
	 * Insertion past the end of list is concatenation.
	 */

	return Col_ConcatLists(into, list);
    }

    /* 
     * General case. 
     */

    return Col_ConcatLists(Col_ConcatLists(
		    Col_Sublist(into, 0, index-1), list), 
	    Col_Sublist(into, index, length-1));
}

/*---------------------------------------------------------------------------
 * Function: Col_ListRemove
 *
 *	Remove a range of elements from a list, taking cyclicity into account. 
 *	As target list is immutable, this results in a new list.
 *
 * Arguments:
 *	list		- List to remove sequence from.
 *	first, last	- Range of elements to remove.
 *
 * Note:
 *	Only perform minimal tests to prevent overflow, basic ops should 
 *	perform further optimizations anyway.
 *
 * Result:
 *	The resulting list.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

Col_Word 
Col_ListRemove(
    Col_Word list,
    size_t first, 
    size_t last)
{
    size_t length, loop;

    if (first > last) {
	/* 
	 * No-op. 
	 */

	return Col_CopyMList(list);
    }

    list = GetRoot(list);
    length = Col_ListLength(list);
    loop = Col_ListLoopLength(list);
    if (length == loop) {
	/*
	 * List is circular.
	 */

	ASSERT(WORD_TYPE(list) == WORD_TYPE_CIRCLIST);
	if (first == 0) {
	    /*
	     * Remove beginning.
	     */

	    last %= loop;
	    return Col_ConcatLists(Col_Sublist(list, last+1, length-1), list);
	} else {
	    /*
	     * Remove middle of loop.
	     */

	    first %= loop;
	    if (first == 0) first = loop;
	    last %= loop;

	    if (last < first) {
		/*
		 * Keep inner part of loop and use beginning as head.
		 */

		return Col_ConcatLists(Col_Sublist(list, 0, last), 
			Col_CircularList(Col_Sublist(list, last+1, first-1)));
	    } else {
		/*
		 * Remove inner part of loop.
		 */

		return Col_CircularList(Col_ListRemove(WORD_CIRCLIST_CORE(list),
			first, last));
	    }
	}
    } else if (loop) {
	/*
	 * List is cyclic.
	 */

	ASSERT(WORD_TYPE(list) == WORD_TYPE_CONCATLIST || WORD_TYPE(list) == WORD_TYPE_MCONCATLIST);
	ASSERT(WORD_TYPE(WORD_CONCATLIST_RIGHT(list)) == WORD_TYPE_CIRCLIST);

	if (last < length-loop) {
	    /*
	     * Remove within head.
	     */

	    return Col_ConcatLists(Col_ListRemove(WORD_CONCATLIST_LEFT(list),
		    first, last), WORD_CONCATLIST_RIGHT(list));
	} else if (first >= length-loop) {
	    /*
	     * Remove within tail.
	     */

	    first -= length-loop;
	    last -= length-loop;
	    return Col_ConcatLists(WORD_CONCATLIST_LEFT(list), Col_ListRemove(
		    WORD_CONCATLIST_RIGHT(list), first, last));
	} else {
	    /*
	     * Remove end of head + beginning of tail.
	     */

	    return Col_ConcatLists(Col_ListRemove(WORD_CONCATLIST_LEFT(list),
		    first, length-loop-1), Col_ListRemove(
		    WORD_CONCATLIST_RIGHT(list), 0, last - (length-loop)));
	}
    }

    /*
     * Acyclic list.
     */

    if (length == 0 || first >= length) {
	/*
	 * No-op.
	 */

	return Col_CopyMList(list);
    } else if (first == 0) {
	/*
	 * Trim begin.
	 */

	return Col_Sublist(list, last+1, length-1);
    } else if (last >= length-1) {
	/* 
	 * Trim end. 
	 */

	return Col_Sublist(list, 0, first-1);
    }

    /* 
     * General case. 
     */

    return Col_ConcatLists(Col_Sublist(list, 0, first-1), 
	    Col_Sublist(list, last+1, length-1));
}

/*---------------------------------------------------------------------------
 * Function: Col_ListReplace
 *
 *	Replace a range of elements in a list with another, taking cyclicity
 *	into account. As target list is immutable, this results in a new list.
 *
 *	Replacement is a combination of remove and insert.
 *
 * Arguments:
 *	list		- Original list.
 *	first, last	- Inclusive range of elements to replace.
 *	with		- Replacement list.
 *
 * Note:
 *	Only perform minimal tests to prevent overflow, basic ops should 
 *	perform further optimizations anyway.
 *
 * Result:
 *	The resulting list.
 *
 * Side effects:
 *	May allocate memory cells.
 *
 * See also:
 *	<Col_ListInsert>, <Col_ListRemove>
 *---------------------------------------------------------------------------*/

Col_Word 
Col_ListReplace(
    Col_Word list,
    size_t first, 
    size_t last,
    Col_Word with)
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

    if (loop && first > length) {
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




/****************************************************************************
 * Group: Immutable List Traversal
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Typedef: ListChunkTraverseInfo
 *
 *	Structure used during recursive list chunk traversal. This avoids
 *	recursive procedure calls thanks to a pre-allocated backtracking 
 *	structure: since the algorithms only recurse on concat nodes and since
 *	we know their depth, we can allocate the needed space once for all.
 *
 * Fields:
 *	backtracks	- Pre-allocated backtracking structure.
 *	list		- Currently traversed list.
 *	start		- Index of first element traversed in list.
 *	max		- Max number of elements traversed in list.
 *	maxDepth	- Depth of toplevel concat node.
 *	prevDepth	- Depth of next concat node for backtracking.
 *
 * Backtrack stack element fields:
 *	backtracks.prevDepth	- Depth of next concat node for backtracking.
 *	backtracks.list		- List.
 *	backtracks.max		- Max number of elements traversed in list.
 *
 * See also:
 *	<Col_TraverseListChunksN>, <Col_TraverseListChunks>
 *---------------------------------------------------------------------------*/

typedef struct ListChunkTraverseInfo {
    struct {
	int prevDepth;
	Col_Word list;
	size_t max;
    } *backtracks;
    Col_Word list;
    size_t start, max;
    int maxDepth, prevDepth;
} ListChunkTraverseInfo;

/*---------------------------------------------------------------------------
 * Internal Function: GetChunk
 *
 *	Get chunk from given traversal info.
 *
 * Arguments:
 *	info		- Traversal info.
 *	elements	- Chunk info for leaf.
 *	reverse		- Whether to traverse in reverse order.
 *
 * See also:
 *	<ListChunkTraverseInfo>, <Col_TraverseListChunksN>, 
 *	<Col_TraverseListChunks>
 *---------------------------------------------------------------------------*/

static void
GetChunk(
    ListChunkTraverseInfo *info,
    const Col_Word **elements,
    int reverse)
{
    int type;

    for (;;) {
	/*
	 * Descend into structure until we find a suitable leaf.
	 */

	type = WORD_TYPE(info->list);
	switch (type) {
	    case WORD_TYPE_CIRCLIST:
		/* 
		 * Recurse on core.
		 */

		info->list = WORD_CIRCLIST_CORE(info->list);
		continue;

	    case WORD_TYPE_WRAP:
		/* 
		 * Recurse on source.
		 */

		info->list = WORD_WRAP_SOURCE(info->list);
		continue;

	    case WORD_TYPE_SUBLIST:
		/* 
		 * Sublist: recurse on source list.
		 */

		info->start += WORD_SUBLIST_FIRST(info->list);
		info->list = WORD_SUBLIST_SOURCE(info->list);
		continue;
        	    
	    case WORD_TYPE_CONCATLIST:
	    case WORD_TYPE_MCONCATLIST: {
		/* 
		 * Concat: descend into covered arms.
		 */

		int depth;
		size_t leftLength = WORD_CONCATLIST_LEFT_LENGTH(info->list);
		if (leftLength == 0) {
		    leftLength = Col_ListLength(
			    WORD_CONCATLIST_LEFT(info->list));
		}
		if (info->start + (reverse ? 0 : info->max-1) < leftLength) {
		    /* 
		     * Recurse on left arm only. 
		     */

		    info->list = WORD_CONCATLIST_LEFT(info->list);
		    continue;
		}
		if (info->start - (reverse ? info->max-1 : 0) >= leftLength) {
		    /* 
		     * Recurse on right arm only. 
		     */

		    info->start -= leftLength;
		    info->list = WORD_CONCATLIST_RIGHT(info->list);
		    continue;
		} 

		/*
		 * Push right (resp. left for reverse) onto stack and recurse on
		 * left (resp. right).
		 */

		ASSERT(info->backtracks);
		depth = WORD_CONCATLIST_DEPTH(info->list);
		ASSERT(depth <= info->maxDepth);
		info->backtracks[depth-1].prevDepth = info->prevDepth;
		info->prevDepth = depth;
		if (reverse) {
		    ASSERT(info->start >= leftLength);
		    info->backtracks[depth-1].list 
			    = WORD_CONCATLIST_LEFT(info->list);
		    info->backtracks[depth-1].max = info->max
			    - (info->start-leftLength+1);
		    info->start -= leftLength;
		    info->max = info->start+1;
		    info->list = WORD_CONCATLIST_RIGHT(info->list);
		} else {
		    ASSERT(info->start < leftLength);
		    info->backtracks[depth-1].list 
			    = WORD_CONCATLIST_RIGHT(info->list);
		    info->backtracks[depth-1].max = info->max
			    - (leftLength-info->start);
		    info->max = leftLength-info->start;
		    info->list = WORD_CONCATLIST_LEFT(info->list);
		}
		continue;
	    }

	    case WORD_TYPE_MLIST:
		/* 
		 * Recurse on list root.
		 */

		info->list = WORD_MLIST_ROOT(info->list);
		continue;

	    /* WORD_TYPE_UNKNOWN */
	}
	break;
    }

    /*
     * Get leaf data.
     */

    ASSERT(info->start + (reverse ? info->max-1 : 0) >= 0);
    ASSERT(info->start + (reverse ? 0 : info->max-1) < Col_ListLength(info->list));
    switch (type) {
	case WORD_TYPE_VECTOR:
	case WORD_TYPE_MVECTOR:
	    *elements = WORD_VECTOR_ELEMENTS(info->list) + info->start
		    - (reverse ? info->max-1 : 0);
	    break;

	case WORD_TYPE_VOIDLIST:
	    *elements = COL_LISTCHUNK_VOID;
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: NextChunk
 *
 *	Get next chunk in traversal order.
 *
 * Arguments:
 *	info	- Traversal info.
 *	nb	- Number of elements to skip, must be < info->max.
 *	reverse	- Whether to traverse in reverse order.
 *
 * See also:
 *	<ListChunkTraverseInfo>, <Col_TraverseListChunksN>, 
 *	<Col_TraverseListChunks>
 *---------------------------------------------------------------------------*/

static void 
NextChunk(
    ListChunkTraverseInfo *info,
    size_t nb,
    int reverse)
{
    ASSERT(info->max >= nb);
    info->max -= nb;
    if (info->max > 0) {
	/*
	 * Still in leaf, advance.
	 */

	if (reverse) {
	    info->start -= nb;
	} else {
	    info->start += nb;
	}
    } else if (info->prevDepth == INT_MAX) {
	/*
	 * Already at toplevel => end of list.
	 */

	info->list = WORD_NIL;
    } else {
	/*
	 * Reached leaf bound, backtracks.
	 */

	ASSERT(info->backtracks);
	info->list = info->backtracks[info->prevDepth-1].list;
	info->max = info->backtracks[info->prevDepth-1].max;
	info->start = (reverse ? Col_ListLength(info->list)-1 : 0);
	info->prevDepth = info->backtracks[info->prevDepth-1].prevDepth;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_TraverseListChunksN
 *
 *	Iterate over the chunks of a number of lists.
 *
 *	For each traversed chunk, proc is called back with the opaque data as
 *	well as the position within the lists. If it returns a non-zero result 
 *	then the iteration ends. 
 *
 * Note:
 *	The algorithm is naturally recursive but this implementation avoids
 *	recursive calls thanks to a stack-allocated backtracking structure.
 *
 * Arguments:
 *	number		- Number of lists to traverse.
 *	lists		- Lists to traverse.
 *	start		- Index of first element.
 *	max		- Max number of elements.
 *	proc		- Callback proc called on each chunk.
 *	clientData	- Opaque data passed as is to above proc.
 *
 * Results:
 *	The return value of the last called proc, or -1 if no traversal was
 *	performed. Additionally:
 *
 *	lengthPtr	- If non-NULL, incremented by the total number of 
 *			  elements traversed upon completion.
 *---------------------------------------------------------------------------*/

int 
Col_TraverseListChunksN(
    size_t number,
    Col_Word *lists,
    size_t start,
    size_t max,
    Col_ListChunksTraverseProc *proc,
    Col_ClientData clientData,
    size_t *lengthPtr)
{
    size_t i;
    ListChunkTraverseInfo *info;
    const Col_Word **elements;
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
	info[i].list = (info[i].max ? lists[i] : WORD_NIL);
	info[i].start = start;
	info[i].maxDepth = GetDepth(lists[i]);
	info[i].backtracks = (info[i].maxDepth ? 
	    alloca(sizeof(*info[i].backtracks) * info[i].maxDepth) : NULL);
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

	    GetChunk(info+i, elements+i, 0);
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
	 * Call proc on leaves data.
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

	    int next = 0;
	    for (i=0; i < number; i++) {
		if (info[i].list) NextChunk(info+i, max, 0);
		if (info[i].list) next = 1;
	    }
	    if (!next) {
		/*
		 * Reached end of all lists, stop there.
		 */

		return 0;
	    }
	}
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_TraverseListChunks
 *
 *	Iterate over the chunks of a list.
 *
 *	For each traversed chunk, proc is called back with the opaque data as
 *	well as the position within the list. If it returns a non-zero result 
 *	then the iteration ends. 
 *
 * Note:
 *	The algorithm is naturally recursive but this implementation avoids
 *	recursive calls thanks to a stack-allocated backtracking structure. 
 *	This procedure is an optimized version of <Col_TraverseListChunksN>.
 *
 * Arguments:
 *	list		- List to traverse.
 *	start		- Index of first element.
 *	max		- Max number of elements.
 *	reverse		- Whether to traverse in reverse order.
 *	proc		- Callback proc called on each chunk.
 *	clientData	- Opaque data passed as is to above proc.
 *
 * Results:
 *	The return value of the last called proc, or -1 if no traversal was
 *	performed. Additionally:
 *
 *	lengthPtr	- If non-NULL, incremented by the total number of 
 *			  elements traversed upon completion.
 *---------------------------------------------------------------------------*/

int 
Col_TraverseListChunks(
    Col_Word list,
    size_t start,
    size_t max,
    int reverse,
    Col_ListChunksTraverseProc *proc,
    Col_ClientData clientData,
    size_t *lengthPtr)
{
    ListChunkTraverseInfo info;
    const Col_Word *elements;
    int result;
    size_t listLength = Col_ListLength(list);

    if (listLength == 0) {
	/*
	 * Nothing to traverse.
	 */

	return -1;
    }

    if (reverse) {
	if (start > listLength) {
	    /* 
	     * Start is past the end of the list.
	     */

	    start = listLength-1;
	}
	if (max > start+1) {
	    /* 
	     * Adjust max to the remaining length. 
	     */

	    max = start+1;
	}
    } else {
	if (start > listLength) {
	    /* 
	     * Start is past the end of the list.
	     */

	    return -1;
	}
	if (max > listLength-start) {
	    /* 
	     * Adjust max to the remaining length. 
	     */

	    max = listLength-start;
	}
    }

    if (max == 0) {
	/*
	 * Nothing to traverse.
	 */

	return -1;
    }

    info.list = list;
    info.start = start;
    info.max = max;
    info.maxDepth = GetDepth(list);
    info.backtracks = (info.maxDepth ? 
	    alloca(sizeof(*info.backtracks) * info.maxDepth) : NULL);
    info.prevDepth = INT_MAX;

    for (;;) {
	GetChunk(&info, &elements, reverse);
	max = info.max;

	/*
	 * Call proc on leaf data.
	 */

	if (lengthPtr) *lengthPtr += max;
	if (reverse) {
	    result = proc(start-max+1, max, 1, &elements, clientData);
	    start -= max;
	} else {
	    result = proc(start, max, 1, &elements, clientData);
	    start += max;
	}
	if (result != 0) {
	    /*
	     * Stop there.
	     */

	    return result;
	} else {
	    /*
	     * Continue iteration.
	     */

	    NextChunk(&info, max, reverse);
	    if (!info.list) {
		/*
		 * Reached end of list, stop there.
		 */

		return 0;
	    }
	}
    }
}


/****************************************************************************
 * Group: Immutable List Iteration
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Constant: MAX_ITERATOR_SUBNODE_DEPTH
 *
 *	Max depth of subnodes in iterators.
 *---------------------------------------------------------------------------*/

#define MAX_ITERATOR_SUBNODE_DEPTH	3

/*---------------------------------------------------------------------------
 * Internal Function: IterAtVector
 *
 *	Element access proc for vector leaves from iterators. Follows
 *	<ColListIterLeafAtProc> signature.
 *
 * Arguments:
 *	leaf	- Leaf node.
 *	index	- Leaf-relative index of element.
 *
 * Result:
 *	Element at given index.
 *
 * See also:
 *	<ColListIterLeafAtProc>, <ColListIterator>, 
 *	<ColListIterUpdateTraversalInfo>
 *---------------------------------------------------------------------------*/

static Col_Word 
IterAtVector(
    Col_Word leaf, 
    size_t index)
{
    return WORD_VECTOR_ELEMENTS(leaf)[index];
}

/*---------------------------------------------------------------------------
 * Internal Function: IterAtVoid
 *
 *	Element access proc for void list leaves from iterators. Follows
 *	<ColListIterLeafAtProc> signature.
 *
 * Arguments:
 *	leaf	- Leaf node.
 *	index	- Leaf-relative index of element.
 *
 * Result:
 *	Always nil.
 *
 * See also:
 *	<ColListIterLeafAtProc>, <ColListIterator>, 
 *	<ColListIterUpdateTraversalInfo>
 *---------------------------------------------------------------------------*/

static Col_Word 
IterAtVoid(
    Col_Word leaf, 
    size_t index)
{
    return WORD_NIL;
}

/*---------------------------------------------------------------------------
 * Internal Function: ColListIterUpdateTraversalInfo
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
 * Argument:
 *	it	- The iterator to update.
 *---------------------------------------------------------------------------*/

void
ColListIterUpdateTraversalInfo(
    Col_ListIterator *it)
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
	    case WORD_TYPE_CIRCLIST:
		/*
		 * Recurse into core.
		 */

		node = WORD_CIRCLIST_CORE(node);
		break;

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

	    case WORD_TYPE_MLIST:
		/*
		 * Recurse into root.
		 */

		node = WORD_MLIST_ROOT(node);
		break;

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

/*---------------------------------------------------------------------------
 * Function: Col_ListIterBegin
 *
 *	Initialize the list iterator so that it points to the index-th
 *	element within the list. If index points past the end of the list, the
 *	iterator is initialized to the end iterator.
 *
 * Arguments:
 *	list	- List to iterate over.
 *	index	- Index of element.
 *	it	- Iterator to initialize.
 *
 * Result:
 *	Whether the iterator looped or not.
 *---------------------------------------------------------------------------*/

int
Col_ListIterBegin(
    Col_Word list,
    size_t index,
    Col_ListIterator *it)
{
    int looped=0;

    it->list = list;
    it->length = Col_ListLength(list);
    if (index >= it->length) {
	size_t loop = Col_ListLoopLength(list);
	if (!loop) {
	    /*
	     * End of list.
	     */

	    index = it->length;
	} else {
	    /*
	     * Cyclic list. Normalize index.
	     */

	    looped = 1;
	    index = (index - (it->length-loop)) % loop + (it->length-loop);
	}
    }
    it->index = index;

    /*
     * Traversal info will be lazily computed.
     */

    it->traversal.subnode = WORD_NIL;
    it->traversal.leaf = WORD_NIL;

    return looped;
}

/*---------------------------------------------------------------------------
 * Function: Col_ListIterFirst
 *
 *	Initialize the list iterator so that it points to the first
 *	character within the list. If list is empty, the iterator is initialized
 *	to the end iterator.
 *
 * Arguments:
 *	list	- List to iterate over.
 *	it	- Iterator to initialize.
 *---------------------------------------------------------------------------*/

void
Col_ListIterFirst(
    Col_Word list,
    Col_ListIterator *it)
{
    it->list = list;
    it->length = Col_ListLength(list);
    it->index = 0;

    /*
     * Traversal info will be lazily computed.
     */

    it->traversal.subnode = WORD_NIL;
    it->traversal.leaf = WORD_NIL;
}

/*---------------------------------------------------------------------------
 * Function: Col_ListIterLast
 *
 *	Initialize the list iterator so that it points to the last
 *	character within the list. If list is empty, the iterator is initialized
 *	to the end iterator.
 *
 * Arguments:
 *	list	- List to iterate over.
 *	it	- Iterator to initialize.
 *---------------------------------------------------------------------------*/

void
Col_ListIterLast(
    Col_Word list,
    Col_ListIterator *it)
{
    it->list = list;
    it->length = Col_ListLength(list);
    if (it->length == 0) {
	/*
	 * End of list.
	 */

	it->index = 0;
    } else {
	it->index = it->length-1;
    }

    /*
     * Traversal info will be lazily computed.
     */

    it->traversal.subnode = WORD_NIL;
    it->traversal.leaf = WORD_NIL;
}

/*---------------------------------------------------------------------------
 * Function: Col_ListIterCompare
 *
 *	Compare two iterators by their respective positions.
 *
 * Arguments:
 *	it1	- First iterator.
 *	it2	- Second iterator.
 *
 * Result:
 *	-  < 0 if it1 is before it2
 *	-  > 0 if it1 is after it2
 *	-  0 if they are equal
 *---------------------------------------------------------------------------*/

int
Col_ListIterCompare(
    const Col_ListIterator *it1,
    const Col_ListIterator *it2)
{
    /*
     * Note: iterators at end have their index set to SIZE_MAX.
     */

    if (it1->index < it2->index) {
	return -1;
    } else if (it1->index > it2->index) {
	return 1;
    } else {
	return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_ListIterMoveTo
 *
 *	Move the iterator to the given absolute position.
 *
 * Arguments:
 *	it	- The iterator to move.
 *	index	- Position.
 *
 * Result:
 *	Whether the iterator looped or not.
 *---------------------------------------------------------------------------*/

int
Col_ListIterMoveTo(
    Col_ListIterator *it,
    size_t index)
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

/*---------------------------------------------------------------------------
 * Function: Col_ListIterForward
 *
 *	Move the iterator to the nb-th next element.
 *
 * Arguments:
 *	it	- The iterator to move.
 *	nb	- Offset.
 *
 * Result:
 *	Whether the iterator looped or not.
 *---------------------------------------------------------------------------*/

int
Col_ListIterForward(
    Col_ListIterator *it,
    size_t nb)
{
    int looped=0;

    if (nb == 0) {
	/*
	 * No-op.
	 */

	return looped;
    }

    if (Col_ListIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid list iterator");
	return looped;
    }

    if (nb >= it->length - it->index) {
	size_t loop = Col_ListLoopLength(it->list);
	if (!loop) {
	    /*
	     * End of list.
	     */

	    it->index = it->length;
	    return looped;
	}

	/*
	 * Cyclic list.
	 */

	looped = 1;
	if (it->index < it->length-loop) {
	    /*
	     * Currently before loop, forward into loop.
	     */

	    size_t nb1 = (it->length-loop) - it->index;
	    ASSERT(nb >= nb1);
	    nb = nb1 + (nb-nb1) % loop;
	    it->index += nb;
	} else {
	    /*
	     * Currently within loop.
	     */

	    nb %= loop;
	    if (it->index >= it->length-nb) {
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

/*---------------------------------------------------------------------------
 * Function: Col_ListIterBackward
 *
 *	Move the iterator to the nb-th previous element.
 *
 * Arguments:
 *	it	- The iterator to move.
 *	nb	- Offset.
 *---------------------------------------------------------------------------*/

void
Col_ListIterBackward(
    Col_ListIterator *it,
    size_t nb)
{
    if (nb == 0) {
	/*
	 * No-op.
	 */

	return;
    }

    if (Col_ListIterEnd(it)) {
	if (nb <= it->length) {
	    /*
	     * Allow iterators at end to go back.
	     */

	    it->index = it->length-nb;
	    it->traversal.leaf = WORD_NIL;
	    return;
	}
	Col_Error(COL_ERROR, "Invalid list iterator");
	return;
    }

    if (it->index < nb) {
	/*
	 * Beginning of list.
	 */
    
	it->index = it->length;
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
================================================================================
Section: Mutable Lists
================================================================================
*/

/****************************************************************************
 * Group: Mutable List Creation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_NewMList
 *
 *	Create a new mutable list word.
 *
 * Result:
 *	The new word.
 *
 * Side effects:
 *	Allocates memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewMList()
{
    Col_Word mlist;

    mlist = (Col_Word) AllocCells(1);
    WORD_MLIST_INIT(mlist, WORD_LIST_EMPTY);

    return mlist;
}

/*---------------------------------------------------------------------------
 * Function: Col_CopyMList
 *
 *	Create an immutable list from a mutable list. If an immutable list is 
 *	given, it is simply returned. If a mutable list is given, its content
 *	is frozen and shared with the new one. Mutable vectors are copied.
 *
 * Argument:
 *	mlist	- The mutable list to copy.
 *
 * Side effects:
 *	The content of a mutable list is frozen.
 *
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_CopyMList(
    Col_Word mlist)
{
    switch (WORD_TYPE(mlist)) {
	case WORD_TYPE_WRAP:
	    /*
	     * Recurse on source.
	     */

	    return Col_CopyMList(WORD_WRAP_SOURCE(mlist));

	case WORD_TYPE_CIRCLIST:
	    /*
	     * Recurse on core.
	     */

	    return WORD_CIRCLIST_NEW(Col_CopyMList(WORD_CIRCLIST_CORE(mlist)));

	case WORD_TYPE_VOIDLIST:
	case WORD_TYPE_VECTOR:
	case WORD_TYPE_SUBLIST:
	case WORD_TYPE_CONCATLIST:
	    /*
	     * No-op.
	     */

	    return mlist;

	case WORD_TYPE_MVECTOR:
	    /*
	     * Copy to immutable vector.
	     */

	    return Col_NewVector(WORD_VECTOR_LENGTH(mlist), 
		    WORD_VECTOR_ELEMENTS(mlist));

	case WORD_TYPE_MCONCATLIST:
	    /*
	     * Copy to immutable concat.
	     */

	    return Col_ConcatLists(WORD_CONCATLIST_LEFT(mlist),
		    WORD_CONCATLIST_RIGHT(mlist));

	case WORD_TYPE_MLIST:
	    /*
	     * Freeze and return content.
	     */

	    FreezeMList(WORD_MLIST_ROOT(mlist));
	    return WORD_MLIST_ROOT(mlist);

	default:
	    Col_Error(COL_ERROR, "%x is not a mutable list", mlist);
	    return WORD_NIL;
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: FreezeMList
 *
 *	Freeze mutable list recursively. This changes mutable words to immutable
 *	versions inplace.
 *
 * Argument:
 *	mlist	- Mutable list to freeze.
 *
 * See also:
 *	<Col_CopyMList>
 *---------------------------------------------------------------------------*/

static void
FreezeMList(
    Col_Word mlist)
{
    /*
     * Entry point for tail recursive calls.
     */

start:
    switch (WORD_TYPE(mlist)) {
	case WORD_TYPE_CIRCLIST:
	    /*
	     * Recursive on core.
	     */

	    mlist = WORD_CIRCLIST_CORE(mlist);
	    goto start;

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
	     * Change type ID and recurse on child nodes (tail recurse on right 
	     * node).
	     */

	    int pinned = WORD_PINNED(mlist);
	    WORD_SET_TYPEID(mlist, WORD_TYPE_CONCATLIST);
	    if (pinned) {
		WORD_SET_PINNED(mlist);
	    }
	    FreezeMList(WORD_CONCATLIST_LEFT(mlist));
	    mlist = WORD_CONCATLIST_RIGHT(mlist);
	    goto start;
	}

	default:
	    /*
	     * Already immutable, stop there.
	     */

	    return;
    }
}


/****************************************************************************
 * Group: Mutable List Operations
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_MListSetLength
 *
 *	Resize the mutable list. Newly added elements are set to nil.
 *
 * Arguments:
 *	mlist	- Mutable list to resize.
 *	length	- New length.
 *---------------------------------------------------------------------------*/

void
Col_MListSetLength(
    Col_Word mlist,
    size_t length)
{
    size_t listLength, listLoop;

    if (WORD_TYPE(mlist) != WORD_TYPE_MLIST) {
	Col_Error(COL_ERROR, "%x is not a mutable list", mlist);
	return;
    }

    if (length == 0) {
	/*
	 * Empty list.
	 */

	WORD_MLIST_ROOT(mlist) = WORD_LIST_EMPTY;
	return;
    }

    listLength = Col_ListLength(mlist);
    listLoop = Col_ListLoopLength(mlist);
    if (listLoop == listLength) {
	/*
	 * List is circular.
	 */

	Col_Word root = WORD_MLIST_ROOT(mlist);
	ASSERT(WORD_TYPE(root) == WORD_TYPE_CIRCLIST);
	if (length <= listLength) {
	    /*
	     * Shorten loop.
	     */

	    WORD_MLIST_ROOT(mlist) = WORD_CIRCLIST_CORE(root);
	    Col_MListSetLength(mlist, length);
	} else {
	    /*
	     * Sublist of circular list.
	     */

	    WORD_MLIST_ROOT(mlist) = Col_Sublist(root, 0, length-1);
	}
	return;
    } else if (listLoop) {
	/*
	 * List is cyclic.
	 */

	Col_Word root = WORD_MLIST_ROOT(mlist);
	ASSERT(WORD_TYPE(root) == WORD_TYPE_CONCATLIST || WORD_TYPE(root) == WORD_TYPE_MCONCATLIST);
	ASSERT(WORD_TYPE(WORD_CONCATLIST_RIGHT(root)) == WORD_TYPE_CIRCLIST);

	if (length <= listLength-listLoop) {
	    /*
	     * Keep head.
	     */

	    ASSERT(listLoop < listLength);
	    WORD_MLIST_ROOT(mlist) = WORD_CONCATLIST_LEFT(root);
	    ASSERT(Col_ListLength(mlist) >= length);
	    Col_MListSetLength(mlist, length);
	} else {
	    /*
	     * Shorten tail. Temporarily use tail as root before recursive call.
	     */

	    if (WORD_TYPE(root) != WORD_TYPE_MCONCATLIST) {
		ConvertToMConcatNode(&root);
	    }
	    ASSERT(WORD_TYPE(root) == WORD_TYPE_MCONCATLIST);
	    WORD_MLIST_ROOT(mlist) = WORD_CONCATLIST_RIGHT(root);
	    Col_MListSetLength(mlist, length - (listLength-listLoop));
	    WORD_CONCATLIST_RIGHT(root) = WORD_MLIST_ROOT(mlist);
	    UpdateMConcatNode(root);
	    WORD_MLIST_ROOT(mlist) = root;
	}
	return;
    }

    /*
     * Acyclic list.
     */

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

/*---------------------------------------------------------------------------
 * Function: Col_MListLoop
 *
 *	Make a mutable list circular. Do nothing if cyclic (not necessarily 
 *	circular).
 *
 * Argument:
 *	mlist	- The mutable list to make circular.
 *---------------------------------------------------------------------------*/

void
Col_MListLoop(
    Col_Word mlist)

{
    if (WORD_TYPE(mlist) != WORD_TYPE_MLIST) {
	Col_Error(COL_ERROR, "%x is not a mutable list", mlist);
	return;
    }

    if (Col_ListLength(mlist) == 0) {
	/*
	 * No-op.
	 */

	return;
    }

    if (Col_ListLoopLength(mlist)) {
	/*
	 * List is cyclic, no-op.
	 */

	return;
    }

    /*
     * Make root cyclic.
     */

    WORD_MLIST_ROOT(mlist) = WORD_CIRCLIST_NEW(WORD_MLIST_ROOT(mlist));
}

/*---------------------------------------------------------------------------
 * Function: Col_MListSetAt
 *
 *	Set the element of a mutable list at a given position. 
 *
 * Arguments:
 *	mlist	- Mutable list to modify.
 *	index	- Element index.
 *	element	- New element.
 *---------------------------------------------------------------------------*/

void
Col_MListSetAt(
    Col_Word mlist,
    size_t index,
    Col_Word element)
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
    MListSetAt(&WORD_MLIST_ROOT(mlist), index, element);
}

/*---------------------------------------------------------------------------
 * Internal Function: MListSetAt
 *
 *	Set the element of a mutable list at a given position, converting nodes
 *	to mutable forms in the process.
 *
 * Arguments:
 *	nodePtr	- Mutable list node to modify. May be overwritten if it gets
 *		  converted to mutable.
 *	index	- Element index.
 *	element	- New element.
 *---------------------------------------------------------------------------*/

static void
MListSetAt(
    Col_Word * nodePtr, 
    size_t index,
    Col_Word element)
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

		MListSetAt(&WORD_CONCATLIST_LEFT(*nodePtr), index, element);
	    } else {
		/*
		 * Recurse on right.
		 */

		MListSetAt(&WORD_CONCATLIST_RIGHT(*nodePtr), index - leftLength,
			element);
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
	    return;

	/*
	 * ... then immediate and immutable types.
	 */

	case WORD_TYPE_CONCATLIST:
	    /*
	     * Convert to mutable concat then retry.
	     */

	    ConvertToMConcatNode(nodePtr);
	    goto start;

	case WORD_TYPE_SUBLIST:
	    if (WORD_SUBLIST_DEPTH(*nodePtr) >= 1) {
		/*
		 * Source is concat. Convert to mutable then retry.
		 */

		ConvertToMConcatNode(nodePtr);
		goto start;
	    }
	    /* continued. */
	default:
	    /*
	     * Convert to mutable around index then retry.
	     */

	    ConvertToMutableAt(nodePtr, index);
	    goto start;
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: NewMConcatList
 *
 *	Create a new mutable concat list word.
 *
 * Arguments:
 *	left	- Left part.
 *	right	- Right part.
 *
 * Result:
 *	The new word.
 *
 * Side effects:
 *	Allocates memory cells.
 *---------------------------------------------------------------------------*/

static Col_Word
NewMConcatList(
    Col_Word left,
    Col_Word right)
{
    size_t leftDepth = GetDepth(left), rightDepth = GetDepth(right);
    size_t leftLength = Col_ListLength(left), 
	    rightLength = Col_ListLength(right);
    Col_Word mconcatNode = (Col_Word) AllocCells(1);
    WORD_MCONCATLIST_INIT(mconcatNode, 
	    (leftDepth>rightDepth?leftDepth:rightDepth) + 1, 
	    leftLength + rightLength, leftLength, left, right);
    return mconcatNode;
}

/*---------------------------------------------------------------------------
 * Internal Function: ConvertToMutableAt
 *
 *	Convert an immutable node (vector, void list or sublist) to mutable
 *	around a given index. The original is not modified, a mutable copy is 
 *	made.
 *
 * Arguments:
 *	nodePtr	- Mutable list node to convert. Gets overwritten upon return.
 *	index	- Element index.
 *---------------------------------------------------------------------------*/

static void
ConvertToMutableAt(
    Col_Word *nodePtr,
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

	converted = NewMConcatList(mvector, Col_Sublist(*nodePtr, 
		MAX_SHORT_MVECTOR_LENGTH, length-1));
    } else if (index >= WORD_VOIDLIST_LENGTH(*nodePtr)
	    - MAX_SHORT_MVECTOR_LENGTH) {
	/*
	 * Convert tail.
	 */

	size_t leftLength = length - MAX_SHORT_MVECTOR_LENGTH;
	Col_Word mvector = Col_NewMVector(0, MAX_SHORT_MVECTOR_LENGTH, 
		(elements ? elements + leftLength : NULL));

	ASSERT(leftLength >= 1);

	converted = NewMConcatList(Col_Sublist(*nodePtr, 0, leftLength-1), 
		mvector);
    } else {
	/*
	 * Convert central part starting at index.
	 */

	Col_Word mvector = Col_NewMVector(0, MAX_SHORT_MVECTOR_LENGTH,
		(elements ? elements + index : NULL));
	Col_Word left = NewMConcatList(Col_Sublist(*nodePtr, 0, index-1), 
		mvector);

	converted = NewMConcatList(left, Col_Sublist(*nodePtr, index 
		+ MAX_SHORT_MVECTOR_LENGTH, length-1));
    }

    *nodePtr = converted;
}

/*---------------------------------------------------------------------------
 * Internal Function: ConvertToMConcatNode
 *
 *	Convert an immutable node (concat or sublist) to a mutable concat list.
 *	The original is not modified, a mutable copy is made.
 *
 * Argument:
 *	nodePtr	- Mutable list node to convert. Gets overwritten upon return.
 *---------------------------------------------------------------------------*/

static void
ConvertToMConcatNode(
    Col_Word *nodePtr)
{
    Col_Word converted;

    switch (WORD_TYPE(*nodePtr)) {
	case WORD_TYPE_SUBLIST: {
	    /*
	     * Split sublist into mutable concat node.
	     */

	    Col_Word left, right;

	    ASSERT(WORD_SUBLIST_DEPTH(*nodePtr) >= 1);
	    ASSERT(WORD_TYPE(WORD_SUBLIST_SOURCE(*nodePtr)) == WORD_TYPE_CONCATLIST);

	    GetArms(*nodePtr, &left, &right);

	    converted = NewMConcatList(left, right);
	    break;
	}

	case WORD_TYPE_CONCATLIST: {
	    /*
	     * Convert to mutable concat node.
	     */

	    ASSERT(WORD_CONCATLIST_DEPTH(*nodePtr) >= 1);

	    converted = NewMConcatList(WORD_CONCATLIST_LEFT(*nodePtr),
		    WORD_CONCATLIST_RIGHT(*nodePtr));
	    break;
	}

	default: 
	    /* CANTHAPPEN */
	    ASSERT(0);
    }

    *nodePtr = converted;
}

/*---------------------------------------------------------------------------
 * Internal Function: UpdateMConcatNode
 *
 *	Update (depth, left length...) & rebalance mutable concat list node.
 *
 * Argument:
 *	node	- Mutable concat list.
 *
 * Rebalancing:
 *	Rebalancing works as for immutable lists (see <Col_ConcatLists>)
 *	except that it is done in place whereas for the latter new concat nodes
 *	are created in the process. Consequently no extraneous word is created.
 *---------------------------------------------------------------------------*/

//FIXME: keep pinned flag for subtrees?
static void
UpdateMConcatNode(
    Col_Word node)
{
    Col_Word left, right;
    unsigned char leftDepth, rightDepth;
    size_t leftLength;
    int pinned;

    ASSERT(WORD_TYPE(node) == WORD_TYPE_MCONCATLIST);

    left = WORD_CONCATLIST_LEFT(node);
    leftDepth = GetDepth(left);
    right = WORD_CONCATLIST_RIGHT(node);
    rightDepth = GetDepth(right);

    if (WORD_TYPE(right) != WORD_TYPE_CIRCLIST) {
	/*
	 * Balance tree.
	 */

	for (;;) {
	    if (leftDepth > rightDepth+1) {
		/* 
		 * Left is deeper by more than 1 level, rebalance.
		 */

		unsigned char left1Depth, left2Depth;
		Col_Word left1, left2;

		ASSERT(leftDepth >= 2);
		if (WORD_TYPE(left) != WORD_TYPE_MCONCATLIST) {
		    ConvertToMConcatNode(&left);
		}
		ASSERT(WORD_TYPE(left) == WORD_TYPE_MCONCATLIST);

		left1 = WORD_CONCATLIST_LEFT(left);
		left1Depth = GetDepth(left1);
		left2 = WORD_CONCATLIST_RIGHT(left);
		left2Depth = GetDepth(left2);
		if (left1Depth < left2Depth) {
		    /* 
		     * Left2 is deeper, split it between both arms. 
		     */

		    Col_Word left21, left22;
		    unsigned char left21Depth, left22Depth;

		    if (WORD_TYPE(left2) != WORD_TYPE_MCONCATLIST) {
			ConvertToMConcatNode(&left2);
		    }
		    ASSERT(WORD_TYPE(left2) == WORD_TYPE_MCONCATLIST);
		    ASSERT(WORD_CONCATLIST_DEPTH(left2) >= 1);

		    left21 = WORD_CONCATLIST_LEFT(left2);
		    left21Depth = GetDepth(left21);
		    left22 = WORD_CONCATLIST_RIGHT(left2);
		    left22Depth = GetDepth(left22);

		    /*
		     * Update left node.
		     */

		    leftLength = Col_ListLength(left1);
		    leftDepth = (left1Depth>left21Depth?left1Depth:left21Depth) 
			    + 1;
		    WORD_MCONCATLIST_INIT(left, leftDepth, 
			    leftLength+Col_ListLength(left21), leftLength,
			    left1, left21);

		    /*
		     * Update right node.
		     */

		    leftLength = Col_ListLength(left22);
		    rightDepth = (left22Depth>rightDepth?left22Depth:rightDepth) 
			    + 1;
		    WORD_MCONCATLIST_INIT(left2, rightDepth, 
			leftLength+Col_ListLength(right), leftLength, left22,
			right);

		    /*
		     * Update node.
		     */

		    leftLength = WORD_CONCATLIST_LENGTH(left);
		    WORD_MCONCATLIST_INIT(node, 
			    (leftDepth>rightDepth?leftDepth:rightDepth) + 1, 
			    leftLength+WORD_CONCATLIST_LENGTH(left2), 
			    leftLength, left, left2);
		} else {
		    /* 
		     * Left1 is deeper or at the same level, rotate to right.
		     */

		    unsigned char left2Depth;

		    ASSERT(left1Depth >= 1);
		    left2Depth = GetDepth(left2);
		    rightDepth = (left2Depth>rightDepth?left2Depth:rightDepth)
			    + 1;

		    /*
		     * Update right node.
		     */

		    leftLength = Col_ListLength(left2);
		    WORD_MCONCATLIST_INIT(left, rightDepth, 
			    leftLength+Col_ListLength(right), leftLength, left2,
			    right);

		    /*
		     * Update node.
		     */

		    leftLength = Col_ListLength(left1);
		    WORD_MCONCATLIST_INIT(node, 
			    (left1Depth>rightDepth?left1Depth:rightDepth) + 1, 
			    leftLength+WORD_CONCATLIST_LENGTH(left), leftLength,
			    left1, left);
		}
	    } else if (leftDepth+1 < rightDepth) {
		/* 
		 * Right is deeper by more than 1 level, rebalance. 
		 */

		unsigned char right2Depth;
		Col_Word right1, right2;

		ASSERT(rightDepth >= 2);
		if (WORD_TYPE(right) != WORD_TYPE_MCONCATLIST) {
		    ConvertToMConcatNode(&right);
		}
		ASSERT(WORD_TYPE(right) == WORD_TYPE_MCONCATLIST);

		right1 = WORD_CONCATLIST_LEFT(right);
		right2 = WORD_CONCATLIST_RIGHT(right);
		right2Depth = GetDepth(right2);
		if (right2Depth < rightDepth-1) {
		    /* 
		     * Right1 is deeper, split it between both arms. 
		     */

		    Col_Word right11, right12;
		    unsigned char right11Depth, right12Depth;

		    if (WORD_TYPE(right1) != WORD_TYPE_MCONCATLIST) {
			ConvertToMConcatNode(&right1);
		    }
		    ASSERT(WORD_TYPE(right1) == WORD_TYPE_MCONCATLIST);
		    ASSERT(WORD_CONCATLIST_DEPTH(right1) >= 1);

		    right11 = WORD_CONCATLIST_LEFT(right1);
		    right11Depth = GetDepth(right11);
		    right12 = WORD_CONCATLIST_RIGHT(right1);
		    right12Depth = GetDepth(right12);

		    /*
		     * Update left node.
		     */

		    leftLength = Col_ListLength(left);
		    leftDepth = (leftDepth>right11Depth?leftDepth:right11Depth) 
			    + 1;
		    WORD_MCONCATLIST_INIT(right1, leftDepth, 
			    leftLength+Col_ListLength(right11), leftLength,
			    left, right11);

		    /*
		     * Update right node.
		     */

		    leftLength = Col_ListLength(right12);
		    rightDepth = (right12Depth>right2Depth?right12Depth:right2Depth) 
			    + 1;
		    WORD_MCONCATLIST_INIT(right, rightDepth, 
			    leftLength+Col_ListLength(right2), leftLength, 
			    right12, right2);

		    /*
		     * Update node.
		     */

		    leftLength = WORD_CONCATLIST_LENGTH(right1);
		    WORD_MCONCATLIST_INIT(node, 
			    (leftDepth>rightDepth?leftDepth:rightDepth) + 1, 
			    leftLength+WORD_CONCATLIST_LENGTH(right), 
			    leftLength, right1, right);
		} else {
		    /* 
		     * Right2 is deeper or at the same level, rotate to left.
		     */

		    unsigned char right1Depth;

		    ASSERT(right2Depth >= 1);
		    right1Depth = GetDepth(right1);
		    leftDepth = (leftDepth>right1Depth?leftDepth:right1Depth) 
			    + 1;

		    /*
		     * Update left node.
		     */

		    leftLength = Col_ListLength(left);
		    WORD_MCONCATLIST_INIT(right, rightDepth,
			    leftLength+Col_ListLength(right1), leftLength, left, 
			    right1);

		    /*
		     * Update node.
		     */

		    leftLength = WORD_CONCATLIST_LENGTH(right);
		    WORD_MCONCATLIST_INIT(node, 
			    (leftDepth>right2Depth?leftDepth:right2Depth) + 1, 
			    leftLength+Col_ListLength(right2), leftLength, right, 
			    right2);
		}
	    } else {
		/*
		 * Tree is balanced, stop there.
		 */

		break;
	    }
	}
    }
 
    /*
     * Update node.
     */

    pinned = WORD_PINNED(node);
    leftLength = Col_ListLength(left);
    WORD_MCONCATLIST_INIT(node, 
	    (leftDepth>rightDepth?leftDepth:rightDepth) + 1,
	    leftLength+Col_ListLength(right), leftLength, left, right);
    if (pinned) {
	WORD_SET_PINNED(node);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_MListInsert
 *
 *	Insert a list into a mutable list, just before the given insertion 
 *	point, taking cyclicity into account.
 *
 * Arguments:
 *	into	- Mutable list to insert into.
 *	index	- Index of insertion point.
 *	list	- List to insert.
 *---------------------------------------------------------------------------*/

void
Col_MListInsert(
    Col_Word into,
    size_t index,
    Col_Word list)
{
    size_t length, loop, listLength;

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

    if (length == 0) {
	/*
	 * Target is empty, replace content with input.
	 */

	WORD_MLIST_ROOT(into) = Col_CopyMList(list);
	return;
    }

    loop = Col_ListLoopLength(into);

    if (Col_ListLoopLength(list)) {
	/*
	 * Inserted list is cyclic. Shorten destination (making it acyclic)
	 *  before insertion.
	 */

	Col_Word node;

	if (loop) {
	    /*
	     * Destination is cyclic, adjust index.
	     */

	    if (index > length-loop) {
		index = (index - (length-loop)) % loop + (length-loop);
		if (index == length-loop) index = length;
	    }
	} else {
	    /*
	     * Destination is acyclic.
	     */

	    if (index > length) index = length;
	}
	Col_MListSetLength(into, index);

	list = Col_CopyMList(list);
	if (WORD_TYPE(list) == WORD_TYPE_CONCATLIST) {
	    /*
	     * Append head to destination first.
	     */

	    Col_MListInsert(into, index, WORD_CONCATLIST_LEFT(list));
	    list = WORD_CONCATLIST_RIGHT(list);
	}

	/*
	 * Concat root and loop.
	 */

	ASSERT(WORD_TYPE(list) == WORD_TYPE_CIRCLIST);
	node = NewMConcatList(WORD_MLIST_ROOT(into), list);
	WORD_MLIST_ROOT(into) = node;
	return;
    }

    /*
     * Inserted list is acyclic.
     */

    if (length == loop) {
	/*
	 * Destination is circular.
	 */

	Col_Word root = WORD_MLIST_ROOT(into);
	ASSERT(WORD_TYPE(root) == WORD_TYPE_CIRCLIST);

	if (index == 0) {
	    /*
	     * Insert before loop: concat list and loop.
	     */

	    Col_Word node = NewMConcatList(Col_CopyMList(list), root);
	    WORD_MLIST_ROOT(into) = node;
	} else {
	    /*
	     * Insert into loop.
	     */

	    index %= loop;
	    if (index == 0) index = loop;
	    root = WORD_CIRCLIST_CORE(root);
	    MListInsert(&root, index, list);
	    WORD_MLIST_ROOT(into) = WORD_CIRCLIST_NEW(root);
	}
	return;
    } else if (loop) {
	/*
	 * Destination is cyclic.
	 */

	Col_Word root = WORD_MLIST_ROOT(into);
	if (WORD_TYPE(root) != WORD_TYPE_MCONCATLIST) {
	    ConvertToMConcatNode(&root);
	    WORD_MLIST_ROOT(into) = root;
	}
	ASSERT(WORD_TYPE(root) == WORD_TYPE_MCONCATLIST);
	ASSERT(WORD_TYPE(WORD_CONCATLIST_RIGHT(root)) == WORD_TYPE_CIRCLIST);

	if (index <= length-loop) {
	    /*
	     * Insert within head.
	     */

	    MListInsert(&WORD_CONCATLIST_LEFT(root), index, list);
	} else {
	    /*
	     * Insert within tail. Temporarily use tail as root before recursive
	     * call.
	     */

	    WORD_MLIST_ROOT(into) = WORD_CONCATLIST_RIGHT(root);
	    Col_MListInsert(into, index - (length-loop), list);
	    WORD_CONCATLIST_RIGHT(root) = WORD_MLIST_ROOT(into);
	    ASSERT(WORD_TYPE(WORD_CONCATLIST_RIGHT(root)) == WORD_TYPE_CIRCLIST);
	    WORD_MLIST_ROOT(into) = root;
	}
	UpdateMConcatNode(root);
	return;
    }

    /*
     * Destination is acyclic.
     */

    if (index >= length) {
	/*
	 * Insertion past the end of list is concatenation.
	 */

	index = length;
    }

    /* 
     * General case. 
     */

    MListInsert(&WORD_MLIST_ROOT(into), index, list);
}

/*---------------------------------------------------------------------------
 * Internal Function: MListInsert
 *
 *	Insert a list into a mutable list node, just before the given insertion 
 *	point. Recursively descend into containing node.
 *
 * Arguments:
 *	nodePtr	- Mutable list node to insert into. May be overwritten in the
 *		  process (e.g. if it gets converted to mutable).
 *	index	- Index of insertion point.
 *	list	- List to insert.
 *
 * See also:
 *	<Col_MListInsert>
 *---------------------------------------------------------------------------*/

static void
MListInsert(
    Col_Word * nodePtr, 
    size_t index,
    Col_Word list)
{
    size_t length = Col_ListLength(*nodePtr);
    size_t listLength = Col_ListLength(list);
    int type;

    ASSERT(index <= length);
    ASSERT(SIZE_MAX-length >= listLength);
    ASSERT(listLength > 0);

    if (length == 0) {
	/*
	 * Replace content.
	 */

	*nodePtr = Col_CopyMList(list);;
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

		if (GetDepth(left) < WORD_CONCATLIST_DEPTH(*nodePtr)-1) {
		    /*
		     * Right is deeper, insert into left.
		     */

		    MListInsert(&WORD_CONCATLIST_LEFT(*nodePtr), index, 
			    list);
		    UpdateMConcatNode(*nodePtr);
		    return;
		}
	    }

	    if (index <= leftLength) {
		/*
		 * Insert into left.
		 */

		MListInsert(&WORD_CONCATLIST_LEFT(*nodePtr), index, list);
	    } else {
		/*
		 * Insert into right.
		 */

		MListInsert(&WORD_CONCATLIST_RIGHT(*nodePtr), 
			index-leftLength, list);
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
		Col_TraverseListChunks(list, 0, listLength, 0, MergeChunksProc, 
			&info, NULL);
		ASSERT(info.length == index+listLength);
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
	
	Col_Word node = Col_NewMVector(0, length+listLength, NULL);

	info.length = 0;
	info.vector = node;
	Col_TraverseListChunks(*nodePtr, 0, index, 0, MergeChunksProc, &info, 
		NULL);
	Col_TraverseListChunks(list, 0, listLength, 0, MergeChunksProc, &info, 
		NULL);
	Col_TraverseListChunks(*nodePtr, index, length-index, 0, 
		MergeChunksProc, &info, NULL);
	ASSERT(info.length == length+listLength);

	*nodePtr = node;
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

	    ConvertToMConcatNode(nodePtr);
	    goto start;

	case WORD_TYPE_SUBLIST:
	    if (WORD_SUBLIST_DEPTH(*nodePtr) >= 1) {
		/*
		 * Source is concat. Convert to mutable concat then retry.
		 */

		ConvertToMConcatNode(nodePtr);
		goto start;
	    }
	    /* continued. */
	default: 
	    if (index > 0 && index < length) {
		/*
		 * Split at insertion point then retry.
		 */

		SplitMutableAt(nodePtr, index);
		goto start;
	    }

	    /*
	     * Build a mutable concat node with current node and list to insert.
	     */

	    if (index == 0) {
		/*
		 * Insert before.
		 */

		*nodePtr = NewMConcatList(list, *nodePtr);
	    } else {
		/*
		 * Insert after.
		 */

		ASSERT(index >= length);
		*nodePtr = NewMConcatList(*nodePtr, list);
	    }
	    return;
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: SplitMutableAt
 *
 *	Split a mutable list node at a given index.
 *
 * Arguments:
 *	nodePtr	- Mutable list node to split. Gets overwritten upon return.
 *	index	- Index of split point.
 *---------------------------------------------------------------------------*/

static void
SplitMutableAt(
    Col_Word *nodePtr, 
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

    if (!elements) {
	ASSERT(type == WORD_TYPE_VOIDLIST);
	converted = NewMConcatList(WORD_VOIDLIST_NEW(index), 
		WORD_VOIDLIST_NEW(length-index));
    } else {
	Col_Word right = Col_NewMVector(0, length-index, elements+index);

	if (type == WORD_TYPE_MVECTOR) {
	    /*
	     * Shrink existing vector and use as left arm.
	     */

	    Col_MVectorSetLength(*nodePtr, index);
	    converted = NewMConcatList(*nodePtr, right);
	} else {
	    /*
	     * Build new vector for left arm.
	     */

	    converted = NewMConcatList(Col_NewMVector(0, index, elements), 
		    right);
	}
    }

    *nodePtr = converted;
}

/*---------------------------------------------------------------------------
 * Function: Col_MListRemove
 *
 *	Remove a range of elements from a mutable list.
 *
 * Arguments:
 *	mlist		- Mutable list to modify.
 *	first, last	- Range of elements to remove.
 *---------------------------------------------------------------------------*/

void
Col_MListRemove(
    Col_Word mlist,
    size_t first, 
    size_t last)
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
    if (length == loop) {
	/*
	 * List is circular.
	 */

	Col_Word root = WORD_MLIST_ROOT(mlist);
	ASSERT(WORD_TYPE(root) == WORD_TYPE_CIRCLIST);

	if (first == 0) {
	    /*
	     * Remove beginning.
	     */

	    last %= loop;
	    Col_MListInsert(mlist, 0, Col_Sublist(root, last+1, length-1));
	} else {
	    /*
	     * Remove middle of loop.
	     */

	    first %= loop;
	    if (first == 0) first = loop;
	    last %= loop;

	    if (last < first) {
		/*
		 * Keep inner part of loop and use beginning as head.
		 */

		Col_Word head, node;

		head = Col_Sublist(mlist, 0, last);
		root = WORD_CIRCLIST_CORE(root);
		if (first < length) {
		    MListRemove(&root, first, length-1);
		}
		MListRemove(&root, 0, last);

		node = NewMConcatList(head, WORD_CIRCLIST_NEW(root));
		WORD_MLIST_ROOT(mlist) = node;
	    } else {
		/*
		 * Remove inner part of loop.
		 */

		root = WORD_CIRCLIST_CORE(root);
		MListRemove(&root, first, last);
		WORD_MLIST_ROOT(mlist) = WORD_CIRCLIST_NEW(root);
	    }
	}
	return;
    } else if (loop) {
	/*
	 * List is cyclic.
	 */

	Col_Word root = WORD_MLIST_ROOT(mlist);
	if (WORD_TYPE(root) != WORD_TYPE_MCONCATLIST) {
	    ConvertToMConcatNode(&root);
	    WORD_MLIST_ROOT(mlist) = root;
	}
	ASSERT(WORD_TYPE(root) == WORD_TYPE_MCONCATLIST);
	ASSERT(WORD_TYPE(WORD_CONCATLIST_RIGHT(root)) == WORD_TYPE_CIRCLIST);

	if (first == 0 && last >= length-loop-1) {
	    /*
	     * Remove whole head and recurse on tail.
	     */

	    WORD_MLIST_ROOT(mlist) = WORD_CONCATLIST_RIGHT(root);
	    if (last >= length-loop) {
		Col_MListRemove(mlist, 0, last - (length-loop));
	    }
	    return;
	}

	if (last < length-loop) {
	    /*
	     * Remove within head.
	     */

	    MListRemove(&WORD_CONCATLIST_LEFT(root), first, last);
	} else {
	    /*
	     * Remove part of tail.
	     */

	    Col_Word tail;

	    if (first >= length-loop) {
		first -= length-loop;
		last -= length-loop;
	    } else {
		/*
		 * Remove end of head first.
		 */

		MListRemove(&WORD_CONCATLIST_LEFT(root), first, length-loop-1);
		first = 0;
		last -= length-loop;
	    }

	    /*
	     *  Temporarily use tail as root before recursive call.
	     */

	    WORD_MLIST_ROOT(mlist) = WORD_CONCATLIST_RIGHT(root);
	    Col_MListRemove(mlist, first, last);
	    tail = WORD_MLIST_ROOT(mlist);
	    if (WORD_TYPE(tail) != WORD_TYPE_CIRCLIST) {
		/*
		 * Circular list was made cyclic in the process. Move head to 
		 * left.
		 */

		if (WORD_TYPE(tail) != WORD_TYPE_MCONCATLIST) {
		    ConvertToMConcatNode(&tail);
		}
		ASSERT(WORD_TYPE(tail) == WORD_TYPE_MCONCATLIST);
		WORD_CONCATLIST_RIGHT(root) = WORD_CONCATLIST_RIGHT(tail);
		WORD_CONCATLIST_RIGHT(tail) = WORD_CONCATLIST_LEFT(tail);
		WORD_CONCATLIST_LEFT(tail) = WORD_CONCATLIST_LEFT(root);
		WORD_CONCATLIST_LEFT(root) = tail;
		UpdateMConcatNode(tail);
	    } else {
		WORD_CONCATLIST_RIGHT(root) = tail;
	    }

	    ASSERT(WORD_TYPE(root) == WORD_TYPE_MCONCATLIST);
	    ASSERT(WORD_TYPE(WORD_CONCATLIST_RIGHT(root)) == WORD_TYPE_CIRCLIST);

	    /* 
	     * Restore root.
	     */

	    WORD_MLIST_ROOT(mlist) = root;
	}
	UpdateMConcatNode(root);
	return;
    }

    /*
     * Acyclic list.
     */

    if (length == 0 || first >= length) {
	/*
	 * No-op.
	 */

	return;
    }
    
    if (last >= length) {
	last = length-1;
    }

    /* 
     * General case. 
     */

    MListRemove(&WORD_MLIST_ROOT(mlist), first, last);
}

/*---------------------------------------------------------------------------
 * Internal Function: MListRemove
 *
 *	Remove a range of elements from a mutable list. Recursively descend into
 *	containing nodes.
 *
 * Arguments:
 *	nodePtr		- Mutable list node to modify. May be overwritten in the
 *			  process (e.g. if it gets converted to mutable).
 *	first, last	- Range of elements to remove.
 *
 * See also:
 *	<Col_MListRemove>
 *---------------------------------------------------------------------------*/

static void
MListRemove(
    Col_Word * nodePtr, 
    size_t first, 
    size_t last)
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
			(first<leftLength?0:first-leftLength), last-leftLength);
	    }
	    if (first < leftLength) {
		/*
		 * Remove part on left arm.
		 */

		MListRemove(&WORD_CONCATLIST_LEFT(*nodePtr), first, 
			(last<leftLength?last:leftLength-1));
		leftLength = Col_ListLength(WORD_CONCATLIST_LEFT(*nodePtr));
	    }

	    if (leftLength == 0) {
		/*
		 * Whole left arm removed, replace node by right.
		 */

		*nodePtr = WORD_CONCATLIST_RIGHT(*nodePtr);
	    } else if (Col_ListLength(WORD_CONCATLIST_RIGHT(*nodePtr)) 
		    == 0) {
		/*
		 * Whole right arm removed, replace node by left.
		 */

		*nodePtr = WORD_CONCATLIST_LEFT(*nodePtr);
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
	Col_TraverseListChunks(*nodePtr, 0, first, 0, MergeChunksProc, &info, 
		NULL);
	Col_TraverseListChunks(*nodePtr, last+1, length-last, 0, 
		MergeChunksProc, &info, NULL);
	ASSERT(info.length == length-(last-first+1));

	*nodePtr = node;
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

	    ConvertToMConcatNode(nodePtr);
	    goto start;

	case WORD_TYPE_SUBLIST: 
	    if (WORD_SUBLIST_DEPTH(*nodePtr) >= 1) {
		/*
		 * Source is concat. Convert to mutable concat then retry.
		 */

		ConvertToMConcatNode(nodePtr);
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

		node = NewMConcatList(Col_Sublist(*nodePtr, 0, first-1), 
			Col_Sublist(*nodePtr, last+1, length-1));
	    }
	    *nodePtr = node;
	    return;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_MListReplace
 *
 *	Replace a range of elements in a mutable list with another.
 *
 *	Replacement is a combination of remove and insert.
 *
 * Arguments:
 *	mlist		- Mutable list to modify.
 *	first, last	- Inclusive range of elements to replace.
 *	with		- Replacement list.
 *
 * See also:
 *	<Col_MListInsert>, <Col_MListRemove>
 *---------------------------------------------------------------------------*/

void
Col_MListReplace(
    Col_Word mlist,
    size_t first, size_t last,
    Col_Word with)
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

    if (loop && first > length) {
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
