#include <colibri.h>
#include <picotest.h>

/*
 * Test failure cases (must be defined before test hooks)
 */

#include "failureFixture.h"
PICOTEST_CASE(doRopeTypeCheck, failureFixture) {
    Col_RopeLength(WORD_NIL);
}
PICOTEST_CASE(doStringTypeCheck, failureFixture) {
    Col_StringWordFormat(WORD_NIL);
}
PICOTEST_CASE(doCharacterTypeCheck, failureFixture) {
    Col_CharWordValue(WORD_NIL);
}
// TODO gather all type/value checks into one suite
// PICOTEST_CASE(doRopeIterAtEndValueCheck, failureFixture) {
//     Col_RopeIterator it;
//     Col_RopeIterFirst(it, Col_EmptyRope());
//     PICOTEST_VERIFY(Col_RopeIterEnd(it));
//     Col_RopeIterAt(it);
// }
// PICOTEST_CASE(testRopeIterAtEndIsInvalid, colibriFixture) {
//     PICOTEST_ASSERT(doRopeIterAtEndValueCheck(NULL) == 2);
// }

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

/*
 * Ropes
 */

#include "hooks.h"
#include "colibriFixture.h"

PICOTEST_SUITE(testRopes, testRopeTypeCheck, testEmptyRope, testCharacterWords,
               testNewRope, testRopeOperations, testRopeTraversal,
               testRopeIteration);

