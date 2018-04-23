#include <limits.h>
#include <float.h>

#include <colibri.h>

#include "unitTest.h"


/*
 *---------------------------------------------------------------------------
 *
 * testImmediateWords --
 *
 *---------------------------------------------------------------------------
 */

TEST_FIXTURE_SETUP(testImmediateWords) {
    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();
}
TEST_FIXTURE_TEARDOWN(testImmediateWords) {
    if (!TEST_FAIL) {
	Col_ResumeGC();
    }
    Col_Cleanup();
}
TEST_SUITE(testImmediateWords, 
   testBools, testImmediateInts, testImmediateFloats, testImmediateStrings, 
   testImmediateLists
)

/*
 *---------------------------------------------------------------------------
 *
 * testBools --
 *
 *---------------------------------------------------------------------------
 */

TEST_CASE(testBools, testImmediateWords) {
    ASSERT(Col_WordType(WORD_TRUE) & COL_BOOL);
    ASSERT(Col_WordType(WORD_FALSE) & COL_BOOL);
    ASSERT(!Col_BoolWordValue(WORD_FALSE));
    ASSERT(Col_BoolWordValue(WORD_TRUE));
    ASSERT(Col_NewBoolWord(0) == WORD_FALSE);
    ASSERT(Col_NewBoolWord(1) == WORD_TRUE);

    ASSERT(!(Col_WordType(Col_NewCharWord(0)) & COL_BOOL));
    ASSERT(!(Col_WordType(Col_NewIntWord(0)) & COL_BOOL));
}

/*
 *---------------------------------------------------------------------------
 *
 * testImmediateInts --
 *
 *---------------------------------------------------------------------------
 */

TEST_CASE(testImmediateInts, testImmediateWords) {
    Col_Word word;

    // Immediate ints.
    word = Col_NewIntWord(0x12345678);
    ASSERT(Col_IntWordValue(word) == 0x12345678);
    ASSERT(word & 1);
    ASSERT(Col_WordType(word) & COL_INT);
    ASSERT(Col_IntWordValue(word) == 0x12345678);

    word = Col_NewIntWord(-0x3FEDCBA9);
    ASSERT(Col_IntWordValue(word) == -0x3FEDCBA9);
    ASSERT(word & 1);
    ASSERT(Col_WordType(word) & COL_INT);
    ASSERT(Col_IntWordValue(word) == -0x3FEDCBA9);
					
    // Non-immediate ints on 32-bit systems, immediate on 64-bit.
    word = Col_NewIntWord(0x87654321);
    ASSERT(Col_IntWordValue(word) == 0x87654321);
    ASSERT(sizeof(intptr_t) == 4
	? (word & 0xF) == 0	/* 32-bit */
	: (word & 1)		/* 64-bit */
    );
    ASSERT(Col_WordType(word) & COL_INT);
    ASSERT(Col_IntWordValue(word) == 0x87654321);

    // Regular int.
    word = Col_NewIntWord(INTPTR_MAX);
    ASSERT(Col_IntWordValue(word) == INTPTR_MAX);
    ASSERT((word & 0xF) == 0);
    ASSERT(Col_WordType(word) & COL_INT);
    ASSERT(Col_IntWordValue(word) == INTPTR_MAX);
}

/*
 *---------------------------------------------------------------------------
 *
 * testImmediateFloats --
 *
 *---------------------------------------------------------------------------
 */

