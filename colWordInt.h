/*
 * Internal Header: colWordInt.h
 *
 *	This header file defines the word internals of Colibri.
 *
 *	Words are a generic abstract datatype framework used in conjunction with
 *	the exact generational garbage collector and the cell-based allocator.
 *
 * See also:
 *	<colWord.c>, <colWord.h>
 */

#ifndef _COLIBRI_WORD_INT
#define _COLIBRI_WORD_INT


/*
================================================================================
Internal Section: Word Types
================================================================================
*/

/*---------------------------------------------------------------------------
 * Internal Constants: WORD_TYPE_* Constants
 *
 *	Word type identifier constants.
 *
 *  WORD_TYPE_NIL		- Nil singleton.
 *
 *  WORD_TYPE_CUSTOM		- <Custom Word>
 *
 *  WORD_TYPE_SMALLINT		- <Small Integer Word>
 *  WORD_TYPE_SMALLFP		- <Small Floating Point Word>
 *  WORD_TYPE_CHAR		- <Character Word>
 *  WORD_TYPE_SMALLSTR		- <Small String Word>
 *  WORD_TYPE_CIRCLIST		- <Circular List Word>
 *  WORD_TYPE_VOIDLIST		- <Void List Word>
 *
 *  WORD_TYPE_WRAP		- <Wrap Word>
 *
 *  WORD_TYPE_UCSSTR		- <Fixed-Width UCS String Word>
 *  WORD_TYPE_UTFSTR		- <Variable-Width UTF String Word>
 *  WORD_TYPE_SUBROPE		- <Subrope Word>
 *  WORD_TYPE_CONCATROPE	- <Concat Rope Word>
 *
 *  WORD_TYPE_VECTOR		- <Immutable Vector Word>
 *  WORD_TYPE_MVECTOR		- <Mutable Vector Word>
 *  WORD_TYPE_SUBLIST		- <Sublist Word>
 *  WORD_TYPE_CONCATLIST	- Immutable <Concat List Word>
 *  WORD_TYPE_MCONCATLIST	- Mutable <Concat List Word>
 *
 *  WORD_TYPE_STRHASHMAP	- String <Hash Map Word>
 *  WORD_TYPE_INTHASHMAP	- Integer <Hash Map Word>
 *  WORD_TYPE_HASHENTRY		- Immutable <Hash Entry Word>
 *  WORD_TYPE_MHASHENTRY	- Mutable <Hash Entry Word>
 *  WORD_TYPE_INTHASHENTRY	- Immutable Integer <Hash Entry Word>
 *  WORD_TYPE_MINTHASHENTRY	- Mutable Integer <Hash Entry Word>
 *
 *  WORD_TYPE_STRTRIEMAP	- String <Trie Map Word>
 *  WORD_TYPE_INTTRIEMAP	- Integer <Trie Map Word>
 *  WORD_TYPE_STRTRIENODE	- Immutable <String Trie Node Word>
 *  WORD_TYPE_MSTRTRIENODE	- Mutable <String Trie Node Word>
 *  WORD_TYPE_INTTRIENODE	- Immutable <Integer Trie Node Word>
 *  WORD_TYPE_MINTTRIENODE	- Mutable <Integer Trie Node Word>
 *  WORD_TYPE_TRIENODE	- Immutable <Custom Trie Node Word>
 *  WORD_TYPE_MTRIENODE	- Mutable <Custom Trie Node Word>
 *  WORD_TYPE_TRIELEAF		- Immutable <Trie Leaf Word>
 *  WORD_TYPE_MTRIELEAF		- Mutable <Trie Leaf Word>
 *  WORD_TYPE_INTTRIELEAF	- Immutable Integer <Trie Leaf Word>
 *  WORD_TYPE_MINTTRIELEAF	- Mutable Integer <Trie Leaf Word>
 *
 *  WORD_TYPE_STRBUF		- <String Buffer Word>
 *
 *  WORD_TYPE_REDIRECT		- Redirects <TODO>
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

#define WORD_TYPE_VECTOR	22
#define WORD_TYPE_MVECTOR	26
#define WORD_TYPE_SUBLIST	30
#define WORD_TYPE_CONCATLIST	34
#define WORD_TYPE_MCONCATLIST	38

#define WORD_TYPE_STRHASHMAP	42
#define WORD_TYPE_INTHASHMAP	46
#define WORD_TYPE_HASHENTRY	50
#define WORD_TYPE_MHASHENTRY	54
#define WORD_TYPE_INTHASHENTRY	58
#define WORD_TYPE_MINTHASHENTRY	62

#define WORD_TYPE_STRTRIEMAP	66
#define WORD_TYPE_INTTRIEMAP	70
#define WORD_TYPE_TRIENODE	74
#define WORD_TYPE_MTRIENODE	78
#define WORD_TYPE_STRTRIENODE	82
#define WORD_TYPE_MSTRTRIENODE	86
#define WORD_TYPE_INTTRIENODE	90
#define WORD_TYPE_MINTTRIENODE	94
#define WORD_TYPE_TRIELEAF	98
#define WORD_TYPE_MTRIELEAF	102
#define WORD_TYPE_INTTRIELEAF	106
#define WORD_TYPE_MINTTRIELEAF	110

#define WORD_TYPE_STRBUF	114

#ifdef PROMOTE_COMPACT
#   define WORD_TYPE_REDIRECT	254
#endif

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
 *	<WORD_TYPE_* Constants>, <immediateWordTypes>, <Immediate Words>,
 *	<Regular Words>
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


/*
================================================================================
Internal Section: Immediate Words

	Immediate words are immutable datatypes that store their value directly 
	in the word identifier, contrary to regular words whose identifier is a 
	pointer to a cell-based structure. As cells are aligned on a multiple of
	their size (16 bytes on 32-bit systems), this means that a number of low
	bits are always zero in regular word identifiers (4 bits on 32-bit 
	systems with 16-byte cells). Immediate values are distinguished from 
	regular pointers by setting one of these bits. Immediate word types are
	identified by these bit patterns, called tags.
================================================================================
*/

