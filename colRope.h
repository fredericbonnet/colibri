#ifndef _COLIBRI_ROPE
#define _COLIBRI_ROPE

#include <stddef.h> /* For size_t */
#include <stdarg.h> /* For variadic procs */


/* 
 * Colibri ropes are a superset of C strings.
 */

typedef const char * Col_Rope;

/*
 * String characters use the 32-bit Unicode encoding.
 */

typedef unsigned int Col_Char;
#define COL_CHAR_INVALID	((Col_Char)-1)

/* 
 * Strings can use various formats. 
 *
 * Note: we assume that UTF-8 data is always well-formed. It is up to the 
 * caller responsibility to validate and ensure well-formedness of UTF-8 data, 
 * notably for security reasons. 
 */

typedef enum Col_StringFormat {
    COL_UCS1, COL_UCS2, COL_UCS4, COL_UTF8
} Col_StringFormat;

typedef uint8_t Col_Char1;
typedef uint16_t Col_Char2;
typedef uint32_t Col_Char4;

/* 
 * Empty C strings cannot be used as is by ropes, use this constant instead, 
 * which containts 2 NUL terminators instead of one. 
 */

#define STRING_EMPTY		"\0"


/* 
 *----------------------------------------------------------------
 * Basic operations. 
 *----------------------------------------------------------------
 */

/* 
 * Creating from a regular, flat buffer. 
 */

EXTERN Col_Rope		Col_NewRope(Col_StringFormat format, const void *data, 
			    size_t byteLength);

/* 
 * Length in characters. 
 */

EXTERN size_t		Col_RopeLength(Col_Rope rope);

/* 
 * Subrope. 
 */

EXTERN Col_Rope		Col_Subrope(Col_Rope rope, size_t first, size_t last);

/* 
 * Concatenation of several ropes. 
 */

EXTERN Col_Rope		Col_ConcatRopes(Col_Rope left, Col_Rope right);
EXTERN Col_Rope		Col_ConcatRopesA(size_t number, const Col_Rope * ropes);
EXTERN Col_Rope		Col_ConcatRopesNV(size_t number, ...);
#define Col_ConcatRopesV(...) \
    Col_ConcatRopesNV(COL_ARGCOUNT(__VA_ARGS__),__VA_ARGS__)

/*
 *----------------------------------------------------------------
 * High level operations.
 *----------------------------------------------------------------
 */

/* 
 * Accessing. 
 */

EXTERN Col_Char		Col_RopeAt(Col_Rope rope, size_t index);

/* 
 * Repetition of a given rope. 
 */

EXTERN Col_Rope		Col_RepeatRope(Col_Rope rope, size_t count);

/* 
 * Insertion/removal. 
 */

EXTERN Col_Rope		Col_RopeInsert(Col_Rope into, size_t index, 
			    Col_Rope rope);
EXTERN Col_Rope		Col_RopeRemove(Col_Rope rope, size_t first, 
			    size_t last);
EXTERN Col_Rope		Col_RopeReplace(Col_Rope rope, size_t first, 
			    size_t last, Col_Rope with);


/* 
 *----------------------------------------------------------------
 * Iteration and traversal. 
 *----------------------------------------------------------------
 */

/* 
 * Iteration over the basic chunks of data. 
 */

typedef int (Col_RopeChunkEnumProc) (Col_StringFormat format, const void *data, 
	size_t length, size_t byteLength, Col_ClientData clientData);

EXTERN int		Col_TraverseRopeChunks(Col_Rope rope, size_t start, 
			    size_t max, Col_RopeChunkEnumProc *proc, 
			    Col_ClientData clientData, size_t *lengthPtr);

/*
 * Iteration over individual characters. Each iterator takes 8 words on the
 * stack.
 */

typedef struct Col_RopeIterator {
    Col_Rope rope;		/* Rope being iterated. */
    size_t index;		/* Current position. */
    struct {			/* Traversal info: */
	Col_Rope subrope;	/*  - Traversed subrope. */
	size_t first, last;	/*  - Range of validity. */
	size_t offset;		/*  - Index offset wrt. root. */
	Col_Rope leaf;		/*  - Leaf (deepest) rope. */ 
	union {			/*  - Index within leaf: */
	    size_t fixed;	/*     - fixed width formats (UCS, C string). */
	    struct {unsigned short c, b;} var;
				/*     - variable width formats (UTF-8). */
	} index;
    } traversal;
} Col_RopeIterator;

