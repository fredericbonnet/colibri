/**
 * @file colWord.h
 *
 * This header file defines the word handling features of Colibri.
 *
 * Words are a generic abstract datatype framework used in conjunction with
 * the exact generational garbage collector and the cell-based allocator.
 */

#ifndef _COLIBRI_WORD
#define _COLIBRI_WORD

#include <stddef.h> /* For size_t, uintptr_t and the like */


/*
===========================================================================*//*!
\defgroup words Words

Words are a generic abstract datatype framework used in conjunction with
the exact generational garbage collector and the cell-based allocator.
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Basic Word Types & Constants
 ***************************************************************************\{*/

/** Colibri words are opaque types. */
typedef uintptr_t Col_Word;

/** Nil word. */
#define WORD_NIL \
    ((Col_Word) 0)

/* End of Basic Word Types & Constants *//*!\}*/


/***************************************************************************//*!
 * \name Boolean Values
 *
 * Boolean singleton words. Values are immediate and constant, which means
 * that they can be safely compared and stored in static storage.
 *
 * @warning
 *      C and Colibri booleans are not interchangeable. More specifically,
 *      #WORD_FALSE is not false in the C sense because it is nonzero. They are
 *      also distinct from integer words: an integer zero is not a boolean false
 *      contrary to C. So it is an error to write e.g. "if (WORD_FALSE)".
 ***************************************************************************\{*/

/** False boolean word. */
#define WORD_FALSE \
    ((Col_Word) 0x004)

/** True boolean word. */
#define WORD_TRUE \
    ((Col_Word) 0x104)

/* End of Boolean Values *//*!\}*/


/***************************************************************************//*!
 * @anchor word_types
 * \name Word Types
 *
 * Data types recognized by Colibri. Values are OR-able so that a word can
 * match several types (e.g. a custom rope type as returned by
 * Col_WordType() would be #COL_CUSTOM | #COL_ROPE ).
 *
 * @see Col_WordType
 * @see Col_CustomWordType
 ***************************************************************************\{*/

#define COL_NIL         0x0000  /*!< Nil. */
#define COL_CUSTOM      0x0001  /*!< Custom type. */
#define COL_BOOL        0x0002  /*!< Boolean. */
#define COL_INT         0x0004  /*!< Integer. */
#define COL_FLOAT       0x0008  /*!< Floating point. */
#define COL_CHAR        0x0010  /*!< Single character. */
#define COL_STRING      0x0020  /*!< Flat string. */
#define COL_ROPE        0x0040  /*!< Generic rope. */
#define COL_VECTOR      0x0080  /*!< Vector. */
#define COL_MVECTOR     0x0100  /*!< Mutable vector. */
#define COL_LIST        0x0200  /*!< Generic list. */
#define COL_MLIST       0x0400  /*!< Mutable list. */
#define COL_MAP         0x0800  /*!< Map (word- or string-keyed). */
#define COL_INTMAP      0x1000  /*!< Integer-keyed map. */
#define COL_HASHMAP     0x2000  /*!< Hash map. */
#define COL_TRIEMAP     0x4000  /*!< Trie map. */
#define COL_STRBUF      0x8000  /*!< String buffer. */

/* End of Word Types *//*!\}*/


/***************************************************************************//*!
 * \name Word Creation
 ***************************************************************************\{*/

EXTERN Col_Word     Col_NewBoolWord(int value);
EXTERN Col_Word     Col_NewIntWord(intptr_t value);
EXTERN Col_Word     Col_NewFloatWord(double value);

/* End of Word Creation *//*!\}*/


/***************************************************************************//*!
 * \name Word Accessors
 ***************************************************************************\{*/

EXTERN int          Col_WordType(Col_Word word);
EXTERN int          Col_BoolWordValue(Col_Word word);
EXTERN intptr_t     Col_IntWordValue(Col_Word word);
EXTERN double       Col_FloatWordValue(Col_Word word);

/* End of Word Accessors *//*!\}*/


/***************************************************************************//*!
 * \name Word Synonyms
 ***************************************************************************\{*/

EXTERN Col_Word     Col_WordSynonym(Col_Word word);
EXTERN void         Col_WordAddSynonym(Col_Word *wordPtr, Col_Word synonym);
EXTERN void         Col_WordClearSynonym(Col_Word word);

/* End of Word Synonyms *//*!\}*/


/***************************************************************************//*!
 * \name Word Lifetime Management
 ***************************************************************************\{*/