/*---------------------------------------------------------------------------
 * Internal Variable: immediateWordTypes
 *
 *	Lookup table for immediate word types, gives the word type ID from the
 *	first 5 bits of the word value (= the tag).
 *
 *	Defined as static so that all modules have their own for potential 
 *	compiler optimizations.
 *
 * See also:
 *	<WORD_TYPE>
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


/*
================================================================================
Internal Section: Small Integers
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Small Integer Word
 *
 *	Small integer words are immediate words storing integers whose width 
 *	is one bit less than machine integers. Larger integers are cell-based.
 *
 * Requirements:
 *	Small integer words need to store the integer value in the word 
 *	identifier.
 *
 * Fields:
 *	Value	- Integer value of word.
 *
 * Layout:
 *	On all architectures the word layout is as follows:
 *
 * (start table)
 *      0 1                                                           n
 *     +-+-------------------------------------------------------------+
 *   0 |1|                            Value                            |
 *     +-+-------------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<WORD_TYPE>, <immediateWordTypes>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_SMALLINT_* Accessors
 *
 *	Small integer word field accessor macros.
 *
 *  WORD_SMALLINT_GET	- Get value of small integer word.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Small Integer Word>, <WORD_SMALLINT_NEW>
 *---------------------------------------------------------------------------*/

#define WORD_SMALLINT_GET(word)	(((intptr_t)(word))>>1)

/*---------------------------------------------------------------------------
 * Internal Constants: SMALLINT_* Constants
 *
 *	Small integer limit constants.
 *
 *  SMALLINT_MIN	- Minimum value of small integer words.
 *  SMALLINT_MAX	- Maximum value of small integer words.
 *
 * See also:
 *	<Small Integer Word>
 *---------------------------------------------------------------------------*/

