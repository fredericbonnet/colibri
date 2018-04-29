#include <stdlib.h>
#include <limits.h>

#include <colibri.h>

#include "picotest.h"

#include "testColibri.h"

/*
 *---------------------------------------------------------------------------
 *
 * testRopes --
 *
 *---------------------------------------------------------------------------
 */

static void ropeAccess(Col_Word rope);
static void ropeInsert(Col_Word into, Col_Word rope);
static void ropeRemove(Col_Word rope);
static void ropeReplace(Col_Word rope, Col_Word with);
static void ropesEqual(Col_Word rope1, Col_Word rope2);
static int ropeCheckFormat(Col_Word rope, Col_StringFormat format);

TEST_SUITE(testRopes, 
    testNewRope, testSubropes, testRopeOps, testRopeIterators, testRopeUtf,
    testRopeNormalize
)

/*
 * Rope creation.
 */

TEST_SUITE(testNewRope, 
    testNewSmallRope, testNewLargeRope
)


TEST_FIXTURE_SETUP(testNewRope) {
    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();
}
TEST_FIXTURE_TEARDOWN(testNewRope) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
}
TEST_CASE(testNewSmallRope, testNewRope) {
    Col_Char1 c[10];
    size_t l = sizeof(c)/sizeof(*c);
    size_t index;
    Col_RopeIterator it;
    Col_Word rope;

    for (index=0; index < l; index++) {
        c[index] = (Col_Char1) index;
    }

    rope = Col_NewRope(COL_UCS1, c, sizeof(c));
    ASSERT(Col_RopeLength(rope) == l);
    ropeAccess(rope);
    for (Col_RopeIterFirst(it, rope); !Col_RopeIterEnd(it); Col_RopeIterNext(it)) {
        Col_Char c = Col_RopeIterAt(it);
        index = Col_RopeIterIndex(it);
        ASSERT(c == (Col_Char) index, "index=%u", index);
        ASSERT(Col_RopeFindFirst(rope, (Col_Char) index) == index, "index=%u", index);
        ASSERT(Col_RopeFindLast(rope, (Col_Char) index) == index, "index=%u", index);
        ASSERT(Col_RopeFindFirstN(rope, (Col_Char) index, index) == SIZE_MAX, "index=%u", index);
        ASSERT(Col_RopeFindFirstN(rope, (Col_Char) index, index+1) == index, "index=%u", index);
        ASSERT(Col_RopeFindLastN(rope, (Col_Char) index, l-index-1) == SIZE_MAX, "index=%u", index);
        ASSERT(Col_RopeFindLastN(rope, (Col_Char) index, l-index) == index, "index=%u", index);
        ASSERT(Col_RopeFind(rope, (Col_Char) index, index+1, SIZE_MAX, 0) == SIZE_MAX, "index=%u", index);
        if (index > 0) ASSERT(Col_RopeFind(rope, (Col_Char) index, index-1, SIZE_MAX, 1) == SIZE_MAX, "index=%u", index);
    }
}
TEST_CASE(testNewLargeRope, testNewRope) {
    Col_Char2 c[1000];
    size_t l = sizeof(c)/sizeof(*c);
    size_t index;
    Col_RopeIterator it;
    Col_Word rope;

    for (index=0; index < l; index++) {
        c[index] = (Col_Char2) index;
    }

    rope = Col_NewRope(COL_UCS2, c, sizeof(c));
    ASSERT(Col_RopeLength(rope) == l);
    ropeAccess(rope);
    for (Col_RopeIterFirst(it, rope); !Col_RopeIterEnd(it); Col_RopeIterNext(it)) {
        Col_Char c = Col_RopeIterAt(it);
        index = Col_RopeIterIndex(it);
        ASSERT(c == (Col_Char) index, "index=%u", index);
        ASSERT(Col_RopeFindFirst(rope, c) == index, "index=%u", index);
        ASSERT(Col_RopeFindLast(rope, c) == index, "index=%u", index);
        ASSERT(Col_RopeFindFirstN(rope, c, index) == SIZE_MAX, "index=%u", index);
        ASSERT(Col_RopeFindFirstN(rope, c, index+1) == index, "index=%u", index);
        ASSERT(Col_RopeFindLastN(rope, c, l-index-1) == SIZE_MAX, "index=%u", index);
        ASSERT(Col_RopeFindLastN(rope, c, l-index) == index, "index=%u", index);
        ASSERT(Col_RopeFind(rope, c, index+1, SIZE_MAX, 0) == SIZE_MAX, "index=%u", index);
        if (index > 0) ASSERT(Col_RopeFind(rope, c, index-1, SIZE_MAX, 1) == SIZE_MAX, "index=%u", index);
    }
}


/*
 * Subropes.
 */

TEST_SUITE(testSubropes, 
    testSmallSubropes, testLargeSubropes, testNestedSubropes
)


TEST_FIXTURE_CONTEXT(testSubropes) {
    Col_Word rope;
};
TEST_FIXTURE_SETUP(testSubropes, context) {
    Col_Char2 c[1000];
    size_t l = sizeof(c)/sizeof(*c);
    size_t index;

    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();

    for (index=0; index < l; index++) {
        c[index] = (Col_Char2) index;
    }

    context->rope = Col_NewRope(COL_UCS2, c, sizeof(c));
}
TEST_FIXTURE_TEARDOWN(testSubropes) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
}
TEST_CASE(testSmallSubropes, testSubropes, context) {
    size_t length = Col_RopeLength(context->rope);
    size_t index;
    size_t offset, sublength=10;
    Col_Word subrope;
    Col_RopeIterator it;

    for (offset=0; offset < length; offset++) {
        size_t l = (offset<=length-sublength?sublength:length-offset);
        subrope = Col_Subrope(context->rope, offset, offset+sublength-1);
        ASSERT(Col_RopeLength(subrope) == l);

        ropeAccess(subrope);

        for (Col_RopeIterFirst(it, subrope); !Col_RopeIterEnd(it); Col_RopeIterNext(it)) {
            Col_Char c = Col_RopeIterAt(it);
            index = Col_RopeIterIndex(it);
            ASSERT(c == (Col_Char) index+offset, "index=%u", index);
            ASSERT(Col_RopeFindFirst(subrope, c) == index, "index=%u", index);
            ASSERT(Col_RopeFindLast(subrope, c) == index, "index=%u", index);
            ASSERT(Col_RopeFindFirstN(subrope, c, index) == SIZE_MAX, "index=%u", index);
            ASSERT(Col_RopeFindFirstN(subrope, c, index+1) == index, "index=%u", index);
            ASSERT(Col_RopeFindLastN(subrope, c, l-index-1) == SIZE_MAX, "index=%u", index);
            ASSERT(Col_RopeFindLastN(subrope, c, l-index) == index, "index=%u", index);
            ASSERT(Col_RopeFind(subrope, c, index+1, SIZE_MAX, 0) == SIZE_MAX, "index=%u", index);
            if (index > 0) ASSERT(Col_RopeFind(subrope, c, index-1, SIZE_MAX, 1) == SIZE_MAX, "index=%u", index);
        }

        ASSERT(Col_RopeSearchFirst(context->rope, subrope) == offset, "offset=%u", offset);
        ASSERT(Col_RopeSearchLast(context->rope, subrope) == offset, "offset=%u", offset);
        ASSERT(Col_RopeSearch(context->rope, subrope, offset, SIZE_MAX, 0) == offset, "offset=%u", offset);
        ASSERT(Col_RopeSearch(context->rope, subrope, offset+1, SIZE_MAX, 0) == SIZE_MAX, "offset=%u", offset);
        ASSERT(Col_RopeSearch(context->rope, subrope, offset, SIZE_MAX, 1) == offset, "offset=%u", offset);
        if (offset > 0) ASSERT(Col_RopeSearch(context->rope, subrope, offset-1, SIZE_MAX, 1) == SIZE_MAX, "offset=%u", offset);
    }
}
TEST_CASE(testLargeSubropes, testSubropes, context) {
    size_t length = Col_RopeLength(context->rope);
    size_t index;
    size_t offset, sublength=length/2;
    Col_Word subrope;
    Col_RopeIterator it;

    for (offset=0; offset < length; offset++) {
        size_t l = (offset<=length-sublength?sublength:length-offset);
        subrope = Col_Subrope(context->rope, offset, offset+sublength-1);
        ASSERT(Col_RopeLength(subrope) == l);

        ropeAccess(subrope);

        for (Col_RopeIterFirst(it, subrope); !Col_RopeIterEnd(it); Col_RopeIterNext(it)) {
            Col_Char c = Col_RopeIterAt(it);
            index = Col_RopeIterIndex(it);
            ASSERT(c == (Col_Char) index+offset, "index=%u", index);
            ASSERT(Col_RopeFindFirst(subrope, c) == index, "index=%u", index);
            ASSERT(Col_RopeFindLast(subrope, c) == index, "index=%u", index);
            ASSERT(Col_RopeFindFirstN(subrope, c, index) == SIZE_MAX, "index=%u", index);
            ASSERT(Col_RopeFindFirstN(subrope, c, index+1) == index, "index=%u", index);
            ASSERT(Col_RopeFindLastN(subrope, c, l-index-1) == SIZE_MAX, "index=%u", index);
            ASSERT(Col_RopeFindLastN(subrope, c, l-index) == index, "index=%u", index);
            ASSERT(Col_RopeFind(subrope, c, index+1, SIZE_MAX, 0) == SIZE_MAX, "index=%u", index);
            if (index > 0) ASSERT(Col_RopeFind(subrope, c, index-1, SIZE_MAX, 1) == SIZE_MAX, "index=%u", index);
        }

        ASSERT(Col_RopeSearchFirst(context->rope, subrope) == offset, "offset=%u", offset);
        ASSERT(Col_RopeSearchLast(context->rope, subrope) == offset, "offset=%u", offset);
        ASSERT(Col_RopeSearch(context->rope, subrope, offset, SIZE_MAX, 0) == offset, "offset=%u", offset);
        ASSERT(Col_RopeSearch(context->rope, subrope, offset+1, SIZE_MAX, 0) == SIZE_MAX, "offset=%u", offset);
        ASSERT(Col_RopeSearch(context->rope, subrope, offset, SIZE_MAX, 1) == offset, "offset=%u", offset);
        if (offset > 0) ASSERT(Col_RopeSearch(context->rope, subrope, offset-1, SIZE_MAX, 1) == SIZE_MAX, "offset=%u", offset);
    }
}
TEST_CASE(testNestedSubropes, testSubropes, context) {
    /*
     * Level 2 nesting:
     *
     *  #0 [0             ...           999] = [<0...249>+<250...499>] + [<500...749>+<750...999>]
     *  #1    [50    ...449,550...   949]    =    [(50  ...   449)     +     (550   ...   949)]
     *  #2       [100...449,550...899]       =       (100             ...               899)
     */

    size_t length = Col_RopeLength(context->rope), l;
    size_t half = length/2;
    size_t index, offset;
    size_t level;

    Col_Word rope = Col_Subrope(
        Col_ConcatRopes(
            Col_Subrope(context->rope, 50, half-51),
            Col_Subrope(context->rope, half+50, length-51)
        ),
        50, length-251
    );

    l = length-300;
    ASSERT(Col_RopeLength(rope) == l);

    ropeAccess(rope);

    for (index=0, offset=100; index < l; index++) {
        Col_Char c = Col_RopeAt(rope, index);
        if (index == half-150) offset=200;
        ASSERT(c == (Col_Char) index+offset, "index=%u", index);
        ASSERT(Col_RopeFindFirst(rope, c) == index, "index=%u", index);
        ASSERT(Col_RopeFindLast(rope, c) == index, "index=%u", index);
        ASSERT(Col_RopeFindFirstN(rope, c, index) == SIZE_MAX, "index=%u", index);
        ASSERT(Col_RopeFindFirstN(rope, c, index+1) == index, "index=%u", index);
        ASSERT(Col_RopeFindLastN(rope, c, l-index-1) == SIZE_MAX, "index=%u", index);
        ASSERT(Col_RopeFindLastN(rope, c, l-index) == index, "index=%u", index);
        ASSERT(Col_RopeFind(rope, c, index+1, SIZE_MAX, 0) == SIZE_MAX, "index=%u", index);
        if (index > 0) ASSERT(Col_RopeFind(rope, c, index-1, SIZE_MAX, 1) == SIZE_MAX, "index=%u", index);
    }

    /* 
     * Further level nesting. 
     */

    rope = context->rope;
    for (level = 0; level < 5; level++) {
        rope = Col_ConcatRopes(rope, rope);
        rope = Col_Subrope(rope, 1, Col_RopeLength(rope)-2);

        ropeAccess(rope);
    }
}


