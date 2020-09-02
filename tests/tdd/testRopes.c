#include <colibri.h>
#include <picotest.h>

/*
 * Failure test cases (must be defined before test hooks)
 */

#include "failureFixture.h"

/* Col_NormalizeRope */
PICOTEST_CASE(normalizeRope_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_NormalizeRope(WORD_NIL, COL_UCS, COL_CHAR_INVALID, 0) ==
                    WORD_NIL);
}

/* Col_CharWordValue */
PICOTEST_CASE(charWordValue_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_CHAR);
    PICOTEST_ASSERT(Col_CharWordValue(WORD_NIL) == COL_CHAR_INVALID);
}

/* Col_StringWordFormat */
PICOTEST_CASE(stringWordFormat_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_STRING);
    PICOTEST_ASSERT(Col_StringWordFormat(WORD_NIL) == 0);
}

/* Col_RopeLength */
PICOTEST_CASE(ropeLength_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_RopeLength(WORD_NIL) == 0);
}

/* Col_RopeDepth */
PICOTEST_CASE(ropeDepth_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_RopeDepth(WORD_NIL) == 0);
}

/* Col_RopeAt */
PICOTEST_CASE(ropeAt_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_RopeAt(WORD_NIL, 0) == COL_CHAR_INVALID);
}

/* Col_RopeFind */
PICOTEST_CASE(ropeFind_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_RopeFind(WORD_NIL, 'a', 0, SIZE_MAX, 0) == SIZE_MAX);
}

/* Col_RopeSearch */
PICOTEST_CASE(ropeSearch_typeCheck_rope, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_RopeSearch(WORD_NIL, Col_EmptyRope(), 0, SIZE_MAX, 0) ==
                    SIZE_MAX);
}
PICOTEST_CASE(ropeSearch_typeCheck_subrope, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_RopeSearch(Col_EmptyRope(), WORD_NIL, 0, SIZE_MAX, 0) ==
                    SIZE_MAX);
}

/* Col_CompareRopesL */
PICOTEST_CASE(compareRopesL_typecheck_rope1, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_CompareRopesL(WORD_NIL, Col_EmptyRope(), 0, SIZE_MAX,
                                      NULL, NULL, NULL) == 1);
}
PICOTEST_CASE(compareRopesL_typecheck_rope2, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_CompareRopesL(Col_EmptyRope(), WORD_NIL, 0, SIZE_MAX,
                                      NULL, NULL, NULL) == -1);
}

/* Col_Subrope */
PICOTEST_CASE(subrope_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_Subrope(WORD_NIL, 0, 0) == WORD_NIL);
}

/* Col_ConcatRopes */
PICOTEST_CASE(concatRopes_typeCheck_left, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_ConcatRopes(WORD_NIL, Col_EmptyRope()) == WORD_NIL);
}
PICOTEST_CASE(concatRopes_typeCheck_right, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_ConcatRopes(Col_EmptyRope(), WORD_NIL) == WORD_NIL);
}
PICOTEST_CASE(concatRopes_valueCheck_length, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPELENGTH_CONCAT);
    Col_Word rope = Col_RepeatRope(Col_NewCharWord('a'), SIZE_MAX);
    PICOTEST_ASSERT(Col_ConcatRopes(rope, rope) == WORD_NIL);
}

/* Col_ConcatRopesA */
PICOTEST_CASE(concatRopesA_valueCheck_number, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_GENERIC);
    PICOTEST_ASSERT(Col_ConcatRopesA(0, NULL) == WORD_NIL);
}

/* Col_ConcatRopesNV */
PICOTEST_CASE(concatRopesNV_valueCheck_number, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_GENERIC);
    PICOTEST_ASSERT(Col_ConcatRopesNV(0) == WORD_NIL);
}

/* Col_RepeatRopes */
PICOTEST_CASE(repeatRope_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_RepeatRope(WORD_NIL, 0) == WORD_NIL);
}
PICOTEST_CASE(repeatRope_valueCheck_length, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPELENGTH_REPEAT);
    Col_Word rope = Col_RepeatRope(Col_NewCharWord('a'), 2);
    PICOTEST_ASSERT(Col_RepeatRope(rope, SIZE_MAX) == WORD_NIL);
}

/* Col_RopeInsert */
PICOTEST_CASE(ropeInsert_typeCheck_into, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_RopeInsert(WORD_NIL, 0, Col_EmptyRope()) == WORD_NIL);
}
PICOTEST_CASE(ropeInsert_typeCheck_rope, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_RopeInsert(Col_EmptyRope(), 0, WORD_NIL) == WORD_NIL);
}

/* Col_RopeRemove */
PICOTEST_CASE(ropeRemove_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_RopeRemove(WORD_NIL, 0, 0) == WORD_NIL);
}

/* Col_RopeReplace */
PICOTEST_CASE(ropeReplace_typeCheck_rope, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_RopeReplace(WORD_NIL, 0, 0, Col_EmptyRope()) ==
                    WORD_NIL);
}
PICOTEST_CASE(ropeReplace_typeCheck_with, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(Col_RopeReplace(Col_EmptyRope(), 0, 0, WORD_NIL) ==
                    WORD_NIL);
}

/* Col_TraverseRopeChunksN */
PICOTEST_CASE(traverseRopeChunksN_valueCheck_proc, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_GENERIC);
    Col_Word ropes[] = {Col_EmptyRope()};
    PICOTEST_ASSERT(Col_TraverseRopeChunksN(1, ropes, 0, 0, NULL, NULL, NULL) ==
                    -1);
}

/* Col_TraverseRopeChunks */
PICOTEST_CASE(traverseRopeChunks_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    PICOTEST_ASSERT(
        Col_TraverseRopeChunks(WORD_NIL, 0, 0, 0, NULL, NULL, NULL) == -1);
}
PICOTEST_CASE(traverseRopeChunks_valueCheck_proc, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_GENERIC);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(Col_EmptyRope(), 0, 0, 0, NULL, NULL,
                                           NULL) == -1);
}

/* Col_RopeIterBegin */
PICOTEST_CASE(ropeIterBegin_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    Col_RopeIterator it;
    Col_RopeIterBegin(it, WORD_NIL, 0);
    PICOTEST_ASSERT(Col_RopeIterNull(it));
}

/* Col_RopeIterFirst */
PICOTEST_CASE(ropeIterFirst_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    Col_RopeIterator it;
    Col_RopeIterFirst(it, WORD_NIL);
    PICOTEST_ASSERT(Col_RopeIterNull(it));
}

/* Col_RopeIterLast */
PICOTEST_CASE(ropeIterLast_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPE);
    Col_RopeIterator it;
    Col_RopeIterLast(it, WORD_NIL);
    PICOTEST_ASSERT(Col_RopeIterNull(it));
}

/* Col_RopeIterCompare */
PICOTEST_CASE(ropeIterCompare_typeCheck_it1, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPEITER);
    Col_RopeIterator it1 = COL_ROPEITER_NULL;
    Col_RopeIterator it2;
    Col_RopeIterFirst(it2, Col_NewCharWord('a'));
    PICOTEST_ASSERT(Col_RopeIterCompare(it1, it2) == 1);
}
PICOTEST_CASE(ropeIterCompare_typeCheck_it2, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPEITER);
    Col_RopeIterator it1;
    Col_RopeIterFirst(it1, Col_NewCharWord('a'));
    Col_RopeIterator it2 = COL_ROPEITER_NULL;
    PICOTEST_ASSERT(Col_RopeIterCompare(it1, it2) == -1);
}

/* Col_RopeIterForward */
PICOTEST_CASE(ropeIterForward_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPEITER);
    Col_RopeIterator it = COL_ROPEITER_NULL;
    Col_RopeIterForward(it, 0);
    PICOTEST_ASSERT(Col_RopeIterNull(it));
}
PICOTEST_CASE(ropeIterForward_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPEITER_END);
    Col_RopeIterator it;
    Col_RopeIterFirst(it, Col_EmptyRope());
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
    Col_RopeIterForward(it, 1);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
}

/* Col_RopeIterBackward */
PICOTEST_CASE(ropeIterBackward_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPEITER);
    Col_RopeIterator it = COL_ROPEITER_NULL;
    Col_RopeIterBackward(it, 0);
    PICOTEST_ASSERT(Col_RopeIterNull(it));
}

/* Col_RopeIterAt */
PICOTEST_CASE(ropeIterAt_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPEITER_END);
    Col_RopeIterator it;
    Col_RopeIterFirst(it, Col_EmptyRope());
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
    PICOTEST_ASSERT(Col_RopeIterAt(it) == COL_CHAR_INVALID);
}

/* Col_RopeIterNext */
PICOTEST_CASE(ropeIterNext_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPEITER);
    Col_RopeIterator it = COL_ROPEITER_NULL;
    Col_RopeIterNext(it);
    PICOTEST_ASSERT(Col_RopeIterNull(it));
}
PICOTEST_CASE(ropeIterNext_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPEITER_END);
    Col_RopeIterator it;
    Col_RopeIterFirst(it, Col_EmptyRope());
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
    Col_RopeIterNext(it);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
}

/* Col_RopeIterPrevious */
PICOTEST_CASE(ropeIterPrevious_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_ROPEITER);
    Col_RopeIterator it = COL_ROPEITER_NULL;
    Col_RopeIterPrevious(it);
    PICOTEST_ASSERT(Col_RopeIterNull(it));
}

/*
 * Utilities
 */

#define DECLARE_ROPE_DATA_UCS(name, type, len, first, next)                    \
    type name[len];                                                            \
    name[0] = first;                                                           \
    for (int i = 1; i < len; i++) {                                            \
        name[i] = next;                                                        \
    }
#define NEW_ROPE_UCS(type, format, len, first, next)                           \
    {                                                                          \
        DECLARE_ROPE_DATA_UCS(data, type, len, first, next);                   \
        return Col_NewRope(format, data, sizeof(data));                        \
    }
#define DECLARE_ROPE_DATA_UTF8(name, len, first, next)                         \
    Col_Char1 name[len * COL_UTF8_MAX_WIDTH], *p = data;                       \
    p = Col_Utf8Set(p, first);                                                 \
    for (int i = 1; i < len; i++) {                                            \
        p = Col_Utf8Set(p, next);                                              \
    }
#define NEW_ROPE_UTF8(len, first, next)                                        \
    {                                                                          \
        DECLARE_ROPE_DATA_UTF8(data, len, first, next);                        \
        return Col_NewRope(COL_UTF8, data, (p - data) * sizeof(*data));        \
    }
#define DECLARE_ROPE_DATA_UTF16(name, len, first, next)                        \
    Col_Char2 name[len * COL_UTF16_MAX_WIDTH], *p = data;                      \
    p = Col_Utf16Set(p, first);                                                \
    for (int i = 1; i < len; i++) {                                            \
        p = Col_Utf16Set(p, next);                                             \
    }
#define NEW_ROPE_UTF16(len, first, next)                                       \
    {                                                                          \
        DECLARE_ROPE_DATA_UTF16(data, len, first, next);                       \
        return Col_NewRope(COL_UTF16, data, (p - data) * sizeof(*data));       \
    }
#define NEW_ROPE_STRING(len, first, next)                                      \
    {                                                                          \
        DECLARE_ROPE_DATA_UCS(data, char, len + 1, first, next);               \
        data[len] = 0;                                                         \
        return Col_NewRopeFromString(data);                                    \
    }

/*
 * Test data
 */

/* Rope creation */
static Col_Word NEW_ROPE_UCS1_CHAR(c)
    NEW_ROPE_UCS(Col_Char1, COL_UCS1, 1, c, c);
static Col_Word NEW_ROPE_UCS2_CHAR(c)
    NEW_ROPE_UCS(Col_Char2, COL_UCS2, 1, c, c);
static Col_Word NEW_ROPE_UCS4_CHAR(c)
    NEW_ROPE_UCS(Col_Char4, COL_UCS4, 1, c, c);
static Col_Word NEW_ROPE_UCS_CHAR(c) NEW_ROPE_UCS(Col_Char, COL_UCS, 1, c, c);

static Col_Word NEW_ROPE_STRING_CHAR(c) NEW_ROPE_STRING(1, c, c);

#define ROPE_SMALL_LEN (sizeof(Col_Word) - 1)
static Col_Word NEW_ROPE_UCS1_SMALL()
    NEW_ROPE_UCS(Col_Char1, COL_UCS1, ROPE_SMALL_LEN, 'a', 'a' + i);
static Col_Word NEW_ROPE_UCS_SMALL()
    NEW_ROPE_UCS(Col_Char, COL_UCS, ROPE_SMALL_LEN, 'a', 'a' + i);

static Col_Word NEW_ROPE_STRING_SMALL()
    NEW_ROPE_STRING(ROPE_SMALL_LEN, 'a', 'a' + i);

#define ROPE_UCS_LEAF_LEN (ROPE_SMALL_LEN + 1)
static Col_Word NEW_ROPE_UCS1_LEAF()
    NEW_ROPE_UCS(Col_Char1, COL_UCS1, ROPE_UCS_LEAF_LEN, 'a', 'a' + i);
static Col_Word NEW_ROPE_UCS2_LEAF()
    NEW_ROPE_UCS(Col_Char2, COL_UCS2, ROPE_UCS_LEAF_LEN, 'a', 'a' + i);
static Col_Word NEW_ROPE_UCS4_LEAF()
    NEW_ROPE_UCS(Col_Char4, COL_UCS4, ROPE_UCS_LEAF_LEN, 'a', 'a' + i);
static Col_Word NEW_ROPE_UCS_LEAF_1()
    NEW_ROPE_UCS(Col_Char, COL_UCS, ROPE_UCS_LEAF_LEN, COL_CHAR1_MAX, 'a' + i);
static Col_Word NEW_ROPE_UCS_LEAF_2()
    NEW_ROPE_UCS(Col_Char, COL_UCS, ROPE_UCS_LEAF_LEN, COL_CHAR2_MAX, 'a' + i);
static Col_Word NEW_ROPE_UCS_LEAF_4()
    NEW_ROPE_UCS(Col_Char, COL_UCS, ROPE_UCS_LEAF_LEN, COL_CHAR4_MAX, 'a' + i);

#define ROPE_UTF_LEAF_LEN 1
static Col_Word NEW_ROPE_UTF8_LEAF()
    NEW_ROPE_UTF8(ROPE_UTF_LEAF_LEN, 'a', 'a' + i);
static Col_Word NEW_ROPE_UTF16_LEAF()
    NEW_ROPE_UTF16(ROPE_UTF_LEAF_LEN, 'a', 'a' + i);

#define ROPE_STRING_LEAF_LEN ROPE_UCS_LEAF_LEN
static Col_Word NEW_ROPE_STRING_LEAF()
    NEW_ROPE_STRING(ROPE_STRING_LEAF_LEN, 'a', 'a' + i);

#define ROPE_UCS_BIG_LEN 65536
static Col_Word NEW_ROPE_UCS1_BIG()
    NEW_ROPE_UCS(Col_Char1, COL_UCS1, ROPE_UCS_BIG_LEN, 'a', 'a' + i);
static Col_Word NEW_ROPE_UCS2_BIG()
    NEW_ROPE_UCS(Col_Char2, COL_UCS2, ROPE_UCS_BIG_LEN, 'a', 'a' + i);
static Col_Word NEW_ROPE_UCS4_BIG()
    NEW_ROPE_UCS(Col_Char4, COL_UCS4, ROPE_UCS_BIG_LEN, 'a', 'a' + i);
static Col_Word NEW_ROPE_UCS_BIG()
    NEW_ROPE_UCS(Col_Char, COL_UCS, ROPE_UCS_BIG_LEN, 'a', 'a' + i);

#define ROPE_UTF_BIG_LEN 4096
static Col_Word NEW_ROPE_UTF8_BIG()
    NEW_ROPE_UTF8(ROPE_UTF_BIG_LEN, 'a', 'a' + i);
static Col_Word NEW_ROPE_UTF16_BIG()
    NEW_ROPE_UTF16(ROPE_UTF_BIG_LEN, 'a', 'a' + i);

#define ROPE_STRING_BIG_LEN ROPE_UCS_BIG_LEN
static Col_Word NEW_ROPE_STRING_BIG()
    NEW_ROPE_STRING(ROPE_UCS_BIG_LEN, 'a', 'a');

/* Rope operations */
#define SMALL_STRING_LEN ROPE_SMALL_LEN
#define SMALL_STRING() NEW_ROPE_UCS1_SMALL()
#define SHORT_STRING_LEN 10
#define SHORT_STRING() SHORT_STRING_UCS1()
static Col_Word SHORT_STRING_UCS1()
    NEW_ROPE_UCS(Col_Char1, COL_UCS1, SHORT_STRING_LEN, 'a', 'a' + i);
static Col_Word SHORT_STRING_UCS2()
    NEW_ROPE_UCS(Col_Char2, COL_UCS2, SHORT_STRING_LEN, 'a', 'a' + i);
static Col_Word SHORT_STRING_UCS4()
    NEW_ROPE_UCS(Col_Char4, COL_UCS4, SHORT_STRING_LEN, 'a', 'a' + i);
static Col_Word SHORT_STRING_UTF8()
    NEW_ROPE_UTF8(SHORT_STRING_LEN, 'a', 'a' + i);
static Col_Word SHORT_STRING_UTF16()
    NEW_ROPE_UTF16(SHORT_STRING_LEN, 'a', 'a' + i);
#define MEDIUM_STRING_UCS1_LEN 80
static Col_Word MEDIUM_STRING_UCS1()
    NEW_ROPE_UCS(Col_Char1, COL_UCS1, MEDIUM_STRING_UCS1_LEN, 'a', 'a' + i);
#define MEDIUM_STRING_UCS2_LEN 40
static Col_Word MEDIUM_STRING_UCS2()
    NEW_ROPE_UCS(Col_Char2, COL_UCS2, MEDIUM_STRING_UCS2_LEN, 'a', 'a' + i);
#define MEDIUM_STRING_UCS4_LEN 20
static Col_Word MEDIUM_STRING_UCS4()
    NEW_ROPE_UCS(Col_Char4, COL_UCS4, MEDIUM_STRING_UCS4_LEN, 'a', 'a' + i);
#define MEDIUM_STRING_UTF8_LEN 40
static Col_Word MEDIUM_STRING_UTF8()
    NEW_ROPE_UTF8(MEDIUM_STRING_UTF8_LEN, COL_CHAR1_MAX, COL_CHAR1_MAX + i);
#define MEDIUM_STRING_UTF16_LEN 20
static Col_Word MEDIUM_STRING_UTF16()
    NEW_ROPE_UTF16(MEDIUM_STRING_UTF16_LEN, COL_CHAR2_MAX, COL_CHAR2_MAX + i);
#define FLAT_STRING_LEN 200
#define FLAT_STRING() FLAT_STRING_UCS1()
static Col_Word FLAT_STRING_UCS1()
    NEW_ROPE_UCS(Col_Char1, COL_UCS1, FLAT_STRING_LEN, 'a', 'a' + i);
