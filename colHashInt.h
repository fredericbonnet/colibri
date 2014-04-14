/*                                                                              *//*!   @cond PRIVATE @file \
 * colHashInt.h
 *
 *  This header file defines the hash map word internals of Colibri.
 *
 *  Hash maps are an implementation of generic @ref map_words that use key
 *  hashing and flat bucket arrays for string, integer and custom keys.
 *
 *  They are always mutable.
 *
 *  @see colHash.c
 *  @see colHash.h
 *  @see colMapInt.h
 *
 *  @private
 */

#ifndef _COLIBRI_HASH_INT
#define _COLIBRI_HASH_INT


/*
================================================================================*//*!   @addtogroup hashmap_words \
Hash Maps
                                                                                        @ingroup words map_words
  Hash maps are an implementation of generic @ref map_words that use key
  hashing and flat bucket arrays for string, integer and custom keys.

  They are always mutable.

  @par Requirements
  - Hash maps can use string or integer keys, but also custom keys. In the
    latter case they need to point to custom word type information. So
    string and integer keyed hash maps use predefined numeric types, and
    custom keyed hash maps use a custom word type.

  - As hash maps are containers they must know their size, i.e. the number
    of entries they contain.

  - Hash maps store entries in buckets according to their hash value.
    Buckets are flat arrays of entries (themselves forming a linked
    list). So hash maps must know the location of their bucket container.
    For small sizes, buckets are stored inline within the word cells. For
    larger sizes we use @ref mvector_words for container.

  - Hash map words use one cell for the header plus several extra cells for
    static buckets (see #HASHMAP_STATICBUCKETS_NBCELLS).

  @param Type               Type descriptor.
  @param Synonym            [Generic word synonym field] (@ref WORD_SYNONYM).
  @param Size               Number of elements in map.
  @param Buckets            Bucket container for larger sizes. If nil, buckets
                            are stored in the inline static bucket array. Else
                            they use @ref mvector_words.
  @param Static_buckets     Buckets for smaller sizes.

  @par Cell Layout
    On all architectures the cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        hashmap_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="160" align="left">0</td><td sides="B" width="160" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEINFO" title="WORD_TYPEINFO" colspan="2" width="320">Type</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_SYNONYM" title="WORD_SYNONYM" colspan="2">Synonym</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_HASHMAP_SIZE" title="WORD_HASHMAP_SIZE" colspan="2">Size</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_HASHMAP_BUCKETS" title="WORD_HASHMAP_BUCKETS" colspan="2">Buckets</td>
            </tr>
            <tr><td sides="R">.</td>
                <td href="@ref WORD_HASHMAP_STATICBUCKETS" title="WORD_HASHMAP_STATICBUCKETS" colspan="2" rowspan="4">Static buckets</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }
    @enddot

                                                                                        @if IGNORE
           0                                                             n
          +---------------------------------------------------------------+
        0 |                           Type info                           |
          +---------------------------------------------------------------+
        1 |                            Synonym                            |
          +---------------------------------------------------------------+
        2 |                             Size                              |
          +---------------------------------------------------------------+
        3 |                            Buckets                            |
          +---------------------------------------------------------------+
          .                                                               .
          .                        Static buckets                         .
        N |                                                               |
          +---------------------------------------------------------------+
                                                                                        @endif
                                                                                *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Hash Map Constants                                                           *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * HASHMAP_HEADER_SIZE
 *  Byte size of hash map header.
 *                                                                              *//*!   @def \
 * HASHMAP_STATICBUCKETS_NBCELLS
 *  Number of cells allocated for static bucket array.
 *                                                                              *//*!   @def \
 * HASHMAP_STATICBUCKETS_SIZE
 *  Number of elements in static bucket array.
 *                                                                              *//*!   @def \
 * HASHMAP_NBCELLS
 *  Number of cells taken by hash maps.
 *//*-----------------------------------------------------------------------*/

