/*                                                                              *//*!   @file \
 * colMap.c
 *
 *  This file implements the generic map handling features of Colibri.
 *
 *  Maps are an associative collection datatype that associates keys to
 *  values. Keys can be integers, strings or generic words. Values are
 *  arbitrary words.
 *
 *  They are always mutable.
 *
 *  @see colMap.h
 */

#include "include/colibri.h"
#include "colInternal.h"

#include "colWordInt.h"
#include "colMapInt.h"
#include "colHashInt.h"
#include "colTrieInt.h"


/*
================================================================================*//*!   @addtogroup map_words \
Maps                                                                            *//*!   @{ *//*
================================================================================
*/

/*******************************************************************************
 * Map Accessors
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * Col_MapSize
 *                                                                              *//*!
 *  Get the size of the map, i.e.\ the number of entries.
 *
 *  @return
 *      The map size.
 *
 *  @return
 *  The map size.
 *//*-----------------------------------------------------------------------*/

size_t
Col_MapSize(
    Col_Word map)   /*!< Map to get size for. */
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAP(map) return 0;                                                /*!     @typecheck{COL_ERROR_MAP,map} */

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
    case WORD_TYPE_INTHASHMAP:
        return WORD_HASHMAP_SIZE(map);

    case WORD_TYPE_STRTRIEMAP:
    case WORD_TYPE_INTTRIEMAP:
        return WORD_TRIEMAP_SIZE(map);

    case WORD_TYPE_CUSTOM:
        switch (WORD_TYPEINFO(map)->type) {
        case COL_HASHMAP: return WORD_HASHMAP_SIZE                                      (map);
        case COL_TRIEMAP: return WORD_TRIEMAP_SIZE                                      (map);
        case COL_MAP:     return ((Col_CustomMapType *)    WORD_TYPEINFO(map))->sizeProc(map);
        case COL_INTMAP:  return ((Col_CustomIntMapType *) WORD_TYPEINFO(map))->sizeProc(map);
        }
        /* continued. */

    /* WORD_TYPE_UNKNOWN */

    default:
        /* CANTHAPPEN */
        ASSERT(0);
        return 0;
    }
}

/*---------------------------------------------------------------------------
 * Col_MapGet
 *                                                                              *//*!
 *  Get value mapped to the given key if present.
 *
 *  Generic interface to map get, calls actual function depending on type.
 *
 *  @retval 0               if the key wasn't found.
 *  @retval <>0             if the key was found, in this case the value is
 *                          returned through **valuePtr**.
 *//*-----------------------------------------------------------------------*/

int
Col_MapGet(
    Col_Word map,       /*!< Map to get entry for. */
    Col_Word key,       /*!< Entry key. Can be any word type, including string,
                             however it must match the actual type used by the
                             map. */

    /*! [out] Returned entry value, if found. */
    Col_Word *valuePtr)
{
    Col_MapGetProc *proc;

    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(map) return 0;                                            /*!     @typecheck{COL_ERROR_WORDMAP,map} */

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
        proc = Col_HashMapGet;
        break;

    case WORD_TYPE_STRTRIEMAP:
        proc = Col_TrieMapGet;
        break;

    case WORD_TYPE_CUSTOM: {
        Col_CustomMapType *type = (Col_CustomMapType *) WORD_TYPEINFO(map);
        switch (type->type.type) {
        case COL_HASHMAP: proc = Col_HashMapGet; break;
        case COL_TRIEMAP: proc = Col_TrieMapGet; break;
        case COL_MAP:     proc = type->getProc; break;
        }
        break;
        }

    /* WORD_TYPE_UNKNOWN */

    default:
        /* CANTHAPPEN */
        ASSERT(0);
        return 0;
    }

    return proc(map, key, valuePtr);
}

/*---------------------------------------------------------------------------
 * Col_IntMapGet
 *                                                                              *//*!
 *  Get value mapped to the given integer key if present.
 *
 *  Generic interface to integer map get, calls actual function depending on
 *  type.
 *
 *  @retval 0               if the key wasn't found.
 *  @retval <>0             if the key was found, in this case the value is
 *                          returned through **valuePtr**.
 *//*-----------------------------------------------------------------------*/

