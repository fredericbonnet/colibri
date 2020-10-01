/**
 * @file colRope.h
 *
 * This header file defines the rope handling features of Colibri.
 *
 * Ropes are a string datatype that allows for fast insertion, extraction
 * and composition of strings. Internally they use self-balanced binary
 * trees.
 *
 * They are always immutable.
 */

#ifndef _COLIBRI_ROPE
#define _COLIBRI_ROPE

#include <stddef.h> /* For size_t */
#include <stdarg.h> /* For variadic procs */
#include <string.h> /* for memset */


/*
===========================================================================*//*!
\defgroup rope_words Ropes
\ingroup words

Ropes are a string datatype that allows for fast insertion, extraction
and composition of strings.
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Rope Creation
 ***************************************************************************\{*/

/**
 * When passed to Col_NewRope() or Col_NormalizeRope(), use the shortest
 * possible fixed-width format. Input format is always #COL_UCS4.
 *
 * @note
 *      Numeric value is chosen so that the lower 3 bits give the character
 *      width in the data chunk.
 */
#define COL_UCS                 (Col_StringFormat) 0x24

/*
 * Remaining declarations.
 */

EXTERN Col_Word         Col_EmptyRope();
EXTERN Col_Word         Col_NewRopeFromString(const char *string);
EXTERN Col_Word         Col_NewCharWord(Col_Char c);
EXTERN Col_Word         Col_NewRope(Col_StringFormat format, const void *data,
                            size_t byteLength);
EXTERN Col_Word         Col_NormalizeRope(Col_Word rope,
                            Col_StringFormat format, Col_Char replace,
                            int flatten);

/* End of Rope Creation *//*!\}*/


/***************************************************************************//*!
 * \name Rope Accessors
 ***************************************************************************\{*/

EXTERN Col_Char         Col_CharWordValue(Col_Word ch);
EXTERN Col_StringFormat Col_StringWordFormat(Col_Word string);
EXTERN size_t           Col_RopeLength(Col_Word rope);
EXTERN unsigned char    Col_RopeDepth(Col_Word rope);
EXTERN Col_Char         Col_RopeAt(Col_Word rope, size_t index);

/* End of Rope Accessors *//*!\}*/


/***************************************************************************//*!
 * \name Rope Search and Comparison
 ***************************************************************************\{*/

/**
 * Simple version of Col_RopeFind(), find first occurrence of a character
 * in whole rope from its beginning. This is the rope counterpart to C's
 * **strchr**.
 *
 * @param rope          Rope to search character into.
 * @param c             Character to search for.
 *
 * @retval SIZE_MAX     if not found (which is an invalid character index since
 *                      this is the maximum rope length, and indices are
 *                      zero-based)
 * @retval index        position of **subrope** in **rope**.
 *
 * @see Col_RopeFind
 */
#define Col_RopeFindFirst(rope, c) \
        Col_RopeFind((rope), (c), 0, SIZE_MAX, 0)

/**
 * Simple version of Col_RopeFind(), find first occurrence of a character
 * in rope from its beginning up to a given number of characters. This is
 * the rope counterpart to C's **strnchr**.
 *
 * @param rope          Rope to search character into.
 * @param c             Character to search for.
 * @param max           Maximum number of characters to search.
 *
 * @retval SIZE_MAX     if not found (which is an invalid character index since
 *                      this is the maximum rope length, and indices are
 *                      zero-based)
 * @retval index        position of **c** in **rope**.
 *
 * @see Col_RopeFind
 */
#define Col_RopeFindFirstN(rope, c, max) \
        Col_RopeFind((rope), (c), 0, (max), 0)

/**
 * Simple version of Col_RopeFind(), find last occurrence of a character
 * in whole rope from its end. This is the rope counterpart to C's **strrchr**.
 *
 * @param rope          Rope to search character into.
 * @param c             Character to search for.
 *
 * @retval SIZE_MAX     if not found (which is an invalid character index since
 *                      this is the maximum rope length, and indices are
 *                      zero-based)
 * @retval index        position of **c** in **rope**.
 *
 * @see Col_RopeFind
 */
