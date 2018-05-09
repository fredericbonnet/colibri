/*                                                                              *//*!   @cond PRIVATE @file \
 * colVectorInt.h
 *
 *  This header file defines the vector word internals of Colibri.
 *
 *  Vectors are arrays of words that are directly accessible through a
 *  pointer value.
 *
 *  They come in both immutable and mutable forms.
 *
 *  @see colVector.c
 *  @see colVector.h
 *
 *  @private
 */

#ifndef _COLIBRI_VECTOR_INT
#define _COLIBRI_VECTOR_INT


/*
================================================================================*//*!   @addtogroup vector_words \
Immutable Vectors
                                                                                        @ingroup predefined_words
  Immutable vectors are constant, fixed-length arrays of words that are directly
  accessible through a pointer value.

  @par Requirements
  - Vector words must know their length, i.e. the number of elements they
    contain.

  - Elements are stored within the word cells following the header.

  - Vector words use as much cells as needed to store their elements.

  @param Length     Size of element array.
  @param Elements   Array of element words.

  @par Cell Layout
    On all architectures the cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        vector_word [shape=none, label=<
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
                <td href="@ref WORD_VECTOR_LENGTH" title="WORD_VECTOR_LENGTH" colspan="4">Length</td>
            </tr>
            <tr><td sides="R">.</td>
                <td href="@ref WORD_VECTOR_ELEMENTS" title="WORD_VECTOR_ELEMENTS" colspan="4" rowspan="4">Elements</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0     7                                                       n
          +-------+-------------------------------------------------------+
        0 | Type  |                        Unused                         |
          +-------+-------------------------------------------------------+
        1 |                            Length                             |
          +---------------------------------------------------------------+
          .                                                               .
          .                           Elements                            .
        N |                                                               |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see WORD_TYPE_VECTOR                                                         *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Immutable Vector Constants                                                   *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * VECTOR_HEADER_SIZE
 *  Byte size of vector header.
 *//*-----------------------------------------------------------------------*/

#define VECTOR_HEADER_SIZE              (sizeof(size_t)*2)
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Immutable Vector Utilities                                                   *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * VECTOR_SIZE
 *
 *  Get number of cells for a vector of a given length.
 *
 *  @param length   Vector length.
 *
 *  @result
 *      Number of cells taken by word.
 *//*-----------------------------------------------------------------------*/

#define VECTOR_SIZE(length) \
    (NB_CELLS(VECTOR_HEADER_SIZE+(length)*sizeof(Col_Word)))

/*---------------------------------------------------------------------------   *//*!   @def \
 * VECTOR_MAX_LENGTH
 *
 *  Get maximum vector length for a given byte size.
 *
 *  @param byteSize     Available size.
 *
 *  @result
 *      Vector length fitting the given size.
 *//*-----------------------------------------------------------------------*/

#define VECTOR_MAX_LENGTH(byteSize) \
    (((byteSize)-VECTOR_HEADER_SIZE)/sizeof(Col_Word))
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Immutable Vector Creation                                                    *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_VECTOR_INIT
 *
 *  Immutable vector word initializer.
 *
 *  @param word     Word to initialize.
 *  @param length   #WORD_VECTOR_LENGTH.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_VECTOR
 *//*-----------------------------------------------------------------------*/

#define WORD_VECTOR_INIT(word, length) \
    WORD_SET_TYPEID((word), WORD_TYPE_VECTOR); \
    WORD_VECTOR_LENGTH(word) = (length);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Immutable Vector Accessors                                                   *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_VECTOR_LENGTH
 *  Get/set size of element array.
 *
 *  Used by both mutable and immutable versions.
 *
 *  @param word     Word to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see WORD_VECTOR_INIT
 *  @see WORD_MVECTOR_INIT
 *
 *                                                                              *//*!   @def \
 * WORD_VECTOR_ELEMENTS
 *  Pointer to vector elements (array of words).
 *
 *  Used by both mutable and immutable versions.
 *
 *  @param word     Word to access.
 *//*-----------------------------------------------------------------------*/

#define WORD_VECTOR_LENGTH(word)        (((size_t *)(word))[1])
#define WORD_VECTOR_ELEMENTS(word)      ((Col_Word *)(word)+2)
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Immutable Vector Exceptions                                                  *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * TYPECHECK_VECTOR
 *                                                                                      @hideinitializer
 *  Type checking macro for vectors.
 *
 *  @param word     Checked word.
 *
 *  @typecheck{COL_ERROR_VECTOR,word}
 *//*-----------------------------------------------------------------------*/

#define TYPECHECK_VECTOR(word) \
    TYPECHECK((Col_WordType(word) & COL_VECTOR), COL_ERROR_VECTOR, (word))

/*---------------------------------------------------------------------------   *//*!   @def \
 * VALUECHECK_VECTORLENGTH
 *                                                                                      @hideinitializer
 *  Value checking macro for vectors, ensures that length does not exceed
 *  the maximum value.
 *
 *  @param length       Checked length.
 *  @param maxLength    Maximum allowed value.
 *
 *  @valuecheck{COL_ERROR_VECTORLENGTH,length <= maxLength}
 *//*-----------------------------------------------------------------------*/

