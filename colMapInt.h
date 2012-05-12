/*
 * Internal Header: colMapInt.h
 *
 *	This header file defines the generic map word internals of Colibri.
 *
 *	Maps are a collection datatype that associates keys to values. Keys can
 *	be integers, strings or generic words.
 *
 *	They are always mutable.
 *
 * See also:
 *	<colMap.c>, <colMap.h>
 */

#ifndef _COLIBRI_MAP_INT
#define _COLIBRI_MAP_INT


/*
================================================================================
Internal Section: Map Entries
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Map Entry Word
 *
 *	Generic map entries are key-value pairs. 
 *
 * Requirements:
 *	Map entry words use one single cell.
 *
 *	Map entries must store at least a key and a value.
 *
 * Fields:
 *	Key	- A word in the general case but can be a native integer for 
 *		  integer maps. 
 *	Value	- A word.
 *
 * Layout:
 *	The key and value take the cell's last two machine words. Subtypes
 *	are free to use the remaining data.
 *
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8                                                     n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                                                       |
 *     +-------+               Type-specific data                      +
 *   1 |                                                               |
 *     +---------------------------------------------------------------+
 *   2 |                              Key                              |
 *     +---------------------------------------------------------------+
 *   3 |                             Value                             |
 *     +---------------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<Hash Entry Word>, <Trie Leaf Word>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_( MAPENTRY | INTMAPENTRY )_* Accessors
 *
 *	Map entry field accessor macros.
 *
 *  WORD_MAPENTRY_KEY		- Entry key word.
 *  WORD_INTMAPENTRY_KEY	- Integer entry key.
 *  WORD_MAPENTRY_VALUE		- Entry value word.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Map Entry Word>
 *---------------------------------------------------------------------------*/

#define WORD_MAPENTRY_KEY(word)		(((Col_Word *)(word))[2])
#define WORD_INTMAPENTRY_KEY(word)	(((intptr_t *)(word))[2])
#define WORD_MAPENTRY_VALUE(word)	(((Col_Word *)(word))[3])


/*
================================================================================
Internal Section: Type Checking
================================================================================
*/

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_MAP
 *
 *	Type checking macro for maps.
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not a map.
 *
 * See also:
 *	<Col_Error>, <Col_WordIsMap>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_MAP(word) \
    if (!(Col_WordType(word) & (COL_MAP | COL_STRMAP | COL_INTMAP))) { \
	Col_Error(COL_TYPECHECK, "%x is not a map", (word)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_WORDMAP
 *
 *	Type checking macro for word-based maps (generic or string).
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not a word-based map.
 *
 * See also:
 *	<Col_Error>, <Col_WordIsStringMap>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_WORDMAP(word) \
    if (!(Col_WordType(word) & (COL_MAP | COL_STRMAP))) { \
	Col_Error(COL_TYPECHECK, "%x is not a generic or string map", (word)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_STRMAP
 *
 *	Type checking macro for string maps.
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not a string map.
 *
 * See also:
 *	<Col_Error>, <Col_WordIsStringMap>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_STRMAP(word) \
    if (!(Col_WordType(word) & COL_STRMAP)) { \
	Col_Error(COL_TYPECHECK, "%x is not a string map", (word)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_INTMAP
 *
 *	Type checking macro for integer maps.
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not an integer map.
 *
 * See also:
 *	<Col_Error>, <Col_WordIsIntMap>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_INTMAP(word) \
    if (!(Col_WordType(word) & COL_INTMAP)) { \
	Col_Error(COL_TYPECHECK, "%x is not an integer map", (word)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_MAPITER
 *
 *	Type checking macro for map iterators.
 *
 * Argument:
 *	it	- Checked iterator.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *it* is not a valid map iterator.
 *
 * See also:
 *	<Col_Error>, <Col_MapIterNull>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_MAPITER(it) \
    if (Col_MapIterNull(it)) { \
	Col_Error(COL_TYPECHECK, "%x is not a map iterator", (it)); \
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
 * Internal Macro: RANGECHECK_MAPITER
 *
 *	Range checking macro for map iterators, ensures that iterator is not
 *	at end.
 *
 * Argument:
 *	it	- Checked iterator.
 *
 * Side effects:
 *	Generate <COL_RANGECHECK> error when *it* is at end.
 *
 * See also:
 *	<Col_Error>, <Col_MapIterEnd>
 *---------------------------------------------------------------------------*/

#define RANGECHECK_MAPITER(it) \
    if (Col_MapIterEnd(it)) { \
	Col_Error(COL_RANGECHECK, "Iterator %x is at end", (it)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

#endif /* _COLIBRI_MAP_INT */