int
Col_IntMapGet(
    Col_Word map,       /*!< Integer map to get entry for. */
    intptr_t key,       /*!< Integer entry key */

    /*! [out] Returned entry value, if found. */
    Col_Word *valuePtr)
{
    Col_IntMapGetProc *proc;

    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(map) return 0;                                             /*!     @typecheck{COL_ERROR_INTMAP,map} */

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_INTHASHMAP:
        proc = Col_IntHashMapGet;
        break;

    case WORD_TYPE_INTTRIEMAP:
        proc = Col_IntTrieMapGet;
        break;

    case WORD_TYPE_CUSTOM:
        ASSERT(WORD_TYPEINFO(map)->type == COL_INTMAP);
        proc = ((Col_CustomIntMapType *) WORD_TYPEINFO(map))->getProc;
        break;

    /* WORD_TYPE_UNKNOWN */

    default:
        /* CANTHAPPEN */
        ASSERT(0);
        return 0;
    }

    return proc(map, key, valuePtr);
}

/*---------------------------------------------------------------------------
 * Col_MapSet
 *                                                                              *//*!
 *  Map the value to the key, replacing any existing.
 *
 *  Generic interface to map set, calls actual function depending on type.
 *
 *  @retval 0       if an existing entry was updated with **value**.
 *  @retval <>0     if a new entry was created with **key** and **value**.
 *//*-----------------------------------------------------------------------*/

int
Col_MapSet(
    Col_Word map,   /*!< Map to insert entry into. */
    Col_Word key,   /*!< Entry key. Can be any word type, including string,
                         however it must match the actual type used by the
                         map. */
    Col_Word value) /*!< Entry value. */
{
    Col_MapSetProc *proc;

    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(map) return 0;                                            /*!     @typecheck{COL_ERROR_WORDMAP,map} */

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
        proc = Col_HashMapSet;
        break;

    case WORD_TYPE_STRTRIEMAP:
        proc = Col_TrieMapSet;
        break;

    case WORD_TYPE_CUSTOM: {
        Col_CustomMapType *type = (Col_CustomMapType *) WORD_TYPEINFO(map);
        switch (type->type.type) {
        case COL_HASHMAP: proc = Col_HashMapSet; break;
        case COL_TRIEMAP: proc = Col_TrieMapSet; break;
        case COL_MAP:     proc = type->setProc; break;
        }
        break;
        }

    /* WORD_TYPE_UNKNOWN */

    default:
        /* CANTHAPPEN */
        ASSERT(0);
        return 0;
    }

    return proc(map, key, value);
}

/*---------------------------------------------------------------------------
 * Col_IntMapSet
 *                                                                              *//*!
 *  Map the value to the integer key, replacing any existing.
 *
 *  Generic interface to integer map set, calls actual function depending on
 *  type.
 *
 *  @retval 0       if an existing entry was updated with **value**.
 *  @retval <>0     if a new entry was created with **key** and **value**.
 *//*-----------------------------------------------------------------------*/

int
Col_IntMapSet(
    Col_Word map,   /*!< Integer map to insert entry into. */
    intptr_t key,   /*!< Integer entry key. */
    Col_Word value) /*!< Entry value. */
{
    Col_IntMapSetProc *proc;

    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(map) return 0;                                             /*!     @typecheck{COL_ERROR_INTMAP,map} */

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_INTHASHMAP:
        proc = Col_IntHashMapSet;
        break;

    case WORD_TYPE_INTTRIEMAP:
        proc = Col_IntTrieMapSet;
        break;

    case WORD_TYPE_CUSTOM:
        ASSERT(WORD_TYPEINFO(map)->type == COL_INTMAP);
        proc = ((Col_CustomIntMapType *) WORD_TYPEINFO(map))->setProc;
        break;

    /* WORD_TYPE_UNKNOWN */

    default:
        /* CANTHAPPEN */
        ASSERT(0);
        return 0;
    }

    return proc(map, key, value);
}

/*---------------------------------------------------------------------------
 * Col_MapUnset
 *                                                                              *//*!
 *  Remove any value mapped to the given key.
 *
 *  Generic interface to map unset, calls actual function depending on type.
 *
 *  @retval 0       if no entry matching **key** was found.
 *  @retval <>0     if the existing entry was removed.
 *//*-----------------------------------------------------------------------*/

