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
#include "colInternal.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>
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
static Col_Word		NewListWord(Col_Word root, size_t loop);
static Col_Word		NewListNode(size_t length, const Col_Word * elements,
			    int createWord);
static Col_Word		Sublist(Col_Word list, size_t first, size_t last, 
			    int createWord);
static void		GetArms(Col_Word node, Col_Word * leftPtr, 
			    Col_Word * rightPtr);
static Col_Word		ConcatLists(Col_Word left, Col_Word right, 
			    int createWord);
static Col_Word		ConcatListsA(size_t number, const Col_Word * words,
			    int createWord);
static Col_Word		ConcatListsL(Col_Word list, size_t first, size_t last,
			    int createWord);
static Col_Word		RepeatList(Col_Word list, size_t count, int createWord);
static void		UpdateTraversalInfo(Col_ListIterator *it);


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

	Col_Error(COL_ERROR, "Reference %x is already bound", ref);
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
	    Col_Error(COL_ERROR, 
		    "Binding reference %x to %x would form a circular reference chain",
		    ref, source);
	    return;
	}
    }
    WORD_REFERENCE_SOURCE(ref) = source;
    Col_AddWordSynonym(&ref, source, WORD_NIL);
}

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
 * Results:
 *	If the given length is larger than the maximum length allowed, nil.
 *	If the given length is zero, empty.
 *	Else the new word (which may be empty).
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

    /*
     * Quick cases.
     */

    if (!elements) {
	Col_Error(COL_ERROR, "Element array is NULL");
	return WORD_NIL;
    }

    if (length > VECTOR_MAX_LENGTH) {
	/*
	 * Too large.
	 */

	Col_Error(COL_ERROR, 
		"Length %u exceeds the maximum allowed value for vectors (%u)", 
		length, VECTOR_MAX_LENGTH);
	return WORD_NIL;
    }

    if (length == 0) {
	/* 
	 * Use immediate value.
	 */

	return WORD_EMPTY;
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
    memcpy(WORD_VECTOR_ELEMENTS(vector), elements, sizeof(*elements) * length);

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

    /*
     * Quick cases.
     */

    if (length > VECTOR_MAX_LENGTH) {
	/*
	 * Too large.
	 */

	Col_Error(COL_ERROR, 
		"Length %u exceeds the maximum allowed value for vectors (%u)", 
		length, VECTOR_MAX_LENGTH);
	return WORD_NIL;
    }
    if (length == 0) {
	/* 
	 * Use immediate value.
	 */

	return WORD_EMPTY;
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
    }

    return vector;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewMVector --
 *
 *	Create a new mutable vector word.
 *
 *	Note that the actual maximum length will be rounded up to fit an even
 *	number of cells. If this length is zero then take up to the maximum
 *	length.
 *
 * Results:
 *	If the given length is larger than the maximum length allowed, nil.
 *	Else the new word.
 *
 * Side effects:
 *	May allocate memory cells.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_NewMVector(
    size_t maxLength)		/* Maximum length of vector. */
{
    Col_Word mvector;		/* Resulting word in the general case. */
    size_t size;

    /*
     * Quick cases.
     */

    if (maxLength > VECTOR_MAX_LENGTH) {
	/*
	 * Too large.
	 */

	Col_Error(COL_ERROR, 
		"Length %u exceeds the maximum allowed value for vectors (%u)", 
		maxLength, VECTOR_MAX_LENGTH);
	return WORD_NIL;
    }

    /*
     * Create a new vector word.
     */

    if (maxLength == 0) {
	/*
	 * Use maximum size allowed.
	 */

	size = AVAILABLE_CELLS;
    } else {
	size = NB_CELLS(WORD_HEADER_SIZE + (maxLength * sizeof(Col_Word)));
    }

    mvector = (Col_Word) AllocCells(size);
    WORD_SET_TYPE_ID(mvector, WORD_TYPE_MVECTOR);
    WORD_SYNONYM(mvector) = WORD_NIL;
    WORD_MVECTOR_SIZE(mvector) = (unsigned char) size;
    WORD_VECTOR_LENGTH(mvector) = (unsigned short) 0;

    return mvector;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_FreezeMVector --
 *
 *	Turn a mutable vector immutable. If an immutable vector is given,
 *	does nothing.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The mutable vector becomes immutable.
 *
 *---------------------------------------------------------------------------
 */

void
Col_FreezeMVector(
    Col_Word mvector)		/* The mutable vector to freeze. */
{
    RESOLVE_WORD(mvector);

    switch (WORD_TYPE(mvector)) {
	case WORD_TYPE_VECTOR:
	    /*
	     * No-op.
	     */

	    break;

	case WORD_TYPE_MVECTOR:
	    /*
	     * Simply change type ID. Don't mark extraneous cells, they will
	     * be collected during GC.
	     */

	    WORD_SET_TYPE_ID(mvector, WORD_TYPE_VECTOR);
	    break;

	default:
	    Col_Error(COL_ERROR, "%x is not a mutable vector", mvector);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_MVectorSetLength --
 *
 *	Resize the mutable vector.
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
Col_MVectorSetLength(
    Col_Word mvector,		/* Mutable vector to resize. */
    size_t length)		/* New length. */
{
    RESOLVE_WORD(mvector);

    if (WORD_TYPE(mvector) != WORD_TYPE_MVECTOR) {
	/*
	 * Invalid type.
	 */

	Col_Error(COL_ERROR, "%x is not a mutable vector", mvector);
	return;
    }

    if (length > VECTOR_MAX_LENGTH) {
	/*
	 * Too large.
	 */

	Col_Error(COL_ERROR, 
		"Vector length %u exceeds the maximum allowed value (%u)", 
		length, VECTOR_MAX_LENGTH);
	WORD_VECTOR_LENGTH(mvector) = 0;
	return;
    }

    if (length > WORD_VECTOR_LENGTH(mvector)) {
	/*
	 * Initialize elements to nil.
	 */

	memset(WORD_VECTOR_ELEMENTS(mvector)+WORD_VECTOR_LENGTH(mvector),
	    0, (length-WORD_VECTOR_LENGTH(mvector)) * sizeof(Col_Word));
    }
    WORD_VECTOR_LENGTH(mvector) = (unsigned short) length;
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
    Col_Word root,		/* List root. */
    size_t loop)		/* Loop length (zero for non-cyclic). */
{
    Col_Word list;
    switch (WORD_TYPE(root)) {
	case WORD_TYPE_VECTOR:
	case WORD_TYPE_VOID_LIST:
	    if (!loop) {
		return root;
	    }
	    break;

	case WORD_TYPE_LIST:
	    if (WORD_LIST_LOOP(root) == loop) {
		return root;
	    }
	    root = WORD_LIST_ROOT(root);
	    break;
    }

    list = (Col_Word) AllocCells(1);
    WORD_SET_TYPE_ID(list, WORD_TYPE_LIST);
    WORD_SYNONYM(list) = WORD_NIL;
    WORD_LIST_ROOT(list) = root;
    WORD_LIST_LOOP(list) = loop;

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
    return NewListNode(length, elements, 1);
}

static Col_Word
NewListNode(
    size_t length,		/* Length of below array. */
    const Col_Word * elements,	/* Array of words to populate list with. */
    int createWord)		/* Whether to create the list word. */
{
    size_t half=0;		/* Index of the split point. */

    if (length == 0) {
	/*
	 * Empty list.
	 */

	return WORD_EMPTY;
    }

    if (!elements) {
	/*
	 * Void list.
	 */

	if (length <= VOID_LIST_MAX_LENGTH) {
	    return WORD_VOID_LIST_NEW(length);
	}

	/*
	 * Length is too long to fit within an immediate value. Split.
	 */

	half = length/2;
	return ConcatLists(NewListNode(half, NULL, 0),
		NewListNode(length-half, NULL, 0), createWord);
    }

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
    return ConcatLists(NewListNode(half, elements, 0),
	    NewListNode(length-half, elements+half, 0), createWord);
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
	case WORD_TYPE_VOID_LIST:
	    return WORD_VOID_LIST_LENGTH(list);

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
    const Col_Word *elements, 
    size_t length,
    Col_ClientData clientData) 
{
    MergeChunksInfo *info = (MergeChunksInfo *) clientData;

    if (elements) {
	if (WORD_TYPE(info->vector) == WORD_TYPE_VOID_LIST) {
	    /*
	     * Allocate vector and initialize elements to nil.
	     */

	    size_t vectorLength = WORD_VOID_LIST_LENGTH(info->vector);
	    /* ASSERT(vectorLength < VECTOR_MAX_LENGTH) */
	    info->vector = (Col_Word) AllocCells(NB_CELLS(WORD_HEADER_SIZE 
		    + (vectorLength * sizeof(Col_Word))));
	    WORD_SET_TYPE_ID(info->vector, WORD_TYPE_VECTOR);
	    WORD_SYNONYM(info->vector) = WORD_NIL;
	    WORD_VECTOR_LENGTH(info->vector) = (unsigned short) vectorLength;
	    memset(WORD_VECTOR_ELEMENTS(info->vector), 0, 
		    sizeof(Col_Word) * vectorLength);
	}

	/* 
	 * Set elements. 
	 */

	memcpy(WORD_VECTOR_ELEMENTS(info->vector)+info->length, elements, 
		sizeof(*elements) * length);
    }
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
    RESOLVE_WORD(list);

    return Sublist(list, first, last, 1);
}

static Col_Word
Sublist(
    Col_Word list,		/* The list to extract the sublist from. */
    size_t first, size_t last,	/* Range of sublist. */
    int createWord)		/* Whether to create the list word. */
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

    /*
     * Additional type checking.
     */

    switch (WORD_TYPE(list)) {
	case WORD_TYPE_MVECTOR:
	case WORD_TYPE_MLIST:
	    /*
	     * Can't get sublist of mutable list/vector.
	     */

	    Col_Error(COL_ERROR, "%x is not an immutable list", list);
	    return WORD_NIL;
    }

    if (last < first) {
	/*
	 * Invalid range.
	 */

	return WORD_EMPTY;
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

		return Sublist(list, first, last, createWord);
	    } else if (first > listLength-loop) {
		/* 
		 * Leading slice plus remainder.
		 */

		return ConcatLists(Sublist(list, first, listLength-1, 0), 
			Sublist(list, listLength, last, 0), createWord);
	    } else {
		/*
		 * Repeat loop.
		 */

		/* ASSERT(first == listLength-loop) */
		size_t count = (last - first) / loop;
		/* ASSERT(count > 0) */
		if (last >= first + (loop * count)) {
		    /*
		     * Loop repeat + trailing slice.
		     */

		    /* ASSERT(last - length < listLength) */
		    return ConcatLists(RepeatList(Sublist(list, first, 
			    listLength-1, 0), count, 0), Sublist(list, first, 
			    last - (loop * count), 0), createWord);
		} else {
		    /*
		     * Simple loop repeat.
		     */

		    return RepeatList(Sublist(list, first, listLength-1, 0), 
			    count, createWord);
		}
	    }
	}

	/*
	 * This handles empty as well.
	 */

	return WORD_EMPTY;
    }

    if (last >= listLength) {
	if (loop) {
	    /*
	     * Append loop part.
	     */

	    return ConcatLists(Sublist(list, first, listLength-1, 0), 
		    Sublist(list, listLength, last, 0), createWord);
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

	return list;
    }

    if (WORD_TYPE(list) == WORD_TYPE_LIST) {
	list = WORD_LIST_ROOT(list);
    }
    switch (WORD_TYPE(list)) {
	case WORD_TYPE_VOID_LIST:
	    /*
	     * Use immediate value.
	     */

	    /* ASSERT(length <= VOID_LIST_MAX_LENGTH) */
	    return WORD_VOID_LIST_NEW(length);

	case WORD_TYPE_SUBLIST:
	    /* 
	     * Point to original source. 
	     */

	    return Sublist(WORD_SUBLIST_SOURCE(list), 
		    WORD_SUBLIST_FIRST(list)+first, 
		    WORD_SUBLIST_FIRST(list)+last, createWord);

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

		return Sublist(WORD_CONCATLIST_LEFT(list), first, last, 
			createWord);
	    } else if (first >= leftLength) {
		/*
		 * Right arm is a superset of sublist. 
		 */

		return Sublist(WORD_CONCATLIST_RIGHT(list), first-leftLength, 
			last-leftLength, createWord);
	    }
	    depth = WORD_LISTNODE_DEPTH(list);
	}
    }

    /* 
     * Build a vector for short sublists.
     */

    if (length <= MAX_SHORT_LIST_LENGTH) {
	MergeChunksInfo info; 

	info.length = 0;
	info.vector = WORD_VOID_LIST_NEW(length);
	Col_TraverseListChunks(list, first, length, MergeChunksProc, &info, 
		NULL);

	return info.vector;
    }

    /* 
     * General case: build a sublist node.
     */

    sublist = (Col_Word) AllocCells(1);
    WORD_SET_TYPE_ID(sublist, WORD_TYPE_SUBLIST);
    WORD_LISTNODE_DEPTH(sublist) = depth;
    WORD_SUBLIST_SOURCE(sublist) = list;
    WORD_SUBLIST_FIRST(sublist) = first;
    WORD_SUBLIST_LAST(sublist) = last;

    return (createWord ? NewListWord(sublist, 0) : sublist);
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
    /* ASSERT(WORD_LISTNODE_DEPTH(node) >= 1) */
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
		WORD_SUBLIST_FIRST(node), leftLength-1, 0);
	*rightPtr = Sublist(WORD_CONCATLIST_RIGHT(source), 0, 
		WORD_SUBLIST_LAST(node)-leftLength, 0);
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
    RESOLVE_WORD(left);
    RESOLVE_WORD(right);

    return ConcatLists(left, right, 1);
}

