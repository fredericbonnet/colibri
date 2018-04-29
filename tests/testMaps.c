#include <stdio.h>
#include <limits.h>

#include <colibri.h>

#include "picotest.h"

#include "testColibri.h"

/*
 *---------------------------------------------------------------------------
 *
 * testMaps --
 *
 *---------------------------------------------------------------------------
 */

TEST_SUITE(testMaps, 
   testStringMaps, testIntMaps
)

/*
 *---------------------------------------------------------------------------
 *
 * testStringMaps --
 *
 *---------------------------------------------------------------------------
 */

TEST_SUITE(testStringMaps, 
   testStringTries, testStringHashes
)

/*
 *---------------------------------------------------------------------------
 *
 * testStringTries --
 *
 *---------------------------------------------------------------------------
 */

TEST_FIXTURE_SETUP(testStringTries) {
    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();
}
TEST_FIXTURE_TEARDOWN(testStringTries) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
}
TEST_SUITE(testStringTries, 
   testStringTrie, testStringTrieReverse
)

TEST_CASE(testStringTrie, testStringTries) {
    Col_MapIterator it;
    Col_Word map = Col_NewStringTrieMap();
    int i;
    const int nb=1000;
    Col_Word prev;

    for (i=0; i<nb; i++) {
        Col_Word key;
        char buf[32];
        snprintf(buf, 32, "%d", i);
        key = Col_NewRopeFromString(buf);
        ASSERT(Col_TrieMapSet(map, key, Col_NewIntWord(i+1000)), "index=%d", i);
        ASSERT(!Col_TrieMapSet(map, key, Col_NewIntWord(i+2000)), "index=%d", i);
    }
    ASSERT(Col_MapSize(map) == nb);

    for (i=0; i<nb; i++) {
        Col_Word key, value;
        char buf[32];
        snprintf(buf, 32, "%d", i);
        key = Col_NewRopeFromString(buf);
        ASSERT(Col_TrieMapGet(map, key, &value), "index=%d", i);
        ASSERT(Col_WordType(value) == COL_INT, "index=%d", i);
        ASSERT(Col_IntWordValue(value) == i+2000, "index=%d", i);
    }

    Col_TrieMapIterFirst(it, map);
    ASSERT(!Col_MapIterEnd(it));
    prev = WORD_NIL;
    i = 0;
    while (!Col_MapIterEnd(it)) {
        Col_Word key, value;
        Col_MapIterGet(it, &key, &value);
        if (prev) {
            ASSERT(Col_CompareRopes(prev, key) < 0, "index=%d", i);
        }
        prev = key;
        Col_TrieMapIterNext(it);
        i++;
    }
    ASSERT(i == nb);

    Col_TrieMapIterLast(it, map);
    ASSERT(!Col_MapIterEnd(it));
    prev = WORD_NIL;
    i = 0;
    while (!Col_MapIterEnd(it)) {
        Col_Word key, value;
        Col_MapIterGet(it, &key, &value);
        if (prev) {
            ASSERT(Col_CompareRopes(prev, key) > 0, "index=%d", i);
        }
        prev = key;
        Col_TrieMapIterPrevious(it);
        i++;
    }
    ASSERT(i == nb);

    for (i=0; i<nb; i++) {
        Col_Word key, value;
        char buf[32];
        snprintf(buf, 32, "%d", i);
        key = Col_NewRopeFromString(buf);
        ASSERT(Col_TrieMapUnset(map, key), "index=%d", i);
        ASSERT(!Col_TrieMapUnset(map, key), "index=%d", i);
        ASSERT(!Col_TrieMapGet(map, key, &value), "index=%d", i);
    }
    ASSERT(Col_MapSize(map) == 0);
}