static Col_Word FLAT_STRING_UCS2()
    NEW_ROPE_UCS(Col_Char2, COL_UCS2, FLAT_STRING_LEN, COL_CHAR1_MAX,
                 COL_CHAR1_MAX + i);
static Col_Word FLAT_STRING_UCS4()
    NEW_ROPE_UCS(Col_Char4, COL_UCS4, FLAT_STRING_LEN, COL_CHAR2_MAX,
                 COL_CHAR2_MAX + i);
static Col_Word FLAT_STRING_UTF8()
    NEW_ROPE_UTF8(FLAT_STRING_LEN, COL_CHAR1_MAX, COL_CHAR1_MAX + i);
static Col_Word FLAT_STRING_UTF16()
    NEW_ROPE_UTF16(FLAT_STRING_LEN, COL_CHAR1_MAX, COL_CHAR1_MAX + i);
#define BIG_STRING_UCS_LEN ROPE_UCS_BIG_LEN
#define BIG_STRING_UCS() NEW_ROPE_UCS4_BIG()
#define BIG_STRING_UTF_LEN ROPE_UTF_BIG_LEN
#define BIG_STRING_UTF() NEW_ROPE_UTF8_BIG()

/*
 * Ropes
 */

#include "hooks.h"
#include "colibriFixture.h"

PICOTEST_SUITE(testRopes, testRopeTypeChecks, testEmptyRope, testCharacterWords,
               testRopeCreation, testRopeOperations, testRopeTraversal,
               testRopeIteration, testRopeSearchAndComparison);

