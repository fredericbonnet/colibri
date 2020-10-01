#include <colibri.h>
#include <picotest.h>

/*
 * Failure test cases (must be defined before test hooks)
 */

#include "failureFixture.h"

/* Col_NewStringBuffer */
PICOTEST_CASE(newStringBuffer_valueCheck_format_utf8, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_STRBUF_FORMAT);
    PICOTEST_ASSERT(Col_NewStringBuffer(0, COL_UTF8) == WORD_NIL);
}
PICOTEST_CASE(newStringBuffer_valueCheck_format_utf16, failureFixture,
              context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_STRBUF_FORMAT);
    PICOTEST_ASSERT(Col_NewStringBuffer(0, COL_UTF16) == WORD_NIL);
}

/* Col_StringBufferFormat */
PICOTEST_CASE(stringBufferFormat_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_STRBUF);
    PICOTEST_ASSERT(Col_StringBufferFormat(WORD_NIL) == 0);
}

/* Col_StringBufferCapacity */
PICOTEST_CASE(stringBufferCapacity_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_STRBUF);
    PICOTEST_ASSERT(Col_StringBufferCapacity(WORD_NIL) == 0);
}

/* Col_StringBufferLength */
PICOTEST_CASE(stringBufferLength_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_STRBUF);
    PICOTEST_ASSERT(Col_StringBufferLength(WORD_NIL) == 0);
}

/* Col_StringBufferValue */
PICOTEST_CASE(stringBufferValue_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_STRBUF);
    PICOTEST_ASSERT(Col_StringBufferValue(WORD_NIL) == WORD_NIL);
}

/* Col_StringBufferAppendChar */
PICOTEST_CASE(stringBufferAppendChar_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_STRBUF);
    PICOTEST_ASSERT(Col_StringBufferAppendChar(WORD_NIL, 'a') == 0);
}

/* Col_StringBufferAppendRope */
PICOTEST_CASE(stringBufferAppendRope_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_STRBUF);
    PICOTEST_ASSERT(Col_StringBufferAppendRope(WORD_NIL, 'a') == 0);
}

/* Col_StringBufferAppendSequence */
PICOTEST_CASE(stringBufferAppendSequence_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_STRBUF);
    Col_Word rope = Col_NewCharWord('a');
    Col_RopeIterator first, last;
    Col_RopeIterFirst(first, rope);
    Col_RopeIterLast(last, rope);
    PICOTEST_ASSERT(Col_StringBufferAppendSequence(WORD_NIL, first, last) == 0);
}

/* Col_StringBufferReserve */
PICOTEST_CASE(stringBufferReserve_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_STRBUF);
    PICOTEST_ASSERT(Col_StringBufferReserve(WORD_NIL, 1) == NULL);
}

/* Col_StringBufferRelease */
PICOTEST_CASE(stringBufferRelease_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_STRBUF);
    Col_StringBufferRelease(WORD_NIL, 1);
}

/* Col_StringBufferReset */
PICOTEST_CASE(stringBufferReset_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_STRBUF);
    Col_StringBufferReset(WORD_NIL);
}

/* Col_StringBufferFreeze */
PICOTEST_CASE(stringBufferFreeze_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_STRBUF);
    PICOTEST_ASSERT(Col_StringBufferFreeze(WORD_NIL) == WORD_NIL);
}

/*
 * Test data
 */

#include "ropeUtils.h"

#define STRING_LEN 200

#define STRING_DATA_UCS1(data)                                                 \
    DECLARE_ROPE_DATA_UCS(data, Col_Char1, STRING_LEN, 'a', 'a' + i)
#define STRING_DATA_UCS2(data)                                                 \
    DECLARE_ROPE_DATA_UCS(data, Col_Char2, STRING_LEN, 'a', 'a' + i)
#define STRING_DATA_UCS4(data)                                                 \
    DECLARE_ROPE_DATA_UCS(data, Col_Char4, STRING_LEN, COL_CHAR2_MAX,          \
                          COL_CHAR2_MAX + i)

