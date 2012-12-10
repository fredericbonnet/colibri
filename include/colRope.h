/*
 * Header: colRope.h
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
#include <string.h> /* for memset */


/*
================================================================================
Section: Ropes
================================================================================
*/

/****************************************************************************
 * Group: Rope Creation
 *
 * Declarations:
 *	<Col_EmptyRope>, <Col_NewRopeFromString>, <Col_NewCharWord>, 
 *	<Col_NewRope>, <Col_NormalizeRope>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Constant: COL_UCS
 *
 *	When passed to <Col_NewRope> or <Col_NormalizeRope>, use the shortest 
 *	possible fixed-width format. Input format is always <COL_UCS4>.
 *
 * Note:
 *	Numeric value is chosen so that the lower 3 bits give the character
 *	width in the data chunk. 
 *---------------------------------------------------------------------------*/

#define COL_UCS			(Col_StringFormat) 0x24

/*
 * Remaining declarations.
 */

EXTERN Col_Word		Col_EmptyRope();
EXTERN Col_Word		Col_NewRopeFromString(const char *string);
EXTERN Col_Word		Col_NewCharWord(Col_Char c);
EXTERN Col_Word		Col_NewRope(Col_StringFormat format, const void *data, 
			    size_t byteLength);
EXTERN Col_Word		Col_NormalizeRope(Col_Word rope, 
			    Col_StringFormat format, Col_Char replace,
			    int flatten);


/****************************************************************************
 * Group: Rope Accessors
 *
 * Declarations:
 *	<Col_CharWordValue>, <Col_StringWordFormat>, <Col_RopeLength>
 *	<Col_RopeAt>
 ****************************************************************************/

EXTERN Col_Char		Col_CharWordValue(Col_Word ch);
EXTERN Col_StringFormat	Col_StringWordFormat(Col_Word string);
EXTERN size_t		Col_RopeLength(Col_Word rope);
EXTERN Col_Char		Col_RopeAt(Col_Word rope, size_t index);


/****************************************************************************
 * Group: Rope Search and Comparison
 *
 * Declarations:
 *	<Col_RopeFind>, <Col_RopeSearch>, <Col_CompareRopesL>
 ****************************************************************************/

EXTERN size_t		Col_RopeFind(Col_Word rope, Col_Char c, size_t start, 
			    size_t max, int reverse);
EXTERN size_t		Col_RopeSearch(Col_Word rope, Col_Word subrope, 
			    size_t start, size_t max, int reverse);
EXTERN int		Col_CompareRopesL(Col_Word rope1, Col_Word rope2,
			    size_t start, size_t max, size_t *posPtr, 
			    Col_Char *c1Ptr, Col_Char *c2Ptr);

/*---------------------------------------------------------------------------
 * Macro: Col_RopeFindFirst
 *
 *	Simple version of <Col_RopeFind>, find first occurrence of a character 
 *	in whole rope from its beginning. This is the rope counterpart to C's
 *	strchr.
 *
 * Arguments:
 *	rope	- Rope to search character into.
 *	c	- Character to search for.
 *
 * Result:
 *	If found, returns the position of the character in rope. Else returns
 *	SIZE_MAX (which is an invalid character index since this is the maximum
 *	rope length, and indices are zero-based).
 *
 * See also:
 *	<Col_RopeFind>
 *---------------------------------------------------------------------------*/

#define Col_RopeFindFirst(rope, c) \
	Col_RopeFind((rope), (c), 0, SIZE_MAX, 0)

/*---------------------------------------------------------------------------
 * Macro: Col_RopeFindFirstN
 *
 *	Simple version of <Col_RopeFind>, find first occurrence of a character 
 *	in rope from its beginning up to a given number of characters. This is
 *	the rope counterpart to C's strnchr.
 *
 * Arguments:
 *	rope	- Rope to search character into.
 *	c	- Character to search for.
 *	max	- Maximum number of characters to search.
 *
 * Result:-
 *	If found, returns the position of the character in rope. Else returns
 *	SIZE_MAX (which is an invalid character index since this is the maximum
 *	rope length, and indices are zero-based).
 *
 * See also:
 *	<Col_RopeFind>
 *---------------------------------------------------------------------------*/

