/*
 * Vectors implemented as words.
 *
 * Vectors are flat arrays of words that are directly addressable. They come
 * in mutable and immutable versions.
 */

#include "colibri.h"
#include "colInternal.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>


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
    return VECTOR_MAX_LENGTH(SIZE_MAX);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewVector --
 * Col_NewVectorNV --
 *
 *	Create a new vector word.
 *	Col_NewVectorWordV is a variadic version of Col_NewVectorWord.
 *
 * Results:
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

    if (!elements) {
	Col_Error(COL_ERROR, "Element array is NULL");
	return WORD_NIL;
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
    memcpy(WORD_VECTOR_ELEMENTS(vector), elements, length * sizeof(Col_Word));

    return vector;
}

Col_Word
Col_NewVectorNV(
    size_t length,		/* Number of arguments. */
    ...)			/* Remaining arguments, i.e. words to add in 
				 * order. */
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
 *---------------------------------------------------------------------------
 *
 * Col_GetMaxMVectorLength --
 *
 *	Get the maximum length of a mutable vector word.
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
Col_GetMaxMVectorLength()
{
    return VECTOR_MAX_LENGTH(MVECTOR_MAX_SIZE * CELL_SIZE);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewMVector --
 *
 *	Create a new mutable vector word, and optionally populate with the
 *	given elements.
 *
 *	Note that the actual maximum length will be rounded up to fit an even
 *	number of cells.
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
    size_t maxLength,		/* Maximum length of vector. */
    size_t length,		/* Actual length of vector. */
    const Col_Word * elements)	/* Array of words to populate vector with or 
				 * NULL. */
{
    Col_Word mvector;		/* Resulting word in the general case. */
    size_t size;

    /*
     * Quick cases.
     */

    if (maxLength < length) {
	maxLength = length;
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
	     *
	     * Note: no need to declare children as by construction they are older than 
	     * the newly created vector.
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
