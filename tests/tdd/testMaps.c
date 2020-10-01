#include <colibri.h>
#include <picotest.h>

/*
 * Failure test cases (must be defined before test hooks)
 */

#include "failureFixture.h"

/* Col_MapSize */
PICOTEST_CASE(mapSize_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(), COL_ERROR_MAP);
    PICOTEST_ASSERT(Col_MapSize(WORD_NIL) == 0);
}

/* Col_MapGet */
PICOTEST_CASE(mapGet_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_WORDMAP);
    PICOTEST_ASSERT(Col_MapGet(WORD_NIL, WORD_TRUE, NULL) == WORD_NIL);
}

/* Col_IntMapGet */
PICOTEST_CASE(intMapGet_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_INTMAP);
    PICOTEST_ASSERT(Col_IntMapGet(WORD_NIL, 0, NULL) == 0);
}

/* Col_MapSet */
PICOTEST_CASE(mapSet_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_WORDMAP);
    PICOTEST_ASSERT(Col_MapSet(WORD_NIL, WORD_TRUE, WORD_FALSE) == 0);
}

/* Col_IntMapSet */
PICOTEST_CASE(intMapSet_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_INTMAP);
    PICOTEST_ASSERT(Col_IntMapSet(WORD_NIL, 0, WORD_FALSE) == 0);
}

/* Col_MapUnset */
PICOTEST_CASE(mapUnset_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_WORDMAP);
    PICOTEST_ASSERT(Col_MapUnset(WORD_NIL, WORD_TRUE) == 0);
}

/* Col_IntMapUnset */
PICOTEST_CASE(intMapUnset_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_INTMAP);
    PICOTEST_ASSERT(Col_IntMapUnset(WORD_NIL, 0) == 0);
}

/* Col_MapIterBegin */
PICOTEST_CASE(mapIterBegin_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(), COL_ERROR_MAP);
    Col_MapIterator it;
    Col_MapIterBegin(it, WORD_NIL);
    PICOTEST_ASSERT(Col_MapIterNull(it));
}

/* Col_MapIterFind */
PICOTEST_CASE(mapIterFind_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_WORDMAP);
    Col_MapIterator it;
    Col_MapIterFind(it, WORD_NIL, WORD_TRUE, NULL);
    PICOTEST_ASSERT(Col_MapIterNull(it));
}

/* Col_IntMapIterFind */
PICOTEST_CASE(intMapIterFind_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_INTMAP);
    Col_MapIterator it;
    Col_IntMapIterFind(it, WORD_NIL, WORD_TRUE, NULL);
    PICOTEST_ASSERT(Col_MapIterNull(it));
}

/* Col_MapIterGet */
PICOTEST_CASE(mapIterGet_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER);
    Col_MapIterator it = COL_MAPITER_NULL;
    Col_Word key, value;
    Col_MapIterGet(it, &key, &value);
}
PICOTEST_CASE(mapIterGet_typeCheck_map, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_WORDMAP);
    Col_MapIterator it = COL_MAPITER_NULL;
    Col_MapIterBegin(it, Col_NewIntTrieMap());
    Col_Word key, value;
    Col_MapIterGet(it, &key, &value);
}
PICOTEST_CASE(mapIterGet_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER_END);
    Col_MapIterator it;
    Col_MapIterBegin(it, Col_NewStringHashMap(0));
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    Col_Word key, value;
    Col_MapIterGet(it, &key, &value);
}

/* Col_IntMapIterGet */
PICOTEST_CASE(intMapIterGet_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER);
    Col_MapIterator it = COL_MAPITER_NULL;
    intptr_t key;
    Col_Word value;
    Col_IntMapIterGet(it, &key, &value);
}
PICOTEST_CASE(intMapIterGet_typeCheck_map, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_INTMAP);
    Col_MapIterator it = COL_MAPITER_NULL;
    Col_MapIterBegin(it, Col_NewStringHashMap(0));
    intptr_t key;
    Col_Word value;
    Col_IntMapIterGet(it, &key, &value);
}
PICOTEST_CASE(intMapIterGet_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER_END);
    Col_MapIterator it;
    Col_MapIterBegin(it, Col_NewIntTrieMap());
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    intptr_t key;
    Col_Word value;
    Col_IntMapIterGet(it, &key, &value);
}

