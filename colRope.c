/*
 * File: colRope.c
 *
 *	This file implements the rope handling features of Colibri.
 *
 *	Ropes are a string datatype that allows for fast insertion, extraction
 *	and composition of strings. Internally they use self-balanced binary 
 *	trees.
 *
 *	They are always immutable.
 *
 * See also: 
 *	<colRope.h>
 */

#include "include/colibri.h"
#include "colInternal.h"

#include "colWordInt.h"
#include "colRopeInt.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <malloc.h> /* For alloca */

/*
 * Prototypes for functions used only in this file.
 */

static Col_RopeChunksTraverseProc Ucs1ComputeLengthProc, Ucs2ComputeLengthProc,
	Utf8ComputeByteLengthProc, Utf16ComputeByteLengthProc, 
	UcsComputeFormatProc, Ucs1CopyDataProc, Ucs2CopyDataProc, 
	Ucs4CopyDataProc, Utf8CopyDataProc, Utf16CopyDataProc, MergeChunksProc,
	FindCharProc, SearchSubropeProc, CompareChunksProc;
static ColRopeIterLeafAtProc IterAtUcs1, IterAtUcs2, IterAtUcs4, IterAtUtf8,
	IterAtUtf16;
typedef struct RopeChunkTraverseInfo *pRopeChunkTraverseInfo;
static int		IsCompatible(Col_Word rope, Col_StringFormat format);
static unsigned char	GetDepth(Col_Word rope);
static void		GetArms(Col_Word rope, Col_Word * leftPtr, 
			    Col_Word * rightPtr);
static void		GetChunk(struct RopeChunkTraverseInfo *info, 
			    Col_RopeChunk *chunk, int reverse);
static void		NextChunk(struct RopeChunkTraverseInfo *info, 
			    size_t nb, int reverse);


/*
================================================================================
Section: Ropes
================================================================================
*/

/****************************************************************************
 * Group: Rope Creation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_EmptyRope
 *
 *	Create an empty rope. The returned word is immediate and takes no space.
 *
 * Result:
 *	A new empty rope.
 *---------------------------------------------------------------------------*/

Col_Word
Col_EmptyRope()
{
    return WORD_SMALLSTR_EMPTY;
}

/*---------------------------------------------------------------------------
 * Function: Col_NewRopeFromString
 *
 *	Create a new rope from a C string. The string is treated as an UCS1
 *	character buffer whose length is computed with strlen() and is passed to
 *	<Col_NewRope>.
 *
 * Argument:
 *	string	- C string to build rope from.
 *
 * Result:
 *	A new rope containing the character data.
 *
 * See also:
 *	<Col_NewRope>
 *---------------------------------------------------------------------------*/

Col_Word 
Col_NewRopeFromString(
    const char *string)
{
    return Col_NewRope(COL_UCS1, string, strlen(string));
}

