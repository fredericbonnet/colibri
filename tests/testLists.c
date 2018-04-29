#include <stdlib.h>
#include <limits.h>

#include <colibri.h>

#include "picotest.h"

#include "testColibri.h"

/*
 *---------------------------------------------------------------------------
 *
 * testLists --
 *
 *---------------------------------------------------------------------------
 */

static void listAccess(Col_Word list);
static void listInsert(Col_Word into, Col_Word list);
static void listRemove(Col_Word list);
static void listReplace(Col_Word list, Col_Word with);
static void listsEqual(Col_Word list1, Col_Word list2);

TEST_SUITE(testLists, 
    testNewList, testSublists, testListIterators, testAcyclicListOps,
    testCyclicListOps    
)

/*
 * List creation.
 */

TEST_SUITE(testNewList, 
    testNewSmallList, testNewLargeList
)


TEST_FIXTURE_SETUP(testNewList) {
    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();
}
TEST_FIXTURE_TEARDOWN(testNewList) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
}
TEST_CASE(testNewSmallList, testNewList) {
    Col_Word e[10];
    size_t l = sizeof(e)/sizeof(*e);
    size_t i;
    Col_ListIterator it;
    Col_Word list;

    for (i=0; i < l; i++) {
        e[i] = Col_NewIntWord(i);
    }

    list = Col_NewList(l, e);
    ASSERT(Col_ListLength(list) == l);
    listAccess(list);
    for (Col_ListIterFirst(it, list); !Col_ListIterEnd(it); Col_ListIterNext(it)) {
        size_t index = Col_ListIterIndex(it);
        Col_Word e = Col_ListIterAt(it);
        ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
        ASSERT(Col_IntWordValue(e) == (int) index, "index=%u", index);
    }
}
TEST_CASE(testNewLargeList, testNewList) {
    Col_Word e[1000];
    size_t l = sizeof(e)/sizeof(*e);
    size_t i;
    Col_ListIterator it;
    Col_Word list;

    for (i=0; i < l; i++) {
        e[i] = Col_NewIntWord(i);
    }

    list = Col_NewList(l, e);
    ASSERT(Col_ListLength(list) == l);
    listAccess(list);
    for (Col_ListIterFirst(it, list); !Col_ListIterEnd(it); Col_ListIterNext(it)) {
        size_t index = Col_ListIterIndex(it);
        Col_Word e = Col_ListIterAt(it);
        ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
        ASSERT(Col_IntWordValue(e) == (int) index, "index=%u", index);
    }
}


/*
 * Sublists.
 */

TEST_SUITE(testSublists, 
    testSmallSublists, testLargeSublists, testNestedSublists
)


TEST_FIXTURE_CONTEXT(testSublists) {
    Col_Word list;
};
TEST_FIXTURE_SETUP(testSublists, context) {
    Col_Word e[1000];
    size_t l = sizeof(e)/sizeof(*e);
    size_t i;

    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();

    for (i=0; i < l; i++) {
        e[i] = Col_NewIntWord(i);
    }

    context->list = Col_NewList(l, e);
}
TEST_FIXTURE_TEARDOWN(testSublists, context) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
}
TEST_CASE(testSmallSublists, testSublists, context) {
    size_t length = Col_ListLength(context->list);
    size_t offset, sublength=5;
    Col_Word sublist;
    Col_ListIterator it;

    for (offset=0; offset < length; offset++) {
        sublist = Col_Sublist(context->list, offset, offset+sublength-1);
        ASSERT(Col_ListLength(sublist) == (offset<=length-sublength?sublength:length-offset));

        listAccess(sublist);

        for (Col_ListIterFirst(it, sublist); !Col_ListIterEnd(it); Col_ListIterNext(it)) {
            size_t index = Col_ListIterIndex(it);
            Col_Word e = Col_ListIterAt(it);
            ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
            ASSERT(Col_IntWordValue(e) == (int) (index+offset), "index=%u", index);
        }
    }
}
TEST_CASE(testLargeSublists, testSublists, context) {
    size_t length = Col_ListLength(context->list);
    size_t offset, sublength=length/2;
    Col_Word sublist;
    Col_ListIterator it;

    for (offset=0; offset < length; offset++) {
        sublist = Col_Sublist(context->list, offset, offset+sublength-1);
        ASSERT(Col_ListLength(sublist) == (offset<=length-sublength?sublength:length-offset));

        listAccess(sublist);

        for (Col_ListIterFirst(it, sublist); !Col_ListIterEnd(it); Col_ListIterNext(it)) {
            size_t index = Col_ListIterIndex(it);
            Col_Word e = Col_ListIterAt(it);
            ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
            ASSERT(Col_IntWordValue(e) == (int) (index+offset), "index=%u", index);
        }
    }
}
TEST_CASE(testNestedSublists, testSublists, context) {
    /*
     * Level 2 nesting:
     *
     *  #0 [0             ...           999] = [<0...249>+<250...499>] + [<500...749>+<750...999>]
     *  #1    [50    ...449,550...   949]    =    [(50  ...   449)     +     (550   ...   949)]
     *  #2       [100...449,550...899]       =       (100             ...               899)
     */

    size_t length = Col_ListLength(context->list);
    size_t half = length/2;
    size_t index, offset;
    size_t level;

    Col_Word list = Col_Sublist(
        Col_ConcatLists(
            Col_Sublist(context->list, 50, half-51),
            Col_Sublist(context->list, half+50, length-51)
        ),
        50, length-251
    );

    ASSERT(Col_ListLength(list) == length-300);

    listAccess(list);

    offset=100;
    for (index=0; index < half-150; index++) {
        Col_Word e = Col_ListAt(list, index);
        ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
        ASSERT(Col_IntWordValue(e) == (int) (index+offset), "index=%u", index);
    }
    offset=200;
    for (; index < length-300; index++) {
        Col_Word e = Col_ListAt(list, index);
        ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
        ASSERT(Col_IntWordValue(e) == (int) (index+offset), "index=%u", index);
    }

    /* 
     * Further level nesting. 
     */

    list = context->list;
    for (level = 0; level < 5; level++) {
        list = Col_ConcatLists(list, list);
        list = Col_Sublist(list, 1, Col_ListLength(list)-2);

        listAccess(list);
    }
}


