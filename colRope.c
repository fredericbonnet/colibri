#include "colibri.h"
#include "colInternal.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <malloc.h> /* For alloca */


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

static Col_RopeChunkEnumProc MergeStringsProc;
static void		GetArms(Col_Rope rope, Col_Rope * leftPtr, 
			    Col_Rope * rightPtr);
static void		UpdateTraversalInfo(Col_RopeIterator *it);
static const Col_Char1 * Utf8CharAddr(const Col_Char1 * data, 
			    size_t index, size_t length, size_t byteLength);
static Col_Char		Utf8CharValue(const Col_Char1 * data);


/*
 *---------------------------------------------------------------------------
 *
 * Col_NewRope --
 *
 *	Create a new rope from flat character data. This can either be a 
 *	single leaf rope containing the whole data, or a concatenation of 
 *	leaves if data is too large.
 *
 *	If the original string is= too large, data may span several 
 *	multi-cell leaf ropes. In this case we recursively split the data in 
 *	half and build a concat tree.
 *
 * Results:
 *	A new rope containing the character data.
 *
 * Side effects:
 *	Memory cells are allocated.
 *
 *---------------------------------------------------------------------------
 */

Col_Rope 
Col_NewRope(
    Col_StringFormat format,	/* Format of data in buffer. */
    const void *data,		/* Buffer containing flat data. */
    size_t byteLength)		/* Length of data in bytes. */
{
    size_t half=0;		/* Index of the split point. */

    /* 
     * Quick cases. 
     */

    if (byteLength == 0) {
	/* Empty string. */
	return STRING_EMPTY;
    }

    if (format == COL_UTF8) {
	const Col_Char1 *source = (const Col_Char1 *) data, *p;
	if (byteLength <= UTF8_MAX_SIZE) {
	    /*
	     * String fits into one multi-cell leaf rope. We know the byte 
	     * length, now get the char length by counting the number of 
	     * sequence starts (!= 10xxxxxx). 
	     */

	    Col_Rope rope;
	    size_t length = 0;
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
	    
	    rope = (Col_Rope) AllocCells(NB_CELLS(ROPE_UTF8_HEADER_SIZE
		    + byteLength));
	    ROPE_SET_TYPE(rope, ROPE_TYPE_UTF8);
	    ROPE_UTF8_LENGTH(rope) = (unsigned short) length;
	    ROPE_UTF8_BYTELENGTH(rope) = (unsigned short) byteLength;
	    memcpy((void *) ROPE_UTF8_DATA(rope), source, byteLength);
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
	 */

	if (byteLength <= UCS_MAX_SIZE) {
	    /* 
	     * String fits into one multi-cell leaf rope. 
	     */

	    Col_Rope rope = (Col_Rope) AllocCells(NB_CELLS(ROPE_UCS_HEADER_SIZE
		    + byteLength));
	    switch (format) {
		case COL_UCS1:
		    ROPE_SET_TYPE(rope, ROPE_TYPE_UCS1);
		    ROPE_UCS_LENGTH(rope) = (unsigned short) byteLength
			    / sizeof(Col_Char1);
		    break;

		case COL_UCS2:
		    ROPE_SET_TYPE(rope, ROPE_TYPE_UCS2);
		    ROPE_UCS_LENGTH(rope) = (unsigned short) byteLength
			    / sizeof(Col_Char2);
		    break;

		case COL_UCS4:
		    ROPE_SET_TYPE(rope, ROPE_TYPE_UCS4);
		    ROPE_UCS_LENGTH(rope) = (unsigned short) byteLength
			    / sizeof(Col_Char4);
		    break;
	    }
	    memcpy((void *) ROPE_UCS_DATA(rope), data, byteLength);
	    return rope;
	} 
        
	/* 
	 * Split data in half at char boundary. 
	 */

	switch (format) {
	    case COL_UCS1:
		half = ((byteLength/sizeof(Col_Char1))/2)*sizeof(Col_Char1);
		break;

	    case COL_UCS2:
		half = ((byteLength/sizeof(Col_Char2))/2)*sizeof(Col_Char2);
		break;

	    case COL_UCS4:
		half = ((byteLength/sizeof(Col_Char4))/2)*sizeof(Col_Char4);
		break;
	}
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
    Col_Rope rope)		/* Rope to get char length for. */
{
    RESOLVE_ROPE(rope);

    switch (ROPE_TYPE(rope)) {
	case ROPE_TYPE_NULL:
	case ROPE_TYPE_EMPTY:
	    /* 
	     * Null or empty strings have a zero length.
	     */

	    return 0;

	case ROPE_TYPE_C:
	    /* 
	     * C string are NUL-terminated.
	     */

	    return strlen(rope);

	/* 
	 * Flat string, fixed or variable.
	 */

	case ROPE_TYPE_UCS1:
	case ROPE_TYPE_UCS2:
	case ROPE_TYPE_UCS4:
	    return ROPE_UCS_LENGTH(rope);

	case ROPE_TYPE_UTF8:
	    return ROPE_UTF8_LENGTH(rope);

	case ROPE_TYPE_SUBROPE:
	    /* 
	     * Subrope length is the range width.
	     */

	    return ROPE_SUBROPE_LAST(rope)-ROPE_SUBROPE_FIRST(rope)+1;
	    
	case ROPE_TYPE_CONCAT:
	    /* 
	     * Concat length is the sum of its arms (known at creation time).
	     */

	    return ROPE_CONCAT_LENGTH(rope);

	case ROPE_TYPE_CUSTOM:
	    /*
	     * Custom ropes.
	     */

	    return ROPE_CUSTOM_TYPE(rope)->lengthProc(rope);

	/* ROPE_TYPE_UNKNOWN */
    }

    /* CANTHAPPEN */
    return 0;
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
    Col_StringFormat format, 
    const void *data, 
    size_t length, 
    size_t byteLength, 
    Col_ClientData clientData) 
{
    MergeStringsInfo *info = (MergeStringsInfo *) clientData;
    if (info->length == 0) {
	info->format = format;
    }
    if (info->format == COL_UTF8) {
	if (format != COL_UTF8) {
	    /* 
	     * Don't merge UTF-8 with fixed-width formats. 
	     */

	    return -1;
	}
	if (info->byteLength + byteLength 
		> MAX_SHORT_LEAF_SIZE - ROPE_UTF8_HEADER_SIZE) {
	    /* 
	     * Data won't fit. 
	     */

	    return -1;
	}

	/* 
	 * Append data. 
	 */

	memcpy(info->data+info->byteLength, data, byteLength);
	info->length += length;
	info->byteLength += byteLength;
	return 0;
    } else {
	size_t index;		/* For upconversions. */

	if (format == COL_UTF8) {
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
		switch (format) {
		    case COL_UCS2:
			if ((info->length + length) * sizeof(Col_Char2)
				> MAX_SHORT_LEAF_SIZE - ROPE_UCS_HEADER_SIZE) {
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
				> MAX_SHORT_LEAF_SIZE - ROPE_UCS_HEADER_SIZE) {
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
		switch (format) {
		    case COL_UCS1:
			if (info->byteLength + length*sizeof(Col_Char2)
				> MAX_SHORT_LEAF_SIZE - ROPE_UCS_HEADER_SIZE) {
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
				    = ((Col_Char1 *) data)[index];
			}
			info->length += length;
			info->byteLength += length*sizeof(Col_Char2);
			return 0;

		    case COL_UCS4:
			if ((info->length + length) * sizeof(Col_Char4)
				> MAX_SHORT_LEAF_SIZE - ROPE_UCS_HEADER_SIZE) {
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
		switch (format) {
		    case COL_UCS1:
			if (info->byteLength + length*sizeof(Col_Char4)
				> MAX_SHORT_LEAF_SIZE - ROPE_UCS_HEADER_SIZE) {
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
				    = ((Col_Char1 *) data)[index];
			}
			info->length += length;
			info->byteLength += length*sizeof(Col_Char4);
			return 0;

		    case COL_UCS2:
			if (info->byteLength + length*sizeof(Col_Char4)
				> MAX_SHORT_LEAF_SIZE - ROPE_UCS_HEADER_SIZE) {
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
				    = ((Col_Char2 *) data)[index];
			}
			info->length += length;
			info->byteLength += length*sizeof(Col_Char4);
			return 0;
		}
		break;
	}

	ASSERT(info->format == format);

	if (info->byteLength + byteLength 
		> MAX_SHORT_LEAF_SIZE - ROPE_UCS_HEADER_SIZE) {
	    /* 
	     * Data won't fit. 
	     */

	    return -1;
	}

	/* 
	 * Append data. 
	 */

	memcpy(info->data+info->byteLength, data, byteLength);
	info->length += length;
	info->byteLength += byteLength;
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

Col_Rope 
Col_Subrope(
    Col_Rope rope,		/* The rope to extract the subrope from. */
    size_t first, size_t last)	/* Range of subrope. */
{
    Col_Rope subrope;		/* Resulting rope in the general case. */
    size_t ropeLength;		/* Length of source rope. */
    unsigned char depth=0;	/* Depth of source rope. */
    size_t length;		/* Length of resulting subrope. */

    RESOLVE_ROPE(rope);

    /* 
     * Quick cases. 
     */

    if (last < first) {
	return NULL;
    }

    ropeLength = Col_RopeLength(rope);
    if (first >= ropeLength) {
	return NULL;
    }

    /* 
     * Truncate overlong subropes. (Note: at this point ropeLength > 1)
     */

    if (last >= ropeLength) {
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

    switch (ROPE_TYPE(rope)) {
	case ROPE_TYPE_C:
	    if (length == ropeLength) {
		/* 
		 * Subrope is the tail of the source string from index. 
		 * Simply point to the subtring within the original data. 
		 */

		return rope+first;
	    }
	    break;

	/* 
	 * Handle fixed width leaf ropes directly; this will be faster than the 
	 * generic case below. 
	 */

	case ROPE_TYPE_UCS1:
	    if (length <= MAX_SHORT_LEAF_SIZE - ROPE_UCS_HEADER_SIZE) {
		return Col_NewRope(COL_UCS1, ROPE_UCS1_DATA(rope)+first, 
			length);
	    }
	    break;

	case ROPE_TYPE_UCS2:
	    if (length*2 <= MAX_SHORT_LEAF_SIZE - ROPE_UCS_HEADER_SIZE) {
		return Col_NewRope(COL_UCS2, ROPE_UCS2_DATA(rope)+first, 
			length*2);
	    }
	    break;

	case ROPE_TYPE_UCS4:
	    if (length*4 <= MAX_SHORT_LEAF_SIZE - ROPE_UCS_HEADER_SIZE) {
		return Col_NewRope(COL_UCS2, ROPE_UCS4_DATA(rope)+first, 
			length*4);
	    }
	    break;

	case ROPE_TYPE_SUBROPE:
	    /* 
	     * Point to original source. 
	     */

	    return Col_Subrope(ROPE_SUBROPE_SOURCE(rope), 
		    ROPE_SUBROPE_FIRST(rope)+first, 
		    ROPE_SUBROPE_FIRST(rope)+last);

	case ROPE_TYPE_CONCAT: {
	    /* 
	     * Try to find the deepest superset of the subrope. 
	     */

	    size_t leftLength = ROPE_CONCAT_LEFT_LENGTH(rope);
	    if (leftLength == 0) {
		leftLength = Col_RopeLength(ROPE_CONCAT_LEFT(rope));
	    }
	    if (last < leftLength) {
		/* 
		 * Left arm is a superset of subrope. 
		 */

		return Col_Subrope(ROPE_CONCAT_LEFT(rope), first, last);
	    } else if (first >= leftLength) {
		/*
		 * Right arm is a superset of subrope. 
		 */

		return Col_Subrope(ROPE_CONCAT_RIGHT(rope), first-leftLength, 
			last-leftLength);
	    }
	    depth = ROPE_CONCAT_DEPTH(rope);
	    break;
	}

	case ROPE_TYPE_CUSTOM:
	    if (ROPE_CUSTOM_TYPE(rope)->subropeProc) {
		if ((subrope = ROPE_CUSTOM_TYPE(rope)->subropeProc(rope, 
			first, last)) != NULL) {
		    return subrope;
		}
	    }
	    break;

	/* ROPE_TYPE_UNKNOWN */
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

	if (Col_TraverseRopeChunks(rope, first, length, MergeStringsProc, 
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

    subrope = (Col_Rope) AllocCells(1);
    ROPE_SUBROPE_INIT(subrope, depth, rope, first, last);

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
    Col_Rope rope,		/* Rope to extract arms from. */
    Col_Rope * leftPtr,		/* Returned left arm. */
    Col_Rope * rightPtr)	/* Returned right arm. */
{
    if (ROPE_TYPE(rope) == ROPE_TYPE_SUBROPE) {
	/* Create one subrope for each concat arm. */
	Col_Rope source = ROPE_SUBROPE_SOURCE(rope);
	size_t leftLength;

	ASSERT(ROPE_TYPE(source) == ROPE_TYPE_CONCAT);
	ASSERT(ROPE_SUBROPE_DEPTH(rope) >= 1);
	ASSERT(ROPE_CONCAT_DEPTH(source) == ROPE_SUBROPE_DEPTH(rope));

	leftLength = ROPE_CONCAT_LEFT_LENGTH(source);
	if (leftLength == 0) {
	    leftLength = Col_RopeLength(ROPE_CONCAT_LEFT(source));
	}
	*leftPtr = Col_Subrope(ROPE_CONCAT_LEFT(source), 
		ROPE_SUBROPE_FIRST(rope), leftLength-1);
	*rightPtr = Col_Subrope(ROPE_CONCAT_RIGHT(source), 0, 
		ROPE_SUBROPE_LAST(rope)-leftLength);
    } else {
	ASSERT(ROPE_TYPE(rope) == ROPE_TYPE_CONCAT);
	ASSERT(ROPE_CONCAT_DEPTH(rope) >= 1);

	*leftPtr  = ROPE_CONCAT_LEFT(rope);
	*rightPtr = ROPE_CONCAT_RIGHT(rope);
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
Col_Rope 
Col_ConcatRopes(
    Col_Rope left,		/* Left part. */
    Col_Rope right)		/* Right part. */
{
    Col_Rope concatRope;	/* Resulting rope in the general case. */
    unsigned char leftDepth=0, rightDepth=0; 
				/* Respective depths of left and right ropes. */
    size_t leftLength, rightLength;
				/* Respective lengths. */

    RESOLVE_ROPE(left);
    RESOLVE_ROPE(right);

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

    /* 
     * Handle quick cases and get input node depths. 
     */
    
    switch (ROPE_TYPE(left)) {
	case ROPE_TYPE_NULL:
	    /* 
	     * Concat is a no-op on right. 
	     */

	    return right;

	case ROPE_TYPE_EMPTY:
	    /* 
	     * Distinguish between empty+null vs. empty+non-null so that concat
	     * will return a non-null result. 
	     */

	    return (ROPE_TYPE(right)==ROPE_TYPE_NULL?left:right);

	case ROPE_TYPE_SUBROPE:
	    if (ROPE_TYPE(right) == ROPE_TYPE_SUBROPE
		    && ROPE_SUBROPE_SOURCE(left) == ROPE_SUBROPE_SOURCE(right) 
		    && ROPE_SUBROPE_LAST(left)+1 == ROPE_SUBROPE_FIRST(right)) {
		/* 
		 * Merge if left and right are adjacent subropes. This allows
		 * for fast consecutive insertions/removals at a given index.
		 */

		return Col_Subrope(ROPE_SUBROPE_SOURCE(left), 
			ROPE_SUBROPE_FIRST(left), ROPE_SUBROPE_LAST(right));
	    }
	    leftDepth = ROPE_SUBROPE_DEPTH(left);
	    break;

	case ROPE_TYPE_CONCAT:
	    leftDepth = ROPE_CONCAT_DEPTH(left);
	    break;

	case ROPE_TYPE_CUSTOM:
	    if (ROPE_CUSTOM_TYPE(left)->concatProc) {
		if ((concatRope = ROPE_CUSTOM_TYPE(left)->concatProc(left, 
			right)) != NULL) {
		    return concatRope;
		}
	    }
	    break;

	/* ROPE_TYPE_UNKNOWN */
    }
    switch (ROPE_TYPE(right)) {
	case ROPE_TYPE_NULL:
	case ROPE_TYPE_EMPTY:
	    /* 
	     * Concat is a no-op on left. Special cases with null are already 
	     * handled above. 
	     */

	    return left;

	case ROPE_TYPE_SUBROPE:
	    rightDepth = ROPE_SUBROPE_DEPTH(right);
	    break;

	case ROPE_TYPE_CONCAT:
	    rightDepth = ROPE_CONCAT_DEPTH(right);
	    break;

	case ROPE_TYPE_CUSTOM:
	    if (ROPE_CUSTOM_TYPE(right)->concatProc) {
		if ((concatRope = ROPE_CUSTOM_TYPE(right)->concatProc(left, 
			right)) != NULL) {
		    return concatRope;
		}
	    }
	    break;

	/* ROPE_TYPE_UNKNOWN */
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

	if (Col_TraverseRopeChunks(left, 0, leftLength, MergeStringsProc, 
			&info, NULL) == 0
		&& Col_TraverseRopeChunks(right, 0, rightLength, 
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
	Col_Rope left1, left2;

	ASSERT(leftDepth >= 2);

	GetArms(left, &left1, &left2);
	switch (ROPE_TYPE(left1)) {
	    case ROPE_TYPE_SUBROPE: left1Depth = ROPE_SUBROPE_DEPTH(left1); break;
	    case ROPE_TYPE_CONCAT: left1Depth = ROPE_CONCAT_DEPTH(left1); break;
	}
	switch (ROPE_TYPE(left2)) {
	    case ROPE_TYPE_SUBROPE: left2Depth = ROPE_SUBROPE_DEPTH(left2); break;
	    case ROPE_TYPE_CONCAT: left2Depth = ROPE_CONCAT_DEPTH(left2); break;
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

	    Col_Rope left21, left22;
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
	Col_Rope right1, right2;

	ASSERT(rightDepth >= 2);

	GetArms(right, &right1, &right2);
	switch (ROPE_TYPE(right1)) {
	    case ROPE_TYPE_SUBROPE: right1Depth = ROPE_SUBROPE_DEPTH(right1); break;
	    case ROPE_TYPE_CONCAT: right1Depth = ROPE_CONCAT_DEPTH(right1); break;
	}
	switch (ROPE_TYPE(right2)) {
	    case ROPE_TYPE_SUBROPE: right2Depth = ROPE_SUBROPE_DEPTH(right2); break;
	    case ROPE_TYPE_CONCAT: right2Depth = ROPE_CONCAT_DEPTH(right2); break;
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

	    Col_Rope right11, right12;
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

    concatRope = (Col_Rope) AllocCells(1);
    ROPE_CONCAT_INIT(concatRope, (leftDepth>rightDepth?leftDepth:rightDepth)
	    + 1, leftLength + rightLength, leftLength, left, right);

    return concatRope;
}

/* - Array version. */
Col_Rope 
Col_ConcatRopesA(
    size_t number,		/* Number of ropes in below array. */
    const Col_Rope * ropes)	/* Ropes to concatenate in order. */
{
    size_t half;

    /* 
     * Quick cases.
     */

    if (number == 0) {return NULL;}
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
Col_Rope 
Col_ConcatRopesNV(
    size_t number,		/* Number of arguments. */
    ...)			/* Remaining arguments, i.e. ropes to 
				 * concatenate in order. */
{
    size_t i;
    va_list args;
    Col_Rope *ropes;
    
    /* 
     * Convert vararg list to array. Use alloca since a vararg list is 
     * typically small. 
     */

    ropes = alloca(number*sizeof(Col_Rope));
    va_start(args, number);
    for (i=0; i < number; i++) {
	ropes[i] = va_arg(args, Col_Rope);
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
    Col_Rope rope,		/* Rope to get char from. */
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

Col_Rope 
Col_RepeatRope(
    Col_Rope rope,		/* Rope to repeat. */
    size_t count)		/* Repetition factor. */
{
    /* Quick cases. */
    if (count == 0) {return NULL;}
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

Col_Rope 
Col_RopeInsert(
    Col_Rope into,		/* Where to insert. */
    size_t index,		/* Index of insertion point. */
    Col_Rope rope)		/* Rope to insert. */
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

Col_Rope 
Col_RopeRemove(
    Col_Rope rope,		/* Rope to remove sequence from. */
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

Col_Rope 
Col_RopeReplace(
    Col_Rope rope,		/* Original rope. */
    size_t first, size_t last,	/* Inclusive range of chars to replace. */
    Col_Rope with)		/* Replacement rope. */
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
 * Col_TraverseRopeChunks --
 *
 *	Iterate over the basic chunks of data, i.e. the leaf ropes.
 *
 *	For each traversed leaf, proc is called back with the opaque data as
 *	well as the position within the rope. If it returns a non-zero result 
 *	then the iteration ends. 
 *
 * Results:
 *	The return value of the last called proc, or -1 if no traversal was
 *	performed.
 *	If non-NULL, the value pointed by lengthPtr is the total length of the 
 *	traversed string.
 *
 * Side effects:
 *	If non-NULL, the value pointed to by lengthPtr is overwritten.
 *
 *---------------------------------------------------------------------------
 */

int 
Col_TraverseRopeChunks(
    Col_Rope rope,		/* Rope to traverse. */
    size_t start,		/* Index of first character. */
    size_t max,			/* Max number of characters. */
    Col_RopeChunkEnumProc *proc,	
				/* Callback proc called at each leaf node. */
    Col_ClientData clientData,	/* Opaque data passed as is to above proc. */
    size_t *lengthPtr)		/* If non-NULL, will hold the total number of 
				 * chars traversed upon completion. */
{
    size_t ropeLength;

    RESOLVE_ROPE(rope);

    if (lengthPtr) {
	*lengthPtr = 0;
    }

    /* 
     * Quick cases.
     */

    ropeLength = Col_RopeLength(rope);
    if (start >= ropeLength) {
	/* 
	 * Start is past the end of the string.
	 */

	return -1;
    }
    if (max > ropeLength-start) {
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

    switch (ROPE_TYPE(rope)) {
	case ROPE_TYPE_C:
	    /* 
	     * C string: traverse range of chars. 
	     */

	    if (lengthPtr) *lengthPtr = max;
	    return proc(COL_UCS1, rope+start, max, max, clientData);

	/* 
	 * Fixed-width flat strings: traverse range of chars.
	 */

	case ROPE_TYPE_UCS1:
	    if (lengthPtr) *lengthPtr = max;
	    return proc(COL_UCS1, ROPE_UCS1_DATA(rope)+start, max, 
		    max*sizeof(Col_Char1), clientData);

	case ROPE_TYPE_UCS2:
	    if (lengthPtr) *lengthPtr = max;
	    return proc(COL_UCS2, ROPE_UCS2_DATA(rope)+start, max, 
		    max*sizeof(Col_Char2), clientData);

	case ROPE_TYPE_UCS4:
	    if (lengthPtr) *lengthPtr = max;
	    return proc(COL_UCS4, ROPE_UCS4_DATA(rope)+start, max, 
		    max*sizeof(Col_Char4), clientData);
	    
	case ROPE_TYPE_UTF8:
	    /*
	     * Variable-width flat string.
	     */

	    if (lengthPtr) *lengthPtr = max;
	    if (start == 0 && max == ropeLength) {
		/* 
		 * Traversal covers the whole string. 
		 */

		return proc(COL_UTF8, ROPE_UTF8_DATA(rope), 
			ROPE_UTF8_LENGTH(rope), ROPE_UTF8_BYTELENGTH(rope), 
			clientData);
	    } else {
		/*
		 * In order to traverse a subrope, we need the start & end 
		 * bytes of the UTF-8 sequence.
		 */

		const Col_Char1 *startByte 
			= Utf8CharAddr(ROPE_UTF8_DATA(rope), start, 
				ROPE_UTF8_LENGTH(rope), 
				ROPE_UTF8_BYTELENGTH(rope)),
			*endByte = Utf8CharAddr(startByte, max, 
				ROPE_UTF8_LENGTH(rope) - start, 
				ROPE_UTF8_BYTELENGTH(rope) 
					- (startByte-ROPE_UTF8_DATA(rope)));
		return proc(COL_UTF8, startByte, max, endByte-startByte, 
			clientData);
	    }

	case ROPE_TYPE_SUBROPE:
	    /* 
	     * Subrope: propagage to source string.
	     */

	    return Col_TraverseRopeChunks(ROPE_SUBROPE_SOURCE(rope), 
		    ROPE_SUBROPE_FIRST(rope)+start, max, proc, clientData, 
		    lengthPtr);
	    
	case ROPE_TYPE_CONCAT: {
	    /* 
	     * Concat: propagate to covered arms.
	     */

	    int result;
	    size_t leftLength = ROPE_CONCAT_LEFT_LENGTH(rope);
	    if (leftLength == 0) {
		leftLength = Col_RopeLength(ROPE_CONCAT_LEFT(rope));
	    }
	    if (start >= leftLength) {
		/* 
		 * Traverse right arm only. 
		 */

		return Col_TraverseRopeChunks(ROPE_CONCAT_RIGHT(rope), 
			start-leftLength, max, proc, clientData, lengthPtr);
	    } 

	    /*
	     * Traverse left arm.
	     */

	    result = Col_TraverseRopeChunks(ROPE_CONCAT_LEFT(rope), start, max, 
		    proc, clientData, lengthPtr);

	    /*
	     * Stop if previous result is non-zero, or when traversal doesn't 
	     * cover the right arm. 
	     */

	    if (result == 0 && start + max > leftLength) {
		size_t length2;
		result = Col_TraverseRopeChunks(ROPE_CONCAT_RIGHT(rope), 0, 
			start+max-leftLength, proc, clientData, &length2);
		if (lengthPtr) {
		    *lengthPtr += length2;
		}
	    }
	    return result;
	}

	case ROPE_TYPE_CUSTOM:
	    if (ROPE_CUSTOM_TYPE(rope)->traverseProc) {
		return ROPE_CUSTOM_TYPE(rope)->traverseProc(rope, start, max, 
			proc, clientData, lengthPtr);
	    } else {
		/*
		 * Traverse chars individually.
		 */

		size_t i;
		Col_Char c;
		Col_Char1 c1;
		Col_Char2 c2;
		Col_Char4 c4;
		int result = -1;

		for (i = 0; i < max && !result; i++) {
		    c = ROPE_CUSTOM_TYPE(rope)->charAtProc(rope, start+i);

		    /*
		     * Use the shortest possible format.
		     */

		    if ((c1 = c) == c) {
			result = proc(COL_UCS1, &c1, 1, sizeof(c1), clientData);
		    } else if ((c2 = c) == c) {
			result = proc(COL_UCS2, &c2, 1, sizeof(c2), clientData);
		    } else {
			c4 = c;
			result = proc(COL_UCS4, &c, 1, sizeof(c4), clientData);
		    }
		}
		if (lengthPtr) *lengthPtr = i;
		return result;
	    }
	    break;

	/* ROPE_TYPE_UNKNOWN */
    }

    /* CANTHAPPEN */
    return 0;
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
    Col_Rope rope,		/* Rope to iterate over. */
    size_t index,		/* Index of character. */
    Col_RopeIterator *it)	/* Iterator to initialize. */
{
    int type;

    RESOLVE_ROPE(rope);

    type = ROPE_TYPE(rope);
    if (type == ROPE_TYPE_NULL || type == ROPE_TYPE_EMPTY 
	    || index >= Col_RopeLength(rope)) {
	/*
	 * End of rope.
	 */

	it->rope = NULL;

	return;
    }

    it->rope = rope;
    it->index = index;

    /*
     * Traversal info will be lazily computed.
     */

    it->traversal.subrope = NULL;
    it->traversal.leaf = NULL;
}

/*
 *---------------------------------------------------------------------------
 *
 * UpdateTraversalInfo --
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

static void
UpdateTraversalInfo(
    Col_RopeIterator *it)	/* The iterator to update. */
{
    Col_Rope rope;
    size_t first, last, offset;

    if (it->traversal.subrope && (it->index < it->traversal.first 
		    || it->index > it->traversal.last)) {
	/*
	 * Out of range.
	 */

	it->traversal.subrope = NULL;
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

    it->traversal.leaf = NULL;
    while (!it->traversal.leaf) {
	size_t subFirst=first, subLast=last;

	switch (ROPE_TYPE(rope)) {
	    case ROPE_TYPE_NULL:
	    case ROPE_TYPE_EMPTY:
		/* CANTHAPPEN */
		return;

	    case ROPE_TYPE_C:
	    case ROPE_TYPE_UCS1:
	    case ROPE_TYPE_UCS2:
	    case ROPE_TYPE_UCS4:
		/*
		 * Flat fixed-width strings are directly addressable. 
		 */

		it->traversal.leaf = rope;
		it->traversal.index.fixed = it->index - offset;
		ASSERT(it->traversal.index.fixed < ROPE_UCS_LENGTH(rope));
		break;

	    case ROPE_TYPE_UTF8: {
		/*
		 * Flat variable-width strings need both char and byte indices. 
		 */

		const Col_Char1 * data = ROPE_UTF8_DATA(rope);
		it->traversal.leaf = rope;
		it->traversal.index.var.c 
			= (unsigned short) (it->index - offset);
		it->traversal.index.var.b = (unsigned short) (Utf8CharAddr(data, 
			it->index - offset, ROPE_UTF8_LENGTH(rope), 
			ROPE_UTF8_BYTELENGTH(rope)) - data);
		ASSERT(it->traversal.index.var.c < ROPE_UTF8_LENGTH(rope));
		break;
	    }

	    case ROPE_TYPE_SUBROPE: 
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

		offset -= ROPE_SUBROPE_FIRST(rope);
		subLast = first - ROPE_SUBROPE_FIRST(rope) 
			+ ROPE_SUBROPE_LAST(rope);
		rope = ROPE_SUBROPE_SOURCE(rope);
		break;

	    case ROPE_TYPE_CONCAT: {
		size_t leftLength = ROPE_CONCAT_LEFT_LENGTH(rope);
		if (leftLength == 0) {
		    leftLength = Col_RopeLength(ROPE_CONCAT_LEFT(rope));
		}
		if (ROPE_CONCAT_DEPTH(rope) == MAX_ITERATOR_SUBROPE_DEPTH
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
		    rope = ROPE_CONCAT_LEFT(rope);
		} else {
		    /*
		     * Recurse into right arm.
		     */

		    subFirst = offset + leftLength;
		    offset += leftLength;
		    rope = ROPE_CONCAT_RIGHT(rope);
		}
		break;
	    }

	    case ROPE_TYPE_CUSTOM:
		if (ROPE_CUSTOM_TYPE(rope)->childAtProc) {
		    Col_Rope subrope;
		    size_t subOffset;
		    if ((subrope = ROPE_CUSTOM_TYPE(rope)->childAtProc(rope, 
			    it->index - offset, &subOffset)) != NULL) {
			/*
			 * Recurse into custom rope's subrope.
			 */

			subFirst = offset + subOffset;
			subLast = first + Col_RopeLength(subrope)-1;
			offset += subOffset;
			rope = subrope;
			break;
		    }
		}

		/*
		 * Address custom rope directly. 
		 */

		it->traversal.leaf = rope;
		it->traversal.index.fixed = it->index - offset;
		ASSERT(it->traversal.index.fixed < ROPE_CUSTOM_TYPE(rope)->lengthProc(rope));
		break;

	    /* ROPE_TYPE_UNKNOWN */

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
 * Col_RopeIterAt --
 *
 *	Get the codepoint of the character designated by the iterator.
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
Col_RopeIterAt(
    Col_RopeIterator *it)	/* Iterator that points to the character. */
{
    if (Col_RopeIterEnd(it)) {
	/*
	 * No such element.
	 */

	Col_Error(COL_ERROR, "Invalid rope iterator");
	return COL_CHAR_INVALID;
    }

    if (!it->traversal.leaf) {
	UpdateTraversalInfo(it);
    }

    switch (ROPE_TYPE(it->traversal.leaf)) {
	case ROPE_TYPE_C:
	    /*
	     * Bounds checking was done elsewhere.
	     */

	    return (Col_Char) ((const Col_Char1 *)(it->traversal.leaf))[it->traversal.index.fixed];

	case ROPE_TYPE_UCS1:
	    ASSERT(it->traversal.index.fixed < ROPE_UCS_LENGTH(it->traversal.leaf));
	    return (Col_Char) ROPE_UCS1_DATA(it->traversal.leaf)[it->traversal.index.fixed];

	case ROPE_TYPE_UCS2:
	    ASSERT(it->traversal.index.fixed < ROPE_UCS_LENGTH(it->traversal.leaf));
	    return (Col_Char) ROPE_UCS2_DATA(it->traversal.leaf)[it->traversal.index.fixed];

	case ROPE_TYPE_UCS4:
	    ASSERT(it->traversal.index.fixed < ROPE_UCS_LENGTH(it->traversal.leaf));
	    return (Col_Char) ROPE_UCS4_DATA(it->traversal.leaf)[it->traversal.index.fixed];

	case ROPE_TYPE_UTF8:
	    ASSERT(it->traversal.index.var.c < ROPE_UTF8_LENGTH(it->traversal.leaf));
	    return Utf8CharValue(ROPE_UTF8_DATA(it->traversal.leaf) 
		    + it->traversal.index.var.b);

	case ROPE_TYPE_CUSTOM:
	    ASSERT(it->traversal.index.fixed < ROPE_CUSTOM_TYPE(it->traversal.leaf)->lengthProc(it->traversal.leaf));
	    return ROPE_CUSTOM_TYPE(it->traversal.leaf)->charAtProc(
		    it->traversal.leaf, it->traversal.index.fixed);

	/* ROPE_TYPE_UNKNOWN */

	default: 
	    /* CANTHAPPEN */
	    ASSERT(0);
	    return COL_CHAR_INVALID;
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
    
    if (ROPE_TYPE(it->rope) == ROPE_TYPE_C) {
	size_t i;
	for (i = 0; i < nb; i++) {
	    if (!it->rope[++it->index]) {
		/*
		 * Reached NUL terminator. 
		 * 
		 * Handle C strings separately as Col_RopeLength is O(n).
		 */

		it->rope = NULL;
		return;
	    }
	}
    } else {
	if (nb >= Col_RopeLength(it->rope) - it->index) {
	    /*
	     * Reached end of rope.
	     */

	    it->rope = NULL;
	    return;
	}
	it->index += nb;
    }

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

	it->traversal.leaf = NULL;
	return;
    }

    /*
     * Update traversal info.
     */

    switch (ROPE_TYPE(it->traversal.leaf)) {
	case ROPE_TYPE_C: {
	    size_t i;
	    for (i = 0; i < nb; i++) {
		if (!it->traversal.leaf[++it->traversal.index.fixed]) {
		    /*
		     * Reached NUL terminator.
		     */

		    it->traversal.leaf = NULL;
		    break;
		}
	    }
	    break;
	}

	case ROPE_TYPE_UCS1:
	case ROPE_TYPE_UCS2:
	case ROPE_TYPE_UCS4:
	    if (nb >= ROPE_UCS_LENGTH(it->traversal.leaf) 
		    - it->traversal.index.fixed) {
		/*
		 * Reached end of leaf.
		 */

		it->traversal.leaf = NULL;
	    } else {
		it->traversal.index.fixed += nb;
	    }
	    break;

	case ROPE_TYPE_UTF8:
	    if (nb >= (size_t) (ROPE_UTF8_LENGTH(it->traversal.leaf) 
		    - it->traversal.index.var.c)) {
		/*
		 * Reached end of leaf.
		 */

		it->traversal.leaf = NULL;
	    } else {
		/*
		 * Update byte index as well.
		 */

		const Col_Char1 * data = ROPE_UTF8_DATA(it->traversal.leaf);
		size_t i;
		for (i = 0; i < nb; i++) {
		    while ((data[++it->traversal.index.var.b] & 0xC0) == 0x80); 
				/* Move byte pointer to next char boundary. */
		}

		it->traversal.index.var.c += (unsigned short) nb;
	    }
	    break;

	case ROPE_TYPE_CUSTOM:
	    if (nb >= ROPE_CUSTOM_TYPE(it->traversal.leaf)->lengthProc(it->traversal.leaf) 
		    - it->traversal.index.fixed) {
		/*
		 * Reached end of leaf.
		 */

		it->traversal.leaf = NULL;
	    } else {
		it->traversal.index.fixed += nb;
	    }
	    break;

	/* ROPE_TYPE_UNKNOWN */
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
	it->rope = NULL;
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

	it->traversal.leaf = NULL;
	return;
    }

    /*
     * Update traversal info.
     */

    switch (ROPE_TYPE(it->traversal.leaf)) {
	case ROPE_TYPE_C:
	case ROPE_TYPE_UCS1:
	case ROPE_TYPE_UCS2:
	case ROPE_TYPE_UCS4:
	    if (it->traversal.index.fixed < nb) {
		/*
		 * Reached beginning of leaf. 
		 */

		it->traversal.leaf = NULL;
	    } else {
		it->traversal.index.fixed -= nb;
	    }
	    break;

	case ROPE_TYPE_UTF8:
	    if (it->traversal.index.var.c < nb) {
		/*
		 * Reached beginning of leaf. 
		 */

		it->traversal.leaf = NULL;
	    } else {
		/*
		 * Update byte index as well.
		 */

		const Col_Char1 * data = ROPE_UTF8_DATA(it->traversal.leaf);
		size_t i;
		for (i = 0; i < nb; i++) {
		    while ((data[--it->traversal.index.var.b] & 0xC0) == 0x80);
				/* Move byte pointer to previous char boundary. */
		}

		it->traversal.index.var.c -= (unsigned short) nb;
	    }
	    break;

	case ROPE_TYPE_CUSTOM:
	    if (it->traversal.index.fixed < nb) {
		/*
		 * Reached end of leaf.
		 */

		it->traversal.leaf = NULL;
	    } else {
		it->traversal.index.fixed -= nb;
	    }
	    break;

	/* ROPE_TYPE_UNKNOWN */
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

/*
 *---------------------------------------------------------------------------
 *
 * Utf8CharAddr --
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

static const Col_Char1 * 
Utf8CharAddr(
    const Col_Char1 * data,	/* UTF-8 byte sequence. */
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
	const Col_Char1 * p = data;
	while (i != index) {
	    i++;		/* Increment char index. */
	    while ((*++p & 0xC0) == 0x80);
				/* Move byte pointer to next char boundary. */
	}
	return p;
    } else {
	/* 
	 * Second half; search backwards from end. 
	 */

	size_t i = length;
	const Col_Char1 * p = data + byteLength;
	while (i != index) {
	    i--;		/* Decrement char index. */
	    while ((*--p & 0xC0) == 0x80); 
				/* Move byte pointer to previous char 
				 * boundary. */
	}
	return p;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Utf8CharValue --
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

static Col_Char 
Utf8CharValue(
    const Col_Char1 * data)
				/* UTF-8 byte sequence. */
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
 * Col_NewCustomRope --
 *
 *	Create a new custom rope.
 *
 * Results:
 *	A new custom rope of given size.
 *
 * Side effects:
 *	Memory cells are allocated.
 *
 *---------------------------------------------------------------------------
 */

Col_Rope
Col_NewCustomRope(
    Col_RopeCustomType *type,	/* The custom type. */
    size_t size,		/* Size of data. */
    void **dataPtr)		/* Will hold a pointer to the allocated data. */
{
    Col_Rope rope;
    size_t actualSize = size;

    if (type->freeProc) {
	/* 
	 * Leave aligned space for next pointer. 
	 */

	actualSize += ROPE_CUSTOM_TRAILER_SIZE;
    }
    if (actualSize > CUSTOM_MAX_SIZE) {
	/*
	 * Not enough room.
	 */

	*dataPtr = NULL;
	return NULL;
    }
    
    rope = (Col_Rope) AllocCells(NB_CELLS(ROPE_CUSTOM_HEADER_SIZE+actualSize));
    ROPE_CUSTOM_INIT(rope, type, size);

    DeclareCustomRope(rope);

    if (dataPtr) *dataPtr = ROPE_CUSTOM_DATA(rope);
    return rope;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_GetCustomRopeInfo --
 *
 *	Get information about a custom rope (type and data).
 *
 * Results:
 *	If rope is custom, a pointer to its type, else NULL. In the former case,
 *	sizePtr and dataPtr will hold the other rope info.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

Col_RopeCustomType *
Col_GetCustomRopeInfo(
    Col_Rope rope,		/* The rope to get info for. */
    size_t *sizePtr,		/* Returned data size. */
    void **dataPtr)		/* Returned data. */
{
    RESOLVE_ROPE(rope);

    if (ROPE_TYPE(rope) != ROPE_TYPE_CUSTOM) {
	return NULL;
    }
    *sizePtr = ROPE_CUSTOM_SIZE(rope);
    *dataPtr = ROPE_CUSTOM_DATA(rope);

    return ROPE_CUSTOM_TYPE(rope);
}
