/**
 * @file colList.h
 *
 * This header file defines the list handling features of Colibri.
 *
 * Lists are a linear collection datatype that allows for fast insertion,
 * extraction and composition of other lists. Internally they use
 * self-balanced binary trees, like ropes, except that they use vectors
 * as basic containers instead of character arrays.
 *
 * They come in both immutable and mutable forms.
 */

#ifndef _COLIBRI_LIST
#define _COLIBRI_LIST

#include <stddef.h> /* For size_t */
#include <stdarg.h> /* For variadic procs */
#include <string.h> /* for memset */


/*
===========================================================================*//*!
\defgroup list_words Immutable Lists
\ingroup words

Immutable lists are constant, arbitrary-sized, linear collections of words.

Immutable lists can be composed of immutable vectors and lists. Immutable
vectors can themselves be used in place of immutable lists.

@warning
    Mutable vectors, when used in place of immutable lists, may be potentially
    frozen in the process. To avoid that, they should be properly duplicated
    to an immutable vector beforehand.
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Immutable List Creation
 ***************************************************************************\{*/

EXTERN Col_Word         Col_EmptyList();
EXTERN Col_Word         Col_NewList(size_t length,
                            const Col_Word * elements);

/* End of Immutable List Creation *//*!\}*/


/***************************************************************************//*!
 * \name Immutable List Accessors
 *
 * @note
 *      Works with mutable or immutable lists and vectors.
 ***************************************************************************\{*/

EXTERN size_t           Col_ListLength(Col_Word list);
EXTERN size_t           Col_ListLoopLength(Col_Word list);
EXTERN unsigned char    Col_ListDepth(Col_Word list);
EXTERN Col_Word         Col_ListAt(Col_Word list, size_t index);

/* End of Immutable List Accessors *//*!\}*/


/***************************************************************************//*!
 * \name Immutable List Operations
 *
 * @warning
 *      Works with mutable or immutable lists and vectors, however mutable
 *      words may be frozen in the process.
 ***************************************************************************\{*/

/**
 * Variadic macro version of Col_ConcatListsNV() that deduces its number
 * of arguments automatically.
 *
 * @param first First list to concatenate.
 * @param ...   Next lists to concatenate.
 *
 * @see COL_ARGCOUNT
 */