TEST_CASE(testImmediateFloats, testImmediateWords) {
    Col_Word word;

    // Immediate floats.
    word = Col_NewFloatWord(0.0);
    ASSERT(Col_FloatWordValue(word) == 0.0);
    ASSERT(word & 2);
    ASSERT(Col_WordType(word) & COL_FLOAT);
    ASSERT(Col_FloatWordValue(word)  == 0.0);

    word = Col_NewFloatWord(1.0);
    ASSERT(Col_FloatWordValue(word) == 1.0);
    ASSERT(word & 2);
    ASSERT(Col_WordType(word) & COL_FLOAT);
    ASSERT(Col_FloatWordValue(word)  == 1.0);
					
    word = Col_NewFloatWord(4194303 /* = 2^22-1 */);
    ASSERT(Col_FloatWordValue(word) == 4194303);
    ASSERT(word & 2);
    ASSERT(Col_WordType(word) & COL_FLOAT);
    ASSERT(Col_FloatWordValue(word)  == 4194303);
					
    word = Col_NewFloatWord(0.0000002384185791015625 /* = 2^-22 */);
    ASSERT(Col_FloatWordValue(word) == 0.0000002384185791015625);
    ASSERT(word & 2);
    ASSERT(Col_WordType(word) & COL_FLOAT);
    ASSERT(Col_FloatWordValue(word)  == 0.0000002384185791015625);
					
    // Non-immediate floats on 32-bit systems, immediate on 64-bit.
    word = Col_NewFloatWord(8388607 /* = 2^23-1 */);
    ASSERT(Col_FloatWordValue(word) == 8388607);
    ASSERT(sizeof(intptr_t) == 4
	? (word & 0xF) == 0	/* 32-bit */
	: (word & 2)		/* 64-bit */
    );
    ASSERT(Col_WordType(word) & COL_FLOAT);
    ASSERT(Col_FloatWordValue(word)  == 8388607);

    // Regular floats.
    word = Col_NewFloatWord(1.7/4.3);
    ASSERT(Col_FloatWordValue(word) == 1.7/4.3);
    ASSERT((word & 0xF) == 0);
    ASSERT(Col_WordType(word) & COL_FLOAT);
    ASSERT(Col_FloatWordValue(word)  == 1.7/4.3);
}

/*
 *---------------------------------------------------------------------------
 *
 * testImmediateStrings --
 *
 *---------------------------------------------------------------------------
 */

