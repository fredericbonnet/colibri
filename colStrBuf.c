/**
 * @file colStrBuf.c
 *
 * This file implements the string buffer handling features of Colibri.
 *
 * String buffers are used to build strings incrementally in an efficient
 * manner, by appending individual characters or whole ropes. The current
 * accumulated rope can be retrieved at any time.
 *
 * @see colStrBuf.h
 */

#include "include/colibri.h"
#include "colInternal.h"

#include "colWordInt.h"
#include "colRopeInt.h"
#include "colStrBufInt.h"

#include <string.h>

/*
 * Prototypes for functions used only in this file.
 */

/*! \cond IGNORE */
static void             CommitBuffer(Col_Word strbuf);
/*! \endcond *//* IGNORE */


/*
===========================================================================*//*!
\weakgroup strbuf_words String Buffers
\{*//*==========================================================================
*/

/*******************************************************************************
 * String Buffer Creation
 ******************************************************************************/

/** @beginprivate @cond PRIVATE */

/**
 * Default size for string buffers.
 *
 * @see Col_NewStringBuffer
 */
#define STRBUF_DEFAULT_SIZE     5

/** @endcond @endprivate */

/**
 * Get the maximum buffer length of string buffers.
 *
 * @return The max string buffer length.
 */
size_t
Col_MaxStringBufferLength(
    Col_StringFormat format)    /*!< Format policy. */
{
    return STRBUF_MAX_LENGTH(STRBUF_MAX_SIZE * CELL_SIZE, format);
}

/**
 * Create a new string buffer word.
 *
 * @note
 *      The actual maximum length will be rounded up to fit an even number of
 *      cells, and won't exceed the maximum value given by
 *      [Col_MaxStringBufferLength(format)](@ref Col_MaxStringBufferLength).
 *
 * @return The new word.
 */
Col_Word
Col_NewStringBuffer(
    size_t maxLength,           /*!< Maximum length of string buffer. If zero,
                                     use a default value. */
    Col_StringFormat format)    /*!< String format. */
{
    Col_Word strbuf;            /* Resulting word in the general case. */
    size_t size;                /* Number of allocated cells storing a minimum
                                 * of maxLength elements. */

    /*
     * Create a new string buffer word.
     */

    if (maxLength == 0) {
        size = STRBUF_DEFAULT_SIZE;
    } else {
        size = STRBUF_SIZE(maxLength * CHAR_WIDTH(format));
        if (size > STRBUF_MAX_SIZE) size = STRBUF_MAX_SIZE;
    }
    strbuf = (Col_Word) AllocCells(size);
    WORD_STRBUF_INIT(strbuf, size, format);

    return strbuf;
}

/* End of String Buffer Creation */


/*******************************************************************************
 * String Buffer Accessors
 ******************************************************************************/

/**
 * Get the character format used by the internal buffer.
 *
 * @return The string buffer format.
 *
 * @see Col_NewStringBuffer
 */
Col_StringFormat
Col_StringBufferFormat(
    Col_Word strbuf)    /*!< String buffer to get length for. */
{
    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return 0;

    WORD_UNWRAP(strbuf);

    return (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);
}

/**
 * Get the maximum length of the string buffer.
 *
 * @return The string buffer maximum length.
 *
 * @see Col_NewStringBuffer
 */
size_t
Col_StringBufferMaxLength(
    Col_Word strbuf)    /*!< String buffer to get maximum length for. */
{
    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return 0;

    WORD_UNWRAP(strbuf);

    return STRBUF_MAX_LENGTH(WORD_STRBUF_SIZE(strbuf) * CELL_SIZE,
            WORD_STRBUF_FORMAT(strbuf));
}

/**
 * Get the current length of the string buffer.
 *
 * @return The string buffer length.
 */
size_t
Col_StringBufferLength(
    Col_Word strbuf)    /*!< String buffer to get length for. */
{
    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return 0;

    WORD_UNWRAP(strbuf);

    /*
     * String buffer length is the cumulated string length plus the current
     * length of the character buffer.
     */

    return Col_RopeLength(WORD_STRBUF_ROPE(strbuf))
            + WORD_STRBUF_LENGTH(strbuf);
}

/**
 * Get the current cumulated string.
 *
 * @return The string buffer string value.
 */
