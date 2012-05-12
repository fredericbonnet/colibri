/*
 * Internal Header: colRopeInt.h
 *
 *	This header file defines the rope word internals of Colibri.
 *
 *	Ropes are a string datatype that allows for fast insertion, extraction
 *	and composition of strings. Internally they use self-balanced binary 
 *	trees.
 *
 *	They are always immutable.
 *
 * See also:
 *	<colRope.c>, <colRope.h>, <WORD_TYPE_UCSSTR>, <WORD_TYPE_UTFSTR>,
 *	<WORD_TYPE_SUBROPE>, <WORD_TYPE_CONCATROPE>
 */

#ifndef _COLIBRI_ROPE_INT
#define _COLIBRI_ROPE_INT


/*
================================================================================
Internal Section: Fixed-Width UCS Strings
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Fixed-Width UCS String Word
 *
 *	Fixed-width UCS strings are constant arrays of characters that are 
 *	directly addressable.
 *
 * Requirements:
 *	UCS string words must know their length, i.e. the number of characters
 *	they contain, as well as the character format (either <COL_UCS1>,
 *	<COL_UCS2> or <COL_UCS4>. Both informations give the byte length.
 *
 *	Elements are stored within the word cells following the header.
 *
 *	UCS string words use as much cells as needed to store their characters.
 *
 * Fields:
 *	Format	- Character format.
 *	Length	- Size of character array.
 *	Data	- Array of character codepoints.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0             7 8            15 16                           31
 *     +---------------+---------------+-------------------------------+
 *     |      Type     |    Format     |            Length             |
 *     +---------------+---------------+-------------------------------+
 *     .                                                               .
 *     .                         Character data                        .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<WORD_TYPEINFO>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_UCSSTR_* Accessors
 *
 *	Fixed-width UCS string field accessor macros.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 *  WORD_UCSSTR_FORMAT	- Character format.
 *  WORD_UCSSTR_LENGTH	- Character length.
 *  WORD_UCSSTR_DATA	- String data.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Fixed-Width UCS String Word>, <WORD_UCSSTR_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_UCSSTR_FORMAT(word)	(((int8_t *)(word))[1])
#define WORD_UCSSTR_LENGTH(word)	(((uint16_t *)(word))[1])
#define WORD_UCSSTR_DATA(word)		((const char *)(word)+WORD_UCSSTR_HEADER_SIZE)

/*---------------------------------------------------------------------------
 * Internal Constant: WORD_UCSSTR_HEADER_SIZE
 *
 *	Byte size of UCS string header.
 *
 * See also:
 *	<Fixed-Width UCS String Word>
 *---------------------------------------------------------------------------*/

#define WORD_UCSSTR_HEADER_SIZE		(sizeof(uint16_t)*2)

/*---------------------------------------------------------------------------
 * Internal Constant: WORD_UCSSTR_HEADER_SIZE
 *
 *	Maximum char length of UCS strings.
 *
 * See also:
 *	<Fixed-Width UCS String Word>
 *---------------------------------------------------------------------------*/

#define UCSSTR_MAX_LENGTH		UINT16_MAX

/*---------------------------------------------------------------------------
 * Internal Macro: UCSSTR_SIZE
 *
 *	Get number of cells taken by the UCS string.
 *
 * Argument:
 *	byteLength	- Byte size of string.
 *
 * Result:
 *	Number of cells taken by word.
 *
 * See also:
 *	<Fixed-Width UCS String Word>
 *---------------------------------------------------------------------------*/

#define UCSSTR_SIZE(byteLength) \
    (NB_CELLS(WORD_UCSSTR_HEADER_SIZE+(byteLength)))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_UCSSTR_INIT
 *
 *	UCS string word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	format	- <WORD_UCSSTR_FORMAT>
 *	length	- <WORD_UCSSTR_LENGTH>
 *
 * See also:
 *	<Fixed-Width UCS String Word>, <WORD_TYPE_UCSSTR>, <Col_NewRope>
 *---------------------------------------------------------------------------*/

