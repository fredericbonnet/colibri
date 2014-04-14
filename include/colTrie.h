/*                                                                              *//*!   @file \
 * colTrie.h
 *
 *  This header file defines the trie map handling features of Colibri.
 *
 *  Trie maps are an implementation of generic @ref map_words that use crit-bit
 *  trees for string, integer and custom keys.
 *
 *  They are always mutable.
 *
 *  @see colMap.h
 */

#ifndef _COLIBRI_TRIE
#define _COLIBRI_TRIE


/*
================================================================================*//*!   @addtogroup triemap_words \
Trie Maps
                                                                                        @ingroup words map_words
  Trie maps are an implementation of generic @ref map_words that use crit-bit
  trees for string, integer and custom keys.

  They are always mutable.                                                      *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Trie Map Creation                                                            *//*!   @{ *//*
 ******************************************************************************/

EXTERN Col_Word         Col_NewStringTrieMap();
EXTERN Col_Word         Col_NewIntTrieMap();
EXTERN Col_Word         Col_CopyTrieMap(Col_Word map);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Trie Map Accessors                                                           *//*!   @{ *//*
 ******************************************************************************/

EXTERN int              Col_TrieMapGet(Col_Word map, Col_Word key,
                            Col_Word *valuePtr);
EXTERN int              Col_IntTrieMapGet(Col_Word map, intptr_t key,
                            Col_Word *valuePtr);
EXTERN int              Col_TrieMapSet(Col_Word map, Col_Word key,
                            Col_Word value);
EXTERN int              Col_IntTrieMapSet(Col_Word map, intptr_t key,
                            Col_Word value);
EXTERN int              Col_TrieMapUnset(Col_Word map, Col_Word key);
EXTERN int              Col_IntTrieMapUnset(Col_Word map, intptr_t key);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Trie Map Iteration                                                           *//*!   @{ *//*
 ******************************************************************************/

EXTERN void             Col_TrieMapIterFirst(Col_MapIterator it, Col_Word map);
EXTERN void             Col_TrieMapIterLast(Col_MapIterator it, Col_Word map);
EXTERN void             Col_TrieMapIterFind(Col_MapIterator it, Col_Word map,
                            Col_Word key, int *createPtr);
EXTERN void             Col_IntTrieMapIterFind(Col_MapIterator it, Col_Word map,
                            intptr_t key, int *createPtr);
EXTERN void             Col_TrieMapIterSetValue(Col_MapIterator it,
                            Col_Word value);
EXTERN void             Col_TrieMapIterNext(Col_MapIterator it);
EXTERN void             Col_TrieMapIterPrevious(Col_MapIterator it);
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup customtriemap_words \
Custom Trie Maps
                                                                                        @ingroup triemap_words custom_words
  Custom trie maps are @ref custom_words implementing @ref triemap_words with
  applicative code.                                                             *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Custom Trie Map Type Descriptors                                             *//*!   @{ *//*
 ******************************************************************************/


/*---------------------------------------------------------------------------
 * Col_TrieBitTestProc
 *                                                                              *//*!
 *  Function signature of custom trie map's key bit test function.
 *
 *  @param map      Custom trie map the key belongs to.
 *  @param key      Key to get bit value for.
 *  @param index    Index of key element.
 *  @param bit      Position of bit to test.
 *
 *  @return
 *      The key bit value.
 *
 *  @see Col_CustomTrieMapType
 *//*-----------------------------------------------------------------------*/

typedef int (Col_TrieBitTestProc) (Col_Word map, Col_Word key, size_t index,
    size_t bit);

/*---------------------------------------------------------------------------
 * Col_TrieKeyDiffProc
 *                                                                              *//*!
 *  Function signature of custom trie map's key comparison function.
 *
 *  @param map              Custom trie map the key belongs to.
 *  @param key1, key2       Keys to compare.
 *
 *  @param [out] diffPtr    If non-NULL and keys are different, index of first
 *                          differing key element.
 *  @param [out] bitPtr     If non-NULL and keys are different, critical bit
 *                          where key elements differ.
 *
 *  @retval negative        if **key1** is less than **key2**.
 *  @retval positive        if **key1** is greater than **key2**.
 *  @retval zero            if both keys are equal.
 *
 *  @see Col_CustomTrieMapType
 *//*-----------------------------------------------------------------------*/

typedef int (Col_TrieKeyDiffProc) (Col_Word map, Col_Word key1, Col_Word key2,
    size_t *diffPtr, size_t *bitPtr);

/*---------------------------------------------------------------------------
 * Col_CustomTrieMapType
 *                                                                              *//*!
 *      Custom trie map type descriptor. Inherits from #Col_CustomWordType.
 *
 *  @see Col_NewCustomTrieMap
 *//*-----------------------------------------------------------------------*/

typedef struct Col_CustomTrieMapType {
    /*! Generic word type descriptor. Type field must be equal to
        #COL_TRIEMAP. */
    Col_CustomWordType type;

    /*! Called to test key bits. */
    Col_TrieBitTestProc *bitTestProc;

    /*! Called to compare keys. */
    Col_TrieKeyDiffProc *keyDiffProc;
} Col_CustomTrieMapType;
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Custom Trie Map Creation                                                     *//*!   @{ *//*
 ******************************************************************************/

EXTERN Col_Word         Col_NewCustomTrieMap(Col_CustomTrieMapType *type,
                            size_t size, void **dataPtr);
                                                                                /*!     @} */
                                                                                /*!     @} */
#endif /* _COLIBRI_TRIE */
