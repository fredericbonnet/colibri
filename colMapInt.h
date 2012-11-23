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
 *	<TYPECHECK>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_MAP(word) \
    TYPECHECK((Col_WordType(word) & (COL_MAP | COL_INTMAP)), COL_ERROR_MAP, \
	    (word))

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_WORDMAP
 *
 *	Type checking macro for word-based maps (string or custom).
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not a word-based map.
 *
 * See also:
 *	<TYPECHECK>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_WORDMAP(word) \
    TYPECHECK(((Col_WordType(word) & (COL_MAP | COL_INTMAP)) == COL_MAP), \
	    COL_ERROR_WORDMAP, (word))

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
 *	<TYPECHECK>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_INTMAP(word) \
    TYPECHECK((Col_WordType(word) & COL_INTMAP), COL_ERROR_INTMAP, (word))

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
 *	<TYPECHECK>, <Col_MapIterNull>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_MAPITER(it) \
    TYPECHECK(!Col_MapIterNull(it), COL_ERROR_MAPITER, (it))


/*
================================================================================
Internal Section: Value Checking
================================================================================
*/

/*---------------------------------------------------------------------------
 * Internal Macro: VALUECHECK_MAPITER
 *
 *	Value checking macro for map iterators, ensures that iterator is not
 *	at end.
 *
 * Argument:
 *	it	- Checked iterator.
 *
 * Side effects:
 *	Generate <COL_VALUECHECK> error when *it* is at end.
 *
 * See also:
 *	<VALUECHECK>, <Col_MapIterEnd>
 *---------------------------------------------------------------------------*/

#define VALUECHECK_MAPITER(it) \
    VALUECHECK(!Col_MapIterEnd(it), COL_ERROR_MAPITER_END, (it))

#endif /* _COLIBRI_MAP_INT */
