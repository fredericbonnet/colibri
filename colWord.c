#include "colibri.h"
#include "colInt.h"

#include <string.h>
#include <limits.h>

#define WORD_CHAR_MAX		0xFFFFFF

/* 
 * Max byte size of words. A word can take no more than the available size of 
 * a page. Larger words must be built by concatenation of smaller ones. 
 */

#define WORD_MAX_SIZE		(AVAILABLE_CELLS*CELL_SIZE-WORD_HEADER_SIZE)

/*
 * Prototypes for functions used only in this file.
 */

static Col_Word		UpconvertWord(Col_Word word);


/*
 *---------------------------------------------------------------------------
 *
 * Col_NewIntWord --
 *
 *	Create a new integer word.
 *
 *	If the integer value is sufficiently small, return an immediate 
 *	value instead of allocating memory.
 *
 * Results:
 *	The new word.
 *
 * Side effects:
 *	May allocate memory cells.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_NewIntWord(
    int value)			/* Integer value of the word to create. */
{
    Col_Word word;		/* Resulting word in the general case. */

    /*
     * Return integer value if possible.
     */

    if (value <= (INT_MAX>>1) && value >= (INT_MIN>>1)) {
	return WORD_SMALL_INT_NEW(value);
    }

    /*
     * Create a new integer word.
     */

    word = AllocCells(1);
    WORD_SET_TYPE_ID(word, WORD_TYPE_INT);
    WORD_VALUE(word) = NULL;
    WORD_INT_DATA(word) = value;

    return word;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewStringWord --
 *
 *	Create a new string word.
 *
 *	If the string contains a single Unicode char, or if the string is 
 *	8-bit clean and is sufficiently small, return an immediate value 
 *	instead of allocating memory.
 *
 * Results:
 *	The new word, which can be an immediate value if the string is
 *	sufficiently small, or a rope.
 *
 * Side effects:
 *	May allocate memory cells.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_NewStringWord(
    Col_StringFormat format,	/* */
    const void *data,		/* Same arguments as Col_NewRope. */
    size_t byteLength)		/* */
{
    Col_Word word;

    if (byteLength == 0) {
	/*
	 * Empty string.
	 */

	return WORD_SMALL_STRING_EMPTY;
    }

    switch (format) {
	case COL_UCS1: {
	    size_t length = byteLength;
	    if (length <= sizeof(Col_Word)-1) {
		/*
		 * Return small string value.
		 */

		WORD_SMALL_STRING_SET_LENGTH(word, length);
		memcpy(WORD_SMALL_STRING_DATA(word), data, length);
		return word;
	    }
	    break;
	}

	case COL_UCS2: {
	    size_t length = byteLength/2;
	    const unsigned short * string = data;
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
		WORD_SMALL_STRING_SET_LENGTH(word, length);
		for (i = 0; i < length; i++) {
		    if (string[i] > UCHAR_MAX) break;
		    WORD_SMALL_STRING_DATA(word)[i] = (unsigned char) string[i];
		}
		if (i == length) {
		    return word;
		}
	    }
	    break;
	}

	case COL_UCS4: {
	    size_t length = byteLength/4;
	    const unsigned int * string = data;
	    if (length == 1 && string[0] > UCHAR_MAX 
		    && string[0] <= WORD_CHAR_MAX) {
		/*
		 * Return char value.
		 */

		return WORD_CHAR_NEW(string[0]);
	    } else if (length <= sizeof(Col_Word)-1) {
		/*
		 * Return small string value if possible.
		 */

		size_t i;
		WORD_SMALL_STRING_SET_LENGTH(word, length);
		for (i = 0; i < length; i++) {
		    if (string[i] > UCHAR_MAX) break;
		    WORD_SMALL_STRING_DATA(word)[i] = (unsigned char) string[i];
		}
		if (i == length) {
		    return word;
		}
	    }
	    break;
	}
    }

    /*
     * Return a new rope.
     */

    return Col_NewRope(format, data, byteLength);
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewRopeWord --
 *
 *	Create a new rope word.
 *
 *	If the rope is a C string, creates a new word that wraps this string.
 *	Regular ropes can be used as words.
 *
 * Results:
 *	A new word or the rope itself.
 *
 * Side effects:
 *	May allocate memory cells.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_NewRopeWord(
    Col_Rope rope)		/* Rope value of the word to create. */
{
    Col_Word word;
    size_t length;

    RESOLVE_ROPE(rope);
    switch (ROPE_TYPE(rope)) {
	case ROPE_TYPE_NULL:
	    return NULL;

	case ROPE_TYPE_EMPTY:
	    return WORD_SMALL_STRING_EMPTY;

	case ROPE_TYPE_C:
	    /*
	     * Return small string value if possible.
	     */

	    length = strnlen(rope, sizeof(Col_Word));
	    if (length <= sizeof(Col_Word)-1) {
		WORD_SMALL_STRING_SET_LENGTH(word, length);
		memcpy(WORD_SMALL_STRING_DATA(word), rope, length);
		return word;
	    }

	    word = AllocCells(1);
	    WORD_SET_TYPE_ID(word, WORD_TYPE_STRING);
	    WORD_VALUE(word) = NULL;
	    WORD_STRING_DATA(word) = rope;
	    return word;

	default:
	    return rope;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewWord --
 *
 *	Create a new word.
 *
 * Results:
 *	A new rope of the given size.
 *
 * Side effects:
 *	Memory cells are allocated.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_NewWord(
    Col_WordType *type,		/* The word type. */
    size_t size,		/* Size of data. */
    void **dataPtr)		/* Will hold a pointer to the allocated data. */
{
    Col_Word word;
    size_t actualSize = size;

    if (type->freeProc) {
	/* 
	 * Leave aligned space for next pointer. 
	 */

	actualSize += WORD_TRAILER_SIZE;
    }
    if (actualSize > WORD_MAX_SIZE) {
	/*
	 * Not enough room.
	 */

	*dataPtr = NULL;
	return NULL;
    }
    
    word = AllocCells(NB_CELLS(WORD_HEADER_SIZE+actualSize));
    WORD_SET_TYPE_ADDR(word, type);
    WORD_VALUE(word) = NULL;
    if (dataPtr) *dataPtr = WORD_DATA(word);

    DeclareWord(word);

    return word;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_GetWordInfo --
 *
 *	Get information about a word (type and data).
 *
 * Results:
 *	A type ID or pointer, and additional info in dataPtr.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

Col_WordType *
Col_GetWordInfo(
    Col_Word word,		/* The word to get info for. */
    Col_WordData *dataPtr)	/* Returned data. */
{
    Col_WordType * typeInfo;
    int type;

    RESOLVE_WORD(word);
    
    type = WORD_TYPE(word);
    switch (type) {
	/*
	 * Immediate values.
	 */

	case WORD_TYPE_NULL:
	    return COL_NULL;
	    
	case WORD_TYPE_SMALL_INT:
	    dataPtr->i = WORD_SMALL_INT_GET(word);
	    return COL_INT;

	case WORD_TYPE_CHAR:
	    dataPtr->ch = WORD_CHAR_GET(word);
	    return COL_CHAR;

	case WORD_TYPE_SMALL_STRING:
	    dataPtr->str.length = WORD_SMALL_STRING_GET_LENGTH(word);
	    memcpy(dataPtr->str.data, WORD_SMALL_STRING_DATA(word), 
		    dataPtr->str.length);
	    return COL_SMALL_STRING;

	/*
	 * Predefined types.
	 */

	case WORD_TYPE_INT:
	    dataPtr->i = WORD_INT_DATA(word);
	    return COL_INT;

	case WORD_TYPE_STRING:
	    dataPtr->rope = WORD_STRING_DATA(word);
	    return COL_ROPE;

	case WORD_TYPE_ROPE:
	    RESOLVE_ROPE((Col_Rope) word);
	    dataPtr->rope = word;
	    return COL_ROPE;

	/*
	 * Regular word.
	 */

	case WORD_TYPE_REGULAR:
	    WORD_GET_TYPE_ADDR(word, typeInfo);
	    dataPtr->data = WORD_DATA(word);
	    return typeInfo;

	default:
	    return COL_NULL;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_FindWordInfo --
 *
 *	Find the value of a word with the given type.
 *
 * Results:
 *	The word or NULL if not found, and additional info in dataPtr.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_FindWordInfo(
    Col_Word word,		/* The word to get value from. */
    Col_WordType * type,	/* The required type. */
    Col_WordData *dataPtr)	/* Returned data. */
{
    Col_Word value;

    RESOLVE_WORD(word);

    value = word;
    while (value) {
	if (Col_GetWordInfo(value, dataPtr) == type) {
	    /*
	     * Found !
	     */

	    return value;
	}

	if (IS_IMMEDIATE(value) || !IS_WORD(value)) {
	    /*
	     * End of chain.
	     */

	    break;
	}

	value = WORD_VALUE(value);
	if (value == word) {
	    /*
	     * Looped back.
	     */

	    break;
	}
    }
    return NULL;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_GetWordValue --
 *
 *	Get the word's underlying value.
 *
 *	Values can be chained, i.e. the value can itself be a word with a
 *	value. To iterate over the chain, simply call this function several
 *	times with the intermediary results until it returns NULL.
 *
 * Results:
 *	A value, which can be a rope, an immediate value, or another word.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_GetWordValue(
    Col_Word word)		/* The word to get value for. */
{
    if (IS_IMMEDIATE(word) || !IS_WORD(word)) {
	/*
	 * Ropes or immediate values have no underlying value.
	 */

	return NULL;
    } else {
	RESOLVE_WORD(word);
	return WORD_VALUE(word);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_AddWordValue --
 *
 *	Add a value to a word.
 *
 * Results:
 *	The modified word, which can be the original one. This in turn must be
 *	declared as child of its original container.
 *
 * Side effects:
 *	Modify the value chain, may allocate new words.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_AddWordValue(
    Col_Word word,		/* The word to append value to. */
    Col_Word value)		/* The value to append. */
{
    /*
     * Quick cases.
     */

    if (!value) {
	return word;
    } else if (!word) {
	return value;
    }

    if (IS_IMMEDIATE(word) || !IS_WORD(word)) {
	if (!IS_IMMEDIATE(value) && IS_WORD(value)) {
	    /*
	     * Reverse: add word to value.
	     */

	    return Col_AddWordValue(value, word);
	}

	/*
	 * Upconvert word so that it can store the value.
	 */

	word = UpconvertWord(word);
    } else {
	RESOLVE_WORD(word);
    }

    if (IS_IMMEDIATE(value) || !IS_WORD(value)) {
	if (!WORD_VALUE(word)) {
	    /*
	     * Word has no current value, simply add the new value.
	     */

	    WORD_VALUE(word) = value;
	    Col_DeclareChildWord(word, value);
	    return word;
	}

	/*
	 * Upconvert value as well.
	 */

	value = UpconvertWord(value);
    } else {
	RESOLVE_WORD(value);
    }

    /*
     * Valueless words have a NULL value. Words with one single immediate or 
     * rope value simply store this value. Larger value chains must be circular 
     * lists of real words. Here, ensure that both word and value's values are 
     * circular lists.
     */

    if (!WORD_VALUE(word)) {
	WORD_VALUE(word) = word;
    } else if (IS_IMMEDIATE(WORD_VALUE(word)) || !IS_WORD(WORD_VALUE(word))) {
	WORD_VALUE(word) = UpconvertWord(WORD_VALUE(word));
	WORD_VALUE(WORD_VALUE(word)) = word;
	Col_DeclareChildWord(word, WORD_VALUE(word));
    }
    if (!WORD_VALUE(value)) {
	WORD_VALUE(value) = value;
    } else if (IS_IMMEDIATE(WORD_VALUE(word)) || !IS_WORD(WORD_VALUE(word))) {
	WORD_VALUE(value) = UpconvertWord(WORD_VALUE(value));
	WORD_VALUE(WORD_VALUE(value)) = value;
	Col_DeclareChildWord(value, WORD_VALUE(value));
    }

    /*
     * Merging circular lists is simply done by exchanging the heads' next
     * pointers.
     */

    SWAP_PTR(WORD_VALUE(word), WORD_VALUE(value));
    Col_DeclareChildWord(word, value);
    Col_DeclareChildWord(value, word);

    return word;
}

/*
 *---------------------------------------------------------------------------
 *
 * UpconvertWord --
 *
 *	Convert immediate value or rope to full-fledged word. Argument must not
 *	be one itself (no test is done).
 *
 * Results:
 *	The new word.
 *
 * Side effects:
 *	Allocate a new word.
 *
 *---------------------------------------------------------------------------
 */

static Col_Word
UpconvertWord(
    Col_Word word)		/* The word to upconvert. */
{
    Col_WordData data;
    Col_Word converted;
    
    converted = AllocCells(1);
    WORD_VALUE(converted) = NULL;
    switch ((int) Col_GetWordInfo(word, &data)) {
	case COL_INT:
	    WORD_SET_TYPE_ID(converted, WORD_TYPE_INT);
	    WORD_INT_DATA(converted) = data.i;
	    break;

	case COL_CHAR:
	    WORD_SET_TYPE_ID(converted, WORD_TYPE_STRING);
	    if (data.ch <= USHRT_MAX) {
		unsigned short ch2 = (unsigned short) data.ch; 
		WORD_STRING_DATA(converted) = Col_NewRope(COL_UCS2, &ch2, 2);
	    } else {
		WORD_STRING_DATA(converted) = Col_NewRope(COL_UCS4, &data.ch, 4);
	    }
	    break;

	case COL_SMALL_STRING:
	    WORD_SET_TYPE_ID(converted, WORD_TYPE_STRING);
	    WORD_STRING_DATA(converted) = Col_NewRope(COL_UCS1, data.str.data, 
		    data.str.length);
	    break;

	case COL_ROPE:
	    WORD_SET_TYPE_ID(converted, WORD_TYPE_STRING);
	    WORD_STRING_DATA(converted) = data.rope;
	    break;
    }

    return converted;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ClearWordValue --
 *
 *	Clear a word's value. This removes the word from the value chain it 
 *	belongs to.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

void
Col_ClearWordValue(
    Col_Word word)		/* The word to clear value for. */
{
    Col_Word value;

    RESOLVE_WORD(word);

    if (!word || IS_IMMEDIATE(word) || !IS_WORD(word)) {
	/*
	 * Ropes or immediate values have no underlying value.
	 */

	return;
    }

    value = WORD_VALUE(word);
    if (!value || value == word || IS_IMMEDIATE(value) || !IS_WORD(value)) {
	/*
	 * No value chain. Simply clear word's value.
	 */

	WORD_VALUE(word) = NULL;
	return;
    }

    /*
     * Find the value pointing back to this word, and remove word from the 
     * chain.
     */

    while (WORD_VALUE(value) != word) {
	value = WORD_VALUE(value);
    }
    WORD_VALUE(value) = WORD_VALUE(word);
    Col_DeclareChildWord(value, WORD_VALUE(word));
    WORD_VALUE(word) = NULL;
}