TEST_CASE(testStringTrieReverse, testStringTries) {
    Col_MapIterator it;
    Col_Word map = Col_NewStringTrieMap();
    int i;
    const int nb=1000;
    Col_Word prev;

    for (i=nb-1; i>=0; i--) {
        Col_Word key;
        char buf[32];
        snprintf(buf, 32, "%d", i);
        key = Col_NewRopeFromString(buf);
        ASSERT(Col_TrieMapSet(map, key, Col_NewIntWord(i+1000)), "index=%d", i);
        ASSERT(!Col_TrieMapSet(map, key, Col_NewIntWord(i+2000)), "index=%d", i);
    }
    ASSERT(Col_MapSize(map) == nb);

    for (i=0; i<nb; i++) {
        Col_Word key, value;
        char buf[32];
        snprintf(buf, 32, "%d", i);
        key = Col_NewRopeFromString(buf);
        ASSERT(Col_TrieMapGet(map, key, &value), "index=%d", i);
        ASSERT(Col_WordType(value) == COL_INT, "index=%d", i);
        ASSERT(Col_IntWordValue(value) == i+2000, "index=%d", i);
    }

    Col_TrieMapIterFirst(it, map);
    ASSERT(!Col_MapIterEnd(it));
    prev = WORD_NIL;
    i = 0;
    while (!Col_MapIterEnd(it)) {
        Col_Word key;
        key = Col_MapIterGetKey(it);
        if (prev) {
            ASSERT(Col_CompareRopes(prev, key) < 0, "index=%d", i);
        }
        prev = key;
        Col_TrieMapIterNext(it);
        i++;
    }
    ASSERT(i == nb);

    Col_TrieMapIterLast(it, map);
    ASSERT(!Col_MapIterEnd(it));
    prev = WORD_NIL;
    i = 0;
    while (!Col_MapIterEnd(it)) {
        Col_Word key;
        key = Col_MapIterGetKey(it);
        if (prev) {
            ASSERT(Col_CompareRopes(prev, key) > 0, "index=%d", i);
        }
        prev = key;
        Col_TrieMapIterPrevious(it);
        i++;
    }
    ASSERT(i == nb);

    for (i=0; i<nb; i++) {
        Col_Word key, value;
        char buf[32];
        snprintf(buf, 32, "%d", i);
        key = Col_NewRopeFromString(buf);
        ASSERT(Col_TrieMapUnset(map, key), "index=%d", i);
        ASSERT(!Col_TrieMapUnset(map, key), "index=%d", i);
        ASSERT(!Col_TrieMapGet(map, key, &value), "index=%d", i);
    }
    ASSERT(Col_MapSize(map) == 0);
}

/*
 *---------------------------------------------------------------------------
 *
 * testStringHashes --
 *
 *---------------------------------------------------------------------------
 */

TEST_FIXTURE_SETUP(testStringHashes) {
    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();
}
TEST_FIXTURE_TEARDOWN(testStringHashes) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
}
TEST_CASE(testStringHashes, testStringHashes) {
    Col_MapIterator it;
    Col_Word map = Col_NewStringHashMap(0);
    int i;
    const int nb=1000;

    for (i=0; i<nb; i++) {
        Col_Word key;
        char buf[32];
        snprintf(buf, 32, "%d", i);
        key = Col_NewRopeFromString(buf);
        ASSERT(Col_HashMapSet(map, key, Col_NewIntWord(i+1000)), "index=%d", i);
        ASSERT(!Col_HashMapSet(map, key, Col_NewIntWord(i+2000)), "index=%d", i);
    }
    ASSERT(Col_MapSize(map) == nb);

    for (i=0; i<nb; i++) {
        Col_Word key, value;
        char buf[32];
        snprintf(buf, 32, "%d", i);
        key = Col_NewRopeFromString(buf);
        ASSERT(Col_HashMapGet(map, key, &value), "index=%d", i);
        ASSERT(Col_WordType(value) == COL_INT, "index=%d", i);
        ASSERT(Col_IntWordValue(value) == i+2000, "index=%d", i);
    }

    Col_HashMapIterBegin(it, map);
    ASSERT(!Col_MapIterEnd(it));
    i = 0;
    while (!Col_MapIterEnd(it)) {
        Col_Word key, value;
        Col_MapIterGet(it, &key, &value);
        Col_HashMapIterNext(it);
        i++;
    }
    ASSERT(i == nb);

    for (i=0; i<nb; i++) {
        Col_Word key, value;
        char buf[32];
        snprintf(buf, 32, "%d", i);
        key = Col_NewRopeFromString(buf);
        ASSERT(Col_HashMapUnset(map, key), "index=%d", i);
        ASSERT(!Col_HashMapUnset(map, key), "index=%d", i);
        ASSERT(!Col_HashMapGet(map, key, &value), "index=%d", i);
    }
    ASSERT(Col_MapSize(map) == 0);
}

