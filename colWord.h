#ifndef _COLIBRI_WORD
#define _COLIBRI_WORD

/* 
 * Colibri words are opaque types.
 */

typedef uintptr_t Col_Word;

/*
 * Nil is a valid word.
 */

#define WORD_NIL		((Col_Word) 0)

/*
 * Function types and structures for word types.
 */

typedef size_t (Col_WordSizeProc) (Col_Word word);
typedef void (Col_WordFreeProc) (Col_Word word);

typedef void (Col_WordChildEnumProc) (Col_Word word, Col_Word *childPtr, 
    Col_ClientData clientData);
typedef void (Col_WordChildrenProc) (Col_Word word, Col_WordChildEnumProc *proc,
    Col_ClientData clientData);

typedef struct Col_WordType {
    const char *name;		/* Name of the type, e.g. "int". */
    Col_WordSizeProc *sizeProc;	/* Called to get the size of the word. Must 
				 * be constant during the whole lifetime. */
    Col_WordFreeProc *freeProc;	/* Called once the word gets collected. NULL if
				 * not necessary. */
    Col_WordChildrenProc *childrenProc;
				/* Called to iterate over the words owned by
				 * the word, in no special order. Called during
				 * the garbage collection. Words are movable, 
				 * so pointer values may be modified in the 
				 * process. If NULL, do nothing. */
} Col_WordType;

/*
 * Word data types. Use odd values to ensure no clash with Col_WordType 
 * pointer values.
 */

#define COL_NIL			((Col_WordType *) 0)	/* Nil word. */
#define COL_INT			((Col_WordType *) 1)	/* Integer. */
#define COL_CHAR		((Col_WordType *) 3)	/* Unicode character. */
#define COL_SMALL_STRING	((Col_WordType *) 5)	/* Small string. */
#define COL_ROPE		((Col_WordType *) 7)	/* Rope (string). */
#define COL_REFERENCE		((Col_WordType *) 9)	/* Reference. */
#define COL_VECTOR		((Col_WordType *) 11)	/* Flat vector. */
#define COL_MVECTOR		((Col_WordType *) 13)	/* Flat mutable vector. */
#define COL_LIST		((Col_WordType *) 15)	/* List. */
#define COL_MLIST		((Col_WordType *) 17)	/* Mutable list. */
#define COL_MAP			((Col_WordType *) 19)	/* Map (any kind). */

typedef union {
    int i;			/* COL_INT. */
    Col_Char ch;		/* COL_CHAR. */
    struct {			/* COL_SMALL_STRING. */
	char length;
	const Col_Char1 data[sizeof(Col_Word)-1];
    } str;
    Col_Rope rope;		/* COL_ROPE. */
    Col_Word refSource;		/* COL_REFERENCE. */
    struct {			/* COL_VECTOR. */
	size_t length;
	const Col_Word *elements;
    } vector;
    struct {			/* COL_MVECTOR. */
	size_t maxLength;
	size_t length;
	Col_Word *elements;
    } mvector;
				/* COL_(M)LIST and COL_MAP use accessors and iterators. */
    void *data;			/* Other values = (Col_WordType *). */
} Col_WordData;

/*
 * Word creation.
 */

EXTERN Col_Word		Col_NewIntWord(int value);
EXTERN Col_Word		Col_NewStringWord(Col_StringFormat format, 
			    const void *data,  size_t byteLength);
EXTERN Col_Word		Col_NewRopeWord(Col_Rope rope);
EXTERN Col_Word		Col_NewWord(Col_WordType *type, size_t size, 
			    void **dataPtr);

/*
 * Word values.
 */

EXTERN Col_WordType *	Col_GetWordInfo(Col_Word word, Col_WordData *dataPtr);
EXTERN Col_Word		Col_FindWordInfo(Col_Word word, Col_WordType * type, Col_WordData *dataPtr);
EXTERN Col_Word		Col_GetWordSynonym(Col_Word word);
EXTERN void		Col_AddWordSynonym(Col_Word *wordPtr, Col_Word synonym, Col_Word *parentPtr);
EXTERN void		Col_ClearWordSynonym(Col_Word word);


/* 
 *----------------------------------------------------------------
 * Lifetime management. 
 *----------------------------------------------------------------
 */

EXTERN void		Col_PreserveWord(Col_Word word);
EXTERN void		Col_ReleaseWord(Col_Word word);

#endif /* _COLIBRI_WORD */
