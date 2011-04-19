/*
 * File: colRope.h
 *
 *	This header file defines the rope handling features of Colibri.
 *
 *	Ropes are a string datatype that allows for fast insertion, extraction
 *	and composition of strings. Internally they use self-balanced binary 
 *	trees.
 *
 *	They are always immutable.
 */

#ifndef _COLIBRI_ROPE
#define _COLIBRI_ROPE

#include <stddef.h> /* For size_t */
#include <stdarg.h> /* For variadic procs */


/****************************************************************************
 * Group: Rope Creation
 *
 * Declarations:
 *	<Col_EmptyRope>, <Col_NewRopeFromString>, <Col_NewRope>
 ****************************************************************************/

EXTERN Col_Word		Col_EmptyRope();
EXTERN Col_Word		Col_NewRopeFromString(const char *string);
EXTERN Col_Word		Col_NewRope(Col_StringFormat format, const void *data, 
			    size_t byteLength);

/****************************************************************************
 * Group: Rope Access and Comparison
 *
 * Declarations:
 *	<Col_RopeLength>, <Col_RopeAt>, <Col_CompareRopes>
 ****************************************************************************/

EXTERN size_t		Col_RopeLength(Col_Word rope);
EXTERN Col_Char		Col_RopeAt(Col_Word rope, size_t index);
EXTERN int		Col_CompareRopes(Col_Word rope1, Col_Word rope2,
			    size_t start, size_t max, size_t *posPtr, 
			    Col_Char *c1Ptr, Col_Char *c2Ptr);

/****************************************************************************
 * Group: Rope Operations
 *
 * Declarations:
 *	<Col_Subrope>, <Col_ConcatRopes>, <Col_ConcatRopesA>,
 *	<Col_ConcatRopesNV>, <Col_RepeatRope>, <Col_RopeInsert>, 
 *	<Col_RopeRemove>, <Col_RopeReplace>
 ****************************************************************************/

EXTERN Col_Word		Col_Subrope(Col_Word rope, size_t first, size_t last);
EXTERN Col_Word		Col_ConcatRopes(Col_Word left, Col_Word right);
EXTERN Col_Word		Col_ConcatRopesA(size_t number, const Col_Word * ropes);
EXTERN Col_Word		Col_ConcatRopesNV(size_t number, ...);
EXTERN Col_Word		Col_RepeatRope(Col_Word rope, size_t count);
EXTERN Col_Word		Col_RopeInsert(Col_Word into, size_t index, 
			    Col_Word rope);
EXTERN Col_Word		Col_RopeRemove(Col_Word rope, size_t first, 
			    size_t last);
EXTERN Col_Word		Col_RopeReplace(Col_Word rope, size_t first, 
			    size_t last, Col_Word with);

/*---------------------------------------------------------------------------
 * Macro: Col_ConcatRopesV
 *
 *	Variadic macro version of <Col_ConcatRopesNV> that deduces its number
 *	of arguments automatically.
 *
 * See also:
 *	<COL_ARGCOUNT>
 *---------------------------------------------------------------------------*/

#define Col_ConcatRopesV(...) \
    Col_ConcatRopesNV(COL_ARGCOUNT(__VA_ARGS__),__VA_ARGS__)


/****************************************************************************
 * Group: Rope Traversal
 *
 * Declarations:
 *	<Col_TraverseRopeChunks>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Typedef: Col_RopeChunk
 *
 *	Describes a rope chunk encountered during traversal. A chunk is a 
 *	contiguous portion of rope with a given format.
 *
 * Fields:
 *	format		- Format of the traversed chunk. See <Col_StringFormat>.
 *	data		- Pointer to format-specific data.
 *	byteLength	- Data length in bytes.
 *
 * See also: 
 *	<Col_TraverseRopeChunks>, <Col_RopeChunksTraverseProc>
 *---------------------------------------------------------------------------*/

