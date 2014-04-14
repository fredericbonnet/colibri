/*                                                                              *//*!   @file \
 * colVector.c
 *
 *  This file implements the vector handling features of Colibri.
 *
 *  Vectors are arrays of words that are directly accessible through a
 *  pointer value.
 *
 *  They come in both immutable and mutable forms.
 *
 *  @see colVector.h
 */

#include "include/colibri.h"
#include "colInternal.h"

#include "colWordInt.h"
#include "colVectorInt.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>


/*
================================================================================*//*!   @addtogroup vector_words \
Immutable Vectors                                                               *//*!   @{ *//*
================================================================================
*/

/*******************************************************************************
 * Immutable Vector Creation
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * Col_MaxVectorLength
 *                                                                              *//*!
 *  Get the maximum length of a vector word.
 *
 *  @return
 *    The max vector length.
 *//*-----------------------------------------------------------------------*/

size_t
Col_MaxVectorLength()
{
    return VECTOR_MAX_LENGTH(SIZE_MAX);
}

/*---------------------------------------------------------------------------
 * Col_NewVector
 *                                                                              *//*!
 *  Create a new vector word.
 *
 *  @return
 *      The new word.
 *//*-----------------------------------------------------------------------*/

Col_Word
Col_NewVector(
    size_t length,              /*!< Length of below array. */
    const Col_Word * elements)  /*!< Array of words to populate vector with, or
                                     NULL. In the latter case, elements are
                                     initialized to nil. */
{
    Col_Word vector;            /* Resulting word in the general case. */

    /*
     * Check preconditions.
     */

    VALUECHECK_VECTORLENGTH(length, VECTOR_MAX_LENGTH(SIZE_MAX))                /*!     @valuecheck{COL_ERROR_VECTORLENGTH,length < Col_MaxVectorLength()} */
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
 * Col_NewVectorNV
 *                                                                              *//*!
 *  Create a new vector word from a list of arguments.
 *
 *  @return
 *      The new word.
 *//*-----------------------------------------------------------------------*/

Col_Word
Col_NewVectorNV(
    size_t length,  /*!< Number of arguments following. */
    ...)            /*!< Words to add in order. */
{
    size_t i;
    va_list args;
    Col_Word vector, *elements;

    /*
     * Check preconditions.
     */

    VALUECHECK_VECTORLENGTH(length, VECTOR_MAX_LENGTH(SIZE_MAX))                /*!     @valuecheck{COL_ERROR_VECTORLENGTH,length < Col_MaxVectorLength()} */
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


/*******************************************************************************
 * Immutable Vector Accessors
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * Col_VectorLength
 *                                                                              *//*!
 *  Get the length of the vector.
 *
 *  @return
 *      The vector length.
 *
 *  @see Col_NewVector
 *  @see Col_VectorElements
 *//*-----------------------------------------------------------------------*/

size_t
Col_VectorLength(
    Col_Word vector)    /*!< Vector to get length for. */
{
    /*
     * Check preconditions.
     */

    TYPECHECK_VECTOR(vector) return 0;                                          /*!     @typecheck{COL_ERROR_VECTOR,vector} */

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
 * Col_VectorElements
 *                                                                              *//*!
 *  Get the vector element array.
 *
 *  @return
 *      The vector element array.
 *
 *  @see Col_NewVector
 *  @see Col_VectorLength
 *//*-----------------------------------------------------------------------*/

const Col_Word *
Col_VectorElements(
    Col_Word vector)    /*!< Vector to get elements for. */
{
    /*
     * Check preconditions.
     */

    TYPECHECK_VECTOR(vector) return NULL;                                       /*!     @typecheck{COL_ERROR_VECTOR,vector} */

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

                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup mvector_words \
Mutable Vectors                                                                 *//*!   @{ *//*
================================================================================
*/

/*******************************************************************************
 * Mutable Vector Creation
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * Col_MaxMVectorLength
 *                                                                              *//*!
 *  Get the maximum length of a mutable vector word.
 *
 *  @return
 *      The max vector length.
 *//*-----------------------------------------------------------------------*/

size_t
Col_MaxMVectorLength()
{
    return VECTOR_MAX_LENGTH(MVECTOR_MAX_SIZE * CELL_SIZE);
}

/*---------------------------------------------------------------------------
 * Col_NewMVector
 *                                                                              *//*!
 *  Create a new mutable vector word, and optionally populate with the
 *  given elements.
 *
 *  @note
 *      The actual maximum length will be rounded up to fit an even
 *      number of cells.
 *
 *  @return
 *      The new word.
 *//*-----------------------------------------------------------------------*/

Col_Word
Col_NewMVector(
    size_t maxLength,           /*!< Maximum length of mutable vector. */
    size_t length,              /*!< Length of below array. */
    const Col_Word * elements)  /*!< Array of words to populate vector with, or
                                     NULL. In the latter case, elements are
                                     initialized to nil. */
{
    Col_Word mvector;           /* Resulting word in the general case. */
    size_t size;                /* Number of allocated cells storing a minimum
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

    VALUECHECK_VECTORLENGTH(maxLength,                                          /*!     @valuecheck{COL_ERROR_VECTORLENGTH,maxLength < Col_MaxMVectorLength()} */
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


/*******************************************************************************
 * Mutable Vector Accessors
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * Col_MVectorMaxLength
 *                                                                              *//*!
 *  Get the maximum length of the mutable vector.
 *
 *  @return
 *      The mutable vector maximum length.
 *
 *  @see Col_NewMVector
 *//*-----------------------------------------------------------------------*/

size_t
Col_MVectorMaxLength(
    Col_Word mvector)   /*!< Mutable vector to get maximum length for. */
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MVECTOR(mvector) return 0;                                        /*!     @typecheck{COL_ERROR_MVECTOR,mvector} */

    WORD_UNWRAP(mvector);

    return VECTOR_MAX_LENGTH(WORD_MVECTOR_SIZE(mvector) * CELL_SIZE);
}

/*---------------------------------------------------------------------------
 * Col_MVectorElements
 *                                                                              *//*!
 *  Get the mutable vector element array.
 *
 *  @return
 *      The mutable vector element array.
 *
 *  @see Col_VectorLength
 *//*-----------------------------------------------------------------------*/

Col_Word *
Col_MVectorElements(
    Col_Word mvector)   /*!< Mutable vector to get elements for. */
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MVECTOR(mvector) return NULL;                                     /*!     @typecheck{COL_ERROR_MVECTOR,mvector} */

    WORD_UNWRAP(mvector);

    return WORD_VECTOR_ELEMENTS(mvector);
}


/*******************************************************************************
 * Mutable Vector Operations
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * Col_MVectorSetLength
 *                                                                              *//*!
 *  Resize the mutable vector. Newly added elements are set to nil.
 *//*-----------------------------------------------------------------------*/

void
Col_MVectorSetLength(
    Col_Word mvector,   /*!< Mutable vector to resize. */
    size_t length)      /*!< New length. Must not exceed max length set at
                             creation time. */
{
    size_t maxLength, oldLength;

    /*
     * Check preconditions.
     */

    TYPECHECK_MVECTOR(mvector) return;                                          /*!     @typecheck{COL_ERROR_MVECTOR,mvector} */

    WORD_UNWRAP(mvector);

    maxLength = VECTOR_MAX_LENGTH(WORD_MVECTOR_SIZE(mvector) * CELL_SIZE);
    VALUECHECK_VECTORLENGTH(length, maxLength) return;                          /*!     @valuecheck{COL_ERROR_VECTORLENGTH,length < [Col_MVectorMaxLength(mvector)](@ref Col_MVectorMaxLength).} */

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
 * Col_MVectorFreeze
 *                                                                              *//*!
 *  Turn a mutable vector immutable. Does nothing on immutable vectors.
 *//*-----------------------------------------------------------------------*/

void
Col_MVectorFreeze(
    Col_Word mvector)   /*!< Mutable vector to freeze. */
{
    /*
     * Check preconditions.
     */

    TYPECHECK_VECTOR(mvector) return;                                           /*!     @typecheck{COL_ERROR_MVECTOR,mvector} */

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

                                                                                /*!     @} */
