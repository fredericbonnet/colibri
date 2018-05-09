/*                                                                              *//*!   @file \
 * colMap.h
 *
 *  This header file defines the generic map handling features of Colibri.
 *
 *  Maps are an associative collection datatype that associates keys to
 *  values. Keys can be integers, strings or generic words. Values are
 *  arbitrary words.
 *
 *  They are always mutable.
 */

#ifndef _COLIBRI_MAP
#define _COLIBRI_MAP

#include <stddef.h> /* For size_t */


/*
================================================================================*//*!   @addtogroup map_words \
Maps
                                                                                        @ingroup words
  Maps are mutable associative arrays whose keys can be integers, strings or
  generic words.

  Maps can be implemented in several ways, each with distinct features, so here
  we define the generic interface common to all implementations.                *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Map Accessors                                                                *//*!   @{ *//*
 ******************************************************************************/

EXTERN size_t           Col_MapSize(Col_Word map);
EXTERN int              Col_MapGet(Col_Word map, Col_Word key,
                            Col_Word *valuePtr);
EXTERN int              Col_IntMapGet(Col_Word map, intptr_t key,
                            Col_Word *valuePtr);
EXTERN int              Col_MapSet(Col_Word map, Col_Word key, Col_Word value);
EXTERN int              Col_IntMapSet(Col_Word map, intptr_t key, Col_Word value);
EXTERN int              Col_MapUnset(Col_Word map, Col_Word key);
EXTERN int              Col_IntMapUnset(Col_Word map, intptr_t key);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Map Iteration                                                                *//*!   @{ *//*
 ******************************************************************************/
                                                                                /*!     @cond PRIVATE */
/*---------------------------------------------------------------------------
 * ColMapIterator
 *                                                                              *//*!
 *  Internal implementation of map iterators.
 *
 *  @see Col_MapIterator
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

typedef struct ColMapIterator {
    Col_Word map;   /*!< Map being iterated. */
    Col_Word entry; /*!< Current entry. */

    /*! Implementation-specific traversal info. */
    union {
        /*! Hash-specific fields. */
        struct {
            size_t bucket;  /*!< Current bucket index. */
        } hash;

        /*! Trie-specific fields. */
        struct {
            Col_Word prev;  /*!< Subtrie whose rightmost leaf is the previous
                                 entry. */
            Col_Word next;  /*!< Subtrie whose leftmost leaf is the next
                                  entry. */
        } trie;

        /*! Custom-specific fields. */
        Col_ClientData custom[2];   /*!< Opaque client data. A pair of values
                                         is available to custom map types for
                                         iteration purpose. */
    } traversal;
} ColMapIterator;
                                                                                /*!     @endcond */
/*---------------------------------------------------------------------------
 * Col_MapIterator
 *                                                                              *//*!
 *  Map iterator. Encapsulates the necessary info to iterate & access map
 *  data transparently.
 *
 *  @note                                                                               @parblock
 *      Datatype is opaque. Fields should not be accessed by client code.
 *
 *      Each iterator takes 4 words on the stack.
 *
 *      The type is defined as a single-element array of the internal datatype:
 *
 *      - declared variables allocate the right amount of space on the stack,
 *      - calls use pass-by-reference (i.e. pointer) and not pass-by-value,
 *      - forbidden as return type.
 *                                                                                      @endparblock
 *//*-----------------------------------------------------------------------*/

typedef ColMapIterator Col_MapIterator[1];

/*---------------------------------------------------------------------------   *//*!   @def \
 * COL_MAPITER_NULL
 *                                                                                      @hideinitializer
 *  Static initializer for null map iterators.
 *
 *  @see Col_MapIterator
 *  @see Col_MapIterNull
 *//*-----------------------------------------------------------------------*/

#define COL_MAPITER_NULL        {{WORD_NIL}}

