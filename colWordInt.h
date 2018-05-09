/*                                                                              *//*!   @cond PRIVATE @file \
 * colWordInt.h
 *
 *  This header file defines the word internals of Colibri.
 *
 *  @see colWord.c
 *  @see colWord.h
 *
 *  @private
 */

#ifndef _COLIBRI_WORD_INT
#define _COLIBRI_WORD_INT


/*
================================================================================*//*!   @addtogroup words \
Words                                                                           *//*!   @{ *//*
================================================================================
*/
                                                                                /*!     @anchor word_type_ids */
/********************************************************************************//*!   @name \
 * Word Type Identifiers
 *
 *  Internal word type identifiers.
 *
 *  Predefined type IDs for regular word types are chosen so that their bit
 *  0 is cleared, their bit 1 is set, and their value fit into a byte. This
 *  gives up to 64 predefined type IDs (2-254 with steps of 4).
 *
 *  Immediate word type IDs use negative values to avoid clashes with
 *  regular word type IDs.                                                      *//*!   @{ *//*
 ******************************************************************************/

#define WORD_TYPE_NIL             0 /*!< Nil singleton. */

#define WORD_TYPE_CUSTOM         -1 /*!< @ref custom_words. */

#define WORD_TYPE_SMALLINT       -2 /*!< @ref smallint_words. */
#define WORD_TYPE_SMALLFP        -3 /*!< @ref smallfp_words. */
#define WORD_TYPE_CHARBOOL       -4 /*!< @ref char_words *or* @ref bool_words. */
#define WORD_TYPE_SMALLSTR       -5 /*!< @ref smallstr_words. */
#define WORD_TYPE_CIRCLIST       -6 /*!< @ref circlist_words. */
#define WORD_TYPE_VOIDLIST       -7 /*!< @ref voidlist_words. */

#define WORD_TYPE_WRAP            2 /*!< @ref wrap_words. */

#define WORD_TYPE_UCSSTR          6 /*!< @ref ucsstr_words. */
#define WORD_TYPE_UTFSTR         10 /*!< @ref utfstr_words. */
#define WORD_TYPE_SUBROPE        14 /*!< @ref subrope_words. */
#define WORD_TYPE_CONCATROPE     18 /*!< @ref concatrope_words. */

#define WORD_TYPE_VECTOR         22 /*!< @ref vector_words. */
#define WORD_TYPE_MVECTOR        26 /*!< @ref mvector_words. */
#define WORD_TYPE_SUBLIST        30 /*!< @ref sublist_words. */
#define WORD_TYPE_CONCATLIST     34 /*!< @ref concatlist_words. */
#define WORD_TYPE_MCONCATLIST    38 /*!< @ref mconcatlist_words. */

#define WORD_TYPE_STRHASHMAP     42 /*!< @ref strhashmap_words. */
#define WORD_TYPE_INTHASHMAP     46 /*!< @ref inthashmap_words. */
#define WORD_TYPE_HASHENTRY      50 /*!< @ref hashentry_words. */
#define WORD_TYPE_MHASHENTRY     54 /*!< @ref mhashentry_words. */
#define WORD_TYPE_INTHASHENTRY   58 /*!< @ref inthashentry_words. */
#define WORD_TYPE_MINTHASHENTRY  62 /*!< @ref minthashentry_words. */

#define WORD_TYPE_STRTRIEMAP     66 /*!< @ref strtriemap_words. */
#define WORD_TYPE_INTTRIEMAP     70 /*!< @ref inttriemap_words. */
#define WORD_TYPE_TRIENODE       74 /*!< @ref trienode_words. */
#define WORD_TYPE_MTRIENODE      78 /*!< @ref mtrienode_words. */
#define WORD_TYPE_STRTRIENODE    82 /*!< @ref strtrienode_words. */
#define WORD_TYPE_MSTRTRIENODE   86 /*!< @ref mstrtrienode_words. */
#define WORD_TYPE_INTTRIENODE    90 /*!< @ref inttrienode_words. */
#define WORD_TYPE_MINTTRIENODE   94 /*!< @ref minttrienode_words. */
#define WORD_TYPE_TRIELEAF       98 /*!< @ref trieleaf_words. */
#define WORD_TYPE_MTRIELEAF     102 /*!< @ref mtrieleaf_words. */
#define WORD_TYPE_INTTRIELEAF   106 /*!< @ref inttrieleaf_words. */
#define WORD_TYPE_MINTTRIELEAF  110 /*!< @ref minttrieleaf_words. */

#define WORD_TYPE_STRBUF        114 /*!< @ref strbuf_words. */

#ifdef PROMOTE_COMPACT
#   define WORD_TYPE_REDIRECT   254 /*!< @ref redirect_words. */
#endif

#define WORD_TYPE_UNKNOWN           /*!< Used as a tag in the source code to
                                         mark places where predefined type
                                         specific code is needed. Search for
                                         this tag when adding new predefined
                                         word types. */
                                                                                /*!     @} */

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_TYPE
 *
 *  Get word type identifier.
 *
 *  @param word     Word to get type for.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @result
 *      Word type identifier.
 *
 *  @see @ref word_type_ids "Word Type Identifiers"
 *  @see immediateWordTypes
 *  @see @ref immediate_words
 *  @see @ref regular_words
 *//*-----------------------------------------------------------------------*/

/* Careful: don't give arguments with side effects! */
#define WORD_TYPE(word) \
    /* Nil? */                                                  \
    ((!(word))?                         WORD_TYPE_NIL           \
    /* Immediate Word? */                                       \
    :(((uintptr_t)(word))&15)?          immediateWordTypes[(((uintptr_t)(word))&31)] \
    /* Predefined Type ID? */                                   \
    :((((uint8_t *)(word))[0])&2)?      WORD_TYPEID(word)       \
    /* Custom Type */                                           \
    :                                   WORD_TYPE_CUSTOM)
                                                                                /*!     @} */

/*
================================================================================*//*!   @addtogroup immediate_words \
Immediate Words
                                                                                        @ingroup words
  Immediate words are immutable datatypes that store their value directly
  in the word identifier, contrary to @ref regular_words whose identifier is a
  pointer to a cell-based structure. As cells are aligned on a multiple of
  their size (16 bytes on 32-bit systems), this means that a number of low
  bits are always zero in regular word identifiers (4 bits on 32-bit
  systems with 16-byte cells). Immediate values are distinguished from
  regular pointers by setting one of these bits. Immediate word types are
  identified by these bit patterns, called tags.                                *//*!   @{ *//*
================================================================================
*/

