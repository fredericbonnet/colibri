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

#include "include/colibri.h"
#include "colInternal.h"

#include "colWordInt.h"
#include "colRopeInt.h"
#include "colVectorInt.h"
#include "colListInt.h"

#include <string.h>
#include <limits.h>

/*
 * Prototypes for functions used only in this file.
 */

static int		HasSynonymField(Col_Word word);
static void		AddSynonymField(Col_Word *wordPtr);


//TODO documentation overhaul

/****************************************************************************
 * Section: Word Creation
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
    WORD_INTWRAP_INIT(word, 0, value);

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
    WORD_FPWRAP_INIT(word, 0, value);

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

    if (type->freeProc) RememberSweepable(word, type);

    return word;
}


/****************************************************************************
 * Section: Word Accessors and Synonyms
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Function: Col_WordType
 *
 *	Get word type. Actual value may be a combination of known <Word Type
 *	Identifiers>.
 *
 * Argument:
 *	word	- The word to get type for.
 *
 * Result:
 *	A combination of <Word Type Identifiers>.
 *---------------------------------------------------------------------------*/

int
Col_WordType(
    Col_Word word)
{
    switch (WORD_TYPE(word)) {
    /*
     * Immediate words.
     */

    case WORD_TYPE_NIL:
	return COL_NIL;
	    
    case WORD_TYPE_SMALLINT:
	return COL_INT;

    case WORD_TYPE_SMALLFP:
	return COL_FLOAT;

    case WORD_TYPE_CHAR:
	return COL_CHAR | COL_STRING | COL_ROPE;

    case WORD_TYPE_SMALLSTR:
	return COL_STRING | COL_ROPE;

    case WORD_TYPE_CIRCLIST:
	return COL_LIST;

    case WORD_TYPE_VOIDLIST:
	return (WORD_VOIDLIST_LENGTH(word) == 0 ? COL_VECTOR : 0) | COL_LIST;

    /*
     * Predefined types.
     */

    case WORD_TYPE_WRAP:
	return WORD_WRAP_TYPE(word);

    case WORD_TYPE_UCSSTR:
	return COL_STRING | COL_ROPE;

    case WORD_TYPE_UTFSTR:
	return COL_STRING | COL_ROPE;

    case WORD_TYPE_SUBROPE:
    case WORD_TYPE_CONCATROPE:
	return COL_ROPE;

    case WORD_TYPE_VECTOR:
	return COL_VECTOR | COL_LIST;

    case WORD_TYPE_MVECTOR:
	return COL_MVECTOR | COL_VECTOR | COL_LIST;

    case WORD_TYPE_SUBLIST:
    case WORD_TYPE_CONCATLIST:
	return COL_LIST;

    case WORD_TYPE_STRHASHMAP:
	return COL_HASHMAP | COL_STRMAP;

    case WORD_TYPE_INTHASHMAP:
	return COL_HASHMAP | COL_INTMAP;

    case WORD_TYPE_STRTRIEMAP:
	return COL_TRIEMAP | COL_STRMAP;

    case WORD_TYPE_INTTRIEMAP:
	return COL_TRIEMAP | COL_INTMAP;

    case WORD_TYPE_STRBUF:
	return COL_STRBUF;

    /*
     * Mutable concat list nodes are only used internally, but handle them here 
     * anyway for proper type checking in list procs.
     */

    case WORD_TYPE_MCONCATLIST:
	return COL_MLIST | COL_LIST;

    /*
     * Custom word.
     */

    case WORD_TYPE_CUSTOM:
	return WORD_TYPEINFO(word)->type | COL_CUSTOM;

    /* WORD_TYPE_UNKNOWN */

    default:
	/* CANTHAPPEN */
	ASSERT(0);
	return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_IntWordValue
 *
 *	Get value of integer word.
 *
 * Argument:
 *	word	- The word to get value for.
 *
 * Type checking:
 *	TODO
 *
 * Result:
 *	The integer value.
 *
 * See also:
 *	<Col_NewIntWord>
 *---------------------------------------------------------------------------*/

intptr_t
Col_IntWordValue(
    Col_Word word)
{
    switch (WORD_TYPE(word)) {
    case WORD_TYPE_SMALLINT:
	return WORD_SMALLINT_GET(word);

    case WORD_TYPE_WRAP:
	if (WORD_WRAP_TYPE(word) == COL_INT) {
	    return WORD_INTWRAP_VALUE(word);
	}
	/* continued. */
    default:
	/*
	 * Invalid type.
	 */

	Col_Error(COL_TYPECHECK, "%x is not an integer word", word);
	return 0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_FloatWordValue
 *
 *	Get value of floating point word.
 *
 * Argument:
 *	word	- The word to get value for.
 *
 * Type checking:
 *	TODO
 *
 * Result:
 *	The floating point value.
 *
 * See also:
 *	<Col_NewFloatWord>
 *---------------------------------------------------------------------------*/

double
Col_FloatWordValue(
    Col_Word word)
{
    switch (WORD_TYPE(word)) {
    case WORD_TYPE_SMALLFP: {
	FloatConvert c;
	return WORD_SMALLFP_GET(word, c);
    }

    case WORD_TYPE_WRAP:
	if (WORD_WRAP_TYPE(word) == COL_FLOAT) {
	    return WORD_FPWRAP_VALUE(word);
	}
	/* continued. */
    default:
	/*
	 * Invalid type.
	 */

	Col_Error(COL_TYPECHECK, "%x is not a floating point word", word);
	return 0.0;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_CustomWordInfo
 *
 *	Get custom word type and data.
 *
 * Argument:
 *	word	- The word to get data for.
 *
 * Type checking:
 *	TODO
 *
 * Results:
 *	TODO
 *	dataPtr	- TODO
 *
 * See also:
 *	<Col_NewCustomWord>
 *---------------------------------------------------------------------------*/

Col_CustomWordType *
Col_CustomWordInfo(
    Col_Word word,
    void **dataPtr)
{
    Col_CustomWordType *type;

    switch (WORD_TYPE(word)) {
    case WORD_TYPE_CUSTOM: 
	type = WORD_TYPEINFO(word);
	*dataPtr = WORD_CUSTOM_DATA(word, type);
	return type;

    default:
	/*
	 * Invalid type.
	 */

	Col_Error(COL_TYPECHECK, "%x is not a custom word", word);
	return NULL;
    }
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
    case WORD_TYPE_CUSTOM:
    case WORD_TYPE_WRAP:
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
    switch (WORD_TYPE(*wordPtr)) {
    /*
     * Some types have dedicated wrappers.
     */

    case WORD_TYPE_SMALLINT:
	WORD_INTWRAP_INIT(converted, 0, WORD_SMALLINT_GET(*wordPtr));
	break;

    case WORD_TYPE_SMALLFP: {
	FloatConvert c;
	WORD_FPWRAP_INIT(converted, 0, WORD_SMALLFP_GET(*wordPtr, c));
	break;
    }

    default:
	/*
	 * Use generic wrapper.
	 */

	WORD_WRAP_INIT(converted, 0, Col_WordType(*wordPtr), *wordPtr);
	break;
    }

    *wordPtr = converted;
}

/*---------------------------------------------------------------------------
 * Function: Col_WordSynonym
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
Col_WordSynonym(
    Col_Word word)
{
    if (!HasSynonymField(word)) {
	return WORD_NIL;
    } else {
	return WORD_SYNONYM(word);
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_WordAddSynonym
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
Col_WordAddSynonym(
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
    // FIXME: if words are already part of the same chain, this splits the chain!

    {
	Col_Word tmp = WORD_SYNONYM(word);
	WORD_SYNONYM(word) = WORD_SYNONYM(synonym);
	WORD_SYNONYM(synonym) = tmp;
    }
}

/*---------------------------------------------------------------------------
 * Function: Col_WordClearSynonym
 *
 *	Clear a word's synonym. This removes the word from the synonym chain 
 *	it belongs to.
 *
 * Argument:
 *	word	- The word to clear synonym for.
 *---------------------------------------------------------------------------*/

void
Col_WordClearSynonym(
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