/*---------------------------------------------------------------------------   *//*!   @def \
 * Col_MapIterNull
 *                                                                                      @hideinitializer
 *  Test whether iterator is null (e.g. it has been set to #COL_MAPITER_NULL
 *  or Col_MapIterSetNull()).
 *
 *  @warning
 *      This uninitialized state renders it unusable for any call. Use with
 *      caution.
 *
 *  @param it           The #Col_MapIterator to test.
 *
 *  @retval zero        if iterator if not null.
 *  @retval non-zero    if iterator is null.
 *
 *  @see Col_MapIterator
 *  @see COL_MAPITER_NULL
 *  @see Col_MapIterSetNull
 *//*-----------------------------------------------------------------------*/

#define Col_MapIterNull(it) \
    ((it)->map == WORD_NIL)

/*---------------------------------------------------------------------------   *//*!   @def \
 * Col_MapIterSetNull
 *
 *  Set an iterator to null.
 *
 *  @param it       The #Col_MapIterator to initialize.
 *
 *  @warning
 *      Argument **it** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *//*-----------------------------------------------------------------------*/

#define Col_MapIterSetNull(it) \
    memset((it), 0, sizeof(*(it)))

/*---------------------------------------------------------------------------   *//*!   @def \
 * Col_MapIterMap
 *
 *  Get map for iterator.
 *
 *  @param it   The #Col_MapIterator to access.
 *
 *  @result
 *      The map.
 *
 *  @valuecheck{COL_ERROR_MAPITER_END,it}
 *//*-----------------------------------------------------------------------*/

#define Col_MapIterMap(it) \
    ((it)->map)

/*---------------------------------------------------------------------------   *//*!   @def \
 * Col_MapIterEnd
 *
 *  Test whether iterator reached end of map.
 *
 *  @param it           The #Col_MapIterator to test.
 *
 *  @retval zero        if iterator if not at end.
 *  @retval non-zero    if iterator is at end.
 *
 *  @see Col_MapIterBegin
 *//*-----------------------------------------------------------------------*/

#define Col_MapIterEnd(it) \
    (!(it)->entry)

/*---------------------------------------------------------------------------   *//*!   @def \
 * Col_MapIterSet
 *
 *  Initialize an iterator with another one's value.
 *
 *  @param it       The #Col_MapIterator to initialize.
 *  @param value    The #Col_MapIterator to copy.
 *//*-----------------------------------------------------------------------*/

#define Col_MapIterSet(it, value) \
    (*(it) = *(value))

/*
 * Remaining declarations.
 */

EXTERN void             Col_MapIterBegin(Col_MapIterator it, Col_Word map);
EXTERN void             Col_MapIterFind(Col_MapIterator it, Col_Word map,
                            Col_Word key, int *createPtr);
EXTERN void             Col_IntMapIterFind(Col_MapIterator it, Col_Word map,
                            intptr_t key, int *createPtr);
EXTERN void             Col_MapIterGet(Col_MapIterator it, Col_Word *keyPtr,
                            Col_Word *valuePtr);
EXTERN void             Col_IntMapIterGet(Col_MapIterator it, intptr_t *keyPtr,
                            Col_Word *valuePtr);
EXTERN Col_Word         Col_MapIterGetKey(Col_MapIterator it);
EXTERN intptr_t         Col_IntMapIterGetKey(Col_MapIterator it);
EXTERN Col_Word         Col_MapIterGetValue(Col_MapIterator it);
EXTERN void             Col_MapIterSetValue(Col_MapIterator it,
                            Col_Word value);
EXTERN void             Col_MapIterNext(Col_MapIterator it);
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup custommap_words \
Custom Maps
                                                                                        @ingroup map_words custom_words
  Custom maps are @ref custom_words implementing @ref map_words with
  applicative code.                                                             *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Custom Map Type Descriptors                                                  *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * Col_MapSizeProc
 *                                                                              *//*!
 *  Function signature of custom map size procs.
 *
 *  @param map      Custom map to get size for.
 *
 *  @return
 *      The custom map size.
 *
 *  @see Col_CustomMapType
 *  @see Col_CustomIntMapType
 *  @see Col_MapSize
 *//*-----------------------------------------------------------------------*/

