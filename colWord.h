#ifndef _COLIBRI_WORD
#define _COLIBRI_WORD

#include <stddef.h> /* For size_t, uintptr_t and the like */
#include <limits.h> /* For INT_MIN */


/* 
 * Colibri words are opaque types.
 */

typedef uintptr_t Col_Word;

/*
 * Nil is a valid word.
 */

#define WORD_NIL		((Col_Word) 0)

/*
 * String characters use the 32-bit Unicode encoding.
 */

typedef unsigned int Col_Char;
#define COL_CHAR_INVALID	((Col_Char) -1)

/* 
 * Strings can use various formats. 
 *
 * Note: we assume that UTF-8 data is always well-formed. It is up to the 
 * caller responsibility to validate and ensure well-formedness of UTF-8 data, 
 * notably for security reasons. 
 */

typedef uint8_t Col_Char1;
typedef uint16_t Col_Char2;
typedef uint32_t Col_Char4;

typedef enum Col_StringFormat {
    COL_UTF8=0, COL_UCS1=1, COL_UCS2=2, COL_UCS4=4
} Col_StringFormat;

/*
 * Word data types.
 */

typedef enum {
    COL_NIL,
    COL_CUSTOM,
    COL_CHAR,
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

/*
 * Function types and structures for word types.
 */

typedef size_t (Col_CustomWordSizeProc) (Col_Word word);
typedef void (Col_CustomWordFreeProc) (Col_Word word);
typedef void (Col_CustomWordChildEnumProc) (Col_Word word, Col_Word *childPtr, 
    Col_ClientData clientData);
typedef void (Col_CustomWordChildrenProc) (Col_Word word, 
    Col_CustomWordChildEnumProc *proc, Col_ClientData clientData);

typedef struct Col_CustomWordType {
    Col_WordType type;		/* Type identifier. */
    const char *name;		/* Name of the type, e.g. "int". */
    Col_CustomWordSizeProc *sizeProc;	
				/* Called to get the size of the word. Must 
				 * be constant during the whole lifetime. */
    Col_CustomWordFreeProc *freeProc;	
				/* Called once the word gets collected. NULL if
				 * not necessary. */
    Col_CustomWordChildrenProc *childrenProc;
				/* Called to iterate over the words owned by
				 * the word, in no special order. Called during
				 * the garbage collection. Words are movable, 
				 * so pointer values may be modified in the 
				 * process. If NULL, do nothing. */
} Col_CustomWordType;

typedef union {
    Col_Char ch;		/* COL_CHAR. */
    int i;			/* COL_INT. */
    struct {			/* COL_STRING. */
	Col_StringFormat format;
	const void *data;
	size_t byteLength;
	Col_Word _smallData;
    } string;
    struct {			/* COL_VECTOR. */
	size_t length;
	const Col_Word *elements;
    } vector;
    struct {			/* COL_MVECTOR. */
	size_t maxLength;
	size_t length;
	Col_Word *elements;
    } mvector;
				/* Other predefined types use accessors and iterators. */
    struct {
	Col_CustomWordType *type;
	void *data;
    } custom;			/* Custom words. */
} Col_WordData;

/*
 * Predefined words.
 */

EXTERN Col_Word		Col_NewIntWord(int value);

/*
 * Custom words.
 */

EXTERN Col_Word		Col_NewCustomWord(Col_CustomWordType *type, size_t size, 
			    void **dataPtr);

/*
 * Word values.
 */

EXTERN Col_WordType	Col_GetWordInfo(Col_Word word, Col_WordData *dataPtr);
EXTERN Col_Word		Col_FindWordInfo(Col_Word word, Col_WordType type, 
			    Col_WordData *dataPtr);
EXTERN Col_Word		Col_GetWordSynonym(Col_Word word);
EXTERN void		Col_AddWordSynonym(Col_Word *wordPtr, Col_Word synonym,
			    Col_Word parent);
EXTERN void		Col_ClearWordSynonym(Col_Word word);

/*
 * Child declaration needed by the generational GC.
 */

EXTERN void		Col_WordSetModified(Col_Word word);


/* 
 *----------------------------------------------------------------
 * Lifetime management. 
 *----------------------------------------------------------------
 */

EXTERN void		Col_PreserveWord(Col_Word word);
EXTERN void		Col_ReleaseWord(Col_Word word);

#endif /* _COLIBRI_WORD */