static Col_Word STRING_UCS1()
    NEW_ROPE_UCS(Col_Char1, COL_UCS1, STRING_LEN, 'a', 'a' + i);
static Col_Word STRING_UCS2()
    NEW_ROPE_UCS(Col_Char2, COL_UCS2, STRING_LEN, 'a', 'a' + i);
static Col_Word STRING_UCS4() NEW_ROPE_UCS(Col_Char4, COL_UCS4, STRING_LEN,
                                           COL_CHAR2_MAX, COL_CHAR2_MAX + i);

/*
 * String buffers
 */

#include "hooks.h"
#include "colibriFixture.h"

PICOTEST_SUITE(testStringBuffers, testStringBufferTypeChecks,
               testStringBufferCreation, testStringBufferAccessors,
               testStringBufferOperations);

PICOTEST_CASE(testStringBufferTypeChecks, colibriFixture) {
    PICOTEST_VERIFY(newStringBuffer_valueCheck_format_utf8(NULL) == 1);
    PICOTEST_VERIFY(newStringBuffer_valueCheck_format_utf16(NULL) == 1);
    PICOTEST_VERIFY(stringBufferFormat_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(stringBufferCapacity_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(stringBufferLength_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(stringBufferValue_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(stringBufferAppendChar_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(stringBufferAppendRope_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(stringBufferAppendSequence_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(stringBufferReserve_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(stringBufferRelease_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(stringBufferReset_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(stringBufferFreeze_typeCheck(NULL) == 1);
}

PICOTEST_SUITE(testStringBufferCreation, testNewStringBuffer);
static void checkNewStringBuffer(Col_StringFormat format) {
    Col_Word strbuf = Col_NewStringBuffer(0, format);
    PICOTEST_ASSERT(Col_StringBufferCapacity(strbuf) > 0);
}
PICOTEST_CASE(testNewStringBuffer, colibriFixture) {
    checkNewStringBuffer(COL_UCS1);
    checkNewStringBuffer(COL_UCS2);
    checkNewStringBuffer(COL_UCS4);
    checkNewStringBuffer(COL_UCS);
}

PICOTEST_SUITE(testStringBufferAccessors, testStringBufferFormat,
               testStringBufferCapacity, testStringBufferLength,
               testStringBufferValue);
static void checkStringBufferFormat(Col_StringFormat format) {
    Col_Word strbuf = Col_NewStringBuffer(0, format);
    PICOTEST_ASSERT(Col_StringBufferFormat(strbuf) == format);
}
PICOTEST_CASE(testStringBufferFormat, colibriFixture) {
    checkStringBufferFormat(COL_UCS1);
    checkStringBufferFormat(COL_UCS2);
    checkStringBufferFormat(COL_UCS4);
    checkStringBufferFormat(COL_UCS);
}
PICOTEST_SUITE(testStringBufferCapacity,
               testStringBufferCapacityShouldBeNonZero,
               testStringBufferCapacityShouldAdjustToLength,
               testStringBufferCapacityShouldNotExceedMaxLength);
PICOTEST_CASE(testStringBufferCapacityShouldBeNonZero, colibriFixture) {
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS1);
    PICOTEST_ASSERT(Col_StringBufferCapacity(strbuf) > 0);
}
PICOTEST_CASE(testStringBufferCapacityShouldAdjustToLength, colibriFixture) {
    Col_Word strbuf = Col_NewStringBuffer(10, COL_UCS1);
    PICOTEST_ASSERT(Col_StringBufferCapacity(strbuf) >= 10);
}

static void
checkStringBufferCapacityShouldNotExceedMaxLength(Col_StringFormat format) {
    Col_Word strbuf = Col_NewStringBuffer(SIZE_MAX, format);
    PICOTEST_ASSERT(Col_StringBufferCapacity(strbuf) ==
                    Col_MaxStringBufferLength(format));
}
PICOTEST_CASE(testStringBufferCapacityShouldNotExceedMaxLength,
              colibriFixture) {
    checkStringBufferCapacityShouldNotExceedMaxLength(COL_UCS1);
    checkStringBufferCapacityShouldNotExceedMaxLength(COL_UCS2);
    checkStringBufferCapacityShouldNotExceedMaxLength(COL_UCS4);
    checkStringBufferCapacityShouldNotExceedMaxLength(COL_UCS);
}

PICOTEST_SUITE(testStringBufferLength, testStringBufferLengthEmpty);
PICOTEST_CASE(testStringBufferLengthEmpty, colibriFixture) {
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS1);
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == 0);
}

PICOTEST_SUITE(testStringBufferValue, testStringBufferValueEmpty);
PICOTEST_CASE(testStringBufferValueEmpty, colibriFixture) {
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS1);
    PICOTEST_ASSERT(Col_StringBufferValue(strbuf) == Col_EmptyRope());
}

PICOTEST_SUITE(testStringBufferOperations, testStringBufferAppendChar,
               testStringBufferAppendRope, testStringBufferAppendSequence,
               testStringBufferReserve, testStringBufferRelease,
               testStringBufferReset, testStringBufferFreeze);

PICOTEST_SUITE(testStringBufferAppendChar, testStringBufferAppendCharNoLoss,
               testStringBufferAppendCharDiscard,
               testStringBufferAppendCharWithinCapacity,
               testStringBufferAppendCharAboveCapacity);
static void checkStringBufferAppendChar(Col_StringFormat format, Col_Char c,
                                        Col_StringFormat expectedFormat) {
    Col_Word strbuf = Col_NewStringBuffer(0, format);
    PICOTEST_ASSERT(Col_StringBufferAppendChar(strbuf, c));
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == 1);
    Col_Word value = Col_StringBufferValue(strbuf);
    Col_Word expected = Col_NewCharWord(c);
    PICOTEST_ASSERT(Col_CompareRopes(value, expected) == 0);
    PICOTEST_ASSERT(Col_WordType(value) & COL_STRING);
    PICOTEST_ASSERT(Col_StringWordFormat(value) == expectedFormat);
}
PICOTEST_CASE(testStringBufferAppendCharNoLoss, colibriFixture) {
    checkStringBufferAppendChar(COL_UCS1, COL_CHAR1_MAX, COL_UCS1);
    checkStringBufferAppendChar(COL_UCS2, COL_CHAR2_MAX, COL_UCS2);
    checkStringBufferAppendChar(COL_UCS4, COL_CHAR_MAX, COL_UCS4);
    checkStringBufferAppendChar(COL_UCS, COL_CHAR_MAX, COL_UCS4);
}

static void checkStringBufferAppendCharDiscard(Col_StringFormat format,
                                               Col_Char c) {
    Col_Word strbuf = Col_NewStringBuffer(0, format);
    PICOTEST_ASSERT(!Col_StringBufferAppendChar(strbuf, c));
}
PICOTEST_CASE(testStringBufferAppendCharDiscard, colibriFixture) {
    checkStringBufferAppendCharDiscard(COL_UCS1, COL_CHAR1_MAX + 1);
    checkStringBufferAppendCharDiscard(COL_UCS2, COL_CHAR2_MAX + 1);
}

static void checkStringBufferAppendCharWithinCapacity(Col_Word rope,
                                                      Col_StringFormat format) {
    Col_Word strbuf = Col_NewStringBuffer(10, format);
    size_t capacity = Col_StringBufferCapacity(strbuf);
    PICOTEST_ASSERT(capacity <= STRING_LEN);
    for (size_t i = 0; i < capacity; i++) {
        Col_Char c = Col_RopeAt(rope, i);
        PICOTEST_ASSERT(Col_StringBufferAppendChar(strbuf, c));
    }
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == capacity);
    Col_Word value = Col_StringBufferValue(strbuf);
    PICOTEST_ASSERT(Col_CompareRopesN(value, rope, capacity) == 0);
    PICOTEST_ASSERT(Col_WordType(value) & COL_STRING);
    PICOTEST_ASSERT(Col_StringWordFormat(value) == format);
}
PICOTEST_CASE(testStringBufferAppendCharWithinCapacity, colibriFixture) {
    checkStringBufferAppendCharWithinCapacity(STRING_UCS1(), COL_UCS1);
    checkStringBufferAppendCharWithinCapacity(STRING_UCS2(), COL_UCS2);
    checkStringBufferAppendCharWithinCapacity(STRING_UCS4(), COL_UCS4);
}

static void checkStringBufferAppendCharAboveCapacity(Col_Word rope,
                                                     Col_StringFormat format) {
    Col_Word strbuf = Col_NewStringBuffer(0, format);
    size_t capacity = Col_StringBufferCapacity(strbuf);
    PICOTEST_ASSERT(capacity + 1 <= Col_RopeLength(rope));
    for (size_t i = 0; i < capacity + 1; i++) {
        Col_Char c = Col_RopeAt(rope, i);
        PICOTEST_ASSERT(Col_StringBufferAppendChar(strbuf, c));
    }
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == capacity + 1);
    Col_Word value = Col_StringBufferValue(strbuf);
    PICOTEST_ASSERT(Col_CompareRopesN(value, rope, capacity + 1) == 0);
    PICOTEST_ASSERT(Col_WordType(value) == COL_ROPE);
}
PICOTEST_CASE(testStringBufferAppendCharAboveCapacity, colibriFixture) {
    checkStringBufferAppendCharAboveCapacity(STRING_UCS1(), COL_UCS1);
    checkStringBufferAppendCharAboveCapacity(STRING_UCS2(), COL_UCS2);
    checkStringBufferAppendCharAboveCapacity(STRING_UCS4(), COL_UCS4);
}

PICOTEST_SUITE(testStringBufferAppendRope, testStringBufferAppendEmptyRope,
               testStringBufferAppendRopeToEmptyBuffer,
               testStringBufferAppendRopeToNonEmptyBuffer,
               testStringBufferAppendRopeLoss,
               testStringBufferAppendRopeConcat);
PICOTEST_CASE(testStringBufferAppendEmptyRope, colibriFixture) {
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS1);
    PICOTEST_ASSERT(Col_StringBufferAppendRope(strbuf, Col_EmptyRope()));
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == 0);
}

static void checkStringBufferAppendRopeToEmptyBuffer(Col_Word rope,
                                                     Col_StringFormat format) {
    Col_Word strbuf = Col_NewStringBuffer(0, format);
    PICOTEST_ASSERT(Col_StringBufferAppendRope(strbuf, rope));
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == Col_RopeLength(rope));
    PICOTEST_ASSERT(Col_StringBufferValue(strbuf) == rope);
}
PICOTEST_CASE(testStringBufferAppendRopeToEmptyBuffer, colibriFixture) {
    checkStringBufferAppendRopeToEmptyBuffer(Col_NewCharWord('a'), COL_UCS1);
    checkStringBufferAppendRopeToEmptyBuffer(STRING_UCS1(), COL_UCS1);
    checkStringBufferAppendRopeToEmptyBuffer(STRING_UCS2(), COL_UCS2);
    checkStringBufferAppendRopeToEmptyBuffer(STRING_UCS4(), COL_UCS4);
    checkStringBufferAppendRopeToEmptyBuffer(STRING_UCS1(), COL_UCS);
}