/*
 * Ayclic lists.
 */

TEST_SUITE(testAcyclicListOps,
    testAcyclicListAccess, testAcyclicListInsert, testAcyclicListRemove, 
    testAcyclicListReplace
)
TEST_FIXTURE_CONTEXT(testAcyclicListOps) {
    Col_Word small1;
    Col_Word small2;
    Col_Word large1;
    Col_Word large2;
    Col_Word sub1;
    Col_Word sub2;
    Col_Word smallVoid1;
    Col_Word smallVoid2;
    Col_Word largeVoid1;
    Col_Word largeVoid2;
};
TEST_FIXTURE_SETUP(testAcyclicListOps,context) {
    Col_Word e[1000];
    size_t i;

    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();

    for (i=0; i < 10; i++) {
        e[i] = Col_NewIntWord(i);
    }
    context->small1 = Col_NewList(10, e);

    for (i=0; i < 5; i++) {
        e[i] = Col_NewIntWord(i+1000);
    }
    context->small2 = Col_NewList(5, e);

    for (i=0; i < 1000; i++) {
        e[i] = Col_NewIntWord(i+2000);
    }
    context->large1 = Col_NewList(1000, e);

    for (i=0; i < 500; i++) {
        e[i] = Col_NewIntWord(i+3000);
    }
    context->large2 = Col_NewList(500, e);

    context->sub1 = Col_Sublist(context->large1, 1, 998);
    context->sub2 = Col_Sublist(context->large2, 1, 498);

    context->smallVoid1 = Col_NewList(10, NULL);
    context->smallVoid2 = Col_NewList(5, NULL);
    context->largeVoid1 = Col_NewList(1000, NULL);
    context->largeVoid2 = Col_NewList(500, NULL);
}
TEST_FIXTURE_TEARDOWN(testAcyclicListOps,context) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
}
TEST_CASE(testAcyclicListAccess, testAcyclicListOps,context) {
    listAccess(context->small1);
    listAccess(context->small2);
    listAccess(context->large1);
    listAccess(context->large2);
    listAccess(context->sub1);
    listAccess(context->sub2);
    listAccess(context->smallVoid1);
    listAccess(context->smallVoid2);
    listAccess(context->largeVoid1);
    listAccess(context->largeVoid2);
}
TEST_CASE(testAcyclicListInsert, testAcyclicListOps,context) {
    listInsert(context->small1, context->small2);
    listInsert(context->small2, context->small1);

    listInsert(context->large1, context->large2);
    listInsert(context->large2, context->large1);

    listInsert(context->sub1, context->sub2);
    listInsert(context->sub2, context->sub1);

    listInsert(context->smallVoid1, context->smallVoid2);
    listInsert(context->smallVoid2, context->smallVoid1);

    listInsert(context->largeVoid1, context->largeVoid2);
    listInsert(context->largeVoid2, context->largeVoid1);

    listInsert(context->small1, context->large2);
    listInsert(context->large2, context->small1);

    listInsert(context->small1, context->sub2);
    listInsert(context->sub2, context->small1);

    listInsert(context->small1, context->smallVoid2);
    listInsert(context->smallVoid2, context->small1);

    listInsert(context->small1, context->largeVoid2);
    listInsert(context->largeVoid2, context->small1);

    listInsert(context->large1, context->sub2);
    listInsert(context->sub2, context->large1);

    listInsert(context->large1, context->smallVoid2);
    listInsert(context->smallVoid2, context->large1);

    listInsert(context->large1, context->largeVoid2);
    listInsert(context->largeVoid2, context->large1);

    listInsert(context->sub1, context->smallVoid2);
    listInsert(context->smallVoid2, context->sub1);

    listInsert(context->smallVoid1, context->largeVoid2);
    listInsert(context->largeVoid2, context->smallVoid1);
}
TEST_CASE(testAcyclicListRemove, testAcyclicListOps,context) {
    listRemove(context->small1);
    listRemove(context->small2);
    listRemove(context->large1);
    listRemove(context->large2);
    listRemove(context->sub1);
    listRemove(context->sub2);
    listRemove(context->smallVoid1);
    listRemove(context->smallVoid2);
    listRemove(context->largeVoid1);
    listRemove(context->largeVoid2);
}
TEST_CASE(testAcyclicListReplace, testAcyclicListOps,context) {
    listReplace(context->small1, context->small2);
    listReplace(context->small2, context->small1);

    listReplace(context->large1, context->large2);
    listReplace(context->large2, context->large1);

    listReplace(context->sub1, context->sub2);
    listReplace(context->sub2, context->sub1);

    listReplace(context->smallVoid1, context->smallVoid2);
    listReplace(context->smallVoid2, context->smallVoid1);

    listReplace(context->largeVoid1, context->largeVoid2);
    listReplace(context->largeVoid2, context->largeVoid1);

    listReplace(context->small1, context->large2);
    listReplace(context->large2, context->small1);

    listReplace(context->small1, context->sub2);
    listReplace(context->sub2, context->small1);

    listReplace(context->small1, context->smallVoid2);
    listReplace(context->smallVoid2, context->small1);

    listReplace(context->small1, context->largeVoid2);
    listReplace(context->largeVoid2, context->small1);

    listReplace(context->large1, context->sub2);
    listReplace(context->sub2, context->large1);

    listReplace(context->large1, context->smallVoid2);
    listReplace(context->smallVoid2, context->large1);

    listReplace(context->large1, context->largeVoid2);
    listReplace(context->largeVoid2, context->large1);

    listReplace(context->sub1, context->smallVoid2);
    listReplace(context->smallVoid2, context->sub1);

    listReplace(context->smallVoid1, context->largeVoid2);
    listReplace(context->largeVoid2, context->smallVoid1);
}