#define SMALLINT_MIN		(INTPTR_MIN>>1)
#define SMALLINT_MAX		(INTPTR_MAX>>1)

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
 *
 * See also:
 *	<Small Integer Word>, <WORD_TYPE_SMALLINT>
 *---------------------------------------------------------------------------*/

#define WORD_SMALLINT_NEW(value) \
    ((Col_Word)((((intptr_t)(value))<<1)|1))


/*
================================================================================
Internal Section: Small Floating Points
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Small Floating Point Word
 *
 *	Small floating point words are floating points whose lower 2 bits of the
 *	mantissa are zero, so that they are free for the tag bits. Other values
 *	are cell-based.
 *
 * Requirements:
 *	Small floating point words need to store the floating point value in the
 *	word identifier.
 *
 * Fields:
 *	Value	- Floating point value of word.
 *
 * Layout:
 *	On all architectures the word layout is as follows:
 *
 * (start table)
 *      01 2                                                           n
 *     +--+------------------------------------------------------------+
 *   0 |01|                           Value                            |
 *     +--+------------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<WORD_TYPE>, <immediateWordTypes>
 *---------------------------------------------------------------------------*/

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
 *	Utility union type for immediate floating point words. Because of 
 *	language restrictions (bitwise operations on floating points are 
 *	forbidden), we have to use this intermediary union type for conversions.
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
 * Internal Macros: WORD_SMALLFP_* Accessors
 *
 *	Small floating point word field accessor macros.
 *
 *  WORD_SMALLFP_GET	- Get value of small floating point word.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Small Floating Point Word>, <WORD_SMALLFP_NEW>
 *---------------------------------------------------------------------------*/

#define WORD_SMALLFP_GET(word, c)	((c).w = (word), (c).i &= ~2, (c).f)

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
 *
 * See also:
 *	<Small Floating Point Word>, <WORD_TYPE_SMALLFP>
 *---------------------------------------------------------------------------*/

#define WORD_SMALLFP_NEW(value, c) \
    ((c).f = (SMALLFP_TYPE)(value), (c).i |= 2, (c).w)


/*
================================================================================
Internal Section: Characters
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Character Word
 *
 *	Character words are immediate words storing one Unicode character 
 *	codepoint.
 *
 * Requirements:
 *	Character words need to store Unicode character codepoints up to
 *	<COL_CHAR_MAX>, i.e. at least 21 bits.
 *
 *	Character words also need to know the character format for string
 *	normalization issues. For that the codepoint width is sufficient
 *	(between 1 and 4, i.e. 3 bits).
 *
 * Fields:
 *	Codepoint   - Unicode codepoint of character word.
 *	Width	    - Character width set at creation time.
 *
 * Layout:
 *	On all architectures the word layout is as follows:
 *
 * (start table)
 *      0   4 5	  7 8                31                               n
 *     +-----+-----+-------------------+-------------------------------+
 *   0 |00100|Width|     Codepoint     |        Unused (n > 32)        |
 *     +-----+-----+-------------------+-------------------------------+
 * (end)
 *
 * See also:
 *	<WORD_TYPE>, <immediateWordTypes>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_CHAR_* Accessors
 *
 *	Small integer word field accessor macros.
 *
 *  WORD_CHAR_GET	- Get Unicode codepoint of char word.
 *  WORD_CHAR_WIDTH	- Get char width used at creation time.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Character Word>, <WORD_CHAR_NEW>
 *---------------------------------------------------------------------------*/

#define WORD_CHAR_GET(word)	((Col_Char)(((uintptr_t)(word))>>8))
#define WORD_CHAR_WIDTH(word)	((((uintptr_t)(word))>>5)&7)

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
 *	The new character word.
 *
 * See also:
 *	<Character Word>, <WORD_TYPE_CHAR>
 *---------------------------------------------------------------------------*/

#define WORD_CHAR_NEW(value, width) \
    ((Col_Word)((((uintptr_t)(value))<<8)|(((uintptr_t)(width))<<5)|4))


