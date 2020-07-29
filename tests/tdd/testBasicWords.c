#include <colibri.h>
#include <picotest.h>

#include <math.h> // For NaN

/*
 * Test failure cases (must be defined before test hooks)
 */

#include "failureFixture.h"
PICOTEST_CASE(doBooleanTypeCheck, failureFixture) {
    Col_BoolWordValue(WORD_NIL);
}
PICOTEST_CASE(doIntegerTypeCheck, failureFixture) {
    Col_IntWordValue(WORD_NIL);
}
PICOTEST_CASE(doFloatingPointTypeCheck, failureFixture) {
    Col_FloatWordValue(WORD_NIL);
}

/*
 * Basic words
 */

#include "hooks.h"
#include "colibriFixture.h"

PICOTEST_SUITE(testBasicWords, testNilWord, testBooleanWords, testIntegerWords,
               testFloatingPointWords);

/* Nil */
PICOTEST_CASE(testNilWord, colibriFixture) {
    Col_Word w = WORD_NIL;
    PICOTEST_ASSERT(Col_WordType(w) == COL_NIL);
}

/* Booleans */
PICOTEST_SUITE(testBooleanWords, testBooleanTypeCheck, testTrueWord,
               testFalseWord, testNewBoolWord, testBooleanWordsAreImmediate);

PICOTEST_CASE(testBooleanTypeCheck, colibriFixture) {
    PICOTEST_ASSERT(doBooleanTypeCheck(NULL) == 1);
}

PICOTEST_CASE(testTrueWord, colibriFixture) {
    Col_Word true = WORD_TRUE;
    PICOTEST_ASSERT(Col_WordType(true) == COL_BOOL);
    PICOTEST_ASSERT(Col_BoolWordValue(true));
}
PICOTEST_CASE(testFalseWord, colibriFixture) {
    Col_Word false = WORD_FALSE;
    PICOTEST_ASSERT(Col_WordType(false) == COL_BOOL);
    PICOTEST_ASSERT(!Col_BoolWordValue(false));
}
PICOTEST_CASE(testNewBoolWord, colibriFixture) {
    int FALSE = 0, TRUE = 1, NONFALSE = 0xffff;

    Col_Word false = Col_NewBoolWord(FALSE);
    PICOTEST_ASSERT(Col_WordType(false) == COL_BOOL);
    PICOTEST_ASSERT(!Col_BoolWordValue(false));

    Col_Word true = Col_NewBoolWord(TRUE);
    PICOTEST_ASSERT(Col_WordType(true) == COL_BOOL);
    PICOTEST_ASSERT(Col_BoolWordValue(true));

    Col_Word nonfalse = Col_NewBoolWord(NONFALSE);
    PICOTEST_ASSERT(Col_WordType(nonfalse) == COL_BOOL);
    PICOTEST_ASSERT(Col_BoolWordValue(nonfalse));
}
PICOTEST_CASE(testBooleanWordsAreImmediate, colibriFixture) {
    int FALSE = 0, TRUE = 1, NONFALSE = 0xffff;
    PICOTEST_ASSERT(Col_NewBoolWord(FALSE) == Col_NewBoolWord(FALSE));
    PICOTEST_ASSERT(Col_NewBoolWord(TRUE) == Col_NewBoolWord(TRUE));
    PICOTEST_ASSERT(Col_NewBoolWord(NONFALSE) == Col_NewBoolWord(NONFALSE));
    PICOTEST_ASSERT(Col_NewBoolWord(NONFALSE) == Col_NewBoolWord(TRUE));
}

/* Integers */
PICOTEST_SUITE(testIntegerWords, testIntegerTypeCheck, testNewIntWord);

PICOTEST_CASE(testIntegerTypeCheck, colibriFixture) {
    PICOTEST_ASSERT(doIntegerTypeCheck(NULL) == 1);
}

PICOTEST_SUITE(testNewIntWord, testNewIntWordImmediate, testNewIntWordRegular);