/* Col_MapIterGetKey */
PICOTEST_CASE(mapIterGetKey_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER);
    Col_MapIterator it = COL_MAPITER_NULL;
    PICOTEST_ASSERT(Col_MapIterGetKey(it) == WORD_NIL);
}
PICOTEST_CASE(mapIterGetKey_typeCheck_map, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_WORDMAP);
    Col_MapIterator it = COL_MAPITER_NULL;
    Col_MapIterBegin(it, Col_NewIntTrieMap());
    PICOTEST_ASSERT(Col_MapIterGetKey(it) == WORD_NIL);
}
PICOTEST_CASE(mapIterGetKey_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER_END);
    Col_MapIterator it;
    Col_MapIterBegin(it, Col_NewStringHashMap(0));
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    PICOTEST_ASSERT(Col_MapIterGetKey(it) == WORD_NIL);
}

/* Col_IntMapIterGetKey */
PICOTEST_CASE(intMapIterGetKey_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER);
    Col_MapIterator it = COL_MAPITER_NULL;
    PICOTEST_ASSERT(Col_IntMapIterGetKey(it) == 0);
}
PICOTEST_CASE(intMapIterGetKey_typeCheck_map, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_INTMAP);
    Col_MapIterator it = COL_MAPITER_NULL;
    Col_MapIterBegin(it, Col_NewStringHashMap(0));
    PICOTEST_ASSERT(Col_IntMapIterGetKey(it) == 0);
}
PICOTEST_CASE(intMapIterGetKey_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER_END);
    Col_MapIterator it;
    Col_MapIterBegin(it, Col_NewIntTrieMap());
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    PICOTEST_ASSERT(Col_IntMapIterGetKey(it) == 0);
}

/* Col_MapIterGetValue */
PICOTEST_CASE(mapIterGetValue_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER);
    Col_MapIterator it = COL_MAPITER_NULL;
    PICOTEST_ASSERT(Col_MapIterGetValue(it) == WORD_NIL);
}
PICOTEST_CASE(mapIterGetValue_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER_END);
    Col_MapIterator it;
    Col_MapIterBegin(it, Col_NewStringHashMap(0));
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    PICOTEST_ASSERT(Col_MapIterGetValue(it) == WORD_NIL);
}

/* Col_MapIterSetValue */
PICOTEST_CASE(mapIterSetValue_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER);
    Col_MapIterator it = COL_MAPITER_NULL;
    Col_MapIterSetValue(it, WORD_TRUE);
}
PICOTEST_CASE(mapIterSetValue_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER_END);
    Col_MapIterator it;
    Col_MapIterBegin(it, Col_NewStringHashMap(0));
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    Col_MapIterSetValue(it, WORD_TRUE);
}

/* Col_MapIterNext */
PICOTEST_CASE(mapIterNext_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER);
    Col_MapIterator it = COL_MAPITER_NULL;
    Col_MapIterNext(it);
}
PICOTEST_CASE(mapIterNext_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER_END);
    Col_MapIterator it;
    Col_MapIterBegin(it, Col_NewStringHashMap(0));
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    Col_MapIterNext(it);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
}

/*
 * Generic maps
 */

#include "hooks.h"
#include "colibriFixture.h"

PICOTEST_SUITE(testMaps, testMapTypeChecks, testMapAccessors, testMapIteration);

