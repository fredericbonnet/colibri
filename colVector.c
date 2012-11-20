/*
 * File: colVector.c
 *
 *	This file implements the vector handling features of Colibri.
 *
 *	Vectors are arrays of words that are directly accessible through a
 *	pointer value. 
 *
 *	They come in both immutable and mutable forms :
 *
 *	- Immutable vectors are flat arrays of fixed length.
 *
 *	- Mutable vectors are flat arrays that can grow up to a maximum length, 
 *	whose content is directly modifiable through a C pointer. They can be
 *	"frozen" and turned into immutable versions.
 *
 * See also:
 *	<colVector.h>
 */

#include "include/colibri.h"
#include "colInternal.h"

#include "colWordInt.h"
#include "colVectorInt.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>

/*
 * Prototypes for functions used only in this file.
 */

static void		QuickSort(Col_Word *elements, Col_SortCompareProc *proc,
			    size_t first, size_t last, 
			    Col_ClientData clientData);


/*
================================================================================
Section: Immutable Vectors
================================================================================
*/

/****************************************************************************
 * Group: Immutable Vector Creation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_MaxVectorLength
 *
 *	Get the maximum length of a vector word.
 *
 * Result:
 *	The max vector length.
 *---------------------------------------------------------------------------*/

size_t
Col_MaxVectorLength()
{
    return VECTOR_MAX_LENGTH(SIZE_MAX);
}

/*---------------------------------------------------------------------------
 * Function: Col_NewVector
 *
 *	Create a new vector word.
 *
 * Arguments:
 *	length		- Length of below array.
 *	elements	- Array of words to populate vector with, or NULL. In
 *			  the latter case, elements are initialized to nil.
 *
 * Range checking:
 *	*length* must not exceed the maximum vector length given by 
 *	<Col_MaxVectorLength>.
 *
 * Result:
 *	If the given length is larger than the maximum length allowed, nil.
 *	Else the new word.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewVector(
    size_t length,
    const Col_Word * elements)
{
    Col_Word vector;		/* Resulting word in the general case. */

    /*
     * Check preconditions.
     */

    RANGECHECK_VECTORLENGTH(length, VECTOR_MAX_LENGTH(SIZE_MAX)) 
	    return WORD_NIL;

    if (length == 0) {
	/* 
	 * Use immediate value.
	 */

	return WORD_LIST_EMPTY;
    }

    /*
     * Create a new vector word.
     */

    vector = (Col_Word) AllocCells(VECTOR_SIZE(length));
    WORD_VECTOR_INIT(vector, length);
    if (elements) {
	/*
	 * Copy elements.
	 */

	memcpy(WORD_VECTOR_ELEMENTS(vector), elements, length 
		* sizeof(Col_Word));
    } else {
	/*
	 * Initialize elements to nil.
	 */

	memset(WORD_VECTOR_ELEMENTS(vector), 0, length 
		* sizeof(Col_Word));
    }

    return vector;
}

/*---------------------------------------------------------------------------
 * Function: Col_NewVectorNV
 *
 *	Create a new vector word from a list of arguments.
 *
 * Arguments:
 *	length	- Number of arguments.
 *	...	- Remaining arguments, i.e. words to add in order.
 *
 * Range checking:
 *	*length* must not exceed the maximum vector length given by 
 *	<Col_MaxVectorLength>.
 *
 * Result:
 *	If the given length is larger than the maximum length allowed, nil.
 *	Else the new word.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewVectorNV(
    size_t length,
    ...)
{
    size_t i;
    va_list args;
    Col_Word vector, *elements;

    /*
     * Check preconditions.
     */

    RANGECHECK_VECTORLENGTH(length, VECTOR_MAX_LENGTH(SIZE_MAX)) 
	    return WORD_NIL;

    if (length == 0) {
	/* 
	 * Use immediate value.
	 */

	return WORD_LIST_EMPTY;
    }

    /*
     * Create a new vector word.
     *
     * Note: no need to declare children as by construction they are older than
     * the newly created vector.
     */

    vector = (Col_Word) AllocCells(VECTOR_SIZE(length));
    WORD_VECTOR_INIT(vector, length);
    elements = WORD_VECTOR_ELEMENTS(vector);
    va_start(args, length);
    for (i=0; i < length; i++) {
	elements[i] = va_arg(args, Col_Word);
    }
    va_end(args);

    return vector;
}


