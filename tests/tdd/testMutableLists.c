#include <colibri.h>
#include <picotest.h>

/*
 * Failure test cases (must be defined before test hooks)
 */

#include "failureFixture.h"

/* Col_CopyMList */
PICOTEST_CASE(copyMList_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(Col_CopyMList(WORD_NIL) == WORD_NIL);
}

/* Col_MListSetLength */
PICOTEST_CASE(mlistSetLength_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MLIST);
    Col_MListSetLength(WORD_NIL, 0);
}

/* Col_MListSetLength */
PICOTEST_CASE(mlistLoop_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MLIST);
    Col_MListLoop(WORD_NIL);
}

/* Col_MListSetAt */
PICOTEST_CASE(mlistSetAt_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MLIST);
    Col_MListSetAt(WORD_NIL, 0, WORD_NIL);
}
PICOTEST_CASE(mlistSetAt_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LISTINDEX);
    Col_MListSetAt(Col_NewMList(), 0, WORD_NIL);
}

/* Col_MListInsert */
PICOTEST_CASE(mlistInsert_typeCheck_into, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MLIST);
    Col_MListInsert(WORD_NIL, 0, Col_EmptyList());
}
PICOTEST_CASE(mlistInsert_typeCheck_list, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    Col_MListInsert(Col_NewMList(), 0, WORD_NIL);
}
PICOTEST_CASE(mlistInsert_valueCheck_length, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LISTLENGTH_CONCAT);
    Col_Word list = Col_NewList(SIZE_MAX, NULL);
    Col_Word mlist = Col_NewMList();
    Col_MListInsert(mlist, 0, list);
    Col_MListInsert(mlist, 0, list);
}

/* Col_MListRemove */
PICOTEST_CASE(mlistRemove_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MLIST);
    Col_MListRemove(WORD_NIL, 0, 0);
}

/* Col_MListReplace */
PICOTEST_CASE(mlistReplace_typeCheck_into, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MLIST);
    Col_MListReplace(WORD_NIL, 0, 0, Col_EmptyList());
}
PICOTEST_CASE(mlistReplace_typeCheck_list, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MLIST);
    Col_MListReplace(Col_EmptyList(), 0, 0, WORD_NIL);
}

/*
 * Test data
 */

static Col_Word MVECTOR_INTS(size_t len, intptr_t first) {
    Col_Word mvector = Col_NewMVector(0, len, NULL);
    Col_Word *elements = Col_MVectorElements(mvector);
    for (size_t i = 0; i < len; i++) {
        elements[i] = Col_NewIntWord(first + i);
    }
    return mvector;
}

/* Mutable list operations */
static Col_Word MLIST_MVECTOR() {
    Col_Word mlist = Col_NewMList();
    Col_MListSetLength(mlist, 10);
    Col_MListSetLength(mlist, 30);
    Col_MListSetLength(mlist, 10);
    return mlist;
}

static Col_Word MLIST_SHORT_VOID() {
    Col_Word mlist = Col_NewMList();
    Col_MListSetLength(mlist, 10);
    return mlist;
}

#define MLIST_VOID_LEN 1000
static Col_Word MLIST_VOID() {
    Col_Word mlist = Col_NewMList();
    Col_MListSetLength(mlist, MLIST_VOID_LEN);
    return mlist;
}

#define MLIST_VECTOR_LEN 1000
static Col_Word MLIST_VECTOR() {
    Col_Word mlist = Col_NewMList();
    Col_MListInsert(mlist, 0, MVECTOR_INTS(MLIST_VECTOR_LEN, -10000));
    return mlist;
}

#define MLIST_CONCAT_LEN1 1000
#define MLIST_CONCAT_LEN2 500
#define MLIST_CONCAT_LEN3 800
#define MLIST_CONCAT_LEN (MLIST_CONCAT_LEN1 + MLIST_CONCAT_LEN2)
static Col_Word MLIST_CONCAT() {
    Col_Word mlist = Col_NewMList();
    Col_MListInsert(mlist, 0,
                    Col_ConcatLists(MVECTOR_INTS(MLIST_CONCAT_LEN1, -20000),
                                    MVECTOR_INTS(MLIST_CONCAT_LEN2, -30000)));
    return mlist;
}
#define MLIST_CONCAT2_LEN                                                      \
    (MLIST_CONCAT_LEN1 + MLIST_CONCAT_LEN2 + MLIST_CONCAT_LEN3)
static Col_Word MLIST_CONCAT2L() {
    Col_Word mlist = Col_NewMList();
    Col_MListInsert(
        mlist, 0,
        Col_ConcatLists(
            Col_ConcatLists(MVECTOR_INTS(MLIST_CONCAT_LEN1, -40000),
                            MVECTOR_INTS(MLIST_CONCAT_LEN2, -50000)),
            MVECTOR_INTS(MLIST_CONCAT_LEN3, -60000)));
    return mlist;
}
static Col_Word MLIST_CONCAT2R() {
    Col_Word mlist = Col_NewMList();
    Col_MListInsert(
        mlist, 0,
        Col_ConcatLists(
            MVECTOR_INTS(MLIST_CONCAT_LEN1, -70000),
            Col_ConcatLists(MVECTOR_INTS(MLIST_CONCAT_LEN2, -80000),
                            MVECTOR_INTS(MLIST_CONCAT_LEN3, -90000))));
    return mlist;
}

#define MLIST_SUBLIST_LEN (MLIST_VECTOR_LEN - 1)
static Col_Word MLIST_SUBLIST() {
    Col_Word mlist = Col_NewMList();
    Col_MListInsert(mlist, 0, Col_Sublist(MLIST_VECTOR(), 1, SIZE_MAX));
    return mlist;
}
#define MLIST_SUBLIST2_LEN (MLIST_CONCAT_LEN - 1)
static Col_Word MLIST_SUBLIST2() {
    Col_Word mlist = Col_NewMList();
    Col_MListInsert(mlist, 0, Col_Sublist(MLIST_CONCAT(), 1, SIZE_MAX));
    return mlist;
}