Col_Word
Col_StringBufferValue(
    Col_Word strbuf)    /*!< String buffer to get value for. */
{
    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return WORD_NIL;

    WORD_UNWRAP(strbuf);

    /*!
     * @sideeffect
     * Commit buffer into accumulated rope.
     */
    CommitBuffer(strbuf);
    return WORD_STRBUF_ROPE(strbuf);
}

/* End of String Buffer Accessors */


/*******************************************************************************
 * String Buffer Operations
 ******************************************************************************/

/** @beginprivate @cond PRIVATE */

/**
 * Maximum length a rope must have to be appended character-wise into the
 * buffer, instead of being appended as a whole.
 */
#define MAX_SHORT_ROPE_LENGTH(format) \
        ((size_t)(3*CELL_SIZE*CHAR_WIDTH(format)))

/**
 * Commit buffered characters into accumulated rope and clear buffer.
 *
 * @see Col_StringBufferFormat
 * @see Col_StringBufferAppendChar
 * @see Col_StringBufferAppendRope
 * @see Col_StringBufferAppendSequence
 * @see Col_StringBufferReserve
 */
static void
CommitBuffer(
    Col_Word strbuf)    /*!< String buffer to commit. */
{
    Col_Word rope;
    size_t length;
    Col_StringFormat format;

    /*
     * Check preconditions.
     */

    ASSERT(Col_WordType(strbuf) & COL_STRBUF);
    ASSERT(WORD_TYPE(strbuf) != WORD_TYPE_WRAP);

    if ((length = WORD_STRBUF_LENGTH(strbuf)) == 0) {
        /*
         * Nothing to commit.
         */

        return;
    }

    /*
     * Create rope from buffer and append to accumulated rope.
     */

    format = (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);
    rope = Col_NewRope(format, WORD_STRBUF_BUFFER(strbuf),
            length * CHAR_WIDTH(format));
    WORD_STRBUF_ROPE(strbuf) = Col_ConcatRopes(WORD_STRBUF_ROPE(strbuf), rope);
    WORD_STRBUF_LENGTH(strbuf) = 0;
}

/** @endcond @endprivate */

/**
 * Append the given character to the string buffer.
 *
 * @retval 0    if character was not appended (may occur when the character
 *              codepoint is unrepresentable in the target format).
 * @retval <>0  if character was appended.
 *
 * @see Col_StringBufferAppendRope
 * @see Col_StringBufferAppendSequence
 */
int
Col_StringBufferAppendChar(
    Col_Word strbuf,    /*!< String buffer to append character to. */
    Col_Char c)         /*!< Character to append. */
{
    Col_StringFormat format;
    size_t maxLength;
    size_t width;

    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return 0;

    WORD_UNWRAP(strbuf);

    /*
     * Get character width.
     */

    format = (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);
    switch (format) {
    case COL_UCS1:
        width = (c <= COL_CHAR1_MAX) ? 1 : 0;
        break;

    case COL_UCS2:
        width = (c <= COL_CHAR2_MAX) ? 1 : 0;
        break;

    case COL_UTF8:
        width = COL_UTF8_WIDTH(c);
        break;

    case COL_UTF16:
        width = COL_UTF16_WIDTH(c);
        break;

    default:
        width = 1;
    }
    if (width == 0) {
        /*
         * Lost character.
         */

        return 0;
    }

    maxLength = STRBUF_MAX_LENGTH(WORD_STRBUF_SIZE(strbuf) * CELL_SIZE, format);
    if (WORD_STRBUF_LENGTH(strbuf) + width > maxLength) {
        /*
         * Buffer is full, commit first.
         */

        /*!
         * @sideeffect
         * May commit buffer.
         */
        CommitBuffer(strbuf);
        ASSERT(WORD_STRBUF_LENGTH(strbuf) == 0);
    }

    /*
     * Append character into buffer.
     */

    switch (format) {
    case COL_UCS1:
        ((Col_Char1 *) WORD_STRBUF_BUFFER(strbuf))[WORD_STRBUF_LENGTH(strbuf)]
                = c;
        break;

    case COL_UCS2:
        ((Col_Char2 *) WORD_STRBUF_BUFFER(strbuf))[WORD_STRBUF_LENGTH(strbuf)]
                = c;
        break;

    case COL_UTF8:
        Col_Utf8Set(((Col_Char1 *) WORD_STRBUF_BUFFER(strbuf))
                + WORD_STRBUF_LENGTH(strbuf), c);
        break;

    case COL_UTF16:
        Col_Utf16Set(((Col_Char2 *) WORD_STRBUF_BUFFER(strbuf))
                + WORD_STRBUF_LENGTH(strbuf), c);
        break;

    default:
        ((Col_Char4 *) WORD_STRBUF_BUFFER(strbuf))[WORD_STRBUF_LENGTH(strbuf)]
                = c;
    }
    WORD_STRBUF_LENGTH(strbuf) += width;
    return 1;
}

