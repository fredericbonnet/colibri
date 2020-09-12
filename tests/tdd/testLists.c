#include <colibri.h>
#include <picotest.h>

/*
 * Failure test cases (must be defined before test hooks)
 */

#include "failureFixture.h"

/* Col_ListLength */
PICOTEST_CASE(listLength_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(Col_ListLength(WORD_NIL) == 0);
}

/* Col_ListLoopLength */
PICOTEST_CASE(listLoopLength_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(Col_ListLoopLength(WORD_NIL) == 0);
}

/* Col_ListDepth */
PICOTEST_CASE(listDepth_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(Col_ListDepth(WORD_NIL) == 0);
}

/* Col_ListAt */
PICOTEST_CASE(listAt_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(Col_ListAt(WORD_NIL, 0) == WORD_NIL);
}

/* Col_Sublist */
PICOTEST_CASE(sublist_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(Col_Sublist(WORD_NIL, 0, 0) == WORD_NIL);
}

/* Col_ConcatLists */
PICOTEST_CASE(concatLists_typeCheck_left, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(Col_ConcatLists(WORD_NIL, Col_EmptyList()) == WORD_NIL);
}
PICOTEST_CASE(concatLists_typeCheck_right, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(Col_ConcatLists(Col_EmptyList(), WORD_NIL) == WORD_NIL);
}
PICOTEST_CASE(concatLists_valueCheck_length, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LISTLENGTH_CONCAT);
    Col_Word list = Col_NewList(SIZE_MAX, NULL);
    PICOTEST_ASSERT(Col_ConcatLists(list, list) == WORD_NIL);
}

/* Col_ConcatListsA */
PICOTEST_CASE(concatListsA_valueCheck_number, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_GENERIC);
    PICOTEST_ASSERT(Col_ConcatListsA(0, NULL) == WORD_NIL);
}

/* Col_ConcatListsNV */
PICOTEST_CASE(concatListsNV_valueCheck_number, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_GENERIC);
    PICOTEST_ASSERT(Col_ConcatListsNV(0) == WORD_NIL);
}

/* Col_RepeatLists */
PICOTEST_CASE(repeatList_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(Col_RepeatList(WORD_NIL, 0) == WORD_NIL);
}
PICOTEST_CASE(repeatList_valueCheck_length, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LISTLENGTH_REPEAT);
    Col_Word list = Col_NewList(2, NULL);
    PICOTEST_ASSERT(Col_RepeatList(list, SIZE_MAX) == WORD_NIL);
}

/* Col_CircularList */
PICOTEST_CASE(circularList_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(Col_CircularList(WORD_NIL) == WORD_NIL);
}

/* Col_ListInsert */
PICOTEST_CASE(listInsert_typeCheck_into, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(Col_ListInsert(WORD_NIL, 0, Col_EmptyList()) == WORD_NIL);
}
PICOTEST_CASE(listInsert_typeCheck_list, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(Col_ListInsert(Col_EmptyList(), 0, WORD_NIL) == WORD_NIL);
}

/* Col_ListRemove */
PICOTEST_CASE(listRemove_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(Col_ListRemove(WORD_NIL, 0, 0) == WORD_NIL);
}

/* Col_ListReplace */
PICOTEST_CASE(listReplace_typeCheck_list, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(Col_ListReplace(WORD_NIL, 0, 0, Col_EmptyList()) ==
                    WORD_NIL);
}
PICOTEST_CASE(listReplace_typeCheck_with, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(Col_ListReplace(Col_EmptyList(), 0, 0, WORD_NIL) ==
                    WORD_NIL);
}

/* Col_TraverseListChunksN */
PICOTEST_CASE(traverseListChunksN_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    Col_Word lists[] = {WORD_NIL};
    PICOTEST_ASSERT(Col_TraverseListChunksN(1, lists, 0, 0, NULL, NULL, NULL) ==
                    -1);
}
PICOTEST_CASE(traverseListChunksN_valueCheck_proc, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_GENERIC);
    Col_Word lists[] = {Col_EmptyList()};
    PICOTEST_ASSERT(Col_TraverseListChunksN(1, lists, 0, 0, NULL, NULL, NULL) ==
                    -1);
}

/* Col_TraverseListChunks */
PICOTEST_CASE(traverseListChunks_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(
        Col_TraverseListChunks(WORD_NIL, 0, 0, 0, NULL, NULL, NULL) == -1);
}
PICOTEST_CASE(traverseListChunks_valueCheck_proc, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_GENERIC);
    PICOTEST_ASSERT(Col_TraverseListChunks(Col_EmptyList(), 0, 0, 0, NULL, NULL,
                                           NULL) == -1);
}

/* Col_ListIterBegin */
PICOTEST_CASE(listIterBegin_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    Col_ListIterator it;
    PICOTEST_ASSERT(Col_ListIterBegin(it, WORD_NIL, 0) == 0);
    PICOTEST_ASSERT(Col_ListIterNull(it));
}

/* Col_ListIterFirst */
PICOTEST_CASE(listIterFirst_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    Col_ListIterator it;
    Col_ListIterFirst(it, WORD_NIL);
    PICOTEST_ASSERT(Col_ListIterNull(it));
}

/* Col_ListIterLast */
PICOTEST_CASE(listIterLast_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    Col_ListIterator it;
    Col_ListIterLast(it, WORD_NIL);
    PICOTEST_ASSERT(Col_ListIterNull(it));
}

/* Col_ListIterCompare */
PICOTEST_CASE(listIterCompare_typeCheck_it1, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LISTITER);
    Col_ListIterator it1 = COL_LISTITER_NULL;
    Col_ListIterator it2;
    Col_ListIterFirst(it2, Col_NewVector(1, NULL));
    PICOTEST_ASSERT(Col_ListIterCompare(it1, it2) == 1);
}
PICOTEST_CASE(listIterCompare_typeCheck_it2, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LISTITER);
    Col_ListIterator it1;
    Col_ListIterFirst(it1, Col_NewVector(1, NULL));
    Col_ListIterator it2 = COL_LISTITER_NULL;
    PICOTEST_ASSERT(Col_ListIterCompare(it1, it2) == -1);
}

/* Col_ListIterForward */
PICOTEST_CASE(listIterForward_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LISTITER);
    Col_ListIterator it = COL_LISTITER_NULL;
    PICOTEST_ASSERT(Col_ListIterForward(it, 0) == 0);
    PICOTEST_ASSERT(Col_ListIterNull(it));
}
PICOTEST_CASE(listIterForward_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LISTITER_END);
    Col_ListIterator it;
    Col_ListIterFirst(it, Col_EmptyList());
    PICOTEST_ASSERT(Col_ListIterEnd(it));
    PICOTEST_ASSERT(Col_ListIterForward(it, 1) == 0);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
}

/* Col_ListIterBackward */
PICOTEST_CASE(listIterBackward_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LISTITER);
    Col_ListIterator it = COL_LISTITER_NULL;
    Col_ListIterBackward(it, 0);
    PICOTEST_ASSERT(Col_ListIterNull(it));
}

/* Col_ListIterAt */
PICOTEST_CASE(listIterAt_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LISTITER_END);
    Col_ListIterator it;
    Col_ListIterFirst(it, Col_EmptyList());
    PICOTEST_ASSERT(Col_ListIterEnd(it));
    PICOTEST_ASSERT(Col_ListIterAt(it) == WORD_NIL);
}

/* Col_ListIterNext */
PICOTEST_CASE(listIterNext_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LISTITER);
    Col_ListIterator it = COL_LISTITER_NULL;
    Col_ListIterNext(it);
    PICOTEST_ASSERT(Col_ListIterNull(it));
}
PICOTEST_CASE(listIterNext_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LISTITER_END);
    Col_ListIterator it;
    Col_ListIterFirst(it, Col_EmptyList());
    PICOTEST_ASSERT(Col_ListIterEnd(it));
    Col_ListIterNext(it);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
}

/* Col_ListIterPrevious */
PICOTEST_CASE(listIterPrevious_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LISTITER);
    Col_ListIterator it = COL_LISTITER_NULL;
    Col_ListIterPrevious(it);
    PICOTEST_ASSERT(Col_ListIterNull(it));
}

/*
 * Utilities
 */

#define DECLARE_LIST_DATA(name, len, first, next)                              \
    Col_Word name[len];                                                        \
    name[0] = first;                                                           \
    for (int i = 1; i < len; i++) {                                            \
        name[i] = next;                                                        \
    }
#define NEW_LIST(len, first, next)                                             \
    {                                                                          \
        DECLARE_LIST_DATA(data, len, first, next);                             \
        return Col_NewList(len, data);                                         \
    }

/*
 * Test data
 */

/* List creation */
#define LIST_VECTOR_LEN 10
static Col_Word NEW_LIST_VECTOR()
    NEW_LIST(LIST_VECTOR_LEN, Col_NewCharWord('a'), Col_NewCharWord('a' + i));

/* List operations */
#define SMALL_LIST_LEN 3
static Col_Word SMALL_LIST()
    NEW_LIST(SMALL_LIST_LEN, Col_NewCharWord('a'), Col_NewCharWord('a' + i));

#define SMALL_VOID_LIST() Col_NewList(SMALL_LIST_LEN, NULL)

#define LARGE_LIST_LEN 200
static Col_Word LARGE_LIST()
    NEW_LIST(LARGE_LIST_LEN, Col_NewCharWord('a'), Col_NewCharWord('a' + i));

#define VOID_LIST_LEN 1000
#define VOID_LIST() Col_NewList(VOID_LIST_LEN, NULL)

#define CIRCULAR_LIST_LEN 100
static Col_Word CIRCULAR_LIST_CORE()
    NEW_LIST(CIRCULAR_LIST_LEN, Col_NewCharWord('a'), Col_NewCharWord('a' + i));
#define CIRCULAR_LIST() Col_CircularList(CIRCULAR_LIST_CORE())

#define CYCLIC_LIST_HEAD_LEN 300
#define CYCLIC_LIST_HEAD() Col_NewList(CYCLIC_LIST_HEAD_LEN, NULL)
#define CYCLIC_LIST_TAIL_LEN CIRCULAR_LIST_LEN
#define CYCLIC_LIST_TAIL() CIRCULAR_LIST()
#define CYCLIC_LIST_LEN (CYCLIC_LIST_HEAD_LEN + CYCLIC_LIST_TAIL_LEN)
#define CYCLIC_LIST() Col_ConcatLists(CYCLIC_LIST_HEAD(), CYCLIC_LIST_TAIL())

/*
 * Lists
 */

#include "hooks.h"
#include "colibriFixture.h"

PICOTEST_SUITE(testLists, testListTypeChecks, testEmptyList, testListCreation,
               testListAccessors, testListOperations, testListTraversal,
               testListIteration);