#define MLIST_CIRCULAR_LEN 200
static Col_Word MLIST_CIRCULAR() {
    Col_Word mlist = Col_NewMList();
    Col_MListInsert(mlist, 0, MVECTOR_INTS(MLIST_CIRCULAR_LEN, -100000));
    Col_MListLoop(mlist);
    return mlist;
}

#define MLIST_CYCLIC_HEAD_LEN 150
#define MLIST_CYCLIC_TAIL_LEN 120
#define MLIST_CYCLIC_LEN (MLIST_CYCLIC_HEAD_LEN + MLIST_CYCLIC_TAIL_LEN)
static Col_Word MLIST_CYCLIC() {
    Col_Word head = MVECTOR_INTS(MLIST_CYCLIC_HEAD_LEN, -110000);
    Col_Word tail = Col_NewMList();
    Col_MListInsert(tail, 0, MVECTOR_INTS(MLIST_CYCLIC_TAIL_LEN, -120000));
    Col_MListLoop(tail);
    Col_Word mlist = Col_NewMList();
    Col_MListInsert(mlist, 0, head);
    Col_MListInsert(mlist, MLIST_CYCLIC_HEAD_LEN, tail);
    return mlist;
}

/*
 * Mutable lists
 */

#include "hooks.h"
#include "colibriFixture.h"

PICOTEST_SUITE(testMutableLists, testMutableListTypeChecks,
               testMutableListCreation, testMutableListOperations);

PICOTEST_CASE(testMutableListTypeChecks, colibriFixture) {
    PICOTEST_VERIFY(copyMList_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mlistSetLength_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mlistLoop_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mlistSetAt_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mlistInsert_typeCheck_into(NULL) == 1);
    PICOTEST_VERIFY(mlistInsert_typeCheck_list(NULL) == 1);
    PICOTEST_VERIFY(mlistRemove_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mlistReplace_typeCheck_into(NULL) == 1);
    PICOTEST_VERIFY(mlistReplace_typeCheck_list(NULL) == 1);
}

/* Mutable list creation */
PICOTEST_SUITE(testMutableListCreation, testNewMList, testCopyMList);

PICOTEST_CASE(testNewMList, colibriFixture) {
    Col_Word mlist = Col_NewMList();
    PICOTEST_ASSERT(Col_WordType(mlist) == (COL_MLIST | COL_LIST));
    PICOTEST_ASSERT(Col_ListLength(mlist) == 0);
    PICOTEST_ASSERT(Col_ListLoopLength(mlist) == 0);
}

PICOTEST_SUITE(testCopyMList, testCopyEmptyListIsIdentity,
               testCopyVectorIsIdentity, testCopyListIsIdentity,
               testCopyEmptyMutableListIsEmpty,
               testCopyEmptyMutableVectorIsNotFrozen,
               testCopyMutableVectorIsFrozen, testCopyMutableConcat);
static void checkCopyMListIdentity(Col_Word mlist) {
    PICOTEST_ASSERT(Col_CopyMList(mlist) == mlist);
}
PICOTEST_CASE(testCopyEmptyListIsIdentity, colibriFixture) {
    checkCopyMListIdentity(Col_EmptyList());
}
PICOTEST_CASE(testCopyVectorIsIdentity, colibriFixture) {
    checkCopyMListIdentity(Col_NewVector(10, NULL));
}
PICOTEST_CASE(testCopyListIsIdentity, colibriFixture) {
    checkCopyMListIdentity(Col_NewList(10, NULL));
    checkCopyMListIdentity(Col_Sublist(Col_NewList(10, NULL), 1, 2));
    checkCopyMListIdentity(Col_RepeatList(Col_NewList(10, NULL), 1000));
}
PICOTEST_CASE(testCopyEmptyMutableListIsEmpty, colibriFixture) {
    Col_Word mlist = Col_NewMList();
    PICOTEST_ASSERT(Col_ListLength(mlist) == 0);
    PICOTEST_ASSERT(Col_CopyMList(mlist) == Col_EmptyList());
}
PICOTEST_CASE(testCopyEmptyMutableVectorIsNotFrozen, colibriFixture) {
    Col_Word mvector = Col_NewMVector(0, 0, NULL);
    PICOTEST_ASSERT(Col_ListLength(mvector) == 0);
    PICOTEST_ASSERT(Col_CopyMList(mvector) == Col_EmptyList());
    PICOTEST_ASSERT(Col_WordType(mvector) & COL_MVECTOR);
}
PICOTEST_CASE(testCopyMutableVectorIsFrozen, colibriFixture) {
    Col_Word mvector = Col_NewMVector(0, 0, NULL);
    Col_MVectorSetLength(mvector, 1);
    PICOTEST_ASSERT(Col_ListLength(mvector) == 1);
    PICOTEST_ASSERT(Col_CopyMList(mvector) == mvector);
    PICOTEST_ASSERT(!(Col_WordType(mvector) & COL_MVECTOR));
}
PICOTEST_CASE(testCopyMutableConcat, colibriFixture) {
    Col_Word mlist = Col_NewMList();
    Col_MListSetLength(mlist, 1000);
    Col_MListSetAt(mlist, 1, WORD_TRUE);
    Col_MListSetAt(mlist, 500, WORD_FALSE);
    PICOTEST_ASSERT(Col_WordType(mlist) == COL_MLIST | COL_LIST);
    Col_Word copy = Col_CopyMList(mlist);
    PICOTEST_ASSERT(Col_ListDepth(mlist) == 2);
    PICOTEST_ASSERT(copy != mlist);
    PICOTEST_ASSERT(Col_WordType(mlist) == COL_MLIST | COL_LIST);
    PICOTEST_ASSERT(Col_WordType(copy) == COL_LIST);
    PICOTEST_ASSERT(Col_ListDepth(copy) == 2);
    PICOTEST_ASSERT(Col_ListAt(copy, 1) == Col_ListAt(mlist, 1))
    PICOTEST_ASSERT(Col_ListAt(copy, 500) == Col_ListAt(mlist, 500))
    Col_MListSetAt(mlist, 1, WORD_FALSE);
    Col_MListSetAt(mlist, 500, WORD_TRUE);
    PICOTEST_ASSERT(Col_ListAt(copy, 1) != Col_ListAt(mlist, 1))
    PICOTEST_ASSERT(Col_ListAt(copy, 500) != Col_ListAt(mlist, 500))
}

