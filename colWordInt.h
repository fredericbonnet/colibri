/*
 * Internal Header: colWordInt.h
 *
 *	This header file defines the word internals of Colibri.
 *
 *	Words are a generic abstract datatype framework used in conjunction with
 *	the exact generational garbage collector and the cell-based allocator.
 */

#ifndef _COLIBRI_WORD_INT
#define _COLIBRI_WORD_INT


//TODO documentation overhaul

/****************************************************************************
 * Internal Section: Word Types
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Constants: WORD_TYPE_* Constants
 *
 *	Word type identifier constants.
 *
 *  WORD_TYPE_NIL		- Nil singleton.
 *
 *  WORD_TYPE_CUSTOM		- Custom Words (<WORD_CUSTOM_INIT>)
 *
 *  WORD_TYPE_SMALLINT		- Small Integers (<WORD_SMALLINT_NEW>)
 *  WORD_TYPE_SMALLFP		- Small Floating Points
 *  WORD_TYPE_CHAR		- Character Words (<WORD_CHAR_NEW>)
 *  WORD_TYPE_SMALLSTR		- Small String Words (<WORD_SMALLSTR_DATA>, 
 *				  <WORD_SMALLSTR_SET_LENGTH>)
 *  WORD_TYPE_CIRCLIST		- Circular Lists (<WORD_CIRCLIST_NEW>)
 *  WORD_TYPE_VOIDLIST		- Void Lists (<WORD_VOIDLIST_NEW>)
 *
 *  WORD_TYPE_WRAP		- Wrap Words (<WORD_WRAP_INIT>)
 *
 *  WORD_TYPE_UCSSTR		- Fixed-Width Strings (<WORD_UCSSTR_INIT>)
 *  WORD_TYPE_UTFSTR		- Variable-Width UTF-8/16 Strings
 *				  (<WORD_UTFSTR_INIT>)
 *  WORD_TYPE_SUBROPE		- Subropes (<WORD_SUBROPE_INIT>)
 *  WORD_TYPE_CONCATROPE	- Concat Ropes (<WORD_CONCATROPE_INIT>)
 *
 *  WORD_TYPE_INT		- Integers (<WORD_INT_INIT>)
 *  WORD_TYPE_FP		- Floating Points (<WORD_FP_INIT>)
 *
 *  WORD_TYPE_VECTOR		- Immutable Vectors (<WORD_VECTOR_INIT>)
 *  WORD_TYPE_MVECTOR		- Mutable Vectors (<WORD_MVECTOR_INIT>)
 *  WORD_TYPE_SUBLIST		- <Sublist Word> (<WORD_SUBLIST_INIT>)
 *  WORD_TYPE_CONCATLIST	- Immutable <Concat List Word>
 *				  (<WORD_CONCATLIST_INIT>)
 *  WORD_TYPE_MCONCATLIST	- Mutable <Concat List Word> (<WORD_MCONCATLIST_INIT>)
 *  WORD_TYPE_MLIST		- <Mutable List Word> (<WORD_MLIST_INIT>)
 *
 *  WORD_TYPE_STRHASHMAP	- String <Hash Map Word> (<WORD_STRHASHMAP_INIT>)
 *  WORD_TYPE_INTHASHMAP	- Integer <Hash Map Word> (<WORD_INTHASHMAP_INIT>)
 *  WORD_TYPE_HASHENTRY		- Immutable <Hash Entry Word> (<WORD_HASHENTRY_INIT>)
 *  WORD_TYPE_MHASHENTRY	- Mutable <Hash Entry Word> (<WORD_MHASHENTRY_INIT>)
 *  WORD_TYPE_INTHASHENTRY	- Immutable Integer <Hash Entry Word>
 *				  (<WORD_INTHASHENTRY_INIT>)
 *  WORD_TYPE_MINTHASHENTRY	- Mutable Integer <Hash Entry Word>
 *				  (<WORD_MINTHASHENTRY_INIT>)
 *
 *  WORD_TYPE_STRTRIEMAP	- String Trie Maps (<WORD_STRTRIEMAP_INIT>)
 *  WORD_TYPE_INTTRIEMAP	- Integer Trie Maps (<WORD_INTTRIEMAP_INIT>)
 *  WORD_TYPE_STRTRIENODE	- Immutable String Trie Nodes 
 *				  (<WORD_STRTRIENODE_INIT>)
 *  WORD_TYPE_MSTRTRIENODE	- Mutable String Trie Nodes 
 *				  (<WORD_MSTRTRIENODE_INIT>)
 *  WORD_TYPE_INTTRIENODE	- Immutable Integer Trie Nodes 
 *				  (<WORD_INTTRIENODE_INIT>)
 *  WORD_TYPE_MINTTRIENODE	- Mutable Integer Trie Nodes 
 *				  (<WORD_MINTTRIENODE_INIT>)
 *  WORD_TYPE_TRIELEAF		- Immutable Trie Leaves (<WORD_TRIELEAF_INIT>)
 *  WORD_TYPE_MTRIELEAF		- Mutable Trie Leaves (<WORD_MTRIELEAF_INIT>)
 *  WORD_TYPE_INTTRIELEAF	- Immutable Integer Trie Leaves
 *				  (<WORD_INTTRIELEAF_INIT>)
 *  WORD_TYPE_MINTTRIELEAF	- Mutable Integer Trie Leaves
 *				  (<WORD_MINTTRIELEAF_INIT>)
 *
 *  WORD_TYPE_STRBUF		- String buffer (<WORD_STRBUF_INIT>)
 *
 *  WORD_TYPE_REDIRECT		- Redirects
 *
 *  WORD_TYPE_UNKNOWN		- Used as a tag in the source code to mark 
 *				  places where predefined type specific code is
 *				  needed. Search for this tag when adding new 
 *				  predefined word types.
 *
 * Note:
 *	Predefined type IDs for regular word types are chosen so that their bit
 *	0 is cleared, their bit 1 is set, and their value fit into a byte. This
 *	gives up to 64 predefined type IDs (2-254 with steps of 4).
 *
 *	Immediate word type IDs use negative values to avoid clashes with 
 *	regular word type IDs.
 *---------------------------------------------------------------------------*/
 