/*
================================================================================
Internal Section: Small Strings
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Small String Word
 *
 *	Small integer words are immediate words storing short 8-bit strings. 
 *	Maximum capacity is the machine word size minus one (i.e. 3 on 32-bit, 
 *	7 on 64-bit). Larger strings are cell-based.
 *
 * Requirements:
 *	Small integer words need to store the characters and the length.
 *
 * Fields:
 *	Length	- Number of characters.
 *	Data	- Character data.
 *
 * Layout:
 *	On all architectures the word layout is as follows:
 *
 * (start table)
 *      0   4 5            7 8                                        n
 *     +-----+--------------+------------------------------------------+
 *   0 |00110| 0<=Length<=7 |                   Data                   |
 *     +-----+--------------+------------------------------------------+
 * (end)
 *
 * See also:
 *	<WORD_TYPE>, <immediateWordTypes>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_SMALLSTR_* Accessors
 *
 *	Small string word field accessor macros.
 *
 *  WORD_SMALLSTR_LENGTH	- Get small string length.
 *  WORD_SMALLSTR_SET_LENGTH	- Set small string length.
 *  WORD_SMALLSTR_DATA		- Small string data.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Small String Word>
 *---------------------------------------------------------------------------*/

#define WORD_SMALLSTR_LENGTH(value)	((((uintptr_t)(value))&0xFF)>>5)
#define WORD_SMALLSTR_SET_LENGTH(word, length) (*((uintptr_t *)&(word)) = ((length)<<5)|12)
#ifdef COL_BIGENDIAN
#   define WORD_SMALLSTR_DATA(word)	((Col_Char1  *)&(word))
#else
#   define WORD_SMALLSTR_DATA(word)	(((Col_Char1 *)&(word))+1)
#endif

/*---------------------------------------------------------------------------
 * Internal Constant: SMALLSTR_MAX_LENGTH
 *
 *	Maximum length of small string words.
 *
 * See also:
 *	<Small String Word>
 *---------------------------------------------------------------------------*/

#define SMALLSTR_MAX_LENGTH	(sizeof(Col_Word)-1)

/*---------------------------------------------------------------------------
 * Internal Constant: WORD_SMALLSTR_EMPTY
 *
 *	Empty string immediate singleton.
 *---------------------------------------------------------------------------*/

#define WORD_SMALLSTR_EMPTY	((Col_Word) 12)


/*
================================================================================
Internal Section: Circular Lists
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Circular List Word
 *
 *	Circular lists words are list words made of a core list that repeats 
 *	infinitely. It can be either a regular list or an immediate void list.
 *
 * Requirements:
 *	Circular list words need to store their core list word.
 *
 * Fields:
 *	Core	- Core list word.
 *
 * Layout:
 *	On all architectures the word layout is as follows:
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
 * See also:
 *	<WORD_TYPE>, <immediateWordTypes>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_CIRCLIST_* Accessors
 *
 *	Circular list word field accessor macros.
 *
 *  WORD_CIRCLIST_CORE	- Get core list.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Circular List Word>, <WORD_CIRCLIST_NEW>
 *---------------------------------------------------------------------------*/

#define WORD_CIRCLIST_CORE(word)	((Col_Word)(((uintptr_t)(word))&~8))

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
 *
 * See also:
 *	<Circular List Word>, <WORD_TYPE_CIRCLIST>
 *---------------------------------------------------------------------------*/

#define WORD_CIRCLIST_NEW(core) \
    ((Col_Word)(((uintptr_t)(core))|8))


/*
================================================================================
Internal Section: Void Lists
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Void List Word
 *
 *	Void lists are immediate words representing lists whose elements are all
 *	nil. 
 *
 * Requirements:
 *	Void list words need to know their length. Length width is the machine
 *	word width minus 4 bits, so the maximum length is about 1/16th of the 
 *	theoretical maximum. Larger void lists are built by concatenating
 *	several shorter immediate void lists.
 *
 *	Void list tag bit comes after the circular list tag so that void lists
 *	can be made circular. Void circular lists thus combine both tag bits.
 *
 * Fields:
 *	Length	- List length.
 *
 * Layout:
 *	On all architectures the word layout is as follows:
 *
 * (start table)
 *      0   4 5                                                       n
 *     +-----+---------------------------------------------------------+
 *   0 |00101|                         Length                          |
 *     +-----+---------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<WORD_TYPE>, <immediateWordTypes>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_VOIDLIST_* Accessors
 *
 *	Void list word field accessor macros.
 *
 *  WORD_VOIDLIST_LENGTH	- Get void list length.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Void List Word>
 *---------------------------------------------------------------------------*/

