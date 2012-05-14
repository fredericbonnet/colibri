/*
 * Header: colList.h
 *
 *	This header file defines the list handling features of Colibri.
 *
 *	Lists are a collection datatype that allows for fast insertion, 
 *	extraction and composition of other lists. Internally they use 
 *	self-balanced binary trees, like ropes, except that they use vectors
 *	as basic containers instead of character arrays.
 *
 *	They come in both immutable and mutable forms :
 *	
 *	- Immutable lists can be composed of immutable vectors and lists. 
 *	  Immutable vectors can themselves be used in place of immutable lists.
 *
 *	- Mutable lists can be composed of either mutable or immutable lists and
 *	  vectors. They can be "frozen" and turned into immutable versions.
 *	  Mutable vectors *cannot* be used in place of mutable lists, as the 
 *	  latter can grow indefinitely whereas the former have a maximum length 
 *	  set at creation time.
 *
 * See also:
 *	<colList.c>, <colVector.h>
 */

#ifndef _COLIBRI_LIST
#define _COLIBRI_LIST

#include <stddef.h> /* For size_t */
#include <stdarg.h> /* For variadic procs */


/*
================================================================================
Section: Immutable Lists
================================================================================
*/

/****************************************************************************
 * Group: Immutable List Creation
 *
 * Declarations:
 *	<Col_NewList>
 ****************************************************************************/

EXTERN Col_Word		Col_NewList(size_t length, 
			    const Col_Word * elements);


/****************************************************************************
 * Group: Immutable List Accessors
 *
 * Declarations:
 *	<Col_ListLength>, <Col_ListLoopLength>, <Col_ListAt>
 ****************************************************************************/

EXTERN size_t		Col_ListLength(Col_Word list);
EXTERN size_t		Col_ListLoopLength(Col_Word list);
EXTERN Col_Word		Col_ListAt(Col_Word list, size_t index);


/****************************************************************************
 * Group: Immutable List Operations
 *
 * Declarations:
 *	<Col_Sublist>, <Col_ConcatLists>, <Col_ConcatListsA>,
 *	<Col_ConcatListsNV>, <Col_RepeatList>, <Col_CircularList>, 
 *	<Col_ListInsert>, <Col_ListRemove>, <Col_ListReplace>
 *	
 ****************************************************************************/

EXTERN Col_Word		Col_Sublist(Col_Word list, size_t first, size_t last);
EXTERN Col_Word		Col_ConcatLists(Col_Word left, Col_Word right);
EXTERN Col_Word		Col_ConcatListsA(size_t number, const Col_Word * words);
EXTERN Col_Word		Col_ConcatListsNV(size_t number, ...);
EXTERN Col_Word		Col_RepeatList(Col_Word list, size_t count);
EXTERN Col_Word		Col_CircularList(Col_Word core);
EXTERN Col_Word		Col_ListInsert(Col_Word into, size_t index, 
			    Col_Word list);
EXTERN Col_Word		Col_ListRemove(Col_Word list, size_t first, 
			    size_t last);
EXTERN Col_Word		Col_ListReplace(Col_Word list, size_t first, 
			    size_t last, Col_Word with);

/*---------------------------------------------------------------------------
 * Macro: Col_ConcatListsV
 *
 *	Variadic macro version of <Col_ConcatListsNV> that deduces its number
 *	of arguments automatically.
 *
 * Arguments:
 *	List of lists to concatenate.
 *
 * See also:
 *	<COL_ARGCOUNT>
 *---------------------------------------------------------------------------*/

#define Col_ConcatListsV(...) \
    Col_ConcatListsNV(COL_ARGCOUNT(__VA_ARGS__),__VA_ARGS__)


