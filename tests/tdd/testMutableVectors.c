#include <colibri.h>
#include <picotest.h>

/*
 * Test failure cases (must be defined before test hooks)
 */

#include "failureFixture.h"

/* Type checks */
PICOTEST_CASE(typeCheckMVectorCapacity, failureFixture) {
    Col_MVectorCapacity(WORD_NIL);
}
PICOTEST_CASE(typeCheckMVectorElements, failureFixture) {
    Col_MVectorElements(WORD_NIL);
}
PICOTEST_CASE(typeCheckMVectorSetLength, failureFixture) {
    Col_MVectorSetLength(WORD_NIL, 1);
}
PICOTEST_CASE(typeCheckMVectorFreeze, failureFixture) {
    Col_MVectorFreeze(WORD_NIL);
}

/* Value checks */
PICOTEST_CASE(valueCheckNewMVectorLength, failureFixture) {
    Col_NewMVector(Col_MaxMVectorLength() + 1, 0, NULL);
}
PICOTEST_CASE(valueCheckMVectorSetLength, failureFixture) {
    Col_Word mvector = Col_NewMVector(0, 0, NULL);
    Col_MVectorSetLength(mvector, Col_MVectorCapacity(mvector) + 1);
}

/* Error cases */
PICOTEST_CASE(errorNewMVectorOutOfMemory, failureFixture) {
    Col_NewMVector(Col_MaxMVectorLength(), 0, NULL);
}

/*
 * Mutable vectors
 */

#include "hooks.h"
#include "colibriFixture.h"

PICOTEST_SUITE(testMutableVectors, testMutableVectorTypeChecks, testNewMVector,
               testMVectorCapacity, testMVectorSetLength, testMVectorFreeze);

PICOTEST_CASE(testMutableVectorTypeChecks, colibriFixture) {
    PICOTEST_ASSERT(typeCheckMVectorCapacity(NULL) == 1);
    PICOTEST_ASSERT(typeCheckMVectorElements(NULL) == 1);
    PICOTEST_ASSERT(typeCheckMVectorSetLength(NULL) == 1);
    PICOTEST_ASSERT(typeCheckMVectorFreeze(NULL) == 1);
}
PICOTEST_SUITE(testNewMVector, testNewMVectorErrors, testNewMVectorNil,
               testNewMVectorValues);

PICOTEST_SUITE(testNewMVectorErrors, testNewMVectorTooLarge,
               testNewMVectorOutOfMemory);
PICOTEST_CASE(testNewMVectorTooLarge, colibriFixture) {
    PICOTEST_ASSERT(valueCheckNewMVectorLength(NULL) == 1);
}
PICOTEST_CASE(testNewMVectorOutOfMemory, colibriFixture) {
    PICOTEST_ASSERT(errorNewMVectorOutOfMemory(NULL) == 1);
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
    PICOTEST_ASSERT(valueCheckMVectorSetLength(NULL) == 1);
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