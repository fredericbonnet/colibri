/*
 * File: colStrBuf.c
 *
 *	This file implements the string buffer handling features of Colibri.
 *
 *	String buffers are used to build strings incrementally in an efficient
 *	manner, by appending individual characters or whole ropes. The current
 *	accumulated rope can be retrieved at any time.
 *
 * See also: 
 *	<colStrBuf.h>
 */

#include "include/colibri.h"
#include "colInternal.h"

#include "colWordInt.h"
#include "colRopeInt.h"
#include "colStrBufInt.h"

#include <string.h>

/*
 * Prototypes for functions used only in this file.
 */

static void		CommitBuffer(Col_Word strbuf);


/*
================================================================================
Section: String Buffers
================================================================================
*/

/****************************************************************************
 * Group: String Buffer Creation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Constant: STRBUF_DEFAULT_SIZE
 *
 *	Default size for string buffers.
 *
 * See also:
 *	<Col_NewStringBuffer>
 *---------------------------------------------------------------------------*/

#define STRBUF_DEFAULT_SIZE	5

/*---------------------------------------------------------------------------
 * Function: Col_GetMaxStringBufferLength
 *
 *	Get the maximum length of a string buffer.
 *
 * Argument:
 *	format	- Format policy (see <Col_StringBufferFormat>).
 *
 * Result:
 *	The max string buffer length.
 *---------------------------------------------------------------------------*/

size_t
Col_GetMaxStringBufferLength(Col_StringFormat format)
{
    return STRBUF_MAX_LENGTH(STRBUF_MAX_SIZE * CELL_SIZE, format);
}

/*---------------------------------------------------------------------------
 * Function: Col_NewStringBuffer
 *
 *	Create a new string buffer word.
 *
 *	Note that the actual maximum length will be rounded up to fit an even
 *	number of cells.
 *
 * Arguments:
 *	maxLength	- Maximum length of string buffer. If zero, use a 
 *			  default value.
 *	format		- String format.
 *
 * Result:
 *	The new word.
 *
 * Side effects:
 *	Allocates memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewStringBuffer(
    size_t maxLength,
    Col_StringFormat format)
{
    Col_Word strbuf;		/* Resulting word in the general case. */
    size_t size;		/* Number of allocated cells storing a minimum
				 * of maxLength elements. */

    /*
     * Create a new string buffer word.
     */

    if (maxLength == 0) {
	size = STRBUF_DEFAULT_SIZE;
    } else {
	size = STRBUF_SIZE(maxLength * CHAR_WIDTH(format));
	if (size > STRBUF_MAX_SIZE) size = STRBUF_MAX_SIZE;
    }
    strbuf = (Col_Word) AllocCells(size);
    WORD_STRBUF_INIT(strbuf, size, format);

    return strbuf;
}


/****************************************************************************
 * Group: String Buffer Access
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_StringBufferLength
 *
 *	Get the current length of the string buffer.
 *
 * Argument:
 *	strbuf	- String buffer to get length for.
 *
 * Result:
 *	The string buffer length.
 *---------------------------------------------------------------------------*/

size_t
Col_StringBufferLength(
    Col_Word strbuf)
{
    if (WORD_TYPE(strbuf) != WORD_TYPE_STRBUF) {
	Col_Error(COL_ERROR, "%x is not a string buffer", strbuf);
	return 0;
    }

    /*
     * String buffer length is the cumulated string length plus the current 
     * length of the character buffer.
     */

    return Col_RopeLength(WORD_STRBUF_ROPE(strbuf))
	    + WORD_STRBUF_LENGTH(strbuf);
}

/*---------------------------------------------------------------------------
 * Function: Col_StringBufferValue
 *
 *	Get the current cumulated string.
 *
 * Argument:
 *	strbuf	- String buffer to get string for.
 *
 * Result:
 *	The string buffer string.
 *
 * Side effects:
 *	Buffer is committed into accumulated rope.
 *---------------------------------------------------------------------------*/

Col_Word
Col_StringBufferValue(
    Col_Word strbuf)
{
    CommitBuffer(strbuf);
    return WORD_STRBUF_ROPE(strbuf);
}


/****************************************************************************
 * Group: String Buffer Operations
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Function: CommitBuffer
 *
 *	Commit buffered characters into accumumulated rope and clear buffer.
 *
 * Argument:
 *	strbuf	- String buffer to commit.
 *
 * Result:
 *	A rope that fullfills the format requirement.
 *
 * Side effects:
 *	Data buffer may be cleared, accumulated rope may be modified, memory
 *	cells can be allocated.
 *
 * See also:
 *	<Col_StringBufferFormat>, <Col_StringBufferAppendRope>
 *---------------------------------------------------------------------------*/