EXTERN void         Col_WordPreserve(Col_Word word);
EXTERN void         Col_WordRelease(Col_Word word);

/* End of Word Lifetime Management *//*!\}*/


/***************************************************************************//*!
 * \name Word Operations
 ***************************************************************************\{*/

/**
 * Function signature of word comparison function. Used for sorting.
 *
 * @param w1, w2        Words to compare.
 * @param clientData    Opaque client data. Typically passed to the calling
 *                      proc (e.g. Col_SortWords()).
 *
 * @retval negative     if **w1** is less than **w2**.
 * @retval positive     if **w1** is greater than **w2**.
 * @retval zero         if both words are equal.
 *
 * @see Col_SortWords
 */
typedef int (Col_WordCompareProc) (Col_Word w1, Col_Word w2,
    Col_ClientData clientData);

/*
 * Remaining declarations.
 */

EXTERN void     Col_SortWords(Col_Word *first, Col_Word *last,
                Col_WordCompareProc *proc,
                Col_ClientData clientData);

/* End of Word Operations *//*!\}*/

/* End of Words *//*!\}*/


/*
===========================================================================*//*!
\defgroup custom_words Custom Words
\ingroup words

Custom words are words defined by applicative code. They can extend existing
word types like ropes or maps, or define application-specific data types.

@see Col_CustomWordType
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Custom Word Type Descriptors
 ***************************************************************************\{*/

/**
 * Function signature of custom word size procs.
 *
 * @param word  Custom word to get size for.
 *
 * @return The custom word size in bytes.
 *
 * @see Col_CustomWordType
 */
typedef size_t (Col_CustomWordSizeProc) (Col_Word word);

/**
 * Function signature of custom word cleanup procs. Called on collected
 * words during the sweep phase of the garbage collection.
 *
 * @param word  Custom word to cleanup.
 *
 * @see Col_CustomWordType
 */
typedef void (Col_CustomWordFreeProc) (Col_Word word);

/**
 * Function signature of custom word child enumeration procs. Called during
 * the mark phase of the garbage collection. Words are movable, so pointer
 * values may be modified in the process.
 *
 * @param word          Custom word whose child is being followed.
 * @param childPtr      Pointer to child, may be overwritten if moved.
 * @param clientData    Opaque client data. Same value as passed to
 *                      Col_CustomWordChildrenProc().
 *
 * @see Col_CustomWordType
 * @see Col_CustomWordChildrenProc
 */
typedef void (Col_CustomWordChildEnumProc) (Col_Word word, Col_Word *childPtr,
    Col_ClientData clientData);

/**
 * Function signature of custom word child enumeration procs. Called during
 * the mark phase of the garbage collection.
 *
 * @param word          Custom word to follow children for.
 * @param proc          Callback proc called at each child.
 * @param clientData    Opaque data passed as is to above proc.
 * @see Col_CustomWordType
 * @see Col_CustomWordChildEnumProc
 */
typedef void (Col_CustomWordChildrenProc) (Col_Word word,
    Col_CustomWordChildEnumProc *proc, Col_ClientData clientData);

/**
 * Custom word type descriptor.
 *
 * @see @ref word_types "Word Types"
 */
typedef struct Col_CustomWordType {
    /*! Type identifier. */
    int type;

    /*! Name of the type, e.g.\ "int". */
    const char *name;

    /*! Called to get the size in bytes of the word. Must be constant during
        the whole lifetime. */
    Col_CustomWordSizeProc *sizeProc;

    /*! Called once the word gets collected. NULL if not needed. */
    Col_CustomWordFreeProc *freeProc;

    /*! Called during the garbage collection to iterate over the words owned by
        the word, in no special order. If NULL, do nothing. */
    Col_CustomWordChildrenProc *childrenProc;
} Col_CustomWordType;

/* End of Custom Word Type Descriptors *//*!\}*/


/***************************************************************************//*!
 * \name Custom Word Creation
 ***************************************************************************\{*/

EXTERN Col_Word     Col_NewCustomWord(Col_CustomWordType *type, size_t size,
                        void **dataPtr);

/* End of Custom Word Creation *//*!\}*/


/***************************************************************************//*!
 * \name Custom Word Accessors
 ***************************************************************************\{*/

EXTERN Col_CustomWordType * Col_CustomWordInfo(Col_Word word, void **dataPtr);

/* End of Custom Word Accessors *//*!\}*/

/* End of Custom Words *//*!\}*/

#endif /* _COLIBRI_WORD */
