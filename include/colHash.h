/*                                                                              *//*!   @file \
 * colHash.h
 *
 *  This header file defines the hash map handling features of Colibri.
 *
 *  Hash maps are an implementation of generic @ref map_words that use key
 *  hashing and flat bucket arrays for string, integer and custom keys.
 *
 *  They are always mutable.
 *
 *  @see colMap.h
 */

#ifndef _COLIBRI_HASH
#define _COLIBRI_HASH

#include <stddef.h> /* For size_t */


/*
================================================================================*//*!   @addtogroup hashmap_words \
Hash Maps
                                                                                        @ingroup words map_words
  Hash maps are an implementation of generic @ref map_words that use key
  hashing and flat bucket arrays for string, integer and custom keys.

  They are always mutable.                                                      *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Hash Map Creation                                                            *//*!   @{ *//*
 ******************************************************************************/

EXTERN Col_Word         Col_NewStringHashMap(size_t capacity);
EXTERN Col_Word         Col_NewIntHashMap(size_t capacity);
EXTERN Col_Word         Col_CopyHashMap(Col_Word map);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Hash Map Accessors                                                           *//*!   @{ *//*
 ******************************************************************************/

EXTERN int              Col_HashMapGet(Col_Word map, Col_Word key,
                            Col_Word *valuePtr);
EXTERN int              Col_IntHashMapGet(Col_Word map, intptr_t key,
                            Col_Word *valuePtr);
EXTERN int              Col_HashMapSet(Col_Word map, Col_Word key,
                            Col_Word value);
EXTERN int              Col_IntHashMapSet(Col_Word map, intptr_t key,
                            Col_Word value);
EXTERN int              Col_HashMapUnset(Col_Word map, Col_Word key);
EXTERN int              Col_IntHashMapUnset(Col_Word map, intptr_t key);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Hash Map Iteration                                                           *//*!   @{ *//*
 ******************************************************************************/

EXTERN void             Col_HashMapIterBegin(Col_MapIterator it, Col_Word map);
EXTERN void             Col_HashMapIterFind(Col_MapIterator it, Col_Word map,
                            Col_Word key, int *createPtr);
EXTERN void             Col_IntHashMapIterFind(Col_MapIterator it, Col_Word map,
                            intptr_t key, int *createPtr);
EXTERN void             Col_HashMapIterSetValue(Col_MapIterator it,
                            Col_Word value);
EXTERN void             Col_HashMapIterNext(Col_MapIterator it);
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup customhashmap_words \
Custom Hash Maps
                                                                                        @ingroup hashmap_words custom_words
  Custom hash maps are @ref custom_words implementing @ref hashmap_words with
  applicative code.                                                             *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Custom Hash Map Type Descriptors                                             *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * Col_HashProc
 *                                                                              *//*!
 *  Function signature of custom hash map key hashing function.
 *
 *  @param map      Custom hash map the key belongs to.
 *  @param key      Key to generate hash value for.
 *
 *  @return
 *      The key hash value.
 *
 *  @see Col_CustomHashMapType
 *//*-----------------------------------------------------------------------*/

typedef uintptr_t (Col_HashProc) (Col_Word map, Col_Word key);

/*---------------------------------------------------------------------------
 * Col_HashCompareKeysProc
 *                                                                              *//*!
 *  Function signature of custom hash map key comparison function.
 *
 *  @param map          Custom hash map the keys belong to.
 *  @param key1, key2   Keys to compare.
 *
 *  @retval negative    if **key1** is less than **key2**.
 *  @retval positive    if **key1** is greater than **key2**.
 *  @retval zero        if both keys are equal.
 *
 *  @see Col_CustomHashMapType
 *//*-----------------------------------------------------------------------*/

typedef int (Col_HashCompareKeysProc) (Col_Word map, Col_Word key1,
    Col_Word key2);

/*---------------------------------------------------------------------------
 * Col_CustomHashMapType
 *                                                                              *//*!
 *  Custom hash map type descriptor. Inherits from #Col_CustomWordType.
 *
 *  @see Col_NewCustomHashMap
 *//*-----------------------------------------------------------------------*/

typedef struct Col_CustomHashMapType {
    /*! Generic word type descriptor. Type field must be equal to
        #COL_HASHMAP. */
    Col_CustomWordType type;

    /*! Called to compute the hash value of a key. */
    Col_HashProc *hashProc;

    /*! Called to compare keys. */
    Col_HashCompareKeysProc *compareKeysProc;
} Col_CustomHashMapType;
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Custom Hash Map Creation                                                     *//*!   @{ *//*
 ******************************************************************************/

EXTERN Col_Word         Col_NewCustomHashMap(Col_CustomHashMapType *type,
                            size_t capacity, size_t size, void **dataPtr);
                                                                                /*!     @} */
                                                                                /*!     @} */
#endif /* _COLIBRI_HASH */