int
Col_MapUnset(
    Col_Word map,   /*!< Map to remove entry from. */
    Col_Word key)   /*!< Entry key. Can be any word type, including string,
                         however it must match the actual type used by the
                         map. */
{
    Col_MapUnsetProc *proc;

    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(map) return 0;                                            /*!     @typecheck{COL_ERROR_WORDMAP,map} */

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
        proc = Col_HashMapUnset;
        break;

    case WORD_TYPE_STRTRIEMAP:
        proc = Col_TrieMapUnset;
        break;

    case WORD_TYPE_CUSTOM: {
        Col_CustomMapType *type = (Col_CustomMapType *) WORD_TYPEINFO(map);
        switch (type->type.type) {
        case COL_HASHMAP: proc = Col_HashMapUnset; break;
        case COL_TRIEMAP: proc = Col_TrieMapUnset; break;
        case COL_MAP:     proc = type->unsetProc; break;
        }
        break;
        }

    /* WORD_TYPE_UNKNOWN */

    default:
        /* CANTHAPPEN */
        ASSERT(0);
        return 0;
    }

    return proc(map, key);
}

/*---------------------------------------------------------------------------
 * Col_IntMapUnset
 *                                                                              *//*!
 *  Remove any value mapped to the given integer key.
 *
 *  Generic interface to integer map unset, calls actual function depending
 *  on type.
 *
 *  @retval 0       if no entry matching **key** was found.
 *  @retval <>0     if the existing entry was removed.
 *//*-----------------------------------------------------------------------*/

int
Col_IntMapUnset(
    Col_Word map,   /*!< Integer map to remove entry fromo. */
    intptr_t key)   /*!< Integer entry key. */
{
    Col_IntMapUnsetProc *proc;

    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(map) return 0;                                             /*!     @typecheck{COL_ERROR_INTMAP,map} */

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_INTHASHMAP:
        proc = Col_IntHashMapUnset;
        break;

    case WORD_TYPE_INTTRIEMAP:
        proc = Col_IntTrieMapUnset;
        break;

    case WORD_TYPE_CUSTOM:
        ASSERT(WORD_TYPEINFO(map)->type == COL_INTMAP);
        proc = ((Col_CustomIntMapType *) WORD_TYPEINFO(map))->unsetProc;
        break;

    /* WORD_TYPE_UNKNOWN */

    default:
        /* CANTHAPPEN */
        ASSERT(0);
        return 0;
    }

    return proc(map, key);
}


/*******************************************************************************
 * Map Iteration
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * Col_MapIterBegin
 *                                                                              *//*!
 *  Initialize the map iterator so that it points to the first entry within
 *  the map.
 *
 *  Generic interface to map iteration, calls actual function depending
 *  on type.
 *//*-----------------------------------------------------------------------*/

void
Col_MapIterBegin(
    Col_MapIterator it, /*!< Iterator to initialize. */
    Col_Word map)       /*!< Map to iterate over. */
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAP(map) {                                                        /*!     @typecheck{COL_ERROR_MAP,map} */
        Col_MapIterSetNull(it);
        return;
    }

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
    case WORD_TYPE_INTHASHMAP:
        Col_HashMapIterBegin(it, map);
        break;

    case WORD_TYPE_STRTRIEMAP:
    case WORD_TYPE_INTTRIEMAP:
        Col_TrieMapIterFirst(it, map);
        break;

    case WORD_TYPE_CUSTOM:
        switch (WORD_TYPEINFO(map)->type) {
        case COL_HASHMAP: Col_HashMapIterBegin(it, map); return;
        case COL_TRIEMAP: Col_TrieMapIterFirst(it, map); return;

        /*
         * Note: for custom maps we set the iterator entry field to a non-nil
         * value upon success (i.e. when iterBeginProc returns nonzero) so that
         * Col_MapIterEnd() is false.
         */

        case COL_MAP:    it->entry = ((Col_CustomMapType *)    WORD_TYPEINFO(map))->iterBeginProc(map, &it->traversal.custom) ? 1 : WORD_NIL; return;
        case COL_INTMAP: it->entry = ((Col_CustomIntMapType *) WORD_TYPEINFO(map))->iterBeginProc(map, &it->traversal.custom) ? 1 : WORD_NIL; return;
        }
        /* continued. */

    /* WORD_TYPE_UNKNOWN */

    default:
        /* CANTHAPPEN */
        ASSERT(0);
        Col_MapIterSetNull(it);
    }
}