/**
 * Append the given rope to the string buffer.
 *
 * @retval 0    if not all characters were appended (may occur when some
 *              character codepoints are unrepresentable in the target
 *              format).
 * @retval <>0  if all characters were appended.
 *
 * @see Col_StringBufferAppendChar
 * @see Col_StringBufferAppendSequence
 */
int
Col_StringBufferAppendRope(
    Col_Word strbuf,    /*!< String buffer to append rope to. */
    Col_Word rope)      /*!< Rope to append. */
{
    Col_StringFormat format;
    size_t length, ropeLength;
    Col_Word rope2;

    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return 0;

    if ((ropeLength = Col_RopeLength(rope)) == 0) {
        /*
         * Nothing to do.
         */

        return 1;
    }

    WORD_UNWRAP(strbuf);

    format = (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);
    if ((length = WORD_STRBUF_LENGTH(strbuf)) > 0) {
        /*
         * Buffer is not empty.
         */

        if (ropeLength < MAX_SHORT_ROPE_LENGTH(format)
                && ropeLength <= STRBUF_MAX_LENGTH(WORD_STRBUF_SIZE(strbuf)
                * CELL_SIZE, format) - length) {
            /*
             * Rope is short and fits into buffer, append character-wise.
             */

            Col_RopeIterator it;
            int noLoss = 1;
            for (Col_RopeIterFirst(it, rope); !Col_RopeIterEnd(it);
                    Col_RopeIterNext(it)) {
                noLoss &= Col_StringBufferAppendChar(strbuf,
                        Col_RopeIterAt(it));
            }
            return noLoss;
        }

        /*
         * Commit buffer before appending rope.
         */

        /*!
         * @sideeffect
         * May commit buffer.
         */
        CommitBuffer(strbuf);
    }

    /*
     * Normalize & append rope.
     */

    ASSERT(WORD_STRBUF_LENGTH(strbuf) == 0);
    rope2 = Col_NormalizeRope(rope, format, COL_CHAR_INVALID, 0);
    WORD_STRBUF_ROPE(strbuf) = Col_ConcatRopes(WORD_STRBUF_ROPE(strbuf), rope2);
    return (Col_RopeLength(rope2) == ropeLength);
}

/**
 * Append the given rope sequence to the string buffer.
 *
 * @retval 0    if not all characters were appended (may occur when some
 *              character codepoints are unrepresentable in the target
 *              format).
 * @retval <>0  if all characters were appended.
 *
 * @see Col_StringBufferAppendChar
 * @see Col_StringBufferAppendRope
 */
int
Col_StringBufferAppendSequence(
    Col_Word strbuf,                /*!< String buffer to append character
                                         sequence to. */
    const Col_RopeIterator begin,   /*!< First character in sequence. */
    const Col_RopeIterator end)     /*!< First character past sequence end. */
{
    Col_StringFormat format;
    Col_Word rope;

    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return 0;

    if (Col_RopeIterCompare(begin, end) >= 0) {
        /*
         * Nothing to do.
         */

        return 1;
    }

    WORD_UNWRAP(strbuf);

    rope = Col_RopeIterRope(begin);
    format = (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);
    if (!rope || (Col_RopeIterIndex(end) - Col_RopeIterIndex(begin))
            <= STRBUF_MAX_LENGTH(WORD_STRBUF_SIZE(strbuf)
            * CELL_SIZE, format) - WORD_STRBUF_LENGTH(strbuf)) {
        /*
         * Sequence is in string mode or fits into buffer, append
         * character-wise.
         */

        Col_RopeIterator it;
        int noLoss = 1;
        for (Col_RopeIterSet(it, begin); Col_RopeIterCompare(it, end) < 0;
                Col_RopeIterNext(it)) {
            noLoss &= Col_StringBufferAppendChar(strbuf, Col_RopeIterAt(it));
        }
        return noLoss;
    }

    /*
     * Append subrope.
     */

    return Col_StringBufferAppendRope(strbuf, Col_Subrope(rope,
            Col_RopeIterIndex(begin), Col_RopeIterIndex(end)-1));
}