#define WORD_TYPE_NIL		0

#define WORD_TYPE_CUSTOM	-1

#define WORD_TYPE_SMALLINT	-2
#define WORD_TYPE_SMALLFP	-3
#define WORD_TYPE_CHAR		-4
#define WORD_TYPE_SMALLSTR	-5
#define WORD_TYPE_CIRCLIST	-6
#define WORD_TYPE_VOIDLIST	-7

#define WORD_TYPE_WRAP		2

#define WORD_TYPE_UCSSTR	6
#define WORD_TYPE_UTFSTR	10
#define WORD_TYPE_SUBROPE	14
#define WORD_TYPE_CONCATROPE	18

#define WORD_TYPE_INT		22
#define WORD_TYPE_FP		26

#define WORD_TYPE_VECTOR	30
#define WORD_TYPE_MVECTOR	34
#define WORD_TYPE_SUBLIST	38
#define WORD_TYPE_CONCATLIST	42
#define WORD_TYPE_MCONCATLIST	46
#define WORD_TYPE_MLIST		50

#define WORD_TYPE_STRHASHMAP	54
#define WORD_TYPE_INTHASHMAP	58
#define WORD_TYPE_HASHENTRY	62
#define WORD_TYPE_MHASHENTRY	66
#define WORD_TYPE_INTHASHENTRY	70
#define WORD_TYPE_MINTHASHENTRY	74

#define WORD_TYPE_STRTRIEMAP	78
#define WORD_TYPE_INTTRIEMAP	82
#define WORD_TYPE_STRTRIENODE	86
#define WORD_TYPE_MSTRTRIENODE	90
#define WORD_TYPE_INTTRIENODE	94
#define WORD_TYPE_MINTTRIENODE	98
#define WORD_TYPE_TRIELEAF	102
#define WORD_TYPE_MTRIELEAF	106
#define WORD_TYPE_INTTRIELEAF	110
#define WORD_TYPE_MINTTRIELEAF	114

#define WORD_TYPE_STRBUF	118

#ifdef PROMOTE_COMPACT
#   define WORD_TYPE_REDIRECT	254
#endif