static void
checkStringBufferAppendRopeToNonEmptyBuffer(Col_Word rope,
                                            Col_StringFormat format) {
    Col_Word expected = Col_ConcatRopes(Col_NewCharWord('a'), rope);
    Col_Word strbuf = Col_NewStringBuffer(0, format);
    PICOTEST_ASSERT(Col_StringBufferAppendChar(strbuf, 'a'));
    PICOTEST_ASSERT(Col_StringBufferAppendRope(strbuf, rope));
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == Col_RopeLength(expected));
    Col_Word value = Col_StringBufferValue(strbuf);
    PICOTEST_ASSERT(Col_CompareRopes(value, expected) == 0);
}
PICOTEST_CASE(testStringBufferAppendRopeToNonEmptyBuffer, colibriFixture) {
    checkStringBufferAppendRopeToNonEmptyBuffer(Col_NewCharWord('a'), COL_UCS1);
    checkStringBufferAppendRopeToNonEmptyBuffer(STRING_UCS1(), COL_UCS1);
    checkStringBufferAppendRopeToNonEmptyBuffer(STRING_UCS2(), COL_UCS2);
    checkStringBufferAppendRopeToNonEmptyBuffer(STRING_UCS4(), COL_UCS4);
    checkStringBufferAppendRopeToNonEmptyBuffer(STRING_UCS1(), COL_UCS);
}