PICOTEST_CASE(testRopeTypeChecks, colibriFixture) {
    PICOTEST_VERIFY(normalizeRope_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(stringWordFormat_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(ropeLength_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(ropeDepth_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(ropeAt_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(ropeFind_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(ropeSearch_typeCheck_rope(NULL) == 1);
    PICOTEST_VERIFY(ropeSearch_typeCheck_subrope(NULL) == 1);
    PICOTEST_VERIFY(compareRopesL_typecheck_rope1(NULL) == 1);
    PICOTEST_VERIFY(compareRopesL_typecheck_rope2(NULL) == 1);
    PICOTEST_VERIFY(subrope_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(concatRopes_typeCheck_left(NULL) == 1);
    PICOTEST_VERIFY(concatRopes_typeCheck_right(NULL) == 1);
    PICOTEST_VERIFY(repeatRope_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(ropeInsert_typeCheck_into(NULL) == 1);
    PICOTEST_VERIFY(ropeInsert_typeCheck_rope(NULL) == 1);
    PICOTEST_VERIFY(ropeRemove_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(ropeReplace_typeCheck_rope(NULL) == 1);
    PICOTEST_VERIFY(ropeReplace_typeCheck_with(NULL) == 1);
    PICOTEST_VERIFY(traverseRopeChunks_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(ropeIterBegin_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(ropeIterFirst_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(ropeIterLast_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(ropeIterCompare_typeCheck_it1(NULL) == 1);
    PICOTEST_VERIFY(ropeIterCompare_typeCheck_it2(NULL) == 1);
    PICOTEST_VERIFY(ropeIterForward_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(ropeIterBackward_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(ropeIterNext_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(ropeIterPrevious_typeCheck(NULL) == 1);
}

/* Empty rope */
PICOTEST_SUITE(testEmptyRope, testEmptyRopeConstant, testEmptyRopeIsImmediate,
               testEmptyRopeInvariants);

PICOTEST_CASE(testEmptyRopeConstant, colibriFixture) {
    Col_Word empty = Col_EmptyRope();
    PICOTEST_ASSERT(Col_WordType(empty) == (COL_STRING | COL_ROPE));
}
PICOTEST_CASE(testEmptyRopeIsImmediate, colibriFixture) {
    PICOTEST_ASSERT(Col_EmptyRope() == Col_EmptyRope());
}
PICOTEST_CASE(testEmptyRopeInvariants, colibriFixture) {
    Col_Word empty = Col_EmptyRope();
    PICOTEST_ASSERT(Col_RopeLength(empty) == 0);
    PICOTEST_ASSERT(Col_RopeDepth(empty) == 0);
    PICOTEST_ASSERT(Col_StringWordFormat(empty) == COL_UCS1);
}

/* Characters */
PICOTEST_SUITE(testCharacterWords, testCharacterTypeCheck, testNewCharWord,
               testCharWordsAreImmediate);

PICOTEST_CASE(testCharacterTypeCheck, colibriFixture) {
    PICOTEST_ASSERT(charWordValue_typeCheck(NULL) == 1);
}

PICOTEST_SUITE(testNewCharWord, testNewCharWordUcs1, testNewCharWordUcs2,
               testNewCharWordUcs4, testNewCharWordInvalid);
PICOTEST_CASE(testNewCharWordUcs1, colibriFixture) {
    Col_Word c = Col_NewCharWord(COL_CHAR1_MAX);
    PICOTEST_ASSERT(Col_WordType(c) == (COL_CHAR | COL_STRING | COL_ROPE));
    PICOTEST_ASSERT(Col_CharWordValue(c) == COL_CHAR1_MAX);
    PICOTEST_ASSERT(Col_StringWordFormat(c) == COL_UCS1);
}
PICOTEST_CASE(testNewCharWordUcs2, colibriFixture) {
    Col_Word c = Col_NewCharWord(COL_CHAR2_MAX);
    PICOTEST_ASSERT(Col_WordType(c) == (COL_CHAR | COL_STRING | COL_ROPE));
    PICOTEST_ASSERT(Col_CharWordValue(c) == COL_CHAR2_MAX);
    PICOTEST_ASSERT(Col_StringWordFormat(c) == COL_UCS2);
}
PICOTEST_CASE(testNewCharWordUcs4, colibriFixture) {
    Col_Word c = Col_NewCharWord(COL_CHAR4_MAX);
    PICOTEST_ASSERT(Col_WordType(c) == (COL_CHAR | COL_STRING | COL_ROPE));
    PICOTEST_ASSERT(Col_CharWordValue(c) == COL_CHAR4_MAX);
    PICOTEST_ASSERT(Col_StringWordFormat(c) == COL_UCS4);
}
PICOTEST_CASE(testNewCharWordInvalid, colibriFixture) {
    Col_Word c = Col_NewCharWord(-1);
    PICOTEST_ASSERT(c == Col_EmptyRope());
}
PICOTEST_CASE(testCharWordsAreImmediate, colibriFixture) {
    PICOTEST_ASSERT(Col_NewCharWord(COL_CHAR1_MAX) ==
                    Col_NewCharWord(COL_CHAR1_MAX));
    PICOTEST_ASSERT(Col_NewCharWord(COL_CHAR2_MAX) ==
                    Col_NewCharWord(COL_CHAR2_MAX));
    PICOTEST_ASSERT(Col_NewCharWord(COL_CHAR4_MAX) ==
                    Col_NewCharWord(COL_CHAR4_MAX));
}

/* Rope creation */
PICOTEST_SUITE(testRopeCreation, testNewRope, testNewRopeFromString,
               testNormalizeRope);
PICOTEST_SUITE(testNewRope, testNewRopeEmpty, testNewRopeUcs1, testNewRopeUcs2,
               testNewRopeUcs4, testNewRopeUcs, testNewRopeUtf8,
               testNewRopeUtf16);
PICOTEST_SUITE(testNewRopeEmpty, testNewRopeEmptyFromData,
               testNewRopeEmptyFromString);
PICOTEST_SUITE(testNewRopeUcs1, testNewRopeUcs1Char, testNewRopeUcs1Small,
               testNewRopeUcs1Flat, testNewRopeUcs1Big);
PICOTEST_SUITE(testNewRopeUcs2, testNewRopeUcs2Char, testNewRopeUcs2Flat,
               testNewRopeUcs2Big);
PICOTEST_SUITE(testNewRopeUcs4, testNewRopeUcs4Char, testNewRopeUcs4Flat,
               testNewRopeUcs4Big);
PICOTEST_SUITE(testNewRopeUcs, testNewRopeUcsChar, testNewRopeUcsSmall,
               testNewRopeUcsFlat, testNewRopeUcsBig);
PICOTEST_SUITE(testNewRopeUtf8, testNewRopeUtf8Flat, testNewRopeUtf8Big);
PICOTEST_SUITE(testNewRopeUtf16, testNewRopeUtf16Flat, testNewRopeUtf16Big);
PICOTEST_SUITE(testNewRopeFromString, testNewRopeFromStringChar,
               testNewRopeFromStringSmall, testNewRopeFromStringFlat,
               testNewRopeFromStringBig);

static void checkRopeEmpty(Col_Word rope) {
    PICOTEST_ASSERT(rope == Col_EmptyRope());
}
PICOTEST_CASE(testNewRopeEmptyFromData, colibriFixture) {
    checkRopeEmpty(Col_NewRope(COL_UCS1, NULL, 0));
    checkRopeEmpty(Col_NewRope(COL_UCS2, NULL, 0));
    checkRopeEmpty(Col_NewRope(COL_UCS4, NULL, 0));
    checkRopeEmpty(Col_NewRope(COL_UTF8, NULL, 0));
    checkRopeEmpty(Col_NewRope(COL_UTF16, NULL, 0));
    checkRopeEmpty(Col_NewRope(COL_UCS, NULL, 0));
}
PICOTEST_CASE(testNewRopeEmptyFromString, colibriFixture) {
    checkRopeEmpty(Col_NewRopeFromString(""));
}

static void checkRopeChar(Col_Word rope, Col_Char c, Col_StringFormat format) {
    PICOTEST_ASSERT(Col_WordType(rope) == (COL_CHAR | COL_STRING | COL_ROPE));
    PICOTEST_ASSERT(Col_StringWordFormat(rope) == format);
    PICOTEST_ASSERT(Col_RopeLength(rope) == 1);
    PICOTEST_ASSERT(Col_RopeDepth(rope) == 0);
    PICOTEST_ASSERT(Col_CharWordValue(rope) == c);
}
PICOTEST_CASE(testNewRopeUcs1Char, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS1_CHAR(COL_CHAR1_MAX);
    PICOTEST_ASSERT(rope == NEW_ROPE_UCS1_CHAR(COL_CHAR1_MAX));
    checkRopeChar(rope, COL_CHAR1_MAX, COL_UCS1);
}
PICOTEST_CASE(testNewRopeUcs2Char, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS2_CHAR(COL_CHAR2_MAX);
    PICOTEST_ASSERT(rope == NEW_ROPE_UCS2_CHAR(COL_CHAR2_MAX));
    checkRopeChar(rope, COL_CHAR2_MAX, COL_UCS2);
}
PICOTEST_CASE(testNewRopeUcs4Char, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS4_CHAR(COL_CHAR4_MAX);
    PICOTEST_ASSERT(rope == NEW_ROPE_UCS4_CHAR(COL_CHAR4_MAX));
    checkRopeChar(rope, COL_CHAR4_MAX, COL_UCS4);
}
PICOTEST_SUITE(testNewRopeUcsChar, testNewRopeUcsChar1, testNewRopeUcsChar2,
               testNewRopeUcsChar4);
PICOTEST_CASE(testNewRopeUcsChar1, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS_CHAR(COL_CHAR1_MAX);
    PICOTEST_ASSERT(rope == NEW_ROPE_UCS_CHAR(COL_CHAR1_MAX));
    checkRopeChar(rope, COL_CHAR1_MAX, COL_UCS1);
}
PICOTEST_CASE(testNewRopeUcsChar2, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS_CHAR(COL_CHAR2_MAX);
    PICOTEST_ASSERT(rope == NEW_ROPE_UCS_CHAR(COL_CHAR2_MAX));
    checkRopeChar(rope, COL_CHAR2_MAX, COL_UCS2);
}
PICOTEST_CASE(testNewRopeUcsChar4, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS_CHAR(COL_CHAR4_MAX);
    PICOTEST_ASSERT(rope == NEW_ROPE_UCS_CHAR(COL_CHAR4_MAX));
    checkRopeChar(rope, COL_CHAR4_MAX, COL_UCS4);
}
PICOTEST_CASE(testNewRopeFromStringChar, colibriFixture) {
    Col_Word rope = NEW_ROPE_STRING_CHAR('a');
    PICOTEST_ASSERT(rope == NEW_ROPE_STRING_CHAR('a'));
    PICOTEST_ASSERT(rope == NEW_ROPE_UCS1_CHAR('a'));
    checkRopeChar(rope, 'a', COL_UCS1);
}

static void checkRopeString(Col_Word rope, size_t len,
                            Col_StringFormat format) {
    PICOTEST_ASSERT(Col_WordType(rope) == (COL_STRING | COL_ROPE));
    PICOTEST_ASSERT(Col_StringWordFormat(rope) == format);
    PICOTEST_ASSERT(Col_RopeLength(rope) == len);
    PICOTEST_ASSERT(Col_RopeDepth(rope) == 0);
}
PICOTEST_CASE(testNewRopeUcs1Small, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS1_SMALL();
    PICOTEST_ASSERT(rope == NEW_ROPE_UCS1_SMALL());
    checkRopeString(rope, ROPE_SMALL_LEN, COL_UCS1);
}
PICOTEST_CASE(testNewRopeUcsSmall, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS_SMALL();
    PICOTEST_ASSERT(rope == NEW_ROPE_UCS_SMALL());
    PICOTEST_ASSERT(rope == NEW_ROPE_UCS1_SMALL());
    checkRopeString(rope, ROPE_SMALL_LEN, COL_UCS1);
}
PICOTEST_CASE(testNewRopeFromStringSmall, colibriFixture) {
    Col_Word rope = NEW_ROPE_STRING_SMALL();
    PICOTEST_ASSERT(rope == NEW_ROPE_STRING_SMALL());
    PICOTEST_ASSERT(rope == NEW_ROPE_UCS1_SMALL());
    checkRopeString(rope, ROPE_SMALL_LEN, COL_UCS1);
}

PICOTEST_CASE(testNewRopeUcs1Flat, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS1_LEAF();
    PICOTEST_ASSERT(rope != NEW_ROPE_UCS1_LEAF());
    checkRopeString(rope, ROPE_UCS_LEAF_LEN, COL_UCS1);
}
PICOTEST_CASE(testNewRopeUcs2Flat, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS2_LEAF();
    PICOTEST_ASSERT(rope != NEW_ROPE_UCS2_LEAF());
    checkRopeString(rope, ROPE_UCS_LEAF_LEN, COL_UCS2);
}
PICOTEST_CASE(testNewRopeUcs4Flat, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS4_LEAF();
    PICOTEST_ASSERT(rope != NEW_ROPE_UCS4_LEAF());
    checkRopeString(rope, ROPE_UCS_LEAF_LEN, COL_UCS4);
}
PICOTEST_SUITE(testNewRopeUcsFlat, testNewRopeUcsFlat1, testNewRopeUcsFlat2,
               testNewRopeUcsFlat4);
PICOTEST_CASE(testNewRopeUcsFlat1, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS_LEAF_1();
    PICOTEST_ASSERT(rope != NEW_ROPE_UCS_LEAF_1());
    checkRopeString(rope, ROPE_UCS_LEAF_LEN, COL_UCS1);
}
PICOTEST_CASE(testNewRopeUcsFlat2, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS_LEAF_2();
    PICOTEST_ASSERT(rope != NEW_ROPE_UCS_LEAF_2());
    checkRopeString(rope, ROPE_UCS_LEAF_LEN, COL_UCS2);
}
PICOTEST_CASE(testNewRopeUcsFlat4, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS_LEAF_4();
    PICOTEST_ASSERT(rope != NEW_ROPE_UCS_LEAF_4());
    checkRopeString(rope, ROPE_UCS_LEAF_LEN, COL_UCS4);
}
PICOTEST_CASE(testNewRopeUtf8Flat, colibriFixture) {
    Col_Word rope = NEW_ROPE_UTF8_LEAF();
    PICOTEST_ASSERT(rope != NEW_ROPE_UTF8_LEAF());
    checkRopeString(rope, ROPE_UTF_LEAF_LEN, COL_UTF8);
}
PICOTEST_CASE(testNewRopeUtf16Flat, colibriFixture) {
    Col_Word rope = NEW_ROPE_UTF16_LEAF();
    PICOTEST_ASSERT(rope != NEW_ROPE_UTF16_LEAF());
    checkRopeString(rope, ROPE_UTF_LEAF_LEN, COL_UTF16);
}
PICOTEST_CASE(testNewRopeFromStringFlat, colibriFixture) {
    Col_Word rope = NEW_ROPE_STRING_LEAF();
    PICOTEST_ASSERT(rope != NEW_ROPE_STRING_LEAF());
    checkRopeString(rope, ROPE_STRING_LEAF_LEN, COL_UCS1);
}

static void checkRope(Col_Word rope, size_t len, size_t depth) {
    PICOTEST_ASSERT(Col_WordType(rope) == COL_ROPE);
    PICOTEST_ASSERT(Col_RopeLength(rope) == len);
    PICOTEST_ASSERT(Col_RopeDepth(rope) == depth);
}
PICOTEST_CASE(testNewRopeUcs1Big, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS1_BIG();
    PICOTEST_ASSERT(rope != NEW_ROPE_UCS1_BIG());
    checkRope(rope, ROPE_UCS_BIG_LEN, 1);
}
PICOTEST_CASE(testNewRopeUcs2Big, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS2_BIG();
    PICOTEST_ASSERT(rope != NEW_ROPE_UCS2_BIG());
    checkRope(rope, ROPE_UCS_BIG_LEN, 1);
}
PICOTEST_CASE(testNewRopeUcs4Big, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS4_BIG();
    PICOTEST_ASSERT(rope != NEW_ROPE_UCS4_BIG());
    checkRope(rope, ROPE_UCS_BIG_LEN, 1);
}
PICOTEST_CASE(testNewRopeUcsBig, colibriFixture) {
    Col_Word rope = NEW_ROPE_UCS_BIG();
    PICOTEST_ASSERT(rope != NEW_ROPE_UCS_BIG());
    checkRope(rope, ROPE_UCS_BIG_LEN, 1);
}

PICOTEST_CASE(testNewRopeUtf8Big, colibriFixture) {
    Col_Word rope = NEW_ROPE_UTF8_BIG();
    PICOTEST_ASSERT(rope != NEW_ROPE_UTF8_BIG());
    PICOTEST_ASSERT(Col_WordType(rope) == COL_ROPE);
    PICOTEST_ASSERT(Col_RopeLength(rope) == ROPE_UTF_BIG_LEN);
    PICOTEST_ASSERT(Col_RopeDepth(rope) > 1);
}
PICOTEST_CASE(testNewRopeUtf16Big, colibriFixture) {
    Col_Word rope = NEW_ROPE_UTF16_BIG();
    PICOTEST_ASSERT(rope != NEW_ROPE_UTF16_BIG());
    PICOTEST_ASSERT(Col_WordType(rope) == COL_ROPE);
    PICOTEST_ASSERT(Col_RopeLength(rope) == ROPE_UTF_BIG_LEN);
    PICOTEST_ASSERT(Col_RopeDepth(rope) > 1);
}

PICOTEST_CASE(testNewRopeFromStringBig, colibriFixture) {
    Col_Word rope = NEW_ROPE_STRING_BIG();
    PICOTEST_ASSERT(rope != NEW_ROPE_STRING_BIG());
    checkRope(rope, ROPE_STRING_BIG_LEN, 1);
}

PICOTEST_SUITE(testNormalizeRope, testNormalizeRopeIdentity,
               testNormalizeRopeToUcs, testNormalizeRopeToUtf,
               testNormalizeSubropes, testNormalizeConcatRopes,
               testNormalizeFlatten);

PICOTEST_SUITE(testNormalizeRopeIdentity, testNormalizeEmptyRopeIdentity,
               testNormalizeCharacterIdentity, testNormalizeSmallStringIdentity,
               testNormalizeFlatStringIdentity, testNormalizeSubropesIdentity,
               testNormalizeConcatRopesIdentity);
static void checkNormalizeIdentity(Col_Word rope, Col_StringFormat format) {
    PICOTEST_ASSERT(Col_NormalizeRope(rope, format, COL_CHAR_INVALID, 0) ==
                    rope);
}
PICOTEST_CASE(testNormalizeEmptyRopeIdentity, colibriFixture) {
    checkNormalizeIdentity(Col_EmptyRope(), COL_UCS);
}
PICOTEST_CASE(testNormalizeCharacterIdentity, colibriFixture) {
    checkNormalizeIdentity(Col_NewCharWord(COL_CHAR1_MAX), COL_UCS1);
    checkNormalizeIdentity(Col_NewCharWord(COL_CHAR1_MAX), COL_UCS);
    checkNormalizeIdentity(Col_NewCharWord(COL_CHAR2_MAX), COL_UCS2);
    checkNormalizeIdentity(Col_NewCharWord(COL_CHAR2_MAX), COL_UCS);
    checkNormalizeIdentity(Col_NewCharWord(COL_CHAR_MAX), COL_UCS4);
    checkNormalizeIdentity(Col_NewCharWord(COL_CHAR_MAX), COL_UCS);
}
PICOTEST_CASE(testNormalizeSmallStringIdentity, colibriFixture) {
    checkNormalizeIdentity(
        Col_NormalizeRope(SMALL_STRING(), COL_UCS1, COL_CHAR_INVALID, 0),
        COL_UCS1);
    checkNormalizeIdentity(
        Col_NormalizeRope(SMALL_STRING(), COL_UCS, COL_CHAR_INVALID, 0),
        COL_UCS1);
}
PICOTEST_CASE(testNormalizeFlatStringIdentity, colibriFixture) {
    checkNormalizeIdentity(
        Col_NormalizeRope(FLAT_STRING_UCS1(), COL_UCS1, COL_CHAR_INVALID, 0),
        COL_UCS1);
    checkNormalizeIdentity(
        Col_NormalizeRope(FLAT_STRING_UCS2(), COL_UCS2, COL_CHAR_INVALID, 0),
        COL_UCS2);
    checkNormalizeIdentity(
        Col_NormalizeRope(FLAT_STRING_UCS4(), COL_UCS4, COL_CHAR_INVALID, 0),
        COL_UCS4);
    checkNormalizeIdentity(
        Col_NormalizeRope(FLAT_STRING_UCS1(), COL_UCS, COL_CHAR_INVALID, 0),
        COL_UCS1);
    checkNormalizeIdentity(
        Col_NormalizeRope(FLAT_STRING_UCS2(), COL_UCS, COL_CHAR_INVALID, 0),
        COL_UCS2);
    checkNormalizeIdentity(
        Col_NormalizeRope(FLAT_STRING_UCS4(), COL_UCS, COL_CHAR_INVALID, 0),
        COL_UCS4);
    checkNormalizeIdentity(
        Col_NormalizeRope(FLAT_STRING_UTF8(), COL_UTF8, COL_CHAR_INVALID, 0),
        COL_UTF8);
    checkNormalizeIdentity(
        Col_NormalizeRope(FLAT_STRING_UTF16(), COL_UTF16, COL_CHAR_INVALID, 0),
        COL_UTF16);
}

PICOTEST_SUITE(testNormalizeSubropesIdentity,
               testNormalizeSubropesOfFlatStringIdentity,
               testNormalizeSubropesOfConcatRopeIdentity);
PICOTEST_CASE(testNormalizeSubropesOfFlatStringIdentity, colibriFixture) {
    checkNormalizeIdentity(Col_Subrope(FLAT_STRING_UCS1(), 1, SIZE_MAX),
                           COL_UCS1);
    checkNormalizeIdentity(Col_Subrope(FLAT_STRING_UCS2(), 1, SIZE_MAX),
                           COL_UCS2);
    checkNormalizeIdentity(Col_Subrope(FLAT_STRING_UCS4(), 1, SIZE_MAX),
                           COL_UCS4);
    checkNormalizeIdentity(Col_Subrope(FLAT_STRING_UCS1(), 1, SIZE_MAX),
                           COL_UCS);
    checkNormalizeIdentity(Col_Subrope(FLAT_STRING_UCS2(), 1, SIZE_MAX),
                           COL_UCS);
    checkNormalizeIdentity(Col_Subrope(FLAT_STRING_UCS4(), 1, SIZE_MAX),
                           COL_UCS);
    checkNormalizeIdentity(Col_Subrope(FLAT_STRING_UTF8(), 1, SIZE_MAX),
                           COL_UTF8);
    checkNormalizeIdentity(Col_Subrope(FLAT_STRING_UTF16(), 1, SIZE_MAX),
                           COL_UTF16);
}
PICOTEST_CASE(testNormalizeSubropesOfConcatRopeIdentity, colibriFixture) {
    checkNormalizeIdentity(
        Col_Subrope(
            Col_ConcatRopes(FLAT_STRING_UCS1(), Col_NewCharWord(COL_CHAR1_MAX)),
            1, SIZE_MAX),
        COL_UCS1);
    checkNormalizeIdentity(
        Col_Subrope(Col_ConcatRopes(FLAT_STRING_UCS1(), SMALL_STRING()), 1,
                    SIZE_MAX),
        COL_UCS1);
    checkNormalizeIdentity(
        Col_Subrope(Col_ConcatRopes(FLAT_STRING_UCS1(), SHORT_STRING_UCS1()), 1,
                    SIZE_MAX),
        COL_UCS1);
    checkNormalizeIdentity(
        Col_Subrope(
            Col_ConcatRopes(FLAT_STRING_UCS2(), Col_NewCharWord(COL_CHAR2_MAX)),
            1, SIZE_MAX),
        COL_UCS2);
    checkNormalizeIdentity(
        Col_Subrope(Col_ConcatRopes(FLAT_STRING_UCS2(), SHORT_STRING_UCS2()), 1,
                    SIZE_MAX),
        COL_UCS2);
    checkNormalizeIdentity(
        Col_Subrope(
            Col_ConcatRopes(FLAT_STRING_UCS4(), Col_NewCharWord(COL_CHAR_MAX)),
            1, SIZE_MAX),
        COL_UCS4);
    checkNormalizeIdentity(
        Col_Subrope(Col_ConcatRopes(FLAT_STRING_UCS4(), SHORT_STRING_UCS4()), 1,
                    SIZE_MAX),
        COL_UCS4);
    checkNormalizeIdentity(
        Col_Subrope(
            Col_ConcatRopes(FLAT_STRING_UCS2(), Col_NewCharWord(COL_CHAR1_MAX)),
            1, SIZE_MAX),
        COL_UCS);
    checkNormalizeIdentity(
        Col_Subrope(Col_ConcatRopes(FLAT_STRING_UCS2(), SHORT_STRING_UCS4()), 1,
                    SIZE_MAX),
        COL_UCS);
    checkNormalizeIdentity(
        Col_Subrope(Col_ConcatRopes(FLAT_STRING_UTF8(), SHORT_STRING_UTF8()), 1,
                    SIZE_MAX),
        COL_UTF8);
    checkNormalizeIdentity(
        Col_Subrope(Col_ConcatRopes(FLAT_STRING_UTF16(), SHORT_STRING_UTF16()),
                    1, SIZE_MAX),
        COL_UTF16);
}

PICOTEST_CASE(testNormalizeConcatRopesIdentity, colibriFixture) {
    checkNormalizeIdentity(
        Col_ConcatRopes(FLAT_STRING_UCS1(), FLAT_STRING_UCS1()), COL_UCS1);
    checkNormalizeIdentity(
        Col_ConcatRopes(FLAT_STRING_UCS2(), FLAT_STRING_UCS2()), COL_UCS2);
    checkNormalizeIdentity(
        Col_ConcatRopes(FLAT_STRING_UCS4(), FLAT_STRING_UCS4()), COL_UCS4);
    checkNormalizeIdentity(
        Col_ConcatRopes(FLAT_STRING_UCS1(), FLAT_STRING_UCS2()), COL_UCS);
    checkNormalizeIdentity(
        Col_ConcatRopes(FLAT_STRING_UCS1(), FLAT_STRING_UCS4()), COL_UCS);
    checkNormalizeIdentity(
        Col_ConcatRopes(FLAT_STRING_UCS2(), FLAT_STRING_UCS4()), COL_UCS);
    checkNormalizeIdentity(
        Col_ConcatRopes(FLAT_STRING_UTF8(), FLAT_STRING_UTF8()), COL_UTF8);
    checkNormalizeIdentity(
        Col_ConcatRopes(FLAT_STRING_UTF16(), FLAT_STRING_UTF16()), COL_UTF16);
}

PICOTEST_SUITE(testNormalizeRopeToUcs, testNormalizeRopeToUcsUpconvert,
               testNormalizeRopeToUcsDownconvert,
               testNormalizeRopeToUcsAdaptive);

PICOTEST_SUITE(testNormalizeRopeToUcsUpconvert,
               testNormalizeCharacterToUcsUpconvert,

               testNormalizeSmallStringToUcsUpconvert);
PICOTEST_CASE(testNormalizeCharacterToUcsUpconvert, colibriFixture) {
    checkRopeChar(
        Col_NormalizeRope(Col_NewCharWord(COL_CHAR1_MAX), COL_UCS2, 0, 0),
        COL_CHAR1_MAX, COL_UCS2);
    checkRopeChar(
        Col_NormalizeRope(Col_NewCharWord(COL_CHAR1_MAX), COL_UCS4, 0, 0),
        COL_CHAR1_MAX, COL_UCS4);
    checkRopeChar(
        Col_NormalizeRope(Col_NewCharWord(COL_CHAR2_MAX), COL_UCS4, 0, 0),
        COL_CHAR2_MAX, COL_UCS4);
}
PICOTEST_CASE(testNormalizeSmallStringToUcsUpconvert, colibriFixture) {
    checkRopeString(
        Col_NormalizeRope(SMALL_STRING(), COL_UCS2, COL_CHAR_INVALID, 0),
        SMALL_STRING_LEN, COL_UCS2);
    checkRopeString(
        Col_NormalizeRope(SMALL_STRING(), COL_UCS4, COL_CHAR_INVALID, 0),
        SMALL_STRING_LEN, COL_UCS4);
}

PICOTEST_SUITE(testNormalizeRopeToUcsDownconvert,
               testNormalizeRopeToUcsDownconvertSmallString,
               testNormalizeRopeToUcsDownconvertSkip,
               testNormalizeRopeToUcsDownconvertReplace);
PICOTEST_CASE(testNormalizeRopeToUcsDownconvertSmallString, colibriFixture) {
    Col_Word small2 =
        Col_NormalizeRope(SMALL_STRING(), COL_UCS2, COL_CHAR_INVALID, 0);
    PICOTEST_ASSERT(Col_NormalizeRope(small2, COL_UCS1, COL_CHAR_INVALID, 0) ==
                    SMALL_STRING());

    Col_Word small4 =
        Col_NormalizeRope(SMALL_STRING(), COL_UCS2, COL_CHAR_INVALID, 0);
    PICOTEST_ASSERT(Col_NormalizeRope(small4, COL_UCS1, COL_CHAR_INVALID, 0) ==
                    SMALL_STRING());
}

PICOTEST_SUITE(testNormalizeRopeToUcsDownconvertSkip,
               testNormalizeCharacterToUcsDownconvertSkip,
               testNormalizeFlatStringToUcsDownconvertSkip);
PICOTEST_CASE(testNormalizeCharacterToUcsDownconvertSkip, colibriFixture) {
    checkRopeEmpty(Col_NormalizeRope(Col_NewCharWord(COL_CHAR1_MAX + 1),
                                     COL_UCS1, COL_CHAR_INVALID, 0));
    checkRopeEmpty(Col_NormalizeRope(Col_NewCharWord(COL_CHAR1_MAX + 1),
                                     COL_UCS1, COL_CHAR_MAX, 0));
    checkRopeEmpty(Col_NormalizeRope(Col_NewCharWord(COL_CHAR2_MAX + 1),
                                     COL_UCS2, COL_CHAR_INVALID, 0));
    checkRopeEmpty(Col_NormalizeRope(Col_NewCharWord(COL_CHAR2_MAX + 1),
                                     COL_UCS2, COL_CHAR_MAX, 0));
}
PICOTEST_CASE(testNormalizeFlatStringToUcsDownconvertSkip, colibriFixture) {
    checkRopeChar(
        Col_NormalizeRope(FLAT_STRING_UCS2(), COL_UCS1, COL_CHAR_INVALID, 1),
        COL_CHAR1_MAX, COL_UCS1);
    checkRopeEmpty(
        Col_NormalizeRope(FLAT_STRING_UCS4(), COL_UCS1, COL_CHAR_INVALID, 1));
    checkRopeChar(
        Col_NormalizeRope(FLAT_STRING_UCS4(), COL_UCS2, COL_CHAR_INVALID, 1),
        COL_CHAR2_MAX, COL_UCS2);
}

PICOTEST_SUITE(testNormalizeRopeToUcsDownconvertReplace,
               testNormalizeCharacterToUcsDownconvertReplace,
               testNormalizeFlatStringToUcsDownconvertReplace);
PICOTEST_CASE(testNormalizeCharacterToUcsDownconvertReplace, colibriFixture) {
    checkRopeChar(
        Col_NormalizeRope(Col_NewCharWord(COL_CHAR1_MAX + 1), COL_UCS1, 0, 0),
        0, COL_UCS1);
    checkRopeChar(
        Col_NormalizeRope(Col_NewCharWord(COL_CHAR2_MAX + 1), COL_UCS2, 0, 0),
        0, COL_UCS2);
}
PICOTEST_CASE(testNormalizeFlatStringToUcsDownconvertReplace, colibriFixture) {
    checkRopeString(Col_NormalizeRope(FLAT_STRING_UCS2(), COL_UCS1, 0, 1),
                    FLAT_STRING_LEN, COL_UCS1);
    checkRopeString(Col_NormalizeRope(FLAT_STRING_UCS4(), COL_UCS1, 0, 1),
                    FLAT_STRING_LEN, COL_UCS1);
    checkRopeString(Col_NormalizeRope(FLAT_STRING_UCS4(), COL_UCS2, 0, 1),
                    FLAT_STRING_LEN, COL_UCS2);
}

PICOTEST_CASE(testNormalizeRopeToUcsAdaptive, colibriFixture) {
    checkRopeString(Col_NormalizeRope(SHORT_STRING_UTF8(), COL_UCS, 0, 0),
                    SHORT_STRING_LEN, COL_UCS1);
    checkRopeString(Col_NormalizeRope(FLAT_STRING_UTF8(), COL_UCS, 0, 0),
                    FLAT_STRING_LEN, COL_UCS2);
    checkRopeString(Col_NormalizeRope(MEDIUM_STRING_UTF16(), COL_UCS, 0, 0),
                    MEDIUM_STRING_UTF16_LEN, COL_UCS4);
}

PICOTEST_CASE(testNormalizeRopeToUtf, colibriFixture) {
    checkRopeString(Col_NormalizeRope(FLAT_STRING_UCS1(), COL_UTF8, 0, 0),
                    FLAT_STRING_LEN, COL_UTF8);
    checkRopeString(Col_NormalizeRope(FLAT_STRING_UCS2(), COL_UTF8, 0, 0),
                    FLAT_STRING_LEN, COL_UTF8);
    checkRopeString(Col_NormalizeRope(FLAT_STRING_UCS4(), COL_UTF8, 0, 0),
                    FLAT_STRING_LEN, COL_UTF8);
    checkRopeString(Col_NormalizeRope(FLAT_STRING_UCS1(), COL_UTF16, 0, 0),
                    FLAT_STRING_LEN, COL_UTF16);
    checkRopeString(Col_NormalizeRope(FLAT_STRING_UCS2(), COL_UTF16, 0, 0),
                    FLAT_STRING_LEN, COL_UTF16);
    checkRopeString(Col_NormalizeRope(FLAT_STRING_UCS4(), COL_UTF16, 0, 0),
                    FLAT_STRING_LEN, COL_UTF16);
}

PICOTEST_SUITE(testNormalizeSubropes,
               testNormalizeSubropeOfFlatStringIsFlattened,
               testNormalizeSubropeOfConcatRopeIsNotFlattened);
PICOTEST_CASE(testNormalizeSubropeOfFlatStringIsFlattened, colibriFixture) {
    Col_Word subrope = Col_Subrope(FLAT_STRING_UCS1(), 1, SIZE_MAX);
    checkRope(subrope, FLAT_STRING_LEN - 1, 0);
    checkRopeString(Col_NormalizeRope(subrope, COL_UCS2, COL_CHAR_INVALID, 0),
                    FLAT_STRING_LEN - 1, COL_UCS2);
}
PICOTEST_CASE(testNormalizeSubropeOfConcatRopeIsNotFlattened, colibriFixture) {
    Col_Word subrope = Col_Subrope(
        Col_ConcatRopes(FLAT_STRING_UCS1(), SMALL_STRING()), 1, SIZE_MAX);
    checkRope(subrope, FLAT_STRING_LEN + SMALL_STRING_LEN - 1, 1);
    checkRope(Col_NormalizeRope(subrope, COL_UCS2, COL_CHAR_INVALID, 0),
              FLAT_STRING_LEN + SMALL_STRING_LEN - 1, 1);
}

PICOTEST_SUITE(testNormalizeConcatRopes, testNormalizeConcatRopeDontFlatten,
               testNormalizeConcatRopeFlatten);
PICOTEST_CASE(testNormalizeConcatRopeDontFlatten, colibriFixture) {
    Col_Word left = FLAT_STRING_UCS1(), right = SHORT_STRING_UCS2();
    Col_Word concatRope = Col_ConcatRopes(left, right);
    checkRope(concatRope, FLAT_STRING_LEN + SHORT_STRING_LEN, 1);
    Col_Word normalizedRope =
        Col_NormalizeRope(concatRope, COL_UCS2, COL_CHAR_INVALID, 0);
    PICOTEST_ASSERT(concatRope != normalizedRope);
    checkRope(normalizedRope, FLAT_STRING_LEN + SHORT_STRING_LEN, 1);
    PICOTEST_ASSERT(Col_Subrope(concatRope, 0, FLAT_STRING_LEN - 1) == left);
    PICOTEST_ASSERT(Col_Subrope(concatRope, FLAT_STRING_LEN, SIZE_MAX) ==
                    right);
    PICOTEST_ASSERT(Col_Subrope(normalizedRope, 0, FLAT_STRING_LEN - 1) !=
                    left);
    PICOTEST_ASSERT(Col_Subrope(normalizedRope, FLAT_STRING_LEN, SIZE_MAX) ==
                    right);
}
PICOTEST_CASE(testNormalizeConcatRopeFlatten, colibriFixture) {
    Col_Word left = FLAT_STRING_UCS1(), right = SHORT_STRING_UCS1();
    Col_Word concatRope = Col_ConcatRopes(left, right);
    checkRope(concatRope, FLAT_STRING_LEN + SHORT_STRING_LEN, 1);
    Col_Word normalizedRope =
        Col_NormalizeRope(concatRope, COL_UCS1, COL_CHAR_INVALID, 1);
    PICOTEST_ASSERT(concatRope != normalizedRope);
    checkRopeString(normalizedRope, FLAT_STRING_LEN + SHORT_STRING_LEN,
                    COL_UCS1);
}

PICOTEST_SUITE(testNormalizeFlatten, testNormalizeFlattenMerge,
               testNormalizeFlattenSplit);
PICOTEST_CASE(testNormalizeFlattenMerge, colibriFixture) {
    Col_Word rope = BIG_STRING_UTF();
    PICOTEST_ASSERT(Col_RopeDepth(rope) > 1);
    Col_Word normalized =
        Col_NormalizeRope(rope, COL_UCS4, COL_CHAR_INVALID, 1);
    PICOTEST_ASSERT(Col_RopeDepth(normalized) == 0);
}
PICOTEST_CASE(testNormalizeFlattenSplit, colibriFixture) {
    Col_Word rope = BIG_STRING_UCS();
    PICOTEST_ASSERT(Col_RopeDepth(rope) == 1);
    Col_Word normalized =
        Col_NormalizeRope(rope, COL_UTF8, COL_CHAR_INVALID, 1);
    PICOTEST_ASSERT(Col_RopeDepth(normalized) > 1);
}

/* Rope operations */
PICOTEST_SUITE(testRopeOperations, testSubrope, testConcatRopes,
               testConcatRopesA, testConcatRopesNV, testConcatRopesV,
               testRepeatRope, testRopeInsert, testRopeRemove, testRopeReplace);

// TODO compare original/subrope characters
PICOTEST_SUITE(testSubrope, testSubropeOfEmptyRopeIsEmpty,
               testSubropeOfEmptyRangeIsEmpty,
               testSubropeOfWholeRangeIsIdentity,
               testSubropeOfOneCharacterRangeIsCharacterWord,
               testSubropeOfSmallStringIsSmallString,
               testSubropeOfShortStringIsNewString,
               testSubropeOfFlatStringIsNewRope, testSubropeOfSubropeIsNewRope,
               testSubropeOfConcatRope);

PICOTEST_CASE(testSubropeOfEmptyRopeIsEmpty, colibriFixture) {
    checkRopeEmpty(Col_Subrope(Col_EmptyRope(), 0, 0));
}
PICOTEST_CASE(testSubropeOfEmptyRangeIsEmpty, colibriFixture) {
    Col_Word rope = SMALL_STRING();
    checkRopeEmpty(Col_Subrope(rope, 1, 0));
    checkRopeEmpty(Col_Subrope(rope, 2, 1));
    checkRopeEmpty(Col_Subrope(rope, SMALL_STRING_LEN, SMALL_STRING_LEN));
    checkRopeEmpty(Col_Subrope(rope, SIZE_MAX, 2));
    checkRopeEmpty(Col_Subrope(rope, SIZE_MAX, SIZE_MAX));
}
PICOTEST_CASE(testSubropeOfWholeRangeIsIdentity, colibriFixture) {
    Col_Word rope = SMALL_STRING();
    PICOTEST_ASSERT(Col_Subrope(rope, 0, Col_RopeLength(rope)) == rope);
    PICOTEST_ASSERT(Col_Subrope(rope, 0, SIZE_MAX) == rope);
}
PICOTEST_CASE(testSubropeOfOneCharacterRangeIsCharacterWord, colibriFixture) {
    Col_Word rope = SMALL_STRING();

    PICOTEST_ASSERT(Col_Subrope(rope, 0, 0) == Col_Subrope(rope, 0, 0));

    Col_Word subrope = Col_Subrope(rope, 1, 1);
    PICOTEST_ASSERT(subrope == Col_Subrope(rope, 1, 1));
    checkRopeChar(subrope, 'a' + 1, COL_UCS1);
}

PICOTEST_CASE(testSubropeOfSmallStringIsSmallString, colibriFixture) {
    Col_Word rope = SMALL_STRING();

    PICOTEST_ASSERT(Col_Subrope(rope, 0, 1) == Col_Subrope(rope, 0, 1));

    Col_Word subrope = Col_Subrope(rope, 1, SIZE_MAX);
    PICOTEST_ASSERT(subrope == Col_Subrope(rope, 1, SIZE_MAX));
    checkRopeString(subrope, SMALL_STRING_LEN - 1, COL_UCS1);
}
PICOTEST_CASE(testSubropeOfShortStringIsNewString, colibriFixture) {
    Col_Word rope = SHORT_STRING();

    Col_Word subrope = Col_Subrope(rope, 1, SIZE_MAX);
    PICOTEST_ASSERT(subrope != Col_Subrope(rope, 1, SIZE_MAX));
    checkRopeString(subrope, SHORT_STRING_LEN - 1, COL_UCS1);
}
PICOTEST_CASE(testSubropeOfFlatStringIsNewRope, colibriFixture) {
    Col_Word rope = FLAT_STRING();

    Col_Word subrope = Col_Subrope(rope, 1, SIZE_MAX);
    PICOTEST_ASSERT(subrope != Col_Subrope(rope, 1, SIZE_MAX));
    checkRope(subrope, FLAT_STRING_LEN - 1, 0);
}
PICOTEST_CASE(testSubropeOfSubropeIsNewRope, colibriFixture) {
    Col_Word rope = Col_Subrope(FLAT_STRING(), 1, SIZE_MAX);

    Col_Word subrope = Col_Subrope(rope, 1, SIZE_MAX);
    PICOTEST_ASSERT(subrope != Col_Subrope(rope, 1, SIZE_MAX));
    checkRope(subrope, FLAT_STRING_LEN - 2, 0);
}

PICOTEST_SUITE(testSubropeOfConcatRope, testSubropeOfConcatRopeArms,
               testShortSubropeOfConcatRope);
PICOTEST_CASE(testSubropeOfConcatRopeArms, colibriFixture) {
    Col_Word left = FLAT_STRING(), right = SMALL_STRING();
    PICOTEST_ASSERT(left != right);
    Col_Word rope = Col_ConcatRopes(left, right);
    PICOTEST_ASSERT(Col_Subrope(rope, 0, FLAT_STRING_LEN - 1) == left);
    PICOTEST_ASSERT(Col_Subrope(rope, FLAT_STRING_LEN, SIZE_MAX) == right);
}
PICOTEST_SUITE(testShortSubropeOfConcatRope, testMergeableSubropes,
               testUnmergeableSubropes);
PICOTEST_SUITE(testMergeableSubropes, testMergeableSubropesUniform,
               testMergeableSubropesUpconvert);
static void checkMergedSubrope(Col_Word left, Col_Word right, size_t delta,
                               Col_StringFormat format) {
    size_t leftLen = Col_RopeLength(left), rightLen = Col_RopeLength(right);
    Col_Word rope = Col_ConcatRopes(left, right);
    Col_Word subrope = Col_Subrope(rope, leftLen - delta, leftLen + delta);
    checkRope(rope, leftLen + rightLen, 1);
    checkRopeString(subrope, delta * 2 + 1, format);
}
PICOTEST_CASE(testMergeableSubropesUniform, colibriFixture) {
    checkMergedSubrope(FLAT_STRING(), SMALL_STRING(), 2, COL_UCS1);
    checkMergedSubrope(SMALL_STRING(), FLAT_STRING(), 2, COL_UCS1);
    checkMergedSubrope(FLAT_STRING(), FLAT_STRING(), 2, COL_UCS1);
    checkMergedSubrope(FLAT_STRING_UCS1(), FLAT_STRING_UCS1(), 10, COL_UCS1);
    checkMergedSubrope(FLAT_STRING_UCS2(), FLAT_STRING_UCS2(), 10, COL_UCS2);
    checkMergedSubrope(FLAT_STRING_UCS4(), FLAT_STRING_UCS4(), 10, COL_UCS4);
    checkMergedSubrope(FLAT_STRING_UTF8(), FLAT_STRING_UTF8(), 10, COL_UTF8);
    checkMergedSubrope(FLAT_STRING_UTF16(), FLAT_STRING_UTF16(), 10, COL_UTF16);
}
PICOTEST_CASE(testMergeableSubropesUpconvert, colibriFixture) {
    checkMergedSubrope(FLAT_STRING_UCS1(), FLAT_STRING_UCS2(), 10, COL_UCS2);
    checkMergedSubrope(FLAT_STRING_UCS1(), FLAT_STRING_UCS4(), 10, COL_UCS4);
    checkMergedSubrope(FLAT_STRING_UCS2(), FLAT_STRING_UCS1(), 10, COL_UCS2);
    checkMergedSubrope(FLAT_STRING_UCS2(), FLAT_STRING_UCS4(), 10, COL_UCS4);
    checkMergedSubrope(FLAT_STRING_UCS4(), FLAT_STRING_UCS1(), 10, COL_UCS4);
    checkMergedSubrope(FLAT_STRING_UCS4(), FLAT_STRING_UCS2(), 10, COL_UCS4);
}
PICOTEST_SUITE(testUnmergeableSubropes, testUnmergeableSubropesUniform,
               testUnmergeableSubropesUpconvert,
               testUnmergeableSubropesIncompatible);
static void checkUnmergedSubrope(Col_Word left, Col_Word right, size_t delta) {
    size_t leftLen = Col_RopeLength(left), rightLen = Col_RopeLength(right);
    Col_Word rope = Col_ConcatRopes(left, right);
    Col_Word subrope = Col_Subrope(rope, leftLen - delta, leftLen + delta);
    checkRope(rope, leftLen + rightLen, 1);
    checkRope(subrope, delta * 2 + 1, 1);
}
PICOTEST_CASE(testUnmergeableSubropesUniform, colibriFixture) {
    checkUnmergedSubrope(FLAT_STRING_UCS2(), FLAT_STRING_UCS2(), 100 / 4);
    checkUnmergedSubrope(FLAT_STRING_UCS4(), FLAT_STRING_UCS4(), 100 / 8);
    checkUnmergedSubrope(FLAT_STRING_UTF8(), FLAT_STRING_UTF8(), 40);
    checkUnmergedSubrope(FLAT_STRING_UTF16(), FLAT_STRING_UTF16(), 40);
}
PICOTEST_CASE(testUnmergeableSubropesUpconvert, colibriFixture) {
    checkUnmergedSubrope(FLAT_STRING_UCS1(), FLAT_STRING_UCS2(), 100 / 3);
    checkUnmergedSubrope(FLAT_STRING_UCS2(), FLAT_STRING_UCS1(), 100 / 3);
    checkUnmergedSubrope(FLAT_STRING_UCS1(), FLAT_STRING_UCS4(), 100 / 5);
    checkUnmergedSubrope(FLAT_STRING_UCS4(), FLAT_STRING_UCS1(), 100 / 5);
    checkUnmergedSubrope(FLAT_STRING_UCS2(), FLAT_STRING_UCS4(), 100 / 6);
    checkUnmergedSubrope(FLAT_STRING_UCS4(), FLAT_STRING_UCS2(), 100 / 6);
}
PICOTEST_CASE(testUnmergeableSubropesIncompatible, colibriFixture) {
    checkUnmergedSubrope(FLAT_STRING_UTF8(), FLAT_STRING_UCS1(), 1);
    checkUnmergedSubrope(FLAT_STRING_UTF16(), FLAT_STRING_UCS2(), 1);
    checkUnmergedSubrope(FLAT_STRING_UCS4(), FLAT_STRING_UTF8(), 1);
    checkUnmergedSubrope(FLAT_STRING_UCS1(), FLAT_STRING_UTF16(), 1);
    checkUnmergedSubrope(FLAT_STRING_UTF8(), FLAT_STRING_UTF16(), 1);
}

PICOTEST_SUITE(testConcatRopes, testConcatRopesErrors,
               testConcatRopeWithEmptyIsIdentity, testConcatShortStrings,
               testConcatFlatStringsIsNewRope,
               testConcatAdjacentSubropesIsOriginalRope,
               testConcatRopeBalancing);

PICOTEST_SUITE(testConcatRopesErrors, testConcatRopesTooLarge);
PICOTEST_CASE(testConcatRopesTooLarge, colibriFixture) {
    PICOTEST_ASSERT(concatRopes_valueCheck_length(NULL) == 1);
}

PICOTEST_CASE(testConcatRopeWithEmptyIsIdentity, colibriFixture) {
    Col_Word rope = SMALL_STRING();
    PICOTEST_ASSERT(Col_ConcatRopes(rope, Col_EmptyRope()) == rope);
    PICOTEST_ASSERT(Col_ConcatRopes(Col_EmptyRope(), rope) == rope);
}
PICOTEST_SUITE(testConcatShortStrings, testConcatMergeableStrings,
               testConcatUnmergeableStrings);
PICOTEST_SUITE(testConcatMergeableStrings, testConcatMergeableStringsUniform,
               testConcatMergeableStringsUpconvert);
static void checkMergedConcatRope(Col_Word left, Col_Word right,
                                  Col_StringFormat format) {
    size_t leftLen = Col_RopeLength(left), rightLen = Col_RopeLength(right);
    Col_Word rope = Col_ConcatRopes(left, right);
    checkRopeString(rope, leftLen + rightLen, format);
}
PICOTEST_CASE(testConcatMergeableStringsUniform, colibriFixture) {
    checkMergedConcatRope(SMALL_STRING(), SMALL_STRING(), COL_UCS1);
    checkMergedConcatRope(SHORT_STRING(), SMALL_STRING(), COL_UCS1);
    checkMergedConcatRope(SMALL_STRING(), SHORT_STRING(), COL_UCS1);
    checkMergedConcatRope(SHORT_STRING(), SHORT_STRING(), COL_UCS1);
    checkMergedConcatRope(SHORT_STRING_UCS1(), SHORT_STRING_UCS1(), COL_UCS1);
    checkMergedConcatRope(SHORT_STRING_UCS2(), SHORT_STRING_UCS2(), COL_UCS2);
    checkMergedConcatRope(SHORT_STRING_UCS4(), SHORT_STRING_UCS4(), COL_UCS4);
    checkMergedConcatRope(SHORT_STRING_UTF8(), SHORT_STRING_UTF8(), COL_UTF8);
    checkMergedConcatRope(SHORT_STRING_UTF16(), SHORT_STRING_UTF16(),
                          COL_UTF16);
}
PICOTEST_CASE(testConcatMergeableStringsUpconvert, colibriFixture) {
    checkMergedConcatRope(SHORT_STRING_UCS1(), SHORT_STRING_UCS2(), COL_UCS2);
    checkMergedConcatRope(SHORT_STRING_UCS1(), SHORT_STRING_UCS4(), COL_UCS4);
    checkMergedConcatRope(SHORT_STRING_UCS2(), SHORT_STRING_UCS1(), COL_UCS2);
    checkMergedConcatRope(SHORT_STRING_UCS2(), SHORT_STRING_UCS4(), COL_UCS4);
    checkMergedConcatRope(SHORT_STRING_UCS4(), SHORT_STRING_UCS1(), COL_UCS4);
    checkMergedConcatRope(SHORT_STRING_UCS4(), SHORT_STRING_UCS2(), COL_UCS4);
}

PICOTEST_SUITE(testConcatUnmergeableStrings,
               testConcatUnmergeableStringsUniform,
               testConcatUnmergeableStringsUpconvert,
               testConcatUnmergeableStringsIncompatible);
static void checkUnmergedConcatRope(Col_Word left, Col_Word right,
                                    size_t depth) {
    size_t leftLen = Col_RopeLength(left), rightLen = Col_RopeLength(right);
    Col_Word rope = Col_ConcatRopes(left, right);
    checkRope(rope, leftLen + rightLen, depth);
}
PICOTEST_CASE(testConcatUnmergeableStringsUniform, colibriFixture) {
    checkUnmergedConcatRope(SHORT_STRING_UCS2(), MEDIUM_STRING_UCS2(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UCS4(), MEDIUM_STRING_UCS4(), 1);
    checkUnmergedConcatRope(MEDIUM_STRING_UTF8(), MEDIUM_STRING_UTF8(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UTF16(), MEDIUM_STRING_UTF16(), 1);
}
PICOTEST_CASE(testConcatUnmergeableStringsUpconvert, colibriFixture) {
    checkUnmergedConcatRope(SHORT_STRING_UCS1(), MEDIUM_STRING_UCS2(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UCS2(), MEDIUM_STRING_UCS1(), 1);
    checkUnmergedConcatRope(MEDIUM_STRING_UCS1(), MEDIUM_STRING_UCS4(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UCS4(), MEDIUM_STRING_UCS1(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UCS2(), MEDIUM_STRING_UCS4(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UCS4(), MEDIUM_STRING_UCS2(), 1);
}
PICOTEST_CASE(testConcatUnmergeableStringsIncompatible, colibriFixture) {
    checkUnmergedConcatRope(SHORT_STRING_UTF8(), SHORT_STRING_UCS1(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UTF16(), SHORT_STRING_UCS2(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UCS4(), SHORT_STRING_UTF8(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UCS1(), SHORT_STRING_UTF16(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UTF8(), SHORT_STRING_UTF16(), 1);
}
PICOTEST_CASE(testConcatFlatStringsIsNewRope, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    Col_Word concatRope = Col_ConcatRopes(rope, rope);
    PICOTEST_ASSERT(concatRope != Col_ConcatRopes(rope, rope));
    checkRope(concatRope, FLAT_STRING_LEN * 2, 1);
}
PICOTEST_CASE(testConcatAdjacentSubropesIsOriginalRope, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    size_t split = FLAT_STRING_LEN / 2;
    Col_Word left = Col_Subrope(rope, 0, split - 1);
    Col_Word right = Col_Subrope(rope, split, SIZE_MAX);
    PICOTEST_ASSERT(Col_ConcatRopes(left, right) == rope);
}

PICOTEST_SUITE(testConcatRopeBalancing,
               testConcatBalancedRopeBranchesAreNotRebalanced,
               testConcatInbalancedLeftOuterRopeBranchesAreRotated,
               testConcatInbalancedRightOuterRopeBranchesAreRotated,
               testConcatInbalancedLeftInnerRopeBranchesAreSplit,
               testConcatInbalancedRightInnerRopeBranchesAreSplit,
               testConcatInbalancedSubropeBranchesAreSplit);
PICOTEST_CASE(testConcatBalancedRopeBranchesAreNotRebalanced, colibriFixture) {
    Col_Word leaf[8];
    for (int i = 0; i < 8; i++) {
        leaf[i] = FLAT_STRING();
    }

    Col_Word left1 = Col_ConcatRopes(leaf[0], leaf[1]);
    Col_Word left2 = Col_ConcatRopes(leaf[2], leaf[3]);
    PICOTEST_ASSERT(Col_RopeDepth(left1) == 1);
    PICOTEST_ASSERT(Col_RopeDepth(left2) == 1);

    Col_Word left = Col_ConcatRopes(left1, left2);
    PICOTEST_ASSERT(Col_RopeDepth(left) == 2);
    PICOTEST_ASSERT(Col_RopeDepth(Col_ConcatRopes(left1, leaf[0])) == 2);
    PICOTEST_ASSERT(Col_RopeDepth(Col_ConcatRopes(leaf[0], left2)) == 2);
    for (int i = 0; i < 4; i++) {
        PICOTEST_ASSERT(Col_Subrope(left, FLAT_STRING_LEN * i,
                                    FLAT_STRING_LEN * (i + 1) - 1) == leaf[i]);
    }

    Col_Word right1 = Col_ConcatRopes(leaf[4], leaf[5]);
    Col_Word right2 = Col_ConcatRopes(leaf[6], leaf[7]);
    PICOTEST_ASSERT(Col_RopeDepth(right1) == 1);
    PICOTEST_ASSERT(Col_RopeDepth(right2) == 1);

    Col_Word right = Col_ConcatRopes(right1, right2);
    PICOTEST_ASSERT(Col_RopeDepth(right) == 2);
    PICOTEST_ASSERT(Col_RopeDepth(Col_ConcatRopes(right1, leaf[0])) == 2);
    PICOTEST_ASSERT(Col_RopeDepth(Col_ConcatRopes(leaf[0], right2)) == 2);
    for (int i = 0; i < 4; i++) {
        PICOTEST_ASSERT(Col_Subrope(right, FLAT_STRING_LEN * i,
                                    FLAT_STRING_LEN * (i + 1) - 1) ==
                        leaf[i + 4]);
    }

    Col_Word rope = Col_ConcatRopes(left, right);
    PICOTEST_ASSERT(Col_RopeDepth(rope) == 3);
    for (int i = 0; i < 8; i++) {
        PICOTEST_ASSERT(Col_Subrope(rope, FLAT_STRING_LEN * i,
                                    FLAT_STRING_LEN * (i + 1) - 1) == leaf[i]);
    }
}
PICOTEST_CASE(testConcatInbalancedLeftOuterRopeBranchesAreRotated,
              colibriFixture) {
    Col_Word leaf[4];
    for (int i = 0; i < 4; i++) {
        leaf[i] = FLAT_STRING();
    }

    Col_Word left1 = Col_ConcatRopes(leaf[0], leaf[1]);
    Col_Word left2 = leaf[2];
    PICOTEST_ASSERT(Col_RopeDepth(left1) == 1);
    PICOTEST_ASSERT(Col_RopeDepth(left2) == 0);

    Col_Word left = Col_ConcatRopes(left1, left2);
    PICOTEST_ASSERT(Col_RopeDepth(left) == 2);

    Col_Word right = leaf[3];
    PICOTEST_ASSERT(Col_RopeDepth(right) == 0);

    Col_Word rope = Col_ConcatRopes(left, right);
    PICOTEST_ASSERT(Col_RopeDepth(rope) == 2);
    for (int i = 0; i < 4; i++) {
        PICOTEST_ASSERT(Col_Subrope(rope, FLAT_STRING_LEN * i,
                                    FLAT_STRING_LEN * (i + 1) - 1) == leaf[i]);
    }
}
PICOTEST_CASE(testConcatInbalancedRightOuterRopeBranchesAreRotated,
              colibriFixture) {
    Col_Word leaf[4];
    for (int i = 0; i < 4; i++) {
        leaf[i] = FLAT_STRING();
    }

    Col_Word left = leaf[0];
    PICOTEST_ASSERT(Col_RopeDepth(left) == 0);

    Col_Word right1 = leaf[1];
    Col_Word right2 = Col_ConcatRopes(leaf[2], leaf[3]);
    PICOTEST_ASSERT(Col_RopeDepth(right1) == 0);
    PICOTEST_ASSERT(Col_RopeDepth(right2) == 1);

    Col_Word right = Col_ConcatRopes(right1, right2);
    PICOTEST_ASSERT(Col_RopeDepth(right) == 2);

    Col_Word rope = Col_ConcatRopes(left, right);
    PICOTEST_ASSERT(Col_RopeDepth(rope) == 2);
    for (int i = 0; i < 4; i++) {
        PICOTEST_ASSERT(Col_Subrope(rope, FLAT_STRING_LEN * i,
                                    FLAT_STRING_LEN * (i + 1) - 1) == leaf[i]);
    }
}
PICOTEST_CASE(testConcatInbalancedLeftInnerRopeBranchesAreSplit,
              colibriFixture) {
    Col_Word leaf[4];
    for (int i = 0; i < 4; i++) {
        leaf[i] = FLAT_STRING();
    }

    Col_Word left1 = leaf[0];
    Col_Word left2 = Col_ConcatRopes(leaf[1], leaf[2]);
    PICOTEST_ASSERT(Col_RopeDepth(left1) == 0);
    PICOTEST_ASSERT(Col_RopeDepth(left2) == 1);

    Col_Word left = Col_ConcatRopes(left1, left2);
    PICOTEST_ASSERT(Col_RopeDepth(left) == 2);

    Col_Word right = leaf[3];
    PICOTEST_ASSERT(Col_RopeDepth(right) == 0);

    Col_Word rope = Col_ConcatRopes(left, right);
    PICOTEST_ASSERT(Col_RopeDepth(rope) == 2);
    for (int i = 0; i < 4; i++) {
        PICOTEST_ASSERT(Col_Subrope(rope, FLAT_STRING_LEN * i,
                                    FLAT_STRING_LEN * (i + 1) - 1) == leaf[i]);
    }
}
PICOTEST_CASE(testConcatInbalancedRightInnerRopeBranchesAreSplit,
              colibriFixture) {
    Col_Word leaf[4];
    for (int i = 0; i < 4; i++) {
        leaf[i] = FLAT_STRING();
    }

    Col_Word left = leaf[0];
    PICOTEST_ASSERT(Col_RopeDepth(left) == 0);

    Col_Word right1 = Col_ConcatRopes(leaf[1], leaf[2]);
    Col_Word right2 = leaf[3];
    PICOTEST_ASSERT(Col_RopeDepth(right1) == 1);
    PICOTEST_ASSERT(Col_RopeDepth(right2) == 0);

    Col_Word right = Col_ConcatRopes(right1, right2);
    PICOTEST_ASSERT(Col_RopeDepth(right) == 2);

    Col_Word rope = Col_ConcatRopes(left, right);
    PICOTEST_ASSERT(Col_RopeDepth(rope) == 2);
    for (int i = 0; i < 4; i++) {
        PICOTEST_ASSERT(Col_Subrope(rope, FLAT_STRING_LEN * i,
                                    FLAT_STRING_LEN * (i + 1) - 1) == leaf[i]);
    }
}
PICOTEST_CASE(testConcatInbalancedSubropeBranchesAreSplit, colibriFixture) {
    Col_Word leaf = FLAT_STRING();
    Col_Word node1 = Col_ConcatRopes(leaf, leaf);
    Col_Word node2 = Col_ConcatRopes(node1, node1);
    PICOTEST_ASSERT(Col_RopeDepth(leaf) == 0);
    PICOTEST_ASSERT(Col_RopeDepth(node1) == 1);
    PICOTEST_ASSERT(Col_RopeDepth(node2) == 2);
    PICOTEST_ASSERT(Col_Subrope(node2, Col_RopeLength(node1), SIZE_MAX) ==
                    node1);
    Col_Word subrope = Col_Subrope(node2, 1, SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeDepth(subrope) == 2);
    PICOTEST_ASSERT(Col_Subrope(subrope, Col_RopeLength(node1) - 1, SIZE_MAX) ==
                    node1);

    Col_Word rope = Col_ConcatRopes(subrope, leaf);
    PICOTEST_ASSERT(Col_Subrope(rope, 0, Col_RopeLength(subrope) - 1) !=
                    subrope);
    PICOTEST_ASSERT(Col_Subrope(rope, Col_RopeLength(node1) - 1, SIZE_MAX) !=
                    node1);
}

PICOTEST_SUITE(testConcatRopesA, testConcatRopesAErrors, testConcatRopesAOne,
               testConcatRopesATwo, testConcatRopesARecurse);

PICOTEST_SUITE(testConcatRopesAErrors, testConcatRopesAZero);
PICOTEST_CASE(testConcatRopesAZero, colibriFixture) {
    PICOTEST_ASSERT(concatRopesA_valueCheck_number(NULL) == 1);
}

PICOTEST_CASE(testConcatRopesAOne, colibriFixture) {
    Col_Word ropes[] = {FLAT_STRING()};
    Col_Word rope = Col_ConcatRopesA(1, ropes);
    PICOTEST_ASSERT(rope == ropes[0]);
}
PICOTEST_CASE(testConcatRopesATwo, colibriFixture) {
    Col_Word ropes[] = {FLAT_STRING_UCS1(), FLAT_STRING_UTF8()};
    Col_Word rope = Col_ConcatRopesA(2, ropes);
    checkRope(rope, FLAT_STRING_LEN * 2, 1);
}
PICOTEST_CASE(testConcatRopesARecurse, colibriFixture) {
    Col_Word ropes[] = {FLAT_STRING_UCS1(), SMALL_STRING(), FLAT_STRING_UTF8()};
    Col_Word rope = Col_ConcatRopesA(3, ropes);
    checkRope(rope, FLAT_STRING_LEN * 2 + SMALL_STRING_LEN, 2);
}

PICOTEST_SUITE(testConcatRopesNV, testConcatRopesNVErrors);

PICOTEST_SUITE(testConcatRopesNVErrors, testConcatRopesNVZero);
PICOTEST_CASE(testConcatRopesNVZero, colibriFixture) {
    PICOTEST_ASSERT(concatRopesNV_valueCheck_number(NULL) == 1);
}

PICOTEST_SUITE(testConcatRopesV, testConcatRopesVOne, testConcatRopesVTwo,
               testConcatRopesVRecurse);

PICOTEST_CASE(testConcatRopesVOne, colibriFixture) {
    Col_Word leaf = FLAT_STRING();
    Col_Word rope = Col_ConcatRopesV(leaf);
    PICOTEST_ASSERT(rope == leaf);
}
PICOTEST_CASE(testConcatRopesVTwo, colibriFixture) {
    Col_Word rope = Col_ConcatRopesV(FLAT_STRING_UCS1(), FLAT_STRING_UTF8());
    checkRope(rope, FLAT_STRING_LEN * 2, 1);
}
PICOTEST_CASE(testConcatRopesVRecurse, colibriFixture) {
    Col_Word rope = Col_ConcatRopesV(FLAT_STRING_UCS1(), SMALL_STRING(),
                                     FLAT_STRING_UTF8());
    checkRope(rope, FLAT_STRING_LEN * 2 + SMALL_STRING_LEN, 2);
}

PICOTEST_SUITE(testRepeatRope, testRepeatRopeErrors, testRepeatRopeZeroIsEmpty,
               testRepeatRopeOnceIsIdentity, testRepeatRopeTwiceIsConcatSelf,
               testRepeatRopeRecurse, testRepeatEmptyRope, testRepeatRopeMax);

PICOTEST_SUITE(testRepeatRopeErrors, testRepeatRopeTooLarge);
PICOTEST_CASE(testRepeatRopeTooLarge, colibriFixture) {
    PICOTEST_ASSERT(repeatRope_valueCheck_length(NULL) == 1);
}

PICOTEST_CASE(testRepeatRopeZeroIsEmpty, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    checkRopeEmpty(Col_RepeatRope(rope, 0));
}
PICOTEST_CASE(testRepeatRopeOnceIsIdentity, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    PICOTEST_ASSERT(Col_RepeatRope(rope, 1) == rope);
}
PICOTEST_CASE(testRepeatRopeTwiceIsConcatSelf, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    Col_Word repeat = Col_RepeatRope(rope, 2);
    checkRope(repeat, FLAT_STRING_LEN * 2, 1);
    PICOTEST_ASSERT(Col_Subrope(repeat, 0, FLAT_STRING_LEN - 1) == rope);
    PICOTEST_ASSERT(Col_Subrope(repeat, FLAT_STRING_LEN, SIZE_MAX) == rope);
}
PICOTEST_CASE(testRepeatRopeRecurse, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    checkRope(Col_RepeatRope(rope, 3), FLAT_STRING_LEN * 3, 2);
    checkRope(Col_RepeatRope(rope, 4), FLAT_STRING_LEN * 4, 2);
    checkRope(Col_RepeatRope(rope, 8), FLAT_STRING_LEN * 8, 3);
    checkRope(Col_RepeatRope(rope, 16), FLAT_STRING_LEN * 16, 4);
    checkRope(Col_RepeatRope(rope, 32), FLAT_STRING_LEN * 32, 5);
    checkRope(Col_RepeatRope(rope, 65536), FLAT_STRING_LEN * 65536, 16);
    checkRope(Col_RepeatRope(rope, 65537), FLAT_STRING_LEN * 65537, 17);
}
PICOTEST_CASE(testRepeatEmptyRope, colibriFixture) {
    checkRopeEmpty(Col_RepeatRope(Col_EmptyRope(), SIZE_MAX));
}
PICOTEST_CASE(testRepeatRopeMax, colibriFixture) {
    Col_Word rope = Col_RepeatRope(Col_NewCharWord('a'), SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeLength(rope) == SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeDepth(rope) > 17);
}

PICOTEST_SUITE(testRopeInsert, testRopeInsertEmptyIsIdentity,
               testRopeInsertAtBeginningIsConcat, testRopeInsertPastEndIsConcat,
               testRopeInsertMiddle);
PICOTEST_CASE(testRopeInsertEmptyIsIdentity, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    PICOTEST_ASSERT(Col_RopeInsert(rope, 0, Col_EmptyRope()) == rope);
}
PICOTEST_CASE(testRopeInsertAtBeginningIsConcat, colibriFixture) {
    Col_Word into = FLAT_STRING(), rope = SHORT_STRING();
    Col_Word result = Col_RopeInsert(into, 0, rope);
    PICOTEST_ASSERT(Col_Subrope(result, 0, SHORT_STRING_LEN - 1) == rope);
    PICOTEST_ASSERT(Col_Subrope(result, SHORT_STRING_LEN, SIZE_MAX) == into);
}
PICOTEST_CASE(testRopeInsertPastEndIsConcat, colibriFixture) {
    Col_Word into = FLAT_STRING(), rope = SHORT_STRING();
    Col_Word result = Col_RopeInsert(into, SIZE_MAX, rope);
    PICOTEST_ASSERT(Col_Subrope(result, 0, FLAT_STRING_LEN - 1) == into);
    PICOTEST_ASSERT(Col_Subrope(result, FLAT_STRING_LEN, SIZE_MAX) == rope);
}
PICOTEST_CASE(testRopeInsertMiddle, colibriFixture) {
    Col_Word into = FLAT_STRING(), rope = SHORT_STRING();
    size_t index = FLAT_STRING_LEN / 3;
    Col_Word result = Col_RopeInsert(into, index, rope);
    PICOTEST_ASSERT(Col_RopeSearchLast(result, rope) == index);
}

PICOTEST_SUITE(testRopeRemove, testRopeRemoveFromEmptyIsIdentity,
               testRopeRemovePastEndIsIdentity,
               testRopeRemoveInvalidRangeIsIdentity,
               testRopeRemoveTrimIsSubrope, testRopeRemoveMiddle);
PICOTEST_CASE(testRopeRemoveFromEmptyIsIdentity, colibriFixture) {
    checkRopeEmpty(Col_RopeRemove(Col_EmptyRope(), 0, 0));
}
PICOTEST_CASE(testRopeRemovePastEndIsIdentity, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    PICOTEST_ASSERT(Col_RopeRemove(rope, FLAT_STRING_LEN, SIZE_MAX) == rope);
}
PICOTEST_CASE(testRopeRemoveInvalidRangeIsIdentity, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    PICOTEST_ASSERT(Col_RopeRemove(rope, 1, 0) == rope);
}
PICOTEST_CASE(testRopeRemoveTrimIsSubrope, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    size_t index = FLAT_STRING_LEN / 3;

    Col_Word trimBegin = Col_RopeRemove(rope, 0, index);
    PICOTEST_ASSERT(Col_CompareRopes(trimBegin, Col_Subrope(rope, index + 1,
                                                            SIZE_MAX)) == 0);

    Col_Word trimEnd = Col_RopeRemove(rope, index + 1, SIZE_MAX);
    PICOTEST_ASSERT(Col_CompareRopes(trimEnd, Col_Subrope(rope, 0, index)) ==
                    0);

    PICOTEST_ASSERT(
        Col_CompareRopes(Col_ConcatRopes(trimEnd, trimBegin), rope) == 0);
}
PICOTEST_CASE(testRopeRemoveMiddle, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    size_t first = FLAT_STRING_LEN / 3, last = FLAT_STRING_LEN / 2;
    Col_Word result = Col_RopeRemove(rope, first, last);
    PICOTEST_ASSERT(Col_CompareRopes(Col_Subrope(result, 0, first - 1),
                                     Col_Subrope(rope, 0, first - 1)) == 0);
    PICOTEST_ASSERT(Col_CompareRopes(Col_Subrope(result, first, SIZE_MAX),
                                     Col_Subrope(rope, last + 1, SIZE_MAX)) ==
                    0);
}

PICOTEST_SUITE(testRopeReplace, testRopeReplaceInvalidRangeIsIdentity,
               testRopeReplaceOneChar);
PICOTEST_CASE(testRopeReplaceInvalidRangeIsIdentity, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    PICOTEST_ASSERT(Col_RopeReplace(rope, 1, 0, SMALL_STRING()) == rope);
}
PICOTEST_CASE(testRopeReplaceOneChar, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    Col_Char index = FLAT_STRING_LEN / 3;
    Col_Char c = Col_RopeAt(rope, index);
    Col_Char c2 = COL_CHAR_MAX;
    PICOTEST_ASSERT(c != c2);
    Col_Word result = Col_RopeReplace(rope, index, index, Col_NewCharWord(c2));
    checkRopeString(rope, FLAT_STRING_LEN, COL_UCS1);
    checkRope(result, FLAT_STRING_LEN, 2);
    PICOTEST_ASSERT(Col_RopeAt(result, index) == c2);
}

PICOTEST_SUITE(testRopeTraversal, testRopeTraversalErrors,
               testTraverseSingleRope, testTraverseMultipleRopes);
// TODO reverse traversal

PICOTEST_SUITE(testRopeTraversalErrors, testTraverseRopeProcMustNotBeNull);
PICOTEST_CASE(testTraverseRopeProcMustNotBeNull, colibriFixture) {
    PICOTEST_VERIFY(traverseRopeChunks_valueCheck_proc(NULL) == 1);
    PICOTEST_VERIFY(traverseRopeChunksN_valueCheck_proc(NULL) == 1);
}

PICOTEST_SUITE(testTraverseSingleRope, testTraverseEmptyRopeIsNoop,
               testTraverseRopePastEndIsNoop, testTraverseZeroCharIsNoop,
               testTraverseCharacterWordHasOneChunk,
               testTraverseSmallStringHasOneChunk,
               testTraverseFlatStringHasOneChunk, testTraverseSubrope,
               testTraverseConcatRope, testTraverseRopeRange,
               testTraverseRopeBreak);

#define MAX_CHUNKS 5
#define CHUNK_SAMPLE_SIZE 10
typedef struct RopeChunkInfo {
    size_t length;
    Col_RopeChunk chunks[MAX_CHUNKS];
    char sample[CHUNK_SAMPLE_SIZE];
} RopeChunkInfo;
typedef struct RopeChunkCounterData {
    size_t number;
    size_t size;
    size_t nbChunks;
    RopeChunkInfo *infos;
} RopeChunkCounterData;
#define DECLARE_CHUNKS_DATA(name, size) DECLARE_CHUNKS_DATA_N(name, 1, size)
#define DECLARE_CHUNKS_DATA_N(name, number, size)                              \
    RopeChunkInfo name##datas[size];                                           \
    RopeChunkCounterData name = {number, size, 0, name##datas};
#define CHUNK_INFO(name, index) (name##datas)[index]

static int ropeChunkCounter(size_t index, size_t length, size_t number,
                            const Col_RopeChunk *chunks,
                            Col_ClientData clientData) {
    RopeChunkCounterData *data = (RopeChunkCounterData *)clientData;
    PICOTEST_ASSERT(number == data->number);
    PICOTEST_ASSERT(number < MAX_CHUNKS);
    PICOTEST_ASSERT(data != NULL);
    PICOTEST_ASSERT(data->nbChunks < data->size);

    RopeChunkInfo *info = data->infos + data->nbChunks++;
    info->length = length;
    memcpy(info->chunks, chunks, sizeof(*chunks) * number);
    if (chunks[0].data)
        memcpy(info->sample, chunks[0].data,
               chunks->byteLength < CHUNK_SAMPLE_SIZE ? chunks->byteLength
                                                      : CHUNK_SAMPLE_SIZE);
    return 0;
}

PICOTEST_CASE(testTraverseEmptyRopeIsNoop, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(Col_EmptyRope(), 0, SIZE_MAX, 0,
                                           ropeChunkCounter, &data,
                                           NULL) == -1);
    PICOTEST_ASSERT(data.nbChunks == 0);
}
PICOTEST_CASE(testTraverseRopePastEndIsNoop, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(SMALL_STRING(), SMALL_STRING_LEN,
                                           SIZE_MAX, 0, ropeChunkCounter, &data,
                                           NULL) == -1);
    PICOTEST_ASSERT(data.nbChunks == 0);
}
PICOTEST_CASE(testTraverseZeroCharIsNoop, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(SMALL_STRING(), 0, 0, 0,
                                           ropeChunkCounter, &data,
                                           NULL) == -1);
    PICOTEST_ASSERT(data.nbChunks == 0);
}
PICOTEST_CASE(testTraverseCharacterWordHasOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Char c = 'a';
    Col_Word rope = Col_NewCharWord(c);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0,
                                           ropeChunkCounter, &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].format == COL_UCS1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].byteLength == 1);
    PICOTEST_ASSERT(*(Col_Char1 *)CHUNK_INFO(data, 0).sample == c);
}
PICOTEST_CASE(testTraverseSmallStringHasOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Word rope = SMALL_STRING();
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0,
                                           ropeChunkCounter, &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == SMALL_STRING_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].format == COL_UCS1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].byteLength ==
                    SMALL_STRING_LEN);
    PICOTEST_ASSERT(rope ==
                    Col_NewRope(CHUNK_INFO(data, 0).chunks[0].format,
                                CHUNK_INFO(data, 0).sample,
                                CHUNK_INFO(data, 0).chunks[0].byteLength));
}
PICOTEST_SUITE(testTraverseFlatStringHasOneChunk,
               testTraverseFlatUcsStringHasOneChunk,
               testTraverseFlatUtf8StringHasOneChunk,
               testTraverseFlatUtf16StringHasOneChunk);
PICOTEST_CASE(testTraverseFlatUcsStringHasOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Word rope = FLAT_STRING_UCS1();
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0,
                                           ropeChunkCounter, &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == FLAT_STRING_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].format == COL_UCS1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].byteLength ==
                    FLAT_STRING_LEN);
}
PICOTEST_CASE(testTraverseFlatUtf8StringHasOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Word rope = FLAT_STRING_UTF8();
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0,
                                           ropeChunkCounter, &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == FLAT_STRING_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].format == COL_UTF8);
}
PICOTEST_CASE(testTraverseFlatUtf16StringHasOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Word rope = FLAT_STRING_UTF16();
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0,
                                           ropeChunkCounter, &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == FLAT_STRING_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].format == COL_UTF16);
}

PICOTEST_SUITE(testTraverseSubrope,
               testTraverseSubropeOfFlatStringHasOneOffsetChunk);
// TODO complex concat scenarios
PICOTEST_CASE(testTraverseSubropeOfFlatStringHasOneOffsetChunk,
              colibriFixture) {
    DECLARE_CHUNKS_DATA(ropeData, 1);
    DECLARE_CHUNKS_DATA(subropeData, 1);
    Col_Word rope = FLAT_STRING();
    Col_Word subrope = Col_Subrope(rope, 1, SIZE_MAX);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, 1, SIZE_MAX, 0,
                                           ropeChunkCounter, &ropeData,
                                           NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(subrope, 0, SIZE_MAX, 0,
                                           ropeChunkCounter, &subropeData,
                                           NULL) == 0);
    PICOTEST_ASSERT(ropeData.nbChunks == 1);
    PICOTEST_ASSERT(subropeData.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(ropeData, 0).length ==
                    CHUNK_INFO(subropeData, 0).length);
    PICOTEST_ASSERT(CHUNK_INFO(ropeData, 0).chunks[0].format ==
                    CHUNK_INFO(subropeData, 0).chunks[0].format);
    PICOTEST_ASSERT(CHUNK_INFO(ropeData, 0).chunks[0].data ==
                    CHUNK_INFO(subropeData, 0).chunks[0].data);
    PICOTEST_ASSERT(CHUNK_INFO(ropeData, 0).chunks[0].byteLength ==
                    CHUNK_INFO(subropeData, 0).chunks[0].byteLength);
}

PICOTEST_SUITE(testTraverseConcatRope, testTraverseConcatStringsHaveTwoChunks,
               testTraverseConcatStringsLeftArmHaveOneChunk,
               testTraverseConcatStringsRightArmHaveOneChunk,
               testTraverseConcatSubropeHasTwoChunks);
PICOTEST_CASE(testTraverseConcatStringsHaveTwoChunks, colibriFixture) {
    DECLARE_CHUNKS_DATA(leftData, 1);
    DECLARE_CHUNKS_DATA(rightData, 1);
    DECLARE_CHUNKS_DATA(concatRopeData, 2);
    Col_Word left = FLAT_STRING(), right = SMALL_STRING();
    Col_Word concatRope = Col_ConcatRopes(left, right);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(left, 1, SIZE_MAX, 0,
                                           ropeChunkCounter, &leftData,
                                           NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(right, 0, 5, 0, ropeChunkCounter,
                                           &rightData, NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(concatRope, 1, FLAT_STRING_LEN + 4,
                                           0, ropeChunkCounter, &concatRopeData,
                                           NULL) == 0);
    PICOTEST_ASSERT(leftData.nbChunks == 1);
    PICOTEST_ASSERT(rightData.nbChunks == 1);
    PICOTEST_ASSERT(concatRopeData.nbChunks == 2);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).length ==
                    CHUNK_INFO(concatRopeData, 0).length);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).chunks[0].format ==
                    CHUNK_INFO(concatRopeData, 0).chunks[0].format);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).chunks[0].data ==
                    CHUNK_INFO(concatRopeData, 0).chunks[0].data);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).chunks[0].byteLength ==
                    CHUNK_INFO(concatRopeData, 0).chunks[0].byteLength);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).length ==
                    CHUNK_INFO(concatRopeData, 1).length);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).chunks[0].format ==
                    CHUNK_INFO(concatRopeData, 1).chunks[0].format);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).chunks[0].data ==
                    CHUNK_INFO(concatRopeData, 1).chunks[0].data);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).chunks[0].byteLength ==
                    CHUNK_INFO(concatRopeData, 1).chunks[0].byteLength);
}
PICOTEST_CASE(testTraverseConcatStringsLeftArmHaveOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(leftData, 1);
    DECLARE_CHUNKS_DATA(concatRopeData, 1);
    Col_Word left = FLAT_STRING(), right = SMALL_STRING();
    Col_Word concatRope = Col_ConcatRopes(left, right);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(left, 1, 5, 0, ropeChunkCounter,
                                           &leftData, NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(concatRope, 1, 5, 0,
                                           ropeChunkCounter, &concatRopeData,
                                           NULL) == 0);
    PICOTEST_ASSERT(concatRopeData.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).length ==
                    CHUNK_INFO(concatRopeData, 0).length);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).chunks[0].format ==
                    CHUNK_INFO(concatRopeData, 0).chunks[0].format);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).chunks[0].data ==
                    CHUNK_INFO(concatRopeData, 0).chunks[0].data);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).chunks[0].byteLength ==
                    CHUNK_INFO(concatRopeData, 0).chunks[0].byteLength);
}
PICOTEST_CASE(testTraverseConcatStringsRightArmHaveOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(rightData, 1);
    DECLARE_CHUNKS_DATA(concatRopeData, 1);
    Col_Word left = FLAT_STRING(), right = SMALL_STRING();
    Col_Word concatRope = Col_ConcatRopes(left, right);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(right, 1, 5, 0, ropeChunkCounter,
                                           &rightData, NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(concatRope, FLAT_STRING_LEN + 1, 5,
                                           0, ropeChunkCounter, &concatRopeData,
                                           NULL) == 0);
    PICOTEST_ASSERT(concatRopeData.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).length ==
                    CHUNK_INFO(concatRopeData, 0).length);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).chunks[0].format ==
                    CHUNK_INFO(concatRopeData, 0).chunks[0].format);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).chunks[0].data ==
                    CHUNK_INFO(concatRopeData, 0).chunks[0].data);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).chunks[0].byteLength ==
                    CHUNK_INFO(concatRopeData, 0).chunks[0].byteLength);
}
PICOTEST_CASE(testTraverseConcatSubropeHasTwoChunks, colibriFixture) {
    DECLARE_CHUNKS_DATA(leftData, 1);
    DECLARE_CHUNKS_DATA(rightData, 1);
    DECLARE_CHUNKS_DATA(subropeData, 2);
    Col_Word left = FLAT_STRING_UCS1(), right = FLAT_STRING_UTF8();
    Col_Word concatRope = Col_ConcatRopes(left, right);
    Col_Word subrope = Col_Subrope(concatRope, 1, FLAT_STRING_LEN * 2 - 2);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(left, 2, SIZE_MAX, 0,
                                           ropeChunkCounter, &leftData,
                                           NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(right, 0, 6, 0, ropeChunkCounter,
                                           &rightData, NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(subrope, 1, FLAT_STRING_LEN + 4, 0,
                                           ropeChunkCounter, &subropeData,
                                           NULL) == 0);
    PICOTEST_ASSERT(leftData.nbChunks == 1);
    PICOTEST_ASSERT(rightData.nbChunks == 1);
    PICOTEST_ASSERT(subropeData.nbChunks == 2);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).length ==
                    CHUNK_INFO(subropeData, 0).length);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).chunks[0].format ==
                    CHUNK_INFO(subropeData, 0).chunks[0].format);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).chunks[0].data ==
                    CHUNK_INFO(subropeData, 0).chunks[0].data);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).chunks[0].byteLength ==
                    CHUNK_INFO(subropeData, 0).chunks[0].byteLength);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).length ==
                    CHUNK_INFO(subropeData, 1).length);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).chunks[0].format ==
                    CHUNK_INFO(subropeData, 1).chunks[0].format);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).chunks[0].data ==
                    CHUNK_INFO(subropeData, 1).chunks[0].data);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).chunks[0].byteLength ==
                    CHUNK_INFO(subropeData, 1).chunks[0].byteLength);
}