/*---------------------------------------------------------------------------
 * immediateWordTypes
 *                                                                              *//*!
 *  Lookup table for immediate word types, gives the word type ID from the
 *  first 5 bits of the word value (= the tag).
 *
 *  @note
 *      Defined as static so that all modules use their own instance for
 *      potential compiler optimizations.
 *
 *  @see WORD_TYPE
 *//*-----------------------------------------------------------------------*/

static const int immediateWordTypes[32] = {
    WORD_TYPE_NIL,      /* 00000 */
    WORD_TYPE_SMALLINT, /* 00001 */
    WORD_TYPE_SMALLFP,  /* 00010 */
    WORD_TYPE_SMALLINT, /* 00011 */
    WORD_TYPE_CHARBOOL, /* 00100 */
    WORD_TYPE_SMALLINT, /* 00101 */
    WORD_TYPE_SMALLFP,  /* 00110 */
    WORD_TYPE_SMALLINT, /* 00111 */
    WORD_TYPE_CIRCLIST, /* 01000 */
    WORD_TYPE_SMALLINT, /* 01001 */
    WORD_TYPE_SMALLFP,  /* 01010 */
    WORD_TYPE_SMALLINT, /* 01011 */
    WORD_TYPE_SMALLSTR, /* 01100 */
    WORD_TYPE_SMALLINT, /* 01101 */
    WORD_TYPE_SMALLFP,  /* 01110 */
    WORD_TYPE_SMALLINT, /* 01111 */
    WORD_TYPE_NIL,      /* 10000 */
    WORD_TYPE_SMALLINT, /* 10001 */
    WORD_TYPE_SMALLFP,  /* 10010 */
    WORD_TYPE_SMALLINT, /* 10011 */
    WORD_TYPE_VOIDLIST, /* 10100 */
    WORD_TYPE_SMALLINT, /* 10101 */
    WORD_TYPE_SMALLFP,  /* 10110 */
    WORD_TYPE_SMALLINT, /* 10111 */
    WORD_TYPE_CIRCLIST, /* 11000 */
    WORD_TYPE_SMALLINT, /* 11001 */
    WORD_TYPE_SMALLFP,  /* 11010 */
    WORD_TYPE_SMALLINT, /* 11011 */
    WORD_TYPE_CIRCLIST, /* 11100 */
    WORD_TYPE_SMALLINT, /* 11101 */
    WORD_TYPE_SMALLFP,  /* 11110 */
    WORD_TYPE_SMALLINT, /* 11111 */
};
                                                                                /*!     @} */

/*
================================================================================*//*!   @addtogroup smallint_words \
Small Integer Words
                                                                                        @ingroup immediate_words
  Small integer words are @ref immediate_words storing integers whose width
  is one bit less than machine integers.

  Larger integers are stored in @ref int_wrappers.

  @par Requirements
  - Small integer words need to store the integer value in the word
    identifier.

  @param Value      Integer value of word.

  @par Word Layout
    On all architectures the *n*-bit word layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        smalllint_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="10">0</td>
                <td sides="B" width="150" align="left">1</td><td sides="B" width="155" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td>1</td>
                <td href="@ref WORD_SMALLINT_VALUE" title="WORD_SMALLINT_VALUE" colspan="2">Value</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0 1                                                           n
          +-+-------------------------------------------------------------+
        0 |1|                            Value                            |
          +-+-------------------------------------------------------------+
                                                                                        @endif
  @see WORD_TYPE_SMALLINT
  @see immediateWordTypes                                                       *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Small Integer Word Constants                                                 *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * SMALLINT_MIN
 *  Minimum value of small integer words.
 *                                                                              *//*!   @def \
 * SMALLINT_MAX
 *  Maximum value of small integer words.
 *//*-----------------------------------------------------------------------*/

#define SMALLINT_MIN            (INTPTR_MIN>>1)
#define SMALLINT_MAX            (INTPTR_MAX>>1)
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Small Integer Word Creation                                                  *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_SMALLINT_NEW
 *
 *  Small integer word creation.
 *
 *  @param value    Integer value. Must be between #SMALLINT_MIN and
 *                  #SMALLINT_MAX.
 *
 *  @result
 *      The new small integer word.
 *
 *  @see WORD_SMALLINT_VALUE
 *//*-----------------------------------------------------------------------*/

#define WORD_SMALLINT_NEW(value) \
    ((Col_Word)((((intptr_t)(value))<<1)|1))
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Small Integer Word Accessors                                                 *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_SMALLINT_VALUE
 *  Get value of small integer word.
 *
 *  @param word     Word to access.
 *
 *  @see WORD_SMALLINT_NEW
 *//*-----------------------------------------------------------------------*/

#define WORD_SMALLINT_VALUE(word)   (((intptr_t)(word))>>1)
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup smallfp_words \
Small Floating Point Words
                                                                                        @ingroup immediate_words
  Small floating point words are @ref immediate_words storing floating points
  whose lower 2 bits of the mantissa are zero, so that they are free for the
  tag bits. This includes IEEE 754 special values such as +/-0, +/-INF and NaN.

  Other values are stored in @ref fp_wrappers.

  @par Requirements
  - Small floating point words need to store the floating point value in the
    word identifier.

  @param Value      Floating point value of word.

  @par Word Layout
    On all architectures the *n*-bit word layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        smallfp_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="20">01</td>
                <td sides="B" width="147" align="left">2</td><td sides="B" width="150" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td>01</td>
                <td href="@ref WORD_SMALLFP_VALUE" title="WORD_SMALLFP_VALUE" colspan="2">Value</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           01 2                                                          n
          +--+------------------------------------------------------------+
        0 |01|                           Value                            |
          +--+------------------------------------------------------------+
                                                                                        @endif
  @see WORD_TYPE_SMALLFP
  @see immediateWordTypes                                                       *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Small Floating Point Word Utilities                                          *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * SMALLFP_TYPE
 *
 *  C Type used by immediate floating point words.
 *
 *  @see FloatConvert
 *  @see WORD_SMALLFP_NEW
 *//*-----------------------------------------------------------------------*/

#if SIZE_BIT == 32
#   define SMALLFP_TYPE         float
#elif SIZE_BIT == 64
#   define SMALLFP_TYPE         double
#endif