/*
 * Rope operations.
 */

TEST_SUITE(testRopeOps,
    testRopeAccess, testRopeInsert, testRopeRemove, testRopeReplace
)


TEST_FIXTURE_CONTEXT(testRopeOps) {
    Col_Word small1;
    Col_Word small2;
    Col_Word large1;
    Col_Word large2;
};
TEST_FIXTURE_SETUP(testRopeOps, context) {
    Col_Char1 c1[10];
    Col_Char2 c2[1000];
    size_t i;

    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();

    for (i=0; i < 10; i++) {
        c1[i] = (Col_Char1) i;
    }
    context->small1 = Col_NewRope(COL_UCS1, c1, 10*sizeof(*c1));

    for (i=0; i < 5; i++) {
        c1[i] = (Col_Char1) (i+100);
    }
    context->small2 = Col_NewRope(COL_UCS1, c1, 5*sizeof(*c1));

    for (i=0; i < 1000; i++) {
        c2[i] = (Col_Char2) (i+1000);
    }
    context->large1 = Col_NewRope(COL_UCS2, c2, 1000*sizeof(*c2));

    for (i=0; i < 500; i++) {
        c2[i] = (Col_Char2) (i+2000);
    }
    context->large2 = Col_NewRope(COL_UCS2, c2, 500*sizeof(*c2));
}
TEST_FIXTURE_TEARDOWN(testRopeOps) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
}
TEST_CASE(testRopeAccess, testRopeOps, context) {
    ropeAccess(context->small1);
    ropeAccess(context->small2);
    ropeAccess(context->large1);
    ropeAccess(context->large2);
}
TEST_CASE(testRopeInsert, testRopeOps, context) {
    ropeInsert(context->small1, context->small2);
    ropeInsert(context->small2, context->small1);

    ropeInsert(context->large1, context->large2);
    ropeInsert(context->large2, context->large1);

    ropeInsert(context->small1, context->large2);
    ropeInsert(context->large2, context->small1);
}
TEST_CASE(testRopeRemove, testRopeOps, context) {
    ropeRemove(context->small1);
    ropeRemove(context->small2);
    ropeRemove(context->large1);
    ropeRemove(context->large2);
}
TEST_CASE(testRopeReplace, testRopeOps, context) {
    ropeReplace(context->small1, context->small2);
    ropeReplace(context->small2, context->small1);

    ropeReplace(context->large1, context->large2);
    ropeReplace(context->large2, context->large1);

    ropeReplace(context->small1, context->large2);
    ropeReplace(context->large2, context->small1);
}


/*
 * Rope iterators.
 */

TEST_SUITE(testRopeIterators, 
    testRopeIteratorDirect, testRopeIteratorForward, testRopeIteratorBackward, 
    testRopeIteratorStride
)


TEST_FIXTURE_CONTEXT(testRopeIterators) {
    size_t length;
    Col_Char2 *data;
    Col_Word rope, subrope;
    size_t subropeOffset;
};
TEST_FIXTURE_SETUP(testRopeIterators, context) {
    size_t i;

    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();

    context->length = COL_CHAR2_MAX+1;
    context->data = (Col_Char2 *) malloc(context->length  * sizeof(Col_Char2));
    for (i = 0; i < context->length; i++) {
        context->data[i] = (Col_Char2) i;
    }
    //context->data[16] = 0; // To generate an error.

    context->rope = Col_NewRope(COL_UCS2, context->data, 
            context->length  * sizeof(Col_Char2));

    context->subropeOffset = 13;
    context->subrope = Col_Subrope(context->rope, 
            context->subropeOffset, context->length-41);
}
TEST_FIXTURE_TEARDOWN(testRopeIterators, context) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
    free(context->data);
}
TEST_CASE(testRopeIteratorDirect, testRopeIterators, context) {
    size_t i, length;
    Col_RopeIterator it;

    length = Col_RopeLength(context->rope);
    for (i = 0; i < length; i++) {
        Col_Char c = (Col_RopeIterBegin(it, context->rope, i), Col_RopeIterAt(it));
        size_t index = Col_RopeIterIndex(it);

        ASSERT(index == i, "index=%u", index);
        ASSERT(c == (Col_Char) index, "index=%u", index);
        ASSERT(Col_RopeIterAt(it) == Col_RopeAt(context->rope, index), "index=%u", index);
    }

    length = Col_RopeLength(context->subrope);
    for (i = 0; i < length; i++) {
        Col_Char c = (Col_RopeIterBegin(it, context->subrope, i), Col_RopeIterAt(it));
        size_t index = Col_RopeIterIndex(it);

        ASSERT(index == i, "index=%u", index);
        ASSERT(c == (Col_Char) (index+context->subropeOffset), "index=%u", index);
        ASSERT(Col_RopeIterAt(it) == Col_RopeAt(context->subrope, index), "index=%u", index);
    }

    length = context->length;
    for (i = 0; i < length; i++) {
        Col_Char c = (Col_RopeIterString(it, COL_UCS2, context->data, context->length), Col_RopeIterMoveTo(it, i), Col_RopeIterAt(it));
        size_t index = Col_RopeIterIndex(it);

        ASSERT(index == i, "index=%u", index);
        ASSERT(c == (Col_Char) index, "index=%u", index);
        ASSERT(Col_RopeIterAt(it) == Col_RopeAt(context->rope, index), "index=%u", index);
    }
}

TEST_CASE(testRopeIteratorForward, testRopeIterators, context) {
    Col_RopeIterator it;

    for (Col_RopeIterFirst(it, context->rope); 
            !Col_RopeIterEnd(it); 
            Col_RopeIterNext(it)) {
        Col_Char c = Col_RopeIterAt(it);
        size_t index = Col_RopeIterIndex(it);

        ASSERT(c == (Col_Char) index, "index=%u", index);
        ASSERT(Col_RopeIterAt(it) == Col_RopeAt(context->rope, index), "index=%u", index);
    }

    for (Col_RopeIterFirst(it, context->subrope); 
            !Col_RopeIterEnd(it); 
            Col_RopeIterNext(it)) {
        Col_Char c = Col_RopeIterAt(it);
        size_t index = Col_RopeIterIndex(it);

        ASSERT(c == (Col_Char) (index+context->subropeOffset), "index=%u", index);
        ASSERT(Col_RopeIterAt(it) == Col_RopeAt(context->subrope, index), "index=%u", index);
    }

    for (Col_RopeIterString(it, COL_UCS2, context->data, context->length);
            !Col_RopeIterEnd(it); 
            Col_RopeIterNext(it)) {
        Col_Char c = Col_RopeIterAt(it);
        size_t index = Col_RopeIterIndex(it);

        ASSERT(c == (Col_Char) index, "index=%u", index);
        ASSERT(Col_RopeIterAt(it) == Col_RopeAt(context->rope, index), "index=%u", index);
    }

}

TEST_CASE(testRopeIteratorBackward, testRopeIterators, context) {
    Col_RopeIterator it;

    for (Col_RopeIterLast(it, context->rope); 
            !Col_RopeIterEnd(it);
            Col_RopeIterPrevious(it)) {
        Col_Char c = Col_RopeIterAt(it);
        size_t index = Col_RopeIterIndex(it);

        ASSERT(c == (Col_Char) index, "index=%u", index);
        ASSERT(Col_RopeIterAt(it) == Col_RopeAt(context->rope, index), "index=%u", index);
    }

    for (Col_RopeIterLast(it, context->subrope); 
            !Col_RopeIterEnd(it); 
            Col_RopeIterPrevious(it)) {
        Col_Char c = Col_RopeIterAt(it);
        size_t index = Col_RopeIterIndex(it);

        ASSERT(c == (Col_Char) (index+context->subropeOffset), "index=%u", index);
        ASSERT(Col_RopeIterAt(it) == Col_RopeAt(context->subrope, index), "index=%u", index);
    }

    for (Col_RopeIterString(it, COL_UCS2, context->data, context->length),
            Col_RopeIterMoveTo(it, context->length-1);
            !Col_RopeIterEnd(it);
            Col_RopeIterPrevious(it)) {
        Col_Char c = Col_RopeIterAt(it);
        size_t index = Col_RopeIterIndex(it);

        ASSERT(c == (Col_Char) index, "index=%u", index);
        ASSERT(Col_RopeIterAt(it) == Col_RopeAt(context->rope, index), "index=%u", index);
    }
}

