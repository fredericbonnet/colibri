#include <stdlib.h>
#include <limits.h>

#include <colibri.h>

#include "picotest.h"

#include "testColibri.h"


// TODO test UTF-8/16

/*
 *---------------------------------------------------------------------------
 *
 * testStrBufs --
 *
 *---------------------------------------------------------------------------
 */

static void append(Col_Word strbuf, Col_Word rope, int success, size_t *lengthPtr);
static Col_Word appended(Col_Word rope);

TEST_SUITE(testStrBufs,
    testStrBufs8, testStrBufs16, testStrBufs32, testStrBufsFreeze
)


TEST_FIXTURE_CONTEXT(testStrBufs) {
    Col_Word rope8;
    Col_Word rope16;
    Col_Word rope32;
};
TEST_FIXTURE_SETUP(testStrBufs, context) {
    static Col_Char1 data8[64];
    static Col_Char2 data16[64];
    static Col_Char4 data32[64];
    size_t i;

    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();

    for (i=0; i < sizeof(data8)/sizeof(*data8); i++) {
        data8[i] = (Col_Char1) i+0x20;
    }
    context->rope8 = Col_NewRope(COL_UCS1, data8, sizeof(data8));

    for (i=0; i < sizeof(data16)/sizeof(*data16); i++) {
        data16[i] = (Col_Char2) i+0x100;
    }
    context->rope16 = Col_NewRope(COL_UCS2, data16, sizeof(data16));

    for (i=0; i < sizeof(data32)/sizeof(*data32); i++) {
        data32[i] = (Col_Char4) i+0x10000;
    }
    context->rope32 = Col_NewRope(COL_UCS4, data32, sizeof(data32));
}
TEST_FIXTURE_TEARDOWN(testStrBufs, context) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
}
TEST_CASE(testStrBufs8, testStrBufs, context) {
    size_t length = 0;
    Col_Word result = Col_EmptyRope();

    /* Create 8-bit string buffer. */
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS1);
    ASSERT(strbuf);

    /* Append ropes of various widths. */
    append(strbuf, context->rope8, 1, &length);
    result = Col_ConcatRopes(result, appended(context->rope8));
    append(strbuf, context->rope16, 0, &length);
    append(strbuf, context->rope32, 0, &length);
    ASSERT(Col_CompareRopes(Col_StringBufferValue(strbuf), result) == 0);

    /* Do it in reverse. */
    append(strbuf, context->rope32, 0, &length);
    append(strbuf, context->rope16, 0, &length);
    append(strbuf, context->rope8, 1, &length);
    result = Col_ConcatRopes(result, appended(context->rope8));
    ASSERT(Col_CompareRopes(Col_StringBufferValue(strbuf), result) == 0);
}
TEST_CASE(testStrBufs16, testStrBufs, context) {
    size_t length = 0;
    Col_Word result = Col_EmptyRope();

    /* Create 16-bit string buffer. */
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS2);
    ASSERT(strbuf);

    /* Append ropes of various widths. */
    append(strbuf, context->rope8, 1, &length);
    result = Col_ConcatRopes(result, appended(context->rope8));
    append(strbuf, context->rope16, 1, &length);
    result = Col_ConcatRopes(result, appended(context->rope16));
    append(strbuf, context->rope32, 0, &length);
    ASSERT(Col_CompareRopes(Col_StringBufferValue(strbuf), result) == 0);

    /* Do it in reverse. */
    append(strbuf, context->rope32, 0, &length);
    append(strbuf, context->rope16, 1, &length);
    result = Col_ConcatRopes(result, appended(context->rope16));
    append(strbuf, context->rope8, 1, &length);
    result = Col_ConcatRopes(result, appended(context->rope8));
    ASSERT(Col_CompareRopes(Col_StringBufferValue(strbuf), result) == 0);
}
TEST_CASE(testStrBufs32, testStrBufs, context) {
    size_t length = 0;
    Col_Word result = Col_EmptyRope();

    /* Create 32-bit string buffer. */
    Col_Word strbuf = Col_NewStringBuffer(0, COL_UCS4);
    ASSERT(strbuf);

    /* Append ropes of various widths. */
    append(strbuf, context->rope8, 1, &length);
    result = Col_ConcatRopes(result, appended(context->rope8));
    append(strbuf, context->rope16, 1, &length);
    result = Col_ConcatRopes(result, appended(context->rope16));
    append(strbuf, context->rope32, 1, &length);
    result = Col_ConcatRopes(result, appended(context->rope32));
    ASSERT(Col_CompareRopes(Col_StringBufferValue(strbuf), result) == 0);

    /* Do it in reverse. */
    append(strbuf, context->rope32, 1, &length);
    result = Col_ConcatRopes(result, appended(context->rope32));
    append(strbuf, context->rope16, 1, &length);
    result = Col_ConcatRopes(result, appended(context->rope16));
    append(strbuf, context->rope8, 1, &length);
    result = Col_ConcatRopes(result, appended(context->rope8));
    ASSERT(Col_CompareRopes(Col_StringBufferValue(strbuf), result) == 0);
}