#define Col_RopeFindLast(rope, c) \
        Col_RopeFind((rope), (c), SIZE_MAX, SIZE_MAX, 1)

/**
 * Simple version of Col_RopeFind(), find last occurrence of a character
 * in rope from its end up to a given number of characters. This function
 * has no C counterpart and is provided for symmetry.
 *
 * @param rope          Rope to search character into.
 * @param c             Character to search for.
 * @param max           Maximum number of characters to search.
 *
 * @retval SIZE_MAX     if not found (which is an invalid character index since
 *                      this is the maximum rope length, and indices are
 *                      zero-based)
 * @retval index        position of **c** in **rope**.
 *
 * @see Col_RopeFind
 */
#define Col_RopeFindLastN(rope, c, max) \
        Col_RopeFind((rope), (c), SIZE_MAX, (max), 1)

/**
 * Simple version of Col_RopeSearch(), find first occurrence of a subrope
 * in whole rope from its beginning. This is the rope counterpart to C's
 * **strstr**.
 *
 * @param rope          Rope to search subrope into.
 * @param subrope       Subrope to search for.
 *
 * @retval SIZE_MAX     if not found (which is an invalid character index since
 *                      this is the maximum rope length, and indices are
 *                      zero-based)
 * @retval index        position of **subrope** in **rope**.
 *
 * @see Col_RopeSearch
 */
#define Col_RopeSearchFirst(rope, subrope) \
        Col_RopeSearch((rope), (subrope), 0, SIZE_MAX, 0)

/**
 * Simple version of Col_RopeSearch(), find last occurrence of a subrope
 * in whole rope from its end. This function has no C counterpart and is
 * provided for symmetry.
 *
 * @param rope          Rope to search subrope into.
 * @param subrope       Subrope to search for.
 *
 * @retval SIZE_MAX     if not found (which is an invalid character index since
 *                      this is the maximum rope length, and indices are
 *                      zero-based)
 * @retval index        position of **subrope** in **rope**.
 *
 * @see Col_RopeSearch
 */
#define Col_RopeSearchLast(rope, subrope) \
        Col_RopeSearch((rope), (subrope), SIZE_MAX, SIZE_MAX, 1)

/**
 * Simple version of Col_CompareRopesL(), compare two ropes. This is the
 * rope counterpart to C's **strcmp**.
 *
 * @param rope1         First rope to compare.
 * @param rope2         Second rope to compare.
 *
 * @retval zero         if both ropes are identical
 * @retval negative     if **rope1** is lexically before **rope2**.
 * @retval positive     if **rope1** is lexically after **rope2**.
 *
 * @see Col_CompareRopesL
 */
#define Col_CompareRopes(rope1, rope2) \
        Col_CompareRopesL((rope1), (rope2), 0, SIZE_MAX, NULL, NULL, NULL)

/**
 * Simple version of Col_CompareRopesL(), compare two ropes up to a given
 * number of characters. This is the rope counterpart to C's **strncmp**.
 *
 * @param rope1         First rope to compare.
 * @param rope2         Second rope to compare.
 * @param max           Maximum number of characters to compare.
 *
 * @retval zero         if both ropes are identical
 * @retval negative     if **rope1** is lexically before **rope2**.
 * @retval positive     if **rope1** is lexically after **rope2**.
 *
 * @see Col_CompareRopesL
 */
#define Col_CompareRopesN(rope1, rope2, max) \
        Col_CompareRopesL((rope1), (rope2), 0, (max), NULL, NULL, NULL)

/*
 * Remaining declarations.
 */

EXTERN size_t           Col_RopeFind(Col_Word rope, Col_Char c, size_t start,
                            size_t max, int reverse);
EXTERN size_t           Col_RopeSearch(Col_Word rope, Col_Word subrope,
                            size_t start, size_t max, int reverse);
EXTERN int              Col_CompareRopesL(Col_Word rope1, Col_Word rope2,
                            size_t start, size_t max, size_t *posPtr,
                            Col_Char *c1Ptr, Col_Char *c2Ptr);

