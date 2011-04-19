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

#include <stdlib.h>
#include <limits.h>
#include <malloc.h> /* For alloca */

/* 
 * Max byte size/length of short list (inc. header) created with sublist or 
 * concat. 
 */

#define MAX_SHORT_LIST_SIZE	(3*CELL_SIZE)
#define MAX_SHORT_LIST_LENGTH	((MAX_SHORT_LIST_SIZE-WORD_HEADER_SIZE)/sizeof(Col_Word))

/*
 * Max depth of subnodes in iterators.
 */

#define MAX_ITERATOR_SUBNODE_DEPTH 3

/*
 * Prototypes for functions used only in this file.
 */

static Col_ListChunksEnumProc MergeChunksProc;
static Col_Word		NewListWord(Col_Word root);
static Col_Word		NewListNode(size_t length, const Col_Word * elements);
static Col_Word		Sublist(Col_Word list,	size_t first, size_t last);
static void		GetArms(Col_Word node, Col_Word * leftPtr, 
			    Col_Word * rightPtr);
static Col_Word		ConcatLists(Col_Word left, Col_Word right);
static Col_Word		ConcatListsA(size_t number, const Col_Word * words);
static Col_Word		ConcatListsL(Col_Word list, size_t first, size_t last);
static Col_Word		NewSequenceWord(Col_Word root);
static Col_Word		NewSequenceNode(Col_Word part, Col_Word *sequencePtr, 
			    Col_Word prev);
static Col_Word		NewSequence(const Col_Word * partsA, Col_Word partsL,
			    size_t first, size_t last);