static Col_Word
ConcatLists(
    Col_Word left,		/* Left part. */
    Col_Word right,		/* Right part. */
    int createWord)		/* Whether to create the list word. */
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

    /*
     * Additional type checking.
     */

    switch (WORD_TYPE(left)) {
	case WORD_TYPE_MVECTOR:
	case WORD_TYPE_MLIST:
	    /*
	     * Can't get sublist of mutable list/vector.
	     */

	    Col_Error(COL_ERROR, "%x is not an immutable list", left);
	    return WORD_NIL;

	case WORD_TYPE_LIST:
	    if (WORD_LIST_LOOP(left)) {
		/*
		 * Left is cyclic, discard right.
		 */

		return left;
	    }
	    break;
    }

    switch (WORD_TYPE(right)) {
	case WORD_TYPE_MVECTOR:
	case WORD_TYPE_MLIST:
	    /*
	     * Can't get sublist of mutable list/vector.
	     */

	    Col_Error(COL_ERROR, "%x is not an immutable list", right);
	    return WORD_NIL;

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

    /* 
     * Handle quick cases and get input node depths. 
     */

    if (leftLength == 0) {
	/* 
	 * Concat is a no-op on right. 
	 */

	return right;
    } else if (rightLength == 0) {
	/* 
	 * Concat is a no-op on left.
	 */

	return left;
    }

    if (WORD_TYPE(left) == WORD_TYPE_LIST) {
	left = WORD_LIST_ROOT(left);
    }
    if (WORD_TYPE(right) == WORD_TYPE_LIST) {
	right = WORD_LIST_ROOT(right);
    }

    switch (WORD_TYPE(left)) {
	case WORD_TYPE_VOID_LIST:
	    if (WORD_TYPE(right) == WORD_TYPE_VOID_LIST 
		    && leftLength + rightLength <= VOID_LIST_MAX_LENGTH) {
		/*
		 * Merge void lists.
		 */

		return WORD_VOID_LIST_NEW(leftLength + rightLength);
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

		concatNode = Sublist(WORD_SUBLIST_SOURCE(left), 
			WORD_SUBLIST_FIRST(left), WORD_SUBLIST_LAST(right), 0);
		return (createWord ? NewListWord(concatNode, loop) 
			: createWord);
	    }
	    leftDepth = WORD_LISTNODE_DEPTH(left);
	    break;

	case WORD_TYPE_CONCATLIST:
	    leftDepth = WORD_LISTNODE_DEPTH(left);
	    break;
    }
    switch (WORD_TYPE(right)) {
	case WORD_TYPE_SUBLIST:
	case WORD_TYPE_CONCATLIST:
	    rightDepth = WORD_LISTNODE_DEPTH(right);
	    break;
    }

    /* 
     * Build a vector for short sublists.
     */

    if (leftLength + rightLength <= MAX_SHORT_LIST_LENGTH) {
	MergeChunksInfo info; 

	info.length = 0;
	info.vector = WORD_VOID_LIST_NEW(leftLength + rightLength);
	Col_TraverseListChunks(left, 0, leftLength, MergeChunksProc, &info, 
		NULL);
	Col_TraverseListChunks(right, 0, rightLength, MergeChunksProc, &info, 
		NULL);

	return (createWord ? NewListWord(info.vector, loop) : info.vector);
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
	    concatNode = ConcatLists(ConcatLists(left1, left21, 0), 
		    ConcatLists(left22, right, 0), 0);
	    return (createWord ? NewListWord(concatNode, loop) : concatNode);
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

	    concatNode = ConcatLists(left1, ConcatLists(left2, right, 0), 0);
	    return (createWord ? NewListWord(concatNode, loop) : concatNode);
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
	    concatNode = ConcatLists(ConcatLists(left, right11, 0),
		    ConcatLists(right12, right2, 0), 0);
	    return (createWord ? NewListWord(concatNode, loop) : concatNode);
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

	    concatNode = ConcatLists(ConcatLists(left, right1, 0), right2, 0);
	    return (createWord ? NewListWord(concatNode, loop) : concatNode);
	}
    }

    /* 
     * General case: build a concat node.
     */

    concatNode = (Col_Word) AllocCells(1);
    WORD_SET_TYPE_ID(concatNode, WORD_TYPE_CONCATLIST);
    WORD_LISTNODE_DEPTH(concatNode) 
	    = (leftDepth>rightDepth?leftDepth:rightDepth) + 1;
    WORD_CONCATLIST_LENGTH(concatNode) = leftLength + rightLength;
    WORD_CONCATLIST_LEFT_LENGTH(concatNode) 
	    = (leftLength<=UCHAR_MAX)?(unsigned char) leftLength:0;
    WORD_CONCATLIST_LEFT(concatNode) = left;
    WORD_CONCATLIST_RIGHT(concatNode) = right;

    return (createWord ? NewListWord(concatNode, loop) : concatNode);
}