#define WORD_UCSSTR_INIT(word, format, length) \
    WORD_SET_TYPEID((word), WORD_TYPE_UCSSTR); \
    WORD_UCSSTR_FORMAT(word) = (uint8_t) (format); \
    WORD_UCSSTR_LENGTH(word) = (uint16_t) (length);


/*
================================================================================
Internal Section: Variable-Width UTF Strings
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Variable-Width UTF-8/16 String Word
 *
 *	Variable-width UTF-8/16 strings are constant arrays of variable-width
 *	characters requiring dedicated encoding/decoding procedures for access.
 *
 * Requirements:
 *	UTF-8/16 string words must know their byte length, as well as their 
 *	character format (either <COL_UTF8> or <COL_UTF16>.
 *
 *	Elements are stored within the word cells following the header.
 *
 *	UTF-8/16 string words use as much cells as needed to store their 
 *	characters.
 *
 * Fields:
 *	Format		- Character format.
 *	Length		- Number of characters in array.
 *	Byte Length	- Byte length of character arrays.
 *	Data		- Array of character code units.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0             7 8            15 16                           31
 *     +---------------+---------------+-------------------------------+
 *     |      Type     |    Format     |            Length             |
 *     +---------------+---------------+-------------------------------+
 *     |          Byte Length          |                               |
 *     +-------------------------------+                               +
 *     .                                                               .
 *     .                         Character data                        .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<WORD_TYPEINFO>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_UTFSTR_* Accessors
 *
 *	Variable-width UTF-8/16 string field accessor macros.
 *
 *  WORD_UTFSTR_FORMAT		- Character format.
 *  WORD_UTFSTR_LENGTH		- Character length.
 *  WORD_UTFSTR_BYTELENGTH	- Byte length.
 *  WORD_UTFSTR_DATA		- String data.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Variable-Width UTF-8/16 String Word>, <WORD_UTFSTR_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_UTFSTR_FORMAT(word)	(((int8_t *)(word))[1])
#define WORD_UTFSTR_LENGTH(word)	(((uint16_t *)(word))[1])
#define WORD_UTFSTR_BYTELENGTH(word)	(((uint16_t *)(word))[2])
#define WORD_UTFSTR_DATA(word)		((const char *)(word)+WORD_UTFSTR_HEADER_SIZE)

/*---------------------------------------------------------------------------
 * Internal Constant: WORD_UTFSTR_HEADER_SIZE
 *
 *	Byte size of UTF-8/16 string header.
 *
 * See also:
 *	<Variable-Width UTF-8/16 String Word>
 *---------------------------------------------------------------------------*/

#define WORD_UTFSTR_HEADER_SIZE		(sizeof(uint16_t)*3)

/*---------------------------------------------------------------------------
 * Internal Constant: UTFSTR_MAX_BYTELENGTH
 *
 *	Maximum byte length of UTF-8/16 strings. Contrary to fixed-width 
 *	versions, UTF-8/16 strings are limited in size to one page, so 
 *	that access performances are better. The consequence is that large
 *	UTF-8/16 strings get split into several smaller chunks that get 
 *	concatenated.
 *
 * See also:
 *	<Variable-Width UTF-8/16 String Word>
 *---------------------------------------------------------------------------*/

#define UTFSTR_MAX_BYTELENGTH		(AVAILABLE_CELLS*CELL_SIZE-WORD_UTFSTR_HEADER_SIZE)

/*---------------------------------------------------------------------------
 * Internal Macro: UTFSTR_SIZE
 *
 *	Get number of cells taken by the UTF-8/16 string.
 *
 * Argument:
 *	byteLength	- Byte size of string.
 *
 * Result:
 *	Number of cells taken by word.
 *
 * See also:
 *	<Variable-Width UTF-8/16 String Word>
 *---------------------------------------------------------------------------*/

#define UTFSTR_SIZE(byteLength) \
    (NB_CELLS(WORD_UTFSTR_HEADER_SIZE+(byteLength)))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_UTFSTR_INIT
 *
 *	UTF-8/16 string word initializer.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times 
 *			  during macro expansion)
 *	format		- <WORD_UTFSTR_FORMAT>
 *	length		- <WORD_UTFSTR_LENGTH>
 *	byteLength	- <WORD_UTFSTR_BYTELENGTH>
 *
 * See also:
 *	<Variable-Width UTF-8/16 String Word>, <WORD_TYPE_UTFSTR>, <Col_NewRope>
 *---------------------------------------------------------------------------*/