TEST_CASE(testRopeIteratorStride, testRopeIterators, context) {
    size_t stride = 7;
    Col_RopeIterator it;

    /* Forward. */
    for (Col_RopeIterFirst(it, context->rope); 
            !Col_RopeIterEnd(it); 
            Col_RopeIterForward(it, stride)) {
        Col_Char c = Col_RopeIterAt(it);
        size_t index = Col_RopeIterIndex(it);

        ASSERT(c == (Col_Char) index, "index=%u", index);
        ASSERT(Col_RopeIterAt(it) == Col_RopeAt(context->rope, index), "index=%u", index);
    }

    for (Col_RopeIterFirst(it, context->subrope); 
            !Col_RopeIterEnd(it); 
            Col_RopeIterForward(it, stride)) {
        Col_Char c = Col_RopeIterAt(it);
        size_t index = Col_RopeIterIndex(it);

        ASSERT(c == (Col_Char) (index+context->subropeOffset), "index=%u", index);
        ASSERT(Col_RopeIterAt(it) == Col_RopeAt(context->subrope, index), "index=%u", index);
    }

    /* Backward. */
    for (Col_RopeIterLast(it, context->rope); 
            !Col_RopeIterEnd(it);
            Col_RopeIterBackward(it, stride)) {
        Col_Char c = Col_RopeIterAt(it);
        size_t index = Col_RopeIterIndex(it);

        ASSERT(c == (Col_Char) index, "index=%u", index);
        ASSERT(Col_RopeIterAt(it) == Col_RopeAt(context->rope, index), "index=%u", index);
    }

    for (Col_RopeIterLast(it, context->subrope); 
            !Col_RopeIterEnd(it); 
            Col_RopeIterBackward(it, stride)) {
        Col_Char c = Col_RopeIterAt(it);
        size_t index = Col_RopeIterIndex(it);

        ASSERT(c == (Col_Char) (index+context->subropeOffset), "index=%u", index);
        ASSERT(Col_RopeIterAt(it) == Col_RopeAt(context->subrope, index), "index=%u", index);
    }
}


/*
 * UTF ropes.
 */

TEST_SUITE(testRopeUtf, 
    testRopeUtf8, testRopeUtf16
)

/*
 * UTF-8 ropes.
 */

TEST_SUITE(testRopeUtf8,
    testRopeUtf8Data, testRopeUtf8Create, testRopeUtf8Access, 
    testRopeUtf8Iterate
)


TEST_CASE(testRopeUtf8Data) {
    Col_Char1 data[COL_UTF8_MAX_WIDTH];
    const Col_Char1 *p;
    Col_Char c, c2;
    for (c=0; c <= COL_CHAR_MAX+1000; c++) {
        size_t width = COL_UTF8_WIDTH(c);
        ASSERT(width <= COL_UTF8_MAX_WIDTH, "c=U+%x", c);
        p = Col_Utf8Set(data, c);
        ASSERT(p == data + width, "c=U+%x", c);
        if (c <= 0xD700 || (c >= 0xE000 && c <= COL_CHAR_MAX)) {
            c2 = Col_Utf8Get(data);
            ASSERT(c2 == c, "c=U+%x, c2=U+%x", c, c2);
            p = Col_Utf8Next(data);
            ASSERT(p == data + width, "c=U+%x", c);
            p = Col_Utf8Prev(p);
            ASSERT(p == data, "c=U+%x", c);
        }
    }
}


TEST_FIXTURE_CONTEXT(testRopeUtf8) {
    size_t length, byteLength;
    Col_Char1 *data;
};
TEST_FIXTURE_SETUP(testRopeUtf8, context) {
    /*
     * UTF-8 string containing all the codepoints in range (inc. the invalid
     * ones such as surrogage halves, as we only care about numeric 
     * representation here).
     */

    Col_Char1 *p;
    Col_Char c;
    size_t i;

    context->length = (0xD7FF-0x00+1) + (0x10FFFF-0xE000+1);
    context->byteLength = (
            (0x7F-0x00+1)		/* 1-char sequences. */
        + 2*(0x7FF-0x80+1)		/* 2-char sequences. */
        + 3*(  0xD7FF-0x800+1		/* 3-char sequences. */
               /* surrogate halves */
             + 0xFFFF-0xE000+1)
        + 4*(0x10FFFF-0x10000+1)	/* 4-char sequences. */
    );
    context->data = (Col_Char1 *) malloc(context->byteLength);

    /* 1-char sequences. */
    for (c=0, i=0, p=context->data; c <= 0x7F; c++, i++) {
        *p++ = (Col_Char1) c;
    }
    /* 2-char sequences. */
    for (; c <= 0x7FF; c++, i++) {
        *p++ = (Col_Char1) (((c>>6)&0x1F)|0xC0);
        *p++ = (Col_Char1) (( c    &0x3F)|0x80);
    }
    /* 3-char sequences. */
    for (; c <= 0xFFFF; (c == 0xD7FF ? c = 0xE000 : c++), i++) {
        *p++ = (Col_Char1) (((c>>12)&0x1F)|0xE0);
        *p++ = (Col_Char1) (((c>> 6)&0x3F)|0x80);
        *p++ = (Col_Char1) (( c     &0x3F)|0x80);
    }
    /* 4-char sequences. */
    for (; c <= 0x10FFFF; c++, i++) {
        *p++ = (Col_Char1) (((c>>18)&0x1F)|0xF0);
        *p++ = (Col_Char1) (((c>>12)&0x3F)|0x80);
        *p++ = (Col_Char1) (((c>> 6)&0x3F)|0x80);
        *p++ = (Col_Char1) (( c     &0x3F)|0x80);
    }

    ASSERT(p == context->data+context->byteLength);
    ASSERT(i == context->length);
    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();
}
TEST_FIXTURE_TEARDOWN(testRopeUtf8, context) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
    free(context->data);
}
TEST_CASE(testRopeUtf8Create, testRopeUtf8, context) {
    Col_Word rope;
    size_t length;
    
    /* Entire string. */
    rope = Col_NewRope(COL_UTF8, context->data, context->byteLength);
    length = Col_RopeLength(rope);
    ASSERT(length == context->length);

    /* 1-char sequences. */
    rope = Col_NewRope(COL_UTF8, context->data, 0x7F-0x00+1);
    length = Col_RopeLength(rope);
    ASSERT(length == (0x7F-0x00+1));

    /* 2-char sequences. */
    rope = Col_NewRope(COL_UTF8, context->data+(0x7F-0x00+1), 2*(0x7FF-0x80+1));
    length = Col_RopeLength(rope);
    ASSERT(length == (0x7FF-0x80+1));

    /* 3-char sequences. */
    rope = Col_NewRope(COL_UTF8, context->data+(0x7F-0x00+1)+2*(0x7FF-0x80+1), 3*(0xFFFF-0xE000+1+0xD7FF-0x800+1));
    length = Col_RopeLength(rope);
    ASSERT(length == (0xFFFF-0xE000+1+0xD7FF-0x800+1));

    /* 4-char sequences. */
    rope = Col_NewRope(COL_UTF8, context->data+(0x7F-0x00+1)+2*(0x7FF-0x80+1)+3*(0xFFFF-0xE000+1+0xD7FF-0x800+1), 4*(0x10FFFF-0x10000+1));
    length = Col_RopeLength(rope);
    ASSERT(length == (0x10FFFF-0x10000+1));
}
TEST_CASE(testRopeUtf8Access, testRopeUtf8, context) {
    ropeAccess(Col_NewRope(COL_UTF8, context->data, context->byteLength));
}


TEST_SUITE(testRopeUtf8Iterate,
    testRopeUtf8IterateForward, testRopeUtf8IterateBackward
)


TEST_CASE(testRopeUtf8IterateForward, testRopeUtf8, context) {
    Col_RopeIterator it;
    Col_Char c1, c2;
    size_t index;
    Col_Word rope = Col_NewRope(COL_UTF8, context->data, context->byteLength);

    for (c1=0, Col_RopeIterFirst(it, rope); !Col_RopeIterEnd(it); (c1 == 0xD7FF ? c1 = 0xE000 : c1++), Col_RopeIterNext(it)) {
        index = Col_RopeIterIndex(it);
        c2 = Col_RopeIterAt(it);
        ASSERT(c1 == c2, "index=%u, c1=U+%x, c2=U+%x", index, c1, c2);
        c2 = Col_RopeAt(rope, index);
        ASSERT(c1 == c2, "index=%u, c1=U+%x, c2=U+%x", index, c1, c2);
    }
    ASSERT(c1 == 0x10FFFF+1);

    for (c1=0, Col_RopeIterString(it, COL_UTF8, context->data, context->length); !Col_RopeIterEnd(it); (c1 == 0xD7FF ? c1 = 0xE000 : c1++), Col_RopeIterNext(it)) {
        index = Col_RopeIterIndex(it);
        c2 = Col_RopeIterAt(it);
        ASSERT(c1 == c2, "index=%u, c1=U+%x, c2=U+%x", index, c1, c2);
        c2 = Col_RopeAt(rope, index);
        ASSERT(c1 == c2, "index=%u, c1=U+%x, c2=U+%x", index, c1, c2);
    }
    ASSERT(c1 == 0x10FFFF+1);
}
TEST_CASE(testRopeUtf8IterateBackward, testRopeUtf8, context) {
    Col_RopeIterator it;
    Col_Char c1, c2;
    size_t index;
    Col_Word rope = Col_NewRope(COL_UTF8, context->data, context->byteLength);

    for (c1=0x10FFFF, Col_RopeIterLast(it, rope); !Col_RopeIterEnd(it); (c1 == 0xE000 ? c1 = 0xD7FF : c1--), Col_RopeIterPrevious(it)) {
        index = Col_RopeIterIndex(it);
        c2 = Col_RopeIterAt(it);
        ASSERT(c1 == c2, "index=%u, c1=U+%x, c2=U+%x", index, c1, c2);
        c2 = Col_RopeAt(rope, index);
        ASSERT(c1 == c2, "index=%u, c1=U+%x, c2=U+%x", index, c1, c2);
    }
    ASSERT(c1 == -1);
}

/*
 * UTF-16 ropes.
 */

TEST_SUITE(testRopeUtf16,
    testRopeUtf16Data, testRopeUtf16Create, testRopeUtf16Access, 
    testRopeUtf16Iterate
)


