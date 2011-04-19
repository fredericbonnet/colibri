#ifndef _COLIBRI_WORD
#define _COLIBRI_WORD

/* 
 * Colibri words are opaque types.
 */

typedef const void * Col_Word;

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
 * Word data types.
 */

#define COL_NULL		((Col_WordType *) 0)	/* No value. */
#define COL_INT			((Col_WordType *) 1)	/* Integer. */
#define COL_CHAR		((Col_WordType *) 2)	/* Unicode character. */
#define COL_SMALL_STRING	((Col_WordType *) 3)	/* Small string. */
#define COL_ROPE		((Col_WordType *) 4)	/* Rope (string). */
#define COL_VECTOR		((Col_WordType *) 5)	/* Flat vector. */
#define COL_LIST		((Col_WordType *) 6)	/* List. */

typedef union {
    int i;			/* COL_INT. */
    Col_Char ch;		/* COL_CHAR. */
    struct {			/* COL_SMALL_STRING. */
	char length;
	char data[sizeof(Col_Word)-1];
    } str;
    Col_Rope rope;		/* COL_ROPE. */
    struct {
	size_t length;
	const Col_Word *elements;
    } vector;			/* COL_VECTOR. */
				/* COL_LIST use accessors and iterators. */
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
EXTERN Col_Word		Col_AddWordSynonym(Col_Word word, Col_Word synonym);
EXTERN void		Col_ClearWordSynonym(Col_Word word);


/* 
 *----------------------------------------------------------------
 * Lifetime management. 
 *----------------------------------------------------------------
 */

EXTERN Col_Word		Col_PreserveWord(Col_Word word);
EXTERN Col_Word		Col_ReleaseWord(Col_Word word);

#endif /* _COLIBRI_WORD */
