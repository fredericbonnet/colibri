/*                                                                              *//*!   @cond PRIVATE @file \
 * colRopeInt.h
 *
 *  This header file defines the rope internals of Colibri.
 *
 *  Ropes are a string datatype that allows for fast insertion, extraction
 *  and composition of strings. Internally they use self-balanced binary
 *  trees.
 *
 *  They are always immutable.
 *
 *  @see colRope.c
 *  @see colRope.h
 *
 *  @private
 */

#ifndef _COLIBRI_ROPE_INT
#define _COLIBRI_ROPE_INT


/*
================================================================================*//*!   @addtogroup ucsstr_words \
Fixed-Width UCS Strings
                                                                                        @ingroup predefined_words rope_words
  Fixed-width UCS strings are constant arrays of characters that are
  directly addressable.

  @par Requirements
  - UCS string words must know their character length as well as their
    character format (either #COL_UCS1, #COL_UCS2 or #COL_UCS4).

  - Byte length is deduced from character length and format.

  - Characters are stored within the word cells following the header.

  - UCS string words use as much cells as needed to store their characters.

  @param Format     Character format.
  @param Length     Size of character array.
  @param Data       Array of character codepoints.

  @par Cell Layout
    On all architectures the cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        ucsstr_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">31</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_UCSSTR_FORMAT" title="WORD_UCSSTR_FORMAT" colspan="2">Format</td>
                <td href="@ref WORD_UCSSTR_LENGTH" title="WORD_UCSSTR_LENGTH" colspan="2">Length</td>
            </tr>
            <tr><td sides="R">.</td>
                <td href="@ref WORD_UCSSTR_DATA" title="WORD_UCSSTR_DATA" colspan="6" rowspan="4">Character data</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0             7 8            15 16                           31
          +---------------+---------------+-------------------------------+
        0 |      Type     |    Format     |            Length             |
          +---------------+---------------+-------------------------------+
          .                                                               .
          .                         Character data                        .
        N |                                                               |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see WORD_TYPE_UCSSTR                                                         *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * UCS String Word Constants                                                    *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * UCSSTR_HEADER_SIZE
 *  Byte size of UCS string header.
 *//*-----------------------------------------------------------------------*/

#define UCSSTR_HEADER_SIZE              (sizeof(uint16_t)*2)

/*---------------------------------------------------------------------------   *//*!   @def \
 * UCSSTR_MAX_LENGTH
 *  Maximum char length of UCS strings.
 *//*-----------------------------------------------------------------------*/

#define UCSSTR_MAX_LENGTH               UINT16_MAX
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * UCS String Word Utilities                                                    *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * UCSSTR_SIZE
 *
 *  Get number of cells taken by an UCS string of the given byte length.
 *
 *  @param byteLength   Byte size of string.
 *
 *  @result
 *      Number of cells taken by word.
 *//*-----------------------------------------------------------------------*/

#define UCSSTR_SIZE(byteLength) \
    (NB_CELLS(UCSSTR_HEADER_SIZE+(byteLength)))
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * UCS String Word Creation                                                     *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_UCSSTR_INIT
 *
 *  UCS string word initializer.
 *
 *  @param word     Word to initialize.
 *  @param format   #WORD_UCSSTR_FORMAT.
 *  @param length   #WORD_UCSSTR_LENGTH.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_UCSSTR
 *//*-----------------------------------------------------------------------*/

#define WORD_UCSSTR_INIT(word, format, length) \
    WORD_SET_TYPEID((word), WORD_TYPE_UCSSTR); \
    WORD_UCSSTR_FORMAT(word) = (uint8_t) (format); \
    WORD_UCSSTR_LENGTH(word) = (uint16_t) (length);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * UCS String Word Accessors                                                    *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_UCSSTR_FORMAT
 *  Get/set character format.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_UCSSTR_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_UCSSTR_LENGTH
 *  Get/set character length.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_UCSSTR_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_UCSSTR_DATA
 *  Pointer to beginning of string data.
 *
 *  @param word     Word to access.
 *//*-----------------------------------------------------------------------*/