/****************************************************************************
 * Group: Immutable Vector Accessors
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_VectorLength
 *
 *	Get the length of the vector.
 *
 * Argument:
 *	vector	- Vector to get length for.
 *
 * Type checking:
 *	*vector* must be a valid vector.
 *
 * Result:
 *	The vector length.
 *
 * See also:
 *	<Col_NewVector>, <Col_VectorElements>
 *---------------------------------------------------------------------------*/

size_t
Col_VectorLength(
    Col_Word vector)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_VECTOR(vector) return 0;

    WORD_UNWRAP(vector);

    switch (WORD_TYPE(vector)) {
    case WORD_TYPE_VECTOR: 
    case WORD_TYPE_MVECTOR: 
	return WORD_VECTOR_LENGTH(vector);

    case WORD_TYPE_VOIDLIST:
	ASSERT(WORD_VOIDLIST_LENGTH(vector) == 0);
	return 0;

    /* WORD_TYPE_UNKNOWN */

    default:
	/*CANTHAPPEN*/
	ASSERT(0);
	return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_VectorElements
 *
 *	Get the vector element array.
 *
 * Argument:
 *	vector	- Vector to get elements for.
 *
 * Type checking:
 *	*vector* must be a valid vector.
 *
 * Result:
 *	The vector element array.
 *
 * See also:
 *	<Col_NewVector>, <Col_VectorLength>
 *---------------------------------------------------------------------------*/

const Col_Word *
Col_VectorElements(
    Col_Word vector)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_VECTOR(vector) return NULL;

    WORD_UNWRAP(vector);

    switch (WORD_TYPE(vector)) {
    case WORD_TYPE_VECTOR: 
    case WORD_TYPE_MVECTOR: 
	return WORD_VECTOR_ELEMENTS(vector);

    case WORD_TYPE_VOIDLIST:
	ASSERT(WORD_VOIDLIST_LENGTH(vector) == 0);
	return NULL;

    /* WORD_TYPE_UNKNOWN */

    default:
	/*CANTHAPPEN*/
	ASSERT(0);
	return 0;
    }
}


/*
================================================================================
Section: Mutable Vectors
================================================================================
*/

/****************************************************************************
 * Group: Mutable Vector Creation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_MaxMVectorLength
 *
 *	Get the maximum length of a mutable vector word.
 *
 * Result:
 *	The max vector length.
 *---------------------------------------------------------------------------*/

size_t
Col_MaxMVectorLength()
{
    return VECTOR_MAX_LENGTH(MVECTOR_MAX_SIZE * CELL_SIZE);
}

/*---------------------------------------------------------------------------
 * Function: Col_NewMVector
 *
 *	Create a new mutable vector word, and optionally populate with the
 *	given elements.
 *
 *	Note that the actual maximum length will be rounded up to fit an even
 *	number of cells.
 *
 * Arguments:
 *	maxLength	- Maximum length of mutable vector.
 *	length		- Length of below array.
 *	elements	- Array of words to populate vector with, or NULL. In
 *			  the latter case, elements are initialized to nil.
 *
 * Range checking:
 *	*maxLength* and *length* must not exceed the maximum mutable vector 
 *	length given by <Col_MaxMVectorLength>.
 *
 * Result:
 *	If the given length is larger than the maximum length allowed, nil.
 *	Else the new word.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewMVector(
    size_t maxLength,
    size_t length,
    const Col_Word * elements)
{
    Col_Word mvector;		/* Resulting word in the general case. */
    size_t size;		/* Number of allocated cells storing a minimum
				 * of maxLength elements. */

    /*
     * Normalize max length.
     */

    if (maxLength < length) {
	maxLength = length;
    }

    /*
     * Check preconditions.
     */

    RANGECHECK_VECTORLENGTH(maxLength, 
	    VECTOR_MAX_LENGTH(MVECTOR_MAX_SIZE * CELL_SIZE)) return WORD_NIL;

    /*
     * Create a new mutable vector word.
     */

    size = VECTOR_SIZE(maxLength);
    mvector = (Col_Word) AllocCells(size);
    WORD_MVECTOR_INIT(mvector, size, length);
    if (length > 0) {
	if (elements) {
	    /*
	     * Copy elements.
	     */

	    memcpy(WORD_VECTOR_ELEMENTS(mvector), elements, length 
		    * sizeof(Col_Word));
	} else {
	    /*
	     * Initialize elements to nil.
	     */

	    memset(WORD_VECTOR_ELEMENTS(mvector), 0, length 
		    * sizeof(Col_Word));
	}
    }

    return mvector;
}