#define WORD_VOIDLIST_LENGTH(word)	(((size_t)(intptr_t)(word))>>5)

/*---------------------------------------------------------------------------
 * Internal Constant: VOIDLIST_MAX_LENGTH
 *
 *	Maximum length of void list words.
 *
 * See also:
 *	<Void List Word>
 *---------------------------------------------------------------------------*/

#define VOIDLIST_MAX_LENGTH	(SIZE_MAX>>5)

/*---------------------------------------------------------------------------
 * Internal Constant: WORD_LIST_EMPTY
 *
 *	Empty list immediate singleton. This is simply a void list of zero
 *	length.
 *---------------------------------------------------------------------------*/

#define WORD_LIST_EMPTY		WORD_VOIDLIST_NEW(0)

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
 *
 * See also:
 *	<Void List Word>, <WORD_TYPE_VOIDLIST>
 *---------------------------------------------------------------------------*/

#define WORD_VOIDLIST_NEW(length) \
    ((Col_Word)(intptr_t)((((size_t)(length))<<5)|20))


/*
================================================================================
Internal Section: Regular Words
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Word
 *
 *	Regular words are datatypes that store their structure in cells, and
 *	are identified by their cell address.
 *
 *	Regular words store their data in cells whose 1st machine word is used 
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
 * Requirements:
 *	Regular words must store their type info and a pinned flag.
 *
 * Fields:
 *	Pinned	- Pinned flag.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
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
 * See also:
 *	<WORD_TYPEINFO>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Data Structure: Predefined Word
 *
 *	Predefined words are datatypes that are identified by a fixed ID and
 *	managed by hardwired internal code.
 *
 *	Predefined type IDs for regular word types are chosen so that their bit
 *	0 is cleared and bit 1 is set, to distinguish them with type pointers 
 *	and avoid value clashes.
 *	
 *	On all architectures the single-cell layout is as follows:
 *
 * Requirements:
 *	Regular words must store their type ID.
 *
 * Fields:
 *	Type	- Type identifier.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
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
 * See also:
 *	<WORD_TYPEINFO>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: Word Pinned Flag Accessors
 *
 *	Accessors for cell-based word pinned flag.
 *
 *  WORD_PINNED		- Get pinned flag.
 *  WORD_SET_PINNED	- Set pinned flag.
 *  WORD_CLEAR_PINNED	- Clear pinned flag.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Word>
 *---------------------------------------------------------------------------*/

#define WORD_PINNED(word)	(((uint8_t *)(word))[0] & 1)
#define WORD_SET_PINNED(word)	(((uint8_t *)(word))[0] |= 1)
#define WORD_CLEAR_PINNED(word)	(((uint8_t *)(word))[0] &= ~1)

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_* Accessors
 *
 *	Cell-based word field accessor macros.
 *
 *  WORD_TYPEID		- Get type ID for predefined word type.
 *  WORD_SET_TYPEID	- Set type ID for predefined word type. Also clears the
 *			  pinned flag.
 *  WORD_TYPEINFO	- Get <Col_CustomWordType> custom type descriptor. 
 *			  (Caution: word is evaluated several times during macro
 *			  expansion)
 *  WORD_SET_TYPEINFO	- Set <Col_CustomWordType> custom type descriptor. Also 
 *			  clears the pinned flag. (Caution: word is evaluated 
 *			  several times during macro expansion)
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Word>, <Predefined Word>, <Custom Word>
 *---------------------------------------------------------------------------*/

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