#define WORD_UTFSTR_INIT(word, format, length, byteLength) \
    WORD_SET_TYPEID((word), WORD_TYPE_UTFSTR); \
    WORD_UTFSTR_FORMAT(word) = (uint8_t) (format); \
    WORD_UTFSTR_LENGTH(word) = (uint16_t) (length); \
    WORD_UTFSTR_BYTELENGTH(word) = (uint16_t) (byteLength); \


/*
================================================================================
Internal Section: Subropes
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Subrope Word
 *
 *	Subropes are immutable ropes pointing to a slice of a larger rope. 
 *
 *	Extraction of a subrope from a large rope can be expensive. For example,
 *	removing one character from a string of several thousands implies 
 *	copying the whole data minus this element. Likewise, extracting a range
 *	of characters from a complex binary tree of rope nodes implies 
 *	traversing and copying large parts of this subtree.
 *
 *	To limit the memory and processing costs of rope slicing, the subrope
 *	word simply points to the original rope and the index range of 
 *	characters in the slice. This representation is preferred over raw copy
 *	when the source rope is large.
 *
 * Requirements:
 *	Subrope words use one single cell.
 *
 *	Subropes must know their source rope word.
 *
 *	Subropes must also know the indices of the first and last characters in
 *	the source rope that belongs to the slice. These indices are inclusive.
 *	Difference plus one gives the subrope length.
 *
 *	Last, subropes must know their depth for tree balancing (see 
 *	<Rope Tree Balancing>). This is the same as the source depth, but to
 *	avoid one pointer dereference we cache the value in the word.
 *
 * Fields:
 *	Depth	- Depth of subrope. 8 bits will code up to 255 depth levels, 
 *		  which is more than sufficient for balanced binary trees. 
 *	Source	- Rope of which this one is a subrope.
 *	First	- First character in source.
 *	Last	- Last character in source.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8    15                                               n
 *     +-------+-------+-----------------------------------------------+
 *   0 | Type  | Depth |                    Unused                     |
 *     +-------+-------+-----------------------------------------------+
 *   1 |                            Source                             |
 *     +---------------------------------------------------------------+
 *   2 |                             First                             |
 *     +---------------------------------------------------------------+
 *   3 |                             Last                              |
 *     +---------------------------------------------------------------+
 * (end)
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_SUBROPE_* Accessors
 *
 *	Subrope field accessor macros.
 *
 *  WORD_SUBROPE_DEPTH	- Depth of subrope.
 *  WORD_SUBROPE_SOURCE	- Rope from which this one is extracted.
 *  WORD_SUBROPE_FIRST	- First character in source.
 *  WORD_SUBROPE_LAST	- Last character in source.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Subrope Word>, <WORD_SUBROPE_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_SUBROPE_DEPTH(word)	(((uint8_t *)(word))[1])
#define WORD_SUBROPE_SOURCE(word)	(((Col_Word *)(word))[1])
#define WORD_SUBROPE_FIRST(word)	(((size_t *)(word))[2])
#define WORD_SUBROPE_LAST(word)		(((size_t *)(word))[3])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_SUBROPE_INIT
 *
 *	Subrope word initializer.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times 
 *			  during macro expansion)
 *	depth		- <WORD_SUBROPE_DEPTH>
 *	source		- <WORD_SUBROPE_SOURCE>
 *	first		- <WORD_SUBROPE_FIRST>
 *	last		- <WORD_SUBROPE_LAST>
 *
 * See also:
 *	<Subrope Word>, <WORD_TYPE_SUBROPE>, <Col_Subrope>
 *---------------------------------------------------------------------------*/

#define WORD_SUBROPE_INIT(word, depth, source, first, last) \
    WORD_SET_TYPEID((word), WORD_TYPE_SUBROPE); \
    WORD_SUBROPE_DEPTH(word) = (uint8_t) (depth); \
    WORD_SUBROPE_SOURCE(word) = (source); \
    WORD_SUBROPE_FIRST(word) = (first); \
    WORD_SUBROPE_LAST(word) = (last);