/**
 * Reserve a number of characters for direct buffer access. Characters can
 * then be accessed as array elements using the format given at creation time.
 *
 *
 * @retval NULL     if the buffer is too small to store the given number of
 *                  characters.
 * @retval pointer  to the beginning of the reserved area.
 */
void *
Col_StringBufferReserve(
    Col_Word strbuf,    /*!< String buffer to reserve into. */
    size_t length)      /*!< Number of characters to reserve. Must be <=
                             [Col_StringBufferMaxLength(strbuf)](@ref Col_StringBufferMaxLength).
                         */
{
    Col_StringFormat format;
    size_t maxLength;
    const char *data;

    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return NULL;

    WORD_UNWRAP(strbuf);

    format = (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);
    maxLength = STRBUF_MAX_LENGTH(WORD_STRBUF_SIZE(strbuf) * CELL_SIZE,
            format);
    if (length > maxLength) {
        /*
         * Not enough room.
         */

        return NULL;
    }

    ASSERT(WORD_STRBUF_LENGTH(strbuf) <= maxLength);
    if (length > maxLength - WORD_STRBUF_LENGTH(strbuf)) {
        /*
         * Not enough remaining space, commit buffer first.
         */

        /*!
         * @sideeffect
         * May commit buffer.
         */
        CommitBuffer(strbuf);
        ASSERT(WORD_STRBUF_LENGTH(strbuf) == 0);
    }

    /*
     * Reserve area by shifting current length.
     */

    ASSERT(WORD_STRBUF_LENGTH(strbuf) + length <= maxLength);
    data = WORD_STRBUF_BUFFER(strbuf)
            + CHAR_WIDTH(format) * WORD_STRBUF_LENGTH(strbuf);
    WORD_STRBUF_LENGTH(strbuf) += length;
    return (void *) data;
}

/**
 * Release previously reserved characters.
 */
void
Col_StringBufferRelease(
    Col_Word strbuf,    /*!< String buffer to release. */
    size_t length)      /*!< Number of characters to release. */
{
    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return;

    WORD_UNWRAP(strbuf);

    if (WORD_STRBUF_LENGTH(strbuf) < length) {
        WORD_STRBUF_LENGTH(strbuf) = 0;
    } else {
        /*!
         * @sideeffect
         * Decrease current length.
         */
        WORD_STRBUF_LENGTH(strbuf) -= length;
    }
}

/**
 * Empty the string buffer: reset buffer length & accumulated rope.
 */
void
Col_StringBufferReset(
    Col_Word strbuf)    /*!< String buffer to reset. */
{
    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return;

    WORD_UNWRAP(strbuf);

    WORD_STRBUF_ROPE(strbuf) = WORD_SMALLSTR_EMPTY;
    WORD_STRBUF_LENGTH(strbuf) = 0;
}

/**
 * Freeze the string buffer word, turning it into a rope.
 *
 * @return The accumulated rope so far.
 *
 * @sideeffect
 *      As the word may be transmuted into a rope, it can no longer be used as a
 *      string buffer.
 */
