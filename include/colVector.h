/*
 * Header: colVector.h
 *
 *	This header file defines the vector handling features of Colibri.
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
 *	<colVector.c>
 */

#ifndef _COLIBRI_VECTOR
#define _COLIBRI_VECTOR

#include <stddef.h> /* For size_t */
#include <stdarg.h> /* For variadic procs */


/*
================================================================================
Section: Immutable Vectors
================================================================================
*/

/****************************************************************************
 * Group: Immutable Vector Creation
 *
 * Declarations:
 *	<Col_MaxVectorLength>, <Col_NewVector>, <Col_NewVectorNV>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Macro: Col_NewVectorV
 *
 *	Variadic macro version of <Col_NewVectorNV> that deduces its number
 *	of arguments automatically.
 *
 * See also:
 *	<COL_ARGCOUNT>
 *---------------------------------------------------------------------------*/

#define Col_NewVectorV(...) \
    Col_NewVectorNV(COL_ARGCOUNT(__VA_ARGS__),__VA_ARGS__)

/*
 * Remaining declarations.
 */

EXTERN size_t		Col_MaxVectorLength(void);
EXTERN Col_Word		Col_NewVector(size_t length, 
			    const Col_Word * elements);
EXTERN Col_Word		Col_NewVectorNV(size_t length, ...);


/****************************************************************************
 * Group: Immutable Vector Accessors
 *
 * Declarations:
 *	<Col_VectorLength>, <Col_VectorElements>
 ****************************************************************************/

EXTERN size_t		Col_VectorLength(Col_Word vector);
EXTERN const Col_Word *	Col_VectorElements(Col_Word vector);


/*
================================================================================
Section: Mutable Vectors
================================================================================
*/

/****************************************************************************
 * Group: Mutable Vector Creation
 *
 * Declarations:
 *	<Col_MaxMVectorLength>, <Col_NewMVector>
 ****************************************************************************/

EXTERN size_t		Col_MaxMVectorLength(void);
EXTERN Col_Word		Col_NewMVector(size_t maxLength, size_t length, 
			    const Col_Word * elements);


/****************************************************************************
 * Group: Mutable Vector Accessors
 *
 * Declarations:
 *	<Col_MVectorMaxLength>, <Col_MVectorElements>
 ****************************************************************************/

EXTERN size_t		Col_MVectorMaxLength(Col_Word mvector);
EXTERN Col_Word *	Col_MVectorElements(Col_Word mvector);


/****************************************************************************
 * Group: Mutable Vector Operations
 *
 * Declarations:
 *	<Col_MVectorSetLength>, <Col_MVectorFreeze>, <Col_MVectorSort>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Typedef: Col_SortCompareProc
 *
 *	Function signature of word comparison function. Used for sorting.
 *
 * Arguments:
 *	w1, w1		- Words to compare.
 *	clientData	- Opaque client data. Same value as passed to the
 *			  calling proc.
 *
 * Result:
 *	Negative if w1 is less than w2, positive if w1 is greater than w2, zero
 *	if both words are equal.
 *
 * See also: 
 *	<Col_MVectorSort>
 *---------------------------------------------------------------------------*/

typedef int (Col_SortCompareProc) (Col_Word w1, Col_Word w2,
    Col_ClientData clientData);

/*
 * Remaining declarations.
 */

EXTERN void		Col_MVectorSetLength(Col_Word mvector, size_t length);
EXTERN void		Col_MVectorFreeze(Col_Word mvector);
EXTERN void		Col_MVectorSort(Col_Word mvector, 
			    Col_SortCompareProc *proc, size_t first, 
			    size_t last, Col_ClientData clientData);

#endif /* _COLIBRI_VECTOR */
