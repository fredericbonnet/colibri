/*
 * File: colibri.c
 *
 *	This file implements the string, initialization/cleanup, and error
 *	handling functions.
 *
 * See also:
 *	<colibri.h>
 */

#include "include/colibri.h"
#include "colInternal.h"
#include "colPlatform.h"

#include <stdio.h>
#include <stdlib.h>


/****************************************************************************
 * Section: Strings
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_Utf8Addr
 *
 *	Find the index-th char in a UTF-8 sequence. 
 *
 *	Iterate over char boundaries from the beginning or end of the string, 
 *	whichever is closest, until the char is reached.
 *
 *	Assume input is OK.
 *
 * Arguments:
 *	data		- UTF-8 code unit sequence.
 *	index		- Index of char to find.
 *	length		- Char length of sequence.
 *	bytelength	- Byte length of sequence.
 *
 * Result:
 *	Pointer to the character.
 *---------------------------------------------------------------------------*/

const Col_Char1 * 
Col_Utf8Addr(
    const Col_Char1 * data,
    size_t index,
    size_t length,
    size_t byteLength)
{
    /* 
     * Don't check bounds; assume input values are OK. 
     */

    if (byteLength == length * sizeof(*data)) {
	/*
	 * No variable-width sequence, use simple arithmetics instead
	 * of character iteration.
	 */

	return data + index;
    }
    if (index <= length/2) {
	/* 
	 * First half; search from beginning. 
	 */

	size_t i = 0;
	while (i != index) {
	    i++;		/* Increment char index. */
	    data = Col_Utf8Next(data);
	}
	return data;
    } else {
	/* 
	 * Second half; search backwards from end. 
	 */

	size_t i = length;
	data = (const Col_Char1 *) ((const char *) data + byteLength);
	while (i != index) {
	    i--;		/* Decrement char index. */
	    data = Col_Utf8Prev(data);
	}
	return data;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_Utf8Get
 *
 *	Get the first character codepoint of a UTF-8 sequence.
 *
 * Argument:
 *	data	- UTF-8 code unit sequence.
 *
 * Result:
 *	32-bit Unicode codepoint of the char.
 *---------------------------------------------------------------------------*/

Col_Char 
Col_Utf8Get(
    const Col_Char1 * data)
{
    if (*data <= 0x7F) {
	/* 
	 * Single byte, 0-7F codepoints. 
	 */

	return *data;
    } else if (*data <= 0xDF) {
	/* 
	 * 2-byte sequence, 80-7FF codepoints. 
	 */

	return   ((data[0] & 0x1F) << 6)
	       |  (data[1] & 0x3F);
    } else if (*data <= 0xEF) {
	/* 
	 * 3-byte sequence, 800-FFFF codepoints. 
	 */

	return   ((data[0] & 0x0F) << 12)
	       | ((data[1] & 0x3F) << 6)
	       |  (data[2] & 0x3F);
    } else if (*data <= 0xF7) {
	/* 
	 * 4-byte sequence, 10000-1FFFFF codepoints. 
	 */

	return   ((data[0] & 0x07) << 18)
	       | ((data[1] & 0x3F) << 12)
	       | ((data[2] & 0x3F) << 6)
	       |  (data[3] & 0x3F);
    }

    /* 
     * Invalid sequence.
     */

    return COL_CHAR_INVALID;
}

/*---------------------------------------------------------------------------
 * Function: Col_Utf8Set
 *
 *	Append character in a UTF-8 sequence.
 *
 * Arguments:
 *	data	- UTF-8 code unit sequence.
 *	c	- Character to write.
 *
 * Result:
 *	Position just past the newly added character in sequence.
 *---------------------------------------------------------------------------*/

Col_Char1 *
Col_Utf8Set(
    Col_Char1 * data,
    Col_Char c)
{
    if (c <= 0x7F) {
	*data++ = (Col_Char1) c;
    } else if (c <= 0x7FF) {
	*data++ = (Col_Char1) (((c>>6)&0x1F)|0xC0);
	*data++ = (Col_Char1) (( c    &0x3F)|0x80);
    } else if (c <= 0xD7FF || (c >= 0xE000 && c <= 0xFFFF)) {
	*data++ = (Col_Char1) (((c>>12)&0x1F)|0xE0);
	*data++ = (Col_Char1) (((c>> 6)&0x3F)|0x80);
	*data++ = (Col_Char1) (( c     &0x3F)|0x80);
    } else if (c >= 0x10000 && c <= 0x10FFFF) {
	*data++ = (Col_Char1) (((c>>18)&0x1F)|0xF0);
	*data++ = (Col_Char1) (((c>>12)&0x3F)|0x80);
	*data++ = (Col_Char1) (((c>> 6)&0x3F)|0x80);
	*data++ = (Col_Char1) (( c     &0x3F)|0x80);
    }

    return data;
}

/*---------------------------------------------------------------------------
 * Function: Col_Utf8Next
 *
 *	Get next character in a UTF-8 sequence.
 *
 * Argument:
 *	data	- UTF-8 code unit sequence.
 *
 * Result:
 *	Position just past the first character in sequence.
 *---------------------------------------------------------------------------*/

const Col_Char1 *
Col_Utf8Next(
    const Col_Char1 * data)
{
    return data + (
          (((*data) & 0xE0) == 0xC0) ?	2 \
        : (((*data) & 0xF0) == 0xE0) ?	3 \
        : (((*data) & 0xF8) == 0xF0) ?	4 \
        :				1 );
}

/*---------------------------------------------------------------------------
 * Function: Col_Utf8Prev
 *
 *	Get previous character in a UTF-8 sequence. This is done by skipping
 *      all continuation code units.
 *
 * Argument:
 *	data	- UTF-8 code unit sequence.
 *
 * Result:
 *	Position of the previous character in sequence.
 *---------------------------------------------------------------------------*/

const Col_Char1 *
Col_Utf8Prev(
    const Col_Char1 * data)
{
    while (((*--data) & 0xC0) == 0x80);
    return data;
}

/*---------------------------------------------------------------------------
 * Function: Col_Utf16Addr
 *
 *	Find the index-th char in a UTF-16 code unit sequence. 
 *
 *	Iterate over char boundaries from the beginning or end of the string, 
 *	whichever is closest, until the char is reached.
 *
 *	Assume input is OK.
 *
 * Arguments:
 *	data		- UTF-16 code unit sequence.
 *	index		- Index of char to find.
 *	length		- Char length of sequence.
 *	bytelength	- Byte length of sequence.
 *
 * Result:
 *	Pointer to the character.
 *---------------------------------------------------------------------------*/

const Col_Char2 * 
Col_Utf16Addr(
    const Col_Char2 * data,
    size_t index,
    size_t length,
    size_t byteLength)
{
    /* 
     * Don't check bounds; assume input values are OK. 
     */

    if (byteLength == length * sizeof(*data)) {
	/*
	 * No variable-width sequence, use simple arithmetics instead
	 * of character iteration.
	 */

	return data + index;
    }
    if (index <= length/2) {
	/* 
	 * First half; search from beginning. 
	 */

	size_t i = 0;
	while (i != index) {
	    i++;		/* Increment char index. */
	    data = Col_Utf16Next(data);
	}
	return data;
    } else {
	/* 
	 * Second half; search backwards from end. 
	 */

	size_t i = length;
	data = (const Col_Char2 *) ((const char *) data + byteLength);
	while (i != index) {
	    i--;		/* Decrement char index. */
	    data = Col_Utf16Prev(data);
	}
	return data;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_Utf16Get
 *
 *	Get the first character codepoint of a UTF-16 sequence.
 *
 * Argument:
 *	data	- UTF-16 code unit sequence.
 *
 * Result:
 *	Unicode codepoint of the char.
 *---------------------------------------------------------------------------*/

Col_Char 
Col_Utf16Get(
    const Col_Char2 * data)
{
    if ((data[0] & 0xFC00) == 0xD800 && (data[1] & 0xFC00) == 0xDC00) {
	/*
	 * Surrogate pair.
	 */

	return   (  ((data[0] & 0x3FF) << 10)
	          |  (data[1] & 0x3FF))
	       + 0x10000;
    } else {
	/*
	 * Single codepoint.
	 */

	return *data;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_Utf16Set
 *
 *	Append character in a UTF-16 sequence.
 *
 * Arguments:
 *	data	- UTF-16 code unit sequence.
 *	c	- Character to write.
 *
 * Result:
 *	Position just past the newly added character in sequence.
 *---------------------------------------------------------------------------*/

Col_Char2 *
Col_Utf16Set(
    Col_Char2 * data,
    Col_Char c)
{
    if (c <= 0xD7FF || (c >= 0xE000 && c <= 0xFFFF)) {
	*data++ = (Col_Char2) c;
    } else if (c >= 0x10000 && c <= 0x10FFFF) {
 	*data++ = (Col_Char2) ((((c-0x10000)>>10)&0x3FF)+0xD800);
	*data++ = (Col_Char2) (( (c-0x10000)     &0x3FF)+0xDC00);
    }

    return data;
}

/*---------------------------------------------------------------------------
 * Function: Col_Utf16Next
 *
 *	Get next character in a UTF-16 sequence.
 *
 * Argument:
 *	data	- UTF-16 code unit sequence.
 *
 * Result:
 *	Position just past the first character in sequence.
 *---------------------------------------------------------------------------*/

const Col_Char2 *
Col_Utf16Next(
    const Col_Char2 * data)
{
    return data + (
          (((*data) & 0xFC00) == 0xD800) ?  2 \
        :				    1 );
}

/*---------------------------------------------------------------------------
 * Function: Col_Utf16Prev
 *
 *	Get previous character in a UTF-16 sequence. This is done by skipping
 *      all low surrogate code units.
 *
 * Argument:
 *	data	- UTF-16 code unit sequence.
 *
 * Result:
 *	Position of the previous character in sequence.
 *---------------------------------------------------------------------------*/

const Col_Char2 *
Col_Utf16Prev(
    const Col_Char2 * data)
{
    while (((*--data) & 0xFC00) == 0xDC00);
    return data;
}


/****************************************************************************
 * Section: Initialization/Cleanup
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_Init
 *
 *	Initialize the library. Must be called in every thread.
 *
 * Argument:
 *	model	- Threading model.
 *
 * Side effects:
 *	Initialize the memory allocator & garbage collector.
 *
 * See also:
 *	<Threading Model Constants>
 *---------------------------------------------------------------------------*/

void 
Col_Init(
    unsigned int model)
{
    PlatEnter(model);
}

/*---------------------------------------------------------------------------
 * Function: Col_Cleanup
 *
 *	Cleanup the library. Must be called in every thread.
 *
 * Side effects:
 *	Cleanup the memory allocator & garbage collector.
 *---------------------------------------------------------------------------*/

void 
Col_Cleanup() 
{
    PlatLeave();
}


/****************************************************************************
 * Section: Error Handling
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Variable: ColibriDomain
 *
 *	Domain for Colibri error codes. Defines a message string for each code.
 *
 * See also:
 *	<Col_ErrorCode>
 *---------------------------------------------------------------------------*/

const char * const ColibriDomain[] = {
    "Generic error: %s",			// COL_ERROR_GENERIC (message)
    "%s(%d) : assertion failed! (%s)",		// COL_ERROR_ASSERTION (file, line, expression)
    "Memory error: %s",				// COL_ERROR_MEMORY
    "Called outside of a GC-protected section",	// COL_ERROR_GCPROTECT
    "%x is not a boolean word",			// COL_ERROR_BOOL (word)
    "%x is not an integer word",		// COL_ERROR_INT (word)
    "%x is not a floating point word",		// COL_ERROR_FLOAT (word)
    "%x is not a custom word",			// COL_ERROR_CUSTOMWORD (word)
    "%x is not a character",			// COL_ERROR_CHAR (word)
    "%x is not a string",			// COL_ERROR_STRING (word)
    "%x is not a rope",				// COL_ERROR_ROPE (word)
    "Rope index %u out of bounds (length=%u)",	// COL_ERROR_ROPEINDEX (index, length)
    "Combined length %u+%u exceeds the maximum"
	    " allowed value %u",		// COL_ERROR_ROPELENGTH_CONCAT (length1, length2, maxLength)	
    "Length %u times %u exceeds the maximum"
	    " allowed value %u",		// COL_ERROR_ROPELENGTH_REPEAT (length, count, maxLength)
    "Rope iterator %x is not valid",		// COL_ERROR_ROPEITER (iterator)
    "Rope iterator %x is at end"		// COL_ERROR_ROPEITER_END (iterator)
    "%x is not a vector",			// COL_ERROR_VECTOR (word)
    "%x is not a mutable vector",		// COL_ERROR_MVECTOR (word)
    "Vector length %u exceeds maximum value"
	    " %u",				// COL_ERROR_VECTORLENGTH (length, maxLength)
    "%x is not a list",				// COL_ERROR_LIST (word)
    "%x is not a mutable list",			// COL_ERROR_MLIST (word)
    "List index %u out of bounds (length=%u)",	// COL_ERROR_LISTINDEX (index, length)
    "Combined length %u+%u exceeds the maximum"
	    " allowed value %u",		// COL_ERROR_LISTLENGTH_CONCAT (length1, length2, maxLength)	
    "Length %u times %u exceeds the maximum"
	    " allowed value %u",		// COL_ERROR_LISTLENGTH_REPEAT (length, count, maxLength)
    "List iterator %x is not valid",		// COL_ERROR_LISTITER (iterator)
    "List iterator %x is at end"		// COL_ERROR_LISTITER_END (iterator)
    "%x is not a map",				// COL_ERROR_MAP (word)
    "%x is not a string or custom map",		// COL_ERROR_WORDMAP (word)
    "%x is not an integer map",			// COL_ERROR_INTMAP (word)
    "%x is not a hash map",			// COL_ERROR_HASHMAP (word)
    "%x is not a string or custom hash map",	// COL_ERROR_WORDHASHMAP (word)
    "%x is not an integer hash map",		// COL_ERROR_INTHASHMAP (word)
    "%x is not a trie map",			// COL_ERROR_TRIEMAP (word)
    "%x is not a string or custom trie map",	// COL_ERROR_WORDTRIEMAP (word)
    "%x is not an integer trie map",		// COL_ERROR_INTTRIEMAP (word)
    "Map iterator %x is not valid",		// COL_ERROR_MAPITER (iterator)
    "Map iterator %x is at end",		// COL_ERROR_MAPITER_END (iterator)
    "%x is not a string buffer",		// COL_ERROR_STRBUF (word)
};

/*---------------------------------------------------------------------------
 * Function: Col_Error
 *
 *	Signal an error condition.
 *
 * Arguments:
 *	level	- Error level.
 *	domain	- Error domain.
 *	code	- Error code.
 *	...	- Remaining arguments passed to domain proc.
 *
 * Side effects:
 *	Default implementation exits the processus when level is <COL_FATAL>
 *	or <COL_ERROR>.
 *
 * See also: 
 *	<Col_SetErrorProc>, <Col_ErrorDomain>
 *---------------------------------------------------------------------------*/

void
Col_Error(
    Col_ErrorLevel level,
    Col_ErrorDomain domain, 
    int code,
    ...)
{
    va_list args;
    ThreadData *data = PlatGetThreadData();

    va_start(args, code);
    if (data->errorProc) {
	/*
	 * Call custom proc.
	 */

	if ((data->errorProc)(level, domain, code, args)) {
	    /*
	     * Stop processing.
	     */

	    va_end(args);
	    return;
	}
    }

    /*
     * Process error.
     */

    /*
     * Prefix with error level.
     */

    switch (level) {
    case COL_FATAL:      fprintf(stderr, "[FATAL] ");          break;
    case COL_ERROR:      fprintf(stderr, "[ERROR] ");          break;
    case COL_TYPECHECK:  fprintf(stderr, "[TYPECHECK] ");      break;
    case COL_VALUECHECK: fprintf(stderr, "[VALUECHECK] ");     break;
    default:             fprintf(stderr, "[LEVEL%d] ", level); break;
    }

    /*
     * Domain-specific error messages.
     */

    vfprintf(stderr, domain[code], args);

    fprintf(stderr, "\n");
    fflush(stderr);
    va_end(args);

    switch (level) {
	case COL_FATAL:
	case COL_ERROR:
	    abort();
	    break;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_SetErrorProc
 *
 *	Set or reset the thread's custom error proc.
 *
 * Argument:
 *	proc	- The new error proc (may be NULL).
 *
 * See also: 
 *	<Col_Error>
 *---------------------------------------------------------------------------*/

void
Col_SetErrorProc(
    Col_ErrorProc *proc)
{
    PlatGetThreadData()->errorProc = proc;
}

/*---------------------------------------------------------------------------
 * Function: Col_GetErrorDomain
 *
 *	Get the domain for Colibri error codes.
 *
 * Result:
 *	The domain.
 *
 * See also: 
 *	<Col_Error>
 *---------------------------------------------------------------------------*/

Col_ErrorDomain
Col_GetErrorDomain()
{
    return ColibriDomain;
}