/*---------------------------------------------------------------------------
 * Internal Variable: immediateWordTypes
 *
 *	Lookup table for immediate word types, gives the word type ID from the
 *	first 5 bits of the word value.
 *
 *	Defined as static so that all modules have their own for compiler 
 *	optimization.
 *
 * See also:
 *	<WORD_TYPE>, 
 *	<WORD_( SMALLINT | SMALLFP | CHAR | SMALLSTR | CIRCLIST | VOIDLIST )_* Accessors>
 *---------------------------------------------------------------------------*/

static const int immediateWordTypes[32] = {
    WORD_TYPE_NIL,	/* 00000 */
    WORD_TYPE_SMALLINT,	/* 00001 */
    WORD_TYPE_SMALLFP,	/* 00010 */
    WORD_TYPE_SMALLINT,	/* 00011 */
    WORD_TYPE_CHAR,	/* 00100 */
    WORD_TYPE_SMALLINT,	/* 00101 */
    WORD_TYPE_SMALLFP,	/* 00110 */
    WORD_TYPE_SMALLINT,	/* 00111 */
    WORD_TYPE_CIRCLIST,	/* 01000 */
    WORD_TYPE_SMALLINT,	/* 01001 */
    WORD_TYPE_SMALLFP,	/* 01010 */
    WORD_TYPE_SMALLINT,	/* 01011 */
    WORD_TYPE_SMALLSTR,	/* 01100 */
    WORD_TYPE_SMALLINT,	/* 01101 */
    WORD_TYPE_SMALLFP,	/* 01110 */
    WORD_TYPE_SMALLINT,	/* 01111 */
    WORD_TYPE_NIL,	/* 10000 */
    WORD_TYPE_SMALLINT,	/* 10001 */
    WORD_TYPE_SMALLFP,	/* 10010 */
    WORD_TYPE_SMALLINT,	/* 10011 */
    WORD_TYPE_VOIDLIST,	/* 10100 */
    WORD_TYPE_SMALLINT,	/* 10101 */
    WORD_TYPE_SMALLFP,	/* 10110 */
    WORD_TYPE_SMALLINT,	/* 10111 */
    WORD_TYPE_CIRCLIST,	/* 11000 */
    WORD_TYPE_SMALLINT,	/* 11001 */
    WORD_TYPE_SMALLFP,	/* 11010 */
    WORD_TYPE_SMALLINT,	/* 11011 */
    WORD_TYPE_CIRCLIST,	/* 11100 */
    WORD_TYPE_SMALLINT,	/* 11101 */
    WORD_TYPE_SMALLFP,	/* 11110 */
    WORD_TYPE_SMALLINT,	/* 11111 */
};

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_TYPE
 *
 *	Get word type identifier.
 *
 * Argument:
 *	word	- Word to get type for. (Caution: evaluated several times during
 *		  macro expansion)
 *
 * Result:
 *	Word type identifier.
 *
 * See also:
 *	<WORD_TYPE_* Constants>, <immediateWordTypes>
 *---------------------------------------------------------------------------*/

/* Careful: don't give arguments with side effects! */
#define WORD_TYPE(word) \
    /* Nil? */							\
    ((!(word))?				WORD_TYPE_NIL		\
    /* Immediate Word? */					\
    :(((uintptr_t)(word))&15)?		immediateWordTypes[(((uintptr_t)(word))&31)] \
    /* Predefined Type ID? */					\
    :((((uint8_t *)(word))[0])&2)?	WORD_TYPEID(word)	\
    /* Custom Type */						\
    :					WORD_TYPE_CUSTOM)