/*---------------------------------------------------------------------------
 * Function: Col_NewChar
 *
 *	Create a new rope from a single character. Use adaptive format.
 *
 * Argument:
 *	c	- Character.
 *
 * Result:
 *	A new rope made of the single character.
 *
 * See also:
 *	<Col_NewRope>
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewChar(
    Col_Char c)
{
    if (c <= COL_CHAR1_MAX) {
	/* 
	 * Return 1-byte char value.
	 */

	return WORD_CHAR_NEW(c, COL_UCS1);
    } else if (c <= COL_CHAR2_MAX) {
	/*
	 * Return 2-byte char value.
	 */

	return WORD_CHAR_NEW(c, COL_UCS2);
    } else if (c <= COL_CHAR_MAX) {
	/*
	 * Return char value.
	 */

	return WORD_CHAR_NEW(c, COL_UCS4);
    } else {
	/*
	 * Invalid character.
	 */

	return WORD_SMALLSTR_EMPTY;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_NewRope
 *
 *	Create a new rope from flat character data. This can either be a 
 *	single leaf rope containing the whole data, or a concatenation of 
 *	leaves if data is too large.
 *
 *	If the string contains a single Unicode char, or if the string is 
 *	8-bit clean and is sufficiently small, return an immediate value 
 *	instead of allocating memory.
 *
 *	If the original string is too large, data may span several 
 *	multi-cell leaf ropes. In this case we recursively split the data in 
 *	half and build a concat tree.
 *
 * Arguments:
 *	format		- Format of data in buffer (see <Col_StringFormat>).
 *			  if <COL_UCS>, data is provided as with <COL_UCS4>
 *			  but will use <COL_UCS1> or <COL_UCS2> if data fits.
 *	data		- Buffer containing flat data.
 *	byteLength	- Length of data in bytes.
 *
 * Result:
 *	A new rope containing the character data.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

Col_Word 
Col_NewRope(
    Col_StringFormat format,
    const void *data,
    size_t byteLength)
{
    Col_Word rope;
    size_t length;
    size_t half=0;		/* Index of the split point. */

    /* 
     * Quick cases. 
     */

    if (byteLength == 0) {
	/* Empty string. */
	return WORD_SMALLSTR_EMPTY;
    }

    if (FORMAT_UTF(format)) {
	/*
	 * Variable-width UTF-8/16 string.
	 */

	const char *source = (const char *) data, *p;
	size_t codeUnits = byteLength / CHAR_WIDTH(format);
	if (byteLength <= UTFSTR_MAX_BYTELENGTH) {
	    /*
	     * String fits into one multi-cell leaf rope. We know the byte 
	     * length, now get the char length.
	     */

	    length = 0;
	    for (p = source; p < source+byteLength; length++) {
		switch (format) {
		    case COL_UTF8:  p = (const char *) Col_Utf8Next ((const Col_Char1 *) p); break;
		    case COL_UTF16: p = (const char *) Col_Utf16Next((const Col_Char2 *) p); break;
		}
	    }
	    rope = (Col_Word) AllocCells(UTFSTR_SIZE(byteLength));
	    WORD_UTFSTR_INIT(rope, format, length, byteLength);
	    memcpy((void *) WORD_UTFSTR_DATA(rope), data, byteLength);
	    return rope;
	}

	/* 
	 * Split data in half at char boundary. 
	 */

	p = source + (codeUnits/2 + 1) * CHAR_WIDTH(format);
	switch (format) {
	    case COL_UTF8:  p = (const char *) Col_Utf8Prev ((const Col_Char1 *) p); break;
	    case COL_UTF16: p = (const char *) Col_Utf16Prev((const Col_Char2 *) p); break;
	}
	half = p-source;
    } else {
	/* 
	 * Fixed-width UCS string.
	 */

	length = byteLength / CHAR_WIDTH(format);
	if (length == 1) {
	    /*
	     * Single char.
	     */

	    switch (format) {
		case COL_UCS1: return WORD_CHAR_NEW(*(const Col_Char1 *) data, format);
		case COL_UCS2: return WORD_CHAR_NEW(*(const Col_Char2 *) data, format);
		case COL_UCS4: return WORD_CHAR_NEW(*(const Col_Char4 *) data, format);
		case COL_UCS: return Col_NewChar(*(const Col_Char4 *) data);
	    }
	} else if (length <= SMALLSTR_MAX_LENGTH) {
	    switch (format) {
		case COL_UCS1:
		    /*
		     * Immediate string.
		     */

		    WORD_SMALLSTR_SET_LENGTH(rope, length);
		    memcpy(WORD_SMALLSTR_DATA(rope), data, length);
		    return rope;

		case COL_UCS: {
		    /*
		     * Use immediate string if possible.
		     */

		    const Col_Char4 * string = (const Col_Char4 *) data;
		    size_t i;
		    WORD_SMALLSTR_SET_LENGTH(rope, length);
		    for (i = 0; i < length; i++) {
			if (string[i] > COL_CHAR1_MAX) break;
			WORD_SMALLSTR_DATA(rope)[i] = (Col_Char1) string[i];
		    }
		    if (i == length) {
			return rope;
		    }
		    break;
		}
	    }
	}
	if (length <= UCSSTR_MAX_LENGTH) {
	    /* 
	     * String fits into one multi-cell leaf rope. 
	     */

	    if (format == COL_UCS) {
		/*
		 * Adaptive format.
		 */

		const Col_Char4 * string = (const Col_Char4 *) data;
		size_t i;
		format = COL_UCS1;
		for (i = 0; i < length; i++) {
		    if (string[i] > COL_CHAR2_MAX) {
			format = COL_UCS4;
			break;
		    } else if (string[i] > COL_CHAR1_MAX) {
			format = COL_UCS2;
		    }
		}
		byteLength = length * CHAR_WIDTH(format);
		rope = (Col_Word) AllocCells(UCSSTR_SIZE(byteLength));
		WORD_UCSSTR_INIT(rope, format, length);
		switch (format) {
		    case COL_UCS1: {
			Col_Char1 * target 
				= (Col_Char1 *) WORD_UCSSTR_DATA(rope);
			for (i = 0; i < length; i++) {
			    target[i] = string[i];
			}
			break;
		    }

		    case COL_UCS2: {
			Col_Char2 * target 
				= (Col_Char2 *) WORD_UCSSTR_DATA(rope);
			for (i = 0; i < length; i++) {
			    target[i] = string[i];
			}
			break;
		    }

		    case COL_UCS4:
			memcpy((void *) WORD_UCSSTR_DATA(rope), data, 
				byteLength);
			break;
		}
	    } else {
		/*
		 * Fixed format.
		 */

		rope = (Col_Word) AllocCells(UCSSTR_SIZE(byteLength));
		WORD_UCSSTR_INIT(rope, format, length);
		memcpy((void *) WORD_UCSSTR_DATA(rope), data, byteLength);
	    }
	    return rope;
	}

	/* 
	 * Split data in half at char boundary. 
	 */

	half = (length/2) * CHAR_WIDTH(format);
    }

    /* 
     * The rope is built by concatenating the two halves of the string. This 
     * recursive halving ensures that the resulting binary tree is properly 
     * balanced. 
     */

    return Col_ConcatRopes(Col_NewRope(format, data, half),
	    Col_NewRope(format, (const char *) data+half, byteLength-half));
}

/*---------------------------------------------------------------------------
 * Internal Function: Ucs1ComputeLengthProc
 *
 *	Rope traversal procedure used to compute length of ropes normalized 
 *	to UCS-1. Follows <Col_RopeChunksTraverseProc> signature.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 1.
 *	chunks		- Array of chunks. First chunk never NULL.
 *	clientData	- Points to the length variable to compute.
 *
 * Result:
 *	Always 0.
 *
 * Side effects:
 *	Length pointed to by clientData is updated.
 *
 * See also:
 *	<Col_NormalizeRope>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static int 
Ucs1ComputeLengthProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_RopeChunk *chunks,
    Col_ClientData clientData) 
{
    size_t *lengthPtr = (size_t *) clientData;
    const char *p = (const char *) chunks->data;
    Col_Char c;
    size_t i;

    ASSERT(number == 1);
    if (chunks->format == COL_UCS1) {
	*lengthPtr += length;
	return 0;
    }
    for (i = 0; i < length; i++) {
	COL_CHAR_NEXT(chunks->format, p, c);
	if (c <= COL_CHAR1_MAX) (*lengthPtr)++;
    }
    return 0;
}

/*---------------------------------------------------------------------------
 * Internal Function: Ucs2ComputeLengthProc
 *
 *	Rope traversal procedure used to compute length of ropes normalized 
 *	to UCS-2. Follows <Col_RopeChunksTraverseProc> signature.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 1.
 *	chunks		- Array of chunks. First chunk never NULL.
 *	clientData	- Points to the length variable to compute.
 *
 * Result:
 *	Always 0.
 *
 * Side effects:
 *	Length pointed to by clientData is updated.
 *
 * See also:
 *	<Col_NormalizeRope>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static int 
Ucs2ComputeLengthProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_RopeChunk *chunks,
    Col_ClientData clientData) 
{
    size_t *lengthPtr = (size_t *) clientData;
    const char *p = (const char *) chunks->data;
    Col_Char c;
    size_t i;

    ASSERT(number == 1);

    if (chunks->format == COL_UCS1 || chunks->format == COL_UCS2) {
	*lengthPtr += length;
	return 0;
    }
    for (i = 0; i < length; i++) {
	COL_CHAR_NEXT(chunks->format, p, c);
	if (c <= COL_CHAR2_MAX) (*lengthPtr)++;
    }
    return 0;
}

/*---------------------------------------------------------------------------
 * Internal Function: Utf8ComputeByteLengthProc
 *
 *	Rope traversal procedure used to compute byte length of ropes normalized
 *	to UTF-8. Follows <Col_RopeChunksTraverseProc> signature.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 1.
 *	chunks		- Array of chunks. First chunk never NULL.
 *	clientData	- Points to the length variable to compute.
 *
 * Result:
 *	1 if byte length exceeds <UTFSTR_MAX_BYTELENGTH> (which stops the 
 *	traversal), else 0.
 *
 * Side effects:
 *	Length pointed to by clientData is updated.
 *
 * See also:
 *	<Col_NormalizeRope>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static int 
Utf8ComputeByteLengthProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_RopeChunk *chunks,
    Col_ClientData clientData) 
{
    size_t *byteLengthPtr = (size_t *) clientData;
    const char *p = (const char *) chunks->data;
    Col_Char c;
    size_t i;

    ASSERT(number == 1);

    for (i = 0; i < length; i++) {
	COL_CHAR_NEXT(chunks->format, p, c);
	*byteLengthPtr += COL_UTF8_WIDTH(c) * CHAR_WIDTH(COL_UTF8);
	if (*byteLengthPtr > UTFSTR_MAX_BYTELENGTH) return 1;
    }
    return 0;
}

/*---------------------------------------------------------------------------
 * Internal Function: Utf16ComputeByteLengthProc
 *
 *	Rope traversal procedure used to compute byte length of ropes normalized
 *	to UTF-16. Follows <Col_RopeChunksTraverseProc> signature.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 1.
 *	chunks		- Array of chunks. First chunk never NULL.
 *	clientData	- Points to the length variable to compute.
 *
 * Result:
 *	1 if byte length exceeds <UTFSTR_MAX_BYTELENGTH> (which stops the 
 *	traversal), else 0.
 *
 * Side effects:
 *	Length pointed to by clientData is updated.
 *
 * See also:
 *	<Col_NormalizeRope>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static int 
Utf16ComputeByteLengthProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_RopeChunk *chunks,
    Col_ClientData clientData) 
{
    size_t *byteLengthPtr = (size_t *) clientData;
    const char *p = (const char *) chunks->data;
    Col_Char c;
    size_t i;

    ASSERT(number == 1);

    for (i = 0; i < length; i++) {
	COL_CHAR_NEXT(chunks->format, p, c);
	*byteLengthPtr += COL_UTF16_WIDTH(c) * CHAR_WIDTH(COL_UTF16);
	if (*byteLengthPtr > UTFSTR_MAX_BYTELENGTH) return 1;
    }
    return 0;
}

/*---------------------------------------------------------------------------
 * Internal Function: UcsComputeFormatProc
 *
 *	Rope traversal procedure used to compute actual format of ropes 
 *	normalized to UCS. Follows <Col_RopeChunksTraverseProc> signature.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 1.
 *	chunks		- Array of chunks. First chunk never NULL.
 *	clientData	- Points to the format variable to compute.
 *
 * Result:
 *	1 upon first UCS-4 character (which stops the traversal), else 0.
 *
 * Side effects:
 *	Format pointed to by clientData is updated.
 *
 * See also:
 *	<Col_NormalizeRope>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static int 
UcsComputeFormatProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_RopeChunk *chunks,
    Col_ClientData clientData) 
{
    Col_StringFormat *formatPtr = (Col_StringFormat *) clientData;
    const char *p = (const char *) chunks->data;
    Col_Char c;
    size_t i;

    ASSERT(number == 1);
    ASSERT(*formatPtr != COL_UCS4);

    for (i = 0; i < length; i++) {
	COL_CHAR_NEXT(chunks->format, p, c);
	if (c > COL_CHAR2_MAX) {
	    *formatPtr = COL_UCS4;
	    return 1;
	} else if (c > COL_CHAR1_MAX) {
	    *formatPtr = COL_UCS2;
	}
    }
    return 0;
}

/*---------------------------------------------------------------------------
 * Internal Typedef: CopyDataInfo
 *
 *	Structure used to copy data during the traversal of ropes when 
 *	normalizing data.
 *
 * Fields:
 *	data		- Buffer storing the normalized data.
 *	replace		- Replacement character for unrepresentable codepoints.
 *
 * See also:
 *	<Col_NormalizeRope>
 *---------------------------------------------------------------------------*/

typedef struct CopyDataInfo {
    char *data;
    Col_Char replace;
} CopyDataInfo;

/*---------------------------------------------------------------------------
 * Internal Function: UcsComputeFormatProc
 *
 *	Rope traversal procedure used to copy normalized data from a rope using
 *	the UCS-1 format. Follows <Col_RopeChunksTraverseProc> signature.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 1.
 *	chunks		- Array of chunks. First chunk never NULL.
 *	clientData	- Points to <CopyDataInfo>.
 *
 * Result:
 *	Always 0.
 *
 * Side effects:
 *	Copy the string data into the associated <CopyDataInfo> fields.
 *
 * See also:
 *	<Col_NormalizeRope>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static int 
Ucs1CopyDataProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_RopeChunk *chunks,
    Col_ClientData clientData) 
{
    CopyDataInfo *info = (CopyDataInfo *) clientData;
    const char *p = (const char *) chunks->data;
    Col_Char c;
    size_t i;

    ASSERT(number == 1);

    for (i = 0; i < length; i++) {
	COL_CHAR_NEXT(chunks->format, p, c);
	if (c > COL_CHAR1_MAX) {
	    if (info->replace == COL_CHAR_INVALID) continue;
	    c = info->replace;
	}
	*(Col_Char1 *) info->data = c; 
	info->data += CHAR_WIDTH(COL_UCS1);
    }
    return 0;
}

/*---------------------------------------------------------------------------
 * Internal Function: Ucs2CopyDataProc
 *
 *	Rope traversal procedure used to copy normalized data from a rope using
 *	the UCS-2 format. Follows <Col_RopeChunksTraverseProc> signature.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 1.
 *	chunks		- Array of chunks. First chunk never NULL.
 *	clientData	- Points to <CopyDataInfo>.
 *
 * Result:
 *	Always 0.
 *
 * Side effects:
 *	Copy the string data into the associated <CopyDataInfo> fields.
 *
 * See also:
 *	<Col_NormalizeRope>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static int 
Ucs2CopyDataProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_RopeChunk *chunks,
    Col_ClientData clientData) 
{
    CopyDataInfo *info = (CopyDataInfo *) clientData;
    const char *p = (const char *) chunks->data;
    Col_Char c;
    size_t i;

    ASSERT(number == 1);

    for (i = 0; i < length; i++) {
	COL_CHAR_NEXT(chunks->format, p, c);
	if (c > COL_CHAR2_MAX) {
	    if (info->replace == COL_CHAR_INVALID) continue;
	    c = info->replace;
	}
	*(Col_Char2 *) info->data = c; 
	info->data += CHAR_WIDTH(COL_UCS2);
    }
    return 0;
}

/*---------------------------------------------------------------------------
 * Internal Function: Ucs4CopyDataProc
 *
 *	Rope traversal procedure used to copy normalized data from a rope using
 *	the UCS-4 format. Follows <Col_RopeChunksTraverseProc> signature.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 1.
 *	chunks		- Array of chunks. First chunk never NULL.
 *	clientData	- Points to <CopyDataInfo>.
 *
 * Result:
 *	Always 0.
 *
 * Side effects:
 *	Copy the string data into the associated <CopyDataInfo> fields.
 *
 * See also:
 *	<Col_NormalizeRope>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static int 
Ucs4CopyDataProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_RopeChunk *chunks,
    Col_ClientData clientData) 
{
    CopyDataInfo *info = (CopyDataInfo *) clientData;
    const char *p = (const char *) chunks->data;
    Col_Char c;
    size_t i;

    ASSERT(number == 1);

    for (i = 0; i < length; i++) {
	COL_CHAR_NEXT(chunks->format, p, c);
	*(Col_Char4 *) info->data = c; 
	info->data += CHAR_WIDTH(COL_UCS4);
    }
    return 0;
}

/*---------------------------------------------------------------------------
 * Internal Function: Utf8CopyDataProc
 *
 *	Rope traversal procedure used to copy normalized data from a rope using
 *	the UTF-8 format. Follows <Col_RopeChunksTraverseProc> signature.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 1.
 *	chunks		- Array of chunks. First chunk never NULL.
 *	clientData	- Points to <CopyDataInfo>.
 *
 * Result:
 *	Always 0.
 *
 * Side effects:
 *	Copy the string data into the associated <CopyDataInfo> fields.
 *
 * See also:
 *	<Col_NormalizeRope>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static int 
Utf8CopyDataProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_RopeChunk *chunks,
    Col_ClientData clientData) 
{
    CopyDataInfo *info = (CopyDataInfo *) clientData;
    const char *p = (const char *) chunks->data;
    Col_Char c;
    size_t i;

    ASSERT(number == 1);

    for (i = 0; i < length; i++) {
	COL_CHAR_NEXT(chunks->format, p, c);
	info->data = (char *) Col_Utf8Set((Col_Char1 *) info->data, c);
    }
    return 0;
}

/*---------------------------------------------------------------------------
 * Internal Function: Utf16CopyDataProc
 *
 *	Rope traversal procedure used to copy normalized data from a rope using
 *	the UTF-16 format. Follows <Col_RopeChunksTraverseProc> signature.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 1.
 *	chunks		- Array of chunks. First chunk never NULL.
 *	clientData	- Points to <CopyDataInfo>.
 *
 * Result:
 *	Always 0.
 *
 * Side effects:
 *	Copy the string data into the associated <CopyDataInfo> fields.
 *
 * See also:
 *	<Col_NormalizeRope>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static int 
Utf16CopyDataProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_RopeChunk *chunks,
    Col_ClientData clientData) 
{
    CopyDataInfo *info = (CopyDataInfo *) clientData;
    const char *p = (const char *) chunks->data;
    Col_Char c;
    size_t i;

    ASSERT(number == 1);

    for (i = 0; i < length; i++) {
	COL_CHAR_NEXT(chunks->format, p, c);
	info->data = (char *) Col_Utf16Set((Col_Char2 *) info->data, c); 
    }
    return 0;
}

/*---------------------------------------------------------------------------
 * Internal Function: IsCompatible
 *
 *	Check whether rope is compatible with the given format.
 *
 * Arguments:
 *	rope	- The rope to check.
 *	format	- Target format.
 *
 * Result:
 *	Nonzero if compatible, else zero. Custom ropes are always considered
 *	incompatible so that they are flattened in any case.
 *
 * See also:
 *	<Col_NormalizeRope>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static int
IsCompatible(
    Col_Word rope,
    Col_StringFormat format)
{
    switch (WORD_TYPE(rope)) {
	case WORD_TYPE_CHAR:
	    return (format == WORD_CHAR_WIDTH(rope) || format == COL_UCS);

	case WORD_TYPE_SMALLSTR:
	    return (format == COL_UCS1 || format == COL_UCS);

	case WORD_TYPE_UCSSTR:
	    return (format == WORD_UCSSTR_FORMAT(rope) || format == COL_UCS);

	case WORD_TYPE_UTFSTR:
	    return (format == WORD_UTFSTR_FORMAT(rope));

	case WORD_TYPE_CUSTOM:
	    return 0;

	case WORD_TYPE_SUBROPE:
	    return IsCompatible(WORD_SUBROPE_SOURCE(rope), format);

	case WORD_TYPE_CONCATROPE:
	    return IsCompatible(WORD_CONCATROPE_LEFT(rope), format)
		    && IsCompatible(WORD_CONCATROPE_RIGHT(rope), format);

	default:
	    /* CANTHAPPEN */
	    return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_NormalizeRope
 *
 *	Create a copy of a rope using a given target format. Unrepresentable
 *	characters (i.e. whose codepoint is too large to fit the target 
 *	representation) can be skipped or replaced by a replacement character.
 *	The rope is converted chunk-wise (i.e. subropes that have the right
 *	format remain unchanged) but can optionally be flattened to form a 
 *	single chunk (or several concatenated chunks for larger strings).
 *
 *	Immediate representations are used as long as they match the format.
 *
 *	Custom ropes are always converted whatever their format.
 *
 * Arguments:
 *	rope	- Rope to copy.
 *	format	- Target format (see <Col_StringFormat> and <COL_UCS>).
 *	replace	- Replacement characters for unrepresentable codepoints,
 *		  or <COL_CHAR_INVALID> to skip.
 *	flatten	- If true, flatten ropes into large string arrays, else keep
 *		  ropes fitting the target format whatever their structure.
 *
 * Result:
 *	A new rope containing the character data.
 *
 * Side effects:
 *	May allocate memory cells.
 *
 * See also:
 *	<Col_NewRope>
 *---------------------------------------------------------------------------*/

Col_Word
Col_NormalizeRope(
    Col_Word rope, 
    Col_StringFormat format, 
    Col_Char replace,
    int flatten)
{
    size_t length = Col_RopeLength(rope), byteLength;
    Col_Word normalized;
    CopyDataInfo copyDataInfo;
    Col_RopeChunksTraverseProc *copyDataProc;
    Col_Char c;
    int type;

    if (length == 0) return WORD_SMALLSTR_EMPTY;

    /*
     * Replacement char must respect the target format.
     */

    if ((format == COL_UCS1 && replace > COL_CHAR1_MAX)
	    || (format == COL_UCS2 && replace > COL_CHAR2_MAX)) {
	replace = COL_CHAR_INVALID;
    }

    /*
     * Quick cases for leaf nodes.
     */

    type = WORD_TYPE(rope);
    switch (type) {
	case WORD_TYPE_CHAR:
	    flatten = 1;
	    c = WORD_CHAR_GET(rope);
	    switch (format) {
		case COL_UCS1:
		    if (c < COL_CHAR1_MAX) {
			return WORD_CHAR_NEW(c, format);
		    } else if (replace != COL_CHAR_INVALID) {
			return WORD_CHAR_NEW(replace, format);
		    } else {
			return WORD_SMALLSTR_EMPTY;
		    }

		case COL_UCS2:
		    if (c < COL_CHAR2_MAX) {
			return WORD_CHAR_NEW(c, format);
		    } else if (replace != COL_CHAR_INVALID) {
			return WORD_CHAR_NEW(replace, format);
		    } else {
			return WORD_SMALLSTR_EMPTY;
		    }

		case COL_UCS4:
		    return WORD_CHAR_NEW(c, format);

		case COL_UCS:
		    return Col_NewChar(c);
	    }
	    break;

	case WORD_TYPE_SMALLSTR:
	    flatten = 1;
	    if (WORD_SMALLSTR_LENGTH(rope) == 1) {
		switch (format) {
		    case COL_UCS1:
		    case COL_UCS2:
		    case COL_UCS4:
			return WORD_CHAR_NEW(*WORD_SMALLSTR_DATA(rope), format);

		    case COL_UCS:
			return Col_NewChar(*WORD_SMALLSTR_DATA(rope));
		}
	    } else if (format == COL_UCS1 || format == COL_UCS) {
		return rope;
	    }
	    break;

	case WORD_TYPE_UCSSTR:
	    flatten = 1;
	    if (WORD_UCSSTR_FORMAT(rope) == format || format == COL_UCS) 
		    return rope;
	    break;

	case WORD_TYPE_UTFSTR:
	    flatten = 1;
	    if (WORD_UTFSTR_FORMAT(rope) == format) return rope;
	    break;

	case WORD_TYPE_CUSTOM:
	    flatten = 1;
	    break;
    }

    /*
     * Formats differ or rope is complex.
     */

    if (flatten) {
	/*
	 * Flatten rope into large uniform chunks.
	 */

	int single = 0;

	switch (format) {
	    case COL_UCS1:
		if (length <= UCSSTR_MAX_LENGTH) {
		    /* 
		     * String fits into one multi-cell leaf rope. 
		     */

		    single = 1;
		    if (replace == COL_CHAR_INVALID) {
			/*
			 * Skip unrepresentable chars: compute actual length.
			 */

			length = 0;
			Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0, 
				Ucs1ComputeLengthProc, &length, NULL);
		    }
		    byteLength = length * CHAR_WIDTH(format);
		}
		break;

	    case COL_UCS2:
		if (length <= UCSSTR_MAX_LENGTH) {
		    /* 
		     * String fits into one multi-cell leaf rope. 
		     */

		    single = 1;
		    if (replace == COL_CHAR_INVALID) {
			/*
			 * Skip unrepresentable chars: compute actual length.
			 */

			length = 0;
			Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0, 
				Ucs2ComputeLengthProc, &length, NULL);
		    }
		    byteLength = length * CHAR_WIDTH(format);
		}
		break;

	    case COL_UCS4:
		if (length <= UCSSTR_MAX_LENGTH) {
		    /* 
		     * String fits into one multi-cell leaf rope. 
		     */

		    single = 1;
		    byteLength = length * CHAR_WIDTH(format);
		}
		break;

	    case COL_UCS:
		/*
		 * Adaptive format.
		 */

		if (length <= UCSSTR_MAX_LENGTH) {
		    /* 
		     * String fits into one multi-cell leaf rope. Get actual
		     * format.
		     */

		    single = 1;
		    format = COL_UCS1;
		    Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0, 
			    UcsComputeFormatProc, &format, NULL);
		    byteLength = length * CHAR_WIDTH(format);
		}
		break;

	    case COL_UTF8:
		if (length * CHAR_WIDTH(format) <= UTFSTR_MAX_BYTELENGTH) {
		    /*
		     * String may fit into one multi-cell leaf rope, compute 
		     * actual byte length.
		     */

		    byteLength = 0;
		    single = !Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0, 
			    Utf8ComputeByteLengthProc, &byteLength, NULL);
		}
		break;

	    case COL_UTF16:
		if (length * CHAR_WIDTH(format) <= UTFSTR_MAX_BYTELENGTH) {
		    /*
		     * String may fit into one multi-cell leaf rope, compute 
		     * actual byte length.
		     */

		    byteLength = 0;
		    single = !Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0, 
			    Utf16ComputeByteLengthProc, &byteLength, NULL);
		}
		break;
	}

	if (length == 0) return WORD_SMALLSTR_EMPTY;

	if (!single) {
	    /*
	     * Normalize and concat halves.
	     */

	    size_t half = length / 2;
	    return Col_ConcatRopes(Col_NormalizeRope(Col_Subrope(rope, 0, 
		    half-1), format, replace, flatten), Col_NormalizeRope(
		    Col_Subrope(rope, half, length-1), format, replace, 
		    flatten));
	}

	/*
	 * Create new string from existing data.
	 */

	if (FORMAT_UTF(format)) {
	    /*
	     * Variable-width UTF-8/16 string.
	     */

	    ASSERT(byteLength <= UTFSTR_MAX_BYTELENGTH);
	    normalized = (Col_Word) AllocCells(UTFSTR_SIZE(byteLength));
	    WORD_UTFSTR_INIT(normalized, format, length, byteLength);
	    copyDataInfo.data = (char *) WORD_UTFSTR_DATA(normalized);
	} else if (length > (format == COL_UCS1 ? SMALLSTR_MAX_LENGTH : 1)) {
	    /*
	     * Fixed-width UCS string.
	     */

	    ASSERT(length <= UCSSTR_MAX_LENGTH);
	    normalized = (Col_Word) AllocCells(UCSSTR_SIZE(byteLength));
	    WORD_UCSSTR_INIT(normalized, format, length);
	    copyDataInfo.data = (char *) WORD_UCSSTR_DATA(normalized);
	} else if (format == COL_UCS1 && length > 1) {
	    /*
	     * Immediate string.
	     */

	    ASSERT(length <= SMALLSTR_MAX_LENGTH);
	    WORD_SMALLSTR_SET_LENGTH(normalized, length);
	    copyDataInfo.data = (char *) WORD_SMALLSTR_DATA(normalized);
	} else {
	    /*
	     * Single character.
	     */

	    copyDataInfo.data = (char *) &c;
	}

	/*
	 * Copy data.
	 */

	copyDataInfo.replace = replace;
	switch (format) {
	    case COL_UCS1:  copyDataProc = Ucs1CopyDataProc; break;
	    case COL_UCS2:  copyDataProc = Ucs2CopyDataProc; break;
	    case COL_UCS4:  copyDataProc = Ucs4CopyDataProc; break;
	    case COL_UTF8:  copyDataProc = Utf8CopyDataProc; break;
	    case COL_UTF16: copyDataProc = Utf16CopyDataProc; break;
	}
	Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0, copyDataProc, 
		&copyDataInfo, NULL);
	if (length == 1 && !FORMAT_UTF(format)) {
	    /*
	     * Single character.
	     */

	    return WORD_CHAR_NEW(c, format);
	}
	ASSERT(copyDataInfo.data - (FORMAT_UTF(format) ? WORD_UTFSTR_DATA(normalized) : WORD_UCSSTR_DATA(normalized)) == byteLength);
	return normalized;
    }

    /*
     * Complex nodes. Keep existing nodes matching the format and convert 
     * others.
     */

    switch (type) {
	case WORD_TYPE_SUBROPE:
	    if (IsCompatible(rope, format)) {
		return rope;
	    }
	    if (WORD_SUBROPE_DEPTH(rope) == 0) {
		/*
		 * Source is leaf, flatten.
		 */

		return Col_NormalizeRope(rope, format, replace, 1);
	    } else {
		/*
		 * Split rope and normalize both arms.
		 */

		Col_Word source = WORD_SUBROPE_SOURCE(rope),
			left = Col_Subrope(WORD_CONCATROPE_LEFT(source),
			WORD_SUBROPE_FIRST(rope), SIZE_MAX),
			right = Col_Subrope(WORD_CONCATROPE_RIGHT(source),
			0, WORD_SUBROPE_LAST(rope) - WORD_SUBROPE_FIRST(rope) 
			+ 1 - Col_RopeLength(left));
		left = Col_NormalizeRope(left, format, replace, 0);
		right = Col_NormalizeRope(right, format, replace, 0);
		return Col_ConcatRopes(left, right);
	    }

	case WORD_TYPE_CONCATROPE: {
	    /*
	     * Normalize both arms.
	     */

	    Col_Word left = WORD_CONCATROPE_LEFT(rope), 
		    right = WORD_CONCATROPE_RIGHT(rope);
	    left = Col_NormalizeRope(left, format, replace, 0);
	    right = Col_NormalizeRope(right, format, replace, 0);
	    if (left == WORD_CONCATROPE_LEFT(rope) 
		    && right == WORD_CONCATROPE_RIGHT(rope)) {
		/*
		 * Both arms are compatible and so is the rope.
		 */

		return rope;
	    } else {
		return Col_ConcatRopes(left, right);
	    }
	}

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
	    return WORD_NIL;
    }
}


