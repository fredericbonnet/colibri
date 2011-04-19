/*
 * File: colList.h
 *
 *	This header file defines the list handling features of Colibri.
 *
 *	Lists are a collection datatype that allows for fast insertion, 
 *	extraction and composition of other lists. Internally they use 
 *	self-balanced binary trees, like ropes, except that they use vectors
 *	as basic containers instead of character arrays.
 *
 *	They come in both immutable and mutable forms: 
 *	
 *	- Immutable lists can be composed of immutable vectors and lists. 
 *	Immutable vectors can themselves be used in place of immutable lists. 
 *
 *	- Mutable lists can be composed of either mutable or immutable lists and
 *	vectors. They can be "frozen" and turned into immutable versions.
 *	Mutable vectors *cannot* be used in place of mutable lists, as the 
 *	latter can grow indefinitely whereas the former have a maximum length 
 *	set at creation time.
 */

#ifndef _COLIBRI_LIST
#define _COLIBRI_LIST

#include <stddef.h> /* For size_t */
#include <stdarg.h> /* For variadic procs */


/****************************************************************************
 * Group: Immutable List Creation
 *
 * Declarations:
 *	<Col_NewList>
 ****************************************************************************/

EXTERN Col_Word		Col_NewList(size_t length, 
			    const Col_Word * elements);


/****************************************************************************
 * Group: Immutable List Access
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
 *	<Col_ConcatListsNV>, <Col_RepeatList>, <Col_LoopList>, 
 *	<Col_ListInsert>, <Col_ListRemove>, <Col_ListReplace>
 *	
 ****************************************************************************/

EXTERN Col_Word		Col_Sublist(Col_Word list, size_t first, size_t last);
EXTERN Col_Word		Col_ConcatLists(Col_Word left, Col_Word right);
EXTERN Col_Word		Col_ConcatListsA(size_t number, const Col_Word * words);
EXTERN Col_Word		Col_ConcatListsNV(size_t number, ...);
EXTERN Col_Word		Col_RepeatList(Col_Word list, size_t count);
EXTERN Col_Word		Col_LoopList(Col_Word list);
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
 * See also:
 *	<COL_ARGCOUNT>
 *---------------------------------------------------------------------------*/

#define Col_ConcatListsV(...) \
    Col_ConcatListsNV(COL_ARGCOUNT(__VA_ARGS__),__VA_ARGS__)


/****************************************************************************
 * Group: Mutable List Creation
 *
 * Declarations:
 *	<Col_NewMList>
 ****************************************************************************/

EXTERN Col_Word		Col_NewMList();


/****************************************************************************
 * Group: Mutable List Operations
 *
 * Declarations:
 *	<Col_FreezeMList>, <Col_MListSetLength>, <Col_MListSetLoop>,
 *	<Col_MListSetAt>, <Col_MListInsert>, <Col_MListRemove>,
 *	<Col_MListReplace>
 *	
 ****************************************************************************/

EXTERN void		Col_FreezeMList(Col_Word mlist);
EXTERN void		Col_MListSetLength(Col_Word mlist, size_t length);
EXTERN void		Col_MListSetLoop(Col_Word mlist, size_t loopLength);
EXTERN void		Col_MListSetAt(Col_Word mlist, size_t index, 
			    Col_Word element);
EXTERN void		Col_MListInsert(Col_Word into, size_t index, 
			    Col_Word list);
EXTERN void		Col_MListRemove(Col_Word mlist, size_t first, 
			    size_t last);
EXTERN void		Col_MListReplace(Col_Word mlist, size_t first, 
			    size_t last, Col_Word with);


/****************************************************************************
 * Group: List Traversal
 *
 * Declarations:
 *	<Col_TraverseListChunks>
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
 *			  <Col_TraverseListChunks>
 *
 * Returns:
 *	If non-zero, interrupts the traversal.
 *
 * See also: 
 *	<Col_TraverseListChunks>
 *---------------------------------------------------------------------------*/

typedef int (Col_ListChunksTraverseProc) (size_t index, size_t length, 
	size_t number, const Col_Word **chunks, Col_ClientData clientData);