/*
 * cyclic lists.
 */

TEST_SUITE(testCyclicListOps,
    testCyclicListAccess, testCyclicListInsert, testCyclicListRemove, 
    testCyclicListReplace
)
TEST_FIXTURE_CONTEXT(testCyclicListOps) {
    Col_Word small1;
    Col_Word small2;
    Col_Word large1;
    Col_Word large2;
    Col_Word smallVoid1;
    Col_Word smallVoid2;
    Col_Word largeVoid1;
    Col_Word largeVoid2;
};
TEST_FIXTURE_SETUP(testCyclicListOps, context) {
    Col_Word e[1000];
    size_t i;

    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();

    for (i=0; i < 10; i++) {
        e[i] = Col_NewIntWord(i);
    }
    context->small1 = Col_ConcatLists(Col_NewList(6, e), Col_CircularList(Col_NewList(4, e+6)));

    for (i=0; i < 5; i++) {
        e[i] = Col_NewIntWord(i+10000);
    }
    context->small2 = Col_NewList(5, e);

    for (i=0; i < 1000; i++) {
        e[i] = Col_NewIntWord(i+20000);
    }
    context->large1 = Col_ConcatLists(Col_NewList(600, e), Col_CircularList(Col_NewList(400, e+600)));

    for (i=0; i < 500; i++) {
        e[i] = Col_NewIntWord(i+30000);
    }
    context->large2 = Col_NewList(500, e);

    context->smallVoid1 = Col_ConcatLists(Col_NewList(6, NULL), Col_CircularList(Col_NewList(4, NULL)));
    context->smallVoid2 = Col_NewList(5, NULL);
    context->largeVoid1 = Col_ConcatLists(Col_NewList(600, NULL), Col_CircularList(Col_NewList(400, NULL)));
    context->largeVoid2 = Col_NewList(500, NULL);
}
TEST_FIXTURE_TEARDOWN(testCyclicListOps, context) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
}
TEST_CASE(testCyclicListAccess, testCyclicListOps, context) {
    listAccess(context->small1);
    listAccess(context->small2);
    listAccess(context->large1);
    listAccess(context->large2);
    listAccess(context->smallVoid1);
    listAccess(context->smallVoid2);
    listAccess(context->largeVoid1);
    listAccess(context->largeVoid2);
}
TEST_CASE(testCyclicListInsert, testCyclicListOps, context) {
    listInsert(context->small1, context->small2);
    listInsert(context->small2, context->small1);
    listInsert(context->small1, context->small1);

    listInsert(context->large1, context->large2);
    listInsert(context->large2, context->large1);
    listInsert(context->large1, context->large1);

    listInsert(context->smallVoid1, context->smallVoid2);
    listInsert(context->smallVoid2, context->smallVoid1);
    listInsert(context->smallVoid1, context->smallVoid1);

    listInsert(context->largeVoid1, context->largeVoid2);
    listInsert(context->largeVoid2, context->largeVoid1);
    listInsert(context->largeVoid1, context->largeVoid1);

    listInsert(context->small1, context->large2);
    listInsert(context->large2, context->small1);

    listInsert(context->small1, context->smallVoid2);
    listInsert(context->smallVoid2, context->small1);

    listInsert(context->small1, context->largeVoid2);
    listInsert(context->largeVoid2, context->small1);

    listInsert(context->large1, context->smallVoid2);
    listInsert(context->smallVoid2, context->large1);

    listInsert(context->large1, context->largeVoid2);
    listInsert(context->largeVoid2, context->large1);

    listInsert(context->smallVoid1, context->largeVoid2);
    listInsert(context->largeVoid2, context->smallVoid1);
}
TEST_CASE(testCyclicListRemove, testCyclicListOps, context) {
    listRemove(context->small1);
    listRemove(context->small2);
    listRemove(context->large1);
    listRemove(context->large2);
    listRemove(context->smallVoid1);
    listRemove(context->smallVoid2);
    listRemove(context->largeVoid1);
    listRemove(context->largeVoid2);
}
TEST_CASE(testCyclicListReplace, testCyclicListOps, context) {
    listReplace(context->small1, context->small2);
    listReplace(context->small2, context->small1);
    listReplace(context->small1, context->small1);

    listReplace(context->large1, context->large2);
    listReplace(context->large2, context->large1);
    listReplace(context->large1, context->large1);

    listReplace(context->smallVoid1, context->smallVoid2);
    listReplace(context->smallVoid2, context->smallVoid1);
    listReplace(context->smallVoid1, context->smallVoid1);

    listReplace(context->largeVoid1, context->largeVoid2);
    listReplace(context->largeVoid2, context->largeVoid1);
    listReplace(context->largeVoid1, context->largeVoid1);

    listReplace(context->small1, context->large2);
    listReplace(context->large2, context->small1);

    listReplace(context->small1, context->smallVoid2);
    listReplace(context->smallVoid2, context->small1);

    listReplace(context->small1, context->largeVoid2);
    listReplace(context->largeVoid2, context->small1);

    listReplace(context->large1, context->smallVoid2);
    listReplace(context->smallVoid2, context->large1);

    listReplace(context->large1, context->largeVoid2);
    listReplace(context->largeVoid2, context->large1);

    listReplace(context->smallVoid1, context->largeVoid2);
    listReplace(context->largeVoid2, context->smallVoid1);
}