Col_Word
Col_StringBufferFreeze(
    Col_Word strbuf)    /*!< String buffer to freeze. */
{
    Col_StringFormat format;
    size_t length;
    int pinned;

    /*
     * Check preconditions.
     */

    /*! @typecheck{COL_ERROR_STRBUF,strbuf} */
    TYPECHECK_STRBUF(strbuf) return WORD_NIL;

    WORD_UNWRAP(strbuf);

    length = WORD_STRBUF_LENGTH(strbuf);
    if (length == 0 || WORD_STRBUF_ROPE(strbuf) != WORD_SMALLSTR_EMPTY) {
        /*
         * Buffer is empty or accumulated rope is not, return value.
         */

        return Col_StringBufferValue(strbuf);
    }

    /*
     * Turn word into rope.
     */

    pinned = WORD_PINNED(strbuf);
    format = (Col_StringFormat) WORD_STRBUF_FORMAT(strbuf);
    switch (format) {
    case COL_UCS1:
    case COL_UCS2:
    case COL_UCS4:
        if (length > (format == COL_UCS1 ? SMALLSTR_MAX_LENGTH : 1)
                && length <= UCSSTR_MAX_LENGTH) {
            /*
             * Convert word in-place.
             */

            ASSERT(UCSSTR_HEADER_SIZE <= STRBUF_HEADER_SIZE);
            WORD_UCSSTR_INIT(strbuf, format, length);
            if (pinned) {
                WORD_SET_PINNED(strbuf);
            }
            memcpy((void *) WORD_UCSSTR_DATA(strbuf),
                    WORD_STRBUF_BUFFER(strbuf),
                    length * CHAR_WIDTH(format));
            return strbuf;
        }
        break;

    case COL_UCS:
        if (length <= UCSSTR_MAX_LENGTH) {
            /*
             * Compute format.
             */

            Col_Char4 *data = (Col_Char4 *) WORD_STRBUF_BUFFER(strbuf);
            size_t i;
            format = COL_UCS1;
            for (i=0; i < length; i++) {
                if (data[i] > COL_CHAR2_MAX) {
                    format = COL_UCS4;
                    break;
                } else if (data[i] > COL_CHAR1_MAX) {
                    format = COL_UCS2;
                }
            }

            if (length <= (format == COL_UCS1 ? SMALLSTR_MAX_LENGTH : 1)) {
                /*
                 * Fits immediate string.
                 */

                break;
            }

            /*
             * Convert word in-place.
             */

            ASSERT(UCSSTR_HEADER_SIZE <= STRBUF_HEADER_SIZE);
            WORD_UCSSTR_INIT(strbuf, format, length);
            if (pinned) {
                WORD_SET_PINNED(strbuf);
            }
            if (format == COL_UCS1) {
                Col_Char1 *data1 = (Col_Char1 *) WORD_UCSSTR_DATA(strbuf);
                for (i=0; i < length; i++) {
                    *data1++ = *data++;
                }
            } else if (format == COL_UCS2) {
                Col_Char2 *data2 = (Col_Char2 *) WORD_UCSSTR_DATA(strbuf);
                for (i=0; i < length; i++) {
                    *data2++ = *data++;
                }
            } else {
                ASSERT(format == COL_UCS4);
                memcpy((void *) WORD_UCSSTR_DATA(strbuf),
                        WORD_STRBUF_BUFFER(strbuf),
                        length * CHAR_WIDTH(format));
            }
            return strbuf;
        }
        break;

    case COL_UTF8:
    case COL_UTF16:
        if (length * CHAR_WIDTH(format) <= UTFSTR_MAX_BYTELENGTH) {
            /*
             * Convert word in-place.
             */

            const char *begin = (const char *) WORD_STRBUF_BUFFER(strbuf),
                    *p;
            size_t charLength;
            for (p=begin, charLength=0; p < begin+length; charLength++) {
                switch (format) {
                case COL_UTF8:  p = (const char *) Col_Utf8Next ((const Col_Char1 *) p); break;
                case COL_UTF16: p = (const char *) Col_Utf16Next((const Col_Char2 *) p); break;
                }
            }
            WORD_UTFSTR_INIT(strbuf, COL_UTF8, charLength,
                    length * CHAR_WIDTH(format));
            if (pinned) {
                WORD_SET_PINNED(strbuf);
            }
            memcpy((void *) WORD_UTFSTR_DATA(strbuf),
                    WORD_STRBUF_BUFFER(strbuf),
                    length * CHAR_WIDTH(format));
            return strbuf;
        }
        break;
    }

    /*
     * Could not convert in-place (either too large or fits immediate word),
     * return value.
     */

    return Col_StringBufferValue(strbuf);
}

/* End of String Buffer Operations */

/* End of String Buffers *//*!\}*/