typedef struct Col_RopeChunk {
    Col_StringFormat format;
    const void *data;
    size_t byteLength;
} Col_RopeChunk;

/*---------------------------------------------------------------------------
 * Typedef: Col_RopeChunksTraverseProc
 *
 *	Function signature of rope traversal procs.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks.
 *	chunks		- Array of chunks. When chunk is NULL, means the
 *			  index is past the end of the traversed rope.
 *	clientData	- Opaque client data. Same value as passed to 
 *			  <Col_TraverseRopeChunks>
 *
 * Returns:
 *	If non-zero, interrupts the traversal.
 *
 * See also: 
 *	<Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

typedef int (Col_RopeChunksTraverseProc) (size_t index, size_t length, 
	size_t number, const Col_RopeChunk *chunks, Col_ClientData clientData);

EXTERN int		Col_TraverseRopeChunks(size_t number, Col_Word *ropes, 
			    size_t start, size_t max, 
			    Col_RopeChunksTraverseProc *proc, 
			    Col_ClientData clientData, size_t *lengthPtr);


/****************************************************************************
 * Group: Rope Iterators
 *
 * Declarations: 
 *	<Col_RopeIterBegin>, <Col_RopeIterCompare>, <Col_RopeIterMoveTo>, 
 *	<Col_RopeIterForward>, <Col_RopeIterBackward>, 
 *	<ColRopeIterUpdateTraversalInfo>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Typedef: ColRopeIterLeafAtProc
 *
 *	Helper for rope iterators to access characters in leaves.
 *
 * Arguments:
 *	leaf	- Leaf node.
 *	index	- Leaf-relative index of character.
 *
 * Result:
 *	Character at given index.
 *
 * See also:
 *	<ColRopeIterator>
 *---------------------------------------------------------------------------*/

typedef Col_Char (ColRopeIterLeafAtProc) (Col_Word leaf, size_t index);

/*---------------------------------------------------------------------------
 * Internal Typedef: ColRopeIterator
 *
 *	Internal implementation of rope iterators.
 *
 * Fields:
 *	rope			- Rope being iterated.
 *	index			- Current position.
 *	traversal		- Traversal info:
 *	traversal.subrope	- Traversed subrope.
 *	traversal.(first, last)	- Range of validity.
 *	traversal.offset	- Index offset wrt. root.
 *	traversal.leaf		- Leaf (deepest) rope.
 *	traversal.index		- Index within leaf (byte index for variable 
 *				  width format).
 *	traversal.proc		- Access proc within leaf.
 *
 * See also:
 *	<Col_RopeIterator>
 *---------------------------------------------------------------------------*/

typedef struct ColRopeIterator {
    Col_Word rope;
    size_t index;
    struct {
	Col_Word subrope;
	size_t first, last;
	size_t offset;
	Col_Word leaf;
	size_t index;
	ColRopeIterLeafAtProc *proc;
    } traversal;
} ColRopeIterator;

/*---------------------------------------------------------------------------
 * Typedef: Col_RopeIterator
 *
 *	Rope iterator. Encapsulates the necessary info to iterate & access rope
 *	data transparently.
 *
 * Note:
 *	Datatype is opaque. Fields should not be accessed by client code.
 *
 *	Each iterator takes 9 words on the stack.
 *---------------------------------------------------------------------------*/

typedef ColRopeIterator Col_RopeIterator;

/*---------------------------------------------------------------------------
 * Macro: Col_RopeIterEnd
 *
 *	Test whether iterator reached end of rope.
 *
 * Arguments:
 *	it	- The iterator to test.
 *
 * Result:
 *	Non-zero if iterator is at end.
 *
 * See also: 
 *	<Col_RopeIterator>, <Col_RopeIterBegin>
 *---------------------------------------------------------------------------*/

#define Col_RopeIterEnd(it)	\
    (!(it)->rope)