/* Append various parts of rope to string buffer. */
static void append(Col_Word strbuf, Col_Word rope, int successExpected, size_t *lengthPtr) {
    /* Add char. */
    ASSERT(!!Col_StringBufferAppendChar(strbuf, Col_RopeAt(rope, 0)) == !!successExpected);
    if (successExpected) *lengthPtr += 1;
    ASSERT(Col_StringBufferLength(strbuf) == *lengthPtr);

    /* Add single char rope. */
    ASSERT(!!Col_StringBufferAppendRope(strbuf, Col_Subrope(rope, 1, 1)) == !!successExpected);
    if (successExpected) *lengthPtr += 1;
    ASSERT(Col_StringBufferLength(strbuf) == *lengthPtr);

    /* Add whole rope. */
    ASSERT(!!Col_StringBufferAppendRope(strbuf, rope) == !!successExpected);
    if (successExpected) *lengthPtr += Col_RopeLength(rope);
    ASSERT(Col_StringBufferLength(strbuf) == *lengthPtr);
}

/* Get result of above operation. */
static Col_Word appended(Col_Word rope) {
    return Col_ConcatRopesV(
        Col_NewCharWord(Col_RopeAt(rope, 0)),
        Col_Subrope(rope, 1, 1),
        rope
    );
}