/*
================================================================================
Internal Section: Concat Ropes
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Concat Rope Word
 *
 *	Concat ropes are balanced binary trees concatenating left and right 
 *	subropes. The tree is balanced by construction (see 
 *	<Rope Tree Balancing>).
 *
 * Requirements:
 *	Concat rope words use one single cell.
 *
 *	Concat ropes must know their left and right subrope words.
 *
 *	Concat ropes must know their length, which is the sum of their left and
 *	right arms. To save full recursive subtree traversal this length is
 *	stored at each level in the concat words. We also cache the left arm's
 *	length whenever possible to save a pointer dereference (the right 
 *	length being the total minus left lengths).
 *
 *	Last, concat ropes must know their depth for tree balancing (see 
 *	<Rope Tree Balancing>). This is the max depth of their left and
 *	right arms, plus one.
 *
 * Fields:
 *	Depth		- Depth of concat rope. 8 bits will code up to 255 depth
 *			  levels, which is more than sufficient for balanced 
 *			  binary trees. 
 *	Left length	- Used as shortcut to avoid dereferencing the left arm. 
 *			  Zero if actual length is too large to fit.
 *	Length		- Rope length, which is the sum of both arms'.
 *	Left		- Left concatenated rope.
 *	Right		- Right concatenated rope.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8    15 16           31                               n
 *     +-------+-------+---------------+-------------------------------+
 *   0 | Type  | Depth |  Left length  |        Unused (n > 32)        |
 *     +-------+-------+---------------+-------------------------------+
 *   1 |                            Length                             |
 *     +---------------------------------------------------------------+
 *   2 |                             Left                              |
 *     +---------------------------------------------------------------+
 *   3 |                             Right                             |
 *     +---------------------------------------------------------------+
 * (end)
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_CONCATROPE_* Accessors
 *
 *	Concat rope field accessor macros.
 *
 *  WORD_CONCATROPE_DEPTH	- Depth of concat rope.
 *  WORD_CONCATROPE_LEFT_LENGTH	- Left arm's length or zero if too large.
 *  WORD_CONCATROPE_LENGTH	- Rope length
 *  WORD_CONCATROPE_LEFT	- Left concatenated rope.
 *  WORD_CONCATROPE_RIGHT	- Right concatenated rope.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Concat Rope Word>, <WORD_CONCATROPE_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_CONCATROPE_DEPTH(word)	(((uint8_t *)(word))[1])
#define WORD_CONCATROPE_LEFT_LENGTH(word) (((uint16_t *)(word))[1])
#define WORD_CONCATROPE_LENGTH(word)	(((size_t *)(word))[1])
#define WORD_CONCATROPE_LEFT(word)	(((Col_Word *)(word))[2])
#define WORD_CONCATROPE_RIGHT(word)	(((Col_Word *)(word))[3])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_CONCATROPE_INIT
 *
 *	Concat rope word initializer.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times 
 *			  during macro expansion)
 *	depth		- <WORD_CONCATROPE_DEPTH>
 *	length		- <WORD_CONCATROPE_LENGTH>
 *	leftLength	- <WORD_CONCATROPE_LEFT_LENGTH> (Caution: evaluated 
 *			  several times during macro expansion)
 *	left		- <WORD_CONCATROPE_LEFT>
 *	right		- <WORD_CONCATROPE_RIGHT>
 *
 * See also:
 *	<Concat Rope Word>, <WORD_TYPE_CONCATROPE>, <Col_ConcatRopes>
 *---------------------------------------------------------------------------*/

#define WORD_CONCATROPE_INIT(word, depth, length, leftLength, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_CONCATROPE); \
    WORD_CONCATROPE_DEPTH(word) = (uint8_t) (depth); \
    WORD_CONCATROPE_LENGTH(word) = (length); \
    WORD_CONCATROPE_LEFT_LENGTH(word) = (uint16_t) ((leftLength)>UINT16_MAX?0:(leftLength)); \
    WORD_CONCATROPE_LEFT(word) = (left); \
    WORD_CONCATROPE_RIGHT(word) = (right);