TEST_CASE(testRopeUtf16Data) {
    Col_Char2 data[COL_UTF16_MAX_WIDTH];
    const Col_Char2 *p;
    Col_Char c, c2;
    for (c=0; c <= COL_CHAR_MAX+1000; c++) {
        size_t width = COL_UTF16_WIDTH(c);
        ASSERT(width <= COL_UTF16_MAX_WIDTH, "c=U+%x", c);
        p = Col_Utf16Set(data, c);
        ASSERT(p == data + width, "c=U+%x", c);
        if (c <= 0xD700 || (c >= 0xE000 && c <= COL_CHAR_MAX)) {
            c2 = Col_Utf16Get(data);
            ASSERT(c2 == c, "c=U+%x, c2=U+%x", c, c2);
            p = Col_Utf16Next(data);
            ASSERT(p == data + width, "c=U+%x", c);
            p = Col_Utf16Prev(p);
            ASSERT(p == data, "c=U+%x", c);
        }
    }
}


TEST_FIXTURE_CONTEXT(testRopeUtf16) {
    size_t length, byteLength;
    Col_Char2 *data;
};
TEST_FIXTURE_SETUP(testRopeUtf16, context) {
    /*
     * UTF-16 string containing all the valid codepoints.
     */

    Col_Char2 *p;
    Col_Char c;
    size_t i;

    context->length = (0xD7FF-0x00+1) + (0x10FFFF-0xE000+1);
    context->byteLength = (
          2*(  0xD7FF-0x00+1		/* 1-char/2-byte sequences. */
               /* surrogate halves */
             + 0xFFFF-0xE000+1)
        + 4*(0x10FFFF-0x10000+1)	/* 2-char/4-byte sequences. */
    );
    context->data = (Col_Char2 *) malloc(context->byteLength);

    /* 1-char sequences. */
    for (c=0, i=0, p=context->data; c <= 0xFFFF; (c == 0xD7FF ? c = 0xE000 : c++), i++) {
        *p++ = (Col_Char2) c;
    }
    /* 2-char sequences. */
    for (; c <= 0x10FFFF; c++, i++) {
        *p++ = (Col_Char2) ((((c-0x10000)>>10)&0x3FF)+0xD800);
        *p++ = (Col_Char2) (( (c-0x10000)     &0x3FF)+0xDC00);
    }

    ASSERT(p == context->data+(context->byteLength/2));
    ASSERT(i == context->length);
    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();
}
TEST_FIXTURE_TEARDOWN(testRopeUtf16, context) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
    free(context->data);
}
TEST_CASE(testRopeUtf16Create, testRopeUtf16, context) {
    Col_Word rope;
    size_t length;
    
    /* Entire string. */
    rope = Col_NewRope(COL_UTF16, context->data, context->byteLength);
    length = Col_RopeLength(rope);
    ASSERT(length == context->length);

    /* 1-char sequences. */
    rope = Col_NewRope(COL_UTF16, context->data, 2*((0xD7FF-0x00+1)+(0xFFFF-0xE000+1)));
    length = Col_RopeLength(rope);
    ASSERT(length == (0xD7FF-0x00+1)+(0xFFFF-0xE000+1));

    /* 2-char sequences. */
    rope = Col_NewRope(COL_UTF16, context->data+((0xD7FF-0x00+1)+(0xFFFF-0xE000+1)), 4*(0x10FFFF-0x10000+1));
    length = Col_RopeLength(rope);
    ASSERT(length == (0x10FFFF-0x10000+1));
}
TEST_CASE(testRopeUtf16Access, testRopeUtf16, context) {
    ropeAccess(Col_NewRope(COL_UTF16, context->data, context->byteLength));
}


TEST_SUITE(testRopeUtf16Iterate,
    testRopeUtf16IterateForward, testRopeUtf16IterateBackward
)


TEST_CASE(testRopeUtf16IterateForward, testRopeUtf16, context) {
    Col_RopeIterator it;
    Col_Char c1, c2;
    size_t index;
    Col_Word rope = Col_NewRope(COL_UTF16, context->data, context->byteLength);

    for (c1=0, Col_RopeIterFirst(it, rope); !Col_RopeIterEnd(it); (c1 == 0xD7FF ? c1 = 0xE000 : c1++), Col_RopeIterNext(it)) {
        index = Col_RopeIterIndex(it);
        c2 = Col_RopeIterAt(it);
        ASSERT(c1 == c2, "index=%u, c1=U+%x, c2=U+%x", index, c1, c2);
        c2 = Col_RopeAt(rope, index);
        ASSERT(c1 == c2, "index=%u, c1=U+%x, c2=U+%x", index, c1, c2);
    }
    ASSERT(c1 == 0x10FFFF+1);

    for (c1=0, Col_RopeIterString(it, COL_UTF16, context->data, context->length); !Col_RopeIterEnd(it); (c1 == 0xD7FF ? c1 = 0xE000 : c1++), Col_RopeIterNext(it)) {
        index = Col_RopeIterIndex(it);
        c2 = Col_RopeIterAt(it);
        ASSERT(c1 == c2, "index=%u, c1=U+%x, c2=U+%x", index, c1, c2);
        c2 = Col_RopeAt(rope, index);
        ASSERT(c1 == c2, "index=%u, c1=U+%x, c2=U+%x", index, c1, c2);
    }
    ASSERT(c1 == 0x10FFFF+1);
}
TEST_CASE(testRopeUtf16IterateBackward, testRopeUtf16, context) {
    Col_RopeIterator it;
    Col_Char c1, c2;
    size_t index;
    Col_Word rope = Col_NewRope(COL_UTF16, context->data, context->byteLength);

    for (c1=0x10FFFF, Col_RopeIterLast(it, rope); !Col_RopeIterEnd(it); (c1 == 0xE000 ? c1 = 0xD7FF : c1--), Col_RopeIterPrevious(it)) {
        index = Col_RopeIterIndex(it);
        c2 = Col_RopeIterAt(it);
        ASSERT(c1 == c2, "index=%u, c1=U+%x, c2=U+%x", index, c1, c2);
        c2 = Col_RopeAt(rope, index);
        ASSERT(c1 == c2, "index=%u, c1=U+%x, c2=U+%x", index, c1, c2);
    }
    ASSERT(c1 == -1);
}


/*
 * Rope normalization.
 */


TEST_SUITE(testRopeNormalize, 
    testRopeNormalizePrerequisites, testRopeNormalizeSmallChar, 
    testRopeNormalizeSmallString, testRopeNormalizeSubstring,
    testRopeNormalizeConcat
)


TEST_FIXTURE_CONTEXT(testRopeNormalize) {
    Col_Word char1_1;
    Col_Word char2_1;
    Col_Word char2_2;
    Col_Word char4_1;
    Col_Word char4_2;
    Col_Word char4_4;
    Col_Word smallstr;
    Col_Word ucs1_1;
    Col_Word ucs2_1;
    Col_Word ucs2_2;
    Col_Word ucs4_1;
    Col_Word ucs4_2;
    Col_Word ucs4_4;
    Col_Word utf8_1;
    Col_Word utf8_2;
    Col_Word utf8_4;
    Col_Word utf16_1;
    Col_Word utf16_2;
    Col_Word utf16_4;
};
TEST_FIXTURE_SETUP(testRopeNormalize, context) {
    Col_Char1 ucs1[256];
    Col_Char2 ucs2[256+256];
    Col_Char4 ucs4[256+256+256];
    Col_Char1 utf8[384+512+1024];
    Col_Char2 utf16[256+256+512];
    Col_Char c;
    Col_Char1 *p1;
    Col_Char2 *p2;
    Col_Char4 *p4;

    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();

    p1 = ucs1;
    p2 = ucs2;
    p4 = ucs4;
    for (c=0; c <= 0xFF; c++) {
        *p1++ = (Col_Char1) c;
        *p2++ = (Col_Char2) c;
        *p4++ = (Col_Char4) c;
    }
    for (c=0x100; c <= 0x1FF; c++) {
        *p2++ = (Col_Char2) c;
        *p4++ = (Col_Char4) c;
    }
    for (c=0x10000; c <= 0x100FF; c++) {
        *p4++ = (Col_Char4) c;
    }

    p1 = utf8;
    p2 = utf16;
    for (c=0; c <= 0xFF; c++) {
        p1 = Col_Utf8Set(p1, c);
        p2 = Col_Utf16Set(p2, c);
    }
    for (c=0x100; c <= 0x1FF; c++) {
        p1 = Col_Utf8Set(p1, c);
        p2 = Col_Utf16Set(p2, c);
    }
    for (c=0x10000; c <= 0x100FF; c++) {
        p1 = Col_Utf8Set(p1, c);
        p2 = Col_Utf16Set(p2, c);
    }

    context->char1_1 = Col_NewRope(COL_UCS1, ucs1, 1);
    context->char2_1 = Col_NewRope(COL_UCS2, ucs2, 2);
    context->char2_2 = Col_NewRope(COL_UCS2, ucs2+256, 2);
    context->char4_1 = Col_NewRope(COL_UCS4, ucs4, 4);
    context->char4_2 = Col_NewRope(COL_UCS4, ucs4+256, 4);
    context->char4_4 = Col_NewRope(COL_UCS4, ucs4+256+256, 4);

    context->smallstr = Col_NewRope(COL_UCS1, ucs1, 3);

    context->ucs1_1 = Col_NewRope(COL_UCS1, ucs1, 256);

    context->ucs2_1 = Col_NewRope(COL_UCS2, ucs2, 512);
    context->ucs2_2 = Col_NewRope(COL_UCS2, ucs2+256, 512);

    context->ucs4_1 = Col_NewRope(COL_UCS4, ucs4, 1024);
    context->ucs4_2 = Col_NewRope(COL_UCS4, ucs4+256, 1024);
    context->ucs4_4 = Col_NewRope(COL_UCS4, ucs4+256+256, 1024);

    context->utf8_1 = Col_NewRope(COL_UTF8, utf8, 384);
    context->utf8_2 = Col_NewRope(COL_UTF8, utf8+384, 512);
    context->utf8_4 = Col_NewRope(COL_UTF8, utf8+384+512, 1024);

    context->utf16_1 = Col_NewRope(COL_UTF16, utf16, 512);
    context->utf16_2 = Col_NewRope(COL_UTF16, utf16+256, 512);
    context->utf16_4 = Col_NewRope(COL_UTF16, utf16+256+256, 1024);
}
TEST_FIXTURE_TEARDOWN(testRopeNormalize, context) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
}