/*---------------------------------------------------------------------------
 * FloatConvert
 *                                                                              *//*!
 *  Utility union type for immediate floating point words. Because of C
 *  language restrictions (bitwise operations on floating points are
 *  forbidden), we have to use this intermediary union type for conversions.
 *
 *  @see WORD_SMALLFP_VALUE
 *  @see WORD_SMALLFP_NEW
 *//*-----------------------------------------------------------------------*/

typedef union {
    Col_Word w;     /*!< Word value.  */
    uintptr_t i;    /*!< Integer value used for bitwise operations. */
    SMALLFP_TYPE f; /*!< Floating point value.*/
} FloatConvert;
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Small Floating Point Word Creation                                           *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_SMALLFP_NEW
 *
 *  Small floating point word creation.
 *
 *  @param value    Floating point value.
 *  @param c        #FloatConvert conversion structure.
 *
 *  @warning
 *      Argument **c** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @result
 *      The new small floating point word.
 *
 *  @see WORD_SMALLFP_VALUE
 *//*-----------------------------------------------------------------------*/

#define WORD_SMALLFP_NEW(value, c) \
    ((c).f = (SMALLFP_TYPE)(value), (c).i &= ~3, (c).i |= 2, (c).w)
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Small Floating Point Word Accessors                                          *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_SMALLFP_VALUE
 *  Get value of small floating point word.
 *
 *  @param word     Word to access.
 *  @param c        #FloatConvert conversion structure.
 *
 *  @result
 *      The floating point value.
 *
 *  @see WORD_SMALLFP_NEW
 *//*-----------------------------------------------------------------------*/

#define WORD_SMALLFP_VALUE(word, c) \
    ((c).w = (word), (c).i &= ~3, (c).f)
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup bool_words \
Boolean Words
                                                                                        @ingroup immediate_words
  Boolean words are @ref immediate_words taking only two values: true or false.

  @par Requirements
  - Boolean words need only one bit to distinguish between true and false.

  @param Value(V)       Zero for false, nonzero for true.

  @par Word Layout
    On all architectures the *n*-bit word layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        bool_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="10">8</td>
                <td sides="B" width="110" align="left">9</td><td sides="B" width="115" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td colspan="2">00100000</td>
                <td href="@ref WORD_BOOL_VALUE" title="WORD_BOOL_VALUE">V</td>
                <td colspan="2" bgcolor="grey75">Unused</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0      7 8 9                                                  n
          +--------+-+----------------------------------------------------+
        0 |00100000|V|                       Unused                       |
          +--------+-+----------------------------------------------------+
                                                                                        @endif
  @see WORD_TYPE_CHARBOOL
  @see immediateWordTypes                                                       *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Boolean Word Creation                                                        *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_BOOL_NEW
 *
 *  Boolean word creation.
 *
 *  @param value    Boolean value: zero for false, nonzero for true.
 *
 *  @result
 *      The new boolean ord.
 *
 *  @see WORD_BOOL_VALUE
 *//*-----------------------------------------------------------------------*/

#define WORD_BOOL_NEW(value) \
    ((value)?WORD_TRUE:WORD_FALSE)
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Boolean Word Accessors                                                       *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_BOOL_VALUE
 *  Get value of boolean word: zero for false, nonzero for true.
 *
 *  @param word     Word to access.
 *
 *  @see WORD_BOOL_NEW
 *//*-----------------------------------------------------------------------*/

#define WORD_BOOL_VALUE(word)   (((uintptr_t)(word))&0x100)
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup char_words \
Character Words
                                                                                        @ingroup immediate_words rope_words
  Character words are @ref immediate_words storing one Unicode character
  codepoint.

  @par Requirements
  - Character words need to store Unicode character codepoints up to
    #COL_CHAR_MAX, i.e. at least 21 bits.

  - Character words also need to know the character format for string
    normalization issues. For that the codepoint width is sufficient
    (between 1 and 4, i.e. 3 bits).

  - As character and boolean words share the same tag, we distinguish
    both types with boolean words having a zero width field.

  @param Codepoint    Unicode codepoint of character word.
  @param Width(W)     Character width set at creation time.

  @par Word Layout
    On all architectures the *n*-bit word layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        char_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="25" align="left">0</td><td sides="B" width="25" align="right">4</td>
                <td sides="B" width="15" align="left">5</td><td sides="B" width="15" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td colspan="2">00100</td>
                <td href="@ref WORD_CHAR_WIDTH" title="WORD_CHAR_WIDTH" colspan="2">W</td>
                <td href="@ref WORD_CHAR_CP" title="WORD_CHAR_CP" colspan="2">Codepoint</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0   4 5 7 8                  31                             n
          +-----+---+---------------------+-------------------------------+
        0 |00100| W |       Codepoint     |        Unused (n > 32)        |
          +-----+---+ --------------------+-------------------------------+
                                                                                        @endif
  @see WORD_TYPE_CHARBOOL
  @see immediateWordTypes                                                       *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Character Word Creation                                                      *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_CHAR_NEW
 *
 *  Unicode char word creation.
 *
 *  @param value        Unicode codepoint.
 *  @param width        Character width.
 *
 *  @result
 *      The new character word.
 *
 *  @see WORD_CHAR_CP
 *  @see WORD_CHAR_WIDTH
 *//*-----------------------------------------------------------------------*/

#define WORD_CHAR_NEW(value, width) \
    ((Col_Word)((((uintptr_t)(value))<<8)|(((uintptr_t)(width))<<5)|4))
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Character Word Accessors                                                     *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_CHAR_CP
 *  Get Unicode codepoint of char word.
 *
 *  @param word     Word to access.
 *
 *  @see WORD_CHAR_NEW
 *
 *                                                                              *//*!   @def \
 * WORD_CHAR_WIDTH
 *  Get char width used at creation time.
 *
 *  @param word     Word to access.
 *
 *  @see WORD_CHAR_NEW
 *//*-----------------------------------------------------------------------*/