/* Mutable list operations */
PICOTEST_SUITE(testMutableListOperations, testMListSetLength, testMListLoop,
               testMListSetAt, testMListInsert, testMListRemove,
               testMListReplace);

PICOTEST_SUITE(testMListSetLength, testMListSetLengthInitial,
               testMListSetLengthResize, testMListSetLengthMax,
               testMListSetLengthCircular);
PICOTEST_CASE(testMListSetLengthInitial, colibriFixture) {
    Col_Word mlist = Col_NewMList();
    Col_MListSetLength(mlist, 1);
    PICOTEST_ASSERT(Col_ListLength(mlist) == 1);
    PICOTEST_ASSERT(Col_ListAt(mlist, 0) == WORD_NIL);
}
PICOTEST_CASE(testMListSetLengthResize, colibriFixture) {
    Col_Word mlist = Col_NewMList();
    Col_MListSetLength(mlist, 10);
    PICOTEST_ASSERT(Col_ListLength(mlist) == 10);
    Col_MListSetLength(mlist, 0);
    PICOTEST_ASSERT(Col_ListLength(mlist) == 0);
    Col_MListSetLength(mlist, 10);
    PICOTEST_ASSERT(Col_ListLength(mlist) == 10);
    Col_MListSetLength(mlist, 20);
    PICOTEST_ASSERT(Col_ListLength(mlist) == 20);
    Col_MListSetLength(mlist, 10);
    PICOTEST_ASSERT(Col_ListLength(mlist) == 10);
    Col_MListSetLength(mlist, 0);
    PICOTEST_ASSERT(Col_ListLength(mlist) == 0);
}
PICOTEST_CASE(testMListSetLengthMax, colibriFixture) {
    Col_Word mlist = Col_NewMList();
    Col_MListSetLength(mlist, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListLength(mlist) == SIZE_MAX);
    PICOTEST_ASSERT(Col_ListAt(mlist, SIZE_MAX - 1) == WORD_NIL);
}

static void checkMListSetLengthCircular(Col_Word mlist, size_t len) {
    Col_Word element = Col_ListAt(mlist, len - 1);
    PICOTEST_ASSERT(Col_ListLength(mlist) == Col_ListLoopLength(mlist));
    PICOTEST_ASSERT(Col_ListLoopLength(mlist) != 0);
    Col_MListSetLength(mlist, len);
    PICOTEST_ASSERT(Col_ListLength(mlist) == len);
    PICOTEST_ASSERT(Col_ListLoopLength(mlist) == 0);
    PICOTEST_ASSERT(Col_ListAt(mlist, len - 1) == element);
}
PICOTEST_CASE(testMListSetLengthCircular, colibriFixture) {
    checkMListSetLengthCircular(MLIST_CIRCULAR(), 1);
    checkMListSetLengthCircular(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN);
    checkMListSetLengthCircular(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN + 1);
    checkMListSetLengthCircular(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN * 2 - 1);
    checkMListSetLengthCircular(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN * 4 + 1);
    checkMListSetLengthCircular(MLIST_CIRCULAR(), SIZE_MAX);
}

PICOTEST_SUITE(testMListLoop, testMListLoopOfEmptyMListIsNoop,
               testMListLoopOfCircularMListIsNoop, testMListLoopVector);
PICOTEST_CASE(testMListLoopOfEmptyMListIsNoop, colibriFixture) {
    Col_Word mlist = Col_NewMList();
    PICOTEST_ASSERT(Col_ListLength(mlist) == 0);
    PICOTEST_ASSERT(Col_ListLoopLength(mlist) == 0);
    Col_MListLoop(mlist);
    PICOTEST_ASSERT(Col_ListLength(mlist) == 0);
    PICOTEST_ASSERT(Col_ListLoopLength(mlist) == 0);
}
PICOTEST_CASE(testMListLoopOfCircularMListIsNoop, colibriFixture) {
    Col_Word mlist = MLIST_CIRCULAR();
    PICOTEST_ASSERT(Col_ListLength(mlist) == MLIST_CIRCULAR_LEN);
    PICOTEST_ASSERT(Col_ListLoopLength(mlist) == MLIST_CIRCULAR_LEN);
    Col_MListLoop(mlist);
    PICOTEST_ASSERT(Col_ListLength(mlist) == MLIST_CIRCULAR_LEN);
    PICOTEST_ASSERT(Col_ListLoopLength(mlist) == MLIST_CIRCULAR_LEN);
}
PICOTEST_CASE(testMListLoopVector, colibriFixture) {
    Col_Word mlist = MLIST_VECTOR();
    PICOTEST_ASSERT(Col_ListLength(mlist) == MLIST_VECTOR_LEN);
    PICOTEST_ASSERT(Col_ListLoopLength(mlist) == 0);
    Col_MListLoop(mlist);
    PICOTEST_ASSERT(Col_ListLength(mlist) == MLIST_VECTOR_LEN);
    PICOTEST_ASSERT(Col_ListLoopLength(mlist) == MLIST_VECTOR_LEN);
    PICOTEST_ASSERT(Col_ListAt(mlist, 0) ==
                    Col_ListAt(mlist, MLIST_VECTOR_LEN));
    PICOTEST_ASSERT(Col_ListAt(mlist, 1) ==
                    Col_ListAt(mlist, MLIST_VECTOR_LEN * 2 + 1));
}