static void checkStringBufferAppendRopeLoss(Col_Word rope,
                                            Col_StringFormat format) {
    Col_Word strbuf = Col_NewStringBuffer(0, format);
    PICOTEST_ASSERT(!Col_StringBufferAppendRope(strbuf, rope));
    Col_Word normalized = Col_NormalizeRope(rope, format, COL_CHAR_INVALID, 0);
    PICOTEST_ASSERT(Col_RopeLength(normalized) < Col_RopeLength(rope));
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) ==
                    Col_RopeLength(normalized));
}
PICOTEST_CASE(testStringBufferAppendRopeLoss, colibriFixture) {
    checkStringBufferAppendRopeLoss(Col_NewCharWord(COL_CHAR1_MAX + 1),
                                    COL_UCS1);
    checkStringBufferAppendRopeLoss(Col_NewCharWord(COL_CHAR2_MAX + 1),
                                    COL_UCS2);
    checkStringBufferAppendRopeLoss(STRING_UCS2(), COL_UCS1);
    checkStringBufferAppendRopeLoss(STRING_UCS4(), COL_UCS2);
}

static void checkStringBufferAppendRopeConcat(Col_Word rope1, Col_Word rope2,
                                              Col_StringFormat format) {
    Col_Word expected = Col_ConcatRopes(rope1, rope2);
    Col_Word strbuf = Col_NewStringBuffer(0, format);
    PICOTEST_ASSERT(Col_StringBufferAppendRope(strbuf, rope1));
    PICOTEST_ASSERT(Col_StringBufferAppendRope(strbuf, rope2));
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == Col_RopeLength(expected));
    Col_Word value = Col_StringBufferValue(strbuf);
    PICOTEST_ASSERT(Col_RopeDepth(value) == 1);
    PICOTEST_ASSERT(Col_CompareRopes(value, expected) == 0);
}
PICOTEST_CASE(testStringBufferAppendRopeConcat, colibriFixture) {
    checkStringBufferAppendRopeConcat(STRING_UCS1(), Col_NewCharWord('a'),
                                      COL_UCS1);
    checkStringBufferAppendRopeConcat(STRING_UCS1(), STRING_UCS1(), COL_UCS1);
    checkStringBufferAppendRopeConcat(STRING_UCS1(), STRING_UCS2(), COL_UCS2);
}