typedef size_t (Col_MapSizeProc) (Col_Word map);

/*---------------------------------------------------------------------------
 * Col_MapGetProc
 *                                                                              *//*!
 *  Function signature of custom map get procs.
 *
 *  @param map              Custom map to get entry for.
 *  @param key              Entry key. Can be any word type, including string,
 *                          however it must match the actual type used by the
 *                          map.
 *
 *  @param[out] valuePtr    Returned entry value, if found.
 *
 *  @retval zero            if the key wasn't found.
 *  @retval non-zero        if the key was found, in this case the value is
 *                          returned through **valuePtr**.
 *
 *  @see Col_CustomMapType
 *  @see Col_MapGet
 *//*-----------------------------------------------------------------------*/

typedef int (Col_MapGetProc)(Col_Word map, Col_Word key, Col_Word *valuePtr);

/*---------------------------------------------------------------------------
 * Col_IntMapGetProc
 *                                                                              *//*!
 *  Function signature of custom integer map get procs.
 *
 *  @param map              Custom integer map to get entry for.
 *  @param key              Integer entry key.
 *
 *  @param[out] valuePtr    Returned entry value, if found.
 *
 *  @retval zero            if the key wasn't found.
 *  @retval non-zero        if the key was found, in this case the value is
 *                          returned through **valuePtr**.
 *
 *  @see Col_CustomIntMapType
 *  @see Col_IntMapGet
 *//*-----------------------------------------------------------------------*/

typedef int (Col_IntMapGetProc)(Col_Word map, intptr_t key, Col_Word *valuePtr);

/*---------------------------------------------------------------------------
 * Col_MapSetProc
 *                                                                              *//*!
 *  Function signature of custom map set procs.
 *
 *  @param map          Custom map to insert entry into.
 *  @param key          Entry key. Can be any word type, including string, 
 *                      however it must match the actual type used by the map.
 *  @param value        Entry value.
 *
 *  @retval zero        if an existing entry was updated with **value**.
 *  @retval non-zero    if a new entry was created with **key** and **value**.
 *
 *  @see Col_CustomMapType
 *  @see Col_MapSet
 *//*-----------------------------------------------------------------------*/

typedef int (Col_MapSetProc)(Col_Word map, Col_Word key, Col_Word value);

/*---------------------------------------------------------------------------
 * Col_IntMapSetProc
 *                                                                              *//*!
 *  Function signature of custom integer map set procs.
 *
 *  @param map          Custom integer map to insert entry into.
 *  @param key          Integer entry key.
 *  @param value        Entry value.
 *
 *  @retval zero        if the existing entry was updated with **value**.
 *  @retval non-zero    if a new entry was created with **key** and **value**.
 *
 *  @see Col_CustomIntMapType
 *  @see Col_IntMapSet
 *//*-----------------------------------------------------------------------*/

typedef int (Col_IntMapSetProc)(Col_Word map, intptr_t key, Col_Word value);

/*---------------------------------------------------------------------------
 * Col_MapUnsetProc
 *                                                                              *//*!
 *  Function signature of custom map unset procs.
 *
 *  @param map          Custom map to remove entry from.
 *  @param key          Entry key. Can be any word type, including string, 
 *                      however it must match the actual type used by the map.
 *
 *  @retval zero        if no entry matching **key** was found.
 *  @retval non-zero    if the existing entry was removed.
 *
 *  @see Col_CustomMapType
 *  @see Col_MapUnset
 *//*-----------------------------------------------------------------------*/

typedef int (Col_MapUnsetProc)(Col_Word map, Col_Word key);

/*---------------------------------------------------------------------------
 * Col_IntMapUnsetProc
 *                                                                              *//*!
 *  Function signature of custom integer map unset procs.
 *
 *  @param map          Custom integer map to remove entry from.
 *  @param key          Integer entry key.
 *
 *  @retval zero        if no entry matching **key** was found.
 *  @retval non-zero    if the existing entry was removed.
 *
 *  @see Col_CustomIntMapType
 *  @see Col_IntMapUnset
 *//*-----------------------------------------------------------------------*/

