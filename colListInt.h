/*
 * Internal Header: colListInt.h
 *
 *	This header file defines the list word internals of Colibri.
 *
 *	Lists are a collection datatype that allows for fast insertion, 
 *	extraction and composition of other lists. Internally they use 
 *	self-balanced binary trees, like ropes, except that they use vectors
 *	as basic containers instead of character arrays.
 *
 *	They come in both immutable and mutable forms :
 *	
 *	- Immutable lists can be composed of immutable vectors and lists. 
 *	Immutable vectors can themselves be used in place of immutable lists. 
 *
 *	- Mutable lists can be composed of either mutable or immutable lists and
 *	vectors. They can be "frozen" and turned into immutable versions.
 *	Mutable vectors *cannot* be used in place of mutable lists, as the 
 *	latter can grow indefinitely whereas the former have a maximum length 
 *	set at creation time.
 *
 * See also:
 *	<colList.c>, <colList.h>, <colVectorInt.h>, <WORD_TYPE_SUBLIST>, 
 *	<WORD_TYPE_CONCATLIST>, <WORD_TYPE_MCONCATLIST>, <WORD_TYPE_MLIST>
 */

#ifndef _COLIBRI_LIST_INT
#define _COLIBRI_LIST_INT


/*
================================================================================
Internal Section: Sublists
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Sublist Word
 *
 *	Sublists are immutable lists pointing to a slice of a larger list. 
 *
 *	Extraction of a sublist from a large list can be expensive. For example,
 *	removing one element from a vector of several thousands implies copying
 *	the whole data minus this element. Likewise, extracting a range of 
 *	elements from a complex binary tree of list nodes implies traversing and
 *	copying large parts of this subtree.
 *
 *	To limit the memory and processing costs of list slicing, the sublist
 *	word simply points to the original list and the index range of elements
 *	in the slice. This representation is preferred over raw copy when the
 *	source list is large.
 *
 * Requirements:
 *	Sublist words use one single cell.
 *
 *	Sublists must know their source list word. This source must be 
 *	immutable.
 *
 *	Sublists must also know the indices of the first and last elements in 
 *	the source list that belongs to the slice. These indices are inclusive.
 *	Difference plus one gives the sublist length.
 *
 *	Last, sublists must know their depth for tree balancing (see 
 *	<List Tree Balancing>). This is the same as the source depth, but to
 *	avoid one pointer dereference we cache the value in the word.
 *
 * Fields:
 *	Depth	- Depth of sublist. 8 bits will code up to 255 depth levels, 
 *		  which is more than sufficient for balanced binary trees. 
 *	Source	- List of which this one is a sublist.
 *	First	- First element in source.
 *	Last	- Last element in source.
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
 * Internal Macros: WORD_SUBLIST_* Accessors
 *
 *	Sublist word field accessors.
 *
 *  WORD_SUBLIST_DEPTH	- Depth of sublist.
 *  WORD_SUBLIST_SOURCE	- List from which this one is extracted.
 *  WORD_SUBLIST_FIRST	- First element in source.
 *  WORD_SUBLIST_LAST	- Last element in source.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Sublist Word>, <WORD_SUBLIST_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_SUBLIST_DEPTH(word)	(((uint8_t *)(word))[1])
#define WORD_SUBLIST_SOURCE(word)	(((Col_Word *)(word))[1])
#define WORD_SUBLIST_FIRST(word)	(((size_t *)(word))[2])
#define WORD_SUBLIST_LAST(word)		(((size_t *)(word))[3])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_SUBLIST_INIT
 *
 *	Sublist word initializer.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times 
 *			  during macro expansion)
 *	depth		- <WORD_SUBLIST_DEPTH>
 *	source		- <WORD_SUBLIST_SOURCE>
 *	first		- <WORD_SUBLIST_FIRST>
 *	last		- <WORD_SUBLIST_LAST>
 *
 * See also:
 *	<Sublist Word>, <WORD_TYPE_SUBLIST>, <Col_Sublist>
 *---------------------------------------------------------------------------*/