PICOTEST_CASE(testNewIntWordImmediate, colibriFixture) {
    intptr_t ZERO = 0, MIN = INTPTR_MIN >> 1, MAX = INTPTR_MAX >> 1;

    Col_Word zero = Col_NewIntWord(ZERO);
    PICOTEST_ASSERT(zero == Col_NewIntWord(ZERO));
    PICOTEST_ASSERT(Col_WordType(zero) == COL_INT);
    PICOTEST_ASSERT(Col_IntWordValue(zero) == ZERO);

    Col_Word min = Col_NewIntWord(MIN);
    PICOTEST_ASSERT(min = Col_NewIntWord(MIN));
    PICOTEST_ASSERT(Col_WordType(min) == COL_INT);
    PICOTEST_ASSERT(Col_IntWordValue(min) == MIN);

    Col_Word max = Col_NewIntWord(MAX);
    PICOTEST_ASSERT(max == Col_NewIntWord(MAX));
    PICOTEST_ASSERT(Col_WordType(max) == COL_INT);
    PICOTEST_ASSERT(Col_IntWordValue(max) == MAX);
}
PICOTEST_CASE(testNewIntWordRegular, colibriFixture) {
    intptr_t MIN = INTPTR_MIN >> 1, MAX = INTPTR_MAX >> 1;

    Col_Word min1 = Col_NewIntWord(MIN - 1);
    PICOTEST_ASSERT(min1 != Col_NewIntWord(MIN - 1));
    PICOTEST_ASSERT(Col_WordType(min1) == COL_INT);
    PICOTEST_ASSERT(Col_IntWordValue(min1) == MIN - 1);

    Col_Word max1 = Col_NewIntWord(MAX + 1);
    PICOTEST_ASSERT(max1 != Col_NewIntWord(MAX + 1));
    PICOTEST_ASSERT(Col_WordType(max1) == COL_INT);
    PICOTEST_ASSERT(Col_IntWordValue(max1) == MAX + 1);
}

/* Floating points */
PICOTEST_SUITE(testFloatingPointWords, testFloatingPointTypeCheck,
               testNewFloatWord);

PICOTEST_CASE(testFloatingPointTypeCheck, colibriFixture) {
    PICOTEST_ASSERT(doFloatingPointTypeCheck(NULL) == 1);
}

PICOTEST_SUITE(testNewFloatWord, testNewFloatWordImmediate,
               testNewFloatWordRegular, testNewFloatWordSpecialValues);

PICOTEST_CASE(testNewFloatWordImmediate, colibriFixture) {
    double ZERO = 0.0, MIN = INTPTR_MIN >> 1, MAX = INTPTR_MAX >> 1;

    Col_Word zero = Col_NewFloatWord(ZERO);
    PICOTEST_ASSERT(zero == Col_NewFloatWord(ZERO));
    PICOTEST_ASSERT(Col_WordType(zero) == COL_FLOAT);
    PICOTEST_ASSERT(Col_FloatWordValue(zero) == ZERO);

    Col_Word min = Col_NewFloatWord(MIN);
    PICOTEST_ASSERT(min == Col_NewFloatWord(MIN));
    PICOTEST_ASSERT(Col_WordType(min) == COL_FLOAT);
    PICOTEST_ASSERT(Col_FloatWordValue(min) == MIN);

    Col_Word max = Col_NewFloatWord(MAX);
    PICOTEST_ASSERT(max == Col_NewFloatWord(MAX));
    PICOTEST_ASSERT(Col_WordType(max) == COL_FLOAT);
    PICOTEST_ASSERT(Col_FloatWordValue(max) == MAX);
}
PICOTEST_CASE(testNewFloatWordRegular, colibriFixture) {
    double F = 1.7 / 4.3;

    Col_Word f = Col_NewFloatWord(F);
    PICOTEST_ASSERT(f != Col_NewFloatWord(F));
    PICOTEST_ASSERT(Col_WordType(f) == COL_FLOAT);
    PICOTEST_ASSERT(Col_FloatWordValue(f) == F);
}
PICOTEST_CASE(testNewFloatWordSpecialValues, colibriFixture) {
    Col_Word nan = Col_NewFloatWord(NAN);
    PICOTEST_ASSERT(nan == Col_NewFloatWord(NAN));
    PICOTEST_ASSERT(Col_WordType(nan) == COL_FLOAT);
    PICOTEST_ASSERT(isnan(Col_FloatWordValue(nan)));

    Col_Word infp = Col_NewFloatWord(INFINITY);
    PICOTEST_ASSERT(infp == Col_NewFloatWord(INFINITY));
    PICOTEST_ASSERT(Col_WordType(infp) == COL_FLOAT);
    PICOTEST_ASSERT(Col_FloatWordValue(infp) == INFINITY);

    Col_Word infn = Col_NewFloatWord(-INFINITY);
    PICOTEST_ASSERT(infn == Col_NewFloatWord(-INFINITY));
    PICOTEST_ASSERT(Col_WordType(infn) == COL_FLOAT);
    PICOTEST_ASSERT(Col_FloatWordValue(infn) == -INFINITY);
}