#define HASHMAP_HEADER_SIZE             (sizeof(Col_CustomWordType*)+sizeof(Col_Word)*2+sizeof(size_t))
#define HASHMAP_STATICBUCKETS_NBCELLS   2
#define HASHMAP_STATICBUCKETS_SIZE      (HASHMAP_STATICBUCKETS_NBCELLS*CELL_SIZE/sizeof(Col_Word))
#define HASHMAP_NBCELLS                 (HASHMAP_STATICBUCKETS_NBCELLS+1)
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Hash Map Accessors                                                           *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_HASHMAP_SIZE
 *  Get/set number of elements in map.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_HASHMAP_INIT
 *  @see WORD_STRHASHMAP_INIT
 *  @see WORD_INTHASHMAP_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_HASHMAP_BUCKETS
 *  Get/set bucket container.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_HASHMAP_INIT
 *  @see WORD_STRHASHMAP_INIT
 *  @see WORD_INTHASHMAP_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_HASHMAP_STATICBUCKETS
 *  Pointer to inline bucket array for small sized maps.
 *//*-----------------------------------------------------------------------*/

#define WORD_HASHMAP_SIZE(word)         (((size_t *)(word))[2])
#define WORD_HASHMAP_BUCKETS(word)      (((Col_Word *)(word))[3])
#define WORD_HASHMAP_STATICBUCKETS(word) ((Col_Word *)((char *)(word)+HASHMAP_HEADER_SIZE))
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Hash Map Exceptions                                                          *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * TYPECHECK_HASHMAP
 *                                                                                      @hideinitializer
 *  Type checking macro for hash maps.
 *
 *  @param word     Checked word.
 *
 *  @typecheck{COL_ERROR_HASHMAP,word}
 *//*-----------------------------------------------------------------------*/

#define TYPECHECK_HASHMAP(word) \
    TYPECHECK((Col_WordType(word) & COL_HASHMAP), COL_ERROR_HASHMAP, (word))

/*---------------------------------------------------------------------------   *//*!   @def \
 * TYPECHECK_WORDHASHMAP
 *                                                                                      @hideinitializer
 *  Type checking macro for word-based hash maps (string or custom).
 *
 *  @param word     Checked word.
 *
 *  @typecheck{COL_ERROR_WORDHASHMAP,word}
 *//*-----------------------------------------------------------------------*/

#define TYPECHECK_WORDHASHMAP(word) \
    TYPECHECK(((Col_WordType(word) & (COL_HASHMAP | COL_INTMAP)) \
            == COL_HASHMAP), COL_ERROR_WORDHASHMAP, (word))
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup strhashmap_words \
String Hash Maps
                                                                                        @ingroup predefined_words strmap_words hashmap_words
  String hash maps are @ref hashmap_words with string keys.

  @see WORD_TYPE_STRHASHMAP                                                     *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * String Hash Map Creation                                                     *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_STRHASHMAP_INIT
 *
 *  String hash map word initializer.
 *
 *  String hash maps are specialized hash maps using built-in compare and
 *  hash procs on string keys.
 *
 *  @param word         Word to initialize.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_STRHASHMAP
 *//*-----------------------------------------------------------------------*/

#define WORD_STRHASHMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_STRHASHMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_HASHMAP_SIZE(word) = 0; \
    WORD_HASHMAP_BUCKETS(word) = WORD_NIL;
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup inthashmap_words \
Integer Hash Maps
                                                                                        @ingroup predefined_words intmap_words hashmap_words
  Integer hash maps are @ref hashmap_words with integer keys.

  @see WORD_TYPE_INTHASHMAP                                                     *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Integer Hash Map Creation                                                    *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_INTHASHMAP_INIT
 *
 *  Integer hash map word initializer.
 *
 *  Integer hash maps are specialized hash maps where the hash value is the
 *  randomized integer key.
 *
 *  @param word         Word to initialize.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_INTHASHMAP
 *//*-----------------------------------------------------------------------*/