PICOTEST_SUITE(testTraverseRopeRange, testTraverseSingleCharacter);
PICOTEST_SUITE(testTraverseSingleCharacter, testTraverseSingleCharacterUcs1,
               testTraverseSingleCharacterUcs2, testTraverseSingleCharacterUcs4,
               testTraverseSingleCharacterUtf8,
               testTraverseSingleCharacterUtf16);
static void checkTraverseCharacterUcs(Col_Word rope, Col_StringFormat format,
                                      size_t index) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Char c = Col_RopeAt(rope, index);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, index, 1, 0, ropeChunkCounter,
                                           &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].format == format);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].byteLength == (size_t)format);
    PICOTEST_ASSERT(COL_CHAR_GET(format, CHUNK_INFO(data, 0).chunks[0].data) ==
                    c);
}
PICOTEST_CASE(testTraverseSingleCharacterUcs1, colibriFixture) {
    Col_Word rope = FLAT_STRING_UCS1();
    checkTraverseCharacterUcs(rope, COL_UCS1, 0);
    checkTraverseCharacterUcs(rope, COL_UCS1, FLAT_STRING_LEN / 2);
    checkTraverseCharacterUcs(rope, COL_UCS1, FLAT_STRING_LEN - 1);
}
PICOTEST_CASE(testTraverseSingleCharacterUcs2, colibriFixture) {
    Col_Word rope = FLAT_STRING_UCS2();
    checkTraverseCharacterUcs(rope, COL_UCS2, 0);
    checkTraverseCharacterUcs(rope, COL_UCS2, FLAT_STRING_LEN / 2);
    checkTraverseCharacterUcs(rope, COL_UCS2, FLAT_STRING_LEN - 1);
}
PICOTEST_CASE(testTraverseSingleCharacterUcs4, colibriFixture) {
    Col_Word rope = FLAT_STRING_UCS4();
    checkTraverseCharacterUcs(rope, COL_UCS4, 0);
    checkTraverseCharacterUcs(rope, COL_UCS4, FLAT_STRING_LEN / 2);
    checkTraverseCharacterUcs(rope, COL_UCS4, FLAT_STRING_LEN - 1);
}
static void checkTraverseCharacterUtf8(Col_Word rope, size_t index,
                                       Col_Char c) {
    DECLARE_CHUNKS_DATA(data, 1);
    PICOTEST_ASSERT(Col_RopeAt(rope, index) == c);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, index, 1, 0, ropeChunkCounter,
                                           &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].format == COL_UTF8);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].byteLength ==
                    COL_UTF8_WIDTH(c) * sizeof(Col_Char1));
    PICOTEST_ASSERT(
        COL_CHAR_GET(COL_UTF8, CHUNK_INFO(data, 0).chunks[0].data) == c);
    PICOTEST_ASSERT(Col_Utf8Get(CHUNK_INFO(data, 0).chunks[0].data) == c);
}
PICOTEST_CASE(testTraverseSingleCharacterUtf8, colibriFixture) {
    Col_Word rope = FLAT_STRING_UTF8();
    Col_Char first = Col_RopeAt(rope, 0);
    checkTraverseCharacterUtf8(rope, 0, first);
    checkTraverseCharacterUtf8(rope, FLAT_STRING_LEN / 2,
                               first + FLAT_STRING_LEN / 2);
    checkTraverseCharacterUtf8(rope, FLAT_STRING_LEN - 1,
                               first + FLAT_STRING_LEN - 1);
}
static void checkTraverseCharacterUtf16(Col_Word rope, size_t index,
                                        Col_Char c) {
    DECLARE_CHUNKS_DATA(data, 1);
    PICOTEST_ASSERT(Col_RopeAt(rope, index) == c);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, index, 1, 0, ropeChunkCounter,
                                           &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].format == COL_UTF16);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].byteLength ==
                    COL_UTF16_WIDTH(c) * sizeof(Col_Char2));
    PICOTEST_ASSERT(
        COL_CHAR_GET(COL_UTF16, CHUNK_INFO(data, 0).chunks[0].data) == c);
    PICOTEST_ASSERT(Col_Utf16Get(CHUNK_INFO(data, 0).chunks[0].data) == c);
}
PICOTEST_CASE(testTraverseSingleCharacterUtf16, colibriFixture) {
    Col_Word rope = FLAT_STRING_UTF16();
    Col_Char first = Col_RopeAt(rope, 0);
    checkTraverseCharacterUtf16(rope, 0, first);
    checkTraverseCharacterUtf16(rope, FLAT_STRING_LEN / 2,
                                first + FLAT_STRING_LEN / 2);
    checkTraverseCharacterUtf16(rope, FLAT_STRING_LEN - 1,
                                first + FLAT_STRING_LEN - 1);
}