/*
================================================================================
Internal Section: Custom Words
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Custom Word
 *
 *	Custom words are datatypes that are described by a <Col_CustomWordType>
 *	type descriptor structure defined and managed by client code.
 *
 *	As such type descriptor structures are machine word-aligned, this means
*	that the four lower bits of their address are cleared.
 *	
 * Requirements:
 *	Custom words must store a pointer to their <Col_CustomWordType>.
 *
 *	Custom words can be part of a synonym chain.
 *
 *	Some custom word types define cleanup procedures (freeProc), such words
 *	need to be traversed during the sweep phase of the GC process. To do so,
 *	they are singly-linked together and so need to store the next word in 
 *	chain.
 *
 * Fields:
 *	Type	- Type descriptor.
 *	Synonym	- Generic <Word> Synonym field.
 *	Next	- Next word in custom cleanup chain (optional).
 *	Data	- Custom type payload.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0 1                                                           n
 *     +-+-+-----------------------------------------------------------+
 *   0 |P|0|                         Type                              |
 *     +-+-+-----------------------------------------------------------+
 *   1 |                            Synonym                            |
 *     +---------------------------------------------------------------+
 *   2 |                        Next (optional)                        |
 *     +---------------------------------------------------------------+
 *     .                                                               .
 *     .                      Type-specific data                       .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<WORD_TYPEINFO>, <Col_CustomWordType>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_CUSTOM_* Accessors
 *
 *	Custom word field accessor macros.
 *
 *  WORD_CUSTOM_NEXT    - Next word in custom cleanup chain.
 *  WORD_CUSTOM_DATA    - Beginning of custom word data.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_CUSTOM_INIT>, <WORD_CUSTOM_HEADER_SIZE>, <SweepUnreachableCells>,
 *	<Col_CustomWordType>
 *---------------------------------------------------------------------------*/

#define WORD_CUSTOM_NEXT(word, typeInfo) (*(Col_Word *)(((char *)(word))+WORD_CUSTOM_HEADER_SIZE(typeInfo)))
#define WORD_CUSTOM_DATA(word, typeInfo) ((void *)(&WORD_CUSTOM_NEXT((word), (typeInfo))+((typeInfo)->freeProc?1:0)))

/*---------------------------------------------------------------------------
 * Internal Constant: CUSTOM_HEADER_SIZE
 *
 *	Byte size of custom word header.
 *
 * See also:
 *	<Custom Word>
 *---------------------------------------------------------------------------*/

#define CUSTOM_HEADER_SIZE		(sizeof(Col_CustomWordType *)+sizeof(Col_Word *))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_CUSTOM_HEADER_SIZE
 *
 *	Get number of cells taken by the custom word header.
 *
 * Arguments:
 *	typeInfo	- <WORD_TYPEINFO>
 *
 * Result:
 *	Number of cells taken by word header.
 *
 * See also:
 *	<Custom Word>, <CUSTOM_HEADER_SIZE>, <CUSTOMHASHMAP_HEADER_SIZE>
 *---------------------------------------------------------------------------*/

#define WORD_CUSTOM_HEADER_SIZE(typeInfo) \
    ((typeInfo)->type&COL_HASHMAP?CUSTOMHASHMAP_HEADER_SIZE:CUSTOM_HEADER_SIZE)

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_CUSTOM_SIZE
 *
 *	Get number of cells taken by the custom word.
 *
 * Arguments:
 *	typeInfo	- <WORD_TYPEINFO>
 *	size		- Byte size of custom word data.
 *
 * Result:
 *	Number of cells taken by word.
 *
 * See also:
 *	<Custom Word>
 *---------------------------------------------------------------------------*/

#define WORD_CUSTOM_SIZE(typeInfo, size) \
    (NB_CELLS(WORD_CUSTOM_HEADER_SIZE(typeInfo)+((typeInfo)->freeProc?sizeof(Cell *):0)+(size)))

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
 *	<Custom Word>, <Col_CustomWordType>, <Col_NewCustomWord>
 *---------------------------------------------------------------------------*/

