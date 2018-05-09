/*                                                                              *//*!   @cond PRIVATE @file \
 * colTrieInt.h
 *
 *  This header file defines the trie map word internals of Colibri.
 *
 *  Trie maps are an implementation of generic @ref map_words that use crit-bit
 *  trees for string, integer and custom keys.
 *
 *  They are always mutable.
 *
 *  @see colTrie.c
 *  @see colTrie.h
 *  @see colMapInt.h
 *
 *  @private
 */

#ifndef _COLIBRI_TRIE_INT
#define _COLIBRI_TRIE_INT


/*
================================================================================*//*!   @addtogroup triemap_words \
Trie Maps

  Trie maps are an implementation of generic @ref map_words that use crit-bit
  trees for string, integer and custom keys.

  They are always mutable.

  @par Requirements
  - Trie maps words use one single cell.

  - As trie maps are containers they must know their size, i.e. the number
    of entries they contain.

  - Trie maps store entries in leaves within a tree made of nodes. So they
    need to know the root node of this tree.

  @param Type       Type descriptor.
  @param Synonym    [Generic word synonym field] (@ref WORD_SYNONYM).
  @param Size       Number of elements in map.
  @param Root       Root trie [node] (@ref trienode_words) or [leaf]
                    (@ref trieleaf_words).

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        triemap_word [shape=none, label=<
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
                <td href="@ref WORD_TRIEMAP_SIZE" title="WORD_TRIEMAP_SIZE" colspan="2">Size</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_TRIEMAP_ROOT" title="WORD_TRIEMAP_ROOT" colspan="2">Root</td>
            </tr>
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
        3 |                             Root                              |
          +---------------------------------------------------------------+
                                                                                        @endif
                                                                                *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Trie Map Accessors                                                           *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_TRIEMAP_SIZE
 *  Get/set number of elements in map.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_TRIEMAP_INIT
 *  @see WORD_STRTRIEMAP_INIT
 *  @see WORD_INTTRIEMAP_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_TRIEMAP_ROOT
 *  Get/set root node of the trie.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_TRIEMAP_INIT
 *  @see WORD_STRTRIEMAP_INIT
 *  @see WORD_INTTRIEMAP_INIT
 *//*-----------------------------------------------------------------------*/

#define WORD_TRIEMAP_SIZE(word)         (((size_t *)(word))[2])
#define WORD_TRIEMAP_ROOT(word)         (((Col_Word *)(word))[3])
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Trie Map Exceptions                                                          *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * TYPECHECK_TRIEMAP
 *                                                                                      @hideinitializer
 *  Type checking macro for trie maps.
 *
 *  @param word     Checked word.
 *
 *  @typecheck{COL_ERROR_TRIEMAP,word}
 *//*-----------------------------------------------------------------------*/

#define TYPECHECK_TRIEMAP(word) \
    TYPECHECK((Col_WordType(word) & COL_TRIEMAP), COL_ERROR_TRIEMAP, (word))

/*---------------------------------------------------------------------------   *//*!   @def \
 * TYPECHECK_WORDTRIEMAP
 *                                                                                      @hideinitializer
 *  Type checking macro for word-based trie maps (string or custom).
 *
 *  @param word     Checked word.
 *
 *  @typecheck{COL_ERROR_WORDTRIEMAP,word}
 *//*-----------------------------------------------------------------------*/

#define TYPECHECK_WORDTRIEMAP(word) \
    TYPECHECK(((Col_WordType(word) & (COL_TRIEMAP | COL_INTMAP)) \
            == COL_TRIEMAP), COL_ERROR_WORDTRIEMAP, (word))
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup strtriemap_words \
String Trie Maps
                                                                                        @ingroup predefined_words strmap_words triemap_words
  String trie maps are @ref triemap_words with string keys.

  @see WORD_TYPE_STRTRIEMAP                                                     *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * String Trie Map Creation                                                     *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_STRTRIEMAP_INIT
 *
 *  String trie map word initializer.
 *
 *  String trie maps are specialized trie maps using built-in bit test and
 *  key comparison procs on string keys.
 *
 *  @param word         Word to initialize.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_STRTRIEMAP
 *//*-----------------------------------------------------------------------*/

