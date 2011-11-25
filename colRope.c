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

#include "colibri.h"
#include "colInternal.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <malloc.h> /* For alloca */

/*
 * Prototypes for functions used only in this file.
 */

typedef struct ChunkTraverseInfo *pChunkTraverseInfo;
static Col_RopeChunksTraverseProc MergeChunksProc;
static Col_RopeChunksTraverseProc CompareChunksProc;
static unsigned char	GetDepth(Col_Word rope);
static void		GetArms(Col_Word rope, Col_Word * leftPtr, 
			    Col_Word * rightPtr);
static void		GetChunk(struct ChunkTraverseInfo *info, 
			    Col_RopeChunk *chunk);
static void		NextChunk(struct ChunkTraverseInfo *info, size_t nb);
static ColRopeIterLeafAtProc IterAtUcs1, IterAtUcs2, IterAtUcs4, IterAtUtf8;


/****************************************************************************
 * Internal Group: Internal Definitions
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Constant: ROPE_CHAR_MAX
 *
 *	Max codepoint value of characters in immediate form.
 *
 * See also:
 *	<WORD_CHAR_NEW>
 *---------------------------------------------------------------------------*/

#define ROPE_CHAR_MAX		0xFFFFFF

/*---------------------------------------------------------------------------
 * Internal Constants: Short Leaves
 *
 *	Constants controlling the creation of short leaves during subrope/
 *	concatenation. Ropes built this way normally use subrope and concat 
 *	nodes, but to avoid fragmentation, multiple short ropes are flattened 
 *	into a single leaf.
 *
 *  MAX_SHORT_LEAF_SIZE		- Maximum number of bytes a short leaf can take.
 *---------------------------------------------------------------------------*/

#define MAX_SHORT_LEAF_SIZE \
    (3*CELL_SIZE)

/*---------------------------------------------------------------------------
 * Internal Constant: MAX_ITERATOR_SUBROPE_DEPTH
 *
 *	Max depth of subropes in iterators.
 *---------------------------------------------------------------------------*/

