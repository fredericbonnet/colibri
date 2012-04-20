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
================================================================================
Section: Immutable Vectors
================================================================================
*/

/****************************************************************************
 * Group: Immutable Vector Creation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_GetMaxVectorLength
 *
 *	Get the maximum length of a vector word.
 *
 * Result:
 *	The max vector length.
 *---------------------------------------------------------------------------*/

size_t
Col_GetMaxVectorLength()
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

    if (length > VECTOR_MAX_LENGTH(SIZE_MAX)) {
	/*
	 * Too large.
	 */

	Col_Error(COL_ERROR, 
		"Length %u exceeds the maximum allowed value for vectors (%u)", 
		length, VECTOR_MAX_LENGTH(SIZE_MAX));
	return WORD_NIL;
    }

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

    if (length == 0) {
	/* 
	 * Use immediate value.
	 */

	return WORD_LIST_EMPTY;
    }
    
    if (length > VECTOR_MAX_LENGTH(SIZE_MAX)) {
	/*
	 * Too large.
	 */

	Col_Error(COL_ERROR, 
		"Length %u exceeds the maximum allowed value for vectors (%u)", 
		length, VECTOR_MAX_LENGTH(SIZE_MAX));
	return WORD_NIL;
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

    return vector;
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
 * Function: Col_GetMaxMVectorLength
 *
 *	Get the maximum length of a mutable vector word.
 *
 * Result:
 *	The max vector length.
 *---------------------------------------------------------------------------*/

size_t
Col_GetMaxMVectorLength()
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
     * Quick cases.
     */

    if (maxLength < length) {
	maxLength = length;
    }
    if (maxLength == 0) {
	/* 
	 * Mutable vector will always be empty. Use immediate value.
	 */

	return WORD_LIST_EMPTY;
    }
    if (maxLength > VECTOR_MAX_LENGTH(MVECTOR_MAX_SIZE * CELL_SIZE)) {
	/*
	 * Too large.
	 */

	Col_Error(COL_ERROR, 
		"Length %u exceeds the maximum allowed value for mutable vectors (%u)", 
		maxLength, VECTOR_MAX_LENGTH(MVECTOR_MAX_SIZE) * CELL_SIZE);
	return WORD_NIL;
    }

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
 *---------------------------------------------------------------------------*/

void
Col_MVectorSetLength(
    Col_Word mvector,
    size_t length)
{
    size_t maxLength, oldLength;

    if (WORD_TYPE(mvector) != WORD_TYPE_MVECTOR) {
	/*
	 * Invalid type.
	 */

	Col_Error(COL_ERROR, "%x is not a mutable vector", mvector);
	return;
    }

    maxLength = VECTOR_MAX_LENGTH(WORD_MVECTOR_SIZE(mvector) * CELL_SIZE);
    if (length > maxLength) {
	/*
	 * Too large.
	 */

	Col_Error(COL_ERROR, 
		"Length %u exceeds the maximum value for this mutable vector (%u)", 
		length, maxLength);
	return;
    }

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
 *---------------------------------------------------------------------------*/

void
Col_MVectorFreeze(
    Col_Word mvector)
{
    switch (WORD_TYPE(mvector)) {
	case WORD_TYPE_VECTOR:
	    /*
	     * No-op.
	     */

	    break;

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
	    break;
	}

	default:
	    Col_Error(COL_ERROR, "%x is not a mutable vector", mvector);
    }
}
