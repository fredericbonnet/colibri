/*
 * Internal Header: colTrieInt.h
 *
 *	This header file defines the trie map word internals of Colibri.
 *
 *	Trie maps are an implementation of generic maps that use crit-bit trees
 *	for string and integer keys.
 *
 *	They are always mutable.
 *
 * See also:
 *	<colTrie.c>, <colTrie.h>, <colMapInt.h>, <WORD_TYPE_STRTRIEMAP>, 
 *	<WORD_TYPE_INTTRIEMAP>, <WORD_TYPE_TRIENODE>, <WORD_TYPE_MTRIENODE>, 
 *	<WORD_TYPE_STRTRIENODE>, <WORD_TYPE_MSTRTRIENODE>, 
 *	<WORD_TYPE_INTTRIENODE>, <WORD_TYPE_MINTTRIENODE>, <WORD_TYPE_TRIELEAF>,
 *	<WORD_TYPE_MTRIELEAF>, <WORD_TYPE_INTTRIELEAF>, <WORD_TYPE_MINTTRIELEAF>
 */

#ifndef _COLIBRI_TRIE_INT
#define _COLIBRI_TRIE_INT


/*
================================================================================
Internal Section: Trie Maps
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Trie Map Word
 *
 *	Trie maps are an implementation of generic maps that use crit-bit trees
 *	for string, integer and custom keys.
 *
 *	They are always mutable.
 *
 * Requirements:
 *	Trie maps words use one single cell.
 *
 *	As trie maps are containers they must know their size, i.e. the number
 *	of entries they contain.
 *
 *	Trie maps store entries in leaves within a tree made of nodes. So they
 *	need to know the root node of this tree.
 *
 * Fields:
 *	Type info	- Generic <Word> Type info field.
 *	Synonym		- Generic <Word> Synonym field.
 *	Size		- Number of elements in map.
 *	Root		- Root <Trie Node Word> or <Trie Leaf Word>.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0                                                             n
 *     +---------------------------------------------------------------+
 *   0 |                           Type info                           |
 *     +---------------------------------------------------------------+
 *   1 |                            Synonym                            |
 *     +---------------------------------------------------------------+
 *   2 |                             Size                              |
 *     +---------------------------------------------------------------+
 *   3 |                             Root                              |
 *     +---------------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<WORD_TYPEINFO>, <WORD_SYNONYM>, <Trie Node Word>, <Trie Leaf Word>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_TRIEMAP_* Accessors
 *
 *	Trie map field accessor macros.
 *
 *  WORD_TRIEMAP_SIZE	- Number of elements in map.
 *  WORD_TRIEMAP_ROOT	- Root node of the trie.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Trie Map Word>, <WORD_STRTRIEMAP_INIT>, <WORD_INTTRIEMAP_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_TRIEMAP_SIZE(word)		(((size_t *)(word))[2])
#define WORD_TRIEMAP_ROOT(word)		(((Col_Word *)(word))[3])

/*---------------------------------------------------------------------------
 * Internal Constant: CUSTOMTRIEMAP_HEADER_SIZE
 *
 *	Byte size of custom trie map word header.
 *
 * See also:
 *	<Custom Word>
 *---------------------------------------------------------------------------*/

#define CUSTOMTRIEMAP_HEADER_SIZE	CELL_SIZE

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_TRIEMAP_INIT
 *
 *	Generic trie map word initializer.
 *
 *	Generic trie maps are custom word types of type <COL_TRIEMAP>.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times 
 *			  during macro expansion)
 *	typeInfo	- <WORD_SET_TYPEINFO>
 *
 * See also:
 *	<Trie Map Word>, <Col_NewCustomTrieMap>, <Col_CustomTrieMapType>,
 *	<COL_TRIEMAP>
 *---------------------------------------------------------------------------*/

#define WORD_TRIEMAP_INIT(word, typeInfo) \
    WORD_SET_TYPEINFO((word), (typeInfo)); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_TRIEMAP_SIZE(word) = 0; \
    WORD_TRIEMAP_ROOT(word) = WORD_NIL;

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_STRTRIEMAP_INIT
 *
 *	String trie map word initializer.
 *
 * Argument:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *
 * See also:
 *	<Trie Map Word>, <WORD_TYPE_STRTRIEMAP>, <Col_NewStringTrieMap>
 *---------------------------------------------------------------------------*/