typedef struct RopeChunkBreakData {
    size_t max;
    size_t counter;
} RopeChunkBreakData;
static int ropeChunkBreak(size_t index, size_t length, size_t number,
                          const Col_RopeChunk *chunks,
                          Col_ClientData clientData) {
    RopeChunkBreakData *data = (RopeChunkBreakData *)clientData;

    if (++data->counter >= data->max)
        return data->counter;
    return 0;
}
PICOTEST_CASE(testTraverseRopeBreak, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 10);
    Col_Word rope = Col_RepeatRope(FLAT_STRING(), 10);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0,
                                           ropeChunkCounter, &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 10);

    RopeChunkBreakData breakData = {5, 0};
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0, ropeChunkBreak,
                                           &breakData, NULL) == 5);
    PICOTEST_ASSERT(breakData.counter == 5);

    RopeChunkBreakData breakData2 = {20, 0};
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0, ropeChunkBreak,
                                           &breakData2, NULL) == 0);
    PICOTEST_ASSERT(breakData2.counter == 10);
}

PICOTEST_SUITE(testTraverseMultipleRopes, testTraverseMultipleEmptyRopesIsNoop,
               testTraverseMultipleRopesPastEndIsNoop,
               testTraverseMultipleRopesZeroCharIsNoop,
               testTraverseEquallySizedStrings, testTraverseVaryingSizedStrings,
               testTraverseMultipleRopesBreak);