/****************************************************************************
 * Group: Immutable List Traversal
 *
 * Declarations:
 *	<Col_TraverseListChunksN>, <Col_TraverseListChunks>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Typedef: Col_ListChunksTraverseProc
 *
 *	Function signature of list traversal procs.
 *
 * Arguments:
 *	index		- List-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks.
 *	chunks		- Array of chunks. When chunk is NULL, means the
 *			  index is past the end of the traversed list. When 
 *			  chunk is <COL_LISTCHUNK_VOID>, means the traversed 
 *			  list is a void list.
 *	clientData	- Opaque client data. Same value as passed to 
 *			  <Col_TraverseListChunks> procedure family.
 *
 * Returns:
 *	If non-zero, interrupts the traversal.
 *
 * See also: 
 *	<Col_TraverseListChunksN>, <Col_TraverseListChunks>
 *---------------------------------------------------------------------------*/

typedef int (Col_ListChunksTraverseProc) (size_t index, size_t length, 
	size_t number, const Col_Word **chunks, Col_ClientData clientData);

/*---------------------------------------------------------------------------
 * Constant: COL_LISTCHUNK_VOID
 *
 *	Value passed as chunk pointer to list traversal procs when traversing
 *	void lists.
 *
 * See also: 
 *	<Col_ListChunksTraverseProc>
 *---------------------------------------------------------------------------*/

#define COL_LISTCHUNK_VOID \
    ((Col_Word *)-1)

EXTERN int		Col_TraverseListChunksN(size_t number, Col_Word *lists, 
			    size_t start, size_t max, 
			    Col_ListChunksTraverseProc *proc, 
			    Col_ClientData clientData, size_t *lengthPtr);
EXTERN int		Col_TraverseListChunks(Col_Word list, size_t start, 
			    size_t max, int reverse, 
			    Col_ListChunksTraverseProc *proc, 
			    Col_ClientData clientData, size_t *lengthPtr);


/****************************************************************************
 * Group: Immutable List Iteration
 *
 * Declarations: 
 *	<Col_ListIterBegin>, <Col_ListIterFirst>, <Col_ListIterLast>, 
 *	<Col_ListIterArray>, <Col_ListIterCompare>, <Col_ListIterMoveTo>, 
 *	<Col_ListIterForward>, <Col_ListIterBackward>, 
 *	<ColListIterUpdateTraversalInfo>
 *
 * Note:
 *	Works with mutable or immutable lists and vectors, however modifying a 
 *	mutable list during iteration results in undefined behavior.
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Typedef: ColListIterLeafAtProc
 *
 *	Helper for list iterators to access elements in leaves.
 *
 * Arguments:
 *	leaf	- Leaf node.
 *	index	- Leaf-relative index of element.
 *
 * Result:
 *	Element at given index.
 *
 * See also:
 *	<ColListIterator>
 *---------------------------------------------------------------------------*/

typedef Col_Word (ColListIterLeafAtProc) (Col_Word leaf, size_t index);

/*---------------------------------------------------------------------------
 * Internal Typedef: ColListIterator
 *
 *	Internal implementation of list iterators.
 *
 * Fields:
 *	list		- List being iterated. If nil, use array iterator mode.
 *	length		- List length.
 *	index		- Current position.
 *	traversal	- Traversal info:
 *	traversal.array	- Traversal info in string mode.
 *	traversal.list	- Traversal info in list mode.
 *
 * Array mode fields:
 *	begin	- Beginning of array.
 *	current	- Current position.
 *
 * List mode fields:
 *	subnode		- Traversed subnode.
 *	first, last	- Range of validity.
 *	offset		- Index offset wrt. root.
 *	leaf		- Leaf (deepest) node.
 *	index		- Index within leaf.
 *	proc		- Access proc within leaf.
 *
 * See also:
 *	<Col_ListIterator>
 *---------------------------------------------------------------------------*/

typedef struct ColListIterator {
    Col_Word list;
    size_t length;
    size_t index;
    union {
	struct {
	    const Col_Word *begin, *current;
	} array;
       struct {
	    Col_Word subnode;
	    size_t first, last;
	    size_t offset;
	    Col_Word leaf;
	    size_t index;
	    ColListIterLeafAtProc *proc;
	} list;
    } traversal;
} ColListIterator;