#define WORD_INTHASHMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTHASHMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_HASHMAP_SIZE(word) = 0; \
    WORD_HASHMAP_BUCKETS(word) = WORD_NIL;
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Integer Hash Map Exceptions                                                  *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * TYPECHECK_INTHASHMAP
 *                                                                                      @hideinitializer
 *  Type checking macro for integer hash maps.
 *
 *  @param word     Checked word.
 *
 *  @typecheck{COL_ERROR_INTHASHMAP,word}
 *//*-----------------------------------------------------------------------*/

#define TYPECHECK_INTHASHMAP(word) \
    TYPECHECK(((Col_WordType(word) & (COL_HASHMAP | COL_INTMAP)) \
            == (COL_HASHMAP | COL_INTMAP)), COL_ERROR_INTHASHMAP, (word))
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup customhashmap_words \
Custom Hash Maps
                                                                                        @ingroup hashmap_words custom_words
  Custom hash maps are @ref custom_words implementing @ref hashmap_words with
  applicative code.

  @see Col_CustomHashMapType                                                    *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Custom Hash Map Constants                                                    *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * CUSTOMHASHMAP_HEADER_SIZE
 *  Byte size of custom hash map word header.
 *//*-----------------------------------------------------------------------*/

#define CUSTOMHASHMAP_HEADER_SIZE       (HASHMAP_NBCELLS*CELL_SIZE)
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Custom Hash Map Creation                                                     *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_HASHMAP_INIT
 *
 *  Custom hash map word initializer.
 *
 *  Custom hash maps are custom word types of type #COL_HASHMAP.
 *
 *  @param word         Word to initialize.
 *  @param typeInfo     #WORD_SET_TYPEINFO.
 *
 *  @see Col_CustomHashMapType
 *//*-----------------------------------------------------------------------*/

#define WORD_HASHMAP_INIT(word, typeInfo) \
    WORD_SET_TYPEINFO((word), (typeInfo)); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_HASHMAP_SIZE(word) = 0; \
    WORD_HASHMAP_BUCKETS(word) = WORD_NIL;
                                                                               /*!     @} */
                                                                               /*!     @} */
/*
================================================================================*//*!   @addtogroup mhashentry_words \
Hash Entries
                                                                                        @ingroup predefined_words mapentry_words hashmap_words
  Hash entries are @ref mapentry_words with hash-specific data.

  @par Requirements
  - Hash entry words, like generic @ref mapentry_words, use one single cell.

  - Hash entries sharing the same bucket are chained using a linked list, so
    each entry store must store a pointer to the next entry in chain.

  - In addition we cache part of the hash value in the remaining bytes to
    avoid recomputing in many cases.

  @param Key        [Generic map entry key field] (@ref WORD_MAPENTRY_KEY).
  @param Value      [Generic map entry value field] (@ref WORD_MAPENTRY_VALUE).
  @param Next       Next entry in bucket. Also used during iteration.
  @param Hash       Higher order bits of the hash value for fast negative test
                    and rehashing. The full value is computed by combining these
                    high order bits and the bucket index whenever possible, else
                    the value is recomputed from the key.

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        hashentry_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_HASHENTRY_HASH" title="WORD_HASHENTRY_HASH" colspan="2">Hash</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_HASHENTRY_NEXT" title="WORD_HASHENTRY_NEXT" colspan="4">Next</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_MAPENTRY_KEY" title="WORD_MAPENTRY_KEY" colspan="4">Key (Generic)</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_MAPENTRY_VALUE" title="WORD_MAPENTRY_VALUE" colspan="4">Value (Generic)</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0     7 8                                                     n
          +-------+-------------------------------------------------------+
        0 | Type  |                         Hash                          |
          +-------+-------------------------------------------------------+
        1 |                             Next                              |
          +---------------------------------------------------------------+
        2 |                          Key (Generic)                        |
          +---------------------------------------------------------------+
        3 |                         Value (Generic)                       |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see @ref mapentry_words
  @see @ref hashmap_words                                                       *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Hash Entry Creation                                                          *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_MHASHENTRY_INIT
 *
 *  Mutable hash entry word initializer.
 *
 *  @param word         Word to initialize.
 *  @param key          #WORD_MAPENTRY_KEY.
 *  @param value        #WORD_MAPENTRY_VALUE.
 *  @param next         #WORD_HASHENTRY_NEXT.
 *  @param hash         #WORD_HASHENTRY_SET_HASH.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_MHASHENTRY
 *//*-----------------------------------------------------------------------*/