#define WORD_CHAR_CP(word)      ((Col_Char)(((uintptr_t)(word))>>8))
#define WORD_CHAR_WIDTH(word)   ((((uintptr_t)(word))>>5)&7)
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup smallstr_words \
Small String Words
                                                                                        @ingroup immediate_words rope_words
  Small string words are @ref immediate_words storing short 8-bit strings.
  Maximum capacity is the machine word size minus one (i.e. 3 on 32-bit,
  7 on 64-bit). Larger strings are cell-based.

  @par Requirements
  - Small string words need to store the characters and the length.

  @param Length(L)  Number of characters [0,7].
  @param Data       Character data.

  @par Word Layout
    On all architectures the *n*-bit word layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        smallstr_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="25" align="left">0</td><td sides="B" width="25" align="right">4</td>
                <td sides="B" width="15" align="left">5</td><td sides="B" width="15" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td colspan="2">00110</td>
                <td href="@ref WORD_SMALLSTR_LENGTH" title="WORD_SMALLSTR_LENGTH" colspan="2">L</td>
                <td href="@ref WORD_SMALLSTR_DATA" title="WORD_SMALLSTR_DATA" colspan="2">Data</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0   4 5 7 8                                                  n
          +-----+---+-----------------------------------------------------+
        0 |00110| L |                        Data                         |
          +-----+---+-----------------------------------------------------+
                                                                                        @endif
  @see WORD_TYPE_SMALLSTR
  @see immediateWordTypes                                                       *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Small String Word Constants                                                  *//*!   @{ *//*
 ******************************************************************************/

 /*---------------------------------------------------------------------------   *//*!   @def \
 * SMALLSTR_MAX_LENGTH
 *  Maximum length of small string words.
 *//*-----------------------------------------------------------------------*/

#define SMALLSTR_MAX_LENGTH     (sizeof(Col_Word)-1)

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_SMALLSTR_EMPTY
 *
 *  Empty string immediate singleton. This is simply a small string of zero
 *  length.
 *//*-----------------------------------------------------------------------*/

#define WORD_SMALLSTR_EMPTY     ((Col_Word) 12)
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Small String Word Accessors                                                  *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_SMALLSTR_LENGTH
 *  Get small string length.
 *
 *  @param word     Word to access.
 *
 *  @see WORD_SMALLSTR_SET_LENGTH
 *
 *                                                                              *//*!   @def \
 * WORD_SMALLSTR_SET_LENGTH
 *  Set small string length.
 *
 *  @param word     Word to access.
 *  @param length   Length value.
 *
 *  @see WORD_SMALLSTR_LENGTH
 *
 *                                                                              *//*!   @def \
 * WORD_SMALLSTR_DATA
 *  Pointer to beginning of small string data.
 *
 *  @param word     Word to access.
 *//*-----------------------------------------------------------------------*/

#define WORD_SMALLSTR_LENGTH(word)      ((((uintptr_t)(word))&0xFF)>>5)
#define WORD_SMALLSTR_SET_LENGTH(word, length) (*((uintptr_t *)&(word)) = ((length)<<5)|12)
#ifdef COL_BIGENDIAN
#   define WORD_SMALLSTR_DATA(word)     ((Col_Char1  *)&(word))
#else
#   define WORD_SMALLSTR_DATA(word)     (((Col_Char1 *)&(word))+1)
#endif
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup circlist_words \
Circular List Words
                                                                                        @ingroup immediate_words
  Circular lists words are @ref immediate_words made of a core list that
  repeats infinitely. Core lists can be either regular @ref list_words or
  @ref voidlist_words.

  @par Requirements
  - Circular list words need to store their core list word.

  @param Core       Core list word.

  @par Word Layout
    On all architectures the *n*-bit word layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        circlist_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="20" align="left">0</td><td sides="B" width="20" align="right">3</td>
                <td sides="B" width="140" align="left">4</td><td sides="B" width="140" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td colspan="2">0001</td>
                <td href="@ref WORD_CIRCLIST_CORE" title="WORD_CIRCLIST_CORE" colspan="2">Core (regular list)</td>
            </tr>
            </table>
        >]
    }
    @enddot
    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        circvoidlist_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="25" align="left">0</td><td sides="B" width="25" align="right">4</td>
                <td sides="B" width="135" align="left">5</td><td sides="B" width="135" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td colspan="2">00111</td>
                <td href="@ref WORD_VOIDLIST_LENGTH" title="WORD_VOIDLIST_LENGTH" colspan="2">Core length (void list)</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0  3 4                                                        n
          +----+----------------------------------------------------------+
        0 |0001|                    Core (regular list)                   |
          +----+----------------------------------------------------------+

           0   4 5                                                       n
          +-----+---------------------------------------------------------+
        0 |00111|                Core length (void list)                  |
          +-----+---------------------------------------------------------+
                                                                                        @endif
  @see WORD_TYPE_CIRCLIST
  @see @ref list_words
  @see @ref voidlist_words
  @see immediateWordTypes                                                       *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Core List Word Creation                                                      *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_CIRCLIST_NEW
 *
 *  Circular list word creation.
 *
 *  @param core     Core list. Must be acyclic.
 *
 *  @result
 *      The new circular list word.
 *
 *  @see WORD_CIRCLIST_CORE
 *//*-----------------------------------------------------------------------*/

#define WORD_CIRCLIST_NEW(core) \
    ((Col_Word)(((uintptr_t)(core))|8))
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Core List Word Accessors                                                     *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_CIRCLIST_CORE
 *  Get core list.
 *
 *  @param word     Word to access.
 *
 *  @see WORD_CIRCLIST_NEW
 *//*-----------------------------------------------------------------------*/

#define WORD_CIRCLIST_CORE(word)        ((Col_Word)(((uintptr_t)(word))&~8))
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup voidlist_words \
Void List Words
                                                                                        @ingroup immediate_words
  Void list words are @ref immediate_words representing lists whose elements
  are all nil.

  @par Requirements
  - Void list words need to know their length. Length width is the machine
    word width minus 4 bits, so the maximum length is about 1/16th of the
    theoretical maximum. Larger void lists are built by concatenating
    several shorter immediate void lists.

  - Void list tag bit comes after the circular list tag so that void lists
    can be made circular. Void circular lists thus combine both tag bits.

  @param Length     List length.

  @par Word Layout
    On all architectures the *n*-bit word layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        voidlist_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="25" align="left">0</td><td sides="B" width="25" align="right">4</td>
                <td sides="B" width="135" align="left">5</td><td sides="B" width="135" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td colspan="2">00101</td>
                <td href="@ref WORD_VOIDLIST_LENGTH" title="WORD_VOIDLIST_LENGTH" colspan="2">Length</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0   4 5                                                       n
          +-----+---------------------------------------------------------+
        0 |00101|                         Length                          |
          +-----+---------------------------------------------------------+
                                                                                        @endif
  @see WORD_TYPE_VOIDLIST
  @see immediateWordTypes                                                       *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Void List Word Constants                                                     *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * VOIDLIST_MAX_LENGTH
 *  Maximum length of void list words.
 *//*-----------------------------------------------------------------------*/

