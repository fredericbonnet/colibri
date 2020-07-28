/**
 * @file colStrBufInt.h
 *
 * This header file defines the string buffer word internals of Colibri.
 *
 * String buffers are used to build strings incrementally in an efficient
 * manner, by appending individual characters or whole ropes. The current
 * accumulated rope can be retrieved at any time.
 *
 * @see colStrBuf.c
 * @see colStrBuf.h
 *
 * @beginprivate @cond PRIVATE
 */

#ifndef _COLIBRI_STRBUF_INT
#define _COLIBRI_STRBUF_INT


/*
===========================================================================*//*!
\internal \addtogroup strbuf_words String Buffers

@par Requirements
    - String buffer words can use different character formats for internal
      storage. So they need to store this information.

    - Characters can be appended individually or in bulks (ropes or sequences
      as iterator ranges). When adding single or multiple characters, they are
      appended to an internal buffer stored within the word, up to its maximum
      capacity. When the buffer is full, a rope is appended, or the
      accumulated value is retrieved, the buffer content is appended to the
      accumulated rope so far and the buffer is cleared before performing the
      operation.

    - String buffers have a buffer length set at creation time. This plus the
      format determines the byte size of the buffer and thus number of cells
      the word takes. So we store this cell size and deduce the actual buffer
      length from the word size in cells minus the header, divided by the
      character width of the used format.

    @param Format   Preferred format for string building.
    @param Size     Number of allocated cells.
    @param Rope     Cumulated rope so far.
    @param Length   Current length of character buffer.
    @param Buffer   Character buffer data (array of characters).

@par Cell Layout
    On all architectures the cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        strbuf_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="40" align="left">8</td><td sides="B" width="40" align="right">15</td>
                <td sides="B" width="80" align="left">16</td><td sides="B" width="80" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td href="@ref WORD_STRBUF_FORMAT" title="WORD_STRBUF_FORMAT" colspan="2">Format</td>
                <td href="@ref WORD_STRBUF_SIZE" title="WORD_STRBUF_SIZE" colspan="2">Size</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref WORD_STRBUF_ROPE" title="WORD_STRBUF_ROPE" colspan="7">Rope</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_STRBUF_LENGTH" title="WORD_STRBUF_LENGTH" colspan="7">Length</td>
            </tr>
            <tr><td sides="R">.</td>
                <td href="@ref WORD_STRBUF_BUFFER" title="WORD_STRBUF_BUFFER" colspan="7" rowspan="3">Buffer</td>
            </tr>
            <tr><td sides="R">.</td></tr>
            <tr><td sides="R">N</td></tr>
            </table>
        >]
    }
    @enddot

    @begindiagram
        0     7 8     15 16          31                               n
       +-------+--------+--------------+-------------------------------+
     0 | Type  | Format |     Size     |        Unused (n > 32)        |
       +-------+--------+--------------+-------------------------------+
     1 |                              Rope                             |
       +---------------------------------------------------------------+
     2 |                             Length                            |
       +---------------------------------------------------------------+
       .                                                               .
       .                             Buffer                            .
     N |                                                               |
       +---------------------------------------------------------------+
    @enddiagram

@see WORD_TYPE_STRBUF
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name String Buffer Constants
 ***************************************************************************\{*/

/** Byte size of string buffer header. */
#define STRBUF_HEADER_SIZE              (sizeof(size_t)*3)

/** Maximum cell size taken by string buffers. */
#define STRBUF_MAX_SIZE                 UINT16_MAX

/* End of String Buffer Constants *//*!\}*/


/***************************************************************************//*!
 * \name String Buffer Utilities
 ***************************************************************************\{*/

/**
 * Get number of cells for a string buffer of a given length.
 *
 * @param byteLength  Buffer length in bytes.
 *
 * @return Number of cells taken by word.
 */
#define STRBUF_SIZE(byteLength) \
    (NB_CELLS(STRBUF_HEADER_SIZE+(byteLength)))

/**
 * Get maximum string buffer length for a given byte size.
 *
 * @param byteSize  Available size.
 * @param format    Character format.
 *
 * @return String buffer length fitting the given size.
 */
#define STRBUF_MAX_LENGTH(byteSize, format) \
    (((byteSize)-STRBUF_HEADER_SIZE)/CHAR_WIDTH(format))

/* End of String Buffer Utilities *//*!\}*/


/***************************************************************************//*!
 * \name String Buffer Creation
 ***************************************************************************\{*/

/**
 * String buffer word initializer.
 *
 * @param word      Word to initialize.
 * @param size      #WORD_STRBUF_SIZE.
 * @param format    #WORD_STRBUF_FORMAT.
 *
 * @warning
 *      Argument **word** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 * @see WORD_TYPE_STRBUF
 */
#define WORD_STRBUF_INIT(word, size, format) \
    WORD_SET_TYPEID((word), WORD_TYPE_STRBUF); \
    WORD_STRBUF_FORMAT(word) = (format); \
    WORD_STRBUF_SIZE(word) = (uint16_t) (size); \
    WORD_STRBUF_ROPE(word) = WORD_SMALLSTR_EMPTY; \
    WORD_STRBUF_LENGTH(word) = 0;

/* End of String Buffer Creation *//*!\}*/


/***************************************************************************//*!
 * \name String Buffer Accessors
 ***************************************************************************\{*/

/**
 * Get/set preferred format for string building.
 *
 * @param word  Word to access.
 *
 * @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 * @see WORD_STRBUF_INIT
 */
#define WORD_STRBUF_FORMAT(word)        (((int8_t *)(word))[1])

/**
 * Get/set number of allocated cells.
 *
 * @param word  Word to access.
 *
 * @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 * @see WORD_STRBUF_INIT
 */
#define WORD_STRBUF_SIZE(word)          (((uint16_t *)(word))[1])

/**
 * Get/set cumulated rope so far.
 *
 * @param word  Word to access.
 *
 * @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 * @see WORD_STRBUF_INIT
 */
#define WORD_STRBUF_ROPE(word)          (((Col_Word *)(word))[1])

/**
 * Get/set current length of character buffer.
 *
 * @param word  Word to access.
 *
 * @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 * @see WORD_STRBUF_INIT
 */
#define WORD_STRBUF_LENGTH(word)        (((size_t *)(word))[2])

/**
 * Pointer to character buffer data (array of characters in the format given
 * by #WORD_STRBUF_FORMAT).
 *
 * @param word  Word to access.
 */
#define WORD_STRBUF_BUFFER(word)        ((const char *)(word)+STRBUF_HEADER_SIZE)

/* End of String Buffer Accessors *//*!\}*/


/***************************************************************************//*!
 * \name String Buffer Exceptions
 ***************************************************************************\{*/

/**
 * Type checking macro for string buffers.
 *
 * @param word  Checked word.
 *
 * @typecheck{COL_ERROR_STRBUF,word}
 */
#define TYPECHECK_STRBUF(word) \
    TYPECHECK((Col_WordType(word) & COL_STRBUF), COL_ERROR_STRBUF, (word))

/* End of String Buffer Exceptions *//*!\}*/

/* End of String Buffers *//*!\}*/

#endif /* _COLIBRI_STRBUF_INT */
/*! @endcond @endprivate */