PICOTEST_CASE(testListTypeChecks, colibriFixture) {
    PICOTEST_VERIFY(listLength_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(listLoopLength_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(listDepth_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(listAt_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(sublist_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(concatLists_typeCheck_left(NULL) == 1);
    PICOTEST_VERIFY(concatLists_typeCheck_right(NULL) == 1);
    PICOTEST_VERIFY(repeatList_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(circularList_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(listInsert_typeCheck_into(NULL) == 1);
    PICOTEST_VERIFY(listInsert_typeCheck_list(NULL) == 1);
    PICOTEST_VERIFY(listRemove_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(listReplace_typeCheck_list(NULL) == 1);
    PICOTEST_VERIFY(listReplace_typeCheck_with(NULL) == 1);
    PICOTEST_VERIFY(traverseListChunksN_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(traverseListChunks_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(listIterBegin_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(listIterFirst_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(listIterLast_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(listIterCompare_typeCheck_it1(NULL) == 1);
    PICOTEST_VERIFY(listIterCompare_typeCheck_it2(NULL) == 1);
    PICOTEST_VERIFY(listIterForward_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(listIterBackward_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(listIterNext_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(listIterPrevious_typeCheck(NULL) == 1);
}

/* Empty list */
PICOTEST_SUITE(testEmptyList, testEmptyListConstant, testEmptyListIsImmediate,
               testEmptyListInvariants);

PICOTEST_CASE(testEmptyListConstant, colibriFixture) {
    Col_Word empty = Col_EmptyList();
    PICOTEST_ASSERT(Col_WordType(empty) == (COL_VECTOR | COL_LIST));
}
PICOTEST_CASE(testEmptyListIsImmediate, colibriFixture) {
    PICOTEST_ASSERT(Col_EmptyList() == Col_EmptyList());
}
PICOTEST_CASE(testEmptyListInvariants, colibriFixture) {
    Col_Word empty = Col_EmptyList();
    PICOTEST_ASSERT(Col_ListLength(empty) == 0);
    PICOTEST_ASSERT(Col_ListDepth(empty) == 0);
}

/* List creation */
PICOTEST_SUITE(testListCreation, testNewList);
PICOTEST_SUITE(testNewList, testNewListEmpty, testNewListVector,
               testNewVoidList);
PICOTEST_SUITE(testNewVoidList, testNewVoidListSmall, testNewVoidListBig);

static void checkListEmpty(Col_Word list) {
    PICOTEST_ASSERT(list == Col_EmptyList());
}
PICOTEST_CASE(testNewListEmpty, colibriFixture) {
    checkListEmpty(Col_NewList(0, NULL));
}

static void checkListVector(Col_Word list, size_t len) {
    PICOTEST_ASSERT(Col_WordType(list) == (COL_VECTOR | COL_LIST));
    PICOTEST_ASSERT(Col_ListLength(list) == len);
    PICOTEST_ASSERT(Col_ListDepth(list) == 0);
}

PICOTEST_CASE(testNewListVector, colibriFixture) {
    Col_Word list = NEW_LIST_VECTOR();
    PICOTEST_ASSERT(list != NEW_LIST_VECTOR());
    checkListVector(list, LIST_VECTOR_LEN);
}

static void checkVoidList(Col_Word list, size_t len) {
    PICOTEST_ASSERT(Col_WordType(list) == COL_LIST);
    PICOTEST_ASSERT(Col_ListLength(list) == len);
    PICOTEST_ASSERT(Col_ListDepth(list) == 0);
}
PICOTEST_CASE(testNewVoidListSmall, colibriFixture) {
    Col_Word list = Col_NewList(LIST_VECTOR_LEN, NULL);
    PICOTEST_ASSERT(list == Col_NewList(LIST_VECTOR_LEN, NULL));
    checkVoidList(list, LIST_VECTOR_LEN);
}

static void checkList(Col_Word list, size_t len, size_t depth) {
    PICOTEST_ASSERT(Col_WordType(list) == COL_LIST);
    PICOTEST_ASSERT(Col_ListLength(list) == len);
    PICOTEST_ASSERT(Col_ListDepth(list) == depth);
}
PICOTEST_CASE(testNewVoidListBig, colibriFixture) {
    Col_Word list = Col_NewList(SIZE_MAX, NULL);
    PICOTEST_ASSERT(list != Col_NewList(SIZE_MAX, NULL));
    PICOTEST_ASSERT(Col_WordType(list) == COL_LIST);
    PICOTEST_ASSERT(Col_ListLength(list) == SIZE_MAX);
    PICOTEST_ASSERT(Col_ListDepth(list) > 1);
}

/* List accessors */
PICOTEST_SUITE(testListAccessors, testListLength, testListLoopLength,
               testListDepth, testListAt);

PICOTEST_SUITE(testListLength, testListLengthOfEmptyListIsZero,
               testListLengthOfVector, testListLengthOfFlatList,
               testListLengthOfVoidList, testListLengthOfSublistIsRangeLength,
               testListLengthOfConcatListIsSumOfArmLengths,
               testListLengthOfCircularListIsSameAsCore);
PICOTEST_CASE(testListLengthOfEmptyListIsZero, colibriFixture) {
    PICOTEST_ASSERT(Col_ListLength(Col_EmptyList()) == 0);
}
PICOTEST_CASE(testListLengthOfVector, colibriFixture) {
    PICOTEST_ASSERT(Col_ListLength(Col_NewVector(100, NULL)) == 100);
}
PICOTEST_CASE(testListLengthOfFlatList, colibriFixture) {
    PICOTEST_ASSERT(Col_ListLength(SMALL_LIST()) == SMALL_LIST_LEN);
    PICOTEST_ASSERT(Col_ListLength(LARGE_LIST()) == LARGE_LIST_LEN);
}
PICOTEST_CASE(testListLengthOfVoidList, colibriFixture) {
    PICOTEST_ASSERT(Col_ListLength(VOID_LIST()) == VOID_LIST_LEN);
}
PICOTEST_CASE(testListLengthOfSublistIsRangeLength, colibriFixture) {
    Col_Word list = LARGE_LIST();
    Col_Word sublist = Col_Sublist(list, 1, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListLength(sublist) == LARGE_LIST_LEN - 1);
}
PICOTEST_CASE(testListLengthOfConcatListIsSumOfArmLengths, colibriFixture) {
    Col_Word left = LARGE_LIST(), right = SMALL_LIST();
    Col_Word concat = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_ListLength(concat) ==
                    Col_ListLength(left) + Col_ListLength(right));
}
PICOTEST_CASE(testListLengthOfCircularListIsSameAsCore, colibriFixture) {
    Col_Word core = SMALL_LIST();
    Col_Word list = Col_CircularList(core);
    PICOTEST_ASSERT(Col_ListLength(list) == Col_ListLength(core));
}

PICOTEST_SUITE(testListLoopLength, testListLoopLengthOfEmptyListIsZero,
               testListLoopLengthOfVectorIsZero,
               testListLoopLengthOfFlatListIsZero,
               testListLoopLengthOfVoidListIsZero,
               testListLoopLengthOfSublistIsZero,
               testListLoopLengthOfConcatListIsSameAsRightArm,
               testListLoopLengthOfCircularListIsCoreLength);
PICOTEST_CASE(testListLoopLengthOfEmptyListIsZero, colibriFixture) {
    PICOTEST_ASSERT(Col_ListLoopLength(Col_EmptyList()) == 0);
}
PICOTEST_CASE(testListLoopLengthOfVectorIsZero, colibriFixture) {
    PICOTEST_ASSERT(Col_ListLoopLength(Col_NewVector(100, NULL)) == 0);
}
PICOTEST_CASE(testListLoopLengthOfFlatListIsZero, colibriFixture) {
    PICOTEST_ASSERT(Col_ListLoopLength(SMALL_LIST()) == 0);
    PICOTEST_ASSERT(Col_ListLoopLength(LARGE_LIST()) == 0);
}
PICOTEST_CASE(testListLoopLengthOfVoidListIsZero, colibriFixture) {
    PICOTEST_ASSERT(Col_ListLoopLength(VOID_LIST()) == 0);
}
static void checkListLoopLengthOfSublistIsZero(Col_Word list) {
    Col_Word sublist = Col_Sublist(list, 1, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListLoopLength(sublist) == 0);
}
PICOTEST_CASE(testListLoopLengthOfSublistIsZero, colibriFixture) {
    checkListLoopLengthOfSublistIsZero(LARGE_LIST());
    checkListLoopLengthOfSublistIsZero(CIRCULAR_LIST());
}
static void checkListLoopLengthOfConcatListIsSameAsRightArm(Col_Word left,
                                                            Col_Word right) {
    Col_Word concat = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_ListLoopLength(concat) == Col_ListLoopLength(right));
}
PICOTEST_CASE(testListLoopLengthOfConcatListIsSameAsRightArm, colibriFixture) {
    checkListLoopLengthOfConcatListIsSameAsRightArm(LARGE_LIST(), SMALL_LIST());
    checkListLoopLengthOfConcatListIsSameAsRightArm(LARGE_LIST(),
                                                    CIRCULAR_LIST());
    checkListLoopLengthOfConcatListIsSameAsRightArm(LARGE_LIST(),
                                                    CYCLIC_LIST());
}
PICOTEST_CASE(testListLoopLengthOfCircularListIsCoreLength, colibriFixture) {
    Col_Word core = SMALL_LIST();
    Col_Word list = Col_CircularList(core);
    PICOTEST_ASSERT(Col_ListLoopLength(list) == Col_ListLength(core));
}

PICOTEST_SUITE(testListDepth, testListDepthOfEmptyListIsZero,
               testListDepthOfVectorIsZero, testListDepthOfFlatListIsZero,
               testListDepthOfVoidListIsZero,
               testListDepthOfSublistIsSameAsSource,
               testListDepthOfConcatListIsOne,
               testListDepthOfCircularListIsSameAsCore);
PICOTEST_CASE(testListDepthOfEmptyListIsZero, colibriFixture) {
    PICOTEST_ASSERT(Col_ListDepth(Col_EmptyList()) == 0);
}
PICOTEST_CASE(testListDepthOfVectorIsZero, colibriFixture) {
    PICOTEST_ASSERT(Col_ListDepth(Col_NewVector(100, NULL)) == 0);
}
PICOTEST_CASE(testListDepthOfFlatListIsZero, colibriFixture) {
    PICOTEST_ASSERT(Col_ListDepth(SMALL_LIST()) == 0);
    PICOTEST_ASSERT(Col_ListDepth(LARGE_LIST()) == 0);
}
PICOTEST_CASE(testListDepthOfVoidListIsZero, colibriFixture) {
    PICOTEST_ASSERT(Col_ListDepth(VOID_LIST()) == 0);
}
static void checkListDepthOfSublistIsSameAsSource(Col_Word source) {
    Col_Word sublist = Col_Sublist(source, 1, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListDepth(sublist) == Col_ListDepth(source));
}
PICOTEST_CASE(testListDepthOfSublistIsSameAsSource, colibriFixture) {
    checkListDepthOfSublistIsSameAsSource(LARGE_LIST());
    checkListDepthOfSublistIsSameAsSource(
        Col_ConcatLists(LARGE_LIST(), SMALL_LIST()));
    checkListDepthOfSublistIsSameAsSource(Col_RepeatList(LARGE_LIST(), 10));
}
PICOTEST_CASE(testListDepthOfConcatListIsOne, colibriFixture) {
    Col_Word left = LARGE_LIST(), right = SMALL_LIST();
    Col_Word concat = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_ListDepth(concat) == 1);
}
static void checkListDepthOfCircularListIsSameAsCore(Col_Word core) {
    Col_Word list = Col_CircularList(core);
    PICOTEST_ASSERT(Col_ListDepth(list) == Col_ListDepth(core));
}
PICOTEST_CASE(testListDepthOfCircularListIsSameAsCore, colibriFixture) {
    checkListDepthOfCircularListIsSameAsCore(LARGE_LIST());
    checkListDepthOfCircularListIsSameAsCore(
        Col_ConcatLists(LARGE_LIST(), SMALL_LIST()));
    checkListDepthOfCircularListIsSameAsCore(Col_RepeatList(LARGE_LIST(), 10));
}

PICOTEST_SUITE(testListAt, testListAtVoidListIsNil, testListAtEndIsNil);
PICOTEST_CASE(testListAtVoidListIsNil, colibriFixture) {
    PICOTEST_ASSERT(Col_ListAt(VOID_LIST(), 0) == WORD_NIL);
}
PICOTEST_CASE(testListAtEndIsNil, colibriFixture) {
    PICOTEST_ASSERT(Col_ListAt(SMALL_LIST(), SMALL_LIST_LEN) == WORD_NIL);
}

/* List operations */
PICOTEST_SUITE(testListOperations, testSublist, testConcatLists,
               testConcatListsA, testConcatListsNV, testConcatListsV,
               testRepeatList, testCircularList, testListInsert, testListRemove,
               testListReplace);

// TODO compare original/sublist elements
PICOTEST_SUITE(testSublist, testSublistOfEmptyListIsEmpty,
               testSublistOfEmptyRangeIsEmpty,
               testSublistOfWholeRangeIsIdentity,
               testSublistOfVoidListIsNewVoidList,
               testSublistOfFlatListIsNewList, testSublistOfCircularList,
               testSublistOfCyclicList, testSublistOfSublistIsNewList,
               testSublistOfConcatList);

static void checkSublist(Col_Word list, size_t first, size_t last) {
    Col_Word sublist = Col_Sublist(list, first, last);
    PICOTEST_ASSERT(Col_ListLength(sublist) == last - first + 1);
    PICOTEST_ASSERT(Col_ListLoopLength(sublist) == 0);
    PICOTEST_ASSERT(Col_ListAt(sublist, 0) == Col_ListAt(list, first));
    PICOTEST_ASSERT(Col_ListAt(sublist, last - first) ==
                    Col_ListAt(list, last));
}

PICOTEST_CASE(testSublistOfEmptyListIsEmpty, colibriFixture) {
    checkListEmpty(Col_Sublist(Col_EmptyList(), 0, 0));
}
PICOTEST_CASE(testSublistOfEmptyRangeIsEmpty, colibriFixture) {
    Col_Word list = SMALL_LIST();
    checkListEmpty(Col_Sublist(list, 1, 0));
    checkListEmpty(Col_Sublist(list, 2, 1));
    PICOTEST_ASSERT(Col_Sublist(list, SMALL_LIST_LEN, SMALL_LIST_LEN) ==
                    Col_EmptyList());
    checkListEmpty(Col_Sublist(list, SIZE_MAX, 2));
    checkListEmpty(Col_Sublist(list, SIZE_MAX, SIZE_MAX));
}
PICOTEST_CASE(testSublistOfWholeRangeIsIdentity, colibriFixture) {
    Col_Word list = SMALL_LIST();
    PICOTEST_ASSERT(Col_Sublist(list, 0, Col_ListLength(list)) == list);
    PICOTEST_ASSERT(Col_Sublist(list, 0, SIZE_MAX) == list);
}
PICOTEST_CASE(testSublistOfVoidListIsNewVoidList, colibriFixture) {
    Col_Word list = VOID_LIST();

    Col_Word sublist = Col_Sublist(list, 1, SIZE_MAX);
    PICOTEST_ASSERT(sublist == Col_Sublist(list, 1, SIZE_MAX));
    checkVoidList(sublist, VOID_LIST_LEN - 1);
    checkSublist(list, 1, VOID_LIST_LEN - 1);
}
PICOTEST_CASE(testSublistOfFlatListIsNewList, colibriFixture) {
    Col_Word list = LARGE_LIST();

    Col_Word sublist = Col_Sublist(list, 1, SIZE_MAX);
    PICOTEST_ASSERT(sublist != Col_Sublist(list, 1, SIZE_MAX));
    checkList(sublist, LARGE_LIST_LEN - 1, 0);
    checkSublist(list, 1, LARGE_LIST_LEN - 1);
}
PICOTEST_CASE(testSublistOfCircularList, colibriFixture) {
    checkSublist(CIRCULAR_LIST(), 1, CIRCULAR_LIST_LEN - 2);
    checkSublist(CIRCULAR_LIST(), 1, CIRCULAR_LIST_LEN - 1);
    checkSublist(CIRCULAR_LIST(), 1, CIRCULAR_LIST_LEN + 1);
    checkSublist(CIRCULAR_LIST(), 1, CIRCULAR_LIST_LEN + 2);
    checkSublist(CIRCULAR_LIST(), 1, CIRCULAR_LIST_LEN * 2 - 2);
    checkSublist(CIRCULAR_LIST(), 1, CIRCULAR_LIST_LEN * 2 - 1);
    checkSublist(CIRCULAR_LIST(), 1, CIRCULAR_LIST_LEN * 2 + 1);
    checkSublist(CIRCULAR_LIST(), 1, CIRCULAR_LIST_LEN * 2 + 2);
    checkSublist(CIRCULAR_LIST(), SIZE_MAX - CIRCULAR_LIST_LEN - 1, SIZE_MAX);
    checkSublist(CIRCULAR_LIST(), SIZE_MAX - CIRCULAR_LIST_LEN + 1, SIZE_MAX);
    checkSublist(CIRCULAR_LIST(), 0, SIZE_MAX - 2);
    checkSublist(CIRCULAR_LIST(), 0, SIZE_MAX - 1);
    checkSublist(CIRCULAR_LIST(), 1, SIZE_MAX);
}
PICOTEST_CASE(testSublistOfCyclicList, colibriFixture) {
    checkSublist(CYCLIC_LIST(), 1, CYCLIC_LIST_HEAD_LEN - 2);
    checkSublist(CYCLIC_LIST(), 1, CYCLIC_LIST_HEAD_LEN - 1);
    checkSublist(CYCLIC_LIST(), 1, CYCLIC_LIST_HEAD_LEN + 1);
    checkSublist(CYCLIC_LIST(), 1, CYCLIC_LIST_HEAD_LEN + 2);
    checkSublist(CYCLIC_LIST(), 1,
                 CYCLIC_LIST_HEAD_LEN + CYCLIC_LIST_TAIL_LEN - 2);
    checkSublist(CYCLIC_LIST(), 1,
                 CYCLIC_LIST_HEAD_LEN + CYCLIC_LIST_TAIL_LEN - 1);
    checkSublist(CYCLIC_LIST(), 1,
                 CYCLIC_LIST_HEAD_LEN + CYCLIC_LIST_TAIL_LEN + 1);
    checkSublist(CYCLIC_LIST(), 1,
                 CYCLIC_LIST_HEAD_LEN + CYCLIC_LIST_TAIL_LEN + 2);
    checkSublist(CYCLIC_LIST(), CYCLIC_LIST_HEAD_LEN + 1,
                 CYCLIC_LIST_HEAD_LEN + CYCLIC_LIST_TAIL_LEN - 2);
    checkSublist(CYCLIC_LIST(), CYCLIC_LIST_HEAD_LEN + 1,
                 CYCLIC_LIST_HEAD_LEN + CYCLIC_LIST_TAIL_LEN - 1);
    checkSublist(CYCLIC_LIST(), CYCLIC_LIST_HEAD_LEN + 1,
                 CYCLIC_LIST_HEAD_LEN + CYCLIC_LIST_TAIL_LEN * 2 - 2);
    checkSublist(CYCLIC_LIST(), CYCLIC_LIST_HEAD_LEN + 1,
                 CYCLIC_LIST_HEAD_LEN + CYCLIC_LIST_TAIL_LEN * 2 - 1);
    checkSublist(CYCLIC_LIST(), CYCLIC_LIST_HEAD_LEN + 1, SIZE_MAX - 2);
    checkSublist(CYCLIC_LIST(), CYCLIC_LIST_HEAD_LEN + 1, SIZE_MAX - 1);
    checkSublist(CYCLIC_LIST(), CYCLIC_LIST_HEAD_LEN + 1, SIZE_MAX);
}

PICOTEST_CASE(testSublistOfSublistIsNewList, colibriFixture) {
    Col_Word list = Col_Sublist(LARGE_LIST(), 1, SIZE_MAX);

    Col_Word sublist = Col_Sublist(list, 1, SIZE_MAX);
    PICOTEST_ASSERT(sublist != Col_Sublist(list, 1, SIZE_MAX));
    checkList(sublist, LARGE_LIST_LEN - 2, 0);
    checkSublist(list, 1, LARGE_LIST_LEN - 2);
}

PICOTEST_SUITE(testSublistOfConcatList, testSublistOfConcatListArms,
               testShortSublistOfConcatList);
PICOTEST_CASE(testSublistOfConcatListArms, colibriFixture) {
    Col_Word left = LARGE_LIST(), right = SMALL_LIST();
    PICOTEST_ASSERT(left != right);
    Col_Word list = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_Sublist(list, 0, LARGE_LIST_LEN - 1) == left);
    PICOTEST_ASSERT(Col_Sublist(list, LARGE_LIST_LEN, SIZE_MAX) == right);
}
PICOTEST_SUITE(testShortSublistOfConcatList, testMergeableSublists,
               testUnmergeableSublists);
static void checkMergedSublist(Col_Word left, Col_Word right, size_t delta,
                               int isVoid) {
    size_t leftLen = Col_ListLength(left), rightLen = Col_ListLength(right);
    Col_Word list = Col_ConcatLists(left, right);
    Col_Word sublist = Col_Sublist(list, leftLen - delta, leftLen + delta);
    if (isVoid) {
        checkVoidList(sublist, delta * 2 + 1);
    } else {
        checkList(list, leftLen + rightLen, 1);
        checkListVector(sublist, delta * 2 + 1);
    }
}
PICOTEST_CASE(testMergeableSublists, colibriFixture) {
    checkMergedSublist(LARGE_LIST(), SMALL_LIST(), 2, 0);
    checkMergedSublist(SMALL_LIST(), LARGE_LIST(), 2, 0);
    checkMergedSublist(LARGE_LIST(), LARGE_LIST(), 2, 0);
    checkMergedSublist(VOID_LIST(), VOID_LIST(), 2, 1);
    checkMergedSublist(LARGE_LIST(), VOID_LIST(), 2, 0);
}
static void checkUnmergedSublist(Col_Word left, Col_Word right, size_t delta) {
    size_t leftLen = Col_ListLength(left), rightLen = Col_ListLength(right);
    Col_Word list = Col_ConcatLists(left, right);
    Col_Word sublist = Col_Sublist(list, leftLen - delta, leftLen + delta);
    checkList(list, leftLen + rightLen, 1);
    checkList(sublist, delta * 2 + 1, 1);
}
PICOTEST_CASE(testUnmergeableSublists, colibriFixture) {
    checkUnmergedSublist(LARGE_LIST(), LARGE_LIST(), 10);
    checkUnmergedSublist(LARGE_LIST(), VOID_LIST(), 10);
    checkUnmergedSublist(VOID_LIST(), LARGE_LIST(), 10);
}

PICOTEST_SUITE(testConcatLists, testConcatListsErrors,
               testConcatListWithEmptyIsIdentity, testConcatShortLists,
               testConcatFlatListsIsNewList,
               testConcatAdjacentSublistsIsOriginalList,
               testConcatCircularLists, testConcatCyclicLists,
               testConcatListBalancing);

PICOTEST_SUITE(testConcatListsErrors, testConcatListsTooLarge);
PICOTEST_CASE(testConcatListsTooLarge, colibriFixture) {
    PICOTEST_ASSERT(concatLists_valueCheck_length(NULL) == 1);
}

PICOTEST_CASE(testConcatListWithEmptyIsIdentity, colibriFixture) {
    Col_Word list = SMALL_LIST();
    PICOTEST_ASSERT(Col_ConcatLists(list, Col_EmptyList()) == list);
    PICOTEST_ASSERT(Col_ConcatLists(Col_EmptyList(), list) == list);
}
PICOTEST_SUITE(testConcatShortLists, testConcatMergeableLists,
               testConcatUnmergeableLists);
static void checkMergedConcatList(Col_Word left, Col_Word right, int isVoid) {
    size_t leftLen = Col_ListLength(left), rightLen = Col_ListLength(right);
    Col_Word list = Col_ConcatLists(left, right);
    if (isVoid) {
        checkVoidList(list, leftLen + rightLen);
    } else {
        checkListVector(list, leftLen + rightLen);
    }
}
PICOTEST_CASE(testConcatMergeableLists, colibriFixture) {
    checkMergedConcatList(SMALL_LIST(), SMALL_LIST(), 0);
    checkMergedConcatList(SMALL_LIST(), SMALL_VOID_LIST(), 0);
    checkMergedConcatList(SMALL_VOID_LIST(), SMALL_LIST(), 0);
    checkMergedConcatList(SMALL_VOID_LIST(), SMALL_VOID_LIST(), 1);
}

static void checkUnmergedConcatList(Col_Word left, Col_Word right,
                                    size_t depth) {
    size_t leftLen = Col_ListLength(left), rightLen = Col_ListLength(right);
    Col_Word list = Col_ConcatLists(left, right);
    checkList(list, leftLen + rightLen, depth);
}
PICOTEST_CASE(testConcatUnmergeableLists, colibriFixture) {
    checkUnmergedConcatList(SMALL_LIST(), LARGE_LIST(), 1);
    checkUnmergedConcatList(SMALL_LIST(), VOID_LIST(), 1);
    checkUnmergedConcatList(LARGE_LIST(), SMALL_VOID_LIST(), 1);
    checkUnmergedConcatList(VOID_LIST(), LARGE_LIST(), 1);
    checkUnmergedConcatList(LARGE_LIST(), LARGE_LIST(), 1);
    checkUnmergedConcatList(LARGE_LIST(), VOID_LIST(), 1);
}

PICOTEST_CASE(testConcatFlatListsIsNewList, colibriFixture) {
    Col_Word list = LARGE_LIST();
    Col_Word concatList = Col_ConcatLists(list, list);
    PICOTEST_ASSERT(concatList != Col_ConcatLists(list, list));
    checkList(concatList, LARGE_LIST_LEN * 2, 1);
}
PICOTEST_CASE(testConcatAdjacentSublistsIsOriginalList, colibriFixture) {
    Col_Word list = LARGE_LIST();
    size_t split = LARGE_LIST_LEN / 2;
    Col_Word left = Col_Sublist(list, 0, split - 1);
    Col_Word right = Col_Sublist(list, split, SIZE_MAX);
    PICOTEST_ASSERT(Col_ConcatLists(left, right) == list);
}

PICOTEST_SUITE(testConcatCircularLists, testConcatLeftCircularListIsLeft,
               testConcatRightCircularListIsCyclic);
PICOTEST_CASE(testConcatLeftCircularListIsLeft, colibriFixture) {
    Col_Word left = CIRCULAR_LIST(), right = LARGE_LIST();
    Col_Word concat = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(concat == left);
}
static void checkCyclicList(Col_Word list, size_t len, size_t loopLen) {
    PICOTEST_ASSERT(Col_ListLength(list) == len);
    PICOTEST_ASSERT(Col_ListLoopLength(list) == loopLen);
}
PICOTEST_CASE(testConcatRightCircularListIsCyclic, colibriFixture) {
    Col_Word left = LARGE_LIST(), right = CIRCULAR_LIST();
    Col_Word concat = Col_ConcatLists(left, right);
    checkCyclicList(concat, LARGE_LIST_LEN + CIRCULAR_LIST_LEN,
                    CIRCULAR_LIST_LEN);
}

PICOTEST_SUITE(testConcatCyclicLists, testConcatLeftCyclicListIsLeft,
               testConcatRightCyclicListIsCyclic);
PICOTEST_CASE(testConcatLeftCyclicListIsLeft, colibriFixture) {
    Col_Word left = CYCLIC_LIST(), right = LARGE_LIST();
    Col_Word concat = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(concat == left);
}
PICOTEST_CASE(testConcatRightCyclicListIsCyclic, colibriFixture) {
    Col_Word left = LARGE_LIST(), right = CYCLIC_LIST();
    Col_Word concat = Col_ConcatLists(left, right);
    checkCyclicList(concat, LARGE_LIST_LEN + CYCLIC_LIST_LEN,
                    CYCLIC_LIST_TAIL_LEN);
}

PICOTEST_SUITE(testConcatListBalancing,
               testConcatBalancedListBranchesAreNotRebalanced,
               testConcatCircularListsAreNotRebalanced,
               testConcatCyclicListHeadsAreRebalanced,
               testConcatInbalancedLeftOuterListBranchesAreRotated,
               testConcatInbalancedRightOuterListBranchesAreRotated,
               testConcatInbalancedLeftInnerListBranchesAreSplit,
               testConcatInbalancedRightInnerListBranchesAreSplit,
               testConcatInbalancedSublistListBranchesAreSplit);
PICOTEST_CASE(testConcatBalancedListBranchesAreNotRebalanced, colibriFixture) {
    Col_Word leaf[8];
    for (int i = 0; i < 8; i++) {
        leaf[i] = LARGE_LIST();
    }

    Col_Word left1 = Col_ConcatLists(leaf[0], leaf[1]);
    Col_Word left2 = Col_ConcatLists(leaf[2], leaf[3]);
    PICOTEST_ASSERT(Col_ListDepth(left1) == 1);
    PICOTEST_ASSERT(Col_ListDepth(left2) == 1);

    Col_Word left = Col_ConcatLists(left1, left2);
    PICOTEST_ASSERT(Col_ListDepth(left) == 2);
    PICOTEST_ASSERT(Col_ListDepth(Col_ConcatLists(left1, leaf[0])) == 2);
    PICOTEST_ASSERT(Col_ListDepth(Col_ConcatLists(leaf[0], left2)) == 2);
    for (int i = 0; i < 4; i++) {
        PICOTEST_ASSERT(Col_Sublist(left, LARGE_LIST_LEN * i,
                                    LARGE_LIST_LEN * (i + 1) - 1) == leaf[i]);
    }

    Col_Word right1 = Col_ConcatLists(leaf[4], leaf[5]);
    Col_Word right2 = Col_ConcatLists(leaf[6], leaf[7]);
    PICOTEST_ASSERT(Col_ListDepth(right1) == 1);
    PICOTEST_ASSERT(Col_ListDepth(right2) == 1);

    Col_Word right = Col_ConcatLists(right1, right2);
    PICOTEST_ASSERT(Col_ListDepth(right) == 2);
    PICOTEST_ASSERT(Col_ListDepth(Col_ConcatLists(right1, leaf[0])) == 2);
    PICOTEST_ASSERT(Col_ListDepth(Col_ConcatLists(leaf[0], right2)) == 2);
    for (int i = 0; i < 4; i++) {
        PICOTEST_ASSERT(Col_Sublist(right, LARGE_LIST_LEN * i,
                                    LARGE_LIST_LEN * (i + 1) - 1) ==
                        leaf[i + 4]);
    }

    Col_Word list = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_ListDepth(list) == 3);
    for (int i = 0; i < 8; i++) {
        PICOTEST_ASSERT(Col_Sublist(list, LARGE_LIST_LEN * i,
                                    LARGE_LIST_LEN * (i + 1) - 1) == leaf[i]);
    }
}
PICOTEST_CASE(testConcatCircularListsAreNotRebalanced, colibriFixture) {
    Col_Word left = Col_RepeatList(LARGE_LIST(), 10);
    Col_Word right = CIRCULAR_LIST();
    PICOTEST_ASSERT(Col_ListDepth(left) == 4);
    PICOTEST_ASSERT(Col_ListDepth(right) == 0);
    PICOTEST_ASSERT(Col_ListDepth(Col_ConcatLists(left, right)) == 5);
}
PICOTEST_CASE(testConcatCyclicListHeadsAreRebalanced, colibriFixture) {
    Col_Word left = Col_RepeatList(LARGE_LIST(), 10);
    Col_Word right = CYCLIC_LIST();
    Col_Word head = CYCLIC_LIST_HEAD();
    PICOTEST_ASSERT(Col_ListDepth(left) == 4);
    PICOTEST_ASSERT(Col_ListDepth(right) == 1);
    PICOTEST_ASSERT(Col_ListDepth(head) == 0);
    PICOTEST_ASSERT(Col_ListDepth(Col_ConcatLists(left, head)) == 4);
    PICOTEST_ASSERT(Col_ListDepth(Col_ConcatLists(left, right)) == 5);
}
PICOTEST_CASE(testConcatInbalancedLeftOuterListBranchesAreRotated,
              colibriFixture) {
    Col_Word leaf[4];
    for (int i = 0; i < 4; i++) {
        leaf[i] = LARGE_LIST();
    }

    Col_Word left1 = Col_ConcatLists(leaf[0], leaf[1]);
    Col_Word left2 = leaf[2];
    PICOTEST_ASSERT(Col_ListDepth(left1) == 1);
    PICOTEST_ASSERT(Col_ListDepth(left2) == 0);

    Col_Word left = Col_ConcatLists(left1, left2);
    PICOTEST_ASSERT(Col_ListDepth(left) == 2);

    Col_Word right = leaf[3];
    PICOTEST_ASSERT(Col_ListDepth(right) == 0);

    Col_Word list = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_ListDepth(list) == 2);
    for (int i = 0; i < 4; i++) {
        PICOTEST_ASSERT(Col_Sublist(list, LARGE_LIST_LEN * i,
                                    LARGE_LIST_LEN * (i + 1) - 1) == leaf[i]);
    }
}
PICOTEST_CASE(testConcatInbalancedRightOuterListBranchesAreRotated,
              colibriFixture) {
    Col_Word leaf[4];
    for (int i = 0; i < 4; i++) {
        leaf[i] = LARGE_LIST();
    }

    Col_Word left = leaf[0];
    PICOTEST_ASSERT(Col_ListDepth(left) == 0);

    Col_Word right1 = leaf[1];
    Col_Word right2 = Col_ConcatLists(leaf[2], leaf[3]);
    PICOTEST_ASSERT(Col_ListDepth(right1) == 0);
    PICOTEST_ASSERT(Col_ListDepth(right2) == 1);

    Col_Word right = Col_ConcatLists(right1, right2);
    PICOTEST_ASSERT(Col_ListDepth(right) == 2);

    Col_Word list = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_ListDepth(list) == 2);
    for (int i = 0; i < 4; i++) {
        PICOTEST_ASSERT(Col_Sublist(list, LARGE_LIST_LEN * i,
                                    LARGE_LIST_LEN * (i + 1) - 1) == leaf[i]);
    }
}
PICOTEST_CASE(testConcatInbalancedLeftInnerListBranchesAreSplit,
              colibriFixture) {
    Col_Word leaf[4];
    for (int i = 0; i < 4; i++) {
        leaf[i] = LARGE_LIST();
    }

    Col_Word left1 = leaf[0];
    Col_Word left2 = Col_ConcatLists(leaf[1], leaf[2]);
    PICOTEST_ASSERT(Col_ListDepth(left1) == 0);
    PICOTEST_ASSERT(Col_ListDepth(left2) == 1);

    Col_Word left = Col_ConcatLists(left1, left2);
    PICOTEST_ASSERT(Col_ListDepth(left) == 2);

    Col_Word right = leaf[3];
    PICOTEST_ASSERT(Col_ListDepth(right) == 0);

    Col_Word list = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_ListDepth(list) == 2);
    for (int i = 0; i < 4; i++) {
        PICOTEST_ASSERT(Col_Sublist(list, LARGE_LIST_LEN * i,
                                    LARGE_LIST_LEN * (i + 1) - 1) == leaf[i]);
    }
}
PICOTEST_CASE(testConcatInbalancedRightInnerListBranchesAreSplit,
              colibriFixture) {
    Col_Word leaf[4];
    for (int i = 0; i < 4; i++) {
        leaf[i] = LARGE_LIST();
    }

    Col_Word left = leaf[0];
    PICOTEST_ASSERT(Col_ListDepth(left) == 0);

    Col_Word right1 = Col_ConcatLists(leaf[1], leaf[2]);
    Col_Word right2 = leaf[3];
    PICOTEST_ASSERT(Col_ListDepth(right1) == 1);
    PICOTEST_ASSERT(Col_ListDepth(right2) == 0);

    Col_Word right = Col_ConcatLists(right1, right2);
    PICOTEST_ASSERT(Col_ListDepth(right) == 2);

    Col_Word list = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_ListDepth(list) == 2);
    for (int i = 0; i < 4; i++) {
        PICOTEST_ASSERT(Col_Sublist(list, LARGE_LIST_LEN * i,
                                    LARGE_LIST_LEN * (i + 1) - 1) == leaf[i]);
    }
}
PICOTEST_CASE(testConcatInbalancedSublistListBranchesAreSplit, colibriFixture) {
    Col_Word leaf = LARGE_LIST();
    Col_Word node1 = Col_ConcatLists(leaf, leaf);
    Col_Word node2 = Col_ConcatLists(node1, node1);
    PICOTEST_ASSERT(Col_ListDepth(leaf) == 0);
    PICOTEST_ASSERT(Col_ListDepth(node1) == 1);
    PICOTEST_ASSERT(Col_ListDepth(node2) == 2);
    PICOTEST_ASSERT(Col_Sublist(node2, Col_ListLength(node1), SIZE_MAX) ==
                    node1);
    Col_Word sublist = Col_Sublist(node2, 1, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListDepth(sublist) == 2);
    PICOTEST_ASSERT(Col_Sublist(sublist, Col_ListLength(node1) - 1, SIZE_MAX) ==
                    node1);

    Col_Word list = Col_ConcatLists(sublist, leaf);
    PICOTEST_ASSERT(Col_Sublist(list, 0, Col_ListLength(sublist) - 1) !=
                    sublist);
    PICOTEST_ASSERT(Col_Sublist(list, Col_ListLength(node1) - 1, SIZE_MAX) !=
                    node1);
}

PICOTEST_SUITE(testConcatListsA, testConcatListsAErrors, testConcatListsAOne,
               testConcatListsATwo, testConcatListsARecurse,
               testConcatListsACyclic);

PICOTEST_SUITE(testConcatListsAErrors, testConcatListsAZero);
PICOTEST_CASE(testConcatListsAZero, colibriFixture) {
    PICOTEST_ASSERT(concatListsA_valueCheck_number(NULL) == 1);
}
PICOTEST_CASE(testConcatListsAOne, colibriFixture) {
    Col_Word lists[] = {LARGE_LIST()};
    Col_Word list = Col_ConcatListsA(1, lists);
    PICOTEST_ASSERT(list == lists[0]);
}
PICOTEST_CASE(testConcatListsATwo, colibriFixture) {
    Col_Word lists[] = {LARGE_LIST(), LARGE_LIST()};
    Col_Word list = Col_ConcatListsA(2, lists);
    checkList(list, LARGE_LIST_LEN * 2, 1);
}
PICOTEST_CASE(testConcatListsARecurse, colibriFixture) {
    Col_Word lists[] = {LARGE_LIST(), SMALL_LIST(), LARGE_LIST()};
    Col_Word list = Col_ConcatListsA(3, lists);
    checkList(list, LARGE_LIST_LEN * 2 + SMALL_LIST_LEN, 2);
}
PICOTEST_CASE(testConcatListsACyclic, colibriFixture) {
    Col_Word lists[] = {CYCLIC_LIST(), SMALL_LIST(), LARGE_LIST()};
    Col_Word list = Col_ConcatListsA(3, lists);
    PICOTEST_ASSERT(list == lists[0]);
}

PICOTEST_SUITE(testConcatListsNV, testConcatListsNVErrors);

PICOTEST_SUITE(testConcatListsNVErrors, testConcatListsNVZero);
PICOTEST_CASE(testConcatListsNVZero, colibriFixture) {
    PICOTEST_ASSERT(concatListsNV_valueCheck_number(NULL) == 1);
}

PICOTEST_SUITE(testConcatListsV, testConcatListsVOne, testConcatListsVTwo,
               testConcatListsVRecurse);

PICOTEST_CASE(testConcatListsVOne, colibriFixture) {
    Col_Word leaf = LARGE_LIST();
    Col_Word list = Col_ConcatListsV(leaf);
    PICOTEST_ASSERT(list == leaf);
}
PICOTEST_CASE(testConcatListsVTwo, colibriFixture) {
    Col_Word list = Col_ConcatListsV(LARGE_LIST(), LARGE_LIST());
    checkList(list, LARGE_LIST_LEN * 2, 1);
}
PICOTEST_CASE(testConcatListsVRecurse, colibriFixture) {
    Col_Word list = Col_ConcatListsV(LARGE_LIST(), SMALL_LIST(), LARGE_LIST());
    checkList(list, LARGE_LIST_LEN * 2 + SMALL_LIST_LEN, 2);
}

PICOTEST_SUITE(testRepeatList, testRepeatListErrors, testRepeatListZeroIsEmpty,
               testRepeatListOnceIsIdentity, testRepeatListTwiceIsConcatSelf,
               testRepeatCircularListIsIdentity, testRepeatCyclicListIsIdentity,
               testRepeatListRecurse, testRepeatEmptyList, testRepeatListMax);

PICOTEST_SUITE(testRepeatListErrors, testRepeatListTooLarge);
PICOTEST_CASE(testRepeatListTooLarge, colibriFixture) {
    PICOTEST_ASSERT(repeatList_valueCheck_length(NULL) == 1);
}

PICOTEST_CASE(testRepeatListZeroIsEmpty, colibriFixture) {
    Col_Word list = LARGE_LIST();
    checkListEmpty(Col_RepeatList(list, 0));
}
PICOTEST_CASE(testRepeatListOnceIsIdentity, colibriFixture) {
    Col_Word list = LARGE_LIST();
    PICOTEST_ASSERT(Col_RepeatList(list, 1) == list);
}
PICOTEST_CASE(testRepeatListTwiceIsConcatSelf, colibriFixture) {
    Col_Word list = LARGE_LIST();
    Col_Word repeat = Col_RepeatList(list, 2);
    checkList(repeat, LARGE_LIST_LEN * 2, 1);
    PICOTEST_ASSERT(Col_Sublist(repeat, 0, LARGE_LIST_LEN - 1) == list);
    PICOTEST_ASSERT(Col_Sublist(repeat, LARGE_LIST_LEN, SIZE_MAX) == list);
}
PICOTEST_CASE(testRepeatCircularListIsIdentity, colibriFixture) {
    Col_Word list = CIRCULAR_LIST();
    PICOTEST_ASSERT(Col_RepeatList(list, 2) == list);
}
PICOTEST_CASE(testRepeatCyclicListIsIdentity, colibriFixture) {
    Col_Word list = CYCLIC_LIST();
    PICOTEST_ASSERT(Col_RepeatList(list, 2) == list);
}
PICOTEST_CASE(testRepeatListRecurse, colibriFixture) {
    Col_Word list = LARGE_LIST();
    checkList(Col_RepeatList(list, 3), LARGE_LIST_LEN * 3, 2);
    checkList(Col_RepeatList(list, 4), LARGE_LIST_LEN * 4, 2);
    checkList(Col_RepeatList(list, 8), LARGE_LIST_LEN * 8, 3);
    checkList(Col_RepeatList(list, 16), LARGE_LIST_LEN * 16, 4);
    checkList(Col_RepeatList(list, 32), LARGE_LIST_LEN * 32, 5);
    checkList(Col_RepeatList(list, 65536), LARGE_LIST_LEN * 65536, 16);
    checkList(Col_RepeatList(list, 65537), LARGE_LIST_LEN * 65537, 17);
}
PICOTEST_CASE(testRepeatEmptyList, colibriFixture) {
    PICOTEST_ASSERT(Col_RepeatList(Col_EmptyList(), SIZE_MAX) ==
                    Col_EmptyList());
}
PICOTEST_CASE(testRepeatListMax, colibriFixture) {
    Col_Word list = Col_RepeatList(Col_NewList(1, NULL), SIZE_MAX);
    PICOTEST_ASSERT(Col_ListLength(list) == SIZE_MAX);
    PICOTEST_ASSERT(Col_ListDepth(list) > 5);
}

PICOTEST_SUITE(testCircularList, testCircularListOfEmptyListIsEmptyList,
               testCircularListOfImmutableListIsImmediate,
               testCircularListOfCircularListIsIdentity,
               testCircularListOfCyclicListIsIdentity);

PICOTEST_CASE(testCircularListOfEmptyListIsEmptyList, colibriFixture) {
    checkListEmpty(Col_CircularList(Col_EmptyList()));
}
PICOTEST_CASE(testCircularListOfImmutableListIsImmediate, colibriFixture) {
    Col_Word core = SMALL_LIST();
    PICOTEST_ASSERT(Col_CircularList(core) == Col_CircularList(core));
}
PICOTEST_CASE(testCircularListOfCircularListIsIdentity, colibriFixture) {
    Col_Word list = CIRCULAR_LIST();
    PICOTEST_ASSERT(Col_CircularList(list) == list);
}
PICOTEST_CASE(testCircularListOfCyclicListIsIdentity, colibriFixture) {
    Col_Word list = CYCLIC_LIST();
    PICOTEST_ASSERT(Col_CircularList(list) == list);
}

PICOTEST_SUITE(testListInsert, testListInsertEmptyIsIdentity,
               testListInsertAtBeginningIsConcat, testListInsertPastEndIsConcat,
               testListInsertMiddle, testListInsertIntoCircularList,
               testListInsertIntoCyclicList);
PICOTEST_CASE(testListInsertEmptyIsIdentity, colibriFixture) {
    Col_Word list = LARGE_LIST();
    PICOTEST_ASSERT(Col_ListInsert(list, 0, Col_EmptyList()) == list);
}
PICOTEST_CASE(testListInsertAtBeginningIsConcat, colibriFixture) {
    Col_Word into = LARGE_LIST(), list = SMALL_LIST();
    Col_Word result = Col_ListInsert(into, 0, list);
    PICOTEST_ASSERT(Col_Sublist(result, 0, SMALL_LIST_LEN - 1) == list);
    PICOTEST_ASSERT(Col_Sublist(result, SMALL_LIST_LEN, SIZE_MAX) == into);
}
PICOTEST_CASE(testListInsertPastEndIsConcat, colibriFixture) {
    Col_Word into = LARGE_LIST(), list = SMALL_LIST();
    Col_Word result = Col_ListInsert(into, SIZE_MAX, list);
    PICOTEST_ASSERT(Col_Sublist(result, 0, LARGE_LIST_LEN - 1) == into);
    PICOTEST_ASSERT(Col_Sublist(result, LARGE_LIST_LEN, SIZE_MAX) == list);
}
PICOTEST_CASE(testListInsertMiddle, colibriFixture) {
    Col_Word into = LARGE_LIST(), list = SMALL_LIST();
    size_t index = LARGE_LIST_LEN / 3;
    Col_Word result = Col_ListInsert(into, index, list);
    PICOTEST_ASSERT(Col_ListAt(result, index) == Col_ListAt(list, 0));
}
PICOTEST_CASE(testListInsertIntoCircularList, colibriFixture) {
    Col_Word into = CIRCULAR_LIST(), list = SMALL_LIST();
    size_t index = CIRCULAR_LIST_LEN / 3;
    Col_Word result = Col_ListInsert(into, index, list);
    PICOTEST_ASSERT(Col_ListLoopLength(result) == Col_ListLength(result));
    PICOTEST_ASSERT(Col_ListAt(result, index) == Col_ListAt(list, 0));
}
PICOTEST_SUITE(testListInsertIntoCyclicList, testListInsertIntoCyclicListHead,
               testListInsertIntoCyclicListTail);
PICOTEST_CASE(testListInsertIntoCyclicListHead, colibriFixture) {
    Col_Word into = CYCLIC_LIST(), list = SMALL_LIST();
    size_t index = CYCLIC_LIST_HEAD_LEN / 3;
    Col_Word result = Col_ListInsert(into, index, list);
    PICOTEST_ASSERT(Col_ListLoopLength(result) == CYCLIC_LIST_TAIL_LEN);
    PICOTEST_ASSERT(Col_ListAt(result, index) == Col_ListAt(list, 0));
}
PICOTEST_CASE(testListInsertIntoCyclicListTail, colibriFixture) {
    Col_Word into = CYCLIC_LIST(), list = SMALL_LIST();
    size_t index = CYCLIC_LIST_HEAD_LEN + CYCLIC_LIST_TAIL_LEN / 3;
    Col_Word result = Col_ListInsert(into, index, list);
    PICOTEST_ASSERT(Col_ListLoopLength(result) ==
                    CYCLIC_LIST_TAIL_LEN + SMALL_LIST_LEN);
    PICOTEST_ASSERT(Col_ListAt(result, index) == Col_ListAt(list, 0));
}

PICOTEST_SUITE(testListRemove, testListRemoveFromEmptyIsIdentity,
               testListRemovePastEndIsIdentity,
               testListRemoveInvalidRangeIsIdentity,
               testListRemoveTrimIsSublist, testListRemoveMiddle,
               testListRemoveFromCircularList, testListRemoveFromCyclicList);
PICOTEST_CASE(testListRemoveFromEmptyIsIdentity, colibriFixture) {
    checkListEmpty(Col_ListRemove(Col_EmptyList(), 0, 0));
}
PICOTEST_CASE(testListRemovePastEndIsIdentity, colibriFixture) {
    Col_Word list = LARGE_LIST();
    PICOTEST_ASSERT(Col_ListRemove(list, LARGE_LIST_LEN, SIZE_MAX) == list);
}
PICOTEST_CASE(testListRemoveInvalidRangeIsIdentity, colibriFixture) {
    Col_Word list = LARGE_LIST();
    PICOTEST_ASSERT(Col_ListRemove(list, 1, 0) == list);
}
PICOTEST_CASE(testListRemoveTrimIsSublist, colibriFixture) {
    Col_Word list = LARGE_LIST();
    size_t index = LARGE_LIST_LEN / 3;

    Col_Word trimBegin = Col_ListRemove(list, 0, index);
    PICOTEST_ASSERT(Col_ListLength(trimBegin) == LARGE_LIST_LEN - index - 1);
    PICOTEST_ASSERT(Col_ListAt(trimBegin, 0) == Col_ListAt(list, index + 1));

    Col_Word trimEnd = Col_ListRemove(list, index + 1, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListLength(trimEnd) == index + 1);
    PICOTEST_ASSERT(Col_ListAt(trimEnd, 0) == Col_ListAt(list, 0));
    PICOTEST_ASSERT(Col_ListAt(trimEnd, index) == Col_ListAt(list, index));
}
PICOTEST_CASE(testListRemoveMiddle, colibriFixture) {
    Col_Word list = LARGE_LIST();
    size_t first = LARGE_LIST_LEN / 3, last = LARGE_LIST_LEN / 2;
    Col_Word result = Col_ListRemove(list, first, last);
    PICOTEST_ASSERT(Col_ListAt(result, 0) == Col_ListAt(list, 0));
    PICOTEST_ASSERT(Col_ListAt(result, first - 1) ==
                    Col_ListAt(list, first - 1));
    PICOTEST_ASSERT(Col_ListAt(result, first) == Col_ListAt(list, last + 1));
}

PICOTEST_SUITE(testListRemoveFromCircularList,
               testListRemoveFromCircularListBeginning,
               testListRemoveFromCircularListCoreInner,
               testListRemoveFromCircularListCoreOuter);

static void checkListRemoveFromCircularListBeginning(Col_Word list,
                                                     size_t last) {
    size_t loop = Col_ListLoopLength(list);
    Col_Word result = Col_ListRemove(list, 0, last);
    PICOTEST_ASSERT(Col_ListAt(result, 0) == Col_ListAt(list, last + 1));
    PICOTEST_ASSERT(Col_ListLength(result) == Col_ListLength(list));
    PICOTEST_ASSERT(Col_ListLength(result) == loop);
    PICOTEST_ASSERT(Col_ListLoopLength(result) == loop);
}
PICOTEST_CASE(testListRemoveFromCircularListBeginning, colibriFixture) {
    checkListRemoveFromCircularListBeginning(CIRCULAR_LIST(),
                                             CIRCULAR_LIST_LEN / 3);
    checkListRemoveFromCircularListBeginning(CIRCULAR_LIST(),
                                             CIRCULAR_LIST_LEN * 2 / 3);
    checkListRemoveFromCircularListBeginning(CIRCULAR_LIST(),
                                             CIRCULAR_LIST_LEN * 2 + 1);
    checkListRemoveFromCircularListBeginning(CIRCULAR_LIST(), SIZE_MAX - 1);
}

static void checkListRemoveFromCircularListCoreInner(Col_Word list,
                                                     size_t first,
                                                     size_t last) {
    size_t loop = Col_ListLoopLength(list);
    Col_Word result = Col_ListRemove(list, first, last);
    PICOTEST_ASSERT(Col_ListAt(result, first - 1) ==
                    Col_ListAt(list, first - 1));
    PICOTEST_ASSERT(Col_ListAt(result, first) == Col_ListAt(list, last + 1));
    PICOTEST_ASSERT(Col_ListLength(result) == Col_ListLoopLength(result));
    PICOTEST_ASSERT(Col_ListLoopLength(result) ==
                    loop - (last - first + 1) % loop);
}
PICOTEST_CASE(testListRemoveFromCircularListCoreInner, colibriFixture) {
    checkListRemoveFromCircularListCoreInner(CIRCULAR_LIST(), 1,
                                             CIRCULAR_LIST_LEN / 3);
    checkListRemoveFromCircularListCoreInner(
        CIRCULAR_LIST(), CIRCULAR_LIST_LEN / 3, CIRCULAR_LIST_LEN * 2 / 3);
    checkListRemoveFromCircularListCoreInner(CIRCULAR_LIST(), 1,
                                             CIRCULAR_LIST_LEN * 2 - 1);
    checkListRemoveFromCircularListCoreInner(CIRCULAR_LIST(), 1, SIZE_MAX - 1);
}

static void checkListRemoveFromCircularListCoreOuter(Col_Word list,
                                                     size_t first,
                                                     size_t last) {
    size_t loop = Col_ListLoopLength(list);
    Col_Word result = Col_ListRemove(list, first, last);
    PICOTEST_ASSERT(Col_ListAt(result, (first % loop) - 1) ==
                    Col_ListAt(list, first - 1));
    PICOTEST_ASSERT(Col_ListAt(result, first % loop) ==
                    Col_ListAt(list, last + 1));
    size_t head = (last % loop) + 1;
    PICOTEST_ASSERT(Col_ListLength(result) ==
                    head + Col_ListLoopLength(result));
    PICOTEST_ASSERT(Col_ListLoopLength(result) ==
                    loop - (last - first + 1) % loop);
}
PICOTEST_CASE(testListRemoveFromCircularListCoreOuter, colibriFixture) {
    checkListRemoveFromCircularListCoreOuter(
        CIRCULAR_LIST(), CIRCULAR_LIST_LEN / 3, CIRCULAR_LIST_LEN);
    checkListRemoveFromCircularListCoreOuter(
        CIRCULAR_LIST(), CIRCULAR_LIST_LEN / 3, CIRCULAR_LIST_LEN + 1);
    checkListRemoveFromCircularListCoreOuter(
        CIRCULAR_LIST(), CIRCULAR_LIST_LEN - 1, CIRCULAR_LIST_LEN);
    checkListRemoveFromCircularListCoreOuter(
        CIRCULAR_LIST(), CIRCULAR_LIST_LEN - 1, CIRCULAR_LIST_LEN + 1);
    checkListRemoveFromCircularListCoreOuter(
        CIRCULAR_LIST(), CIRCULAR_LIST_LEN * 2 - 1, CIRCULAR_LIST_LEN * 2 + 1);
    checkListRemoveFromCircularListCoreOuter(
        CIRCULAR_LIST(), CIRCULAR_LIST_LEN * 3 - 2, CIRCULAR_LIST_LEN * 10 + 5);
}

PICOTEST_SUITE(testListRemoveFromCyclicList, testListRemoveFromCyclicListHead,
               testListRemoveFromCyclicListTail,
               testListRemoveFromCyclicListMiddle);
PICOTEST_CASE(testListRemoveFromCyclicListHead, colibriFixture) {
    Col_Word list = CYCLIC_LIST();
    Col_Word result = Col_ListRemove(list, 1, CYCLIC_LIST_HEAD_LEN - 2);
    PICOTEST_ASSERT(Col_ListAt(result, 0) == Col_ListAt(list, 0));
    PICOTEST_ASSERT(Col_ListAt(result, 1) ==
                    Col_ListAt(list, CYCLIC_LIST_HEAD_LEN - 1));
    PICOTEST_ASSERT(Col_ListLength(result) == 2 + CYCLIC_LIST_TAIL_LEN);
    PICOTEST_ASSERT(Col_ListLoopLength(result) == CYCLIC_LIST_TAIL_LEN);
}
PICOTEST_CASE(testListRemoveFromCyclicListTail, colibriFixture) {
    Col_Word list = CYCLIC_LIST();
    Col_Word result =
        Col_ListRemove(list, CYCLIC_LIST_HEAD_LEN + 2,
                       CYCLIC_LIST_HEAD_LEN + CYCLIC_LIST_TAIL_LEN - 2);
    PICOTEST_ASSERT(Col_ListAt(result, 0) == Col_ListAt(list, 0));
    PICOTEST_ASSERT(Col_ListAt(result, CYCLIC_LIST_HEAD_LEN + 1) ==
                    Col_ListAt(list, CYCLIC_LIST_HEAD_LEN + 1));
    PICOTEST_ASSERT(
        Col_ListAt(result, CYCLIC_LIST_HEAD_LEN + 2) ==
        Col_ListAt(list, CYCLIC_LIST_HEAD_LEN + CYCLIC_LIST_TAIL_LEN - 1));
    PICOTEST_ASSERT(Col_ListLength(result) == CYCLIC_LIST_HEAD_LEN + 3);
    PICOTEST_ASSERT(Col_ListLoopLength(result) == 3);
}
PICOTEST_CASE(testListRemoveFromCyclicListMiddle, colibriFixture) {
    Col_Word list = CYCLIC_LIST();
    Col_Word result = Col_ListRemove(list, CYCLIC_LIST_HEAD_LEN - 2,
                                     CYCLIC_LIST_HEAD_LEN + 3);
    PICOTEST_ASSERT(Col_ListAt(result, 0) == Col_ListAt(list, 0));
    PICOTEST_ASSERT(Col_ListAt(result, CYCLIC_LIST_HEAD_LEN - 3) ==
                    Col_ListAt(list, CYCLIC_LIST_HEAD_LEN - 3));
    PICOTEST_ASSERT(Col_ListAt(result, CYCLIC_LIST_HEAD_LEN - 2) ==
                    Col_ListAt(list, CYCLIC_LIST_HEAD_LEN + 4));
    PICOTEST_ASSERT(Col_ListLength(result) ==
                    CYCLIC_LIST_HEAD_LEN + CYCLIC_LIST_TAIL_LEN - 2);
    PICOTEST_ASSERT(Col_ListLoopLength(result) == CYCLIC_LIST_TAIL_LEN);
}

PICOTEST_SUITE(testListReplace, testListReplaceInvalidRangeIsIdentity,
               testListReplaceOneElement, testListReplaceInCircularList,
               testListReplaceInCyclicList);
PICOTEST_CASE(testListReplaceInvalidRangeIsIdentity, colibriFixture) {
    Col_Word list = LARGE_LIST();
    PICOTEST_ASSERT(Col_ListReplace(list, 1, 0, SMALL_LIST()) == list);
}
PICOTEST_CASE(testListReplaceOneElement, colibriFixture) {
    Col_Word list = LARGE_LIST();
    size_t index = LARGE_LIST_LEN / 3;
    Col_Word e = Col_ListAt(list, index);
    PICOTEST_ASSERT(e != WORD_NIL);
    Col_Word result = Col_ListReplace(list, index, index, Col_NewList(1, NULL));
    checkListVector(list, LARGE_LIST_LEN);
    checkList(result, LARGE_LIST_LEN, 2);
    PICOTEST_ASSERT(Col_ListAt(result, index) == WORD_NIL);
}

static void checkListReplaceInCircularList(Col_Word list, size_t first,
                                           size_t last, Col_Word with) {
    size_t loop = Col_ListLoopLength(list);
    Col_Word result = Col_ListReplace(list, first, last, with);
    size_t first2 = first % loop;
    if (first2 == 0 && first != 0)
        first2 = loop;
    if (first > 0) {
        PICOTEST_ASSERT(Col_ListAt(result, 0) == Col_ListAt(list, 0));
        PICOTEST_ASSERT(Col_ListAt(result, first2 - 1) ==
                        Col_ListAt(list, first - 1));
    }
    PICOTEST_ASSERT(Col_ListAt(result, first2) == Col_ListAt(with, 0));
    PICOTEST_ASSERT(Col_ListAt(result, first2 + Col_ListLength(with) - 1) ==
                    Col_ListAt(with, Col_ListLength(with) - 1));
    PICOTEST_ASSERT(Col_ListAt(result, first2 + Col_ListLength(with)) ==
                    Col_ListAt(list, last + 1));
}
PICOTEST_CASE(testListReplaceInCircularList, colibriFixture) {
    checkListReplaceInCircularList(CIRCULAR_LIST(), 0, 10, SMALL_LIST());
    checkListReplaceInCircularList(CIRCULAR_LIST(), 10, 20, SMALL_LIST());
    checkListReplaceInCircularList(CIRCULAR_LIST(), CIRCULAR_LIST_LEN * 2,
                                   CIRCULAR_LIST_LEN * 3 + 10, SMALL_LIST());
    checkListReplaceInCircularList(CIRCULAR_LIST(), CIRCULAR_LIST_LEN + 2,
                                   CIRCULAR_LIST_LEN + 10, SMALL_LIST());
    checkListReplaceInCircularList(CIRCULAR_LIST(), CIRCULAR_LIST_LEN + 2,
                                   CIRCULAR_LIST_LEN * 4 + 5, SMALL_LIST());
}
static void checkListReplaceInCyclicList(Col_Word list, size_t first,
                                         size_t last, Col_Word with) {
    size_t tailLen = Col_ListLoopLength(list);
    size_t headLen = Col_ListLength(list) - tailLen;
    Col_Word result = Col_ListReplace(list, first, last, with);
    size_t first2 =
        (first > headLen) ? (first - headLen) % tailLen + headLen : first;
    if (first > 0) {
        PICOTEST_ASSERT(Col_ListAt(result, 0) == Col_ListAt(list, 0));
        PICOTEST_ASSERT(Col_ListAt(result, first2 - 1) ==
                        Col_ListAt(list, first - 1));
    }
    PICOTEST_ASSERT(Col_ListAt(result, first2) == Col_ListAt(with, 0));
    PICOTEST_ASSERT(Col_ListAt(result, first2 + Col_ListLength(with) - 1) ==
                    Col_ListAt(with, Col_ListLength(with) - 1));
    PICOTEST_ASSERT(Col_ListAt(result, first2 + Col_ListLength(with)) ==
                    Col_ListAt(list, last + 1));
}
PICOTEST_CASE(testListReplaceInCyclicList, colibriFixture) {
    checkListReplaceInCyclicList(CYCLIC_LIST(), 0, 10, SMALL_LIST());
    checkListReplaceInCyclicList(CYCLIC_LIST(), 10, 20, SMALL_LIST());
    checkListReplaceInCyclicList(
        CYCLIC_LIST(), CYCLIC_LIST_HEAD_LEN - 5,
        CYCLIC_LIST_HEAD_LEN + CYCLIC_LIST_TAIL_LEN + 15, SMALL_LIST());
    checkListReplaceInCyclicList(
        CYCLIC_LIST(), CYCLIC_LIST_HEAD_LEN,
        CYCLIC_LIST_HEAD_LEN + CYCLIC_LIST_TAIL_LEN * 3 + 10, SMALL_LIST());
}

PICOTEST_SUITE(testListTraversal, testListTraversalErrors,
               testTraverseSingleList, testTraverseMultipleLists);
// TODO reverse traversal

PICOTEST_SUITE(testListTraversalErrors, testTraverseListProcMustNotBeNull);
PICOTEST_CASE(testTraverseListProcMustNotBeNull, colibriFixture) {
    PICOTEST_VERIFY(traverseListChunks_valueCheck_proc(NULL) == 1);
    PICOTEST_VERIFY(traverseListChunksN_valueCheck_proc(NULL) == 1);
}

PICOTEST_SUITE(testTraverseSingleList, testTraverseEmptyListIsNoop,
               testTraverseListPastEndIsNoop, testTraverseZeroElementIsNoop,
               testTraverseFlatListHasOneChunk, testTraverseSublist,
               testTraverseConcatList, testTraverseCircularList,
               testTraverseCyclicList, testTraverseListRange,
               testTraverseListBreak);
// TODO void list

#define MAX_CHUNKS 5
#define CHUNK_SAMPLE_SIZE 10
typedef struct ListChunkInfo {
    size_t length;
    Col_Word *const chunks[MAX_CHUNKS];
    Col_Word sample[CHUNK_SAMPLE_SIZE];
} ListChunkInfo;
typedef struct ListChunkCounterData {
    size_t number;
    size_t size;
    size_t nbChunks;
    ListChunkInfo *infos;
} ListChunkCounterData;
#define DECLARE_CHUNKS_DATA(name, size) DECLARE_CHUNKS_DATA_N(name, 1, size)
#define DECLARE_CHUNKS_DATA_N(name, number, size)                              \
    ListChunkInfo name##datas[size];                                           \
    ListChunkCounterData name = {number, size, 0, name##datas};
#define CHUNK_INFO(name, index) (name##datas)[index]

static int listChunkCounter(size_t index, size_t length, size_t number,
                            const Col_Word **chunks,
                            Col_ClientData clientData) {
    ListChunkCounterData *data = (ListChunkCounterData *)clientData;
    PICOTEST_ASSERT(number == data->number);
    PICOTEST_ASSERT(number < MAX_CHUNKS);
    PICOTEST_ASSERT(data != NULL);
    PICOTEST_ASSERT(data->nbChunks < data->size);

    ListChunkInfo *info = data->infos + data->nbChunks++;
    info->length = length;
    memcpy(info->chunks, chunks, sizeof(*chunks) * number);
    if (chunks[0] == COL_LISTCHUNK_VOID)
        memset(info->sample, 0, CHUNK_SAMPLE_SIZE * sizeof(Col_Word));
    else if (chunks[0])
        memcpy(info->sample, chunks[0],
               (length < CHUNK_SAMPLE_SIZE ? length : CHUNK_SAMPLE_SIZE) *
                   sizeof(Col_Word));
    return 0;
}

PICOTEST_CASE(testTraverseEmptyListIsNoop, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    PICOTEST_ASSERT(Col_TraverseListChunks(Col_EmptyList(), 0, SIZE_MAX, 0,
                                           listChunkCounter, &data,
                                           NULL) == -1);
    PICOTEST_ASSERT(data.nbChunks == 0);
}
PICOTEST_CASE(testTraverseListPastEndIsNoop, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    PICOTEST_ASSERT(Col_TraverseListChunks(SMALL_LIST(), SMALL_LIST_LEN,
                                           SIZE_MAX, 0, listChunkCounter, &data,
                                           NULL) == -1);
    PICOTEST_ASSERT(data.nbChunks == 0);
}
PICOTEST_CASE(testTraverseZeroElementIsNoop, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    PICOTEST_ASSERT(Col_TraverseListChunks(SMALL_LIST(), 0, 0, 0,
                                           listChunkCounter, &data,
                                           NULL) == -1);
    PICOTEST_ASSERT(data.nbChunks == 0);
}
PICOTEST_CASE(testTraverseFlatListHasOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Word list = LARGE_LIST();
    PICOTEST_ASSERT(Col_TraverseListChunks(list, 0, SIZE_MAX, 0,
                                           listChunkCounter, &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == LARGE_LIST_LEN);
}

PICOTEST_SUITE(testTraverseSublist,
               testTraverseSublistOfFlatListHasOneOffsetChunk);
// TODO complex concat scenarios
PICOTEST_CASE(testTraverseSublistOfFlatListHasOneOffsetChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(listData, 1);
    DECLARE_CHUNKS_DATA(sublistData, 1);
    Col_Word list = LARGE_LIST();
    Col_Word sublist = Col_Sublist(list, 1, SIZE_MAX);
    PICOTEST_ASSERT(Col_TraverseListChunks(list, 1, SIZE_MAX, 0,
                                           listChunkCounter, &listData,
                                           NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseListChunks(sublist, 0, SIZE_MAX, 0,
                                           listChunkCounter, &sublistData,
                                           NULL) == 0);
    PICOTEST_ASSERT(listData.nbChunks == 1);
    PICOTEST_ASSERT(sublistData.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(listData, 0).length ==
                    CHUNK_INFO(sublistData, 0).length);
    PICOTEST_ASSERT(CHUNK_INFO(listData, 0).chunks[0] ==
                    CHUNK_INFO(sublistData, 0).chunks[0]);
}

PICOTEST_SUITE(testTraverseConcatList, testTraverseConcatListsHaveTwoChunks,
               testTraverseConcatListsLeftArmHaveOneChunk,
               testTraverseConcatListsRightArmHaveOneChunk,
               testTraverseConcatSublistHasTwoChunks);
PICOTEST_CASE(testTraverseConcatListsHaveTwoChunks, colibriFixture) {
    DECLARE_CHUNKS_DATA(leftData, 1);
    DECLARE_CHUNKS_DATA(rightData, 1);
    DECLARE_CHUNKS_DATA(concatListData, 2);
    Col_Word left = LARGE_LIST(), right = SMALL_LIST();
    Col_Word concatList = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_TraverseListChunks(left, 1, SIZE_MAX, 0,
                                           listChunkCounter, &leftData,
                                           NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseListChunks(right, 0, 5, 0, listChunkCounter,
                                           &rightData, NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseListChunks(concatList, 1, LARGE_LIST_LEN + 4, 0,
                                           listChunkCounter, &concatListData,
                                           NULL) == 0);
    PICOTEST_ASSERT(leftData.nbChunks == 1);
    PICOTEST_ASSERT(rightData.nbChunks == 1);
    PICOTEST_ASSERT(concatListData.nbChunks == 2);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).length ==
                    CHUNK_INFO(concatListData, 0).length);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).chunks[0] ==
                    CHUNK_INFO(concatListData, 0).chunks[0]);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).length ==
                    CHUNK_INFO(concatListData, 1).length);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).chunks[0] ==
                    CHUNK_INFO(concatListData, 1).chunks[0]);
}
PICOTEST_CASE(testTraverseConcatListsLeftArmHaveOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(leftData, 1);
    DECLARE_CHUNKS_DATA(concatListData, 1);
    Col_Word left = LARGE_LIST(), right = SMALL_LIST();
    Col_Word concatList = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_TraverseListChunks(left, 1, 5, 0, listChunkCounter,
                                           &leftData, NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseListChunks(concatList, 1, 5, 0,
                                           listChunkCounter, &concatListData,
                                           NULL) == 0);
    PICOTEST_ASSERT(concatListData.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).length ==
                    CHUNK_INFO(concatListData, 0).length);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).chunks[0] ==
                    CHUNK_INFO(concatListData, 0).chunks[0]);
}
PICOTEST_CASE(testTraverseConcatListsRightArmHaveOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(rightData, 1);
    DECLARE_CHUNKS_DATA(concatListData, 1);
    Col_Word left = LARGE_LIST(), right = SMALL_LIST();
    Col_Word concatList = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_TraverseListChunks(right, 1, 5, 0, listChunkCounter,
                                           &rightData, NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseListChunks(concatList, LARGE_LIST_LEN + 1, 5, 0,
                                           listChunkCounter, &concatListData,
                                           NULL) == 0);
    PICOTEST_ASSERT(concatListData.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).length ==
                    CHUNK_INFO(concatListData, 0).length);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).chunks[0] ==
                    CHUNK_INFO(concatListData, 0).chunks[0]);
}
PICOTEST_CASE(testTraverseConcatSublistHasTwoChunks, colibriFixture) {
    DECLARE_CHUNKS_DATA(leftData, 1);
    DECLARE_CHUNKS_DATA(rightData, 1);
    DECLARE_CHUNKS_DATA(sublistData, 2);
    Col_Word left = LARGE_LIST(), right = LARGE_LIST();
    Col_Word concatList = Col_ConcatLists(left, right);
    Col_Word sublist = Col_Sublist(concatList, 1, LARGE_LIST_LEN * 2 - 2);
    PICOTEST_ASSERT(Col_TraverseListChunks(left, 2, SIZE_MAX, 0,
                                           listChunkCounter, &leftData,
                                           NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseListChunks(right, 0, 6, 0, listChunkCounter,
                                           &rightData, NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseListChunks(sublist, 1, LARGE_LIST_LEN + 4, 0,
                                           listChunkCounter, &sublistData,
                                           NULL) == 0);
    PICOTEST_ASSERT(leftData.nbChunks == 1);
    PICOTEST_ASSERT(rightData.nbChunks == 1);
    PICOTEST_ASSERT(sublistData.nbChunks == 2);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).length ==
                    CHUNK_INFO(sublistData, 0).length);
    PICOTEST_ASSERT(CHUNK_INFO(leftData, 0).chunks[0] ==
                    CHUNK_INFO(sublistData, 0).chunks[0]);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).length ==
                    CHUNK_INFO(sublistData, 1).length);
    PICOTEST_ASSERT(CHUNK_INFO(rightData, 0).chunks[0] ==
                    CHUNK_INFO(sublistData, 1).chunks[0]);
}