#define VALUECHECK_VECTORLENGTH(length, maxLength) \
    VALUECHECK(((length) <= (maxLength)), COL_ERROR_VECTORLENGTH, (length), \
            (maxLength))
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup mvector_words \
Mutable Vectors
                                                                                        @ingroup predefined_words vector_words
  Mutable vectors are arrays of words that are directly accessible and
  modifiable through a pointer value, and whose length can vary up to a
  given capacity set at creation time; they can be"frozen" and turned into
  immutable vectors.

  @par Requirements
  - Mutable vector words use the same basic structure as immutable vector words
    so that they can be turned immutable in-place ([frozen]
    (@ref Col_MVectorFreeze)).

  - In addition to immutable vector fields, mutable vectors must know their
    maximum capacity. It is deduced from the word size in cells minus the
    header. So we just have to know the word cell size. When frozen, unused
    trailing cells are freed.

  @param Size       Number of allocated cells. This determines the maximum
                    length of the element array. Given the storage capacity,
                    the maximum size of a mutable vector is smaller than that
                    of immutable vectors (see Col_MaxMVectorLength() vs.
                    Col_MaxVectorLength()).
  @param Length     [Generic immutable vector length field]
                    (@ref WORD_VECTOR_LENGTH).
  @param Elements   [Generic immutable vector elements array]
                    (@ref WORD_VECTOR_ELEMENTS).

  @par Cell Layout
    On all architectures the cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        mvector_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_MVECTOR_SIZE" title="WORD_MVECTOR_SIZE" colspan="2">Size</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_VECTOR_LENGTH" title="WORD_VECTOR_LENGTH" colspan="4">Length</td>
            </tr>
            <tr><td sides="R">.</td>
                <td href="@ref WORD_VECTOR_ELEMENTS" title="WORD_VECTOR_ELEMENTS" colspan="4" rowspan="4">Elements</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
             0     7                                                       n
          +-------+-------------------------------------------------------+
        0 | Type  |                         Size                          |
          +-------+-------------------------------------------------------+
        1 |                            Length                             |
          +---------------------------------------------------------------+
          .                                                               .
          .                           Elements                            .
        N |                                                               |
          +---------------------------------------------------------------+
                                                                                        @endif
  @see @ref vector_words
  @see WORD_TYPE_MVECTOR                                                        *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Mutable Vector Constants                                                     *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * MVECTOR_MAX_SIZE
 *    Maximum cell size taken by mutable vectors.
 *//*-----------------------------------------------------------------------*/

#define MVECTOR_MAX_SIZE        (SIZE_MAX>>CHAR_BIT)
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Mutable Vector Creation                                                      *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_MVECTOR_INIT
 *
 *  Mutable vector word initializer.
 *
 *  @param word     Word to initialize.
 *  @param size     #WORD_MVECTOR_SET_SIZE.
 *  @param length   #WORD_VECTOR_LENGTH.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_TYPE_MVECTOR
 *//*-----------------------------------------------------------------------*/

#define WORD_MVECTOR_INIT(word, size, length) \
    WORD_SET_TYPEID((word), WORD_TYPE_MVECTOR); \
    WORD_MVECTOR_SET_SIZE((word), (size)); \
    WORD_VECTOR_LENGTH(word) = (length);
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Mutable Vector Accessors                                                     *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * WORD_MVECTOR_SIZE
 *  Get the number of allocated cells.
 *
 *  @param word     Word to access.
 *
 *  @see WORD_MVECTOR_SET_SIZE
 *
 *                                                                              *//*!   @def \
 * WORD_MVECTOR_SET_SIZE
 *  Set the number of allocated cells.
 *  
 *  @param word     Word to access.
 *  @param size     Size of word in cells.
 *
 *  @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see WORD_MVECTOR_SIZE
 *  @see WORD_MVECTOR_INIT
 *//*-----------------------------------------------------------------------*/
                                                                                #       ifndef DOXYGEN
#define MVECTOR_SIZE_MASK               MVECTOR_MAX_SIZE
                                                                                #       endif /* DOXYGEN */
#ifdef COL_BIGENDIAN
#   define WORD_MVECTOR_SIZE(word)      ((((size_t *)(word))[0])&MVECTOR_SIZE_MASK)
#   define WORD_MVECTOR_SET_SIZE(word, size) ((((size_t *)(word))[0])&=~MVECTOR_SIZE_MASK,(((size_t *)(word))[0])|=((size)&MVECTOR_SIZE_MASK))
#else
#   define WORD_MVECTOR_SIZE(word)      ((((size_t *)(word))[0]>>CHAR_BIT)&MVECTOR_SIZE_MASK)
#   define WORD_MVECTOR_SET_SIZE(word, size) ((((size_t *)(word))[0])&=~(MVECTOR_SIZE_MASK<<CHAR_BIT),(((size_t *)(word))[0])|=(((size)&MVECTOR_SIZE_MASK)<<CHAR_BIT))
#endif
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Mutable Vector Exceptions                                                    *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * TYPECHECK_MVECTOR
 *                                                                                      @hideinitializer
 *  Type checking macro for mutable vectors.
 *
 *  @param word     Checked word.
 *
 *  @typecheck{COL_ERROR_MVECTOR,word}
 *//*-----------------------------------------------------------------------*/

#define TYPECHECK_MVECTOR(word) \
    TYPECHECK((Col_WordType(word) & COL_MVECTOR), COL_ERROR_MVECTOR, (word))
                                                                                /*!     @} */
                                                                                /*!     @} */
#endif /* _COLIBRI_VECTOR_INT */
                                                                                /*!     @endcond */