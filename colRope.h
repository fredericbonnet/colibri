#ifndef _COLIBRI_ROPE
#define _COLIBRI_ROPE

#include <stddef.h> /* For size_t */
#include <stdarg.h> /* For variadic procs */


/*
 * UTF-8 utility procs & macros.
 */

EXTERN const char *	Col_Utf8CharAddr(const char * data, 
			    size_t index, size_t length, size_t byteLength);
EXTERN const char *	Col_Utf8CharNext(const char * data);
EXTERN const char *	Col_Utf8CharPrevious(const char * data);
EXTERN Col_Char		Col_Utf8CharAt(const char * data);

#define COL_UTF8_NEXT(data) \
    while ((*++(data) & 0xC0) == 0x80);

#define COL_UTF8_PREVIOUS(data) \
    while ((*--(data) & 0xC0) == 0x80);


/* 
 *----------------------------------------------------------------
 * Basic operations. 
 *----------------------------------------------------------------
 */

/*
 * Empty rope.
 */

EXTERN Col_Word		Col_EmptyRope();

/* 
 * Creating from a C string. 
 */

EXTERN Col_Word		Col_NewRopeFromString(const char *string);

/* 
 * Creating from a regular, flat buffer. 
 */

EXTERN Col_Word		Col_NewRope(Col_StringFormat format, const void *data, 
			    size_t byteLength);

/* 
 * Length in characters. 
 */

EXTERN size_t		Col_RopeLength(Col_Word rope);

/* 
 * Subrope. 
 */

EXTERN Col_Word		Col_Subrope(Col_Word rope, size_t first, size_t last);

/* 
 * Concatenation of several ropes. 
 */

EXTERN Col_Word		Col_ConcatRopes(Col_Word left, Col_Word right);
EXTERN Col_Word		Col_ConcatRopesA(size_t number, const Col_Word * ropes);
EXTERN Col_Word		Col_ConcatRopesNV(size_t number, ...);
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

EXTERN Col_Char		Col_RopeAt(Col_Word rope, size_t index);

/* 
 * Repetition of a given rope. 
 */

EXTERN Col_Word		Col_RepeatRope(Col_Word rope, size_t count);

/* 
 * Insertion/removal. 
 */

EXTERN Col_Word		Col_RopeInsert(Col_Word into, size_t index, 
			    Col_Word rope);
EXTERN Col_Word		Col_RopeRemove(Col_Word rope, size_t first, 
			    size_t last);
EXTERN Col_Word		Col_RopeReplace(Col_Word rope, size_t first, 
			    size_t last, Col_Word with);

/*
 * Comparison.
 */

EXTERN int		Col_CompareRopes(Col_Word rope1, Col_Word rope2,
			    size_t start, size_t max, size_t *posPtr, 
			    Col_Char *c1Ptr, Col_Char *c2Ptr);


/* 
 *----------------------------------------------------------------
 * Iteration and traversal. 
 *----------------------------------------------------------------
 */

/* 
 * Iteration over the basic chunks of data. 
 */

typedef struct Col_RopeChunk {
    Col_StringFormat format;
    const void *data;
    size_t byteLength;
} Col_RopeChunk;
typedef int (Col_RopeChunksTraverseProc) (size_t index, size_t length, 
	size_t number, const Col_RopeChunk *chunks, Col_ClientData clientData);
EXTERN int		Col_TraverseRopeChunks(size_t number, Col_Word *ropes, 
			    size_t start, size_t max, 
			    Col_RopeChunksTraverseProc *proc, 
			    Col_ClientData clientData, size_t *lengthPtr);

/*
 * Iteration over individual characters. Each iterator takes 9 words on the
 * stack.
 */

typedef Col_Char (ColRopeIterLeafAtProc) (Col_Word leaf, size_t index);
typedef struct Col_RopeIterator {
    Col_Word rope;		/* Rope being iterated. */
    size_t index;		/* Current position. */
    struct {			/* Traversal info: */
	Col_Word subrope;	/*  - Traversed subrope. */
	size_t first, last;	/*  - Range of validity. */
	size_t offset;		/*  - Index offset wrt. root. */
	Col_Word leaf;		/*  - Leaf (deepest) rope. */ 
	size_t index;		/*  - Index within leaf (byte index for variable width format). */
	ColRopeIterLeafAtProc *proc;
				/*  - Access proc within leaf. */
    } traversal;
} Col_RopeIterator;

EXTERN void		ColRopeIterUpdateTraversalInfo(Col_RopeIterator *it);

#define Col_RopeIterEnd(it)	(!(it)->rope)
#define Col_RopeIterRope(it)	((it)->rope)
#define Col_RopeIterIndex(it)	((it)->index)
#define Col_RopeIterAt(it)	(!(it)->traversal.leaf?ColRopeIterUpdateTraversalInfo(it),0:0,(it)->traversal.proc((it)->traversal.leaf, (it)->traversal.index))
	
EXTERN void		Col_RopeIterBegin(Col_Word rope, size_t index, 
			    Col_RopeIterator *it);
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

typedef size_t (Col_RopeLengthProc) (Col_Word rope);
typedef Col_Char (Col_RopeCharAtProc) (Col_Word rope, size_t index);
typedef void (Col_RopeChunkAtProc) (Col_Word rope, size_t start, size_t max, 
    size_t *lengthPtr, Col_RopeChunk *chunkPtr);
typedef Col_Word (Col_RopeSubropeProc) (Col_Word rope, size_t first, 
    size_t last);
typedef Col_Word (Col_RopeConcatProc) (Col_Word left, Col_Word right);

typedef struct Col_CustomRopeType {
    Col_CustomWordType type;	/* Must be first, and type field must be 
				 * COL_ROPE. */

    Col_RopeLengthProc *lengthProc;
				/* Called to get the length of the rope. Must 
				 * be constant during the whole lifetime. */
    Col_RopeCharAtProc *charAtProc;	
				/* Called to get the character at a given 
				 * position. Must be constant during the whole
				 * lifetime. */
    Col_RopeChunkAtProc *chunkAtProc;
				/* Called during traversal. If NULL, traversal
				 * is done per character. */
    Col_RopeSubropeProc *subropeProc;
				/* Called to extract subrope. If NULL, or if
				 * it returns nil, use the standard procedure.
				 */
    Col_RopeConcatProc *concatProc;
				/* Called to concat ropes. If NULL, or if it 
				 * returns nil, use the standard procedure. */
} Col_CustomRopeType;

#endif /* _COLIBRI_ROPE */
