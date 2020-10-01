#include <colibri.h>
#include <picotest.h>

/*
 * Test data
 */

const Col_Char1 utf8Data[] = {
    'a',                    //  0 :  0
    'b',                    //  1 :  1
    0xC2, 0x81,             //  2 :  2
    0xC2, 0x82,             //  3 :  4
    0xF4, 0x80, 0x80, 0x82, //  4 :  6
    0xF4, 0x8F, 0xBF, 0xBC, //  5 : 10
    0xE0, 0xA0, 0xA1,       //  6 : 14
    'c',                    //  7 : 17
    0xF4, 0x8F, 0xBF, 0xBD, //  8 : 18
    0xDF, 0xBF,             //  9 : 22
    0xE0, 0xA0, 0xA3,       // 10 : 24
    'e',                    // 11 : 27
    0xE0, 0xA0, 0xA4,       // 12 : 28
};                          // 13 : 31
const size_t utf8DataLength = 13;
const size_t utf8DataOffset[] = {
    0, 1, 2, 4, 6, 10, 14, 17, 18, 22, 24, 27, 28, 31,
};
const Col_Char1 utf8Data1[] = {'a', 'b', 'c', 'd', 'e', 'f'};
const Col_Char1 utf8Data2[] = {
    0xC2, 0x80, // min
    0xDF, 0xBF, // max
    0xC2, 0x81, //
    0xC2, 0x82, //
    0xDF, 0xBD, //
    0xDF, 0xBE, //
};
const Col_Char1 utf8Data3[] = {
    0xE0, 0xA0, 0xA0, // min
    0xEF, 0xBF, 0xBF, // max
    0xE0, 0xA0, 0xA1, //
    0xE0, 0xA0, 0xA2, //
    0xE0, 0xA0, 0xA3, //
    0xE0, 0xA0, 0xA4, //
    0xE0, 0xBF, 0xBD, //
    0xEF, 0xBF, 0xBE, //
};
const Col_Char1 utf8Data4[] = {
    0xF4, 0x80, 0x80, 0x80, // min
    0xF4, 0x8F, 0xBF, 0xBF, // max
    0xF4, 0x80, 0x80, 0x81, //
    0xF4, 0x80, 0x80, 0x82, //
    0xF4, 0x8F, 0xBF, 0xBC, //
    0xF4, 0x8F, 0xBF, 0xBD, //
    0xF4, 0x8F, 0xBF, 0xBE, //
};

const Col_Char2 utf16Data[] = {
    'a',            //  0 :  0
    'b',            //  1 :  1
    0x0101,         //  2 :  2
    0x0102,         //  3 :  3
    0xD800, 0xDC01, //  4 :  4
    0xD800, 0xDC02, //  5 :  6
    0x0103,         //  6 :  8
    0xD800, 0xDC03, //  7 :  9
    'c',            //  8 : 11
    0x0104,         //  9 : 12
    0x0105,         // 10 : 13
    0xD800, 0xDC03, // 11 : 14
    'd',            // 12 : 16
};                  // 13 : 17
const size_t utf16DataLength = 13;
const size_t utf16DataOffset[] = {
    0, 1, 2, 3, 4, 6, 8, 9, 11, 12, 13, 14, 16, 17,
};
const Col_Char2 utf16Data1[] = {'a', 'b', 'c', 0x0100, 0xD7FF, 0xE000, 0xFFFF};
const Col_Char2 utf16Data2[] = {0xD800, 0xDC00,  // min
                                0xDBFF, 0xDFFF,  // max
                                0xD800, 0xDC01,  //
                                0xD800, 0xDC02,  //
                                0xDBFF, 0xDFFD,  //
                                0xDBFF, 0xDFFE}; //

/*
 * Strings
 */

#include "hooks.h"
#include "colibriFixture.h"

PICOTEST_SUITE(testStrings, testStringConstants, testUtf8, testUtf16,
               testCharGet, testCharNext, testCharPrevious);

PICOTEST_CASE(testStringConstants) {
    PICOTEST_ASSERT(COL_CHAR_MAX == 0x10FFFF);
}

PICOTEST_SUITE(testUtf8, testUtf8Width, testUtf8SurrogatePairs, testUtf8Get,
               testUtf8Set, testUtf8Next, testUtf8Prev, testUtf8Addr);
PICOTEST_CASE(testUtf8Width) {
    PICOTEST_VERIFY(COL_UTF8_WIDTH(0) == 1);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(0x7F) == 1);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(0x80) == 2);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(0x7FF) == 2);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(0x800) == 3);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(0xFFFF) == 3);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(0x10000) == 4);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(COL_CHAR_MAX) == 4);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(COL_CHAR_MAX + 1) == 0);
}