#define WORD_SUBLIST_INIT(word, depth, source, first, last) \
    WORD_SET_TYPEID((word), WORD_TYPE_SUBLIST); \
    WORD_SUBLIST_DEPTH(word) = (uint8_t) (depth); \
    WORD_SUBLIST_SOURCE(word) = (source); \
    WORD_SUBLIST_FIRST(word) = (first); \
    WORD_SUBLIST_LAST(word) = (last);


/*
================================================================================
Internal Section: Concat Lists
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Concat List Word
 *
 *	Concat lists are balanced binary trees concatenating left and right 
 *	sublists. They can be immutable or mutable. In the former case, the tree
 *	is balanced by construction (see <List Tree Balancing>). In the latter
 *	case, mutable concat nodes can contain both immutable and mutable 
 *	subnodes; immutable ones are converted to mutable using copy-on-write 
 *	semantics during mutable operations; mutable ones are freezed and turned
 *	immutable when they become shared during immutable operations.
 *
 * Requirements:
 *	Concat list words use one single cell.
 *
 *	Concat lists must know their left and right sublist words.
 *
 *	Concat lists must know their length, which is the sum of their left and
 *	right arms. To save full recursive subtree traversal this length is
 *	stored at each level in the concat words. We also cache the left arm's
 *	length whenever possible to save a pointer dereference (the right 
 *	length being the total minus left lengths).
 *
 *	Last, concat lists must know their depth for tree balancing (see 
 *	<List Tree Balancing>). This is the max depth of their left and
 *	right arms, plus one.
 *
 * Fields:
 *	Depth		- Depth of concat list. 8 bits will code up to 255 depth
 *			  levels, which is more than sufficient for balanced 
 *			  binary trees. 
 *	Left length	- Used as shortcut to avoid dereferencing the left arm. 
 *			  Zero if actual length is too large to fit.
 *	Length		- List length, which is the sum of both arms'.
 *	Left		- Left concatenated list.
 *	Right		- Right concatenated list.
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
 * Internal Macros: WORD_CONCATLIST_* Accessors
 *
 *	Concat list word field accessor macros. Both immutable and mutable
 *	versions use these fields.
 *
 *  WORD_CONCATLIST_DEPTH	- Depth of concat list. 
 *  WORD_CONCATLIST_LEFT_LENGTH	- Left arm's length or zero if too large.
 *  WORD_CONCATLIST_LENGTH	- List length.
 *  WORD_CONCATLIST_LEFT	- Left concatenated list.
 *  WORD_CONCATLIST_RIGHT	- Right concatenated list.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Concat List Word>, <WORD_CONCATLIST_INIT>, <WORD_MCONCATLIST_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_CONCATLIST_DEPTH(word)	(((uint8_t *)(word))[1])
#define WORD_CONCATLIST_LEFT_LENGTH(word) (((uint16_t *)(word))[1])
#define WORD_CONCATLIST_LENGTH(word)	(((size_t *)(word))[1])
#define WORD_CONCATLIST_LEFT(word)	(((Col_Word *)(word))[2])
#define WORD_CONCATLIST_RIGHT(word)	(((Col_Word *)(word))[3])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_CONCATLIST_INIT
 *
 *	Immutable concat list word initializer.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times 
 *			  during macro expansion)
 *	depth		- <WORD_CONCATLIST_DEPTH>
 *	length		- <WORD_CONCATLIST_LENGTH>
 *	leftLength	- <WORD_CONCATLIST_LEFT_LENGTH> (Caution: evaluated 
 *			  several times during macro expansion)
 *	left		- <WORD_CONCATLIST_LEFT>
 *	right		- <WORD_CONCATLIST_RIGHT>
 *
 * See also:
 *	<Concat List Word>, <WORD_TYPE_CONCATLIST>, <Col_ConcatLists>
 *---------------------------------------------------------------------------*/