PICOTEST_CASE(testTraverseCircularList, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Word list = CIRCULAR_LIST();
    PICOTEST_ASSERT(Col_TraverseListChunks(list, 0, SIZE_MAX, 0,
                                           listChunkCounter, &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == CIRCULAR_LIST_LEN);
}
PICOTEST_CASE(testTraverseCyclicList, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 2);
    Col_Word list = CYCLIC_LIST();
    PICOTEST_ASSERT(Col_TraverseListChunks(list, 0, SIZE_MAX, 0,
                                           listChunkCounter, &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 2);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == CYCLIC_LIST_HEAD_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 1).length == CYCLIC_LIST_TAIL_LEN);
}

PICOTEST_SUITE(testTraverseListRange, testTraverseSingleElement);
static void checkTraverseElement(Col_Word list, size_t index) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Word e = Col_ListAt(list, index);
    PICOTEST_ASSERT(Col_TraverseListChunks(list, index, 1, 0, listChunkCounter,
                                           &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == 1);
    PICOTEST_ASSERT(*(CHUNK_INFO(data, 0).chunks[0]) == e);
}
PICOTEST_CASE(testTraverseSingleElement, colibriFixture) {
    Col_Word list = LARGE_LIST();
    checkTraverseElement(list, 0);
    checkTraverseElement(list, LARGE_LIST_LEN / 2);
    checkTraverseElement(list, LARGE_LIST_LEN - 1);
}

