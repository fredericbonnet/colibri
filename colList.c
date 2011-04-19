/*
 * Vector and list data structures implemented as words.
 *
 * Vectors are flat arrays of words. Their size is limited to the maximum
 * word data size, because each word must fit within one single page.
 *
 * To handle larger data, lists can assemble vectors in a binary tree structure,
 * in the same fashion as ropes with string chunks. The present code is actually
 * an adaptation of the rope implementation.
 */

#include "colibri.h"
#include "colInt.h"

#include <string.h>
#include <limits.h>
#include <malloc.h> /* For alloca */

/* 
 * Max byte size of short list (inc. header) created with sublist or concat. 
 */

#define MAX_SHORT_LIST_SIZE	(3*CELL_SIZE)

/*
 * Max depth of subnodes in iterators.
 */

#define MAX_ITERATOR_SUBNODE_DEPTH 3

/*
 * Prototypes for functions used only in this file.
 */

static Col_ListChunksEnumProc MergeChunksProc;
static Col_Word		NewList(Col_Word root);
static void		GetArms(Col_Word node, Col_Word * leftPtr, 
			    Col_Word * rightPtr);
static void		UpdateTraversalInfo(Col_ListIterator *it);


/*
 *---------------------------------------------------------------------------
 *
 * Col_GetMaxVectorLength --
 *
 *	Get the maximum length of a vector word.
 *
 * Results:
 *	The max vector length.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

size_t
Col_GetMaxVectorLength()
{
    return VECTOR_MAX_LENGTH;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewVectorWord --
 * Col_NewVectorWordV --
 *
 *	Create a new list word.
 *	Col_NewVectorWordV is a variadic version of Col_NewVectorWord.
 *
 *	Truncates the provided data if it is larger than the maximum length.
 *	Calling code must thus check the length of the created word, and use
 *	use additional structures to assemble vectors into larger lists.
 *
 * Results:
 *	The new word, or NULL when length is zero.
 *
 * Side effects:
 *	May allocate memory cells.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_NewVectorWord(
    size_t length,		/* Length of below array. */
    const Col_Word * elements)	/* Array of words to populate vector with. */
{
    Col_Word vector;		/* Resulting word in the general case. */

    if (length == 0) {
	return NULL;
    }

    /*
     * Get maximum length. Only copy the elements into vector up to this length.
     */

    if (length > VECTOR_MAX_LENGTH) {
	length = VECTOR_MAX_LENGTH;
    }

    /*
     * Create a new vector word.
     *
     * Note: no need to declare children as by construction they are older than
     * the newly created vector.
     */

    vector = AllocCells(NB_CELLS(WORD_HEADER_SIZE 
	    + (length * sizeof(Col_Word))));
    WORD_SET_TYPE_ID(vector, WORD_TYPE_VECTOR);
    WORD_SYNONYM(vector) = NULL;
    WORD_VECTOR_FLAGS(vector) = 0;
    WORD_VECTOR_LENGTH(vector) = (unsigned short) length;
    memcpy((void *) WORD_VECTOR_ELEMENTS(vector), elements, 
	    length * sizeof(Col_Word));

    return vector;
}

