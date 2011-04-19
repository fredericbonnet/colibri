#include "colibri.h"
#include "colInternal.h"

#include <string.h>
#include <limits.h>

#define WORD_CHAR_MAX		0xFFFFFF

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

    if (value <= SMALL_INT_MAX && value >= SMALL_INT_MIN) {
	return WORD_SMALL_INT_NEW(value);
    }

    /*
     * Create a new integer word.
     */

    word = (Col_Word) AllocCells(1);
    WORD_INT_INIT(word, value);

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
	    size_t length = byteLength/sizeof(Col_Char1);
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
	    size_t length = byteLength/sizeof(Col_Char2);
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
		WORD_SMALL_STRING_SET_LENGTH(word, length);
		for (i = 0; i < length; i++) {
		    if (string[i] > UCHAR_MAX) break;
		    WORD_SMALL_STRING_DATA(word)[i] = (Col_Char1) string[i];
		}
		if (i == length) {
		    return word;
		}
	    }
	    break;
	}

	case COL_UCS4: {
	    size_t length = byteLength/sizeof(Col_Char4);
	    const Col_Char4 * string = data;
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
		    WORD_SMALL_STRING_DATA(word)[i] = (Col_Char1) string[i];
		}
		if (i == length) {
		    return word;
		}
	    }
	    break;
	}
    }

    /*
     * Return a new rope word.
     */

    return WORD_ROPE_NEW(Col_NewRope(format, data, byteLength));
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_NewRopeWord --
 *
 *	Create a new rope word.
 *
 *	If the rope is a C string, creates a new word that wraps this string.
 *
 * Results:
 *	A new word or the rope itself wrapped as word.
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

	    /*
	     * Wrap the C string.
	     */

	    word = (Col_Word) AllocCells(1);
	    WORD_STRING_INIT(word, rope, NULL);

	    return word;

	/* ROPE_TYPE_UNKNOWN */

	default: /* Including ROPE_TYPE_NULL */
	    return WORD_ROPE_NEW(rope);
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

	Col_Error(COL_ERROR, "Word size %u too large (max %u)", size, 
		WORD_MAX_SIZE - (actualSize - size));
	*dataPtr = NULL;
	return WORD_NIL;
    }
    
    word = (Col_Word) AllocCells(NB_CELLS(WORD_HEADER_SIZE+actualSize));
    WORD_SET_TYPE_ADDR(word, type);
    WORD_SYNONYM(word) = WORD_NIL;
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

    RESOLVE_WORD(word);
    
    switch (WORD_TYPE(word)) {
	/*
	 * Immediate values.
	 */

	case WORD_TYPE_NIL:
	    return COL_NIL;
	    
	case WORD_TYPE_SMALL_INT:
	    dataPtr->i = WORD_SMALL_INT_GET(word);
	    return COL_INT;

	case WORD_TYPE_CHAR:
	    dataPtr->ch = WORD_CHAR_GET(word);
	    return COL_CHAR;

	case WORD_TYPE_SMALL_STRING:
	    dataPtr->str.length = WORD_SMALL_STRING_GET_LENGTH(word);
	    memcpy((void *) dataPtr->str.data, WORD_SMALL_STRING_DATA(word), 
		    dataPtr->str.length);
	    return COL_SMALL_STRING;

	case WORD_TYPE_VOID_LIST:
	    return COL_LIST;

	/*
	 * Ropes.
	 */

	case WORD_TYPE_ROPE:
	    dataPtr->rope = WORD_ROPE_GET(word);
	    return COL_ROPE;

	/*
	 * Predefined types.
	 */

	case WORD_TYPE_INT:
	    dataPtr->i = WORD_INT_DATA(word);
	    return COL_INT;

	case WORD_TYPE_STRING:
	    dataPtr->rope = WORD_STRING_DATA(word);
	    return COL_ROPE;

	case WORD_TYPE_VECTOR:
	    dataPtr->vector.length = WORD_VECTOR_LENGTH(word);
	    dataPtr->vector.elements = WORD_VECTOR_ELEMENTS(word);
	    return COL_VECTOR;

	case WORD_TYPE_MVECTOR:
	    dataPtr->mvector.maxLength = (WORD_MVECTOR_SIZE(word)*CELL_SIZE 
		    - WORD_HEADER_SIZE) / sizeof(Col_Word);
	    dataPtr->mvector.length = WORD_VECTOR_LENGTH(word);
	    dataPtr->mvector.elements = WORD_VECTOR_ELEMENTS(word);
	    return COL_MVECTOR;

	case WORD_TYPE_LIST:
	    return COL_LIST;

	case WORD_TYPE_MLIST:
	    return COL_MLIST;

	case WORD_TYPE_REFERENCE:
	    dataPtr->refSource = WORD_REFERENCE_SOURCE(word);
	    return COL_REFERENCE;

	/*
	 * Regular word.
	 */

	case WORD_TYPE_REGULAR:
	    WORD_GET_TYPE_ADDR(word, typeInfo);
	    dataPtr->data = WORD_DATA(word);
	    return typeInfo;

	/* WORD_TYPE_UNKNOWN */

	default:
	    return COL_NIL;
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
 *	The word or nil if not found, and additional info in dataPtr.
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
    Col_Word synonym;

    RESOLVE_WORD(word);

    synonym = word;
    while (synonym) {
	if (Col_GetWordInfo(synonym, dataPtr) == type) {
	    /*
	     * Found !
	     */

	    return synonym;
	}

	if (IS_IMMEDIATE(synonym) || IS_ROPE(synonym)) {
	    /*
	     * Not a chain.
	     */

	    break;
	}

	synonym = WORD_SYNONYM(synonym);
	if (synonym == word) {
	    /*
	     * Looped back.
	     */

	    break;
	}
    }
    return WORD_NIL;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_GetWordSynonym --
 *
 *	Get a synonym for the word.
 *
 *	Words form chains of synonyms, i.e. circular linked lists, except 
 *	when a word only has one synonym that is an immediate value or a rope
 *	(as they have no such concept). 
 *	To iterate over the chain, simply call this function several times 
 *	with the intermediary results until it returns nil.
 *
 * Results:
 *	The word synonym, which can be a rope, an immediate value, or 
 *	another word.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_GetWordSynonym(
    Col_Word word)		/* The word to get synonym for. */
{
    if (!word || IS_IMMEDIATE(word) || IS_ROPE(word)) {
	/*
	 * Ropes or immediate values have no synonyms.
	 */

	return WORD_NIL;
    } else {
	RESOLVE_WORD(word);
	return WORD_SYNONYM(word);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_AddWordSynonym --
 *
 *	Add a synonym to a word.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Modify the chain of synonyms, may allocate new words. If the word is
 *	an immediate value, it may be upconverted to a regular word, and
 *	declared as child if the parent is given.
 *
 *---------------------------------------------------------------------------
 */

void
Col_AddWordSynonym(
    Col_Word *wordPtr,		/* Point to the word to add synonym to. */
    Col_Word synonym,		/* The synonym to add. */
    Col_Word *parentPtr)	/* If non-nil, parent of the given word. */
{
    Col_Word word = *wordPtr;

    /*
     * Quick cases.
     */

    if (!word || !synonym) {
	/*
	 * Nil can't have synonyms.
	 */

	return;
    }

    if (IS_IMMEDIATE(word) || IS_ROPE(word)) {
	/*
	 * Upconvert word so that it can point to the synonym.
	 */

	word = *wordPtr = UpconvertWord(word);
	if (*parentPtr) {
	    Col_DeclareChild((void *) word, (void *) *parentPtr);
	}
    } else {
	RESOLVE_WORD(word);
    }

    if (IS_IMMEDIATE(synonym) || IS_ROPE(synonym)) {
	if (!WORD_SYNONYM(word)) {
	    /*
	     * Word has no current synonym, simply add the new one.
	     */

	    WORD_SYNONYM(word) = synonym;
	    Col_DeclareChild((void *) word, (void *) synonym);
	    return;
	}

	/*
	 * Upconvert synonym as well.
	 */

	synonym = UpconvertWord(synonym);
    } else {
	RESOLVE_WORD(synonym);
    }

    if (word == synonym) {
	/*
	 * A word cannot be synonym with itself.
	 */

	return;
    }

    /*
     * Synonym-less words have a nil synonym pointer. Words with one single 
     * immediate or rope value simply point to it. Larger synonym chains must be
     * circular lists of real words. Here, ensure that both word and synonym's 
     * synonym chains are circular lists.
     *
     * Note: No need to declare children here as it will be done anyway in the 
     * final step.
     *
     */

    if (!WORD_SYNONYM(word)) {
	WORD_SYNONYM(word) = word;
    } else if (IS_IMMEDIATE(WORD_SYNONYM(word)) 
	    || IS_ROPE(WORD_SYNONYM(word))) {
	WORD_SYNONYM(word) = UpconvertWord(WORD_SYNONYM(word));
	WORD_SYNONYM(WORD_SYNONYM(word)) = word;
    }
    if (!WORD_SYNONYM(synonym)) {
	WORD_SYNONYM(synonym) = synonym;
    } else if (IS_IMMEDIATE(WORD_SYNONYM(synonym)) 
	    || IS_ROPE(WORD_SYNONYM(synonym))) {
	WORD_SYNONYM(synonym) = UpconvertWord(WORD_SYNONYM(synonym));
	WORD_SYNONYM(WORD_SYNONYM(synonym)) = synonym;
    }

    /*
     * Merging circular lists is simply done by exchanging the heads' next
     * pointers.
     */

    {
	Col_Word tmp = WORD_SYNONYM(word);
	WORD_SYNONYM(word) = WORD_SYNONYM(synonym);
	WORD_SYNONYM(synonym) = tmp;
    }
    Col_DeclareChild((void *) word, (void *) WORD_SYNONYM(word));
    Col_DeclareChild((void *) synonym, (void *) WORD_SYNONYM(synonym));
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
    
    converted = (Col_Word) AllocCells(1);
    switch ((intptr_t) Col_GetWordInfo(word, &data)) {
	case (intptr_t) COL_INT:
	    WORD_INT_INIT(converted, data.i);
	    break;

	case (intptr_t) COL_CHAR:
	    if (data.ch <= USHRT_MAX) {
		unsigned short ch2 = (unsigned short) data.ch; 
		WORD_STRING_INIT(converted, Col_NewRope(COL_UCS2, &ch2, 2), 
			NULL);
	    } else {
		WORD_STRING_INIT(converted, Col_NewRope(COL_UCS4, &data.ch, 4),
			NULL);
	    }
	    break;

	case (intptr_t) COL_SMALL_STRING:
	    WORD_STRING_INIT(converted, Col_NewRope(COL_UCS1, data.str.data, 
		    data.str.length), NULL);
	    break;

	case (intptr_t) COL_LIST:
	    ASSERT(WORD_TYPE(word) == WORD_TYPE_VOID_LIST);

	    WORD_LIST_INIT(converted, WORD_VOID_LIST_NEW(data.vector.length), 0);
	    break;

	case (intptr_t) COL_ROPE:
	    WORD_STRING_INIT(converted, data.rope, NULL);
	    break;
    }

    return converted;
}

/*
 *---------------------------------------------------------------------------
 *
 * Col_ClearWordSynonym --
 *
 *	Clear a word's synonym. This removes the word from the synonym chain 
 *	it belongs to.
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
Col_ClearWordSynonym(
    Col_Word word)		/* The word to clear synonym for. */
{
    Col_Word synonym;

    RESOLVE_WORD(word);

    if (!word || IS_IMMEDIATE(word) || IS_ROPE(word)) {
	/*
	 * Ropes or immediate values have no synonyms.
	 */

	return;
    }

    synonym = WORD_SYNONYM(word);
    if (!synonym || synonym == word || IS_IMMEDIATE(synonym) 
	    || IS_ROPE(synonym)) {
	/*
	 * No synonym chain. Simply clear word's synonym.
	 */

	WORD_SYNONYM(word) = WORD_NIL;
	return;
    }

    /*
     * Find the synonym pointing back to this word, and remove word from the 
     * chain.
     */

    while (WORD_SYNONYM(synonym) != word) {
	synonym = WORD_SYNONYM(synonym);
    }
    WORD_SYNONYM(synonym) = WORD_SYNONYM(word);
    Col_DeclareChild((void *) synonym, (void *) WORD_SYNONYM(synonym));
    WORD_SYNONYM(word) = WORD_NIL;
}