/****************************************************************************
 * Group: Rope Access and Comparison
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_RopeLength
 *
 *	Get the length of the rope.
 *
 * Argument:
 *	rope	- Rope to get length for.
 *
 * Result:
 *	The rope length.
 *---------------------------------------------------------------------------*/

size_t 
Col_RopeLength(
    Col_Word rope)
{
    switch (WORD_TYPE(rope)) {
	/*
	 * Immediate types.
	 */

	case WORD_TYPE_CHAR:
	    return 1;

	case WORD_TYPE_SMALLSTR:
	    return WORD_SMALLSTR_LENGTH(rope);

	/* 
	 * Flat string, fixed or variable.
	 */

	case WORD_TYPE_UCSSTR:
	    return WORD_UCSSTR_LENGTH(rope);

	case WORD_TYPE_UTFSTR:
	    return WORD_UTFSTR_LENGTH(rope);

	/*
	 * Rope nodes.
	 */

	case WORD_TYPE_SUBROPE:
	    /* 
	     * Subrope length is the range width.
	     */

	    return WORD_SUBROPE_LAST(rope)-WORD_SUBROPE_FIRST(rope)+1;
	    
	case WORD_TYPE_CONCATROPE:
	    /* 
	     * Concat length is the sum of its arms (known at creation time).
	     */

	    return WORD_CONCATROPE_LENGTH(rope);

	case WORD_TYPE_CUSTOM: {
	    Col_CustomWordType *typeInfo = WORD_TYPEINFO(rope);
	    if (typeInfo->type == COL_ROPE) {
		/*
		 * Custom ropes.
		 */

		return ((Col_CustomRopeType *) typeInfo)->lengthProc(rope);
	    }
	    /* continued */
	}

	/* WORD_TYPE_UNKNOWN */

	default:
	    /*
	     * Invalid type.
	     */

	    Col_Error(COL_ERROR, "%x is not a rope", rope);
	    return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeAt
 *
 *	Get the character codepoint of a rope at a given position. 
 *
 * Argument:
 *	rope	- Rope to get character from.
 *	index	- Character index.
 *
 * Result:
 *	If the index is past the end of the rope, the invalid codepoint
 *	<COL_CHAR_INVALID>, else the Unicode codepoint of the character.
 *---------------------------------------------------------------------------*/

Col_Char 
Col_RopeAt(
    Col_Word rope,
    size_t index)
{
    Col_RopeIterator it;
    
    Col_RopeIterBegin(rope, index, &it);
    return Col_RopeIterEnd(&it) ? COL_CHAR_INVALID : Col_RopeIterAt(&it);
}

/*---------------------------------------------------------------------------
 * Internal Typedef: FindCharInfo
 *
 *	Structure used to collect data during character search.
 *
 * Fields:
 *	c	- Character to search for.
 *	reverse	- Whether to traverse in reverse order.
 *	pos	- Upon return, position of character if found
 *
 * See also:
 *	<FindCharProc>, <Col_RopeFind>
 *---------------------------------------------------------------------------*/

typedef struct FindCharInfo {
    Col_Char c;
    int reverse;
    size_t pos;
} FindCharInfo;

/*---------------------------------------------------------------------------
 * Internal Function: FindCharProc
 *
 *	Rope traversal procedure used to find characters in ropes. Follows 
 *	<Col_RopeChunksTraverseProc> signature.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 2.
 *	chunks		- Array of chunks. When chunk is NULL, means the
 *			  index is past the end of the traversed rope.
 *	clientData	- Points to <FindCharInfo>.
 *
 * Results:
 *	1 if character was found, else 0. Additional info returned through 
 *	clientData.
 *---------------------------------------------------------------------------*/

static int 
FindCharProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_RopeChunk *chunks,
    Col_ClientData clientData) 
{
    FindCharInfo *info = (FindCharInfo *) clientData;
    size_t i;
    const char *data;
    Col_Char c;

    ASSERT(number == 1);

    /*
     * Search for character.
     */

    ASSERT(chunks->data);
    data = (const char *) chunks->data;
    if (info->reverse) data += chunks->byteLength;
    for (i = (info->reverse ? length-1 : 0); 
	    (info->reverse ? i >= 0 : i < length); 
	    (info->reverse ? i-- : i++)) {
	if (info->reverse) {
	    COL_CHAR_PREVIOUS(chunks->format, data, c);
	} else {
	    COL_CHAR_NEXT(chunks->format, data, c);
	}
	if (c == info->c) {
	    /*
	     * Found!
	     */

	    info->pos = index+i;
	    return 1;
	}
	if (info->reverse) {
	    if (i == 0) break; /* Avoids overflow. */
	}
    }

    /*
     * Not found.
     */

    return 0;
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeFind
 *
 *	Find first occurrence of a character in a rope.
 *
 * Arguments:
 *	rope	- Rope to search character into.
 *	c	- Character to search for.
 *	start	- Starting index.
 *	max	- Maximum number of characters to search.
 *	reverse	- Whether to traverse in reverse order.
 *
 * Results:
 *	If found, returns the position of the character in rope. Else returns
 *	SIZE_MAX (which is an invalid character index since this is the maximum
 *	rope length, and indices are zero-based).
 *
 * See also:
 *	<Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

size_t
Col_RopeFind(
    Col_Word rope, 
    Col_Char c, 
    size_t start, 
    size_t max, 
    int reverse)
{
    FindCharInfo info = {c, reverse, SIZE_MAX};

    Col_TraverseRopeChunks(rope, start, max, reverse, FindCharProc, &info, 
	    NULL);

    return info.pos;
}

/*---------------------------------------------------------------------------
 * Internal Typedef: SearchSubropeInfo
 *
 *	Structure used to collect data during subrope search.
 *
 * Fields:
 *	rope		- Rope to search subrope into.
 *	subrope		- Subrope to search for.
 *	first		- First character of rope to search for.
 *	reverse		- Whether to traverse in reverse order.
 *	pos		- Upon return, position of character if found
 *
 * See also:
 *	<FindCharProc>, <Col_RopeFind>
 *---------------------------------------------------------------------------*/

typedef struct SearchSubropeInfo {
    Col_Word rope, subrope;
    Col_Char first;
    int reverse;
    size_t pos;
} SearchSubropeInfo;

/*---------------------------------------------------------------------------
 * Internal Function: SearchSubropeProc
 *
 *	Rope traversal procedure used to find subrope in ropes. Follows 
 *	<Col_RopeChunksTraverseProc> signature.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 2.
 *	chunks		- Array of chunks. When chunk is NULL, means the
 *			  index is past the end of the traversed rope.
 *	clientData	- Points to <SearchSubropeInfo>.
 *
 * Results:
 *	1 if character was found, else 0. Additional info returned through 
 *	clientData.
 *---------------------------------------------------------------------------*/

static int 
SearchSubropeProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_RopeChunk *chunks,
    Col_ClientData clientData) 
{
    SearchSubropeInfo *info = (SearchSubropeInfo *) clientData;
    size_t i;
    const char *data;
    Col_Char c;

    ASSERT(number == 1);

    /*
     * Search for character.
     */

    ASSERT(chunks->data);
    data = (const char *) chunks->data;
    if (info->reverse) data += chunks->byteLength;
    for (i = (info->reverse ? length-1 : 0); 
	    (info->reverse ? i >= 0 : i < length); 
	    (info->reverse ? i-- : i++)) {
	if (info->reverse) {
	    COL_CHAR_PREVIOUS(chunks->format, data, c);
	} else {
	    COL_CHAR_NEXT(chunks->format, data, c);
	}
	if (c == info->first) {
	    /*
	     * Found first character, look for remainder.
	     */

	    Col_RopeIterator it1, it2;
	    for (Col_RopeIterBegin(info->rope, index+i, &it1), 
		    Col_RopeIterFirst(info->subrope, &it2);
		    !Col_RopeIterEnd(&it1) && !Col_RopeIterEnd(&it2)
		    && Col_RopeIterAt(&it1) == Col_RopeIterAt(&it2);
		    Col_RopeIterNext(&it1), Col_RopeIterNext(&it2));
	    if (Col_RopeIterEnd(&it2)) {
		/*
		 * Found!
		 */

		info->pos = index+i;
		return 1;
	    }
	}
	if (info->reverse) {
	    if (i == 0) break; /* Avoids overflow. */
	}
    }

    /*
     * Not found.
     */

    return 0;
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeSearch
 *
 *	Find first occurrence of a subrope in a rope.
 *
 * Arguments:
 *	rope	- Rope to search subrope into.
 *	subrope	- Subrope to search for.
 *	start	- Starting index.
 *	max	- Maximum number of characters to search.
 *	reverse	- Whether to traverse in reverse order.
 *
 * Results:
 *	If found, returns the position of the subrope in rope. Else returns
 *	SIZE_MAX (which is an invalid character index since this is the maximum
 *	rope length, and indices are zero-based).
 *
 * See also:
 *	<Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

size_t
Col_RopeSearch(
    Col_Word rope, 
    Col_Word subrope, 
    size_t start, 
    size_t max, 
    int reverse)
{
    SearchSubropeInfo info = {rope, subrope, Col_RopeAt(subrope, 0), reverse, 
	    SIZE_MAX};
    size_t length = Col_RopeLength(rope), 
	    subropeLength = Col_RopeLength(subrope);

    if (subropeLength == 0) {
	/*
	 * Subrope is empty.
	 */

	return SIZE_MAX;
    } else if (subropeLength == 1) {
	/*
	 * Single char subrope: find char instead.
	 */

	return Col_RopeFind(rope, Col_RopeAt(subrope, 0), start, max, reverse);
    }
    
    if (length-start < subropeLength) {
	/*
	 * Rope tail is shorter than subrope, adjust indices.
	 */

	if (!reverse) {
	    /*
	     * Not enough characters to fit subrope into.
	     */

	    return SIZE_MAX;
	} else {
	    size_t offset = subropeLength-(length-start);
	    if (max <= offset || start < offset) {
		/*
		 * Not enough characters to fit subrope into.
		 */

		return SIZE_MAX;
	    }
	    max -= offset;
	    start -= offset;
	}
    }

    /*
     * Generic case.
     */

    Col_TraverseRopeChunks(rope, start, max, reverse, SearchSubropeProc,
	    &info, NULL);

    return info.pos;
}

/*---------------------------------------------------------------------------
 * Internal Typedef: CompareChunksInfo
 *
 *	Structure used to collect data during rope comparison.
 *
 * Fields:
 *	posPtr		- If non-NULL, position of the first differing 
 *			  character.
 *	c1Ptr, c2Ptr	- If non-NULL, respective codepoints of differing 
 *			  characters.
 *
 * See also:
 *	<CompareChunksProc>, <Col_CompareRopesL>
 *---------------------------------------------------------------------------*/

typedef struct CompareChunksInfo {
    size_t *posPtr; 
    Col_Char *c1Ptr; 
    Col_Char *c2Ptr;
} CompareChunksInfo;

/*---------------------------------------------------------------------------
 * Internal Function: CompareChunksProc
 *
 *	Rope traversal procedure used to compare rope chunks. Follows 
 *	<Col_RopeChunksTraverseProc> signature.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 2.
 *	chunks		- Array of chunks. When chunk is NULL, means the
 *			  index is past the end of the traversed rope.
 *	clientData	- Points to <CompareChunksInfo>.
 *
 * Results:
 *	0 if chunks are identical, else 1. Additional info returned through 
 *	clientData.
 *---------------------------------------------------------------------------*/

static int 
CompareChunksProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_RopeChunk *chunks,
    Col_ClientData clientData) 
{
    CompareChunksInfo *info = (CompareChunksInfo *) clientData;
    size_t i;
    Col_Char c1;
    Col_Char c2;
    const char *data[2] = {(const char *) chunks[0].data, 
	    (const char *) chunks[1].data};

    ASSERT(number == 2);

    /*
     * Quick cases.
     */

    if (!data[1]) {
	/*
	 * Chunk 1 is longer.
	 */

	ASSERT(data[0]);
	if (info->posPtr) *info->posPtr = index;
	if (info->c1Ptr) COL_CHAR_NEXT(chunks[0].format, data[0], *info->c1Ptr);
	if (info->c2Ptr) *info->c2Ptr = COL_CHAR_INVALID;
	return 1;
    } else if (!chunks[0].data) {
	/*
	 * Chunk 2 is longer.
	 */

	ASSERT(data[1]);
	if (info->posPtr) *info->posPtr = index;
	if (info->c1Ptr) *info->c1Ptr = COL_CHAR_INVALID;
	if (info->c2Ptr) COL_CHAR_NEXT(chunks[1].format, data[1], *info->c2Ptr);
	return -1;
    } else if (data[0] == data[1]) {
	/*
	 * Chunks are identical.
	 */

	return 0;
    }

    /*
     * Compare char by char.
     */

    ASSERT(data[0]);
    ASSERT(data[1]);
    for (i = 0; i < length; i++) {
	COL_CHAR_NEXT(chunks[0].format, data[0], c1);
	COL_CHAR_NEXT(chunks[1].format, data[1], c2);
	if (c1 != c2) {
	    if (info->posPtr) *info->posPtr = index+i;
	    if (info->c1Ptr) *info->c1Ptr = c1;
	    if (info->c2Ptr) *info->c2Ptr = c2;
	    return (c1 > c2 ? 1 : -1);
	}
    }

    /*
     * Chunks were equal.
     */

    return 0;
}

/*---------------------------------------------------------------------------
 * Function: Col_CompareRopesL
 *
 *	Compare two ropes and find the first differing characters if any. This
 *	is the rope counterpart to strncmp with extra features.
 *
 * Arguments:
 *	rope1, rope2	- Ropes to compare.
 *	start		- Starting index.
 *	max		- Maximum number of characters to compare.
 *
 * Results:
 *	Returns an integral value indicating the relationship between the 
 *	ropes:
 *	    - A zero value indicates that both strings are equal;
 *	    - A value greater than zero indicates that the first character that 
 *	      does not match has a greater value in rope1 than in rope2, or that
 *	      rope1 is longer than rope2; 
 *	    - A value less than zero indicates the opposite.
 *
 *	Additionally, if ropes differ:
 *
 *	posPtr		- If non-NULL, position of the first differing character
 *			  upon return.
 *	c1Ptr, c2Ptr	- If non-NULL, respective codepoints of differing 
 *			  characters upon return.
 *
 * See also:
 *	<Col_TraverseRopeChunksN>
 *---------------------------------------------------------------------------*/

int
Col_CompareRopesL(
    Col_Word rope1, 
    Col_Word rope2,
    size_t start, 
    size_t max, 
    size_t *posPtr, 
    Col_Char *c1Ptr, 
    Col_Char *c2Ptr)
{
    Col_Word ropes[2];
    CompareChunksInfo info;

    if (WORD_TYPE(rope1) == WORD_TYPE_WRAP) {
	ropes[0] = WORD_WRAP_SOURCE(rope1);
    } else {
	ropes[0] = rope1;
    }
    if (WORD_TYPE(rope2) == WORD_TYPE_WRAP) {
	ropes[1] = WORD_WRAP_SOURCE(rope2);
    } else {
	ropes[1] = rope2;
    }
    if (ropes[0] == ropes[1]) {
	/*
	 * Ropes are equal, no need to traverse.
	 */

	return 0;
    }
    if (Col_RopeLength(rope1) == 0 && Col_RopeLength(rope2) == 0) {
	/*
	 * Return 0 here, as Col_TraverseRopeChunksN would return -1.
	 */

	return 0;
    }

    info.posPtr = posPtr;
    info.c1Ptr = c1Ptr;
    info.c2Ptr = c2Ptr;
    return Col_TraverseRopeChunksN(2, ropes, start, max, CompareChunksProc, 
	    &info, NULL);
}


/****************************************************************************
 * Group: Rope Operations
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Algorithm: Rope Tree Balancing
 *
 *	Large ropes are built by concatenating several subropes, forming a
 *	balanced binary tree. A balanced tree is one where the depth of the
 *	left and right arms do not differ by more than one level.
 *
 *	Rope trees are self-balanced by construction: when two ropes are 
 *	concatenated, if their respective depths differ by more than 1, then 
 *	the tree is recursively rebalanced by splitting and merging subarms. 
 *	There are four major cases, two if we consider symmetry:
 *
 *	- Deepest subtree is an outer branch (i.e. the left resp. right child of
 *	  the left resp. right arm). In this case the tree is rotated: the
 *	  opposite child is moved and concatenated with the opposite arm.
 *	  For example, with left being deepest:
 *
 * (start diagram)
 *  Before:
 *            concat = (left1 + left2) + right
 *           /      \
 *         left    right
 *        /    \
 *     left1  left2
 *    /     \
 *   ?       ?
 *
 *
 * After:
 *           concat = left1 + (left2 + right)
 *          /      \
 *     left1        concat
 *    /     \      /      \
 *   ?       ?   left2   right
 * (end)
 *
 *	- Deepest subtree is an inner branch (i.e. the right resp. left child of
 *	  the left resp. right arm). In this case the subtree is split 
 *	  between both arms. For example, with left being deepest:
 *
 * (start diagram)
 *  Before:
 *          concat = (left1 + (left21+left22)) + right
 *         /      \
 *       left    right
 *      /    \
 *   left1  left2
 *         /     \
 *      left21  left22
 *
 *
 * After:
 *              concat = (left1 + left21) + (left22 + right)
 *             /      \
 *       concat        concat
 *      /      \      /      \
 *   left1  left21  left22  right
 * (end)
 *
 * See also:
 *	<Col_ConcatRopes>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Constant: MAX_SHORT_LEAF_SIZE
 *
 *	Maximum number of bytes a short leaf can take. This constant controls
 *	the creation of short leaves during subrope/concatenation. Ropes built
 *	this way normally use subrope and concat nodes, but to avoid 
 *	fragmentation, multiple short ropes are flattened into a single flat 
 *	string.
 *---------------------------------------------------------------------------*/

#define MAX_SHORT_LEAF_SIZE	(3*CELL_SIZE)

/*---------------------------------------------------------------------------
 * Internal Typedef: MergeChunksInfo
 *
 *	Structure used to collect data during the traversal of ropes when merged
 *	into one string.
 *
 * Fields:
 *	format		- Character format for the below buffer.
 *	data		- Buffer storing the flattened data.
 *	length		- Character length so far.
 *	byteLength	- Byte length so far.
 *
 * See also:
 *	<MAX_SHORT_LEAF_SIZE>, <MergeChunksProc>
 *---------------------------------------------------------------------------*/

typedef struct MergeChunksInfo {
    Col_StringFormat format;
    char data[MAX_SHORT_LEAF_SIZE];
    size_t length, byteLength;
} MergeChunksInfo;

/*---------------------------------------------------------------------------
 * Internal Function: MergeChunksProc
 *
 *	Rope traversal procedure used to concatenate all portions of ropes into
 *	one to fit within one leaf rope. Follows <Col_RopeChunksTraverseProc> 
 *	signature.
 *
 * Arguments:
 *	index		- Rope-relative index where chunks begin.
 *	length		- Length of chunks.
 *	number		- Number of chunks. Always 1.
 *	chunks		- Array of chunks. First chunk never NULL.
 *	clientData	- Points to <MergeChunksInfo>.
 *
 * Result:
 *	0 if successful, else this means that the data wouldn't fit into a 
 *	short leaf rope.
 *
 * Side effects:
 *	Copy the string data into the associated <MergeChunksInfo> fields.
 *
 * See also:
 *	<MAX_SHORT_LEAF_SIZE>, <MergeChunksInfo>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static int 
MergeChunksProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_RopeChunk *chunks,
    Col_ClientData clientData) 
{
    MergeChunksInfo *info = (MergeChunksInfo *) clientData;

    ASSERT(number == 1);
    if (info->length == 0) {
	info->format = chunks->format;
    }
    if (info->format == COL_UTF8 || info->format == COL_UTF16) {
	if (chunks->format != info->format) {
	    /* 
	     * Don't merge UTF-8/16 with other formats. 
	     */

	    return -1;
	}
	if (info->byteLength + chunks->byteLength
		> MAX_SHORT_LEAF_SIZE - WORD_UTFSTR_HEADER_SIZE) {
	    /* 
	     * Data won't fit. 
	     */

	    return -1;
	}

	/* 
	 * Append data. 
	 */

	memcpy(info->data+info->byteLength, chunks->data, chunks->byteLength);
	info->length += length;
	info->byteLength += chunks->byteLength;
	return 0;
    } else {
	size_t index;		/* For upconversions. */

	if (chunks->format == COL_UTF8 || chunks->format == COL_UTF16) {
	    /* 
	     * Don't merge UTF-8/16 with fixed-width formats. 
	     */

	    return -1;
	}

	/* 
	 * Convert and append data if needed. Conversion of existing data is
	 * done in-place, so iterate backwards. 
	 */

	if (CHAR_WIDTH(info->format) >= CHAR_WIDTH(chunks->format)) {
	    /*
	     * Appended data will be upconverted.
	     */

	    if (info->byteLength + (length * CHAR_WIDTH(info->format))
		    > MAX_SHORT_LEAF_SIZE - WORD_UCSSTR_HEADER_SIZE) {
		/* 
		    * Data won't fit. 
		    */

		return -1;
	    }
	} else {
	    /*
	     * Existing data will be upconverted.
	     */

	    if ((info->length + length) * CHAR_WIDTH(chunks->format)
		    > MAX_SHORT_LEAF_SIZE - WORD_UCSSTR_HEADER_SIZE) {
		/* 
		 * Data won't fit. 
		 */

		return -1;
	    }
	}
	switch (info->format) {
	    case COL_UCS1:
		switch (chunks->format) {
		    case COL_UCS2:
			/* 
			 * Upconvert existing data to UCS-2. 
			 */

			for (index = info->length-1; index != -1; index--) {
			    ((Col_Char2 *) info->data)[index]
				    = info->data[index];
			}
			info->byteLength = info->length*sizeof(Col_Char2);
			info->format = COL_UCS2;
			break;

		    case COL_UCS4:
			/* 
			 * Upconvert existing data to UCS-4. 
			 */

			for (index = info->length-1; index != -1; index--) {
			    ((Col_Char4 *) info->data)[index] 
				    = info->data[index];
			}
			info->byteLength = info->length * sizeof(Col_Char4);
			info->format = COL_UCS4;
			break;
		}
		break;

	    case COL_UCS2:
		switch (chunks->format) {
		    case COL_UCS1:
			/* 
			 * Append data and return, upconverting in the process.
			 */

			for (index = 0; index < length; index++) {
			    ((Col_Char2 *) (info->data+info->byteLength))[index] 
				    = ((Col_Char1 *) chunks->data)[index];
			}
			info->length += length;
			info->byteLength += length*sizeof(Col_Char2);
			return 0;

		    case COL_UCS4:
			/* 
			 Upconvert existing data to UCS-4. 
			 */

			for (index = info->length-1; index != -1; index--) {
			    ((Col_Char4 *) info->data)[index] 
				    = ((Col_Char2 *) info->data)[index];
			}
			info->byteLength = info->length * sizeof(Col_Char4);
			info->format = COL_UCS4;
			break;
		}
		break;

	    case COL_UCS4:
		switch (chunks->format) {
		    case COL_UCS1:
			/* 
			 * Append data and return, upconverting in the process.
			 */

			for (index = 0; index < length; index++) {
			    ((Col_Char4 *) (info->data+info->byteLength))[index] 
				    = ((Col_Char1 *) chunks->data)[index];
			}
			info->length += length;
			info->byteLength += length*sizeof(Col_Char4);
			return 0;

		    case COL_UCS2:
			/* 
			 * Append data and return, upconverting in the process.
			 */

			for (index = 0; index < length; index++) {
			    ((Col_Char4 *) (info->data+info->byteLength))[index]
				    = ((Col_Char2 *) chunks->data)[index];
			}
			info->length += length;
			info->byteLength += length*sizeof(Col_Char4);
			return 0;
		}
		break;
	}

	ASSERT(info->format == chunks->format);

	if (info->byteLength + chunks->byteLength
		> MAX_SHORT_LEAF_SIZE - WORD_UCSSTR_HEADER_SIZE) {
	    /* 
	     * Data won't fit. 
	     */

	    return -1;
	}

	/* 
	 * Append data. 
	 */

	memcpy(info->data+info->byteLength, chunks->data, chunks->byteLength);
	info->length += length;
	info->byteLength += chunks->byteLength;
	return 0;
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: GetDepth
 *
 *	Get the depth of the rope.
 *
 * Argument:
 *	node	- Rope node to get depth from.
 *
 * Result:
 *	Depth of rope.
 *
 * See also:
 *	<Col_ConcatRopes>
 *---------------------------------------------------------------------------*/

static unsigned char
GetDepth(
    Col_Word rope)
{
    for (;;) {
	switch (WORD_TYPE(rope)) {
	    case WORD_TYPE_WRAP:
		rope = WORD_WRAP_SOURCE(rope);
		break;

 	   case WORD_TYPE_SUBROPE:
		return WORD_SUBROPE_DEPTH(rope);

	    case WORD_TYPE_CONCATROPE:
		return WORD_CONCATROPE_DEPTH(rope);

	    /* WORD_TYPE_UNKNOWN */

	    default:
		return 0;
	}
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: GetArms
 *
 *	Get the left and right arms of a rope, i.e. a concat or one of its 
 *	subropes.
 *
 * Argument:
 *	rope		- Rope to extract arms from. Either a subrope or
 *			  concat rope.
 *
 * Results:
 *	leftPtr		- Left arm.
 *	rightPtr	- Right arm.
 *
 * Side effects:
 *	May create new words.
 *
 * See also:
 *	<Col_ConcatRopes>
 *---------------------------------------------------------------------------*/

static void 
GetArms(
    Col_Word rope,
    Col_Word * leftPtr,
    Col_Word * rightPtr)
{
    if (WORD_TYPE(rope) == WORD_TYPE_SUBROPE) {
	/* Create one subrope for each concat arm. */
	Col_Word source = WORD_SUBROPE_SOURCE(rope);
	size_t leftLength;

	ASSERT(WORD_SUBROPE_DEPTH(rope) >= 1);
	ASSERT(WORD_TYPE(source) == WORD_TYPE_CONCATROPE);
	ASSERT(WORD_CONCATROPE_DEPTH(source) == WORD_SUBROPE_DEPTH(rope));

	leftLength = WORD_CONCATROPE_LEFT_LENGTH(source);
	if (leftLength == 0) {
	    leftLength = Col_RopeLength(WORD_CONCATROPE_LEFT(source));
	}
	*leftPtr = Col_Subrope(WORD_CONCATROPE_LEFT(source), 
		WORD_SUBROPE_FIRST(rope), leftLength-1);
	*rightPtr = Col_Subrope(WORD_CONCATROPE_RIGHT(source), 0, 
		WORD_SUBROPE_LAST(rope)-leftLength);
    } else {
	ASSERT(WORD_CONCATROPE_DEPTH(rope) >= 1);
	ASSERT(WORD_TYPE(rope) == WORD_TYPE_CONCATROPE);

	*leftPtr  = WORD_CONCATROPE_LEFT(rope);
	*rightPtr = WORD_CONCATROPE_RIGHT(rope);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_Subrope
 *
 *	Create a new rope which is a subrope of another.
 *
 *	We try to minimize the overhead as much as possible, such as: 
 *	 - identity.
 *       - create leaf ropes for small subropes.
 *	 - subropes of subropes point to original data.
 *	 - subropes of concats point to the deepest superset subrope.
 *
 * Arguments:
 *	rope		- The rope to extract the subrope from.
 *	first, last	- Range of subrope.
 *
 * Result:
 *	When first is past the end of the rope, or last is before first, an
 *	empty rope.
 *	Else, a rope representing the subrope.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

Col_Word 
Col_Subrope(
    Col_Word rope,
    size_t first,
    size_t last)
{
    Col_Word subrope;		/* Resulting rope in the general case. */
    size_t ropeLength;		/* Length of source rope. */
    unsigned char depth=0;	/* Depth of source rope. */
    size_t length;		/* Length of resulting subrope. */

    /* 
     * Quick cases. 
     */

    if (last < first) {
	/*
	 * Invalid range.
	 */

	return WORD_SMALLSTR_EMPTY;
    }

    ropeLength = Col_RopeLength(rope);
    if (first >= ropeLength) {
	/*
	 * Beginning is past the end of the string.
	 */

	return WORD_SMALLSTR_EMPTY;
    }

    if (last >= ropeLength) {
	/* 
	 * Truncate overlong subropes. (Note: at this point ropeLength > 1)
	 */

	last = ropeLength-1;
    }

    length = last-first+1;

    if (first == 0 && length == ropeLength) {
	/* 
	 * Identity. 
	 */

	return rope;
    }

    /* 
     * Type-specific quick cases. 
     */

    switch (WORD_TYPE(rope)) {
	/*
	 * Immediate types.
	 */

	case WORD_TYPE_CHAR:
	    /*
	     * Identity, already done.
	     */

	    ASSERT(0);
	    return rope;

	case WORD_TYPE_SMALLSTR:
	    return Col_NewRope(COL_UCS1, WORD_SMALLSTR_DATA(rope)+first, 
		    length);

	/* 
	 * Handle fixed width leaf ropes directly; this will be faster than the 
	 * generic case below. 
	 */

	case WORD_TYPE_UCSSTR: {
	    Col_StringFormat format 
		    = (Col_StringFormat) WORD_UCSSTR_FORMAT(rope);
	    if (length * CHAR_WIDTH(format) <= MAX_SHORT_LEAF_SIZE 
		    - WORD_UCSSTR_HEADER_SIZE) {
		return Col_NewRope(format, WORD_UCSSTR_DATA(rope)
			+ first * CHAR_WIDTH(format), 
			length * CHAR_WIDTH(format));
	    }
	    break;
	}

	case WORD_TYPE_SUBROPE:
	    /* 
	     * Point to original source. 
	     */

	    return Col_Subrope(WORD_SUBROPE_SOURCE(rope), 
		    WORD_SUBROPE_FIRST(rope)+first, 
		    WORD_SUBROPE_FIRST(rope)+last);

	case WORD_TYPE_CONCATROPE: {
	    /* 
	     * Try to find the deepest superset of the subrope. 
	     */

	    size_t leftLength = WORD_CONCATROPE_LEFT_LENGTH(rope);
	    if (leftLength == 0) {
		leftLength = Col_RopeLength(WORD_CONCATROPE_LEFT(rope));
	    }
	    if (last < leftLength) {
		/* 
		 * Left arm is a superset of subrope. 
		 */

		return Col_Subrope(WORD_CONCATROPE_LEFT(rope), first, last);
	    } else if (first >= leftLength) {
		/*
		 * Right arm is a superset of subrope. 
		 */

		return Col_Subrope(WORD_CONCATROPE_RIGHT(rope), first-leftLength, 
			last-leftLength);
	    }
	    depth = WORD_CONCATROPE_DEPTH(rope);
	    break;
	}

	case WORD_TYPE_CUSTOM: {
	    Col_CustomRopeType *typeInfo 
		    = (Col_CustomRopeType *) WORD_TYPEINFO(rope);
	    ASSERT(typeInfo->type.type == COL_ROPE);
	    if (typeInfo->subropeProc) {
		if ((subrope = typeInfo->subropeProc(rope, first, last)) 
			!= WORD_NIL) {
		    return subrope;
		}
	    }
	    break;
	}

	/* WORD_TYPE_UNKNOWN */
    }

    /* 
     * Try to build a short subrope if possible.
     *
     * Make the test on char length, the traversal will check for byte length. 
     */

    if (length <= MAX_SHORT_LEAF_SIZE) {
	MergeChunksInfo info; 
	info.length = 0;
	info.byteLength = 0;

	if (Col_TraverseRopeChunks(rope, first, length, 0, MergeChunksProc, 
		&info, NULL) == 0) {
	    /* 
	     * Zero result means data fits into one short leaf. 
	     */

	    return Col_NewRope(info.format, info.data, info.byteLength);
	}
    }

    /* 
     * General case: build a subrope node.
     */

    subrope = (Col_Word) AllocCells(1);
    WORD_SUBROPE_INIT(subrope, depth, rope, first, last);

    return subrope;
}

/*---------------------------------------------------------------------------
 * Function: Col_ConcatRopes
 *
 *	Concatenate ropes.
 *
 *	Concatenation forms self-balanced binary trees. Each node has a depth 
 *	level. Concat nodes have a depth > 1. Subrope nodes have a depth equal
 *	to that of their source node. Other nodes have a depth of 0. 
 *
 * Arguments:
 *	left	- Left part.
 *	right	- Right part.
 *
 * Result:
 *	If the resulting rope would exceed the maximum length, nil. Else, a 
 *	rope representing the concatenation of both ropes.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

Col_Word 
Col_ConcatRopes(
    Col_Word left,
    Col_Word right)
{
    Col_Word concatRope;	/* Resulting rope in the general case. */
    unsigned char leftDepth, rightDepth; 
				/* Respective depths of left and right lists. */
    size_t leftLength, rightLength;
				/* Respective lengths. */

    leftLength = Col_RopeLength(left);
    rightLength = Col_RopeLength(right);
    if (SIZE_MAX-leftLength < rightLength) {
	/*
	 * Concatenated rope would be too long.
	 */

	Col_Error(COL_ERROR, 
		"Combined length %u+%u exceeds the maximum allowed value for ropes (%u)", 
		leftLength, rightLength, SIZE_MAX);
	return WORD_NIL;
    }

    if (leftLength == 0) {
	/* 
	 * Concat is a no-op on right. 
	 */

	return right;
    } else if (rightLength == 0) {
	/* 
	 * Concat is a no-op on left.
	 */

	return left;
    }

    switch (WORD_TYPE(left)) {
	case WORD_TYPE_SUBROPE:
	    if (WORD_TYPE(right) == WORD_TYPE_SUBROPE
		    && WORD_SUBROPE_SOURCE(left) == WORD_SUBROPE_SOURCE(right) 
		    && WORD_SUBROPE_LAST(left)+1 == WORD_SUBROPE_FIRST(right)) {
		/* 
		 * Merge if left and right are adjacent subropes. This allows
		 * for fast consecutive insertions/removals at a given index.
		 */

		return Col_Subrope(WORD_SUBROPE_SOURCE(left), 
			WORD_SUBROPE_FIRST(left), WORD_SUBROPE_LAST(right));
	    }
	    break;

	case WORD_TYPE_CUSTOM: {
	    Col_CustomRopeType *typeInfo 
		    = (Col_CustomRopeType *) WORD_TYPEINFO(left);
	    ASSERT(typeInfo->type.type == COL_ROPE);
	    if (typeInfo->concatProc) {
		if ((concatRope = typeInfo->concatProc(left, right)) 
			!= WORD_NIL) {
		    return concatRope;
		}
	    }
	    break;
	}

	/* WORD_TYPE_UNKNOWN */
    }
    switch (WORD_TYPE(right)) {
	case WORD_TYPE_CUSTOM: {
	    Col_CustomRopeType *typeInfo 
		    = (Col_CustomRopeType *) WORD_TYPEINFO(right);
	    ASSERT(typeInfo->type.type == COL_ROPE);
	    if (typeInfo->concatProc) {
		if ((concatRope = typeInfo->concatProc(left, right)) 
			!= WORD_NIL) {
		    return concatRope;
		}
	    }
	    break;
	}

	/* WORD_TYPE_UNKNOWN */
    }

    /* 
     * Try to build a short string if possible.
     *
     * Make the test on char length, the traversal will check for byte length. 
     */

    if (leftLength + rightLength <= MAX_SHORT_LEAF_SIZE) {
	MergeChunksInfo info;

	info.length = 0;
	info.byteLength = 0;

	if (Col_TraverseRopeChunks(left, 0, leftLength, 0, MergeChunksProc, 
		&info, NULL) == 0 && Col_TraverseRopeChunks(right, 0, 
		rightLength, 0, MergeChunksProc, &info, NULL) == 0) {
	    /* 
	     * Zero result means data fits into one short leaf. 
	     */

	    ASSERT(info.length == leftLength+rightLength);
	    return Col_NewRope(info.format, info.data, info.byteLength);
	}
    }

    /* 
     * Assume the input ropes are well balanced by construction. Build a rope 
     * that is balanced as well, i.e. where left and right depths don't differ 
     * by more that 1 level.

    /* Note that a subrope with depth >= 1 always points to a concat, as by 
     * construction it cannot point to another subrope (see Col_Subrope). 
     */

    leftDepth = GetDepth(left);
    rightDepth = GetDepth(right);
    if (leftDepth > rightDepth+1) {
	/* 
	 * Left is deeper by more than 1 level, rebalance.
	 */

	Col_Word left1, left2;

	ASSERT(leftDepth >= 2);

	GetArms(left, &left1, &left2);
	if (GetDepth(left1) < GetDepth(left2)) {
	    /* 
	     * Left2 is deeper, split it between both arms. 
	     */

	    Col_Word left21, left22;
	    GetArms(left2, &left21, &left22);
	    return Col_ConcatRopes(Col_ConcatRopes(left1, left21), 
		    Col_ConcatRopes(left22, right));
	} else {
	    /* 
	     * Left1 is deeper or at the same level, rotate to right.
	     */

	    return Col_ConcatRopes(left1, Col_ConcatRopes(left2, right));
	}
    } else if (leftDepth+1 < rightDepth) {
	/* 
	 * Right is deeper by more than 1 level, rebalance. 
	 */

	Col_Word right1, right2;

	ASSERT(rightDepth >= 2);

	GetArms(right, &right1, &right2);
	if (GetDepth(right1) > GetDepth(right2)) {
	    /* 
	     * Right1 is deeper, split it between both arms. 
	     */

	    Col_Word right11, right12;
	    GetArms(right1, &right11, &right12);
	    return Col_ConcatRopes(Col_ConcatRopes(left, right11), 
		    Col_ConcatRopes(right12, right2));
	} else {
	    /* 
	     * Right2 is deeper or at the same level, rotate to left.
	     */

	    return Col_ConcatRopes(Col_ConcatRopes(left, right1), right2);
	}
    }
    
    /* 
     * General case: build a concat node.
     */

    concatRope = (Col_Word) AllocCells(1);
    WORD_CONCATROPE_INIT(concatRope, (leftDepth>rightDepth?leftDepth:rightDepth)
	    + 1, leftLength + rightLength, leftLength, left, right);

    return concatRope;
}

/*---------------------------------------------------------------------------
 * Function: Col_ConcatRopesA
 *
 *	Concatenate several ropes given in an array. Concatenation is done 
 *	recursively, by halving the array until it contains one or two elements,
 *	at this point we respectively return the element or use 
 *	<Col_ConcatRopes>.
 *
 * Arguments:
 *	number	- Number of ropes in below array.
 *	ropes	- Ropes to concatenate in order.
 *
 * Result:
 *	If the array is empty or the resulting rope would exceed the maximum 
 *	length, nil. Else, a rope representing the concatenation of all ropes.
 *
 * Side effects:
 *	See <Col_ConcatRopes>.
 *---------------------------------------------------------------------------*/

Col_Word 
Col_ConcatRopesA(
    size_t number,
    const Col_Word * ropes)
{
    size_t half;

    /* 
     * Quick cases.
     */

    if (number == 0) {return WORD_NIL;}
    if (number == 1) {return ropes[0];}
    if (number == 2) {return Col_ConcatRopes(ropes[0], ropes[1]);}

    /* 
     * Split array and concatenate both halves. This should result in a well 
     * balanced tree. 
     */

    half = number/2;
    return Col_ConcatRopes(Col_ConcatRopesA(half, ropes), 
	    Col_ConcatRopesA(number-half, ropes+half));
}

/*---------------------------------------------------------------------------
 * Function: Col_ConcatRopesNV
 *
 *	Concatenate ropes given as arguments. The argument list is first 
 *	copied into a stack-allocated array then passed to <Col_ConcatRopesA>.
 *
 * Arguments:
 *	number	- Number of arguments.
 *	...	- Remaining arguments, i.e. ropes to concatenate in order.
 *
 * Result:
 *	If the number of arguments is zero or the resulting rope would exceed 
 *	the maximum length, nil. Else, a rope representing the concatenation of
 *	all ropes.
 *
 * Side effects:
 *	See <Col_ConcatRopesA>.
 *---------------------------------------------------------------------------*/

Col_Word 
Col_ConcatRopesNV(
    size_t number,
    ...)
{
    size_t i;
    va_list args;
    Col_Word *ropes;
    
    /* 
     * Convert vararg list to array. Use alloca since a vararg list is 
     * typically small. 
     */

    ropes = alloca(number*sizeof(Col_Word));
    va_start(args, number);
    for (i=0; i < number; i++) {
	ropes[i] = va_arg(args, Col_Word);
    }
    return Col_ConcatRopesA(number, ropes);
}

/*---------------------------------------------------------------------------
 * Function: Col_RepeatRope
 *
 *	Create a rope formed by the repetition of a source rope.
 *
 *	This method is based on recursive concatenations of the rope 
 *	following the bit pattern of the count factor. Doubling a rope simply 
 *	consists of a concat with itself. In the end the resulting tree is 
 *	very compact, and only a minimal number of extraneous cells are 
 *	allocated during the balancing process (and will be eventually 
 *	collected).
 *
 * Arguments:
 *	rope	- The rope to repeat.
 *	count	- Repetition factor.
 *
 * Result:
 *	If count is zero or the resulting rope would exceed the maximum length,
 *	nil. Else, a rope representing the repetition of the source rope.
 *---------------------------------------------------------------------------*/

Col_Word 
Col_RepeatRope(
    Col_Word rope,
    size_t count)
{
    /* Quick cases. */
    if (count == 0) {return WORD_NIL;}
    if (count == 1) {return rope;}
    if (count == 2) {return Col_ConcatRopes(rope, rope);}

    if (count & 1) {
	/* Odd.*/
	return Col_ConcatRopes(rope, Col_RepeatRope(rope, count-1));
    } else {
	/* Even. */
	return Col_RepeatRope(Col_ConcatRopes(rope, rope), count>>1);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeInsert
 *
 *	Insert a rope into another one, just before the given insertion point. 
 *	As target rope is immutable, this results in a new rope.
 *
 *	Insertion past the end of the rope results in a concatenation.
 *
 *
 * Arguments:
 *	into	- Where to insert.
 *	index	- Index of insertion point.
 *	rope	- Rope to insert.
 *
 * Note:
 *	Only perform minimal tests to prevent overflow, basic ops should 
 *	perform further optimizations anyway.
 *
 * Result:
 *	The resulting rope.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

Col_Word 
Col_RopeInsert(
    Col_Word into,
    size_t index,
    Col_Word rope)
{
    size_t length;
 
    if (Col_RopeLength(rope) == 0) {
	/*
	 * No-op.
	 */

	return into;
    }

    if (index == 0) {
	/*
	 * Insert at beginning.
	 */

	return Col_ConcatRopes(rope, into);
    }

    length = Col_RopeLength(into);
    if (index >= length) {
	/*
	 * Insertion past the end of rope is concatenation.
	 */

	return Col_ConcatRopes(into, rope);
    }

    /* 
     * General case. 
     */

    return Col_ConcatRopes(Col_ConcatRopes(
		    Col_Subrope(into, 0, index-1), rope), 
	    Col_Subrope(into, index, length-1));
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeRemove
 *
 *	Remove a range of characters from a rope. As target rope is immutable, 
 *	this results in a new rope.
 *
 * Arguments:
 *	rope		- Rope to remove sequence from.
 *	first, last	- Range of characters to remove.
 *
 * Note:
 *	Only perform minimal tests to prevent overflow, basic ops should 
 *	perform further optimizations anyway.
 *
 * Result:
 *	The resulting rope.
 *
 * Side effects:
 *	May allocate memory cells.
 *---------------------------------------------------------------------------*/

Col_Word 
Col_RopeRemove(
    Col_Word rope,
    size_t first, 
    size_t last)
{
    size_t length;

    if (first > last) {
	/* 
	 * No-op. 
	 */

	return rope;
    }

    length = Col_RopeLength(rope);
    if (length == 0 || first >= length) {
	/*
	 * No-op.
	 */

	return rope;
    } else if (first == 0) {
	/*
	 * Trim begin.
	 */

	return Col_Subrope(rope, last+1, length-1);
    } else if (last >= length-1) {
	/* 
	 * Trim end. 
	 */

	return Col_Subrope(rope, 0, first-1);
    }

    /* 
     * General case. 
     */

    return Col_ConcatRopes(Col_Subrope(rope, 0, first-1), 
	    Col_Subrope(rope, last+1, length-1));
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeReplace
 *
 *	Replace a range of characters in a rope with another, taking cyclicity
 *	into account. As target rope is immutable, this results in a new rope.
 *
 *	Replacement is a combination of remove and insert.
 *
 * Arguments:
 *	rope		- Original rope.
 *	first, last	- Inclusive range of characters to replace.
 *	with		- Replacement rope.
 *
 * Note:
 *	Only perform minimal tests to prevent overflow, basic ops should 
 *	perform further optimizations anyway.
 *
 * Result:
 *	The resulting rope.
 *
 * Side effects:
 *	May allocate memory cells.
 *
 * See also:
 *	<Col_RopeInsert>, <Col_RopeRemove>
 *---------------------------------------------------------------------------*/

Col_Word 
Col_RopeReplace(
    Col_Word rope,
    size_t first, 
    size_t last,
    Col_Word with)
{
    if (first > last) {
	/* 
	 * No-op. 
	 */

	return rope;
    }

    /* 
     * General case. 
     */

    return Col_RopeInsert(Col_RopeRemove(rope, first, last), first, with);
}


/****************************************************************************
 * Group: Rope Traversal
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Typedef: RopeChunkTraverseInfo
 *
 *	Structure used during recursive rope chunk traversal. This avoids
 *	recursive procedure calls thanks to a pre-allocated backtracking 
 *	structure: since the algorithms only recurse on concat nodes and since
 *	we know their depth, we can allocate the needed space once for all.
 *
 * Fields:
 *	backtracks	- Pre-allocated backtracking structure.
 *	rope		- Currently traversed rope.
 *	c		- Character used as buffer for single char ropes (e.g.
 *			  immediate chars of char-based custom ropes).
 *	start		- Index of first character traversed in rope.
 *	max		- Max number of characters traversed in rope.
 *	maxDepth	- Depth of toplevel concat node.
 *	prevDepth	- Depth of next concat node for backtracking.
 *
 * Backtrack stack element fields:
 *	backtracks.prevDepth	- Depth of next concat node for backtracking.
 *	backtracks.rope		- Rope.
 *	backtracks.max		- Max number of characters traversed in rope.
 *
 * See also:
 *	<Col_TraverseRopeChunksN>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

typedef struct RopeChunkTraverseInfo {
    struct {
	int prevDepth;
	Col_Word rope;
	size_t max;
    } *backtracks;
    Col_Word rope;
    Col_Char4 c;
    size_t start, max;
    int maxDepth, prevDepth;
} RopeChunkTraverseInfo;

/*---------------------------------------------------------------------------
 * Internal Function: GetChunk
 *
 *	Get chunk from given traversal info.
 *
 * Note: 
 *	This doesn't compute the chunk bytelength as it depends on the
 *	algorithm (for example, Col_TraverseRopeChunksN truncates chunks to the
 *	shortest one in the group of traversed ropes).
 *
 * Arguments:
 *	info	- Traversal info.
 *	chunk	- Chunk info for leaf.
 *	reverse	- Whether to traverse in reverse order.
 *
 * See also:
 *	<RopeChunkTraverseInfo>, <Col_TraverseRopeChunksN>, 
 *	<Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static void
GetChunk(
    RopeChunkTraverseInfo *info,
    Col_RopeChunk *chunk,
    int reverse)
{
    int type;

    for (;;) {
	/*
	 * Descend into structure until we find a suitable leaf.
	 */

	type = WORD_TYPE(info->rope);
	switch (type) {
	    case WORD_TYPE_WRAP:
		/* 
		 * Recurse on source.
		 */

		info->rope = WORD_WRAP_SOURCE(info->rope);
		continue;

	    case WORD_TYPE_SUBROPE:
		/* 
		 * Subrope: recurse on source rope.
		 */

		info->start += WORD_SUBROPE_FIRST(info->rope);
		info->rope = WORD_SUBROPE_SOURCE(info->rope);
		continue;
        	    
	    case WORD_TYPE_CONCATROPE: {
		/* 
		 * Concat: descend into covered arms.
		 */

		int depth;
		size_t leftLength = WORD_CONCATROPE_LEFT_LENGTH(info->rope);
		if (leftLength == 0) {
		    leftLength = Col_RopeLength(
			    WORD_CONCATROPE_LEFT(info->rope));
		}
		if (info->start + (reverse ? 0 : info->max-1) < leftLength) {
		    /* 
		     * Recurse on left arm only. 
		     */

		    info->rope = WORD_CONCATROPE_LEFT(info->rope);
		    continue;
		}
		if (info->start - (reverse ? info->max-1 : 0) >= leftLength) {
		    /* 
		     * Recurse on right arm only. 
		     */

		    info->start -= leftLength;
		    info->rope = WORD_CONCATROPE_RIGHT(info->rope);
		    continue;
		} 

		/*
		 * Push right (resp. left for reverse) onto stack and recurse on
		 * left (resp. right).
		 */

		ASSERT(info->backtracks);
		depth = WORD_CONCATROPE_DEPTH(info->rope);
		ASSERT(depth <= info->maxDepth);
		info->backtracks[depth-1].prevDepth = info->prevDepth;
		info->prevDepth = depth;
		if (reverse) {
		    ASSERT(info->start >= leftLength);
		    info->backtracks[depth-1].rope 
			    = WORD_CONCATROPE_LEFT(info->rope);
		    info->backtracks[depth-1].max = info->max
			    - (info->start-leftLength+1);
		    info->start -= leftLength;
		    info->max = info->start+1;
		    info->rope = WORD_CONCATROPE_RIGHT(info->rope);
		} else {
		    ASSERT(info->start < leftLength);
		    info->backtracks[depth-1].rope 
			    = WORD_CONCATROPE_RIGHT(info->rope);
		    info->backtracks[depth-1].max = info->max
			    - (leftLength-info->start);
		    info->max = leftLength-info->start;
		    info->rope = WORD_CONCATROPE_LEFT(info->rope);
		}
		continue;

	    /* WORD_TYPE_UNKNOWN */
	    }
	}
	break;
    }

    /*
     * Get leaf data.
     */

    ASSERT(info->start + (reverse ? info->max-1 : 0) >= 0);
    ASSERT(info->start + (reverse ? 0 : info->max-1) < Col_RopeLength(info->rope));
    switch (type) {
	case WORD_TYPE_CHAR:
	    ASSERT(info->start == 0);
	    ASSERT(info->max == 1);
	    chunk->format = (Col_StringFormat) WORD_CHAR_WIDTH(info->rope);
	    info->c = WORD_CHAR_GET(info->rope);
	    chunk->data = &info->c;
	    break;

	case WORD_TYPE_SMALLSTR:
	    chunk->format = COL_UCS1;
	    chunk->data = WORD_SMALLSTR_DATA(info->rope) + info->start 
		    - (reverse ? info->max-1 : 0);
	    break;

	case WORD_TYPE_UCSSTR: {
	    /* 
	     * Fixed-width flat strings: traverse range of chars.
	     */

	    chunk->format = (Col_StringFormat) WORD_UCSSTR_FORMAT(info->rope);
	    chunk->data = WORD_UCSSTR_DATA(info->rope)
		    + (info->start - (reverse ? info->max-1 : 0))
		    * CHAR_WIDTH(chunk->format);
	    break;
	}
        	    
	case WORD_TYPE_UTFSTR:
	    /*
	     * Variable-width flat string.
	     */

	    chunk->format = (Col_StringFormat) WORD_UTFSTR_FORMAT(info->rope);
	    switch (chunk->format) {
		case COL_UTF8:
		    chunk->data = Col_Utf8Addr(
			    (const Col_Char1 *) WORD_UTFSTR_DATA(info->rope), 
			    info->start - (reverse ? info->max-1 : 0), 
			    WORD_UTFSTR_LENGTH(info->rope), 
			    WORD_UTFSTR_BYTELENGTH(info->rope));
		    break;

		case COL_UTF16:
		    chunk->data = Col_Utf16Addr(
			    (const Col_Char2 *) WORD_UTFSTR_DATA(info->rope), 
			    info->start - (reverse ? info->max-1 : 0), 
			    WORD_UTFSTR_LENGTH(info->rope), 
			    WORD_UTFSTR_BYTELENGTH(info->rope));
		    break;
	    }
	    break;

	case WORD_TYPE_CUSTOM: {
	    Col_CustomRopeType *typeInfo 
		    = (Col_CustomRopeType *) WORD_TYPEINFO(info->rope);
	    ASSERT(typeInfo->type.type == COL_ROPE);
	    if (typeInfo->chunkAtProc) {
		typeInfo->chunkAtProc(info->rope, info->start 
			- (reverse ? info->max-1 : 0), info->max, &info->max, 
			chunk);
	    } else {
		/*
		 * Traverse chars individually.
		 */

		info->max = 1;
		chunk->format = COL_UCS4;
		info->c = typeInfo->charAtProc(info->rope, info->start);
		chunk->data = &info->c;
	    }
	    break;
	}

	/* WORD_TYPE_UNKNOWN */

	default:
	    /* CANTHAPPEN */
	    ASSERT(0);
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: NextChunk
 *
 *	Get next chunk in traversal order.
 *
 * Arguments:
 *	info	- Traversal info.
 *	nb	- Number of characters to skip, must be < info->max.
 *	reverse	- Whether to traverse in reverse order.
 *
 * See also:
 *	<RopeChunkTraverseInfo>, <Col_TraverseRopeChunksN>, 
 *	<Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static void 
NextChunk(
    RopeChunkTraverseInfo *info,
    size_t nb,
    int reverse)
{
    ASSERT(info->max >= nb);
    info->max -= nb;
    if (info->max > 0) {
	/*
	 * Still in leaf, advance.
	 */

	if (reverse) {
	    info->start -= nb;
	} else {
	    info->start += nb;
	}
    } else if (info->prevDepth == INT_MAX) {
	/*
	 * Already at toplevel => end of rope.
	 */

	info->rope = WORD_NIL;
    } else {
	/*
	 * Reached leaf bound, backtracks.
	 */

	ASSERT(info->backtracks);
	info->rope = info->backtracks[info->prevDepth-1].rope;
	info->max = info->backtracks[info->prevDepth-1].max;
	info->start = (reverse ? Col_RopeLength(info->rope)-1 : 0);
	info->prevDepth = info->backtracks[info->prevDepth-1].prevDepth;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_TraverseRopeChunksN
 *
 *	Iterate over the chunks of a number of ropes.
 *
 *	For each traversed chunk, proc is called back with the opaque data as
 *	well as the position within the ropes. If it returns a non-zero result 
 *	then the iteration ends. 
 *
 * Note:
 *	The algorithm is naturally recursive but this implementation avoids
 *	recursive calls thanks to a stack-allocated backtracking structure.
 *
 * Arguments:
 *	number		- Number of ropes to traverse.
 *	ropes		- Ropes to traverse.
 *	start		- Index of first character.
 *	max		- Max number of characters.
 *	proc		- Callback proc called on each chunk.
 *	clientData	- Opaque data passed as is to above proc.
 *
 * Results:
 *	The return value of the last called proc, or -1 if no traversal was
 *	performed. Additionally:
 *
 *	lengthPtr	- If non-NULL, incremented by the total number of 
 *			  characters traversed upon completion.
 *---------------------------------------------------------------------------*/

int 
Col_TraverseRopeChunksN(
    size_t number,
    Col_Word *ropes,
    size_t start,
    size_t max,
    Col_RopeChunksTraverseProc *proc,
    Col_ClientData clientData,
    size_t *lengthPtr)
{
    size_t i;
    RopeChunkTraverseInfo *info;
    Col_RopeChunk *chunks;
    int result;

    info = alloca(sizeof(*info) * number);
    chunks = alloca(sizeof(*chunks) * number);

    for (i=0; i < number; i++) {
	size_t ropeLength = Col_RopeLength(ropes[i]);
	if (start > ropeLength) {
	    /* 
	     * Start is past the end of the rope.
	     */

	    info[i].max = 0;
	} else if (max > ropeLength-start) {
	    /* 
	     * Adjust max to the remaining length. 
	     */

	    info[i].max = ropeLength-start;
	} else {
	    info[i].max = max;
	}
    }

    max = 0;
    for (i=0; i < number; i++) {
	if (max < info[i].max) {
	    max = info[i].max;
	}
    }
    if (max == 0) {
	/*
	 * Nothing to traverse.
	 */

	return -1;
    }

    for (i=0; i < number; i++) {
	ASSERT(info[i].max <= max);
	info[i].rope = (info[i].max ? ropes[i] : WORD_NIL);
	info[i].start = start;
	info[i].maxDepth = GetDepth(ropes[i]);
	info[i].backtracks = (info[i].maxDepth ? 
	    alloca(sizeof(*info[i].backtracks) * info[i].maxDepth) : NULL);
	info[i].prevDepth = INT_MAX;
    }

    for (;;) {
	for (i=0; i < number; i++) {
	    if (!info[i].rope) {
		/*
		 * Past end of rope.
		 */

		chunks[i].data = NULL;
		continue;
	    }

	    GetChunk(info+i, chunks+i, 0);
	}

	/*
	 * Limit chunk lengths to the shortest one.
	 */

	max = SIZE_MAX;
	for (i=0; i < number; i++) {
	    if (info[i].rope && max > info[i].max) {
		max = info[i].max;
	    }
	}

	/*
	 * Compute actual byte lengths.
	 */

	for (i=0; i < number; i++) {
	    if (!info[i].rope) continue;
	    switch (chunks[i].format) {
		case COL_UCS1:
		case COL_UCS2:
		case COL_UCS4:
		    chunks[i].byteLength = max * CHAR_WIDTH(chunks[i].format);
		    break;

		case COL_UTF8:
		    chunks[i].byteLength = (const char *) Col_Utf8Addr(
			    (const Col_Char1 *) chunks[i].data, max, 
			    info[i].max, WORD_UTFSTR_BYTELENGTH(info[i].rope)
			    - ((const char *) chunks[i].data
			    - WORD_UTFSTR_DATA(info[i].rope)))
			    - (const char *) chunks[i].data;
		    break;

		case COL_UTF16:
		    chunks[i].byteLength = (const char *) Col_Utf16Addr(
			    (const Col_Char2 *) chunks[i].data, max, 
			    info[i].max, WORD_UTFSTR_BYTELENGTH(info[i].rope)
			    - ((const char *) chunks[i].data
			    - WORD_UTFSTR_DATA(info[i].rope)))
			    - (const char *) chunks[i].data;
		    break;
	    }
	}

	/*
	 * Call proc on leaves data.
	 */

	if (lengthPtr) *lengthPtr += max;
	result = proc(start, max, number, chunks, clientData);
	start += max;
	if (result != 0) {
	    /*
	     * Stop there.
	     */

	    return result;
	} else {
	    /*
	     * Continue iteration.
	     */

	    int next = 0;
	    for (i=0; i < number; i++) {
		if (info[i].rope) NextChunk(info+i, max, 0);
		if (info[i].rope) next = 1;
	    }
	    if (!next) {
		/*
		 * Reached end of all ropes, stop there.
		 */

		return 0;
	    }
	}
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_TraverseRopeChunks
 *
 *	Iterate over the chunks of a rope.
 *
 *	For each traversed chunk, proc is called back with the opaque data as
 *	well as the position within the rope. If it returns a non-zero result 
 *	then the iteration ends. 
 *
 * Note:
 *	The algorithm is naturally recursive but this implementation avoids
 *	recursive calls thanks to a stack-allocated backtracking structure. 
 *	This procedure is an optimized version of <Col_TraverseRopeChunksN>.
 *
 * Arguments:
 *	rope		- Rope to traverse.
 *	start		- Index of first character.
 *	max		- Max number of characters.
 *	reverse		- Whether to traverse in reverse order.
 *	proc		- Callback proc called on each chunk.
 *	clientData	- Opaque data passed as is to above proc.
 *
 * Results:
 *	The return value of the last called proc, or -1 if no traversal was
 *	performed. Additionally:
 *
 *	lengthPtr	- If non-NULL, incremented by the total number of 
 *			  characters traversed upon completion.
 *---------------------------------------------------------------------------*/

int 
Col_TraverseRopeChunks(
    Col_Word rope,
    size_t start,
    size_t max,
    int reverse,
    Col_RopeChunksTraverseProc *proc,
    Col_ClientData clientData,
    size_t *lengthPtr)
{
    RopeChunkTraverseInfo info;
    Col_RopeChunk chunk;
    int result;
    size_t ropeLength = Col_RopeLength(rope);

    if (ropeLength == 0) {
	/*
	 * Nothing to traverse.
	 */

	return -1;
    }

    if (reverse) {
	if (start > ropeLength) {
	    /* 
	     * Start is past the end of the rope.
	     */

	    start = ropeLength-1;
	}
	if (max > start+1) {
	    /* 
	     * Adjust max to the remaining length. 
	     */

	    max = start+1;
	}
    } else {
	if (start > ropeLength) {
	    /* 
	     * Start is past the end of the rope.
	     */

	    return -1;
	}
	if (max > ropeLength-start) {
	    /* 
	     * Adjust max to the remaining length. 
	     */

	    max = ropeLength-start;
	}
    }

    if (max == 0) {
	/*
	 * Nothing to traverse.
	 */

	return -1;
    }

    info.rope = rope;
    info.start = start;
    info.max = max;
    info.maxDepth = GetDepth(rope);
    info.backtracks = (info.maxDepth ? 
	    alloca(sizeof(*info.backtracks) * info.maxDepth) : NULL);
    info.prevDepth = INT_MAX;

    for (;;) {
	GetChunk(&info, &chunk, reverse);
	max = info.max;

	/*
	 * Compute actual byte length.
	 */

	switch (chunk.format) {
	    case COL_UCS1:
	    case COL_UCS2:
	    case COL_UCS4:
		chunk.byteLength = max * CHAR_WIDTH(chunk.format);
		break;

	    case COL_UTF8:
		chunk.byteLength = (const char *) Col_Utf8Addr(
			(const Col_Char1 *) chunk.data, max, info.max, 
			WORD_UTFSTR_BYTELENGTH(info.rope)
			- ((const char *) chunk.data
			- WORD_UTFSTR_DATA(info.rope)))
			- (const char *) chunk.data;
		break;

	    case COL_UTF16:
		chunk.byteLength = (const char *) Col_Utf16Addr(
			(const Col_Char2 *) chunk.data, max, info.max, 
			WORD_UTFSTR_BYTELENGTH(info.rope)
			- ((const char *) chunk.data
			- WORD_UTFSTR_DATA(info.rope)))
			- (const char *) chunk.data;
		break;
	}

	/*
	 * Call proc on leaf data.
	 */

	if (lengthPtr) *lengthPtr += max;
	if (reverse) {
	    result = proc(start-max+1, max, 1, &chunk, clientData);
	    start -= max;
	} else {
	    result = proc(start, max, 1, &chunk, clientData);
	    start += max;
	}
	if (result != 0) {
	    /*
	     * Stop there.
	     */

	    return result;
	} else {
	    /*
	     * Continue iteration.
	     */

	    NextChunk(&info, max, reverse);
	    if (!info.rope) {
		/*
		 * Reached end of rope, stop there.
		 */

		return 0;
	    }
	}
    }
}


/****************************************************************************
 * Group: Rope Iteration
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Function: IterAtChar
 *
 *	Character access proc for immediate char leaves from iterators. Follows
 *	<ColRopeIterLeafAtProc> signature.
 *
 * Arguments:
 *	leaf	- Leaf node.
 *	index	- Leaf-relative index of character. Always zero.
 *
 * Result:
 *	Character at given index.
 *
 * See also:
 *	<ColRopeIterLeafAtProc>, <ColRopeIterator>, 
 *	<ColRopeIterUpdateTraversalInfo>
 *---------------------------------------------------------------------------*/

static Col_Char IterAtChar(
    Col_Word leaf, 
    size_t index
)
{
    ASSERT(index == 0);
    return WORD_CHAR_GET(leaf);
}

/*---------------------------------------------------------------------------
 * Internal Function: IterAtSmallStr
 *
 *	Character access proc for immediate string leaves from iterators. 
 *	Follows <ColRopeIterLeafAtProc> signature.
 *
 * Arguments:
 *	leaf	- Leaf node.
 *	index	- Leaf-relative index of character.
 *
 * Result:
 *	Character at given index.
 *
 * See also:
 *	<ColRopeIterLeafAtProc>, <ColRopeIterator>, 
 *	<ColRopeIterUpdateTraversalInfo>
 *---------------------------------------------------------------------------*/

static Col_Char IterAtSmallStr(
    Col_Word leaf, 
    size_t index
)
{
    return WORD_SMALLSTR_DATA(leaf)[index];
}

/*---------------------------------------------------------------------------
 * Internal Function: IterAtUcs1
 *
 *	Character access proc for UCS-1 string leaves from iterators. Follows
 *	<ColRopeIterLeafAtProc> signature.
 *
 * Arguments:
 *	leaf	- Leaf node.
 *	index	- Leaf-relative index of character.
 *
 * Result:
 *	Character at given index.
 *
 * See also:
 *	<ColRopeIterLeafAtProc>, <ColRopeIterator>, 
 *	<ColRopeIterUpdateTraversalInfo>
 *---------------------------------------------------------------------------*/

static Col_Char IterAtUcs1(
    Col_Word leaf, 
    size_t index
)
{
    return ((Col_Char1 *) WORD_UCSSTR_DATA(leaf))[index];
}

/*---------------------------------------------------------------------------
 * Internal Function: IterAtUcs2
 *
 *	Character access proc for UCS-2 string leaves from iterators. Follows
 *	<ColRopeIterLeafAtProc> signature.
 *
 * Arguments:
 *	leaf	- Leaf node.
 *	index	- Leaf-relative index of character.
 *
 * Result:
 *	Character at given index.
 *
 * See also:
 *	<ColRopeIterLeafAtProc>, <ColRopeIterator>, 
 *	<ColRopeIterUpdateTraversalInfo>
 *---------------------------------------------------------------------------*/

static Col_Char IterAtUcs2(
    Col_Word leaf, 
    size_t index
)
{
    return ((Col_Char2 *) WORD_UCSSTR_DATA(leaf))[index];
}

/*---------------------------------------------------------------------------
 * Internal Function: IterAtUcs4
 *
 *	Character access proc for UCS-4 string leaves from iterators. Follows
 *	<ColRopeIterLeafAtProc> signature.
 *
 * Arguments:
 *	leaf	- Leaf node.
 *	index	- Leaf-relative index of character.
 *
 * Result:
 *	Character at given index.
 *
 * See also:
 *	<ColRopeIterLeafAtProc>, <ColRopeIterator>, 
 *	<ColRopeIterUpdateTraversalInfo>
 *---------------------------------------------------------------------------*/

static Col_Char IterAtUcs4(
    Col_Word leaf, 
    size_t index
)
{
    return ((Col_Char4 *) WORD_UCSSTR_DATA(leaf))[index];
}

/*---------------------------------------------------------------------------
 * Internal Function: IterAtUtf8
 *
 *	Character access proc for UTF-8 string leaves from iterators. Follows
 *	<ColRopeIterLeafAtProc> signature.
 *
 * Arguments:
 *	leaf	- Leaf node.
 *	index	- Leaf-relative index of character.
 *
 * Result:
 *	Character at given index.
 *
 * See also:
 *	<ColRopeIterLeafAtProc>, <ColRopeIterator>, 
 *	<ColRopeIterUpdateTraversalInfo>
 *---------------------------------------------------------------------------*/

static Col_Char IterAtUtf8(
    Col_Word leaf, 
    size_t index
)
{
    return Col_Utf8Get((const Col_Char1 *) (WORD_UTFSTR_DATA(leaf) + index));
}

/*---------------------------------------------------------------------------
 * Internal Function: IterAtUtf16
 *
 *	Character access proc for UTF-16 string leaves from iterators. Follows
 *	<ColRopeIterLeafAtProc> signature.
 *
 * Arguments:
 *	leaf	- Leaf node.
 *	index	- Leaf-relative index of character.
 *
 * Result:
 *	Character at given index.
 *
 * See also:
 *	<ColRopeIterLeafAtProc>, <ColRopeIterator>, 
 *	<ColRopeIterUpdateTraversalInfo>
 *---------------------------------------------------------------------------*/

static Col_Char IterAtUtf16(
    Col_Word leaf, 
    size_t index
)
{
    return Col_Utf16Get((const Col_Char2 *) (WORD_UTFSTR_DATA(leaf) + index));
}

/*---------------------------------------------------------------------------
 * Internal Constant: MAX_ITERATOR_SUBROPE_DEPTH
 *
 *	Max depth of subropes in iterators.
 *
 * See also:
 *	<ColRopeIterUpdateTraversalInfo>
 *---------------------------------------------------------------------------*/

#define MAX_ITERATOR_SUBROPE_DEPTH	3

/*---------------------------------------------------------------------------
 * Internal Function: ColRopeIterUpdateTraversalInfo
 *
 *	Get the deepest subropes needed to access the current character
 *	designated by the iterator.
 *
 *	Iterators point to the leaf containing the current character. To avoid 
 *	rescanning the whole tree when leaving this leaf, it also stores a
 *	higher level subrope containing this leaf, so that traversing all this
 *	subrope's children is fast. The subrope is the leaf's highest ancestor
 *	with a maximum depth of <MAX_ITERATOR_SUBROPE_DEPTH>. Some indices are
 *	stored along with this subrope in a way that traversal can be resumed
 *	quickly: If the current index is withing the subrope's range of 
 *	validity, then traversal starts at the subrope, else it restarts from
 *	the root.
 *
 *	Traversal info is updated lazily, each time actual character data needs 
 *	to be retrieved. This means that a blind iteration over an arbitrarily 
 *	complex rope is no more computationally intensive than over a flat 
 *	string.
 *
 * Argument:
 *	it	- The iterator to update.
 *---------------------------------------------------------------------------*/

void
ColRopeIterUpdateTraversalInfo(
    Col_RopeIterator *it)
{
    Col_Word rope;
    size_t first, last, offset;

    ASSERT(it->rope);
    if (it->traversal.rope.subrope 
	    && (it->index < it->traversal.rope.first 
	    || it->index > it->traversal.rope.last)) {
	/*
	 * Out of range.
	 */

	it->traversal.rope.subrope = WORD_NIL;
    }

    /*
     * Search for leaf rope, remember containing subrope in the process.
     */

    if (it->traversal.rope.subrope) {
	rope = it->traversal.rope.subrope;
    } else {
	rope = it->rope;
	it->traversal.rope.first = 0;
	it->traversal.rope.last = SIZE_MAX;
	it->traversal.rope.offset = 0;
    }
    first = it->traversal.rope.first;
    last = it->traversal.rope.last;
    offset = it->traversal.rope.offset;

    it->traversal.rope.leaf = WORD_NIL;
    while (!it->traversal.rope.leaf) {
	size_t subFirst=first, subLast=last;

	switch (WORD_TYPE(rope)) {
	    case WORD_TYPE_CHAR:
		it->traversal.rope.leaf = rope;
		it->traversal.rope.index = it->index - offset;
		it->traversal.rope.proc = IterAtChar;
		ASSERT(it->traversal.rope.index == 0);
		break;

	    case WORD_TYPE_SMALLSTR:
		it->traversal.rope.leaf = rope;
		it->traversal.rope.index = it->index - offset;
		it->traversal.rope.proc = IterAtSmallStr;
		ASSERT(it->traversal.rope.index < WORD_SMALLSTR_LENGTH(rope));
		break;

	    case WORD_TYPE_UCSSTR:
		it->traversal.rope.leaf = rope;
		it->traversal.rope.index = it->index - offset;
		switch (WORD_UCSSTR_FORMAT(rope)) {
		    case COL_UCS1: it->traversal.rope.proc = IterAtUcs1; break;
		    case COL_UCS2: it->traversal.rope.proc = IterAtUcs2; break;
		    case COL_UCS4: it->traversal.rope.proc = IterAtUcs4; break;
		}
		ASSERT(it->traversal.rope.index < WORD_UCSSTR_LENGTH(rope));
		break;

	    case WORD_TYPE_UTFSTR: {
		const char * data = WORD_UTFSTR_DATA(rope);
		it->traversal.rope.leaf = rope;
		switch (WORD_UTFSTR_FORMAT(rope)) {
		    case COL_UTF8:
			it->traversal.rope.index 
				= (unsigned short) ((const char *)
				Col_Utf8Addr((const Col_Char1 *) data, 
					it->index - offset, 
					WORD_UTFSTR_LENGTH(rope), 
					WORD_UTFSTR_BYTELENGTH(rope)) 
				- data);
			it->traversal.rope.proc = IterAtUtf8;
			break;

		    case COL_UTF16:
			it->traversal.rope.index 
				= (unsigned short) ((const char *)
				Col_Utf16Addr((const Col_Char2 *) data, 
					it->index - offset, 
					WORD_UTFSTR_LENGTH(rope), 
					WORD_UTFSTR_BYTELENGTH(rope)) 
				- data);
			it->traversal.rope.proc = IterAtUtf16;	
			break;
		}
		ASSERT(it->traversal.rope.index < WORD_UTFSTR_BYTELENGTH(rope));
		break;
	    }

	    case WORD_TYPE_CUSTOM: {
		Col_CustomRopeType *typeInfo 
			= (Col_CustomRopeType *) WORD_TYPEINFO(rope);
		it->traversal.rope.leaf = rope;
		it->traversal.rope.index = it->index - offset;
		it->traversal.rope.proc = typeInfo->charAtProc;
		ASSERT(it->traversal.rope.index < typeInfo->lengthProc(rope));
		break;
	    }

	    case WORD_TYPE_SUBROPE: 
		/*
		 * Always remember as subrope.
		 */

		it->traversal.rope.subrope = rope;
		it->traversal.rope.first = first;
		it->traversal.rope.last = last;
		it->traversal.rope.offset = offset;

		/*
		 * Recurse into source.
		 * Note: offset may become negative (in 2's complement) but it 
		 * doesn't matter.
		 */

		offset -= WORD_SUBROPE_FIRST(rope);
		subLast = first - WORD_SUBROPE_FIRST(rope) 
			+ WORD_SUBROPE_LAST(rope);
		rope = WORD_SUBROPE_SOURCE(rope);
		break;

	    case WORD_TYPE_CONCATROPE: {
		size_t leftLength = WORD_CONCATROPE_LEFT_LENGTH(rope);
		if (leftLength == 0) {
		    leftLength = Col_RopeLength(WORD_CONCATROPE_LEFT(rope));
		}
		if (WORD_CONCATROPE_DEPTH(rope) == MAX_ITERATOR_SUBROPE_DEPTH
			|| !it->traversal.rope.subrope) {
		    /*
		     * Remember as subrope.
		     */

		    it->traversal.rope.subrope = rope;
		    it->traversal.rope.first = first;
		    it->traversal.rope.last = last;
		    it->traversal.rope.offset = offset;
		}

		if (it->index - offset < leftLength) {
		    /*
		     * Recurse into left arm.
		     */

		    subLast = offset + leftLength-1;
		    rope = WORD_CONCATROPE_LEFT(rope);
		} else {
		    /*
		     * Recurse into right arm.
		     */

		    subFirst = offset + leftLength;
		    offset += leftLength;
		    rope = WORD_CONCATROPE_RIGHT(rope);
		}
		break;
	    }

	    case WORD_TYPE_WRAP:
		/*
		 * Recurse into source.
		 */

		rope = WORD_WRAP_SOURCE(rope);
		break;

	    /* WORD_TYPE_UNKNOWN */

	    default:
		/* CANTHAPPEN */
		ASSERT(0);
		return;
	}

	/*
	 * Shorten validity range.
	 */

	if (subFirst > first) first = subFirst;
	if (subLast < last) last = subLast;
    }
    if (!it->traversal.rope.subrope) {
	it->traversal.rope.subrope = it->traversal.rope.leaf;
    }
}

/*---------------------------------------------------------------------------
 * Internal Function: ColStringIterCharAt
 *
 *	Get the character at the position designated by the iterator. The latter
 *	must have been initialized by <Col_RopeIterString>.
 *
 * Argument:
 *	it	- The iterator.
 *
 * Result:
 *	The character at the current position.
 *---------------------------------------------------------------------------*/

Col_Char
ColStringIterCharAt(
    const Col_RopeIterator *it)
{
    ASSERT(!it->rope);
    switch (it->traversal.str.format) {
	case COL_UCS1:  return             *(const Col_Char1 *) it->traversal.str.current;
	case COL_UCS2:  return             *(const Col_Char2 *) it->traversal.str.current;
	case COL_UCS4:  return             *(const Col_Char4 *) it->traversal.str.current;
	case COL_UTF8:  return Col_Utf8Get ((const Col_Char1 *) it->traversal.str.current);
	case COL_UTF16: return Col_Utf16Get((const Col_Char2 *) it->traversal.str.current);
    }

    /* CANTHAPPEN */
    ASSERT(0);
    return COL_CHAR_INVALID;
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeIterBegin
 *
 *	Initialize the rope iterator so that it points to the index-th
 *	character within the rope. If index points past the end of the rope, the
 *	iterator is initialized to the end iterator.
 *
 * Arguments:
 *	rope	- Rope to iterate over.
 *	index	- Index of character.
 *	it	- Iterator to initialize.
 *---------------------------------------------------------------------------*/

void
Col_RopeIterBegin(
    Col_Word rope,
    size_t index,
    Col_RopeIterator *it)
{
    it->rope = rope;
    it->length = Col_RopeLength(rope);;
    if (index >= it->length) {
	/*
	 * End of list.
	 */

	index = it->length;
    }
    it->index = index;

    /*
     * Traversal info will be lazily computed.
     */

    it->traversal.rope.subrope = WORD_NIL;
    it->traversal.rope.leaf = WORD_NIL;
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeIterFirst
 *
 *	Initialize the rope iterator so that it points to the first
 *	character within the rope. If rope is empty, the iterator is initialized
 *	to the end iterator.
 *
 * Arguments:
 *	rope	- Rope to iterate over.
 *	it	- Iterator to initialize.
 *---------------------------------------------------------------------------*/

void
Col_RopeIterFirst(
    Col_Word rope,
    Col_RopeIterator *it)
{
    it->rope = rope;
    it->length = Col_RopeLength(rope);
    it->index = 0;

    /*
     * Traversal info will be lazily computed.
     */

    it->traversal.rope.subrope = WORD_NIL;
    it->traversal.rope.leaf = WORD_NIL;
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeIterLast
 *
 *	Initialize the rope iterator so that it points to the last
 *	character within the rope. If rope is empty, the iterator is initialized
 *	to the end iterator.
 *
 * Arguments:
 *	rope	- Rope to iterate over.
 *	it	- Iterator to initialize.
 *---------------------------------------------------------------------------*/

void
Col_RopeIterLast(
    Col_Word rope,
    Col_RopeIterator *it)
{
    it->rope = rope;
    it->length = Col_RopeLength(rope);
    if (it->length == 0) {
	/*
	 * End of list.
	 */

	it->index = 0;
    } else {
	it->index = it->length-1;
    }

    /*
     * Traversal info will be lazily computed.
     */

    it->traversal.rope.subrope = WORD_NIL;
    it->traversal.rope.leaf = WORD_NIL;
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeIterString
 *
 *	Initialize the rope iterator so that it points to the first character
 *	in a string.
 *
 * Arguments:
 *	format	- Format of data in string (see <Col_StringFormat>).
 *	data	- Buffer containing flat data.
 *	length	- Character length of string.
 *	it	- Iterator to initialize.
 *---------------------------------------------------------------------------*/

void
Col_RopeIterString(
    Col_StringFormat format, 
    const void *data, 
    size_t length,
    Col_RopeIterator *it)
{
    it->rope = WORD_NIL;
    it->length = length;
    it->index = 0;
    it->traversal.str.format = format;
    it->traversal.str.begin = (const char *) data;
    it->traversal.str.current = (const char *) data;
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeIterCompare
 *
 *	Compare two iterators by their respective positions.
 *
 * Arguments:
 *	it1	- First iterator.
 *	it2	- Second iterator.
 *
 * Result:
 *	-  < 0 if it1 is before it2
 *	-  > 0 if it1 is after it2
 *	-  0 if they are equal
 *---------------------------------------------------------------------------*/

int
Col_RopeIterCompare(
    const Col_RopeIterator *it1,
    const Col_RopeIterator *it2)
{
    /*
     * Note: iterators at end have their index set to SIZE_MAX.
     */

    if (it1->index < it2->index) {
	return -1;
    } else if (it1->index > it2->index) {
	return 1;
    } else {
	return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeIterMoveTo
 *
 *	Move the iterator to the given absolute position.
 *
 * Arguments:
 *	it	- The iterator to move.
 *	index	- Position.
 *---------------------------------------------------------------------------*/

void
Col_RopeIterMoveTo(
    Col_RopeIterator *it,
    size_t index)
{
    if (index > it->index) {
	Col_RopeIterForward(it, index - it->index);
    } else if (index < it->index) {
	Col_RopeIterBackward(it, it->index - index);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeIterForward
 *
 *	Move the iterator to the nb-th next character.
 *
 * Arguments:
 *	it	- The iterator to move.
 *	nb	- Offset.
 *---------------------------------------------------------------------------*/

void
Col_RopeIterForward(
    Col_RopeIterator *it,
    size_t nb)
{
    if (nb == 0) {
	/*
	 * No-op.
	 */

	return;
    }

    if (Col_RopeIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid rope iterator");
	return;
    }

    if (nb >= it->length - it->index) {
	/*
	 * End of rope.
	 */

	it->index = it->length;
	return;
    }
    it->index += nb;

    if (!it->rope) {
	/*
	 * Chunk iterator.
	 */

	Col_StringFormat format = it->traversal.str.format;
	switch (format) {
	    case COL_UCS1:
	    case COL_UCS2:
	    case COL_UCS4:
		it->traversal.str.current += nb * CHAR_WIDTH(format);
		break;

	    case COL_UTF8:
		while (nb--) it->traversal.str.current 
			= (const char *) Col_Utf8Next(
				(const Col_Char1 *) it->traversal.str.current);
		break;

	    case COL_UTF16:
		while (nb--) it->traversal.str.current 
			= (const char *) Col_Utf16Next(
				(const Col_Char2 *) it->traversal.str.current);
		break;
	}
	return;
    }

    ASSERT(it->rope);
    if (!it->traversal.rope.subrope || !it->traversal.rope.leaf) {
	/*
	 * No traversal info.
	 */

	return;
    }
    if (it->index > it->traversal.rope.last) {
	/*
	 * Traversal info out of range. Discard leaf node info, but not 
	 * subrope, as it may be used again should the iteration go back.
	 */

	it->traversal.rope.leaf = WORD_NIL;
	return;
    }

    /*
     * Update traversal info.
     */

    switch (WORD_TYPE(it->traversal.rope.leaf)) {
	case WORD_TYPE_CHAR:
	    /*
	     * Reached end of leaf.
	     */

	    ASSERT(it->traversal.rope.index == 0);
	    ASSERT(nb > 0);
	    it->traversal.rope.leaf = WORD_NIL;

	case WORD_TYPE_SMALLSTR:
	    if (nb >= WORD_SMALLSTR_LENGTH(it->traversal.rope.leaf) 
		    - it->traversal.rope.index) {
		/*
		 * Reached end of leaf.
		 */

		it->traversal.rope.leaf = WORD_NIL;
	    } else {
		it->traversal.rope.index += nb;
	    }
	    break;

	case WORD_TYPE_UCSSTR:
	    if (nb >= WORD_UCSSTR_LENGTH(it->traversal.rope.leaf) 
		    - it->traversal.rope.index) {
		/*
		 * Reached end of leaf.
		 */

		it->traversal.rope.leaf = WORD_NIL;
	    } else {
		it->traversal.rope.index += nb;
	    }
	    break;

	case WORD_TYPE_UTFSTR: {
	    size_t byteLength = WORD_UTFSTR_BYTELENGTH(it->traversal.rope.leaf),
		    length = WORD_UTFSTR_LENGTH(it->traversal.rope.leaf);
	    const char * data = WORD_UTFSTR_DATA(it->traversal.rope.leaf),
		    *p = data + it->traversal.rope.index;
	    int format = WORD_UTFSTR_FORMAT(it->traversal.rope.leaf);
	    size_t i;
	    if (length == byteLength) {
		/*
		 * No variable-width sequence, use simple arithmetics instead
		 * of character iteration.
		 */

		if (nb * CHAR_WIDTH(format) >= byteLength 
			- it->traversal.rope.index) {
		    /*
		     * Reached end of leaf.
		     */

		    it->traversal.rope.leaf = WORD_NIL;
		} else {
		    it->traversal.rope.index += nb * CHAR_WIDTH(format);
		}
		break;
	    }
	    for (i = 0; i < nb; i++) {
		switch (format) {
		    case COL_UTF8:  p = (const char *) Col_Utf8Next ((const Col_Char1 *) p); break;
		    case COL_UTF16: p = (const char *) Col_Utf16Next((const Col_Char2 *) p); break;
		}
		if (p >= data + byteLength) {
		    /*
		     * Reached end of leaf.
		     */

		    it->traversal.rope.leaf = WORD_NIL;
		    break;
		}
	    }
	    it->traversal.rope.index = p - data;
	    break;
	}

	case WORD_TYPE_CUSTOM: {
	    Col_CustomRopeType *typeInfo 
		    = (Col_CustomRopeType *) 
		    WORD_TYPEINFO(it->traversal.rope.leaf);
	    ASSERT(typeInfo->type.type == COL_ROPE);
	    if (nb >= typeInfo->lengthProc(it->traversal.rope.leaf) 
		    - it->traversal.rope.index) {
		/*
		 * Reached end of leaf.
		 */

		it->traversal.rope.leaf = WORD_NIL;
	    } else {
		it->traversal.rope.index += nb;
	    }
	    break;
	}

	/* WORD_TYPE_UNKNOWN */
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeIterBackward
 *
 *	Move the iterator to the nb-th previous character.
 *
 * Arguments:
 *	it	- The iterator to move.
 *	nb	- Offset.
 *---------------------------------------------------------------------------*/

void
Col_RopeIterBackward(
    Col_RopeIterator *it,
    size_t nb)
{
    if (nb == 0) {
	/*
	 * No-op.
	 */

	return;
    }

    if (Col_RopeIterEnd(it)) {
	if (nb <= it->length) {
	    /*
	     * Allow iterators at end to go back.
	     */

	    it->index = it->length-nb;
	    if (!it->rope) {
		/*
		 * Chunk iterator.
		 */

		Col_StringFormat format = it->traversal.str.format;
		switch (format) {
		    case COL_UCS1:
		    case COL_UCS2:
		    case COL_UCS4:
			it->traversal.str.current = it->traversal.str.begin
				+ it->index * CHAR_WIDTH(format);
			break;

		    case COL_UTF8:
			it->traversal.str.current = it->traversal.str.begin;
			nb = it->index;
			while (nb--) it->traversal.str.current 
				= (const char *) Col_Utf8Next(
					(const Col_Char1 *) it->traversal.str.current);
			break;

		    case COL_UTF16:
			it->traversal.str.current = it->traversal.str.begin;
			nb = it->index;
			while (nb--) it->traversal.str.current 
				= (const char *) Col_Utf16Next(
					(const Col_Char2 *) it->traversal.str.current);
			break;
		}
		return;
	    }

	    ASSERT(it->rope);
	    it->traversal.rope.leaf = WORD_NIL;
	    return;
	}
	Col_Error(COL_ERROR, "Invalid rope iterator");
	return;
    }

    if (it->index < nb) {
	/*
	 * Beginning of rope.
	 */
    
	it->index = it->length;
	return;
    }
    it->index -= nb;

    if (!it->rope) {
	/*
	 * Chunk iterator.
	 */

	Col_StringFormat format = it->traversal.str.format;
	switch (format) {
	    case COL_UCS1:
	    case COL_UCS2:
	    case COL_UCS4:
		it->traversal.str.current -= nb * CHAR_WIDTH(format);
		break;

	    case COL_UTF8:
		while (nb--) it->traversal.str.current 
			= (const char *) Col_Utf8Prev(
				(const Col_Char1 *) it->traversal.str.current);
		break;

	    case COL_UTF16:
		while (nb--) it->traversal.str.current
			= (const char *) Col_Utf16Prev(
				(const Col_Char2 *) it->traversal.str.current);
		break;
	}
	return;
    }

    ASSERT(it->rope);
    if (!it->traversal.rope.subrope || !it->traversal.rope.leaf) {
	/*
	 * No traversal info.
	 */

	return;
    }
    if (it->index < it->traversal.rope.first) {
	/*
	 * Traversal info out of range. Discard leaf node info, but not 
	 * subrope, as it may be used again should the iteration go back.
	 */

	it->traversal.rope.leaf = WORD_NIL;
	return;
    }

    /*
     * Update traversal info.
     */

    switch (WORD_TYPE(it->traversal.rope.leaf)) {
	case WORD_TYPE_CHAR:
	    /*
	     * Reached beginning of leaf. 
	     */

	    ASSERT(it->traversal.rope.index == 0);
	    ASSERT(nb > 0);
	    it->traversal.rope.leaf = WORD_NIL;

	case WORD_TYPE_SMALLSTR:
	    if (it->traversal.rope.index < nb) {
		/*
		 * Reached beginning of leaf.
		 */

		it->traversal.rope.leaf = WORD_NIL;
	    } else {
		it->traversal.rope.index -= nb;
	    }
	    break;

	case WORD_TYPE_UCSSTR:
	    if (it->traversal.rope.index < nb) {
		/*
		 * Reached beginning of leaf. 
		 */

		it->traversal.rope.leaf = WORD_NIL;
	    } else {
		it->traversal.rope.index -= nb;
	    }
	    break;

	case WORD_TYPE_UTFSTR: {
	    size_t byteLength = WORD_UTFSTR_BYTELENGTH(it->traversal.rope.leaf),
		    length = WORD_UTFSTR_LENGTH(it->traversal.rope.leaf);
	    const char * data = WORD_UTFSTR_DATA(it->traversal.rope.leaf),
		    *p = data + it->traversal.rope.index;
	    int format = WORD_UTFSTR_FORMAT(it->traversal.rope.leaf);
	    size_t i;
	    if (length == byteLength) {
		/*
		 * No variable-width sequence, use simple arithmetics instead
		 * of character iteration.
		 */

		if (it->traversal.rope.index < nb * CHAR_WIDTH(format)) {
		    /*
		     * Reached beginning of leaf. 
		     */

		    it->traversal.rope.leaf = WORD_NIL;
		} else {
		    it->traversal.rope.index -= nb * CHAR_WIDTH(format);
		}
		break;
	    }
	    for (i = 0; i < nb; i++) {
		if (p == data) {
		    /*
		     * Reached beginning of leaf.
		     */

		    it->traversal.rope.leaf = WORD_NIL;
		    break;
		}
		switch (format) {
		    case COL_UTF8:  p = (const char *) Col_Utf8Prev ((const Col_Char1 *) p); break;
		    case COL_UTF16: p = (const char *) Col_Utf16Prev((const Col_Char2 *) p); break;
		}
	    }
	    it->traversal.rope.index = p - data;
	    break;
	}

	case WORD_TYPE_CUSTOM:
	    if (it->traversal.rope.index < nb) {
		/*
		 * Reached beginning of leaf.
		 */

		it->traversal.rope.leaf = WORD_NIL;
	    } else {
		it->traversal.rope.index -= nb;
	    }
	    break;

	/* WORD_TYPE_UNKNOWN */
    }
}