/*
 * List iterators.
 */

TEST_SUITE(testListIterators, 
    testListIteratorDirect, testListIteratorForward, testListIteratorBackward, 
    testListIteratorStride
)

TEST_FIXTURE_CONTEXT(testListIterators) {
    size_t length;
    Col_Word *elements;
    Col_Word list, sublist;
    size_t sublistOffset;
};
TEST_FIXTURE_SETUP(testListIterators, context) {
    size_t i;

    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();

    context->length = 10000;
    context->elements = (Col_Word *) malloc(context->length  * sizeof(Col_Word));
    for (i = 0; i < context->length; i++) {
        context->elements[i] = Col_NewIntWord(i);
    }
    //context->elements[16] = Col_NewIntWord(0); // To generate an error.

    context->list = Col_NewList(context->length, context->elements);

    context->sublistOffset = 13;
    context->sublist = Col_Sublist(context->list, 
            context->sublistOffset, context->length-41);

    // TODO cyclic lists
}
TEST_FIXTURE_TEARDOWN(testListIterators, context) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
}

TEST_CASE(testListIteratorDirect, testListIterators, context) {
    size_t i, length;
    Col_ListIterator it;

    length = Col_ListLength(context->list);
    for (i = 0; i < length; i++) {
        Col_Word e = (Col_ListIterBegin(it, context->list, i), Col_ListIterAt(it));
        size_t index = Col_ListIterIndex(it);

        ASSERT(index == i, "index=%u", index);
        ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
        ASSERT(Col_IntWordValue(e) == (int) index, "index=%u", index);
        ASSERT(Col_ListIterAt(it) == Col_ListAt(context->list, index), "index=%u", index);
    }

    length = Col_ListLength(context->sublist);
    for (i = 0; i < length; i++) {
        Col_Word e = (Col_ListIterBegin(it, context->sublist, i), Col_ListIterAt(it));
        size_t index = Col_ListIterIndex(it);

        ASSERT(index == i, "index=%u", index);
        ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
        ASSERT(Col_IntWordValue(e) == (int) (index+context->sublistOffset), "index=%u", index);
        ASSERT(Col_ListIterAt(it) == Col_ListAt(context->sublist, index), "index=%u", index);
    }

    length = context->length;
    for (i = 0; i < length; i++) {
        Col_Word e = (Col_ListIterArray(it, context->length, context->elements), Col_ListIterMoveTo(it, i), Col_ListIterAt(it));
        size_t index = Col_ListIterIndex(it);

        ASSERT(index == i, "index=%u", index);
        ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
        ASSERT(Col_IntWordValue(e) == (int) index, "index=%u", index);
        ASSERT(Col_ListIterAt(it) == Col_ListAt(context->list, index), "index=%u", index);
    }
}

TEST_CASE(testListIteratorForward, testListIterators, context) {
    Col_ListIterator it;

    for (Col_ListIterFirst(it, context->list); 
            !Col_ListIterEnd(it); 
            Col_ListIterNext(it)) {
        Col_Word e = Col_ListIterAt(it);
        size_t index = Col_ListIterIndex(it);

        ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
        ASSERT(Col_IntWordValue(e) == (int) index, "index=%u", index);
        ASSERT(Col_ListIterAt(it) == Col_ListAt(context->list, index), "index=%u", index);
    }

    for (Col_ListIterFirst(it, context->sublist); 
            !Col_ListIterEnd(it); 
            Col_ListIterNext(it)) {
        Col_Word e = Col_ListIterAt(it);
        size_t index = Col_ListIterIndex(it);

        ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
        ASSERT(Col_IntWordValue(e) == (int) (index+context->sublistOffset), "index=%u", index);
        ASSERT(Col_ListIterAt(it) == Col_ListAt(context->sublist, index), "index=%u", index);
    }
}

TEST_CASE(testListIteratorBackward, testListIterators, context) {
    Col_ListIterator it;

    for (Col_ListIterLast(it, context->list); 
            !Col_ListIterEnd(it);
            Col_ListIterPrevious(it)) {
        Col_Word e = Col_ListIterAt(it);
        size_t index = Col_ListIterIndex(it);

        ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
        ASSERT(Col_IntWordValue(e) == (int) index, "index=%u", index);
        ASSERT(Col_ListIterAt(it) == Col_ListAt(context->list, index), "index=%u", index);
    }

    for (Col_ListIterLast(it, context->sublist); 
            !Col_ListIterEnd(it); 
            Col_ListIterPrevious(it)) {
        Col_Word e = Col_ListIterAt(it);
        size_t index = Col_ListIterIndex(it);

        ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
        ASSERT(Col_IntWordValue(e) == (int) (index+context->sublistOffset), "index=%u", index);
        ASSERT(Col_ListIterAt(it) == Col_ListAt(context->sublist, index), "index=%u", index);
    }
}