/*---------------------------------------------------------------------------
 * Typedef: Col_ListIterator
 *
 *	List iterator. Encapsulates the necessary info to iterate & access list
 *	data transparently.
 *
 * Note:
 *	Datatype is opaque. Fields should not be accessed by client code.
 *
 *	Each iterator takes 10 words on the stack.
 *---------------------------------------------------------------------------*/

typedef ColListIterator Col_ListIterator;

/*---------------------------------------------------------------------------
 * Internal Variable: colListIterNull
 *
 *	Static variable for list iterator initialization.
 *
 * See also:
 *	<COL_LISTITER_NULL>
 *---------------------------------------------------------------------------*/

static const Col_ListIterator colListIterNull = {WORD_NIL,0,0,{NULL}};

/*---------------------------------------------------------------------------
 * Constant: COL_LISTITER_NULL
 *
 *	Static initializer for null list iterators.
 *
 * See also: 
 *	<Col_ListIterator>, <Col_ListIterNull>
 *---------------------------------------------------------------------------*/

#define COL_LISTITER_NULL	colListIterNull

/*---------------------------------------------------------------------------
 * Macro: Col_ListIterNull
 *
 *	Test whether iterator is null (e.g. it has been set to 
 *	<COL_LISTITER_NULL>). This uninitialized states renders it unusable for 
 *	any call. Use with caution.
 *
 * Argument:
 *	it	- The iterator to test. (Caution: evaluated several times during
 *		  macro expansion)
 *
 * Result:
 *	Non-zero if iterator is null.
 *
 * See also: 
 *	<Col_ListIterator>, <COL_LISTITER_NULL>
 *---------------------------------------------------------------------------*/

#define Col_ListIterNull(it) \
    ((it)->list == WORD_NIL && (it)->traversal.array.begin == NULL)

/*---------------------------------------------------------------------------
 * Macro: Col_ListIterList
 *
 *	Get list for iterator.
 *
 * Argument:
 *	it	- The iterator to get list for.
 *
 * Result:
 *	The list, or <WORD_NIL> if iterator was initialized with
 *	<Col_ListIterArray>. Undefined if at end.
 *
 * See also: 
 *	<Col_ListIterator>, <Col_ListIterEnd>
 *---------------------------------------------------------------------------*/

#define Col_ListIterList(it) \
    ((it)->list)

/*---------------------------------------------------------------------------
 * Macro: Col_ListIterLength
 *
 *	Get length of the iterated sequence.
 *
 * Argument:
 *	it	- The iterator to get length for.
 *
 * Result:
 *	Length of iterated sequence.
 *
 * See also: 
 *	<Col_ListIterator>
 *---------------------------------------------------------------------------*/

#define Col_ListIterLength(it) \
    ((it)->length)

/*---------------------------------------------------------------------------
 * Macro: Col_ListIterIndex
 *
 *	Get current index within list for iterator.
 *
 * Argument:
 *	it	- The iterator to get index for.
 *
 * Result:
 *	Current index. Undefined if at end.
 *
 * See also: 
 *	<Col_ListIterator>
 *---------------------------------------------------------------------------*/

#define Col_ListIterIndex(it) \
    ((it)->index)

/*---------------------------------------------------------------------------
 * Macro: Col_ListIterAt
 *
 *	Get current list element for iterator. Undefined if at end.
 *
 * Argument:
 *	it	- The iterator to get element for. (Caution: evaluated several
 *		  times during macro expansion)
 *
 * Result:
 *	Current element. Undefined if at end.
 *
 * Side effects:
 *	The argument is referenced several times by the macro. Make sure to
 *	avoid any side effect.
 *
 * See also: 
 *	<Col_ListIterator>
 *---------------------------------------------------------------------------*/

#define Col_ListIterAt(it) \
    ((it)->list \
	? (!(it)->traversal.list.leaf?ColListIterUpdateTraversalInfo(it),0:0,(it)->traversal.list.proc((it)->traversal.list.leaf, (it)->traversal.list.index)) \
	: *(it)->traversal.array.current)