typedef struct ListChunkBreakData {
    size_t max;
    size_t counter;
} ListChunkBreakData;
static int listChunkBreak(size_t index, size_t length, size_t number,
                          const Col_Word **chunks, Col_ClientData clientData) {
    ListChunkBreakData *data = (ListChunkBreakData *)clientData;

    if (++data->counter >= data->max)
        return data->counter;
    return 0;
}
PICOTEST_CASE(testTraverseListBreak, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 10);
    Col_Word list = Col_RepeatList(LARGE_LIST(), 10);
    PICOTEST_ASSERT(Col_TraverseListChunks(list, 0, SIZE_MAX, 0,
                                           listChunkCounter, &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 10);

    ListChunkBreakData breakData = {5, 0};
    PICOTEST_ASSERT(Col_TraverseListChunks(list, 0, SIZE_MAX, 0, listChunkBreak,
                                           &breakData, NULL) == 5);
    PICOTEST_ASSERT(breakData.counter == 5);

    ListChunkBreakData breakData2 = {20, 0};
    PICOTEST_ASSERT(Col_TraverseListChunks(list, 0, SIZE_MAX, 0, listChunkBreak,
                                           &breakData2, NULL) == 0);
    PICOTEST_ASSERT(breakData2.counter == 10);
}

PICOTEST_SUITE(testTraverseMultipleLists, testTraverseMultipleEmptyListsIsNoop,
               testTraverseMultipleListsPastEndIsNoop,
               testTraverseMultipleListsZeroElementIsNoop,
               testTraverseEquallySizedLists, testTraverseVaryingSizedLists,
               testTraverseMultipleListsBreak);