static void		UpdateTraversalInfo(Col_ListIterator *it);
static int		PushNode(Col_SequenceIterator *it);
static void		PopNode(Col_SequenceIterator *it);
static void		FindPart(Col_SequenceIterator *it);


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
 * Col_NewVector --
 * Col_NewVectorV --
 *
 *	Create a new vector word.
 *	Col_NewVectorWordV is a variadic version of Col_NewVectorWord.
 *
 *	Truncates the provided data if it is larger than the maximum length.
 *	Calling code must thus check the length of the created word, and use
 *	additional structures to assemble vectors into larger lists.
 *
 * Results:
 *	The new word.
 *
 * Side effects:
 *	May allocate memory cells.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_NewVector(
    size_t length,		/* Length of below array. */
    const Col_Word * elements)	/* Array of words to populate vector with. */
{
    Col_Word vector;		/* Resulting word in the general case. */
    size_t i;
    Col_Word *dst;
    int hasRefs=0;

    if (length == 0) {
	/* 
	 * Use singleton immediate value.
	 */

	return WORD_EMPTY;
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

    vector = (Col_Word) AllocCells(NB_CELLS(WORD_HEADER_SIZE 
	    + (length * sizeof(Col_Word))));
    WORD_SET_TYPE_ID(vector, WORD_TYPE_VECTOR);
    WORD_SYNONYM(vector) = WORD_NIL;
    WORD_VECTOR_LENGTH(vector) = (unsigned short) length;
    dst = WORD_VECTOR_ELEMENTS(vector);
    for (i=0; i < length; i++) {
	dst[i] = elements[i];
	if (WORD_TYPE(dst[i]) == WORD_TYPE_REFERENCE) hasRefs = 1;
    }
    WORD_VECTOR_FLAGS(vector) = (hasRefs?WORD_VECTOR_FLAG_HASREFS:0);

    return vector;
}

Col_Word
Col_NewVectorV(
    size_t length,		/* Number of arguments. */
    ...)			/* Remaining arguments, i.e. words to add in 
				 * order. */
{
    size_t i;
    va_list args;
    Col_Word vector, *elements;
    int hasRefs=0;

    if (length == 0) {
	/* 
	 * Use singleton immediate value.
	 */

	return WORD_EMPTY;
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

    vector = (Col_Word) AllocCells(NB_CELLS(WORD_HEADER_SIZE 
	    + (length * sizeof(Col_Word))));
    WORD_SET_TYPE_ID(vector, WORD_TYPE_VECTOR);
    WORD_SYNONYM(vector) = WORD_NIL;
    WORD_VECTOR_LENGTH(vector) = (unsigned short) length;
    elements = WORD_VECTOR_ELEMENTS(vector);
    va_start(args, length);
    for (i=0; i < length; i++) {
	elements[i] = va_arg(args, Col_Word);
	if (WORD_TYPE(elements[i]) == WORD_TYPE_REFERENCE) hasRefs = 1;
    }
    WORD_VECTOR_FLAGS(vector) = (hasRefs?WORD_VECTOR_FLAG_HASREFS:0);

    return vector;
}

/*
 *---------------------------------------------------------------------------
 *
 * NewListWord --
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
NewListWord(
    Col_Word root)		/* The list root. */
{
    Col_Word list;
    switch (WORD_TYPE(root)) {
	case WORD_TYPE_SUBLIST:
	case WORD_TYPE_CONCATLIST:
	    list = (Col_Word) AllocCells(1);
	    WORD_SET_TYPE_ID(list, WORD_TYPE_LIST);
	    WORD_LIST_LENGTH(list) = Col_ListLength(root);
	    WORD_LIST_ROOT(list) = root;

	    return list;
	
	default:
	    return root;
    }
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
 * Results:
 *	The new word, or nil when length is zero.
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
    return NewListWord(NewListNode(length, elements));
}

static Col_Word
NewListNode(
    size_t length,		/* Length of below array. */
    const Col_Word * elements)	/* Array of words to populate list with. */
{
    size_t half=0;		/* Index of the split point. */

    if (length <= VECTOR_MAX_LENGTH) {
	/* 
	 * List fits into one vector. 
	 */

	return Col_NewVector(length, elements);
    }

    /* 
     * The list is built by concatenating the two halves of the array. This 
     * recursive halving ensures that the resulting binary tree is properly 
     * balanced. 
     */

    half = length/2;
    return ConcatLists(NewListNode(half, elements),
	    NewListNode(length-half, elements+half));
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

	default: /* Including WORD_TYPE_EMPTY */
	    /* 
	     * Note: non-list nodes (i.e. scalars) are not considered as
	     * single-element lists and thus have zero length.
	     */

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
    const Col_Word *elements, 
    size_t length,
    Col_ClientData clientData) 
{
    MergeChunksInfo *info = (MergeChunksInfo *) clientData;
    size_t i;
    Col_Word *dst;
    int hasRefs;

    /* 
     * Append elements. 
     */

    hasRefs = WORD_VECTOR_FLAGS(info->vector) & WORD_VECTOR_FLAG_HASREFS;
    dst = WORD_VECTOR_ELEMENTS(info->vector)+info->length;
    for (i=0; i < length; i++) {
	dst[i] = elements[i];
	if (WORD_TYPE(dst[i]) == WORD_TYPE_REFERENCE) hasRefs = 1;
    }
    WORD_VECTOR_FLAGS(info->vector) |= (hasRefs?WORD_VECTOR_FLAG_HASREFS:0);
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
 *	When first is past the end of the list, or last is before first, nil. 
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
    Col_Word listRoot;		/* Actual root node. */

    RESOLVE_WORD(list);

    /*
     * Avoid creating useless list roots for identity cases.
     */

    if (WORD_TYPE(list) == WORD_TYPE_LIST) {
	listRoot = WORD_LIST_ROOT(list);
    } else {
	listRoot = list;
    }

    sublist = Sublist(listRoot, first, last);

    if (sublist == listRoot) {
	return list;
    } else {
	return NewListWord(sublist);
    }
}

static Col_Word
Sublist(
    Col_Word list,		/* The list to extract the sublist from. */
    size_t first, size_t last)	/* Range of sublist. */
{
    Col_Word sublist;		/* Resulting list node in the general case. */
    size_t listLength;		/* Length of source list. */
    unsigned char depth=0;	/* Depth of source list. */
    size_t length;		/* Length of resulting sublist. */
    int hasRefs=0;		/* Whether source has references. */

    /* 
     * Quick cases. 
     */

    if (last < first) {
	return WORD_NIL;
    }

    listLength = Col_ListLength(list);
    if (first >= listLength) {
	/*
	 * This handles empty and scalars as well.
	 */

	return WORD_NIL;
    }

    /* 
     * Truncate overlong sublists. (Note: at this point listLength > 1)
     */

    if (last >= listLength) {
	last = listLength-1;
    }

    length = last-first+1;

    if (first == 0 && length == listLength) {
	/* 
	 * Identity. 
	 */

	return list;
    }

    /* 
     * Type-specific quick cases. 
     */

    /* ASSERT(WORD_TYPE(list) != WORD_TYPE_EMPTY */
    /* ASSERT(WORD_TYPE(list) != WORD_TYPE_LIST */
    switch (WORD_TYPE(list)) {
	case WORD_TYPE_VECTOR:
	     /*
	     * Don't scan the whole vector to check for references, simply 
	     * replicate the flag value.
	     */

	    hasRefs = WORD_VECTOR_FLAGS(list) & WORD_VECTOR_FLAG_HASREFS;
	    break;

	case WORD_TYPE_SUBLIST:
	    /* 
	     * Point to original source. 
	     */

	    return Sublist(WORD_SUBLIST_SOURCE(list), 
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

		return Sublist(WORD_CONCATLIST_LEFT(list), first, last);
	    } else if (first >= leftLength) {
		/*
		 * Right arm is a superset of sublist. 
		 */

		return Sublist(WORD_CONCATLIST_RIGHT(list), first-leftLength, 
			last-leftLength);
	    }
	    depth = WORD_LISTNODE_DEPTH(list);

	    /*
	     * Don't scan the whole list to check for references, simply 
	     * replicate the flag value.
	     */

	    hasRefs = WORD_LISTNODE_FLAGS(list) & WORD_LISTNODE_FLAG_HASREFS;
	    break;
	}
    }

    /* 
     * Build a vector for short sublists.
     */

    if (length <= MAX_SHORT_LIST_LENGTH) {
	MergeChunksInfo info; 
	Col_Word vector = (Col_Word) AllocCells(NB_CELLS(WORD_HEADER_SIZE 
		+ (length * sizeof(Col_Word))));
	WORD_SET_TYPE_ID(vector, WORD_TYPE_VECTOR);
	WORD_SYNONYM(vector) = WORD_NIL;
	WORD_VECTOR_FLAGS(vector) = 0;
	
	info.length = 0;
	info.vector = vector;
	Col_TraverseListChunks(list, first, length, MergeChunksProc, &info, 
		NULL);
	WORD_VECTOR_LENGTH(vector) = (unsigned short) info.length;

	return vector;
    }

    /* 
     * General case: build a sublist node.
     */

    sublist = (Col_Word) AllocCells(1);
    WORD_SET_TYPE_ID(sublist, WORD_TYPE_SUBLIST);
    WORD_LISTNODE_FLAGS(sublist) = (hasRefs?WORD_LISTNODE_FLAG_HASREFS:0);
    WORD_LISTNODE_DEPTH(sublist) = depth;
    WORD_SUBLIST_SOURCE(sublist) = list;
    WORD_SUBLIST_FIRST(sublist) = first;
    WORD_SUBLIST_LAST(sublist) = last;

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
	*leftPtr = Sublist(WORD_CONCATLIST_LEFT(source), 
		WORD_SUBLIST_FIRST(node), leftLength-1);
	*rightPtr = Sublist(WORD_CONCATLIST_RIGHT(source), 0, 
		WORD_SUBLIST_LAST(node)-leftLength);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ConcatLists --
 * Col_ConcatListsA --
 * Col_ConcatListsV --
 * Col_ConcatListsL --
 *
 *	Concatenate lists.
 *	Col_ConcatLists concatenates two lists.
 *	Col_ConcatListsA concatenates several lists given in an array, by 
 *	recursive halvings until it contains one or two elements. 
 *	Col_ConcatListsV is a variadic version of Col_ConcatListsA.
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
 *	If both lists are nil, or if the resulting list would exceed the
 *	maximum length, nil. Else, a list representing the 
 *	concatenation of both lists.
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
    Col_Word leftRoot, rightRoot;
				/* Actual root nodes. */

    RESOLVE_WORD(left);
    RESOLVE_WORD(right);

    /*
     * Avoid creating useless list roots for identity cases.
     */

    if (WORD_TYPE(left) == WORD_TYPE_LIST) {
	leftRoot  = WORD_LIST_ROOT(left);
    } else {
	leftRoot = left;
    }
    if (WORD_TYPE(right) == WORD_TYPE_LIST) {
	rightRoot = WORD_LIST_ROOT(right);
    } else {
	rightRoot = right;
    }

    concatNode = ConcatLists(leftRoot, rightRoot);

    if (concatNode == leftRoot)  return left;
    if (concatNode == rightRoot) return right;

    return NewListWord(concatNode);
}

static Col_Word
ConcatLists(
    Col_Word left,		/* Left part. */
    Col_Word right)		/* Right part. */
{
    Col_Word concatNode;	/* Resulting list node in the general case. */
    unsigned char leftDepth=0, rightDepth=0; 
				/* Respective depths of left and right lists. */
    size_t leftLength, rightLength;
				/* Respective lengths. */
    int hasRefs=0;		/* Whether either part has references. */

    RESOLVE_WORD(left);
    RESOLVE_WORD(right);

    leftLength = Col_ListLength(left);
    rightLength = Col_ListLength(right);
    if (SIZE_MAX-leftLength < rightLength) {
	/*
	 * Prevent the creation of too long lists.
	 */

	return WORD_NIL;
    }

    /* 
     * Handle quick cases and get input node depths. 
     */

    if (leftLength == 0) {
	/* 
	 * Distinguish between empty+nil vs. empty+non-nil so that concat
	 * will return a non-nil result. 
	 *
	 * This handles scalars as well.
	 */

	return (WORD_TYPE(right)==WORD_TYPE_NIL?left:right);
    } else if (rightLength == 0) {
	/* 
	 * Concat is a no-op on left. Special cases with nil are already 
	 * handled above. 
	 */

	return left;
    }

    if (WORD_TYPE(left) == WORD_TYPE_LIST) {
	left = WORD_LIST_ROOT(left);
    }
    if (WORD_TYPE(right) == WORD_TYPE_LIST) {
	right = WORD_LIST_ROOT(right);
    }
    /* ASSERT(WORD_TYPE(left) != WORD_TYPE_EMPTY */
    /* ASSERT(WORD_TYPE(left) != WORD_TYPE_LIST */
    switch (WORD_TYPE(left)) {
	case WORD_TYPE_VECTOR:
	    hasRefs = WORD_VECTOR_FLAGS(left) & WORD_VECTOR_FLAG_HASREFS;
	    break;

	case WORD_TYPE_SUBLIST:
	    if (WORD_TYPE(right) == WORD_TYPE_SUBLIST
		    && WORD_SUBLIST_SOURCE(left) == WORD_SUBLIST_SOURCE(right) 
		    && WORD_SUBLIST_LAST(left)+1 == WORD_SUBLIST_FIRST(right)) {
		/* 
		 * Merge if left and right are adjacent sublists. This allows
		 * for fast consecutive insertions/removals at a given index.
		 */

		return Sublist(WORD_SUBLIST_SOURCE(left), 
			WORD_SUBLIST_FIRST(left), WORD_SUBLIST_LAST(right));
	    }
	    /* continued. */

	case WORD_TYPE_CONCATLIST:
	    leftDepth = WORD_LISTNODE_DEPTH(left);
	    hasRefs = WORD_LISTNODE_FLAGS(left) & WORD_LISTNODE_FLAG_HASREFS;
	    break;
    }
    /* ASSERT(WORD_TYPE(right) != WORD_TYPE_EMPTY */
    /* ASSERT(WORD_TYPE(right) != WORD_TYPE_LIST */
    switch (WORD_TYPE(right)) {
	case WORD_TYPE_VECTOR:
	    hasRefs |= WORD_VECTOR_FLAGS(right) & WORD_VECTOR_FLAG_HASREFS;
	    break;

	case WORD_TYPE_SUBLIST:
	case WORD_TYPE_CONCATLIST:
	    rightDepth = WORD_LISTNODE_DEPTH(right);
	    hasRefs |= WORD_LISTNODE_FLAGS(right) & WORD_LISTNODE_FLAG_HASREFS;
	    break;
    }

    /* 
     * Build a vector for short sublists.
     */

    if (leftLength + rightLength <= MAX_SHORT_LIST_LENGTH) {
	MergeChunksInfo info; 
	Col_Word vector = (Col_Word) AllocCells(NB_CELLS(WORD_HEADER_SIZE 
		+ (leftLength + rightLength) * sizeof(Col_Word)));
	WORD_SET_TYPE_ID(vector, WORD_TYPE_VECTOR);
	WORD_SYNONYM(vector) = WORD_NIL;
	WORD_VECTOR_FLAGS(vector) = 0;
	
	info.length = 0;
	info.vector = vector;
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
	    case WORD_TYPE_SUBLIST: 
	    case WORD_TYPE_CONCATLIST:
		left1Depth = WORD_LISTNODE_DEPTH(left1); 
		break;
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
	    return ConcatLists(ConcatLists(left1, left21), 
		    ConcatLists(left22, right));
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

	    return ConcatLists(left1, ConcatLists(left2, right));
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
	    case WORD_TYPE_SUBLIST:
	    case WORD_TYPE_CONCATLIST:
		right2Depth = WORD_LISTNODE_DEPTH(right2); 
		break;
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
	    return ConcatLists(ConcatLists(left, right11), 
		    ConcatLists(right12, right2));
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

	    return ConcatLists(ConcatLists(left, right1), right2);
	}
    }
    
    /* 
     * General case: build a concat node.
     */

    concatNode = (Col_Word) AllocCells(1);
    WORD_SET_TYPE_ID(concatNode, WORD_TYPE_CONCATLIST);
    WORD_LISTNODE_FLAGS(concatNode) = (hasRefs?WORD_LISTNODE_FLAG_HASREFS:0);
    WORD_LISTNODE_DEPTH(concatNode) 
	= (leftDepth>rightDepth?leftDepth:rightDepth) + 1;
    WORD_CONCATLIST_LENGTH(concatNode) = leftLength + rightLength;
    WORD_CONCATLIST_LEFT_LENGTH(concatNode) 
	= (leftLength<=UCHAR_MAX)?(unsigned char) leftLength:0;
    WORD_CONCATLIST_LEFT(concatNode) = left;
    WORD_CONCATLIST_RIGHT(concatNode) = right;

    return concatNode;
}