/*---------------------------------------------------------------------------
 * Col_MapIterFind
 *                                                                              *//*!
 *  Initialize the map iterator so that it points to the entry with the
 *  given key within the map.
 *
 *  Generic interface to map iteration, calls actual function depending
 *  on type.
 *//*-----------------------------------------------------------------------*/

void
Col_MapIterFind(
    Col_MapIterator it, /*!< Iterator to initialize. */
    Col_Word map,       /*!< Map to iterate over. */
    Col_Word key,       /*!< Entry key. Can be any word type, including string,
                             however it must match the actual type used by the
                             map. */
    int *createPtr)     /*!< [in,out] If non-NULL, whether to create entry if
                             absent on input, and whether an entry was created
                             on output. */
{
    /*
     * Check preconditions.
     */

    TYPECHECK_WORDMAP(map) {                                                    /*!     @typecheck{COL_ERROR_WORDMAP,map} */
        Col_MapIterSetNull(it);
        return;
    }

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_STRHASHMAP:
        Col_HashMapIterFind(it, map, key, createPtr);
        break;

    case WORD_TYPE_STRTRIEMAP:
        Col_TrieMapIterFind(it, map, key, createPtr);
        break;

    case WORD_TYPE_CUSTOM:
        switch (WORD_TYPEINFO(map)->type) {
        case COL_HASHMAP: Col_HashMapIterFind(it, map, key, createPtr); return;
        case COL_TRIEMAP: Col_TrieMapIterFind(it, map, key, createPtr); return;

        /*
         * Note: for custom maps we set the iterator entry field to a non-nil
         * value upon success (i.e. when iterFindProc returns nonzero) so that
         * Col_MapIterEnd() is false.
         */

        case COL_MAP: it->entry = ((Col_CustomMapType *) WORD_TYPEINFO(map))->iterFindProc(map, key, createPtr, &it->traversal.custom) ? 1 : WORD_NIL; return;
        }
        /* continued. */

    /* WORD_TYPE_UNKNOWN */

    default:
        /* CANTHAPPEN */
        ASSERT(0);
        Col_MapIterSetNull(it);
    }
}

/*---------------------------------------------------------------------------
 * Col_IntMapIterFind
 *                                                                              *//*!
 *  Initialize the map iterator so that it points to the entry with the
 *  given integer key within the map.
 *
 *  Generic interface to integer map iteration, calls actual function depending
 *  on type.
 *//*-----------------------------------------------------------------------*/

void
Col_IntMapIterFind(
    Col_MapIterator it, /*!< Iterator to initialize. */
    Col_Word map,       /*!< Integer map to iterate over. */
    intptr_t key,       /*!< Integer entry key. */
    int *createPtr)     /*!< [in,out] If non-NULL, whether to create entry if
                             absent on input, and whether an entry was created
                             on output. */
{
    /*
     * Check preconditions.
     */

    TYPECHECK_INTMAP(map) {                                                     /*!     @typecheck{COL_ERROR_INTMAP,map} */
        Col_MapIterSetNull(it);
        return;
    }

    switch (WORD_TYPE(map)) {
    case WORD_TYPE_INTHASHMAP:
        Col_IntHashMapIterFind(it, map, key, createPtr);
        break;

    case WORD_TYPE_INTTRIEMAP:
        Col_IntTrieMapIterFind(it, map, key, createPtr);
        break;

    case WORD_TYPE_CUSTOM:
        ASSERT(WORD_TYPEINFO(map)->type == COL_INTMAP);
        /*
         * Note: for custom maps we set the iterator entry field to a non-nil
         * value upon success (i.e. when iterFindProc returns nonzero) so that
         * Col_MapIterEnd() is false.
         */

        it->entry = ((Col_CustomIntMapType *) WORD_TYPEINFO(map))->iterFindProc(map, key, createPtr, &it->traversal.custom) ? 1 : WORD_NIL; return;
        break;

    /* WORD_TYPE_UNKNOWN */

    default:
        /* CANTHAPPEN */
        ASSERT(0);
        Col_MapIterSetNull(it);
    }
}

/*---------------------------------------------------------------------------
 * Col_MapIterGet
 *                                                                              *//*!
 *  Get key & value from map iterator.
 *//*-----------------------------------------------------------------------*/