/****************************************************************************
 * Internal Section: Immediate Words
 *
 * Files:
 *	<colWord.c>, <colWord.h>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_( SMALLINT | SMALLFP | CHAR | SMALLSTR | CIRCLIST | VOIDLIST )_* Accessors
 *
 *	Immediate word field accessor macros.
 *
 *	Immediate words are immutable datatypes that store their value directly 
 *	in the word identifier, contrary to regular words whose identifier is a 
 *	pointer to a cell-based structure. As cells are aligned on a multiple of
 *	their size (16 bytes on 32-bit systems), this means that a number of low
 *	bits are always zero in regular word identifiers (4 bits on 32-bit 
 *	systems with 16-byte cells). Immediate values are distinguished from 
 *	regular pointers by setting one of these bits. Immediate word types are
 *	identified by these bit patterns, called tags. The following binary tags
 *	are recognized:
 *
 * Nil:
 *	<WORD_TYPE_NIL>
 *
 * (start table)
 *      0                                                             n
 *     +---------------------------------------------------------------+
 *   0 |                               0                               |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	Nil is a singleton word whose identifier is <WORD_NIL> or zero.
 *
 *
 * Small Integers:
 *	<WORD_TYPE_SMALLINT>
 *
 * (start table)
 *      0 1                                                           n
 *     +-+-------------------------------------------------------------+
 *   0 |1|                            Value                            |
 *     +-+-------------------------------------------------------------+
 * (end)
 *
 *	Small integer words are integers whose width is one bit less than 
 *	machine integers. Larger integers are cell-based.
 *
 *	WORD_SMALLINT_GET	- Get value of small integer word.
 *
 *
 * Small Floating Points:
 *	<WORD_TYPE_SMALLFP>
 *
 * (start table)
 *      01 2                                                           n
 *     +--+------------------------------------------------------------+
 *   0 |01|                           Value                            |
 *     +--+------------------------------------------------------------+
 * (end)
 *
 *	Small floating point words are floating points whose lower 2 bits of the
 *	mantissa are zero, so that they are free for the tag bits. Other values
 *	are cell-based.
 *
 *	Note that because of language restrictions (bitwise operations on 
 *	floating points are forbidden), we have to use the intermediary union 
 *	type <FloatConvert> for conversions.
 *
 *	WORD_SMALLFP_GET	- Get value of small floating point word.
 *
 *
 * Unicode Chars:
 *	<WORD_TYPE_CHAR>
 *
 * (start table)
 *      0   4 5	  7 8                31                               n
 *     +-----+-----+-------------------+-------------------------------+
 *   0 |00100|Width|     Codepoint     |        Unused (n > 32)        |
 *     +-----+-----+-------------------+-------------------------------+
 * (end)
 *
 *	Unicode char words can store one Unicode character (24-bit, which is
 *	sufficient in the Unicode/UCS standard, see <COL_CHAR_MAX>).
 *	
 *	WORD_CHAR_GET	- Get Unicode codepoint of char word.
 *	WORD_CHAR_WIDTH	- Get char width used at creation time.
 *
 *
 * Small Strings:
 *	<WORD_TYPE_SMALLSTR>
 *
 * (start table)
 *      0   4 5            7 8                                        n
 *     +-----+--------------+------------------------------------------+
 *   0 |00110| 0<=Length<=7 |                  String                  |
 *     +-----+--------------+------------------------------------------+
 * (end)
 *
 *	Small string words can store short 8-bit strings. Length is the machine
 *	word size minus one (i.e. 3 on 32-bit, 7 on 64-bit). Larger strings are
 *	cell-based.
 *	
 *	WORD_SMALLSTR_LENGTH		- Get small string length.
 *	WORD_SMALLSTR_SET_LENGTH	- Set small string length.
 *	WORD_SMALLSTR_DATA		- Small string data.
 *
 *
 * Circular lists:
 *	<WORD_TYPE_CIRCLIST>
 *
 * (start table)
 *      0  3 4                                                        n
 *     +----+----------------------------------------------------------+
 *   0 |0001|                    Core (regular list)                   |
 *     +----+----------------------------------------------------------+
 *
 *      0   4 5                                                       n
 *     +-----+---------------------------------------------------------+
 *   0 |00111|                Core length (void list)                   |
 *     +-----+---------------------------------------------------------+
 * (end)
 *	
 *	Circular lists are lists made of a core list that repeats infinitely.
 *	It can be either a regular list or an immediate void list.
 *	
 *	WORD_CIRCLIST_CORE	- Get core list.
 *
 *
 * Void lists:
 *	<WORD_TYPE_VOIDLIST>
 *
 * (start table)
 *      0   4 5                                                       n
 *     +-----+---------------------------------------------------------+
 *   0 |00101|                         Length                          |
 *     +-----+---------------------------------------------------------+
 * (end)
 *
 *	Void lists are lists whose elements are all nil. Length width is the
 *	machine word width minus 4 bits, so the maximum length is about 1/16th
 *	of the theoretical maximum. Larger void lists are built by concatenating
 *	several shorter immediate void lists.
 *	
 *	WORD_VOIDLIST_LENGTH	- Get void list length.
 *
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 *	Void list tag bit comes after the circular list tag so that void lists
 *	can be made circular. Void circular lists thus combine both tag bits.
 *
 * See also:
 *	<WORD_TYPE>, <WORD_TYPE_* Constants>
 *---------------------------------------------------------------------------*/

