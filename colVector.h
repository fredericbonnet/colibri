#ifndef _COLIBRI_VECTOR
#define _COLIBRI_VECTOR

#include <stddef.h> /* For size_t */
#include <stdarg.h> /* For variadic procs */


/* 
 *----------------------------------------------------------------
 * Basic operations. 
 *----------------------------------------------------------------
 */

/* 
 * Vectors are immutable flat arrays of bounded length.
 */

EXTERN size_t		Col_GetMaxVectorLength(void);
EXTERN Col_Word		Col_NewVector(size_t length, 
			    const Col_Word * elements);
EXTERN Col_Word		Col_NewVectorNV(size_t length, ...);
#define Col_NewVectorV(...) \
    Col_NewVectorNV(COL_ARGCOUNT(__VA_ARGS__),__VA_ARGS__)

/*
 * Mutable vectors are flat arrays that can grow up to a maximum length, whose
 * content is directly modifiable through a C array; newly added children need 
 * proper declaration (Col_DeclareChild).
 */

EXTERN size_t		Col_GetMaxMVectorLength(void);
EXTERN Col_Word		Col_NewMVector(size_t maxLength, size_t length, 
			    const Col_Word * elements);
EXTERN void		Col_FreezeMVector(Col_Word mvector);
EXTERN void		Col_MVectorSetLength(Col_Word mvector, size_t length);

#endif /* _COLIBRI_VECTOR */
