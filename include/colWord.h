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


/****************************************************************************
 * Section: Word Types
 ****************************************************************************/

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
 * Enum: Col_WordType
 *
 *	Data types recognized by Colibri. Subtype identifiers are OR'ed with 
 *	generic ones. For example, flat strings are also ropes, so (<COL_STRING>
 *	& <COL_ROPE>) == <COL_ROPE>.
 *
 *  COL_NIL	- Nil.
 *  COL_CUSTOM	- Custom data type.
 *  COL_INT	- Integer.
 *  COL_FLOAT	- Floating point.
 *  COL_ROPE	- Generic rope.
 *  COL_STRING	- Flat string.
 *  COL_LIST	- Generic list.
 *  COL_MLIST	- Mutable list.
 *  COL_VECTOR	- Vector.
 *  COL_MVECTOR	- Mutable vector.
 *  COL_STRMAP	- String-keyed map.
 *  COL_INTMAP	- Integer-keyed map.
 *  COL_HASHMAP	- Custom hash map type.
 *
 * See also:
 *	<Col_GetWordInfo>, <Col_FindWordInfo>, <Col_CustomWordType>
 *---------------------------------------------------------------------------*/

typedef enum {
    COL_NIL	= 1,
    COL_CUSTOM	= 2,
    COL_INT	= 4,
    COL_FLOAT	= 8,
    COL_ROPE	= 16,
    COL_STRING	= 48,		/* = 32 | COL_ROPE */
    COL_LIST	= 64,
    COL_MLIST	= 192,		/* = 128 | COL_LIST */
    COL_VECTOR	= 320,		/* = 256 | COL_LIST */
    COL_MVECTOR	= 960,		/* = 512 | COL_VECTOR | COL_MLIST */
    COL_STRMAP	= 1024,
    COL_INTMAP	= 2048,
    COL_HASHMAP	= 4096,
    COL_STRBUF = 8192
} Col_WordType;

/*---------------------------------------------------------------------------
 * Typedef: Col_WordData
 *
 *	Colibri words are opaque types, this structure can be used to get the 
 *	underlying data of predefined types.
 *
 *	This type is a union so only the member matching the actual type is 
 *	valid.
 *
 * Fields:
 *	i			- <COL_INT>.
 *	f			- <COL_FLOAT>.
 *	string			- <COL_STRING>:
 *	string.format		- Format of the string. See <Col_StringFormat>.
 *	string.data		- Pointer to format-specific data.
 *	string.bytelength	- Data length in bytes.
 *	vector			- <COL_VECTOR>:
 *	vector.length		- Length of vector.
 *	vector.elements		- Array of word elements.
 *	mvector			- <COL_MVECTOR>:
 *	mvector.length		- Actual length of vector.
 *	mvector.elements	- Array of word elements.
 *	mvector.maxLength	- Maximum length of vector.
 *	custom			- Custom types:
 *	custom.type		- The word type.
 *	custom.data		- Pointer to the allocated data.
 *
 * See also:
 *	<Col_WordType>, <Col_GetWordInfo>, <Col_FindWordInfo>, 
 *	<Col_CustomWordType>
 *---------------------------------------------------------------------------*/

typedef union {
    intptr_t i;
    double f;
    struct {
	Col_StringFormat format;
	const void *data;
	size_t byteLength;
	union {
	    Col_Char1 c1;
	    Col_Char2 c2;
	    Col_Char4 c4;
	    Col_Word s;
	} _smallData;
    } string;
    struct {
	size_t length;
	const Col_Word *elements;
    } vector;
    struct {
	size_t length;
	Col_Word *elements;
	size_t maxLength;
    } mvector;
    struct {
	struct Col_CustomWordType *type;
	void *data;
    } custom;
} Col_WordData;


/****************************************************************************
 * Section: Custom Word Types
 ****************************************************************************/

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
 *	type		- Type identifier. Type field must be equal to 
 *			  <COL_CUSTOM>.
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
 *	<Col_CustomWordSizeProc>, <Col_CustomWordFreeProc>, 
 *	<Col_CustomWordChildrenProc>
 *---------------------------------------------------------------------------*/

typedef struct Col_CustomWordType {
    Col_WordType type;
    const char *name;
    Col_CustomWordSizeProc *sizeProc;
    Col_CustomWordFreeProc *freeProc;
    Col_CustomWordChildrenProc *childrenProc;
} Col_CustomWordType;


/****************************************************************************
 * Section: Word Creation
 *
 * Declarations:
 *	<Col_NewIntWord>, <Col_NewFloatWord>, <Col_NewCustomWord>
 ****************************************************************************/

EXTERN Col_Word		Col_NewIntWord(intptr_t value);
EXTERN Col_Word		Col_NewFloatWord(double value);
EXTERN Col_Word		Col_NewCustomWord(Col_CustomWordType *type, size_t size, 
			    void **dataPtr);


/****************************************************************************
 * Section: Word Access and Synonyms
 *
 * Declarations:
 *	<Col_IntWordValue>, <Col_FloatWordValue>, <Col_GetWordInfo>, 
 *	<Col_FindWordInfo>, <Col_GetWordSynonym>, <Col_AddWordSynonym>, 
 *	<Col_ClearWordSynonym>
 ****************************************************************************/

EXTERN intptr_t		Col_IntWordValue(Col_Word word);
EXTERN double		Col_FloatWordValue(Col_Word word);
EXTERN Col_WordType	Col_GetWordInfo(Col_Word word, Col_WordData *dataPtr);
EXTERN Col_Word		Col_FindWordInfo(Col_Word word, Col_WordType type, 
			    Col_WordData *dataPtr, int *altTypePtr, 
			    Col_Word *altWordPtr);
EXTERN Col_Word		Col_GetWordSynonym(Col_Word word);
EXTERN void		Col_AddWordSynonym(Col_Word *wordPtr, Col_Word synonym);
EXTERN void		Col_ClearWordSynonym(Col_Word word);


/****************************************************************************
 * Section: Word Lifetime Management
 *
 * Declarations:
 *	<Col_PreserveWord>, <Col_ReleaseWord>
 ****************************************************************************/

EXTERN void		Col_PreserveWord(Col_Word word);
EXTERN void		Col_ReleaseWord(Col_Word word);

#endif /* _COLIBRI_WORD */