#define WORD_STRTRIEMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_STRTRIEMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_TRIEMAP_SIZE(word) = 0; \
    WORD_TRIEMAP_ROOT(word) = WORD_NIL;

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_INTTRIEMAP_INIT
 *
 *	Integer trie map word initializer.
 *
 * Argument:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *
 * See also:
 *	<Trie Map Word>, <WORD_TYPE_INTTRIEMAP>, <Col_NewIntTrieMap>
 *---------------------------------------------------------------------------*/

#define WORD_INTTRIEMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTTRIEMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_TRIEMAP_SIZE(word) = 0; \
    WORD_TRIEMAP_ROOT(word) = WORD_NIL;


/*
================================================================================
Internal Section: Trie Nodes
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Trie Node Word
 *
 *	Generic trie nodes.
 *
 *	Trie maps store map entries hierarchically, in such a way that all 
 *	entries in a subtree share a common prefix. This implementation uses 
 *	crit-bit trees for keys: each node stores a critical bit, which is the 
 *	index of the bit where both branches differ. Entries where this bit is 
 *	cleared are stored in the left subtree, and those with this bit set are 
 *	stored in the right subtree, all in a recursive manner.
 *
 *	They are usually mutable but can become immutable as a result of a copy
 *	(see <Col_CopyTrieMap>): in this case the copy and the original would
 *	share most of their internals (rendered immutable in the process).
 *	Likewise, mutable operations on the copies would use copy-on-write
 *	semantics: immutable structures are copied to new mutable ones before 
 *	modification.
 *
 * Requirements:
 *	Trie node words use one single cell.
 *
 *	Trie node words must know their critical bit, which is the highest bit 
 *	of the first differing key element where left and right subtrie keys 
 *	differ. They store this information as the index of the differing key 
 *	element as well as the position of the critical bit.
 *
 *	Crit-bit tries are binary trees and thus must know their left and right
 *	subtrie.
 *
 * Fields:
 *	Diff	- Index of first differing key element in left and right keys.
 *	Bit	- Position of the critical bit in key element. 24-bit should be
 *		  sufficient for most applications (this handles up to 
 *		  16777216-bit/2MB-long key elements).
 *	Left	- Left subtrie. Keys have their critical bit cleared.
 *	Right	- Right subtrie. Keys have their critical bit set.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8                    31                               n
 *     +-------+-----------------------+-------------------------------+
 *   0 | Type  |        Bit            |        Unused (n > 32)        |
 *     +-------+-----------------------+-------------------------------+
 *   1 |                             Diff                              |
 *     +---------------------------------------------------------------+
 *   2 |                        Left (Generic)                         |
 *     +---------------------------------------------------------------+
 *   3 |                        Right (Generic)                        |
 *     +---------------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<Trie Node Word>, <Trie Map Word>, <Trie Leaf Word>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_TRIENODE_* Accessors
 *
 *	Trie node field accessor macros.
 *
 *  WORD_TRIENODE_BIT		- Get critical bit position.
 *  WORD_TRIENODE_SET_BIT	- Set critical bit position.
 *  WORD_TRIENODE_DIFF		- Index of first differing key element in left 
 *  WORD_TRIENODE_LEFT	- Left subtrie.
 *  WORD_TRIENODE_RIGHT	- Right subtrie.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Trie Node Word>, <WORD_TRIENODE_INIT>, <WORD_MTRIENODE_INIT>
 *---------------------------------------------------------------------------*/

#define TRIENODE_MASK			(UINT_MAX>>CHAR_BIT)
#ifdef COL_BIGENDIAN
#   define WORD_TRIENODE_BIT(word)	((((Col_Char *)(word))[0])&TRIENODE_MASK)
#   define WORD_TRIENODE_SET_BIT(word, mask) ((((Col_Char *)(word))[0])&=~TRIENODE_MASK,(((Col_Char *)(word))[0])|=((size)&TRIENODE_MASK))
#else
#   define WORD_TRIENODE_BIT(word)	((((Col_Char *)(word))[0]>>CHAR_BIT)&TRIENODE_MASK)
#   define WORD_TRIENODE_SET_BIT(word, mask) ((((Col_Char *)(word))[0])&=~(TRIENODE_MASK<<CHAR_BIT),(((Col_Char *)(word))[0])|=(((mask)&TRIENODE_MASK)<<CHAR_BIT))
#endif
#define WORD_TRIENODE_DIFF(word)	(((size_t *)(word))[1])
#define WORD_TRIENODE_LEFT(word)	(((Col_Word *)(word))[2])
#define WORD_TRIENODE_RIGHT(word)	(((Col_Word *)(word))[3])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_TRIENODE_INIT
 *
 *	Immutable trie node initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	diff	- <WORD_TRIENODE_DIFF>
 *	bit	- <WORD_TRIENODE_SET_BIT>
 *	left	- <WORD_TRIENODE_LEFT>
 *	right	- <WORD_TRIENODE_RIGHT>
 *
 * See also:
 *	<Trie Node Word>, <WORD_TYPE_TRIENODE>
 *---------------------------------------------------------------------------*/