typedef int (Col_IntMapUnsetProc)(Col_Word map, intptr_t key);

/*---------------------------------------------------------------------------
 * Col_MapIterBeginProc
 *                                                                              *//*!
 *  Function signature of custom map iter begin procs.
 *
 *  @param map          Custom map to begin iteration for.
 *  @param clientData   Pair of opaque values available for iteration purpose.
 *
 *  @retval zero        if at end.
 .  @retval non-zero    if iteration began.
 *
 *  @see Col_CustomMapType
 *  @see Col_MapIterBegin
 *//*-----------------------------------------------------------------------*/

typedef int (Col_MapIterBeginProc)(Col_Word map,
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Col_MapIterFindProc
 *                                                                              *//*!
 *  Function signature of custom map iter find procs.
 *
 *  @param map                  Custom map to find or create entry into.
 *  @param key                  Entry key. Can be any word type, including
 *                              string, however it must match the actual type
 *                              used by the map.
 *  @param[in,out] createPtr    If non-NULL, whether to create entry if absent
 *                              on input, and whether an entry was created on
 *                              output.
 *  @param clientData           Pair of opaque values available for iteration
 *                              purpose.
 *
 *  @retval zero                if at end.
 *  @retval non-zero            if iteration began.
 *
 *  @see Col_CustomMapType
 *  @see Col_MapIterFind
 *//*-----------------------------------------------------------------------*/

typedef int (Col_MapIterFindProc)(Col_Word map, Col_Word key, int *createPtr,
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Col_IntMapIterFindProc
 *                                                                              *//*!
 *  Function signature of custom integer map iter find procs.
 *
 *  @param map                  Custom integer map to find or create entry into.
 *  @param key                  Integer entry key.
 *  @param[in,out] createPtr    If non-NULL, whether to create entry if absent
 *                              on input, and whether an entry was created on
 *                              output.
 *  @param clientData           Pair of opaque values available for iteration
 *                              purpose.
 *
 *  @retval zero                if at end.
 *  @retval non-zero            if iteration began.
 *
 *  @see Col_CustomIntMapType
 *  @see Col_IntMapIterFind
 *//*-----------------------------------------------------------------------*/

typedef int (Col_IntMapIterFindProc)(Col_Word map, intptr_t key, int *createPtr,
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Col_MapIterNextProc
 *                                                                              *//*!
 *  Function signature of custom map iter next procs.
 *
 *  @param map          Custom map to continue iteration for.
 *  @param clientData   Pair of opaque values available for iteration purpose.
 *
 *  @retval zero        if at end.
 *  @retval non-zero    if iteration continued.
 *
 *  @see Col_CustomMapType
 *  @see Col_MapIterNext
 *//*-----------------------------------------------------------------------*/

typedef int (Col_MapIterNextProc)(Col_Word map,
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Col_MapIterGetKeyProc
 *                                                                              *//*!
 *  Function signature of custom map iter key get procs.
 *
 *  @param map          Custom map to get iterator key from.
 *  @param clientData   Pair of opaque values available for iteration purpose.
 *
 *  @return
 *      Entry key.
 *
 *  @see Col_CustomMapType
 *  @see Col_MapIterGet
 *  @see Col_MapIterGetKey
 *//*-----------------------------------------------------------------------*/

typedef Col_Word (Col_MapIterGetKeyProc)(Col_Word map,
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Col_IntMapIterGetKeyProc
 *                                                                              *//*!
 *  Function signature of custom integer map iter key get procs.
 *
 *  @param map          Custom integer map to get iterator key from.
 *  @param clientData   Pair of opaque values available for iteration purpose.
 *
 *  @return
 *      Integer entry key.
 *
 *  @see Col_CustomIntMapType
 *  @see Col_IntMapIterGet
 *  @see Col_IntMapIterGetKey
 *//*-----------------------------------------------------------------------*/

typedef intptr_t (Col_IntMapIterGetKeyProc)(Col_Word map,
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Col_MapIterGetValueProc
 *                                                                              *//*!
 *  Function signature of custom map iter value get procs.
 *
 *  @param map          Custom map to get iterator value from.
 *  @param clientData   Pair of opaque values available for iteration purpose.
 *
 *  @return
 *      Entry value.
 *
 *  @see Col_CustomMapType
 *  @see Col_MapIterGetValue
 *//*-----------------------------------------------------------------------*/

typedef Col_Word (Col_MapIterGetValueProc)(Col_Word map,
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Col_MapIterSetValueProc
 *                                                                              *//*!
 *  Function signature of custom map iter value set procs.
 *
 *  @param map          Custom map to set iterator value from.
 *  @param value        Value to set.
 *  @param clientData   Pair of opaque values available for iteration purpose.
 *
 *  @see Col_CustomMapType
 *  @see Col_MapIterSetValue
 *//*-----------------------------------------------------------------------*/

typedef void (Col_MapIterSetValueProc)(Col_Word map, Col_Word value,
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Col_CustomMapType
 *                                                                              *//*!
 *  Custom map type descriptor. Inherits from #Col_CustomWordType.
 *//*-----------------------------------------------------------------------*/

typedef struct Col_CustomMapType {
    /*! Generic word type descriptor. Type field must be equal to #COL_MAP. */
    Col_CustomWordType type;

    /*! Called to get map size. */
    Col_MapSizeProc *sizeProc;

    /*! Called to get entry value. */
    Col_MapGetProc *getProc;

    /*! Called to set entry value. */
    Col_MapSetProc *setProc;

    /*! Called to unset entry. */
    Col_MapUnsetProc *unsetProc;

    /*! Called to begin iteration. */
    Col_MapIterBeginProc *iterBeginProc;

    /*! Called to begin iteration at given key. */
    Col_MapIterFindProc *iterFindProc;

    /*!Called to continue iteration.  */
    Col_MapIterNextProc *iterNextProc;

    /*! Called to get iterator key. */
    Col_MapIterGetKeyProc *iterGetKeyProc;

    /*! Called to get iterator value. */
    Col_MapIterGetValueProc *iterGetValueProc;

    /*! Called to set iterator value. */
    Col_MapIterSetValueProc *iterSetValueProc;
} Col_CustomMapType;

/*---------------------------------------------------------------------------
 * Col_CustomIntMapType
 *                                                                              *//*!
 *  Custom integer map type descriptor. Inherits from #Col_CustomWordType.
 *//*-----------------------------------------------------------------------*/

typedef struct Col_CustomIntMapType {
    /*! Generic word type descriptor. Type field must be equal to
        #COL_INTMAP. */
    Col_CustomWordType type;

    /*! Called to get map size. */
    Col_MapSizeProc *sizeProc;

    /*! Called to get entry value. */
    Col_IntMapGetProc *getProc;

    /*! Called to set entry value. */
    Col_IntMapSetProc *setProc;

    /*! Called to unset entry. */
    Col_IntMapUnsetProc *unsetProc;

    /*! Called to begin iteration. */
    Col_MapIterBeginProc *iterBeginProc;

    /*! Called to begin iteration at given key. */
    Col_IntMapIterFindProc *iterFindProc;

    /*! Called to continue iteration. */
    Col_MapIterNextProc *iterNextProc;

    /*! Called to get iterator key. */
    Col_IntMapIterGetKeyProc *iterGetKeyProc;

    /*! Called to get iterator value. */
    Col_MapIterGetValueProc *iterGetValueProc;

    /*! Called to set iterator value. */
    Col_MapIterSetValueProc *iterSetValueProc;
} Col_CustomIntMapType;
                                                                                /*!     @} */
                                                                                /*!     @} */
#endif /* _COLIBRI_MAP */