#define WORD_CONCATLIST_INIT(word, depth, length, leftLength, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_CONCATLIST); \
    WORD_CONCATLIST_DEPTH(word) = (uint8_t) (depth); \
    WORD_CONCATLIST_LENGTH(word) = (length); \
    WORD_CONCATLIST_LEFT_LENGTH(word) = (uint16_t) ((leftLength)>UINT16_MAX?0:(leftLength)); \
    WORD_CONCATLIST_LEFT(word) = (left); \
    WORD_CONCATLIST_RIGHT(word) = (right);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MCONCATLIST_INIT
 *
 *	Mutable concat list word initializer.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times 
 *			  during macro expansion)
 *	depth		- <WORD_CONCATLIST_DEPTH>
 *	length		- <WORD_CONCATLIST_LENGTH>
 *	leftLength	- <WORD_CONCATLIST_LEFT_LENGTH> (Caution: evaluated 
 *			  several times during macro expansion)
 *	left		- <WORD_CONCATLIST_LEFT>
 *	right		- <WORD_CONCATLIST_RIGHT>
 *
 * See also:
 *	<Concat List Word>, <WORD_TYPE_MCONCATLIST>, <NewMConcatList>, <UpdateMConcatNode>
 *---------------------------------------------------------------------------*/

#define WORD_MCONCATLIST_INIT(word, depth, length, leftLength, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_MCONCATLIST); \
    WORD_CONCATLIST_DEPTH(word) = (uint8_t) (depth); \
    WORD_CONCATLIST_LENGTH(word) = (length); \
    WORD_CONCATLIST_LEFT_LENGTH(word) = (uint16_t) ((leftLength)>UINT16_MAX?0:(leftLength)); \
    WORD_CONCATLIST_LEFT(word) = (left); \
    WORD_CONCATLIST_RIGHT(word) = (right);


/*
================================================================================
Internal Section: Mutable Lists
================================================================================
*/

//FIXME: remove mutable lists and replace by generic mutable wrap.

/*---------------------------------------------------------------------------
 * Data Structure: Mutable List Word
 *
 *	Mutable lists, like mutable vectors but with no length limit.
 *
 *	Mutable lists are just wrappers around a container that can be any
 *	immutable or mutable list or vector. That way the content may change
 *	but the mutable list word doesn't. They are similar to a <Wrap Word>
 *	but with list-specific semantics. They are always toplevel, and never
 *	part of a concat subtree or a sublist.
 *
 * Requirements:
 *	Mutable list words use one single cell.
 *
 *	Mutable lists need only know their root node.
 *
 * Fields:
 *	Synonym	- Generic <Word> Synonym field.
 *	Root	- The root node, may be a vector, sublist or concat list.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                        Unused                         |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Synonym                            |
 *     +---------------------------------------------------------------+
 *   2 |                             Root                              |
 *     +---------------------------------------------------------------+
 *   3 |                            Unused                             |
 *     +---------------------------------------------------------------+
 * (end)
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_MLIST_* Accessors
 *
 *	Mutable list word field accessor macros.
 *
 *  WORD_MLIST_ROOT - Root node.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Mutable List Word>, <WORD_MLIST_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_MLIST_ROOT(word)		(((Col_Word *)(word))[2])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MLIST_INIT
 *
 *	Mutable list word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	root	- <WORD_MLIST_ROOT>
 *
 * See also:
 *	<Mutable List Word>, <WORD_TYPE_MLIST>, <Col_NewMList>
 *---------------------------------------------------------------------------*/

#define WORD_MLIST_INIT(word, root) \
    WORD_SET_TYPEID((word), WORD_TYPE_MLIST); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_MLIST_ROOT(word) = (root); \

#endif /* _COLIBRI_LIST_INT */