PICOTEST_SUITE(testStringBufferAppendSequence,
               testStringBufferAppendEmptySequence,
               testStringBufferAppendSequenceCharacterWise,
               testStringBufferAppendSequenceSubrope,
               testStringBufferAppendSequenceString);
PICOTEST_CASE(testStringBufferAppendEmptySequence, colibriFixture) {
    Col_Word rope = STRING_UCS1();
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS1);
    Col_RopeIterator first, last;
    Col_RopeIterFirst(first, rope);
    Col_RopeIterLast(last, rope);
    PICOTEST_ASSERT(Col_StringBufferAppendSequence(strbuf, last, first));
}
PICOTEST_CASE(testStringBufferAppendSequenceCharacterWise, colibriFixture) {
    Col_Word rope = STRING_UCS1();
    size_t first = STRING_LEN / 3, last = STRING_LEN / 2;
    Col_Word expected = Col_Subrope(rope, first, last);
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS1);
    Col_RopeIterator begin, end;
    Col_RopeIterBegin(begin, rope, first);
    Col_RopeIterBegin(end, rope, last + 1);
    PICOTEST_ASSERT(Col_StringBufferAppendSequence(strbuf, begin, end));
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == Col_RopeLength(expected));
    Col_Word value = Col_StringBufferValue(strbuf);
    PICOTEST_ASSERT(Col_CompareRopes(value, expected) == 0);
}
PICOTEST_CASE(testStringBufferAppendSequenceSubrope, colibriFixture) {
    Col_Word rope = STRING_UCS4();
    size_t first = 1, last = STRING_LEN - 2;
    Col_Word expected = Col_Subrope(rope, first, last);
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS4);
    Col_RopeIterator begin, end;
    Col_RopeIterBegin(begin, rope, first);
    Col_RopeIterBegin(end, rope, last + 1);
    PICOTEST_ASSERT(Col_StringBufferAppendSequence(strbuf, begin, end));
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == Col_RopeLength(expected));
    Col_Word value = Col_StringBufferValue(strbuf);
    PICOTEST_ASSERT(Col_CompareRopes(value, expected) == 0);
}
PICOTEST_CASE(testStringBufferAppendSequenceString, colibriFixture) {
    char *data = "Test string";
    Col_Word expected = Col_NewRopeFromString(data);
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS1);
    Col_RopeIterator begin, end;
    Col_RopeIterString(begin, COL_UCS1, data, strlen(data));
    Col_RopeIterSet(end, begin);
    Col_RopeIterForward(end, SIZE_MAX);
    PICOTEST_ASSERT(Col_StringBufferAppendSequence(strbuf, begin, end));
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == Col_RopeLength(expected));
    Col_Word value = Col_StringBufferValue(strbuf);
    PICOTEST_ASSERT(Col_CompareRopes(value, expected) == 0);
}

