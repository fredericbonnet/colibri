#include <colibri.h>
#include <picotest.h>

/*
 * Failure test cases (must be defined before test hooks)
 */

#include "failureFixture.h"

/* Col_NewMVector */
PICOTEST_CASE(newMVector_valueCheck_length, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_VECTORLENGTH);
    Col_NewMVector(Col_MaxMVectorLength() + 1, 0, NULL);
}
PICOTEST_CASE(newMVector_fatal_outOfMemory, failureFixture, context) {
    EXPECT_FAILURE(context, COL_FATAL, Col_GetErrorDomain(), COL_ERROR_MEMORY);
    Col_NewMVector(Col_MaxMVectorLength(), 0, NULL);
    PICOTEST_ASSERT(!"UNREACHABLE");
}

/* Col_MVectorCapacity */
PICOTEST_CASE(mvectorCapacity_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MVECTOR);
    PICOTEST_ASSERT(Col_MVectorCapacity(WORD_NIL) == 0);
}

/* Col_MVectorElements */
PICOTEST_CASE(mvectorElements_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MVECTOR);
    PICOTEST_ASSERT(Col_MVectorElements(WORD_NIL) == NULL);
}

/* Col_MVectorSetLength */
PICOTEST_CASE(mvectorSetLength_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MVECTOR);
    Col_MVectorSetLength(WORD_NIL, 1);
}
PICOTEST_CASE(mvectorSetLength_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_VECTORLENGTH);
    Col_Word mvector = Col_NewMVector(0, 0, NULL);
    Col_MVectorSetLength(mvector, Col_MVectorCapacity(mvector) + 1);
    PICOTEST_ASSERT(Col_VectorLength(mvector) == 0);
}

/* Col_MVectorFreeze */
PICOTEST_CASE(mvectorFreeze_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_VECTOR);
    Col_MVectorFreeze(WORD_NIL);
}

/*
 * Mutable vectors
 */

#include "hooks.h"
#include "colibriFixture.h"

PICOTEST_SUITE(testMutableVectors, testMutableVectorTypeChecks, testNewMVector,
               testMVectorCapacity, testMVectorSetLength, testMVectorFreeze);

PICOTEST_CASE(testMutableVectorTypeChecks, colibriFixture) {
    PICOTEST_VERIFY(mvectorCapacity_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mvectorElements_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mvectorSetLength_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mvectorFreeze_typeCheck(NULL) == 1);
}
PICOTEST_SUITE(testNewMVector, testNewMVectorErrors, testNewMVectorNil,
               testNewMVectorValues);

PICOTEST_SUITE(testNewMVectorErrors, testNewMVectorTooLarge,
               testNewMVectorOutOfMemory);
PICOTEST_CASE(testNewMVectorTooLarge, colibriFixture) {
    PICOTEST_ASSERT(newMVector_valueCheck_length(NULL) == 1);
}
PICOTEST_CASE(testNewMVectorOutOfMemory, colibriFixture) {
    PICOTEST_ASSERT(newMVector_fatal_outOfMemory(NULL) == 1);
}

static void checkElements(size_t len, const Col_Word *elements1,
                          const Col_Word *elements2) {
    for (size_t i = 0; i < len; i++) {
        PICOTEST_ASSERT(elements1[i] == (elements2 ? elements2[i] : WORD_NIL));
    }
}
static void checkMVector(Col_Word mvector, size_t capacity, size_t len,
                         const Col_Word *words) {
    PICOTEST_ASSERT(Col_WordType(mvector) ==
                    (COL_MVECTOR | COL_VECTOR | COL_LIST))
    PICOTEST_ASSERT(Col_VectorLength(mvector) == len);
    PICOTEST_ASSERT(Col_MVectorCapacity(mvector) >= capacity);
    const Col_Word *elements = Col_VectorElements(mvector);
    PICOTEST_ASSERT(elements != NULL);
    PICOTEST_ASSERT(elements == Col_MVectorElements(mvector));
    checkElements(len, elements, words);
}
PICOTEST_CASE(testNewMVectorNil, colibriFixture) {
    checkMVector(Col_NewMVector(0, 0, NULL), 0, 0, NULL);
}
PICOTEST_CASE(testNewMVectorValues, colibriFixture) {
    Col_Word elements[] = {WORD_NIL, WORD_TRUE, WORD_FALSE, Col_NewIntWord(0),
                           Col_NewCharWord('a')};
    size_t len = sizeof(elements) / sizeof(*elements);
    Col_Word mvector = Col_NewMVector(0, len, elements);
    checkMVector(mvector, len, len, elements);
}

PICOTEST_SUITE(testMVectorCapacity, testMVectorCapacityShouldBeNonZero,
               testMVectorCapacityShouldAdjustToLength);