/*---------------------------------------------------------------------------
 * Macro: Col_RopeIterRope
 *
 *	Get rope for iterator.
 *
 * Arguments:
 *	it	- The iterator to get rope for.
 *
 * Result:
 *	The rope, or <WORD_NIL> if at end.
 *
 * See also: 
 *	<Col_RopeIterator>, <Col_RopeIterEnd>
 *---------------------------------------------------------------------------*/

#define Col_RopeIterRope(it)	\
    ((it)->rope)

/*---------------------------------------------------------------------------
 * Macro: Col_RopeIterIndex
 *
 *	Get current index within rope for iterator.
 *
 * Arguments:
 *	it	- The iterator to get index for.
 *
 * Result:
 *	Current index. Undefined if at end.
 *
 * See also: 
 *	<Col_RopeIterator>
 *---------------------------------------------------------------------------*/

#define Col_RopeIterIndex(it) \
    ((it)->index)

/*---------------------------------------------------------------------------
 * Macro: Col_RopeIterAt
 *
 *	Get current rope character for iterator.
 *
 * Arguments:
 *	it	- The iterator to get character for.
 *
 * Result:
 *	Current character. Undefined if at end.
 *
 * Side effects:
 *	The argument is referenced several times by the macro. Make sure to
 *	avoid any side effect.
 *
 * See also: 
 *	<Col_RopeIterator>
 *---------------------------------------------------------------------------*/

#define Col_RopeIterAt(it) \
    (!(it)->traversal.leaf?ColRopeIterUpdateTraversalInfo(it),0:0,(it)->traversal.proc((it)->traversal.leaf, (it)->traversal.index))

/*---------------------------------------------------------------------------
 * Macro: Col_RopeIterNext
 *
 *	Move the iterator to the next character.
 *
 * Argument:
 *	it	- The iterator to move.
 *
 * Side effects:
 *	Update the iterator.
 *
 * See also: 
 *	<Col_RopeIterForward>
 *---------------------------------------------------------------------------*/

#define Col_RopeIterNext(it) \
    Col_RopeIterForward((it), 1)

/*---------------------------------------------------------------------------
 * Macro: Col_RopeIterPrevious
 *
 *	Move the iterator to the previous character.
 *
 * Argument:
 *	it	- The iterator to move.
 *
 * Side effects:
 *	Update the iterator.
 *
 * See also: 
 *	<Col_RopeIterBackward>
 *---------------------------------------------------------------------------*/

#define Col_RopeIterPrevious(it) \
    Col_RopeIterBackward((it), 1)

EXTERN void		Col_RopeIterBegin(Col_Word rope, size_t index, 
			    Col_RopeIterator *it);
EXTERN int		Col_RopeIterCompare(Col_RopeIterator *it1, 
			    Col_RopeIterator *it2);
EXTERN void		Col_RopeIterMoveTo(Col_RopeIterator *it, size_t index);
EXTERN void		Col_RopeIterForward(Col_RopeIterator *it, size_t nb);
EXTERN void		Col_RopeIterBackward(Col_RopeIterator *it, size_t nb);

EXTERN void		ColRopeIterUpdateTraversalInfo(Col_RopeIterator *it);


/****************************************************************************
 * Group: Custom Ropes
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Typedef: Col_RopeLengthProc
 *
 *	Function signature of custom rope length procs.
 *
 * Arguments:
 *	rope	- Custom rope to get length for.
 *
 * Result:
 *	The custom rope length.
 *
 * See also: 
 *	<Col_CustomRopeType>
 *---------------------------------------------------------------------------*/

typedef size_t (Col_RopeLengthProc) (Col_Word rope);

/*---------------------------------------------------------------------------
 * Typedef: Col_RopeCharAtProc
 *
 *	Function signature of custom rope character access procs.
 *
 * Arguments:
 *	rope	- Custom rope to get character from.
 *	index	- Character index.
 *
 * Note:
 *	By construction, indices are guaranteed to be within valid range.
 *
 * Result:
 *	The Unicode codepoint of the character at the given index.
 *
 * See also: 
 *	<Col_CustomRopeType>
 *---------------------------------------------------------------------------*/

