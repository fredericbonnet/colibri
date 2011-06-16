/*
 * File: colWord.h
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
 * Group: Word Types
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

#define WORD_NIL		((Col_Word) 0)

/*---------------------------------------------------------------------------
 * Enum: Col_WordType
 *
 *	Data types recognized by Colibri.
 *
 *  COL_NIL	- Nil.
 *  COL_CUSTOM	- Custom data type.
 *  COL_STRING	- String (including single characters and ropes).
 *  COL_ROPE	- Custo rope type.
 *  COL_INT	- Integer
 *  COL_VECTOR	- Vector.
 *  COL_MVECTOR	- Mutable vector.
 *  COL_LIST	- List.
 *  COL_MLIST	- Mutable list.
 *  COL_MAP	- Map.
 *  COL_STRMAP	- String-keyed map.
 *  COL_INTMAP	- Integer-keyed map.
 *  COL_HASHMAP	- Custom hash map type.
 *
 * See also:
 *	<Col_GetWordInfo>, <Col_FindWordInfo>
 *---------------------------------------------------------------------------*/

typedef enum {
    COL_NIL,
    COL_CUSTOM,
    COL_STRING,
    COL_ROPE,
    COL_INT,
    COL_VECTOR,
    COL_MVECTOR,
    COL_LIST,
    COL_MLIST,
    COL_MAP,
    COL_STRMAP,
    COL_INTMAP,
    COL_HASHMAP
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
 *	string			- <COL_STRING>:
 *	string.format		- Format of the string. See <Col_StringFormat>.
 *	string.data		- Pointer to format-specific data.
 *	string.bytelength	- Data length in bytes.
 *	vector			- <COL_VECTOR>:
 *	vector.length		- Length of vector.
 *	vector.elements		- Array of word elements.
 *	mvector			- <COL_MVECTOR>:
 *	mvector.maxLength	- Maximum length of vector.
 *	mvector.length		- Actual length of vector.
 *	mvector.elements	- Array of word elements.
 *	custom			- Custom types:
 *	custom.type		- The word type.
 *	custom.data		- Pointer to the allocated data.
 *
 * See also:
 *	<Col_WordType>, <Col_GetWordInfo>, <Col_FindWordInfo>
 *---------------------------------------------------------------------------*/

typedef union {
    intptr_t i;
    struct {
	Col_StringFormat format;
	const void *data;
	size_t byteLength;
	uintptr_t _smallData;
    } string;
    struct {
	size_t length;
	const Col_Word *elements;
    } vector;
    struct {
	size_t maxLength;
	size_t length;
	Col_Word *elements;
    } mvector;
    struct {
	struct Col_CustomWordType *type;
	void *data;
    } custom;
} Col_WordData;


/****************************************************************************
 * Group: Custom Word Types
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Typedef: Col_CustomWordSizeProc
 *
 *	Function signature of custom word size procs.
 *
 * Arguments:
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
 * Arguments:
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
 * Group: Word Creation
 *
 * Declarations:
 *	<Col_NewIntWord>, <Col_NewCustomWord>
 ****************************************************************************/

EXTERN Col_Word		Col_NewIntWord(intptr_t value);
EXTERN Col_Word		Col_NewCustomWord(Col_CustomWordType *type, size_t size, 
			    void **dataPtr);


/****************************************************************************
 * Group: Word Access and Synonyms
 *
 * Declarations:
 *	<Col_GetWordInfo>, <Col_FindWordInfo>, <Col_GetWordSynonym>,
 *	<Col_AddWordSynonym>, <Col_ClearWordSynonym>
 ****************************************************************************/

EXTERN Col_WordType	Col_GetWordInfo(Col_Word word, Col_WordData *dataPtr);
EXTERN Col_Word		Col_FindWordInfo(Col_Word word, Col_WordType type, 
			    Col_WordData *dataPtr);
EXTERN Col_Word		Col_GetWordSynonym(Col_Word word);
EXTERN void		Col_AddWordSynonym(Col_Word *wordPtr, Col_Word synonym,
			    Col_Word parent);
EXTERN void		Col_ClearWordSynonym(Col_Word word);


/****************************************************************************
 * Group: Word Modification and Lifetime Management
 *
 * Declarations:
 *	<Col_WordSetModified>, <Col_PreserveWord>, <Col_ReleaseWord>
 ****************************************************************************/

EXTERN void		Col_WordSetModified(Col_Word word);
EXTERN void		Col_PreserveWord(Col_Word word);
EXTERN void		Col_ReleaseWord(Col_Word word);

#endif /* _COLIBRI_WORD */