/*
 *---------------------------------------------------------------------------
 *
 * testIntMaps --
 *
 *---------------------------------------------------------------------------
 */

TEST_SUITE(testIntMaps, 
   testIntTries, testIntHashes
)

/*
 *---------------------------------------------------------------------------
 *
 * testIntTries --
 *
 *---------------------------------------------------------------------------
 */

TEST_FIXTURE_SETUP(testIntTries) {
    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();
}
TEST_FIXTURE_TEARDOWN(testIntTries) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
}
TEST_SUITE(testIntTries, 
   testIntTrie, testIntTrieReverse
)

TEST_CASE(testIntTrie, testIntTries) {
    Col_MapIterator it;
    Col_Word map = Col_NewIntTrieMap();
    int i;
    const int nb=1000;
    intptr_t prev;

    for (i=0; i<nb; i++) {
        ASSERT(Col_IntTrieMapSet(map, i, Col_NewIntWord(i+1000)), "index=%d", i);
        ASSERT(!Col_IntTrieMapSet(map, i, Col_NewIntWord(i+2000)), "index=%d", i);
    }
    ASSERT(Col_MapSize(map) == nb);

    for (i=0; i<nb; i++) {
        Col_Word value;
        ASSERT(Col_IntTrieMapGet(map, i, &value), "index=%d", i);
        ASSERT(Col_WordType(value) == COL_INT, "index=%d", i);
        ASSERT(Col_IntWordValue(value) == i+2000, "index=%d", i);
    }

    Col_TrieMapIterFirst(it, map);
    ASSERT(!Col_MapIterEnd(it));
    prev = -1;
    i = 0;
    while (!Col_MapIterEnd(it)) {
        intptr_t key;
        Col_Word value;
        Col_IntMapIterGet(it, &key, &value);
        if (prev != -1) {
            ASSERT(prev < key, "index=%d", i);
        }
        prev = key;
        Col_TrieMapIterNext(it);
        i++;
    }
    ASSERT(i == nb);

    Col_TrieMapIterLast(it, map);
    ASSERT(!Col_MapIterEnd(it));
    prev = -1;
    i = 0;
    while (!Col_MapIterEnd(it)) {
        intptr_t key;
        Col_Word value;
        Col_IntMapIterGet(it, &key, &value);
        if (prev != -1) {
            ASSERT(prev > key, "index=%d", i);
        }
        prev = key;
        Col_TrieMapIterPrevious(it);
        i++;
    }
    ASSERT(i == nb);

    for (i=0; i<nb; i++) {
        Col_Word value;
        ASSERT(Col_IntTrieMapUnset(map, i), "index=%d", i);
        ASSERT(!Col_IntTrieMapUnset(map, i), "index=%d", i);
        ASSERT(!Col_IntTrieMapGet(map, i, &value), "index=%d", i);
    }
    ASSERT(Col_MapSize(map) == 0);
}