/*---------------------------------------------------------------------------
 * Macro: Col_ListIterNext
 *
 *	Move the iterator to the next element.
 *
 * Argument:
 *	it	- The iterator to move.
 *
 * Result:
 *	Whether the iterator looped or not.
 *
 * Side effects:
 *	Update the iterator.
 *
 * See also: 
 *	<Col_ListIterForward>
 *---------------------------------------------------------------------------*/

#define Col_ListIterNext(it) \
    Col_ListIterForward((it), 1)

/*---------------------------------------------------------------------------
 * Macro: Col_ListIterPrevious
 *
 *	Move the iterator to the previous element.
 *
 * Argument:
 *	it	- The iterator to move.
 *
 * Side effects:
 *	Update the iterator.
 *
 * See also: 
 *	<Col_ListIterBackward>
 *---------------------------------------------------------------------------*/

#define Col_ListIterPrevious(it) \
    Col_ListIterBackward((it), 1)

/*---------------------------------------------------------------------------
 * Macro: Col_ListIterEnd
 *
 *	Test whether iterator reached end of list.
 *
 * Argument:
 *	it	- The iterator to test. (Caution: evaluated several times 
 *		  during macro expansion)
 *
 * Result:
 *	Non-zero if iterator is at end.
 *
 * See also: 
 *	<Col_ListIterator>, <Col_ListIterBegin>
 *---------------------------------------------------------------------------*/

#define Col_ListIterEnd(it) \
    ((it)->index >= (it)->length)

/*
 * Remaining declarations.
 */

EXTERN int		Col_ListIterBegin(Col_Word list, size_t index, 
			    Col_ListIterator *it);
EXTERN void		Col_ListIterFirst(Col_Word list, Col_ListIterator *it);
EXTERN void		Col_ListIterLast(Col_Word list, Col_ListIterator *it);
EXTERN void		Col_ListIterArray(size_t length, 
			    const Col_Word *elements, Col_ListIterator *it);
EXTERN int		Col_ListIterCompare(const Col_ListIterator *it1, 
			    const Col_ListIterator *it2);
EXTERN int		Col_ListIterMoveTo(Col_ListIterator *it, size_t index);
EXTERN int		Col_ListIterForward(Col_ListIterator *it, size_t nb);
EXTERN void		Col_ListIterBackward(Col_ListIterator *it, size_t nb);

EXTERN void		ColListIterUpdateTraversalInfo(Col_ListIterator *it);


/*
================================================================================
Section: Custom Lists
================================================================================
*/

/*---------------------------------------------------------------------------
 * Typedef: Col_ListLengthProc
 *
 *	Function signature of custom list length procs.
 *
 * Argument:
 *	list	- Custom list to get length for.
 *
 * Result:
 *	The custom list length.
 *
 * See also: 
 *	<Col_CustomListType>
 *---------------------------------------------------------------------------*/

typedef size_t (Col_ListLengthProc) (Col_Word list);

/*---------------------------------------------------------------------------
 * Typedef: Col_ListElementAtProc
 *
 *	Function signature of custom list element access procs.
 *
 * Arguments:
 *	list	- Custom list to get element from.
 *	index	- Element index.
 *
 * Note:
 *	By construction, indices are guaranteed to be within valid range.
 *
 * Result:
 *	The element at the given index.
 *
 * See also: 
 *	<Col_CustomListType>
 *---------------------------------------------------------------------------*/

typedef Col_Word (Col_ListElementAtProc) (Col_Word list, size_t index);

/*---------------------------------------------------------------------------
 * Typedef: Col_ListChunkAtProc
 *
 *	Function signature of custom list chunk access procs.
 *
 * Arguments:
 *	list		- Custom list to get chunk from.
 *	start		- Start index of chunk.
 *	max		- Maximum length of chunk.
 *
 * Note:
 *	By construction, indices are guaranteed to be within valid range.
 *
 * Results:
 *	lengthPtr	- Actual length.
 *	chunkPtr	- Chunk. If <COL_LISTCHUNK_VOID>, the chunk is void.
 *
 * See also: 
 *	<Col_CustomListType>
 *---------------------------------------------------------------------------*/