TEST_CASE(testImmediateStrings, testImmediateWords) {
    Col_Word word;
    char str1[] = "12345678";
    unsigned short str21[] = {'1', '2', '3', '4', '5', '6', '7', '8', 0}, str22[] = {0x1234, 0x5678};
    int str41[] = {'1', '2', '3', '4', '5', '6', '7', '8', 0}, str42[] = {0x1234, 0x5678, 0x9ABCD};

    // Immediate chars.
    word = Col_NewRope(COL_UCS1, str1, 1);
    ASSERT(word == 0x3124);
    ASSERT(Col_WordType(word) & COL_CHAR);
    ASSERT(Col_StringWordFormat(word) == COL_UCS1);
    word = Col_NewRope(COL_UCS2, str21, 2);
    ASSERT((word & 0xFF) == 0x44);
    ASSERT(Col_WordType(word) & COL_CHAR);
    ASSERT(Col_StringWordFormat(word) == COL_UCS2);
    word = Col_NewRope(COL_UCS2, str22, 2);
    ASSERT(word == 0x123444);
    ASSERT(Col_WordType(word) & COL_CHAR);
    ASSERT(Col_StringWordFormat(word) == COL_UCS2);
    word = Col_NewRope(COL_UCS4, str41, 4);
    ASSERT((word & 0xFF) == 0x84);
    ASSERT(Col_WordType(word) & COL_CHAR);
    ASSERT(Col_StringWordFormat(word) == COL_UCS4);
    word = Col_NewRope(COL_UCS4, str42, 4);
    ASSERT(word == 0x123484);
    ASSERT(Col_WordType(word) & COL_CHAR);
    ASSERT(Col_StringWordFormat(word) == COL_UCS4);
    word = Col_NewRope(COL_UCS, str41, 4);
    ASSERT(word == 0x3124);
    ASSERT(Col_WordType(word) & COL_CHAR);
    ASSERT(Col_StringWordFormat(word) == COL_UCS1);
    word = Col_NewRope(COL_UCS, str42, 4);
    ASSERT((word & 0xFF) == 0x44);
    ASSERT(Col_WordType(word) & COL_CHAR);
    ASSERT(Col_StringWordFormat(word) == COL_UCS2);
    word = Col_NewRope(COL_UCS, str42+2, 4);
    ASSERT((word & 0xFF) == 0x84);
    ASSERT(Col_WordType(word) & COL_CHAR);
    ASSERT(Col_StringWordFormat(word) == COL_UCS4);

    // Immediate strings.
    word = Col_NewRope(COL_UCS1, str1, 0);
    ASSERT(word == 0xc);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS1);
    word = Col_NewRope(COL_UCS1, str1, 2);
    ASSERT((word & 0xFF) == 0x4C);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS1);
    word = Col_NewRope(COL_UCS1, str1, 3);
    ASSERT((word & 0xFF) == 0x6C);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS1);

    word = Col_NewRope(COL_UCS2, str21, 0);
    ASSERT(word == 0xc);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS1);

    word = Col_NewRope(COL_UCS4, str41, 0);
    ASSERT(word == 0xc);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS1);

    word = Col_NewRope(COL_UCS, str41, 8);
    ASSERT((word & 0xFF) == 0x4C);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS1);
    word = Col_NewRope(COL_UCS, str41, 12);
    ASSERT((word & 0xFF) == 0x6C);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS1);

    // Non-immediate strings on 32-bit systems, immediate on 64-bit.
    word = Col_NewRope(COL_UCS1, str1, 4);
    ASSERT(sizeof(intptr_t) == 4
	? (word & 0xF) == 0		/* 32-bit */
	: ((word & 0xFF) == 0x8C)	/* 64-bit */
    );
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS1);
    word = Col_NewRope(COL_UCS, str41, 16);
    ASSERT(sizeof(intptr_t) == 4
	? (word & 0xF) == 0		/* 32-bit */
	: ((word & 0xFF) == 0x8C)	/* 64-bit */
    );
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS1);

    // Non-immediate strings.
    word = Col_NewRope(COL_UCS1, str1, 8);
    ASSERT((word & 0xF) == 0);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS1);

    word = Col_NewRope(COL_UCS2, str21, 6);
    ASSERT((word & 0xF) == 0);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS2);
    word = Col_NewRope(COL_UCS2, str21, 8);
    ASSERT((word & 0xF) == 0);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS2);
    word = Col_NewRope(COL_UCS2, str22, 4);
    ASSERT((word & 0xF) == 0);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS2);

    word = Col_NewRope(COL_UCS4, str41, 8);
    ASSERT((word & 0xF) == 0);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS4);
    word = Col_NewRope(COL_UCS4, str41, 12);
    ASSERT((word & 0xF) == 0);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS4);
    word = Col_NewRope(COL_UCS4, str41, 16);
    ASSERT((word & 0xF) == 0);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS4);
    word = Col_NewRope(COL_UCS4, str42, 8);
    ASSERT((word & 0xF) == 0);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS4);

    word = Col_NewRope(COL_UCS, str41, 32);
    ASSERT((word & 0xF) == 0);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS1);
    word = Col_NewRope(COL_UCS, str42, 8);
    ASSERT((word & 0xF) == 0);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS2);
    word = Col_NewRope(COL_UCS, str42, 12);
    ASSERT((word & 0xF) == 0);
    ASSERT(Col_WordType(word) & COL_STRING);
    ASSERT(Col_StringWordFormat(word) == COL_UCS4);
}

/*
 *---------------------------------------------------------------------------
 *
 * testImmediateLists --
 *
 *---------------------------------------------------------------------------
 */

TEST_CASE(testImmediateLists, testImmediateWords) {
    Col_Word word;

    // Immediate void lists.
    word = Col_NewList(0, NULL);
    ASSERT(word == 0x14);
    ASSERT(Col_WordType(word) & COL_LIST);
    word = Col_NewList(1, NULL);
    ASSERT(word == 0x34);
    ASSERT(Col_WordType(word) & COL_LIST);

    // Regular void lists.
    word = Col_NewList(SIZE_MAX, NULL);
    ASSERT((word & 0xF) == 0);
    ASSERT(Col_WordType(word) & COL_LIST);

    // Circular lists.
    word = Col_CircularList(Col_NewList(SIZE_MAX, NULL));
    ASSERT((word & 0xF) == 8);
    ASSERT(Col_WordType(word) & COL_LIST);

    // Circular void lists.
    word = Col_CircularList(Col_NewList(0, NULL));
    ASSERT(word == 0x14); // Empty list cannot be circular.
    ASSERT(Col_WordType(word) & COL_LIST);
    word = Col_CircularList(Col_NewList(1, NULL));
    ASSERT(word == 0x3C);
    ASSERT(Col_WordType(word) & COL_LIST);
}