/* - Array version. */
Col_Word
Col_ConcatListsA(
    size_t number,		/* Number of lists in below array. */
    const Col_Word * lists)	/* Lists to concatenate in order. */
{
    return ConcatListsA(number, lists, 1);
}

static Col_Word
ConcatListsA(
    size_t number,		/* Number of lists in below array. */
    const Col_Word * lists,	/* Lists to concatenate in order. */
    int createWord)		/* Whether to create the list word. */
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
	return ConcatLists(lists[0], lists[1], 
		Col_ListLoopLength(lists[1]) ? 1 : createWord);
    }

    /* 
     * Split array and concatenate both halves. This should result in a well 
     * balanced tree. 
     */

    half = number/2;
    l1 = ConcatListsA(half, lists, 0);
    if (Col_ListLoopLength(l1)) {
	/*
	 * Skip remaining args.
	 */

	return l1;
    }
    l2 = ConcatListsA(number-half, lists+half, 0);
    return ConcatLists(l1, l2, Col_ListLoopLength(l2) ? 1 : createWord);
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
    return ConcatListsL(list, first, last, 1);
}

static Col_Word
ConcatListsL(
    Col_Word list,		/* List of lists to concatenate. */
    size_t first, size_t last,	/* Range to consider. */
    int createWord)		/* Whether to create the list word. */
{
    size_t number = last-first+1, half;
    Col_Word l1, l2;

    /* 
     * Quick cases.
     */

    if (number == 0) {return WORD_NIL;}
    if (number == 1) {return Col_ListGetAt(list, first);}
    if (number == 2) {
	Col_ListIterator it;

	Col_ListIterBegin(list, first, &it);
	l1 = Col_ListIterElementAt(&it);
	if (Col_ListLoopLength(l1)) {
	    return l1;
	}
	Col_ListIterNext(&it);
	l2 = Col_ListIterElementAt(&it);
	return ConcatLists(l1, l2, Col_ListLoopLength(l2) ? 1 : createWord);
    }

    /* 
     * Split range and concatenate both halves. This should result in a well 
     * balanced tree. 
     */

    half = number/2;
    l1 = ConcatListsL(list, first, first+half-1, 0);
    if (Col_ListLoopLength(l1)) {
	/*
	 * Skip remaining args.
	 */

	return l1;
    }
    l2 = ConcatListsL(list, first+half, last, 0);
    return ConcatLists(l1, l2, Col_ListLoopLength(l2) ? 1 : createWord);
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

    RESOLVE_WORD(list);

    switch (WORD_TYPE(list)) {
	case WORD_TYPE_VOID_LIST:
	    length = WORD_VOID_LIST_LENGTH(list);
	    if (length == 0) {
		/*
		 * No-op.
		 */

		return WORD_EMPTY;
	    }
	    return NewListWord(list, length);

	case WORD_TYPE_VECTOR:
	    length = WORD_VECTOR_LENGTH(list);
	    if (length == 0) {
		/*
		 * No-op.
		 */

		return WORD_EMPTY;
	    }
	    return NewListWord(list, length);

	case WORD_TYPE_LIST:
	    if (WORD_LIST_LOOP(list)) {
		/*
		 * List is already cyclic.
		 */

		return list;
	    }
	    length = Col_ListLength(list);
	    if (length == 0) {
		/*
		 * No-op.
		 */

		return WORD_EMPTY;
	    }
	    return NewListWord(WORD_LIST_ROOT(list), length);

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
    RESOLVE_WORD(list);

    switch (WORD_TYPE(list)) {
	case WORD_TYPE_VOID_LIST:
	case WORD_TYPE_VECTOR:
	case WORD_TYPE_MVECTOR:
	    return 0;

	case WORD_TYPE_LIST:
	case WORD_TYPE_MLIST:
	    return WORD_LIST_LOOP(list);

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
    Col_Word mlist = (Col_Word) AllocCells(1);
    WORD_SET_TYPE_ID(mlist, WORD_TYPE_MLIST);
    WORD_SYNONYM(mlist) = WORD_NIL;
    WORD_LIST_ROOT(mlist) = WORD_NIL;
    WORD_LIST_LOOP(mlist) = 0;

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
    RESOLVE_WORD(mlist);

    switch (WORD_TYPE(mlist)) {
	case WORD_TYPE_LIST:
	    /*
	     * No-op.
	     */

	    break;

	case WORD_TYPE_MLIST:
	    /* TODO */
	    break;

	default:
	    Col_Error(COL_ERROR, "%x is not a mutable list", mlist);
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
    RESOLVE_WORD(mlist);

    if (WORD_TYPE(mlist) != WORD_TYPE_MLIST) {
	/*
	 * Invalid type.
	 */

	Col_Error(COL_ERROR, "%x is not a mutable list", mlist);
	return;
    }

    /* TODO */
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_MListLoop --
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
Col_MListLoop(
    Col_Word mlist,		/* The mutable list to loop. */
    size_t length)		/* Length of the terminal loop. */

{
    RESOLVE_WORD(mlist);

    /* TODO */
    return;
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
    RESOLVE_WORD(mlist);

    /* TODO */
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
    RESOLVE_WORD(into);

    /* TODO */
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
    RESOLVE_WORD(mlist);

    /* TODO */
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
    RESOLVE_WORD(mlist);

    /* TODO */
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ListGetAt --
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
Col_ListGetAt(
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

    RESOLVE_WORD(list);

    switch (WORD_TYPE(list)) {
	case WORD_TYPE_VOID_LIST:
	case WORD_TYPE_VECTOR:
	    listRoot = list;
	    break;

	case WORD_TYPE_LIST:
	    if (WORD_LIST_LOOP(list)) {
		/*
		 * List is cyclic, won't repeat.
		 */

		return list;
	    }
	    break;

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

	return WORD_EMPTY;
    }

    return RepeatList(listRoot, count, 1);
}

static Col_Word
RepeatList(
    Col_Word list,		/* The list to repeat. */
    size_t count,		/* Repetition factor. */
    int createWord)		/* Whether to create the list word. */
{
    /* Quick cases. */
    if (count == 0) {return WORD_NIL;}
    if (count == 1) {return list;}
    if (count == 2) {return ConcatLists(list, list, createWord);}

    if (count & 1) {
	/* Odd.*/
	return ConcatLists(list, RepeatList(list, count-1, 0), createWord);
    } else {
	/* Even. */
	return RepeatList(ConcatLists(list, list, 0), count>>1, createWord);
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
    size_t length, loop, listLoop;
    Col_Word listRoot;

    if (index == 0) {
	/*
	 * Insert at beginning.
	 */

	return Col_ConcatLists(list, into);
    }

    if (Col_ListLength(list) == 0) {
	/*
	 * No-op.
	 */

	return into;
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
	loop += Col_ListLength(list);
    }

    listRoot = ConcatLists(Sublist(into, 0, index-1, 0), list, 0);

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

	listRoot = ConcatLists(listRoot, Sublist(into, index, length-1, 0), 0);
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

    listRoot = WORD_EMPTY;
    if (first > 0) {
	/*
	 * Keep beginning of list.
	 */

	listRoot = Sublist(list, 0, first-1, 0);
    }

    if (last < first) {
	/*
	 * Keep inner part of loop (shorten it as it's already part of listRoot,
	 * see above).
	 */

	/* ASSERT(loop > 0) */
	/* ASSERT(first > length-loop) */
	/* ASSERT(last >= length-loop) */
	loop = first-last-1;
    } else {
	if (last < length-1) {
	    /* 
	     * Keep end of list.
	     */

	    listRoot = ConcatLists(listRoot, Sublist(list, last+1, length-1, 
		    0), 0);
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

	    listRoot = ConcatLists(listRoot, Sublist(list, length-loop, last, 
		    0), 0);
	}
    }

    /* ASSERT(listRoot != WORD_EMPTY) */
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

    if (first > length) {
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
	 * Start is past the end of the list. This handles empty as well.
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
	case WORD_TYPE_VOID_LIST:
	    if (lengthPtr) *lengthPtr = max;
	    return proc(NULL, max, clientData);

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
    size_t length;

    RESOLVE_WORD(list);

    length = Col_ListLength(list);
    if (index >= length) {
	size_t loop = Col_ListLoopLength(list);
	if (!loop) {
	    /*
	     * End of list.
	     */

	    it->list = WORD_NIL;
	    return;
	}

	/*
	 * Cyclic list. Normalize index.
	 */

	index = (index - (length-loop)) % loop + (length-loop);
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
	    case WORD_TYPE_VOID_LIST:
	    case WORD_TYPE_VECTOR:
		/*
		 * Void lists and Flat vectors are directly addressable. 
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
	/*
	 * No such element.
	 */

	Col_Error(COL_ERROR, "Invalid list iterator");
	return WORD_NIL;
    }

    if (!it->traversal.leaf) {
	UpdateTraversalInfo(it);
    }

    if (WORD_TYPE(it->traversal.leaf) == WORD_TYPE_VOID_LIST) {
	return WORD_NIL;
    } else {
	/* ASSERT(WORD_TYPE(it->traversal.leaf) == WORD_TYPE_VECTOR) */
	return WORD_VECTOR_ELEMENTS(it->traversal.leaf)[it->traversal.index];
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
    size_t length;

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
	    return;
	}

	/*
	 * Cyclic list.
	 */

	if (it->index < length-loop) {
	    /*
	     * Currently before loop, forward into loop.
	     */

	    nb -= (length-loop) - it->index;
	    it->index = (length-loop) + (nb % loop);
	} else {
	    /*
	     * Currently within loop.
	     */

	    nb %= loop;
	    if (it->index >= length-nb) {
		/*
		 * Loop backward.
		 */

		it->index -= loop-nb;
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

    if (WORD_TYPE(it->traversal.leaf) == WORD_TYPE_VOID_LIST) {
	if (nb >= WORD_VOID_LIST_LENGTH(it->traversal.leaf) 
		- it->traversal.index) {
	    /*
	     * Reached end of leaf.
	     */

	    it->traversal.leaf = WORD_NIL;
	    return;
	}
    } else {
	/* ASSERT(WORD_TYPE(it->traversal.leaf) == WORD_TYPE_VECTOR) */
	if (nb >= WORD_VECTOR_LENGTH(it->traversal.leaf) 
		- it->traversal.index) {
	    /*
	     * Reached end of leaf.
	     */

	    it->traversal.leaf = WORD_NIL;
	    return;
	}
    }

    /*
     * Go forward.
     */

    it->traversal.index += nb;
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