/****************************************************************************
 * Group: Mutable Vector Accessors
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_MVectorMaxLength
 *
 *	Get the maximum length of the mutable vector.
 *
 * Argument:
 *	mvector	- Mutable vector to get maximum length for.
 *
 * Type checking:
 *	*mvector* must be a valid mutable vector.
 *
 * Result:
 *	The mutable vector maximum length.
 *
 * See also:
 *	<Col_NewMVector>
 *---------------------------------------------------------------------------*/

size_t
Col_MVectorMaxLength(
    Col_Word mvector)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MVECTOR(mvector) return 0;

    if (WORD_TYPE(mvector) == WORD_TYPE_WRAP) {
	mvector = WORD_WRAP_SOURCE(mvector);
    }

    return VECTOR_MAX_LENGTH(WORD_MVECTOR_SIZE(mvector) * CELL_SIZE);
}

/*---------------------------------------------------------------------------
 * Function: Col_MVectorElements
 *
 *	Get the mutable vector element array.
 *
 * Argument:
 *	mvector	- Mutable ector to get elements for.
 *
 * Type checking:
 *	*mvector* must be a valid mutable vector.
 *
 * Result:
 *	The mutable vector element array.
 *
 * See also:
 *	<Col_VectorLength>
 *---------------------------------------------------------------------------*/

Col_Word *
Col_MVectorElements(
    Col_Word mvector)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MVECTOR(mvector) return NULL;

    if (WORD_TYPE(mvector) == WORD_TYPE_WRAP) {
	mvector = WORD_WRAP_SOURCE(mvector);
    }

    return WORD_VECTOR_ELEMENTS(mvector);
}


/****************************************************************************
 * Group: Mutable Vector Operations
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_MVectorSetLength
 *
 *	Resize the mutable vector. Newly added elements are set to nil.
 *
 * Arguments:
 *	mvector	- Mutable vector to resize.
 *	length	- New length. Must not exceed max length set at creation.
 *
 * Type checking:
 *	*mvector* must be a valid mutable vector.
 *
 * Range checking:
 *	*length* must not exceed the maximum length given at mutable vector 
 *	creation time.
 *---------------------------------------------------------------------------*/

void
Col_MVectorSetLength(
    Col_Word mvector,
    size_t length)
{
    size_t maxLength, oldLength;

    /*
     * Check preconditions.
     */

    TYPECHECK_MVECTOR(mvector) return;

    if (WORD_TYPE(mvector) == WORD_TYPE_WRAP) {
	mvector = WORD_WRAP_SOURCE(mvector);
    }

    maxLength = VECTOR_MAX_LENGTH(WORD_MVECTOR_SIZE(mvector) * CELL_SIZE);
    RANGECHECK_VECTORLENGTH(length, maxLength) return;

    oldLength = WORD_VECTOR_LENGTH(mvector);
    if (length > oldLength) {
	/*
	 * Initialize elements to nil.
	 */

	memset(WORD_VECTOR_ELEMENTS(mvector) + oldLength, 0, (length 
		- oldLength)*sizeof(Col_Word));
    }
    WORD_VECTOR_LENGTH(mvector) = length;
}

/*---------------------------------------------------------------------------
 * Function: Col_MVectorFreeze
 *
 *	Turn a mutable vector immutable. If an immutable vector is given,
 *	does nothing.
 *
 * Argument:
 *	mvector	- Mutable vector to freeze. 
 *
 * Type checking:
 *	*mvector* must be a valid vector.
 *---------------------------------------------------------------------------*/