/*
================================================================================
Internal Section: Type Checking
================================================================================
*/

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_CHAR
 *
 *	Type checking macro for characters.
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not a character.
 *
 * See also:
 *	<Col_Error>, <Col_WordIsChar>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_CHAR(word) \
    if (!(Col_WordType(word) & COL_CHAR)) { \
	Col_Error(COL_TYPECHECK, "%x is not a character", (word)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_STRING
 *
 *	Type checking macro for strings.
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not a string.
 *
 * See also:
 *	<Col_Error>, <Col_WordIsString>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_STRING(word) \
    if (!(Col_WordType(word) & COL_STRING)) { \
	Col_Error(COL_TYPECHECK, "%x is not a string", (word)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_ROPE
 *
 *	Type checking macro for ropes.
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not a rope.
 *
 * See also:
 *	<Col_Error>, <Col_WordIsRope>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_ROPE(word) \
    if (!(Col_WordType(word) & COL_ROPE)) { \
	Col_Error(COL_TYPECHECK, "%x is not a rope", (word)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_ROPEITER
 *
 *	Type checking macro for rope iterators.
 *
 * Argument:
 *	it	- Checked iterator.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *it* is not a valid rope iterator.
 *
 * See also:
 *	<Col_Error>, <Col_RopeIterNull>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_ROPEITER(it) \
    if (Col_RopeIterNull(it)) { \
	Col_Error(COL_TYPECHECK, "%x is not a rope iterator", (it)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 


/*
================================================================================
Internal Section: Range Checking
================================================================================
*/

/*---------------------------------------------------------------------------
 * Internal Macro: RANGECHECK_CONCATLENGTH
 *
 *	Range checking macro for ropes, ensures that combined lengths of two
 *	concatenated ropes don't exceed the maximum value.
 *
 * Argument:
 *	length1, length2    - Checked lengths.
 *
 * Side effects:
 *	Generate <COL_RANGECHECK> error when resulting length exceeds the max
 *	rope length (SIZE_MAX).
 *
 * See also:
 *	<Col_Error>
 *---------------------------------------------------------------------------*/

#define RANGECHECK_CONCATLENGTH(length1, length2) \
    if (SIZE_MAX-(length1) < (length2)) { \
	Col_Error(COL_RANGECHECK, \
		"Combined length %u+%u exceeds the maximum allowed value %u", \
		(length1), (length2), SIZE_MAX); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

/*---------------------------------------------------------------------------
 * Internal Macro: RANGECHECK_REPEATLENGTH
 *
 *	Range checking macro for ropes, ensures that length of a repeated rope
 *	doesn't exceed the maximum value.
 *
 * Argument:
 *	length, count	- Checked length and repetition factor.
 *
 * Side effects:
 *	Generate <COL_RANGECHECK> error when resulting length exceeds the max
 *	rope length (SIZE_MAX).
 *
 * See also:
 *	<Col_Error>
 *---------------------------------------------------------------------------*/

#define RANGECHECK_REPEATLENGTH(length, count) \
    if ((count) > 1 && SIZE_MAX/(count) < (length)) { \
	Col_Error(COL_RANGECHECK, \
		"Length %u times %u exceeds the maximum allowed value %u", \
		(length), (count), SIZE_MAX); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

/*---------------------------------------------------------------------------
 * Internal Macro: RANGECHECK_ROPEITER
 *
 *	Range checking macro for rope iterators, ensures that iterator is not
 *	at end.
 *
 * Argument:
 *	it	- Checked iterator.
 *
 * Side effects:
 *	Generate <COL_RANGECHECK> error when *it* is at end.
 *
 * See also:
 *	<Col_Error>, <Col_RopeIterEnd>
 *---------------------------------------------------------------------------*/

#define RANGECHECK_ROPEITER(it) \
    if (Col_RopeIterEnd(it)) { \
	Col_Error(COL_RANGECHECK, "Iterator %x is at end", (it)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

#endif /* _COLIBRI_ROPE_INT */