#define WORD_MHASHENTRY_INIT(word, key, value, next, hash) \
    WORD_SET_TYPEID((word), WORD_TYPE_MHASHENTRY); \
    WORD_MAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value); \
    WORD_HASHENTRY_NEXT(word) = (next); \
    WORD_HASHENTRY_SET_HASH(word, hash);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Hash Entry Accessors                                                         *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_HASHENTRY_HASH
 *  Get higher order bits of the hash value.
 *
 *  @param word     Word to access.
 *
 *  @see WORD_HASHENTRY_SET_HASH
 *
 *                                                                              *//*!   @def \
 * WORD_HASHENTRY_SET_HASH
 *  Set hash value.
 *
 *  @param word     Word to access.
 *  @param hash     Hash value. Only retains the high order bits.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_HASHENTRY_HASH
 *  @see WORD_HASHENTRY_INIT
 *  @see WORD_MHASHENTRY_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_HASHENTRY_NEXT
 *  Get/set pointer to next entry in bucket.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_HASHENTRY_INIT
 *  @see WORD_MHASHENTRY_INIT
 *  @see WORD_INTHASHENTRY_INIT
 *  @see WORD_MINTHASHENTRY_INIT
 *//*-----------------------------------------------------------------------*/
                                                                                #       ifndef DOXYGEN
#define HASHENTRY_HASH_MASK             (UINTPTR_MAX^UCHAR_MAX)
                                                                                #       endif DOXYGEN
#ifdef COL_BIGENDIAN
#   define WORD_HASHENTRY_HASH(word)    ((((uintptr_t *)(word))[0]<<CHAR_BIT)&HASHENTRY_HASH_MASK)
#   define WORD_HASHENTRY_SET_HASH(word, hash) ((((uintptr_t *)(word))[0])&=~(HASHENTRY_HASH_MASK>>CHAR_BIT),(((uintptr_t *)(word))[0])|=(((hash)&HASHENTRY_HASH_MASK)>>CHAR_BIT))
#else
#   define WORD_HASHENTRY_HASH(word)    ((((uintptr_t *)(word))[0])&HASHENTRY_HASH_MASK)
#   define WORD_HASHENTRY_SET_HASH(word, hash) ((((uintptr_t *)(word))[0])&=~HASHENTRY_HASH_MASK,(((uintptr_t *)(word))[0])|=((hash)&HASHENTRY_HASH_MASK))
#endif
#define WORD_HASHENTRY_NEXT(word)       (((Col_Word *)(word))[1])
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup hashentry_words \
Immutable Hash Entries
                                                                                        @ingroup predefined_words mapentry_words hashmap_words mhashentry_words
  Immutable hash entries are @ref mapentry_words with hash-specific data.

  They are former @ref mhashentry_words that became immutable as a result of
  a copy (see #Col_CopyHashMap): in this case the copy and the original
  would share most of their internals (rendered immutable in the process).
  Likewise, mutable operations on the copies would use copy-on-write
  semantics: immutable structures are copied to new mutable ones before
  modification.

  @see @ref mapentry_words
  @see @ref mhashentry_words
  @see @ref hashmap_words                                                       *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Immutable Hash Entry Creation                                                *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_HASHENTRY_INIT
 *
 *  Immutable hash entry word initializer.
 *
 *  @param word         Word to initialize.
 *  @param key          #WORD_MAPENTRY_KEY.
 *  @param value        #WORD_MAPENTRY_VALUE.
 *  @param next         #WORD_HASHENTRY_NEXT.
 *  @param hash         #WORD_HASHENTRY_SET_HASH.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_HASHENTRY
 *//*-----------------------------------------------------------------------*/

#define WORD_HASHENTRY_INIT(word, key, value, next, hash) \
    WORD_SET_TYPEID((word), WORD_TYPE_HASHENTRY); \
    WORD_MAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value); \
    WORD_HASHENTRY_NEXT(word) = (next); \
    WORD_HASHENTRY_SET_HASH(word, hash);
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup minthashentry_words \
Integer Hash Entries
                                                                                        @ingroup predefined_words mapentry_words inthashmap_words
  Integer hash entries are @ref intmapentry_words with hash-specific data.

  @par Requirements
  - Integer hash entry words, like generic @ref intmapentry_words, use one
    single cell.

  - Integer hash entries sharing the same bucket are chained using a linked
    list, so each entry store must store a pointer to the next entry in chain.

  @param Key        [Generic integer map entry key field]
                    (@ref WORD_INTMAPENTRY_KEY).
  @param Value      [Generic map entry value field] (@ref WORD_MAPENTRY_VALUE).
  @param Next       Next entry in bucket. Also used during iteration.

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        inthashentry_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td colspan="2" bgcolor="grey75">Unused</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_HASHENTRY_NEXT" title="WORD_HASHENTRY_NEXT" colspan="4">Next</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_INTMAPENTRY_KEY" title="WORD_INTMAPENTRY_KEY" colspan="4">Key (Generic)</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_MAPENTRY_VALUE" title="WORD_MAPENTRY_VALUE" colspan="4">Value (Generic)</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0     7 8                                                     n
          +-------+-------------------------------------------------------+
        0 | Type  |                        Unused                         |
          +-------+-------------------------------------------------------+
        1 |                             Next                              |
          +---------------------------------------------------------------+
        2 |                          Key (Generic)                        |
          +---------------------------------------------------------------+
        3 |                         Value (Generic)                       |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see @ref intmapentry_words
  @see @ref inthashmap_words
  @see @ref inthashentry_words                                                  *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Integer Hash Entry Creation                                                  *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_MINTHASHENTRY_INIT
 *
 *  Mutable integer hash entry word initializer.
 *
 *  @param word         Word to initialize.
 *  @param key          #WORD_INTMAPENTRY_KEY.
 *  @param value        #WORD_MAPENTRY_VALUE.
 *  @param next         #WORD_HASHENTRY_NEXT.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_MINTHASHENTRY
 *//*-----------------------------------------------------------------------*/