TEST_CASE(testRopeNormalizePrerequisites, testRopeNormalize, context) {
    ASSERT(ropeCheckFormat(context->char1_1, COL_UCS1));
    ASSERT(ropeCheckFormat(context->char1_1, COL_UCS));
    ASSERT(ropeCheckFormat(context->char2_1, COL_UCS2));
    ASSERT(ropeCheckFormat(context->char2_1, COL_UCS));
    ASSERT(ropeCheckFormat(context->char2_2, COL_UCS2));
    ASSERT(ropeCheckFormat(context->char2_2, COL_UCS));
    ASSERT(ropeCheckFormat(context->char4_1, COL_UCS4));
    ASSERT(ropeCheckFormat(context->char4_1, COL_UCS));
    ASSERT(ropeCheckFormat(context->char4_2, COL_UCS4));
    ASSERT(ropeCheckFormat(context->char4_2, COL_UCS));
    ASSERT(ropeCheckFormat(context->char4_4, COL_UCS4));
    ASSERT(ropeCheckFormat(context->char4_4, COL_UCS));
    ASSERT(ropeCheckFormat(context->smallstr, COL_UCS1));
    ASSERT(ropeCheckFormat(context->smallstr, COL_UCS));
    ASSERT(ropeCheckFormat(context->ucs1_1, COL_UCS1));
    ASSERT(ropeCheckFormat(context->ucs1_1, COL_UCS));
    ASSERT(ropeCheckFormat(context->ucs2_1, COL_UCS2));
    ASSERT(ropeCheckFormat(context->ucs2_1, COL_UCS));
    ASSERT(ropeCheckFormat(context->ucs2_2, COL_UCS2));
    ASSERT(ropeCheckFormat(context->ucs2_2, COL_UCS));
    ASSERT(ropeCheckFormat(context->ucs4_1, COL_UCS4));
    ASSERT(ropeCheckFormat(context->ucs4_1, COL_UCS));
    ASSERT(ropeCheckFormat(context->ucs4_2, COL_UCS4));
    ASSERT(ropeCheckFormat(context->ucs4_2, COL_UCS));
    ASSERT(ropeCheckFormat(context->ucs4_4, COL_UCS4));
    ASSERT(ropeCheckFormat(context->ucs4_4, COL_UCS));
    ASSERT(ropeCheckFormat(context->utf8_1, COL_UTF8));
    ASSERT(ropeCheckFormat(context->utf8_2, COL_UTF8));
    ASSERT(ropeCheckFormat(context->utf8_4, COL_UTF8));
    ASSERT(ropeCheckFormat(context->utf16_1, COL_UTF16));
    ASSERT(ropeCheckFormat(context->utf16_2, COL_UTF16));
    ASSERT(ropeCheckFormat(context->utf16_4, COL_UTF16));

    ASSERT(context->char1_1 != context->char2_1);
    ASSERT(context->char1_1 != context->char4_1);

    ASSERT(context->char2_1 != context->char4_1);

    ASSERT(context->char2_2 != context->char4_2);

    ropesEqual(context->char1_1, context->char2_1);
    ropesEqual(context->char1_1, context->char4_1);

    ropesEqual(context->char2_2, context->char4_2);

    ropesEqual(context->ucs1_1, context->ucs2_1);
    ropesEqual(context->ucs1_1, context->ucs4_1);
    ropesEqual(context->ucs1_1, context->utf8_1);
    ropesEqual(context->ucs1_1, context->utf16_1);

    ropesEqual(context->ucs2_2, context->ucs4_2);
    ropesEqual(context->ucs2_2, context->utf8_2);
    ropesEqual(context->ucs2_2, context->utf16_2);

    ropesEqual(context->ucs4_4, context->utf8_4);
    ropesEqual(context->ucs4_4, context->utf16_4);
}
TEST_CASE(testRopeNormalizeSmallChar, testRopeNormalize, context) {
    Col_Char c1_1 = Col_RopeAt(context->char1_1, 0);
    Col_Char c2_1 = Col_RopeAt(context->char2_1, 0);
    Col_Char c2_2 = Col_RopeAt(context->char2_2, 0);
    Col_Char c4_1 = Col_RopeAt(context->char4_1, 0);
    Col_Char c4_2 = Col_RopeAt(context->char4_2, 0);
    Col_Char c4_4 = Col_RopeAt(context->char4_4, 0);

    ASSERT(c1_1 == c2_1);
    ASSERT(c1_1 == c4_1);
    ASSERT(c2_1 == c4_1);
    ASSERT(c2_2 == c4_2);

    ASSERT(context->char1_1 == Col_NewCharWord(c1_1));
    ASSERT(context->char1_1 == Col_NewCharWord(c2_1));
    ASSERT(context->char1_1 == Col_NewCharWord(c4_1));

    ASSERT(context->char2_2 == Col_NewCharWord(c2_2));
    ASSERT(context->char2_2 == Col_NewCharWord(c4_2));

    ASSERT(context->char4_2 != Col_NewCharWord(c4_2));
    ASSERT(context->char4_4 == Col_NewCharWord(c4_4));

    /* Lossless conversions. */
    ASSERT(Col_NormalizeRope(context->char1_1, COL_UCS1, COL_CHAR_INVALID, 0) == context->char1_1);
    ASSERT(Col_NormalizeRope(context->char1_1, COL_UCS1, COL_CHAR_INVALID, 1) == context->char1_1);
    ASSERT(Col_NormalizeRope(context->char1_1, COL_UCS2, COL_CHAR_INVALID, 0) == context->char2_1);
    ASSERT(Col_NormalizeRope(context->char1_1, COL_UCS2, COL_CHAR_INVALID, 1) == context->char2_1);
    ASSERT(Col_NormalizeRope(context->char1_1, COL_UCS4, COL_CHAR_INVALID, 0) == context->char4_1);
    ASSERT(Col_NormalizeRope(context->char1_1, COL_UCS4, COL_CHAR_INVALID, 1) == context->char4_1);
    ASSERT(Col_NormalizeRope(context->char1_1, COL_UCS, COL_CHAR_INVALID, 0) == context->char1_1);
    ASSERT(Col_NormalizeRope(context->char1_1, COL_UCS, COL_CHAR_INVALID, 1) == context->char1_1);

    ASSERT(Col_NormalizeRope(context->char2_1, COL_UCS1, COL_CHAR_INVALID, 0) == context->char1_1);
    ASSERT(Col_NormalizeRope(context->char2_1, COL_UCS1, COL_CHAR_INVALID, 1) == context->char1_1);
    ASSERT(Col_NormalizeRope(context->char2_1, COL_UCS2, COL_CHAR_INVALID, 0) == context->char2_1);
    ASSERT(Col_NormalizeRope(context->char2_1, COL_UCS2, COL_CHAR_INVALID, 1) == context->char2_1);
    ASSERT(Col_NormalizeRope(context->char2_1, COL_UCS4, COL_CHAR_INVALID, 0) == context->char4_1);
    ASSERT(Col_NormalizeRope(context->char2_1, COL_UCS4, COL_CHAR_INVALID, 1) == context->char4_1);
    ASSERT(Col_NormalizeRope(context->char2_1, COL_UCS, COL_CHAR_INVALID, 0) == context->char1_1);
    ASSERT(Col_NormalizeRope(context->char2_1, COL_UCS, COL_CHAR_INVALID, 1) == context->char1_1);

    ASSERT(Col_NormalizeRope(context->char2_2, COL_UCS2, COL_CHAR_INVALID, 0) == context->char2_2);
    ASSERT(Col_NormalizeRope(context->char2_2, COL_UCS2, COL_CHAR_INVALID, 1) == context->char2_2);
    ASSERT(Col_NormalizeRope(context->char2_2, COL_UCS4, COL_CHAR_INVALID, 0) == context->char4_2);
    ASSERT(Col_NormalizeRope(context->char2_2, COL_UCS4, COL_CHAR_INVALID, 1) == context->char4_2);
    ASSERT(Col_NormalizeRope(context->char2_2, COL_UCS, COL_CHAR_INVALID, 0) == context->char2_2);
    ASSERT(Col_NormalizeRope(context->char2_2, COL_UCS, COL_CHAR_INVALID, 1) == context->char2_2);

    ASSERT(Col_NormalizeRope(context->char4_1, COL_UCS1, COL_CHAR_INVALID, 0) == context->char1_1);
    ASSERT(Col_NormalizeRope(context->char4_1, COL_UCS1, COL_CHAR_INVALID, 1) == context->char1_1);
    ASSERT(Col_NormalizeRope(context->char4_1, COL_UCS2, COL_CHAR_INVALID, 0) == context->char2_1);
    ASSERT(Col_NormalizeRope(context->char4_1, COL_UCS2, COL_CHAR_INVALID, 1) == context->char2_1);
    ASSERT(Col_NormalizeRope(context->char4_1, COL_UCS4, COL_CHAR_INVALID, 0) == context->char4_1);
    ASSERT(Col_NormalizeRope(context->char4_1, COL_UCS4, COL_CHAR_INVALID, 1) == context->char4_1);
    ASSERT(Col_NormalizeRope(context->char4_1, COL_UCS, COL_CHAR_INVALID, 0) == context->char1_1);
    ASSERT(Col_NormalizeRope(context->char4_1, COL_UCS, COL_CHAR_INVALID, 1) == context->char1_1);

    ASSERT(Col_NormalizeRope(context->char4_2, COL_UCS2, COL_CHAR_INVALID, 0) == context->char2_2);
    ASSERT(Col_NormalizeRope(context->char4_2, COL_UCS2, COL_CHAR_INVALID, 1) == context->char2_2);
    ASSERT(Col_NormalizeRope(context->char4_2, COL_UCS4, COL_CHAR_INVALID, 0) == context->char4_2);
    ASSERT(Col_NormalizeRope(context->char4_2, COL_UCS4, COL_CHAR_INVALID, 1) == context->char4_2);
    ASSERT(Col_NormalizeRope(context->char4_2, COL_UCS, COL_CHAR_INVALID, 0) == context->char2_2);
    ASSERT(Col_NormalizeRope(context->char4_2, COL_UCS, COL_CHAR_INVALID, 1) == context->char2_2);

    ASSERT(Col_NormalizeRope(context->char4_4, COL_UCS4, COL_CHAR_INVALID, 0) == context->char4_4);
    ASSERT(Col_NormalizeRope(context->char4_4, COL_UCS4, COL_CHAR_INVALID, 1) == context->char4_4);
    ASSERT(Col_NormalizeRope(context->char4_4, COL_UCS, COL_CHAR_INVALID, 0) == context->char4_4);
    ASSERT(Col_NormalizeRope(context->char4_4, COL_UCS, COL_CHAR_INVALID, 1) == context->char4_4);

    /* Lossy conversions. */
    ASSERT(Col_NormalizeRope(context->char2_2, COL_UCS1, COL_CHAR_INVALID, 0) == Col_EmptyRope());
    ASSERT(Col_NormalizeRope(context->char2_2, COL_UCS1, c2_2, 0) == Col_EmptyRope());
    ASSERT(Col_NormalizeRope(context->char2_2, COL_UCS1, c4_4, 0) == Col_EmptyRope());

    ASSERT(Col_NormalizeRope(context->char4_2, COL_UCS1, COL_CHAR_INVALID, 0) == Col_EmptyRope());
    ASSERT(Col_NormalizeRope(context->char4_2, COL_UCS1, c2_2, 0) == Col_EmptyRope());
    ASSERT(Col_NormalizeRope(context->char4_2, COL_UCS1, c4_4, 0) == Col_EmptyRope());

    ASSERT(Col_NormalizeRope(context->char4_4, COL_UCS1, COL_CHAR_INVALID, 0) == Col_EmptyRope());
    ASSERT(Col_NormalizeRope(context->char4_4, COL_UCS1, c2_2, 0) == Col_EmptyRope());
    ASSERT(Col_NormalizeRope(context->char4_4, COL_UCS1, c4_4, 0) == Col_EmptyRope());
    ASSERT(Col_NormalizeRope(context->char4_4, COL_UCS2, COL_CHAR_INVALID, 0) == Col_EmptyRope());
    ASSERT(Col_NormalizeRope(context->char4_4, COL_UCS2, c4_4, 0) == Col_EmptyRope());

    /* Conversions with replacement char. */
    ASSERT(Col_NormalizeRope(context->char2_2, COL_UCS1, c1_1, 0) == context->char1_1);

    ASSERT(Col_NormalizeRope(context->char4_2, COL_UCS1, c1_1, 0) == context->char1_1);

    ASSERT(Col_NormalizeRope(context->char4_4, COL_UCS1, c1_1, 0) == context->char1_1);
    ASSERT(Col_NormalizeRope(context->char4_4, COL_UCS2, c1_1, 0) == context->char2_1);
    ASSERT(Col_NormalizeRope(context->char4_4, COL_UCS2, c2_2, 0) == context->char2_2);
}