Col_Word
Col_NewVectorWordV(
    size_t length,		/* Number of arguments. */
    ...)			/* Remaining arguments, i.e. words to add in 
				 * order. */
{
    size_t i;
    va_list args;
    Col_Word vector, *elements;

    if (length == 0) {
	return NULL;
    }

    /*
     * Get maximum length. Only copy the elements into vector up to this length.
     */

    if (length > VECTOR_MAX_LENGTH) {
	length = VECTOR_MAX_LENGTH;
    }

    /*
     * Create a new vector word.
     *
     * Note: no need to declare children as by construction they are older than
     * the newly created vector.
     */

    vector = AllocCells(NB_CELLS(WORD_HEADER_SIZE + (length * sizeof(Col_Word))));
    WORD_SET_TYPE_ID(vector, WORD_TYPE_VECTOR);
    WORD_SYNONYM(vector) = NULL;
    WORD_VECTOR_FLAGS(vector) = 0;
    WORD_VECTOR_LENGTH(vector) = (unsigned short) length;
    elements = WORD_VECTOR_ELEMENTS(vector);
    va_start(args, length);
    for (i=0; i < length; i++) {
	elements[i] = va_arg(args, Col_Word);
    }
    return vector;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewListWord --
 *
 *	Create a new list word.
 *
 *	Short lists are created as vectors. Larger lists are recursively split
 *	in half and assembled in a tree.
 *
 * Results:
 *	The new word, or NULL when length is zero.
 *
 * Side effects:
 *	May allocate memory cells.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_NewListWord(
    size_t length,		/* Length of below array. */
    const Col_Word * elements)	/* Array of words to populate list with. */
{
    size_t half=0;		/* Index of the split point. */

    if (length <= VECTOR_MAX_LENGTH) {
	/* 
	 * List fits into one vector. 
	 */

	return Col_NewVectorWord(length, elements);
    }

    /* 
     * The list is built by concatenating the two halves of the array. This 
     * recursive halving ensures that the resulting binary tree is properly 
     * balanced. 
     *
     * TODO: optimize by avoiding useless roots?
     */

    half = length/2;
    return Col_ConcatLists(Col_NewListWord(half, elements),
	    Col_NewListWord(length-half, elements+half));
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
    RESOLVE_WORD(list);

    switch (WORD_TYPE(list)) {
	case WORD_TYPE_VECTOR:
	    return WORD_VECTOR_LENGTH(list);

	case WORD_TYPE_LIST:
	    return WORD_LIST_LENGTH(list);

	case WORD_TYPE_CONCATLIST:
	    return WORD_CONCATLIST_LENGTH(list);

	case WORD_TYPE_SUBLIST:
	    /* 
	     * Sublist length is the range width.
	     */

	    return WORD_SUBLIST_LAST(list)-WORD_SUBLIST_FIRST(list)+1;

	default:
	    return 0;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * NewList --
 *
 *	Utility proc used to wrap a list around a root node. List nodes are
 *	not regular words in the sense that they have no synonym field, so they
 *	must be wrapped around a list word.
 *
 * Results:
 *	The new word.
 *
 * Side effects:
 *	Allocates memory cells.
 *
 *---------------------------------------------------------------------------
 */

static Col_Word	
NewList(
    Col_Word root)		/* The list root. */
{
    Col_Word list = AllocCells(1);
    WORD_SET_TYPE_ID(list, WORD_TYPE_LIST);
    WORD_LIST_LENGTH(list) = Col_ListLength(root);
    WORD_LIST_ROOT(list) = root;

    return list;
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
    Col_Word *elements;		/* Element array. */
} MergeChunksInfo;

static int 
MergeChunksProc(
    const Col_Word *elements, 
    size_t length,
    Col_ClientData clientData) 
{
    MergeChunksInfo *info = (MergeChunksInfo *) clientData;

    /* 
     * Append elements. 
     */

    memcpy((void *) (info->elements+info->length), elements, 
	    length * sizeof(Col_Word));
    info->length += length;
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
 *	When first is past the end of the list, or last is before first, NULL. 
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
    Col_Word sublist;		/* Resulting list in the general case. */
    size_t listLength;		/* Length of source list. */
    unsigned char depth=0;	/* Depth of source list. */
    size_t length;		/* Length of resulting sublist. */

    RESOLVE_WORD(list);

    /* 
     * Quick cases. 
     */

    if (last < first) {
	return NULL;
    }

    listLength = Col_ListLength(list);
    if (first >= listLength) {
	return NULL;
    }

    /* 
     * Truncate overlong sublists. (Note: at this point listLength > 1)
     */

    if (last >= listLength) {
	last = listLength-1;
    }

    length = last-first+1;

    /* 
     * Identity. 
     */

    if (first == 0 && length == listLength) {
	return list;
    }

    /* 
     * Type-specific quick cases. 
     */

    if (WORD_TYPE(list) == WORD_TYPE_LIST) {
	list = WORD_LIST_ROOT(list);
    }
    switch (WORD_TYPE(list)) {
	case WORD_TYPE_SUBLIST:
	    /* 
	     * Point to original source. 
	     */

	    return Col_Sublist(WORD_SUBLIST_SOURCE(list), 
		    WORD_SUBLIST_FIRST(list)+first, 
		    WORD_SUBLIST_FIRST(list)+last);

	case WORD_TYPE_CONCATLIST: {
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

		return Col_Sublist(WORD_CONCATLIST_RIGHT(list), first-leftLength, 
			last-leftLength);
	    }
	    depth = WORD_CONCATLIST_DEPTH(list);
	    break;
	}
    }

    /* 
     * Build a vector for short sublists.
     */

    if (length * sizeof(Col_Word) <= MAX_SHORT_LIST_SIZE - WORD_HEADER_SIZE) {
	MergeChunksInfo info; 
	Col_Word vector = AllocCells(NB_CELLS(WORD_HEADER_SIZE 
		+ (length * sizeof(Col_Word))));
	WORD_SET_TYPE_ID(vector, WORD_TYPE_VECTOR);
	WORD_SYNONYM(vector) = NULL;
	WORD_VECTOR_FLAGS(vector) = 0;

	info.elements = WORD_VECTOR_ELEMENTS(vector);
	info.length = 0;
	Col_TraverseListChunks(list, first, length, MergeChunksProc, &info, 
		NULL);
	WORD_VECTOR_LENGTH(vector) = (unsigned short) info.length;

	return vector;
    }

    /* 
     * General case: build a sublist node.
     */

    sublist = AllocCells(1);
    WORD_SET_TYPE_ID(sublist, WORD_TYPE_SUBLIST);
    WORD_SUBLIST_DEPTH(sublist) = depth;
    WORD_SUBLIST_SOURCE(sublist) = list;
    WORD_SUBLIST_FIRST(sublist) = first;
    WORD_SUBLIST_LAST(sublist) = last;

    return NewList(sublist);
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
    if (WORD_TYPE(node) == WORD_TYPE_CONCATLIST) {
	*leftPtr  = WORD_CONCATLIST_LEFT(node);
	*rightPtr = WORD_CONCATLIST_RIGHT(node);
    } else {
	/* ASSERT(WORD_TYPE(node) == WORD_TYPE_SUBLIST) */
	/* Create one sublist for each list node arm. */
	Col_Word source = WORD_SUBLIST_SOURCE(node);
	/* ASSERT(WORD_TYPE(source) == WORD_TYPE_CONCATLIST) */
	size_t leftLength = WORD_CONCATLIST_LEFT_LENGTH(source);
	if (leftLength == 0) {
	    leftLength = Col_ListLength(WORD_CONCATLIST_LEFT(source));
	}
	*leftPtr = Col_Sublist(WORD_CONCATLIST_LEFT(source), 
		WORD_SUBLIST_FIRST(node), leftLength-1);
	*rightPtr = Col_Sublist(WORD_CONCATLIST_RIGHT(source), 0, 
		WORD_SUBLIST_LAST(node)-leftLength);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ConcatLists --
 * Col_ConcatListsA --
 * Col_ConcatListsV --
 *
 *	Concatenate lists.
 *	Col_ConcatLists concatenates two lists.
 *	Col_ConcatListsA concatenates several lists given in an array, by 
 *	recursive halvings until it contains one or two elements. 
 *	Col_ConcatListsV is a variadic version of Col_ConcatListsA.
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
 *	If both lists are NULL, or if the resulting list would exceed the
 *	maximum length, NULL. Else, a list representing the 
 *	concatenation of both lists.
 *	Array and variadic versions return NULL when concatenating zero lists,
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

    RESOLVE_WORD(left);
    RESOLVE_WORD(right);

    leftLength = Col_ListLength(left);
    rightLength = Col_ListLength(right);
    if (SIZE_MAX-leftLength < rightLength) {
	/*
	 * Avoid creating too long lists.
	 */

	return NULL;	
    }

    /* 
     * Handle quick cases and get input node depths. 
     */

    if (WORD_TYPE(left) == WORD_TYPE_LIST) {
	left = WORD_LIST_ROOT(left);
    }
    if (WORD_TYPE(right) == WORD_TYPE_LIST) {
	right = WORD_LIST_ROOT(right);
    }
    switch (WORD_TYPE(left)) {
	case WORD_TYPE_NULL:
	    /* 
	     * Concat is a no-op on right. 
	     */

	    return right;

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
	    leftDepth = WORD_SUBLIST_DEPTH(left);
	    break;

	case WORD_TYPE_CONCATLIST:
	    leftDepth = WORD_CONCATLIST_DEPTH(left);
	    break;
    }
    switch (WORD_TYPE(right)) {
	case WORD_TYPE_NULL:
	    /* 
	     * Concat is a no-op on left.
	     */

	    return left;

	case WORD_TYPE_SUBLIST:
	    rightDepth = WORD_SUBLIST_DEPTH(right);
	    break;

	case WORD_TYPE_CONCATLIST:
	    rightDepth = WORD_CONCATLIST_DEPTH(right);
	    break;
    }

    /* 
     * Build a vector for short sublists.
     */

    if ((leftLength + rightLength) * sizeof(Col_Word) 
	    <= MAX_SHORT_LIST_SIZE - WORD_HEADER_SIZE) {
	MergeChunksInfo info; 
	Col_Word vector = AllocCells(NB_CELLS(WORD_HEADER_SIZE 
		+ (leftLength + rightLength) * sizeof(Col_Word)));
	WORD_SET_TYPE_ID(vector, WORD_TYPE_VECTOR);
	WORD_SYNONYM(vector) = NULL;
	WORD_VECTOR_FLAGS(vector) = 0;

	info.elements = WORD_VECTOR_ELEMENTS(vector);
	Col_TraverseListChunks(left, 0, leftLength, MergeChunksProc, &info, 
		NULL);
	Col_TraverseListChunks(right, 0, rightLength, MergeChunksProc, &info, 
		NULL);
	WORD_VECTOR_LENGTH(vector) = (unsigned short) info.length;

	return vector;
    }

    /* 
     * Assume the input lists are well balanced by construction. Build a list 
     * that is balanced as well, i.e. where left and right depths don't differ 
     * by more that 1 level.

    /* Note that by construction, a sublist with depth >= 1 points to a concat
     * node, as by construction it cannot point to another sublist (see 
     * Col_Sublist). Likewise, the arms cannot be list root words.
     */

    if (leftDepth > rightDepth+1) {
	/* 
	 * Left is deeper by more than 1 level, rebalance.
	 */

	unsigned char left1Depth=0;
	Col_Word left1, left2;

	/* ASSERT(leftDepth >= 2) */

	GetArms(left, &left1, &left2);
	switch (WORD_TYPE(left1)) {
	    case WORD_TYPE_SUBLIST: left1Depth = WORD_SUBLIST_DEPTH(left1); break;
	    case WORD_TYPE_CONCATLIST: left1Depth = WORD_CONCATLIST_DEPTH(left1); break;
	}
	if (left1Depth < leftDepth-1) {
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
	    return Col_ConcatLists(Col_ConcatLists(left1, left21), 
		    Col_ConcatLists(left22, right));
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

	    return Col_ConcatLists(left1, Col_ConcatLists(left2, right));
	}
    } else if (leftDepth+1 < rightDepth) {
	/* 
	 * Right is deeper by more than 1 level, rebalance. 
	 */

	unsigned char right2Depth=0;
	Col_Word right1, right2;

	/* ASSERT(rightDepth >= 2) */

	GetArms(right, &right1, &right2);
	switch (WORD_TYPE(right2)) {
	    case WORD_TYPE_SUBLIST: right2Depth = WORD_SUBLIST_DEPTH(right2); break;
	    case WORD_TYPE_CONCATLIST: right2Depth = WORD_CONCATLIST_DEPTH(right2); break;
	}
	if (right2Depth < rightDepth-1) {
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
	    return Col_ConcatLists(Col_ConcatLists(left, right11), 
		    Col_ConcatLists(right12, right2));
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

	    return Col_ConcatLists(Col_ConcatLists(left, right1), right2);
	}
    }
    
    /* 
     * General case: build a concat node.
     */

    concatNode = AllocCells(1);
    WORD_SET_TYPE_ID(concatNode, WORD_TYPE_CONCATLIST);
    WORD_CONCATLIST_DEPTH(concatNode) 
	= (leftDepth>rightDepth?leftDepth:rightDepth) + 1;
    WORD_CONCATLIST_LENGTH(concatNode) = leftLength + rightLength;
    WORD_CONCATLIST_LEFT_LENGTH(concatNode) 
	= (leftLength<=USHRT_MAX)?(unsigned short) leftLength:0;
    WORD_CONCATLIST_LEFT(concatNode) = left;
    WORD_CONCATLIST_RIGHT(concatNode) = right;

    return NewList(concatNode);
}

/* - Array version. */
Col_Word
Col_ConcatListsA(
    size_t number,		/* Number of lists in below array. */
    Col_Word * lists)		/* Lists to concatenate in order. */
{
    size_t half;

    /* 
     * Quick cases.
     */

    if (number == 0) {return NULL;}
    if (number == 1) {return lists[0];}
    if (number == 2) {return Col_ConcatLists(lists[0], lists[1]);}

    /* 
     * Split array and concatenate both halves. This should result in a well 
     * balanced tree. 
     */

    half = number/2;
    return Col_ConcatLists(Col_ConcatListsA(half, lists), 
	    Col_ConcatListsA(number-half, lists+half));
}

/* - Variadic version. */
Col_Word
Col_ConcatListsV(
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
 * Col_TraverseListChunks --
 *
 *	Iterate over the vectors of a list.
 *
 *	For each traversed vector, proc is called back with the opaque data as
 *	well as the position within the list. If it returns a non-zero result 
 *	then the iteration ends. 
 *
 * Results:
 *	The return value of the last called proc, or -1 if no traversal was
 *	performed.
 *	If non-NULL, the value pointed by lengthPtr is the total length of the 
 *	traversed list, i.e. the number of elements.
 *
 * Side effects:
 *	If non-NULL, the value pointed to by lengthPtr is overwritten.
 *
 *---------------------------------------------------------------------------
 */

int 
Col_TraverseListChunks(
    Col_Word list,		/* List to traverse. */
    size_t start,		/* Index of first element. */
    size_t max,			/* Max number of elements. */
    Col_ListChunksEnumProc *proc,	
				/* Callback proc called at each vector. */
    Col_ClientData clientData,	/* Opaque data passed as is to above proc. */
    size_t *lengthPtr)		/* If non-NULL, will hold the total number of 
				 * elements traversed upon completion. */
{
    size_t listLength;

    RESOLVE_WORD(list);

    if (lengthPtr) {
	*lengthPtr = 0;
    }

    /* 
     * Quick cases.
     */

    listLength = Col_ListLength(list);
    if (start >= listLength) {
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
    if (max == 0) {
	/*
	 * Nothing to traverse.
	 */

	return -1;
    }

    if (WORD_TYPE(list) == WORD_TYPE_LIST) {
	list = WORD_LIST_ROOT(list);
    }
    switch (WORD_TYPE(list)) {
	case WORD_TYPE_VECTOR:
	    if (lengthPtr) *lengthPtr = max;
	    return proc(WORD_VECTOR_ELEMENTS(list)+start, max, clientData);

	case WORD_TYPE_SUBLIST:
	    /* 
	     * Sublist: propagage to source list.
	     */

	    return Col_TraverseListChunks(WORD_SUBLIST_SOURCE(list), 
		    WORD_SUBLIST_FIRST(list)+start, max, proc, clientData, 
		    lengthPtr);
	    
	case WORD_TYPE_CONCATLIST: {
	    /* 
	     * Concat: propagate to covered arms.
	     */

	    int result;
	    size_t leftLength = WORD_CONCATLIST_LEFT_LENGTH(list);
	    if (leftLength == 0) {
		leftLength = Col_ListLength(WORD_CONCATLIST_LEFT(list));
	    }
	    if (start >= leftLength) {
		/* 
		 * Traverse right arm only. 
		 */

		return Col_TraverseListChunks(WORD_CONCATLIST_RIGHT(list), 
			start-leftLength, max, proc, clientData, lengthPtr);
	    } 

	    /*
	     * Traverse left arm.
	     */

	    result = Col_TraverseListChunks(WORD_CONCATLIST_LEFT(list), start, 
		    max, proc, clientData, lengthPtr);

	    /*
	     * Stop if previous result is non-zero, or when traversal doesn't 
	     * cover the right arm. 
	     */

	    if (result == 0 && start + max > leftLength) {
		size_t length2;
		result = Col_TraverseListChunks(WORD_CONCATLIST_RIGHT(list), 0, 
			start+max-leftLength, proc, clientData, &length2);
		if (lengthPtr) {
		    *lengthPtr += length2;
		}
	    }
	    return result;
	}
    }

    /* CANTHAPPEN */
    return 0;
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
 *	None.
 *
 * Side effects:
 *	If index points past the end of the list, the iterator pointed to by 
 *	it is initialized to the end iterator (i.e. whose list field is 
 *	NULL), else it points to the element within the list.
 *
 *---------------------------------------------------------------------------
 */

void
Col_ListIterBegin(
    Col_Word list,		/* List to iterate over. */
    size_t index,		/* Index of element. */
    Col_ListIterator *it)	/* Iterator to initialize. */
{
    RESOLVE_WORD(list);

    if (index >= Col_ListLength(list)) {
	/*
	 * End of rope.
	 */

	it->list = NULL;

	return;
    }

    it->list = list;
    it->index = index;

    /*
     * Traversal info will be lazily computed.
     */

    it->traversal.subnode = NULL;
    it->traversal.leaf = NULL;
}

/*
 *---------------------------------------------------------------------------
 *
 * UpdateTraversalInfo --
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

static void
UpdateTraversalInfo(
    Col_ListIterator *it)	/* The iterator to update. */
{
    Col_Word node;
    size_t first, last, offset;

    if (it->traversal.subnode && (it->index < it->traversal.first 
		    || it->index > it->traversal.last)) {
	/*
	 * Out of range.
	 */

	it->traversal.subnode = NULL;
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

    it->traversal.leaf = NULL;
    while (!it->traversal.leaf) {
	switch (WORD_TYPE(node)) {
	    case WORD_TYPE_VECTOR:
		/*
		 * Flat vectors are directly addressable. 
		 */

		it->traversal.leaf = node;
		it->traversal.index = it->index - offset;
		break;

	    case WORD_TYPE_LIST:
		/*
		 * Recurse into root.
		 */

		node = WORD_LIST_ROOT(node);
		break;

	    case WORD_TYPE_SUBLIST: 
		if (WORD_SUBLIST_DEPTH(node) == MAX_ITERATOR_SUBNODE_DEPTH 
			|| !it->traversal.subnode) {
		    /*
		     * Remember as subnode.
		     */

		    it->traversal.subnode = node;
		    it->traversal.first = first;
		    it->traversal.last = last;
		    it->traversal.offset = offset;
		}

		/*
		 * Recurse into source.
		 * Note: offset may become negative but it doesn't matter.
		 */

		offset -= WORD_SUBLIST_FIRST(node);
		node = WORD_SUBLIST_SOURCE(node);
		break;

	    case WORD_TYPE_CONCATLIST: {
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

		if (it->index < offset + leftLength) {
		    /*
		     * Recurse into left arm.
		     */

		    last = offset + leftLength-1;
		    node = WORD_CONCATLIST_LEFT(node);
		} else {
		    /*
		     * Recurse into right arm.
		     */

		    first = offset + leftLength;
		    offset += leftLength;
		    node = WORD_CONCATLIST_RIGHT(node);
		}
		break;

	    default:
		/* CANTHAPPEN */
		return;
	    }
	}
    }
    if (!it->traversal.subnode) {
	it->traversal.subnode = it->traversal.leaf;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ListIterElementAt --
 *
 *	Get the element designated by the iterator.
 *
 * Results:
 *	If the index is past the end of the list, NULL, else the element.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_ListIterElementAt(
    Col_ListIterator *it)	/* Iterator that points to the element. */
{
    if (Col_ListIterEnd(it)) {
	return NULL;
    }

    if (!it->traversal.leaf) {
	UpdateTraversalInfo(it);
    }

    return WORD_VECTOR_ELEMENTS(it->traversal.leaf)[it->traversal.index];
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
 *	None.
 *
 * Side effects:
 *	Update the iterator.
 *
 *---------------------------------------------------------------------------
 */

void
Col_ListIterForward(
    Col_ListIterator *it,	/* The iterator to move. */
    size_t nb)			/* Offset. */
{
    if (Col_ListIterEnd(it) || nb == 0) {
	return;
    }

    /*
     * Check for end of list.
     */
    
    if (nb >= Col_ListLength(it->list) - it->index) {
	/*
	 * Reached end of list.
	 */

	it->list = NULL;
	return;
    }
    it->index += nb;

    if (!it->traversal.subnode || !it->traversal.leaf) {
	/*
	 * No traversal info.
	 */

	return;
    }
    if (it->index > it->traversal.last) {
	/*
	 * Traversal info out of range. Discard leaf node info, but not 
	 * subnode, as it may be used again should the iteration go back.
	 */

	it->traversal.leaf = NULL;
	return;
    }

    /*
     * Update traversal info.
     */

    if (nb >= WORD_VECTOR_LENGTH(it->traversal.leaf) - it->traversal.index) {
	/*
	 * Reached end of leaf.
	 */

	it->traversal.leaf = NULL;
    } else {
	it->traversal.index += nb;
    }
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
    if (Col_ListIterEnd(it) || nb == 0) {
	return;
    }

    /*
     * Check for beginning of list.
     */
    
    if (it->index < nb) {
	it->list = NULL;
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

	it->traversal.leaf = NULL;
	return;
    }

    /*
     * Update traversal info.
     */

    if (it->traversal.index < nb) {
	/*
	 * Reached beginning of leaf. 
	 */

	it->traversal.leaf = NULL;
    } else {
	it->traversal.index -= nb;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ListIterMoveTo --
 *
 *	Move the iterator to the given absolute position.
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
Col_ListIterMoveTo(
    Col_ListIterator *it,	/* The iterator to move. */
    size_t index)		/* Position. */
{
    if (index > it->index) {
	Col_ListIterForward(it, index - it->index);
    } else if (index < it->index) {
	Col_ListIterBackward(it, it->index - index);
    }
}