void
Col_MVectorFreeze(
    Col_Word mvector)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_VECTOR(mvector) return;

    for (;;) {
	switch (WORD_TYPE(mvector)) {
	case WORD_TYPE_WRAP:
	    mvector = WORD_WRAP_SOURCE(mvector);
	    continue;

	case WORD_TYPE_VOIDLIST:
	case WORD_TYPE_VECTOR:
	    /*
	     * No-op.
	     */

	    return;

	case WORD_TYPE_MVECTOR: {
	    /*
	     * Simply change type ID. Don't mark extraneous cells, they will
	     * be collected during GC.
	     */

	    int pinned = WORD_PINNED(mvector);
	    WORD_SET_TYPEID(mvector, WORD_TYPE_VECTOR);
	    if (pinned) {
		WORD_SET_PINNED(mvector);
	    }
	    return;
	}

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
	}
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_MVectorSort
 *
 *	Sort a mutable vector.
 *
 *	Uses the quicksort algorithm with 3-way partitioning given in 
 *	"Quicksort is optimal" by Robert Sedgewick and Jon Bentley (with slight
 *	adaptations to unsigned indices).
 *
 * Argument:
 *	mvector		- Mutable vector to sort. 
 *	proc		- <Col_SortCompareProc>.
 *	first, last	- Range to sort.
 *	clientData	- Opaque data passed as is to above proc.
 *
 * Type checking:
 *	*mvector* must be a valid mutable vector.
 *---------------------------------------------------------------------------*/

void
Col_MVectorSort(
    Col_Word mvector, 
    Col_SortCompareProc *proc, 
    size_t first, 
    size_t last, 
    Col_ClientData clientData)
{
    Col_Word *elements;
    size_t length;

    /*
     * Check preconditions.
     */

    TYPECHECK_MVECTOR(mvector) return;

    for (;;) {
	switch (WORD_TYPE(mvector)) {
	case WORD_TYPE_WRAP:
	    mvector = WORD_WRAP_SOURCE(mvector);
	    continue;

	case WORD_TYPE_MVECTOR: 
	    elements = WORD_VECTOR_ELEMENTS(mvector);
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
	    return;
	}
    }

    length = WORD_VECTOR_LENGTH(mvector);
    if (length <= 1) {
	/*
	 * Nothing to do.
	 */

	return;
    }
    if (first >= length || last < first) {
	/*
	 * Invalid range.
	 */

	return;
    }
    if (last >= length) {
	last = length-1;
    }
    QuickSort(elements, proc, first, last, clientData);
}

/*---------------------------------------------------------------------------
 * Internal Function: QuickSort
 *
 *	Sort a mutable vector using the quicksort algorithm with 3-way 
 *	partitioning given in "Quicksort is optimal" by Robert Sedgewick and 
 *	Jon Bentley (with slight adaptations to unsigned indices).
 *
 * Argument:
 *	elements	- Array of words to sort.
 *	proc		- <Col_SortCompareProc>.
 *	first, last	- Range to sort.
 *	clientData	- Opaque data passed as is to above proc.
 *
 * See also:
 *	<Col_MVectorSort>
 *---------------------------------------------------------------------------*/

static void
QuickSort(
    Col_Word *elements, 
    Col_SortCompareProc *proc, 
    size_t first, 
    size_t last, 
    Col_ClientData clientData)
{
    size_t i, j, k, p, q;
    Col_Word v;

#define SWAP(a, b) {Col_Word tmp=b; b=a; a=tmp;}

    /*
     * Entry point for tail recursive calls.
     */

#define TAIL_RECURSE(_first, _last) \
    first = (_first); last = (_last); goto start;

start:

    if (last <= first) return;
    i = p = first; j = q = last;
    v = elements[last];
    for (;;) {
	/*
	 * Move from left to find an element that is not less.
	 */

	while (elements[i] != v && proc(elements[i], v, clientData) < 0) {
	    i++; 
	}

	/*
	 * Move from right to find an element that is not greater.
	 */

	while (v != elements[--j] && proc(v, elements[j], clientData) < 0) {
	    if (j == first) break;
	}

	/*
	 * Stop if pointers have crossed.
	 */

	if (i >= j) break;

	/*
	 * Exchange.
	 */

	SWAP(elements[i], elements[j]);

	/*
	 * If left element equal, exchange to left end.
	 */

	if (elements[i] == v || proc(elements[i], v, clientData) == 0) {
	    SWAP(elements[p], elements[i]); 
	    p++; 
	} 

	/*
	 * If right element equal, exchange to right end.
	 */

	if (v == elements[j] || proc(v, elements[j], clientData) == 0) {
	    q--; 
	    SWAP(elements[j], elements[q]);
	}
    }
    SWAP(elements[i], elements[last]); 
    j = i+1; 

    /*
     * Swap left equals to center.
     */

    if (p) for (k = first; k < p-1; k++, i--) {
	SWAP(elements[k], elements[i]);
    }

    /*
     * Swap right equals to center.
     */

    for (k = last-1; k > q; k--, j++) {
	SWAP(elements[j], elements[k]);
    }

    /*
     * Recurse on smallest subrange and tail call on largest to minimize 
     * recursion depth and ensure O(logn) space.
     */

    if (i-first < last-j+1) {
	/*
	 * Left subrange is shorter.
	 */

	if (i) QuickSort(elements, proc, first, --i, clientData);
	TAIL_RECURSE(j, last);
    } else {
	/*
	 * Right subrange is shorter.
	 */

	QuickSort(elements, proc, j, last, clientData);
	TAIL_RECURSE(first, --i);
    }
}