TEST_CASE(testRopeNormalizeSmallString, testRopeNormalize, context) {
    static const Col_StringFormat formats[] = {COL_UCS1, COL_UCS2, COL_UCS4, 
            COL_UCS, COL_UTF8, COL_UTF16};
    int i, flatten;
    Col_Word normalized;
    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->smallstr, formats[i], 
                    COL_CHAR_INVALID, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                case COL_UCS:
                    ASSERT(normalized == context->smallstr);
                    break;

                default:
                    ASSERT(normalized != context->smallstr);
                    ropesEqual(normalized, context->smallstr);
            }
        }
    }
}

TEST_SUITE(testRopeNormalizeString,
    testRopeNormalizeStringLossless, testRopeNormalizeStringLossy, 
    testRopeNormalizeStringReplace
)

TEST_CASE(testRopeNormalizeStringLossless, testRopeNormalize, context) {
    static const Col_StringFormat formats[] = {COL_UCS1, COL_UCS2, COL_UCS4, 
            COL_UCS, COL_UTF8, COL_UTF16};
    int i, flatten;
    Col_Word normalized;
    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->ucs1_1, formats[i], 
                    COL_CHAR_INVALID, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                case COL_UCS:
                    ASSERT(normalized == context->ucs1_1);
                    break;

                default:
                    ASSERT(normalized != context->ucs1_1);
                    ropesEqual(normalized, context->ucs1_1);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->ucs2_1, formats[i], 
                    COL_CHAR_INVALID, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS2:
                case COL_UCS:
                    ASSERT(normalized == context->ucs2_1);
                    break;

                default:
                    ASSERT(normalized != context->ucs2_1);
                    ropesEqual(normalized, context->ucs2_1);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->ucs4_1, formats[i], 
                    COL_CHAR_INVALID, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS4:
                case COL_UCS:
                    ASSERT(normalized == context->ucs4_1);
                    break;

                default:
                    ASSERT(normalized != context->ucs4_1);
                    ropesEqual(normalized, context->ucs4_1);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->utf8_1, formats[i], 
                    COL_CHAR_INVALID, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UTF8:
                    ASSERT(normalized == context->utf8_1);
                    break;

                default:
                    ASSERT(normalized != context->utf8_1);
                    ropesEqual(normalized, context->utf8_1);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->utf16_1, formats[i], 
                    COL_CHAR_INVALID, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UTF16:
                    ASSERT(normalized == context->utf16_1);
                    break;

                default:
                    ASSERT(normalized != context->utf16_1);
                    ropesEqual(normalized, context->utf16_1);
            }
        }
    }
}

TEST_CASE(testRopeNormalizeStringLossy, testRopeNormalize, context) {
    static const Col_StringFormat formats[] = {COL_UCS1, COL_UCS2, COL_UCS4, 
            COL_UCS, COL_UTF8, COL_UTF16};
    int i, flatten;
    Col_Word normalized;
    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->ucs2_2, formats[i], 
                    COL_CHAR_INVALID, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                    ASSERT(normalized == Col_EmptyRope());
                    break;

                case COL_UCS2:
                case COL_UCS:
                    ASSERT(normalized == context->ucs2_2);
                    break;

                default:
                    ASSERT(normalized != context->ucs2_2);
                    ropesEqual(normalized, context->ucs2_2);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->ucs4_2, formats[i], 
                    COL_CHAR_INVALID, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                    ASSERT(normalized == Col_EmptyRope());
                    break;

                case COL_UCS4:
                case COL_UCS:
                    ASSERT(normalized == context->ucs4_2);
                    break;

                default:
                    ASSERT(normalized != context->ucs4_2);
                    ropesEqual(normalized, context->ucs4_2);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->ucs4_4, formats[i], 
                    COL_CHAR_INVALID, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                case COL_UCS2:
                    ASSERT(normalized == Col_EmptyRope());
                    break;

                case COL_UCS4:
                case COL_UCS:
                    ASSERT(normalized == context->ucs4_4);
                    break;

                default:
                    ASSERT(normalized != context->ucs4_4);
                    ropesEqual(normalized, context->ucs4_4);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->utf8_2, formats[i], 
                    COL_CHAR_INVALID, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                    ASSERT(normalized == Col_EmptyRope());
                    break;

                case COL_UTF8:
                    ASSERT(normalized == context->utf8_2);
                    break;

                default:
                    ASSERT(normalized != context->utf8_2);
                    ropesEqual(normalized, context->utf8_2);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->utf8_4, formats[i], 
                    COL_CHAR_INVALID, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                case COL_UCS2:
                    ASSERT(normalized == Col_EmptyRope());
                    break;

                case COL_UTF8:
                    ASSERT(flatten ? (normalized != context->utf8_4) : (normalized == context->utf8_4));
                    ropesEqual(normalized, context->utf8_4);
                    break;

                default:
                    ASSERT(normalized != context->utf8_4);
                    ropesEqual(normalized, context->utf8_4);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->utf16_2, formats[i], 
                    COL_CHAR_INVALID, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                    ASSERT(normalized == Col_EmptyRope());
                    break;

                case COL_UTF16:
                    ASSERT(normalized == context->utf16_2);
                    ropesEqual(normalized, context->utf16_2);
                    break;

                default:
                    ASSERT(normalized != context->utf16_2);
                    ropesEqual(normalized, context->utf16_2);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->utf16_4, formats[i], 
                    COL_CHAR_INVALID, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                case COL_UCS2:
                    ASSERT(normalized == Col_EmptyRope());
                    break;

                case COL_UTF16:
                    ASSERT(normalized == context->utf16_4 || flatten);
                    ropesEqual(normalized, context->utf16_4);
                    break;

                default:
                    ASSERT(normalized != context->utf16_4);
                    ropesEqual(normalized, context->utf16_4);
            }
        }
    }
}

TEST_CASE(testRopeNormalizeStringReplace, testRopeNormalize, context) {
    static const Col_StringFormat formats[] = {COL_UCS1, COL_UCS2, COL_UCS4, 
            COL_UCS, COL_UTF8, COL_UTF16};
    int i, flatten;
    Col_Word normalized;
    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->ucs2_2, formats[i], 
                    0, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                    ropesEqual(normalized, Col_RepeatRope(Col_NewCharWord(0), Col_RopeLength(context->ucs2_2)));
                    break;

                case COL_UCS2:
                case COL_UCS:
                    ASSERT(normalized == context->ucs2_2);
                    break;

                default:
                    ASSERT(normalized != context->ucs2_2);
                    ropesEqual(normalized, context->ucs2_2);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->ucs4_2, formats[i], 
                    0, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                    ropesEqual(normalized, Col_RepeatRope(Col_NewCharWord(0), Col_RopeLength(context->ucs4_2)));
                    break;

                case COL_UCS4:
                case COL_UCS:
                    ASSERT(normalized == context->ucs4_2);
                    break;

                default:
                    ASSERT(normalized != context->ucs4_2);
                    ropesEqual(normalized, context->ucs4_2);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->ucs4_4, formats[i], 
                    0, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                case COL_UCS2:
                    ropesEqual(normalized, Col_RepeatRope(Col_NewCharWord(0), Col_RopeLength(context->ucs4_4)));
                    break;

                case COL_UCS4:
                case COL_UCS:
                    ASSERT(normalized == context->ucs4_4);
                    break;

                default:
                    ASSERT(normalized != context->ucs4_4);
                    ropesEqual(normalized, context->ucs4_4);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->utf8_2, formats[i], 
                    0, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                    ropesEqual(normalized, Col_RepeatRope(Col_NewCharWord(0), Col_RopeLength(context->utf8_2)));
                    break;

                case COL_UTF8:
                    ASSERT(normalized == context->utf8_2);
                    break;

                default:
                    ASSERT(normalized != context->utf8_2);
                    ropesEqual(normalized, context->utf8_2);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->utf8_4, formats[i], 
                    0, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                case COL_UCS2:
                    ropesEqual(normalized, Col_RepeatRope(Col_NewCharWord(0), Col_RopeLength(context->utf8_4)));
                    break;

                case COL_UTF8:
                    ASSERT(flatten ? (normalized != context->utf8_4) : (normalized == context->utf8_4));
                    ropesEqual(normalized, context->utf8_4);
                    break;

                default:
                    ASSERT(normalized != context->utf8_4);
                    ropesEqual(normalized, context->utf8_4);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->utf16_2, formats[i], 
                    0, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                    ropesEqual(normalized, Col_RepeatRope(Col_NewCharWord(0), Col_RopeLength(context->utf16_2)));
                    break;

                case COL_UTF16:
                    ASSERT(normalized == context->utf16_2);
                    ropesEqual(normalized, context->utf16_2);
                    break;

                default:
                    ASSERT(normalized != context->utf16_2);
                    ropesEqual(normalized, context->utf16_2);
            }
        }
    }

    for (i=0; i < sizeof(formats)/sizeof(*formats); i++) {
        for (flatten=0; flatten <= 1; flatten++) {
            normalized = Col_NormalizeRope(context->utf16_4, formats[i], 
                    0, flatten);
            ASSERT(ropeCheckFormat(normalized, formats[i]));
            switch (formats[i]) {
                case COL_UCS1:
                case COL_UCS2:
                    ropesEqual(normalized, Col_RepeatRope(Col_NewCharWord(0), Col_RopeLength(context->utf16_4)));
                    break;

                case COL_UTF16:
                    ASSERT(normalized == context->utf16_4 || flatten);
                    ropesEqual(normalized, context->utf16_4);
                    break;

                default:
                    ASSERT(normalized != context->utf16_4);
                    ropesEqual(normalized, context->utf16_4);
            }
        }
    }
}