void
Col_MapIterGet(
    Col_MapIterator it, /*!< Map iterator to get key & value from. */

    /*! [out] Entry key. */
    Col_Word *keyPtr,

    /*! [out] Entry value. */
    Col_Word *valuePtr)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAPITER(it) return;                                               /*!     @typecheck{COL_ERROR_MAPITER,it} */
    TYPECHECK_WORDMAP(it->map) return;                                          /*!     @typecheck{COL_ERROR_WORDMAP,[Col_MapIterMap(it)](@ref Col_MapIterMap)} */
    VALUECHECK_MAPITER(it) return;                                              /*!     @valuecheck{COL_ERROR_MAPITER_END,it} */

    ASSERT(it->entry);

    if (WORD_TYPE(it->map) == WORD_TYPE_CUSTOM) {
        switch (WORD_TYPEINFO(it->map)->type) {
        case COL_MAP: {
            Col_CustomMapType *type
                    = (Col_CustomMapType *) WORD_TYPEINFO(it->map);
            *keyPtr   = type->iterGetKeyProc  (it->map, &it->traversal.custom);
            *valuePtr = type->iterGetValueProc(it->map, &it->traversal.custom);
            return;
            }
        }

        /*
         * Other custom maps are hash or trie maps and use regular entries.
         */
    }

    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_HASHENTRY
            || WORD_TYPE(it->entry) == WORD_TYPE_MHASHENTRY
            || WORD_TYPE(it->entry) == WORD_TYPE_TRIELEAF
            || WORD_TYPE(it->entry) == WORD_TYPE_MTRIELEAF);

    *keyPtr   = WORD_MAPENTRY_KEY  (it->entry);
    *valuePtr = WORD_MAPENTRY_VALUE(it->entry);
}

/*---------------------------------------------------------------------------
 * Col_IntMapIterGet
 *                                                                              *//*!
 *  Get key & value from integer map iterator.
 *//*-----------------------------------------------------------------------*/

void
Col_IntMapIterGet(
    Col_MapIterator it, /*!< Integer map iterator to get key & value from. */

    /*! [out] Integer entry key. */
    intptr_t *keyPtr,

    /*! [out] Entry value. */
    Col_Word *valuePtr)
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAPITER(it) return;                                               /*!     @typecheck{COL_ERROR_MAPITER,it} */
    TYPECHECK_INTMAP(it->map) return;                                           /*!     @typecheck{COL_ERROR_INTMAP,[Col_MapIterMap(it)](@ref Col_MapIterMap)} */
    VALUECHECK_MAPITER(it) return;                                              /*!     @valuecheck{COL_ERROR_MAPITER_END,it} */

    ASSERT(it->entry);

    if (WORD_TYPE(it->map) == WORD_TYPE_CUSTOM) {
        Col_CustomIntMapType *type
                = (Col_CustomIntMapType *) WORD_TYPEINFO(it->map);
        ASSERT(WORD_TYPEINFO(it->map)->type == COL_INTMAP);
        *keyPtr   = type->iterGetKeyProc  (it->map, &it->traversal.custom);
        *valuePtr = type->iterGetValueProc(it->map, &it->traversal.custom);
        return;
    }

    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_INTHASHENTRY
            || WORD_TYPE(it->entry) == WORD_TYPE_MINTHASHENTRY
            || WORD_TYPE(it->entry) == WORD_TYPE_INTTRIELEAF
            || WORD_TYPE(it->entry) == WORD_TYPE_MINTTRIELEAF);

    *keyPtr   = WORD_INTMAPENTRY_KEY(it->entry);
    *valuePtr = WORD_MAPENTRY_VALUE (it->entry);
}

/*---------------------------------------------------------------------------
 * Col_MapIterGetKey
 *                                                                              *//*!
 *  Get key from map iterator.
 *
 *  @return
 *      Entry key.
 *//*-----------------------------------------------------------------------*/

