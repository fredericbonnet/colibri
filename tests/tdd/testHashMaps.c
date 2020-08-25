#include <colibri.h>
#include <picotest.h>

/*
 * Failure test cases (must be defined before test hooks)
 */

#include "failureFixture.h"

/* Col_NewStringHashMap */
PICOTEST_CASE(newStringHashMap_fatal_outOfMemory, failureFixture, context) {
    EXPECT_FAILURE(context, COL_FATAL, Col_GetErrorDomain(), COL_ERROR_MEMORY);
    Col_NewStringHashMap(SIZE_MAX);
    PICOTEST_ASSERT(!"UNREACHABLE");
}

/* Col_NewIntHashMap */
PICOTEST_CASE(newIntHashMap_fatal_outOfMemory, failureFixture, context) {
    EXPECT_FAILURE(context, COL_FATAL, Col_GetErrorDomain(), COL_ERROR_MEMORY);
    Col_NewIntHashMap(SIZE_MAX);
    PICOTEST_ASSERT(!"UNREACHABLE");
}

/* Col_CopyHashMap */
PICOTEST_CASE(copyHashMap_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_HASHMAP);
    PICOTEST_ASSERT(Col_CopyHashMap(WORD_NIL) == WORD_NIL);
}

/* Col_HashMapGet */
PICOTEST_CASE(hashMapGet_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_WORDHASHMAP);
    PICOTEST_ASSERT(Col_HashMapGet(WORD_NIL, WORD_TRUE, NULL) == WORD_NIL);
}

/* Col_IntHashMapGet */
PICOTEST_CASE(intHashMapGet_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_INTHASHMAP);
    PICOTEST_ASSERT(Col_IntHashMapGet(WORD_NIL, 0, NULL) == 0);
}

/* Col_HashMapSet */
PICOTEST_CASE(hashMapSet_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_WORDHASHMAP);
    PICOTEST_ASSERT(Col_HashMapSet(WORD_NIL, WORD_TRUE, WORD_FALSE) == 0);
}

/* Col_IntHashMapSet */
PICOTEST_CASE(intHashMapSet_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_INTHASHMAP);
    PICOTEST_ASSERT(Col_IntHashMapSet(WORD_NIL, 0, WORD_FALSE) == 0);
}

/* Col_HashMapUnset */
PICOTEST_CASE(hashMapUnset_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_WORDHASHMAP);
    PICOTEST_ASSERT(Col_HashMapUnset(WORD_NIL, WORD_TRUE) == 0);
}

/* Col_IntHashMapUnset */
PICOTEST_CASE(intHashMapUnset_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_INTHASHMAP);
    PICOTEST_ASSERT(Col_IntHashMapUnset(WORD_NIL, 0) == 0);
}

/* Col_HashMapIterBegin */
PICOTEST_CASE(hashMapIterBegin_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_HASHMAP);
    Col_MapIterator it;
    Col_HashMapIterBegin(it, WORD_NIL);
    PICOTEST_ASSERT(Col_MapIterNull(it));
}

/* Col_HashMapIterFind */
PICOTEST_CASE(hashMapIterFind_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_WORDHASHMAP);
    Col_MapIterator it;
    Col_HashMapIterFind(it, WORD_NIL, WORD_TRUE, NULL);
    PICOTEST_ASSERT(Col_MapIterNull(it));
}

/* Col_IntHashMapIterFind */
PICOTEST_CASE(intHashMapIterFind_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_INTHASHMAP);
    Col_MapIterator it;
    Col_IntHashMapIterFind(it, WORD_NIL, WORD_TRUE, NULL);
    PICOTEST_ASSERT(Col_MapIterNull(it));
}

/* Col_HashMapIterSetValue */
PICOTEST_CASE(hashMapIterSetValue_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER);
    Col_MapIterator it = COL_MAPITER_NULL;
    Col_HashMapIterSetValue(it, WORD_TRUE);
}
PICOTEST_CASE(hashMapIterSetValue_typeCheck_map, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_HASHMAP);
    Col_MapIterator it = COL_MAPITER_NULL;
    Col_MapIterMap(it) = WORD_TRUE; // TODO replace with trie iterator
    Col_HashMapIterSetValue(it, WORD_TRUE);
}
PICOTEST_CASE(hashMapIterSetValue_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER_END);
    Col_MapIterator it;
    Col_HashMapIterBegin(it, Col_NewStringHashMap(0));
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    Col_HashMapIterSetValue(it, WORD_TRUE);
}

/* Col_HashMapIterNext */
PICOTEST_CASE(hashMapIterNext_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER);
    Col_MapIterator it = COL_MAPITER_NULL;
    Col_HashMapIterNext(it);
}
PICOTEST_CASE(hashMapIterNext_typeCheck_map, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_HASHMAP);
    Col_MapIterator it = COL_MAPITER_NULL;
    Col_MapIterMap(it) = WORD_TRUE; // TODO replace with trie iterator
    Col_HashMapIterNext(it);
}
PICOTEST_CASE(hashMapIterNext_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER_END);
    Col_MapIterator it;
    Col_HashMapIterBegin(it, Col_NewStringHashMap(0));
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    Col_HashMapIterNext(it);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
}

/*
 * Test data
 */

#define STRING_HASH_KEY(i) Col_NewCharWord('a' + i)
#define STRING_HASH_KEY_COLLISION(i) Col_NewCharWord(COL_CHAR1_MAX + i * 256)
#define STRING_HASH_VALUE(i) Col_NewIntWord(i)
#define INT_HASH_KEY(i) ((intptr_t)(i))
#define INT_HASH_KEY_COLLISION(i) ((intptr_t)(i * 256))
#define INT_HASH_VALUE(i) Col_NewCharWord('A' + i)

static Col_Word STRING_HASH_MAP(size_t size) {
    Col_Word map = Col_NewStringHashMap(0);
    for (size_t i = 0; i < size; i++) {
        Col_HashMapSet(map, STRING_HASH_KEY(i), STRING_HASH_VALUE(i));
    }
    return map;
}
static Col_Word INT_HASH_MAP(size_t size) {
    Col_Word map = Col_NewIntHashMap(0);
    for (size_t i = 0; i < size; i++) {
        Col_IntHashMapSet(map, INT_HASH_KEY(i), INT_HASH_VALUE(i));
    }
    return map;
}
static Col_Word STRING_HASH_MAP_COLLISION(size_t size) {
    Col_Word map = Col_NewStringHashMap(0);
    for (size_t i = 0; i < size; i++) {
        Col_HashMapSet(map, STRING_HASH_KEY_COLLISION(i), STRING_HASH_VALUE(i));
    }
    return map;
}
static Col_Word INT_HASH_MAP_COLLISION(size_t size) {
    Col_Word map = Col_NewIntHashMap(0);
    for (size_t i = 0; i < size; i++) {
        Col_IntHashMapSet(map, INT_HASH_KEY_COLLISION(i), INT_HASH_VALUE(i));
    }
    return map;
}

/*
 * Hash maps
 */

#include "hooks.h"
#include "colibriFixture.h"

PICOTEST_SUITE(testHashMaps, testHashMapTypeChecks, testHashMapCreation,
               testHashMapAccessors, testHashMapIteration);