TEST_CASE(testRopeNormalizeSubstring, testRopeNormalize, context) {
    Col_Word rope, normalized, flattened;

    rope = Col_Subrope(context->ucs1_1, 1, SIZE_MAX);
    normalized = Col_NormalizeRope(rope, COL_UCS, COL_CHAR_INVALID, 0);
    ASSERT(ropeCheckFormat(normalized, COL_UCS1));
    ASSERT(normalized == rope);
    flattened = Col_NormalizeRope(rope, COL_UCS, COL_CHAR_INVALID, 1);
    ASSERT(ropeCheckFormat(flattened, COL_UCS1));
    ASSERT(flattened != rope);
    ropesEqual(normalized, flattened);

    rope = Col_ConcatRopes(rope, Col_Subrope(context->ucs2_2, 1, 
            SIZE_MAX));
    normalized = Col_NormalizeRope(rope, COL_UCS, COL_CHAR_INVALID, 0);
    ASSERT(ropeCheckFormat(normalized, COL_UCS));
    ASSERT(!ropeCheckFormat(normalized, COL_UCS1));
    ASSERT(!ropeCheckFormat(normalized, COL_UCS2));
    ASSERT(normalized == rope);
    flattened = Col_NormalizeRope(rope, COL_UCS, COL_CHAR_INVALID, 1);
    ASSERT(ropeCheckFormat(flattened, COL_UCS));
    ASSERT(ropeCheckFormat(flattened, COL_UCS2));
    ASSERT(flattened != rope);
    ropesEqual(normalized, flattened);

    rope = Col_Subrope(rope, 1, SIZE_MAX);
    normalized = Col_NormalizeRope(rope, COL_UCS, COL_CHAR_INVALID, 0);
    ASSERT(ropeCheckFormat(normalized, COL_UCS));
    ASSERT(!ropeCheckFormat(normalized, COL_UCS1));
    ASSERT(!ropeCheckFormat(normalized, COL_UCS2));
    ASSERT(normalized == rope);
    flattened = Col_NormalizeRope(rope, COL_UCS, COL_CHAR_INVALID, 1);
    ASSERT(ropeCheckFormat(flattened, COL_UCS));
    ASSERT(ropeCheckFormat(flattened, COL_UCS2));
    ASSERT(flattened != rope);
    ropesEqual(normalized, flattened);

    rope = Col_ConcatRopes(Col_Subrope(context->ucs4_4, 1, 
            SIZE_MAX), rope);
    normalized = Col_NormalizeRope(rope, COL_UCS, COL_CHAR_INVALID, 0);
    ASSERT(ropeCheckFormat(normalized, COL_UCS));
    ASSERT(!ropeCheckFormat(normalized, COL_UCS1));
    ASSERT(!ropeCheckFormat(normalized, COL_UCS2));
    ASSERT(!ropeCheckFormat(normalized, COL_UCS4));
    ASSERT(normalized == rope);
    flattened = Col_NormalizeRope(rope, COL_UCS, COL_CHAR_INVALID, 1);
    ASSERT(ropeCheckFormat(flattened, COL_UCS));
    ASSERT(ropeCheckFormat(flattened, COL_UCS4));
    ASSERT(flattened != rope);
    ropesEqual(normalized, flattened);

    rope = Col_Subrope(rope, 1, SIZE_MAX);
    normalized = Col_NormalizeRope(rope, COL_UCS, COL_CHAR_INVALID, 0);
    ASSERT(ropeCheckFormat(normalized, COL_UCS));
    ASSERT(!ropeCheckFormat(normalized, COL_UCS1));
    ASSERT(!ropeCheckFormat(normalized, COL_UCS2));
    ASSERT(!ropeCheckFormat(normalized, COL_UCS4));
    ASSERT(normalized == rope);
    flattened = Col_NormalizeRope(rope, COL_UCS, COL_CHAR_INVALID, 1);
    ASSERT(ropeCheckFormat(flattened, COL_UCS));
    ASSERT(ropeCheckFormat(flattened, COL_UCS4));
    ASSERT(flattened != rope);
    ropesEqual(normalized, flattened);
}

TEST_CASE(testRopeNormalizeConcat, testRopeNormalize, context) {
    Col_Word rope, normalized, flattened;

    rope = Col_ConcatRopes(context->ucs1_1, context->ucs2_1);
    normalized = Col_NormalizeRope(rope, COL_UCS, COL_CHAR_INVALID, 0);
    ASSERT(ropeCheckFormat(normalized, COL_UCS));
    ASSERT(!ropeCheckFormat(normalized, COL_UCS1));
    ASSERT(!ropeCheckFormat(normalized, COL_UCS2));
    ASSERT(normalized == rope);
    flattened = Col_NormalizeRope(rope, COL_UCS, COL_CHAR_INVALID, 1);
    ASSERT(ropeCheckFormat(flattened, COL_UCS));
    ASSERT(ropeCheckFormat(flattened, COL_UCS1));
    ASSERT(flattened != rope);
    ropesEqual(normalized, flattened);

    rope = Col_ConcatRopes(context->ucs1_1, context->ucs2_2);
    normalized = Col_NormalizeRope(rope, COL_UCS, COL_CHAR_INVALID, 0);
    ASSERT(ropeCheckFormat(normalized, COL_UCS));
    ASSERT(!ropeCheckFormat(normalized, COL_UCS1));
    ASSERT(!ropeCheckFormat(normalized, COL_UCS2));
    ASSERT(normalized == rope);
    flattened = Col_NormalizeRope(rope, COL_UCS, COL_CHAR_INVALID, 1);
    ASSERT(ropeCheckFormat(flattened, COL_UCS));
    ASSERT(ropeCheckFormat(flattened, COL_UCS2));
    ASSERT(flattened != rope);
    ropesEqual(normalized, flattened);

    normalized = Col_NormalizeRope(rope, COL_UCS1, COL_CHAR_INVALID, 0);
    ASSERT(ropeCheckFormat(normalized, COL_UCS));
    ASSERT(ropeCheckFormat(normalized, COL_UCS1));
    ASSERT(normalized == context->ucs1_1);
    flattened = Col_NormalizeRope(rope, COL_UCS1, COL_CHAR_INVALID, 1);
    ASSERT(ropeCheckFormat(flattened, COL_UCS));
    ASSERT(ropeCheckFormat(flattened, COL_UCS1));
    ASSERT(flattened != rope);
    ropesEqual(normalized, flattened);
}


/*
 * Generic tests.
 */