TEST_CASE(testIntTrieReverse, testIntTries) {
    Col_MapIterator it;
    Col_Word map = Col_NewIntTrieMap();
    int i;
    const int nb=1000;
    intptr_t prev;

    for (i=nb-1; i>=0; i--) {
        ASSERT(Col_IntTrieMapSet(map, i, Col_NewIntWord(i+1000)), "index=%d", i);
        ASSERT(!Col_IntTrieMapSet(map, i, Col_NewIntWord(i+2000)), "index=%d", i);
    }
    ASSERT(Col_MapSize(map) == nb);

    for (i=0; i<nb; i++) {
        Col_Word value;
        ASSERT(Col_IntTrieMapGet(map, i, &value), "index=%d", i);
        ASSERT(Col_WordType(value) == COL_INT, "index=%d", i);
        ASSERT(Col_IntWordValue(value) == i+2000, "index=%d", i);
    }

    Col_TrieMapIterFirst(it, map);
    ASSERT(!Col_MapIterEnd(it));
    prev = -1;
    i = 0;
    while (!Col_MapIterEnd(it)) {
        intptr_t key;
        Col_Word value;
        Col_IntMapIterGet(it, &key, &value);
        if (prev != -1) {
            ASSERT(prev < key, "index=%d", i);
        }
        prev = key;
        Col_TrieMapIterNext(it);
        i++;
    }
    ASSERT(i == nb);

    Col_TrieMapIterLast(it, map);
    ASSERT(!Col_MapIterEnd(it));
    prev = -1;
    i = 0;
    while (!Col_MapIterEnd(it)) {
        intptr_t key;
        Col_Word value;
        Col_IntMapIterGet(it, &key, &value);
        if (prev != -1) {
            ASSERT(prev > key, "index=%d", i);
        }
        prev = key;
        Col_TrieMapIterPrevious(it);
        i++;
    }
    ASSERT(i == nb);

    for (i=0; i<nb; i++) {
        Col_Word value;
        ASSERT(Col_IntTrieMapUnset(map, i), "index=%d", i);
        ASSERT(!Col_IntTrieMapUnset(map, i), "index=%d", i);
        ASSERT(!Col_IntTrieMapGet(map, i, &value), "index=%d", i);
    }
    ASSERT(Col_MapSize(map) == 0);
}

/*
 *---------------------------------------------------------------------------
 *
 * testIntHashes --
 *
 *---------------------------------------------------------------------------
 */

TEST_FIXTURE_SETUP(testIntHashes) {
    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();
}
TEST_FIXTURE_TEARDOWN(testIntHashes) {
    if (!TEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
}
TEST_CASE(testIntHashes, testIntHashes) {
    Col_MapIterator it;
    Col_Word map = Col_NewIntHashMap(0);
    int i;
    const int nb=1000;

    for (i=0; i<nb; i++) {
        ASSERT(Col_IntHashMapSet(map, i, Col_NewIntWord(i+1000)), "index=%d", i);
        ASSERT(!Col_IntHashMapSet(map, i, Col_NewIntWord(i+2000)), "index=%d", i);
    }
    ASSERT(Col_MapSize(map) == nb);

    for (i=0; i<nb; i++) {
        Col_Word value;
        ASSERT(Col_IntHashMapGet(map, i, &value), "index=%d", i);
        ASSERT(Col_WordType(value) == COL_INT, "index=%d", i);
        ASSERT(Col_IntWordValue(value) == i+2000, "index=%d", i);
    }

    Col_HashMapIterBegin(it, map);
    ASSERT(!Col_MapIterEnd(it));
    i = 0;
    while (!Col_MapIterEnd(it)) {
        intptr_t key;
        Col_Word value;
        Col_IntMapIterGet(it, &key, &value);
        Col_HashMapIterNext(it);
        i++;
    }
    ASSERT(i == nb);

    for (i=0; i<nb; i++) {
        Col_Word value;
        ASSERT(Col_IntHashMapUnset(map, i), "index=%d", i);
        ASSERT(!Col_IntHashMapUnset(map, i), "index=%d", i);
        ASSERT(!Col_IntHashMapGet(map, i, &value), "index=%d", i);
    }
    ASSERT(Col_MapSize(map) == 0);
}