#define MAX_ITERATOR_SUBROPE_DEPTH \
    3


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
 * Arguments:
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
 *	Create a new rope from a single character.
 *
 * Arguments:
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
    if (c <= UCHAR_MAX) {
	/* 
	 * Return small string value.
	 */

	Col_Word rope;
	WORD_SMALLSTR_SET_LENGTH(rope, 1);
	*WORD_SMALLSTR_DATA(rope) = (Col_Char1) c;
	return rope;
    } else if (c <= ROPE_CHAR_MAX) {
	/*
	 * Return char value.
	 */

	return WORD_CHAR_NEW(c);
    }
    return Col_NewRope(COL_UCS4, &c, sizeof(c));
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
 *	format		- Format of data in buffer.
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

    if (format == COL_UTF8) {
	const Col_Char1 *source = (const Col_Char1 *) data, *p;
	if (byteLength <= UTF8STR_MAX_BYTELENGTH) {
	    /*
	     * String fits into one multi-cell leaf rope. We know the byte 
	     * length, now get the char length by counting the number of 
	     * sequence starts (!= 10xxxxxx). 
	     */

	    length = 0;
	    for (p = source; p < source+byteLength; p++) {
		if ((*p & 0xC0) != 0x80) {length++;}
	    }
	    if (length == byteLength) {
		/* 
		 * This implies that the string data is 7-bit clean and 
		 * contains no multibyte sequence. Use the faster UCS-1 format 
		 * instead of UTF-8. 
		 */

		return Col_NewRope(COL_UCS1, source, byteLength);
	    } 
    	    
	    rope = (Col_Word) AllocCells(UTF8STR_SIZE(byteLength));
	    WORD_UTF8STR_INIT(rope, length, byteLength);
	    memcpy((void *) WORD_UTF8STR_DATA(rope), source, byteLength);
	    return rope;
	}

	/* 
	 * Split data in half at char boundary. 
	 */

	for (p = source+byteLength/2; (*p & 0xC0) == 0x80; p++);
	half = p-source;
    } else {
	/* 
	 * Fixed-width UCS.
	 *
	 * Note: the integer values of the format enums match the char size.
	 */

	length = byteLength / format;
	switch (format) {
	    case COL_UCS1:
		if (length <= SMALLSTR_MAX_LENGTH) {
		    /*
		     * Return small string value.
		     */

		    WORD_SMALLSTR_SET_LENGTH(rope, length);
		    memcpy(WORD_SMALLSTR_DATA(rope), data, length);
		    return rope;
		} 
		break;

	    case COL_UCS2: {
		const Col_Char2 * string = data;
		if (length == 1 && string[0] > UCHAR_MAX) {
		    /*
		     * Return char value.
		     */

		    return WORD_CHAR_NEW(string[0]);
		} else if (length <= SMALLSTR_MAX_LENGTH) {
		    /*
		     * Return small string value if possible.
		     */

		    size_t i;
		    WORD_SMALLSTR_SET_LENGTH(rope, length);
		    for (i = 0; i < length; i++) {
			if (string[i] > UCHAR_MAX) break;
			WORD_SMALLSTR_DATA(rope)[i] = (Col_Char1) string[i];
		    }
		    if (i == length) {
			return rope;
		    }
		} 
		break;
	    }

	    case COL_UCS4: {
		const Col_Char4 * string = data;
		if (length == 1 && string[0] > UCHAR_MAX 
			&& string[0] <= ROPE_CHAR_MAX) {
		    /*
		     * Return char value.
		     */

		    return WORD_CHAR_NEW(string[0]);
		} else if (length <= SMALLSTR_MAX_LENGTH) {
		    /*
		     * Return small string value if possible.
		     */

		    size_t i;
		    WORD_SMALLSTR_SET_LENGTH(rope, length);
		    for (i = 0; i < length; i++) {
			if (string[i] > UCHAR_MAX) break;
			WORD_SMALLSTR_DATA(rope)[i] = (Col_Char1) string[i];
		    }
		    if (i == length) {
			return rope;
		    }
		} 
		break;
	    }
	}
	if (length <= UCSSTR_MAX_LENGTH) {
	    /* 
	     * String fits into one multi-cell leaf rope. 
	     */

	    rope = (Col_Word) AllocCells(UCSSTR_SIZE(byteLength));
	    WORD_UCSSTR_INIT(rope, format, length);
	    memcpy((void *) WORD_UCSSTR_DATA(rope), data, byteLength);
	    return rope;
	}

	/* 
	 * Split data in half at char boundary. 
	 */

	half = (length/2) * format;
    }

    /* 
     * The rope is built by concatenating the two halves of the string. This 
     * recursive halving ensures that the resulting binary tree is properly 
     * balanced. 
     */

    return Col_ConcatRopes(Col_NewRope(format, data, half),
	    Col_NewRope(format, (const char *) data+half, byteLength-half));
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

	case WORD_TYPE_UTF8STR:
	    return WORD_UTF8STR_LENGTH(rope);

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
    const char *c[2];

    ASSERT(number == 2);

    /*
     * Quick cases.
     */

    if (!chunks[1].data) {
	/*
	 * Chunk 1 is longer.
	 */

	ASSERT(chunks[0].data);
	if (info->posPtr) *info->posPtr = index;
	if (info->c1Ptr) {
	    switch (chunks[0].format) {
		case COL_UCS1: *info->c1Ptr = *((Col_Char1 *) chunks[0].data); break;
		case COL_UCS2: *info->c1Ptr = *((Col_Char2 *) chunks[0].data); break;
		case COL_UCS4: *info->c1Ptr = *((Col_Char4 *) chunks[0].data); break;
		case COL_UTF8: *info->c1Ptr = Col_Utf8CharAt(chunks[0].data); break;
	    }
	}
	if (info->c2Ptr) *info->c2Ptr = COL_CHAR_INVALID;
	return 1;
    } else if (!chunks[0].data) {
	/*
	 * Chunk 2 is longer.
	 */

	ASSERT(chunks[1].data);
	if (info->posPtr) *info->posPtr = index;
	if (info->c1Ptr) *info->c1Ptr = COL_CHAR_INVALID;
	if (info->c2Ptr) {
	    switch (chunks[1].format) {
		case COL_UCS1: *info->c2Ptr = *((Col_Char1 *) chunks[1].data); break;
		case COL_UCS2: *info->c2Ptr = *((Col_Char2 *) chunks[1].data); break;
		case COL_UCS4: *info->c2Ptr = *((Col_Char4 *) chunks[1].data); break;
		case COL_UTF8: *info->c2Ptr = Col_Utf8CharAt(chunks[1].data); break;
	    }
	}
	return -1;
    } else if (chunks[0].data == chunks[1].data) {
	/*
	 * Chunks are identical.
	 */

	return 0;
    }

    /*
     * Compare char by char.
     */

    ASSERT(chunks[0].data);
    ASSERT(chunks[1].data);
    c[0] = (const char *) chunks[0].data;
    c[1] = (const char *) chunks[1].data;
    for (i = 0; i < length; i++) {
	switch (chunks[0].format) {
	    case COL_UCS1: c1 = ((Col_Char1 *) c[0])[i]; break;
	    case COL_UCS2: c1 = ((Col_Char2 *) c[0])[i]; break;
	    case COL_UCS4: c1 = ((Col_Char4 *) c[0])[i]; break;
	    case COL_UTF8: c1 = Col_Utf8CharAt(c[0]); COL_UTF8_NEXT(c[0]); break;
	}
	switch (chunks[1].format) {
	    case COL_UCS1: c2 = ((Col_Char1 *) c[1])[i]; break;
	    case COL_UCS2: c2 = ((Col_Char2 *) c[1])[i]; break;
	    case COL_UCS4: c2 = ((Col_Char4 *) c[1])[i]; break;
	    case COL_UTF8: c2 = Col_Utf8CharAt(c[1]); COL_UTF8_NEXT(c[1]); break;
	}

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
	 * Return 0 here, as Col_TraverseRopeChunks would return -1.
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
 * Internal Typedef: MergeChunksInfo
 *
 *	Structure used to collect data during the traversal of ropes to merge
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
 *	one to fit wthin one leaf rope. Follows <Col_RopeChunksTraverseProc> 
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
    if (info->format == COL_UTF8) {
	if (chunks->format != COL_UTF8) {
	    /* 
	     * Don't merge UTF-8 with fixed-width formats. 
	     */

	    return -1;
	}
	if (info->byteLength + chunks->byteLength
		> MAX_SHORT_LEAF_SIZE - WORD_UTF8STR_HEADER_SIZE) {
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

	if (chunks->format == COL_UTF8) {
	    /* 
	     * Don't merge UTF-8 with fixed-width formats. 
	     */

	    return -1;
	}

	/* 
	 * Convert and append data if needed. Conversion of existing data is
	 * done in-place, so iterate backwards. 
	 */

	switch (info->format) {
	    case COL_UCS1:
		switch (chunks->format) {
		    case COL_UCS2:
			if ((info->length + length) * sizeof(Col_Char2)
				> MAX_SHORT_LEAF_SIZE 
				- WORD_UCSSTR_HEADER_SIZE) {
			    /* 
			     * Data won't fit. 
			     */

			    return -1;
			}

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
			if ((info->length + length) * sizeof(Col_Char4)
				> MAX_SHORT_LEAF_SIZE - WORD_UCSSTR_HEADER_SIZE) {
			    /* 
			     * Data won't fit. 
			     */

			    return -1;
			}

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
			if (info->byteLength + length*sizeof(Col_Char2)
				> MAX_SHORT_LEAF_SIZE 
				- WORD_UCSSTR_HEADER_SIZE) {
			    /* 
			     * Data won't fit. 
			     */

			    return -1;
			}

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
			if ((info->length + length) * sizeof(Col_Char4)
				> MAX_SHORT_LEAF_SIZE - WORD_UCSSTR_HEADER_SIZE) {
			    /* 
			     * Data won't fit. 
			     */

			    return -1;
			}

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
			if (info->byteLength + length*sizeof(Col_Char4)
				> MAX_SHORT_LEAF_SIZE - WORD_UCSSTR_HEADER_SIZE) {
			    /* 
			     * Data won't fit. 
			     */

			    return -1;
			}

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
			if (info->byteLength + length*sizeof(Col_Char4)
				> MAX_SHORT_LEAF_SIZE - WORD_UCSSTR_HEADER_SIZE) {
			    /* 
			     * Data won't fit. 
			     */

			    return -1;
			}

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
 * Arguments:
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
	 *
	 * Note: the integer values of the format enums match the char size.
	 */

	case WORD_TYPE_UCSSTR: {
	    Col_StringFormat format = WORD_UCSSTR_FORMAT(rope);
	    if (length*format <= MAX_SHORT_LEAF_SIZE 
		    - WORD_UCSSTR_HEADER_SIZE) {
		return Col_NewRope(format, WORD_UCSSTR_DATA(rope)+first*format, 
			length*format);
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

	if (Col_TraverseRopeChunks(rope, first, length, MergeChunksProc, &info, 
		NULL) == 0) {
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
 *
 * Note on rebalancing:
 *	The resulting concat will have a depth equal to the largest of both 
 *	left and right arms' depth plus 1. If they differ by more than 1, then 
 *	the tree is recursively rebalanced. There are four major cases, two if 
 *	we consider symmetry:
 *
 *	- Deepest subtree is an outer branch (i.e. the left resp. right child of
 *	  the left resp. right arm). In this case the tree is rotated: the
 *	  opposite child is moved and concatenated with the opposite arm.
 *	  For example, with left being deepest:
 *
 * (start diagram)
 * Before:
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

	if (Col_TraverseRopeChunks(left, 0, leftLength, MergeChunksProc, &info, 
		NULL) == 0 && Col_TraverseRopeChunks(right, 0, rightLength, 
		MergeChunksProc, &info, NULL) == 0) {
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
 * Internal Typedef: ChunkTraverseInfo
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

typedef struct ChunkTraverseInfo {
    struct {
	int prevDepth;
	Col_Word rope;
	size_t max;
    } *backtracks;
    Col_Word rope;
    Col_Char4 c;
    size_t start, max;
    int maxDepth, prevDepth;
} ChunkTraverseInfo;

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
 *
 * See also:
 *	<ChunkTraverseInfo>, <Col_TraverseRopeChunksN>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static void
GetChunk(
    ChunkTraverseInfo *info,
    Col_RopeChunk *chunk)
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
		size_t leftLength = WORD_CONCATROPE_LEFT_LENGTH(
			info->rope);
		if (leftLength == 0) {
		    leftLength = Col_RopeLength(
			    WORD_CONCATROPE_LEFT(info->rope));
		}
		if (info->start + info->max <= leftLength) {
		    /* 
		     * Recurse on left arm only. 
		     */

		    info->rope = WORD_CONCATROPE_LEFT(info->rope);
		    continue;
		}
		if (info->start >= leftLength) {
		    /* 
		     * Recurse on right arm only. 
		     */

		    info->start -= leftLength;
		    info->rope = WORD_CONCATROPE_RIGHT(info->rope);
		    continue;
		} 

		/*
		 * Push right onto stack and recurse on left.
		 */

		ASSERT(info->backtracks);
		depth = WORD_CONCATROPE_DEPTH(info->rope);
		ASSERT(depth <= info->maxDepth);
		info->backtracks[depth-1].prevDepth 
			= info->prevDepth;
		info->backtracks[depth-1].rope 
			= WORD_CONCATROPE_RIGHT(info->rope);
		info->backtracks[depth-1].max = info->max
			- (leftLength-info->start);
		info->prevDepth = depth;

		info->max = leftLength-info->start;
		info->rope = WORD_CONCATROPE_LEFT(info->rope);
		continue;

	    /* WORD_TYPE_UNKNOWN */
	    }
	}
	break;
    }

    /*
     * Get leaf data.
     */

    switch (type) {
	case WORD_TYPE_CHAR:
	    ASSERT(info->start == 0);
	    ASSERT(info->max == 1);
	    chunk->format = COL_UCS4;
	    info->c = WORD_CHAR_GET(info->rope);
	    chunk->data = &info->c;
	    break;

	case WORD_TYPE_SMALLSTR:
	    chunk->format = COL_UCS1;
	    chunk->data = WORD_SMALLSTR_DATA(info->rope)
		    + info->start;
	    break;

	case WORD_TYPE_UCSSTR: {
	    /* 
	     * Fixed-width flat strings: traverse range of chars.
	     *
	     * Note: the integer values of the format enums match the char size.
	     */

	    chunk->format = WORD_UCSSTR_FORMAT(info->rope);
	    chunk->data = WORD_UCSSTR_DATA(info->rope)
		    + info->start*chunk->format;
	    break;
	}
        	    
	case WORD_TYPE_UTF8STR:
	    /*
	     * Variable-width flat string.
	     */

	    chunk->format = COL_UTF8;
	    chunk->data = Col_Utf8CharAddr(
		    WORD_UTF8STR_DATA(info->rope), info->start, 
		    WORD_UTF8STR_LENGTH(info->rope), 
		    WORD_UTF8STR_BYTELENGTH(info->rope));
	    break;

	case WORD_TYPE_CUSTOM: {
	    Col_CustomRopeType *typeInfo 
		    = (Col_CustomRopeType *) WORD_TYPEINFO(info->rope);
	    ASSERT(typeInfo->type.type == COL_ROPE);
	    if (typeInfo->chunkAtProc) {
		typeInfo->chunkAtProc(info->rope, info->start,
		    info->max, &info->max, chunk);
	    } else {
		/*
		 * Traverse chars individually.
		 */

		info->max = 1;
		chunk->format = COL_UCS4;
		info->c = typeInfo->charAtProc(info->rope, 
			info->start);
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
 *
 * See also:
 *	<ChunkTraverseInfo>, <Col_TraverseRopeChunksN>, <Col_TraverseRopeChunks>
 *---------------------------------------------------------------------------*/

static void 
NextChunk(
    ChunkTraverseInfo *info,
    size_t nb)
{
    ASSERT(info->max >= nb);
    info->max -= nb;
    if (info->max > 0) {
	/*
	 * Still in leaf, advance.
	 */

	info->start += nb;
    } else if (info->prevDepth == INT_MAX) {
	/*
	 * Already at toplevel => end of rope.
	 */

	info->rope = WORD_NIL;
    } else {
	/*
	 * Reached end of leaf, backtracks.
	 */

	ASSERT(info->backtracks);
	info->rope = info->backtracks[info->prevDepth-1].rope;
	info->max = info->backtracks[info->prevDepth-1].max;
	info->start = 0;
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
    ChunkTraverseInfo *info;
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
	info[i].rope = ropes[i];
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

	    GetChunk(info+i, chunks+i);
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
		    chunks[i].byteLength = max * chunks[i].format;
		    break;

		case COL_UTF8:
		    chunks[i].byteLength = Col_Utf8CharAddr(
			    (const char *) chunks[i].data, max, info[i].max, 
			    WORD_UTF8STR_BYTELENGTH(info[i].rope)
			    - ((const char *) chunks[i].data
			    - WORD_UTF8STR_DATA(info[i].rope)))
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
		if (info[i].rope) NextChunk(info+i, max);
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
    Col_RopeChunksTraverseProc *proc,
    Col_ClientData clientData,
    size_t *lengthPtr)
{
    ChunkTraverseInfo info;
    Col_RopeChunk chunk;
    int result;
    size_t ropeLength = Col_RopeLength(rope);

    if (start > ropeLength) {
	/* 
	 * Start is past the end of the rope.
	 */

	max = 0;
    } else if (max > ropeLength-start) {
	/* 
	 * Adjust max to the remaining length. 
	 */

	max = ropeLength-start;
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
	GetChunk(&info, &chunk);
	max = info.max;

	/*
	 * Compute actual byte length.
	 */

	switch (chunk.format) {
	    case COL_UCS1:
	    case COL_UCS2:
	    case COL_UCS4:
		chunk.byteLength = max * chunk.format;
		break;

	    case COL_UTF8:
		chunk.byteLength = Col_Utf8CharAddr(
			(const char *) chunk.data, max, info.max, 
			WORD_UTF8STR_BYTELENGTH(info.rope)
			- ((const char *) chunk.data
			- WORD_UTF8STR_DATA(info.rope)))
			- (const char *) chunk.data;
		break;
	}

	/*
	 * Call proc on leaf data.
	 */

	if (lengthPtr) *lengthPtr += max;
	result = proc(start, max, 1, &chunk, clientData);
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

	    NextChunk(&info, max);
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
 * Group: Rope Iterators
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
    return Col_Utf8CharAt(WORD_UTF8STR_DATA(leaf) + index);
}

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
 *	with a maximum depth of MAX_ITERATOR_SUBROPE_DEPTH. Some indices are
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

    if (it->traversal.subrope && (it->index < it->traversal.first 
		    || it->index > it->traversal.last)) {
	/*
	 * Out of range.
	 */

	it->traversal.subrope = WORD_NIL;
    }

    /*
     * Search for leaf rope, remember containing subrope in the process.
     */

    if (it->traversal.subrope) {
	rope = it->traversal.subrope;
    } else {
	rope = it->rope;
	it->traversal.first = 0;
	it->traversal.last = SIZE_MAX;
	it->traversal.offset = 0;
    }
    first = it->traversal.first;
    last = it->traversal.last;
    offset = it->traversal.offset;

    it->traversal.leaf = WORD_NIL;
    while (!it->traversal.leaf) {
	size_t subFirst=first, subLast=last;

	switch (WORD_TYPE(rope)) {
	    case WORD_TYPE_CHAR:
		it->traversal.leaf = rope;
		it->traversal.index = it->index - offset;
		it->traversal.proc = IterAtChar;
		ASSERT(it->traversal.index == 0);
		break;

	    case WORD_TYPE_SMALLSTR:
		it->traversal.leaf = rope;
		it->traversal.index = it->index - offset;
		it->traversal.proc = IterAtSmallStr;
		ASSERT(it->traversal.index < WORD_SMALLSTR_LENGTH(rope));
		break;

	    case WORD_TYPE_UCSSTR:
		it->traversal.leaf = rope;
		it->traversal.index = it->index - offset;
		switch (WORD_UCSSTR_FORMAT(rope)) {
		    case COL_UCS1: it->traversal.proc = IterAtUcs1; break;
		    case COL_UCS2: it->traversal.proc = IterAtUcs2; break;
		    case COL_UCS4: it->traversal.proc = IterAtUcs4; break;
		}
		ASSERT(it->traversal.index < WORD_UCSSTR_LENGTH(rope));
		break;

	    case WORD_TYPE_UTF8STR: {
		const char * data = WORD_UTF8STR_DATA(rope);
		it->traversal.leaf = rope;
		it->traversal.index = (unsigned short) (Col_Utf8CharAddr(data, 
			it->index - offset, WORD_UTF8STR_LENGTH(rope), 
			WORD_UTF8STR_BYTELENGTH(rope)) - data);
		it->traversal.proc = IterAtUtf8;
		ASSERT(it->traversal.index < WORD_UTF8STR_BYTELENGTH(rope));
		break;
	    }

	    case WORD_TYPE_CUSTOM: {
		Col_CustomRopeType *typeInfo 
			= (Col_CustomRopeType *) WORD_TYPEINFO(rope);
		it->traversal.leaf = rope;
		it->traversal.index = it->index - offset;
		it->traversal.proc = typeInfo->charAtProc;
		ASSERT(it->traversal.index < typeInfo->lengthProc(rope));
		break;
	    }

	    case WORD_TYPE_SUBROPE: 
		/*
		 * Always remember as subrope.
		 */

		it->traversal.subrope = rope;
		it->traversal.first = first;
		it->traversal.last = last;
		it->traversal.offset = offset;

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
			|| !it->traversal.subrope) {
		    /*
		     * Remember as subrope.
		     */

		    it->traversal.subrope = rope;
		    it->traversal.first = first;
		    it->traversal.last = last;
		    it->traversal.offset = offset;
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
    if (!it->traversal.subrope) {
	it->traversal.subrope = it->traversal.leaf;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeIterBegin
 *
 *	Initialize the rope iterator so that it points to the index-th
 *	character within the rope. If index points past the end of the rope, the
 *	iterator is initialized to the end iterator (i.e. whose rope field is 
 *	nil).
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
    if (index >= Col_RopeLength(rope)) {
	/*
	 * End of rope.
	 */

	it->rope = WORD_NIL;

	return;
    }

    it->rope = rope;
    it->index = index;

    /*
     * Traversal info will be lazily computed.
     */

    it->traversal.subrope = WORD_NIL;
    it->traversal.leaf = WORD_NIL;
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeIterFirst
 *
 *	Initialize the rope iterator so that it points to the first
 *	character within the rope. If rope is empty, the iterator is initialized
 *	to the end iterator (i.e. whose rope field is nil).
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
    if (Col_RopeLength(rope) == 0) {
	/*
	 * End of rope.
	 */

	it->rope = WORD_NIL;

	return;
    }

    it->rope = rope;
    it->index = 0;

    /*
     * Traversal info will be lazily computed.
     */

    it->traversal.subrope = WORD_NIL;
    it->traversal.leaf = WORD_NIL;
}

/*---------------------------------------------------------------------------
 * Function: Col_RopeIterLast
 *
 *	Initialize the rope iterator so that it points to the last
 *	character within the rope. If rope is empty, the iterator is initialized
 *	to the end iterator (i.e. whose rope field is nil).
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
    size_t length = Col_RopeLength(rope);
    if (length == 0) {
	/*
	 * End of rope.
	 */

	it->rope = WORD_NIL;

	return;
    }

    it->rope = rope;
    it->index = length-1;

    /*
     * Traversal info will be lazily computed.
     */

    it->traversal.subrope = WORD_NIL;
    it->traversal.leaf = WORD_NIL;
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
    Col_RopeIterator *it1,
    Col_RopeIterator *it2)
{
    if (Col_RopeIterEnd(it1)) {
	if (Col_RopeIterEnd(it2)) {
	    return 0;
	} else {
	    return 1;
	}
    } else if (Col_RopeIterEnd(it2)) {
	return -1;
    } else if (it1->index < it2->index) {
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
    if (Col_RopeIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid rope iterator");
	return;
    }

    if (nb == 0) {
	/*
	 * No-op.
	 */

	return;
    }

    /*
     * Check for end of rope.
     */
    
    if (nb >= Col_RopeLength(it->rope) - it->index) {
	/*
	 * End of rope.
	 */

	it->rope = WORD_NIL;
	return;
    }
    it->index += nb;

    if (!it->traversal.subrope || !it->traversal.leaf) {
	/*
	 * No traversal info.
	 */

	return;
    }
    if (it->index > it->traversal.last) {
	/*
	 * Traversal info out of range. Discard leaf node info, but not 
	 * subrope, as it may be used again should the iteration go back.
	 */

	it->traversal.leaf = WORD_NIL;
	return;
    }

    /*
     * Update traversal info.
     */

    switch (WORD_TYPE(it->traversal.leaf)) {
	case WORD_TYPE_CHAR:
	    /*
	     * Reached end of leaf.
	     */

	    ASSERT(it->traversal.index == 0);
	    ASSERT(nb > 0);
	    it->traversal.leaf = WORD_NIL;

	case WORD_TYPE_SMALLSTR:
	    if (nb >= WORD_SMALLSTR_LENGTH(it->traversal.leaf) 
		    - it->traversal.index) {
		/*
		 * Reached end of leaf.
		 */

		it->traversal.leaf = WORD_NIL;
	    } else {
		it->traversal.index += nb;
	    }
	    break;

	case WORD_TYPE_UCSSTR:
	    if (nb >= WORD_UCSSTR_LENGTH(it->traversal.leaf) 
		    - it->traversal.index) {
		/*
		 * Reached end of leaf.
		 */

		it->traversal.leaf = WORD_NIL;
	    } else {
		it->traversal.index += nb;
	    }
	    break;

	case WORD_TYPE_UTF8STR: {
	    size_t byteLength = WORD_UTF8STR_BYTELENGTH(it->traversal.leaf);
	    const char * data = WORD_UTF8STR_DATA(it->traversal.leaf),
		    *p = data + it->traversal.index;
	    size_t i;
	    for (i = 0; i < nb; i++) {
		COL_UTF8_NEXT(p);
		if (p >= data + byteLength) {
		    /*
		     * Reached end of leaf.
		     */

		    it->traversal.leaf = WORD_NIL;
		    break;
		}
	    }
	    it->traversal.index = p - data;
	    break;
	}

	case WORD_TYPE_CUSTOM: {
	    Col_CustomRopeType *typeInfo 
		    = (Col_CustomRopeType *) WORD_TYPEINFO(it->traversal.leaf);
	    ASSERT(typeInfo->type.type == COL_ROPE);
	    if (nb >= typeInfo->lengthProc(it->traversal.leaf) 
		    - it->traversal.index) {
		/*
		 * Reached end of leaf.
		 */

		it->traversal.leaf = WORD_NIL;
	    } else {
		it->traversal.index += nb;
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
    if (Col_RopeIterEnd(it)) {
	Col_Error(COL_ERROR, "Invalid rope iterator");
	return;
    }

    if (nb == 0) {
	/*
	 * No-op.
	 */

	return;
    }

    /*
     * Check for beginning of rope.
     */
    
    if (it->index < nb) {
	it->rope = WORD_NIL;
	return;
    }

    it->index -= nb;

    if (!it->traversal.subrope || !it->traversal.leaf) {
	/*
	 * No traversal info.
	 */

	return;
    }
    if (it->index < it->traversal.first) {
	/*
	 * Traversal info out of range. Discard leaf node info, but not 
	 * subrope, as it may be used again should the iteration go back.
	 */

	it->traversal.leaf = WORD_NIL;
	return;
    }

    /*
     * Update traversal info.
     */

    switch (WORD_TYPE(it->traversal.leaf)) {
	case WORD_TYPE_CHAR:
	    /*
	     * Reached beginning of leaf. 
	     */

	    ASSERT(it->traversal.index == 0);
	    ASSERT(nb > 0);
	    it->traversal.leaf = WORD_NIL;

	case WORD_TYPE_SMALLSTR:
	    if (it->traversal.index < nb) {
		/*
		 * Reached beginning of leaf.
		 */

		it->traversal.leaf = WORD_NIL;
	    } else {
		it->traversal.index -= nb;
	    }
	    break;

	case WORD_TYPE_UCSSTR:
	    if (it->traversal.index < nb) {
		/*
		 * Reached beginning of leaf. 
		 */

		it->traversal.leaf = WORD_NIL;
	    } else {
		it->traversal.index -= nb;
	    }
	    break;

	case WORD_TYPE_UTF8STR: {
	    const char * data = WORD_UTF8STR_DATA(it->traversal.leaf),
		    *p = data + it->traversal.index;
	    size_t i;
	    for (i = 0; i < nb; i++) {
		COL_UTF8_PREVIOUS(p);
		if (p < data) {
		    /*
		     * Reached beginning of leaf.
		     */

		    it->traversal.leaf = WORD_NIL;
		    break;
		}
	    }
	    it->traversal.index = p - data;
	    break;
	}

	case WORD_TYPE_CUSTOM:
	    if (it->traversal.index < nb) {
		/*
		 * Reached beginning of leaf.
		 */

		it->traversal.leaf = WORD_NIL;
	    } else {
		it->traversal.index -= nb;
	    }
	    break;

	/* WORD_TYPE_UNKNOWN */
    }
}