Col_Word
Col_MapIterGetKey(
    Col_MapIterator it) /*!< Map iterator to get key from. */
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAPITER(it) return WORD_NIL;                                      /*!     @typecheck{COL_ERROR_MAPITER,it} */
    TYPECHECK_WORDMAP(it->map) return WORD_NIL;                                 /*!     @typecheck{COL_ERROR_WORDMAP,[Col_MapIterMap(it)](@ref Col_MapIterMap)} */
    VALUECHECK_MAPITER(it) return WORD_NIL;                                     /*!     @valuecheck{COL_ERROR_MAPITER_END,it} */

    ASSERT(it->entry);

    if (WORD_TYPE(it->map) == WORD_TYPE_CUSTOM) {
        switch (WORD_TYPEINFO(it->map)->type) {
        case COL_MAP: return ((Col_CustomMapType *) WORD_TYPEINFO(it->map))->iterGetKeyProc(it->map, &it->traversal.custom);
        }

        /*
         * Other custom maps are hash or trie maps and use regular entries.
         */
    }

    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_HASHENTRY
            || WORD_TYPE(it->entry) == WORD_TYPE_MHASHENTRY
            || WORD_TYPE(it->entry) == WORD_TYPE_TRIELEAF
            || WORD_TYPE(it->entry) == WORD_TYPE_MTRIELEAF);

    return WORD_MAPENTRY_KEY(it->entry);
}

/*---------------------------------------------------------------------------
 * Col_IntMapIterGetKey
 *                                                                              *//*!
 *  Get integer key from integer map iterator.
 *
 *  @return
 *      Integer entry key.
 *//*-----------------------------------------------------------------------*/

intptr_t
Col_IntMapIterGetKey(
    Col_MapIterator it) /*!< Integer map iterator to get key from. */
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAPITER(it) return 0;                                             /*!     @typecheck{COL_ERROR_MAPITER,it} */
    TYPECHECK_INTMAP(it->map) return 0;                                         /*!     @typecheck{COL_ERROR_INTMAP,[Col_MapIterMap(it)](@ref Col_MapIterMap)} */
    VALUECHECK_MAPITER(it) return 0;                                            /*!     @valuecheck{COL_ERROR_MAPITER_END,it} */

    ASSERT(it->entry);

    if (WORD_TYPE(it->map) == WORD_TYPE_CUSTOM) {
        ASSERT(WORD_TYPEINFO(it->map)->type == COL_INTMAP);
        return ((Col_CustomIntMapType *) WORD_TYPEINFO(it->map))->iterGetKeyProc(it->map, &it->traversal.custom);
    }

    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_INTHASHENTRY
            || WORD_TYPE(it->entry) == WORD_TYPE_MINTHASHENTRY
            || WORD_TYPE(it->entry) == WORD_TYPE_INTTRIELEAF
            || WORD_TYPE(it->entry) == WORD_TYPE_MINTTRIELEAF);

    return WORD_INTMAPENTRY_KEY(it->entry);
}

/*---------------------------------------------------------------------------
 * Col_MapIterGetValue
 *                                                                              *//*!
 *  Get value from map iterator.
 *
 *  @return
 *      Entry value.
 *//*-----------------------------------------------------------------------*/

Col_Word
Col_MapIterGetValue(
    Col_MapIterator it) /*!< Map iterator to get value from. */
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAPITER(it) return WORD_NIL;                                      /*!     @typecheck{COL_ERROR_MAPITER,it} */
    VALUECHECK_MAPITER(it) return WORD_NIL;                                     /*!     @valuecheck{COL_ERROR_MAPITER_END,it} */

    ASSERT(it->entry);

    if (WORD_TYPE(it->map) == WORD_TYPE_CUSTOM) {
        switch (WORD_TYPEINFO(it->map)->type) {
        case COL_MAP:    return ((Col_CustomMapType *)    WORD_TYPEINFO(it->map))->iterGetValueProc(it->map, &it->traversal.custom);
        case COL_INTMAP: return ((Col_CustomIntMapType *) WORD_TYPEINFO(it->map))->iterGetValueProc(it->map, &it->traversal.custom);
        }

        /*
         * Other custom maps are hash or trie maps and use regular entries.
         */
    }

    ASSERT(WORD_TYPE(it->entry) == WORD_TYPE_HASHENTRY
            || WORD_TYPE(it->entry) == WORD_TYPE_MHASHENTRY
            || WORD_TYPE(it->entry) == WORD_TYPE_INTHASHENTRY
            || WORD_TYPE(it->entry) == WORD_TYPE_MINTHASHENTRY
            || WORD_TYPE(it->entry) == WORD_TYPE_TRIELEAF
            || WORD_TYPE(it->entry) == WORD_TYPE_MTRIELEAF
            || WORD_TYPE(it->entry) == WORD_TYPE_INTTRIELEAF
            || WORD_TYPE(it->entry) == WORD_TYPE_MINTTRIELEAF);

    return WORD_MAPENTRY_VALUE(it->entry);
}