PICOTEST_CASE(testTraverseMultipleEmptyListsIsNoop, colibriFixture) {
    Col_Word lists[] = {Col_EmptyList(), Col_EmptyList(), Col_EmptyList()};
    size_t nbLists = sizeof(lists) / sizeof(*lists);
    DECLARE_CHUNKS_DATA_N(data, nbLists, 1);
    PICOTEST_ASSERT(Col_TraverseListChunksN(nbLists, lists, 0, SIZE_MAX,
                                            listChunkCounter, &data,
                                            NULL) == -1);
    PICOTEST_ASSERT(data.nbChunks == 0);
}
PICOTEST_CASE(testTraverseMultipleListsPastEndIsNoop, colibriFixture) {
    Col_Word lists[] = {Col_EmptyList(), SMALL_LIST(), LARGE_LIST()};
    size_t nbLists = sizeof(lists) / sizeof(*lists);
    DECLARE_CHUNKS_DATA_N(data, nbLists, 1);
    PICOTEST_ASSERT(Col_TraverseListChunksN(nbLists, lists, LARGE_LIST_LEN,
                                            SIZE_MAX, listChunkCounter, &data,
                                            NULL) == -1);
    PICOTEST_ASSERT(data.nbChunks == 0);
}
PICOTEST_CASE(testTraverseMultipleListsZeroElementIsNoop, colibriFixture) {
    Col_Word lists[] = {Col_EmptyList(), SMALL_LIST(), LARGE_LIST()};
    size_t nbLists = sizeof(lists) / sizeof(*lists);
    DECLARE_CHUNKS_DATA_N(data, nbLists, 1);
    PICOTEST_ASSERT(Col_TraverseListChunksN(nbLists, lists, 0, 0,
                                            listChunkCounter, &data,
                                            NULL) == -1);
    PICOTEST_ASSERT(data.nbChunks == 0);
}
PICOTEST_CASE(testTraverseEquallySizedLists, colibriFixture) {
    Col_Word lists[] = {LARGE_LIST(), LARGE_LIST(), LARGE_LIST()};
    size_t nbLists = sizeof(lists) / sizeof(*lists);
    DECLARE_CHUNKS_DATA_N(data, nbLists, 1);
    PICOTEST_ASSERT(Col_TraverseListChunksN(nbLists, lists, 0, SIZE_MAX,
                                            listChunkCounter, &data,
                                            NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == LARGE_LIST_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0] != NULL);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[1] != NULL);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[2] != NULL);
}
PICOTEST_CASE(testTraverseVaryingSizedLists, colibriFixture) {
    Col_Word lists[] = {SMALL_LIST(), LARGE_LIST(), VOID_LIST()};
    size_t nbLists = sizeof(lists) / sizeof(*lists);
    DECLARE_CHUNKS_DATA_N(data, nbLists, 3);
    PICOTEST_ASSERT(Col_TraverseListChunksN(nbLists, lists, 0, SIZE_MAX,
                                            listChunkCounter, &data,
                                            NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 3);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).length == SMALL_LIST_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[0] != NULL);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[1] != NULL);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[2] == COL_LISTCHUNK_VOID);

    PICOTEST_ASSERT(CHUNK_INFO(data, 1).length ==
                    LARGE_LIST_LEN - SMALL_LIST_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 1).chunks[0] == NULL);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[1] != NULL);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[2] == COL_LISTCHUNK_VOID);

    PICOTEST_ASSERT(CHUNK_INFO(data, 2).length ==
                    VOID_LIST_LEN - LARGE_LIST_LEN);
    PICOTEST_ASSERT(CHUNK_INFO(data, 2).chunks[0] == NULL);
    PICOTEST_ASSERT(CHUNK_INFO(data, 2).chunks[1] == NULL);
    PICOTEST_ASSERT(CHUNK_INFO(data, 0).chunks[2] == COL_LISTCHUNK_VOID);
}
PICOTEST_CASE(testTraverseMultipleListsBreak, colibriFixture) {
    Col_Word lists[] = {Col_RepeatList(SMALL_LIST(), 10),
                        Col_RepeatList(LARGE_LIST(), 10),
                        Col_RepeatList(VOID_LIST(), 10)};
    size_t nbLists = sizeof(lists) / sizeof(*lists);
    DECLARE_CHUNKS_DATA_N(data, nbLists, 100);
    PICOTEST_ASSERT(Col_TraverseListChunksN(nbLists, lists, 0, SIZE_MAX,
                                            listChunkCounter, &data,
                                            NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 16);

    ListChunkBreakData breakData = {5, 0};
    PICOTEST_ASSERT(Col_TraverseListChunksN(nbLists, lists, 0, SIZE_MAX,
                                            listChunkBreak, &breakData,
                                            NULL) == 5);
    PICOTEST_ASSERT(breakData.counter == 5);

    ListChunkBreakData breakData2 = {20, 0};
    PICOTEST_ASSERT(Col_TraverseListChunksN(nbLists, lists, 0, SIZE_MAX,
                                            listChunkBreak, &breakData2,
                                            NULL) == 0);
    PICOTEST_ASSERT(breakData2.counter == 16);
}

PICOTEST_SUITE(testListIteration, testListIteratorErrors,
               testListIteratorInitialize, testListIteratorCompare,
               testListIteratorAccess, testListIteratorMove,
               testListIteratorEmptyList, testListIteratorArray);

PICOTEST_SUITE(testListIteratorErrors, testListIteratorAtEndIsInvalid);
PICOTEST_CASE(testListIteratorAtEndIsInvalid, colibriFixture) {
    PICOTEST_ASSERT(listIterForward_valueCheck(NULL) == 1);
    PICOTEST_ASSERT(listIterNext_valueCheck(NULL) == 1);
    PICOTEST_ASSERT(listIterAt_valueCheck(NULL) == 1);
}

PICOTEST_SUITE(testListIteratorInitialize, testListIterNull, testListIterBegin,
               testListIterFirst, testListIterLast, testListIterSet);

PICOTEST_CASE(testListIterNull, colibriFixture) {
    Col_ListIterator it = COL_LISTITER_NULL;
    PICOTEST_ASSERT(Col_ListIterNull(it));
    PICOTEST_ASSERT(Col_ListIterList(it) == WORD_NIL);
    PICOTEST_ASSERT(Col_ListIterLength(it) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
}

PICOTEST_SUITE(testListIterBegin, testListIterBeginFlatList,
               testListIterBeginFlatListMax, testListIterBeginCircularList,
               testListIterBeginCyclicList);
PICOTEST_CASE(testListIterBeginFlatList, colibriFixture) {
    Col_Word list = LARGE_LIST();
    size_t index = LARGE_LIST_LEN / 2;
    Col_ListIterator it;
    PICOTEST_ASSERT(Col_ListIterBegin(it, list, index) == 0);
    PICOTEST_ASSERT(!Col_ListIterNull(it));
    PICOTEST_ASSERT(Col_ListIterList(it) == list);
    PICOTEST_ASSERT(Col_ListIterLength(it) == LARGE_LIST_LEN);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
}
PICOTEST_CASE(testListIterBeginFlatListMax, colibriFixture) {
    Col_Word list = LARGE_LIST();
    Col_ListIterator it;
    PICOTEST_ASSERT(Col_ListIterBegin(it, list, SIZE_MAX) == 0);
    PICOTEST_ASSERT(!Col_ListIterNull(it));
    PICOTEST_ASSERT(Col_ListIterList(it) == list);
    PICOTEST_ASSERT(Col_ListIterLength(it) == LARGE_LIST_LEN);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == LARGE_LIST_LEN);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
}

PICOTEST_CASE(testListIterBeginCircularList, colibriFixture) {
    Col_Word list = CIRCULAR_LIST();
    Col_ListIterator it;

    PICOTEST_ASSERT(Col_ListIterBegin(it, list, 1) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 1);
    PICOTEST_ASSERT(Col_ListIterBegin(it, list, CIRCULAR_LIST_LEN - 1) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CIRCULAR_LIST_LEN - 1);
    PICOTEST_ASSERT(Col_ListIterBegin(it, list, CIRCULAR_LIST_LEN) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
    PICOTEST_ASSERT(Col_ListIterBegin(it, list, CIRCULAR_LIST_LEN + 10) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 10);
    PICOTEST_ASSERT(Col_ListIterBegin(it, list, SIZE_MAX) == 1);
}
PICOTEST_CASE(testListIterBeginCyclicList, colibriFixture) {
    Col_Word list = CYCLIC_LIST();
    Col_ListIterator it;

    PICOTEST_ASSERT(Col_ListIterBegin(it, list, 1) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 1);
    PICOTEST_ASSERT(Col_ListIterBegin(it, list, CYCLIC_LIST_LEN - 1) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CYCLIC_LIST_LEN - 1);
    PICOTEST_ASSERT(Col_ListIterBegin(it, list, CYCLIC_LIST_LEN) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CYCLIC_LIST_HEAD_LEN);
    PICOTEST_ASSERT(Col_ListIterBegin(it, list, CYCLIC_LIST_LEN + 10) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CYCLIC_LIST_HEAD_LEN + 10);
    PICOTEST_ASSERT(Col_ListIterBegin(it, list, SIZE_MAX) == 1);
}

PICOTEST_CASE(testListIterFirst, colibriFixture) {
    Col_Word list = LARGE_LIST();
    Col_ListIterator it;
    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(!Col_ListIterNull(it));
    PICOTEST_ASSERT(Col_ListIterList(it) == list);
    PICOTEST_ASSERT(Col_ListIterLength(it) == LARGE_LIST_LEN);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
}
PICOTEST_CASE(testListIterLast, colibriFixture) {
    Col_Word list = LARGE_LIST();
    Col_ListIterator it;
    Col_ListIterLast(it, list);
    PICOTEST_ASSERT(!Col_ListIterNull(it));
    PICOTEST_ASSERT(Col_ListIterList(it) == list);
    PICOTEST_ASSERT(Col_ListIterLength(it) == LARGE_LIST_LEN);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == LARGE_LIST_LEN - 1);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
}
PICOTEST_CASE(testListIterSet, colibriFixture) {
    Col_Word list = LARGE_LIST();
    Col_ListIterator it1;

    Col_ListIterBegin(it1, list, LARGE_LIST_LEN / 2);
    PICOTEST_ASSERT(!Col_ListIterNull(it1));

    Col_ListIterator it2 = COL_LISTITER_NULL;
    PICOTEST_ASSERT(Col_ListIterNull(it2));

    Col_ListIterSet(it2, it1);
    PICOTEST_ASSERT(!Col_ListIterNull(it1));
    PICOTEST_ASSERT(!Col_ListIterNull(it2));
    PICOTEST_ASSERT(Col_ListIterList(it1) == Col_ListIterList(it2));
    PICOTEST_ASSERT(Col_ListIterLength(it1) == Col_ListIterLength(it2));
    PICOTEST_ASSERT(Col_ListIterIndex(it1) == Col_ListIterIndex(it2));

    Col_ListIterNext(it1);
    PICOTEST_ASSERT(Col_ListIterIndex(it1) == Col_ListIterIndex(it2) + 1);
    Col_ListIterPrevious(it2);
    PICOTEST_ASSERT(Col_ListIterIndex(it1) == Col_ListIterIndex(it2) + 2);
}

PICOTEST_SUITE(testListIteratorCompare, testListIteratorCompareFirst,
               testListIteratorCompareLast, testListIteratorCompareEnd,
               testListIteratorCompareMiddle);
PICOTEST_CASE(testListIteratorCompareFirst, colibriFixture) {
    Col_Word list = LARGE_LIST();
    Col_ListIterator it1, it2;

    Col_ListIterFirst(it1, list);
    Col_ListIterBegin(it2, list, 0);
    PICOTEST_ASSERT(Col_ListIterCompare(it1, it2) == 0);
    PICOTEST_ASSERT(Col_ListIterCompare(it2, it1) == 0);
}
PICOTEST_CASE(testListIteratorCompareLast, colibriFixture) {
    Col_Word list = LARGE_LIST();
    Col_ListIterator it1, it2;

    Col_ListIterLast(it1, list);
    Col_ListIterBegin(it2, list, LARGE_LIST_LEN - 1);
    PICOTEST_ASSERT(Col_ListIterCompare(it1, it2) == 0);
    PICOTEST_ASSERT(Col_ListIterCompare(it2, it1) == 0);
}
PICOTEST_CASE(testListIteratorCompareEnd, colibriFixture) {
    Col_Word list = LARGE_LIST();
    Col_ListIterator it1, it2;

    Col_ListIterLast(it1, list);
    Col_ListIterBegin(it2, list, LARGE_LIST_LEN);
    PICOTEST_ASSERT(Col_ListIterEnd(it2));
    PICOTEST_ASSERT(Col_ListIterCompare(it1, it2) < 0);
    PICOTEST_ASSERT(Col_ListIterCompare(it2, it1) > 0);
}
PICOTEST_CASE(testListIteratorCompareMiddle, colibriFixture) {
    Col_Word list = LARGE_LIST();
    Col_ListIterator it1, it2;

    Col_ListIterBegin(it1, list, 1);
    Col_ListIterBegin(it2, list, 2);
    PICOTEST_ASSERT(Col_ListIterCompare(it1, it2) < 0);
    PICOTEST_ASSERT(Col_ListIterCompare(it2, it1) > 0);
}

PICOTEST_SUITE(testListIteratorAccess, testListIterAtSmallList,
               testListIterAtFlatList, testListIterAtSublist,
               testListIterAtConcatList);
// TODO void list

static int listChunkGetElement(size_t index, size_t length, size_t number,
                               const Col_Word **chunks,
                               Col_ClientData clientData) {
    Col_Word *data = (Col_Word *)clientData;
    *data = *chunks[0];
    return 0;
}
static void checkListIterAt(Col_Word list) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Word e;
    Col_ListIterator it;

    Col_TraverseListChunks(list, 0, 1, 0, listChunkGetElement, &e, NULL);
    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(Col_ListIterAt(it) == e);

    Col_TraverseListChunks(list, 1, 1, 0, listChunkGetElement, &e, NULL);
    Col_ListIterBegin(it, list, 1);
    PICOTEST_ASSERT(Col_ListIterAt(it) == e);

    Col_TraverseListChunks(list, Col_ListLength(list) - 1, 1, 0,
                           listChunkGetElement, &e, NULL);
    Col_ListIterLast(it, list);
    PICOTEST_ASSERT(Col_ListIterAt(it) == e);

    Col_ListIterNext(it);
    PICOTEST_ASSERT(Col_ListIterEnd(it));

    Col_TraverseListChunks(list, Col_ListLength(list) - 2, 1, 0,
                           listChunkGetElement, &e, NULL);
    Col_ListIterBackward(it, 2);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == Col_ListLength(list) - 2);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
    PICOTEST_ASSERT(Col_ListIterAt(it) == e);

    Col_ListIterForward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListIterEnd(it));

    Col_TraverseListChunks(list, Col_ListLength(list) - 1, 1, 0,
                           listChunkGetElement, &e, NULL);
    Col_ListIterPrevious(it);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == Col_ListLength(list) - 1);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
    PICOTEST_ASSERT(Col_ListIterAt(it) == e);
}
PICOTEST_CASE(testListIterAtSmallList, colibriFixture) {
    checkListIterAt(SMALL_LIST());
}
PICOTEST_CASE(testListIterAtFlatList, colibriFixture) {
    checkListIterAt(LARGE_LIST());
    checkListIterAt(LARGE_LIST());
    checkListIterAt(LARGE_LIST());
    checkListIterAt(LARGE_LIST());
    checkListIterAt(LARGE_LIST());
}
PICOTEST_CASE(testListIterAtSublist, colibriFixture) {
    checkListIterAt(Col_Sublist(LARGE_LIST(), 1, SIZE_MAX));
}
PICOTEST_CASE(testListIterAtConcatList, colibriFixture) {
    checkListIterAt(Col_ConcatLists(LARGE_LIST(), SMALL_LIST()));
}