#define Col_RopeFindFirstN(rope, c, max) \
	Col_RopeFind((rope), (c), 0, (max), 0)

/*---------------------------------------------------------------------------
 * Macro: Col_RopeFindLast
 *
 *	Simple version of <Col_RopeFind>, find last occurrence of a character 
 *	in whole rope from its end. This is the rope counterpart to C's strrchr.
 *
 * Arguments:
 *	rope	- Rope to search character into.
 *	c	- Character to search for.
 *
 * Result:
 *	If found, returns the position of the character in rope. Else returns
 *	SIZE_MAX (which is an invalid character index since this is the maximum
 *	rope length, and indices are zero-based).
 *
 * See also:
 *	<Col_RopeFind>
 *---------------------------------------------------------------------------*/

#define Col_RopeFindLast(rope, c) \
	Col_RopeFind((rope), (c), SIZE_MAX, SIZE_MAX, 1)

/*---------------------------------------------------------------------------
 * Macro: Col_RopeFindLastN
 *
 *	Simple version of <Col_RopeFind>, find last occurrence of a character 
 *	in rope from its end up to a given number of characters. This function
 *	has no C counterpart and is provided for symmetry.
 *
 * Arguments:
 *	rope	- Rope to search character into.
 *	c	- Character to search for.
 *	max	- Maximum number of characters to search.
 *
 * Result:
 *	If found, returns the position of the character in rope. Else returns
 *	SIZE_MAX (which is an invalid character index since this is the maximum
 *	rope length, and indices are zero-based).
 *
 * See also:
 *	<Col_RopeFind>
 *---------------------------------------------------------------------------*/

#define Col_RopeFindLastN(rope, c, max) \
	Col_RopeFind((rope), (c), SIZE_MAX, (max), 1)

/*---------------------------------------------------------------------------
 * Macro: Col_RopeSearchFirst
 *
 *	Simple version of <Col_RopeSearch>, find first occurrence of a subrope
 *	in whole rope from its beginning. This is the rope counterpart to C's
 *	strstr.
 *
 * Arguments:
 *	rope	- Rope to search character into.
 *	subrope	- Subrope to search for.
 *
 * Result:
 *	If found, returns the position of the subrope in rope. Else returns
 *	SIZE_MAX (which is an invalid character index since this is the maximum
 *	rope length, and indices are zero-based).
 *
 * See also:
 *	<Col_RopeSearch>
 *---------------------------------------------------------------------------*/

#define Col_RopeSearchFirst(rope, subrope) \
	Col_RopeSearch((rope), (subrope), 0, SIZE_MAX, 0)

/*---------------------------------------------------------------------------
 * Macro: Col_RopeSearchLast
 *
 *	Simple version of <Col_RopeSearch>, find last occurrence of a subrope
 *	in whole rope from its end. This is the rope counterpart to C's
 *	strstr.
 *
 * Arguments:
 *	rope	- Rope to search character into.
 *	subrope	- Subrope to search for.
 *
 * Result:
 *	If found, returns the position of the subrope in rope. Else returns
 *	SIZE_MAX (which is an invalid character index since this is the maximum
 *	rope length, and indices are zero-based).
 *
 * See also:
 *	<Col_RopeSearch>
 *---------------------------------------------------------------------------*/

#define Col_RopeSearchLast(rope, subrope) \
	Col_RopeSearch((rope), (subrope), SIZE_MAX, SIZE_MAX, 1)

/*---------------------------------------------------------------------------
 * Macro: Col_CompareRopes
 *
 *	Simple version of <Col_CompareRopesL>, compare two ropes. This is the 
 *	rope counterpart to C's strcmp.
 *
 * Arguments:
 *	rope1, rope2	- Ropes to compare.
 *
 * Result:
 *	Returns an integral value indicating the relationship between the 
 *	ropes:
 *	    - A zero value indicates that both strings are equal;
 *	    - A value greater than zero indicates that the first character that 
 *	      does not match has a greater value in rope1 than in rope2, or that
 *	      rope1 is longer than rope2; 
 *	    - A value less than zero indicates the opposite.
 *
 * See also:
 *	<Col_CompareRopesL>
 *---------------------------------------------------------------------------*/