#define WORD_STRTRIEMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_STRTRIEMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_TRIEMAP_SIZE(word) = 0; \
    WORD_TRIEMAP_ROOT(word) = WORD_NIL;
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup inttriemap_words \
Integer Trie Maps
                                                                                        @ingroup predefined_words intmap_words triemap_words
  Integer trie maps are @ref triemap_words with integer keys.

  @see WORD_TYPE_INTTRIEMAP                                                     *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Integer Trie Map Creation                                                    *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_INTTRIEMAP_INIT
 *
 *  Integer trie map word initializer.
 *
 *  Integer trie maps are specialized trie maps using built-in bit test and
 *  key comparison procs on integer keys.
 *
 *  @param word         Word to initialize.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_INTTRIEMAP
 *//*-----------------------------------------------------------------------*/

#define WORD_INTTRIEMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTTRIEMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_TRIEMAP_SIZE(word) = 0; \
    WORD_TRIEMAP_ROOT(word) = WORD_NIL;
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Integer Trie Map Exceptions                                                  *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * TYPECHECK_INTTRIEMAP
 *                                                                                      @hideinitializer
 *  Type checking macro for integer trie maps.
 *
 *  @param word     Checked word.
 *
 *  @typecheck{COL_ERROR_INTTRIEMAP,word}
 *//*-----------------------------------------------------------------------*/

#define TYPECHECK_INTTRIEMAP(word) \
    TYPECHECK(((Col_WordType(word) & (COL_TRIEMAP | COL_INTMAP)) \
            == (COL_TRIEMAP | COL_INTMAP)), COL_ERROR_INTTRIEMAP, (word))
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup customtriemap_words \
Custom Trie Maps
                                                                                        @ingroup triemap_words custom_words
  Custom trie maps are @ref custom_words implementing @ref triemap_words with
  applicative code.

  @see Col_CustomTrieMapType                                                    *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Custom Trie Map Constants                                                    *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * CUSTOMTRIEMAP_HEADER_SIZE
 *  Byte size of custom trie map word header.
 *//*-----------------------------------------------------------------------*/

#define CUSTOMTRIEMAP_HEADER_SIZE       CELL_SIZE
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Custom Trie Map Creation                                                     *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_TRIEMAP_INIT
 *
 *    Custom trie map word initializer.
 *
 *    Custom trie maps are custom word types of type #COL_TRIEMAP.
 *
 *  @param word         Word to initialize.
 *  @param typeInfo     #WORD_SET_TYPEINFO.
 *
 *  @see Col_CustomTrieMapType
 *//*-----------------------------------------------------------------------*/

#define WORD_TRIEMAP_INIT(word, typeInfo) \
    WORD_SET_TYPEINFO((word), (typeInfo)); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_TRIEMAP_SIZE(word) = 0; \
    WORD_TRIEMAP_ROOT(word) = WORD_NIL;
                                                                               /*!     @} */
                                                                               /*!     @} */
/*
================================================================================*//*!   @addtogroup mtrienode_words \
Trie Nodes
                                                                                        @ingroup predefined_words triemap_words
  Generic trie nodes.

  Trie maps store map entries hierarchically, in such a way that all
  entries in a subtree share a common prefix. This implementation uses
  crit-bit trees for keys: each node stores a critical bit, which is the
  index of the bit where both branches differ. Entries where this bit is
  cleared are stored in the left subtree, and those with this bit set are
  stored in the right subtree, all in a recursive manner.

  @par Requirements
  - Trie node words use one single cell.

  - Trie node words must know their critical bit, which is the highest bit
    of the first differing key element where left and right subtrie keys
    differ. They store this information as the index of the differing key
    element as well as the position of the critical bit.

  - Crit-bit tries are binary trees and thus must know their left and right
    subtries.

  @param Diff       Index of first differing key element in left and right keys.
  @param Bit        Position of the critical bit in key element. 24-bit should
                    be sufficient for most applications (this handles up to
                    16777216-bit/2MB-long key elements).
  @param Left       Left subtrie. Keys have their critical bit cleared.
  @param Right      Right subtrie. Keys have their critical bit set.

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        mtrienode_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_TRIENODE_BIT" title="WORD_TRIENODE_BIT" colspan="2">Bit</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_TRIENODE_DIFF" title="WORD_TRIENODE_DIFF" colspan="5">Diff</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_TRIENODE_LEFT" title="WORD_TRIENODE_LEFT" colspan="5">Left</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_TRIENODE_RIGHT" title="WORD_TRIENODE_RIGHT" colspan="5">Right</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0     7 8                    31                               n
          +-------+-----------------------+-------------------------------+
        0 | Type  |        Bit            |        Unused (n > 32)        |
          +-------+-----------------------+-------------------------------+
        1 |                             Diff                              |
          +---------------------------------------------------------------+
        2 |                             Left                              |
          +---------------------------------------------------------------+
        3 |                             Right                             |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see @ref mtrienode_words
  @see @ref triemap_words                                                       *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Trie Node Creation                                                           *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_MTRIENODE_INIT
 *
 *  Mutable trie node initializer.
 *
 *  @param word         Word to initialize.
 *  @param diff         #WORD_TRIENODE_DIFF.
 *  @param bit          #WORD_TRIENODE_SET_BIT.
 *  @param left         #WORD_TRIENODE_LEFT.
 *  @param right        #WORD_TRIENODE_RIGHT.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_MTRIENODE
 *//*-----------------------------------------------------------------------*/