typedef struct TraverseRopeChunksInfo {
    Col_Word rope;
    size_t index;
} TraverseRopeChunksInfo;
static int traverseRopeChunks(size_t index, size_t length, size_t number, const Col_RopeChunk *chunks, Col_ClientData clientData) {
    TraverseRopeChunksInfo *info = (TraverseRopeChunksInfo *) clientData;
    size_t i;
    const void *p = chunks->data;
    ASSERT(number == 1);
    ASSERT(index == info->index, "index=%u, info->index=%u", index, info->index);
    for (i = 0; i < length; i++, COL_CHAR_NEXT(chunks->format, p)) {
        Col_Char c1 = Col_RopeAt(info->rope, info->index+i);
        Col_Char c2 = COL_CHAR_GET(chunks->format, p);
        ASSERT(c1 == c2, "index=%u, c1=U+%x, c2=U+%x", info->index+i, c1, c2);
    }
    info->index += length;
    return 0;
}
static int traverseRopeChunksR(size_t index, size_t length, size_t number, const Col_RopeChunk *chunks, Col_ClientData clientData) {
    TraverseRopeChunksInfo *info = (TraverseRopeChunksInfo *) clientData;
    size_t i;
    const void *p = chunks->data;
    ASSERT(number == 1);
    ASSERT(index == info->index-length+1, "index=%u, info->index-length+1=%u", index, info->index-length+1);
    for (i = 0; i < length; i++, COL_CHAR_NEXT(chunks->format, p)) {
        Col_Char c1 = Col_RopeAt(info->rope, info->index-length+1+i);
        Col_Char c2 = COL_CHAR_GET(chunks->format, p);
        ASSERT(c1 == c2, "index=%u, c1=U+%x, c2=U+%x", info->index-length+1+i, c1, c2);
    }
    info->index -= length;
    return 0;
}
static void ropeAccess(Col_Word rope) {
    /*
     * Check that direct access and iterators give the same results.
     */

    Col_RopeIterator it;
    TraverseRopeChunksInfo info;
    size_t size;

    /* Forward. */
    for (Col_RopeIterFirst(it, rope); !Col_RopeIterEnd(it); Col_RopeIterNext(it)) {
        size_t index = Col_RopeIterIndex(it);
        Col_Char c1 = Col_RopeAt(rope, index);
        Col_Char c2 = Col_RopeIterAt(it);

        ASSERT(c1 == c2, "index=%u", index);
    }

    /* Backward. */
    for (Col_RopeIterLast(it, rope); !Col_RopeIterEnd(it); Col_RopeIterPrevious(it)) {
        size_t index = Col_RopeIterIndex(it);
        Col_Char c1 = Col_RopeAt(rope, index);
        Col_Char c2 = Col_RopeIterAt(it);

        ASSERT(c1 == c2, "index=%u", index);
    }

    /* Chunks. */
    info.rope = rope;
    info.index = 0;
    size = 0;
    Col_TraverseRopeChunks(rope, info.index, SIZE_MAX, 0, traverseRopeChunks, 
            (Col_ClientData) &info, &size);
    ASSERT(info.index == size, "info.index=%u, size=%u", info.index, size);
    ASSERT(size == Col_RopeLength(rope), "size=%u, length=%u", size, Col_RopeLength(rope));

    /* Chunks (reverse). */
    info.rope = rope;
    info.index = Col_RopeLength(rope)-1;
    size = 0;
    Col_TraverseRopeChunks(rope, info.index, SIZE_MAX, 1, traverseRopeChunksR, 
            (Col_ClientData) &info, &size);
    ASSERT(info.index == (size_t)-1, "info.index=%u", info.index);
    ASSERT(size == Col_RopeLength(rope), "size=%u, length=%u", size, Col_RopeLength(rope));
}
static void ropeInsert(Col_Word into, Col_Word rope) {
     /*
     * Insert a rope into another one and compare with expected result.
     */

    size_t length = Col_RopeLength(into);
    size_t indices[] = {0, 1, length/2, length-1, length};
    size_t i;
    Col_Word result;
    Col_RopeIterator itResult, itInto, itRope;

    for (i=0; i < sizeof(indices)/sizeof(*indices); i++) {
        size_t where = indices[i];

        result = Col_RopeInsert(into, where, rope);
        ASSERT(Col_RopeLength(result) == Col_RopeLength(into)+Col_RopeLength(rope));
        ropeAccess(result);

        /*
         * Compare result with expected result.
         */

        /* Part before insertion point. */
        for (Col_RopeIterFirst(itResult, result), Col_RopeIterFirst(itInto, into);
                !Col_RopeIterEnd(itInto) && Col_RopeIterIndex(itInto) < where; 
                Col_RopeIterNext(itResult), Col_RopeIterNext(itInto)) {
            Col_Char c1 = Col_RopeIterAt(itResult);
            Col_Char c2 = Col_RopeIterAt(itInto);

            ASSERT(c1 == c2, "where=%u, index=%u", where, Col_RopeIterIndex(itResult));
        }

        /* Inserted rope. */
        for (Col_RopeIterFirst(itRope, rope);
                !Col_RopeIterEnd(itRope); 
                Col_RopeIterNext(itResult), Col_RopeIterNext(itRope)) {
            Col_Char c1 = Col_RopeIterAt(itResult);
            Col_Char c2 = Col_RopeIterAt(itRope);

            ASSERT(c1 == c2, "where=%u, index=%u", where, Col_RopeIterIndex(itResult));
        }

        /* Part after insertion point. */
        for (;
                !Col_RopeIterEnd(itInto); 
                Col_RopeIterNext(itResult), Col_RopeIterNext(itInto)) {
            Col_Char c1 = Col_RopeIterAt(itResult);
            Col_Char c2 = Col_RopeIterAt(itInto);

            ASSERT(c1 == c2, "where=%u, index=%u", where, Col_RopeIterIndex(itResult));
        }
        ASSERT(Col_RopeIterEnd(itResult));
    }
}
static void ropeRemove(Col_Word rope) {
     /*
     * Remove a section of rope and compare with expected result.
     */

    size_t length = Col_RopeLength(rope);
    size_t indices[] = {0, 1, length/2, length-1, length};
    size_t i1, i2;
    Col_Word result;
    Col_RopeIterator itResult, itRope;

    for (i1=0; i1 < sizeof(indices)/sizeof(*indices); i1++) {
        size_t last = indices[i1];

        for (i2=0; i2 < sizeof(indices)/sizeof(*indices); i2++) {
            size_t first = indices[i2];

            if (first > last) break;

            result = Col_RopeRemove(rope, first, last);
            ASSERT(Col_RopeLength(result) == Col_RopeLength(rope)
                    - ((last<length?last:length-1)-first+1));
            ropeAccess(result);

            /*
             * Compare result with expected result.
             */

            /* Part before deleted section.*/
            for (Col_RopeIterFirst(itResult, result), Col_RopeIterFirst(itRope, rope);
                    !Col_RopeIterEnd(itRope) && Col_RopeIterIndex(itRope) < first; 
                    Col_RopeIterNext(itResult), Col_RopeIterNext(itRope)) {
                Col_Char c1 = Col_RopeIterAt(itResult);
                Col_Char c2 = Col_RopeIterAt(itRope);

                ASSERT(c1 == c2, "index=%u", Col_RopeIterIndex(itResult));
            }

            /* Part after deleted section.*/
            if (last < length) {
                Col_RopeIterMoveTo(itRope, last+1);
                for (;
                        !Col_RopeIterEnd(itRope); 
                        Col_RopeIterNext(itResult), Col_RopeIterNext(itRope)) {
                    Col_Char c1 = Col_RopeIterAt(itResult);
                    Col_Char c2 = Col_RopeIterAt(itRope);

                    ASSERT(c1 == c2, "index=%u", Col_RopeIterIndex(itResult));
                }
            }
            ASSERT(Col_RopeIterEnd(itResult));
        }
    }
}
static void ropeReplace(Col_Word rope, Col_Word with) {
     /*
     * Replace a section of rope with another and compare with expected result.
     */

    size_t length = Col_RopeLength(rope);
    size_t indices[] = {0, 1, length/2, length-1, length};
    size_t i1, i2;
    Col_Word result;
    Col_RopeIterator itResult, itRope, itWith;

    for (i1=0; i1 < sizeof(indices)/sizeof(*indices); i1++) {
        size_t last = indices[i1];

        for (i2=0; i2 < sizeof(indices)/sizeof(*indices); i2++) {
            size_t first = indices[i2];

            if (first > last) break;

            result = Col_RopeReplace(rope, first, last, with);
            ASSERT(Col_RopeLength(result) == Col_RopeLength(rope)
                    - ((last<length?last:length-1)-first+1)
                    + Col_RopeLength(with));
            ropeAccess(result);

            /*
             * Compare result with expected result.
             */

            /* Part before replaced section.*/
            for (Col_RopeIterFirst(itResult, result), Col_RopeIterFirst(itRope, rope);
                    !Col_RopeIterEnd(itRope) && Col_RopeIterIndex(itRope) < first; 
                    Col_RopeIterNext(itResult), Col_RopeIterNext(itRope)) {
                Col_Char c1 = Col_RopeIterAt(itResult);
                Col_Char c2 = Col_RopeIterAt(itRope);

                ASSERT(c1 == c2, "index=%u", Col_RopeIterIndex(itResult));
            }

            /* Replaced section. */
            for (Col_RopeIterFirst(itWith, with);
                    !Col_RopeIterEnd(itWith); 
                    Col_RopeIterNext(itResult), Col_RopeIterNext(itWith)) {
                Col_Char c1 = Col_RopeIterAt(itResult);
                Col_Char c2 = Col_RopeIterAt(itWith);

                ASSERT(c1 == c2, "index=%u", Col_RopeIterIndex(itResult));
            }

            /* Part after replaced section.*/
            if (last < length) {
                Col_RopeIterMoveTo(itRope, last+1);
                for (;
                        !Col_RopeIterEnd(itRope); 
                        Col_RopeIterNext(itResult), Col_RopeIterNext(itRope)) {
                    Col_Char c1 = Col_RopeIterAt(itResult);
                    Col_Char c2 = Col_RopeIterAt(itRope);

                    ASSERT(c1 == c2, "index=%u", Col_RopeIterIndex(itResult));
                }
            }
            ASSERT(Col_RopeIterEnd(itResult));
        }
    }
}

static int compareRopeChunks(size_t index, size_t length, size_t number, const Col_RopeChunk *chunks, Col_ClientData clientData) {
    size_t i;
    const void *p[2] = {chunks[0].data, chunks[1].data};
    ASSERT(number == 2);
    ASSERT(chunks[0].data);
    ASSERT(chunks[1].data);
    for (i = 0; i < length; i++, COL_CHAR_NEXT(chunks[0].format, p[0]),
            COL_CHAR_NEXT(chunks[1].format, p[1])) {
        Col_Char c1 = COL_CHAR_GET(chunks[0].format, p[0]);
        Col_Char c2 = COL_CHAR_GET(chunks[1].format, p[1]);
        ASSERT(c1 == c2, "index=%u", index+i);
    }
    return 0;
}
static void ropesEqual(Col_Word rope1, Col_Word rope2) {
    Col_RopeIterator it1, it2;
 
    ASSERT(Col_RopeLength(rope1) == Col_RopeLength(rope2));

    /* Iterators. */
    for (Col_RopeIterFirst(it1, rope1), Col_RopeIterFirst(it2, rope2);
            !Col_RopeIterEnd(it1); 
            Col_RopeIterNext(it1), Col_RopeIterNext(it2)) {
        size_t index = Col_RopeIterIndex(it1);
        Col_Char c1 = Col_RopeIterAt(it1);
        Col_Char c2 = Col_RopeIterAt(it2);

        ASSERT(index == Col_RopeIterIndex(it2), "index=%u", index);
        ASSERT(c1 == c2, "index=%u", index);
    }
    ASSERT(Col_RopeIterEnd(it2));

    /* Chunks. */
    {
        Col_Word ropes[] = {rope1, rope2};
        size_t length = 0;
        Col_TraverseRopeChunksN(2, ropes, 0, SIZE_MAX, compareRopeChunks, NULL, &length);
        ASSERT(length == Col_RopeLength(rope1));
    }

    /* Comparison. */
    ASSERT(Col_CompareRopes(rope1, rope2) == 0);
}
static int ropeFormatProc(size_t index, size_t length, size_t number, const Col_RopeChunk *chunks, Col_ClientData clientData) {
    Col_StringFormat format = (Col_StringFormat) clientData;
    if (format == COL_UCS) {
        if (chunks->format != COL_UCS1 && chunks->format != COL_UCS2 && chunks->format != COL_UCS4) return 1;
    } else {
        if (chunks->format != format) return 1;
    }
    return 0;
}
static int ropeCheckFormat(Col_Word rope, Col_StringFormat format) {
    return !Col_TraverseRopeChunks(rope, 0, SIZE_MAX, 0, ropeFormatProc, (Col_ClientData) format, NULL);
}
