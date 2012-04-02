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

#endif /* _COLIBRI_MAP_INT */