#define WORD_MTRIENODE_INIT(word, diff, bit, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_MTRIENODE); \
    WORD_TRIENODE_SET_BIT((word), (bit)); \
    WORD_TRIENODE_DIFF(word) = (diff); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Trie Node Accessors                                                          *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_TRIENODE_BIT
 *  Get critical bit position.
 *
 *  @param word     Word to access.
 *
 *  @see WORD_TRIENODE_SET_BIT
 *
 *                                                                              *//*!   @def \
 * WORD_TRIENODE_SET_BIT
 *  Set critical bit position.
 *
 *  @param word     Word to access.
 *  @param bit      Bit value. Only retains the low order bits.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TRIENODE_BIT
 *  @see WORD_TRIENODE_INIT
 *  @see WORD_MTRIENODE_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_TRIENODE_DIFF
 *  Get/set index of first differing key element in left and right keys.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_TRIENODE_INIT
 *  @see WORD_MTRIENODE_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_TRIENODE_LEFT
 *  Get/set left subtrie.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_TRIENODE_INIT
 *  @see WORD_MTRIENODE_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_TRIENODE_RIGHT
 *  Get/set right subtrie.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_TRIENODE_INIT
 *  @see WORD_MTRIENODE_INIT
 *//*-----------------------------------------------------------------------*/
                                                                                #       ifndef DOXYGEN
#define TRIENODE_MASK                   (UINT_MAX>>CHAR_BIT)
                                                                                #       endif /* DOXYGEN */
