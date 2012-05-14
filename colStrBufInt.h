/*
 * Internal Header: colStrBufInt.h
 *
 *	This header file defines the string buffer word internals of Colibri.
 *
 *	String buffers are used to build strings incrementally in an efficient
 *	manner, by appending individual characters or whole ropes. The current
 *	accumulated rope can be retrieved at any time.
 *
 * See Also:
 *	<colStrBuf.c>, <colStrBuf.h>, <WORD_TYPE_STRBUF>
 */

#ifndef _COLIBRI_STRBUF_INT
#define _COLIBRI_STRBUF_INT


/*
================================================================================
Internal Section: String Buffers
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: String Buffer Word
 *
 *	String buffers are used to build strings incrementally in an efficient
 *	manner, by appending individual characters or whole ropes. The current
 *	accumulated rope can be retrieved at any time.
 *
 * Requirements:
 *	String buffer words can use different character formats for internal
 *	storage. So they need to store this information.
 *
 *	Characters can be appended individually or in bulks (ropes or sequences
 *	as iterator ranges). When adding single or multiple characters, they are
 *	appended to an internal buffer stored within the word, up to its maximum
 *	capacity. When the buffer is full, a rope is appended, or the 
 *	accumulated value is retrieved, the buffer content is appended to the 
 *	accumulated rope so far and the buffer is cleared before performing the 
 *	operation.
 *
 *	String buffers have a buffer length set at creation time. This plus the
 *	format determines the byte size of the buffer and thus number of cells 
 *	the word takes. So we store this cell size and deduce the actual buffer 
 *	length from the word size in cells minus the header, divided by the
 *	character width of the used format.
 *
 * Fields:
 *	Format	- Preferred format for string building.
 *	Size	- Number of allocated cells.
 *	Rope	- Cumulated rope so far.
 *	Length	- Current length of character buffer.
 *	Buffer	- Character buffer data (array of characters).
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8     15 16          31                               n
 *     +-------+--------+--------------+-------------------------------+
 *   0 | Type  | Format |     Size     |        Unused (n > 32)        |
 *     +-------+--------+--------------+-------------------------------+
 *   1 |                              Rope                             |
 *     +---------------------------------------------------------------+
 *   2 |                             Length                            |
 *     +---------------------------------------------------------------+
 *     .                                                               .
 *     .                             Buffer                            .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_STRBUF_* Accessors
 *
 *	String buffer field accessor macros.
 *
 *  WORD_STRBUF_FORMAT	- Preferred format for string building.
 *  WORD_STRBUF_SIZE	- Number of allocated cells.
 *  WORD_STRBUF_ROPE	- Cumulated rope so far.
 *  WORD_STRBUF_LENGTH	- Current length of character buffer.
 *  WORD_STRBUF_BUFFER	- Character buffer data (array of characters).
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<String Buffer Word>, <WORD_STRBUF_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_STRBUF_FORMAT(word)	(((int8_t *)(word))[1])
#define WORD_STRBUF_SIZE(word)		(((uint16_t *)(word))[1])
#define WORD_STRBUF_ROPE(word)		(((Col_Word *)(word))[1])
#define WORD_STRBUF_LENGTH(word)	(((size_t *)(word))[2])
#define WORD_STRBUF_BUFFER(word)	((const char *)(word)+STRBUF_HEADER_SIZE)

/*---------------------------------------------------------------------------
 * Internal Constants: WORD_STRBUF_* Constants
 *
 *	String buffer size limit constants.
 *
 *  STRBUF_HEADER_SIZE		- Byte size of string buffer header.
 *  STRBUF_MAX_SIZE		- Maximum cell size taken by string buffers.
 *
 * See also:
 *	<String Buffer Word>
 *---------------------------------------------------------------------------*/

#define STRBUF_HEADER_SIZE	(sizeof(size_t)*3)
#define STRBUF_MAX_SIZE		UINT16_MAX

/*---------------------------------------------------------------------------
 * Internal Macro: STRBUF_SIZE
 *
 *	Get number of cells for a string buffer of a given length.
 *
 * Argument:
 *	byteLength	- Buffer length in bytes.
 *
 * Result:
 *	Number of cells taken by word.
 *
 * See also:
 *	<String Buffer Word>
 *---------------------------------------------------------------------------*/

#define STRBUF_SIZE(byteLength) \
    (NB_CELLS(STRBUF_HEADER_SIZE+(byteLength)))

/*---------------------------------------------------------------------------
 * Internal Macro: STRBUF_MAX_LENGTH
 *
 *	Get maximum string buffer length for a given byte size.
 *
 * Argument:
 *	byteSize	- Available size.
 *
 * Result:
 *	String buffer length fitting the given size.
 *
 * See also:
 *	<String Buffer Word>
 *---------------------------------------------------------------------------*/

#define STRBUF_MAX_LENGTH(byteSize, format) \
    (((byteSize)-STRBUF_HEADER_SIZE)/CHAR_WIDTH(format))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_STRBUF_INIT
 *
 *	String buffer word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	size	- <WORD_STRBUF_SIZE>
 *	format	- <WORD_STRBUF_FORMAT>
 *
 * See also:
 *	<String Buffer Word>, <WORD_TYPE_STRBUF>, <Col_NewStringBuffer>
 *---------------------------------------------------------------------------*/

#define WORD_STRBUF_INIT(word, size, format) \
    WORD_SET_TYPEID((word), WORD_TYPE_STRBUF); \
    WORD_STRBUF_FORMAT(word) = (format); \
    WORD_STRBUF_SIZE(word) = (size); \
    WORD_STRBUF_ROPE(word) = WORD_SMALLSTR_EMPTY; \
    WORD_STRBUF_LENGTH(word) = 0;


/*
================================================================================
Internal Section: Type Checking
================================================================================
*/

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_STRBUF
 *
 *	Type checking macro for string buffers.
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not a string buffer
 *
 * See also:
 *	<Col_Error>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_STRBUF(word) \
    if (!(Col_WordType(word) & COL_STRBUF)) { \
	Col_Error(COL_TYPECHECK, "%x is not a string buffer", (word)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

#endif /* _COLIBRI_STRBUF_INT */