PICOTEST_SUITE(testStringBufferReserve, testStringBufferReserveZeroIsNoop,
               testStringBufferReserveCapacity,
               testStringBufferReserveCannotExceedCapacity,
               testStringBufferReserveAndFill,
               testStringBufferReserveConsecutive,
               testStringBufferReserveSuccessive);
PICOTEST_CASE(testStringBufferReserveZeroIsNoop, colibriFixture) {
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS1);
    size_t capacity = Col_StringBufferCapacity(strbuf);
    PICOTEST_ASSERT(Col_StringBufferReserve(strbuf, 0) == NULL);
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == 0);
}
PICOTEST_CASE(testStringBufferReserveCapacity, colibriFixture) {
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS1);
    size_t capacity = Col_StringBufferCapacity(strbuf);
    PICOTEST_ASSERT(Col_StringBufferReserve(strbuf, capacity) != NULL);
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == capacity);
}
PICOTEST_CASE(testStringBufferReserveCannotExceedCapacity, colibriFixture) {
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS1);
    size_t capacity = Col_StringBufferCapacity(strbuf);
    PICOTEST_ASSERT(Col_StringBufferReserve(strbuf, capacity + 1) == NULL);
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == 0);
}
PICOTEST_CASE(testStringBufferReserveAndFill, colibriFixture) {
    STRING_DATA_UCS1(data);
    Col_Word expected = STRING_UCS1();
    Col_Word strbuf = Col_NewStringBuffer(STRING_LEN, COL_UCS1);
    size_t capacity = Col_StringBufferCapacity(strbuf);
    PICOTEST_ASSERT(capacity >= STRING_LEN);
    Col_Char1 *buf = Col_StringBufferReserve(strbuf, STRING_LEN);
    PICOTEST_ASSERT(buf != NULL);
    memcpy(buf, data, sizeof(data));
    Col_Word value = Col_StringBufferValue(strbuf);
    PICOTEST_ASSERT(Col_CompareRopes(value, expected) == 0);
}
PICOTEST_CASE(testStringBufferReserveConsecutive, colibriFixture) {
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS4);

    Col_Char *buf1 = Col_StringBufferReserve(strbuf, 1);
    PICOTEST_ASSERT(buf1 != NULL);
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == 1);

    Col_Char *buf2 = Col_StringBufferReserve(strbuf, 3);
    PICOTEST_ASSERT(buf2 != NULL);
    PICOTEST_ASSERT(buf2 - buf1 == 1);
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == 4);

    Col_Char *buf3 = Col_StringBufferReserve(strbuf, 12);
    PICOTEST_ASSERT(buf3 != NULL);
    PICOTEST_ASSERT(buf3 - buf2 == 3);
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == 16);

    Col_Char *buf4 = Col_StringBufferReserve(strbuf, 2);
    PICOTEST_ASSERT(buf4 != NULL);
    PICOTEST_ASSERT(buf4 - buf3 == 12);
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == 18);
}
PICOTEST_CASE(testStringBufferReserveSuccessive, colibriFixture) {
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS1);
    size_t capacity = Col_StringBufferCapacity(strbuf);

    Col_Char *buf1 = Col_StringBufferReserve(strbuf, capacity);
    PICOTEST_ASSERT(buf1 != NULL);
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == capacity);

    Col_Char *buf2 = Col_StringBufferReserve(strbuf, capacity);
    PICOTEST_ASSERT(buf2 != NULL);
    PICOTEST_ASSERT(buf2 == buf1);
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == capacity * 2);
}