#define WORD_MINTHASHENTRY_INIT(word, key, value, next) \
    WORD_SET_TYPEID((word), WORD_TYPE_MINTHASHENTRY); \
    WORD_INTMAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value); \
    WORD_HASHENTRY_NEXT(word) = (next);
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup inthashentry_words \
Immutable Integer Hash Entries
                                                                                        @ingroup predefined_words mapentry_words inthashmap_words minthashentry_words
  Immutable integer hash entries are @ref mapentry_words with hash-specific
  data.

  They are former @ref minthashentry_words that became immutable as a result of
  a copy (see #Col_CopyHashMap): in this case the copy and the original
  would share most of their internals (rendered immutable in the process).
  Likewise, mutable operations on the copies would use copy-on-write
  semantics: immutable structures are copied to new mutable ones before
  modification.

  @see @ref mapentry_words
  @see @ref minthashentry_words
  @see @ref hashmap_words                                                       *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Immutable Integer Hash Entry Creation                                        *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_INTHASHENTRY_INIT
 *
 *  Immutable integer hash entry word initializer.
 *
 *  @param word         Word to initialize.
 *  @param key          #WORD_INTMAPENTRY_KEY.
 *  @param value        #WORD_MAPENTRY_VALUE.
 *  @param next         #WORD_HASHENTRY_NEXT.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_INTHASHENTRY
 *//*-----------------------------------------------------------------------*/

#define WORD_INTHASHENTRY_INIT(word, key, value, next) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTHASHENTRY); \
    WORD_INTMAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value); \
    WORD_HASHENTRY_NEXT(word) = (next);
                                                                                /*!     @} */
                                                                                /*!     @} */
#endif /* _COLIBRI_HASH_INT */
                                                                                /*!     @endcond */