#ifdef COL_BIGENDIAN
#   define WORD_TRIENODE_BIT(word)      ((((Col_Char *)(word))[0])&TRIENODE_MASK)
#   define WORD_TRIENODE_SET_BIT(word, bit) ((((Col_Char *)(word))[0])&=~TRIENODE_MASK,(((Col_Char *)(word))[0])|=((bit)&TRIENODE_MASK))
#else
#   define WORD_TRIENODE_BIT(word)      ((((Col_Char *)(word))[0]>>CHAR_BIT)&TRIENODE_MASK)
#   define WORD_TRIENODE_SET_BIT(word, bit) ((((Col_Char *)(word))[0])&=~(TRIENODE_MASK<<CHAR_BIT),(((Col_Char *)(word))[0])|=(((bit)&TRIENODE_MASK)<<CHAR_BIT))
#endif
#define WORD_TRIENODE_DIFF(word)        (((size_t *)(word))[1])
#define WORD_TRIENODE_LEFT(word)        (((Col_Word *)(word))[2])
#define WORD_TRIENODE_RIGHT(word)       (((Col_Word *)(word))[3])
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup trienode_words \
Immutable Trie Nodes
                                                                                        @ingroup predefined_words triemap_words mtrienode_words
  Immutable trie nodes are former @ref mtrienode_words that became immutable as
  a result of a copy (see #Col_CopyTrieMap): in this case the copy and the
  original would share most of their internals (rendered immutable in the
  process). Likewise, mutable operations on the copies would use copy-on-write
  semantics: immutable structures are copied to new mutable ones before
  modification.

  @see @ref trienode_words
  @see @ref triemap_words                                                       *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Immutable Trie Node Creation                                                 *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_TRIENODE_INIT
 *
 *  Immutable trie node initializer.
 *
 *  @param word         Word to initialize.
 *  @param diff         #WORD_TRIENODE_DIFF.
 *  @param bit          #WORD_TRIENODE_SET_BIT.
 *  @param left         #WORD_TRIENODE_LEFT.
 *  @param right        #WORD_TRIENODE_RIGHT.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_TRIENODE
 *//*-----------------------------------------------------------------------*/

#define WORD_TRIENODE_INIT(word, diff, bit, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_TRIENODE); \
    WORD_TRIENODE_SET_BIT((word), (bit)); \
    WORD_TRIENODE_DIFF(word) = (diff); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup mstrtrienode_words \
String Trie Nodes
                                                                                        @ingroup predefined_words strtriemap_words
  String trie nodes are @ref mtrienode_words nodes with string-specific data.

  @par Requirements
  - String trie node words, like generic @ref mtrienode_words, use one single
    cell and know their left and right subtries.

  - String trie node words must know their critical bit, which is the
    highest bit of the first differing character where left and right
    subtrie keys differ. They store this information as the index of the
    differing character as well as a bit mask with this single bit set.

  @param Diff       Index of first differing character in left and right keys.
  @param Mask       Bitmask where only the critical bit is set. 24-bit is
                    sufficient for Unicode character codepoints.
  @param Left       [Generic trie node left subtrie field]
                    (@ref WORD_TRIENODE_LEFT).
  @param Right      [Generic trie node right subtrie field]
                    (@ref WORD_TRIENODE_RIGHT).

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        mtrienode_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_STRTRIENODE_MASK" title="WORD_STRTRIENODE_MASK" colspan="2">Mask</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_STRTRIENODE_DIFF" title="WORD_STRTRIENODE_DIFF" colspan="5">Diff</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_TRIENODE_LEFT" title="WORD_TRIENODE_LEFT" colspan="5">Left (Generic)</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_TRIENODE_RIGHT" title="WORD_TRIENODE_RIGHT" colspan="5">Right (Generic)</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0     7 8                    31                               n
          +-------+-----------------------+-------------------------------+
        0 | Type  |        Mask           |        Unused (n > 32)        |
          +-------+-----------------------+-------------------------------+
        1 |                             Diff                              |
          +---------------------------------------------------------------+
        2 |                        Left (Generic)                         |
          +---------------------------------------------------------------+
        3 |                        Right (Generic)                        |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see @ref mstrtrienode_words
  @see @ref strtriemap_words                                                    *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * String Trie Node Creation                                                    *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_MSTRTRIENODE_INIT
 *
 *  Mutable string trie node word initializer.
 *
 *  @param word         Word to initialize.
 *  @param diff         #WORD_STRTRIENODE_DIFF.
 *  @param mask         #WORD_STRTRIENODE_SET_MASK.
 *  @param left         #WORD_TRIENODE_LEFT.
 *  @param right        #WORD_TRIENODE_RIGHT.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_MSTRTRIENODE
 *//*-----------------------------------------------------------------------*/

#define WORD_MSTRTRIENODE_INIT(word, diff, mask, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_MSTRTRIENODE); \
    WORD_STRTRIENODE_SET_MASK((word), (mask)); \
    WORD_STRTRIENODE_DIFF(word) = (diff); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * String Trie Node Accessors                                                   *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_STRTRIENODE_MASK
 *  Get bitmask where only the critical bit is set.
 *
 *  @param word     Word to access.
 *
 *  @see WORD_STRTRIENODE_SET_MASK
 *
 *                                                                              *//*!   @def \
 * WORD_STRTRIENODE_SET_MASK
 *  Set bitmask to the critical bit.
 *
 *  @param word     Word to access.
 *  @param mask     Mask value.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_STRTRIENODE_MASK
 *  @see WORD_STRTRIENODE_INIT
 *  @see WORD_MSTRTRIENODE_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_STRTRIENODE_DIFF
 *  Get/set index of first differing character in left and right keys.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_TRIENODE_INIT
 *  @see WORD_MTRIENODE_INIT
 *//*-----------------------------------------------------------------------*/
                                                                                #       ifndef DOXYGEN
#define STRTRIENODE_MASK                (UINT_MAX>>CHAR_BIT)
                                                                                #       endif /* DOXYGEN */
#ifdef COL_BIGENDIAN
#   define WORD_STRTRIENODE_MASK(word)  ((((Col_Char *)(word))[0])&STRTRIENODE_MASK)
#   define WORD_STRTRIENODE_SET_MASK(word, mask) ((((Col_Char *)(word))[0])&=~STRTRIENODE_MASK,(((Col_Char *)(word))[0])|=((size)&STRTRIENODE_MASK))
#else
#   define WORD_STRTRIENODE_MASK(word)  ((((Col_Char *)(word))[0]>>CHAR_BIT)&STRTRIENODE_MASK)
#   define WORD_STRTRIENODE_SET_MASK(word, mask) ((((Col_Char *)(word))[0])&=~(STRTRIENODE_MASK<<CHAR_BIT),(((Col_Char *)(word))[0])|=(((mask)&STRTRIENODE_MASK)<<CHAR_BIT))
#endif
#define WORD_STRTRIENODE_DIFF(word)     (((size_t *)(word))[1])
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup strtrienode_words \
Immutable String Trie Nodes
                                                                                        @ingroup predefined_words strtriemap_words mstrtrienode_words
  String trie nodes are @ref trienode_words nodes with string-specific data.

  They are former @ref mstrtrienode_words that became immutable as a result of
  a copy (see #Col_CopyTrieMap): in this case the copy and the original
  would share most of their internals (rendered immutable in the process).
  Likewise, mutable operations on the copies would use copy-on-write
  semantics: immutable structures are copied to new mutable ones before
  modification.

  @see @ref strtrienode_words
  @see @ref strtriemap_words                                                    *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Immutable String Trie Node Creation                                          *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_STRTRIENODE_INIT
 *
 *  Immutable string trie node word initializer.
 *
 *  @param word         Word to initialize.
 *  @param diff         #WORD_STRTRIENODE_DIFF.
 *  @param mask         #WORD_STRTRIENODE_SET_MASK.
 *  @param left         #WORD_TRIENODE_LEFT.
 *  @param right        #WORD_TRIENODE_RIGHT.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_STRTRIENODE
 *//*-----------------------------------------------------------------------*/

#define WORD_STRTRIENODE_INIT(word, diff, mask, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_STRTRIENODE); \
    WORD_STRTRIENODE_SET_MASK(word, mask); \
    WORD_STRTRIENODE_DIFF(word) = (diff); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup minttrienode_words \
Integer Trie Nodes
                                                                                        @ingroup predefined_words inttriemap_words
  Integer trie nodes are @ref mtrienode_words nodes with integer-specific data.

  @par Requirements
  - Integer trie node words, like generic @ref mtrienode_words, use one single
    cell and know their left and right subtries.

  - Integer trie node words must know their critical bit, which is the
    highest bit where left and right subtrie keys differ. They store this
    information as a bitmask with this single bit set.

  @param Mask       Bitmask where only the critical bit is set.
  @param Left       [Generic trie node left subtrie field]
                    (@ref WORD_TRIENODE_LEFT).
  @param Right      [Generic trie node right subtrie field]
                    (@ref WORD_TRIENODE_RIGHT).

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        minttrienode_word [shape=none, label=<
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
                <td href="@ref WORD_INTTRIENODE_MASK" title="WORD_INTTRIENODE_MASK" colspan="4">Mask</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_TRIENODE_LEFT" title="WORD_TRIENODE_LEFT" colspan="4">Left (Generic)</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_TRIENODE_RIGHT" title="WORD_TRIENODE_RIGHT" colspan="4">Right (Generic)</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
             0     7                                                       n
          +-------+-------------------------------------------------------+
        0 | Type  |                        Unused                         |
          +-------+-------------------------------------------------------+
        1 |                             Mask                              |
          +---------------------------------------------------------------+
        2 |                        Left (Generic)                         |
          +---------------------------------------------------------------+
        3 |                        Right (Generic)                        |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see @ref inttrienode_words
  @see @ref inttriemap_words                                                    *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Integer Trie Node Creation                                                   *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_MINTTRIENODE_INIT
 *
 *  Mutable integer trie node word initializer.
 *
 *  @param word         Word to initialize.
 *  @param mask         #WORD_INTTRIENODE_MASK.
 *  @param left         #WORD_TRIENODE_LEFT.
 *  @param right        #WORD_TRIENODE_RIGHT.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_MINTTRIENODE
 *//*-----------------------------------------------------------------------*/

#define WORD_MINTTRIENODE_INIT(word, mask, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_MINTTRIENODE); \
    WORD_INTTRIENODE_MASK(word) = (mask); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Integer Trie Node Accessors                                                  *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_INTTRIENODE_MASK
 *  Get/set bitmask where only the critical bit is set.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_INTTRIENODE_INIT
 *  @see WORD_MINTTRIENODE_INIT
 *//*-----------------------------------------------------------------------*/

#define WORD_INTTRIENODE_MASK(word)     (((intptr_t *)(word))[1])
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup inttrienode_words \
Immutable Integer Trie Nodes
                                                                                        @ingroup predefined_words inttriemap_words minttrienode_words
  Integer trie nodes are @ref trienode_words nodes with integer-specific data.

  They are former @ref minttrienode_words that became immutable as a result of
  a copy (see #Col_CopyTrieMap): in this case the copy and the original
  would share most of their internals (rendered immutable in the process).
  Likewise, mutable operations on the copies would use copy-on-write
  semantics: immutable structures are copied to new mutable ones before
  modification.

  @see @ref minttrienode_words
  @see @ref inttriemap_words                                                    *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Immutable Integer Trie Node Creation                                         *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_INTTRIENODE_INIT
 *
 *  Mutable integer trie node word initializer.
 *
 *  @param word         Word to initialize.
 *  @param mask         #WORD_INTTRIENODE_MASK.
 *  @param left         #WORD_TRIENODE_LEFT.
 *  @param right        #WORD_TRIENODE_RIGHT.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_INTTRIENODE
 *//*-----------------------------------------------------------------------*/

#define WORD_INTTRIENODE_INIT(word, mask, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTTRIENODE); \
    WORD_INTTRIENODE_MASK(word) = (mask); \
    WORD_TRIENODE_LEFT(word) = (left); \
    WORD_TRIENODE_RIGHT(word) = (right);
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup mtrieleaf_words \
Trie Leaves
                                                                                        @ingroup predefined_words mapentry_words triemap_words
  Trie leaves are @ref mapentry_words with trie-specific data.

  @par Requirements
  - Trie leave words, like generic @ref mapentry_words, use one single cell.

  - They need no specific info besides generic map entry fields.

  @param Key        [Generic map entry key field] (@ref WORD_MAPENTRY_KEY).
  @param Value      [Generic map entry value field] (@ref WORD_MAPENTRY_VALUE).

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        trieleaf_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td sides="TR" colspan="2" bgcolor="grey75"></td>
            </tr>
            <tr><td sides="R">1</td>
                <td colspan="4" sides="LBR" width="320" bgcolor="grey75">Unused</td>
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
        0 | Type  |                                                       |
          +-------+                     Unused                            +
        1 |                                                               |
          +---------------------------------------------------------------+
        2 |                          Key (Generic)                        |
          +---------------------------------------------------------------+
        3 |                         Value (Generic)                       |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see @ref mapentry_words
  @see @ref triemap_words                                                       *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Trie Leaf Creation                                                           *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_MTRIELEAF_INIT
 *
 *  Mutable trie leaf word initializer.
 *
 *  @param word         Word to initialize.
 *  @param key          #WORD_MAPENTRY_KEY.
 *  @param value        #WORD_MAPENTRY_VALUE.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_MTRIELEAF
 *//*-----------------------------------------------------------------------*/

#define WORD_MTRIELEAF_INIT(word, key, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_MTRIELEAF); \
    WORD_MAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value);
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup trieleaf_words \
Immutable Trie Leaves
                                                                                        @ingroup predefined_words mapentry_words triemap_words mtrieleaf_words
  Immutable trie leaves are @ref mapentry_words with trie-specific data.

  They are former @ref mtrieleaf_words that became immutable as a result of
  a copy (see #Col_CopyTrieMap): in this case the copy and the original
  would share most of their internals (rendered immutable in the process).
  Likewise, mutable operations on the copies would use copy-on-write
  semantics: immutable structures are copied to new mutable ones before
  modification.

  @see @ref mapentry_words
  @see @ref mtrieleaf_words
  @see @ref triemap_words                                                       *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Immutable Trie Leaf Creation                                                 *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_TRIELEAF_INIT
 *
 *  Immutable trie leaf word initializer.
 *
 *  @param word         Word to initialize.
 *  @param key          #WORD_MAPENTRY_KEY.
 *  @param value        #WORD_MAPENTRY_VALUE.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_TRIELEAF
 *//*-----------------------------------------------------------------------*/

#define WORD_TRIELEAF_INIT(word, key, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_TRIELEAF); \
    WORD_MAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value);
                                                                               /*!     @} */
                                                                               /*!     @} */
/*
================================================================================*//*!   @addtogroup minttrieleaf_words \
Integer Trie Leaves
                                                                                        @ingroup predefined_words intmapentry_words inttriemap_words
  Trie leaves are @ref intmapentry_words with trie-specific data.

  @par Requirements
  - Trie leave words, like generic @ref intmapentry_words, use one single cell.

  - They need no specific info besides generic map entry fields.

  @param Key        [Generic integer map entry key field]
                    (@ref WORD_INTMAPENTRY_KEY).
  @param Value      [Generic map entry value field] (@ref WORD_MAPENTRY_VALUE).

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        trieleaf_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td sides="TR" colspan="2" bgcolor="grey75"></td>
            </tr>
            <tr><td sides="R">1</td>
                <td colspan="4" sides="LBR" width="320" bgcolor="grey75">Unused</td>
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
        0 | Type  |                                                       |
          +-------+                     Unused                            +
        1 |                                                               |
          +---------------------------------------------------------------+
        2 |                          Key (Generic)                        |
          +---------------------------------------------------------------+
        3 |                         Value (Generic)                       |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see @ref intmapentry_words
  @see @ref inttriemap_words                                                    *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Integer Trie Leaf Creation                                                   *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_MINTTRIELEAF_INIT
 *
 *  Mutable integer leaf word initializer.
 *
 *  @param word         Word to initialize.
 *  @param key          #WORD_INTMAPENTRY_KEY.
 *  @param value        #WORD_MAPENTRY_VALUE.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_MINTTRIELEAF
 *//*-----------------------------------------------------------------------*/

#define WORD_MINTTRIELEAF_INIT(word, key, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_MINTTRIELEAF); \
    WORD_INTMAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value);
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup inttrieleaf_words \
Immutable Integer Trie Leaves
                                                                                        @ingroup predefined_words intmapentry_words inttriemap_words minttrieleaf_words
  Immutable integer trie leaves are @ref intmapentry_words with trie-specific
  data.

  They are former @ref minttrieleaf_words that became immutable as a result of
  a copy (see #Col_CopyTrieMap): in this case the copy and the original
  would share most of their internals (rendered immutable in the process).
  Likewise, mutable operations on the copies would use copy-on-write
  semantics: immutable structures are copied to new mutable ones before
  modification.

  @see @ref intmapentry_words
  @see @ref minttrieleaf_words
  @see @ref inttriemap_words                                                    *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Immutable Integer Trie Leaf Creation                                         *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_INTTRIELEAF_INIT
 *
 *  Immutable integer trie leaf word initializer.
 *
 *  @param word         Word to initialize.
 *  @param key          #WORD_INTMAPENTRY_KEY.
 *  @param value        #WORD_MAPENTRY_VALUE.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_INTTRIELEAF
 *//*-----------------------------------------------------------------------*/

#define WORD_INTTRIELEAF_INIT(word, key, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTTRIELEAF); \
    WORD_INTMAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value);
                                                                                /*!     @} */
                                                                                /*!     @} */
#endif /* _COLIBRI_TRIE_INT */
                                                                                /*!     @endcond */