static void
CommitBuffer(
    Col_Word strbuf)
{
    Col_Word rope;
    size_t length = WORD_STRBUF_LENGTH(strbuf);
    Col_StringFormat format = (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);

    if (length == 0) return;

    /*
     * Create rope from buffer and append to accumulated rope.
     */

   rope = Col_NewRope(format, WORD_STRBUF_BUFFER(strbuf), 
	    length * CHAR_WIDTH(format));
    WORD_STRBUF_ROPE(strbuf) = Col_ConcatRopes(WORD_STRBUF_ROPE(strbuf), rope);
    WORD_STRBUF_LENGTH(strbuf) = 0;
}

/*---------------------------------------------------------------------------
 * Function: Col_StringBufferAppendChar
 *
 *	Append the given character to the string buffer.
 *
 * Arguments:
 *	strbuf	- String buffer to append character to.
 *	c	- Character to append.
 *
 * Result:
 *	Whether the character was appended or not, the latter may occur when
 *	the character codepoint is unrepresentable in the target format.
 *
 * Side effects:
 *	String buffer value may change.
 *
 * See also:
 *	<Col_StringBufferAppendRope>, <Col_StringBufferAppendSequence>
 *---------------------------------------------------------------------------*/

int
Col_StringBufferAppendChar(
    Col_Word strbuf,
    Col_Char c)
{
    int format = WORD_STRBUF_FORMAT(strbuf), width;
    size_t maxLength = STRBUF_MAX_LENGTH(WORD_STRBUF_SIZE(strbuf) * CELL_SIZE,
	    format);

    /*
     * Get character width.
     */

    switch (format) {
	case COL_UCS1:
	    width = (c <= COL_CHAR1_MAX) ? 1 : 0;
	    break;

	case COL_UCS2:
	    width = (c <= COL_CHAR2_MAX) ? 1 : 0;
	    break;

	case COL_UTF8:
	    width = COL_UTF8_WIDTH(c);
	    break;

	case COL_UTF16:
	    width = COL_UTF16_WIDTH(c);
	    break;

	default:
	    width = 1;
    }
    if (width == 0) {
	/* 
	 * Lost character.
	 */

	return 0;
    }

    if (WORD_STRBUF_LENGTH(strbuf) + width > maxLength) {
	/*
	 * Buffer is full, commit first.
	 */

	CommitBuffer(strbuf);
	ASSERT(WORD_STRBUF_LENGTH(strbuf) == 0);
    }

    /*
     * Append character into buffer.
     */

    switch (format) {
	case COL_UCS1:
	    ((Col_Char1 *) WORD_STRBUF_BUFFER(strbuf))[
		    WORD_STRBUF_LENGTH(strbuf)] = c;
	    break;

	case COL_UCS2:
	    ((Col_Char2 *) WORD_STRBUF_BUFFER(strbuf))[
		    WORD_STRBUF_LENGTH(strbuf)] = c;
	    break;

	case COL_UTF8:
	    Col_Utf8SetChar(((Col_Char1 *) WORD_STRBUF_BUFFER(strbuf))
		    + width, c);
	    break;

	case COL_UTF16:
	    Col_Utf16SetChar(((Col_Char2 *) WORD_STRBUF_BUFFER(strbuf))
		    + width, c);
	    break;

	default:
	    ((Col_Char4 *) WORD_STRBUF_BUFFER(strbuf))[
		    WORD_STRBUF_LENGTH(strbuf)] = c;
    }
    WORD_STRBUF_LENGTH(strbuf) += width;
    return 1;
}

/*---------------------------------------------------------------------------
 * Function: Col_StringBufferAppendRope
 *
 *	Append the given rope to the string buffer.
 *
 * Arguments:
 *	strbuf	- String buffer to append character to.
 *	rope	- Rope to append.
 *
 * Result:
 *	Whether all characters were appended or not, the latter may occur when
 *	some character codepoints are unrepresentable in the target format.
 *
 * Side effects:
 *	String buffer value may change.
 *
 * See also:
 *	<Col_StringBufferAppendChar>, <Col_StringBufferAppendSequence>
 *---------------------------------------------------------------------------*/

int
Col_StringBufferAppendRope(
    Col_Word strbuf,
    Col_Word rope)
{
    Col_Word rope2;

    if (Col_RopeLength(rope) == 0) {
	/*
	 * Nothing to do.
	 */

	return 1;
    }

    if (WORD_STRBUF_LENGTH(strbuf) > 0) {
	/*
	 * Commit buffer first.
	 */

	CommitBuffer(strbuf);
	ASSERT(WORD_STRBUF_LENGTH(strbuf) == 0);
    }

    /*
     * Normalize & append rope.
     */

    rope2 = Col_NormalizeRope(rope, 
	    (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf), COL_CHAR_INVALID, 0);
    WORD_STRBUF_ROPE(strbuf) = Col_ConcatRopes(WORD_STRBUF_ROPE(strbuf), rope2);
    return (Col_RopeLength(rope) == Col_RopeLength(rope2));
}

/*---------------------------------------------------------------------------
 * Function: Col_StringBufferReset
 *
 *	Empty the string buffer.
 *
 * Argument:
 *	strbuf	- String buffer to reset.
 *
 * Side effects:
 *	Reset buffer length & accumulated rope.
 *---------------------------------------------------------------------------*/