#define Col_CompareRopes(rope1, rope2) \
	Col_CompareRopesL((rope1), (rope2), 0, SIZE_MAX, NULL, NULL, NULL)

/*---------------------------------------------------------------------------
 * Macro: Col_CompareRopesN
 *
 *	Simple version of <Col_CompareRopesL>, compare two ropes up to a given
 *	number of characters. This is the rope counterpart to C's strncmp.
 *
 * Arguments:
 *	rope1, rope2	- Ropes to compare.
 *	max		- Maximum number of characters to compare.
 *
 * Result:
 *	Returns an integral value indicating the relationship between the 
 *	ropes:
 *	    - A zero value indicates that both strings are equal;
 *	    - A value greater than zero indicates that the first character that 
 *	      does not match has a greater value in rope1 than in rope2, or that
 *	      rope1 is longer than rope2; 
 *	    - A value less than zero indicates the opposite.
 *
 *
 * See also:
 *	<Col_CompareRopesL>
 *---------------------------------------------------------------------------*/

#define Col_CompareRopesN(rope1, rope2, max) \
	Col_CompareRopesL((rope1), (rope2), 0, (max), NULL, NULL, NULL)


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
 * Arguments:
 *	List of ropes to concatenate.
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
 *	<Col_TraverseRopeChunksN>, <Col_TraverseRopeChunks>
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
 *			  <Col_TraverseRopeChunks> procedure family.
 *
 * Returns:
 *	If nonzero, interrupts the traversal.
 *
 * See also: 
 *	<Col_TraverseRopeChunksN>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

typedef int (Col_RopeChunksTraverseProc) (size_t index, size_t length, 
	size_t number, const Col_RopeChunk *chunks, Col_ClientData clientData);

EXTERN int		Col_TraverseRopeChunksN(size_t number, Col_Word *ropes, 
			    size_t start, size_t max, 
			    Col_RopeChunksTraverseProc *proc, 
			    Col_ClientData clientData, size_t *lengthPtr);
EXTERN int		Col_TraverseRopeChunks(Col_Word rope, size_t start, 
			    size_t max, int reverse, 
			    Col_RopeChunksTraverseProc *proc, 
			    Col_ClientData clientData, size_t *lengthPtr);


/****************************************************************************
 * Group: Rope Iteration
 *
 * Declarations: 
 *	<Col_RopeIterBegin>, <Col_RopeIterFirst>, <Col_RopeIterLast>, 
 *	<Col_RopeIterString>, <Col_RopeIterCompare>, <Col_RopeIterMoveTo>,
 *	<Col_RopeIterForward>, <Col_RopeIterBackward>
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
 *	rope		- Rope being iterated. If nil, use string iterator mode.
 *	length		- Rope length.
 *	index		- Current position.
 *	chunk		- Current chunk info.
 *
 * Chunk fields:
 *	first, last	- Range of validity for current chunk.
 *	accessProc	- If non-NULL, element accessor. Else, use direct
 *			  address mode.
 *	current		- Current element information:
 *	current.direct	- Current element info in direct access mod.
 *	current.access	- Current element info in accessor mode.
 *
 * Direct access mode fields:
 *	format		- Format of current chunk.
 *	address		- Address of current element.
 *
 * Accessor mode fields:
 *	leaf		- First argument passed to *accessProc*.
 *	index		- Second argument passed to *accessProc*.
 *
 * See also:
 *	<Col_RopeIterator>
 *---------------------------------------------------------------------------*/