#define WORD_TRIENODE_INIT(word, diff, bit, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_TRIENODE); \
    WORD_TRIENODE_SET_BIT((word), (bit)); \
    WORD_TRIENODE_DIFF(word) = (diff); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MTRIENODE_INIT
 *
 *	Mutable trie node initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	diff	- <WORD_TRIENODE_DIFF>
 *	bit	- <WORD_TRIENODE_SET_BIT>
 *	left	- <WORD_TRIENODE_LEFT>
 *	right	- <WORD_TRIENODE_RIGHT>
 *
 * See also:
 *	<Trie Node Word>, <WORD_TYPE_TRIENODE>
 *---------------------------------------------------------------------------*/

#define WORD_MTRIENODE_INIT(word, diff, bit, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_MTRIENODE); \
    WORD_TRIENODE_SET_BIT((word), (bit)); \
    WORD_TRIENODE_DIFF(word) = (diff); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);


/*
================================================================================
Internal Section: String Trie Nodes
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: String Trie Node Word
 *
 *	String trie nodes are generic trie nodes with string-specific data.
 *
 *	They are usually mutable but can become immutable as a result of a copy
 *	(see <Col_CopyTrieMap>): in this case the copy and the original would
 *	share most of their internals (rendered immutable in the process).
 *	Likewise, mutable operations on the copies would use copy-on-write
 *	semantics: immutable structures are copied to new mutable ones before 
 *	modification.
 *
 * Requirements:
 *	String trie node words use one single cell.
 *
 *	String trie node words must know their critical bit, which is the 
 *	highest bit of the first differing character where left and right 
 *	subtrie keys differ. They store this information as the index of the 
 *	differing character as well as a bit mask with this single bit set.
 *
 * Fields:
 *	Diff	- Index of first differing character in left and right keys.
 *	Mask	- Bitmask where only the critical bit is set. 24-bit is 
 *		  sufficient for Unicode character codepoints.
 *	Left	- Generic <Trie Node Word> Left field.
 *	Right	- Generic <Trie Node Word> Right field.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8                    31                               n
 *     +-------+-----------------------+-------------------------------+
 *   0 | Type  |        Mask           |        Unused (n > 32)        |
 *     +-------+-----------------------+-------------------------------+
 *   1 |                             Diff                              |
 *     +---------------------------------------------------------------+
 *   2 |                        Left (Generic)                         |
 *     +---------------------------------------------------------------+
 *   3 |                        Right (Generic)                        |
 *     +---------------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<Trie Node Word>, <Trie Map Word>, <Trie Leaf Word>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_STRTRIENODE_* Accessors
 *
 *	String trie node field accessor macros.
 *
 *  WORD_STRTRIENODE_MASK	- Get bitmask where only the critical bit is 
 *				  set.
 *  WORD_STRTRIENODE_SET_MASK	- Set bitmask to the critical bit.
 *  WORD_STRTRIENODE_DIFF	- Index of first differing character in left
 *				  and right keys.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<String Trie Node Word>, <WORD_TRIENODE_* Accessors>, 
 *	<WORD_STRTRIENODE_INIT>, <WORD_MSTRTRIENODE_INIT>
 *---------------------------------------------------------------------------*/