TEST_SUITE(testStrBufsFreeze,
    testStrBufsFreezeChar, testStrBufsFreezeSmallString,
    testStrBufsFreezeString, testStrBufsFreezeBig
);
TEST_CASE(testStrBufsFreezeChar, testStrBufs, context) {
    Col_Word strbuf, frozen;
    Col_RopeIterator it;

    strbuf = Col_NewStringBuffer(1, COL_UCS1);
    Col_RopeIterFirst(it, context->rope8);
    ASSERT(Col_StringBufferAppendChar(strbuf, Col_RopeIterAt(it)));
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen != strbuf);
    ASSERT(frozen == Col_Subrope(context->rope8, 0, 0));

    strbuf = Col_NewStringBuffer(1, COL_UCS2);
    Col_RopeIterFirst(it, context->rope16);
    ASSERT(Col_StringBufferAppendChar(strbuf, Col_RopeIterAt(it)));
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen != strbuf);
    ASSERT(frozen == Col_Subrope(context->rope16, 0, 0));

    strbuf = Col_NewStringBuffer(1, COL_UCS4);
    Col_RopeIterFirst(it, context->rope32);
    ASSERT(Col_StringBufferAppendChar(strbuf, Col_RopeIterAt(it)));
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen != strbuf);
    ASSERT(frozen == Col_Subrope(context->rope32, 0, 0));

    strbuf = Col_NewStringBuffer(1, COL_UCS);
    Col_RopeIterFirst(it, context->rope8);
    ASSERT(Col_StringBufferAppendChar(strbuf, Col_RopeIterAt(it)));
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen != strbuf);
    ASSERT(frozen == Col_Subrope(context->rope8, 0, 0));

    strbuf = Col_NewStringBuffer(1, COL_UCS);
    Col_RopeIterFirst(it, context->rope16);
    ASSERT(Col_StringBufferAppendChar(strbuf, Col_RopeIterAt(it)));
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen != strbuf);
    ASSERT(frozen == Col_Subrope(context->rope16, 0, 0));

    strbuf = Col_NewStringBuffer(1, COL_UCS);
    Col_RopeIterFirst(it, context->rope32);
    ASSERT(Col_StringBufferAppendChar(strbuf, Col_RopeIterAt(it)));
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen != strbuf);
    ASSERT(frozen == Col_Subrope(context->rope32, 0, 0));
}
TEST_CASE(testStrBufsFreezeSmallString, testStrBufs, context) {
    Col_Word strbuf, frozen;
    Col_RopeIterator it, begin, end;
    size_t i;

    /* Character-wise. */
    strbuf = Col_NewStringBuffer(3, COL_UCS1);
    for (i = 0, Col_RopeIterFirst(it, context->rope8); 
            i < 3 && !Col_RopeIterEnd(it); i++, Col_RopeIterNext(it)) {
        ASSERT(Col_StringBufferAppendChar(strbuf, Col_RopeIterAt(it)));
    }
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen != strbuf);
    ASSERT(frozen == Col_Subrope(context->rope8, 0, 2));

    strbuf = Col_NewStringBuffer(3, COL_UCS);
    for (i = 0, Col_RopeIterFirst(it, context->rope8); 
            i < 3 && !Col_RopeIterEnd(it); i++, Col_RopeIterNext(it)) {
        ASSERT(Col_StringBufferAppendChar(strbuf, Col_RopeIterAt(it)));
    }
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen != strbuf);
    ASSERT(frozen == Col_Subrope(context->rope8, 0, 2));

    /* Sequence. */
    strbuf = Col_NewStringBuffer(3, COL_UCS1);
    Col_RopeIterFirst(begin, context->rope8); 
    Col_RopeIterBegin(end, context->rope8, 3);
    ASSERT(Col_RopeIterIndex(begin) == 0);
    ASSERT(Col_RopeIterIndex(end) == 3);
    ASSERT(Col_StringBufferAppendSequence(strbuf, begin, end));
    ASSERT(Col_RopeIterIndex(begin) == 0);
    ASSERT(Col_RopeIterIndex(end) == 3);
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen != strbuf);
    ASSERT(frozen == Col_Subrope(context->rope8, 0, 2));

    strbuf = Col_NewStringBuffer(3, COL_UCS);
    Col_RopeIterFirst(begin, context->rope8); 
    Col_RopeIterBegin(end, context->rope8, 3);
    ASSERT(Col_RopeIterIndex(begin) == 0);
    ASSERT(Col_RopeIterIndex(end) == 3);
    ASSERT(Col_StringBufferAppendSequence(strbuf, begin, end));
    ASSERT(Col_RopeIterIndex(begin) == 0);
    ASSERT(Col_RopeIterIndex(end) == 3);
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen != strbuf);
    ASSERT(frozen == Col_Subrope(context->rope8, 0, 2));
}
TEST_CASE(testStrBufsFreezeString, testStrBufs, context) {
    Col_Word strbuf, frozen;
    Col_RopeIterator it, begin, end;

    /* Char-wise. */
    strbuf = Col_NewStringBuffer(Col_RopeLength(context->rope8), COL_UCS1);
    for (Col_RopeIterFirst(it, context->rope8); !Col_RopeIterEnd(it);
            Col_RopeIterNext(it)) {
        ASSERT(Col_StringBufferAppendChar(strbuf, Col_RopeIterAt(it)));
    }
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen == strbuf);
    ASSERT(Col_CompareRopes(frozen, context->rope8) == 0);

    strbuf = Col_NewStringBuffer(Col_RopeLength(context->rope16), COL_UCS2);
    for (Col_RopeIterFirst(it, context->rope16); !Col_RopeIterEnd(it);
            Col_RopeIterNext(it)) {
        ASSERT(Col_StringBufferAppendChar(strbuf, Col_RopeIterAt(it)));
    }
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen == strbuf);
    ASSERT(Col_CompareRopes(frozen, context->rope16) == 0);

    strbuf = Col_NewStringBuffer(Col_RopeLength(context->rope32), COL_UCS4);
    for (Col_RopeIterFirst(it, context->rope32); !Col_RopeIterEnd(it);
            Col_RopeIterNext(it)) {
        ASSERT(Col_StringBufferAppendChar(strbuf, Col_RopeIterAt(it)));
    }
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen == strbuf);
    ASSERT(Col_CompareRopes(frozen, context->rope32) == 0);

    /* Sequence. */
    strbuf = Col_NewStringBuffer(Col_RopeLength(context->rope8), COL_UCS1);
    Col_RopeIterFirst(begin, context->rope8); 
    Col_RopeIterBegin(end, context->rope8, SIZE_MAX);
    ASSERT(Col_RopeIterIndex(begin) == 0);
    ASSERT(Col_RopeIterEnd(end));
    ASSERT(Col_StringBufferAppendSequence(strbuf, begin, end));
    ASSERT(Col_RopeIterIndex(begin) == 0);
    ASSERT(Col_RopeIterEnd(end));
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen == strbuf);
    ASSERT(Col_CompareRopes(frozen, context->rope8) == 0);

    strbuf = Col_NewStringBuffer(Col_RopeLength(context->rope16), COL_UCS2);
    Col_RopeIterFirst(begin, context->rope16); 
    Col_RopeIterBegin(end, context->rope16, SIZE_MAX);
    ASSERT(Col_RopeIterIndex(begin) == 0);
    ASSERT(Col_RopeIterEnd(end));
    ASSERT(Col_StringBufferAppendSequence(strbuf, begin, end));
    ASSERT(Col_RopeIterIndex(begin) == 0);
    ASSERT(Col_RopeIterEnd(end));
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen == strbuf);
    ASSERT(Col_CompareRopes(frozen, context->rope16) == 0);

    strbuf = Col_NewStringBuffer(Col_RopeLength(context->rope32), COL_UCS4);
    Col_RopeIterFirst(begin, context->rope32); 
    Col_RopeIterBegin(end, context->rope32, SIZE_MAX);
    ASSERT(Col_RopeIterIndex(begin) == 0);
    ASSERT(Col_RopeIterEnd(end));
    ASSERT(Col_StringBufferAppendSequence(strbuf, begin, end));
    ASSERT(Col_RopeIterIndex(begin) == 0);
    ASSERT(Col_RopeIterEnd(end));
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen == strbuf);
    ASSERT(Col_CompareRopes(frozen, context->rope32) == 0);
}
TEST_CASE(testStrBufsFreezeBig, testStrBufs, context) {
    Col_Word strbuf, frozen;
    Col_RopeIterator it, begin, end;
    size_t i;

    /* Char-wise. */
    strbuf = Col_NewStringBuffer(100000, COL_UCS1);
    for (i=0, Col_RopeIterFirst(it, context->rope8); i < 100000;
            i++, Col_RopeIterNext(it), Col_RopeIterEnd(it) 
            ? (Col_RopeIterFirst(it, context->rope8), 0) : 0) {
        ASSERT(Col_StringBufferAppendChar(strbuf, Col_RopeIterAt(it)));
    }
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen != strbuf);
    ASSERT(Col_CompareRopes(frozen, Col_Subrope(Col_RepeatRope(context->rope8, 100000/Col_RopeLength(context->rope8)+1), 0, 99999)) == 0);

    /* Sequence. Reuse the above frozen value as source. */
    strbuf = Col_NewStringBuffer(100000, COL_UCS1);
    Col_RopeIterFirst(begin, frozen); 
    Col_RopeIterBegin(end, frozen, SIZE_MAX);
    ASSERT(Col_RopeIterIndex(begin) == 0);
    ASSERT(Col_RopeIterEnd(end));
    ASSERT(Col_StringBufferAppendSequence(strbuf, begin, end));
    ASSERT(Col_RopeIterIndex(begin) == 0);
    ASSERT(Col_RopeIterEnd(end));
    frozen = Col_StringBufferFreeze(strbuf);
    ASSERT(frozen != strbuf);
    ASSERT(Col_CompareRopes(frozen, Col_Subrope(Col_RepeatRope(context->rope8, 100000/Col_RopeLength(context->rope8)+1), 0, 99999)) == 0);
}
