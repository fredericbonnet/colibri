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

/* Col_TraverseListChunks */
PICOTEST_CASE(traverseListChunks_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    PICOTEST_ASSERT(
        Col_TraverseListChunks(WORD_NIL, 0, 0, 0, NULL, NULL, NULL) == -1);
}

/* Col_ListIterBegin */
PICOTEST_CASE(listIterBegin_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LIST);
    Col_ListIterator it;
    Col_ListIterBegin(it, WORD_NIL, 0);
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
    Col_ListIterForward(it, 0);
    PICOTEST_ASSERT(Col_ListIterNull(it));
}
PICOTEST_CASE(listIterForward_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_LISTITER_END);
    Col_ListIterator it;
    Col_ListIterFirst(it, Col_EmptyList());
    PICOTEST_ASSERT(Col_ListIterEnd(it));
    Col_ListIterForward(it, 1);
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
#define FLAT_LIST_LEN 200
static Col_Word FLAT_LIST()
    NEW_LIST(FLAT_LIST_LEN, Col_NewCharWord('a'), Col_NewCharWord('a' + i));
#define VOID_LIST_LEN 1000
#define VOID_LIST() Col_NewList(VOID_LIST_LEN, NULL)

/*
 * Lists
 */

#include "hooks.h"
#include "colibriFixture.h"

PICOTEST_SUITE(testLists, testListTypeChecks, testEmptyList, testListCreation,
               testListOperations, testListTraversal, testListIteration);