PICOTEST_CASE(testTraverseMultipleEmptyRopesIsNoop, colibriFixture) {
    Col_Word ropes[] = {Col_EmptyRope(), Col_EmptyRope(), Col_EmptyRope()};
    size_t nbRopes = sizeof(ropes) / sizeof(*ropes);
    DECLARE_CHUNKS_DATA_N(data, nbRopes, 1);
    PICOTEST_ASSERT(Col_TraverseRopeChunksN(nbRopes, ropes, 0, SIZE_MAX,
                                            ropeChunkCounter, &data,
                                            NULL) == -1);
    PICOTEST_ASSERT(data.nbChunks == 0);
}
PICOTEST_CASE(testTraverseMultipleRopesPastEndIsNoop, colibriFixture) {
    Col_Word ropes[] = {Col_EmptyRope(), SMALL_STRING(), FLAT_STRING()};
    size_t nbRopes = sizeof(ropes) / sizeof(*ropes);
    DECLARE_CHUNKS_DATA_N(data, nbRopes, 1);
    PICOTEST_ASSERT(Col_TraverseRopeChunksN(nbRopes, ropes, FLAT_STRING_LEN,
                                            SIZE_MAX, ropeChunkCounter, &data,
                                            NULL) == -1);
    PICOTEST_ASSERT(data.nbChunks == 0);
}
PICOTEST_CASE(testTraverseMultipleRopesZeroCharIsNoop, colibriFixture) {
    Col_Word ropes[] = {Col_EmptyRope(), SMALL_STRING(), FLAT_STRING()};
    size_t nbRopes = sizeof(ropes) / sizeof(*ropes);
    DECLARE_CHUNKS_DATA_N(data, nbRopes, 1);
    PICOTEST_ASSERT(Col_TraverseRopeChunksN(nbRopes, ropes, 0, 0,
                                            ropeChunkCounter, &data,
                                            NULL) == -1);
    PICOTEST_ASSERT(data.nbChunks == 0);
}
PICOTEST_CASE(testTraverseEquallySizedStrings, colibriFixture) {
    Col_Word ropes[] = {FLAT_STRING_UCS1(), FLAT_STRING_UTF8(),
                        FLAT_STRING_UTF16()};
    size_t nbRopes = sizeof(ropes) / sizeof(*ropes);
    DECLARE_CHUNKS_DATA_N(data, nbRopes, 1);
    PICOTEST_ASSERT(Col_TraverseRopeChunksN(nbRopes, ropes, 0, SIZE_MAX,
                                            ropeChunkCounter, &data,
                                            NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == FLAT_STRING_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].format == COL_UCS1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].byteLength ==
                    FLAT_STRING_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[1].format == COL_UTF8);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[2].format == COL_UTF16);
}
PICOTEST_CASE(testTraverseVaryingSizedStrings, colibriFixture) {
    Col_Word ropes[] = {SMALL_STRING(), SHORT_STRING(), FLAT_STRING()};
    size_t nbRopes = sizeof(ropes) / sizeof(*ropes);
    DECLARE_CHUNKS_DATA_N(data, nbRopes, 3);
    PICOTEST_ASSERT(Col_TraverseRopeChunksN(nbRopes, ropes, 0, SIZE_MAX,
                                            ropeChunkCounter, &data,
                                            NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 3);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == SMALL_STRING_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].format == COL_UCS1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0].byteLength ==
                    SMALL_STRING_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[1].format == COL_UCS1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[1].byteLength ==
                    SMALL_STRING_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[2].format == COL_UCS1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[2].byteLength ==
                    SMALL_STRING_LEN);

    PICOTEST_ASSERT(CHUNK_INFO(data, 1).length ==
                    SHORT_STRING_LEN - SMALL_STRING_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 1).chunks[0].data == NULL);
    PICOTEST_ASSERT(CHUNK_INFO(data, 1).chunks[1].format == COL_UCS1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 1).chunks[1].byteLength ==
                    SHORT_STRING_LEN - SMALL_STRING_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 1).chunks[2].format == COL_UCS1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 1).chunks[2].byteLength ==
                    SHORT_STRING_LEN - SMALL_STRING_LEN);

    PICOTEST_ASSERT(CHUNK_INFO(data, 2).length ==
                    FLAT_STRING_LEN - SHORT_STRING_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 2).chunks[0].data == NULL);
    PICOTEST_ASSERT(CHUNK_INFO(data, 2).chunks[1].data == NULL);
    PICOTEST_ASSERT(CHUNK_INFO(data, 2).chunks[2].format == COL_UCS1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 2).chunks[2].byteLength ==
                    FLAT_STRING_LEN - SHORT_STRING_LEN);
}
PICOTEST_CASE(testTraverseMultipleRopesBreak, colibriFixture) {
    Col_Word ropes[] = {Col_RepeatRope(SMALL_STRING(), 10),
                        Col_RepeatRope(SHORT_STRING(), 10),
                        Col_RepeatRope(FLAT_STRING(), 10)};
    size_t nbRopes = sizeof(ropes) / sizeof(*ropes);
    DECLARE_CHUNKS_DATA_N(data, nbRopes, 100);
    PICOTEST_ASSERT(Col_TraverseRopeChunksN(nbRopes, ropes, 0, SIZE_MAX,
                                            ropeChunkCounter, &data,
                                            NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 13);

    RopeChunkBreakData breakData = {5, 0};
    PICOTEST_ASSERT(Col_TraverseRopeChunksN(nbRopes, ropes, 0, SIZE_MAX,
                                            ropeChunkBreak, &breakData,
                                            NULL) == 5);
    PICOTEST_ASSERT(breakData.counter == 5);

    RopeChunkBreakData breakData2 = {20, 0};
    PICOTEST_ASSERT(Col_TraverseRopeChunksN(nbRopes, ropes, 0, SIZE_MAX,
                                            ropeChunkBreak, &breakData2,
                                            NULL) == 0);
    PICOTEST_ASSERT(breakData2.counter == 13);
}

PICOTEST_SUITE(testRopeIteration, testRopeIteratorErrors,
               testRopeIteratorInitialize, testRopeIteratorCompare,
               testRopeIteratorAccess, testRopeIteratorMove,
               testRopeIteratorEmptyRope, testRopeIteratorString);

PICOTEST_SUITE(testRopeIteratorErrors, testRopeIteratorAtEndIsInvalid);
PICOTEST_CASE(testRopeIteratorAtEndIsInvalid, colibriFixture) {
    PICOTEST_ASSERT(ropeIterForward_valueCheck(NULL) == 1);
    PICOTEST_ASSERT(ropeIterNext_valueCheck(NULL) == 1);
    PICOTEST_ASSERT(ropeIterAt_valueCheck(NULL) == 1);
}

PICOTEST_SUITE(testRopeIteratorInitialize, testRopeIterNull, testRopeIterBegin,
               testRopeIterBeginMax, testRopeIterFirst, testRopeIterLast,
               testRopeIterSet);

PICOTEST_CASE(testRopeIterNull, colibriFixture) {
    Col_RopeIterator it = COL_ROPEITER_NULL;
    PICOTEST_ASSERT(Col_RopeIterNull(it));
    PICOTEST_ASSERT(Col_RopeIterRope(it) == WORD_NIL);
    PICOTEST_ASSERT(Col_RopeIterLength(it) == 0);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == 0);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
}
PICOTEST_CASE(testRopeIterBegin, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    size_t index = FLAT_STRING_LEN / 2;
    Col_RopeIterator it;
    Col_RopeIterBegin(it, rope, index);
    PICOTEST_ASSERT(!Col_RopeIterNull(it));
    PICOTEST_ASSERT(Col_RopeIterRope(it) == rope);
    PICOTEST_ASSERT(Col_RopeIterLength(it) == FLAT_STRING_LEN);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == index);
    PICOTEST_ASSERT(!Col_RopeIterEnd(it));
}
PICOTEST_CASE(testRopeIterBeginMax, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    size_t index = FLAT_STRING_LEN / 2;
    Col_RopeIterator it;
    Col_RopeIterBegin(it, rope, SIZE_MAX);
    PICOTEST_ASSERT(!Col_RopeIterNull(it));
    PICOTEST_ASSERT(Col_RopeIterRope(it) == rope);
    PICOTEST_ASSERT(Col_RopeIterLength(it) == FLAT_STRING_LEN);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == FLAT_STRING_LEN);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
}
PICOTEST_CASE(testRopeIterFirst, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    Col_RopeIterator it;
    Col_RopeIterFirst(it, rope);
    PICOTEST_ASSERT(!Col_RopeIterNull(it));
    PICOTEST_ASSERT(Col_RopeIterRope(it) == rope);
    PICOTEST_ASSERT(Col_RopeIterLength(it) == FLAT_STRING_LEN);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == 0);
    PICOTEST_ASSERT(!Col_RopeIterEnd(it));
}
PICOTEST_CASE(testRopeIterLast, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    Col_RopeIterator it;
    Col_RopeIterLast(it, rope);
    PICOTEST_ASSERT(!Col_RopeIterNull(it));
    PICOTEST_ASSERT(Col_RopeIterRope(it) == rope);
    PICOTEST_ASSERT(Col_RopeIterLength(it) == FLAT_STRING_LEN);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == FLAT_STRING_LEN - 1);
    PICOTEST_ASSERT(!Col_RopeIterEnd(it));
}
PICOTEST_CASE(testRopeIterSet, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    Col_RopeIterator it1;

    Col_RopeIterBegin(it1, rope, FLAT_STRING_LEN / 2);
    PICOTEST_ASSERT(!Col_RopeIterNull(it1));

    Col_RopeIterator it2 = COL_ROPEITER_NULL;
    PICOTEST_ASSERT(Col_RopeIterNull(it2));

    Col_RopeIterSet(it2, it1);
    PICOTEST_ASSERT(!Col_RopeIterNull(it1));
    PICOTEST_ASSERT(!Col_RopeIterNull(it2));
    PICOTEST_ASSERT(Col_RopeIterRope(it1) == Col_RopeIterRope(it2));
    PICOTEST_ASSERT(Col_RopeIterLength(it1) == Col_RopeIterLength(it2));
    PICOTEST_ASSERT(Col_RopeIterIndex(it1) == Col_RopeIterIndex(it2));

    Col_RopeIterNext(it1);
    PICOTEST_ASSERT(Col_RopeIterIndex(it1) == Col_RopeIterIndex(it2) + 1);
    Col_RopeIterPrevious(it2);
    PICOTEST_ASSERT(Col_RopeIterIndex(it1) == Col_RopeIterIndex(it2) + 2);
}