PICOTEST_CASE(testMVectorCapacityShouldBeNonZero, colibriFixture) {
    Col_Word mvector = Col_NewMVector(0, 0, NULL);
    PICOTEST_ASSERT(Col_MVectorCapacity(mvector) > 0);
}
PICOTEST_CASE(testMVectorCapacityShouldAdjustToLength, colibriFixture) {
    Col_Word mvector = Col_NewMVector(0, 10, NULL);
    PICOTEST_ASSERT(Col_MVectorCapacity(mvector) >= 10);
}

PICOTEST_SUITE(testMVectorSetLength, testMVectorSetLengthErrors,
               testMVectorSetLengthIncreaseShouldPreserveExistingElements,
               testMVectorSetLengthIncreaseShouldInitializeNewElementsToNil,
               testMVectorSetLengthDecreaseShouldPreserveExistingElements);

PICOTEST_SUITE(testMVectorSetLengthErrors, testMVectorSetLengthAboveCapacity);
PICOTEST_CASE(testMVectorSetLengthAboveCapacity, colibriFixture) {
    PICOTEST_ASSERT(mvectorSetLength_valueCheck(NULL) == 1);
}

PICOTEST_CASE(testMVectorSetLengthIncreaseShouldPreserveExistingElements,
              colibriFixture) {
    Col_Word words[] = {WORD_NIL, WORD_TRUE, WORD_FALSE, Col_NewIntWord(0),
                        Col_NewCharWord('a')};
    size_t len = sizeof(words) / sizeof(*words);
    Col_Word mvector = Col_NewMVector(len * 3, len, words);
    checkMVector(mvector, len, len, words);
    Col_Word *elements = Col_MVectorElements(mvector);
    Col_MVectorSetLength(mvector, len * 2);
    PICOTEST_ASSERT(Col_MVectorElements(mvector) == elements);
    checkElements(len, elements, words);
    elements[0] = Col_EmptyRope();
    Col_MVectorSetLength(mvector, len * 3);
    PICOTEST_ASSERT(Col_MVectorElements(mvector) == elements);
    PICOTEST_ASSERT(elements[0] == Col_EmptyRope());
}
PICOTEST_CASE(testMVectorSetLengthIncreaseShouldInitializeNewElementsToNil,
              colibriFixture) {
    Col_Word words[] = {WORD_NIL, WORD_TRUE, WORD_FALSE, Col_NewIntWord(0),
                        Col_NewCharWord('a')};
    size_t len = sizeof(words) / sizeof(*words);
    Col_Word mvector = Col_NewMVector(len * 2, len, words);
    checkMVector(mvector, len, len, words);
    Col_Word *elements = Col_MVectorElements(mvector);
    Col_MVectorSetLength(mvector, len * 2);
    PICOTEST_ASSERT(Col_MVectorElements(mvector) == elements);
    checkElements(len, elements, words);
    checkElements(len, elements + len, NULL);
}
PICOTEST_CASE(testMVectorSetLengthDecreaseShouldPreserveExistingElements,
              colibriFixture) {
    Col_Word words[] = {WORD_NIL, WORD_TRUE, WORD_FALSE, Col_NewIntWord(0),
                        Col_NewCharWord('a')};
    size_t len = sizeof(words) / sizeof(*words);
    Col_Word mvector = Col_NewMVector(0, len, words);
    checkMVector(mvector, len, len, words);
    Col_Word *elements = Col_MVectorElements(mvector);
    Col_MVectorSetLength(mvector, len / 2);
    PICOTEST_ASSERT(Col_MVectorElements(mvector) == elements);
    checkMVector(mvector, len, len / 2, words);
    elements[0] = Col_EmptyRope();
    Col_MVectorSetLength(mvector, 1);
    PICOTEST_ASSERT(Col_MVectorElements(mvector) == elements);
    PICOTEST_ASSERT(elements[0] == Col_EmptyRope());
}

PICOTEST_CASE(testMVectorFreeze, colibriFixture) {
    Col_Word words[] = {WORD_NIL, WORD_TRUE, WORD_FALSE, Col_NewIntWord(0),
                        Col_NewCharWord('a')};
    size_t len = sizeof(words) / sizeof(*words);
    Col_Word mvector = Col_NewMVector(0, len, words);
    PICOTEST_ASSERT(Col_WordType(mvector) ==
                    (COL_MVECTOR | COL_VECTOR | COL_LIST));
    Col_Word *elements = Col_MVectorElements(mvector);
    Col_MVectorFreeze(mvector);
    PICOTEST_ASSERT(Col_WordType(mvector) == (COL_VECTOR | COL_LIST));
    PICOTEST_ASSERT(Col_VectorElements(mvector) == elements);
    PICOTEST_ASSERT(Col_VectorLength(mvector) == len);
    checkElements(len, elements, words);
}