/* - Array version. */
Col_Word
Col_ConcatListsA(
    size_t number,		/* Number of lists in below array. */
    const Col_Word * lists)	/* Lists to concatenate in order. */
{
    return NewListWord(ConcatListsA(number, lists));
}

static Col_Word
ConcatListsA(
    size_t number,		/* Number of lists in below array. */
    const Col_Word * lists)	/* Lists to concatenate in order. */
{
    size_t half;

    /* 
     * Quick cases.
     */

    if (number == 0) {return WORD_NIL;}
    if (number == 1) {return lists[0];}
    if (number == 2) {return ConcatLists(lists[0], lists[1]);}

    /* 
     * Split array and concatenate both halves. This should result in a well 
     * balanced tree. 
     */

    half = number/2;
    return ConcatLists(ConcatListsA(half, lists), ConcatListsA(number-half, 
	    lists+half));
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

/* - List version. */
Col_Word
Col_ConcatListsL(
    Col_Word list,		/* List of lists to concatenate. */
    size_t first, size_t last)	/* Range to consider. */
{
    size_t length;

    /* 
     * Normalize range.
     */

    if (last < first) {
	return WORD_NIL;
    }
    length = Col_ListLength(list);
    if (first >= length) {
	return WORD_NIL;
    }
    if (last >= length) {
	last = length-1;
    }
    return NewListWord(ConcatListsL(list, first, last));
}

static Col_Word
ConcatListsL(
    Col_Word list,		/* List of lists to concatenate. */
    size_t first, size_t last)	/* Range to consider. */
{
    size_t number = last-first+1, half;

    /* 
     * Quick cases.
     */

    if (number == 0) {return WORD_NIL;}
    if (number == 1) {return Col_ListElementAt(list, first);}
    if (number == 2) {
	Col_ListIterator it;
	Col_Word l1, l2;

	Col_ListIterBegin(list, first, &it);
	l1 = Col_ListIterElementAt(&it);
	Col_ListIterNext(&it);
	l2 = Col_ListIterElementAt(&it);
	return ConcatLists(l1, l2);
    }

    /* 
     * Split range and concatenate both halves. This should result in a well 
     * balanced tree. 
     */

    half = number/2;
    return ConcatLists(ConcatListsL(list, first, first+half-1), 
	    ConcatListsL(list, first+half, last));
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewSequence --
 * Col_NewSequenceV --
 * Col_NewSequenceL --
 *
 *	Create a new sequence word from parts that must be lists or references
 *	to lists.
 *	Col_NewSequenceV is a variadic version of Col_NewSequence.
 *	Col_NewSequenceL takes a list of parts as input.
 *	Col_NewSequenceI takes an interval of list iterators as input.
 *
 *	Attempts to create a simple list whenever possible (i.e. when there is
 *	no reference and the total length is within the allowed limits).
 *
 * Results:
 *	Nil if the number of parts is zero or if all parts are nil.
 *	Else, the new word, which may be a simple list or reference.
 *
 * Side effects:
 *	Allocates memory cells.
 *
 *---------------------------------------------------------------------------
 */

static Col_Word
NewSequenceWord(
    Col_Word root)		/* Root node of sequence. */
{
    Col_Word sequence = (Col_Word) AllocCells(1);
    WORD_SET_TYPE_ID(sequence, WORD_TYPE_SEQUENCE);
    WORD_SYNONYM(sequence) = WORD_NIL;
    WORD_SEQUENCE_CYCLIC(sequence) = 0;
    WORD_SEQUENCE_ROOT(sequence) = root;
    WORD_SEQUENCE_STACKNODES(sequence) = WORD_NIL;

    return sequence;
}
static Col_Word
NewSequenceNode(
    Col_Word part,
    Col_Word *sequencePtr, 
    Col_Word prev) 
{
    Col_Word node;
    unsigned char cyclic;

    switch (WORD_TYPE(part)) {
	case WORD_TYPE_VECTOR:
	case WORD_TYPE_LIST:
	    node = (Col_Word) AllocCells(1);
	    WORD_SET_TYPE_ID(node, WORD_TYPE_SEQUENCE_NODE);
	    WORD_SEQNODE_LIST(node) = part;
	    WORD_SEQNODE_REF(node) = WORD_NIL;
	    WORD_SEQNODE_NEXT(node) = WORD_NIL;

	    if (!*sequencePtr) {
		*sequencePtr = NewSequenceWord(node);
	    }
	    if (prev) {
		WORD_SEQNODE_NEXT(prev) = node;
	    }
	    return node;

	case WORD_TYPE_REFERENCE:
	    node = (Col_Word) AllocCells(1);
	    WORD_SET_TYPE_ID(node, WORD_TYPE_SEQUENCE_NODE);
	    WORD_SEQNODE_LIST(node) = WORD_NIL;
	    WORD_SEQNODE_REF(node) = part;
	    WORD_SEQNODE_NEXT(node) = WORD_NIL;

	    if (!*sequencePtr) {
		*sequencePtr = NewSequenceWord(node);
	    }
	    if (prev) {
		WORD_SEQNODE_NEXT(prev) = node;
	    }
	    return node;

	case WORD_TYPE_SEQUENCE:
	    /* 
	     * Copy & insert sequence nodes.
	     */

	    cyclic = WORD_SEQUENCE_CYCLIC(part);
	    if (cyclic && *sequencePtr) {
		WORD_SEQUENCE_CYCLIC(*sequencePtr) = 1;
	    }
	    part = WORD_SEQUENCE_ROOT(part);
	    do {
		node = (Col_Word) AllocCells(1);
		WORD_SET_TYPE_ID(node, WORD_TYPE_SEQUENCE_NODE);
		WORD_SEQNODE_LIST(node) = WORD_SEQNODE_LIST(part);
		WORD_SEQNODE_REF(node) = WORD_SEQNODE_REF(part);
		WORD_SEQNODE_NEXT(node) = WORD_NIL;

		if (!*sequencePtr) {
		    *sequencePtr = NewSequenceWord(node);
		    WORD_SEQUENCE_CYCLIC(*sequencePtr) = cyclic;
		}
		if (prev) {
		    WORD_SEQNODE_NEXT(prev) = node;
		}
		prev = node;
		part = WORD_SEQNODE_NEXT(part);
	    } while (WORD_TYPE(part) == WORD_TYPE_SEQUENCE_NODE);
	    if (part) {
		/*
		 * Convert non-node tail as well.
		 */

		/* ASSERT(WORD_TYPE_PART(part) != WORD_TYPE_SEQUENCE_NODE) */
		return NewSequenceNode(part, sequencePtr, prev);
	    } else {
		return prev;
	    }

	default: /* CANTHAPPEN */
	    return WORD_NIL;
    }
}

static Col_Word
NewSequence(
    const Col_Word * partsA,	/* Parts of the sequence to create (array 
				 * version). */
    Col_Word partsL,		/* List containing parts (list version). */
    size_t first, size_t last)	/* Range to consider. */
{
    Col_Word sequence;		/* Resulting word in the general case. */
    Col_Word prev, prevPrev;	/* Used when chaining sequence nodes. */
    size_t listLength;		/* Cumulated length of lists to merge so far. */
    size_t firstList;		/* First of consecutive lists parts to merge. */
    size_t i;
    Col_ListIterator it;
    size_t length;
    int type;

    /*
     * Quick cases.
     */

    if (first > last) {return WORD_NIL;}
    if (first == last) {
	Col_Word part = (partsA ? partsA[first] : 
		Col_ListElementAt(partsL, first));
	RESOLVE_WORD(part);
	switch (WORD_TYPE(part)) {
	    case WORD_TYPE_VECTOR:
	    case WORD_TYPE_LIST:
	    case WORD_TYPE_REFERENCE:
	    case WORD_TYPE_SEQUENCE:
		return part;

	    default:
		return WORD_NIL;
	}
    }

    /*
     * Initialize word to nil for now; we may still be able to return one of 
     * the given part in simple cases (e.g. concatenating several lists into
     * one). Create sequence word only when needed.
     */

    sequence = WORD_NIL;

    /*
     * Add parts sequentially.
     */

    listLength = 0;
    prev = prevPrev = WORD_NIL;
    for (i=first, (partsA ? 0 : Col_ListIterBegin(partsL, first, &it)); 
	    i <= last; i++, (partsA ? 0 : Col_ListIterNext(&it))) {
	Col_Word part = (partsA ? partsA[i] : Col_ListIterElementAt(&it));
	RESOLVE_WORD(part);
	type = WORD_TYPE(part);
	switch (type) {
	    case WORD_TYPE_VECTOR:
	    case WORD_TYPE_LIST:
		length = Col_ListLength(part);
		if (length <= SIZE_MAX-listLength) {
		    /*
		     * Merge list with previous ones.
		     */

		    if (listLength == 0) {
			firstList = i;
		    }
		    listLength += length;
		    continue; /* Continue loop. */
		}
		break;

	    case WORD_TYPE_REFERENCE:
	    case WORD_TYPE_SEQUENCE:
		break;

	    default:
		/*
		 * Unexpected type, skip part altogether.
		 */

		continue;
	}

	if (WORD_TYPE(prev) != WORD_TYPE_SEQUENCE_NODE) {
	    /*
	     * Ensure that previous node is a regular node.
	     */

	    prev = NewSequenceNode(prev, &sequence, prevPrev);
	}

	if (listLength > 0) {
	    /*
	     * Add node with merged lists so far.
	     */

	    Col_Word list = (partsA ? Col_ConcatListsA(i-firstList, 
		    partsA+firstList) : Col_ConcatListsL(partsL, firstList, 
		    i-1));
	    Col_Word node = NewSequenceNode(list, &sequence, prev);
	    prevPrev = prev;
	    prev = node;
	    listLength = 0;
	}

	switch (type) {
	    case WORD_TYPE_VECTOR:
	    case WORD_TYPE_LIST:
		/*
		 * Begin new sequence of lists to merge.
		 */

		firstList = i;
		listLength = length;
		continue;
	}

	if (prev) {
	    /*
	     * Add node to sequence.
	     */

	    /* ASSERT(sequence) */
	    if (type == WORD_TYPE_REFERENCE && !WORD_SEQNODE_REF(prev)) {
		WORD_SEQNODE_REF(prev) = part;
	    } else {
		WORD_SEQNODE_NEXT(prev) = part;
		prevPrev = prev;
		prev = part;
	    }
	} else {
	    /*
	     * Use part as node for now.
	     */

	    /* ASSERT(!prevPrev) */
	    /* ASSERT(!sequence) */
	    prev = part;
	}
	if (type == WORD_TYPE_SEQUENCE && WORD_SEQUENCE_CYCLIC(part)) {
	    /*
	     * Stop there, remaining parts are unreachable.
	     */

	    break;
	}
    }

    if (listLength > 0) {
	/*
	 * Add node with lists merged so far.
	 */

	Col_Word list = (partsA ? Col_ConcatListsA(i-firstList, 
		partsA+firstList) : Col_ConcatListsL(partsL, firstList, i-1));

	if (!prev) {
	    /*
	     * Return simple list.
	     */

	    /* ASSERT(!sequence) */
	    return list;
	}

	if (WORD_TYPE(prev) != WORD_TYPE_SEQUENCE_NODE) {
	    /*
	     * Ensure that previous node is a regular node.
	     */

	    prev = NewSequenceNode(prev, &sequence, prevPrev);
	}

	/*
	 * Add node to sequence.
	 */

	/* ASSERT(sequence) */
	WORD_SEQNODE_NEXT(prev) = list;
    }
    
    if (!sequence) {
	/*
	 * Return simple part.
	 *
	 * Note: this also works for empty sequences.
	 */

	/* ASSERT(!prevPrev) */
	return prev;
    }

    /* ASSERT(sequence) */
    /* ASSERT(prev) */
    if (WORD_TYPE(prev) == WORD_TYPE_SEQUENCE) {
	/*
	 * Point to the sequence root rather than the sequence word itself; this
	 * way, sequence tails may be shared across sequence words. 
	 */

	/* ASSERT(prevPrev) */
	WORD_SEQNODE_NEXT(prevPrev) = WORD_SEQUENCE_ROOT(prev);

	/*
	 * Sequences ending with a cyclic sequence are themselves cyclic.
	 */

	if (WORD_SEQUENCE_CYCLIC(prev)) {
	    WORD_SEQUENCE_CYCLIC(sequence) = 1;
	}
    }

    return sequence;
}

Col_Word
Col_NewSequence(
    size_t nbParts,		/* Number of parts. */
    const Col_Word * parts)	/* Parts of the sequence to create. */
{
    if (nbParts == 0) {return WORD_NIL;}
    return NewSequence(parts, WORD_NIL, 0, nbParts-1);
}

Col_Word
Col_NewSequenceV(
    size_t nbParts,		/* Number of parts. */
    ...)			/* Remaining arguments are parts of the sequence
				 * to create. */
{
    size_t i;
    va_list args;
    Col_Word *parts;
    
    /* 
     * Convert vararg list to array. Use alloca since a vararg list is 
     * typically small. 
     */

    parts = alloca(nbParts*sizeof(Col_Word));
    va_start(args, nbParts);
    for (i=0; i < nbParts; i++) {
	parts[i] = va_arg(args, Col_Word);
    }
    return Col_NewSequence(nbParts, parts);
}

/* - List version. */
Col_Word
Col_NewSequenceL(
    Col_Word parts,		/* List of parts to concatenate. */
    size_t first, size_t last)	/* Range to consider. */
{
    size_t length;

    /* 
     * Normalize range.
     */

    if (last < first) {
	return WORD_NIL;
    }
    length = Col_ListLength(parts);
    if (first >= length) {
	return WORD_NIL;
    }
    if (last >= length) {
	last = length-1;
    }

    return NewSequence(NULL, parts, first, last);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewReference --
 *
 *	Create a new reference word.
 *
 * Side effects:
  *	The new word.
 *
 * Side effects:
 *	Allocates memory cells.
*
 *---------------------------------------------------------------------------
 */

Col_Word
Col_NewReference()
{
    Col_Word ref;

    ref = (Col_Word) AllocCells(1);
    WORD_SET_TYPE_ID(ref, WORD_TYPE_REFERENCE);
    WORD_SYNONYM(ref) = WORD_NIL;
    WORD_REFERENCE_SOURCE(ref) = WORD_NIL;

    return ref;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_BindReference --
 *
 *	Bind the reference to a given word. If this word is itself a reference,
 *	use its source.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The reference and source words become synonyms.
 *
 *---------------------------------------------------------------------------
 */

void
Col_BindReference(
    Col_Word ref,		/* Reference to bind. */
    Col_Word source)		/* Word to bind to. */
{
    Col_Word word;

    RESOLVE_WORD(ref);

    if (WORD_TYPE(ref) != WORD_TYPE_REFERENCE) return;

    if (WORD_REFERENCE_SOURCE(ref)) {
	/*
	 * Reference is already bound.
	 */

	/* TODO: exception */
	return;
    }

    RESOLVE_WORD(source);

    /*
     * Prevent circular references.
     */

    word = source;
    while (WORD_TYPE(word) == WORD_TYPE_REFERENCE) {
	word = WORD_REFERENCE_SOURCE(word);
	if (word == ref) {
	    /* TODO: exception */
	    return;
	}
    }
    WORD_REFERENCE_SOURCE(ref) = source;
    Col_AddWordSynonym(ref, source);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_GetReference --
 *
 *	Resolve a reference, i.e. get the source.
 *
 * Results:
 *	The reference source.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_GetReference(
    Col_Word ref)		/* Reference to resolve. */
{
    RESOLVE_WORD(ref);

    return WORD_REFERENCE_SOURCE(ref);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ListElementAt --
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
Col_ListElementAt(
    Col_Word list,		/* List to get element from. */
    size_t index)		/* Element index. */
{
    Col_ListIterator it;
    
    Col_ListIterBegin(list, index, &it);
    return Col_ListIterElementAt(&it);
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
 *	Nil if count is zero or the list is nil.
 *	Else, the repetition of the source list.
 *
 * Side effects:
 *	New lists may be created.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_RepeatList(
    Col_Word list,		/* List to repeat. */
    size_t count)		/* Repetition factor. */
{
    /* Quick cases. */
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
    size_t listLength;
    if (index == 0) {
	/*
	 * Insert at begin.
	 */

	return Col_ConcatLists(list, into);
    }
    listLength = Col_ListLength(into);
    if (index >= listLength) {
	/* 
	 * Insert at end; don't pad. 
	 */

	return Col_ConcatLists(into, list);
    }

    /* 
     * General case. 
     */

    return Col_ConcatLists(Col_ConcatLists(Col_Sublist(into, 0, index-1), list), 
	    Col_Sublist(into, index, listLength-1));
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
    size_t listLength;
    if (first > last) {
	/* 
	 * No-op. 
	 */

	return list;
    }
    listLength = Col_ListLength(list);
    if (listLength == 0 || first >= listLength) {
	/*
	 * No-op.
	 */

	return list;
    } else if (first == 0) {
	/*
	 * Trim begin.
	 */

	return Col_Sublist(list, last+1, listLength-1);
    } else if (last >= listLength-1) {
	/* 
	 * Trim end. 
	 */

	return Col_Sublist(list, 0, first-1);
    }

    /* 
     * General case. 
     */

    return Col_ConcatLists(Col_Sublist(list, 0, first-1), 
	    Col_Sublist(list, last+1, listLength-1));
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
    if (first > last) {
	/* 
	 * No-op. 
	 */

	return list;
    }

    /* 
     * General case. 
     */

    return Col_ListInsert(Col_ListRemove(list, first, last), first, with);
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
 *	If index points past the end of the list, the iterator is initialized 
 *	to the end iterator (i.e. whose list field is nil), else it points 
 *	to the element within the list.
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
	 * End of list. This also handles scalars.
	 */

	it->list = WORD_NIL;

	return;
    }

    it->list = list;
    it->index = index;

    /*
     * Traversal info will be lazily computed.
     */

    it->traversal.subnode = WORD_NIL;
    it->traversal.leaf = WORD_NIL;
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
		if (WORD_LISTNODE_DEPTH(node) == MAX_ITERATOR_SUBNODE_DEPTH 
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
		if (WORD_LISTNODE_DEPTH(node) == MAX_ITERATOR_SUBNODE_DEPTH
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
 *	If the index is past the end of the list, nil, else the element.
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
	return WORD_NIL;
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

	it->list = WORD_NIL;
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

	it->traversal.leaf = WORD_NIL;
	return;
    }

    /*
     * Update traversal info.
     */

    if (nb >= WORD_VECTOR_LENGTH(it->traversal.leaf) - it->traversal.index) {
	/*
	 * Reached end of leaf.
	 */

	it->traversal.leaf = WORD_NIL;
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

/*
 *---------------------------------------------------------------------------
 *
 * PushNode --
 * PopNode --
 *
 *	Push/pop current node onto stack and update the iterator accordingly.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The iterator is updated.
 *
 *---------------------------------------------------------------------------
 */

static int
PushNode(
    Col_SequenceIterator *it)	/* The iterator to update. */
{
    /* ASSERT(it->seq) */
    /* ASSERT(it->node) */

    if (it->turtle && it->seq == WORD_SEQSTACK_SEQUENCE(it->turtle)) {
	/*
	 * Cycle detected! Mark & trim all sequences in stack as cyclic, and 
	 * clear stack.
	 */

	WORD_SEQUENCE_CYCLIC(it->seq) = 1;
	WORD_SEQNODE_NEXT(it->node) = WORD_NIL;
	while (it->stack) {
	    Col_Word stack = it->stack;
	    Col_Word seq = WORD_SEQSTACK_SEQUENCE(stack);
	    Col_Word node = WORD_SEQSTACK_NODE(stack);

	    WORD_SEQUENCE_CYCLIC(seq) = 1;
	    if (WORD_TYPE(node) == WORD_TYPE_SEQUENCE_NODE) {
		WORD_SEQNODE_NEXT(node) = WORD_NIL;
	    }

	    /*
	     * Move stack node to the free list.
	     */

	    it->stack = WORD_SEQSTACK_NEXT(stack);
	    WORD_SEQSTACK_NEXT(stack) = WORD_SEQUENCE_STACKNODES(seq);
	    WORD_SEQUENCE_STACKNODES(seq) = stack;
	}
	it->turtle = WORD_NIL;
	it->limit = 1;
	it->steps = 0;
    } else {
	/*
	 * Push current node onto stack.
	 */

	Col_Word stack = WORD_SEQUENCE_STACKNODES(it->seq);
	if (stack) {
	    /*
	     * Reuse existing stack node.
	     */

	    /* ASSERT(WORD_SEQSTACK_SEQUENCE(stack) == it->seq) */
	    WORD_SEQUENCE_STACKNODES(it->seq)
		    = WORD_SEQSTACK_NEXT(stack);
	} else {
	    /*
	     * Create new stack node.
	     */

	    stack = (Col_Word) AllocCells(1);
	    WORD_SET_TYPE_ID(stack, WORD_TYPE_SEQUENCE_STACK);
	    WORD_SEQSTACK_SEQUENCE(stack) = it->seq;
	}
	WORD_SEQSTACK_NODE(stack) = it->node;

	/*
	 * Push onto stack.
	 */

	WORD_SEQSTACK_NEXT(stack) = it->stack;
	it->stack = stack;

	/*
	 * Brent's cycle detection algorithm.
	 */

	it->steps++;
	if (it->steps == it->limit) {
	    it->steps = 0;
	    it->limit *= 2;
	    it->turtle = it->stack;
	}
    }

    /*
     * Follow reference.
     */

    if (WORD_TYPE(it->node) == WORD_TYPE_SEQUENCE_NODE) {
	/* ASSERT(WORD_SEQNODE_REF(it->node)) */
	it->node = WORD_SEQNODE_REF(it->node);
    }
    while (WORD_TYPE(it->node) == WORD_TYPE_REFERENCE) {
	it->node = WORD_REFERENCE_SOURCE(it->node);
    }

    /*
     * The stack being empty means a cycle was found.
     */

    return (it->stack ? 0 : 1);
}

static void
PopNode(
    Col_SequenceIterator *it)	/* The iterator to update. */
{
    /* ASSERT(it->seq) */
    Col_Word stack = it->stack;
    if (!stack) {
	/*
	 * End of sequence.
	 */

	it->seq = WORD_NIL;
    } else {
	/*
	 * Reverse Brent's algorithm.
	 *
	 * Note: although this step is O(n), the limit value is typically 
	 * small.
	 */

	if (it->steps == 0) {
	    it->limit /= 2;
	    it->steps = it->limit;
	    while (it->steps >= it->limit) {
		it->turtle = WORD_SEQSTACK_NEXT(it->turtle);
		it->steps--;
	    }
	} else {
	    it->steps--;
	}

	/*
	 * Move stack node to the free list.
	 */

	it->stack = WORD_SEQSTACK_NEXT(stack);
	WORD_SEQSTACK_NEXT(stack) = WORD_SEQUENCE_STACKNODES(it->seq);
	WORD_SEQUENCE_STACKNODES(it->seq) = stack;

	/*
	 * Up one level.
	 */

	it->seq = WORD_SEQSTACK_SEQUENCE(stack);
	it->node = WORD_SEQSTACK_NODE(stack);
    }
}


/*
 *---------------------------------------------------------------------------
 *
 * FindPart --
 *
 *	Update the given sequence iterator so that it points to a node with a 
 *	valid part.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If no valid part is found (e.g. when following unbound references),
 *	the iterator reaches end.
 *	The stack is updated when following reference downwards.
 *
 *---------------------------------------------------------------------------
 */

static void
FindPart(
    Col_SequenceIterator *it)	/* The iterator to update. */
{
    int cycle = 0;
    for (;;) { /* Loop until part found or end reached. */
	switch (WORD_TYPE(it->node)) {
	    case WORD_TYPE_VECTOR:
	    case WORD_TYPE_LIST:
		/*
		 * Node is a valid part.
		 */

		return;

	    case WORD_TYPE_REFERENCE:
		/*
		 * Follow reference.
		 */

		cycle += PushNode(it);
		if (cycle > 1) {
		    /*
		     * Two cycles without finding part means the cycle is empty.
		     * End of sequence.
		     */

		    it->seq = WORD_NIL;
		    return;
		}
		break;

	    case WORD_TYPE_SEQUENCE:
		/*
		 * Descend into sequence.
		 */

		it->seq = it->node;
		it->node = WORD_SEQUENCE_ROOT(it->node);
		break;

	    case WORD_TYPE_SEQUENCE_NODE:
		if (WORD_SEQNODE_LIST(it->node)) {
		    /*
		     * Node contains a valid part.
		     */

		    return;
		}

		/*
		 * Descend into reference.
		 */

		cycle += PushNode(it);
		if (cycle > 1) {
		    /*
		     * Two cycles without finding part means the cycle is empty.
		     * End of sequence.
		     */

		    it->seq = WORD_NIL;
		    return;
		}
		break;

	    case WORD_TYPE_NIL:
		/*
		 * Up one level and to the next node.
		 */

		PopNode(it);
		if (!it->seq) {
		    /*
		     * End of sequence.
		     */

		    return;
		}
		/* ASSERT(it->node) */
		it->node = WORD_SEQNODE_NEXT(it->node);
		break;
	}
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_SequenceIterBegin --
 *
 *	Initialize the sequence iterator so that it points to the first
 *	element within the sequence.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If the sequence is nil, the iterator is initialized to the end iterator
 *	(i.e. whose seq field is nil), else it points to the first element 
 *	within the sequence.
 *
 *---------------------------------------------------------------------------
 */

void
Col_SequenceIterBegin(
    Col_Word sequence,		/* Sequence to iterate over. */
    Col_SequenceIterator *it)	/* Iterator to initialize. */
{
    RESOLVE_WORD(sequence);

begin:
    switch (WORD_TYPE(sequence)) {
	case WORD_TYPE_SEQUENCE:
	    /*
	     * Regular sequence.
	     */

	    it->seq = sequence;
	    it->node = WORD_SEQUENCE_ROOT(sequence);
	    it->stack = WORD_NIL;
	    it->turtle = WORD_NIL;
	    it->limit = 1;
	    it->steps = 0;
	    FindPart(it);
	    break;

	case WORD_TYPE_VECTOR:
	case WORD_TYPE_LIST:
	    /*
	     * Vectors and lists are like single-part sequences.
	     */

	    it->seq = sequence;
	    it->node = sequence;
	    it->stack = WORD_NIL;
	    break;

	case WORD_TYPE_REFERENCE:
	    /*
	     * Follow reference (tail recurse).
	     */

	    sequence = WORD_REFERENCE_SOURCE(sequence);
	    goto begin;

	default:
	    /*
	     * End of iteration.
	     */

	    it->seq = WORD_NIL;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_SequenceIterDone --
 *
 *	Called when we're done with the given iterator.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Cleanup all iterator relative data that may have been allocated. Set
 *	the iterator to the end iterator.
 *
 *---------------------------------------------------------------------------
 */

void
Col_SequenceIterDone(
    Col_SequenceIterator *it)	/* Iterator to cleanup. */
{
    if (Col_SequenceIterEnd(it)) {
	return;
    }

    /*
     * Move stack nodes to free list.
     */

    while (it->stack) {
	Col_Word seq = WORD_SEQSTACK_SEQUENCE(it->stack);
	Col_Word next = WORD_SEQSTACK_NEXT(it->stack);
	WORD_SEQSTACK_NEXT(it->stack) = WORD_SEQUENCE_STACKNODES(seq);
	WORD_SEQUENCE_STACKNODES(seq) = it->stack;
	it->stack = next;
    }
    it->seq = WORD_NIL;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_SequenceIterPartAt --
 *
 *	Get the part designated by the iterator.
 *
 * Results:
 *	If the index is past the end of the sequence, nil, else the part (i.e.
 *	list).
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_SequenceIterPartAt(
    Col_SequenceIterator *it)	/* Iterator that points to the part. */
{
    if (Col_SequenceIterEnd(it)) {
	return WORD_NIL;
    }

    /* ASSERT(WORD_TYPE(it->node) != WORD_TYPE_REFERENCE) */
    switch (WORD_TYPE(it->node)) {
	case WORD_TYPE_VECTOR:
	case WORD_TYPE_LIST:
	    /*
	     * Node is a part itself.
	     */

	    /* ASSERT(it->seq == it->node) */
	    return it->node;

	default:
	    return WORD_SEQNODE_LIST(it->node);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_SequenceIterNext --
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
Col_SequenceIterNext(
    Col_SequenceIterator *it)	/* The iterator to move. */
{
    if (Col_SequenceIterEnd(it)) {
	return;
    }

    switch (WORD_TYPE(it->node)) {
	case WORD_TYPE_VECTOR:
	case WORD_TYPE_LIST:
	    /*
	     * End of sequence.
	     */

	    it->node = WORD_NIL;
	    break;

	case WORD_TYPE_SEQUENCE_NODE:
	    if (WORD_SEQNODE_REF(it->node)) {
		/*
		 * Descend into reference.
		 */

		PushNode(it);
	    } else {
		/*
		 * Next node.
		 */

		it->node = WORD_SEQNODE_NEXT(it->node);
	    }
	    break;
    }

    FindPart(it);
}