/*---------------------------------------------------------------------------
 * Col_MapIterSetValue
 *                                                                              *//*!
 *  Set value of map iterator.
 *
 *  Generic interface to map iteration, calls actual function depending
 *  on type.
 *//*-----------------------------------------------------------------------*/

void
Col_MapIterSetValue(
    Col_MapIterator it, /*!< Map iterator to set value for. */
    Col_Word value)     /*!< Value to set. */
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAPITER(it) return;                                               /*!     @typecheck{COL_ERROR_MAPITER,it} */
    VALUECHECK_MAPITER(it) return;                                              /*!     @valuecheck{COL_ERROR_MAPITER_END,it} */

    switch (WORD_TYPE(it->map)) {
    case WORD_TYPE_STRHASHMAP:
    case WORD_TYPE_INTHASHMAP:
        Col_HashMapIterSetValue(it, value);
        break;

    case WORD_TYPE_STRTRIEMAP:
    case WORD_TYPE_INTTRIEMAP:
        Col_TrieMapIterSetValue(it, value);
        break;

    case WORD_TYPE_CUSTOM:
        switch (WORD_TYPEINFO(it->map)->type) {
        case COL_HASHMAP: Col_HashMapIterSetValue(it, value); return;
        case COL_TRIEMAP: Col_TrieMapIterSetValue(it, value); return;
        case COL_MAP:     ((Col_CustomMapType *)    WORD_TYPEINFO(it->map))->iterSetValueProc(it->map, value, &it->traversal.custom); return;
        case COL_INTMAP:  ((Col_CustomIntMapType *) WORD_TYPEINFO(it->map))->iterSetValueProc(it->map, value, &it->traversal.custom); return;
        }
        /* continued. */

    /* WORD_TYPE_UNKNOWN */

    default:
        /* CANTHAPPEN */
        ASSERT(0);
    }
}

/*---------------------------------------------------------------------------
 * Col_MapIterNext
 *                                                                              *//*!
 *  Move the iterator to the next element.
 *
 *  Generic interface to map iteration, calls actual function depending
 *  on type.
 *//*-----------------------------------------------------------------------*/

void
Col_MapIterNext(
    Col_MapIterator it) /*!< The iterator to move. */
{
    /*
     * Check preconditions.
     */

    TYPECHECK_MAPITER(it) return;                                               /*!     @typecheck{COL_ERROR_MAPITER,it} */
    VALUECHECK_MAPITER(it) return;                                              /*!     @valuecheck{COL_ERROR_MAPITER_END,it} */

    switch (WORD_TYPE(it->map)) {
    case WORD_TYPE_STRHASHMAP:
    case WORD_TYPE_INTHASHMAP:
        Col_HashMapIterNext(it);
        break;

    case WORD_TYPE_STRTRIEMAP:
    case WORD_TYPE_INTTRIEMAP:
        Col_TrieMapIterNext(it);
        break;

    case WORD_TYPE_CUSTOM:
        switch (WORD_TYPEINFO(it->map)->type) {
        case COL_HASHMAP: Col_HashMapIterNext(it); return;
        case COL_TRIEMAP: Col_TrieMapIterNext(it); return;

        /*
         * Note: for custom maps we set the iterator entry field to a non-nil
         * value upon success (i.e. when iterNextProc returns nonzero) so that
         * Col_MapIterEnd() is false.
         */

        case COL_MAP:    it->entry = ((Col_CustomMapType *)    WORD_TYPEINFO(it->map))->iterNextProc(it->map, &it->traversal.custom) ? 1 : WORD_NIL; return;
        case COL_INTMAP: it->entry = ((Col_CustomIntMapType *) WORD_TYPEINFO(it->map))->iterNextProc(it->map, &it->traversal.custom) ? 1 : WORD_NIL; return;
        }
        /* continued. */

    /* WORD_TYPE_UNKNOWN */

    default:
        /* CANTHAPPEN */
        ASSERT(0);
    }
}

                                                                                /*!     @} */