PICOTEST_SUITE(testMListSetAt, testMListSetAtErrors, testMListSetAtMVector,
               testMListSetAtVoid, testMListSetAtConcat, testMListSetAtSublist,
               testMListSetAtCircular, testMListSetAtCyclic);

PICOTEST_SUITE(testMListSetAtErrors, testMListSetAtOutOfBounds);
PICOTEST_CASE(testMListSetAtOutOfBounds, colibriFixture) {
    PICOTEST_ASSERT(mlistSetAt_valueCheck(NULL) == 1);
}

static void checkMListSetAt(Col_Word mlist, size_t index) {
    Col_Word value = WORD_TRUE;
    PICOTEST_ASSERT(Col_ListAt(mlist, index) != value);
    Col_MListSetAt(mlist, index, value);
    PICOTEST_ASSERT(Col_ListAt(mlist, index) == value);
}
PICOTEST_CASE(testMListSetAtMVector, colibriFixture) {
    checkMListSetAt(MLIST_MVECTOR(), 1);
}
PICOTEST_CASE(testMListSetAtVoid, colibriFixture) {
    checkMListSetAt(MLIST_SHORT_VOID(), 1);
    checkMListSetAt(MLIST_VOID(), 1);
    checkMListSetAt(MLIST_VOID(), MLIST_VOID_LEN - 2);
    checkMListSetAt(MLIST_VOID(), MLIST_VOID_LEN / 2);
}
PICOTEST_CASE(testMListSetAtConcat, colibriFixture) {
    checkMListSetAt(MLIST_CONCAT(), 1);
    checkMListSetAt(MLIST_CONCAT(), MLIST_CONCAT_LEN1 - 2);
    checkMListSetAt(MLIST_CONCAT(), MLIST_CONCAT_LEN1 + 2);
    checkMListSetAt(MLIST_CONCAT(), MLIST_CONCAT_LEN - 2);
    checkMListSetAt(MLIST_CONCAT(), MLIST_CONCAT_LEN / 2);
    checkMListSetAt(MLIST_CONCAT2L(), 1);
    checkMListSetAt(MLIST_CONCAT2L(), MLIST_CONCAT_LEN1 - 2);
    checkMListSetAt(MLIST_CONCAT2L(), MLIST_CONCAT_LEN1 + 2);
    checkMListSetAt(MLIST_CONCAT2L(),
                    MLIST_CONCAT_LEN1 + MLIST_CONCAT_LEN2 - 2);
    checkMListSetAt(MLIST_CONCAT2L(),
                    MLIST_CONCAT_LEN1 + MLIST_CONCAT_LEN2 + 2);
    checkMListSetAt(MLIST_CONCAT2L(), MLIST_CONCAT2_LEN - 2);
    checkMListSetAt(MLIST_CONCAT2L(), MLIST_CONCAT2_LEN / 2);
    checkMListSetAt(MLIST_CONCAT2R(), 1);
    checkMListSetAt(MLIST_CONCAT2R(), MLIST_CONCAT_LEN1 - 2);
    checkMListSetAt(MLIST_CONCAT2R(), MLIST_CONCAT_LEN1 + 2);
    checkMListSetAt(MLIST_CONCAT2R(),
                    MLIST_CONCAT_LEN1 + MLIST_CONCAT_LEN2 - 2);
    checkMListSetAt(MLIST_CONCAT2R(),
                    MLIST_CONCAT_LEN1 + MLIST_CONCAT_LEN2 + 2);
    checkMListSetAt(MLIST_CONCAT2R(), MLIST_CONCAT2_LEN - 2);
    checkMListSetAt(MLIST_CONCAT2R(), MLIST_CONCAT2_LEN / 2);
}
PICOTEST_CASE(testMListSetAtSublist, colibriFixture) {
    checkMListSetAt(MLIST_SUBLIST(), 1);
    checkMListSetAt(MLIST_SUBLIST(), MLIST_SUBLIST_LEN - 2);
    checkMListSetAt(MLIST_SUBLIST(), MLIST_SUBLIST_LEN / 2);
    checkMListSetAt(MLIST_SUBLIST2(), 1);
    checkMListSetAt(MLIST_SUBLIST2(), MLIST_SUBLIST2_LEN - 2);
    checkMListSetAt(MLIST_SUBLIST2(), MLIST_SUBLIST2_LEN / 2);
}
PICOTEST_CASE(testMListSetAtCircular, colibriFixture) {
    checkMListSetAt(MLIST_CIRCULAR(), 1);
    checkMListSetAt(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN - 2);
    checkMListSetAt(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN * 2 + 5);
    checkMListSetAt(MLIST_CIRCULAR(), SIZE_MAX);
}
PICOTEST_CASE(testMListSetAtCyclic, colibriFixture) {
    checkMListSetAt(MLIST_CYCLIC(), 1);
    checkMListSetAt(MLIST_CYCLIC(), MLIST_CYCLIC_HEAD_LEN - 2);
    checkMListSetAt(MLIST_CYCLIC(), MLIST_CYCLIC_HEAD_LEN + 2);
    checkMListSetAt(MLIST_CYCLIC(),
                    MLIST_CYCLIC_HEAD_LEN + MLIST_CYCLIC_TAIL_LEN * 2 + 15);
    checkMListSetAt(MLIST_CYCLIC(), MLIST_CYCLIC_LEN - 2);
    checkMListSetAt(MLIST_CYCLIC(), SIZE_MAX);
}