PICOTEST_CASE(testUtf8SurrogatePairs) {
    PICOTEST_VERIFY(COL_UTF8_WIDTH(0xD7FF) == 3);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(0xD800) == 0);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(0xDFFF) == 0);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(0xE000) == 3);
}
PICOTEST_SUITE(testUtf8Get, testUtf8Get1, testUtf8Get2, testUtf8Get3,
               testUtf8Get4, testUtf8GetInvalid);
PICOTEST_CASE(testUtf8Get1) {
    const Col_Char1 mindata[] = {0};
    const Col_Char min = Col_Utf8Get(mindata);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(min) == 1);
    PICOTEST_VERIFY(min == 0);

    const Col_Char1 maxdata[] = {0x7F};
    const Col_Char max = Col_Utf8Get(maxdata);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(max) == 1);
    PICOTEST_VERIFY(max == 0x7F);
}
PICOTEST_CASE(testUtf8Get2) {
    const Col_Char1 mindata[] = {0xC2, 0x80};
    const Col_Char min = Col_Utf8Get(mindata);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(min) == 2);
    PICOTEST_VERIFY(min == 0x80);

    const Col_Char1 maxdata[] = {0xDF, 0xBF};
    const Col_Char max = Col_Utf8Get(maxdata);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(max) == 2);
    PICOTEST_VERIFY(max == 0x7FF);
}
PICOTEST_CASE(testUtf8Get3) {
    const Col_Char1 mindata[] = {0xE0, 0xA0, 0x80};
    const Col_Char min = Col_Utf8Get(mindata);
    PICOTEST_VERIFY(min == 0x800);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(min) == 3);

    const Col_Char1 maxdata[] = {0xEF, 0xBF, 0xBF};
    const Col_Char max = Col_Utf8Get(maxdata);
    PICOTEST_VERIFY(max == 0xFFFF);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(max) == 3);
}
PICOTEST_CASE(testUtf8Get4) {
    const Col_Char1 mindata[] = {0xF4, 0x80, 0x80, 0x80};
    const Col_Char min = Col_Utf8Get(mindata);
    PICOTEST_VERIFY(min == 0x100000);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(min) == 4);

    const Col_Char1 maxdata[] = {0xF4, 0x8F, 0xBF, 0xBF};
    const Col_Char max = Col_Utf8Get(maxdata);
    PICOTEST_VERIFY(max == 0x10FFFF);
    PICOTEST_VERIFY(COL_UTF8_WIDTH(max) == 4);
}
PICOTEST_CASE(testUtf8GetInvalid) {
    const Col_Char1 data[] = {0xF8};
    PICOTEST_ASSERT(Col_Utf8Get(data) == COL_CHAR_INVALID);
}

PICOTEST_SUITE(testUtf8Set, testUtf8Set1, testUtf8Set2, testUtf8Set3,
               testUtf8Set4, testUtf8SetSurrogatePairs, testUtf8SetInvalid);