PICOTEST_CASE(testHashMapTypeChecks, colibriFixture) {
    PICOTEST_VERIFY(copyHashMap_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(hashMapGet_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(intHashMapGet_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(hashMapSet_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(intHashMapSet_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(hashMapUnset_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(intHashMapUnset_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(hashMapIterBegin_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(hashMapIterFind_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(intHashMapIterFind_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(hashMapIterSetValue_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(hashMapIterSetValue_typeCheck_map(NULL) == 1);
    PICOTEST_VERIFY(hashMapIterNext_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(hashMapIterNext_typeCheck_map(NULL) == 1);
}

PICOTEST_SUITE(testHashMapCreation, testStringHashMapCreation,
               testIntegerHashMapCreation, testCopyHashMap);

PICOTEST_SUITE(testStringHashMapCreation, testNewStringHashMapError,
               testNewStringHashMap);

PICOTEST_SUITE(testIntegerHashMapCreation, testNewIntHashMapError,
               testNewIntHashMap);

PICOTEST_SUITE(testNewStringHashMapError, testNewStringHashMapOutOfMemory);
PICOTEST_CASE(testNewStringHashMapOutOfMemory, colibriFixture) {
    PICOTEST_ASSERT(newStringHashMap_fatal_outOfMemory(NULL) == 1);
}

static void checkStringHashMap(Col_Word map, size_t size) {
    PICOTEST_ASSERT(Col_WordType(map) == (COL_MAP | COL_HASHMAP));
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
}
PICOTEST_CASE(testNewStringHashMap, colibriFixture) {
    checkStringHashMap(Col_NewStringHashMap(0), 0);
    checkStringHashMap(Col_NewStringHashMap(10), 0);
    checkStringHashMap(Col_NewStringHashMap(100), 0);
}

PICOTEST_SUITE(testNewIntHashMapError, testNewIntHashMapOutOfMemory);
PICOTEST_CASE(testNewIntHashMapOutOfMemory, colibriFixture) {
    PICOTEST_ASSERT(newIntHashMap_fatal_outOfMemory(NULL) == 1);
}

static void checkIntHashMap(Col_Word map, size_t size) {
    PICOTEST_ASSERT(Col_WordType(map) == (COL_INTMAP | COL_HASHMAP));
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
}
PICOTEST_CASE(testNewIntHashMap, colibriFixture) {
    checkIntHashMap(Col_NewIntHashMap(0), 0);
    checkIntHashMap(Col_NewIntHashMap(10), 0);
    checkIntHashMap(Col_NewIntHashMap(100), 0);
}

static void checkCopyHashMap(Col_Word map) {
    size_t size = Col_MapSize(map);

    Col_Word copy1 = Col_CopyHashMap(map);
    PICOTEST_ASSERT(Col_MapSize(copy1) == size);
    Col_Word copy2 = Col_CopyHashMap(map);
    PICOTEST_ASSERT(Col_MapSize(copy2) == size);
    Col_Word copy3 = Col_CopyHashMap(copy1);
    PICOTEST_ASSERT(Col_MapSize(copy3) == size);
}
PICOTEST_CASE(testCopyHashMap, colibriFixture) {
    checkCopyHashMap(STRING_HASH_MAP(1));
    checkCopyHashMap(STRING_HASH_MAP(10));
    checkCopyHashMap(STRING_HASH_MAP(100));
    checkCopyHashMap(STRING_HASH_MAP_COLLISION(10));
    checkCopyHashMap(STRING_HASH_MAP_COLLISION(100));
    checkCopyHashMap(INT_HASH_MAP(1));
    checkCopyHashMap(INT_HASH_MAP(10));
    checkCopyHashMap(INT_HASH_MAP(100));
    checkCopyHashMap(INT_HASH_MAP_COLLISION(10));
    checkCopyHashMap(INT_HASH_MAP_COLLISION(100));
}

PICOTEST_SUITE(testHashMapAccessors, testStringHashMapAccessors,
               testIntegerHashMapAccessors);

PICOTEST_SUITE(testStringHashMapAccessors, testStringHashMapGet,
               testStringHashMapSet, testStringHashMapUnset);

PICOTEST_SUITE(testStringHashMapGet, testStringHashMapGetEmpty,
               testStringHashMapGetFound, testStringHashMapGetNotFound);
PICOTEST_CASE(testStringHashMapGetEmpty, colibriFixture) {
    Col_Word map = Col_NewStringHashMap(0);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_HashMapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}
PICOTEST_CASE(testStringHashMapGetFound, colibriFixture) {
    Col_Word map = Col_NewStringHashMap(0);
    Col_Word key = Col_NewCharWord('a');
    Col_HashMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_HashMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testStringHashMapGetNotFound, colibriFixture) {
    Col_Word map = Col_NewStringHashMap(0);
    Col_Word key = Col_NewCharWord('a');
    Col_HashMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_HashMapGet(map, Col_NewCharWord('b'), &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}

PICOTEST_SUITE(testStringHashMapSet, testStringHashMapSetEmpty,
               testStringHashMapSetCreated, testStringHashMapSetUpdated,
               testStringHashMapSetUpdatedCopy, testStringHashMapSetGrowBuckets,
               testStringHashMapSetCollisions);
PICOTEST_CASE(testStringHashMapSetEmpty, colibriFixture) {
    Col_Word map = Col_NewStringHashMap(0);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    PICOTEST_ASSERT(Col_HashMapSet(map, key, WORD_TRUE) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
}
PICOTEST_CASE(testStringHashMapSetCreated, colibriFixture) {
    Col_Word map = Col_NewStringHashMap(0);
    Col_Word key = Col_NewCharWord('a');
    Col_HashMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    PICOTEST_ASSERT(Col_HashMapSet(map, Col_NewCharWord('b'), WORD_FALSE) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == 2);
}

static void checkStringHashMapSetUpdated(Col_Word map, Col_Word key) {
    size_t size = Col_MapSize(map);
    Col_HashMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_HashMapSet(map, key, WORD_FALSE) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == size);
}
PICOTEST_CASE(testStringHashMapSetUpdated, colibriFixture) {
    checkStringHashMapSetUpdated(STRING_HASH_MAP(1), STRING_HASH_KEY(0));
    checkStringHashMapSetUpdated(STRING_HASH_MAP(10), STRING_HASH_KEY(0));
    checkStringHashMapSetUpdated(STRING_HASH_MAP(10), STRING_HASH_KEY(5));
    checkStringHashMapSetUpdated(STRING_HASH_MAP(10), STRING_HASH_KEY(9));
    checkStringHashMapSetUpdated(STRING_HASH_MAP(100), STRING_HASH_KEY(0));
    checkStringHashMapSetUpdated(STRING_HASH_MAP(100), STRING_HASH_KEY(33));
    checkStringHashMapSetUpdated(STRING_HASH_MAP(100), STRING_HASH_KEY(66));
    checkStringHashMapSetUpdated(STRING_HASH_MAP(100), STRING_HASH_KEY(99));
    checkStringHashMapSetUpdated(STRING_HASH_MAP_COLLISION(10),
                                 STRING_HASH_KEY_COLLISION(0));
    checkStringHashMapSetUpdated(STRING_HASH_MAP_COLLISION(10),
                                 STRING_HASH_KEY_COLLISION(5));
    checkStringHashMapSetUpdated(STRING_HASH_MAP_COLLISION(10),
                                 STRING_HASH_KEY_COLLISION(9));
    checkStringHashMapSetUpdated(STRING_HASH_MAP_COLLISION(100),
                                 STRING_HASH_KEY_COLLISION(0));
    checkStringHashMapSetUpdated(STRING_HASH_MAP_COLLISION(100),
                                 STRING_HASH_KEY_COLLISION(33));
    checkStringHashMapSetUpdated(STRING_HASH_MAP_COLLISION(100),
                                 STRING_HASH_KEY_COLLISION(66));
    checkStringHashMapSetUpdated(STRING_HASH_MAP_COLLISION(100),
                                 STRING_HASH_KEY_COLLISION(99));
}

static void checkStringHashMapSetUpdatedCopy(Col_Word map, Col_Word key) {
    Col_Word copy1 = Col_CopyHashMap(map);

    checkStringHashMapSetUpdated(map, key);
    checkStringHashMapSetUpdated(copy1, key);

    Col_Word copy2 = Col_CopyHashMap(map);
    Col_Word copy3 = Col_CopyHashMap(copy1);

    checkStringHashMapSetUpdated(copy2, key);
    checkStringHashMapSetUpdated(copy3, key);
}
PICOTEST_CASE(testStringHashMapSetUpdatedCopy, colibriFixture) {
    checkStringHashMapSetUpdatedCopy(STRING_HASH_MAP(1), STRING_HASH_KEY(0));
    checkStringHashMapSetUpdatedCopy(STRING_HASH_MAP(10), STRING_HASH_KEY(0));
    checkStringHashMapSetUpdatedCopy(STRING_HASH_MAP(10), STRING_HASH_KEY(5));
    checkStringHashMapSetUpdatedCopy(STRING_HASH_MAP(10), STRING_HASH_KEY(9));
    checkStringHashMapSetUpdatedCopy(STRING_HASH_MAP(100), STRING_HASH_KEY(0));
    checkStringHashMapSetUpdatedCopy(STRING_HASH_MAP(100), STRING_HASH_KEY(33));
    checkStringHashMapSetUpdatedCopy(STRING_HASH_MAP(100), STRING_HASH_KEY(66));
    checkStringHashMapSetUpdatedCopy(STRING_HASH_MAP(100), STRING_HASH_KEY(99));
    checkStringHashMapSetUpdatedCopy(STRING_HASH_MAP_COLLISION(10),
                                     STRING_HASH_KEY_COLLISION(0));
    checkStringHashMapSetUpdatedCopy(STRING_HASH_MAP_COLLISION(10),
                                     STRING_HASH_KEY_COLLISION(5));
    checkStringHashMapSetUpdatedCopy(STRING_HASH_MAP_COLLISION(10),
                                     STRING_HASH_KEY_COLLISION(9));
    checkStringHashMapSetUpdatedCopy(STRING_HASH_MAP_COLLISION(100),
                                     STRING_HASH_KEY_COLLISION(0));
    checkStringHashMapSetUpdatedCopy(STRING_HASH_MAP_COLLISION(100),
                                     STRING_HASH_KEY_COLLISION(33));
    checkStringHashMapSetUpdatedCopy(STRING_HASH_MAP_COLLISION(100),
                                     STRING_HASH_KEY_COLLISION(66));
    checkStringHashMapSetUpdatedCopy(STRING_HASH_MAP_COLLISION(100),
                                     STRING_HASH_KEY_COLLISION(99));
}

PICOTEST_CASE(testStringHashMapSetGrowBuckets, colibriFixture) {
    Col_Word map = Col_NewStringHashMap(0);
    for (int i = 0; i < 100; i++) {
        Col_Word key = STRING_HASH_KEY(i);
        Col_Word value = STRING_HASH_VALUE(i);
        PICOTEST_ASSERT(Col_MapSize(map) == i);
        PICOTEST_ASSERT(Col_HashMapSet(map, key, value) == 1);
        PICOTEST_ASSERT(Col_MapSize(map) == i + 1);
        if (i == 10)
            Col_CopyHashMap(map); // Freeze internal structures
    }
    PICOTEST_ASSERT(Col_MapSize(map) == 100);
    for (int i = 0; i < 100; i++) {
        Col_Word key = STRING_HASH_KEY(i);
        Col_Word value;
        PICOTEST_ASSERT(Col_HashMapGet(map, key, &value) == 1);
        PICOTEST_ASSERT(value == STRING_HASH_VALUE(i));
    }
}
PICOTEST_CASE(testStringHashMapSetCollisions, colibriFixture) {
    Col_Word map = Col_NewStringHashMap(0);
    for (int i = 0; i < 100; i++) {
        Col_Word key = STRING_HASH_KEY_COLLISION(i);
        Col_Word value = STRING_HASH_VALUE(i);
        PICOTEST_ASSERT(Col_MapSize(map) == i);
        PICOTEST_ASSERT(Col_HashMapSet(map, key, value) == 1);
        PICOTEST_ASSERT(Col_MapSize(map) == i + 1);
        if (i == 10)
            Col_CopyHashMap(map); // Freeze internal structures
    }
    PICOTEST_ASSERT(Col_MapSize(map) == 100);
    for (int i = 0; i < 100; i++) {
        Col_Word key = STRING_HASH_KEY_COLLISION(i);
        Col_Word value;
        PICOTEST_ASSERT(Col_HashMapGet(map, key, &value) == 1);
        PICOTEST_ASSERT(value == STRING_HASH_VALUE(i));
    }
}

PICOTEST_SUITE(testStringHashMapUnset, testStringHashMapUnsetEmpty,
               testStringHashMapUnsetNotFound, testStringHashMapUnsetFound,
               testStringHashMapUnsetFoundCopy);
PICOTEST_CASE(testStringHashMapUnsetEmpty, colibriFixture) {
    Col_Word map = Col_NewStringHashMap(0);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    PICOTEST_ASSERT(Col_HashMapUnset(map, key) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
}
PICOTEST_CASE(testStringHashMapUnsetNotFound, colibriFixture) {
    Col_Word map = Col_NewStringHashMap(0);
    Col_Word key = Col_NewCharWord('a');
    Col_HashMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    PICOTEST_ASSERT(Col_HashMapUnset(map, Col_NewCharWord('b')) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value;
    PICOTEST_ASSERT(Col_HashMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}

static void checkStringHashMapUnsetFound(Col_Word map, Col_Word key) {
    size_t size = Col_MapSize(map);
    PICOTEST_ASSERT(Col_HashMapUnset(map, key) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == size - 1);
    Col_Word value;
    PICOTEST_ASSERT(Col_HashMapGet(map, key, &value) == 0);
}
PICOTEST_CASE(testStringHashMapUnsetFound, colibriFixture) {
    checkStringHashMapUnsetFound(STRING_HASH_MAP(1), STRING_HASH_KEY(0));
    checkStringHashMapUnsetFound(STRING_HASH_MAP(10), STRING_HASH_KEY(0));
    checkStringHashMapUnsetFound(STRING_HASH_MAP(10), STRING_HASH_KEY(5));
    checkStringHashMapUnsetFound(STRING_HASH_MAP(10), STRING_HASH_KEY(9));
    checkStringHashMapUnsetFound(STRING_HASH_MAP(100), STRING_HASH_KEY(0));
    checkStringHashMapUnsetFound(STRING_HASH_MAP(100), STRING_HASH_KEY(33));
    checkStringHashMapUnsetFound(STRING_HASH_MAP(100), STRING_HASH_KEY(66));
    checkStringHashMapUnsetFound(STRING_HASH_MAP(100), STRING_HASH_KEY(99));
    checkStringHashMapUnsetFound(STRING_HASH_MAP_COLLISION(10),
                                 STRING_HASH_KEY_COLLISION(0));
    checkStringHashMapUnsetFound(STRING_HASH_MAP_COLLISION(10),
                                 STRING_HASH_KEY_COLLISION(5));
    checkStringHashMapUnsetFound(STRING_HASH_MAP_COLLISION(10),
                                 STRING_HASH_KEY_COLLISION(9));
    checkStringHashMapUnsetFound(STRING_HASH_MAP_COLLISION(100),
                                 STRING_HASH_KEY_COLLISION(0));
    checkStringHashMapUnsetFound(STRING_HASH_MAP_COLLISION(100),
                                 STRING_HASH_KEY_COLLISION(33));
    checkStringHashMapUnsetFound(STRING_HASH_MAP_COLLISION(100),
                                 STRING_HASH_KEY_COLLISION(66));
    checkStringHashMapUnsetFound(STRING_HASH_MAP_COLLISION(100),
                                 STRING_HASH_KEY_COLLISION(99));
}

static void checkStringHashMapUnsetFoundCopy(Col_Word map, Col_Word key) {
    Col_Word copy1 = Col_CopyHashMap(map);

    Col_Word copy2 = Col_CopyHashMap(map);
    checkStringHashMapUnsetFound(map, key);

    Col_Word copy3 = Col_CopyHashMap(copy1);
    checkStringHashMapUnsetFound(copy1, key);
    checkStringHashMapUnsetFound(copy2, key);
    checkStringHashMapUnsetFound(copy3, key);
}
PICOTEST_CASE(testStringHashMapUnsetFoundCopy, colibriFixture) {
    checkStringHashMapUnsetFoundCopy(STRING_HASH_MAP(1), STRING_HASH_KEY(0));
    checkStringHashMapUnsetFoundCopy(STRING_HASH_MAP(10), STRING_HASH_KEY(0));
    checkStringHashMapUnsetFoundCopy(STRING_HASH_MAP(10), STRING_HASH_KEY(5));
    checkStringHashMapUnsetFoundCopy(STRING_HASH_MAP(10), STRING_HASH_KEY(9));
    checkStringHashMapUnsetFoundCopy(STRING_HASH_MAP(100), STRING_HASH_KEY(0));
    checkStringHashMapUnsetFoundCopy(STRING_HASH_MAP(100), STRING_HASH_KEY(33));
    checkStringHashMapUnsetFoundCopy(STRING_HASH_MAP(100), STRING_HASH_KEY(66));
    checkStringHashMapUnsetFoundCopy(STRING_HASH_MAP(100), STRING_HASH_KEY(99));
    checkStringHashMapUnsetFoundCopy(STRING_HASH_MAP_COLLISION(10),
                                     STRING_HASH_KEY_COLLISION(0));
    checkStringHashMapUnsetFoundCopy(STRING_HASH_MAP_COLLISION(10),
                                     STRING_HASH_KEY_COLLISION(5));
    checkStringHashMapUnsetFoundCopy(STRING_HASH_MAP_COLLISION(10),
                                     STRING_HASH_KEY_COLLISION(9));
    checkStringHashMapUnsetFoundCopy(STRING_HASH_MAP_COLLISION(100),
                                     STRING_HASH_KEY_COLLISION(0));
    checkStringHashMapUnsetFoundCopy(STRING_HASH_MAP_COLLISION(100),
                                     STRING_HASH_KEY_COLLISION(33));
    checkStringHashMapUnsetFoundCopy(STRING_HASH_MAP_COLLISION(100),
                                     STRING_HASH_KEY_COLLISION(66));
    checkStringHashMapUnsetFoundCopy(STRING_HASH_MAP_COLLISION(100),
                                     STRING_HASH_KEY_COLLISION(99));
}

PICOTEST_SUITE(testIntegerHashMapAccessors, testIntHashMapGet,
               testIntHashMapSet, testIntHashMapUnset);

PICOTEST_SUITE(testIntHashMapGet, testIntHashMapGetEmpty,
               testIntHashMapGetFound, testIntHashMapGetNotFound);
PICOTEST_CASE(testIntHashMapGetEmpty, colibriFixture) {
    Col_Word map = Col_NewIntHashMap(0);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    int key = 1;
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntHashMapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}
PICOTEST_CASE(testIntHashMapGetFound, colibriFixture) {
    Col_Word map = Col_NewIntHashMap(0);
    int key = 1;
    Col_IntHashMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntHashMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testIntHashMapGetNotFound, colibriFixture) {
    Col_Word map = Col_NewIntHashMap(0);
    int key = 1;
    Col_IntHashMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntHashMapGet(map, 2, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}

PICOTEST_SUITE(testIntHashMapSet, testIntHashMapSetEmpty,
               testIntHashMapSetCreated, testIntHashMapSetUpdated,
               testIntHashMapSetUpdatedCopy, testIntHashMapSetGrowBuckets,
               testIntHashMapSetCollisions);
PICOTEST_CASE(testIntHashMapSetEmpty, colibriFixture) {
    Col_Word map = Col_NewIntHashMap(0);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    int key = 1;
    PICOTEST_ASSERT(Col_IntHashMapSet(map, key, WORD_TRUE) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
}
PICOTEST_CASE(testIntHashMapSetCreated, colibriFixture) {
    Col_Word map = Col_NewIntHashMap(0);
    int key = 1;
    Col_IntHashMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    PICOTEST_ASSERT(Col_IntHashMapSet(map, 2, WORD_FALSE) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == 2);
}

static void checkIntHashMapSetUpdated(Col_Word map, intptr_t key) {
    size_t size = Col_MapSize(map);
    Col_IntHashMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_IntHashMapSet(map, key, WORD_FALSE) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == size);
}
PICOTEST_CASE(testIntHashMapSetUpdated, colibriFixture) {
    checkIntHashMapSetUpdated(INT_HASH_MAP(1), INT_HASH_KEY(0));
    checkIntHashMapSetUpdated(INT_HASH_MAP(10), INT_HASH_KEY(0));
    checkIntHashMapSetUpdated(INT_HASH_MAP(10), INT_HASH_KEY(5));
    checkIntHashMapSetUpdated(INT_HASH_MAP(10), INT_HASH_KEY(9));
    checkIntHashMapSetUpdated(INT_HASH_MAP(100), INT_HASH_KEY(0));
    checkIntHashMapSetUpdated(INT_HASH_MAP(100), INT_HASH_KEY(33));
    checkIntHashMapSetUpdated(INT_HASH_MAP(100), INT_HASH_KEY(66));
    checkIntHashMapSetUpdated(INT_HASH_MAP(100), INT_HASH_KEY(99));
    checkIntHashMapSetUpdated(INT_HASH_MAP_COLLISION(10),
                              INT_HASH_KEY_COLLISION(0));
    checkIntHashMapSetUpdated(INT_HASH_MAP_COLLISION(10),
                              INT_HASH_KEY_COLLISION(5));
    checkIntHashMapSetUpdated(INT_HASH_MAP_COLLISION(10),
                              INT_HASH_KEY_COLLISION(9));
    checkIntHashMapSetUpdated(INT_HASH_MAP_COLLISION(100),
                              INT_HASH_KEY_COLLISION(0));
    checkIntHashMapSetUpdated(INT_HASH_MAP_COLLISION(100),
                              INT_HASH_KEY_COLLISION(33));
    checkIntHashMapSetUpdated(INT_HASH_MAP_COLLISION(100),
                              INT_HASH_KEY_COLLISION(66));
    checkIntHashMapSetUpdated(INT_HASH_MAP_COLLISION(100),
                              INT_HASH_KEY_COLLISION(99));
}

static void checkIntHashMapSetUpdatedCopy(Col_Word map, intptr_t key) {
    Col_Word copy1 = Col_CopyHashMap(map);

    checkIntHashMapSetUpdated(map, key);
    checkIntHashMapSetUpdated(copy1, key);

    Col_Word copy2 = Col_CopyHashMap(map);
    Col_Word copy3 = Col_CopyHashMap(copy1);

    checkIntHashMapSetUpdated(copy2, key);
    checkIntHashMapSetUpdated(copy3, key);
}
PICOTEST_CASE(testIntHashMapSetUpdatedCopy, colibriFixture) {
    checkIntHashMapSetUpdatedCopy(INT_HASH_MAP(1), INT_HASH_KEY(0));
    checkIntHashMapSetUpdatedCopy(INT_HASH_MAP(10), INT_HASH_KEY(0));
    checkIntHashMapSetUpdatedCopy(INT_HASH_MAP(10), INT_HASH_KEY(5));
    checkIntHashMapSetUpdatedCopy(INT_HASH_MAP(10), INT_HASH_KEY(9));
    checkIntHashMapSetUpdatedCopy(INT_HASH_MAP(100), INT_HASH_KEY(0));
    checkIntHashMapSetUpdatedCopy(INT_HASH_MAP(100), INT_HASH_KEY(33));
    checkIntHashMapSetUpdatedCopy(INT_HASH_MAP(100), INT_HASH_KEY(66));
    checkIntHashMapSetUpdatedCopy(INT_HASH_MAP(100), INT_HASH_KEY(99));
    checkIntHashMapSetUpdatedCopy(INT_HASH_MAP_COLLISION(10),
                                  INT_HASH_KEY_COLLISION(0));
    checkIntHashMapSetUpdatedCopy(INT_HASH_MAP_COLLISION(10),
                                  INT_HASH_KEY_COLLISION(5));
    checkIntHashMapSetUpdatedCopy(INT_HASH_MAP_COLLISION(10),
                                  INT_HASH_KEY_COLLISION(9));
    checkIntHashMapSetUpdatedCopy(INT_HASH_MAP_COLLISION(100),
                                  INT_HASH_KEY_COLLISION(0));
    checkIntHashMapSetUpdatedCopy(INT_HASH_MAP_COLLISION(100),
                                  INT_HASH_KEY_COLLISION(33));
    checkIntHashMapSetUpdatedCopy(INT_HASH_MAP_COLLISION(100),
                                  INT_HASH_KEY_COLLISION(66));
    checkIntHashMapSetUpdatedCopy(INT_HASH_MAP_COLLISION(100),
                                  INT_HASH_KEY_COLLISION(99));
}

PICOTEST_CASE(testIntHashMapSetGrowBuckets, colibriFixture) {
    Col_Word map = Col_NewIntHashMap(0);
    for (int i = 0; i < 100; i++) {
        int key = INT_HASH_KEY(i);
        Col_Word value = INT_HASH_VALUE(i);
        PICOTEST_ASSERT(Col_MapSize(map) == i);
        PICOTEST_ASSERT(Col_IntHashMapSet(map, key, value) == 1);
        PICOTEST_ASSERT(Col_MapSize(map) == i + 1);
        if (i == 10)
            Col_CopyHashMap(map); // Freeze internal structures
    }
    PICOTEST_ASSERT(Col_MapSize(map) == 100);
    for (int i = 0; i < 100; i++) {
        int key = INT_HASH_KEY(i);
        Col_Word value;
        PICOTEST_ASSERT(Col_IntHashMapGet(map, key, &value) == 1);
        PICOTEST_ASSERT(value == INT_HASH_VALUE(i));
    }
}
PICOTEST_CASE(testIntHashMapSetCollisions, colibriFixture) {
    Col_Word map = Col_NewIntHashMap(0);
    for (int i = 0; i < 100; i++) {
        int key = INT_HASH_KEY_COLLISION(i);
        Col_Word value = INT_HASH_VALUE(i);
        PICOTEST_ASSERT(Col_MapSize(map) == i);
        PICOTEST_ASSERT(Col_IntHashMapSet(map, key, value) == 1);
        PICOTEST_ASSERT(Col_MapSize(map) == i + 1);
        if (i == 10)
            Col_CopyHashMap(map); // Freeze internal structures
    }
    PICOTEST_ASSERT(Col_MapSize(map) == 100);
    for (int i = 0; i < 100; i++) {
        int key = INT_HASH_KEY_COLLISION(i);
        Col_Word value;
        PICOTEST_ASSERT(Col_IntHashMapGet(map, key, &value) == 1);
        PICOTEST_ASSERT(value == INT_HASH_VALUE(i));
    }
}

PICOTEST_SUITE(testIntHashMapUnset, testIntHashMapUnsetEmpty,
               testIntHashMapUnsetNotFound, testIntHashMapUnsetFound,
               testIntHashMapUnsetFoundCopy);
PICOTEST_CASE(testIntHashMapUnsetEmpty, colibriFixture) {
    Col_Word map = Col_NewIntHashMap(0);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    int key = 1;
    PICOTEST_ASSERT(Col_IntHashMapUnset(map, key) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
}
PICOTEST_CASE(testIntHashMapUnsetNotFound, colibriFixture) {
    Col_Word map = Col_NewIntHashMap(0);
    int key = 1;
    Col_IntHashMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    PICOTEST_ASSERT(Col_IntHashMapUnset(map, 2) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntHashMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}

static void checkIntHashMapUnsetFound(Col_Word map, intptr_t key) {
    size_t size = Col_MapSize(map);
    PICOTEST_ASSERT(Col_IntHashMapUnset(map, key) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == size - 1);
    Col_Word value;
    PICOTEST_ASSERT(Col_IntHashMapGet(map, key, &value) == 0);
}
PICOTEST_CASE(testIntHashMapUnsetFound, colibriFixture) {
    checkIntHashMapUnsetFound(INT_HASH_MAP(1), INT_HASH_KEY(0));
    checkIntHashMapUnsetFound(INT_HASH_MAP(10), INT_HASH_KEY(0));
    checkIntHashMapUnsetFound(INT_HASH_MAP(10), INT_HASH_KEY(5));
    checkIntHashMapUnsetFound(INT_HASH_MAP(10), INT_HASH_KEY(9));
    checkIntHashMapUnsetFound(INT_HASH_MAP(100), INT_HASH_KEY(0));
    checkIntHashMapUnsetFound(INT_HASH_MAP(100), INT_HASH_KEY(33));
    checkIntHashMapUnsetFound(INT_HASH_MAP(100), INT_HASH_KEY(66));
    checkIntHashMapUnsetFound(INT_HASH_MAP(100), INT_HASH_KEY(99));
    checkIntHashMapUnsetFound(INT_HASH_MAP_COLLISION(10),
                              INT_HASH_KEY_COLLISION(0));
    checkIntHashMapUnsetFound(INT_HASH_MAP_COLLISION(10),
                              INT_HASH_KEY_COLLISION(5));
    checkIntHashMapUnsetFound(INT_HASH_MAP_COLLISION(10),
                              INT_HASH_KEY_COLLISION(9));
    checkIntHashMapUnsetFound(INT_HASH_MAP_COLLISION(100),
                              INT_HASH_KEY_COLLISION(0));
    checkIntHashMapUnsetFound(INT_HASH_MAP_COLLISION(100),
                              INT_HASH_KEY_COLLISION(33));
    checkIntHashMapUnsetFound(INT_HASH_MAP_COLLISION(100),
                              INT_HASH_KEY_COLLISION(66));
    checkIntHashMapUnsetFound(INT_HASH_MAP_COLLISION(100),
                              INT_HASH_KEY_COLLISION(99));
}

static void checkIntHashMapUnsetFoundCopy(Col_Word map, intptr_t key) {
    Col_Word copy1 = Col_CopyHashMap(map);

    Col_Word copy2 = Col_CopyHashMap(map);
    checkIntHashMapUnsetFound(map, key);

    Col_Word copy3 = Col_CopyHashMap(copy1);
    checkIntHashMapUnsetFound(copy1, key);
    checkIntHashMapUnsetFound(copy2, key);
    checkIntHashMapUnsetFound(copy3, key);
}
PICOTEST_CASE(testIntHashMapUnsetFoundCopy, colibriFixture) {
    checkIntHashMapUnsetFoundCopy(INT_HASH_MAP(1), INT_HASH_KEY(0));
    checkIntHashMapUnsetFoundCopy(INT_HASH_MAP(10), INT_HASH_KEY(0));
    checkIntHashMapUnsetFoundCopy(INT_HASH_MAP(10), INT_HASH_KEY(5));
    checkIntHashMapUnsetFoundCopy(INT_HASH_MAP(10), INT_HASH_KEY(9));
    checkIntHashMapUnsetFoundCopy(INT_HASH_MAP(100), INT_HASH_KEY(0));
    checkIntHashMapUnsetFoundCopy(INT_HASH_MAP(100), INT_HASH_KEY(33));
    checkIntHashMapUnsetFoundCopy(INT_HASH_MAP(100), INT_HASH_KEY(66));
    checkIntHashMapUnsetFoundCopy(INT_HASH_MAP(100), INT_HASH_KEY(99));
    checkIntHashMapUnsetFoundCopy(INT_HASH_MAP_COLLISION(10),
                                  INT_HASH_KEY_COLLISION(0));
    checkIntHashMapUnsetFoundCopy(INT_HASH_MAP_COLLISION(10),
                                  INT_HASH_KEY_COLLISION(5));
    checkIntHashMapUnsetFoundCopy(INT_HASH_MAP_COLLISION(10),
                                  INT_HASH_KEY_COLLISION(9));
    checkIntHashMapUnsetFoundCopy(INT_HASH_MAP_COLLISION(100),
                                  INT_HASH_KEY_COLLISION(0));
    checkIntHashMapUnsetFoundCopy(INT_HASH_MAP_COLLISION(100),
                                  INT_HASH_KEY_COLLISION(33));
    checkIntHashMapUnsetFoundCopy(INT_HASH_MAP_COLLISION(100),
                                  INT_HASH_KEY_COLLISION(66));
    checkIntHashMapUnsetFoundCopy(INT_HASH_MAP_COLLISION(100),
                                  INT_HASH_KEY_COLLISION(99));
}

PICOTEST_SUITE(testHashMapIteration, testHashMapIteratorErrors,
               testHashMapIteratorInitialize, testHashMapIteratorAccess,
               testHashMapIteratorMove, testHashMapIteratorEmptyMap);

PICOTEST_SUITE(testHashMapIteratorErrors, testHashMapIteratorAtEndIsInvalid);
PICOTEST_CASE(testHashMapIteratorAtEndIsInvalid, colibriFixture) {
    PICOTEST_VERIFY(hashMapIterSetValue_valueCheck(NULL) == 1);
    PICOTEST_VERIFY(hashMapIterNext_valueCheck(NULL) == 1);
}

PICOTEST_SUITE(testHashMapIteratorInitialize, testHashMapIterBegin,
               testHashMapIterFind, testIntHashMapIterFind);

static void checkHashMapIterBegin(Col_Word map) {
    Col_MapIterator it;
    Col_HashMapIterBegin(it, map);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
}
PICOTEST_CASE(testHashMapIterBegin, colibriFixture) {
    checkHashMapIterBegin(STRING_HASH_MAP(1));
    checkHashMapIterBegin(STRING_HASH_MAP(10));
    checkHashMapIterBegin(STRING_HASH_MAP(100));
    checkHashMapIterBegin(STRING_HASH_MAP_COLLISION(10));
    checkHashMapIterBegin(STRING_HASH_MAP_COLLISION(100));
    checkHashMapIterBegin(INT_HASH_MAP(1));
    checkHashMapIterBegin(INT_HASH_MAP(10));
    checkHashMapIterBegin(INT_HASH_MAP(100));
    checkHashMapIterBegin(INT_HASH_MAP_COLLISION(10));
    checkHashMapIterBegin(INT_HASH_MAP_COLLISION(100));
}

PICOTEST_SUITE(testHashMapIterFind, testHashMapIterFindFound,
               testHashMapIterFindNotFound, testHashMapIterFindCreate);

static void checkHashMapIterFindFound(Col_Word map, Col_Word key) {
    size_t size = Col_MapSize(map);
    Col_MapIterator it;

    Col_HashMapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(Col_MapSize(map) == size);

    int create = 0;
    Col_HashMapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(!create);
    PICOTEST_ASSERT(Col_MapSize(map) == size);
}
PICOTEST_CASE(testHashMapIterFindFound, colibriFixture) {
    checkHashMapIterFindFound(STRING_HASH_MAP(1), STRING_HASH_KEY(0));
    checkHashMapIterFindFound(STRING_HASH_MAP(10), STRING_HASH_KEY(0));
    checkHashMapIterFindFound(STRING_HASH_MAP(10), STRING_HASH_KEY(5));
    checkHashMapIterFindFound(STRING_HASH_MAP(10), STRING_HASH_KEY(9));
    checkHashMapIterFindFound(STRING_HASH_MAP(100), STRING_HASH_KEY(0));
    checkHashMapIterFindFound(STRING_HASH_MAP(100), STRING_HASH_KEY(33));
    checkHashMapIterFindFound(STRING_HASH_MAP(100), STRING_HASH_KEY(66));
    checkHashMapIterFindFound(STRING_HASH_MAP(100), STRING_HASH_KEY(99));
    checkHashMapIterFindFound(STRING_HASH_MAP_COLLISION(10),
                              STRING_HASH_KEY_COLLISION(0));
    checkHashMapIterFindFound(STRING_HASH_MAP_COLLISION(10),
                              STRING_HASH_KEY_COLLISION(5));
    checkHashMapIterFindFound(STRING_HASH_MAP_COLLISION(10),
                              STRING_HASH_KEY_COLLISION(9));
    checkHashMapIterFindFound(STRING_HASH_MAP_COLLISION(100),
                              STRING_HASH_KEY_COLLISION(0));
    checkHashMapIterFindFound(STRING_HASH_MAP_COLLISION(100),
                              STRING_HASH_KEY_COLLISION(33));
    checkHashMapIterFindFound(STRING_HASH_MAP_COLLISION(100),
                              STRING_HASH_KEY_COLLISION(66));
    checkHashMapIterFindFound(STRING_HASH_MAP_COLLISION(100),
                              STRING_HASH_KEY_COLLISION(99));
}

static void checkHashMapIterFindNotFound(Col_Word map, Col_Word key) {
    size_t size = Col_MapSize(map);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_HashMapGet(map, key, &value) == 0);

    Col_MapIterator it;

    Col_HashMapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    PICOTEST_ASSERT(Col_MapSize(map) == size);
    PICOTEST_ASSERT(Col_HashMapGet(map, key, &value) == 0);

    int create = 0;
    Col_HashMapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    PICOTEST_ASSERT(!create);
    PICOTEST_ASSERT(Col_MapSize(map) == size);
    PICOTEST_ASSERT(Col_HashMapGet(map, key, &value) == 0);
}
PICOTEST_CASE(testHashMapIterFindNotFound, colibriFixture) {
    checkHashMapIterFindNotFound(STRING_HASH_MAP(1), STRING_HASH_KEY(1));
    checkHashMapIterFindNotFound(STRING_HASH_MAP(10), STRING_HASH_KEY(10));
    checkHashMapIterFindNotFound(STRING_HASH_MAP(100), STRING_HASH_KEY(100));
    checkHashMapIterFindNotFound(STRING_HASH_MAP_COLLISION(10),
                                 STRING_HASH_KEY_COLLISION(10));
    checkHashMapIterFindNotFound(STRING_HASH_MAP_COLLISION(100),
                                 STRING_HASH_KEY_COLLISION(100));
}

static void checkHashMapIterFindCreate(Col_Word map, Col_Word key) {
    size_t size = Col_MapSize(map);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_HashMapGet(map, key, &value) == 0);

    Col_MapIterator it;
    int create = 1;
    Col_HashMapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(create);
    PICOTEST_ASSERT(Col_MapSize(map) == size + 1);
    PICOTEST_ASSERT(Col_HashMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_NIL);
}
PICOTEST_CASE(testHashMapIterFindCreate, colibriFixture) {
    checkHashMapIterFindCreate(STRING_HASH_MAP(1), STRING_HASH_KEY(1));
    checkHashMapIterFindCreate(STRING_HASH_MAP(10), STRING_HASH_KEY(10));
    checkHashMapIterFindCreate(STRING_HASH_MAP(100), STRING_HASH_KEY(100));
    checkHashMapIterFindCreate(STRING_HASH_MAP_COLLISION(10),
                               STRING_HASH_KEY_COLLISION(10));
    checkHashMapIterFindCreate(STRING_HASH_MAP_COLLISION(100),
                               STRING_HASH_KEY_COLLISION(100));
}

PICOTEST_SUITE(testIntHashMapIterFind, testIntHashMapIterFindFound,
               testIntHashMapIterFindNotFound, testIntHashMapIterFindCreate);

static void checkIntHashMapIterFindFound(Col_Word map, intptr_t key) {
    size_t size = Col_MapSize(map);
    Col_MapIterator it;

    Col_IntHashMapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(Col_MapSize(map) == size);

    int create = 0;
    Col_IntHashMapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(!create);
    PICOTEST_ASSERT(Col_MapSize(map) == size);
}
PICOTEST_CASE(testIntHashMapIterFindFound, colibriFixture) {
    checkIntHashMapIterFindFound(INT_HASH_MAP(1), INT_HASH_KEY(0));
    checkIntHashMapIterFindFound(INT_HASH_MAP(10), INT_HASH_KEY(0));
    checkIntHashMapIterFindFound(INT_HASH_MAP(10), INT_HASH_KEY(5));
    checkIntHashMapIterFindFound(INT_HASH_MAP(10), INT_HASH_KEY(9));
    checkIntHashMapIterFindFound(INT_HASH_MAP(100), INT_HASH_KEY(0));
    checkIntHashMapIterFindFound(INT_HASH_MAP(100), INT_HASH_KEY(33));
    checkIntHashMapIterFindFound(INT_HASH_MAP(100), INT_HASH_KEY(66));
    checkIntHashMapIterFindFound(INT_HASH_MAP(100), INT_HASH_KEY(99));
    checkIntHashMapIterFindFound(INT_HASH_MAP_COLLISION(10),
                                 INT_HASH_KEY_COLLISION(0));
    checkIntHashMapIterFindFound(INT_HASH_MAP_COLLISION(10),
                                 INT_HASH_KEY_COLLISION(5));
    checkIntHashMapIterFindFound(INT_HASH_MAP_COLLISION(10),
                                 INT_HASH_KEY_COLLISION(9));
    checkIntHashMapIterFindFound(INT_HASH_MAP_COLLISION(100),
                                 INT_HASH_KEY_COLLISION(0));
    checkIntHashMapIterFindFound(INT_HASH_MAP_COLLISION(100),
                                 INT_HASH_KEY_COLLISION(33));
    checkIntHashMapIterFindFound(INT_HASH_MAP_COLLISION(100),
                                 INT_HASH_KEY_COLLISION(66));
    checkIntHashMapIterFindFound(INT_HASH_MAP_COLLISION(100),
                                 INT_HASH_KEY_COLLISION(99));
}

static void checkIntHashMapIterFindNotFound(Col_Word map, intptr_t key) {
    size_t size = Col_MapSize(map);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntHashMapGet(map, key, &value) == 0);

    Col_MapIterator it;

    Col_IntHashMapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    PICOTEST_ASSERT(Col_MapSize(map) == size);
    PICOTEST_ASSERT(Col_IntHashMapGet(map, key, &value) == 0);

    int create = 0;
    Col_IntHashMapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    PICOTEST_ASSERT(!create);
    PICOTEST_ASSERT(Col_MapSize(map) == size);
    PICOTEST_ASSERT(Col_IntHashMapGet(map, key, &value) == 0);
}
PICOTEST_CASE(testIntHashMapIterFindNotFound, colibriFixture) {
    checkIntHashMapIterFindNotFound(INT_HASH_MAP(1), INT_HASH_KEY(1));
    checkIntHashMapIterFindNotFound(INT_HASH_MAP(10), INT_HASH_KEY(10));
    checkIntHashMapIterFindNotFound(INT_HASH_MAP(100), INT_HASH_KEY(100));
    checkIntHashMapIterFindNotFound(INT_HASH_MAP_COLLISION(10),
                                    INT_HASH_KEY_COLLISION(10));
    checkIntHashMapIterFindNotFound(INT_HASH_MAP_COLLISION(100),
                                    INT_HASH_KEY_COLLISION(100));
}

static void checkIntHashMapIterFindCreate(Col_Word map, intptr_t key) {
    size_t size = Col_MapSize(map);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntHashMapGet(map, key, &value) == 0);

    Col_MapIterator it;
    int create = 1;
    Col_IntHashMapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(create);
    PICOTEST_ASSERT(Col_IntHashMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_NIL);
}
PICOTEST_CASE(testIntHashMapIterFindCreate, colibriFixture) {
    checkIntHashMapIterFindCreate(INT_HASH_MAP(1), INT_HASH_KEY(1));
    checkIntHashMapIterFindCreate(INT_HASH_MAP(10), INT_HASH_KEY(10));
    checkIntHashMapIterFindCreate(INT_HASH_MAP(100), INT_HASH_KEY(100));
    checkIntHashMapIterFindCreate(INT_HASH_MAP_COLLISION(10),
                                  INT_HASH_KEY_COLLISION(10));
    checkIntHashMapIterFindCreate(INT_HASH_MAP_COLLISION(100),
                                  INT_HASH_KEY_COLLISION(100));
}

PICOTEST_SUITE(testHashMapIteratorAccess, testHashMapIterSetValue,
               testHashMapIterSetValueCopy, testIntHashMapIterSetValue,
               testIntHashMapIterSetValueCopy);

static void checkHashMapIterSetValue(Col_Word map, Col_Word key,
                                     Col_Word oldValue) {
    size_t size = Col_MapSize(map);
    Col_Word value;
    PICOTEST_ASSERT(Col_HashMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == oldValue);

    Col_MapIterator it;
    Col_HashMapIterFind(it, map, key, NULL);
    Col_HashMapIterSetValue(it, WORD_TRUE);
    PICOTEST_ASSERT(Col_HashMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testHashMapIterSetValue, colibriFixture) {
    checkHashMapIterSetValue(STRING_HASH_MAP(10), STRING_HASH_KEY(0),
                             STRING_HASH_VALUE(0));
    checkHashMapIterSetValue(STRING_HASH_MAP(10), STRING_HASH_KEY(5),
                             STRING_HASH_VALUE(5));
    checkHashMapIterSetValue(STRING_HASH_MAP(10), STRING_HASH_KEY(9),
                             STRING_HASH_VALUE(9));
    checkHashMapIterSetValue(STRING_HASH_MAP(100), STRING_HASH_KEY(0),
                             STRING_HASH_VALUE(0));
    checkHashMapIterSetValue(STRING_HASH_MAP(100), STRING_HASH_KEY(33),
                             STRING_HASH_VALUE(33));
    checkHashMapIterSetValue(STRING_HASH_MAP(100), STRING_HASH_KEY(66),
                             STRING_HASH_VALUE(66));
    checkHashMapIterSetValue(STRING_HASH_MAP(100), STRING_HASH_KEY(99),
                             STRING_HASH_VALUE(99));
    checkHashMapIterSetValue(STRING_HASH_MAP_COLLISION(10),
                             STRING_HASH_KEY_COLLISION(0),
                             STRING_HASH_VALUE(0));
    checkHashMapIterSetValue(STRING_HASH_MAP_COLLISION(10),
                             STRING_HASH_KEY_COLLISION(5),
                             STRING_HASH_VALUE(5));
    checkHashMapIterSetValue(STRING_HASH_MAP_COLLISION(10),
                             STRING_HASH_KEY_COLLISION(9),
                             STRING_HASH_VALUE(9));
    checkHashMapIterSetValue(STRING_HASH_MAP_COLLISION(100),
                             STRING_HASH_KEY_COLLISION(0),
                             STRING_HASH_VALUE(0));
    checkHashMapIterSetValue(STRING_HASH_MAP_COLLISION(100),
                             STRING_HASH_KEY_COLLISION(33),
                             STRING_HASH_VALUE(33));
    checkHashMapIterSetValue(STRING_HASH_MAP_COLLISION(100),
                             STRING_HASH_KEY_COLLISION(66),
                             STRING_HASH_VALUE(66));
    checkHashMapIterSetValue(STRING_HASH_MAP_COLLISION(100),
                             STRING_HASH_KEY_COLLISION(99),
                             STRING_HASH_VALUE(99));
}

static void checkHashMapIterSetValueCopy(Col_Word map, Col_Word key,
                                         Col_Word oldValue) {
    Col_Word copy1 = Col_CopyHashMap(map);

    Col_Word copy2 = Col_CopyHashMap(map);
    checkHashMapIterSetValue(map, key, oldValue);

    Col_Word copy3 = Col_CopyHashMap(copy1);
    checkHashMapIterSetValue(copy1, key, oldValue);
    checkHashMapIterSetValue(copy2, key, oldValue);
    checkHashMapIterSetValue(copy3, key, oldValue);
}
PICOTEST_CASE(testHashMapIterSetValueCopy, colibriFixture) {
    checkHashMapIterSetValueCopy(STRING_HASH_MAP(10), STRING_HASH_KEY(0),
                                 STRING_HASH_VALUE(0));
    checkHashMapIterSetValueCopy(STRING_HASH_MAP(10), STRING_HASH_KEY(5),
                                 STRING_HASH_VALUE(5));
    checkHashMapIterSetValueCopy(STRING_HASH_MAP(10), STRING_HASH_KEY(9),
                                 STRING_HASH_VALUE(9));
    checkHashMapIterSetValueCopy(STRING_HASH_MAP(100), STRING_HASH_KEY(0),
                                 STRING_HASH_VALUE(0));
    checkHashMapIterSetValueCopy(STRING_HASH_MAP(100), STRING_HASH_KEY(33),
                                 STRING_HASH_VALUE(33));
    checkHashMapIterSetValueCopy(STRING_HASH_MAP(100), STRING_HASH_KEY(66),
                                 STRING_HASH_VALUE(66));
    checkHashMapIterSetValueCopy(STRING_HASH_MAP(100), STRING_HASH_KEY(99),
                                 STRING_HASH_VALUE(99));
    checkHashMapIterSetValueCopy(STRING_HASH_MAP_COLLISION(10),
                                 STRING_HASH_KEY_COLLISION(0),
                                 STRING_HASH_VALUE(0));
    checkHashMapIterSetValueCopy(STRING_HASH_MAP_COLLISION(10),
                                 STRING_HASH_KEY_COLLISION(5),
                                 STRING_HASH_VALUE(5));
    checkHashMapIterSetValueCopy(STRING_HASH_MAP_COLLISION(10),
                                 STRING_HASH_KEY_COLLISION(9),
                                 STRING_HASH_VALUE(9));
    checkHashMapIterSetValueCopy(STRING_HASH_MAP_COLLISION(100),
                                 STRING_HASH_KEY_COLLISION(0),
                                 STRING_HASH_VALUE(0));
    checkHashMapIterSetValueCopy(STRING_HASH_MAP_COLLISION(100),
                                 STRING_HASH_KEY_COLLISION(33),
                                 STRING_HASH_VALUE(33));
    checkHashMapIterSetValueCopy(STRING_HASH_MAP_COLLISION(100),
                                 STRING_HASH_KEY_COLLISION(66),
                                 STRING_HASH_VALUE(66));
    checkHashMapIterSetValueCopy(STRING_HASH_MAP_COLLISION(100),
                                 STRING_HASH_KEY_COLLISION(99),
                                 STRING_HASH_VALUE(99));
}

static void checkIntHashMapIterSetValue(Col_Word map, intptr_t key,
                                        Col_Word oldValue) {
    size_t size = Col_MapSize(map);
    Col_Word value;
    PICOTEST_ASSERT(Col_IntHashMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == oldValue);

    Col_MapIterator it;
    Col_IntHashMapIterFind(it, map, key, NULL);
    Col_HashMapIterSetValue(it, WORD_TRUE);
    PICOTEST_ASSERT(Col_IntHashMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testIntHashMapIterSetValue, colibriFixture) {
    checkIntHashMapIterSetValue(INT_HASH_MAP(10), INT_HASH_KEY(0),
                                INT_HASH_VALUE(0));
    checkIntHashMapIterSetValue(INT_HASH_MAP(10), INT_HASH_KEY(5),
                                INT_HASH_VALUE(5));
    checkIntHashMapIterSetValue(INT_HASH_MAP(10), INT_HASH_KEY(9),
                                INT_HASH_VALUE(9));
    checkIntHashMapIterSetValue(INT_HASH_MAP(100), INT_HASH_KEY(0),
                                INT_HASH_VALUE(0));
    checkIntHashMapIterSetValue(INT_HASH_MAP(100), INT_HASH_KEY(33),
                                INT_HASH_VALUE(33));
    checkIntHashMapIterSetValue(INT_HASH_MAP(100), INT_HASH_KEY(66),
                                INT_HASH_VALUE(66));
    checkIntHashMapIterSetValue(INT_HASH_MAP(100), INT_HASH_KEY(99),
                                INT_HASH_VALUE(99));
    checkIntHashMapIterSetValue(INT_HASH_MAP_COLLISION(10),
                                INT_HASH_KEY_COLLISION(0), INT_HASH_VALUE(0));
    checkIntHashMapIterSetValue(INT_HASH_MAP_COLLISION(10),
                                INT_HASH_KEY_COLLISION(5), INT_HASH_VALUE(5));
    checkIntHashMapIterSetValue(INT_HASH_MAP_COLLISION(10),
                                INT_HASH_KEY_COLLISION(9), INT_HASH_VALUE(9));
    checkIntHashMapIterSetValue(INT_HASH_MAP_COLLISION(100),
                                INT_HASH_KEY_COLLISION(0), INT_HASH_VALUE(0));
    checkIntHashMapIterSetValue(INT_HASH_MAP_COLLISION(100),
                                INT_HASH_KEY_COLLISION(33), INT_HASH_VALUE(33));
    checkIntHashMapIterSetValue(INT_HASH_MAP_COLLISION(100),
                                INT_HASH_KEY_COLLISION(66), INT_HASH_VALUE(66));
    checkIntHashMapIterSetValue(INT_HASH_MAP_COLLISION(100),
                                INT_HASH_KEY_COLLISION(99), INT_HASH_VALUE(99));
}

static void checkIntHashMapIterSetValueCopy(Col_Word map, intptr_t key,
                                            Col_Word oldValue) {
    Col_Word copy1 = Col_CopyHashMap(map);

    Col_Word copy2 = Col_CopyHashMap(map);
    checkIntHashMapIterSetValue(map, key, oldValue);

    Col_Word copy3 = Col_CopyHashMap(copy1);
    checkIntHashMapIterSetValue(copy1, key, oldValue);
    checkIntHashMapIterSetValue(copy2, key, oldValue);
    checkIntHashMapIterSetValue(copy3, key, oldValue);
}
PICOTEST_CASE(testIntHashMapIterSetValueCopy, colibriFixture) {
    checkIntHashMapIterSetValueCopy(INT_HASH_MAP(10), INT_HASH_KEY(0),
                                    INT_HASH_VALUE(0));
    checkIntHashMapIterSetValueCopy(INT_HASH_MAP(10), INT_HASH_KEY(5),
                                    INT_HASH_VALUE(5));
    checkIntHashMapIterSetValueCopy(INT_HASH_MAP(10), INT_HASH_KEY(9),
                                    INT_HASH_VALUE(9));
    checkIntHashMapIterSetValueCopy(INT_HASH_MAP(100), INT_HASH_KEY(0),
                                    INT_HASH_VALUE(0));
    checkIntHashMapIterSetValueCopy(INT_HASH_MAP(100), INT_HASH_KEY(33),
                                    INT_HASH_VALUE(33));
    checkIntHashMapIterSetValueCopy(INT_HASH_MAP(100), INT_HASH_KEY(66),
                                    INT_HASH_VALUE(66));
    checkIntHashMapIterSetValueCopy(INT_HASH_MAP(100), INT_HASH_KEY(99),
                                    INT_HASH_VALUE(99));
    checkIntHashMapIterSetValueCopy(INT_HASH_MAP_COLLISION(10),
                                    INT_HASH_KEY_COLLISION(0),
                                    INT_HASH_VALUE(0));
    checkIntHashMapIterSetValueCopy(INT_HASH_MAP_COLLISION(10),
                                    INT_HASH_KEY_COLLISION(5),
                                    INT_HASH_VALUE(5));
    checkIntHashMapIterSetValueCopy(INT_HASH_MAP_COLLISION(10),
                                    INT_HASH_KEY_COLLISION(9),
                                    INT_HASH_VALUE(9));
    checkIntHashMapIterSetValueCopy(INT_HASH_MAP_COLLISION(100),
                                    INT_HASH_KEY_COLLISION(0),
                                    INT_HASH_VALUE(0));
    checkIntHashMapIterSetValueCopy(INT_HASH_MAP_COLLISION(100),
                                    INT_HASH_KEY_COLLISION(33),
                                    INT_HASH_VALUE(33));
    checkIntHashMapIterSetValueCopy(INT_HASH_MAP_COLLISION(100),
                                    INT_HASH_KEY_COLLISION(66),
                                    INT_HASH_VALUE(66));
    checkIntHashMapIterSetValueCopy(INT_HASH_MAP_COLLISION(100),
                                    INT_HASH_KEY_COLLISION(99),
                                    INT_HASH_VALUE(99));
}

static void checkHashMapIterTraverse(Col_Word map) {
    Col_MapIterator it;
    size_t i;
    for (i = 0, Col_HashMapIterBegin(it, map); !Col_MapIterEnd(it);
         i++, Col_HashMapIterNext(it))
        ;
    PICOTEST_ASSERT(i == Col_MapSize(map));
}
PICOTEST_CASE(testHashMapIteratorMove, colibriFixture) {
    checkHashMapIterTraverse(STRING_HASH_MAP(1));
    checkHashMapIterTraverse(STRING_HASH_MAP(10));
    checkHashMapIterTraverse(STRING_HASH_MAP(100));
    checkHashMapIterTraverse(INT_HASH_MAP(1));
    checkHashMapIterTraverse(INT_HASH_MAP(10));
    checkHashMapIterTraverse(INT_HASH_MAP(100));
    checkHashMapIterTraverse(STRING_HASH_MAP_COLLISION(10));
    checkHashMapIterTraverse(STRING_HASH_MAP_COLLISION(100));
    checkHashMapIterTraverse(INT_HASH_MAP_COLLISION(10));
    checkHashMapIterTraverse(INT_HASH_MAP_COLLISION(100));
}

PICOTEST_SUITE(testHashMapIteratorEmptyMap, testEmptyHashMapIterBegin,
               testEmptyIntHashMapIterBegin, testEmptyHashMapIterFind,
               testEmptyIntHashMapIterFind);
static void checkMapIterEmpty(Col_MapIterator it) {
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapSize(Col_MapIterMap(it)) == 0);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
}
PICOTEST_CASE(testEmptyHashMapIterBegin, colibriFixture) {
    Col_Word empty = Col_NewStringHashMap(0);
    Col_MapIterator it;

    Col_HashMapIterBegin(it, empty);
    checkMapIterEmpty(it);
}
PICOTEST_CASE(testEmptyIntHashMapIterBegin, colibriFixture) {
    Col_Word empty = Col_NewIntHashMap(0);
    Col_MapIterator it;

    Col_HashMapIterBegin(it, empty);
    checkMapIterEmpty(it);
}
PICOTEST_CASE(testEmptyHashMapIterFind, colibriFixture) {
    Col_Word empty = Col_NewStringHashMap(0);
    Col_MapIterator it;

    Col_HashMapIterFind(it, empty, Col_NewCharWord('a'), NULL);
    checkMapIterEmpty(it);
}
PICOTEST_CASE(testEmptyIntHashMapIterFind, colibriFixture) {
    Col_Word empty = Col_NewIntHashMap(0);
    Col_MapIterator it;

    Col_IntHashMapIterFind(it, empty, 1, NULL);
    checkMapIterEmpty(it);
}
