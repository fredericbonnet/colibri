/*
 * Header: colWord.h
 *
 *	This header file defines the word handling features of Colibri.
 *
 *	Words are a generic abstract datatype framework used in conjunction with
 *	the exact generational garbage collector and the cell-based allocator.
 */

#ifndef _COLIBRI_WORD
#define _COLIBRI_WORD

#include <stddef.h> /* For size_t, uintptr_t and the like */


/*
================================================================================
Section: Words
================================================================================
*/

/*---------------------------------------------------------------------------
 * Typedef: Col_Word
 *
 *	Colibri words are opaque types.
 *---------------------------------------------------------------------------*/

typedef uintptr_t Col_Word;

/*---------------------------------------------------------------------------
 * Constant: WORD_NIL
 *
 *	Nil is a valid word.
 *---------------------------------------------------------------------------*/

#define WORD_NIL \
    ((Col_Word) 0)

/*---------------------------------------------------------------------------
 * Constants: Boolean Values
 *
 *	Boolean singleton words. Values are immediate and constant, which means
 *	that they can be safely compared and stored in static storage.
 *
 * Note:
 *	C and Colibri booleans are not interchangeable. More specifically,
 *	WORD_FALSE is not false in the C sense because it is nonzero. They are
 *	also distinct from integer words: an integer zero is not a boolean false
 *	contrary to C. So it is an error to write e.g. "if (WORD_FALSE)".
 *
 *  WORD_FALSE	- False boolean word.
 *  WORD_TRUE	- True boolean word.
 *---------------------------------------------------------------------------*/

#define WORD_FALSE \
    ((Col_Word) 0x004)
#define WORD_TRUE \
    ((Col_Word) 0x104)

/*---------------------------------------------------------------------------
 * Constants: Word Type Identifiers
 *
 *	Data types recognized by Colibri. Values are OR-able so that a word can
 *	match several types (e.g. a custom rope type as returned by 
 *	<Col_WordType> would be <COL_CUSTOM> | <COL_ROPE> ). 
 *
 *  COL_NIL	- Nil.
 *  COL_CUSTOM	- Custom type.
 *  COL_BOOL	- Boolean.
 *  COL_INT	- Integer.
 *  COL_FLOAT	- Floating point.
 *  COL_CHAR	- Single character.
 *  COL_STRING	- Flat string.
 *  COL_ROPE	- Generic rope.
 *  COL_VECTOR	- Vector.
 *  COL_MVECTOR	- Mutable vector.
 *  COL_LIST	- Generic list.
 *  COL_MLIST	- Mutable list.
 *  COL_MAP	- Map (word- or string-keyed).
 *  COL_INTMAP	- Integer-keyed map.
 *  COL_HASHMAP	- Hash map.
 *  COL_TRIEMAP	- Trie map.
 *  COL_STRBUF	- String buffer.
 *
 * See also:
 *	<Col_WordType>, <Col_CustomWordType>
 *---------------------------------------------------------------------------*/

#define COL_NIL			0x0000
#define COL_CUSTOM		0x0001
#define COL_BOOL		0x0002
#define COL_INT			0x0004
#define COL_FLOAT		0x0008
#define COL_CHAR		0x0010
#define COL_STRING		0x0020
#define COL_ROPE		0x0040
#define COL_VECTOR		0x0080
#define COL_MVECTOR		0x0100
#define COL_LIST		0x0200
#define COL_MLIST		0x0400
#define COL_MAP			0x0800
#define COL_INTMAP		0x1000
#define COL_HASHMAP		0x2000
#define COL_TRIEMAP		0x4000
#define COL_STRBUF		0x8000


/****************************************************************************
 * Group: Word Creation
 *
 * Declarations:
 *	<Col_NewIntWord>, <Col_NewFloatWord>
 ****************************************************************************/

EXTERN Col_Word		Col_NewBoolWord(int value);
EXTERN Col_Word		Col_NewIntWord(intptr_t value);
EXTERN Col_Word		Col_NewFloatWord(double value);


/****************************************************************************
 * Group: Word Accessors and Synonyms
 *
 * Declarations:
 *	<Col_WordType>, <Col_BoolWordValue>, <Col_IntWordValue>, 
 *	<Col_FloatWordValue>, <Col_WordSynonym>, <Col_WordAddSynonym>, 
 *	<Col_WordClearSynonym>
 ****************************************************************************/

EXTERN int		Col_WordType(Col_Word word);
EXTERN int		Col_BoolWordValue(Col_Word word);
EXTERN intptr_t		Col_IntWordValue(Col_Word word);
EXTERN double		Col_FloatWordValue(Col_Word word);
EXTERN Col_Word		Col_WordSynonym(Col_Word word);
EXTERN void		Col_WordAddSynonym(Col_Word *wordPtr, Col_Word synonym);
EXTERN void		Col_WordClearSynonym(Col_Word word);


/****************************************************************************
 * Group: Word Lifetime Management
 *
 * Declarations:
 *	<Col_WordPreserve>, <Col_WordRelease>
 ****************************************************************************/

EXTERN void		Col_WordPreserve(Col_Word word);
EXTERN void		Col_WordRelease(Col_Word word);


