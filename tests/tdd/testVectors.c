#include <colibri.h>
#include <picotest.h>

/*
 * Failure test cases (must be defined before test hooks)
 */

#include "failureFixture.h"

/* Col_NewVector */
PICOTEST_CASE(newVector_valueCheck_length, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_VECTORLENGTH);
    PICOTEST_ASSERT(Col_NewVector(Col_MaxVectorLength() + 1, NULL) == WORD_NIL);
}
PICOTEST_CASE(newVector_fatal_outOfMemory, failureFixture, context) {
    EXPECT_FAILURE(context, COL_FATAL, Col_GetErrorDomain(), COL_ERROR_MEMORY);
    Col_NewVector(Col_MaxVectorLength(), NULL);
    PICOTEST_ASSERT(!"UNREACHABLE");
}

/* Col_VectorLength */
PICOTEST_CASE(vectorLength_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_VECTOR);
    PICOTEST_ASSERT(Col_VectorLength(WORD_NIL) == 0);
}

/* Col_VectorElements */
PICOTEST_CASE(vectorElements_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_VECTOR);
    PICOTEST_ASSERT(Col_VectorElements(WORD_NIL) == NULL);
}

/*
 * Immutable vectors
 */

#include "hooks.h"
#include "colibriFixture.h"

PICOTEST_SUITE(testVectors, testVectorTypeChecks, testNewVector, testNewVectorV,
               testEmptyVector);

PICOTEST_CASE(testVectorTypeChecks, colibriFixture) {
    PICOTEST_VERIFY(vectorLength_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(vectorElements_typeCheck(NULL) == 1);
}
PICOTEST_SUITE(testNewVector, testNewVectorErrors, testNewVectorNil,
               testNewVectorValues);

PICOTEST_SUITE(testNewVectorErrors, testNewVectorTooLarge,
               testNewVectorOutOfMemory);
PICOTEST_CASE(testNewVectorTooLarge, colibriFixture) {
    PICOTEST_ASSERT(newVector_valueCheck_length(NULL) == 1);
}
PICOTEST_CASE(testNewVectorOutOfMemory, colibriFixture) {
    PICOTEST_ASSERT(newVector_fatal_outOfMemory(NULL) == 1);
}

static void checkVector(Col_Word vector, size_t len, const Col_Word *words) {
    PICOTEST_ASSERT(Col_WordType(vector) == (COL_VECTOR | COL_LIST))
    PICOTEST_ASSERT(Col_VectorLength(vector) == len);
    const Col_Word *elements = Col_VectorElements(vector);
    if (len == 0) {
        PICOTEST_ASSERT(elements == NULL);
    } else {
        PICOTEST_ASSERT(elements != NULL);
        for (size_t i = 0; i < len; i++) {
            PICOTEST_ASSERT(elements[i] == (words ? words[i] : WORD_NIL));
        }
    }
}
PICOTEST_CASE(testNewVectorNil, colibriFixture) {
    checkVector(Col_NewVector(10, NULL), 10, NULL);
}
PICOTEST_CASE(testNewVectorValues, colibriFixture) {
    Col_Word words[] = {WORD_NIL, WORD_TRUE, WORD_FALSE, Col_NewIntWord(0),
                        Col_NewCharWord('a')};
    size_t len = sizeof(words) / sizeof(*words);
    Col_Word vector = Col_NewVector(len, words);
    checkVector(vector, len, words);
}
PICOTEST_CASE(testNewVectorV, colibriFixture) {
    Col_Word words[] = {WORD_NIL, WORD_TRUE, WORD_FALSE, Col_NewIntWord(0),
                        Col_NewCharWord('a')};
    size_t len = sizeof(words) / sizeof(*words);
    Col_Word vector = Col_NewVectorV(WORD_NIL, WORD_TRUE, WORD_FALSE,
                                     Col_NewIntWord(0), Col_NewCharWord('a'));
    checkVector(vector, len, words);
}
PICOTEST_CASE(testEmptyVector, colibriFixture) {
    checkVector(Col_NewVector(0, NULL), 0, NULL);
    checkVector(Col_NewVectorNV(0), 0, NULL);
    PICOTEST_ASSERT(Col_NewVector(0, NULL) == Col_NewVector(0, NULL));
    PICOTEST_ASSERT(Col_NewVectorNV(0) == Col_NewVector(0, NULL));
}
