#include "colibri.h"
#include "colInternal.h"

#include <string.h>
#include <limits.h>

/*
 * Prototypes for functions used only in this file.
 */

static int		HasSynonymField(Col_Word word);
static void		AddSynonymField(Col_Word *wordPtr);


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

    if (value <= SMALLINT_MAX && value >= SMALLINT_MIN) {
	return WORD_SMALLINT_NEW(value);
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
 * Col_NewCustomWord --
 *
 *	Create a new custom word.
 *
 * Results:
 *	A new word of the given size.
 *
 * Side effects:
 *	Memory cells are allocated.
 *
 *---------------------------------------------------------------------------
 */

Col_Word
Col_NewCustomWord(
    Col_CustomWordType *type,	/* The word type. */
    size_t size,		/* Size of data. */
    void **dataPtr)		/* Will hold a pointer to the allocated data. */
{
    Col_Word word = (Col_Word) AllocCells(WORD_CUSTOM_SIZE(size, type));
    WORD_CUSTOM_INIT(word, type);
    if (dataPtr) *dataPtr = WORD_CUSTOM_DATA(word, type);

    DeclareCustomWord(word);

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

Col_WordType
Col_GetWordInfo(
    Col_Word word,		/* The word to get info for. */
    Col_WordData *dataPtr)	/* Returned data. */
{
    switch (WORD_TYPE(word)) {
	/*
	 * Immediate values.
	 */

	case WORD_TYPE_NIL:
	    return COL_NIL;
	    
	case WORD_TYPE_SMALLINT:
	    dataPtr->i = WORD_SMALLINT_GET(word);
	    return COL_INT;

	case WORD_TYPE_CHAR:
	    dataPtr->ch = WORD_CHAR_GET(word);
	    return COL_CHAR;

	case WORD_TYPE_SMALLSTR:
	    dataPtr->string._smallData = word;
	    dataPtr->string.format = COL_UCS1;
	    dataPtr->string.data 
		    = WORD_SMALLSTR_DATA(dataPtr->string._smallData);
	    dataPtr->string.byteLength = WORD_SMALLSTR_LENGTH(word);
	    return COL_STRING;

	case WORD_TYPE_VOIDLIST:
	    return COL_LIST;

	/*
	 * Predefined types.
	 */

	case WORD_TYPE_WRAP:
	    return Col_GetWordInfo(word, dataPtr);

	case WORD_TYPE_UCSSTR:
	    dataPtr->string.format = WORD_UCSSTR_FORMAT(word);
	    dataPtr->string.data = WORD_UCSSTR_DATA(word);
	    dataPtr->string.byteLength = WORD_UCSSTR_LENGTH(word)
		    * WORD_UCSSTR_FORMAT(word);
	    return COL_STRING;

	case WORD_TYPE_UTF8STR:
	    dataPtr->string.format = COL_UTF8;
	    dataPtr->string.data = WORD_UCSSTR_DATA(word);
	    dataPtr->string.byteLength = WORD_UTF8STR_BYTELENGTH(word);
	    return COL_STRING;

	case WORD_TYPE_SUBROPE:
	case WORD_TYPE_CONCATROPE:
	    return COL_ROPE;

	case WORD_TYPE_INT:
	    dataPtr->i = WORD_INT_VALUE(word);
	    return COL_INT;

	case WORD_TYPE_VECTOR:
	    dataPtr->vector.length = WORD_VECTOR_LENGTH(word);
	    dataPtr->vector.elements = WORD_VECTOR_ELEMENTS(word);
	    return COL_VECTOR;

	case WORD_TYPE_MVECTOR:
	    dataPtr->mvector.maxLength 
		    = VECTOR_MAX_LENGTH(WORD_MVECTOR_SIZE(word) * CELL_SIZE);
	    dataPtr->mvector.length = WORD_VECTOR_LENGTH(word);
	    dataPtr->mvector.elements = WORD_VECTOR_ELEMENTS(word);
	    return COL_MVECTOR;

	case WORD_TYPE_LIST:
	case WORD_TYPE_SUBLIST:
	case WORD_TYPE_CONCATLIST:
	    return COL_LIST;

	case WORD_TYPE_MLIST:
	    return COL_MLIST;

	case WORD_TYPE_STRHASHMAP:
	case WORD_TYPE_STRTRIEMAP:
	    return COL_STRMAP;

	case WORD_TYPE_INTHASHMAP:
	case WORD_TYPE_INTTRIEMAP:
	    return COL_INTMAP;

	/*
	 * Custom word.
	 */

	case WORD_TYPE_CUSTOM: {
	    Col_CustomWordType *typeInfo = WORD_TYPEINFO(word);
	    dataPtr->custom.type = typeInfo;
	    dataPtr->custom.data = WORD_CUSTOM_DATA(word, dataPtr->custom.type);
	    return COL_CUSTOM;
	}

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
    Col_WordType type,		/* The required type. */
    Col_WordData *dataPtr)	/* Returned data. */
{
    Col_Word synonym;

    synonym = word;
    while (synonym) {
	if (Col_GetWordInfo(synonym, dataPtr) == type) {
	    /*
	     * Found !
	     */

	    return synonym;
	}

	if (!HasSynonymField(synonym)) {
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
 * HasSynonymField --
 *
 *	Test whether the word has a synonym field.
 *
 * Results:
 *	Whether the word has a synonym field.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

static int
HasSynonymField(
    Col_Word word)		/* The word to test. */
{
    switch (WORD_TYPE(word)) {
	case WORD_TYPE_WRAP:
	case WORD_TYPE_INT:
	case WORD_TYPE_LIST:
	case WORD_TYPE_MLIST:
	case WORD_TYPE_STRHASHMAP:
	case WORD_TYPE_INTHASHMAP:
	case WORD_TYPE_STRTRIEMAP:
	case WORD_TYPE_INTTRIEMAP:
	    return 1;

	/* WORD_TYPE_UNKNOWN */

	default:
	    return 0;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * AddSynonymField --
 *
 *	Return a word that is semantically identical to the given one and has
 *	a synonym field.
 *
 * Results:
 *	A word with a synonym field.
 *
 * Side effects:
 *	Allocates memory cells.
 *
 *---------------------------------------------------------------------------
 */

static void
AddSynonymField(
    Col_Word *wordPtr)		/* Point to the word to convert. */
{
    Col_Word converted;

    ASSERT(!HasSynonymField(*wordPtr));

    converted = (Col_Word) AllocCells(1);
    TRACE("\t\tAdding synonym field to 0x%p => 0x%p\n", *wordPtr, converted);
    switch (WORD_TYPE(*wordPtr)) {
	/*
	 * Some types have dedicated wrappers.
	 */

	case WORD_TYPE_SMALLINT:
	    WORD_INT_INIT(converted, WORD_SMALLINT_GET(*wordPtr));
	    break;

	case WORD_TYPE_VOIDLIST:
	case WORD_TYPE_SUBLIST:
	case WORD_TYPE_CONCATLIST:
	    WORD_LIST_INIT(converted, *wordPtr, 0);
	    break;

	/* WORD_TYPE_UNKNOWN */

	default:
	    /*
	     * Use generic wrapper.
	     */

	    WORD_WRAP_INIT(converted, *wordPtr);
	    break;
    }

    *wordPtr = converted;
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
    if (!HasSynonymField(word)) {
	return WORD_NIL;
    } else {
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
    Col_Word parent)		/* If non-nil, parent of the given word. */
{
    Col_Word word;

    /*
     * Quick cases.
     */

    if (!*wordPtr || !synonym) {
	/*
	 * Nil can't have synonyms.
	 */

	return;
    }

    if (!HasSynonymField(*wordPtr)) {
	AddSynonymField(wordPtr);
	if (parent) Col_WordSetModified(parent);
    }
    word = *wordPtr;

    if (word == synonym) {
	/*
	 * A word cannot be synonym with itself.
	 */

	return;
    }

    if (!HasSynonymField(synonym)) {
	if (!WORD_SYNONYM(word)) {
	    /*
	     * Word has no current synonym, simply add the new one without
	     * converting the synonym.
	     */

	    WORD_SYNONYM(word) = synonym;
	    Col_WordSetModified(word);
	    return;
	}
	AddSynonymField(&synonym);
    }
    
    /*
     * Synonym-less words have a nil synonym pointer. Words with one synonym
     * without synonym field itself simply point to it. Larger synonym chains 
     * must be circular lists of words with synonym fields. Here, ensure that 
     * both word and synonym's synonym chains are circular lists.
     */

    ASSERT(HasSynonymField(word));
    ASSERT(HasSynonymField(synonym));
    if (!WORD_SYNONYM(word)) {
	WORD_SYNONYM(word) = word;
    } else if (!HasSynonymField(WORD_SYNONYM(word))) {
	AddSynonymField(&WORD_SYNONYM(word));
	WORD_SYNONYM(WORD_SYNONYM(word)) = word;
    }
    if (!WORD_SYNONYM(synonym)) {
	WORD_SYNONYM(synonym) = synonym;
    } else if (!HasSynonymField(WORD_SYNONYM(synonym))) {
	AddSynonymField(&WORD_SYNONYM(synonym));
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
    Col_WordSetModified(word);
    Col_WordSetModified(synonym);
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

    if (!HasSynonymField(word)) {
	return;
    }

    synonym = WORD_SYNONYM(word);
    if (!HasSynonymField(synonym)) {
	/*
	 * No synonym chain. Simply clear word's synonym field.
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
	ASSERT(HasSynonymField(synonym));
    }
    WORD_SYNONYM(synonym) = WORD_SYNONYM(word);
    Col_WordSetModified(synonym);
    WORD_SYNONYM(word) = WORD_NIL;
}