PICOTEST_SUITE(testMListInsert, testMListInsertErrors,
               testMListInsertEmptyListIsNoop, testMListInsertMax,
               testMListInsertConcatBeginning, testMListInsertConcatEnd,
               testMListInsertMergeShort, testMListInsertIntoAvailableSpace,
               testMListInsertConvert, testMListInsertSplit,
               testMListInsertIntoCircular, testMListInsertIntoCyclic,
               testMListInsertCircular, testMListInsertCyclic);
PICOTEST_SUITE(testMListInsertErrors, testMListInsertTooLarge);
PICOTEST_CASE(testMListInsertTooLarge, colibriFixture) {
    PICOTEST_ASSERT(mlistInsert_valueCheck_length(NULL) == 1);
}
PICOTEST_CASE(testMListInsertEmptyListIsNoop, colibriFixture) {
    Col_Word mlist = Col_NewMList();
    PICOTEST_ASSERT(Col_ListLength(mlist) == 0);
    Col_MListInsert(mlist, 0, Col_EmptyList());
    PICOTEST_ASSERT(Col_ListLength(mlist) == 0);
}
PICOTEST_CASE(testMListInsertMax, colibriFixture) {
    Col_Word mlist = Col_NewMList();
    PICOTEST_ASSERT(Col_ListLength(mlist) == 0);
    Col_MListInsert(mlist, 0, Col_NewList(SIZE_MAX, NULL));
    PICOTEST_ASSERT(Col_ListLength(mlist) == SIZE_MAX);
    PICOTEST_ASSERT(Col_ListAt(mlist, 0) == WORD_NIL);
    PICOTEST_ASSERT(Col_ListAt(mlist, SIZE_MAX - 1) == WORD_NIL);
}
PICOTEST_CASE(testMListInsertConcatBeginning, colibriFixture) {
    Col_Word mlist = Col_NewMList();
    Col_MListSetLength(mlist, 100);
    PICOTEST_ASSERT(Col_ListAt(mlist, 0) == WORD_NIL);
    Col_Word mvector = MVECTOR_INTS(200, 1);
    Col_MListInsert(mlist, 0, mvector);
    PICOTEST_ASSERT(Col_ListAt(mlist, 0) == Col_ListAt(mvector, 0));
    PICOTEST_ASSERT(Col_ListAt(mlist, 200) == WORD_NIL);
}
PICOTEST_CASE(testMListInsertConcatEnd, colibriFixture) {
    Col_Word mlist = Col_NewMList();
    Col_MListSetLength(mlist, 100);
    PICOTEST_ASSERT(Col_ListAt(mlist, 0) == WORD_NIL);
    Col_Word mvector = MVECTOR_INTS(200, 1);
    Col_MListInsert(mlist, SIZE_MAX, mvector);
    PICOTEST_ASSERT(Col_ListAt(mlist, 0) == WORD_NIL);
    PICOTEST_ASSERT(Col_ListAt(mlist, 100) == Col_ListAt(mvector, 0));
}

static void checkMListInsert(Col_Word into, size_t index, size_t len) {
    size_t loop = Col_ListLoopLength(into);
    size_t headLen = Col_ListLength(into) - loop;
    Col_Word elements[] = {Col_ListAt(into, 0), Col_ListAt(into, index - 1),
                           Col_ListAt(into, index)};
    Col_Word mvector = MVECTOR_INTS(len, 1);
    Col_MListInsert(into, index, mvector);
    if (headLen && index >= headLen) {
        index = headLen + (index - headLen) % loop;
    }
    if (index != 0) {
        PICOTEST_ASSERT(Col_ListAt(into, 0) == elements[0]);
        PICOTEST_ASSERT(Col_ListAt(into, index - 1) == elements[1]);
    }
    PICOTEST_ASSERT(Col_ListAt(into, index) == Col_ListAt(mvector, 0));
    PICOTEST_ASSERT(Col_ListAt(into, index + len - 1) ==
                    Col_ListAt(mvector, len - 1));
    PICOTEST_ASSERT(Col_ListAt(into, index + len) == elements[2]);
}

PICOTEST_CASE(testMListInsertMergeShort, colibriFixture) {
    Col_Word mlist = Col_NewMList();
    Col_MListSetLength(mlist, 10);
    checkMListInsert(mlist, 3, 15);
}
PICOTEST_CASE(testMListInsertIntoAvailableSpace, colibriFixture) {
    checkMListInsert(MLIST_MVECTOR(), 3, 15);
}
PICOTEST_CASE(testMListInsertConvert, colibriFixture) {
    checkMListInsert(MLIST_CONCAT(), 3, 200);
    checkMListInsert(MLIST_CONCAT(), MLIST_CONCAT_LEN1, 200);
    checkMListInsert(MLIST_CONCAT(), MLIST_CONCAT_LEN1 + 3, 200);
    checkMListInsert(MLIST_CONCAT2L(), 3, 200);
    checkMListInsert(MLIST_CONCAT2L(), MLIST_CONCAT_LEN1, 200);
    checkMListInsert(MLIST_CONCAT2L(), MLIST_CONCAT_LEN1 + 3, 200);
    checkMListInsert(MLIST_CONCAT2L(), MLIST_CONCAT_LEN1 + MLIST_CONCAT_LEN2,
                     200);
    checkMListInsert(MLIST_CONCAT2L(),
                     MLIST_CONCAT_LEN1 + MLIST_CONCAT_LEN2 + 3, 200);
    checkMListInsert(MLIST_CONCAT2R(), 3, 200);
    checkMListInsert(MLIST_CONCAT2R(), MLIST_CONCAT_LEN1, 200);
    checkMListInsert(MLIST_CONCAT2R(), MLIST_CONCAT_LEN1 + 3, 200);
    checkMListInsert(MLIST_CONCAT2R(), MLIST_CONCAT_LEN1 + MLIST_CONCAT_LEN2,
                     200);
    checkMListInsert(MLIST_CONCAT2R(),
                     MLIST_CONCAT_LEN1 + MLIST_CONCAT_LEN2 + 3, 200);
    checkMListInsert(MLIST_SUBLIST2(), 3, 200);
}
PICOTEST_CASE(testMListInsertSplit, colibriFixture) {
    checkMListInsert(MLIST_MVECTOR(), 3, 200);
    checkMListInsert(MLIST_VOID(), 3, 200);
    checkMListInsert(MLIST_VECTOR(), 3, 200);
    checkMListInsert(MLIST_SUBLIST(), 3, 200);
}