#define VOIDLIST_MAX_LENGTH     (SIZE_MAX>>5)

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_LIST_EMPTY
 *
 *  Empty list immediate singleton. This is simply a void list of zero
 *  length.
 *//*-----------------------------------------------------------------------*/

#define WORD_LIST_EMPTY         WORD_VOIDLIST_NEW(0)
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Void List Word Creation                                                      *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_VOIDLIST_NEW
 *
 *  Void list word creation.
 *
 *  @param length       Void list length. Must be less than
 *                      #VOIDLIST_MAX_LENGTH.
 *
 *  @result
 *      The new void list word.
 *
 *  @see WORD_VOIDLIST_LENGTH
 *//*-----------------------------------------------------------------------*/

#define WORD_VOIDLIST_NEW(length) \
    ((Col_Word)(intptr_t)((((size_t)(length))<<5)|20))
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Void List Word Accessors                                                     *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_VOIDLIST_LENGTH
 *  Get void list length.
 *
 *  @param word     Word to access.
 *
 *  @see WORD_VOIDLIST_NEW
 *//*-----------------------------------------------------------------------*/

#define WORD_VOIDLIST_LENGTH(word)      (((size_t)(intptr_t)(word))>>5)
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup regular_words \
Regular Words
                                                                                        @ingroup words
  Regular words are datatypes that store their structure in cells, and
  are identified by their cell address.

  Regular words store their data in cells whose 1st machine word is used
  for the word type field, which is either a numerical ID for predefined
  types or a pointer to a Col_CustomWordType structure for custom types.
  As such structures are always word-aligned, this means that the two
  least significant bits of their pointer value are zero (for
  architectures with at least 32 bit words) and are free for our purpose.

  On little endian architectures, the LSB of the type pointer is the
  cell's byte 0. On big endian architectures, we rotate the pointer value
  one byte to the right so that the original LSB ends up on byte 0. That
  way the two reserved bits are on byte 0 for both predefined type IDs and
  type pointers.

  We use bit 0 of the type field as the pinned flag for both predefined
  type IDs and type pointers. Given the above, this bit is always on byte
  0. When set, this means that the word isn't movable; its address remains
  fixed as long as this flag is set. Words can normally be moved to the
  upper generation pool during the compaction phase of the GC.

  @par Requirements
  - Regular words must store their type info and a pinned flag.

  @param Pinned(P)     Pinned flag.

  @par Cell Layout
    On all architectures the cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        regular_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="10">0</td>
                <td sides="B" width="150" align="left">1</td><td sides="B" width="155" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_PINNED" title="WORD_PINNED">P</td>
                <td sides="R" colspan="2"></td>
            </tr>
            <tr><td sides="R">.</td>
                <td colspan="3" rowspan="3" sides="BR" width="320">Type-specific data</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0 1                                                           n
          +-+-------------------------------------------------------------+
        0 |P|                                                             |
          +-+                                                             .
          .                                                               .
          .                      Type-specific data                       .
        N |                                                               |
          +---------------------------------------------------------------+
                                                                                        @endif
                                                                                *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Regular Word Accessors                                                       *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_SYNONYM
 *  Get/set synonym word for regular words having a synonym field. For
 *  performance and simplicity all word types with a synonym field use the
 *  same location (2nd word of cell).
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *//*-----------------------------------------------------------------------*/

#define WORD_SYNONYM(word)      (((Col_Word *)(word))[1])

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_PINNED
 *
 *  Get pinned flag.
 *
 *  @see WORD_SET_PINNED
 *  @see WORD_CLEAR_PINNED
 *
 *                                                                              *//*!   @def \
 * WORD_SET_PINNED
 *
 *  Set pinned flag.
 *
 *  @see WORD_PINNED
 *  @see WORD_CLEAR_PINNED
 *
 *                                                                              *//*!   @def \
 * WORD_CLEAR_PINNED
 *
 *  Clear pinned flag.
 *
 *  @see WORD_PINNED
 *  @see WORD_SET_PINNED
 *//*-----------------------------------------------------------------------*/

#define WORD_PINNED(word)       (((uint8_t *)(word))[0] & 1)
#define WORD_SET_PINNED(word)   (((uint8_t *)(word))[0] |= 1)
#define WORD_CLEAR_PINNED(word) (((uint8_t *)(word))[0] &= ~1)
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup predefined_words \
Predefined Words
                                                                                        @ingroup regular_words
  Predefined words are @ref regular_words that are identified by a fixed ID and
  managed by hardwired internal code.

  @ref word_type_ids "Predefined type IDs" for regular word types are chosen
  so that their bit 0 is cleared and bit 1 is set, to distinguish them with
  type pointers and avoid value clashes.

  @par Requirements
  - Regular words must store their type ID.

  @param Type       Type identifier.

  @par Cell Layout
    On all architectures the cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        predefined_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="10">0</td>
                <td sides="B" width="10">1</td>
                <td sides="B" width="20" align="left">2</td><td sides="B" width="30" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_PINNED" title="WORD_PINNED">P</td>
                <td>1</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td sides="R" colspan="2"></td>
            </tr>
            <tr><td sides="R">.</td>
                <td colspan="6" rowspan="3" sides="BR" width="320">Type-specific data</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0 1 2    7                                                    n
          +-+-+------+----------------------------------------------------+
        0 |P|1| Type |                                                    |
          +-+-+------+                                                    .
          .                                                               .
          .                                                               .
          .                      Type-specific data                       .
          .                                                               .
        N |                                                               |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see @ref word_type_ids "Word Type Identifiers"                               *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Predefined Word Accessors                                                    *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_TYPEID
 *  Get type ID for predefined word type.
 *
 *  @param word     Word to access.
 *
 *  @return
 *      Word type ID.
 *
 *  @see @ref word_type_ids "Word Type Identifiers"
 *  @see WORD_SET_TYPEID
 *
 *                                                                              *//*!   @def \
 * WORD_SET_TYPEID
 *  Set type ID for predefined word type.
 *
 *  @param word     Word to access.
 *  @param type     Type ID.
 *
 *  @sideeffect
 *      This also clears the pinned flag.
 *
 *  @see WORD_TYPEID
 *  @see WORD_CLEAR_PINNED
 *//*-----------------------------------------------------------------------*/