#define WORD_UCSSTR_FORMAT(word)        (((int8_t *)(word))[1])
#define WORD_UCSSTR_LENGTH(word)        (((uint16_t *)(word))[1])
#define WORD_UCSSTR_DATA(word)          ((const char *)(word)+UCSSTR_HEADER_SIZE)
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup utfstr_words \
Variable-Width UTF Strings
                                                                                        @ingroup predefined_words rope_words
  Variable-width UTF-8/16 strings are constant arrays of variable-width
  characters requiring dedicated encoding/decoding procedures for access.

  @par Requirements
  - UTF-8/16 string words must know both character and byte lengths, as well
    as their character format (either #COL_UTF8 or #COL_UTF16).

  - Characters are stored within the word cells following the header.

  - UTF-8/16 string words use as much cells as needed to store their
    characters, up to the page limit. Hence, number of cells is at most
    #AVAILABLE_CELLS. This is a tradeoff between space and performance: larger
    strings are split into several smaller chunks that get concatenated, but
    random access is better compared to flat UTF strings thanks to the binary
    tree indexation, as opposed to per-character linear scanning of UTF
    strings.

  @param Format         Character format.
  @param Length         Number of characters in array.
  @param Byte Length    Byte length of character array.
  @param Data           Array of character code units.

  @par Cell Layout
    On all architectures the cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        utfstr_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">31</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_UTFSTR_FORMAT" title="WORD_UTFSTR_FORMAT" colspan="2">Format</td>
                <td href="@ref WORD_UTFSTR_LENGTH" title="WORD_UTFSTR_LENGTH" colspan="2">Length</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_UTFSTR_BYTELENGTH" title="WORD_UTFSTR_BYTELENGTH" colspan="4">Byte Length</td>
                <td href="@ref WORD_UTFSTR_DATA" title="WORD_UTFSTR_DATA" sides="R" colspan="2"></td>
            </tr>
            <tr><td sides="R">.</td>
                <td href="@ref WORD_UTFSTR_DATA" title="WORD_UTFSTR_DATA" colspan="6" rowspan="3" sides="BR" width="320">Character data</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0             7 8            15 16                           31
          +---------------+---------------+-------------------------------+
        0 |      Type     |    Format     |            Length             |
          +---------------+---------------+-------------------------------+
        1 |          Byte Length          |                               |
          +-------------------------------+                               .
          .                                                               .
          .                         Character data                        .
        N |                                                               |
          +---------------------------------------------------------------+
   (end)
                                                                                        @endif
  @see WORD_TYPE_UTFSTR                                                         *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * UTF String Word Constants                                                    *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * UTFSTR_HEADER_SIZE
 *  Byte size of UTF-8/16 string header.
 *//*-----------------------------------------------------------------------*/

#define UTFSTR_HEADER_SIZE              (sizeof(uint16_t)*3)

/*---------------------------------------------------------------------------   *//*!   @def \
 * UTFSTR_MAX_BYTELENGTH
 *  Maximum byte length of UTF-8/16 strings.
 *//*-----------------------------------------------------------------------*/

#define UTFSTR_MAX_BYTELENGTH           (AVAILABLE_CELLS*CELL_SIZE-UTFSTR_HEADER_SIZE)
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * UTF String Word Utilities                                                    *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * UTFSTR_SIZE
 *
 *    Get number of cells taken by an UTF-8/16 string of the given byte length
 *
 *  @param byteLength   Byte size of string.
 *
 *  @result
 *      Number of cells taken by word.
 *//*-----------------------------------------------------------------------*/

#define UTFSTR_SIZE(byteLength) \
    (NB_CELLS(UTFSTR_HEADER_SIZE+(byteLength)))
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * UTF String Word Creation                                                     *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_UTFSTR_INIT
 *
 *  UTF-8/16 string word initializer.
 *
 *  @param word         Word to initialize.
 *  @param format       #WORD_UTFSTR_FORMAT.
 *  @param length       #WORD_UTFSTR_LENGTH.
 *  @param byteLength   #WORD_UTFSTR_BYTELENGTH.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_UTFSTR
 *//*-----------------------------------------------------------------------*/

#define WORD_UTFSTR_INIT(word, format, length, byteLength) \
    WORD_SET_TYPEID((word), WORD_TYPE_UTFSTR); \
    WORD_UTFSTR_FORMAT(word) = (uint8_t) (format); \
    WORD_UTFSTR_LENGTH(word) = (uint16_t) (length); \
    WORD_UTFSTR_BYTELENGTH(word) = (uint16_t) (byteLength);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * UTF String Word Accessors                                                    *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_UTFSTR_FORMAT
 *  Get/set character format.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_UTFSTR_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_UTFSTR_LENGTH
 *  Get/set character length.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_UTFSTR_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_UTFSTR_BYTELENGTH
 *  Get/set byte length.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_UTFSTR_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_UTFSTR_DATA
 *  Pointer to beginning of string data.
 *
 *  @param word     Word to access.
 *//*-----------------------------------------------------------------------*/

#define WORD_UTFSTR_FORMAT(word)        (((int8_t *)(word))[1])
#define WORD_UTFSTR_LENGTH(word)        (((uint16_t *)(word))[1])
#define WORD_UTFSTR_BYTELENGTH(word)    (((uint16_t *)(word))[2])
#define WORD_UTFSTR_DATA(word)          ((const char *)(word)+UTFSTR_HEADER_SIZE)
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup subrope_words \
Subropes
                                                                                        @ingroup predefined_words rope_words
  Subropes are immutable ropes pointing to a slice of a larger rope.

  Extraction of a subrope from a large rope can be expensive. For example,
  removing one character from a string of several thousands implies
  copying the whole data minus this element. Likewise, extracting a range
  of characters from a complex binary tree of rope nodes implies
  traversing and copying large parts of this subtree.

  To limit the memory and processing costs of rope slicing, the subrope
  word simply points to the original rope and the index range of
  characters in the slice. This representation is preferred over raw copy
  when the source rope is large.

  @par Requirements
  - Subrope words use one single cell.

  - Subropes must know their source rope word.

  - Subropes must also know the indices of the first and last characters in
    the source rope that belongs to the slice. These indices are inclusive.
    Difference plus one gives the subrope length.

  - Last, subropes must know their depth for tree balancing (see
    @ref rope_tree_balancing "Rope Tree Balancing"). This is the same as the 
    source depth, but to avoid one pointer dereference we cache the value in 
    the word.

  @param Depth      Depth of subrope. 8 bits will code up to 255 depth levels,
                    which is more than sufficient for balanced binary trees.
  @param Source     Rope of which this one is a subrope.
  @param First      First character in source.
  @param Last       Last character in source.

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        subrope_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_SUBROPE_DEPTH" title="WORD_SUBROPE_DEPTH" colspan="2">Depth</td>
                <td colspan="2" bgcolor="grey75">Unused</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_SUBROPE_SOURCE" title="WORD_SUBROPE_SOURCE" colspan="6">Source</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_SUBROPE_FIRST" title="WORD_SUBROPE_FIRST" colspan="6">First</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_SUBROPE_LAST" title="WORD_SUBROPE_LAST" colspan="6">Last</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0     7 8    15                                               n
          +-------+-------+-----------------------------------------------+
        0 | Type  | Depth |                    Unused                     |
          +-------+-------+-----------------------------------------------+
        1 |                            Source                             |
          +---------------------------------------------------------------+
        2 |                             First                             |
          +---------------------------------------------------------------+
        3 |                             Last                              |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see WORD_TYPE_SUBROPE                                                        *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Subrope Creation                                                             *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_SUBROPE_INIT
 *
 *  Subrope word initializer.
 *
 *  @param word     Word to initialize.
 *  @param depth    #WORD_SUBROPE_DEPTH.
 *  @param source   #WORD_SUBROPE_SOURCE.
 *  @param first    #WORD_SUBROPE_FIRST.
 *  @param last     #WORD_SUBROPE_LAST.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_SUBROPE
 *//*-----------------------------------------------------------------------*/

#define WORD_SUBROPE_INIT(word, depth, source, first, last) \
    WORD_SET_TYPEID((word), WORD_TYPE_SUBROPE); \
    WORD_SUBROPE_DEPTH(word) = (uint8_t) (depth); \
    WORD_SUBROPE_SOURCE(word) = (source); \
    WORD_SUBROPE_FIRST(word) = (first); \
    WORD_SUBROPE_LAST(word) = (last);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Subrope Accessors                                                            *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_SUBROPE_DEPTH
 *  Get/set depth of subrope.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_SUBROPE_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_SUBROPE_SOURCE
 *  Get/set rope from which this one is extracted.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_SUBROPE_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_SUBROPE_FIRST
 *  Get/set index of first character in source.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_SUBROPE_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_SUBROPE_LAST
 *  Get/set index of last character in source.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_SUBROPE_INIT
 *//*-----------------------------------------------------------------------*/

#define WORD_SUBROPE_DEPTH(word)        (((uint8_t *)(word))[1])
#define WORD_SUBROPE_SOURCE(word)       (((Col_Word *)(word))[1])
#define WORD_SUBROPE_FIRST(word)        (((size_t *)(word))[2])
#define WORD_SUBROPE_LAST(word)         (((size_t *)(word))[3])
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup concatrope_words \
Concat Ropes
                                                                                        @ingroup predefined_words rope_words
  Concat ropes are balanced binary trees concatenating left and right subropes. 
  The tree is balanced by construction (see @ref rope_tree_balancing
  "Rope Tree Balancing").

  @par Requirements
  - Concat rope words use one single cell.

  - Concat ropes must know their left and right subrope words.

  - Concat ropes must know their length, which is the sum of their left and
    right arms. To save full recursive subtree traversal this length is
    stored at each level in the concat words. We also cache the left arm's
    length whenever possible to save a pointer dereference (the right
    length being the total minus left lengths).

  - Last, concat ropes must know their depth for tree balancing (see
    @ref rope_tree_balancing "Rope Tree Balancing"). This is the max depth of 
    their left and right arms, plus one.

  @param Depth      Depth of concat rope. 8 bits will code up to 255 depth
                    levels, which is more than sufficient for balanced
                    binary trees.
  @param Left       Used as shortcut to avoid dereferencing the left arm.
                    Zero if actual length is too large to fit.
  @param Length     Rope length, which is the sum of both arms'.
  @param Left       Left concatenated rope.
  @param Right      Right concatenated rope.

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        concatrope_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_CONCATROPE_DEPTH" title="WORD_CONCATROPE_DEPTH" colspan="2">Depth</td>
                <td href="@ref WORD_CONCATROPE_LEFT_LENGTH" title="WORD_CONCATROPE_LEFT_LENGTH" colspan="2">Left length</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_CONCATROPE_LENGTH" title="WORD_CONCATROPE_LENGTH" colspan="7">Length</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_CONCATROPE_LEFT" title="WORD_CONCATROPE_LEFT" colspan="7">Left</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_CONCATROPE_RIGHT" title="WORD_CONCATROPE_RIGHT" colspan="7">Right</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0     7 8    15 16           31                               n
          +-------+-------+---------------+-------------------------------+
        0 | Type  | Depth |  Left length  |        Unused (n > 32)        |
          +-------+-------+---------------+-------------------------------+
        1 |                            Length                             |
          +---------------------------------------------------------------+
        2 |                             Left                              |
          +---------------------------------------------------------------+
        3 |                             Right                             |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see WORD_TYPE_CONCATROPE                                                     *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Concat Rope Creation                                                         *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_CONCATROPE_INIT
 *
 *  Concat rope word initializer.
 *
 *  @param word         Word to initialize.
 *  @param depth        #WORD_CONCATROPE_DEPTH.
 *  @param length       #WORD_CONCATROPE_LENGTH.
 *  @param leftLength   #WORD_CONCATROPE_LEFT_LENGTH.
 *  @param left         #WORD_CONCATROPE_LEFT.
 *  @param right        #WORD_CONCATROPE_RIGHT.
 *
 *  @warning
 *      Arguments **word** and **leftLength** are referenced several times by
 *      the macro. Make sure to avoid any side effect.
 *
 *  @see WORD_TYPE_CONCATROPE
 *//*-----------------------------------------------------------------------*/

#define WORD_CONCATROPE_INIT(word, depth, length, leftLength, left, right) \
    WORD_SET_TYPEID((word), WORD_TYPE_CONCATROPE); \
    WORD_CONCATROPE_DEPTH(word) = (uint8_t) (depth); \
    WORD_CONCATROPE_LENGTH(word) = (length); \
    WORD_CONCATROPE_LEFT_LENGTH(word) = (uint16_t) ((leftLength)>UINT16_MAX?0:(leftLength)); \
    WORD_CONCATROPE_LEFT(word) = (left); \
    WORD_CONCATROPE_RIGHT(word) = (right);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Concat Rope Accessors                                                        *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_CONCATROPE_DEPTH
 *  Get/set depth of concat rope.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_CONCATROPE_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_CONCATROPE_LEFT_LENGTH
 *  Get/set left arm's length (zero when too large).
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_CONCATROPE_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_CONCATROPE_LENGTH
 *  Get/set rope length.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_CONCATROPE_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_CONCATROPE_LEFT
 *  Get/set left concatenated rope.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_CONCATROPE_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_CONCATROPE_RIGHT
 *  Get/set right concatenated rope.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_CONCATROPE_INIT
 *//*-----------------------------------------------------------------------*/

#define WORD_CONCATROPE_DEPTH(word)         (((uint8_t *)(word))[1])
#define WORD_CONCATROPE_LEFT_LENGTH(word)   (((uint16_t *)(word))[1])
#define WORD_CONCATROPE_LENGTH(word)        (((size_t *)(word))[1])
#define WORD_CONCATROPE_LEFT(word)          (((Col_Word *)(word))[2])
#define WORD_CONCATROPE_RIGHT(word)         (((Col_Word *)(word))[3])
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup rope_words \
Ropes                                                                           *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Rope Exceptions                                                              *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * TYPECHECK_CHAR
 *                                                                                      @hideinitializer
 *  Type checking macro for characters.
 *
 *  @param word     Checked word.
 *
 *  @typecheck{COL_ERROR_CHAR,word}
 *//*-----------------------------------------------------------------------*/

#define TYPECHECK_CHAR(word) \
    TYPECHECK((Col_WordType(word) & COL_CHAR), COL_ERROR_CHAR, (word))

/*---------------------------------------------------------------------------   *//*!   @def \
 * TYPECHECK_STRING
 *                                                                                      @hideinitializer
 *  Type checking macro for strings.
 *
 *  @param word     Checked word.
 *
 *  @typecheck{COL_ERROR_STRING,word}
 *//*-----------------------------------------------------------------------*/

#define TYPECHECK_STRING(word) \
    TYPECHECK((Col_WordType(word) & COL_STRING), COL_ERROR_STRING, (word))

/*---------------------------------------------------------------------------   *//*!   @def \
 * TYPECHECK_ROPE
 *                                                                                      @hideinitializer
 *  Type checking macro for ropes.
 *
 *  @param word     Checked word.
 *
 *  @typecheck{COL_ERROR_ROPE,word}
 *//*-----------------------------------------------------------------------*/

#define TYPECHECK_ROPE(word) \
    TYPECHECK((Col_WordType(word) & COL_ROPE), COL_ERROR_ROPE, (word))

/*---------------------------------------------------------------------------   *//*!   @def \
 * VALUECHECK_ROPELENGTH_CONCAT
 *                                                                                      @hideinitializer
 *  Value checking macro for ropes, ensures that combined lengths of two
 *  concatenated ropes don't exceed the maximum value.
 *
 *  @param length1, length2     Checked lengths.
 *
 *  @valuecheck{COL_ERROR_ROPELENGTH_CONCAT,length1+length2}
 *//*-----------------------------------------------------------------------*/

#define VALUECHECK_ROPELENGTH_CONCAT(length1, length2) \
    VALUECHECK((SIZE_MAX-(length1) >= (length2)), COL_ERROR_ROPELENGTH_CONCAT, \
                (length1), (length2), SIZE_MAX)

/*---------------------------------------------------------------------------   *//*!   @def \
 * VALUECHECK_ROPELENGTH_REPEAT
 *                                                                                      @hideinitializer
 *  Value checking macro for ropes, ensures that length of a repeated rope
 *  doesn't exceed the maximum value.
 *
 *  @param length, count    Checked length and repetition factor.
 *
 *  @valuecheck{COL_ERROR_ROPELENGTH_CONCAT,length*count}
 *//*-----------------------------------------------------------------------*/

#define VALUECHECK_ROPELENGTH_REPEAT(length, count) \
    VALUECHECK(((count) <= 1 || SIZE_MAX/(count) >= (length)), \
                COL_ERROR_ROPELENGTH_REPEAT, (length), (count), SIZE_MAX)
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Rope Iterator Exceptions                                                     *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * TYPECHECK_ROPEITER
 *                                                                                      @hideinitializer
 *  Type checking macro for rope iterators.
 *
 *  @param it   Checked iterator.
 *
 *  @typecheck{COL_ERROR_ROPEITER,it}
 *
 *  @see Col_RopeIterNull
 *//*-----------------------------------------------------------------------*/

#define TYPECHECK_ROPEITER(it) \
    TYPECHECK(!Col_RopeIterNull(it), COL_ERROR_ROPEITER, (it))

/*---------------------------------------------------------------------------   *//*!   @def \
 * VALUECHECK_ROPEITER
 *                                                                                      @hideinitializer
 *  Value checking macro for rope iterators, ensures that iterator is not
 *  at end.
 *
 *  @param it   Checked iterator.
 *
 *  @valuecheck{COL_ERROR_ROPEITER_END,it}
 *
 *  @see Col_RopeIterEnd
 *//*-----------------------------------------------------------------------*/

#define VALUECHECK_ROPEITER(it) \
    VALUECHECK(!Col_RopeIterEnd(it), COL_ERROR_ROPEITER_END, (it))
                                                                                /*!     @} */
                                                                                /*!     @} */
#endif /* _COLIBRI_ROPE_INT */
                                                                                /*!     @endcond */