#define WORD_SMALLINT_GET(word)		(((intptr_t)(word))>>1)
#define WORD_SMALLFP_GET(word, c)	((c).w = (word), (c).i &= ~2, (c).f)
#define WORD_CHAR_GET(word)		((Col_Char)(((uintptr_t)(word))>>8))
#define WORD_CHAR_WIDTH(word)		((((uintptr_t)(word))>>5)&7)
#define WORD_SMALLSTR_LENGTH(value)	((((uintptr_t)(value))&0xFF)>>5)
#define WORD_SMALLSTR_SET_LENGTH(word, length) (*((uintptr_t *)&(word)) = ((length)<<5)|12)
#ifdef COL_BIGENDIAN
#   define WORD_SMALLSTR_DATA(word)	((Col_Char1  *)&(word))
#else
#   define WORD_SMALLSTR_DATA(word)	(((Col_Char1 *)&(word))+1)
#endif
#define WORD_CIRCLIST_CORE(word)	((Col_Word)(((uintptr_t)(word))&~8))
#define WORD_VOIDLIST_LENGTH(word)	(((size_t)(intptr_t)(word))>>5)

/*---------------------------------------------------------------------------
 * Internal Define: SMALLFP_TYPE
 *
 *	C Type used by immediate floating point words.
 *
 * See also:
 *	<FloatConvert>, <WORD_SMALLFP_NEW>
 *---------------------------------------------------------------------------*/

#if SIZE_BIT == 32
#   define SMALLFP_TYPE		float
#elif SIZE_BIT == 64
#   define SMALLFP_TYPE		double
#endif

/*---------------------------------------------------------------------------
 * Internal Typedef: FloatConvert
 *
 *	Utility union type for immediate floating point words.
 *
 * See also:
 *	<WORD_TYPE_SMALLFP>, <WORD_SMALLFP_GET>, <WORD_SMALLFP_NEW>
 *---------------------------------------------------------------------------*/

typedef union {
    Col_Word w;
    uintptr_t i;
    SMALLFP_TYPE f;
} FloatConvert;

/*---------------------------------------------------------------------------
 * Internal Constants: ( SMALLINT | SMALLSTR | VOIDLIST )_* Constants
 *
 *	Immediate word type limit and bitmask constants.
 *
 *  SMALLINT_MAX	- Maximum value of small integer words.
 *  SMALLINT_MIN	- Minimum value of small integer words.
 *  SMALLSTR_MAX_LENGTH	- Maximum length of small string words.
 *  VOIDLIST_MAX_LENGTH	- Maximum length of void list words.
 *---------------------------------------------------------------------------*/

#define SMALLINT_MAX		(INTPTR_MAX>>1)
#define SMALLINT_MIN		(INTPTR_MIN>>1)
#define SMALLSTR_MAX_LENGTH	(sizeof(Col_Word)-1)
#define VOIDLIST_MAX_LENGTH	(SIZE_MAX>>5)

/*---------------------------------------------------------------------------
 * Internal Constant: WORD_SMALLSTR_EMPTY
 *
 *	Empty string immediate singleton.
 *---------------------------------------------------------------------------*/

#define WORD_SMALLSTR_EMPTY	((Col_Word) 12)

/*---------------------------------------------------------------------------
 * Internal Constant: WORD_LIST_EMPTY
 *
 *	Empty list immediate singleton.
 *---------------------------------------------------------------------------*/

#define WORD_LIST_EMPTY		WORD_VOIDLIST_NEW(0)

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_SMALLINT_NEW
 *
 *	Small integer word creation.
 *
 * Argument:
 *	value	- Integer value. Must be between <SMALLINT_MIN> and 
 *		  <SMALLINT_MAX>.
 *
 * Result:
 *	The new small integer word.
 *---------------------------------------------------------------------------*/

