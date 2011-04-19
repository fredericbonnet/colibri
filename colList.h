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
 * Vectors are flat arrays of bounded size.
 */

EXTERN size_t		Col_GetMaxVectorLength(void);
EXTERN Col_Word		Col_NewVector(size_t length, 
			    const Col_Word * elements);
EXTERN Col_Word		Col_NewVectorV(size_t length, ...);

/*
 * Lists are immutable structures build by composition of other vectors and 
 * lists.
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

/*
 * Sequences are collections of words that are sequentially traversable and 
 * potentially cyclic. They are implemented as linked lists of parts that are 
 * either lists or references to sequences. The sequence is formed by logically 
 * concatenating its parts, references being resolved recursively. Sequences 
 * thus form a (potentially cyclic) tree that is traversed depth-first.
 */

EXTERN Col_Word		Col_NewSequence(size_t nbParts, 
			    const Col_Word * parts);
EXTERN Col_Word		Col_NewSequenceV(size_t nbParts, ...);
EXTERN Col_Word		Col_NewSequenceL(Col_Word parts, size_t first, 
			    size_t last);

/*
 * References are used to build self-referential immutable structures. They
 * are created first and bound later once the referenced structures are created.
 */

EXTERN Col_Word		Col_NewReference();
EXTERN void		Col_BindReference(Col_Word reference, Col_Word source);
EXTERN Col_Word		Col_GetReference(Col_Word reference);


/*
 *----------------------------------------------------------------
 * High level operations.
 *----------------------------------------------------------------
 */

/* 
 * Indexing. 
 */

EXTERN Col_Word		Col_ListElementAt(Col_Word list, size_t index);

/* 
 * Repetition of a given list.
 */

EXTERN Col_Word		Col_RepeatList(Col_Word list, size_t count);

/* 
 * Insertion/removal. 
 */

EXTERN Col_Word		Col_ListInsert(Col_Word into, size_t index, Col_Word list);
EXTERN Col_Word		Col_ListRemove(Col_Word list, size_t first, size_t last);
EXTERN Col_Word		Col_ListReplace(Col_Word list, size_t first, size_t last, 
			    Col_Word with);


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
 * Iteration over individual list elements. Each iterator takes 8 words on the
 * stack.
 */

typedef struct Col_ListIterator {
    Col_Word list;		/* List being iterated. */
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

/*
 * Iteration over sequence parts (i.e. lists). Each iterator takes 6 words on 
 * the stack, plus dynamically allocated stack cells if needed.
 * To iterate over individual element, one has to iterate over the element of
 * each part using list traversal or iterator.
 */

typedef struct Col_SequenceIterator {
    Col_Word seq;		/* Sequence being iterated. */
    Col_Word node;		/* Current sequence node. */
    Col_Word stack;		/* Recursion stack when following references. */
    Col_Word turtle;		/* "Turtle" stack node for Brent's cycle 
				 * detection algorithm. */
    size_t limit, steps;	/* Brent's algoritm parameters. */
} Col_SequenceIterator;


#define Col_SequenceIterEnd(it)		(!(it)->seq)
#define Col_SequenceIterSequence(it)	((it)->seq)
	
EXTERN void		Col_SequenceIterBegin(Col_Word sequence, 
			    Col_SequenceIterator *it);
EXTERN void		Col_SequenceIterDone(Col_SequenceIterator *it);
EXTERN Col_Word		Col_SequenceIterPartAt(Col_SequenceIterator *it);
EXTERN void		Col_SequenceIterNext(Col_SequenceIterator *it);

#endif /* _COLIBRI_LIST */