TEST_CASE(testListIteratorStride, testListIterators, context) {
    size_t stride = 7;
    Col_ListIterator it;

    /* Forward. */
    for (Col_ListIterFirst(it, context->list); 
            !Col_ListIterEnd(it); 
            Col_ListIterForward(it, stride)) {
        Col_Word e = Col_ListIterAt(it);
        size_t index = Col_ListIterIndex(it);

        ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
        ASSERT(Col_IntWordValue(e) == (int) index, "index=%u", index);
        ASSERT(Col_ListIterAt(it) == Col_ListAt(context->list, index), "index=%u", index);
    }

    for (Col_ListIterFirst(it, context->sublist); 
            !Col_ListIterEnd(it); 
            Col_ListIterForward(it, stride)) {
        Col_Word e = Col_ListIterAt(it);
        size_t index = Col_ListIterIndex(it);

        ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
        ASSERT(Col_IntWordValue(e) == (int) (index+context->sublistOffset), "index=%u", index);
        ASSERT(Col_ListIterAt(it) == Col_ListAt(context->sublist, index), "index=%u", index);
    }

    /* Backward. */
    for (Col_ListIterLast(it, context->list); 
            !Col_ListIterEnd(it);
            Col_ListIterBackward(it, stride)) {
        Col_Word e = Col_ListIterAt(it);
        size_t index = Col_ListIterIndex(it);

        ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
        ASSERT(Col_IntWordValue(e) == (int) index, "index=%u", index);
        ASSERT(Col_ListIterAt(it) == Col_ListAt(context->list, index), "index=%u", index);
    }

    for (Col_ListIterLast(it, context->sublist); 
            !Col_ListIterEnd(it); 
            Col_ListIterBackward(it, stride)) {
        Col_Word e = Col_ListIterAt(it);
        size_t index = Col_ListIterIndex(it);

        ASSERT(Col_WordType(e) == COL_INT, "index=%u", index);
        ASSERT(Col_IntWordValue(e) == (int) (index+context->sublistOffset), "index=%u", index);
        ASSERT(Col_ListIterAt(it) == Col_ListAt(context->sublist, index), "index=%u", index);
    }
}


/*
 * Generic tests.
 */