#define WORD_SMALLINT_NEW(value) \
    ((Col_Word)((((intptr_t)(value))<<1)|1))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_SMALLFP_NEW
 *
 *	Small floating point word creation.
 *
 * Argument:
 *	value	- Floating point value.
 *	c	- <FloatConvert> conversion structure.
 *
 * Result:
 *	The new small floating point word.
 *---------------------------------------------------------------------------*/

#define WORD_SMALLFP_NEW(value, c) \
    ((c).f = (SMALLFP_TYPE)(value), (c).i |= 2, (c).w)

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_CHAR_NEW
 *
 *	Unicode char word creation.
 *
 * Argument:
 *	value	- Unicode codepoint.
 *	width	- Character width.
 *
 * Result:
 *	The new Unicode char word.
 *---------------------------------------------------------------------------*/

#define WORD_CHAR_NEW(value, width) \
    ((Col_Word)((((uintptr_t)(value))<<8)|(((uintptr_t)(width))<<5)|4))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_CIRCLIST_NEW
 *
 *	Circular list word creation.
 *
 * Argument:
 *	core	- Core list. Must be acyclic.
 *
 * Result:
 *	The new circular list word.
 *---------------------------------------------------------------------------*/

#define WORD_CIRCLIST_NEW(core) \
    ((Col_Word)(((uintptr_t)(core))|8))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_VOIDLIST_NEW
 *
 *	Void list word creation.
 *
 * Argument:
 *	length	- Void list length. Must be less than <VOIDLIST_MAX_LENGTH>.
 *
 * Result:
 *	The new void list word.
 *---------------------------------------------------------------------------*/

#define WORD_VOIDLIST_NEW(length) \
    ((Col_Word)(intptr_t)((((size_t)(length))<<5)|20))


/****************************************************************************
 * Internal Section: Regular Words
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_* Accessors
 *
 *	Cell-based word field accessor macros.
 *
 * Common Layout:
 *	Regular word store their data in cells whose 1st machine word is used 
 *	for the word type field, which is either a numerical ID for predefined 
 *	types or a pointer to a <Col_CustomWordType> structure for custom types. 
 *	As such structures are always word-aligned, this means that the two 
 *	least significant bits of their pointer value are zero (for 
 *	architectures with at least 32 bit words) and are free for our purpose. 
 * 
 *	On little endian architectures, the LSB of the type pointer is the 
 *	cell's byte 0. On big endian architectures, we rotate the pointer value 
 *	one byte to the right so that the original LSB ends up on byte 0. That 
 *	way the two reserved bits are on byte 0 for both predefined type IDs and
 *	type pointers.
 *
 *	We use bit 0 of the type field as the pinned flag for both predefined 
 *	type IDs and type pointers. Given the above, this bit is always on byte
 *	0. When set, this means that the word isn't movable; its address remains
 *	fixed as long as this flag is set. Words can normally be moved to the 
 *	upper generation pool during the compaction phase of the GC.
 *
 *	On all architectures the single-cell layout is as follows:
 *
 * (start table)
 *      0 1                                                           n
 *     +-+-------------------------------------------------------------+
 *   0 |P|                                                             |
 *     +-+                                                             +
 *     .                                                               .
 *     .                      Type-specific data                       .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_PINNED		- Get pinned flag.
 *	WORD_SET_PINNED		- Set pinned flag.
 *	WORD_CLEAR_PINNED	- Clear pinned flag.
 *
 *
 * Predefined Word Types:
 *	Predefined type IDs for regular word types are chosen so that their bit
 *	0 is cleared and bit 1 is set, to distinguish them with type pointers 
 *	and avoid value clashes.
 *	
 *	On all architectures the single-cell layout is as follows:
 *
 * (start table)
 *      0 1 2    7                                                    n
 *     +-+-+------+----------------------------------------------------+
 *   0 |P|1| Type |                                                    |
 *     +-+-+------+                                                    +
 *     .                                                               .
 *     .                      Type-specific data                       .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_TYPEID		- Get type ID for predefined word type.
 *	WORD_SET_TYPEID		- Set type ID for predefined word type. Also 
 *				  clears the pinned flag.
 *
 *
 * Custom Word Types:
 *	On all architectures the single-cell layout is as follows:
 *
 * (start table)
 *      0 1                                                           n
 *     +-+-+-----------------------------------------------------------+
 *   0 |P|0|                         Type                              |
 *     +-+-+-----------------------------------------------------------+
 *     .                                                               .
 *     .                      Type-specific data                       .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_TYPEINFO		- Get <Col_CustomWordType> custom type 
 *				  descriptor. (Caution: word is evaluated 
 *				  several times during macro expansion)
 *	WORD_SET_TYPEINFO	- Set <Col_CustomWordType> custom type 
 *				  descriptor. Also clears the pinned flag. 
 *				  (Caution: word is evaluated several times 
 *				  during macro expansion)
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_TYPE>, <Col_CustomWordType>, <WORD_TYPE_* Constants>
 *---------------------------------------------------------------------------*/