PICOTEST_CASE(testStringBufferRelease, colibriFixture) {
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS4);

    Col_Char *buf1 = Col_StringBufferReserve(strbuf, 10);
    PICOTEST_ASSERT(buf1 != NULL);
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == 10);

    Col_StringBufferRelease(strbuf, 3);
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == 7);

    Col_Char *buf2 = Col_StringBufferReserve(strbuf, 5);
    PICOTEST_ASSERT(buf2 != NULL);
    PICOTEST_ASSERT(buf2 - buf1 == 7);
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == 12);

    Col_StringBufferRelease(strbuf, SIZE_MAX);
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == 0);
}
PICOTEST_CASE(testStringBufferReset, colibriFixture) {
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS1);

    Col_StringBufferAppendRope(strbuf, STRING_UCS1());
    Col_StringBufferAppendRope(strbuf, STRING_UCS1());
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == STRING_LEN * 2);
    Col_Word value = Col_StringBufferValue(strbuf);
    PICOTEST_ASSERT(Col_CompareRopes(value, Col_RepeatRope(STRING_UCS1(), 2)) ==
                    0);

    Col_StringBufferReset(strbuf);

    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == 0);
    Col_StringBufferAppendRope(strbuf, STRING_UCS1());
    PICOTEST_ASSERT(Col_StringBufferLength(strbuf) == STRING_LEN);
    value = Col_StringBufferValue(strbuf);
    PICOTEST_ASSERT(Col_CompareRopes(value, STRING_UCS1()) == 0);
}

PICOTEST_SUITE(testStringBufferFreeze, testStringBufferFreezeInPlace,
               testStringBufferFreezeGetValue);

