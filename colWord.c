/*
 * File: colWord.c
 *
 *	This file implements the word handling features of Colibri.
 *
 *	Words are a generic abstract datatype framework used in conjunction with
 *	the exact generational garbage collector and the cell-based allocator.
 *
 * See also:
 *	<colWord.h>
 */

#include "colibri.h"
#include "colInternal.h"

#include <string.h>
#include <limits.h>

/*
 * Prototypes for functions used only in this file.
 */

static int		HasSynonymField(Col_Word word);
static void		AddSynonymField(Col_Word *wordPtr);


/****************************************************************************
 * Group: Word Creation
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_NewIntWord
 *
 *	Create a new integer word.
 *
 *	If the integer value is sufficiently small, return an immediate value 
 *	instead of allocating memory.
 *
 * Argument:
 *	value	- Integer value of the word to create.
 *
 * Result:
 *	The new integer word.
 *
 * Side effects:
 *	Allocates memory cells if word is not immediate.
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewIntWord(
    intptr_t value)
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

/*---------------------------------------------------------------------------
 * Function: Col_NewFloatWord
 *
 *	Create a new floating point word.
 *
 *	If the floating point value fits, return an immediate value instead of 
 *	allocating memory.
 *
 * Argument:
 *	value	- Floating point value of the word to create.
 *
 * Result:
 *	The new floating point word.
 *
 * Side effects:
 *	Allocates memory cells if word is not immediate.
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewFloatWord(
    double value)
{
    Col_Word word;		/* Resulting word in the general case. */
    FloatConvert c;

    /*
     * Return floating point value if possible.
     */

    word = WORD_SMALLFP_NEW(value, c);
    if (WORD_SMALLFP_GET(word, c) == value) {
	return word;
    }

    /*
     * Create a new floating point word.
     */

    word = (Col_Word) AllocCells(1);
    WORD_FP_INIT(word, value);

    return word;
}

/*---------------------------------------------------------------------------
 * Function: Col_NewCustomWord
 *
 *	Create a new custom word.
 *
 * Arguments:
 *	type	- The word type.
 *	size	- Size of data.
 *	dataPtr	- Will hold a pointer to the allocated data.
 *
 * Result:
 *	A new word of the given size.
 *
 * Side effects:
 *	Allocates memory cells.
 *---------------------------------------------------------------------------*/

Col_Word
Col_NewCustomWord(
    Col_CustomWordType *type,
    size_t size,
    void **dataPtr)
{
    Col_Word word = (Col_Word) AllocCells(WORD_CUSTOM_SIZE(size, type));
    WORD_CUSTOM_INIT(word, type);
    if (dataPtr) *dataPtr = WORD_CUSTOM_DATA(word, type);

    DeclareCustomWord(word, type);

    return word;
}


/****************************************************************************
 * Group: Word Access and Synonyms
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_GetWordInfo
 *
 *	Get information about a word (type and data).
 *
 * Arguments:
 *	word	- The word to get info for.
 *
 * Results:
 *	A type ID or pointer. Additionally:
 *
 *	dataPtr	- If non-NULL, type-specific info.
 *---------------------------------------------------------------------------*/