PICOTEST_CASE(testMapTypeChecks, colibriFixture) {
    PICOTEST_VERIFY(mapSize_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mapGet_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(intMapGet_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mapSet_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(intMapSet_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mapUnset_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(intMapUnset_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mapIterBegin_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mapIterFind_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(intMapIterFind_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mapIterGet_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mapIterGet_typeCheck_map(NULL) == 1);
    PICOTEST_VERIFY(mapIterGet_valueCheck(NULL) == 1);
    PICOTEST_VERIFY(intMapIterGet_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(intMapIterGet_typeCheck_map(NULL) == 1);
    PICOTEST_VERIFY(intMapIterGet_valueCheck(NULL) == 1);
    PICOTEST_VERIFY(mapIterGetKey_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mapIterGetKey_typeCheck_map(NULL) == 1);
    PICOTEST_VERIFY(mapIterGetKey_valueCheck(NULL) == 1);
    PICOTEST_VERIFY(intMapIterGetKey_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(intMapIterGetKey_typeCheck_map(NULL) == 1);
    PICOTEST_VERIFY(intMapIterGetKey_valueCheck(NULL) == 1);
    PICOTEST_VERIFY(mapIterGetValue_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mapIterGetValue_valueCheck(NULL) == 1);
    PICOTEST_VERIFY(mapIterSetValue_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mapIterSetValue_valueCheck(NULL) == 1);
    PICOTEST_VERIFY(mapIterNext_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(mapIterNext_valueCheck(NULL) == 1);
}

PICOTEST_SUITE(testMapAccessors, testMapSize, testMapGet, testIntMapGet,
               testMapSet, testIntMapSet, testMapUnset, testIntMapUnset);

PICOTEST_CASE(testMapSize, colibriFixture) {
    PICOTEST_ASSERT(Col_MapSize(Col_NewStringHashMap(0)) == 0);
    PICOTEST_ASSERT(Col_MapSize(Col_NewIntHashMap(0)) == 0);
    PICOTEST_ASSERT(Col_MapSize(Col_NewStringTrieMap()) == 0);
    PICOTEST_ASSERT(Col_MapSize(Col_NewIntTrieMap()) == 0);
}
PICOTEST_SUITE(testMapGet, testMapGetEmpty, testMapGetFound,
               testMapGetNotFound);
static void checkMapGetEmpty(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_MapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}
PICOTEST_CASE(testMapGetEmpty, colibriFixture) {
    checkMapGetEmpty(Col_NewStringHashMap(0));
    checkMapGetEmpty(Col_NewStringTrieMap());
}
static void checkMapGetFound(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    Col_MapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_MapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testMapGetFound, colibriFixture) {
    checkMapGetFound(Col_NewStringHashMap(0));
    checkMapGetFound(Col_NewStringTrieMap());
}

static void checkMapGetNotFound(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    Col_MapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_MapGet(map, Col_NewCharWord('b'), &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}
PICOTEST_CASE(testMapGetNotFound, colibriFixture) {
    checkMapGetNotFound(Col_NewStringHashMap(0));
    checkMapGetNotFound(Col_NewStringTrieMap());
}

PICOTEST_SUITE(testIntMapGet, testIntMapGetEmpty, testIntMapGetFound,
               testIntMapGetNotFound);
static void checkIntMapGetEmpty(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntMapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}
PICOTEST_CASE(testIntMapGetEmpty, colibriFixture) {
    checkIntMapGetEmpty(Col_NewIntHashMap(0));
    checkIntMapGetEmpty(Col_NewIntTrieMap());
}

static void checkIntMapGetFound(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    Col_IntMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testIntMapGetFound, colibriFixture) {
    checkIntMapGetFound(Col_NewIntHashMap(0));
    checkIntMapGetFound(Col_NewIntTrieMap());
}

static void checkIntMapGetNotFound(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    Col_IntMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntMapGet(map, 2, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}
PICOTEST_CASE(testIntMapGetNotFound, colibriFixture) {
    checkIntMapGetNotFound(Col_NewIntHashMap(0));
    checkIntMapGetNotFound(Col_NewIntTrieMap());
}

PICOTEST_SUITE(testMapSet, testMapSetEmpty, testMapSetCreated,
               testMapSetUpdated);

static void checkMapSetEmpty(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    PICOTEST_ASSERT(Col_MapSet(map, key, WORD_TRUE) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_MapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testMapSetEmpty, colibriFixture) {
    checkMapSetEmpty(Col_NewStringHashMap(0));
    checkMapSetEmpty(Col_NewStringTrieMap());
}

static void checkMapSetCreated(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    Col_MapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    PICOTEST_ASSERT(Col_MapSet(map, Col_NewCharWord('b'), WORD_FALSE) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == 2);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_MapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testMapSetCreated, colibriFixture) {
    checkMapSetCreated(Col_NewStringHashMap(0));
    checkMapSetCreated(Col_NewStringTrieMap());
}

static void checkMapSetUpdated(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    PICOTEST_ASSERT(Col_MapSet(map, key, WORD_TRUE) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    PICOTEST_ASSERT(Col_MapSet(map, key, WORD_FALSE) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_MapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_FALSE);
}
PICOTEST_CASE(testMapSetUpdated, colibriFixture) {
    checkMapSetUpdated(Col_NewStringHashMap(0));
    checkMapSetUpdated(Col_NewStringTrieMap());
}

PICOTEST_SUITE(testIntMapSet, testIntMapSetEmpty, testIntMapSetCreated,
               testIntMapSetUpdated);

static void checkIntMapSetEmpty(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    PICOTEST_ASSERT(Col_IntMapSet(map, key, WORD_TRUE) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testIntMapSetEmpty, colibriFixture) {
    checkIntMapSetEmpty(Col_NewIntHashMap(0));
    checkIntMapSetEmpty(Col_NewIntTrieMap());
}

static void checkIntMapSetCreated(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    Col_IntMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    PICOTEST_ASSERT(Col_IntMapSet(map, 2, WORD_FALSE) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == 2);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testIntMapSetCreated, colibriFixture) {
    checkIntMapSetCreated(Col_NewIntHashMap(0));
    checkIntMapSetCreated(Col_NewIntTrieMap());
}

static void checkIntMapSetUpdated(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    PICOTEST_ASSERT(Col_IntMapSet(map, key, WORD_TRUE) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    PICOTEST_ASSERT(Col_IntMapSet(map, key, WORD_FALSE) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_FALSE);
}
PICOTEST_CASE(testIntMapSetUpdated, colibriFixture) {
    checkIntMapSetUpdated(Col_NewIntHashMap(0));
    checkIntMapSetUpdated(Col_NewIntTrieMap());
}

PICOTEST_SUITE(testMapUnset, testMapUnsetEmpty, testMapUnsetNotFound,
               testMapUnsetFound);

static void checkMapUnsetEmpty(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    PICOTEST_ASSERT(Col_MapUnset(map, key) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
}
PICOTEST_CASE(testMapUnsetEmpty, colibriFixture) {
    checkMapUnsetEmpty(Col_NewStringHashMap(0));
    checkMapUnsetEmpty(Col_NewStringTrieMap());
}

static void checkMapUnsetNotFound(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    Col_MapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    PICOTEST_ASSERT(Col_MapUnset(map, Col_NewCharWord('b')) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_MapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testMapUnsetNotFound, colibriFixture) {
    checkMapUnsetNotFound(Col_NewStringHashMap(0));
    checkMapUnsetNotFound(Col_NewStringTrieMap());
}

static void checkMapUnsetFound(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    Col_MapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    PICOTEST_ASSERT(Col_MapUnset(map, key) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_MapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}
PICOTEST_CASE(testMapUnsetFound, colibriFixture) {
    checkMapUnsetFound(Col_NewStringHashMap(0));
    checkMapUnsetFound(Col_NewStringTrieMap());
}

PICOTEST_SUITE(testIntMapUnset, testIntMapUnsetEmpty, testIntMapUnsetNotFound,
               testIntMapUnsetFound);

static void checkIntMapUnsetEmpty(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    PICOTEST_ASSERT(Col_IntMapUnset(map, key) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
}
PICOTEST_CASE(testIntMapUnsetEmpty, colibriFixture) {
    checkIntMapUnsetEmpty(Col_NewIntHashMap(0));
    checkIntMapUnsetEmpty(Col_NewIntTrieMap());
}

static void checkIntMapUnsetNotFound(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    Col_IntMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    PICOTEST_ASSERT(Col_IntMapUnset(map, 2) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testIntMapUnsetNotFound, colibriFixture) {
    checkIntMapUnsetNotFound(Col_NewIntHashMap(0));
    checkIntMapUnsetNotFound(Col_NewIntTrieMap());
}

static void checkIntMapUnsetFound(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    Col_IntMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    PICOTEST_ASSERT(Col_IntMapUnset(map, key) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntMapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}
PICOTEST_CASE(testIntMapUnsetFound, colibriFixture) {
    checkIntMapUnsetFound(Col_NewIntHashMap(0));
    checkIntMapUnsetFound(Col_NewIntTrieMap());
}

PICOTEST_SUITE(testMapIteration, testMapIteratorErrors,
               testMapIteratorInitialize, testMapIteratorAccess,
               testMapIteratorMove, testMapIteratorEmptyMap);

PICOTEST_SUITE(testMapIteratorErrors, testMapIteratorAtEndIsInvalid);
PICOTEST_CASE(testMapIteratorAtEndIsInvalid, colibriFixture) {
    PICOTEST_VERIFY(mapIterSetValue_valueCheck(NULL) == 1);
    PICOTEST_VERIFY(mapIterNext_valueCheck(NULL) == 1);
}

PICOTEST_SUITE(testMapIteratorInitialize, testMapIterNull, testMapIterBegin,
               testMapIterFind, testIntMapIterFind, testMapIterSet);

PICOTEST_CASE(testMapIterNull, colibriFixture) {
    Col_MapIterator it = COL_MAPITER_NULL;
    PICOTEST_ASSERT(Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == WORD_NIL);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
}

static void checkMapIterBegin(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_MapSet(map, Col_NewCharWord('a'), WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    Col_MapIterator it;
    Col_MapIterBegin(it, map);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
}
PICOTEST_CASE(testMapIterBegin, colibriFixture) {
    checkMapIterBegin(Col_NewStringHashMap(0));
    checkMapIterBegin(Col_NewStringTrieMap());
}

PICOTEST_SUITE(testMapIterFind, testMapIterFindFound, testMapIterFindNotFound,
               testMapIterFindCreate);

static void checkMapIterFindFound(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    Col_MapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    Col_MapIterator it;

    Col_MapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    int create = 0;
    Col_MapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(!create);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
}
PICOTEST_CASE(testMapIterFindFound, colibriFixture) {
    checkMapIterFindFound(Col_NewStringHashMap(0));
    checkMapIterFindFound(Col_NewStringTrieMap());
}

static void checkMapIterFindNotFound(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_MapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);

    Col_MapIterator it;

    Col_MapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    PICOTEST_ASSERT(Col_MapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);

    int create = 0;
    Col_MapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    PICOTEST_ASSERT(!create);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    PICOTEST_ASSERT(Col_MapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}
PICOTEST_CASE(testMapIterFindNotFound, colibriFixture) {
    checkMapIterFindNotFound(Col_NewStringHashMap(0));
    checkMapIterFindNotFound(Col_NewStringTrieMap());
}

static void checkMapIterFindCreate(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_MapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);

    Col_MapIterator it;
    int create = 1;
    Col_MapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(create);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    PICOTEST_ASSERT(Col_MapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_NIL);
}
PICOTEST_CASE(testMapIterFindCreate, colibriFixture) {
    checkMapIterFindCreate(Col_NewStringHashMap(0));
    checkMapIterFindCreate(Col_NewStringTrieMap());
}

PICOTEST_SUITE(testIntMapIterFind, testIntMapIterFindFound,
               testIntMapIterFindNotFound, testIntMapIterFindCreate);

static void checkIntMapIterFindFound(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    Col_IntMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    Col_MapIterator it;

    Col_IntMapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    int create = 0;
    Col_IntMapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(!create);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
}
PICOTEST_CASE(testIntMapIterFindFound, colibriFixture) {
    checkIntMapIterFindFound(Col_NewIntHashMap(0));
    checkIntMapIterFindFound(Col_NewIntTrieMap());
}

static void checkIntMapIterFindNotFound(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntMapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);

    Col_MapIterator it;

    Col_IntMapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    PICOTEST_ASSERT(Col_IntMapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);

    int create = 0;
    Col_IntMapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    PICOTEST_ASSERT(!create);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    PICOTEST_ASSERT(Col_IntMapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}
PICOTEST_CASE(testIntMapIterFindNotFound, colibriFixture) {
    checkIntMapIterFindNotFound(Col_NewIntHashMap(0));
    checkIntMapIterFindNotFound(Col_NewIntTrieMap());
}

static void checkIntMapIterFindCreate(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntMapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);

    Col_MapIterator it;
    int create = 1;
    Col_IntMapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(create);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    PICOTEST_ASSERT(Col_IntMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_NIL);
}
PICOTEST_CASE(testIntMapIterFindCreate, colibriFixture) {
    checkIntMapIterFindCreate(Col_NewIntHashMap(0));
    checkIntMapIterFindCreate(Col_NewIntTrieMap());
}

static void checkMapIterSet(Col_Word map) {
    Col_MapSet(map, Col_NewCharWord('a'), WORD_TRUE);
    Col_MapSet(map, Col_NewCharWord('b'), WORD_FALSE);

    Col_MapIterator it1;
    Col_MapIterBegin(it1, map);
    PICOTEST_ASSERT(!Col_MapIterNull(it1));

    Col_MapIterator it2 = COL_MAPITER_NULL;
    PICOTEST_ASSERT(Col_MapIterNull(it2));

    Col_MapIterSet(it2, it1);
    PICOTEST_ASSERT(!Col_MapIterNull(it1));
    PICOTEST_ASSERT(!Col_MapIterNull(it2));
    PICOTEST_ASSERT(Col_MapIterMap(it1) == Col_MapIterMap(it2));
    PICOTEST_ASSERT(Col_MapIterGetKey(it1) == Col_MapIterGetKey(it2));

    Col_MapIterNext(it1);
    PICOTEST_ASSERT(Col_MapIterGetKey(it1) != Col_MapIterGetKey(it2));
}
static void checkIntMapIterSet(Col_Word map) {
    Col_IntMapSet(map, 1, WORD_TRUE);
    Col_IntMapSet(map, 2, WORD_FALSE);

    Col_MapIterator it1;
    Col_MapIterBegin(it1, map);
    PICOTEST_ASSERT(!Col_MapIterNull(it1));

    Col_MapIterator it2 = COL_MAPITER_NULL;
    PICOTEST_ASSERT(Col_MapIterNull(it2));

    Col_MapIterSet(it2, it1);
    PICOTEST_ASSERT(!Col_MapIterNull(it1));
    PICOTEST_ASSERT(!Col_MapIterNull(it2));
    PICOTEST_ASSERT(Col_MapIterMap(it1) == Col_MapIterMap(it2));
    PICOTEST_ASSERT(Col_IntMapIterGetKey(it1) == Col_IntMapIterGetKey(it2));

    Col_MapIterNext(it1);
    PICOTEST_ASSERT(Col_IntMapIterGetKey(it1) != Col_IntMapIterGetKey(it2));
}
PICOTEST_CASE(testMapIterSet, colibriFixture) {
    checkMapIterSet(Col_NewStringHashMap(0));
    checkMapIterSet(Col_NewStringTrieMap());
    checkIntMapIterSet(Col_NewIntHashMap(0));
    checkIntMapIterSet(Col_NewIntTrieMap());
}

PICOTEST_SUITE(testMapIteratorAccess, testMapIterGet, testIntMapIterGet,
               testMapIterGetKey, testIntMapIterGetKey, testMapIterGetValue,
               testMapIterSetValue);

static void checkMapIterGet(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    Col_MapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    Col_Word value;
    PICOTEST_ASSERT(Col_MapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);

    Col_MapIterator it;
    Col_MapIterFind(it, map, key, NULL);
    Col_Word key2;
    Col_Word value2;
    Col_MapIterGet(it, &key2, &value2);
    PICOTEST_ASSERT(key2 == key);
    PICOTEST_ASSERT(value2 == value);
}
PICOTEST_CASE(testMapIterGet, colibriFixture) {
    checkMapIterGet(Col_NewStringHashMap(0));
    checkMapIterGet(Col_NewStringTrieMap());
}

static void checkIntMapIterGet(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    Col_IntMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    Col_Word value;
    PICOTEST_ASSERT(Col_IntMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);

    Col_MapIterator it;
    Col_IntMapIterFind(it, map, key, NULL);
    intptr_t key2;
    Col_Word value2;
    Col_IntMapIterGet(it, &key2, &value2);
    PICOTEST_ASSERT(key2 == key);
    PICOTEST_ASSERT(value2 == value);
}
PICOTEST_CASE(testIntMapIterGet, colibriFixture) {
    checkIntMapIterGet(Col_NewIntHashMap(0));
    checkIntMapIterGet(Col_NewIntTrieMap());
}

static void checkMapIterGetKey(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    Col_MapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    Col_MapIterator it;
    Col_MapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(Col_MapIterGetKey(it) == key);
}
PICOTEST_CASE(testMapIterGetKey, colibriFixture) {
    checkMapIterGetKey(Col_NewStringHashMap(0));
    checkMapIterGetKey(Col_NewStringTrieMap());
}

static void checkIntMapIterGetKey(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    Col_IntMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    Col_MapIterator it;
    Col_IntMapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(Col_IntMapIterGetKey(it) == key);
}
PICOTEST_CASE(testIntMapIterGetKey, colibriFixture) {
    checkIntMapIterGetKey(Col_NewIntHashMap(0));
    checkIntMapIterGetKey(Col_NewIntTrieMap());
}

static void checkMapIterGetValue(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    Col_MapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    Col_MapIterator it;
    Col_MapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(Col_MapIterGetValue(it) == WORD_TRUE);
}
static void checkIntMapIterGetValue(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    Col_IntMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    Col_MapIterator it;
    Col_IntMapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(Col_MapIterGetValue(it) == WORD_TRUE);
}
PICOTEST_CASE(testMapIterGetValue, colibriFixture) {
    checkMapIterGetValue(Col_NewStringHashMap(0));
    checkMapIterGetValue(Col_NewStringTrieMap());
    checkIntMapIterGetValue(Col_NewIntHashMap(0));
    checkIntMapIterGetValue(Col_NewIntTrieMap());
}

static void checkMapIterSetValue(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    Col_MapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    Col_Word value;
    PICOTEST_ASSERT(Col_MapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);

    Col_MapIterator it;
    Col_MapIterFind(it, map, key, NULL);
    Col_MapIterSetValue(it, WORD_FALSE);
    PICOTEST_ASSERT(Col_MapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_FALSE);
}
static void checkIntMapIterSetValue(Col_Word map) {
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    Col_IntMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);

    Col_Word value;
    PICOTEST_ASSERT(Col_IntMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);

    Col_MapIterator it;
    Col_IntMapIterFind(it, map, key, NULL);
    Col_MapIterSetValue(it, WORD_FALSE);
    PICOTEST_ASSERT(Col_IntMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_FALSE);
}
PICOTEST_CASE(testMapIterSetValue, colibriFixture) {
    checkMapIterSetValue(Col_NewStringHashMap(0));
    checkMapIterSetValue(Col_NewStringTrieMap());
    checkIntMapIterSetValue(Col_NewIntHashMap(0));
    checkIntMapIterSetValue(Col_NewIntTrieMap());
}

PICOTEST_SUITE(testMapIteratorMove, testMapIterNext);

static void checkMapIterTraverse(Col_Word map) {
    size_t size = 10;
    for (size_t i = 0; i < size; i++) {
        Col_MapSet(map, Col_NewCharWord('a' + i), Col_NewIntWord(i));
    }

    Col_MapIterator it;
    size_t i;
    for (i = 0, Col_MapIterBegin(it, map); !Col_MapIterEnd(it);
         i++, Col_MapIterNext(it))
        ;
    PICOTEST_ASSERT(i == Col_MapSize(map));
}
static void checkIntMapIterTraverse(Col_Word map) {
    size_t size = 10;
    for (size_t i = 0; i < size; i++) {
        Col_IntMapSet(map, Col_NewCharWord('a' + i), Col_NewIntWord(i));
    }

    Col_MapIterator it;
    size_t i;
    for (i = 0, Col_MapIterBegin(it, map); !Col_MapIterEnd(it);
         i++, Col_MapIterNext(it))
        ;
    PICOTEST_ASSERT(i == Col_MapSize(map));
}
PICOTEST_CASE(testMapIterNext, colibriFixture) {
    checkMapIterTraverse(Col_NewStringHashMap(0));
    checkMapIterTraverse(Col_NewStringTrieMap(0));
    checkIntMapIterTraverse(Col_NewIntHashMap(0));
    checkIntMapIterTraverse(Col_NewIntTrieMap(0));
}

PICOTEST_SUITE(testMapIteratorEmptyMap, testEmptyMapIterBegin,
               testEmptyMapIterFind, testEmptyIntMapIterFind);
static void checkMapIterEmpty(Col_Word map) {
    Col_MapIterator it;

    Col_MapIterBegin(it, map);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapSize(Col_MapIterMap(it)) == 0);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
}
PICOTEST_CASE(testEmptyMapIterBegin, colibriFixture) {
    checkMapIterEmpty(Col_NewStringHashMap(0));
    checkMapIterEmpty(Col_NewStringTrieMap());
    checkMapIterEmpty(Col_NewIntHashMap(0));
    checkMapIterEmpty(Col_NewIntTrieMap());
}

static void checkMapIterFind(Col_Word map) {
    Col_MapIterator it;

    Col_MapIterFind(it, map, Col_NewCharWord('a'), NULL);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapSize(Col_MapIterMap(it)) == 0);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
}
PICOTEST_CASE(testEmptyMapIterFind, colibriFixture) {
    checkMapIterFind(Col_NewStringHashMap(0));
    checkMapIterFind(Col_NewStringTrieMap());
}

static void checkIntMapIterFind(Col_Word map) {
    Col_MapIterator it;

    Col_IntMapIterFind(it, map, 1, NULL);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapSize(Col_MapIterMap(it)) == 0);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
}
PICOTEST_CASE(testEmptyIntMapIterFind, colibriFixture) {
    checkIntMapIterFind(Col_NewIntHashMap(0));
    checkIntMapIterFind(Col_NewIntTrieMap());
}