/* End of Rope Search and Comparison *//*!\}*/


/***************************************************************************//*!
 * \name Rope Operations
 ***************************************************************************\{*/

/**
 * Variadic macro version of Col_ConcatRopesNV() that deduces its number
 * of arguments automatically.
 *
 * @param first First rope to concatenate.
 * @param ...   Next ropes to concatenate.
 *
 * @see COL_ARGCOUNT
 */
#define Col_ConcatRopesV(first, ...) \
    _Col_ConcatRopesV(_,first, ##__VA_ARGS__)
/*! \cond IGNORE */
#define _Col_ConcatRopesV(_, ...) \
    Col_ConcatRopesNV(COL_ARGCOUNT(__VA_ARGS__),__VA_ARGS__)
/*! \endcond *//* IGNORE */

/*
 * Remaining declarations.
 */

EXTERN Col_Word         Col_Subrope(Col_Word rope, size_t first, size_t last);
EXTERN Col_Word         Col_ConcatRopes(Col_Word left, Col_Word right);
EXTERN Col_Word         Col_ConcatRopesA(size_t number, const Col_Word * ropes);
EXTERN Col_Word         Col_ConcatRopesNV(size_t number, ...);
EXTERN Col_Word         Col_RepeatRope(Col_Word rope, size_t count);
EXTERN Col_Word         Col_RopeInsert(Col_Word into, size_t index,
                            Col_Word rope);
EXTERN Col_Word         Col_RopeRemove(Col_Word rope, size_t first,
                            size_t last);
EXTERN Col_Word         Col_RopeReplace(Col_Word rope, size_t first,
                            size_t last, Col_Word with);

/* End of Rope Operations *//*!\}*/


/***************************************************************************//*!
 * \name Rope Traversal
 ***************************************************************************\{*/

/**
 * Describes a rope chunk encountered during traversal. A chunk is a
 * contiguous portion of rope with a given format.
 *
 * @see Col_TraverseRopeChunks
 * @see Col_RopeChunksTraverseProc
 */
typedef struct Col_RopeChunk {
    Col_StringFormat format;    /*!< Format of the traversed chunk. */
    const void *data;           /*!< Pointer to format-specific data. */
    size_t byteLength;          /*!< Data length in bytes. */
} Col_RopeChunk;

/**
 * Function signature of rope traversal procs.
 *
 * @param index         Rope-relative index where chunks begin.
 * @param length        Length of chunks.
 * @param number        Number of chunks.
 * @param chunks        Array of chunks. When chunk is NULL, means the index is
 *                      past the end of the traversed rope.
 * @param clientData    Opaque client data. Same value as passed to
 *                      Col_TraverseRopeChunks() procedure family.
 *
 * @retval zero         to continue traversal.
 * @retval non-zero     to stop traversal. Value is returned as result of
 *                      Col_TraverseRopeChunks() and related procs.
 */
typedef int (Col_RopeChunksTraverseProc) (size_t index, size_t length,
        size_t number, const Col_RopeChunk *chunks, Col_ClientData clientData);

/*
 * Remaining declarations.
 */

EXTERN int              Col_TraverseRopeChunksN(size_t number, Col_Word *ropes,
                            size_t start, size_t max,
                            Col_RopeChunksTraverseProc *proc,
                            Col_ClientData clientData, size_t *lengthPtr);
EXTERN int              Col_TraverseRopeChunks(Col_Word rope, size_t start,
                            size_t max, int reverse,
                            Col_RopeChunksTraverseProc *proc,
                            Col_ClientData clientData, size_t *lengthPtr);

/* End of Rope Traversal *//*!\}*/


/***************************************************************************//*!
 * \name Rope Iteration
 ***************************************************************************\{*/

/** @beginprivate @cond PRIVATE */

/**
 * Helper for rope iterators to access characters in leaves.
 *
 * @param leaf      Leaf node.
 * @param index     Leaf-relative index of character.
 *
 * @return Character at given index.
 *
 * @see ColRopeIterator
 */
typedef Col_Char (ColRopeIterLeafAtProc) (Col_Word leaf, size_t index);

/**
 * Internal implementation of rope iterators.
 *
 * @see Col_RopeIterator
 */
typedef struct ColRopeIterator {
    Col_Word rope;  /*!< Rope being iterated. If nil, use string iterator
                         mode. */
    size_t length;  /*!< Rope length. */
    size_t index;   /*!< Current position. */

    /*! Current chunk info. */
    struct {
        size_t first;   /*!< Begining of range of validity for current chunk. */
        size_t last;    /*!< End of range of validity for current chunk. */

        /*! If non-NULL, element accessor. Else, use direct address mode. */
        ColRopeIterLeafAtProc *accessProc;

        /*! Current element information. */
        union {
            /*! Current element information in accessor mode. */
            struct {
                Col_Word leaf;  /*!< First argument passed to **accessProc**. */
                size_t index;   /*!< Second argument passed to
                                     **accessProc**. */
            } access;

            /*! Current element information in direct access mode.*/
            struct {
                Col_StringFormat format;    /*!< Format of current chunk. */
                const void *address;        /*!< Address of current element. */
            } direct;
        } current;
    } chunk;
} ColRopeIterator;

/*! @endcond @endprivate */

/**
 * Rope iterator. Encapsulates the necessary info to iterate & access rope
 * data transparently.
 *
 * @note @parblock
 *      Datatype is opaque. Fields should not be accessed by client code.
 *
 *      Each iterator takes 8 words on the stack.
 *
 *      The type is defined as a single-element array of the internal datatype:
 *
 *      - declared variables allocate the right amount of space on the stack,
 *      - calls use pass-by-reference (i.e. pointer) and not pass-by-value,
 *      - forbidden as return type.
 * @endparblock
 */
typedef ColRopeIterator Col_RopeIterator[1];

/**
 * Static initializer for null rope iterators.
 *
 * @see Col_RopeIterator
 * @see Col_RopeIterNull
 * @hideinitializer
 */
#define COL_ROPEITER_NULL       {{WORD_NIL,0,0,{0,0,NULL,0,0}}}

/**
 * Test whether iterator is null (e.g.\ it has been set to #COL_ROPEITER_NULL
 * or Col_RopeIterSetNull()).
 *
 * @warning
 *      This uninitialized state renders it unusable for any call. Use with
 *      caution.
 *
 * @param it    The #Col_RopeIterator to test.
 *
 * @warning
 *      Argument **it** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 * @retval zero         if iterator if not null.
 * @retval non-zero     if iterator is null.
 *
 * @see Col_RopeIterator
 * @see COL_ROPEITER_NULL
 * @see Col_RopeIterSetNull
 * @hideinitializer
 */
#define Col_RopeIterNull(it) \
    ((it)->rope == WORD_NIL && (it)->chunk.current.direct.address == NULL)

/**
 * Set an iterator to null.
 *
 * @param it    The #Col_RopeIterator to initialize.
 *
 * @warning
 *      Argument **it** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 */
#define Col_RopeIterSetNull(it) \
    memset((it), 0, sizeof(*(it)))

/**
 * Get rope for iterator.
 *
 * @param it            The #Col_RopeIterator to access.
 *
 * @retval WORD_NIL     if iterating over string (see Col_RopeIterString()).
 * @retval rope         if iterating over rope.
 */
#define Col_RopeIterRope(it)    \
    ((it)->rope)

/**
 * Get length of the iterated sequence.
 *
 * @param it    The #Col_RopeIterator to access.
 *
 * @return Length of iterated sequence.
 */
#define Col_RopeIterLength(it) \
    ((it)->length)

/**
 * Get current index within rope for iterator.
 *
 * @param it    The #Col_RopeIterator to access.
 *
 * @return Current index.
 */
#define Col_RopeIterIndex(it) \
    ((it)->index)

/**
 * Get current rope character for iterator.
 *
 * @param it    The #Col_RopeIterator to access.
 *
 * @warning
 *      Argument **it** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 * @return Unicode codepoint of current character.
 *
 * @valuecheck{COL_ERROR_ROPEITER_END,it}
 * @hideinitializer
 */
#define Col_RopeIterAt(it) \
    (  ((it)->index < (it)->chunk.first || (it)->index > (it)->chunk.last) ? ColRopeIterUpdateTraversalInfo((ColRopeIterator *)(it)) \
     : (it)->chunk.accessProc ? (it)->chunk.accessProc((it)->chunk.current.access.leaf, (it)->chunk.current.access.index) \
     : COL_CHAR_GET((it)->chunk.current.direct.format, (it)->chunk.current.direct.address))

/**
 * Move the iterator to the next rope character.
 *
 * @param it    The #Col_RopeIterator to move.
 *
 * @warning
 *      Argument **it** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 * @see Col_RopeIterForward
 * @hideinitializer
 *
 * @typecheck{COL_ERROR_ROPEITER,it}
 * @valuecheck{COL_ERROR_ROPEITER_END,it}
 */
#define Col_RopeIterNext(it) \
    (  ((it)->index < (it)->chunk.first || (it)->index >= (it)->chunk.last) ? (Col_RopeIterForward((it), 1), 0) \
     : ((it)->index++, \
          (it)->chunk.accessProc ? ((it)->chunk.current.access.index++, 0) \
        : (COL_CHAR_NEXT((it)->chunk.current.direct.format, (it)->chunk.current.direct.address), 0)))

/**
 * Move the iterator to the previous rope character.
 *
 * @param it    The #Col_RopeIterator to move.
 *
 * @warning
 *      Argument **it** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 * @see Col_RopeIterBackward
 * @hideinitializer
 * 
 * @typecheck{COL_ERROR_ROPEITER,it}
 */
#define Col_RopeIterPrevious(it) \
    (  ((it)->index <= (it)->chunk.first || (it)->index > (it)->chunk.last) ? (Col_RopeIterBackward((it), 1), 0) \
     : ((it)->index--, \
          (it)->chunk.accessProc ? ((it)->chunk.current.access.index--, 0) \
        : (COL_CHAR_PREVIOUS((it)->chunk.current.direct.format, (it)->chunk.current.direct.address), 0)))

/**
 * Test whether iterator reached end of rope.
 *
 * @param it    The #Col_RopeIterator to test.
 *
 * @warning
 *      Argument **it** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 * @retval zero         if iterator if not at end.
 * @retval non-zero     if iterator is at end.
 *
 * @see Col_RopeIterBegin
 */
#define Col_RopeIterEnd(it) \
    ((it)->index >= (it)->length)

/**
 * Initialize an iterator with another one's value.
 *
 * @param it        The #Col_RopeIterator to initialize.
 * @param value     The #Col_RopeIterator to copy.
 */
#define Col_RopeIterSet(it, value) \
    (*(it) = *(value))

/*
 * Remaining declarations.
 */

EXTERN void             Col_RopeIterBegin(Col_RopeIterator it, Col_Word rope,
                            size_t index);
EXTERN void             Col_RopeIterFirst(Col_RopeIterator it, Col_Word rope);
EXTERN void             Col_RopeIterLast(Col_RopeIterator it, Col_Word rope);
EXTERN void             Col_RopeIterString(Col_RopeIterator it,
                            Col_StringFormat format, const void *data,
                            size_t length);
EXTERN int              Col_RopeIterCompare(const Col_RopeIterator it1,
                            const Col_RopeIterator it2);
EXTERN void             Col_RopeIterMoveTo(Col_RopeIterator it, size_t index);
EXTERN void             Col_RopeIterForward(Col_RopeIterator it, size_t nb);
EXTERN void             Col_RopeIterBackward(Col_RopeIterator it, size_t nb);

/** @beginprivate @cond PRIVATE */

EXTERN Col_Char         ColRopeIterUpdateTraversalInfo(ColRopeIterator *it);

/** @endcond @endprivate */

/* End of Rope Iteration *//*!\}*/

/* End of Ropes *//*!\}*/


/*
===========================================================================*//*!
\defgroup customrope_words Custom Ropes
\ingroup rope_words custom_words

Custom ropes are @ref custom_words implementing @ref rope_words with
applicative code.
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Custom Rope Type Descriptors
 ***************************************************************************\{*/

/**
 * Function signature of custom rope length procs.
 *
 * @param rope  Custom rope to get length for.
 *
 * @return The custom rope length.
 *
 * @see Col_CustomRopeType
 * @see Col_RopeLength
 */
typedef size_t (Col_RopeLengthProc) (Col_Word rope);

/**
 * Function signature of custom rope character access procs.
 *
 * @param rope      Custom rope to get character from.
 * @param index     Character index.
 *
 * @note
 *      By construction, **index** is guaranteed to be within valid range, so
 *      implementations need not bother with validation.
 *
 * @return The Unicode codepoint of the character at the given index.
 *
 * @see Col_CustomRopeType
 * @see Col_RopeAt
 */
typedef Col_Char (Col_RopeCharAtProc) (Col_Word rope, size_t index);

/**
 * Function signature of custom rope chunk access procs.
 *
 * @param rope              Custom rope to get chunk from.
 * @param index             Index of character to get chunk for.
 *
 * @param[out] chunkPtr     Chunk information (see #Col_RopeChunk).
 * @param[out] firstPtr,
 *             lastPtr      Chunk range of validity.
 *
 * @note
 *      By construction, **index** is guaranteed to be within valid range, so
 *      implementations need not bother with validation.
 *
 * @see Col_CustomRopeType
 */
typedef void (Col_RopeChunkAtProc) (Col_Word rope, size_t index,
    Col_RopeChunk *chunkPtr, size_t *firstPtr, size_t *lastPtr);

/**
 * Function signature of custom rope subrope extraction.
 *
 * @param rope          Custom rope to extract subrope from.
 * @param first, last   Range of subrope to extract (inclusive).
 *
 * @note
 *      By construction, **first** and **last** are guaranteed to be within
 *      valid range, so implementations need not bother with validation.
 *
 * @retval nil      to use the generic representation.
 * @retval rope     representing the subrope otherwise.
 *
 * @see Col_CustomRopeType
 * @see Col_Subrope
 */
typedef Col_Word (Col_RopeSubropeProc) (Col_Word rope, size_t first,
    size_t last);

/**
 * Function signature of custom rope concatenation.
 *
 * @param left, right   Ropes to concatenate.
 *
 * @retval nil          to use the generic representation.
 * @retval rope         representing the concatenation of both ropes otherwise.
 *
 * @see Col_CustomRopeType
 * @see Col_ConcatRopes
 */
typedef Col_Word (Col_RopeConcatProc) (Col_Word left, Col_Word right);

/**
 * Custom rope type descriptor. Inherits from #Col_CustomWordType.
 */
typedef struct Col_CustomRopeType {
    /*! Generic word type descriptor. Type field must be equal to #COL_ROPE. */
    Col_CustomWordType type;

    /*! Called to get the length of the rope. Must be constant during the whole
        lifetime. */
    Col_RopeLengthProc *lengthProc;

    /*! Called to get the character at a given position. Must be constant
        during the whole lifetime. */
    Col_RopeCharAtProc *charAtProc;

    /*! Called during traversal. If NULL, traversal is done per character
        using #charAtProc. */
    Col_RopeChunkAtProc *chunkAtProc;

    /*! Called to extract subrope. If NULL, or if it returns nil, use the
        standard procedure. */
    Col_RopeSubropeProc *subropeProc;

    /*! Called to concat ropes. If NULL, or if it returns nil, use the standard
        procedure. */
    Col_RopeConcatProc *concatProc;
} Col_CustomRopeType;

/* End of Custom Rope Type Descriptors *//*!\}*/

/* End of Custom Ropes *//*!\}*/

#endif /* _COLIBRI_ROPE */