Col_WordType
Col_GetWordInfo(
    Col_Word word,
    Col_WordData *dataPtr)
{
    switch (WORD_TYPE(word)) {
	/*
	 * Immediate words.
	 */

	case WORD_TYPE_NIL:
	    return COL_NIL;
	    
	case WORD_TYPE_SMALLINT:
	    if (dataPtr) dataPtr->i = WORD_SMALLINT_GET(word);
	    return COL_INT;

	case WORD_TYPE_SMALLFP:
	    if (dataPtr) {
		FloatConvert c;
		dataPtr->f = WORD_SMALLFP_GET(word, c);
	    }
	    return COL_FLOAT;

	case WORD_TYPE_CHAR:
	    if (dataPtr) {
		dataPtr->string._smallData = (Col_Word) WORD_CHAR_GET(word);
		dataPtr->string.format = COL_UCS4;
		dataPtr->string.data = &dataPtr->string._smallData;
		dataPtr->string.byteLength = 4;
	    }
	    return COL_STRING;

	case WORD_TYPE_SMALLSTR:
	    if (dataPtr) {
		dataPtr->string._smallData = (uintptr_t) word;
		dataPtr->string.format = COL_UCS1;
		dataPtr->string.data 
			= WORD_SMALLSTR_DATA(dataPtr->string._smallData);
		dataPtr->string.byteLength = WORD_SMALLSTR_LENGTH(word);
	    }
	    return COL_STRING;

	case WORD_TYPE_CIRCLIST:
	    /*
	     * Same as core list.
	     */

	    return Col_GetWordInfo(WORD_CIRCLIST_CORE(word), dataPtr);

	case WORD_TYPE_VOIDLIST:
	    return COL_LIST;

	/*
	 * Predefined types.
	 */

	case WORD_TYPE_WRAP:
	    return Col_GetWordInfo(word, dataPtr);

	case WORD_TYPE_UCSSTR:
	    if (dataPtr) {
		dataPtr->string.format = WORD_UCSSTR_FORMAT(word);
		dataPtr->string.data = WORD_UCSSTR_DATA(word);
		dataPtr->string.byteLength = WORD_UCSSTR_LENGTH(word)
			* WORD_UCSSTR_FORMAT(word);
	    }
	    return COL_STRING;

	case WORD_TYPE_UTF8STR:
	    if (dataPtr) {
		dataPtr->string.format = COL_UTF8;
		dataPtr->string.data = WORD_UCSSTR_DATA(word);
		dataPtr->string.byteLength = WORD_UTF8STR_BYTELENGTH(word);
	    }
	    return COL_STRING;

	case WORD_TYPE_SUBROPE:
	case WORD_TYPE_CONCATROPE:
	    return COL_ROPE;

	case WORD_TYPE_INT:
	    if (dataPtr) dataPtr->i = WORD_INT_VALUE(word);
	    return COL_INT;

	case WORD_TYPE_FP:
	    if (dataPtr) dataPtr->f = WORD_FP_VALUE(word);
	    return COL_FLOAT;

	case WORD_TYPE_VECTOR:
	    if (dataPtr) {
		dataPtr->vector.length = WORD_VECTOR_LENGTH(word);
		dataPtr->vector.elements = WORD_VECTOR_ELEMENTS(word);
	    }
	    return COL_VECTOR;

	case WORD_TYPE_MVECTOR:
	    if (dataPtr) {
		dataPtr->mvector.length = WORD_VECTOR_LENGTH(word);
		dataPtr->mvector.elements = WORD_VECTOR_ELEMENTS(word);
		dataPtr->mvector.maxLength 
			= VECTOR_MAX_LENGTH(WORD_MVECTOR_SIZE(word) 
			* CELL_SIZE);
	    }
	    return COL_MVECTOR;

	case WORD_TYPE_SUBLIST:
	case WORD_TYPE_CONCATLIST:
	    return COL_LIST;

	case WORD_TYPE_MLIST:
	case WORD_TYPE_MCONCATLIST:
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

	case WORD_TYPE_CUSTOM:
	    if (dataPtr) {
		Col_CustomWordType *typeInfo = WORD_TYPEINFO(word);
		dataPtr->custom.type = typeInfo;
		dataPtr->custom.data = WORD_CUSTOM_DATA(word, 
			dataPtr->custom.type);
	    }
	    return COL_CUSTOM;

	/* WORD_TYPE_UNKNOWN */

	default:
	    return COL_NIL;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_FindWordInfo
 *
 *	Find data for a word with the given type in the synonym chain. If none 
 *	found, optionally find first word matching a given set of types.
 *
 * Arguments:
 *	word		- The word to get data from.
 *	type		- The required type.
 *	altTypePtr	- If non-NULL, OR-ed set of acceptable types for 
 *			  alternate word.
 *
 * Results:
 *	The word or nil if not found. Additionally:
 *
 *	dataPtr		- If non-NULL, type-specific info.
 *	altTypePtr	- If non-NULL, actual type of any alternate word found.
 *	altWordPtr	- If non-NULL, alternate word found.
 *---------------------------------------------------------------------------*/

Col_Word
Col_FindWordInfo(
    Col_Word word,
    Col_WordType type,
    Col_WordData *dataPtr,
    int *altTypePtr,
    Col_Word *altWordPtr)
{
    Col_Word synonym;

    synonym = word;
    while (synonym) {
	Col_WordType wordType = Col_GetWordInfo(synonym, dataPtr);
	if (wordType & type) {
	    /*
	     * Found !
	     */

	    return synonym;
	}

	if (altTypePtr && altWordPtr  && (wordType & *altTypePtr)) {
	    /*
	     * Alternate word found. Prevent further finds by NULLing pointers.
	     */

	    *altTypePtr = wordType;
	    *altWordPtr = synonym;
	    altTypePtr = NULL;
	    altWordPtr = NULL;
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
    if (altTypePtr) *altTypePtr = COL_NIL;
    if (altWordPtr) *altWordPtr = WORD_NIL;
    return WORD_NIL;
}

/*---------------------------------------------------------------------------
 * Internal Function: HasSynonymField
 *
 *	Test whether the word has a synonym field.
 *
 * Argument:
 *	word	- The word to test.
 *
 * Result:
 *	Whether the word has a synonym field.
 *---------------------------------------------------------------------------*/

static int
HasSynonymField(
    Col_Word word)
{
    switch (WORD_TYPE(word)) {
	case WORD_TYPE_WRAP:
	case WORD_TYPE_INT:
	case WORD_TYPE_FP:
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

/*---------------------------------------------------------------------------
 * Internal Function: AddSynonymField
 *
 *	Return a word that is semantically identical to the given one and has
 *	a synonym field.
 *
 * Argument:
 *	wordPtr	- Point to the word to convert.
 *
 * Result:
 *	A word with a synonym field.
 *
 * Side effects:
 *	Allocates memory cells.
 *---------------------------------------------------------------------------*/

static void
AddSynonymField(
    Col_Word *wordPtr)
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

	case WORD_TYPE_SMALLFP: {
	    FloatConvert c;
	    WORD_FP_INIT(converted, WORD_SMALLFP_GET(*wordPtr, c));
	    break;
	}

	case WORD_TYPE_MCONCATLIST:
	    WORD_MLIST_INIT(converted, *wordPtr);
	    break;

	case WORD_TYPE_CIRCLIST:
	    if (WORD_TYPE(WORD_CIRCLIST_CORE(*wordPtr)) 
		    == WORD_TYPE_MCONCATLIST) {
		/*
		 * Use mutable list.
		 */

		WORD_MLIST_INIT(converted, *wordPtr);
	    } else {
		/*
		 * Use generic wrapper.
		 */

		WORD_WRAP_INIT(converted, *wordPtr);
	    }
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

/*---------------------------------------------------------------------------
 * Function: Col_GetWordSynonym
 *
 *	Get a synonym for the word.
 *
 *	Words may form chains of synonyms, i.e. circular linked lists. To
 *	iterate over the chain, simply call this function several times on the 
 *	intermediary results until it returns nil or the first word.
 *
 * Argument:
 *	word	- The word to get synonym for.
 *
 * Result:
 *	The word synonym, which may be nil.
 *---------------------------------------------------------------------------*/

Col_Word
Col_GetWordSynonym(
    Col_Word word)
{
    if (!HasSynonymField(word)) {
	return WORD_NIL;
    } else {
	return WORD_SYNONYM(word);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_AddWordSynonym
 *
 *	Add a synonym to a word.
 *
 * Arguments:
 *	wordPtr	- Point to the word to add synonym to. May be modified in the
 *		  process (in this case the original word will be part of the
 *		  returned word's synonym chain).
 *	synonym	- The synonym to add.
 *
 * Side effects:
 *	Modifies the chain of synonyms, may allocate new words.
 *---------------------------------------------------------------------------*/

void
Col_AddWordSynonym(
    Col_Word *wordPtr,
    Col_Word synonym)
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
}

/*---------------------------------------------------------------------------
 * Function: Col_ClearWordSynonym
 *
 *	Clear a word's synonym. This removes the word from the synonym chain 
 *	it belongs to.
 *
 * Argument:
 *	word	- The word to clear synonym for.
 *---------------------------------------------------------------------------*/

void
Col_ClearWordSynonym(
    Col_Word word)
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
    WORD_SYNONYM(word) = WORD_NIL;
}