#define STRTRIENODE_MASK		(UINT_MAX>>CHAR_BIT)
#ifdef COL_BIGENDIAN
#   define WORD_STRTRIENODE_MASK(word)	((((Col_Char *)(word))[0])&STRTRIENODE_MASK)
#   define WORD_STRTRIENODE_SET_MASK(word, mask) ((((Col_Char *)(word))[0])&=~STRTRIENODE_MASK,(((Col_Char *)(word))[0])|=((size)&STRTRIENODE_MASK))
#else
#   define WORD_STRTRIENODE_MASK(word)	((((Col_Char *)(word))[0]>>CHAR_BIT)&STRTRIENODE_MASK)
#   define WORD_STRTRIENODE_SET_MASK(word, mask) ((((Col_Char *)(word))[0])&=~(STRTRIENODE_MASK<<CHAR_BIT),(((Col_Char *)(word))[0])|=(((mask)&STRTRIENODE_MASK)<<CHAR_BIT))
#endif
#define WORD_STRTRIENODE_DIFF(word)	(((size_t *)(word))[1])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_STRTRIENODE_INIT
 *
 *	Immutable string trie node initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	diff	- <WORD_STRTRIENODE_DIFF>
 *	mask	- <WORD_STRTRIENODE_SET_MASK>
 *	left	- <WORD_TRIENODE_LEFT>
 *	right	- <WORD_TRIENODE_RIGHT>
 *
 * See also:
 *	<String Trie Node Word>, <WORD_TYPE_STRTRIENODE>
 *---------------------------------------------------------------------------*/

#define WORD_STRTRIENODE_INIT(word, diff, mask, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_STRTRIENODE); \
    WORD_STRTRIENODE_SET_MASK(word, mask); \
    WORD_STRTRIENODE_DIFF(word) = (diff); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MSTRTRIENODE_INIT
 *
 *	Mutable string trie node initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	diff	- <WORD_STRTRIENODE_DIFF>
 *	mask	- <WORD_STRTRIENODE_SET_MASK>
 *	left	- <WORD_TRIENODE_LEFT>
 *	right	- <WORD_TRIENODE_RIGHT>
 *
 * See also:
 *	<String Trie Node Word>, <WORD_TYPE_MSTRTRIENODE>
 *---------------------------------------------------------------------------*/

#define WORD_MSTRTRIENODE_INIT(word, diff, mask, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_MSTRTRIENODE); \
    WORD_STRTRIENODE_SET_MASK((word), (mask)); \
    WORD_STRTRIENODE_DIFF(word) = (diff); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);


/*
================================================================================
Internal Section: Integer Trie Nodes
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Integer Trie Node Word
 *
 *	Integer trie nodes are generic trie nodes with integer-specific data.
 *
 *	They are usually mutable but can become immutable as a result of a copy
 *	(see <Col_CopyTrieMap>): in this case the copy and the original would
 *	share most of their internals (rendered immutable in the process).
 *	Likewise, mutable operations on the copies would use copy-on-write
 *	semantics: immutable structures are copied to new mutable ones before 
 *	modification.
 *
 * Requirements:
 *	Integer trie node words use one single cell.
 *
 *	Integer trie node words must know their critical bit, which is the 
 *	highest bit where left and right subtrie keys differ. They store this
 *	information as a bitmask with this single bit set.
 *
 * Fields:
 *	Mask	- Bitmask where only the critical bit is set.
 *	Left	- Generic <Trie Node Word> Left field.
 *	Right	- Generic <Trie Node Word> Right field.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                        Unused                         |
 *     +-------+-------------------------------------------------------+
 *   1 |                             Mask                              |
 *     +---------------------------------------------------------------+
 *   2 |                        Left (Generic)                         |
 *     +---------------------------------------------------------------+
 *   3 |                        Right (Generic)                        |
 *     +---------------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<Trie Node Word>, <Trie Map Word>, <Trie Leaf Word>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_INTTRIENODE_* Accessors
 *
 *	Integer trie node field accessor macros.
 *
 *  WORD_INTTRIENODE_MASK	- Bitmask where only the critical bit is set.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Integer Trie Node Word>, <WORD_INTTRIENODE_INIT>, 
 *	<WORD_MINTTRIENODE_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_INTTRIENODE_MASK(word)	(((intptr_t *)(word))[1])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_INTTRIENODE_INIT
 *
 *	Immutable integer trie node initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	mask	- <WORD_INTTRIENODE_MASK>
 *	left	- <WORD_TRIENODE_LEFT>
 *	right	- <WORD_TRIENODE_RIGHT>
 *
 * See also:
 *	<Integer Trie Node Word>, <WORD_TYPE_INTTRIENODE>
 *---------------------------------------------------------------------------*/

