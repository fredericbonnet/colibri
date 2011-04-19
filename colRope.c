#include "colibri.h"
#include "colInternal.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <malloc.h> /* For alloca */


#define ROPE_CHAR_MAX		0xFFFFFF

/* 
 * Max byte size of short leaves (inc. header) created with subrope or concat. 
 */

#define MAX_SHORT_LEAF_SIZE	(3*CELL_SIZE)

/*
 * Max depth of subropes in iterators.
 */

#define MAX_ITERATOR_SUBROPE_DEPTH 3

/*
 * Prototypes for functions used only in this file.
 */

static Col_RopeChunksTraverseProc MergeStringsProc;
static void		GetArms(Col_Word rope, Col_Word * leftPtr, 
			    Col_Word * rightPtr);
static Col_RopeChunksTraverseProc CompareChunksProc;
static ColRopeIterLeafAtProc IterAtUcs1, IterAtUcs2, IterAtUcs4, IterAtUtf8;


/*
 *---------------------------------------------------------------------------
 *
 * Col_Utf8CharAddr --
 *
 *	Find the index-th char in a UTF-8 byte sequence. 
 *
 *	Iterate over char boundaries from the beginning or end of the string, 
 *	whichever is closest, until the char is reached.
 *
 *	Assume input is OK.
 *
 * Results:
 *	Pointer to the character.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

const char * 
Col_Utf8CharAddr(
    const char * data,		/* UTF-8 byte sequence. */
    size_t index,		/* Index of char to find. */
    size_t length,		/* Char length of sequence. */
    size_t byteLength)		/* Byte length of sequence. */
{
    /* 
     * Don't check bounds; assume input values are OK. 
     */

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
	while (i != index) {
	    i--;		/* Decrement char index. */
	    COL_UTF8_PREVIOUS(data);
	}
	return data;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_Utf8CharAt --
 *
 *	Get the char codepoint of a UTF-8 sequence.
 *
 * Results:
 *	32-bit Unicode codepoint of the char.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

Col_Char 
Col_Utf8CharAt(
    const char * data)		/* UTF-8 byte sequence. */
{
    if (*data < 0x80) {
	/* 
	 * Single byte, 0-7F codepoints. 
	 */

	return *data;
    } else if (*data < 0xE0) {
	/* 
	 * 2-byte sequence, 80-7FF codepoints. 
	 */

	return   ((data[0] & 0x1F) << 6)
	       |  (data[1] & 0x3F);
    } else if (*data < 0xF0) {
	/* 
	 * 3-byte sequence, 800-FFFF codepoints. 
	 */

	return   ((data[0] & 0x0F) << 12)
	       | ((data[1] & 0x3F) << 6)
	       |  (data[2] & 0x3F);
    } else if (*data < 0xF8) {
	/* 
	 * 4-byte sequence, 10000-1FFFFF codepoints. 
	 */

	return   ((data[0] & 0x07) << 18)
	       | ((data[1] & 0x3F) << 12)
	       | ((data[2] & 0x3F) << 6)
	       |  (data[3] & 0x3F);
    } else if (*data < 0xFC) {
	/* 
	 * 5-byte sequence, 200000-3FFFFFF codepoints. 
	 */

	return   ((data[0] & 0x03) << 24)
	       | ((data[1] & 0x3F) << 18)
	       | ((data[2] & 0x3F) << 12)
	       | ((data[3] & 0x3F) << 6)
	       |  (data[4] & 0x3F);
    } else if (*data < 0xFE) {
	/* 
	 * 6-byte sequence, 4000000-7FFFFFFF codepoints. 
	 */

	return   ((data[0] & 0x03) << 30)
	       | ((data[1] & 0x3F) << 24)
	       | ((data[2] & 0x3F) << 18)
	       | ((data[3] & 0x3F) << 12)
	       | ((data[4] & 0x3F) << 6)
	       |  (data[5] & 0x3F);
    }

    /* 
     * Invalid sequence.
     */

    return COL_CHAR_INVALID;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewRopeFromString --
 *
 *	Create a new rope from a C string. The string is treated as an UCS1
 *	character buffer whose length is computed with strlen().
 *
 * Results:
 *	A new rope containing the character data.
 *
 * Side effects:
 *	Memory cells may be allocated.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_EmptyRope()
{
    return WORD_SMALLSTR_EMPTY;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewRopeFromString --
 *
 *	Create a new rope from a C string. The string is treated as an UCS1
 *	character buffer whose length is computed with strlen().
 *
 * Results:
 *	A new rope containing the character data.
 *
 * Side effects:
 *	Memory cells may be allocated.
 *
 *---------------------------------------------------------------------------
 */

Col_Word 
Col_NewRopeFromString(
    const char *string)		/* C string to build rope from. */
{
    return Col_NewRope(COL_UCS1, string, strlen(string));
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewRope --
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
 * Results:
 *	A new rope containing the character data.
 *
 * Side effects:
 *	Memory cells may be allocated.
 *
 *---------------------------------------------------------------------------
 */

Col_Word 
Col_NewRope(
    Col_StringFormat format,	/* Format of data in buffer. */
    const void *data,		/* Buffer containing flat data. */
    size_t byteLength)		/* Length of data in bytes. */
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
		if (length <= sizeof(Col_Word)-1) {
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
		} else if (length <= sizeof(Col_Word)-1) {
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
		} else if (length <= sizeof(Col_Word)-1) {
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_RopeLength --
 *
 *	Get or compute the char length of the rope.
 *
 * Results:
 *	The rope length in characters.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

size_t 
Col_RopeLength(
    Col_Word rope)		/* Rope to get char length for. */
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

/*
 *---------------------------------------------------------------------------
 *
 * MergeStringsProc --
 *
 *	Traversal procedure used to concatenate all portions of strings into
 *	one to fit wthin one short leaf rope.
 *
 * Results:
 *	0 if successful, else this means that the data wouldn't fit into a 
 *	short leaf rope.
 *
 * Side effects:
 *	Copy the string data into the associated MergeStringInfo fields.
 *
 *---------------------------------------------------------------------------
 */

/* 
 * Structure used to collect data during the traversal. 
 */

typedef struct MergeStringsInfo {
    Col_StringFormat format;	/* Character format for the below buffer. */
    Col_Char1 data[MAX_SHORT_LEAF_SIZE];
				/* Buffer storing the flattened data. */
    size_t length, byteLength;	/* Char and byte length so far. */
} MergeStringsInfo;

static int 
MergeStringsProc(
    size_t index,
    size_t length,
    size_t number, 
    const Col_RopeChunk *chunks,
    Col_ClientData clientData) 
{
    MergeStringsInfo *info = (MergeStringsInfo *) clientData;

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

			for (index = info->length-1; index != -1; index --) {
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_Subrope --
 *
 *	Create a new rope which is a subrope of another.
 *
 *	We try to minimize the overhead as much as possible, such as: 
 *	 - identity.
 *       - create leaf ropes for small subropes.
 *	 - subropes of subropes point to original data.
 *	 - subropes of concats point to the deepest superset subrope.
 *
 * Results:
 *	When first is past the end of the rope, or last is before first, a null
 *	rope. Else, a rope representing the subrope.
 *
 * Side effects:
 *	Memory cells may be allocated.
 *
 *---------------------------------------------------------------------------
 */

Col_Word 
Col_Subrope(
    Col_Word rope,		/* The rope to extract the subrope from. */
    size_t first, size_t last)	/* Range of subrope. */
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
	MergeStringsInfo info; 
	info.length = 0;
	info.byteLength = 0;

	if (Col_TraverseRopeChunks(1, &rope, first, length, MergeStringsProc, 
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

/*
 *---------------------------------------------------------------------------
 *
 * GetArms --
 *
 *	Get the left and right arms of a concat rope or one of its subropes.
 *
 * Results:
 *	leftPtr and rightPtr will hold the left and right arms.
 *
 * Side effects:
 *	New ropes may be created.
 *
 *---------------------------------------------------------------------------
 */

static void 
GetArms(
    Col_Word rope,		/* Rope to extract arms from. */
    Col_Word * leftPtr,		/* Returned left arm. */
    Col_Word * rightPtr)	/* Returned right arm. */
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_ConcatRopes --
 * Col_ConcatRopesA --
 * Col_ConcatRopesNV --
 *
 *	Concatenate ropes.
 *	Col_ConcatRopes concatenates two ropes.
 *	Col_ConcatRopesA concatenates several ropes given in an array, by 
 *	recursive halvings until it contains one or two elements. 
 *	Col_ConcatRopesNV is a variadic version of Col_ConcatRopesA.
 *
 *	Concatenation forms self-balanced binary trees. Each node has a depth 
 *	level. Concat nodes have a depth > 1. Subrope nodes have a depth equal
 *	to that of their source rope. Other nodes have a depth of 0. 
 *
 *	The resulting concat will have a depth equal to the largest of both 
 *	left and right arms' depth plus 1. If they differ by more than 1, then 
 *	the tree is rebalanced.
 *
 * Results:
 *	If both ropes are null, or if the resulting rope would exceed the
 *	maximum length, a null rope. Else, a rope representing the 
 *	concatenation of both ropes.
 *	Array and variadic versions return null when concatenating zero ropes,
 *	and the source rope when concatenating a single one.
 *
 * Side effects:
 *	Memory cells may be allocated.
 *
 *---------------------------------------------------------------------------
 */

/* - Binary version. */
Col_Word 
Col_ConcatRopes(
    Col_Word left,		/* Left part. */
    Col_Word right)		/* Right part. */
{
    Col_Word concatRope;	/* Resulting rope in the general case. */
    unsigned char leftDepth=0, rightDepth=0; 
				/* Respective depths of left and right ropes. */
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
	    leftDepth = WORD_SUBROPE_DEPTH(left);
	    break;

	case WORD_TYPE_CONCATROPE:
	    leftDepth = WORD_CONCATROPE_DEPTH(left);
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
	case WORD_TYPE_SUBROPE:
	    rightDepth = WORD_SUBROPE_DEPTH(right);
	    break;

	case WORD_TYPE_CONCATROPE:
	    rightDepth = WORD_CONCATROPE_DEPTH(right);
	    break;

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
	MergeStringsInfo info;

	info.length = 0;
	info.byteLength = 0;

	if (Col_TraverseRopeChunks(1, &left, 0, leftLength, MergeStringsProc, 
			&info, NULL) == 0
		&& Col_TraverseRopeChunks(1, &right, 0, rightLength, 
			MergeStringsProc, &info, NULL) == 0) {
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

    if (leftDepth > rightDepth+1) {
	/* 
	 * Left is deeper by more than 1 level, rebalance.
	 */

	unsigned char left1Depth=0, left2Depth=0;
	Col_Word left1, left2;

	ASSERT(leftDepth >= 2);

	GetArms(left, &left1, &left2);
	switch (WORD_TYPE(left1)) {
	    case WORD_TYPE_SUBROPE: left1Depth = WORD_SUBROPE_DEPTH(left1); break;
	    case WORD_TYPE_CONCATROPE: left1Depth = WORD_CONCATROPE_DEPTH(left1); break;
	}
	switch (WORD_TYPE(left2)) {
	    case WORD_TYPE_SUBROPE: left2Depth = WORD_SUBROPE_DEPTH(left2); break;
	    case WORD_TYPE_CONCATROPE: left2Depth = WORD_CONCATROPE_DEPTH(left2); break;
	}
	if (left1Depth < left2Depth) {
	    /* 
	     * Left2 is deeper, split it between both arms. 
	     *
	     * Before:
	     *          concat = (left1 + (left21+left22)) + right
	     *         /      \
	     *       left    right
	     *      /    \
	     *   left1  left2
	     *         /     \
	     *      left21  left22
	     *
	     * After:
	     *              concat = (left1 + left21) + (left22 + right)
	     *             /      \
	     *       concat        concat
	     *      /      \      /      \
	     *   left1  left21  left22  right
	     */

	    Col_Word left21, left22;
	    GetArms(left2, &left21, &left22);
	    return Col_ConcatRopes(Col_ConcatRopes(left1, left21), 
		    Col_ConcatRopes(left22, right));
	} else {
	    /* 
	     * Left1 is deeper or at the same level, rotate to right.
	     *
	     * Before:
	     *            concat = (left1 + left2) + right
	     *           /      \
	     *         left    right
	     *        /    \
	     *     left1  left2
	     *    /     \
	     *   ?       ?
	     *
	     * After:
	     *           concat = left1 + (left2 + right)
	     *          /      \
	     *     left1        concat
	     *    /     \      /      \
	     *   ?       ?   left2   right
	     */

	    return Col_ConcatRopes(left1, Col_ConcatRopes(left2, right));
	}
    } else if (leftDepth+1 < rightDepth) {
	/* 
	 * Right is deeper by more than 1 level, rebalance. 
	 */

	unsigned char right1Depth=0, right2Depth=0;
	Col_Word right1, right2;

	ASSERT(rightDepth >= 2);

	GetArms(right, &right1, &right2);
	switch (WORD_TYPE(right1)) {
	    case WORD_TYPE_SUBROPE: right1Depth = WORD_SUBROPE_DEPTH(right1); break;
	    case WORD_TYPE_CONCATROPE: right1Depth = WORD_CONCATROPE_DEPTH(right1); break;
	}
	switch (WORD_TYPE(right2)) {
	    case WORD_TYPE_SUBROPE: right2Depth = WORD_SUBROPE_DEPTH(right2); break;
	    case WORD_TYPE_CONCATROPE: right2Depth = WORD_CONCATROPE_DEPTH(right2); break;
	}
	if (right2Depth < right1Depth) {
	    /* 
	     * Right1 is deeper, split it between both arms. 
	     *
	     * Before:
	     *          concat = left + ((right11+right12) + right2)
	     *         /      \
	     *       left    right
	     *              /     \
	     *           right1  right2
	     *          /      \
	     *      right11  right12
	     *
	     * After:
	     *               concat = (left + right11) + (right12 + right2)
	     *             /        \
	     *       concat          concat
	     *      /      \        /      \
	     *    left  right11  right12  right2
	     */

	    Col_Word right11, right12;
	    GetArms(right1, &right11, &right12);
	    return Col_ConcatRopes(Col_ConcatRopes(left, right11), 
		    Col_ConcatRopes(right12, right2));
	} else {
	    /* 
	     * Right2 is deeper or at the same level, rotate to left.
	     *
	     * Before:
	     *          concat = left + (right1 + right2)
	     *         /      \
	     *       left    right
	     *              /     \
	     *           right1  right2
	     *                  /      \
	     *                 ?        ?
	     *
	     * After:
	     *            concat = (left + right1) + right2
	     *           /      \
	     *     concat        right2
	     *    /      \      /      \
	     *  left   right1  ?        ?
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

/* - Array version. */
Col_Word 
Col_ConcatRopesA(
    size_t number,		/* Number of ropes in below array. */
    const Col_Word * ropes)	/* Ropes to concatenate in order. */
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

/* - Variadic version. */
Col_Word 
Col_ConcatRopesNV(
    size_t number,		/* Number of arguments. */
    ...)			/* Remaining arguments, i.e. ropes to 
				 * concatenate in order. */
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_RopeAt --
 *
 *	Get the codepoint of the character at a given position. 
 *
 * Results:
 *	If the index is past the end of the string, the invalid codepoint
 *	COL_CHAR_INVALID, else the Unicode codepoint of the character.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

Col_Char 
Col_RopeAt(
    Col_Word rope,		/* Rope to get char from. */
    size_t index)		/* Char index. */
{
    Col_RopeIterator it;
    
    Col_RopeIterBegin(rope, index, &it);
    return Col_RopeIterAt(&it);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_RepeatRope --
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
 * Results:
 *	The repetition of the source rope.
 *
 * Side effects:
 *	New ropes may be created.
 *
 *---------------------------------------------------------------------------
 */

Col_Word 
Col_RepeatRope(
    Col_Word rope,		/* Rope to repeat. */
    size_t count)		/* Repetition factor. */
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_RopeInsert --
 *
 *	Insert a rope into another one, just before the given insertion point.
 *
 *	Only perform minimal tests to prevent overflow, basic ops should 
 *	perform further optimizations anyway.
 *
 *	As ropes are immutable, the result is a new rope.
 *
 *	Insertion past the end of the rope results in a concatenation.
 *
 * Results:
 *	The resulting rope.
 *
 * Side effects:
 *	New ropes may be created.
 *
 *---------------------------------------------------------------------------
 */

Col_Word 
Col_RopeInsert(
    Col_Word into,		/* Where to insert. */
    size_t index,		/* Index of insertion point. */
    Col_Word rope)		/* Rope to insert. */
{
    size_t ropeLength;
    if (index == 0) {
	/*
	 * Insert at begin.
	 */

	return Col_ConcatRopes(rope, into);
    }
    ropeLength = Col_RopeLength(into);
    if (index >= ropeLength) {
	/* 
	 * Insert at end; don't pad. 
	 */

	return Col_ConcatRopes(into, rope);
    }

    /* 
     * General case. 
     */

    return Col_ConcatRopes(Col_ConcatRopes(
		    Col_Subrope(into, 0, index-1), rope), 
	    Col_Subrope(into, index, ropeLength-1));
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_RopeRemove --
 *
 *	Remove a range of characters from a rope.
 *
 *	Only perform minimal tests to prevent overflow, basic ops should 
 *	perform further optimizations anyway.
 *
 *	As ropes are immutable, the result is a new rope.
 *
 * Results:
 *	The resulting rope.
 *
 * Side effects:
 *	New ropes may be created.
 *
 *---------------------------------------------------------------------------
 */

Col_Word 
Col_RopeRemove(
    Col_Word rope,		/* Rope to remove sequence from. */
    size_t first, size_t last)	/* Range of chars to remove. */
{
    size_t ropeLength;
    if (first > last) {
	/* 
	 * No-op. 
	 */

	return rope;
    }
    ropeLength = Col_RopeLength(rope);
    if (ropeLength == 0 || first >= ropeLength) {
	/*
	 * No-op.
	 */

	return rope;
    } else if (first == 0) {
	/*
	 * Trim begin.
	 */

	return Col_Subrope(rope, last+1, ropeLength-1);
    } else if (last >= ropeLength-1) {
	/* 
	 * Trim end. 
	 */

	return Col_Subrope(rope, 0, first-1);
    }

    /* 
     * General case. 
     */

    return Col_ConcatRopes(Col_Subrope(rope, 0, first-1), 
	    Col_Subrope(rope, last+1, ropeLength-1));
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_RopeReplace --
 *
 *	Replace a range of characters in a rope with another.
 *
 *	Only perform minimal tests to prevent overflow, basic ops should 
 *	perform further optimizations anyway.
 *
 *	As ropes are immutable, the result is a new rope.
 *
 * Results:
 *	The resulting rope.
 *
 * Side effects:
 *	New ropes may be created.
 *
 *---------------------------------------------------------------------------
 */

Col_Word 
Col_RopeReplace(
    Col_Word rope,		/* Original rope. */
    size_t first, size_t last,	/* Inclusive range of chars to replace. */
    Col_Word with)		/* Replacement rope. */
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_CompareRopes --
 *
 *	Compare two ropes and find the first differing characters if any. This
 *	is the rope counterpart to strncmp with extra features.
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
 * Side effects:
 *	If non-NULL and if ropes differ:
 *	    - The value pointed to by posPtr is given the position of the first 
 *	      differing character;
 *	    - The value pointed to by c1Ptr (resp. c2Ptr) is given the codepoint
 *	      of the differing character in rope1 (resp. rope2).
 *
 *---------------------------------------------------------------------------
 */

typedef struct CompareChunksInfo {
    size_t *posPtr; 
    Col_Char *c1Ptr; 
    Col_Char *c2Ptr;
} CompareChunksInfo;
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
int
Col_CompareRopes(
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
    return Col_TraverseRopeChunks(2, ropes, start, max, CompareChunksProc, 
	    &info, NULL);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_TraverseRopeChunks --
 *
 *	Iterate over the basic chunks of data, i.e. the leaf ropes.
 *
 *	For each traversed leaf, proc is called back with the opaque data as
 *	well as the position within the rope. If it returns a non-zero result 
 *	then the iteration ends. 
 *
 *	The algorithm is naturally recursive but this implementation avoids
 *	recursive calls thanks to a stack-allocated backtracking structure.
 *
 * Results:
 *	The return value of the last called proc, or -1 if no traversal was
 *	performed.
 *	If non-NULL, the value pointed by lengthPtr is incremented by the total
 *	length of the traversed list, i.e. the number of elements.
 *
 * Side effects:
 *	If non-NULL, the value pointed to by lengthPtr is updated.
 *
 *---------------------------------------------------------------------------
 */

int 
Col_TraverseRopeChunks(
    size_t number,		/* Number of ropes to traverse. */
    Col_Word *ropes,		/* Ropes to traverse. */
    size_t start,		/* Index of first character. */
    size_t max,			/* Max number of characters. */
    Col_RopeChunksTraverseProc *proc,	
				/* Callback proc called at each leaf node. */
    Col_ClientData clientData,	/* Opaque data passed as is to above proc. */
    size_t *lengthPtr)		/* If non-NULL, incremented by the total number 
				 * of elements traversed upon completion. */
{
    struct {
	/*
	 * Non-recursive backtracking structure. Since the algorithm only 
	 * recurses on concat nodes and since we know their depth, we can 
	 * allocate the needed space on the stack.
	 */

	struct {
	    int prevDepth;
	    Col_Word rope;
	    size_t max;
	} *backtracks;

	Col_Word rope;
	Col_Char4 c;
	size_t start, max;
	int maxDepth, prevDepth;
    } *info;
    size_t i;
    Col_RopeChunk *chunks;
    int type;
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
	info[i].backtracks = NULL;
	info[i].start = start;
	info[i].maxDepth = 0;
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

	    for (;;) {
		/*
		 * Descend into structure until we find a suitable leaf.
		 */

		type = WORD_TYPE(info[i].rope);
		switch (type) {
		    case WORD_TYPE_SUBROPE:
			/* 
			 * Subrope: recurse on source rope.
			 */

			info[i].start += WORD_SUBROPE_FIRST(info[i].rope);
			info[i].rope = WORD_SUBROPE_SOURCE(info[i].rope);
			continue;
            	    
		    case WORD_TYPE_CONCATROPE: {
			/* 
			 * Concat: descend into covered arms.
			 */

			int depth;
			size_t leftLength = WORD_CONCATROPE_LEFT_LENGTH(
				info[i].rope);
			if (leftLength == 0) {
			    leftLength = Col_RopeLength(
				    WORD_CONCATROPE_LEFT(info[i].rope));
			}
			if (info[i].start + info[i].max <= leftLength) {
			    /* 
			     * Recurse on left arm only. 
			     */

			    info[i].rope = WORD_CONCATROPE_LEFT(info[i].rope);
			    continue;
			}
			if (info[i].start >= leftLength) {
			    /* 
			     * Recurse on right arm only. 
			     */

			    info[i].start -= leftLength;
			    info[i].rope = WORD_CONCATROPE_RIGHT(info[i].rope);
			    continue;
			} 

			/*
			 * Push right onto stack and recurse on left.
			 */

			depth = WORD_CONCATROPE_DEPTH(info[i].rope);
			if (!info[i].backtracks) {
			    ASSERT(info[i].maxDepth == 0);
			    ASSERT(info[i].prevDepth == INT_MAX);
			    info[i].maxDepth = depth;
			    info[i].backtracks = alloca(
				    sizeof(*info[i].backtracks) * depth);
			} else {
			    ASSERT(depth <= info[i].maxDepth);
			}
			info[i].backtracks[depth-1].prevDepth 
				= info[i].prevDepth;
			info[i].backtracks[depth-1].rope 
				= WORD_CONCATROPE_RIGHT(info[i].rope);
			info[i].backtracks[depth-1].max = info[i].max
				- (leftLength-info[i].start);
			info[i].prevDepth = depth;

			info[i].max = leftLength-info[i].start;
			info[i].rope = WORD_CONCATROPE_LEFT(info[i].rope);
			continue;
		    }

		    case WORD_TYPE_WRAP:
			/* 
			 * Recurse on source.
			 */

			info[i].rope = WORD_WRAP_SOURCE(info[i].rope);
			continue;
		}
		break;
	    }

	    /*
	     * Get leaf data.
	     */

	    switch (type) {
		case WORD_TYPE_CHAR:
		    ASSERT(info[i].start == 0);
		    ASSERT(info[i].max == 1);
		    chunks[i].format = COL_UCS4;
		    info[i].c = WORD_CHAR_GET(info[i].rope);
		    chunks[i].data = &info[i].c;
		    break;

		case WORD_TYPE_SMALLSTR:
		    chunks[i].format = COL_UCS1;
		    chunks[i].data = WORD_SMALLSTR_DATA(info[i].rope)
			    + info[i].start;
		    break;

		case WORD_TYPE_UCSSTR: {
		    /* 
		     * Fixed-width flat strings: traverse range of chars.
		     *
		     * Note: the integer values of the format enums match the char size.
		     */

		    chunks[i].format = WORD_UCSSTR_FORMAT(info[i].rope);
		    chunks[i].data = WORD_UCSSTR_DATA(info[i].rope)
			    + info[i].start*chunks[i].format;
		    break;
		}
            	    
		case WORD_TYPE_UTF8STR:
		    /*
		     * Variable-width flat string.
		     */

		    chunks[i].format = COL_UTF8;
		    chunks[i].data = Col_Utf8CharAddr(
			    WORD_UTF8STR_DATA(info[i].rope), info[i].start, 
			    WORD_UTF8STR_LENGTH(info[i].rope), 
			    WORD_UTF8STR_BYTELENGTH(info[i].rope));
		    break;

		case WORD_TYPE_CUSTOM: {
		    Col_CustomRopeType *typeInfo 
			    = (Col_CustomRopeType *) WORD_TYPEINFO(info[i].rope);
		    ASSERT(typeInfo->type.type == COL_ROPE);
		    if (typeInfo->chunkAtProc) {
			typeInfo->chunkAtProc(info[i].rope, info[i].start,
			    info[i].max, &info[i].max, chunks+i);
		    } else {
			/*
			 * Traverse chars individually.
			 */

			info[i].max = 1;
			chunks[i].format = COL_UCS4;
			info[i].c = typeInfo->charAtProc(info[i].rope, 
				info[i].start);
			chunks[i].data = &info[i].c;
		    }
		    break;
		}

		/* WORD_TYPE_UNKNOWN */

		default:
		    /* CANTHAPPEN */
		    ASSERT(0);
		    return 0;
	    }
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
	 * Call proc on leaves' data.
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

	    size_t nbEnd = 0;
	    for (i=0; i < number; i++) {
		ASSERT(info[i].max >= max);
		info[i].max -= max;
		if (info[i].max > 0) {
		    info[i].start += max;
		} else if (info[i].prevDepth == INT_MAX) {
		    /*
		     * Already at toplevel => end of list.
		     */

		    info[i].rope = WORD_NIL;
		    nbEnd++;
		} else {
		    /*
		     * Reached end of leaf, backtrack.
		     */

		    ASSERT(info[i].backtracks);
		    info[i].rope = info[i].backtracks[info[i].prevDepth-1].rope;
		    info[i].max = info[i].backtracks[info[i].prevDepth-1].max;
		    info[i].start = 0;
		    info[i].prevDepth 
			    = info[i].backtracks[info[i].prevDepth-1].prevDepth;
		}
	    }
	    if (nbEnd == number) {
		/*
		 * Reached end of all lists, stop there.
		 */

		return 0;
	    }
	}
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_RopeIterBegin --
 *
 *	Initialize the rope iterator so that it points to the index-th
 *	character within the rope.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If index points past the end of the rope, the iterator is initialized 
 *	to the end iterator (i.e. whose rope field is NULL), else it points to 
 *	the character within the rope.
 *
 *---------------------------------------------------------------------------
 */

void
Col_RopeIterBegin(
    Col_Word rope,		/* Rope to iterate over. */
    size_t index,		/* Index of character. */
    Col_RopeIterator *it)	/* Iterator to initialize. */
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

/*
 *---------------------------------------------------------------------------
 *
 * IterAtChar --
 * IterAtSmallStr --
 * IterAtUcs1 --
 * IterAtUcs2 --
 * IterAtUcs4 --
 * IterAtUtf8 --
 *
 *	Character accessors for rope iterators.
 *
 * Results:
 *	The character at the given position in the leaf.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

static Col_Char IterAtChar(
    Col_Word leaf, 
    size_t index
)
{
    ASSERT(index == 0);
    return WORD_CHAR_GET(leaf);
}
static Col_Char IterAtSmallStr(
    Col_Word leaf, 
    size_t index
)
{
    return WORD_SMALLSTR_DATA(leaf)[index];
}
static Col_Char IterAtUcs1(
    Col_Word leaf, 
    size_t index
)
{
    return ((Col_Char1 *) WORD_UCSSTR_DATA(leaf))[index];
}
static Col_Char IterAtUcs2(
    Col_Word leaf, 
    size_t index
)
{
    return ((Col_Char2 *) WORD_UCSSTR_DATA(leaf))[index];
}
static Col_Char IterAtUcs4(
    Col_Word leaf, 
    size_t index
)
{
    return ((Col_Char4 *) WORD_UCSSTR_DATA(leaf))[index];
}
static Col_Char IterAtUtf8(
    Col_Word leaf, 
    size_t index
)
{
    return Col_Utf8CharAt(WORD_UTF8STR_DATA(leaf) + index);
}

/*
 *---------------------------------------------------------------------------
 *
 * ColRopeIterUpdateTraversalInfo --
 *
 *	Get the deepest subropes needed to access the current char designated
 *	by the iterator.
 *
 *	Iterators point to the leaf containing the current char. To avoid 
 *	rescanning the whole tree when leaving this leaf, it also stores a
 *	higher level subrope containing this leaf, so that traversing all this
 *	subrope's children is fast. The subrope is the leaf's highest ancestor
 *	with a maximum depth of MAX_ITERATOR_SUBROPE_DEPTH. Some indices are
 *	stored along with this subrope in a way that traversal can be resumed
 *	quickly: If the current index is withing the subrope's range of 
 *	validity, then traversal starts at the subrope, else it restarts from
 *	the root.
 *
 *	Traversal info is updated lazily, each time actual char data needs to
 *	be retrieved. This means that a blind iteration over an arbitrarily 
 *	complex rope is no more computationally intensive than over a flat 
 *	string.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Iterator is updated.
 *
 *---------------------------------------------------------------------------
 */

void
ColRopeIterUpdateTraversalInfo(
    Col_RopeIterator *it)	/* The iterator to update. */
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_RopeIterCompare --
 *
 *	Compare two iterators by their respective positions.
 *
 * Results:
 *	-  < 0 if it1 is before it2
 *	-  > 0 if it1 is after it2
 *	-  0 if they are equal
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

int
Col_RopeIterCompare(
    Col_RopeIterator *it1,	/* First iterator. */
    Col_RopeIterator *it2)	/* Second iterator. */
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_RopeIterForward --
 *
 *	Move the iterator to the nb-th next character.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Update the iterator.
 *
 *---------------------------------------------------------------------------
 */

void
Col_RopeIterForward(
    Col_RopeIterator *it,	/* The iterator to move. */
    size_t nb)			/* Offset. */
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_RopeIterBackward --
 *
 *	Move the iterator to the nb-th previous character.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Update the iterator.
 *
 *---------------------------------------------------------------------------
 */

void
Col_RopeIterBackward(
    Col_RopeIterator *it,	/* The iterator to move. */
    size_t nb)			/* Offset. */
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

/*
 *---------------------------------------------------------------------------
 *
 * Col_RopeIterMoveTo --
 *
 *	Move the iterator to the given absolute position.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Update the iterator.
 *
 *---------------------------------------------------------------------------
 */

void
Col_RopeIterMoveTo(
    Col_RopeIterator *it,	/* The iterator to move. */
    size_t index)		/* Position. */
{
    if (index > it->index) {
	Col_RopeIterForward(it, index - it->index);
    } else if (index < it->index) {
	Col_RopeIterBackward(it, it->index - index);
    }
}