typedef struct ColRopeIterator {
    Col_Word rope;
    size_t length;
    size_t index;
    struct {
	size_t first, last;
	ColRopeIterLeafAtProc *accessProc;
	union {
	    struct {
		Col_StringFormat format;
		const void *address;
	    } direct;
	    struct {
		Col_Word leaf;
		size_t index;
	    } access;
	} current;
    } chunk;
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
 *	Each iterator takes 8 words on the stack.
 *
 *	The type is defined as a single-element array of the internal datatype:
 *
 *	- declared variables allocate the right amount of space on the stack,
 *	- calls use pass-by-reference (i.e. pointer) and not pass-by-value,
 *	- forbidden as return type.
 *---------------------------------------------------------------------------*/

typedef ColRopeIterator Col_RopeIterator[1];

/*---------------------------------------------------------------------------
 * Constant: COL_ROPEITER_NULL
 *
 *	Static initializer for null rope iterators.
 *
 * See also: 
 *	<Col_RopeIterator>, <Col_RopeIterNull>
 *---------------------------------------------------------------------------*/

#define COL_ROPEITER_NULL	{{WORD_NIL,0,0,{0,0,NULL,0,NULL}}}

/*---------------------------------------------------------------------------
 * Macro: Col_RopeIterNull
 *
 *	Test whether iterator is null (e.g. it has been set to 
 *	<COL_ROPEITER_NULL> or <Col_RopeIterSetNull>). This uninitialized state
 *	renders it unusable for any call. Use with caution.
 *
 * Argument:
 *	it	- The iterator to test. (Caution: evaluated several times during
 *		  macro expansion)
 *
 * Result:
 *	Nonzero if iterator is null.
 *
 * See also: 
 *	<Col_RopeIterator>, <COL_ROPEITER_NULL>, <Col_RopeIterSetNull>
 *---------------------------------------------------------------------------*/

#define Col_RopeIterNull(it) \
    ((it)->rope == WORD_NIL && (it)->chunk.current.direct.address == NULL)

/*---------------------------------------------------------------------------
 * Macro: Col_RopeIterSetNull
 *
 *	Set an iterator to null.
 *
 * Argument:
 *	it	- Iterator to initialize.
 *
 * See also: 
 *	<Col_RopeIterator>
 *---------------------------------------------------------------------------*/

#define Col_RopeIterSetNull(it) \
    memset(it, 0, sizeof(it))

/*---------------------------------------------------------------------------
 * Macro: Col_RopeIterRope
 *
 *	Get rope for iterator.
 *
 * Argument:
 *	it	- The iterator to get rope for.
 *
 * Result:
 *	The rope, or <WORD_NIL> if iterator was initialized with
 *	<Col_RopeIterString>. Undefined if at end.
 *
 * See also: 
 *	<Col_RopeIterator>, <Col_RopeIterEnd>
 *---------------------------------------------------------------------------*/

#define Col_RopeIterRope(it)	\
    ((it)->rope)

/*---------------------------------------------------------------------------
 * Macro: Col_RopeIterLength
 *
 *	Get length of the iterated sequence.
 *
 * Argument:
 *	it	- The iterator to get length for.
 *
 * Result:
 *	Length of iterated sequence.
 *
 * See also: 
 *	<Col_RopeIterator>
 *---------------------------------------------------------------------------*/

#define Col_RopeIterLength(it) \
    ((it)->length)

/*---------------------------------------------------------------------------
 * Macro: Col_RopeIterIndex
 *
 *	Get current index within rope for iterator.
 *
 * Argument:
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
 * Argument:
 *	it	- The iterator to get character for. (Caution: evaluated several
 *		  times during macro expansion)
 *
 * Result:
 *	If the index is past the end of the rope, the invalid codepoint
 *	<COL_CHAR_INVALID>, else the Unicode codepoint of the character.
 *
 * Side effects:
 *	The argument is referenced several times by the macro. Make sure to
 *	avoid any side effect.
 *
 * See also: 
 *	<Col_RopeIterator>
 *---------------------------------------------------------------------------*/

#define Col_RopeIterAt(it) \
    (  ((it)->index < (it)->chunk.first || (it)->index > (it)->chunk.last) ? ColRopeIterUpdateTraversalInfo((ColRopeIterator *)(it)) \
     : (it)->chunk.accessProc ? (it)->chunk.accessProc((it)->chunk.current.access.leaf, (it)->chunk.current.access.index) \
     : COL_CHAR_GET((it)->chunk.current.direct.format, (it)->chunk.current.direct.address))

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
 *	The argument is referenced several times by the macro. Make sure to
 *	avoid any side effect.
 *
 * See also: 
 *	<Col_RopeIterForward>
 *---------------------------------------------------------------------------*/

#define Col_RopeIterNext(it) \
    (  ((it)->index < (it)->chunk.first || (it)->index >= (it)->chunk.last) ? (Col_RopeIterForward((it), 1), 0) \
     : ((it)->index++, \
          (it)->chunk.accessProc ? ((it)->chunk.current.access.index++, 0) \
        : (COL_CHAR_NEXT((it)->chunk.current.direct.format, (it)->chunk.current.direct.address), 0)))

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
 *	The argument is referenced several times by the macro. Make sure to
 *	avoid any side effect.
 *
 * See also: 
 *	<Col_RopeIterBackward>
 *---------------------------------------------------------------------------*/

#define Col_RopeIterPrevious(it) \
    (  ((it)->index <= (it)->chunk.first || (it)->index > (it)->chunk.last) ? (Col_RopeIterBackward((it), 1), 0) \
     : ((it)->index--, \
          (it)->chunk.accessProc ? ((it)->chunk.current.access.index--, 0) \
        : (COL_CHAR_PREVIOUS((it)->chunk.current.direct.format, (it)->chunk.current.direct.address), 0)))

/*---------------------------------------------------------------------------
 * Macro: Col_RopeIterEnd
 *
 *	Test whether iterator reached end of rope.
 *
 * Argument:
 *	it	- The iterator to test. (Caution: evaluated several times 
 *		  during macro expansion)
 *
 * Result:
 *	Nonzero if iterator is at end.
 *
 * See also: 
 *	<Col_RopeIterator>, <Col_RopeIterBegin>
 *---------------------------------------------------------------------------*/

#define Col_RopeIterEnd(it) \
    ((it)->index >= (it)->length)

/*---------------------------------------------------------------------------
 * Macro: Col_RopeIterSet
 *
 *	Initialize an iterator to another one's value.
 *
 * Argument:
 *	it	- Iterator to initialize.
 *	value	- Value to set.
 *
 * See also: 
 *	<Col_RopeIterator>
 *---------------------------------------------------------------------------*/

#define Col_RopeIterSet(it, value) \
    (*(it) = *(value))

/*
 * Remaining declarations.
 */

EXTERN void		Col_RopeIterBegin(Col_RopeIterator it, Col_Word rope, 
			    size_t index);
EXTERN void		Col_RopeIterFirst(Col_RopeIterator it, Col_Word rope);
EXTERN void		Col_RopeIterLast(Col_RopeIterator it, Col_Word rope);
EXTERN void		Col_RopeIterString(Col_RopeIterator it, 
			    Col_StringFormat format, const void *data, 
			    size_t length);
EXTERN int		Col_RopeIterCompare(const Col_RopeIterator it1, 
			    const Col_RopeIterator it2);
EXTERN void		Col_RopeIterMoveTo(Col_RopeIterator it, size_t index);
EXTERN void		Col_RopeIterForward(Col_RopeIterator it, size_t nb);
EXTERN void		Col_RopeIterBackward(Col_RopeIterator it, size_t nb);

EXTERN Col_Char		ColRopeIterUpdateTraversalInfo(ColRopeIterator *it);


/*
================================================================================
Section: Custom Ropes
================================================================================
*/

/*---------------------------------------------------------------------------
 * Typedef: Col_RopeLengthProc
 *
 *	Function signature of custom rope length procs.
 *
 * Argument:
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
 *	rope	- Custom rope to get chunk from.
 *	index	- Index of character to get chunk for.
 *
 * Note:
 *	By construction, indices are guaranteed to be within valid range.
 *
 * Results:
 *	chunkPtr		- Chunk information. See <Col_RopeChunk>.
 *	firstPtr, lastPtr	- Chunk range of validity.
 *
 * See also: 
 *	<Col_CustomRopeType>
 *---------------------------------------------------------------------------*/

typedef void (Col_RopeChunkAtProc) (Col_Word rope, size_t index, 
    Col_RopeChunk *chunkPtr, size_t *firstPtr, size_t *lastPtr);

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
 *	<Col_NewCustomWord>, <Col_CustomWordType>, <Col_RopeLengthProc>, 
 *	<Col_RopeCharAtProc>, <Col_RopeChunkAtProc>, <Col_RopeSubropeProc>, 
 *	<Col_RopeConcatProc>
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