#define WORD_PINNED(word)	(((uint8_t *)(word))[0] & 1)
#define WORD_SET_PINNED(word)	(((uint8_t *)(word))[0] |= 1)
#define WORD_CLEAR_PINNED(word)	(((uint8_t *)(word))[0] &= ~1)

#define WORD_TYPEID(word)	((((uint8_t *)(word))[0])&~1)
#define WORD_SET_TYPEID(word, type) (((uint8_t *)(word))[0] = (type))

#ifdef COL_BIGENDIAN
#   define ROTATE_LEFT(value) \
	(((value)>>((sizeof(value)-1)*8)) | ((value)<<8))
#   define ROTATE_RIGHT(value) \
	(((value)<<((sizeof(value)-1)*8)) | ((value)>>8))
#   define WORD_TYPEINFO(word) \
	((Col_CustomWordType *)(ROTATE_LEFT(*(uintptr_t *)(word))&~1))
#   define WORD_SET_TYPEINFO(word, addr) \
	(*(uintptr_t *)(word) = ROTATE_RIGHT(((uintptr_t)(addr))))
#else
#   define WORD_TYPEINFO(word) \
	((Col_CustomWordType *)((*(uintptr_t *)(word))&~1))
#   define WORD_SET_TYPEINFO(word, addr) \
	(*(uintptr_t *)(word) = ((uintptr_t)(addr)))
#endif

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_REDIRECT_* Accessors
 *
 *	Redirect word field accessor macros.
 *
 *	Redirects replace existing words during compacting promotion. They only 
 *	exist during the GC.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                        Unused                         |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Source                             |
 *     +---------------------------------------------------------------+
 *   2 |                                                               |
 *     +                            Unused                             +
 *   3 |                                                               |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_REDIRECT_SOURCE	- New location of the word.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *---------------------------------------------------------------------------*/

#ifdef PROMOTE_COMPACT
#   define WORD_REDIRECT_SOURCE(word)	(((Col_Word *)(word))[1])
#endif /* PROMOTE_COMPACT*/


/****************************************************************************
 * Internal Section: Custom Words
 *
 * Files:
 *	<colWord.c>, <colWord.h>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_CUSTOM_* Accessors
 *
 *	Custom word field accessor macros.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0                                                             n
 *     +---------------------------------------------------------------+
 *   0 |                           Type info                           |
 *     +---------------------------------------------------------------+
 *   1 |                        Next (optional)                        |
 *     +---------------------------------------------------------------+
 *     .                                                               .
 *     .                      Type-specific data                       .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_CUSTOM_NEXT    - Words with a freeProc are singly-linked together 
 *			      using this field, so that unreachable words get 
 *			      swept properly upon GC.
 *	WORD_CUSTOM_DATA    - Beginning of custom word data.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 *
 * See also:
 *	<WORD_CUSTOM_INIT>, <SweepUnreachableCells>, <Col_CustomWordType>
 *---------------------------------------------------------------------------*/