static void checkMListInsertIntoCircular(Col_Word into, size_t index,
                                         size_t len) {
    Col_Word elements[] = {Col_ListAt(into, 0), Col_ListAt(into, index - 1),
                           Col_ListAt(into, index)};
    Col_Word mvector = MVECTOR_INTS(len, 1);
    Col_MListInsert(into, index, mvector);
    if (index != 0) {
        PICOTEST_ASSERT(Col_ListAt(into, 0) == elements[0]);
        PICOTEST_ASSERT(Col_ListAt(into, index - 1) == elements[1]);
    }
    PICOTEST_ASSERT(Col_ListAt(into, index) == Col_ListAt(mvector, 0));
    PICOTEST_ASSERT(Col_ListAt(into, index + len - 1) ==
                    Col_ListAt(mvector, len - 1));
    PICOTEST_ASSERT(Col_ListAt(into, index + len) == elements[2]);
}
PICOTEST_CASE(testMListInsertIntoCircular, colibriFixture) {
    checkMListInsert(MLIST_CIRCULAR(), 0, 200);
    checkMListInsert(MLIST_CIRCULAR(), 10, 200);
    checkMListInsert(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN - 1, 200);
    checkMListInsert(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN, 200);
    checkMListInsert(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN * 2 + 20, 200);
}
PICOTEST_CASE(testMListInsertIntoCyclic, colibriFixture) {
    checkMListInsert(MLIST_CYCLIC(), 0, 200);
    checkMListInsert(MLIST_CYCLIC(), 10, 200);
    checkMListInsert(MLIST_CYCLIC(), MLIST_CYCLIC_HEAD_LEN - 1, 200);
    checkMListInsert(MLIST_CYCLIC(), MLIST_CYCLIC_HEAD_LEN, 200);
    checkMListInsert(MLIST_CYCLIC(), MLIST_CYCLIC_HEAD_LEN + 1, 200);
    checkMListInsert(MLIST_CYCLIC(),
                     MLIST_CYCLIC_HEAD_LEN + MLIST_CYCLIC_TAIL_LEN * 2 + 15,
                     200);
}

static void checkMListInsertCircular(Col_Word into, size_t index) {
    size_t len = Col_ListLength(into);
    size_t loop = Col_ListLoopLength(into);
    size_t headLen = len - loop;
    Col_Word list = MLIST_CIRCULAR();
    Col_MListInsert(into, index, list);
    if (index < headLen) {
        headLen = index;
    } else if (loop && index > headLen) {
        headLen = headLen + (index - headLen) % loop;
        if (headLen == 0)
            headLen = len;
    }
    PICOTEST_ASSERT(Col_ListLength(into) == headLen + MLIST_CIRCULAR_LEN);
    PICOTEST_ASSERT(Col_ListLoopLength(into) == MLIST_CIRCULAR_LEN);
    // TODO test ListAt
}
PICOTEST_CASE(testMListInsertCircular, colibriFixture) {
    checkMListInsertCircular(MLIST_VECTOR(), 0);
    checkMListInsertCircular(MLIST_VECTOR(), 10);
    checkMListInsertCircular(MLIST_VECTOR(), SIZE_MAX);
    checkMListInsertCircular(MLIST_CIRCULAR(), 0);
    checkMListInsertCircular(MLIST_CIRCULAR(), 10);
    checkMListInsertCircular(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN - 1);
    checkMListInsertCircular(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN);
    checkMListInsertCircular(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN * 2 + 20);
    checkMListInsertCircular(MLIST_CIRCULAR(), SIZE_MAX);
    checkMListInsertCircular(MLIST_CYCLIC(), 0);
    checkMListInsertCircular(MLIST_CYCLIC(), 10);
    checkMListInsertCircular(MLIST_CYCLIC(), MLIST_CYCLIC_HEAD_LEN - 1);
    checkMListInsertCircular(MLIST_CYCLIC(), MLIST_CYCLIC_HEAD_LEN);
    checkMListInsertCircular(MLIST_CYCLIC(), MLIST_CYCLIC_HEAD_LEN + 1);
    checkMListInsertCircular(
        MLIST_CYCLIC(), MLIST_CYCLIC_HEAD_LEN + MLIST_CYCLIC_TAIL_LEN * 2 + 15);
    checkMListInsertCircular(MLIST_CYCLIC(), SIZE_MAX);
}

