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
 * Function: Col_Utf8CharAddr
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
Col_Utf8CharAddr(
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
	    COL_UTF8_NEXT(data);
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
	    COL_UTF8_PREVIOUS(data);
	}
	return data;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_Utf8GetChar
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
Col_Utf8GetChar(
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
 * Function: Col_Utf8SetChar
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
Col_Utf8SetChar(
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
 * Function: Col_Utf16CharAddr
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
Col_Utf16CharAddr(
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
	    COL_UTF16_NEXT(data);
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
	    COL_UTF16_PREVIOUS(data);
	}
	return data;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_Utf16GetChar
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
Col_Utf16GetChar(
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
 * Function: Col_Utf16SetChar
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
Col_Utf16SetChar(
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
 * Function: Col_Error
 *
 *	Signal an error condition.
 *
 * Arguments:
 *	level	- Error level.
 *	format	- Format string fed to fprintf().
 *	...	- Remaining arguments fed to fprintf().
 *
 * Side effects:
 *	Default implementation exits the processus when level is critical.
 *
 * See also: 
 *	<Col_SetErrorProc>
 *---------------------------------------------------------------------------*/

void
Col_Error(
    Col_ErrorLevel level,
    const char *format,
    ...)
{
    va_list args;
    ThreadData *data = PlatGetThreadData();

    va_start(args, format);
    if (data->errorProc) {
	/*
	 * Call custom proc.
	 */

	(data->errorProc)(level, format, args);
	return;
    }

    /*
     * Print message and abort process.
     */

    switch (level) {
	case COL_FATAL:
	    fprintf(stderr, "[FATAL] ");
	    break;

	case COL_ERROR:
	    fprintf(stderr, "[ERROR] ");
	    break;
    }
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    fflush(stderr);

    abort();
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