#define WORD_INTTRIENODE_INIT(word, mask, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTTRIENODE); \
    WORD_INTTRIENODE_MASK(word) = (mask); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MINTTRIENODE_INIT
 *
 *	Mutable integer trie node initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	mask	- <WORD_INTTRIENODE_MASK>
 *	left	- <WORD_TRIENODE_LEFT>
 *	right	- <WORD_TRIENODE_RIGHT>
 *
 * See also:
 *	<Integer Trie Node Word>, <WORD_TYPE_MINTTRIENODE>
 *---------------------------------------------------------------------------*/

#define WORD_MINTTRIENODE_INIT(word, mask, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_MINTTRIENODE); \
    WORD_INTTRIENODE_MASK(word) = (mask); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);


/*
================================================================================
Internal Section: Trie Leaves
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Trie Leaf Word
 *
 *	Trie leaves are generic map entries.
 *
 *	They are usually mutable but can become immutable as a result of a copy
 *	(see <Col_CopyTrieMap>): in this case the copy and the original would
 *	share most of their internals (rendered immutable in the process).
 *	Likewise, mutable operations on the copies would use copy-on-write
 *	semantics: immutable structures are copied to new mutable ones before 
 *	modification.
 *
 * Requirements:
 *	Trie leaf words, like generic map entry words, use one single cell.
 *
 *	They need no specific info besides generic map entry fields.
 *
 * Fields:
 *	Key	- Generic <Map Entry Word> Key field.
 *	Value	- Generic <Map Entry Word> Value field.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8                                                     n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                                                       |
 *     +-------+                     Unused                            +
 *   1 |                                                               |
 *     +---------------------------------------------------------------+
 *   2 |                          Key (Generic)                        |
 *     +---------------------------------------------------------------+
 *   3 |                         Value (Generic)                       |
 *     +---------------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<Map Entry Word>, <Trie Map Word>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_TRIELEAF_INIT
 *
 *	Immutable trie leaf word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_MAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Trie Leaf Word>, <WORD_TYPE_TRIELEAF>
 *---------------------------------------------------------------------------*/

#define WORD_TRIELEAF_INIT(word, key, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_TRIELEAF); \
    WORD_MAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MTRIELEAF_INIT
 *
 *	Mutable trie leaf word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_MAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Trie Leaf Word>, <WORD_TYPE_MTRIELEAF>
 *---------------------------------------------------------------------------*/

#define WORD_MTRIELEAF_INIT(word, key, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_MTRIELEAF); \
    WORD_MAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_INTTRIELEAF_INIT
 *
 *	Immutable integer trie leaf word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_INTMAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *
 * See also:
 *	<Trie Leaf Word>, <WORD_TYPE_INTTRIELEAF>
 *---------------------------------------------------------------------------*/

#define WORD_INTTRIELEAF_INIT(word, key, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTTRIELEAF); \
    WORD_INTMAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MINTTRIELEAF_INIT
 *
 *	Mutable integer trie leaf word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_INTMAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *
 * See also:
 *	<Trie Leaf Word>, <WORD_TYPE_MINTTRIELEAF>
 *---------------------------------------------------------------------------*/

#define WORD_MINTTRIELEAF_INIT(word, key, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_MINTTRIELEAF); \
    WORD_INTMAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value);


/*
================================================================================
Internal Section: Type Checking
================================================================================
*/

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_TRIEMAP
 *
 *	Type checking macro for trie maps.
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not a trie map.
 *
 * See also:
 *	<TYPECHECK>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_TRIEMAP(word) \
    TYPECHECK((Col_WordType(word) & COL_TRIEMAP), COL_ERROR_TRIEMAP, (word))

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_WORDTRIEMAP
 *
 *	Type checking macro for word-based trie maps (string or custom).
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not a word-based trie map.
 *
 * See also:
 *	<TYPECHECK>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_WORDTRIEMAP(word) \
    TYPECHECK(((Col_WordType(word) & (COL_TRIEMAP | COL_INTMAP)) \
	    == COL_TRIEMAP), COL_ERROR_WORDTRIEMAP, (word))

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_INTTRIEMAP
 *
 *	Type checking macro for integer trie maps.
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not an integer trie map.
 *
 * See also:
 *	<TYPECHECK>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_INTTRIEMAP(word) \
    TYPECHECK(((Col_WordType(word) & (COL_TRIEMAP | COL_INTMAP)) \
	    == (COL_TRIEMAP | COL_INTMAP)), COL_ERROR_INTTRIEMAP, (word))

#endif /* _COLIBRI_TRIE_INT */