void
Col_StringBufferReset(
    Col_Word strbuf)
{
    WORD_STRBUF_ROPE(strbuf) = WORD_SMALLSTR_EMPTY;
    WORD_STRBUF_LENGTH(strbuf) = 0;
}

/*---------------------------------------------------------------------------
 * Function: Col_StringBufferFreeze
 *
 *	Freeze the string buffer word, turning it into a rope.
 *
 * Argument:
 *	strbuf	- String buffer to freeze.
 *
 * Result:
 *	The accumulated rope so far.
 *
 * Side effects:
 *	Word can no longer be used as a string buffer.
 *---------------------------------------------------------------------------*/

Col_Word
Col_StringBufferFreeze(
    Col_Word strbuf)
{
    Col_StringFormat format = (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);
    size_t length = WORD_STRBUF_LENGTH(strbuf);
    int pinned = WORD_PINNED(strbuf);

    if (length == 0 || WORD_STRBUF_ROPE(strbuf) != WORD_SMALLSTR_EMPTY) {
	/*
	 * Buffer is empty or accumulated rope is not, return value.
	 */

	return Col_StringBufferValue(strbuf);
    }

    /*
     * Turn word into rope.
     */

    switch (format) {
	case COL_UCS1:
	case COL_UCS2:
	case COL_UCS4:
	    if (length > (format == COL_UCS1 ? SMALLSTR_MAX_LENGTH : 1) 
		    && length <= UCSSTR_MAX_LENGTH) {
		/*
		 * Convert word inplace.
		 */

		ASSERT(WORD_UCSSTR_HEADER_SIZE <= WORD_STRBUF_HEADER_SIZE);
		WORD_UCSSTR_INIT(strbuf, format, length);
		if (pinned) {
		    WORD_SET_PINNED(strbuf);
		}
		memcpy((void *) WORD_UCSSTR_DATA(strbuf), 
			WORD_STRBUF_BUFFER(strbuf), 
			length * CHAR_WIDTH(format));
		return strbuf;
	    }
	    break;

	case COL_UCS:
	    if (length <= UCSSTR_MAX_LENGTH) {
		/*
		 * Compute format.
		 */

		Col_Char4 *data = (Col_Char4 *) WORD_STRBUF_BUFFER(strbuf);
		size_t i;
		format = COL_UCS1;
		for (i=0; i < length; i++) {
		    if (data[i] > COL_CHAR2_MAX) {
			format = COL_UCS4;
			break;
		    } else if (data[i] > COL_CHAR1_MAX) {
			format = COL_UCS2;
		    }
		}

		if (length <= (format == COL_UCS1 ? SMALLSTR_MAX_LENGTH : 1)) {
		    /*
		     * Fits immediate string.
		     */
		    
		    break;
		}

		/*
		 * Convert word inplace.
		 */

		ASSERT(WORD_UCSSTR_HEADER_SIZE <= WORD_STRBUF_HEADER_SIZE);
		WORD_UCSSTR_INIT(strbuf, format, length);
		if (pinned) {
		    WORD_SET_PINNED(strbuf);
		}
		if (format == COL_UCS1) {
		    Col_Char1 *data1 = (Col_Char1 *) WORD_UCSSTR_DATA(strbuf);
		    for (i=0; i < length; i++) {
			*data1++ = *data++;
		    }
		} else if (format == COL_UCS2) {
		    Col_Char2 *data2 = (Col_Char2 *) WORD_UCSSTR_DATA(strbuf);
		    for (i=0; i < length; i++) {
			*data2++ = *data++;
		    }
		} else {
		    ASSERT(format == COL_UCS4);
		    memcpy((void *) WORD_UCSSTR_DATA(strbuf), 
			    WORD_STRBUF_BUFFER(strbuf), 
			    length * CHAR_WIDTH(format));
		}
		return strbuf;
	    }
	    break;

	case COL_UTF8:
	case COL_UTF16:
	    if (length * CHAR_WIDTH(format) <= UTFSTR_MAX_BYTELENGTH) {
		/*
		 * Convert word inplace.
		 */

		const Col_Char1 *begin 
			= (const Col_Char1 *) WORD_STRBUF_BUFFER(strbuf), *p;
		size_t charLength;
		for (p=begin, charLength=0; p < begin+length; charLength++) {
		    switch (format) {
			case COL_UTF8:  COL_UTF8_NEXT(p); break;
			case COL_UTF16: COL_UTF16_NEXT(p); break;
		    }
		}
		WORD_UTFSTR_INIT(strbuf, COL_UTF8, charLength, 
			length * CHAR_WIDTH(format));
		if (pinned) {
		    WORD_SET_PINNED(strbuf);
		}
		memcpy((void *) WORD_UTFSTR_DATA(strbuf), 
			WORD_STRBUF_BUFFER(strbuf), 
			length * CHAR_WIDTH(format));
		return strbuf;
	    }
	    break;
    }

    /*
     * Could not convert inplace (either too large or fits immediate word), 
     * return value.
     */

    return Col_StringBufferValue(strbuf);
}