PICOTEST_SUITE(testListIteratorMove, testListIterNext, testListIterPrevious,
               testListIterForward, testListIterBackward, testListIterMoveTo);

PICOTEST_SUITE(testListIterNext, testListIterNextFromLastIsAtEnd,
               testListIterNextIterAt);
PICOTEST_CASE(testListIterNextFromLastIsAtEnd, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;

    Col_ListIterLast(it, list);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
    Col_ListIterNext(it);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
}

static void checkListIterNextIterAt(Col_Word list) {
    Col_Word e;
    Col_ListIterator it;

    Col_ListIterFirst(it, list);
    Col_TraverseListChunks(list, Col_ListIterIndex(it), 1, 0,
                           listChunkGetElement, &e, NULL);
    PICOTEST_ASSERT(Col_ListIterAt(it) == e);

    Col_ListIterNext(it);
    Col_TraverseListChunks(list, Col_ListIterIndex(it), 1, 0,
                           listChunkGetElement, &e, NULL);
    PICOTEST_ASSERT(Col_ListIterAt(it) == e);
}
PICOTEST_CASE(testListIterNextIterAt, colibriFixture) {
    checkListIterNextIterAt(SMALL_LIST());
    checkListIterNextIterAt(LARGE_LIST());
    checkListIterNextIterAt(LARGE_LIST());
    checkListIterNextIterAt(LARGE_LIST());
    checkListIterNextIterAt(LARGE_LIST());
    checkListIterNextIterAt(LARGE_LIST());
}

