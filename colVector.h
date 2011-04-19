/*
 * File: colVector.h
 *
 *	This header file defines the vector handling features of Colibri.
 *
 *	Vectors are arrays of words that are directly accessible through a
 *	pointer value. 
 *
 *	They come in both immutable and mutable forms:
 *
 *	- Immutable vectors are flat arrays of bounded length.
 *
 *	- Mutable vectors are flat arrays that can grow up to a maximum length, 
 *	whose content is directly modifiable through a C pointer. They can be
 *	"frozen" and turned into immutable versions.
 */

#ifndef _COLIBRI_VECTOR
#define _COLIBRI_VECTOR

#include <stddef.h> /* For size_t */
#include <stdarg.h> /* For variadic procs */


/****************************************************************************
 * Group: Immutable Vector Creation
 *
 * Declarations:
 *	<Col_GetMaxVectorLength>, <Col_NewVector>, <Col_NewVectorNV>
 ****************************************************************************/

EXTERN size_t		Col_GetMaxVectorLength(void);
EXTERN Col_Word		Col_NewVector(size_t length, 
			    const Col_Word * elements);
EXTERN Col_Word		Col_NewVectorNV(size_t length, ...);

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


/****************************************************************************
 * Group: Mutable Vector Creation
 *
 * Declarations:
 *	<Col_GetMaxMVectorLength>, <Col_NewMVector>
 ****************************************************************************/

EXTERN size_t		Col_GetMaxMVectorLength(void);
EXTERN Col_Word		Col_NewMVector(size_t maxLength, size_t length, 
			    const Col_Word * elements);


/****************************************************************************
 * Group: Mutable Vector Operations
 *
 * Declarations:
 *	<Col_MVectorSetLength>, <Col_FreezeMVector>
 ****************************************************************************/

EXTERN void		Col_MVectorSetLength(Col_Word mvector, size_t length);
EXTERN void		Col_FreezeMVector(Col_Word mvector);

#endif /* _COLIBRI_VECTOR */
