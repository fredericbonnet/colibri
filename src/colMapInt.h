/**
 * @file colMapInt.h
 *
 * This header file defines the generic map word internals of Colibri.
 *
 * Maps are an associative collection datatype that associates keys to
 * values. Keys can be integers, strings or generic words. Values are
 * arbitrary words.
 *
 * They are always mutable.
 *
 * @see colMap.c
 * @see colMap.h
 *
 * @beginprivate @cond PRIVATE
 */

#ifndef _COLIBRI_MAP_INT
#define _COLIBRI_MAP_INT


/*
===========================================================================*//*!
\internal \defgroup mapentry_words Map Entries
\ingroup predefined_words map_words

Generic map entries are key-value pairs. Type-specific entries extend this
generic type with implementation-dependent data.

@par Requirements
    - Map entry words use one single cell.

    - Map entries must store at least a key and a value.

    @param Key      A word in the general case but can be a native integer for
                    integer maps.
    @param Value    A word.

@par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        mapentry_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td sides="R" colspan="2"></td>
            </tr>
            <tr><td sides="R">1</td>
                <td colspan="4" sides="BR" width="320">Type-specific data</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_MAPENTRY_KEY" title="WORD_MAPENTRY_KEY" colspan="4">Key</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_MAPENTRY_VALUE" title="WORD_MAPENTRY_VALUE" colspan="4">Value</td>
            </tr>
            </table>
        >]
    }
    @enddot

    @begindiagram
           0     7 8                                                     n
          +-------+-------------------------------------------------------+
        0 | Type  |                                                       |
          +-------+               Type-specific data                      +
        1 |                                                               |
          +---------------------------------------------------------------+
        2 |                              Key                              |
          +---------------------------------------------------------------+
        3 |                             Value                             |
          +---------------------------------------------------------------+
    @enddiagram

\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Map Entry Accessors
 ***************************************************************************\{*/

/**
 * Get/set word key of word-based map entry (string or custom).
 *
 * @param word  Word to access.
 *
 * @note
 *      Macro is L-Value and suitable for both read/write operations.
 */
#define WORD_MAPENTRY_KEY(word)         (((Col_Word *)(word))[2])

/**
 * Get/set value of map entry.
 *
 * @param word  Word to access.
 *
 * @note
 *      Macro is L-Value and suitable for both read/write operations.
 */
#define WORD_MAPENTRY_VALUE(word)       (((Col_Word *)(word))[3])

/* End of Map Entry Accessors *//*!\}*/


/***************************************************************************//*!
 * \name Map Entry Exceptions
 ***************************************************************************\{*/

/**
 * Type checking macro for maps.
 *
 * @param word  Checked word.
 *
 * @typecheck{COL_ERROR_MAP,word}
 * @hideinitializer
 */
#define TYPECHECK_MAP(word) \
    TYPECHECK((Col_WordType(word) & (COL_MAP | COL_INTMAP)), COL_ERROR_MAP, \
            (word))

/**
 * Type checking macro for word-based maps (string or custom).
 *
 * @param word  Checked word.
 *
 * @typecheck{COL_ERROR_WORDMAP,word}
 * @hideinitializer
 */
#define TYPECHECK_WORDMAP(word) \
    TYPECHECK(((Col_WordType(word) & (COL_MAP | COL_INTMAP)) == COL_MAP), \
            COL_ERROR_WORDMAP, (word))

/**
 * Type checking macro for integer maps.
 *
 * @param word  Checked word.
 *
 * @typecheck{COL_ERROR_INTMAP,word}
 * @hideinitializer
 */
#define TYPECHECK_INTMAP(word) \
    TYPECHECK((Col_WordType(word) & COL_INTMAP), COL_ERROR_INTMAP, (word))

/**
 * Type checking macro for map iterators.
 *
 * @param it    Checked iterator.
 *
 * @typecheck{COL_ERROR_MAPITER,it}
 *
 * @see Col_MapIterNull
 * @hideinitializer
 */
#define TYPECHECK_MAPITER(it) \
    TYPECHECK(!Col_MapIterNull(it), COL_ERROR_MAPITER, (it))

/**
 * Value checking macro for map iterators, ensures that iterator is not
 * at end.
 *
 * @param it    Checked iterator.
 *
 * @valuecheck{COL_ERROR_MAPITER_END,it}
 *
 * @see Col_MapIterEnd
 * @hideinitializer
 */
#define VALUECHECK_MAPITER(it) \
    VALUECHECK(!Col_MapIterEnd(it), COL_ERROR_MAPITER_END, (it))

/* End of Map Entry Exceptions *//*!\}*/

/* End of Map Entries *//*!\}*/


/*
===========================================================================*//*!
\internal \defgroup intmapentry_words Integer Map Entries
\ingroup predefined_words map_words

Generic integer map entries are key-value pairs. Type-specific entries extend
this generic type with implementation-dependent data.

@par Requirements
    - Map entry words use one single cell.

    - Map entries must store at least a key and a value.

    @param Key      A word in the general case but can be a native integer for
                    integer maps.
    @param Value    A word.

@par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        mapentry_word [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="40" align="left">0</td><td sides="B" width="40" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref WORD_TYPEID" title="WORD_TYPEID" colspan="2">Type</td>
                <td sides="R" colspan="2"></td>
            </tr>
            <tr><td sides="R">1</td>
                <td colspan="4" sides="BR" width="320">Type-specific data</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref WORD_INTMAPENTRY_KEY" title="WORD_INTMAPENTRY_KEY" colspan="4">Key</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref WORD_MAPENTRY_VALUE" title="WORD_MAPENTRY_VALUE" colspan="4">Value</td>
            </tr>
            </table>
        >]
    }
    @enddot

    @begindiagram
           0     7 8                                                     n
          +-------+-------------------------------------------------------+
        0 | Type  |                                                       |
          +-------+               Type-specific data                      +
        1 |                                                               |
          +---------------------------------------------------------------+
        2 |                              Key                              |
          +---------------------------------------------------------------+
        3 |                             Value                             |
          +---------------------------------------------------------------+
    @enddiagram

\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \name Integer Map Entry Accessors
 ***************************************************************************\{*/

/**
 * Get/set key of integer map entry.
 *
 * @param word  Word to access.
 *
 * @note
 *      Macro is L-Value and suitable for both read/write operations.
 */
#define WORD_INTMAPENTRY_KEY(word)      (((intptr_t *)(word))[2])

/* End of Integer Map Entry Accessors *//*!\}*/

/* End of Integer Map Entries *//*!\}*/

#endif /* _COLIBRI_MAP_INT */
/*! @endcond @endprivate */