static void checkMListInsertCyclic(Col_Word into, size_t index) {
    size_t len = Col_ListLength(into);
    size_t loop = Col_ListLoopLength(into);
    size_t headLen = len - loop;
    Col_Word list = MLIST_CYCLIC();
    Col_MListInsert(into, index, list);
    if (index < headLen) {
        headLen = index;
    } else if (loop && index > headLen) {
        headLen = headLen + (index - headLen) % loop;
        if (headLen == 0)
            headLen = len;
    }
    PICOTEST_ASSERT(Col_ListLength(into) == headLen + MLIST_CYCLIC_LEN);
    PICOTEST_ASSERT(Col_ListLoopLength(into) == MLIST_CYCLIC_TAIL_LEN);
    // TODO test ListAt
}
PICOTEST_CASE(testMListInsertCyclic, colibriFixture) {
    checkMListInsertCyclic(MLIST_VECTOR(), 0);
    checkMListInsertCyclic(MLIST_VECTOR(), 10);
    checkMListInsertCyclic(MLIST_VECTOR(), SIZE_MAX);
    checkMListInsertCyclic(MLIST_CIRCULAR(), 0);
    checkMListInsertCyclic(MLIST_CIRCULAR(), 10);
    checkMListInsertCyclic(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN - 1);
    checkMListInsertCyclic(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN);
    checkMListInsertCyclic(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN * 2 + 20);
    checkMListInsertCyclic(MLIST_CIRCULAR(), SIZE_MAX);
    checkMListInsertCyclic(MLIST_CYCLIC(), 0);
    checkMListInsertCyclic(MLIST_CYCLIC(), 10);
    checkMListInsertCyclic(MLIST_CYCLIC(), MLIST_CYCLIC_HEAD_LEN - 1);
    checkMListInsertCyclic(MLIST_CYCLIC(), MLIST_CYCLIC_HEAD_LEN);
    checkMListInsertCyclic(MLIST_CYCLIC(), MLIST_CYCLIC_HEAD_LEN + 1);
    checkMListInsertCyclic(MLIST_CYCLIC(), MLIST_CYCLIC_HEAD_LEN +
                                               MLIST_CYCLIC_TAIL_LEN * 2 + 15);
    checkMListInsertCyclic(MLIST_CYCLIC(), SIZE_MAX);
}

PICOTEST_SUITE(testMListRemove, testMListRemoveFromEmptyIsNoop,
               testMListRemovePastEndIsNoop, testMListRemoveInvalidRangeIsNoop,
               testMListRemoveAllContent, testMListRemoveVector,
               testMListRemoveMVector, testMListRemoveVoid,
               testMListRemoveSublist, testMListRemoveConcat,
               testMListRemoveMerge);
PICOTEST_CASE(testMListRemoveFromEmptyIsNoop, colibriFixture) {
    Col_Word mlist = Col_NewMList();
    PICOTEST_ASSERT(Col_ListLength(mlist) == 0);
    Col_MListRemove(mlist, 0, 0);
    PICOTEST_ASSERT(Col_ListLength(mlist) == 0);
}
PICOTEST_CASE(testMListRemovePastEndIsNoop, colibriFixture) {
    Col_Word mlist = MLIST_VECTOR();
    PICOTEST_ASSERT(Col_ListLength(mlist) == MLIST_VECTOR_LEN);
    Col_MListRemove(mlist, MLIST_VECTOR_LEN, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListLength(mlist) == MLIST_VECTOR_LEN);
}
PICOTEST_CASE(testMListRemoveInvalidRangeIsNoop, colibriFixture) {
    Col_Word mlist = MLIST_VECTOR();
    PICOTEST_ASSERT(Col_ListLength(mlist) == MLIST_VECTOR_LEN);
    Col_MListRemove(mlist, 1, 0);
    PICOTEST_ASSERT(Col_ListLength(mlist) == MLIST_VECTOR_LEN);
}
PICOTEST_CASE(testMListRemoveAllContent, colibriFixture) {
    Col_Word mlist = MLIST_VECTOR();
    PICOTEST_ASSERT(Col_ListLength(mlist) == MLIST_VECTOR_LEN);
    Col_MListRemove(mlist, 0, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListLength(mlist) == 0);
}

static void checkMListRemove(Col_Word mlist, size_t first, size_t last) {
    size_t len = Col_ListLength(mlist);
    if (last >= len)
        last = len - 1;
    Col_Word elements[] = {Col_ListAt(mlist, 0), Col_ListAt(mlist, first - 1),
                           Col_ListAt(mlist, last + 1)};
    Col_MListRemove(mlist, first, last);
    PICOTEST_ASSERT(Col_ListLength(mlist) == len - (last - first + 1));
    if (first > 0)
        PICOTEST_ASSERT(Col_ListAt(mlist, 0) == elements[0]);
    PICOTEST_ASSERT(Col_ListAt(mlist, first - 1) == elements[1]);
    if (last + 1 < len)
        PICOTEST_ASSERT(Col_ListAt(mlist, first) == elements[2]);
}
PICOTEST_CASE(testMListRemoveVector, colibriFixture) {
    checkMListRemove(MLIST_VECTOR(), 0, 1);
    checkMListRemove(MLIST_VECTOR(), 1, 2);
    checkMListRemove(MLIST_VECTOR(), MLIST_VECTOR_LEN - 1, SIZE_MAX);
}
PICOTEST_CASE(testMListRemoveMVector, colibriFixture) {
    checkMListRemove(MLIST_MVECTOR(), 1, 2);
}
PICOTEST_CASE(testMListRemoveVoid, colibriFixture) {
    checkMListRemove(MLIST_VOID(), 1, 2);
}
PICOTEST_CASE(testMListRemoveSublist, colibriFixture) {
    checkMListRemove(MLIST_SUBLIST(), 1, 2);
    checkMListRemove(MLIST_SUBLIST2(), 1, 2);
}
PICOTEST_CASE(testMListRemoveConcat, colibriFixture) {
    checkMListRemove(MLIST_CONCAT(), 1, 2);
    checkMListRemove(MLIST_CONCAT(), MLIST_CONCAT_LEN1 + 1,
                     MLIST_CONCAT_LEN1 + 2);
    checkMListRemove(MLIST_CONCAT(), MLIST_CONCAT_LEN1 - 1,
                     MLIST_CONCAT_LEN1 + 1);
    checkMListRemove(MLIST_CONCAT(), 0, MLIST_CONCAT_LEN1 - 1);
    checkMListRemove(MLIST_CONCAT(), MLIST_CONCAT_LEN1, SIZE_MAX);
}
PICOTEST_CASE(testMListRemoveMerge, colibriFixture) {
    checkMListRemove(MLIST_CONCAT(), 1, MLIST_CONCAT_LEN - 2);
    checkMListRemove(MLIST_CONCAT2L(), 1, MLIST_CONCAT2_LEN - 2);
    checkMListRemove(MLIST_CONCAT2R(), 1, MLIST_CONCAT2_LEN - 2);
}