PICOTEST_SUITE(testRopeIteratorCompare, testRopeIteratorCompareFirst,
               testRopeIteratorCompareLast, testRopeIteratorCompareEnd,
               testRopeIteratorCompareMiddle);
PICOTEST_CASE(testRopeIteratorCompareFirst, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    Col_RopeIterator it1, it2;

    Col_RopeIterFirst(it1, rope);
    Col_RopeIterBegin(it2, rope, 0);
    PICOTEST_ASSERT(Col_RopeIterCompare(it1, it2) == 0);
    PICOTEST_ASSERT(Col_RopeIterCompare(it2, it1) == 0);
}
PICOTEST_CASE(testRopeIteratorCompareLast, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    Col_RopeIterator it1, it2;

    Col_RopeIterLast(it1, rope);
    Col_RopeIterBegin(it2, rope, FLAT_STRING_LEN - 1);
    PICOTEST_ASSERT(Col_RopeIterCompare(it1, it2) == 0);
    PICOTEST_ASSERT(Col_RopeIterCompare(it2, it1) == 0);
}
PICOTEST_CASE(testRopeIteratorCompareEnd, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    Col_RopeIterator it1, it2;

    Col_RopeIterLast(it1, rope);
    Col_RopeIterBegin(it2, rope, FLAT_STRING_LEN);
    PICOTEST_ASSERT(Col_RopeIterEnd(it2));
    PICOTEST_ASSERT(Col_RopeIterCompare(it1, it2) < 0);
    PICOTEST_ASSERT(Col_RopeIterCompare(it2, it1) > 0);
}
PICOTEST_CASE(testRopeIteratorCompareMiddle, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    Col_RopeIterator it1, it2;

    Col_RopeIterBegin(it1, rope, 1);
    Col_RopeIterBegin(it2, rope, 2);
    PICOTEST_ASSERT(Col_RopeIterCompare(it1, it2) < 0);
    PICOTEST_ASSERT(Col_RopeIterCompare(it2, it1) > 0);
}

PICOTEST_SUITE(testRopeIteratorAccess, testRopeIterAtCharacterWord,
               testRopeIterAtSmallString, testRopeIterAtFlatString,
               testRopeIterAtSubrope, testRopeIterAtConcatRope);

PICOTEST_CASE(testRopeIterAtCharacterWord, colibriFixture) {
    Col_Char c = 'a';
    Col_Word rope = Col_NewCharWord(c);
    Col_RopeIterator it;

    Col_RopeIterFirst(it, rope);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == c);

    Col_RopeIterLast(it, rope);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == c);
}

static int ropeChunkGetChar(size_t index, size_t length, size_t number,
                            const Col_RopeChunk *chunks,
                            Col_ClientData clientData) {
    Col_Char *data = (Col_Char *)clientData;
    *data = COL_CHAR_GET(chunks[0].format, chunks[0].data);
    return 0;
}
static void checkRopeIterAt(Col_Word rope) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Char c;
    Col_RopeIterator it;

    Col_TraverseRopeChunks(rope, 0, 1, 0, ropeChunkGetChar, &c, NULL);
    Col_RopeIterFirst(it, rope);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == c);

    Col_TraverseRopeChunks(rope, 1, 1, 0, ropeChunkGetChar, &c, NULL);
    Col_RopeIterBegin(it, rope, 1);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == c);

    Col_TraverseRopeChunks(rope, Col_RopeLength(rope) - 1, 1, 0,
                           ropeChunkGetChar, &c, NULL);
    Col_RopeIterLast(it, rope);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == c);

    Col_RopeIterNext(it);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));

    Col_TraverseRopeChunks(rope, Col_RopeLength(rope) - 2, 1, 0,
                           ropeChunkGetChar, &c, NULL);
    Col_RopeIterBackward(it, 2);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == Col_RopeLength(rope) - 2);
    PICOTEST_ASSERT(!Col_RopeIterEnd(it));
    PICOTEST_ASSERT(Col_RopeIterAt(it) == c);

    Col_RopeIterForward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));

    Col_TraverseRopeChunks(rope, Col_RopeLength(rope) - 1, 1, 0,
                           ropeChunkGetChar, &c, NULL);
    Col_RopeIterPrevious(it);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == Col_RopeLength(rope) - 1);
    PICOTEST_ASSERT(!Col_RopeIterEnd(it));
    PICOTEST_ASSERT(Col_RopeIterAt(it) == c);
}
PICOTEST_CASE(testRopeIterAtSmallString, colibriFixture) {
    checkRopeIterAt(SMALL_STRING());
}
PICOTEST_CASE(testRopeIterAtFlatString, colibriFixture) {
    checkRopeIterAt(FLAT_STRING_UCS1());
    checkRopeIterAt(FLAT_STRING_UCS2());
    checkRopeIterAt(FLAT_STRING_UCS4());
    checkRopeIterAt(FLAT_STRING_UTF8());
    checkRopeIterAt(FLAT_STRING_UTF16());
}
PICOTEST_CASE(testRopeIterAtSubrope, colibriFixture) {
    checkRopeIterAt(Col_Subrope(FLAT_STRING(), 1, SIZE_MAX));
}
PICOTEST_CASE(testRopeIterAtConcatRope, colibriFixture) {
    checkRopeIterAt(Col_ConcatRopes(FLAT_STRING(), SMALL_STRING()));
}

PICOTEST_SUITE(testRopeIteratorMove, testRopeIterNext, testRopeIterPrevious,
               testRopeIterForward, testRopeIterBackward, testRopeIterMoveTo);

PICOTEST_SUITE(testRopeIterNext, testRopeIterNextFromLastIsAtEnd,
               testRopeIterNextIterAt);
PICOTEST_CASE(testRopeIterNextFromLastIsAtEnd, colibriFixture) {
    Col_Word rope = SHORT_STRING();
    Col_RopeIterator it;

    Col_RopeIterLast(it, rope);
    PICOTEST_ASSERT(!Col_RopeIterEnd(it));
    Col_RopeIterNext(it);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
}

static void checkRopeIterNextIterAt(Col_Word rope) {
    Col_Char c;
    Col_RopeIterator it;

    Col_RopeIterFirst(it, rope);
    Col_TraverseRopeChunks(rope, Col_RopeIterIndex(it), 1, 0, ropeChunkGetChar,
                           &c, NULL);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == c);

    Col_RopeIterNext(it);
    Col_TraverseRopeChunks(rope, Col_RopeIterIndex(it), 1, 0, ropeChunkGetChar,
                           &c, NULL);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == c);
}
PICOTEST_CASE(testRopeIterNextIterAt, colibriFixture) {
    checkRopeIterNextIterAt(SMALL_STRING());
    checkRopeIterNextIterAt(FLAT_STRING_UCS1());
    checkRopeIterNextIterAt(FLAT_STRING_UCS2());
    checkRopeIterNextIterAt(FLAT_STRING_UCS4());
    checkRopeIterNextIterAt(FLAT_STRING_UTF8());
    checkRopeIterNextIterAt(FLAT_STRING_UTF16());
}

PICOTEST_SUITE(testRopeIterPrevious, testRopeIterPreviousFromFirstIsAtEnd,
               testRopeIterPreviousFromEndIsLast, testRopeIterPreviousIterAt);
PICOTEST_CASE(testRopeIterPreviousFromFirstIsAtEnd, colibriFixture) {
    Col_Word rope = SHORT_STRING();
    Col_RopeIterator it;

    Col_RopeIterFirst(it, rope);
    PICOTEST_ASSERT(!Col_RopeIterEnd(it));
    Col_RopeIterPrevious(it);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
}
PICOTEST_CASE(testRopeIterPreviousFromEndIsLast, colibriFixture) {
    Col_Word rope = SHORT_STRING();
    Col_RopeIterator it;

    Col_RopeIterBegin(it, rope, SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
    Col_RopeIterPrevious(it);
    PICOTEST_ASSERT(!Col_RopeIterEnd(it));
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == SHORT_STRING_LEN - 1);
}

static void checkRopeIterPreviousIterAt(Col_Word rope) {
    Col_Char c;
    Col_RopeIterator it;

    Col_RopeIterLast(it, rope);
    Col_TraverseRopeChunks(rope, Col_RopeIterIndex(it), 1, 0, ropeChunkGetChar,
                           &c, NULL);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == c);

    Col_RopeIterPrevious(it);
    Col_TraverseRopeChunks(rope, Col_RopeIterIndex(it), 1, 0, ropeChunkGetChar,
                           &c, NULL);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == c);
}
PICOTEST_CASE(testRopeIterPreviousIterAt, colibriFixture) {
    checkRopeIterPreviousIterAt(SMALL_STRING());
    checkRopeIterPreviousIterAt(FLAT_STRING_UCS1());
    checkRopeIterPreviousIterAt(FLAT_STRING_UCS2());
    checkRopeIterPreviousIterAt(FLAT_STRING_UCS4());
    checkRopeIterPreviousIterAt(FLAT_STRING_UTF8());
    checkRopeIterPreviousIterAt(FLAT_STRING_UTF16());
}