typedef struct TraverseListChunksInfo {
    Col_Word list;
    size_t index;
} TraverseListChunksInfo;
static int traverseListChunks(size_t index, size_t length, size_t number, const Col_Word **chunks, Col_ClientData clientData) {
    TraverseListChunksInfo *info = (TraverseListChunksInfo *) clientData;
    size_t i;
    ASSERT(number == 1);
    ASSERT(index == info->index);
    for (i = 0; i < length; i++) {
        Col_Word e1 = Col_ListAt(info->list, info->index+i);
        Col_Word e2 = (*chunks == COL_LISTCHUNK_VOID ? WORD_NIL : (*chunks)[i]);

        ASSERT(e1 == e2, "index=%u, e1=%u, e2=%u", info->index+i, e1, e2);
    }
    info->index += length;
    return 0;
}
static int traverseListChunksR(size_t index, size_t length, size_t number, const Col_Word **chunks, Col_ClientData clientData) {
    TraverseListChunksInfo *info = (TraverseListChunksInfo *) clientData;
    size_t i;
    ASSERT(number == 1);
    ASSERT(index == info->index-length+1, "index=%u, info->index-length+1=%u", index, info->index-length+1);
    for (i = 0; i < length; i++) {
        Col_Word e1 = Col_ListAt(info->list, info->index-length+1+i);
        Col_Word e2 = (*chunks == COL_LISTCHUNK_VOID ? WORD_NIL : (*chunks)[i]);

        ASSERT(e1 == e2, "index=%u, e1=%u, e2=%u", info->index-length+1+i, e1, e2);
    }
    info->index -= length;
    return 0;
}
static void listAccess(Col_Word list) {
     /*
     * Check that direct access and iterators give the same results.
     */

    Col_ListIterator it;
    int looped;
    size_t prev=0;
    TraverseListChunksInfo info;
    size_t size;

   /* Forward. */
    for (looped=0, Col_ListIterFirst(it, list); 
            !Col_ListIterEnd(it); 
            prev = Col_ListIterIndex(it), looped = Col_ListIterNext(it)) {
        size_t index = Col_ListIterIndex(it);
        Col_Word e1 = Col_ListAt(list, index);
        Col_Word e2 = Col_ListIterAt(it);

        ASSERT(e1 == e2, "index=%u", index);

        if (looped) {
            /* Loop detected. */
            ASSERT(Col_ListLoopLength(list) == prev-index+1, "index=%u, prev=%u", index, prev);
            break;
        }
    }

    /* Backward. */
    for (Col_ListIterLast(it, list); !Col_ListIterEnd(it); Col_ListIterPrevious(it)) {
        size_t index = Col_ListIterIndex(it);
        Col_Word e1 = Col_ListAt(list, index);
        Col_Word e2 = Col_ListIterAt(it);

        ASSERT(e1 == e2, "index=%u", index);
    }

    /* Chunks. */
    info.list = list;
    info.index = 0;
    size = 0;
    Col_TraverseListChunks(list, info.index, SIZE_MAX, 0, traverseListChunks, 
            (Col_ClientData) &info, &size);
    ASSERT(info.index == size, "info.index=%u, size=%u", info.index, size);
    ASSERT(size == Col_ListLength(list), "size=%u, length=%u", size, Col_ListLength(list));

    /* Chunks (reverse). */
    info.list = list;
    info.index = Col_ListLength(list)-1;
    size = 0;
    Col_TraverseListChunks(list, info.index, SIZE_MAX, 1, traverseListChunksR, 
            (Col_ClientData) &info, &size);
    ASSERT(info.index == (size_t)-1, "info.index=%u", info.index);
    ASSERT(size == Col_ListLength(list), "size=%u, length=%u", size, Col_ListLength(list));
}
static void listInsert(Col_Word into, Col_Word list) {
     /*
     * Insert a list into another one and compare with expected result.
     */

    size_t length = Col_ListLength(into);
    size_t loop = Col_ListLoopLength(into);
    size_t indices[] = {0, 1, (length-loop)/2, length/2, length-loop-1, length-loop, length-(loop/2), length-1};
    size_t offset;
    size_t i;
    Col_Word iResult, mResult;
    Col_ListIterator itResult, itInto, itList;
    int looped;
    size_t prev=0;

    for (offset=0; offset < 3; offset++) {
        for (i=0; i < sizeof(indices)/sizeof(*indices); i++) {
            size_t where = indices[i] + offset*length;
            size_t whereReal;

            /* Immutable. */
            iResult = Col_ListInsert(into, where, list);
            listAccess(iResult);

            /* Mutable. */
            mResult = Col_NewMList(); Col_MListInsert(mResult, 0, into); 
            Col_MListInsert(mResult, where, list);
            listAccess(mResult);

            /*
             * Compare mutable and immutable results.
             */

            listsEqual(iResult, mResult);

            /*
             * Compare immutable result with expected result.
             */

            /* Part before insertion point. */
            whereReal = where;
            if (loop && where > length-loop) {
                /* Adjust insertion point so that it lies within loop. */
                whereReal = (where - (length-loop)) % loop + (length-loop);
                if (whereReal == length-loop) whereReal = length;
            }
            for (looped=0, Col_ListIterFirst(itResult, iResult), Col_ListIterFirst(itInto, into);
                    !Col_ListIterEnd(itInto) && Col_ListIterIndex(itInto) < whereReal; 
                    prev = Col_ListIterIndex(itResult), looped = Col_ListIterNext(itResult), Col_ListIterNext(itInto)) {
                size_t index = Col_ListIterIndex(itResult);
                Col_Word e1 = Col_ListIterAt(itResult);
                Col_Word e2 = Col_ListIterAt(itInto);

                ASSERT(e1 == e2, "where=%u, index=%u", where, index);

                /* Loop forbidden: inserted list is before loop end. */
                ASSERT(!looped, "where=%u, index=%u, prev=%u", where, index, prev);

                if (loop && Col_ListIterIndex(itInto)+1 == whereReal && whereReal == length) {
                    /* Ensure that loop ends next. */
                    whereReal = length-loop;
                }
            }

            /* Inserted list. */
            for (Col_ListIterFirst(itList, list);
                    !Col_ListIterEnd(itList); 
                    prev = Col_ListIterIndex(itResult), looped = Col_ListIterNext(itResult), Col_ListIterNext(itList)) {
                size_t index = Col_ListIterIndex(itResult);
                Col_Word e1 = Col_ListIterAt(itResult);
                Col_Word e2 = Col_ListIterAt(itList);

                ASSERT(e1 == e2, "where=%u, index=%u", where, index);

                if (looped) {
                    /* Loop detected, inserted list must be cyclic. */
                    ASSERT(Col_ListLoopLength(list) == prev-index+1, "where=%u, index=%u, prev=%u", where, index, prev);
                    break;
                }
            }
            if (!Col_ListIterEnd(itList)) {
                /* Inserted list was cyclic. */
                ASSERT(looped, "where=%u", where);
                ASSERT(Col_ListLoopLength(list), "where=%u", where);
                continue;
            }

            /* Part after insertion point. */
            for (;
                    !Col_ListIterEnd(itInto); 
                    prev = Col_ListIterIndex(itResult), looped = Col_ListIterNext(itResult), Col_ListIterNext(itInto)) {
                size_t index = Col_ListIterIndex(itResult);
                Col_Word e1 = Col_ListIterAt(itResult);
                Col_Word e2 = Col_ListIterAt(itInto);

                ASSERT(e1 == e2, "where=%u, index=%u", where, index);

                if (looped) {
                    /* Loop detected. */
                    ASSERT(Col_ListLoopLength(iResult) == prev-index+1, "where=%u, index=%u, prev=%u", where, index, prev);
                    break;
                }
            }
            if (!Col_ListIterEnd(itResult)) {
                ASSERT(looped, "where=%u", where);
                ASSERT(Col_ListLoopLength(iResult), "where=%u", where);
            }
        }
    }
}
static void listRemove(Col_Word list) {
     /*
     * Remove a section of list and compare with expected result.
     */

    size_t length = Col_ListLength(list);
    size_t loop = Col_ListLoopLength(list);
    size_t indices[] = {0, 1, (length-loop)/2, length/2, length-loop-1, length-loop, length-(loop/2), length-1};
    size_t offset1, offset2;
    size_t i1, i2;
    Col_Word iResult, mResult;
    Col_ListIterator itResult, itList;
    int looped;
    size_t prev=0;

    for (offset1=0; offset1 < 3; offset1++) {
        for (i1=0; i1 < sizeof(indices)/sizeof(*indices); i1++) {
            size_t last = indices[i1] + offset1*length;

            for (offset2=0; offset2 < 3; offset2++) {
                for (i2=0; i2 < sizeof(indices)/sizeof(*indices); i2++) {
                    size_t first = indices[i2] + offset2*length;
                    size_t firstReal;

                    if (first > last) break;

                    /* Immutable. */
                    iResult = Col_ListRemove(list, first, last);
                    listAccess(iResult);

                    /* Mutable. */
                    mResult = Col_NewMList(); Col_MListInsert(mResult, 0, list); 
                    Col_MListRemove(mResult, first, last);
                    listAccess(mResult);

                    /*
                     * Compare mutable and immutable results.
                     */

                    listsEqual(iResult, mResult);

                    /*
                     * Compare immutable result with expected result.
                     */

                    /* Part before deleted section.*/
                    firstReal = first;
                    if (loop && first > length-loop) {
                        /* Adjust first index so that it lies within loop. */
                        firstReal = (first - (length-loop)) % loop + (length-loop);
                        if (firstReal == length-loop) firstReal = length;
                    }
                    for (looped=0, Col_ListIterFirst(itResult, iResult), Col_ListIterFirst(itList, list);
                            !Col_ListIterEnd(itList) && Col_ListIterIndex(itList) < firstReal; 
                            prev = Col_ListIterIndex(itResult), looped = Col_ListIterNext(itResult), Col_ListIterNext(itList)) {
                        size_t index = Col_ListIterIndex(itResult);
                        Col_Word e1 = Col_ListIterAt(itResult);
                        Col_Word e2 = Col_ListIterAt(itList);

                        ASSERT(e1 == e2, "first=%u, last=%u, index=%u", first, last, index);

                        /* Loop forbidden: first index is before loop end. */
                        ASSERT(!looped, "first=%u, last=%u, index=%u, prev=%u", first, last, index, prev);

                        if (loop && Col_ListIterIndex(itList)+1 == firstReal && firstReal == length) {
                            /* Ensure that loop ends next. */
                            firstReal = length-loop;
                        }
                    }

                    /* Part after deleted section.*/
                    if (last < length) {
                        Col_ListIterMoveTo(itList, last+1);
                        for (;
                                !Col_ListIterEnd(itList); 
                                prev = Col_ListIterIndex(itResult), looped = Col_ListIterNext(itResult), Col_ListIterNext(itList)) {
                            size_t index = Col_ListIterIndex(itResult);
                            Col_Word e1 = Col_ListIterAt(itResult);
                            Col_Word e2 = Col_ListIterAt(itList);

                            ASSERT(e1 == e2, "first=%u, last=%u, index=%u", first, last, index);

                            if (looped) {
                                /* Loop detected. */
                                ASSERT(Col_ListLoopLength(iResult) == prev-index+1, "first=%u, last=%u, index=%u, prev=%u", first, last, index, prev);
                                break;
                            }
                        }
                        if (!Col_ListIterEnd(itResult)) {
                            ASSERT(looped, "where=%u", "first=%u, last=%u", first, last);
                            ASSERT(Col_ListLoopLength(iResult), "first=%u, last=%u", first, last);
                        }
                    }
                }
            }
        }
    }
}
static void listReplace(Col_Word list, Col_Word with) {
     /*
     * Replace a section of rope with another and compare with expected result.
     */

    size_t length = Col_ListLength(list);
    size_t loop = Col_ListLoopLength(list);
    size_t indices[] = {0, 1, (length-loop)/2, length/2, length-loop-1, length-loop, length-(loop/2), length-1};
    size_t offset1, offset2;
    size_t i1, i2;
    Col_Word iResult, mResult;
    Col_ListIterator itResult, itList, itWith;
    int looped;
    size_t prev=0;

    for (offset1=0; offset1 < 3; offset1++) {
        for (i1=0; i1 < sizeof(indices)/sizeof(*indices); i1++) {
            size_t last = indices[i1] + offset1*length;

            for (offset2=0; offset2 < 3; offset2++) {
                for (i2=0; i2 < sizeof(indices)/sizeof(*indices); i2++) {
                    size_t first = indices[i2] + offset2*length;
                    size_t firstReal;

                    if (first > last) break;

                    /* Immutable. */
                    iResult = Col_ListReplace(list, first, last, with);
                    listAccess(iResult);

                    /* Mutable. */
                    mResult = Col_NewMList(); Col_MListInsert(mResult, 0, list); 
                    Col_MListReplace(mResult, first, last, with);
                    listAccess(mResult);

                    /*
                     * Compare mutable and immutable results.
                     */

                    listsEqual(iResult, mResult);

                    /*
                     * Compare immutable result with expected result.
                     */

                    /* Part before replaced section.*/
                    firstReal = first;
                    if (loop && first > length-loop) {
                        /* Adjust first index so that it lies within loop. */
                        firstReal = (first - (length-loop)) % loop + (length-loop);
                        if (firstReal == length-loop) firstReal = length;
                    }
                    for (looped=0, Col_ListIterFirst(itResult, iResult), Col_ListIterFirst(itList, list);
                            !Col_ListIterEnd(itList) && Col_ListIterIndex(itList) < firstReal; 
                            prev = Col_ListIterIndex(itResult), looped = Col_ListIterNext(itResult), Col_ListIterNext(itList)) {
                        size_t index = Col_ListIterIndex(itResult);
                        Col_Word e1 = Col_ListIterAt(itResult);
                        Col_Word e2 = Col_ListIterAt(itList);

                        ASSERT(e1 == e2, "foo first=%u, last=%u, index=%u", first, last, index);

                        /* Loop forbidden: first index is before loop end. */
                        ASSERT(!looped, "first=%u, last=%u, index=%u, prev=%u", first, last, index, prev);

                        if (loop && Col_ListIterIndex(itList)+1 == firstReal && firstReal == length) {
                            /* Ensure that loop ends next. */
                            firstReal = length-loop;
                        }
                    }

                    /* Replaced section. */
                    for (Col_ListIterFirst(itWith, with);
                            !Col_ListIterEnd(itWith); 
                            prev = Col_ListIterIndex(itResult), looped = Col_ListIterNext(itResult), Col_ListIterNext(itWith)) {
                        size_t index = Col_ListIterIndex(itResult);
                        Col_Word e1 = Col_ListIterAt(itResult);
                        Col_Word e2 = Col_ListIterAt(itWith);

                        ASSERT(e1 == e2, "first=%u, last=%u, index=%u", first, last, index);

                        if (looped) {
                            /* Loop detected, inserted list must be cyclic. */
                            ASSERT(Col_ListLoopLength(with) == prev-index+1, "first=%u, last=%u, index=%u, prev=%u", first, last, index, prev);
                            break;
                        }
                    }
                    if (!Col_ListIterEnd(itWith)) {
                        /* Inserted list was cyclic. */
                        ASSERT(looped, "first=%u, last=%u", first, last);
                        ASSERT(Col_ListLoopLength(with), "first=%u, last=%u", first, last);
                        continue;
                    }

                    /* Part after replaced section.*/
                    if (last < length) {
                        Col_ListIterMoveTo(itList, last+1);
                        for (;
                                !Col_ListIterEnd(itList); 
                                prev = Col_ListIterIndex(itResult), looped = Col_ListIterNext(itResult), Col_ListIterNext(itList)) {
                            size_t index = Col_ListIterIndex(itResult);
                            Col_Word e1 = Col_ListIterAt(itResult);
                            Col_Word e2 = Col_ListIterAt(itList);

                            ASSERT(e1 == e2, "first=%u, last=%u, index=%u", first, last, index);

                            if (looped) {
                                /* Loop detected. */
                                ASSERT(Col_ListLoopLength(iResult) == prev-index+1, "first=%u, last=%u, index=%u, prev=%u", first, last, index, prev);
                                break;
                            }
                        }
                        if (!Col_ListIterEnd(itResult)) {
                            ASSERT(looped, "first=%u, last=%u", first, last);
                            ASSERT(Col_ListLoopLength(iResult), "first=%u, last=%u", first, last);
                        }
                    }
                }
            }
        }
    }
}