PICOTEST_SUITE(testMListReplace, testMListReplaceInvalidRangeIsNoop,
               testMListReplaceOneElement, testMListReplaceInCircularList,
               testMListReplaceInCyclicList);
PICOTEST_CASE(testMListReplaceInvalidRangeIsNoop, colibriFixture) {
    Col_Word mlist = MLIST_VECTOR();
    Col_MListReplace(mlist, 1, 0, Col_NewList(100, NULL));
    PICOTEST_ASSERT(Col_ListLength(mlist) == MLIST_VECTOR_LEN);
}
PICOTEST_CASE(testMListReplaceOneElement, colibriFixture) {
    Col_Word mlist = MLIST_VECTOR();
    size_t index = MLIST_VECTOR_LEN / 3;
    Col_Word e = Col_ListAt(mlist, index);
    PICOTEST_ASSERT(e != WORD_NIL);
    Col_MListReplace(mlist, index, index, Col_NewList(1, NULL));
    PICOTEST_ASSERT(Col_ListLength(mlist) == MLIST_VECTOR_LEN);
    PICOTEST_ASSERT(Col_ListAt(mlist, index) == WORD_NIL);
}

static void checkMListReplaceInCircularList(Col_Word mlist, size_t first,
                                            size_t last, Col_Word with) {
    Col_Word orig = Col_CopyMList(mlist);
    size_t loop = Col_ListLoopLength(mlist);
    Col_MListReplace(mlist, first, last, with);
    size_t first2 = first % loop;
    if (first2 == 0 && first != 0)
        first2 = loop;
    if (first > 0) {
        PICOTEST_ASSERT(Col_ListAt(mlist, 0) == Col_ListAt(orig, 0));
        PICOTEST_ASSERT(Col_ListAt(mlist, first2 - 1) ==
                        Col_ListAt(orig, first - 1));
    }
    PICOTEST_ASSERT(Col_ListAt(mlist, first2) == Col_ListAt(with, 0));
    PICOTEST_ASSERT(Col_ListAt(mlist, first2 + Col_ListLength(with) - 1) ==
                    Col_ListAt(with, Col_ListLength(with) - 1));
    PICOTEST_ASSERT(Col_ListAt(mlist, first2 + Col_ListLength(with)) ==
                    Col_ListAt(orig, last + 1));
}
PICOTEST_CASE(testMListReplaceInCircularList, colibriFixture) {
    checkMListReplaceInCircularList(MLIST_CIRCULAR(), 0, 10, MLIST_VECTOR());
    checkMListReplaceInCircularList(MLIST_CIRCULAR(), 10, 20, MLIST_VECTOR());
    checkMListReplaceInCircularList(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN * 2,
                                    MLIST_CIRCULAR_LEN * 3 + 10,
                                    MLIST_VECTOR());
    checkMListReplaceInCircularList(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN + 2,
                                    MLIST_CIRCULAR_LEN + 10, MLIST_VECTOR());
    checkMListReplaceInCircularList(MLIST_CIRCULAR(), MLIST_CIRCULAR_LEN + 2,
                                    MLIST_CIRCULAR_LEN * 4 + 5, MLIST_VECTOR());
}
static void checkMListReplaceInCyclicList(Col_Word mlist, size_t first,
                                          size_t last, Col_Word with) {
    Col_Word orig = Col_CopyMList(mlist);
    size_t tailLen = Col_ListLoopLength(mlist);
    size_t headLen = Col_ListLength(mlist) - tailLen;
    Col_MListReplace(mlist, first, last, with);
    size_t first2 =
        (first > headLen) ? (first - headLen) % tailLen + headLen : first;
    if (first > 0) {
        PICOTEST_ASSERT(Col_ListAt(mlist, 0) == Col_ListAt(orig, 0));
        PICOTEST_ASSERT(Col_ListAt(mlist, first2 - 1) ==
                        Col_ListAt(orig, first - 1));
    }
    PICOTEST_ASSERT(Col_ListAt(mlist, first2) == Col_ListAt(with, 0));
    PICOTEST_ASSERT(Col_ListAt(mlist, first2 + Col_ListLength(with) - 1) ==
                    Col_ListAt(with, Col_ListLength(with) - 1));
    PICOTEST_ASSERT(Col_ListAt(mlist, first2 + Col_ListLength(with)) ==
                    Col_ListAt(orig, last + 1));
}
PICOTEST_CASE(testMListReplaceInCyclicList, colibriFixture) {
    checkMListReplaceInCyclicList(MLIST_CYCLIC(), 0, 10, MLIST_VECTOR());
    checkMListReplaceInCyclicList(MLIST_CYCLIC(), 10, 20, MLIST_VECTOR());
    checkMListReplaceInCyclicList(
        MLIST_CYCLIC(), MLIST_CYCLIC_HEAD_LEN - 5,
        MLIST_CYCLIC_HEAD_LEN + MLIST_CYCLIC_TAIL_LEN + 15, MLIST_VECTOR());
    checkMListReplaceInCyclicList(
        MLIST_CYCLIC(), MLIST_CYCLIC_HEAD_LEN,
        MLIST_CYCLIC_HEAD_LEN + MLIST_CYCLIC_TAIL_LEN * 3 + 10, MLIST_VECTOR());
}