PICOTEST_CASE(testRopeTypeCheck, colibriFixture) {
    PICOTEST_ASSERT(doRopeTypeCheck(NULL) == 1);
    PICOTEST_ASSERT(doStringTypeCheck(NULL) == 1);
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
    PICOTEST_ASSERT(doCharacterTypeCheck(NULL) == 1);
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
PICOTEST_SUITE(testNewRope, testNewRopeEmpty, testNewRopeUcs1, testNewRopeUcs2,
               testNewRopeUcs4, testNewRopeUcs, testNewRopeUtf8,
               testNewRopeUtf16, testNewRopeFromString);
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

PICOTEST_CASE(testNewRopeEmptyFromData, colibriFixture) {
    PICOTEST_ASSERT(Col_NewRope(COL_UCS1, NULL, 0) == Col_EmptyRope());
    PICOTEST_ASSERT(Col_NewRope(COL_UCS2, NULL, 0) == Col_EmptyRope());
    PICOTEST_ASSERT(Col_NewRope(COL_UCS4, NULL, 0) == Col_EmptyRope());
    PICOTEST_ASSERT(Col_NewRope(COL_UTF8, NULL, 0) == Col_EmptyRope());
    PICOTEST_ASSERT(Col_NewRope(COL_UTF16, NULL, 0) == Col_EmptyRope());
    PICOTEST_ASSERT(Col_NewRope(COL_UCS, NULL, 0) == Col_EmptyRope());
}
PICOTEST_CASE(testNewRopeEmptyFromString, colibriFixture) {
    PICOTEST_ASSERT(Col_NewRopeFromString("") == Col_EmptyRope());
}

static void checkRopeChar(Col_Word rope, Col_Char c, Col_StringFormat format) {
    PICOTEST_ASSERT(rope == Col_NewCharWord(c));
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
    PICOTEST_ASSERT(Col_RopeDepth(rope) >= 1);
}
PICOTEST_CASE(testNewRopeUtf16Big, colibriFixture) {
    Col_Word rope = NEW_ROPE_UTF16_BIG();
    PICOTEST_ASSERT(rope != NEW_ROPE_UTF16_BIG());
    PICOTEST_ASSERT(Col_WordType(rope) == COL_ROPE);
    PICOTEST_ASSERT(Col_RopeLength(rope) == ROPE_UTF_BIG_LEN);
    PICOTEST_ASSERT(Col_RopeDepth(rope) >= 1);
}

PICOTEST_CASE(testNewRopeFromStringBig, colibriFixture) {
    Col_Word rope = NEW_ROPE_STRING_BIG();
    PICOTEST_ASSERT(rope != NEW_ROPE_STRING_BIG());
    checkRope(rope, ROPE_STRING_BIG_LEN, 1);
}

PICOTEST_SUITE(testRopeOperations, testSubrope, testConcatRopes,
               testConcatRopesA, testConcatRopesV, testRepeatRope);
// TODO repeat/insert/remove/replace

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
    NEW_ROPE_UTF8(SHORT_STRING_LEN, COL_CHAR1_MAX, COL_CHAR1_MAX + i);
static Col_Word SHORT_STRING_UTF16()
    NEW_ROPE_UTF16(SHORT_STRING_LEN, COL_CHAR1_MAX, COL_CHAR1_MAX + i);
static Col_Word MEDIUM_STRING_UCS1()
    NEW_ROPE_UCS(Col_Char1, COL_UCS1, 80, 'a', 'a' + i);
static Col_Word MEDIUM_STRING_UCS2()
    NEW_ROPE_UCS(Col_Char2, COL_UCS2, 40, 'a', 'a' + i);
static Col_Word MEDIUM_STRING_UCS4()
    NEW_ROPE_UCS(Col_Char4, COL_UCS4, 20, 'a', 'a' + i);
static Col_Word MEDIUM_STRING_UTF8()
    NEW_ROPE_UTF8(40, COL_CHAR1_MAX, COL_CHAR1_MAX + i);
static Col_Word MEDIUM_STRING_UTF16()
    NEW_ROPE_UTF16(40, COL_CHAR1_MAX, COL_CHAR1_MAX + i);
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
#define BIG_STRING_LEN ROPE_UCS_BIG_LEN
#define BIG_STRING() NEW_ROPE_UCS1_BIG()

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
    PICOTEST_ASSERT(Col_Subrope(Col_EmptyRope(), 0, 0) == Col_EmptyRope());
}
PICOTEST_CASE(testSubropeOfEmptyRangeIsEmpty, colibriFixture) {
    Col_Word rope = SMALL_STRING();
    PICOTEST_ASSERT(Col_Subrope(rope, 1, 0) == Col_EmptyRope());
    PICOTEST_ASSERT(Col_Subrope(rope, 2, 1) == Col_EmptyRope());
    PICOTEST_ASSERT(Col_Subrope(rope, SMALL_STRING_LEN, SMALL_STRING_LEN) ==
                    Col_EmptyRope());
    PICOTEST_ASSERT(Col_Subrope(rope, SIZE_MAX, 2) == Col_EmptyRope());
    PICOTEST_ASSERT(Col_Subrope(rope, SIZE_MAX, SIZE_MAX) == Col_EmptyRope());
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
    PICOTEST_ASSERT(Col_Subrope(rope, 0, FLAT_STRING_LEN - 1) == left)
    PICOTEST_ASSERT(Col_Subrope(rope, FLAT_STRING_LEN, SIZE_MAX) == right)
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

PICOTEST_SUITE(testConcatRopes, testConcatRopeWithEmptyIsIdentity,
               testConcatShortStrings, testConcatFlatStringsIsNewRope,
               testConcatAdjacentSubropesIsOriginalRope,
               testConcatRopeBalancing);
PICOTEST_CASE(testConcatRopeWithEmptyIsIdentity, colibriFixture) {
    Col_Word rope = SMALL_STRING();
    PICOTEST_ASSERT(Col_ConcatRopes(rope, Col_EmptyRope()) == rope);
    PICOTEST_ASSERT(Col_ConcatRopes(Col_EmptyRope(), rope) == rope);
}
PICOTEST_SUITE(testConcatShortStrings, testMergeableConcats,
               testUnmergeableConcats);
PICOTEST_SUITE(testMergeableConcats, testMergedConcatUniform,
               testMergedConcatUpconvert);
static void checkMergedConcatRope(Col_Word left, Col_Word right,
                                  Col_StringFormat format) {
    size_t leftLen = Col_RopeLength(left), rightLen = Col_RopeLength(right);
    Col_Word rope = Col_ConcatRopes(left, right);
    checkRopeString(rope, leftLen + rightLen, format);
}
PICOTEST_CASE(testMergedConcatUniform, colibriFixture) {
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
PICOTEST_CASE(testMergedConcatUpconvert, colibriFixture) {
    checkMergedConcatRope(SHORT_STRING_UCS1(), SHORT_STRING_UCS2(), COL_UCS2);
    checkMergedConcatRope(SHORT_STRING_UCS1(), SHORT_STRING_UCS4(), COL_UCS4);
    checkMergedConcatRope(SHORT_STRING_UCS2(), SHORT_STRING_UCS1(), COL_UCS2);
    checkMergedConcatRope(SHORT_STRING_UCS2(), SHORT_STRING_UCS4(), COL_UCS4);
    checkMergedConcatRope(SHORT_STRING_UCS4(), SHORT_STRING_UCS1(), COL_UCS4);
    checkMergedConcatRope(SHORT_STRING_UCS4(), SHORT_STRING_UCS2(), COL_UCS4);
}

PICOTEST_SUITE(testUnmergeableConcats, testUnmergeableConcatsUniform,
               testUnmergeableConcatsUpconvert,
               testUnmergeableConcatsIncompatible);
static void checkUnmergedConcatRope(Col_Word left, Col_Word right,
                                    size_t depth) {
    size_t leftLen = Col_RopeLength(left), rightLen = Col_RopeLength(right);
    Col_Word rope = Col_ConcatRopes(left, right);
    checkRope(rope, leftLen + rightLen, depth);
}
PICOTEST_CASE(testUnmergeableConcatsUniform, colibriFixture) {
    checkUnmergedConcatRope(SHORT_STRING_UCS2(), MEDIUM_STRING_UCS2(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UCS4(), MEDIUM_STRING_UCS4(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UTF8(), MEDIUM_STRING_UTF8(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UTF16(), MEDIUM_STRING_UTF16(), 1);
}
PICOTEST_CASE(testUnmergeableConcatsUpconvert, colibriFixture) {
    checkUnmergedConcatRope(SHORT_STRING_UCS1(), MEDIUM_STRING_UCS2(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UCS2(), MEDIUM_STRING_UCS1(), 1);
    checkUnmergedConcatRope(MEDIUM_STRING_UCS1(), MEDIUM_STRING_UCS4(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UCS4(), MEDIUM_STRING_UCS1(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UCS2(), MEDIUM_STRING_UCS4(), 1);
    checkUnmergedConcatRope(SHORT_STRING_UCS4(), MEDIUM_STRING_UCS2(), 1);
}
PICOTEST_CASE(testUnmergeableConcatsIncompatible, colibriFixture) {
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
               testConcatBalancedBranchesAreNotRebalanced,
               testConcatInbalancedLeftOuterBranchesAreRotated,
               testConcatInbalancedRightOuterBranchesAreRotated,
               testConcatInbalancedLeftInnerBranchesAreSplit,
               testConcatInbalancedRightInnerBranchesAreSplit,
               testConcatInbalancedSubropeBranchesAreSplit);
PICOTEST_CASE(testConcatBalancedBranchesAreNotRebalanced, colibriFixture) {
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
PICOTEST_CASE(testConcatInbalancedLeftOuterBranchesAreRotated, colibriFixture) {
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
PICOTEST_CASE(testConcatInbalancedRightOuterBranchesAreRotated,
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
PICOTEST_CASE(testConcatInbalancedLeftInnerBranchesAreSplit, colibriFixture) {
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
PICOTEST_CASE(testConcatInbalancedRightInnerBranchesAreSplit, colibriFixture) {
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

PICOTEST_SUITE(testConcatRopesA, testConcatRopesAOne, testConcatRopesATwo,
               testConcatRopesARecurse);
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

PICOTEST_SUITE(testRepeatRope, testRepeatRopeZeroIsEmpty,
               testRepeatRopeOnceIsIdentity, testRepeatRopeTwiceIsConcatSelf,
               testRepeatRopeRecurse, testRepeatEmptyRope, testRepeatMax);

PICOTEST_CASE(testRepeatRopeZeroIsEmpty, colibriFixture) {
    Col_Word rope = FLAT_STRING();
    PICOTEST_ASSERT(Col_RepeatRope(rope, 0) == Col_EmptyRope());
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
    PICOTEST_ASSERT(Col_RepeatRope(Col_EmptyRope(), SIZE_MAX) ==
                    Col_EmptyRope());
}
PICOTEST_CASE(testRepeatMax, colibriFixture) {
    Col_Word rope = Col_RepeatRope(Col_NewCharWord('a'), SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeLength(rope) == SIZE_MAX);
    PICOTEST_ASSERT(Col_RopeDepth(rope) > 17);
}

PICOTEST_SUITE(testRopeTraversal, testTraverseRope);
// TODO reverse traversal and Col_TraverseRopeN

PICOTEST_SUITE(testTraverseRope, testTraverseProcMustNotBeNull,
               testTraverseEmptyRopeIsNoop,
               testTraverseCharacterWordHasOneChunk,
               testTraverseSmallStringHasOneChunk,
               testTraverseFlatStringHasOneChunk, testTraverseSubrope,
               testTraverseConcatRope, testTraverseRange, testTraverseBreak);

#define CHUNK_SAMPLE_SIZE 10
typedef struct RopeChunkInfo {
    size_t length;
    Col_RopeChunk chunk;
    char sample[CHUNK_SAMPLE_SIZE];
} RopeChunkInfo;
typedef struct RopeChunkCounterData {
    size_t size;
    size_t nbChunks;
    RopeChunkInfo *infos;
} RopeChunkCounterData;
#define DECLARE_CHUNKS_DATA(name, size)                                        \
    RopeChunkInfo name##datas[size];                                           \
    RopeChunkCounterData name = {size, 0, name##datas};

static int ropeChunkCounter(size_t index, size_t length, size_t number,
                            const Col_RopeChunk *chunks,
                            Col_ClientData clientData) {
    RopeChunkCounterData *data = (RopeChunkCounterData *)clientData;
    PICOTEST_ASSERT(number == 1);
    PICOTEST_ASSERT(data != NULL);
    PICOTEST_ASSERT(data->nbChunks < data->size);

    RopeChunkInfo *info = data->infos + data->nbChunks++;
    info->length = length;
    info->chunk = *chunks;
    memcpy(info->sample, chunks->data,
           chunks->byteLength < CHUNK_SAMPLE_SIZE ? chunks->byteLength
                                                  : CHUNK_SAMPLE_SIZE);
    return 0;
}

PICOTEST_CASE(testTraverseEmptyRopeIsNoop, colibriFixture) {
    PICOTEST_ASSERT(Col_TraverseRopeChunks(Col_EmptyRope(), 0, 0, 0, NULL, NULL,
                                           NULL) == -1);
}
PICOTEST_CASE(testTraverseProcMustNotBeNull, colibriFixture) {
    // TODO VALUECHECK proc
}
PICOTEST_CASE(testTraverseCharacterWordHasOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Char c = 'a';
    Col_Word rope = Col_NewCharWord(c);
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, 0, 1, 0, ropeChunkCounter,
                                           &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(data.infos[0].chunk.format == COL_UCS1);
    PICOTEST_ASSERT(data.infos[0].chunk.byteLength == 1);
    PICOTEST_ASSERT(*(Col_Char1 *)data.infos[0].sample == c);
}
PICOTEST_CASE(testTraverseSmallStringHasOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Word rope = SMALL_STRING();
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0,
                                           ropeChunkCounter, &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(data.infos[0].chunk.format == COL_UCS1);
    PICOTEST_ASSERT(data.infos[0].chunk.byteLength == SMALL_STRING_LEN);
    PICOTEST_ASSERT(rope == Col_NewRope(data.infos[0].chunk.format,
                                        data.infos[0].sample,
                                        data.infos[0].chunk.byteLength));
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
    PICOTEST_ASSERT(data.infos[0].chunk.format == COL_UCS1);
    PICOTEST_ASSERT(data.infos[0].chunk.byteLength == FLAT_STRING_LEN);
}
PICOTEST_CASE(testTraverseFlatUtf8StringHasOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Word rope = FLAT_STRING_UTF8();
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0,
                                           ropeChunkCounter, &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(data.infos[0].chunk.format == COL_UTF8);
}
PICOTEST_CASE(testTraverseFlatUtf16StringHasOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Word rope = FLAT_STRING_UTF16();
    PICOTEST_ASSERT(Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0,
                                           ropeChunkCounter, &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(data.infos[0].chunk.format == COL_UTF16);
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
    PICOTEST_ASSERT(ropeData.infos[0].length == subropeData.infos[0].length);
    PICOTEST_ASSERT(ropeData.infos[0].chunk.format ==
                    subropeData.infos[0].chunk.format);
    PICOTEST_ASSERT(ropeData.infos[0].chunk.data ==
                    subropeData.infos[0].chunk.data);
    PICOTEST_ASSERT(ropeData.infos[0].chunk.byteLength ==
                    subropeData.infos[0].chunk.byteLength);
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
    PICOTEST_ASSERT(leftData.infos[0].length == concatRopeData.infos[0].length);
    PICOTEST_ASSERT(leftData.infos[0].chunk.format ==
                    concatRopeData.infos[0].chunk.format);
    PICOTEST_ASSERT(leftData.infos[0].chunk.data ==
                    concatRopeData.infos[0].chunk.data);
    PICOTEST_ASSERT(leftData.infos[0].chunk.byteLength ==
                    concatRopeData.infos[0].chunk.byteLength);
    PICOTEST_ASSERT(rightData.infos[0].length ==
                    concatRopeData.infos[1].length);
    PICOTEST_ASSERT(rightData.infos[0].chunk.format ==
                    concatRopeData.infos[1].chunk.format);
    PICOTEST_ASSERT(rightData.infos[0].chunk.data ==
                    concatRopeData.infos[1].chunk.data);
    PICOTEST_ASSERT(rightData.infos[0].chunk.byteLength ==
                    concatRopeData.infos[1].chunk.byteLength);
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
    PICOTEST_ASSERT(leftData.infos[0].length == concatRopeData.infos[0].length);
    PICOTEST_ASSERT(leftData.infos[0].chunk.format ==
                    concatRopeData.infos[0].chunk.format);
    PICOTEST_ASSERT(leftData.infos[0].chunk.data ==
                    concatRopeData.infos[0].chunk.data);
    PICOTEST_ASSERT(leftData.infos[0].chunk.byteLength ==
                    concatRopeData.infos[0].chunk.byteLength);
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
    PICOTEST_ASSERT(rightData.infos[0].length ==
                    concatRopeData.infos[0].length);
    PICOTEST_ASSERT(rightData.infos[0].chunk.format ==
                    concatRopeData.infos[0].chunk.format);
    PICOTEST_ASSERT(rightData.infos[0].chunk.data ==
                    concatRopeData.infos[0].chunk.data);
    PICOTEST_ASSERT(rightData.infos[0].chunk.byteLength ==
                    concatRopeData.infos[0].chunk.byteLength);
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
    PICOTEST_ASSERT(leftData.infos[0].length == subropeData.infos[0].length);
    PICOTEST_ASSERT(leftData.infos[0].chunk.format ==
                    subropeData.infos[0].chunk.format);
    PICOTEST_ASSERT(leftData.infos[0].chunk.data ==
                    subropeData.infos[0].chunk.data);
    PICOTEST_ASSERT(leftData.infos[0].chunk.byteLength ==
                    subropeData.infos[0].chunk.byteLength);
    PICOTEST_ASSERT(rightData.infos[0].length == subropeData.infos[1].length);
    PICOTEST_ASSERT(rightData.infos[0].chunk.format ==
                    subropeData.infos[1].chunk.format);
    PICOTEST_ASSERT(rightData.infos[0].chunk.data ==
                    subropeData.infos[1].chunk.data);
    PICOTEST_ASSERT(rightData.infos[0].chunk.byteLength ==
                    subropeData.infos[1].chunk.byteLength);
}

PICOTEST_SUITE(testTraverseRange, testTraverseSingleCharacter);
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
    PICOTEST_ASSERT(data.infos[0].chunk.format == format);
    PICOTEST_ASSERT(data.infos[0].chunk.byteLength == (size_t)format);
    PICOTEST_ASSERT(COL_CHAR_GET(format, data.infos[0].chunk.data) == c);
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
    PICOTEST_ASSERT(data.infos[0].chunk.format == COL_UTF8);
    PICOTEST_ASSERT(data.infos[0].chunk.byteLength ==
                    COL_UTF8_WIDTH(c) * sizeof(Col_Char1));
    PICOTEST_ASSERT(COL_CHAR_GET(COL_UTF8, data.infos[0].chunk.data) == c);
    PICOTEST_ASSERT(Col_Utf8Get(data.infos[0].chunk.data) == c);
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
    PICOTEST_ASSERT(data.infos[0].chunk.format == COL_UTF16);
    PICOTEST_ASSERT(data.infos[0].chunk.byteLength ==
                    COL_UTF16_WIDTH(c) * sizeof(Col_Char2));
    PICOTEST_ASSERT(COL_CHAR_GET(COL_UTF16, data.infos[0].chunk.data) == c);
    PICOTEST_ASSERT(Col_Utf16Get(data.infos[0].chunk.data) == c);
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
PICOTEST_CASE(testTraverseBreak, colibriFixture) {
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

PICOTEST_SUITE(testRopeIteration, testRopeIteratorInitialize,
               testRopeIteratorCompare, testRopeIteratorAccess,
               testRopeIteratorMove, testRopeIteratorEmptyRope,
               testRopeIteratorString);

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
    PICOTEST_ASSERT(Col_RopeIterAt(it) == COL_CHAR_INVALID);
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

PICOTEST_SUITE(testRopeIteratorEmptyRope, testRopeIteratorEmptyRopeBegin,
               testRopeIteratorEmptyRopeFirst, testRopeIteratorEmptyRopeLast);
static void checkRopeIterEmpty(Col_RopeIterator it) {
    PICOTEST_ASSERT(!Col_RopeIterNull(it));
    PICOTEST_ASSERT(Col_RopeIterRope(it) == Col_EmptyRope());
    PICOTEST_ASSERT(Col_RopeIterLength(it) == 0);
    PICOTEST_ASSERT(Col_RopeIterIndex(it) == 0);
    PICOTEST_ASSERT(Col_RopeIterEnd(it));
}
PICOTEST_CASE(testRopeIteratorEmptyRopeBegin, colibriFixture) {
    Col_Word empty = Col_EmptyRope();
    Col_RopeIterator it;

    Col_RopeIterBegin(it, empty, 0);
    checkRopeIterEmpty(it);
}
PICOTEST_CASE(testRopeIteratorEmptyRopeFirst, colibriFixture) {
    Col_Word empty = Col_EmptyRope();
    Col_RopeIterator it;

    Col_RopeIterFirst(it, empty);
    checkRopeIterEmpty(it);
}
PICOTEST_CASE(testRopeIteratorEmptyRopeLast, colibriFixture) {
    Col_Word empty = Col_EmptyRope();
    Col_RopeIterator it;

    Col_RopeIterLast(it, empty);
    checkRopeIterEmpty(it);
}

PICOTEST_SUITE(testRopeIteratorString, testRopeIterString,
               testRopeIterStringAt);
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