PICOTEST_SUITE(testListIterPrevious, testListIterPreviousFromFirstIsAtEnd,
               testListIterPreviousFromEndIsLast, testListIterPreviousIterAt);
PICOTEST_CASE(testListIterPreviousFromFirstIsAtEnd, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;

    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
    Col_ListIterPrevious(it);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
}
PICOTEST_CASE(testListIterPreviousFromEndIsLast, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;

    Col_ListIterBegin(it, list, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
    Col_ListIterPrevious(it);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
    PICOTEST_ASSERT(Col_ListIterIndex(it) == SMALL_LIST_LEN - 1);
}

static void checkListIterPreviousIterAt(Col_Word list) {
    Col_Word e;
    Col_ListIterator it;

    Col_ListIterLast(it, list);
    Col_TraverseListChunks(list, Col_ListIterIndex(it), 1, 0,
                           listChunkGetElement, &e, NULL);
    PICOTEST_ASSERT(Col_ListIterAt(it) == e);

    Col_ListIterPrevious(it);
    Col_TraverseListChunks(list, Col_ListIterIndex(it), 1, 0,
                           listChunkGetElement, &e, NULL);
    PICOTEST_ASSERT(Col_ListIterAt(it) == e);
}
PICOTEST_CASE(testListIterPreviousIterAt, colibriFixture) {
    checkListIterPreviousIterAt(SMALL_LIST());
    checkListIterPreviousIterAt(LARGE_LIST());
    checkListIterPreviousIterAt(LARGE_LIST());
    checkListIterPreviousIterAt(LARGE_LIST());
    checkListIterPreviousIterAt(LARGE_LIST());
    checkListIterPreviousIterAt(LARGE_LIST());
}

PICOTEST_SUITE(testListIterForward, testListIterForwardZeroIsNoop,
               testListIterForwardMaxIsAtEnd, testListIterForwardIterAt,
               testListIterForwardCircularList, testListIterForwardCyclicList);
PICOTEST_CASE(testListIterForwardZeroIsNoop, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;

    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
    PICOTEST_ASSERT(Col_ListIterForward(it, 0) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
}
PICOTEST_CASE(testListIterForwardMaxIsAtEnd, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;

    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
    PICOTEST_ASSERT(Col_ListIterForward(it, SIZE_MAX) == 0);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
}

static void checkListIterForwardIterAt(Col_Word list) {
    Col_Word e;
    Col_ListIterator it;

    Col_ListIterFirst(it, list);
    Col_TraverseListChunks(list, Col_ListIterIndex(it), 1, 0,
                           listChunkGetElement, &e, NULL);
    PICOTEST_ASSERT(Col_ListIterAt(it) == e);

    PICOTEST_ASSERT(Col_ListIterForward(it, 1) == 0);
    Col_TraverseListChunks(list, Col_ListIterIndex(it), 1, 0,
                           listChunkGetElement, &e, NULL);
    PICOTEST_ASSERT(Col_ListIterAt(it) == e);
}
PICOTEST_CASE(testListIterForwardIterAt, colibriFixture) {
    checkListIterForwardIterAt(SMALL_LIST());
    checkListIterForwardIterAt(LARGE_LIST());
    checkListIterForwardIterAt(LARGE_LIST());
    checkListIterForwardIterAt(LARGE_LIST());
    checkListIterForwardIterAt(LARGE_LIST());
    checkListIterForwardIterAt(LARGE_LIST());
}

PICOTEST_SUITE(testListIterForwardCircularList,
               testListIterForwardCircularListNotLooped,
               testListIterForwardCircularListLooped,
               testListIterForwardCircularListLoopedEnd,
               testListIterForwardCircularListLoopedMax);
PICOTEST_CASE(testListIterForwardCircularListNotLooped, colibriFixture) {
    Col_Word list = CYCLIC_LIST();
    Col_ListIterator it;
    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(Col_ListIterForward(it, CYCLIC_LIST_LEN - 1) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CYCLIC_LIST_LEN - 1);
}
PICOTEST_CASE(testListIterForwardCircularListLooped, colibriFixture) {
    Col_Word list = CIRCULAR_LIST();
    Col_ListIterator it;
    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(Col_ListIterForward(it, CIRCULAR_LIST_LEN) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
    PICOTEST_ASSERT(Col_ListIterForward(it, CIRCULAR_LIST_LEN) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
    PICOTEST_ASSERT(Col_ListIterForward(it, 5) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 5);
    PICOTEST_ASSERT(Col_ListIterForward(it, CIRCULAR_LIST_LEN * 5 + 10) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 15);
    PICOTEST_ASSERT(Col_ListIterForward(it, CIRCULAR_LIST_LEN * 10 - 2) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 13);
}
PICOTEST_CASE(testListIterForwardCircularListLoopedEnd, colibriFixture) {
    Col_Word list = CIRCULAR_LIST();
    Col_ListIterator it;
    Col_ListIterLast(it, list);
    PICOTEST_ASSERT(Col_ListIterForward(it, 1) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
}
PICOTEST_CASE(testListIterForwardCircularListLoopedMax, colibriFixture) {
    Col_Word list = CIRCULAR_LIST();
    Col_ListIterator it;
    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(Col_ListIterForward(it, SIZE_MAX) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) >= 0);
}

PICOTEST_SUITE(testListIterForwardCyclicList,
               testListIterForwardCyclicListNotLooped,
               testListIterForwardCyclicListLooped,
               testListIterForwardCyclicListLoopedEnd,
               testListIterForwardCyclicListLoopedMax);
PICOTEST_CASE(testListIterForwardCyclicListNotLooped, colibriFixture) {
    Col_Word list = CIRCULAR_LIST();
    Col_ListIterator it;
    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(Col_ListIterForward(it, CIRCULAR_LIST_LEN - 1) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CIRCULAR_LIST_LEN - 1);
}
PICOTEST_CASE(testListIterForwardCyclicListLooped, colibriFixture) {
    Col_Word list = CYCLIC_LIST();
    Col_ListIterator it;
    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(Col_ListIterForward(it, CYCLIC_LIST_LEN) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CYCLIC_LIST_HEAD_LEN);
    PICOTEST_ASSERT(Col_ListIterForward(it, CYCLIC_LIST_TAIL_LEN) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CYCLIC_LIST_HEAD_LEN);
    PICOTEST_ASSERT(Col_ListIterForward(it, 5) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CYCLIC_LIST_HEAD_LEN + 5);
    PICOTEST_ASSERT(Col_ListIterForward(it, CYCLIC_LIST_TAIL_LEN * 5 + 10) ==
                    1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CYCLIC_LIST_HEAD_LEN + 15);
    PICOTEST_ASSERT(Col_ListIterForward(it, CYCLIC_LIST_TAIL_LEN * 10 - 2) ==
                    1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CYCLIC_LIST_HEAD_LEN + 13);
}
PICOTEST_CASE(testListIterForwardCyclicListLoopedEnd, colibriFixture) {
    Col_Word list = CYCLIC_LIST();
    Col_ListIterator it;
    Col_ListIterLast(it, list);
    PICOTEST_ASSERT(Col_ListIterForward(it, 1) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CYCLIC_LIST_HEAD_LEN);
}
PICOTEST_CASE(testListIterForwardCyclicListLoopedMax, colibriFixture) {
    Col_Word list = CYCLIC_LIST();
    Col_ListIterator it;
    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(Col_ListIterForward(it, SIZE_MAX) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) >= CYCLIC_LIST_HEAD_LEN);
}

PICOTEST_SUITE(testListIterBackward, testListIterBackwardZeroIsNoop,
               testListIterBackwardMaxIsAtEnd,
               testListIterBackwardOneFromEndIsLast,
               testListIterBackwardMaxFromEndIsNoop,
               testListIterBackwardIterAt);
PICOTEST_CASE(testListIterBackwardZeroIsNoop, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;

    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
    Col_ListIterBackward(it, 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
}
PICOTEST_CASE(testListIterBackwardMaxIsAtEnd, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;

    Col_ListIterLast(it, list);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
    Col_ListIterBackward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
}
PICOTEST_CASE(testListIterBackwardOneFromEndIsLast, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;

    Col_ListIterBegin(it, list, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
    Col_ListIterBackward(it, 1);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
    PICOTEST_ASSERT(Col_ListIterIndex(it) == SMALL_LIST_LEN - 1);
}
PICOTEST_CASE(testListIterBackwardMaxFromEndIsNoop, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;

    Col_ListIterBegin(it, list, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
    Col_ListIterBackward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
}

static void checkListIterBackwardIterAt(Col_Word list) {
    Col_Word e;
    Col_ListIterator it;

    Col_ListIterLast(it, list);
    Col_TraverseListChunks(list, Col_ListIterIndex(it), 1, 0,
                           listChunkGetElement, &e, NULL);
    PICOTEST_ASSERT(Col_ListIterAt(it) == e);

    Col_ListIterBackward(it, 1);
    Col_TraverseListChunks(list, Col_ListIterIndex(it), 1, 0,
                           listChunkGetElement, &e, NULL);
    PICOTEST_ASSERT(Col_ListIterAt(it) == e);
}
PICOTEST_CASE(testListIterBackwardIterAt, colibriFixture) {
    checkListIterBackwardIterAt(SMALL_LIST());
    checkListIterBackwardIterAt(LARGE_LIST());
    checkListIterBackwardIterAt(LARGE_LIST());
    checkListIterBackwardIterAt(LARGE_LIST());
    checkListIterBackwardIterAt(LARGE_LIST());
    checkListIterBackwardIterAt(LARGE_LIST());
}

PICOTEST_SUITE(testListIterMoveTo, testListIterMoveToForward,
               testListIterMoveToBackward, testListIterMoveToSame,
               testListIterMoveToCircularList, testListIterMoveToCyclicList);
PICOTEST_CASE(testListIterMoveToForward, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;
    size_t index = SMALL_LIST_LEN / 2;

    Col_ListIterBegin(it, list, index);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
    PICOTEST_ASSERT(Col_ListIterMoveTo(it, index + 1) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index + 1);
}
PICOTEST_CASE(testListIterMoveToBackward, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;
    size_t index = SMALL_LIST_LEN / 2;

    Col_ListIterBegin(it, list, index);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
    PICOTEST_ASSERT(Col_ListIterMoveTo(it, index - 1) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index - 1);
}
PICOTEST_CASE(testListIterMoveToSame, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;
    size_t index = SMALL_LIST_LEN / 2;

    Col_ListIterBegin(it, list, index);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
    PICOTEST_ASSERT(Col_ListIterMoveTo(it, index) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
}
PICOTEST_CASE(testListIterMoveToCircularList, colibriFixture) {
    Col_Word list = CIRCULAR_LIST();
    Col_ListIterator it;

    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(Col_ListIterMoveTo(it, 1) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 1);
    PICOTEST_ASSERT(Col_ListIterMoveTo(it, CIRCULAR_LIST_LEN - 1) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CIRCULAR_LIST_LEN - 1);
    PICOTEST_ASSERT(Col_ListIterMoveTo(it, CIRCULAR_LIST_LEN) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
    PICOTEST_ASSERT(Col_ListIterMoveTo(it, CIRCULAR_LIST_LEN * 10 + 5) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 5);
    PICOTEST_ASSERT(Col_ListIterMoveTo(it, CIRCULAR_LIST_LEN * 10 - 20) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CIRCULAR_LIST_LEN - 20);
}
PICOTEST_CASE(testListIterMoveToCyclicList, colibriFixture) {
    Col_Word list = CYCLIC_LIST();
    Col_ListIterator it;

    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(Col_ListIterMoveTo(it, 1) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 1);
    PICOTEST_ASSERT(Col_ListIterMoveTo(it, CYCLIC_LIST_LEN - 1) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CYCLIC_LIST_LEN - 1);
    PICOTEST_ASSERT(Col_ListIterMoveTo(it, CYCLIC_LIST_LEN) == 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CYCLIC_LIST_HEAD_LEN);
    PICOTEST_ASSERT(Col_ListIterMoveTo(it, CYCLIC_LIST_LEN +
                                               CYCLIC_LIST_TAIL_LEN * 10 + 5) ==
                    1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CYCLIC_LIST_HEAD_LEN + 5);
    PICOTEST_ASSERT(
        Col_ListIterMoveTo(it, CYCLIC_LIST_LEN + CYCLIC_LIST_TAIL_LEN * 10 - 20) ==
        1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == CYCLIC_LIST_LEN - 20);
}

PICOTEST_SUITE(testListIteratorEmptyList, testEmptyListIterBegin,
               testEmptyListIterFirst, testEmptyListIterLast);
static void checkListIterEmpty(Col_ListIterator it) {
    PICOTEST_ASSERT(!Col_ListIterNull(it));
    PICOTEST_ASSERT(Col_ListIterList(it) == Col_EmptyList());
    PICOTEST_ASSERT(Col_ListIterLength(it) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
}
PICOTEST_CASE(testEmptyListIterBegin, colibriFixture) {
    Col_Word empty = Col_EmptyList();
    Col_ListIterator it;

    Col_ListIterBegin(it, empty, 0);
    checkListIterEmpty(it);
}
PICOTEST_CASE(testEmptyListIterFirst, colibriFixture) {
    Col_Word empty = Col_EmptyList();
    Col_ListIterator it;

    Col_ListIterFirst(it, empty);
    checkListIterEmpty(it);
}
PICOTEST_CASE(testEmptyListIterLast, colibriFixture) {
    Col_Word empty = Col_EmptyList();
    Col_ListIterator it;

    Col_ListIterLast(it, empty);
    checkListIterEmpty(it);
}

PICOTEST_SUITE(testListIteratorArray, testListIterArray, testListIterArrayAt,
               testListIterArrayFromEnd);
PICOTEST_CASE(testListIterArray, colibriFixture) {
    DECLARE_LIST_DATA(data, LARGE_LIST_LEN, Col_NewCharWord('a'),
                      Col_NewCharWord('a' + i));
    Col_ListIterator it;

    Col_ListIterArray(it, LARGE_LIST_LEN, data);
    PICOTEST_ASSERT(!Col_ListIterNull(it));
    PICOTEST_ASSERT(Col_ListIterList(it) == WORD_NIL);
    PICOTEST_ASSERT(Col_ListIterLength(it) == LARGE_LIST_LEN);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
    PICOTEST_ASSERT(Col_ListIterAt(it) == data[0]);
}
PICOTEST_CASE(testListIterArrayAt, colibriFixture) {
    DECLARE_LIST_DATA(data, LARGE_LIST_LEN, Col_NewCharWord('a'),
                      Col_NewCharWord('a' + i));
    Col_ListIterator it;

    size_t index;

    index = 0;
    Col_ListIterArray(it, LARGE_LIST_LEN, data);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
    PICOTEST_ASSERT(Col_ListIterAt(it) == data[index]);

    index = LARGE_LIST_LEN / 2;
    Col_ListIterMoveTo(it, LARGE_LIST_LEN / 2);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
    PICOTEST_ASSERT(Col_ListIterAt(it) == data[index]);

    Col_ListIterForward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
}
PICOTEST_CASE(testListIterArrayFromEnd, colibriFixture) {
    DECLARE_LIST_DATA(data, LARGE_LIST_LEN, Col_NewCharWord('a'),
                      Col_NewCharWord('a' + i));
    Col_ListIterator it;

    size_t index;

    index = 0;
    Col_ListIterArray(it, LARGE_LIST_LEN, data);
    Col_ListIterMoveTo(it, LARGE_LIST_LEN / 2);
    Col_ListIterForward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListIterEnd(it));

    index = LARGE_LIST_LEN - 1;
    Col_ListIterPrevious(it);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
    PICOTEST_ASSERT(Col_ListIterAt(it) == data[index]);

    Col_ListIterForward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListIterEnd(it));

    index = LARGE_LIST_LEN / 2;
    Col_ListIterMoveTo(it, index);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
    PICOTEST_ASSERT(Col_ListIterAt(it) == data[index]);
}
