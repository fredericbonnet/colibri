#ifndef _COLIBRI_VECTOR
#define _COLIBRI_VECTOR

#include <stddef.h> /* For size_t and NULL */
#include <stdarg.h> /* For variadic procs */


/* 
 *----------------------------------------------------------------
 * Basic operations. 
 *----------------------------------------------------------------
 */

/* 
 * Vectors are flat arrays of bounded size.
 */

EXTERN size_t		Col_GetMaxVectorLength();
EXTERN Col_Word		Col_NewVectorWord(size_t length, 
			    const Col_Word * elements);
EXTERN Col_Word		Col_NewVectorWordV(size_t length, ...);

/*
 * Lists are immutable structuers build by composition of other vectors and 
 * lists.
 */

EXTERN Col_Word		Col_NewListWord(size_t length, 
			    const Col_Word * elements);
EXTERN size_t		Col_ListLength(Col_Word list);
EXTERN Col_Word		Col_Sublist(Col_Word list, size_t first, size_t last);
EXTERN Col_Word		Col_ConcatLists(Col_Word left, Col_Word right);
EXTERN Col_Word		Col_ConcatListsA(size_t number, Col_Word * words);
EXTERN Col_Word		Col_ConcatListsV(size_t number, ...);

/* 
 *----------------------------------------------------------------
 * Iteration and traversal. 
 *----------------------------------------------------------------
 */

/* 
 * Iteration over the basic chunks of data. 
 */

typedef int (Col_ListChunksEnumProc) (const Col_Word *elements, size_t length,
	Col_ClientData clientData);

EXTERN int		Col_TraverseListChunks(Col_Word list, size_t start, 
			    size_t max, Col_ListChunksEnumProc *proc, 
			    Col_ClientData clientData, size_t *sizePtr);

/*
 * Iteration over individual list elements.
 */

typedef struct Col_ListIterator {
    Col_Rope list;		/* List being iterated. */
    size_t index;		/* Current position. */
    struct {			/* Traversal info: */
	Col_Word subnode;	/*  - Traversed subnode. */
	size_t first, last;	/*  - Range of validity. */
	size_t offset;		/*  - Index offset wrt. root. */
	Col_Word leaf;		/*  - Leaf (deepest) word, i.e. vector. */ 
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


#endif /* _COLIBRI_VECTOR */