typedef void (Col_ListChunkAtProc) (Col_Word list, size_t start, size_t max, 
    size_t *lengthPtr, const Col_Word **chunkPtr);

/*---------------------------------------------------------------------------
 * Typedef: Col_ListSublistProc
 *
 *	Function signature of custom list sublist extraction.
 *
 * Arguments:
 *	list		- Custom list to extract sublist from.
 *	first, last	- Range of sublist to extract (inclusive).
 *
 * Note:
 *	By construction, indices are guaranteed to be within valid range.
 *
 * Result:
 *	A list representing the sublist, or nil.
 *
 * See also: 
 *	<Col_CustomListType>
 *---------------------------------------------------------------------------*/

typedef Col_Word (Col_ListSublistProc) (Col_Word list, size_t first, 
    size_t last);

/*---------------------------------------------------------------------------
 * Typedef: Col_ListConcatProc
 *
 *	Function signature of custom list concatenation.
 *
 * Arguments:
 *	left, right	- Lists to concatenate.
 *
 * Result:
 *	A list representing the concatenation of both lists, or nil.
 *
 * See also: 
 *	<Col_CustomListType>
 *---------------------------------------------------------------------------*/

typedef Col_Word (Col_ListConcatProc) (Col_Word left, Col_Word right);

/*---------------------------------------------------------------------------
 * Typedef: Col_CustomListType
 *
 *	Custom list type descriptor. Inherits from <Col_CustomWordType>.
 *
 * Fields:
 *	type		- Generic word type descriptor. Type field must be equal
 *			  to <COL_LIST>.
 *	lengthProc	- Called to get the length of the list. Must be constant
 *			  during the whole lifetime.
 *	elmentAtProc	- Called to get the element at a given position. Must
 *			  be constant during the whole lifetime.
 *	chunkAtProc	- Called during traversal. If NULL, traversal is done 
 *			  per element.
 *	sublistProc	- Called to extract sublist. If NULL, or if it returns 
 *			  nil, use the standard procedure.
 *	concatProc	- Called to concat lists. If NULL, or if it returns nil,
 *			  use the standard procedure.
 *
 * See also:
 *	<Col_CustomWordType>, <Col_ListLengthProc>, <Col_ListElementAtProc>, 
 *	<Col_ListChunkAtProc>, <Col_ListSublistProc>, <Col_ListConcatProc>
 *---------------------------------------------------------------------------*/

typedef struct Col_CustomListType {
    Col_CustomWordType type;
    Col_ListLengthProc *lengthProc;
    Col_ListElementAtProc *elementAtProc;
    Col_ListChunkAtProc *chunkAtProc;
    Col_ListSublistProc *sublistProc;
    Col_ListConcatProc *concatProc;
} Col_CustomListType;


/*
================================================================================
Section: Mutable Lists
================================================================================
*/

/****************************************************************************
 * Group: Mutable List Creation
 *
 * Declarations:
 *	<Col_NewMList>, <Col_CopyMList>
 ****************************************************************************/

EXTERN Col_Word		Col_NewMList();
EXTERN Col_Word		Col_CopyMList(Col_Word mlist);


/****************************************************************************
 * Group: Mutable List Operations
 *
 * Declarations:
 *	<Col_MListSetLength>, <Col_MListLoop>, <Col_MListSetAt>, 
 *	<Col_MListInsert>, <Col_MListRemove>, <Col_MListReplace>
 *	
 ****************************************************************************/

EXTERN void		Col_MListSetLength(Col_Word mlist, size_t length);
EXTERN void		Col_MListLoop(Col_Word mlist);
EXTERN void		Col_MListSetAt(Col_Word mlist, size_t index, 
			    Col_Word element);
EXTERN void		Col_MListInsert(Col_Word into, size_t index, 
			    Col_Word list);
EXTERN void		Col_MListRemove(Col_Word mlist, size_t first, 
			    size_t last);
EXTERN void		Col_MListReplace(Col_Word mlist, size_t first, 
			    size_t last, Col_Word with);

#endif /* _COLIBRI_LIST */
