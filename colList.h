#ifndef _COLIBRI_LIST
#define _COLIBRI_LIST

#include <stddef.h> /* For size_t */
#include <stdarg.h> /* For variadic procs */


/* 
 *----------------------------------------------------------------
 * Basic operations. 
 *----------------------------------------------------------------
 */

/*
 * Lists are immutable structures built by composition of other lists and 
 * vectors. Immutable vectors can be used in place of immutable lists. Mutable
 * lists' content is frozen, and mutable vectors are copied.
 */

EXTERN Col_Word		Col_NewList(size_t length, 
			    const Col_Word * elements);
EXTERN size_t		Col_ListLength(Col_Word list);
EXTERN Col_Word		Col_Sublist(Col_Word list, size_t first, size_t last);
EXTERN Col_Word		Col_ConcatLists(Col_Word left, Col_Word right);
EXTERN Col_Word		Col_ConcatListsA(size_t number, const Col_Word * words);
EXTERN Col_Word		Col_ConcatListsNV(size_t number, ...);
#define Col_ConcatListsV(...) \
    Col_ConcatListsNV(COL_ARGCOUNT(__VA_ARGS__),__VA_ARGS__)
EXTERN Col_Word		Col_LoopList(Col_Word list);
EXTERN size_t		Col_ListLoopLength(Col_Word list);

/*
 * Mutable lists can be composed of either mutable or immutable lists and
 * vectors. Contrary to immutable versions, mutable list operations cannot be 
 * used on mutable vectors: the former can grow indefinitely whereas the latter
 * have a maximum length set at creation time.
 */

EXTERN Col_Word		Col_NewMList();
EXTERN void		Col_FreezeMList(Col_Word mlist);
EXTERN void		Col_MListSetLength(Col_Word mlist, size_t length);
EXTERN void		Col_MListSetLoop(Col_Word mlist, size_t loopLength);
EXTERN void		Col_MListSetAt(Col_Word mlist, size_t index, 
			    Col_Word element);
EXTERN void		Col_MListInsert(Col_Word into, size_t index, 
			    Col_Word list);
EXTERN void		Col_MListRemove(Col_Word mlist, size_t first, 
			    size_t last);


/*
 *----------------------------------------------------------------
 * High level immutable operations. These are done by composition of basic 
 * immutable operations.
 *----------------------------------------------------------------
 */

/* 
 * Accessing.
 */

EXTERN Col_Word		Col_ListAt(Col_Word list, size_t index);

/* 
 * Immutable repetition.
 */

EXTERN Col_Word		Col_RepeatList(Col_Word list, size_t count);

/* 
 * Immutable insertion/removal. 
 */

EXTERN Col_Word		Col_ListInsert(Col_Word into, size_t index, 
			    Col_Word list);
EXTERN Col_Word		Col_ListRemove(Col_Word list, size_t first, 
			    size_t last);
EXTERN Col_Word		Col_ListReplace(Col_Word list, size_t first, 
			    size_t last, Col_Word with);

/* 
 * Mutable insertion/removal. 
 */

EXTERN void		Col_MListReplace(Col_Word mlist, size_t first, 
			    size_t last, Col_Word with);

/* 
 *----------------------------------------------------------------
 * Iteration and traversal. 
 *----------------------------------------------------------------
 */

/* 
 * Iteration over the basic chunks of data. Works with mutable or immutable 
 * lists and vectors.
 */

typedef int (Col_ListChunksTraverseProc) (size_t index, size_t length, 
	size_t number, const Col_Word **elements, Col_ClientData clientData);
EXTERN int		Col_TraverseListChunks(size_t number, Col_Word *lists, 
			    size_t start, size_t max, 
			    Col_ListChunksTraverseProc *proc, 
			    Col_ClientData clientData, size_t *lengthPtr);

#define COL_LISTCHUNK_VOID	((void *)-1)

/*
 * Iteration over individual list elements. Each iterator takes 9 words on the 
 * stack.
 *
 * Works with mutable or immutable lists and vectors, however modifying a 
 * mutable list during iteration results in undefined behavior.
 */

typedef Col_Word (ColListIterLeafAtProc) (Col_Word leaf, size_t index);
typedef struct Col_ListIterator {
    Col_Word list;		/* List being iterated. */
    size_t index;		/* Current position. */
    struct {			/* Traversal info: */
	Col_Word subnode;	/*  - Traversed subnode. */
	size_t first, last;	/*  - Range of validity. */
	size_t offset;		/*  - Index offset wrt. root. */
	Col_Word leaf;		/*  - Leaf (deepest) word, i.e. vector or void list. */ 
	size_t index;		/*  - Index within leaf. */
	ColListIterLeafAtProc *proc;
				/*  - Access proc within leaf. */
    } traversal;
} Col_ListIterator;

EXTERN void		ColListIterUpdateTraversalInfo(Col_ListIterator *it);

#define Col_ListIterEnd(it)	(!(it)->list)
#define Col_ListIterList(it)	((it)->list)
#define Col_ListIterIndex(it)	((it)->index)
#define Col_ListIterAt(it)	(!(it)->traversal.leaf?ColListIterUpdateTraversalInfo(it),0:0,(it)->traversal.proc((it)->traversal.leaf, (it)->traversal.index))
	
EXTERN int		Col_ListIterBegin(Col_Word list, size_t index, 
			    Col_ListIterator *it);
EXTERN int		Col_ListIterCompare(Col_ListIterator *it1, 
			    Col_ListIterator *it2);
EXTERN int		Col_ListIterMoveTo(Col_ListIterator *it, size_t index);
EXTERN int		Col_ListIterForward(Col_ListIterator *it, size_t nb);
EXTERN void		Col_ListIterBackward(Col_ListIterator *it, size_t nb);

#define Col_ListIterNext(it)	Col_ListIterForward((it), 1)
#define Col_ListIterPrevious(it) Col_ListIterBackward((it), 1)

#endif /* _COLIBRI_LIST */
