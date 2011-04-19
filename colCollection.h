#ifndef _COLIBRI_COLLECTION
#define _COLIBRI_COLLECTION

#include <stddef.h> /* For size_t */
#include <stdarg.h> /* For variadic procs */


/* 
 *----------------------------------------------------------------
 * Basic operations. 
 *----------------------------------------------------------------
 */

/*
 * References are used to build self-referential immutable structures. They
 * are created first and bound later once the referenced structures are created.
 */

EXTERN Col_Word		Col_NewReference();
EXTERN void		Col_BindReference(Col_Word reference, Col_Word source);

/* 
 * Vectors are immutable flat arrays of bounded length.
 */

EXTERN size_t		Col_GetMaxVectorLength(void);
EXTERN Col_Word		Col_NewVector(size_t length, 
			    const Col_Word * elements);
EXTERN Col_Word		Col_NewVectorV(size_t length, ...);

/*
 * Mutable vectors are flat arrays that can grow up to a maximum length, whose
 * content is directly modifiable through a C array; newly added children need 
 * proper declaration (Col_DeclareChild).
 */

EXTERN Col_Word		Col_NewMVector(size_t maxLength);
EXTERN void		Col_FreezeMVector(Col_Word mvector);
EXTERN void		Col_MVectorSetLength(Col_Word mvector, size_t length);

/*
 * Lists are immutable structures built by composition of other lists and 
 * vectors. Immutable vectors can be used in place of immutable lists. 
 */

EXTERN Col_Word		Col_NewList(size_t length, 
			    const Col_Word * elements);
EXTERN size_t		Col_ListLength(Col_Word list);
EXTERN Col_Word		Col_Sublist(Col_Word list, size_t first, size_t last);
EXTERN Col_Word		Col_ConcatLists(Col_Word left, Col_Word right);
EXTERN Col_Word		Col_ConcatListsA(size_t number, const Col_Word * words);
EXTERN Col_Word		Col_ConcatListsV(size_t number, ...);
EXTERN Col_Word		Col_ConcatListsL(Col_Word list, size_t first, 
			    size_t last);
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
EXTERN void		Col_MListLoop(Col_Word mlist, size_t length);
EXTERN void		Col_MListSetAt(Col_Word mlist, size_t index, 
			    Col_Word element);
EXTERN void		Col_MListInsert(Col_Word into, size_t index, 
			    Col_Word list);
EXTERN void		Col_MListRemove(Col_Word mlist, size_t first, 
			    size_t last);
EXTERN void		Col_MListReplace(Col_Word mlist, size_t first, 
			    size_t last, Col_Word with);


/*
 *----------------------------------------------------------------
 * High level operations on immutable lists. These are done by composition of
 * basic operations.
 *----------------------------------------------------------------
 */

/* 
 * Accessing. Works on mutable or immutable lists and vectors.
 */

EXTERN Col_Word		Col_ListGetAt(Col_Word list, size_t index);

/* 
 * Repetition/looping of an immutable list.
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
 *----------------------------------------------------------------
 * Iteration and traversal. 
 *----------------------------------------------------------------
 */

/* 
 * Iteration over the basic chunks of data. Works with mutable or immutable 
 * lists and vectors.
 */

typedef int (Col_ListChunksEnumProc) (const Col_Word *elements, size_t length,
	Col_ClientData clientData);

EXTERN int		Col_TraverseListChunks(Col_Word list, size_t start, 
			    size_t max, Col_ListChunksEnumProc *proc, 
			    Col_ClientData clientData, size_t *sizePtr);

/*
 * Iteration over individual list elements. Each iterator takes 8 words on the 
 * stack.
 *
 * Works with mutable or immutable lists and vectors, however modifying a 
 * mutable list during iteration results in undefined behavior.
 */

typedef struct Col_ListIterator {
    Col_Word list;		/* List being iterated. */
    size_t index;		/* Current position. */
    struct {			/* Traversal info: */
	Col_Word subnode;	/*  - Traversed subnode. */
	size_t first, last;	/*  - Range of validity. */
	size_t offset;		/*  - Index offset wrt. root. */
	Col_Word leaf;		/*  - Leaf (deepest) word, i.e. vector or void list. */ 
	size_t index;		/*  - Index within leaf. */
    } traversal;
} Col_ListIterator;

#define Col_ListIterEnd(it)	(!(it)->list)
#define Col_ListIterList(it)	((it)->list)
#define Col_ListIterIndex(it)	((it)->index)
	
EXTERN void		Col_ListIterBegin(Col_Word list, size_t index, 
			    Col_ListIterator *it);
EXTERN Col_Word		Col_ListIterElementAt(Col_ListIterator *it);
EXTERN int		Col_ListIterCompare(Col_ListIterator *it1, 
			    Col_ListIterator *it2);
EXTERN void		Col_ListIterMoveTo(Col_ListIterator *it, size_t index);
EXTERN void		Col_ListIterForward(Col_ListIterator *it, size_t nb);
EXTERN void		Col_ListIterBackward(Col_ListIterator *it, size_t nb);

#define Col_ListIterNext(it)	Col_ListIterForward((it), 1)
#define Col_ListIterPrevious(it) Col_ListIterBackward((it), 1)

#endif /* _COLIBRI_COLLECTION */