typedef Col_Char (Col_RopeCharAtProc) (Col_Word rope, size_t index);

/*---------------------------------------------------------------------------
 * Typedef: Col_RopeChunkAtProc
 *
 *	Function signature of custom rope chunk access procs.
 *
 * Arguments:
 *	rope		- Custom rope to get chunk from.
 *	start		- Start index of chunk.
 *	max		- Maximum length of chunk.
 *	lengthPtr	- Holds actual length upon return.
 *	chunkPtr	- Holds chunk information upon return. See 
 *			  <Col_RopeChunk>.
 *
 * Note:
 *	By construction, indices are guaranteed to be within valid range.
 *
 * Result:
 *	The Unicode codepoint of the character at the given index.
 *
 * See also: 
 *	<Col_CustomRopeType>
 *---------------------------------------------------------------------------*/

typedef void (Col_RopeChunkAtProc) (Col_Word rope, size_t start, size_t max, 
    size_t *lengthPtr, Col_RopeChunk *chunkPtr);

/*---------------------------------------------------------------------------
 * Typedef: Col_RopeSubropeProc
 *
 *	Function signature of custom rope subrope extraction.
 *
 * Arguments:
 *	rope		- Custom rope to extract subrope from.
 *	first, last	- Range of subrope to extract (inclusive).
 *
 * Note:
 *	By construction, indices are guaranteed to be within valid range.
 *
 * Result:
 *	A rope representing the subrope, or nil.
 *
 * See also: 
 *	<Col_CustomRopeType>
 *---------------------------------------------------------------------------*/

typedef Col_Word (Col_RopeSubropeProc) (Col_Word rope, size_t first, 
    size_t last);

/*---------------------------------------------------------------------------
 * Typedef: Col_RopeConcatProc
 *
 *	Function signature of custom rope concatenation.
 *
 * Arguments:
 *	left, right	- Ropes to concatenate.
 *
 * Result:
 *	A rope representing the concatenation of both ropes, or nil.
 *
 * See also: 
 *	<Col_CustomRopeType>
 *---------------------------------------------------------------------------*/

typedef Col_Word (Col_RopeConcatProc) (Col_Word left, Col_Word right);

/*---------------------------------------------------------------------------
 * Typedef: Col_CustomRopeType
 *
 *	Custom rope type descriptor. Inherits from <Col_CustomWordType>.
 *
 * Fields:
 *	type		- Generic word type descriptor. Type field must be equal
 *			  to <COL_ROPE>.
 *	lengthProc	- Called to get the length of the rope. Must be constant
 *			  during the whole lifetime.
 *	charAtProc	- Called to get the character at a given position. Must
 *			  be constant during the whole lifetime.
 *	chunkAtProc	- Called during traversal. If NULL, traversal is done 
 *			  per character.
 *	subropeProc	- Called to extract subrope. If NULL, or if it returns 
 *			  nil, use the standard procedure.
 *	concatProc	- Called to concat ropes. If NULL, or if it returns nil,
 *			  use the standard procedure.
 *
 * See also:
 *	<Col_CustomWordType>, <Col_RopeLengthProc>, <Col_RopeCharAtProc>, 
 *	<Col_RopeChunkAtProc>, <Col_RopeSubropeProc>, <Col_RopeConcatProc>
 *---------------------------------------------------------------------------*/

typedef struct Col_CustomRopeType {
    Col_CustomWordType type;
    Col_RopeLengthProc *lengthProc;
    Col_RopeCharAtProc *charAtProc;
    Col_RopeChunkAtProc *chunkAtProc;
    Col_RopeSubropeProc *subropeProc;
    Col_RopeConcatProc *concatProc;
} Col_CustomRopeType;

#endif /* _COLIBRI_ROPE */