static void checkStringBufferFreezeInPlace(Col_StringFormat format,
                                           Col_Word rope,
                                           Col_StringFormat expectedFormat) {
    Col_Word strbuf = Col_NewStringBuffer(STRING_LEN, format);
    Col_RopeIterator begin, end;
    Col_RopeIterFirst(begin, rope);
    Col_RopeIterBegin(end, rope, SIZE_MAX);
    Col_StringBufferAppendSequence(strbuf, begin, end);
    Col_Word frozen = Col_StringBufferFreeze(strbuf);
    PICOTEST_ASSERT(frozen == strbuf);
    PICOTEST_ASSERT(Col_WordType(frozen) & COL_STRING);
    PICOTEST_ASSERT(Col_StringWordFormat(frozen) == expectedFormat);
    PICOTEST_ASSERT(Col_CompareRopes(frozen, rope) == 0);
}
PICOTEST_CASE(testStringBufferFreezeInPlace, colibriFixture) {
    checkStringBufferFreezeInPlace(COL_UCS1, STRING_UCS1(), COL_UCS1);
    checkStringBufferFreezeInPlace(COL_UCS2, STRING_UCS2(), COL_UCS2);
    checkStringBufferFreezeInPlace(COL_UCS4, STRING_UCS4(), COL_UCS4);
    checkStringBufferFreezeInPlace(COL_UCS, STRING_UCS1(), COL_UCS1);
    checkStringBufferFreezeInPlace(COL_UCS, STRING_UCS2(), COL_UCS2);
    checkStringBufferFreezeInPlace(COL_UCS, STRING_UCS4(), COL_UCS4);
}

PICOTEST_SUITE(testStringBufferFreezeGetValue,
               testStringBufferFreezeGetEmptyValue,
               testStringBufferFreezeGetBufferedValue,
               testStringBufferFreezeGetBigRope,
               testStringBufferFreezeGetImmediateString);
PICOTEST_CASE(testStringBufferFreezeGetEmptyValue, colibriFixture) {
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS1);
    Col_Word frozen = Col_StringBufferFreeze(strbuf);
    PICOTEST_ASSERT(frozen != strbuf);
    PICOTEST_ASSERT(frozen == Col_EmptyRope());
}
PICOTEST_CASE(testStringBufferFreezeGetBufferedValue, colibriFixture) {
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS1);
    Col_StringBufferAppendRope(strbuf, STRING_UCS1());
    Col_StringBufferAppendChar(strbuf, 'a');
    Col_Word frozen = Col_StringBufferFreeze(strbuf);
    PICOTEST_ASSERT(frozen != strbuf);
}
PICOTEST_CASE(testStringBufferFreezeGetBigRope, colibriFixture) {
    Col_Word strbuf = Col_NewStringBuffer(100000, COL_UCS4);
    Col_StringBufferReserve(strbuf, 100000);
    Col_Word frozen = Col_StringBufferFreeze(strbuf);
    PICOTEST_ASSERT(frozen != strbuf);
}

static void checkStringBufferFreezeGetImmediate(Col_StringFormat format,
                                                Col_Word rope) {
    Col_Word strbuf = Col_NewStringBuffer(0, format);
    Col_RopeIterator begin, end;
    Col_RopeIterFirst(begin, rope);
    Col_RopeIterBegin(end, rope, SIZE_MAX);
    Col_StringBufferAppendSequence(strbuf, begin, end);
    Col_Word frozen = Col_StringBufferFreeze(strbuf);
    PICOTEST_ASSERT(frozen != strbuf);
}
PICOTEST_CASE(testStringBufferFreezeGetImmediateString, colibriFixture) {
    checkStringBufferFreezeGetImmediate(COL_UCS1, Col_NewRopeFromString("abc"));
    checkStringBufferFreezeGetImmediate(COL_UCS2, Col_NewCharWord('a'));
    checkStringBufferFreezeGetImmediate(COL_UCS4, Col_NewCharWord('a'));
    checkStringBufferFreezeGetImmediate(COL_UCS, Col_NewRopeFromString("abc"));
    checkStringBufferFreezeGetImmediate(COL_UCS, Col_NewCharWord(COL_CHAR_MAX));
    checkStringBufferFreezeGetImmediate(COL_UCS,
                                        Col_NewCharWord(COL_CHAR2_MAX));
}