#define Col_ConcatListsV(first, ...) \
    _Col_ConcatListsV(_, first, ##__VA_ARGS__)
/*! \cond IGNORE */
#define _Col_ConcatListsV(_, ...) \
    Col_ConcatListsNV(COL_ARGCOUNT(__VA_ARGS__),__VA_ARGS__)
/*! \endcond *//* IGNORE */

/*
 * Remaining declarations.
 */

EXTERN Col_Word         Col_Sublist(Col_Word list, size_t first, size_t last);
EXTERN Col_Word         Col_ConcatLists(Col_Word left, Col_Word right);
EXTERN Col_Word         Col_ConcatListsA(size_t number, const Col_Word * words);
EXTERN Col_Word         Col_ConcatListsNV(size_t number, ...);
EXTERN Col_Word         Col_RepeatList(Col_Word list, size_t count);
EXTERN Col_Word         Col_CircularList(Col_Word core);
/*EXTERN Col_Word               Col_SortList(Col_Word list, Col_SortCompareProc *proc,
                            Col_ClientData clientData);
EXTERN Col_Word         Col_MergeLists(Col_Word list1, Col_Word list2,
                            Col_SortCompareProc *proc,
                            Col_ClientData clientData);*/
EXTERN Col_Word         Col_ListInsert(Col_Word into, size_t index,
                            Col_Word list);
EXTERN Col_Word         Col_ListRemove(Col_Word list, size_t first,
                            size_t last);
EXTERN Col_Word         Col_ListReplace(Col_Word list, size_t first,
                            size_t last, Col_Word with);

/* End of Immutable List Operations *//*!\}*/


/***************************************************************************//*!
 * \name Immutable List Traversal
 *
 * @note
 *      Works with mutable or immutable lists and vectors.
 ***************************************************************************\{*/

/**
 * Function signature of list traversal procs.
 *
 * @param index         List-relative index where chunks begin.
 * @param length        Length of chunks.
 * @param number        Number of chunks.
 * @param chunks        Array of chunks. When chunk is NULL, means the index is
 *                      past the end of the traversed list. When chunk is
 *                      #COL_LISTCHUNK_VOID, means the traversed list is a void
 *                      list.
 * @param clientData    Opaque client data. Same value as passed to
 *                      Col_TraverseListChunks() procedure family.
 *
 * @retval zero         to continue traversal.
 * @retval non-zero     to stop traversal. Value is returned as result of
 *                      Col_TraverseListChunks() and related procs.
 */
typedef int (Col_ListChunksTraverseProc) (size_t index, size_t length,
        size_t number, const Col_Word **chunks, Col_ClientData clientData);

/**
 * Value passed as chunk pointer to list traversal procs when traversing
 * void lists.
 *
 * @see Col_ListChunksTraverseProc
 */
#define COL_LISTCHUNK_VOID \
    ((Col_Word *)-1)

/*
 * Remaining declarations.
 */

EXTERN int              Col_TraverseListChunksN(size_t number, Col_Word *lists,
                            size_t start, size_t max,
                            Col_ListChunksTraverseProc *proc,
                            Col_ClientData clientData, size_t *lengthPtr);
EXTERN int              Col_TraverseListChunks(Col_Word list, size_t start,
                            size_t max, int reverse,
                            Col_ListChunksTraverseProc *proc,
                            Col_ClientData clientData, size_t *lengthPtr);

/* End of Immutable List Traversal *//*!\}*/


/***************************************************************************//*!
 * \name Immutable List Iteration
 *
 * @warning
 *      Works with mutable or immutable lists and vectors, however modifying a
 *      mutable list during iteration results in undefined behavior.
 ***************************************************************************\{*/

/** @beginprivate @cond PRIVATE */

/**
 * Helper for list iterators to access elements in leaves.
 *
 * @param leaf      Leaf node.
 * @param index     Leaf-relative index of element.
 *
 * @return Element at given index.
 *
 * @see ColListIterator
 */
typedef Col_Word (ColListIterLeafAtProc) (Col_Word leaf, size_t index);

/**
 * Internal implementation of list iterators.
 *
 * @see Col_ListIterator
 */
typedef struct ColListIterator {
    Col_Word list;  /*!< List being iterated. If nil, use array iterator
                         mode. */
    size_t length;  /*!< List length. */
    size_t index;   /*!< Current position. */

    /*! Current chunk info. */
    struct {
        size_t first;   /*!< Begining of range of validity for current chunk. */
        size_t last;    /*!< End of range of validity for current chunk. */

        /*! If non-NULL, element accessor. Else, use direct address mode. */
        ColListIterLeafAtProc *accessProc;

        /*! Current element information. */
        union {
            /*! Address of current element in direct access mode. */
            const Col_Word *direct;

            /*! Current element information in accessor mode. */
            struct {
                Col_Word leaf;  /*!< First argument passed to **accessProc**. */
                size_t index;   /*!< Second argument passed to
                                     **accessProc**. */
            } access;
        } current;
    } chunk;
} ColListIterator;

/** @endcond @endprivate */

/**
 * List iterator. Encapsulates the necessary info to iterate & access list
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
typedef ColListIterator Col_ListIterator[1];

/**
 * Static initializer for null list iterators.
 *
 * @see Col_ListIterator
 * @see Col_ListIterNull
 * @hideinitializer
 */
#define COL_LISTITER_NULL       {{WORD_NIL,0,0,{0,0,NULL,0,NULL}}}

/**
 * Test whether iterator is null (e.g.\ it has been set to #COL_LISTITER_NULL
 * or Col_ListIterSetNull()).
 *
 * @warning
 *      This uninitialized state renders it unusable for any call. Use with
 *      caution.
 *
 * @param it    The #Col_ListIterator to test.
 *
 * @warning
 *      Argument **it** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 * @retval zero         if iterator if not null.
 * @retval non-zero     if iterator is null.
 *
 * @see Col_ListIterator
 * @see COL_LISTITER_NULL
 * @see Col_ListIterSetNull
 * @hideinitializer
 */
#define Col_ListIterNull(it) \
    ((it)->list == WORD_NIL && (it)->chunk.current.direct == NULL)

/**
 * Set an iterator to null.
 *
 * @param it    The #Col_ListIterator to initialize.
 *
 * @warning
 *      Argument **it** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 */
#define Col_ListIterSetNull(it) \
    memset((it), 0, sizeof(*(it)))

/**
 * Get list for iterator.
 *
 * @param it            The #Col_ListIterator to access.
 *
 * @retval WORD_NIL     if iterating over array (see Col_ListIterArray()).
 * @retval list         if iterating over list.
 *
 * @valuecheck{COL_ERROR_LISTITER_END,it}
 */
#define Col_ListIterList(it) \
    ((it)->list)

/**
 * Get length of the iterated sequence.
 *
 * @param it    The #Col_ListIterator to access.
 *
 * @return Length of iterated sequence.
 */
#define Col_ListIterLength(it) \
    ((it)->length)

/**
 * Get current index within list for iterator.
 *
 * @param it    The #Col_ListIterator to access.
 *
 * @return Current index.
 *
 * @valuecheck{COL_ERROR_LISTITER_END,it}
 */
#define Col_ListIterIndex(it) \
    ((it)->index)

/**
 * Get current list element for iterator.
 *
 * @param it    The #Col_ListIterator to access.
 *
 * @warning
 *      Argument **it** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 * @return Current element.
 *
 * @valuecheck{COL_ERROR_LISTITER_END,it}
 * @hideinitializer
 */
#define Col_ListIterAt(it) \
    (  ((it)->index < (it)->chunk.first || (it)->index > (it)->chunk.last) ? ColListIterUpdateTraversalInfo((ColListIterator *)(it)) \
     : (it)->chunk.accessProc ? (it)->chunk.accessProc((it)->chunk.current.access.leaf, (it)->chunk.current.access.index) \
     : (it)->chunk.current.direct ? *((it)->chunk.current.direct) : COL_NIL)

/**
 * Move the iterator to the next element.
 *
 * @param it    The #Col_ListIterator to move.
 *
 * @warning
 *      Argument **it** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 * @retval non-zero     if the iterator looped over the cyclic list.
 * @retval zero         in all other cases.
 *
 * @see Col_ListIterForward
 * @hideinitializer
 */
#define Col_ListIterNext(it) \
    (  ((it)->index < (it)->chunk.first || (it)->index >= (it)->chunk.last) ? Col_ListIterForward((it), 1) \
     : ((it)->index++, \
          (it)->chunk.accessProc ? ((it)->chunk.current.access.index++, 0) \
        : (it)->chunk.current.direct ? ((it)->chunk.current.direct++, 0) : 0))

/**
 * Move the iterator to the previous element.
 *
 * @param it    The #Col_ListIterator to move.
 *
 * @warning
 *      Argument **it** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 * @see Col_ListIterBackward
 * @hideinitializer
 */
#define Col_ListIterPrevious(it) \
    (  ((it)->index <= (it)->chunk.first || (it)->index > (it)->chunk.last) ? (Col_ListIterBackward((it), 1), 0) \
     : ((it)->index--, \
          (it)->chunk.accessProc ? ((it)->chunk.current.access.index--, 0) \
        : (it)->chunk.current.direct ? ((it)->chunk.current.direct--, 0) : 0))

/**
 * Test whether iterator reached end of list.
 *
 * @param it    The #Col_ListIterator to test.
 *
 * @warning
 *      Argument **it** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 * @retval zero         if iterator if not at end.
 * @retval non-zero     if iterator is at end.
 *
 * @see Col_ListIterBegin
 */
#define Col_ListIterEnd(it) \
    ((it)->index >= (it)->length)

/**
 * Initialize an iterator with another one's value.
 *
 * @param it        The #Col_ListIterator to initialize.
 * @param value     The #Col_ListIterator to copy.
 */
#define Col_ListIterSet(it, value) \
    (*(it) = *(value))

/*
 * Remaining declarations.
 */

EXTERN int              Col_ListIterBegin(Col_ListIterator it, Col_Word list,
                            size_t index);
EXTERN void             Col_ListIterFirst(Col_ListIterator it, Col_Word list);
EXTERN void             Col_ListIterLast(Col_ListIterator it, Col_Word list);
EXTERN void             Col_ListIterArray(Col_ListIterator it, size_t length,
                            const Col_Word *elements);
EXTERN int              Col_ListIterCompare(const Col_ListIterator it1,
                            const Col_ListIterator it2);
EXTERN int              Col_ListIterMoveTo(Col_ListIterator it, size_t index);
EXTERN int              Col_ListIterForward(Col_ListIterator it, size_t nb);
EXTERN void             Col_ListIterBackward(Col_ListIterator it, size_t nb);

/** @beginprivate @cond PRIVATE */

EXTERN Col_Word         ColListIterUpdateTraversalInfo(ColListIterator *it);

/** @endcond @endprivate */

/* End of Immutable List Iteration *//*!\}*/

/* End of Immutable Lists *//*!\}*/


/*
===========================================================================*//*!
\defgroup customlist_words Custom Lists
\ingroup list_words custom_words

Custom lists are @ref custom_words implementing @ref list_words with
applicative code.
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Custom List Type Descriptors
 ***************************************************************************\{*/

/**
 * Function signature of custom list length procs.
 *
 * @param list  Custom list to get length for.
 *
 * @return The custom list length.
 *
 * @see Col_CustomListType
 * @see Col_ListLength
 */
typedef size_t (Col_ListLengthProc) (Col_Word list);

/**
 * Function signature of custom list element access procs.
 *
 * @param list      Custom list to get element from.
 * @param index     Element index.
 *
 * @note
 *      By construction, **index** is guaranteed to be within valid range, so
 *      implementations need not bother with validation.
 *
 * @return The element at the given index.
 *
 * @see Col_CustomListType
 * @see Col_ListAt
 */
typedef Col_Word (Col_ListElementAtProc) (Col_Word list, size_t index);

/**
 * Function signature of custom list chunk access procs.
 *
 * @param list              Custom list to get chunk from.
 * @param index             Index of element to get chunk for.
 *
 * @param[out] chunkPtr     Chunk. If #COL_LISTCHUNK_VOID, the chunk is void
 *                          (i.e. all its elements are nil).
 * @param[out] firstPtr,
 *              lastPtr     Chunk range of validity.
 *
 * @note
 *      By construction, **index** is guaranteed to be within valid range, so
 *      implementations need not bother with validation.
 *
 * @see Col_CustomListType
 */
typedef void (Col_ListChunkAtProc) (Col_Word list, size_t index,
    const Col_Word **chunkPtr, size_t *firstPtr, size_t *lastPtr);

/**
 * Function signature of custom list sublist extraction.
 *
 * @param list          Custom list to extract sublist from.
 * @param first, last   Range of sublist to extract (inclusive).
 *
 * @note
 *      By construction, **first** and **last** are guaranteed to be within
 *      valid range, so implementations need not bother with validation.
 *
 * @retval nil      to use the generic representation.
 * @retval list     representing the sublist otherwise.
 *
 * @see Col_CustomListType
 * @see Col_Sublist
 */
typedef Col_Word (Col_ListSublistProc) (Col_Word list, size_t first,
    size_t last);

/**
 * Function signature of custom list concatenation.
 *
 * @param left, right   Lists to concatenate.
 *
 * @retval nil          to use the generic representation.
 * @retval rope         representing the concatenation of both lists otherwise.
 *
 * @see Col_CustomListType
 * @see Col_ConcatLists
 */
typedef Col_Word (Col_ListConcatProc) (Col_Word left, Col_Word right);

/**
 * Custom list type descriptor. Inherits from #Col_CustomWordType.
 */
typedef struct Col_CustomListType {
    /*! Generic word type descriptor. Type field must be equal to #COL_LIST. */
    Col_CustomWordType type;

    /*! Called to get the length of the list. Must be constant during the whole
        lifetime. */
    Col_ListLengthProc *lengthProc;

    /*! Called to get the element at a given position. Must be constant
        during the whole lifetime. */
    Col_ListElementAtProc *elementAtProc;

    /*! Called during traversal. If NULL, traversal is done per character
        using #elementAtProc. */
    Col_ListChunkAtProc *chunkAtProc;

    /*! Called to extract sublist. If NULL, or if it returns nil, use the
        standard procedure. */
    Col_ListSublistProc *sublistProc;

    /*! Called to concat lists. If NULL, or if it returns nil, use the standard
        procedure. */
    Col_ListConcatProc *concatProc;
} Col_CustomListType;

/* End of Custom List Type Descriptors *//*!\}*/

/* End of Custom Lists *//*!\}*/


/*
===========================================================================*//*!
\defgroup mlist_words Mutable Lists
\ingroup words

Mutable lists are linear collection of words whose content and length can
vary over time.

Mutable lists can be composed of either mutable or immutable lists and
vectors. They can be "frozen" and turned into immutable versions.

@warning
    Mutable vectors *cannot* be used in place of mutable lists, as the latter
    can grow indefinitely whereas the former have a maximum length set at
    creation time.
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Mutable List Creation
 ***************************************************************************\{*/

EXTERN Col_Word         Col_NewMList();
EXTERN Col_Word         Col_CopyMList(Col_Word mlist);

/* End of Mutable List Creation *//*!\}*/


/***************************************************************************//*!
 * \name Mutable List Operations
 ***************************************************************************\{*/

EXTERN void             Col_MListSetLength(Col_Word mlist, size_t length);
EXTERN void             Col_MListLoop(Col_Word mlist);
EXTERN void             Col_MListSetAt(Col_Word mlist, size_t index,
                            Col_Word element);
/*EXTERN void           Col_MListSort(Col_Word mlist, size_t first, size_t last,
                            Col_SortCompareProc *proc,
                            Col_ClientData clientData);
EXTERN void             Col_MListMerge(Col_Word into, Col_Word list,
                            Col_SortCompareProc *proc,
                            Col_ClientData clientData);*/
EXTERN void             Col_MListInsert(Col_Word into, size_t index,
                            Col_Word list);
EXTERN void             Col_MListRemove(Col_Word mlist, size_t first,
                            size_t last);
EXTERN void             Col_MListReplace(Col_Word mlist, size_t first,
                            size_t last, Col_Word with);

/* End of Mutable List Operations *//*!\}*/

/* End of Mutable Lists *//*!\}*/

#endif /* _COLIBRI_LIST */