PICOTEST_CASE(testListTypeChecks, colibriFixture) {
    PICOTEST_VERIFY(listLength_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(listDepth_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(listAt_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(sublist_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(concatLists_typeCheck_left(NULL) == 1);
    PICOTEST_VERIFY(concatLists_typeCheck_right(NULL) == 1);
    PICOTEST_VERIFY(repeatList_typeCheck(NULL) == 1);
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

PICOTEST_CASE(testNewListEmpty, colibriFixture) {
    PICOTEST_ASSERT(Col_NewList(0, NULL) == Col_EmptyList());
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

/* List operations */
PICOTEST_SUITE(testListOperations, testSublist, testConcatLists,
               testConcatListsA, testConcatListsNV, testConcatListsV,
               testRepeatList);
// TODO repeat/insert/remove/replace

// TODO compare original/sublist characters
PICOTEST_SUITE(testSublist, testSublistOfEmptyListIsEmpty,
               testSublistOfEmptyRangeIsEmpty,
               testSublistOfWholeRangeIsIdentity,
               testSublistOfVoidListIsNewVoidList,
               testSublistOfFlatListIsNewList, testSublistOfSublistIsNewList,
               testSublistOfConcatList);

PICOTEST_CASE(testSublistOfEmptyListIsEmpty, colibriFixture) {
    PICOTEST_ASSERT(Col_Sublist(Col_EmptyList(), 0, 0) == Col_EmptyList());
}
PICOTEST_CASE(testSublistOfEmptyRangeIsEmpty, colibriFixture) {
    Col_Word list = SMALL_LIST();
    PICOTEST_ASSERT(Col_Sublist(list, 1, 0) == Col_EmptyList());
    PICOTEST_ASSERT(Col_Sublist(list, 2, 1) == Col_EmptyList());
    PICOTEST_ASSERT(Col_Sublist(list, SMALL_LIST_LEN, SMALL_LIST_LEN) ==
                    Col_EmptyList());
    PICOTEST_ASSERT(Col_Sublist(list, SIZE_MAX, 2) == Col_EmptyList());
    PICOTEST_ASSERT(Col_Sublist(list, SIZE_MAX, SIZE_MAX) == Col_EmptyList());
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
}
PICOTEST_CASE(testSublistOfFlatListIsNewList, colibriFixture) {
    Col_Word list = FLAT_LIST();

    Col_Word sublist = Col_Sublist(list, 1, SIZE_MAX);
    PICOTEST_ASSERT(sublist != Col_Sublist(list, 1, SIZE_MAX));
    checkList(sublist, FLAT_LIST_LEN - 1, 0);
}
PICOTEST_CASE(testSublistOfSublistIsNewList, colibriFixture) {
    Col_Word list = Col_Sublist(FLAT_LIST(), 1, SIZE_MAX);

    Col_Word sublist = Col_Sublist(list, 1, SIZE_MAX);
    PICOTEST_ASSERT(sublist != Col_Sublist(list, 1, SIZE_MAX));
    checkList(sublist, FLAT_LIST_LEN - 2, 0);
}

PICOTEST_SUITE(testSublistOfConcatList, testSublistOfConcatListArms,
               testShortSublistOfConcatList);
PICOTEST_CASE(testSublistOfConcatListArms, colibriFixture) {
    Col_Word left = FLAT_LIST(), right = SMALL_LIST();
    PICOTEST_ASSERT(left != right);
    Col_Word list = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_Sublist(list, 0, FLAT_LIST_LEN - 1) == left);
    PICOTEST_ASSERT(Col_Sublist(list, FLAT_LIST_LEN, SIZE_MAX) == right);
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
    checkMergedSublist(FLAT_LIST(), SMALL_LIST(), 2, 0);
    checkMergedSublist(SMALL_LIST(), FLAT_LIST(), 2, 0);
    checkMergedSublist(FLAT_LIST(), FLAT_LIST(), 2, 0);
    checkMergedSublist(VOID_LIST(), VOID_LIST(), 2, 1);
    checkMergedSublist(FLAT_LIST(), VOID_LIST(), 2, 0);
}
static void checkUnmergedSublist(Col_Word left, Col_Word right, size_t delta) {
    size_t leftLen = Col_ListLength(left), rightLen = Col_ListLength(right);
    Col_Word list = Col_ConcatLists(left, right);
    Col_Word sublist = Col_Sublist(list, leftLen - delta, leftLen + delta);
    checkList(list, leftLen + rightLen, 1);
    checkList(sublist, delta * 2 + 1, 1);
}
PICOTEST_CASE(testUnmergeableSublists, colibriFixture) {
    checkUnmergedSublist(FLAT_LIST(), FLAT_LIST(), 10);
    checkUnmergedSublist(FLAT_LIST(), VOID_LIST(), 10);
    checkUnmergedSublist(VOID_LIST(), FLAT_LIST(), 10);
}

PICOTEST_SUITE(testConcatLists, testConcatListsErrors,
               testConcatListWithEmptyIsIdentity, testConcatShortLists,
               testConcatFlatListsIsNewList,
               testConcatAdjacentSublistsIsOriginalList,
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
    checkUnmergedConcatList(SMALL_LIST(), FLAT_LIST(), 1);
    checkUnmergedConcatList(SMALL_LIST(), VOID_LIST(), 1);
    checkUnmergedConcatList(FLAT_LIST(), SMALL_VOID_LIST(), 1);
    checkUnmergedConcatList(VOID_LIST(), FLAT_LIST(), 1);
    checkUnmergedConcatList(FLAT_LIST(), FLAT_LIST(), 1);
    checkUnmergedConcatList(FLAT_LIST(), VOID_LIST(), 1);
}

PICOTEST_CASE(testConcatFlatListsIsNewList, colibriFixture) {
    Col_Word list = FLAT_LIST();
    Col_Word concatList = Col_ConcatLists(list, list);
    PICOTEST_ASSERT(concatList != Col_ConcatLists(list, list));
    checkList(concatList, FLAT_LIST_LEN * 2, 1);
}
PICOTEST_CASE(testConcatAdjacentSublistsIsOriginalList, colibriFixture) {
    Col_Word list = FLAT_LIST();
    size_t split = FLAT_LIST_LEN / 2;
    Col_Word left = Col_Sublist(list, 0, split - 1);
    Col_Word right = Col_Sublist(list, split, SIZE_MAX);
    PICOTEST_ASSERT(Col_ConcatLists(left, right) == list);
}

PICOTEST_SUITE(testConcatListBalancing,
               testConcatBalancedListBranchesAreNotRebalanced,
               testConcatInbalancedLeftOuterListBranchesAreRotated,
               testConcatInbalancedRightOuterListBranchesAreRotated,
               testConcatInbalancedLeftInnerListBranchesAreSplit,
               testConcatInbalancedRightInnerListBranchesAreSplit,
               testConcatInbalancedSublistListBranchesAreSplit);
PICOTEST_CASE(testConcatBalancedListBranchesAreNotRebalanced, colibriFixture) {
    Col_Word leaf[8];
    for (int i = 0; i < 8; i++) {
        leaf[i] = FLAT_LIST();
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
        PICOTEST_ASSERT(Col_Sublist(left, FLAT_LIST_LEN * i,
                                    FLAT_LIST_LEN * (i + 1) - 1) == leaf[i]);
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
        PICOTEST_ASSERT(Col_Sublist(right, FLAT_LIST_LEN * i,
                                    FLAT_LIST_LEN * (i + 1) - 1) ==
                        leaf[i + 4]);
    }

    Col_Word list = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_ListDepth(list) == 3);
    for (int i = 0; i < 8; i++) {
        PICOTEST_ASSERT(Col_Sublist(list, FLAT_LIST_LEN * i,
                                    FLAT_LIST_LEN * (i + 1) - 1) == leaf[i]);
    }
}
PICOTEST_CASE(testConcatInbalancedLeftOuterListBranchesAreRotated,
              colibriFixture) {
    Col_Word leaf[4];
    for (int i = 0; i < 4; i++) {
        leaf[i] = FLAT_LIST();
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
        PICOTEST_ASSERT(Col_Sublist(list, FLAT_LIST_LEN * i,
                                    FLAT_LIST_LEN * (i + 1) - 1) == leaf[i]);
    }
}
PICOTEST_CASE(testConcatInbalancedRightOuterListBranchesAreRotated,
              colibriFixture) {
    Col_Word leaf[4];
    for (int i = 0; i < 4; i++) {
        leaf[i] = FLAT_LIST();
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
        PICOTEST_ASSERT(Col_Sublist(list, FLAT_LIST_LEN * i,
                                    FLAT_LIST_LEN * (i + 1) - 1) == leaf[i]);
    }
}
PICOTEST_CASE(testConcatInbalancedLeftInnerListBranchesAreSplit,
              colibriFixture) {
    Col_Word leaf[4];
    for (int i = 0; i < 4; i++) {
        leaf[i] = FLAT_LIST();
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
        PICOTEST_ASSERT(Col_Sublist(list, FLAT_LIST_LEN * i,
                                    FLAT_LIST_LEN * (i + 1) - 1) == leaf[i]);
    }
}
PICOTEST_CASE(testConcatInbalancedRightInnerListBranchesAreSplit,
              colibriFixture) {
    Col_Word leaf[4];
    for (int i = 0; i < 4; i++) {
        leaf[i] = FLAT_LIST();
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
        PICOTEST_ASSERT(Col_Sublist(list, FLAT_LIST_LEN * i,
                                    FLAT_LIST_LEN * (i + 1) - 1) == leaf[i]);
    }
}
PICOTEST_CASE(testConcatInbalancedSublistListBranchesAreSplit, colibriFixture) {
    Col_Word leaf = FLAT_LIST();
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
               testConcatListsATwo, testConcatListsARecurse);

PICOTEST_SUITE(testConcatListsAErrors, testConcatListsAZero);
PICOTEST_CASE(testConcatListsAZero, colibriFixture) {
    PICOTEST_ASSERT(concatListsA_valueCheck_number(NULL) == 1);
}

PICOTEST_CASE(testConcatListsAOne, colibriFixture) {
    Col_Word lists[] = {FLAT_LIST()};
    Col_Word list = Col_ConcatListsA(1, lists);
    PICOTEST_ASSERT(list == lists[0]);
}
PICOTEST_CASE(testConcatListsATwo, colibriFixture) {
    Col_Word lists[] = {FLAT_LIST(), FLAT_LIST()};
    Col_Word list = Col_ConcatListsA(2, lists);
    checkList(list, FLAT_LIST_LEN * 2, 1);
}
PICOTEST_CASE(testConcatListsARecurse, colibriFixture) {
    Col_Word lists[] = {FLAT_LIST(), SMALL_LIST(), FLAT_LIST()};
    Col_Word list = Col_ConcatListsA(3, lists);
    checkList(list, FLAT_LIST_LEN * 2 + SMALL_LIST_LEN, 2);
}

PICOTEST_SUITE(testConcatListsNV, testConcatListsNVErrors);

PICOTEST_SUITE(testConcatListsNVErrors, testConcatListsNVZero);
PICOTEST_CASE(testConcatListsNVZero, colibriFixture) {
    PICOTEST_ASSERT(concatListsNV_valueCheck_number(NULL) == 1);
}

PICOTEST_SUITE(testConcatListsV, testConcatListsVOne, testConcatListsVTwo,
               testConcatListsVRecurse);

PICOTEST_CASE(testConcatListsVOne, colibriFixture) {
    Col_Word leaf = FLAT_LIST();
    Col_Word list = Col_ConcatListsV(leaf);
    PICOTEST_ASSERT(list == leaf);
}
PICOTEST_CASE(testConcatListsVTwo, colibriFixture) {
    Col_Word list = Col_ConcatListsV(FLAT_LIST(), FLAT_LIST());
    checkList(list, FLAT_LIST_LEN * 2, 1);
}
PICOTEST_CASE(testConcatListsVRecurse, colibriFixture) {
    Col_Word list = Col_ConcatListsV(FLAT_LIST(), SMALL_LIST(), FLAT_LIST());
    checkList(list, FLAT_LIST_LEN * 2 + SMALL_LIST_LEN, 2);
}

PICOTEST_SUITE(testRepeatList, testRepeatListErrors, testRepeatListZeroIsEmpty,
               testRepeatListOnceIsIdentity, testRepeatListTwiceIsConcatSelf,
               testRepeatListRecurse, testRepeatEmptyList, testRepeatListMax);

PICOTEST_SUITE(testRepeatListErrors, testRepeatListTooLarge);
PICOTEST_CASE(testRepeatListTooLarge, colibriFixture) {
    PICOTEST_ASSERT(repeatList_valueCheck_length(NULL) == 1);
}

PICOTEST_CASE(testRepeatListZeroIsEmpty, colibriFixture) {
    Col_Word list = FLAT_LIST();
    PICOTEST_ASSERT(Col_RepeatList(list, 0) == Col_EmptyList());
}
PICOTEST_CASE(testRepeatListOnceIsIdentity, colibriFixture) {
    Col_Word list = FLAT_LIST();
    PICOTEST_ASSERT(Col_RepeatList(list, 1) == list);
}
PICOTEST_CASE(testRepeatListTwiceIsConcatSelf, colibriFixture) {
    Col_Word list = FLAT_LIST();
    Col_Word repeat = Col_RepeatList(list, 2);
    checkList(repeat, FLAT_LIST_LEN * 2, 1);
    PICOTEST_ASSERT(Col_Sublist(repeat, 0, FLAT_LIST_LEN - 1) == list);
    PICOTEST_ASSERT(Col_Sublist(repeat, FLAT_LIST_LEN, SIZE_MAX) == list);
}
PICOTEST_CASE(testRepeatListRecurse, colibriFixture) {
    Col_Word list = FLAT_LIST();
    checkList(Col_RepeatList(list, 3), FLAT_LIST_LEN * 3, 2);
    checkList(Col_RepeatList(list, 4), FLAT_LIST_LEN * 4, 2);
    checkList(Col_RepeatList(list, 8), FLAT_LIST_LEN * 8, 3);
    checkList(Col_RepeatList(list, 16), FLAT_LIST_LEN * 16, 4);
    checkList(Col_RepeatList(list, 32), FLAT_LIST_LEN * 32, 5);
    checkList(Col_RepeatList(list, 65536), FLAT_LIST_LEN * 65536, 16);
    checkList(Col_RepeatList(list, 65537), FLAT_LIST_LEN * 65537, 17);
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

PICOTEST_SUITE(testListTraversal, testTraverseList);
// TODO reverse traversal and Col_TraverseListN

PICOTEST_SUITE(testTraverseList, testTraverseListProcMustNotBeNull,
               testTraverseEmptyListIsNoop, testTraverseFlatListHasOneChunk,
               testTraverseSublist, testTraverseConcatList,
               testTraverseListRange, testTraverseListBreak);
// TODO void list

#define CHUNK_SAMPLE_SIZE 10
typedef struct ListChunkInfo {
    size_t length;
    const Col_Word *chunk;
    Col_Word sample[CHUNK_SAMPLE_SIZE];
} ListChunkInfo;
typedef struct ListChunkCounterData {
    size_t size;
    size_t nbChunks;
    ListChunkInfo *infos;
} ListChunkCounterData;
#define DECLARE_CHUNKS_DATA(name, size)                                        \
    ListChunkInfo name##datas[size];                                           \
    ListChunkCounterData name = {size, 0, name##datas};

static int listChunkCounter(size_t index, size_t length, size_t number,
                            const Col_Word **chunks,
                            Col_ClientData clientData) {
    ListChunkCounterData *data = (ListChunkCounterData *)clientData;
    PICOTEST_ASSERT(number == 1);
    PICOTEST_ASSERT(data != NULL);
    PICOTEST_ASSERT(data->nbChunks < data->size);

    ListChunkInfo *info = data->infos + data->nbChunks++;
    info->length = length;
    info->chunk = *chunks;
    memcpy(info->sample, *chunks,
           (length < CHUNK_SAMPLE_SIZE ? length : CHUNK_SAMPLE_SIZE) *
               sizeof(Col_Word));
    return 0;
}

PICOTEST_CASE(testTraverseEmptyListIsNoop, colibriFixture) {
    PICOTEST_ASSERT(Col_TraverseListChunks(Col_EmptyList(), 0, 0, 0, NULL, NULL,
                                           NULL) == -1);
}
PICOTEST_CASE(testTraverseListProcMustNotBeNull, colibriFixture) {
    // TODO VALUECHECK proc
}
PICOTEST_CASE(testTraverseFlatListHasOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Word list = FLAT_LIST();
    PICOTEST_ASSERT(Col_TraverseListChunks(list, 0, SIZE_MAX, 0,
                                           listChunkCounter, &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(data.infos[0].length == FLAT_LIST_LEN);
}

PICOTEST_SUITE(testTraverseSublist,
               testTraverseSublistOfFlatListHasOneOffsetChunk);
// TODO complex concat scenarios
PICOTEST_CASE(testTraverseSublistOfFlatListHasOneOffsetChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(listData, 1);
    DECLARE_CHUNKS_DATA(sublistData, 1);
    Col_Word list = FLAT_LIST();
    Col_Word sublist = Col_Sublist(list, 1, SIZE_MAX);
    PICOTEST_ASSERT(Col_TraverseListChunks(list, 1, SIZE_MAX, 0,
                                           listChunkCounter, &listData,
                                           NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseListChunks(sublist, 0, SIZE_MAX, 0,
                                           listChunkCounter, &sublistData,
                                           NULL) == 0);
    PICOTEST_ASSERT(listData.nbChunks == 1);
    PICOTEST_ASSERT(sublistData.nbChunks == 1);
    PICOTEST_ASSERT(listData.infos[0].length == sublistData.infos[0].length);
    PICOTEST_ASSERT(listData.infos[0].chunk == sublistData.infos[0].chunk);
}

PICOTEST_SUITE(testTraverseConcatList, testTraverseConcatListsHaveTwoChunks,
               testTraverseConcatListsLeftArmHaveOneChunk,
               testTraverseConcatListsRightArmHaveOneChunk,
               testTraverseConcatSublistHasTwoChunks);
PICOTEST_CASE(testTraverseConcatListsHaveTwoChunks, colibriFixture) {
    DECLARE_CHUNKS_DATA(leftData, 1);
    DECLARE_CHUNKS_DATA(rightData, 1);
    DECLARE_CHUNKS_DATA(concatListData, 2);
    Col_Word left = FLAT_LIST(), right = SMALL_LIST();
    Col_Word concatList = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_TraverseListChunks(left, 1, SIZE_MAX, 0,
                                           listChunkCounter, &leftData,
                                           NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseListChunks(right, 0, 5, 0, listChunkCounter,
                                           &rightData, NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseListChunks(concatList, 1, FLAT_LIST_LEN + 4, 0,
                                           listChunkCounter, &concatListData,
                                           NULL) == 0);
    PICOTEST_ASSERT(leftData.nbChunks == 1);
    PICOTEST_ASSERT(rightData.nbChunks == 1);
    PICOTEST_ASSERT(concatListData.nbChunks == 2);
    PICOTEST_ASSERT(leftData.infos[0].length == concatListData.infos[0].length);
    PICOTEST_ASSERT(leftData.infos[0].chunk == concatListData.infos[0].chunk);
    PICOTEST_ASSERT(rightData.infos[0].length ==
                    concatListData.infos[1].length);
    PICOTEST_ASSERT(rightData.infos[0].chunk == concatListData.infos[1].chunk);
}
PICOTEST_CASE(testTraverseConcatListsLeftArmHaveOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(leftData, 1);
    DECLARE_CHUNKS_DATA(concatListData, 1);
    Col_Word left = FLAT_LIST(), right = SMALL_LIST();
    Col_Word concatList = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_TraverseListChunks(left, 1, 5, 0, listChunkCounter,
                                           &leftData, NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseListChunks(concatList, 1, 5, 0,
                                           listChunkCounter, &concatListData,
                                           NULL) == 0);
    PICOTEST_ASSERT(concatListData.nbChunks == 1);
    PICOTEST_ASSERT(leftData.infos[0].length == concatListData.infos[0].length);
    PICOTEST_ASSERT(leftData.infos[0].chunk == concatListData.infos[0].chunk);
}
PICOTEST_CASE(testTraverseConcatListsRightArmHaveOneChunk, colibriFixture) {
    DECLARE_CHUNKS_DATA(rightData, 1);
    DECLARE_CHUNKS_DATA(concatListData, 1);
    Col_Word left = FLAT_LIST(), right = SMALL_LIST();
    Col_Word concatList = Col_ConcatLists(left, right);
    PICOTEST_ASSERT(Col_TraverseListChunks(right, 1, 5, 0, listChunkCounter,
                                           &rightData, NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseListChunks(concatList, FLAT_LIST_LEN + 1, 5, 0,
                                           listChunkCounter, &concatListData,
                                           NULL) == 0);
    PICOTEST_ASSERT(concatListData.nbChunks == 1);
    PICOTEST_ASSERT(rightData.infos[0].length ==
                    concatListData.infos[0].length);
    PICOTEST_ASSERT(rightData.infos[0].chunk == concatListData.infos[0].chunk);
}
PICOTEST_CASE(testTraverseConcatSublistHasTwoChunks, colibriFixture) {
    DECLARE_CHUNKS_DATA(leftData, 1);
    DECLARE_CHUNKS_DATA(rightData, 1);
    DECLARE_CHUNKS_DATA(sublistData, 2);
    Col_Word left = FLAT_LIST(), right = FLAT_LIST();
    Col_Word concatList = Col_ConcatLists(left, right);
    Col_Word sublist = Col_Sublist(concatList, 1, FLAT_LIST_LEN * 2 - 2);
    PICOTEST_ASSERT(Col_TraverseListChunks(left, 2, SIZE_MAX, 0,
                                           listChunkCounter, &leftData,
                                           NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseListChunks(right, 0, 6, 0, listChunkCounter,
                                           &rightData, NULL) == 0);
    PICOTEST_ASSERT(Col_TraverseListChunks(sublist, 1, FLAT_LIST_LEN + 4, 0,
                                           listChunkCounter, &sublistData,
                                           NULL) == 0);
    PICOTEST_ASSERT(leftData.nbChunks == 1);
    PICOTEST_ASSERT(rightData.nbChunks == 1);
    PICOTEST_ASSERT(sublistData.nbChunks == 2);
    PICOTEST_ASSERT(leftData.infos[0].length == sublistData.infos[0].length);
    PICOTEST_ASSERT(leftData.infos[0].chunk == sublistData.infos[0].chunk);
    PICOTEST_ASSERT(rightData.infos[0].length == sublistData.infos[1].length);
    PICOTEST_ASSERT(rightData.infos[0].chunk == sublistData.infos[1].chunk);
}

PICOTEST_SUITE(testTraverseListRange, testTraverseSingleElement);
static void checkTraverseElement(Col_Word list, size_t index) {
    DECLARE_CHUNKS_DATA(data, 1);
    Col_Word e = Col_ListAt(list, index);
    PICOTEST_ASSERT(Col_TraverseListChunks(list, index, 1, 0, listChunkCounter,
                                           &data, NULL) == 0);
    PICOTEST_ASSERT(data.nbChunks == 1);
    PICOTEST_ASSERT(data.infos[0].length == 1);
    PICOTEST_ASSERT(*(data.infos[0].chunk) == e);
}
PICOTEST_CASE(testTraverseSingleElement, colibriFixture) {
    Col_Word list = FLAT_LIST();
    checkTraverseElement(list, 0);
    checkTraverseElement(list, FLAT_LIST_LEN / 2);
    checkTraverseElement(list, FLAT_LIST_LEN - 1);
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
    Col_Word list = Col_RepeatList(FLAT_LIST(), 10);
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
               testListIterBeginMax, testListIterFirst, testListIterLast,
               testListIterSet);

PICOTEST_CASE(testListIterNull, colibriFixture) {
    Col_ListIterator it = COL_LISTITER_NULL;
    PICOTEST_ASSERT(Col_ListIterNull(it));
    PICOTEST_ASSERT(Col_ListIterList(it) == WORD_NIL);
    PICOTEST_ASSERT(Col_ListIterLength(it) == 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
}
PICOTEST_CASE(testListIterBegin, colibriFixture) {
    Col_Word list = FLAT_LIST();
    size_t index = FLAT_LIST_LEN / 2;
    Col_ListIterator it;
    Col_ListIterBegin(it, list, index);
    PICOTEST_ASSERT(!Col_ListIterNull(it));
    PICOTEST_ASSERT(Col_ListIterList(it) == list);
    PICOTEST_ASSERT(Col_ListIterLength(it) == FLAT_LIST_LEN);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
}
PICOTEST_CASE(testListIterBeginMax, colibriFixture) {
    Col_Word list = FLAT_LIST();
    size_t index = FLAT_LIST_LEN / 2;
    Col_ListIterator it;
    Col_ListIterBegin(it, list, SIZE_MAX);
    PICOTEST_ASSERT(!Col_ListIterNull(it));
    PICOTEST_ASSERT(Col_ListIterList(it) == list);
    PICOTEST_ASSERT(Col_ListIterLength(it) == FLAT_LIST_LEN);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == FLAT_LIST_LEN);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
}
PICOTEST_CASE(testListIterFirst, colibriFixture) {
    Col_Word list = FLAT_LIST();
    Col_ListIterator it;
    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(!Col_ListIterNull(it));
    PICOTEST_ASSERT(Col_ListIterList(it) == list);
    PICOTEST_ASSERT(Col_ListIterLength(it) == FLAT_LIST_LEN);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
}
PICOTEST_CASE(testListIterLast, colibriFixture) {
    Col_Word list = FLAT_LIST();
    Col_ListIterator it;
    Col_ListIterLast(it, list);
    PICOTEST_ASSERT(!Col_ListIterNull(it));
    PICOTEST_ASSERT(Col_ListIterList(it) == list);
    PICOTEST_ASSERT(Col_ListIterLength(it) == FLAT_LIST_LEN);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == FLAT_LIST_LEN - 1);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
}
PICOTEST_CASE(testListIterSet, colibriFixture) {
    Col_Word list = FLAT_LIST();
    Col_ListIterator it1;

    Col_ListIterBegin(it1, list, FLAT_LIST_LEN / 2);
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
    Col_Word list = FLAT_LIST();
    Col_ListIterator it1, it2;

    Col_ListIterFirst(it1, list);
    Col_ListIterBegin(it2, list, 0);
    PICOTEST_ASSERT(Col_ListIterCompare(it1, it2) == 0);
    PICOTEST_ASSERT(Col_ListIterCompare(it2, it1) == 0);
}
PICOTEST_CASE(testListIteratorCompareLast, colibriFixture) {
    Col_Word list = FLAT_LIST();
    Col_ListIterator it1, it2;

    Col_ListIterLast(it1, list);
    Col_ListIterBegin(it2, list, FLAT_LIST_LEN - 1);
    PICOTEST_ASSERT(Col_ListIterCompare(it1, it2) == 0);
    PICOTEST_ASSERT(Col_ListIterCompare(it2, it1) == 0);
}
PICOTEST_CASE(testListIteratorCompareEnd, colibriFixture) {
    Col_Word list = FLAT_LIST();
    Col_ListIterator it1, it2;

    Col_ListIterLast(it1, list);
    Col_ListIterBegin(it2, list, FLAT_LIST_LEN);
    PICOTEST_ASSERT(Col_ListIterEnd(it2));
    PICOTEST_ASSERT(Col_ListIterCompare(it1, it2) < 0);
    PICOTEST_ASSERT(Col_ListIterCompare(it2, it1) > 0);
}
PICOTEST_CASE(testListIteratorCompareMiddle, colibriFixture) {
    Col_Word list = FLAT_LIST();
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
    checkListIterAt(FLAT_LIST());
    checkListIterAt(FLAT_LIST());
    checkListIterAt(FLAT_LIST());
    checkListIterAt(FLAT_LIST());
    checkListIterAt(FLAT_LIST());
}
PICOTEST_CASE(testListIterAtSublist, colibriFixture) {
    checkListIterAt(Col_Sublist(FLAT_LIST(), 1, SIZE_MAX));
}
PICOTEST_CASE(testListIterAtConcatList, colibriFixture) {
    checkListIterAt(Col_ConcatLists(FLAT_LIST(), SMALL_LIST()));
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
    checkListIterNextIterAt(FLAT_LIST());
    checkListIterNextIterAt(FLAT_LIST());
    checkListIterNextIterAt(FLAT_LIST());
    checkListIterNextIterAt(FLAT_LIST());
    checkListIterNextIterAt(FLAT_LIST());
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
    checkListIterPreviousIterAt(FLAT_LIST());
    checkListIterPreviousIterAt(FLAT_LIST());
    checkListIterPreviousIterAt(FLAT_LIST());
    checkListIterPreviousIterAt(FLAT_LIST());
    checkListIterPreviousIterAt(FLAT_LIST());
}

PICOTEST_SUITE(testListIterForward, testListIterForwardZeroIsNoop,
               testListIterForwardMaxIsAtEnd, testListIterForwardIterAt);
PICOTEST_CASE(testListIterForwardZeroIsNoop, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;

    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
    Col_ListIterForward(it, 0);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
}
PICOTEST_CASE(testListIterForwardMaxIsAtEnd, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;

    Col_ListIterFirst(it, list);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
    Col_ListIterForward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
}

static void checkListIterForwardIterAt(Col_Word list) {
    Col_Word e;
    Col_ListIterator it;

    Col_ListIterFirst(it, list);
    Col_TraverseListChunks(list, Col_ListIterIndex(it), 1, 0,
                           listChunkGetElement, &e, NULL);
    PICOTEST_ASSERT(Col_ListIterAt(it) == e);

    Col_ListIterForward(it, 1);
    Col_TraverseListChunks(list, Col_ListIterIndex(it), 1, 0,
                           listChunkGetElement, &e, NULL);
    PICOTEST_ASSERT(Col_ListIterAt(it) == e);
}
PICOTEST_CASE(testListIterForwardIterAt, colibriFixture) {
    checkListIterForwardIterAt(SMALL_LIST());
    checkListIterForwardIterAt(FLAT_LIST());
    checkListIterForwardIterAt(FLAT_LIST());
    checkListIterForwardIterAt(FLAT_LIST());
    checkListIterForwardIterAt(FLAT_LIST());
    checkListIterForwardIterAt(FLAT_LIST());
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
    checkListIterBackwardIterAt(FLAT_LIST());
    checkListIterBackwardIterAt(FLAT_LIST());
    checkListIterBackwardIterAt(FLAT_LIST());
    checkListIterBackwardIterAt(FLAT_LIST());
    checkListIterBackwardIterAt(FLAT_LIST());
}

PICOTEST_SUITE(testListIterMoveTo, testListIterMoveToForward,
               testListIterMoveToBackward);
PICOTEST_CASE(testListIterMoveToForward, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;
    size_t index = SMALL_LIST_LEN / 2;

    Col_ListIterBegin(it, list, index);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
    Col_ListIterMoveTo(it, index + 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index + 1);
}
PICOTEST_CASE(testListIterMoveToBackward, colibriFixture) {
    Col_Word list = SMALL_LIST();
    Col_ListIterator it;
    size_t index = SMALL_LIST_LEN / 2;

    Col_ListIterBegin(it, list, index);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
    Col_ListIterMoveTo(it, index - 1);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index - 1);
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
    DECLARE_LIST_DATA(data, FLAT_LIST_LEN, Col_NewCharWord('a'),
                      Col_NewCharWord('a' + i));
    Col_ListIterator it;

    Col_ListIterArray(it, FLAT_LIST_LEN, data);
    PICOTEST_ASSERT(!Col_ListIterNull(it));
    PICOTEST_ASSERT(Col_ListIterList(it) == WORD_NIL);
    PICOTEST_ASSERT(Col_ListIterLength(it) == FLAT_LIST_LEN);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == 0);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
    PICOTEST_ASSERT(Col_ListIterAt(it) == data[0]);
}
PICOTEST_CASE(testListIterArrayAt, colibriFixture) {
    DECLARE_LIST_DATA(data, FLAT_LIST_LEN, Col_NewCharWord('a'),
                      Col_NewCharWord('a' + i));
    Col_ListIterator it;

    size_t index;

    index = 0;
    Col_ListIterArray(it, FLAT_LIST_LEN, data);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
    PICOTEST_ASSERT(Col_ListIterAt(it) == data[index]);

    index = FLAT_LIST_LEN / 2;
    Col_ListIterMoveTo(it, FLAT_LIST_LEN / 2);
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
    PICOTEST_ASSERT(Col_ListIterAt(it) == data[index]);

    Col_ListIterForward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListIterEnd(it));
}
PICOTEST_CASE(testListIterArrayFromEnd, colibriFixture) {
    DECLARE_LIST_DATA(data, FLAT_LIST_LEN, Col_NewCharWord('a'),
                      Col_NewCharWord('a' + i));
    Col_ListIterator it;

    size_t index;

    index = 0;
    Col_ListIterArray(it, FLAT_LIST_LEN, data);
    Col_ListIterMoveTo(it, FLAT_LIST_LEN / 2);
    Col_ListIterForward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListIterEnd(it));

    index = FLAT_LIST_LEN - 1;
    Col_ListIterPrevious(it);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
    PICOTEST_ASSERT(Col_ListIterAt(it) == data[index]);

    Col_ListIterForward(it, SIZE_MAX);
    PICOTEST_ASSERT(Col_ListIterEnd(it));

    index = FLAT_LIST_LEN / 2;
    Col_ListIterMoveTo(it, index);
    PICOTEST_ASSERT(!Col_ListIterEnd(it));
    PICOTEST_ASSERT(Col_ListIterIndex(it) == index);
    PICOTEST_ASSERT(Col_ListIterAt(it) == data[index]);
}