#define Col_RopeIterEnd(it)	(!(it)->rope)
#define Col_RopeIterRope(it)	((it)->rope)
#define Col_RopeIterIndex(it)	((it)->index)
	
EXTERN void		Col_RopeIterBegin(Col_Rope rope, size_t index, 
			    Col_RopeIterator *it);
EXTERN Col_Char		Col_RopeIterAt(Col_RopeIterator *it);
EXTERN int		Col_RopeIterCompare(Col_RopeIterator *it1, 
			    Col_RopeIterator *it2);
EXTERN void		Col_RopeIterMoveTo(Col_RopeIterator *it, size_t index);
EXTERN void		Col_RopeIterForward(Col_RopeIterator *it, size_t nb);
EXTERN void		Col_RopeIterBackward(Col_RopeIterator *it, size_t nb);

#define Col_RopeIterNext(it)	Col_RopeIterForward((it), 1)
#define Col_RopeIterPrevious(it) Col_RopeIterBackward((it), 1)


/* 
 *----------------------------------------------------------------
 * Custom rope types. 
 *----------------------------------------------------------------
 */

/*
 * Function types and structures for custom rope types.
 */

typedef size_t (Col_RopeCustomLengthProc) (Col_Rope rope);
typedef Col_Char (Col_RopeCustomCharAtProc) (Col_Rope rope, size_t index);
typedef void (Col_RopeCustomFreeProc) (Col_Rope rope);
typedef Col_Rope (Col_RopeCustomSubropeProc) (Col_Rope rope, size_t first, 
    size_t last);
typedef Col_Rope (Col_RopeCustomConcatProc) (Col_Rope left, Col_Rope right);
typedef int (Col_RopeCustomTraverseProc) (Col_Rope rope, size_t start, 
    size_t max, Col_RopeChunkEnumProc *proc, Col_ClientData clientData, 
    size_t *lengthPtr);
typedef Col_Rope (Col_RopeCustomChildAtProc) (Col_Rope rope, size_t index, 
    size_t *offsetPtr);

typedef void (Col_RopeCustomChildEnumProc) (Col_Rope rope, Col_Rope *childPtr, 
    Col_ClientData clientData);
typedef void (Col_RopeCustomChildrenProc) (Col_Rope rope, 
    Col_RopeCustomChildEnumProc *proc, Col_ClientData clientData);

typedef struct Col_RopeCustomType {
    const char *name;		/* Name of the type, e.g. "int". */
    Col_RopeCustomLengthProc *lengthProc;
				/* Called to get the length of the rope. Must 
				 * be constant during the whole lifetime. */
    Col_RopeCustomCharAtProc *charAtProc;	
				/* Called to get the character at a given 
				 * position. Must be constant during the whole
				 * lifetime. */
    Col_RopeCustomFreeProc *freeProc;
				/* Called once the rope gets collected. NULL if
				 * not necessary. */
    Col_RopeCustomSubropeProc *subropeProc;
				/* Called to extract subrope. If NULL, or if
				 * it returns NULL, use the standard procedure.
				 */
    Col_RopeCustomConcatProc *concatProc;
				/* Called to concat ropes. If NULL, or if it 
				 * returns NULL, use the standard procedure. */
    Col_RopeCustomTraverseProc *traverseProc;
				/* Called during traversal. If NULL, traversal
				 * is done per character. */
    Col_RopeCustomChildAtProc *childAtProc;
				/* Called to get the child rope at a given 
				 * index. Used by iterators. If NULL, or if it 
				 * returns NULL, the rope is considered a leaf 
				 * that is directly addressable. */
    Col_RopeCustomChildrenProc *childrenProc;
				/* Called to iterate over the ropes owned by
				 * the custom rope, in no special order. Called
				 * during the garbage collection. Ropes are
				 * movable, so pointer values may be modified 
				 * in the process. If NULL, do nothing. */
} Col_RopeCustomType;

/*
 * Custom rope creation.
 */

EXTERN Col_Rope		Col_NewCustomRope(Col_RopeCustomType *type, 
			    size_t size, void **dataPtr);

/*
 * Custom rope value.
 */

EXTERN Col_RopeCustomType * Col_GetCustomRopeInfo(Col_Rope rope, 
			    size_t *sizePtr, void **dataPtr);


/* 
 *----------------------------------------------------------------
 * Lifetime management. 
 *----------------------------------------------------------------
 */

EXTERN Col_Rope		Col_PreserveRope(Col_Rope rope);
EXTERN Col_Rope		Col_ReleaseRope(Col_Rope rope);

#endif /* _COLIBRI_ROPE */