#define WORD_CUSTOM_NEXT(word)		(((Col_Word *)(word))[1])
#define WORD_CUSTOM_DATA(word, typeInfo) ((void *)(&WORD_CUSTOM_NEXT(word)+((typeInfo)->freeProc?1:0)))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_CUSTOM_INIT
 *
 *	Custom word initializer.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times
 *			  during macro expansion)
 *	typeInfo	- <WORD_TYPEINFO>
 *
 * See also:
 *	<Col_NewCustomWord>
 *---------------------------------------------------------------------------*/

#define WORD_CUSTOM_INIT(word, typeInfo) \
    WORD_SET_TYPEINFO((word), (typeInfo));  

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_CUSTOM_SIZE
 *
 *	Get number of cells taken by the custom word.
 *
 * Arguments:
 *	size		- Byte size of custom word data.
 *	typeInfo	- <WORD_TYPEINFO>
 *
 * Result:
 *	Number of cells taken by word.
 *---------------------------------------------------------------------------*/

#define WORD_CUSTOM_SIZE(size, typeInfo) \
    (NB_CELLS((size)+sizeof(Col_CustomWordType *)+((typeInfo)->freeProc?sizeof(Cell *):0)))


/****************************************************************************
 * Internal Section: Predefined Words
 *
 * Files:
 *	<colWord.c>, <colWord.h>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_WRAP_* Accessors
 *
 *	Wrap word field accessor macros.
 *
 *	Words may have synonyms that can take any accepted word value: immediate 
 *	values (inc. nil), or cell-based words. Words can thus be part of chains
 *	of synonyms having different types, but with semantically identical 
 *	values. Such chains form a circular linked list using this field. The 
 *	order of words in a synonym chain has no importance. Some word types 
 *	have no synonym field, in this case they must be wrapped into structures
 *	that have one when they are added to a chain of synonyms. For 
 *	performance reasons, all word types with a synonym field use the same 
 *	location for this field.
 *
 *	This type is a generic wrapper for words needing a synonym field.
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
 *   2 |                            Source                             |
 *     +---------------------------------------------------------------+
 *   3 |                            Unused                             |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_SYNONYM		- Synonym for the word. This accessor is generic
 *				  for all word types having a synonym field.
 *	WORD_WRAP_SOURCE	- Word whose synonym is given.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_WRAP_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_SYNONYM(word)	(((Col_Word *)(word))[1])
#define WORD_WRAP_SOURCE(word)	(((Col_Word *)(word))[2])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_WRAP_INIT
 *
 *	Wrap word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	source	- <WORD_WRAP_SOURCE>
 *
 * See also:
 *	<WORD_TYPE_WRAP>, <AddSynonymField>
 *---------------------------------------------------------------------------*/

#define WORD_WRAP_INIT(word, source) \
    WORD_SET_TYPEID((word), WORD_TYPE_WRAP); \
    WORD_WRAP_SOURCE(word) = (source);

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_INT_* Accessors
 *
 *	Integer word field accessor macros.
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
 *   2 |                             Value                             |
 *     +---------------------------------------------------------------+
 *   3 |                            Unused                             |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_INT_VALUE	- Integer value.
 *
 * See also:
 *	<WORD_INT_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_INT_VALUE(word)	(*(((intptr_t *)(word))+1))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_INT_INIT
 *
 *	Integer word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	value	- <WORD_INT_VALUE>
 *
 * See also:
 *	<WORD_TYPE_INT>, <Col_NewIntWord>
 *---------------------------------------------------------------------------*/

#define WORD_INT_INIT(word, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_INT); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_INT_VALUE(word) = (value);

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_FP_* Accessors
 *
 *	Floating point word field accessor macros.
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
 *   2 |                             Value                             |
 *     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 *   3 |                       (Unused on 64 bit)                      |
 *     +---------------------------------------------------------------+
 * (end)
 *
 *	WORD_FP_VALUE	- Floating point value.
 *
 * See also:
 *	<WORD_FP_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_FP_VALUE(word)	(*(((double *)(word))+1))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_FP_INIT
 *
 *	Floating point word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	value	- <WORD_FP_VALUE>
 *
 * See also:
 *	<WORD_TYPE_FP>, <Col_NewFloatWord>
 *---------------------------------------------------------------------------*/

#define WORD_FP_INIT(word, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_FP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_FP_VALUE(word) = (value);

#endif /* _COLIBRI_WORD_INT */