static int compareListChunks(size_t index, size_t length, size_t number, const Col_Word **chunks, Col_ClientData clientData) {
    size_t i;
    ASSERT(number == 2);
    ASSERT(chunks[0]);
    ASSERT(chunks[1]);
    for (i = 0; i < length; i++) {
        Col_Word e1 = (chunks[0] == COL_LISTCHUNK_VOID ? WORD_NIL : chunks[0][i]);
        Col_Word e2 = (chunks[1] == COL_LISTCHUNK_VOID ? WORD_NIL : chunks[1][i]);

        ASSERT(e1 == e2, "index=%u", index+i);
    }
    return 0;
}
static void listsEqual(Col_Word list1, Col_Word list2) {
    Col_ListIterator it1, it2;
    int looped;
    size_t prev=0;
 
    ASSERT(Col_ListLength(list1) == Col_ListLength(list2));
    ASSERT(Col_ListLoopLength(list1) == Col_ListLoopLength(list2));

    /* Iterators. */
    for (looped=0, Col_ListIterFirst(it1, list1), Col_ListIterFirst(it2, list2);
            !Col_ListIterEnd(it1); 
            prev = Col_ListIterIndex(it1), looped = Col_ListIterNext(it1), Col_ListIterNext(it2)) {
        size_t index = Col_ListIterIndex(it1);
        Col_Word e1 = Col_ListIterAt(it1);
        Col_Word e2 = Col_ListIterAt(it2);

        ASSERT(index == Col_ListIterIndex(it2), "index=%u", index);
        ASSERT(e1 == e2, "index=%u", index);

        if (looped) {
            /* Loop detected. */
            ASSERT(Col_ListLoopLength(list1) == prev-index+1, "index=%u, prev=%u", index, prev);
            ASSERT(Col_ListLoopLength(list2) == prev-index+1, "index=%u, prev=%u", index, prev);
            break;
        }
    }
    if (Col_ListIterEnd(it1)) {
        ASSERT(!looped);
        ASSERT(Col_ListIterEnd(it2));
    } else {
        ASSERT(looped);
        ASSERT(!Col_ListIterEnd(it2));
    }

    /* Chunks. */
    {
        Col_Word lists[] = {list1, list2};
        size_t length = 0;
        Col_TraverseListChunksN(2, lists, 0, SIZE_MAX, compareListChunks, NULL, &length);
        ASSERT(length == Col_ListLength(list1));
    }
}