PICOTEST_CASE(testUtf8Set1) {
    Col_Char1 mindata[] = {0xFF, 0xFF};
    Col_Char1 *min = Col_Utf8Set(mindata, 0);
    PICOTEST_ASSERT(min == mindata + 1);
    PICOTEST_VERIFY(*min == 0xFF);
    PICOTEST_VERIFY(mindata[0] == 0);

    Col_Char1 maxdata[] = {0xFF, 0xFF};
    Col_Char1 *max = Col_Utf8Set(maxdata, 0x7F);
    PICOTEST_ASSERT(max == maxdata + 1);
    PICOTEST_VERIFY(*max == 0xFF);
    PICOTEST_VERIFY(maxdata[0] == 0x7F);
}
PICOTEST_CASE(testUtf8Set2) {
    Col_Char1 mindata[] = {0xFF, 0xFF, 0xFF};
    Col_Char1 *min = Col_Utf8Set(mindata, 0x80);
    PICOTEST_ASSERT(min == mindata + 2);
    PICOTEST_VERIFY(*min == 0xFF);
    PICOTEST_VERIFY(mindata[0] == 0xC2);
    PICOTEST_VERIFY(mindata[1] == 0x80);

    Col_Char1 maxdata[] = {0xFF, 0xFF, 0xFF};
    Col_Char1 *max = Col_Utf8Set(maxdata, 0x7FF);
    PICOTEST_ASSERT(max == maxdata + 2);
    PICOTEST_VERIFY(*max == 0xFF);
    PICOTEST_VERIFY(maxdata[0] == 0xDF);
    PICOTEST_VERIFY(maxdata[1] == 0xBF);
}
PICOTEST_CASE(testUtf8Set3) {
    Col_Char1 mindata[] = {0xFF, 0xFF, 0xFF, 0xFF};
    Col_Char1 *min = Col_Utf8Set(mindata, 0x800);
    PICOTEST_ASSERT(min == mindata + 3);
    PICOTEST_VERIFY(*min == 0xFF);
    PICOTEST_VERIFY(mindata[0] == 0xE0);
    PICOTEST_VERIFY(mindata[1] == 0xA0);
    PICOTEST_VERIFY(mindata[2] == 0x80);

    Col_Char1 maxdata[] = {0xFF, 0xFF, 0xFF, 0xFF};
    Col_Char1 *max = Col_Utf8Set(maxdata, 0xFFFF);
    PICOTEST_ASSERT(max == maxdata + 3);
    PICOTEST_VERIFY(*max == 0xFF);
    PICOTEST_VERIFY(maxdata[0] == 0xEF);
    PICOTEST_VERIFY(maxdata[1] == 0xBF);
    PICOTEST_VERIFY(maxdata[2] == 0xBF);
}
PICOTEST_CASE(testUtf8Set4) {
    Col_Char1 mindata[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    Col_Char1 *min = Col_Utf8Set(mindata, 0x100000);
    PICOTEST_ASSERT(min == mindata + 4);
    PICOTEST_VERIFY(*min == 0xFF);
    PICOTEST_VERIFY(mindata[0] == 0xF4);
    PICOTEST_VERIFY(mindata[1] == 0x80);
    PICOTEST_VERIFY(mindata[2] == 0x80);
    PICOTEST_VERIFY(mindata[3] == 0x80);

    Col_Char1 maxdata[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    Col_Char1 *max = Col_Utf8Set(maxdata, 0x10FFFF);
    PICOTEST_ASSERT(max == maxdata + 4);
    PICOTEST_VERIFY(*max == 0xFF);
    PICOTEST_VERIFY(maxdata[0] == 0xF4);
    PICOTEST_VERIFY(maxdata[1] == 0x8F);
    PICOTEST_VERIFY(maxdata[2] == 0xBF);
    PICOTEST_VERIFY(maxdata[3] == 0xBF);
}
PICOTEST_CASE(testUtf8SetSurrogatePairs) {
    Col_Char1 beforedata[] = {0xFF, 0xFF, 0xFF, 0xFF};
    Col_Char1 *before = Col_Utf8Set(beforedata, 0xD7FF);
    PICOTEST_ASSERT(before == beforedata + 3);
    PICOTEST_VERIFY(*before == 0xFF);
    PICOTEST_VERIFY(beforedata[0] == 0xED);
    PICOTEST_VERIFY(beforedata[1] == 0x9F);
    PICOTEST_VERIFY(beforedata[2] == 0xBF);

    Col_Char1 mindata[] = {0xFF};
    Col_Char1 *min = Col_Utf8Set(mindata, 0xD800);
    PICOTEST_ASSERT(min == mindata);
    PICOTEST_VERIFY(*min == 0xFF);

    Col_Char1 maxdata[] = {0xFF};
    Col_Char1 *max = Col_Utf8Set(maxdata, 0xDFFF);
    PICOTEST_ASSERT(max == maxdata);
    PICOTEST_VERIFY(*max == 0xFF);

    Col_Char1 afterdata[] = {0xFF, 0xFF, 0xFF, 0xFF};
    Col_Char1 *after = Col_Utf8Set(afterdata, 0xE000);
    PICOTEST_ASSERT(after == afterdata + 3);
    PICOTEST_VERIFY(*after == 0xFF);
    PICOTEST_VERIFY(afterdata[0] == 0xEE);
    PICOTEST_VERIFY(afterdata[1] == 0x80);
    PICOTEST_VERIFY(afterdata[2] == 0x80);
}
PICOTEST_CASE(testUtf8SetInvalid) {
    Col_Char1 mindata[] = {0xFF};
    Col_Char1 *min = Col_Utf8Set(mindata, COL_CHAR_MAX + 1);
    PICOTEST_ASSERT(min == mindata);
    PICOTEST_VERIFY(*min == 0xFF);
}

PICOTEST_SUITE(testUtf8Next, testUtf8Next1, testUtf8Next2, testUtf8Next3,
               testUtf8Next4);
PICOTEST_SUITE(testUtf8Prev, testUtf8Prev1, testUtf8Prev2, testUtf8Prev3,
               testUtf8Prev4);

PICOTEST_CASE(testUtf8Next1) {
    const Col_Char1 *p = utf8Data1;

    do {
        const Col_Char1 *next = Col_Utf8Next(p);
        PICOTEST_ASSERT(next = p + 1);
        p = next;
    } while (p < utf8Data1 + sizeof(utf8Data1));
    PICOTEST_ASSERT(p == utf8Data1 + sizeof(utf8Data1));
};
PICOTEST_CASE(testUtf8Prev1) {
    const Col_Char1 *p = utf8Data1 + sizeof(utf8Data1);

    do {
        const Col_Char1 *prev = Col_Utf8Prev(p);
        PICOTEST_ASSERT(prev = p - 1);
        p = prev;
    } while (p > utf8Data1);
    PICOTEST_ASSERT(p == utf8Data1);
};
PICOTEST_CASE(testUtf8Next2) {
    const Col_Char1 *p = utf8Data2;

    do {
        const Col_Char1 *next = Col_Utf8Next(p);
        PICOTEST_ASSERT(next = p + 2);
        p = next;
    } while (p < utf8Data2 + sizeof(utf8Data2));
    PICOTEST_ASSERT(p == utf8Data2 + sizeof(utf8Data2));
};
PICOTEST_CASE(testUtf8Prev2) {
    const Col_Char1 *p = utf8Data2 + sizeof(utf8Data2);

    do {
        const Col_Char1 *prev = Col_Utf8Prev(p);
        PICOTEST_ASSERT(prev = p - 2);
        p = prev;
    } while (p > utf8Data2);
    PICOTEST_ASSERT(p == utf8Data2);
};
PICOTEST_CASE(testUtf8Next3) {
    const Col_Char1 *p = utf8Data3;

    do {
        const Col_Char1 *next = Col_Utf8Next(p);
        PICOTEST_ASSERT(next = p + 3);
        p = next;
    } while (p < utf8Data3 + sizeof(utf8Data3));
    PICOTEST_ASSERT(p == utf8Data3 + sizeof(utf8Data3));
};
PICOTEST_CASE(testUtf8Prev3) {
    const Col_Char1 *p = utf8Data3 + sizeof(utf8Data3);

    do {
        const Col_Char1 *prev = Col_Utf8Prev(p);
        PICOTEST_ASSERT(prev = p - 3);
        p = prev;
    } while (p > utf8Data3);
    PICOTEST_ASSERT(p == utf8Data3);
};
PICOTEST_CASE(testUtf8Next4) {
    const Col_Char1 *p = utf8Data4;

    do {
        const Col_Char1 *next = Col_Utf8Next(p);
        PICOTEST_ASSERT(next = p + 4);
        p = next;
    } while (p < utf8Data4 + sizeof(utf8Data4));
    PICOTEST_ASSERT(p == utf8Data4 + sizeof(utf8Data4));
};
PICOTEST_CASE(testUtf8Prev4) {
    const Col_Char1 *p = utf8Data4 + sizeof(utf8Data4);

    do {
        const Col_Char1 *prev = Col_Utf8Prev(p);
        PICOTEST_ASSERT(prev = p - 4);
        p = prev;
    } while (p > utf8Data4);
    PICOTEST_ASSERT(p == utf8Data4);
};

PICOTEST_SUITE(testUtf8Addr, testUtf8AddrFixed, testUtF8AddrVariable);
PICOTEST_CASE(testUtf8AddrFixed) {
    for (int i = 0; i < sizeof(utf8Data1); i++) {
        PICOTEST_ASSERT(Col_Utf8Addr(utf8Data1, i,
                                     sizeof(utf8Data1) / sizeof(*utf8Data1),
                                     sizeof(utf8Data1)) == utf8Data1 + i);
    }
}
PICOTEST_CASE(testUtF8AddrVariable) {
    for (int i = 0; i < utf8DataLength; i++) {
        PICOTEST_ASSERT(
            Col_Utf8Addr(utf8Data, i, utf8DataLength, sizeof(utf8Data)) ==
            utf8Data + utf8DataOffset[i]);
    }
}

PICOTEST_SUITE(testUtf16, testUtf16Width, testUtf16SurrogatePairs, testUtf16Get,
               testUtf16Set, testUtf16Next, testUtf16Prev, testUtf16Addr);
PICOTEST_CASE(testUtf16Width) {
    PICOTEST_VERIFY(COL_UTF16_WIDTH(0) == 1);
    PICOTEST_VERIFY(COL_UTF16_WIDTH(0xFFFF) == 1);
    PICOTEST_VERIFY(COL_UTF16_WIDTH(0x10000) == 2);
    PICOTEST_VERIFY(COL_UTF16_WIDTH(COL_CHAR_MAX) == 2);
    PICOTEST_VERIFY(COL_UTF16_WIDTH(COL_CHAR_MAX + 1) == 0);
}
PICOTEST_CASE(testUtf16SurrogatePairs) {
    PICOTEST_VERIFY(COL_UTF16_WIDTH(0xD7FF) == 1);
    PICOTEST_VERIFY(COL_UTF16_WIDTH(0xD800) == 0);
    PICOTEST_VERIFY(COL_UTF16_WIDTH(0xDFFF) == 0);
    PICOTEST_VERIFY(COL_UTF16_WIDTH(0xE000) == 1);
}

PICOTEST_SUITE(testUtf16Get, testUtf16Get1, testUtf16Get2);
PICOTEST_CASE(testUtf16Get1) {
    const Col_Char2 mindata[] = {0};
    const Col_Char min = Col_Utf16Get(mindata);
    PICOTEST_VERIFY(COL_UTF16_WIDTH(min) == 1);
    PICOTEST_VERIFY(min == 0);

    const Col_Char2 maxdata[] = {0xFFFF};
    const Col_Char max = Col_Utf16Get(maxdata);
    PICOTEST_VERIFY(COL_UTF16_WIDTH(max) == 1);
    PICOTEST_VERIFY(max == 0xFFFF);
}
PICOTEST_CASE(testUtf16Get2) {
    const Col_Char2 mindata[] = {0xD800, 0xDC00};
    const Col_Char min = Col_Utf16Get(mindata);
    PICOTEST_VERIFY(COL_UTF16_WIDTH(min) == 2);
    PICOTEST_VERIFY(min == 0x10000);

    const Col_Char2 maxdata[] = {0xDBFF, 0xDFFF};
    const Col_Char max = Col_Utf16Get(maxdata);
    PICOTEST_VERIFY(COL_UTF16_WIDTH(max) == 2);
    PICOTEST_VERIFY(max == 0x10FFFF);
}

PICOTEST_SUITE(testUtf16Set, testUtf16Set1, testUtf16Set2,
               testUtf16SetSurrogatePairs, testUtf16SetInvalid);
PICOTEST_CASE(testUtf16Set1) {
    Col_Char2 mindata[] = {0xFFFF, 0xFFFF};
    Col_Char2 *min = Col_Utf16Set(mindata, 0);
    PICOTEST_ASSERT(min == mindata + 1);
    PICOTEST_VERIFY(*min == 0xFFFF);
    PICOTEST_VERIFY(mindata[0] == 0);

    Col_Char2 maxdata[] = {0, 0};
    Col_Char2 *max = Col_Utf16Set(maxdata, 0xFFFF);
    PICOTEST_ASSERT(max == maxdata + 1);
    PICOTEST_VERIFY(*max == 0);
    PICOTEST_VERIFY(maxdata[0] == 0xFFFF);
}
PICOTEST_CASE(testUtf16Set2) {
    Col_Char2 mindata[] = {0xFFFF, 0xFFFF, 0xFFFF};
    Col_Char2 *min = Col_Utf16Set(mindata, 0x10000);
    PICOTEST_ASSERT(min == mindata + 2);
    PICOTEST_VERIFY(*min == 0xFFFF);
    PICOTEST_VERIFY(mindata[0] == 0xD800);
    PICOTEST_VERIFY(mindata[1] == 0xDC00);

    Col_Char2 maxdata[] = {0xFFFF, 0xFFFF, 0xFFFF};
    Col_Char2 *max = Col_Utf16Set(maxdata, 0x10FFFF);
    PICOTEST_ASSERT(max == maxdata + 2);
    PICOTEST_VERIFY(*max == 0xFFFF);
    PICOTEST_VERIFY(maxdata[0] == 0xDBFF);
    PICOTEST_VERIFY(maxdata[1] == 0xDFFF);
}
PICOTEST_CASE(testUtf16SetSurrogatePairs) {
    Col_Char2 beforedata[] = {0xFFFF, 0xFFFF};
    Col_Char2 *before = Col_Utf16Set(beforedata, 0xD7FF);
    PICOTEST_ASSERT(before == beforedata + 1);
    PICOTEST_VERIFY(*before == 0xFFFF);
    PICOTEST_VERIFY(beforedata[0] == 0xD7FF);

    Col_Char2 mindata[] = {0xFFFF};
    Col_Char2 *min = Col_Utf16Set(mindata, 0xD800);
    PICOTEST_ASSERT(min == mindata);
    PICOTEST_VERIFY(*min == 0xFFFF);

    Col_Char2 maxdata[] = {0xFFFF};
    Col_Char2 *max = Col_Utf16Set(maxdata, 0xDFFF);
    PICOTEST_ASSERT(max == maxdata);
    PICOTEST_VERIFY(*max == 0xFFFF);

    Col_Char2 afterdata[] = {0xFFFF, 0xFFFF};
    Col_Char2 *after = Col_Utf16Set(afterdata, 0xE000);
    PICOTEST_ASSERT(after == afterdata + 1);
    PICOTEST_VERIFY(*after == 0xFFFF);
    PICOTEST_VERIFY(afterdata[0] == 0xE000);
}
PICOTEST_CASE(testUtf16SetInvalid) {
    Col_Char2 mindata[] = {0xFFFF};
    Col_Char2 *min = Col_Utf16Set(mindata, COL_CHAR_MAX + 1);
    PICOTEST_ASSERT(min == mindata);
    PICOTEST_VERIFY(*min == 0xFFFF);
}

PICOTEST_SUITE(testUtf16Next, testUtf16Next1, testUtf16Next2)
PICOTEST_SUITE(testUtf16Prev, testUtf16Prev1, testUtf16Prev2)

PICOTEST_CASE(testUtf16Next1) {
    const Col_Char2 *p = utf16Data1;

    do {
        const Col_Char2 *next = Col_Utf16Next(p);
        PICOTEST_ASSERT(next = p + 1);
        p = next;
    } while (p < utf16Data1 + sizeof(utf16Data1));
    PICOTEST_ASSERT(p == utf16Data1 + sizeof(utf16Data1));
}
PICOTEST_CASE(testUtf16Prev1) {
    const Col_Char2 *p = utf16Data1 + sizeof(utf16Data1);

    do {
        const Col_Char2 *prev = Col_Utf16Prev(p);
        PICOTEST_ASSERT(prev = p - 1);
        p = prev;
    } while (p > utf16Data1);
    PICOTEST_ASSERT(p == utf16Data1);
}
PICOTEST_CASE(testUtf16Next2) {
    const Col_Char2 *p = utf16Data2;

    do {
        const Col_Char2 *next = Col_Utf16Next(p);
        PICOTEST_ASSERT(next = p + 2);
        p = next;
    } while (p < utf16Data2 + sizeof(utf16Data2));
    PICOTEST_ASSERT(p == utf16Data2 + sizeof(utf16Data2));
}
PICOTEST_CASE(testUtf16Prev2) {
    const Col_Char2 *p = utf16Data2 + sizeof(utf16Data2);

    do {
        const Col_Char2 *prev = Col_Utf16Prev(p);
        PICOTEST_ASSERT(prev = p - 2);
        p = prev;
    } while (p > utf16Data2);
    PICOTEST_ASSERT(p == utf16Data2);
}

PICOTEST_SUITE(testUtf16Addr, testUtf16AddrFixed, testUtf16AddrVariable);
PICOTEST_CASE(testUtf16AddrFixed) {
    for (int i = 0; i < sizeof(utf16Data1); i++) {
        PICOTEST_ASSERT(Col_Utf16Addr(utf16Data1, i,
                                      sizeof(utf16Data1) / sizeof(*utf16Data1),
                                      sizeof(utf16Data1)) == utf16Data1 + i);
    }
}
PICOTEST_CASE(testUtf16AddrVariable) {
    for (int i = 0; i < utf16DataLength; i++) {
        PICOTEST_ASSERT(
            Col_Utf16Addr(utf16Data, i, utf16DataLength, sizeof(utf16Data)) ==
            utf16Data + utf16DataOffset[i]);
    }
}

PICOTEST_SUITE(testCharGet, testCharGetUcs1, testCharGetUcs2, testCharGetUcs4,
               testCharGetUtf8, testCharGetUtf16, testCharGetInvalid);
PICOTEST_SUITE(testCharNext, testCharNextUcs1, testCharNextUcs2,
               testCharNextUcs4, testCharNextUtf8, testCharNextUtf16,
               testCharNextInvalid);
PICOTEST_SUITE(testCharPrevious, testCharPreviousUcs1, testCharPreviousUcs2,
               testCharPreviousUcs4, testCharPreviousUtf8,
               testCharPreviousUtf16, testCharPreviousInvalid);

PICOTEST_CASE(testCharGetUcs1) {
    const Col_Char1 data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    const char *chunk = (const char *)data;
    PICOTEST_VERIFY(COL_CHAR_GET(COL_UCS1, chunk) == 0xDE);
    PICOTEST_VERIFY(COL_CHAR_GET(COL_UCS1, chunk + 1) == 0xAD);
    PICOTEST_VERIFY(COL_CHAR_GET(COL_UCS1, chunk + 2) == 0xBE);
    PICOTEST_VERIFY(COL_CHAR_GET(COL_UCS1, chunk + 3) == 0xEF);
}
PICOTEST_CASE(testCharNextUcs1) {
    const Col_Char1 data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    const char *chunk = (const char *)data;
    const void *p = chunk;
    COL_CHAR_NEXT(COL_UCS1, p);
    PICOTEST_ASSERT(p == chunk + 1);
    COL_CHAR_NEXT(COL_UCS1, p);
    PICOTEST_ASSERT(p == chunk + 2);
    COL_CHAR_NEXT(COL_UCS1, p);
    PICOTEST_ASSERT(p == chunk + 3);
    COL_CHAR_NEXT(COL_UCS1, p);
    PICOTEST_ASSERT(p == chunk + 4);
}
PICOTEST_CASE(testCharPreviousUcs1) {
    const Col_Char1 data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    const char *chunk = (const char *)data;
    const void *p = chunk + 4;
    COL_CHAR_PREVIOUS(COL_UCS1, p);
    PICOTEST_ASSERT(p == chunk + 3);
    COL_CHAR_PREVIOUS(COL_UCS1, p);
    PICOTEST_ASSERT(p == chunk + 2);
    COL_CHAR_PREVIOUS(COL_UCS1, p);
    PICOTEST_ASSERT(p == chunk + 1);
    COL_CHAR_PREVIOUS(COL_UCS1, p);
    PICOTEST_ASSERT(p == chunk);
}
PICOTEST_CASE(testCharGetUcs2) {
    Col_Char2 data[] = {0xDEAD, 0xBEEF, 0xBAAD, 0xCAFE};
    const char *chunk = (const char *)data;
    PICOTEST_VERIFY(COL_CHAR_GET(COL_UCS2, chunk) == 0xDEAD);
    PICOTEST_VERIFY(COL_CHAR_GET(COL_UCS2, chunk + 2) == 0xBEEF);
    PICOTEST_VERIFY(COL_CHAR_GET(COL_UCS2, chunk + 4) == 0xBAAD);
    PICOTEST_VERIFY(COL_CHAR_GET(COL_UCS2, chunk + 6) == 0xCAFE);
}
PICOTEST_CASE(testCharNextUcs2) {
    Col_Char2 data[] = {0xDEAD, 0xBEEF, 0xBAAD, 0xCAFE};
    const char *chunk = (const char *)data;
    const void *p = chunk;
    COL_CHAR_NEXT(COL_UCS2, p);
    PICOTEST_ASSERT(p == chunk + 2);
    COL_CHAR_NEXT(COL_UCS2, p);
    PICOTEST_ASSERT(p == chunk + 4);
    COL_CHAR_NEXT(COL_UCS2, p);
    PICOTEST_ASSERT(p == chunk + 6);
    COL_CHAR_NEXT(COL_UCS2, p);
    PICOTEST_ASSERT(p == chunk + 8);
}
PICOTEST_CASE(testCharPreviousUcs2) {
    Col_Char2 data[] = {0xDEAD, 0xBEEF, 0xBAAD, 0xCAFE};
    const char *chunk = (const char *)data;
    const void *p = chunk + 8;
    COL_CHAR_PREVIOUS(COL_UCS2, p);
    PICOTEST_ASSERT(p == chunk + 6);
    COL_CHAR_PREVIOUS(COL_UCS2, p);
    PICOTEST_ASSERT(p == chunk + 4);
    COL_CHAR_PREVIOUS(COL_UCS2, p);
    PICOTEST_ASSERT(p == chunk + 2);
    COL_CHAR_PREVIOUS(COL_UCS2, p);
    PICOTEST_ASSERT(p == chunk);
}
PICOTEST_CASE(testCharGetUcs4) {
    Col_Char4 data[] = {0xDEADBEEF, 0xBAADCAFE};
    const char *chunk = (const char *)data;
    PICOTEST_VERIFY(COL_CHAR_GET(COL_UCS4, chunk) == 0xDEADBEEF);
    PICOTEST_VERIFY(COL_CHAR_GET(COL_UCS4, chunk + 4) == 0xBAADCAFE);
}
PICOTEST_CASE(testCharNextUcs4) {
    Col_Char4 data[] = {0xDEADBEEF, 0xBAADCAFE};
    const char *chunk = (const char *)data;
    const void *p = chunk;
    COL_CHAR_NEXT(COL_UCS4, p);
    PICOTEST_ASSERT(p == chunk + 4);
    COL_CHAR_NEXT(COL_UCS4, p);
    PICOTEST_ASSERT(p == chunk + 8);
}
PICOTEST_CASE(testCharPreviousUcs4) {
    Col_Char4 data[] = {0xDEADBEEF, 0xBAADCAFE};
    const char *chunk = (const char *)data;
    const void *p = chunk + 8;
    COL_CHAR_PREVIOUS(COL_UCS4, p);
    PICOTEST_ASSERT(p == chunk + 4);
    COL_CHAR_PREVIOUS(COL_UCS4, p);
    PICOTEST_ASSERT(p == chunk);
}
PICOTEST_CASE(testCharGetUtf8) {
    Col_Char1 data[] = {0xEB, 0xAA, 0xAD, 0xF4, 0x8C, 0xAB, 0xBE};
    const char *chunk = (const char *)data;
    PICOTEST_VERIFY(COL_CHAR_GET(COL_UTF8, chunk) == 0xBAAD);
    PICOTEST_VERIFY(COL_CHAR_GET(COL_UTF8, chunk + 3) == 0x10CAFE);
}
PICOTEST_CASE(testCharNextUtf8) {
    Col_Char1 data[] = {0xEB, 0xAA, 0xAD, 0xF4, 0x8C, 0xAB, 0xBE};
    const char *chunk = (const char *)data;
    const void *p = chunk;
    COL_CHAR_NEXT(COL_UTF8, p);
    PICOTEST_ASSERT(p == chunk + 3);
    COL_CHAR_NEXT(COL_UTF8, p);
    PICOTEST_ASSERT(p == chunk + 7);
}
PICOTEST_CASE(testCharPreviousUtf8) {
    Col_Char1 data[] = {0xEB, 0xAA, 0xAD, 0xF4, 0x8C, 0xAB, 0xBE};
    const char *chunk = (const char *)data;
    const void *p = chunk + 7;
    COL_CHAR_PREVIOUS(COL_UTF8, p);
    PICOTEST_ASSERT(p == chunk + 3);
    COL_CHAR_PREVIOUS(COL_UTF8, p);
    PICOTEST_ASSERT(p == chunk);
}
PICOTEST_CASE(testCharGetUtf16) {
    Col_Char2 data[] = {0xBAAD, 0xDBF2, 0xDEFE};
    const char *chunk = (const char *)data;
    PICOTEST_VERIFY(COL_CHAR_GET(COL_UTF16, chunk) == 0xBAAD);
    PICOTEST_VERIFY(COL_CHAR_GET(COL_UTF16, chunk + 2) == 0x10CAFE);
}
PICOTEST_CASE(testCharNextUtf16) {
    Col_Char2 data[] = {0xBAAD, 0xDBF2, 0xDEFE};
    const char *chunk = (const char *)data;
    const void *p = chunk;
    COL_CHAR_NEXT(COL_UTF16, p);
    PICOTEST_ASSERT(p == chunk + 2);
    COL_CHAR_NEXT(COL_UTF16, p);
    PICOTEST_ASSERT(p == chunk + 6);
}
PICOTEST_CASE(testCharPreviousUtf16) {
    Col_Char2 data[] = {0xBAAD, 0xDBF2, 0xDEFE};
    const char *chunk = (const char *)data;
    const void *p = chunk + 6;
    COL_CHAR_PREVIOUS(COL_UTF16, p);
    PICOTEST_ASSERT(p == chunk + 2);
    COL_CHAR_PREVIOUS(COL_UTF16, p);
    PICOTEST_ASSERT(p == chunk);
}
PICOTEST_CASE(testCharGetInvalid) {
    Col_Char1 data[] = {0};
    const char *chunk = (const char *)data;
    PICOTEST_ASSERT(COL_CHAR_GET(COL_UCS, chunk) == COL_CHAR_INVALID);
}
PICOTEST_CASE(testCharNextInvalid) {
    Col_Char1 data[] = {0};
    const char *chunk = (const char *)data;
    const void *p = chunk + 6;
    COL_CHAR_NEXT(COL_UCS, p);
    PICOTEST_ASSERT(p == NULL);
}
PICOTEST_CASE(testCharPreviousInvalid) {
    Col_Char1 data[] = {0};
    const char *chunk = (const char *)data;
    const void *p = chunk + 1;
    COL_CHAR_PREVIOUS(COL_UCS, p);
    PICOTEST_ASSERT(p == NULL);
}