/*---------------------------------------------------------------------------
 * Macro: COL_LISTCHUNK_VOID
 *
 *	Value passed as chunk pointer to list traversal procs when traversing
 *	void lists.
 *
 * See also: 
 *	<Col_TraverseListChunks>, <Col_ListChunksTraverseProc>
 *---------------------------------------------------------------------------*/

#define COL_LISTCHUNK_VOID \
    ((void *)-1)

EXTERN int		Col_TraverseListChunks(size_t number, Col_Word *lists, 
			    size_t start, size_t max, 
			    Col_ListChunksTraverseProc *proc, 
			    Col_ClientData clientData, size_t *lengthPtr);


/****************************************************************************
 * Group: List Iterators
 *
 * Declarations: 
 *	<Col_ListIterBegin>, <Col_ListIterCompare>, <Col_ListIterMoveTo>, 
 *	<Col_ListIterForward>, <Col_ListIterBackward>, 
 *	<ColListIterUpdateTraversalInfo>
 *
 * Note:
 *	Works with mutable or immutable lists and vectors, however modifying a 
 *	mutable list during iteration results in undefined behavior.
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Typedef: ColListIterLeafAtProc
 *	Helper for list iterators to access characters in leaves.
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
 *	list			- List being iterated.
 *	index			- Current position.
 *	traversal		- Traversal info:
 *	traversal.subnode	- Traversed subnode.
 *	traversal.(first, last)	- Range of validity.
 *	traversal.offset	- Index offset wrt. root.
 *	traversal.leaf		- Leaf (deepest) node.
 *	traversal.index		- Index within leaf.
 *	traversal.proc		- Access proc within leaf.
 *
 * See also:
 *	<Col_ListIterator>
 *---------------------------------------------------------------------------*/

typedef struct ColListIterator {
    Col_Word list;
    size_t index;
    struct {
	Col_Word subnode;
	size_t first, last;
	size_t offset;
	Col_Word leaf;
	size_t index;
	ColListIterLeafAtProc *proc;
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
 *	Each iterator takes 9 words on the stack.
 *---------------------------------------------------------------------------*/

typedef ColListIterator Col_ListIterator;

/*---------------------------------------------------------------------------
 * Macro: Col_ListIterEnd
 *
 *	Test whether iterator reached end of list.
 *
 * Arguments:
 *	it	- The iterator to test.
 *
 * Result:
 *	Non-zero if iterator is at end.
 *
 * See also: 
 *	<Col_ListIterator>, <Col_ListIterBegin>
 *---------------------------------------------------------------------------*/

#define Col_ListIterEnd(it) \
    (!(it)->list)


/*---------------------------------------------------------------------------
 * Macro: Col_ListIterList
 *
 *	Get list for iterator.
 *
 * Arguments:
 *	it	- The iterator to get list for.
 *
 * Result:
 *	The list, or <WORD_NIL> if at end.
 *
 * See also: 
 *	<Col_ListIterator>, <Col_ListIterEnd>
 *---------------------------------------------------------------------------*/

#define Col_ListIterList(it) \
    ((it)->list)

/*---------------------------------------------------------------------------
 * Macro: Col_ListIterIndex
 *
 *	Get current index within list for iterator.
 *
 * Arguments:
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
 *	Get current list element for iterator.
 *
 * Arguments:
 *	it	- The iterator to get element for.
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
    (!(it)->traversal.leaf?ColListIterUpdateTraversalInfo(it),0:0,(it)->traversal.proc((it)->traversal.leaf, (it)->traversal.index))

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

EXTERN int		Col_ListIterBegin(Col_Word list, size_t index, 
			    Col_ListIterator *it);
EXTERN int		Col_ListIterCompare(Col_ListIterator *it1, 
			    Col_ListIterator *it2);
EXTERN int		Col_ListIterMoveTo(Col_ListIterator *it, size_t index);
EXTERN int		Col_ListIterForward(Col_ListIterator *it, size_t nb);
EXTERN void		Col_ListIterBackward(Col_ListIterator *it, size_t nb);

EXTERN void		ColListIterUpdateTraversalInfo(Col_ListIterator *it);

#endif /* _COLIBRI_LIST */