#define WORD_CUSTOM_INIT(word, typeInfo) \
    WORD_SET_TYPEINFO((word), (typeInfo)); \
    WORD_SYNONYM(word) = WORD_NIL;








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
 * Fields:
 *	TODO
 *
 * Generic layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  | Flags | Wrapped type  |        Unused (n > 32)        |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Synonym                            |
 *     +---------------------------------------------------------------+
 *   2 |                                                               |
 *     +                             Data                              +
 *   3 |                                                               |
 *     +---------------------------------------------------------------+
 * (end)
 *
 * Wrapped words layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8    15 16           31                               n
 *     +-------+-------+---------------+-------------------------------+
 *   0 | Type  | Flags | Wrapped type  |        Unused (n > 32)        |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Synonym                            |
 *     +---------------------------------------------------------------+
 *   2 |                            Source                             |
 *     +---------------------------------------------------------------+
 *   3 |                            Unused                             |
 *     +---------------------------------------------------------------+
 * (end)
 *
 * Wrapped integers layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  | Flags | Wrapped type  |        Unused (n > 32)        |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Synonym                            |
 *     +---------------------------------------------------------------+
 *   2 |                             Value                             |
 *     +---------------------------------------------------------------+
 *   3 |                            Unused                             |
 *     +---------------------------------------------------------------+
 * (end)
 *
 * Wrapped floating points layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  | Flags | Wrapped type  |        Unused (n > 32)        |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Synonym                            |
 *     +---------------------------------------------------------------+
 *   2 |                             Value                             |
 *     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 *   3 |                        Unused (n > 32)                        |
 *     +---------------------------------------------------------------+
 * (end)
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<WORD_WRAP_INIT>, <Col_WordTypeId>
 *---------------------------------------------------------------------------*/

#define WORD_WRAP_FLAGS(word)	(((uint8_t *)(word))[1])
#define WORD_WRAP_TYPE(word)	(((uint16_t *)(word))[1])
#define WORD_SYNONYM(word)	(((Col_Word *)(word))[1])
#define WORD_WRAP_SOURCE(word)	(((Col_Word *)(word))[2])
#define WORD_INTWRAP_VALUE(word) (((intptr_t *)(word))[2])
#define WORD_FPWRAP_VALUE(word)	(*(double *)(((intptr_t *)(word))+2))

#define WRAP_FLAG_MUTABLE	1
#define WRAP_FLAG_VARIANT	2

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

#define WORD_WRAP_INIT(word, flags, type, source) \
    WORD_SET_TYPEID((word), WORD_TYPE_WRAP); \
    WORD_WRAP_FLAGS(word) = (flags); \
    WORD_WRAP_TYPE(word) = (type); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_WRAP_SOURCE(word) = (source);


#define WORD_INTWRAP_INIT(word, flags, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_WRAP); \
    WORD_WRAP_FLAGS(word) = (flags); \
    WORD_WRAP_TYPE(word) = COL_INT; \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_INTWRAP_VALUE(word) = (value);

#define WORD_FPWRAP_INIT(word, flags, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_WRAP); \
    WORD_WRAP_FLAGS(word) = (flags); \
    WORD_WRAP_TYPE(word) = COL_FLOAT; \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_FPWRAP_VALUE(word) = (value);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_UNWRAP
 *
 *	If word is wrapped, get its source. Else do nothing. Never use on
 *	integer or floating point wrappers.
 *
 * Argument:
 *	word	- Word to unwrap.
 *
 * Side effects:
 *	If *word* is wrapped, replaced by its source.
 *
 * See also:
 *	<WORD_TYPE_WRAP>
 *---------------------------------------------------------------------------*/

#define WORD_UNWRAP(word) \
    if (WORD_TYPE(word) == WORD_TYPE_WRAP) (word) = WORD_WRAP_SOURCE(word);

#endif /* _COLIBRI_WORD_INT */