PICOTEST_SUITE(testRopeIterForward, testRopeIterForwardZeroIsNoop,
               testRopeIterForwardMaxIsAtEnd, testRopeIterForwardIterAt);
PICOTEST_CASE(testRopeIterForwardZeroIsNoop, colibriFixture) {
    Col_Word rope = SHORT_STRING();
    Col_RopeIterator it;

    Col_RopeIterFirst(it, rope);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == 0);
    Col_RopeIterForward(it, 0);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == 0);
}
PICOTEST_CASE(testRopeIterForwardMaxIsAtEnd, colibriFixture) {
    Col_Word rope = SHORT_STRING();
    Col_RopeIterator it;

    Col_RopeIterFirst(it, rope);
    PICOTEST_ASSERT(!Col_RopeIterEnd(it));
    Col_RopeIterForward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
}

static void checkRopeIterForwardIterAt(Col_Word rope) {
    Col_Char c;
    Col_RopeIterator it;

    Col_RopeIterFirst(it, rope);
    Col_TraverseRopeChunks(rope, Col_RopeIterIndex(it), 1, 0, ropeChunkGetChar,
                           &c, NULL);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == c);

    Col_RopeIterForward(it, 1);
    Col_TraverseRopeChunks(rope, Col_RopeIterIndex(it), 1, 0, ropeChunkGetChar,
                           &c, NULL);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == c);
}
PICOTEST_CASE(testRopeIterForwardIterAt, colibriFixture) {
    checkRopeIterForwardIterAt(SMALL_STRING());
    checkRopeIterForwardIterAt(FLAT_STRING_UCS1());
    checkRopeIterForwardIterAt(FLAT_STRING_UCS2());
    checkRopeIterForwardIterAt(FLAT_STRING_UCS4());
    checkRopeIterForwardIterAt(FLAT_STRING_UTF8());
    checkRopeIterForwardIterAt(FLAT_STRING_UTF16());
}

PICOTEST_SUITE(testRopeIterBackward, testRopeIterBackwardZeroIsNoop,
               testRopeIterBackwardMaxIsAtEnd,
               testRopeIterBackwardOneFromEndIsLast,
               testRopeIterBackwardMaxFromEndIsNoop,
               testRopeIterBackwardIterAt);
PICOTEST_CASE(testRopeIterBackwardZeroIsNoop, colibriFixture) {
    Col_Word rope = SHORT_STRING();
    Col_RopeIterator it;

    Col_RopeIterFirst(it, rope);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == 0);
    Col_RopeIterBackward(it, 0);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == 0);
}
PICOTEST_CASE(testRopeIterBackwardMaxIsAtEnd, colibriFixture) {
    Col_Word rope = SHORT_STRING();
    Col_RopeIterator it;

    Col_RopeIterLast(it, rope);
    PICOTEST_ASSERT(!Col_RopeIterEnd(it));
    Col_RopeIterBackward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
}
PICOTEST_CASE(testRopeIterBackwardOneFromEndIsLast, colibriFixture) {
    Col_Word rope = SHORT_STRING();
    Col_RopeIterator it;

    Col_RopeIterBegin(it, rope, SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
    Col_RopeIterBackward(it, 1);
    PICOTEST_ASSERT(!Col_RopeIterEnd(it));
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == SHORT_STRING_LEN - 1);
}
PICOTEST_CASE(testRopeIterBackwardMaxFromEndIsNoop, colibriFixture) {
    Col_Word rope = SHORT_STRING();
    Col_RopeIterator it;

    Col_RopeIterBegin(it, rope, SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
    Col_RopeIterBackward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
}

static void checkRopeIterBackwardIterAt(Col_Word rope) {
    Col_Char c;
    Col_RopeIterator it;

    Col_RopeIterLast(it, rope);
    Col_TraverseRopeChunks(rope, Col_RopeIterIndex(it), 1, 0, ropeChunkGetChar,
                           &c, NULL);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == c);

    Col_RopeIterBackward(it, 1);
    Col_TraverseRopeChunks(rope, Col_RopeIterIndex(it), 1, 0, ropeChunkGetChar,
                           &c, NULL);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == c);
}
PICOTEST_CASE(testRopeIterBackwardIterAt, colibriFixture) {
    checkRopeIterBackwardIterAt(SMALL_STRING());
    checkRopeIterBackwardIterAt(FLAT_STRING_UCS1());
    checkRopeIterBackwardIterAt(FLAT_STRING_UCS2());
    checkRopeIterBackwardIterAt(FLAT_STRING_UCS4());
    checkRopeIterBackwardIterAt(FLAT_STRING_UTF8());
    checkRopeIterBackwardIterAt(FLAT_STRING_UTF16());
}

PICOTEST_SUITE(testRopeIterMoveTo, testRopeIterMoveToForward,
               testRopeIterMoveToBackward);
PICOTEST_CASE(testRopeIterMoveToForward, colibriFixture) {
    Col_Word rope = SMALL_STRING();
    Col_RopeIterator it;
    size_t index = SMALL_STRING_LEN / 2;

    Col_RopeIterBegin(it, rope, index);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == index);
    Col_RopeIterMoveTo(it, index + 1);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == index + 1);
}
PICOTEST_CASE(testRopeIterMoveToBackward, colibriFixture) {
    Col_Word rope = SMALL_STRING();
    Col_RopeIterator it;
    size_t index = SMALL_STRING_LEN / 2;

    Col_RopeIterBegin(it, rope, index);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == index);
    Col_RopeIterMoveTo(it, index - 1);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == index - 1);
}

PICOTEST_SUITE(testRopeIteratorEmptyRope, testEmptyRopeIterBegin,
               testEmptyRopeIterFirst, testEmptyRopeIterLast);
static void checkRopeIterEmpty(Col_RopeIterator it) {
    PICOTEST_ASSERT(!Col_RopeIterNull(it));
    PICOTEST_ASSERT(Col_RopeIterRope(it) == Col_EmptyRope());
    PICOTEST_ASSERT(Col_RopeIterLength(it) == 0);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == 0);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
}
PICOTEST_CASE(testEmptyRopeIterBegin, colibriFixture) {
    Col_Word empty = Col_EmptyRope();
    Col_RopeIterator it;

    Col_RopeIterBegin(it, empty, 0);
    checkRopeIterEmpty(it);
}
PICOTEST_CASE(testEmptyRopeIterFirst, colibriFixture) {
    Col_Word empty = Col_EmptyRope();
    Col_RopeIterator it;

    Col_RopeIterFirst(it, empty);
    checkRopeIterEmpty(it);
}
PICOTEST_CASE(testEmptyRopeIterLast, colibriFixture) {
    Col_Word empty = Col_EmptyRope();
    Col_RopeIterator it;

    Col_RopeIterLast(it, empty);
    checkRopeIterEmpty(it);
}

PICOTEST_SUITE(testRopeIteratorString, testRopeIterString, testRopeIterStringAt,
               testRopeIterStringFromEnd);
PICOTEST_CASE(testRopeIterString, colibriFixture) {
    DECLARE_ROPE_DATA_UCS(data, Col_Char1, FLAT_STRING_LEN, 'a', 'a' + i);
    Col_RopeIterator it;

    Col_RopeIterString(it, COL_UCS1, data, FLAT_STRING_LEN);
    PICOTEST_ASSERT(!Col_RopeIterNull(it));
    PICOTEST_ASSERT(Col_RopeIterRope(it) == WORD_NIL);
    PICOTEST_ASSERT(Col_RopeIterLength(it) == FLAT_STRING_LEN);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == 0);
    PICOTEST_ASSERT(!Col_RopeIterEnd(it));
    PICOTEST_ASSERT(Col_RopeIterAt(it) == data[0]);
}
PICOTEST_CASE(testRopeIterStringAt, colibriFixture) {
    DECLARE_ROPE_DATA_UCS(data, Col_Char1, FLAT_STRING_LEN, 'a', 'a' + i);
    Col_RopeIterator it;

    size_t index;

    index = 0;
    Col_RopeIterString(it, COL_UCS1, data, FLAT_STRING_LEN);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == index);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == data[index]);

    index = FLAT_STRING_LEN / 2;
    Col_RopeIterMoveTo(it, FLAT_STRING_LEN / 2);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == index);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == data[index]);

    Col_RopeIterForward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
}
PICOTEST_CASE(testRopeIterStringFromEnd, colibriFixture) {
    DECLARE_ROPE_DATA_UCS(data, Col_Char1, FLAT_STRING_LEN, 'a', 'a' + i);
    Col_RopeIterator it;

    size_t index;

    index = 0;
    Col_RopeIterString(it, COL_UCS1, data, FLAT_STRING_LEN);
    Col_RopeIterMoveTo(it, FLAT_STRING_LEN / 2);
    Col_RopeIterForward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));

    index = FLAT_STRING_LEN - 1;
    Col_RopeIterPrevious(it);
    PICOTEST_ASSERT(!Col_RopeIterEnd(it));
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == index);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == data[index]);

    Col_RopeIterForward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));

    index = FLAT_STRING_LEN / 2;
    Col_RopeIterMoveTo(it, index);
    PICOTEST_ASSERT(!Col_RopeIterEnd(it));
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == index);
    PICOTEST_ASSERT(Col_RopeIterAt(it) == data[index]);
}

PICOTEST_SUITE(testRopeSearchAndComparison, testRopeFind, testRopeSearch,
               testCompareRopes);

static void checkRopeFindChar(Col_Word rope, size_t pos) {
    size_t len = Col_RopeLength(rope);
    Col_Char c = Col_RopeAt(rope, pos);
    PICOTEST_ASSERT(Col_RopeFindFirst(rope, c) == pos);
    PICOTEST_ASSERT(Col_RopeFindFirstN(rope, c, pos) == SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeFindFirstN(rope, c, pos + 1) == pos);
    PICOTEST_ASSERT(Col_RopeFindLast(rope, c) == pos);
    PICOTEST_ASSERT(Col_RopeFindLastN(rope, c, len - pos - 1) == SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeFindLastN(rope, c, len - pos) == pos);
    PICOTEST_ASSERT(Col_RopeFind(rope, c, pos + 1, SIZE_MAX, 0) == SIZE_MAX);
    if (pos > 0)
        PICOTEST_ASSERT(Col_RopeFind(rope, c, pos - 1, SIZE_MAX, 1) ==
                        SIZE_MAX);
}
static void checkRopeFind(Col_Word rope) {
    size_t len = Col_RopeLength(rope);
    checkRopeFindChar(rope, 0);
    checkRopeFindChar(rope, len / 3);
    checkRopeFindChar(rope, len - 1);
}
PICOTEST_CASE(testRopeFind, colibriFixture) {
    checkRopeFind(SMALL_STRING());
    checkRopeFind(SHORT_STRING_UCS1());
    checkRopeFind(FLAT_STRING_UCS2());
    checkRopeFind(BIG_STRING_UCS());
    checkRopeFind(SHORT_STRING_UTF8());
    checkRopeFind(FLAT_STRING_UTF16());
    checkRopeFind(BIG_STRING_UTF());
}

PICOTEST_SUITE(testRopeSearch, testRopeSearchEmpty, testRopeSearchCharacter,
               testRopeSearchTooFar, testRopeSearchSubrope);
PICOTEST_CASE(testRopeSearchEmpty, colibriFixture) {
    PICOTEST_ASSERT(Col_RopeSearchFirst(SMALL_STRING(), Col_EmptyRope()) ==
                    SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeSearchFirst(Col_EmptyRope(), SMALL_STRING()) ==
                    SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeSearchLast(SMALL_STRING(), Col_EmptyRope()) ==
                    SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeSearchLast(Col_EmptyRope(), SMALL_STRING()) ==
                    SIZE_MAX);
}
PICOTEST_CASE(testRopeSearchCharacter, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    PICOTEST_ASSERT(
        Col_RopeSearchFirst(rope, Col_NewCharWord(Col_RopeAt(rope, 0))) == 0);
    PICOTEST_ASSERT(
        Col_RopeSearchFirst(
            rope, Col_NewCharWord(Col_RopeAt(rope, FLAT_STRING_LEN - 1))) ==
        FLAT_STRING_LEN - 1);
}
PICOTEST_CASE(testRopeSearchTooFar, colibriFixture) {
    Col_Word rope = Col_RepeatRope(Col_NewCharWord('a'), 10);
    Col_Word subrope = Col_RepeatRope(Col_NewCharWord('a'), 2);
    PICOTEST_ASSERT(Col_RopeSearch(rope, subrope, 9, SIZE_MAX, 0) == SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeSearch(rope, subrope, 9, 1, 1) == SIZE_MAX);
}
static void checkRopeSearchSubrope(Col_Word rope, size_t first, size_t len) {
    Col_Word subrope = Col_Subrope(rope, first, first + len - 1);

    PICOTEST_ASSERT(Col_RopeSearchFirst(rope, subrope) == first);
    PICOTEST_ASSERT(Col_RopeSearch(rope, subrope, first, SIZE_MAX, 0) == first);
    if (first > 0) {
        PICOTEST_ASSERT(Col_RopeSearch(rope, subrope, 0, first - 1, 0) ==
                        SIZE_MAX);
        PICOTEST_ASSERT(Col_RopeSearch(rope, subrope, first - 1, 1, 0) ==
                        SIZE_MAX);
    }
    PICOTEST_ASSERT(Col_RopeSearch(rope, subrope, first + 1, SIZE_MAX, 0) ==
                    SIZE_MAX);

    PICOTEST_ASSERT(Col_RopeSearchLast(rope, subrope) == first);
    PICOTEST_ASSERT(Col_RopeSearch(rope, subrope, SIZE_MAX, SIZE_MAX, 1) ==
                    first);
    PICOTEST_ASSERT(Col_RopeSearch(rope, subrope, first, SIZE_MAX, 1) == first);
    if (first > 0) {
        PICOTEST_ASSERT(Col_RopeSearch(rope, subrope, SIZE_MAX,
                                       Col_RopeLength(rope) - first - 1,
                                       1) == SIZE_MAX);
        PICOTEST_ASSERT(Col_RopeSearch(rope, subrope, first - 1, SIZE_MAX, 1) ==
                        SIZE_MAX);
    }
    PICOTEST_ASSERT(Col_RopeSearch(rope, subrope,
                                   Col_RopeLength(rope) - first + 1, 1,
                                   1) == SIZE_MAX);
}
PICOTEST_CASE(testRopeSearchSubrope, colibriFixture) {
    checkRopeSearchSubrope(FLAT_STRING(), 0, 10);
    checkRopeSearchSubrope(FLAT_STRING(), FLAT_STRING_LEN / 3, 10);
    checkRopeSearchSubrope(FLAT_STRING(), FLAT_STRING_LEN - 10, 10);
    checkRopeSearchSubrope(BIG_STRING_UCS(), 0, 10);
    checkRopeSearchSubrope(BIG_STRING_UTF(), BIG_STRING_UTF_LEN / 3, 10);
    checkRopeSearchSubrope(BIG_STRING_UTF(), BIG_STRING_UTF_LEN - 10, 10);
}

PICOTEST_SUITE(testCompareRopes, testCompareEmptyRopes,
               testCompareIdenticalRopes, testCompareToEmptyRope,
               testCompareStrings, testCompareRopesN, testCompareRopesL);
PICOTEST_CASE(testCompareEmptyRopes, colibriFixture) {
    Col_Word empty = Col_EmptyRope();
    PICOTEST_ASSERT(Col_CompareRopes(empty, empty) == 0);
}
PICOTEST_CASE(testCompareIdenticalRopes, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    PICOTEST_ASSERT(Col_CompareRopes(rope, rope) == 0);
}
PICOTEST_CASE(testCompareToEmptyRope, colibriFixture) {
    Col_Word empty = Col_EmptyRope();
    Col_Word rope = FLAT_STRING();
    PICOTEST_ASSERT(Col_CompareRopes(empty, rope) == -1);
    PICOTEST_ASSERT(Col_CompareRopes(rope, empty) == 1);
}
PICOTEST_CASE(testCompareStrings, colibriFixture) {
    PICOTEST_ASSERT(
        Col_CompareRopes(SHORT_STRING_UCS1(), SHORT_STRING_UCS2()) == 0);
    PICOTEST_ASSERT(
        Col_CompareRopes(SHORT_STRING_UCS2(), SHORT_STRING_UCS4()) == 0);
    PICOTEST_ASSERT(
        Col_CompareRopes(SHORT_STRING_UCS4(), SHORT_STRING_UTF8()) == 0);
    PICOTEST_ASSERT(
        Col_CompareRopes(SHORT_STRING_UTF8(), SHORT_STRING_UTF16()) == 0);
}
PICOTEST_CASE(testCompareRopesN, colibriFixture) {
    PICOTEST_ASSERT(Col_CompareRopesN(SHORT_STRING_UCS1(), MEDIUM_STRING_UCS2(),
                                      SHORT_STRING_LEN) == 0);
    PICOTEST_ASSERT(Col_CompareRopesN(SHORT_STRING_UCS2(), MEDIUM_STRING_UCS4(),
                                      SHORT_STRING_LEN) == 0);
    Col_Word left = SHORT_STRING_UCS1(), right = FLAT_STRING_UCS2();
    PICOTEST_ASSERT(Col_CompareRopesN(Col_ConcatRopes(left, right), left,
                                      SHORT_STRING_LEN) == 0);
}
PICOTEST_CASE(testCompareRopesL, colibriFixture) {
    size_t pos = SIZE_MAX;
    Col_Char c1 = COL_CHAR_INVALID, c2 = COL_CHAR_INVALID;
    Col_Word rope1 = Col_ConcatRopes(SHORT_STRING_UCS1(), FLAT_STRING_UCS1());
    Col_Word rope2 = Col_ConcatRopes(SHORT_STRING_UCS1(), FLAT_STRING_UCS2());
    PICOTEST_ASSERT(
        Col_CompareRopesL(rope1, rope2, 0, SIZE_MAX, &pos, &c1, &c2) == -1);
    PICOTEST_ASSERT(pos == SHORT_STRING_LEN);
    PICOTEST_ASSERT(c1 != c2);
    PICOTEST_ASSERT(c1 == Col_RopeAt(rope1, SHORT_STRING_LEN));
    PICOTEST_ASSERT(c2 == Col_RopeAt(rope2, SHORT_STRING_LEN));
    PICOTEST_ASSERT(Col_CompareRopesL(rope2, rope1, 0, SHORT_STRING_LEN, &pos,
                                      &c1, &c2) == 0);
    PICOTEST_ASSERT(Col_CompareRopesL(rope2, rope1, SHORT_STRING_LEN / 2,
                                      SHORT_STRING_LEN, &pos, &c1, &c2) == 1);
    PICOTEST_ASSERT(pos == SHORT_STRING_LEN);
    PICOTEST_ASSERT(c1 != c2);
    PICOTEST_ASSERT(c1 == Col_RopeAt(rope2, SHORT_STRING_LEN));
    PICOTEST_ASSERT(c2 == Col_RopeAt(rope1, SHORT_STRING_LEN));
}