/****************************************************************************
 * Group: Word Operations
 *
 * Declarations:
 *	<Col_SortWords>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Typedef: Col_WordCompareProc
 *
 *	Function signature of word comparison function. Used for sorting.
 *
 * Arguments:
 *	w1, w1		- Words to compare.
 *	clientData	- Opaque client data. Typically passed to the calling 
 *			  proc (e.g. <Col_SortWords>).
 *
 * Result:
 *	Negative if w1 is less than w2, positive if w1 is greater than w2, zero
 *	if both words are equal.
 *
 * See also: 
 *	<Col_SortWords>
 *---------------------------------------------------------------------------*/

typedef int (Col_WordCompareProc) (Col_Word w1, Col_Word w2,
    Col_ClientData clientData);

/*
 * Remaining declarations.
 */

EXTERN void		Col_SortWords(Col_Word *first, Col_Word *last, 
			    Col_WordCompareProc *proc, 
			    Col_ClientData clientData);


/*
================================================================================
Section: Custom Words
================================================================================
*/

/*---------------------------------------------------------------------------
 * Typedef: Col_CustomWordSizeProc
 *
 *	Function signature of custom word size procs.
 *
 * Argument:
 *	word	- Custom word to get size for.
 *
 * Result:
 *	The custom word size in bytes.
 *
 * See also: 
 *	<Col_CustomWordType>
 *---------------------------------------------------------------------------*/

typedef size_t (Col_CustomWordSizeProc) (Col_Word word);

/*---------------------------------------------------------------------------
 * Typedef: Col_CustomWordFreeProc
 *
 *	Function signature of custom word cleanup procs. Called on collected 
 *	words during the sweep phase of the garbage collection.
 *
 * Argument:
 *	word	- Custom word to cleanup.
 *
 * See also: 
 *	<Col_CustomWordType>
 *---------------------------------------------------------------------------*/

typedef void (Col_CustomWordFreeProc) (Col_Word word);

/*---------------------------------------------------------------------------
 * Typedef: Col_CustomWordChildEnumProc
 *
 *	Function signature of custom word child enumeration procs. Called during
 *	the mark phase of the garbage collection. Words are movable, so pointer 
 *	values may be modified in the process. 
 *
 * Arguments:
 *	word		- Custom word whose child is being followed.
 *	childPtr	- Pointer to child, may be overwritten if moved.
 *	clientData	- Opaque client data. Same value as passed to 
 *			  <Col_CustomWordChildrenProc>
 *
 * See also: 
 *	<Col_CustomWordType>, <Col_CustomWordChildrenProc>
 *---------------------------------------------------------------------------*/

typedef void (Col_CustomWordChildEnumProc) (Col_Word word, Col_Word *childPtr, 
    Col_ClientData clientData);

/*---------------------------------------------------------------------------
 * Typedef: Col_CustomWordChildrenProc
 *
 *	Function signature of custom word child enumeration procs. Called during
 *	the mark phase of the garbage collection.
 *
 * Arguments:
 *	word		- Custom word to follow children for.
 *	proc		- Callback proc called at each child.
 *	clientData	- Opaque data passed as is to above proc.
 *
 * See also: 
 *	<Col_CustomWordType>, <Col_CustomWordChildEnumProc>
 *---------------------------------------------------------------------------*/

typedef void (Col_CustomWordChildrenProc) (Col_Word word, 
    Col_CustomWordChildEnumProc *proc, Col_ClientData clientData);

/*---------------------------------------------------------------------------
 * Typedef: Col_CustomWordType
 *
 *	Custom word type descriptor. 
 *
 * Fields:
 *	type		- Type identifier.
 *	name		- Name of the type, e.g. "int".
 *	sizeProc	- Called to get the size in bytes of the word. Must be 
 *			  constant during the whole lifetime.
 *	freeProc	- Called once the word gets collected. NULL if not 
 *			  necessary.
 *	childrenProc	- Called during the garbage collection to iterate over 
 *			  the words owned by the word, in no special order. If 
 *			  NULL, do nothing.
 *
 * See also:
 *	<Col_NewCustomWord>, <Col_CustomWordType>, <Col_CustomWordSizeProc>, 
 *	<Col_CustomWordFreeProc>, <Col_CustomWordChildrenProc>,
 *	<Word Type Identifiers>
 *---------------------------------------------------------------------------*/

typedef struct Col_CustomWordType {
    int type;
    const char *name;
    Col_CustomWordSizeProc *sizeProc;
    Col_CustomWordFreeProc *freeProc;
    Col_CustomWordChildrenProc *childrenProc;
} Col_CustomWordType;


/****************************************************************************
 * Group: Custom Word Creation
 *
 * Declarations:
 *	<Col_NewCustomWord>
 ****************************************************************************/

EXTERN Col_Word		Col_NewCustomWord(Col_CustomWordType *type, size_t size, 
			    void **dataPtr);


/****************************************************************************
 * Group: Custom Word Accessors
 *
 * Declarations:
 *	<Col_CustomWordInfo>
 ****************************************************************************/

EXTERN Col_CustomWordType * Col_CustomWordInfo(Col_Word word, void **dataPtr);

#endif /* _COLIBRI_WORD */
