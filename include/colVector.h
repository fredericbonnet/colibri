/**
 * @file colVector.h
 *
 * This header file defines the vector handling features of Colibri.
 *
 * Vectors are arrays of words that are directly accessible through a
 * pointer value.
 *
 * They come in both immutable and mutable forms.
 */

#ifndef _COLIBRI_VECTOR
#define _COLIBRI_VECTOR

#include <stddef.h> /* For size_t */
#include <stdarg.h> /* For variadic procs */


/*
===========================================================================*//*!
\defgroup vector_words Immutable Vectors
\ingroup words

Immutable vectors are constant, fixed-length arrays of words that are directly
accessible through a pointer value.
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Immutable Vector Creation
 ***************************************************************************\{*/

/**
 * Variadic macro version of Col_NewVectorNV() that deduces its number
 * of arguments automatically.
 *
 * @param first First word to add.
 * @param ...   Next words to add.
 * 
 * @see COL_ARGCOUNT
 */
#define Col_NewVectorV(first, ...) \
    _Col_NewVectorV(_, first, ##__VA_ARGS__)
/*! \cond IGNORE */
#define _Col_NewVectorV(_, ...) \
    Col_NewVectorNV(COL_ARGCOUNT(__VA_ARGS__),__VA_ARGS__)
/*! \endcond *//* IGNORE */

/*
 * Remaining declarations.
 */

EXTERN size_t           Col_MaxVectorLength(void);
EXTERN Col_Word         Col_NewVector(size_t length,
                            const Col_Word * elements);
EXTERN Col_Word         Col_NewVectorNV(size_t length, ...);

/* End of Immutable Vector Creation *//*!\}*/


/***************************************************************************//*!
 * \name Immutable Vector Accessors
 ***************************************************************************\{*/

EXTERN size_t           Col_VectorLength(Col_Word vector);
EXTERN const Col_Word * Col_VectorElements(Col_Word vector);

/* End of Immutable Vector Accessors *//*!\}*/

/* End of Immutable Vectors *//*!\}*/


/*
===========================================================================*//*!
\defgroup mvector_words Mutable Vectors
\ingroup words

Mutable vectors are arrays of words that are directly accessible and
modifiable through a pointer value, and whose length can vary up to a
given capacity set at creation time; they can be"frozen" and turned into
immutable vectors.
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Mutable Vector Creation
 ***************************************************************************\{*/

EXTERN size_t           Col_MaxMVectorLength(void);
EXTERN Col_Word         Col_NewMVector(size_t capacity, size_t length,
                            const Col_Word * elements);

/* End of Mutable Vector Creation *//*!\}*/


/***************************************************************************//*!
 * \name Mutable Vector Accessors
 ***************************************************************************\{*/

EXTERN size_t           Col_MVectorCapacity(Col_Word mvector);
EXTERN Col_Word *       Col_MVectorElements(Col_Word mvector);

/* End of Mutable Vector Accessors *//*!\}*/


/***************************************************************************//*!
 * \name Mutable Vector Operations
 ***************************************************************************\{*/

EXTERN void             Col_MVectorSetLength(Col_Word mvector, size_t length);
EXTERN void             Col_MVectorFreeze(Col_Word mvector);

/* End of Mutable Vector Operations *//*!\}*/

/* End of Mutable Vectors *//*!\}*/

#endif /* _COLIBRI_VECTOR */