#define WORD_TYPEID(word)           ((((uint8_t *)(word))[0])&~1)
#define WORD_SET_TYPEID(word, type) (((uint8_t *)(word))[0] = (type))
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup custom_words \
Custom Words
                                                                                        @ingroup regular_words
  Custom words are words defined by applicative code. They can extend existing
  word types like ropes or maps, or define application-specific data types.

  Custom words are described by a #Col_CustomWordType type descriptor structure
  defined and managed by client code.

  As such type descriptor structures are machine word-aligned, this means
  that the four lower bits of their address are cleared.

  @par Requirements
  - Custom words must store a pointer to their #Col_CustomWordType.

  - Custom words can be part of a synonym chain.

  - Some custom word types define cleanup procedures
    (#Col_CustomWordType.freeProc), such words need to be traversed during the
    sweep phase of the GC process. To do so, they are singly-linked together
    and so need to store the next word in chain.

  @param Type       Type descriptor.
  @param Synonym    [Generic word synonym field] (@ref WORD_SYNONYM).
  @param Next       Next word in custom cleanup chain (only when
                    #Col_CustomWordType.freeProc is non-NULL).
  @param Data       Custom type payload.

  @par Cell Layout
    On all architectures the cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        custom_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="10">0</td>
                <td sides="B" width="10">1</td>
                <td sides="B" width="140" align="left">2</td><td sides="B" width="150" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_PINNED" title="WORD_PINNED">P</td>
                <td>0</td>
                <td href="@ref WORD_TYPEINFO" title="WORD_TYPEINFO" colspan="2">Type</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_SYNONYM" title="WORD_SYNONYM" colspan="4" width="320">Synonym</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_CUSTOM_NEXT" title="WORD_CUSTOM_NEXT" colspan="4">Next (optional)</td>
            </tr>
            <tr><td sides="R">.</td>
                <td href="@ref WORD_CUSTOM_DATA" title="WORD_CUSTOM_DATA" colspan="4" rowspan="3">Type-specific data</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0 1                                                           n
          +-+-+-----------------------------------------------------------+
        0 |P|0|                         Type                              |
          +-+-+-----------------------------------------------------------+
        1 |                            Synonym                            |
          +---------------------------------------------------------------+
        2 |                        Next (optional)                        |
          +---------------------------------------------------------------+
          .                                                               .
          .                                                               .
          .                      Type-specific data                       .
          .                                                               .
        N |                                                               |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see WORD_TYPE_CUSTOM
  @see Col_CustomWordType                                                       *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Custom Word Constants                                                        *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * CUSTOM_HEADER_SIZE
 *  Byte size of custom word header.
 *//*-----------------------------------------------------------------------*/

#define CUSTOM_HEADER_SIZE              (sizeof(Col_CustomWordType *)+sizeof(Col_Word *))
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Custom Word Creation                                                         *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_CUSTOM_INIT
 *
 *  Custom word initializer.
 *
 *  @param word         Word to initialize.
 *  @param typeInfo     #WORD_SET_TYPEINFO.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPEINFO
 *//*-----------------------------------------------------------------------*/

#define WORD_CUSTOM_INIT(word, typeInfo) \
    WORD_SET_TYPEINFO((word), (typeInfo)); \
    WORD_SYNONYM(word) = WORD_NIL;
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Custom Word Accessors                                                        *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_TYPEINFO
 *  Get #Col_CustomWordType custom type descriptor.
 *
 *  @param word         Word to access.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_SET_TYPEINFO
 *
 *                                                                              *//*!   @def \
 * WORD_SET_TYPEINFO
 *  Set #Col_CustomWordType custom type descriptor.
 *
 *  @param word         Word to access.
 *  @param addr         Address of #Col_CustomWordType descriptor.
 *
 *  @warning
 *      Arguments are referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @sideeffect
 *      This also clears the pinned flag.
 *
 *  @see WORD_TYPEINFO
 *//*-----------------------------------------------------------------------*/

#ifdef COL_BIGENDIAN
                                                                                #       ifndef DOXYGEN
#   define ROTATE_LEFT(value) \
        (((value)>>((sizeof(value)-1)*8)) | ((value)<<8))
#   define ROTATE_RIGHT(value) \
        (((value)<<((sizeof(value)-1)*8)) | ((value)>>8))
                                                                                #       endif /* DOXYGEN */
#   define WORD_TYPEINFO(word) \
        ((Col_CustomWordType *)(ROTATE_LEFT(*(uintptr_t *)(word))&~1))
#   define WORD_SET_TYPEINFO(word, addr) \
        (*(uintptr_t *)(word) = ROTATE_RIGHT(((uintptr_t)(addr))))
#else
#   define WORD_TYPEINFO(word) \
        ((Col_CustomWordType *)((*(uintptr_t *)(word))&~1))
#   define WORD_SET_TYPEINFO(word, addr) \
        (*(uintptr_t *)(word) = ((uintptr_t)(addr)))
#endif

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_CUSTOM_NEXT
 *
 *  Get/set next word in custom cleanup chain.
 *
 *  @warning
 *      Only when #Col_CustomWordType.freeProc is non-NULL.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *//*-----------------------------------------------------------------------*/

#define WORD_CUSTOM_NEXT(word, typeInfo, headerSize) (*(Col_Word *)(((char *)(word))+headerSize))

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_CUSTOM_DATA
 *
 *  Pointer to beginning of custom word data.
 *//*-----------------------------------------------------------------------*/

#define WORD_CUSTOM_DATA(word, typeInfo, headerSize) ((void *)(&WORD_CUSTOM_NEXT((word), (typeInfo), (headerSize))+((typeInfo)->freeProc?1:0)))

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_CUSTOM_SIZE
 *
 *  Get number of cells taken by the custom word.
 *
 *  @param typeInfo     WORD_TYPEINFO
 *  @param headerSize   Byte size of predefined header.
 *  @param size         Byte size of custom word data.
 *
 *  @result
 *      Number of cells taken by word.
 *//*-----------------------------------------------------------------------*/

#define WORD_CUSTOM_SIZE(typeInfo, headerSize, size) \
    (NB_CELLS((headerSize)+((typeInfo)->freeProc?sizeof(Cell *):0)+(size)))
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup wrap_words \
Wrap Words
                                                                                        @ingroup regular_words
  Wrap words are generic wrappers for @ref words lacking specific features.

  - Words may have synonyms that can take any accepted word value:
    @ref immediate_words (e.g. Nil), or cell-based @ref regular_words. Words
    can thus be part of chains of synonyms having different types, but with
    semantically identical values. Such chains form a circular linked list
    using this field. The order of words in a synonym chain has no importance.

    Some word types have no synonym field (typically @ref immediate_words, but
    also many @ref predefined_words), in this case they must be wrapped
    into structures that have one when they are added to a chain of synonyms.

  - @ref immediate_words storing native datatypes often cannot represent the
    whole range of values. For example, @ref smallint_words lack one bit
    (the MSB) compared to native integers. This means that larger values
    cannot be represented as immediate word values. So these datatypes need
    to use wrap words instead of immediate words to store these values. But
    both representations remain semantically identical.

    For example, Col_NewIntWord() will create either @ref smallint_words or
    @ref int_wrappers depending on the integer value.

  @par Requirements
  - Wrap words must store wrapped data.

  - Wrap words can be part of a synonym chain.

  - Wrap words must know the @ref word_types "word type" of the wrapped
    data type.

@todo - Wrap words have a flags field for future purposes.

  @param WrappedType    Wrapped value @ref word_types "type".
  @param Synonym        [Generic word synonym field] (@ref WORD_SYNONYM).
  @param Data           Wrapped value payload.
  @param Flags          Flags.

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        wrap_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_WRAP_FLAGS" title="WORD_WRAP_FLAGS" colspan="2">Flags</td>
                <td href="@ref WORD_WRAP_TYPE" title="WORD_WRAP_TYPE" colspan="2">Wrapped type</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_SYNONYM" title="WORD_SYNONYM" colspan="7">Synonym</td>
            </tr>
            <tr><td sides="R">2</td>
                <td colspan="7" rowspan="2">Type-specific data</td>
            </tr>
            <tr><td sides="R">3</td></tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0     7 8    15 16           31                               n
          +-------+-------+---------------+-------------------------------+
        0 | Type  | Flags | Wrapped type  |        Unused (n > 32)        |
          +-------+-------------------------------------------------------+
        1 |                            Synonym                            |
          +---------------------------------------------------------------+
        2 |                                                               |
          +                       Type-specific data                      +
        3 |                                                               |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see WORD_TYPE_WRAP                                                           *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Wrap Word Constants                                                          *//*!   @{ *//*
 ******************************************************************************/

#define WRAP_FLAG_MUTABLE       1   /*!< @todo */
#define WRAP_FLAG_VARIANT       2   /*!< @todo */
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Wrap Word Accessors                                                          *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_WRAP_FLAGS
 *  Get/set wrap word flags.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_WRAP_INIT
 *  @see WORD_INTWRAP_INIT
 *  @see WORD_FPWRAP_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_WRAP_TYPE
 *  Get/set wrapped value @ref word_types "type".
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *//*-----------------------------------------------------------------------*/

#define WORD_WRAP_FLAGS(word)   (((uint8_t *)(word))[1])
#define WORD_WRAP_TYPE(word)    (((uint16_t *)(word))[1])
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup word_wrappers \
Word Wrappers
                                                                                        @ingroup wrap_words
  Word wrappers are @ref wrap_words that wrap a word lacking a synonym field.

  @par Requirements
  - Word wrappers must store their source word.

  @param Source     Wrapped word.

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        word_wrapper [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_WRAP_FLAGS" title="WORD_WRAP_FLAGS" colspan="2">Flags</td>
                <td href="@ref WORD_WRAP_TYPE" title="WORD_WRAP_TYPE" colspan="2">Wrapped type</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_SYNONYM" title="WORD_SYNONYM" colspan="7">Synonym</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_WRAP_SOURCE" title="WORD_WRAP_SOURCE" colspan="7">Source</td>
            </tr>
            <tr><td sides="R">3</td>
                <td colspan="7" bgcolor="grey75">Unused</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0     7 8    15 16           31                               n
          +-------+-------+---------------+-------------------------------+
        0 | Type  | Flags | Wrapped type  |        Unused (n > 32)        |
          +-------+-------------------------------------------------------+
        1 |                            Synonym                            |
          +---------------------------------------------------------------+
        2 |                            Source                             |
          +---------------------------------------------------------------+
        3 |                            Unused                             |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see WORD_TYPE_WRAP                                                           *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Word Wrapper Creation                                                        *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_WRAP_INIT
 *
 *  Word wrapper initializer.
 *
 *  @param word     Word to initialize.
 *  @param flags    #WORD_WRAP_FLAGS.
 *  @param type     #WORD_WRAP_TYPE.
 *  @param source   #WORD_WRAP_SOURCE.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_WRAP
 *//*-----------------------------------------------------------------------*/

#define WORD_WRAP_INIT(word, flags, type, source) \
    WORD_SET_TYPEID((word), WORD_TYPE_WRAP); \
    WORD_WRAP_FLAGS(word) = (flags); \
    WORD_WRAP_TYPE(word) = (type); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_WRAP_SOURCE(word) = (source);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Word Wrapper Accessors                                                       *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_WRAP_SOURCE
 *  Get/set wrapped word.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_WRAP_INIT
 *//*-----------------------------------------------------------------------*/

#define WORD_WRAP_SOURCE(word)  (((Col_Word *)(word))[2])

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_UNWRAP
 *
 *  If word is a word wrapper, get its source. Else do nothing.
 *
 *  @param[in,out] word     Word to unwrap.
 *
 *  @sideeffect
 *      If **word** is wrapped, it gets replaced by its source.
 *//*-----------------------------------------------------------------------*/

#define WORD_UNWRAP(word) \
    if (   WORD_TYPE(word) == WORD_TYPE_WRAP \
        && !(WORD_WRAP_TYPE(word) & (COL_INT|COL_FLOAT))) \
        (word) = WORD_WRAP_SOURCE(word);
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup int_wrappers \
Integer Wrappers
                                                                                        @ingroup wrap_words
  Integer wrappers are @ref wrap_words that wrap an integer value, either
  because the value wouldn't fit @ref smallint_words or the word would need a
  synonym field.

  @par Requirements
  - Integer wrappers must store a native integer value.

  @param Value      Wrapped integer value.

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        int_wrapper [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_WRAP_FLAGS" title="WORD_WRAP_FLAGS" colspan="2">Flags</td>
                <td href="@ref WORD_WRAP_TYPE" title="WORD_WRAP_TYPE" colspan="2">Wrapped type</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_SYNONYM" title="WORD_SYNONYM" colspan="7">Synonym</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_INTWRAP_VALUE" title="WORD_INTWRAP_VALUE" colspan="7">Value</td>
            </tr>
            <tr><td sides="R">3</td>
                <td colspan="7" bgcolor="grey75">Unused</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0     7 8    15 16           31                               n
          +-------+-------+---------------+-------------------------------+
        0 | Type  | Flags | Wrapped type  |        Unused (n > 32)        |
          +-------+-------------------------------------------------------+
        1 |                            Synonym                            |
          +---------------------------------------------------------------+
        2 |                             Value                             |
          +---------------------------------------------------------------+
        3 |                            Unused                             |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see WORD_TYPE_WRAP                                                           *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Integer Wrapper Creation                                                     *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_INTWRAP_INIT
 *
 *  Integer wrapper initializer.
 *
 *  @param word     Word to initialize.
 *  @param flags    #WORD_WRAP_FLAGS.
 *  @param value    #WORD_INTWRAP_VALUE.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_WRAP
 *//*-----------------------------------------------------------------------*/

#define WORD_INTWRAP_INIT(word, flags, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_WRAP); \
    WORD_WRAP_FLAGS(word) = (flags); \
    WORD_WRAP_TYPE(word) = COL_INT; \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_INTWRAP_VALUE(word) = (value);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Integer Wrapper Accessors                                                    *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_INTWRAP_VALUE
 *  Get/set wrapped integer value.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_INTWRAP_INIT
 *//*-----------------------------------------------------------------------*/

#define WORD_INTWRAP_VALUE(word) (((intptr_t *)(word))[2])
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup fp_wrappers \
Floating Point Wrappers
                                                                                        @ingroup wrap_words
  Floating point wrappers are @ref wrap_words that wrap a floating point value,
  either because the value wouldn't fit @ref smallfp_words or the word would
  need a synonym field.

  @par Requirements
  - Floating point wrappers must store a native floating point value.

  @param Value      Wrapped floating point value.

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        fp_wrapper [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_WRAP_FLAGS" title="WORD_WRAP_FLAGS" colspan="2">Flags</td>
                <td href="@ref WORD_WRAP_TYPE" title="WORD_WRAP_TYPE" colspan="2">Wrapped type</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_SYNONYM" title="WORD_SYNONYM" colspan="7">Synonym</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_FPWRAP_VALUE" title="WORD_FPWRAP_VALUE" sides="LTR" colspan="7">Value</td>
            </tr>
            <tr><td sides="R">3</td>
                <td sides="LBR" colspan="7" bgcolor="grey75">Unused (n &gt; 32)</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0     7 8    15 16           31                               n
          +-------+-------+---------------+-------------------------------+
        0 | Type  | Flags | Wrapped type  |        Unused (n > 32)        |
          +-------+-------------------------------------------------------+
        1 |                            Synonym                            |
          +---------------------------------------------------------------+
        2 |                             Value                             |
          + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
        3 |                        Unused (n > 32)                        |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see WORD_TYPE_WRAP                                                           *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Floating Point Wrapper Creation                                              *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_FPWRAP_INIT
 *
 *  Integer wrapper initializer.
 *
 *  @param word     Word to initialize.
 *  @param flags    #WORD_WRAP_FLAGS.
 *  @param value    #WORD_FPWRAP_VALUE.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_WRAP
 *//*-----------------------------------------------------------------------*/

#define WORD_FPWRAP_INIT(word, flags, value) \
    WORD_SET_TYPEID((word), WORD_TYPE_WRAP); \
    WORD_WRAP_FLAGS(word) = (flags); \
    WORD_WRAP_TYPE(word) = COL_FLOAT; \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_FPWRAP_VALUE(word) = (value);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Floating Point Wrapper Accessors                                             *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_FPWRAP_VALUE
 *  Get/set wrapped floating point value.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_FPWRAP_INIT
 *//*-----------------------------------------------------------------------*/

#define WORD_FPWRAP_VALUE(word) (*(double *)(((intptr_t *)(word))+2))
                                                                                /*!     @} */
                                                                                /*!     @} */
#ifdef PROMOTE_COMPACT
/*
================================================================================*//*!   @addtogroup redirect_words \
Redirect Words
                                                                                        @ingroup regular_words
  Redirect words are temporary words used during GC compaction phase (see
  #PROMOTE_COMPACT).

  When a word is relocated, the original cell is overwritten by a redirect
  word that points to the new cell, so that other words pointing to this word
  can update their pointer.

  @par Requirements
  - Redirect words must store the relocated source word.

  @param Source     New location of the word.

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        redirect_word [shape=none, label=<
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
                <td href="@ref WORD_REDIRECT_SOURCE" title="WORD_REDIRECT_SOURCE" colspan="4">Source</td>
            </tr>
            <tr><td sides="R">2</td>
                <td colspan="4" rowspan="2" bgcolor="grey75">Unused</td>
            </tr>
            <tr><td sides="R">3</td></tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0     7                                                       n
          +-------+-------------------------------------------------------+
        0 | Type  |                        Unused                         |
          +-------+-------------------------------------------------------+
        1 |                            Source                             |
          +---------------------------------------------------------------+
        2 |                                                               |
          +                            Unused                             +
        3 |                                                               |
          +---------------------------------------------------------------+
                                                                                        @endif
                                                                                *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Redirect Word Creation                                                       *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_REDIRECT_INIT
 *
 *  Redirect word initializer.
 *
 *  @param word     Word to initialize.
 *  @param source   #WORD_REDIRECT_SOURCE.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_REDIRECT
 *//*-----------------------------------------------------------------------*/

#define WORD_REDIRECT_INIT(word, source) \
    WORD_SET_TYPEID((word), WORD_TYPE_REDIRECT); \
    WORD_REDIRECT_SOURCE(word) = (source);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Redirect Word Accessors                                                      *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_REDIRECT_SOURCE
 *  Get/set new location of the word.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_REDIRECT_INIT
 *//*-----------------------------------------------------------------------*/

#define WORD_REDIRECT_SOURCE(word)   (((Col_Word *)(word))[1])
                                                                                /*!     @} */
                                                                                /*!     @} */
#endif /* PROMOTE_COMPACT*/

#endif /* _COLIBRI_WORD_INT */
                                                                                /*!     @endcond */