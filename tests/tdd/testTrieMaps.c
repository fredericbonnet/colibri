#include <colibri.h>
#include <picotest.h>

/*
 * Failure test cases (must be defined before test hooks)
 */

#include "failureFixture.h"

/* Col_CopyTrieMap */
PICOTEST_CASE(copyTrieMap_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_TRIEMAP);
    PICOTEST_ASSERT(Col_CopyTrieMap(WORD_NIL) == WORD_NIL);
}

/* Col_TrieMapGet */
PICOTEST_CASE(trieMapGet_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_WORDTRIEMAP);
    PICOTEST_ASSERT(Col_TrieMapGet(WORD_NIL, WORD_TRUE, NULL) == WORD_NIL);
}

/* Col_IntTrieMapGet */
PICOTEST_CASE(intTrieMapGet_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_INTTRIEMAP);
    PICOTEST_ASSERT(Col_IntTrieMapGet(WORD_NIL, 0, NULL) == 0);
}

/* Col_TrieMapSet */
PICOTEST_CASE(trieMapSet_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_WORDTRIEMAP);
    PICOTEST_ASSERT(Col_TrieMapSet(WORD_NIL, WORD_TRUE, WORD_FALSE) == 0);
}

/* Col_IntTrieMapSet */
PICOTEST_CASE(intTrieMapSet_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_INTTRIEMAP);
    PICOTEST_ASSERT(Col_IntTrieMapSet(WORD_NIL, 0, WORD_FALSE) == 0);
}

/* Col_TrieMapUnset */
PICOTEST_CASE(trieMapUnset_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_WORDTRIEMAP);
    PICOTEST_ASSERT(Col_TrieMapUnset(WORD_NIL, WORD_TRUE) == 0);
}

/* Col_IntTrieMapUnset */
PICOTEST_CASE(intTrieMapUnset_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_INTTRIEMAP);
    PICOTEST_ASSERT(Col_IntTrieMapUnset(WORD_NIL, 0) == 0);
}

/* Col_TrieMapIterFirst */
PICOTEST_CASE(trieMapIterFirst_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_TRIEMAP);
    Col_MapIterator it;
    Col_TrieMapIterFirst(it, WORD_NIL);
    PICOTEST_ASSERT(Col_MapIterNull(it));
}

/* Col_TrieMapIterLast */
PICOTEST_CASE(trieMapIterLast_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_TRIEMAP);
    Col_MapIterator it;
    Col_TrieMapIterLast(it, WORD_NIL);
    PICOTEST_ASSERT(Col_MapIterNull(it));
}

/* Col_TrieMapIterFind */
PICOTEST_CASE(trieMapIterFind_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_WORDTRIEMAP);
    Col_MapIterator it;
    Col_TrieMapIterFind(it, WORD_NIL, WORD_TRUE, NULL);
    PICOTEST_ASSERT(Col_MapIterNull(it));
}

/* Col_IntTrieMapIterFind */
PICOTEST_CASE(intTrieMapIterFind_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_INTTRIEMAP);
    Col_MapIterator it;
    Col_IntTrieMapIterFind(it, WORD_NIL, WORD_TRUE, NULL);
    PICOTEST_ASSERT(Col_MapIterNull(it));
}

/* Col_TrieMapIterSetValue */
PICOTEST_CASE(trieMapIterSetValue_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER);
    Col_MapIterator it = COL_MAPITER_NULL;
    Col_TrieMapIterSetValue(it, WORD_TRUE);
}
PICOTEST_CASE(trieMapIterSetValue_typeCheck_map, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_TRIEMAP);
    Col_MapIterator it;
    Col_HashMapIterBegin(it, Col_NewIntHashMap(0));
    Col_TrieMapIterSetValue(it, WORD_TRUE);
}
PICOTEST_CASE(trieMapIterSetValue_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER_END);
    Col_MapIterator it;
    Col_TrieMapIterFirst(it, Col_NewStringTrieMap());
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    Col_TrieMapIterSetValue(it, WORD_TRUE);
}

/* Col_TrieMapIterNext */
PICOTEST_CASE(trieMapIterNext_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER);
    Col_MapIterator it = COL_MAPITER_NULL;
    Col_TrieMapIterNext(it);
}
PICOTEST_CASE(trieMapIterNext_typeCheck_map, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_TRIEMAP);
    Col_MapIterator it;
    Col_HashMapIterBegin(it, Col_NewIntHashMap(0));
    Col_TrieMapIterNext(it);
}
PICOTEST_CASE(trieMapIterNext_valueCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_VALUECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER_END);
    Col_MapIterator it;
    Col_TrieMapIterFirst(it, Col_NewStringTrieMap());
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    Col_TrieMapIterNext(it);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
}

/* Col_TrieMapIterPrevious */
PICOTEST_CASE(trieMapIterPrevious_typeCheck, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_MAPITER);
    Col_MapIterator it = COL_MAPITER_NULL;
    Col_TrieMapIterPrevious(it);
}
PICOTEST_CASE(trieMapIterPrevious_typeCheck_map, failureFixture, context) {
    EXPECT_FAILURE(context, COL_TYPECHECK, Col_GetErrorDomain(),
                   COL_ERROR_TRIEMAP);
    Col_MapIterator it = COL_MAPITER_NULL;
    Col_HashMapIterBegin(it, Col_NewIntHashMap(0));
    Col_TrieMapIterPrevious(it);
}

/*
 * Test data
 */

#define STRING_TRIE_KEY(i) Col_NewCharWord('a' + i)
#define STRING_TRIE_KEY_PREFIX(i)                                              \
    Col_RepeatRope(Col_NewCharWord('a' + (i & 0xF)), (i >> 4) + 1)
#define STRING_TRIE_VALUE(i) Col_NewIntWord(i)
#define INT_TRIE_KEY(i) ((intptr_t)(i))
#define INT_TRIE_VALUE(i) Col_NewCharWord('A' + i)

#define STRING_TRIE_MAP(size) STRING_TRIE_MAP_SKIP(size, SIZE_MAX)
static Col_Word STRING_TRIE_MAP_SKIP(size_t size, size_t skip) {
    Col_Word map = Col_NewStringTrieMap();
    for (size_t i = 0; i < size; i++) {
        if (i == skip)
            continue;
        Col_TrieMapSet(map, STRING_TRIE_KEY(i), STRING_TRIE_VALUE(i));
    }
    return map;
}
static Col_Word STRING_TRIE_MAP_PREFIX(size_t size) {
    Col_Word map = Col_NewStringTrieMap();
    for (size_t i = 0; i < size; i++) {
        Col_TrieMapSet(map, STRING_TRIE_KEY_PREFIX(i), STRING_TRIE_VALUE(i));
    }
    return map;
}
#define INT_TRIE_MAP(size) INT_TRIE_MAP_SKIP(size, SIZE_MAX)
static Col_Word INT_TRIE_MAP_SKIP(size_t size, size_t skip) {
    Col_Word map = Col_NewIntTrieMap();
    for (size_t i = 0; i < size; i++) {
        if (i == skip)
            continue;
        Col_IntTrieMapSet(map, INT_TRIE_KEY(i), INT_TRIE_VALUE(i));
    }
    return map;
}

/*
 * Trie maps
 */

#include "hooks.h"
#include "colibriFixture.h"

PICOTEST_SUITE(testTrieMaps, testTrieMapTypeChecks, testTrieMapCreation,
               testTrieMapAccessors, testTrieMapIteration);

PICOTEST_CASE(testTrieMapTypeChecks, colibriFixture) {
    PICOTEST_VERIFY(copyTrieMap_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(trieMapGet_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(intTrieMapGet_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(trieMapSet_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(intTrieMapSet_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(trieMapUnset_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(intTrieMapUnset_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(trieMapIterFirst_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(trieMapIterLast_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(trieMapIterFind_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(intTrieMapIterFind_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(trieMapIterSetValue_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(trieMapIterSetValue_typeCheck_map(NULL) == 1);
    PICOTEST_VERIFY(trieMapIterNext_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(trieMapIterNext_typeCheck_map(NULL) == 1);
    PICOTEST_VERIFY(trieMapIterPrevious_typeCheck(NULL) == 1);
    PICOTEST_VERIFY(trieMapIterPrevious_typeCheck_map(NULL) == 1);
}

PICOTEST_SUITE(testTrieMapCreation, testStringTrieMapCreation,
               testIntegerTrieMapCreation, testCopyTrieMap);

PICOTEST_SUITE(testStringTrieMapCreation, testNewStringTrieMap);

PICOTEST_SUITE(testIntegerTrieMapCreation, testNewIntTrieMap);

static void checkStringTrieMap(Col_Word map, size_t size) {
    PICOTEST_ASSERT(Col_WordType(map) == (COL_MAP | COL_TRIEMAP));
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
}
PICOTEST_CASE(testNewStringTrieMap, colibriFixture) {
    checkStringTrieMap(Col_NewStringTrieMap(), 0);
}

static void checkIntTrieMap(Col_Word map, size_t size) {
    PICOTEST_ASSERT(Col_WordType(map) == (COL_INTMAP | COL_TRIEMAP));
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
}
PICOTEST_CASE(testNewIntTrieMap, colibriFixture) {
    checkIntTrieMap(Col_NewIntTrieMap(), 0);
}

static void checkCopyTrieMap(Col_Word map) {
    size_t size = Col_MapSize(map);

    Col_Word copy1 = Col_CopyTrieMap(map);
    PICOTEST_ASSERT(Col_MapSize(copy1) == size);
    Col_Word copy2 = Col_CopyTrieMap(map);
    PICOTEST_ASSERT(Col_MapSize(copy2) == size);
    Col_Word copy3 = Col_CopyTrieMap(copy1);
    PICOTEST_ASSERT(Col_MapSize(copy3) == size);
}
PICOTEST_CASE(testCopyTrieMap, colibriFixture) {
    checkCopyTrieMap(STRING_TRIE_MAP(0));
    checkCopyTrieMap(STRING_TRIE_MAP(1));
    checkCopyTrieMap(STRING_TRIE_MAP(2));
    checkCopyTrieMap(STRING_TRIE_MAP(10));
    checkCopyTrieMap(STRING_TRIE_MAP(100));
    checkCopyTrieMap(STRING_TRIE_MAP_PREFIX(10));
    checkCopyTrieMap(STRING_TRIE_MAP_PREFIX(100));
    checkCopyTrieMap(INT_TRIE_MAP(0));
    checkCopyTrieMap(INT_TRIE_MAP(1));
    checkCopyTrieMap(INT_TRIE_MAP(2));
    checkCopyTrieMap(INT_TRIE_MAP(10));
    checkCopyTrieMap(INT_TRIE_MAP(100));
}

PICOTEST_SUITE(testTrieMapAccessors, testStringTrieMapAccessors,
               testIntegerTrieMapAccessors);

PICOTEST_SUITE(testStringTrieMapAccessors, testStringTrieMapGet,
               testStringTrieMapSet, testStringTrieMapUnset);

PICOTEST_SUITE(testStringTrieMapGet, testStringTrieMapGetEmpty,
               testStringTrieMapGetFound, testStringTrieMapGetNotFound);
PICOTEST_CASE(testStringTrieMapGetEmpty, colibriFixture) {
    Col_Word map = Col_NewStringTrieMap();
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_TrieMapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}
PICOTEST_CASE(testStringTrieMapGetFound, colibriFixture) {
    Col_Word map = Col_NewStringTrieMap();
    Col_Word key = Col_NewCharWord('a');
    Col_TrieMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_TrieMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testStringTrieMapGetNotFound, colibriFixture) {
    Col_Word map = Col_NewStringTrieMap();
    Col_Word key = Col_NewCharWord('a');
    Col_TrieMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_TrieMapGet(map, Col_NewCharWord('b'), &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}

PICOTEST_SUITE(testStringTrieMapSet, testStringTrieMapSetCreated,
               testStringTrieMapSetCreatedCopy, testStringTrieMapSetUpdated,
               testStringTrieMapSetUpdatedCopy);

static void checkStringTrieSetCreatedIndex(Col_Word map, size_t index) {
    size_t size = Col_MapSize(map);
    Col_Word key = STRING_TRIE_KEY(index);
    Col_Word value = STRING_TRIE_VALUE(index);
    PICOTEST_ASSERT(Col_TrieMapSet(map, key, value) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == size + 1);
    Col_Word value2 = 0xdeadbeef;
    PICOTEST_ASSERT(Col_TrieMapGet(map, key, &value2) == 1);
    PICOTEST_ASSERT(value2 == value);
}
static void checkStringTrieSetCreated(size_t size) {
    for (size_t index = 0; index <= size; index++) {
        Col_Word map = STRING_TRIE_MAP_SKIP(size + 1, index);
        checkStringTrieSetCreatedIndex(map, index);
    }
}
PICOTEST_CASE(testStringTrieMapSetCreated, colibriFixture) {
    checkStringTrieSetCreated(0);
    checkStringTrieSetCreated(1);
    checkStringTrieSetCreated(2);
    checkStringTrieSetCreated(3);
    checkStringTrieSetCreated(4);
}

static void checkStringTrieSetCreatedCopy(size_t size) {
    for (size_t index = 0; index <= size; index++) {
        Col_Word map = STRING_TRIE_MAP_SKIP(size + 1, index);
        Col_Word copy = Col_CopyTrieMap(map);

        checkStringTrieSetCreatedIndex(map, index);
        checkStringTrieSetCreatedIndex(copy, index);
    }
}
PICOTEST_CASE(testStringTrieMapSetCreatedCopy, colibriFixture) {
    checkStringTrieSetCreatedCopy(0);
    checkStringTrieSetCreatedCopy(1);
    checkStringTrieSetCreatedCopy(2);
    checkStringTrieSetCreatedCopy(3);
    checkStringTrieSetCreatedCopy(4);
}

static void checkStringTrieMapSetUpdated(Col_Word map, Col_Word key) {
    size_t size = Col_MapSize(map);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_TrieMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value != 0xdeadbeef);

    value = (value == WORD_FALSE ? WORD_TRUE : WORD_FALSE);
    PICOTEST_ASSERT(Col_TrieMapSet(map, key, value) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == size);
    Col_Word value2 = 0xdeadbeef;
    PICOTEST_ASSERT(Col_TrieMapGet(map, key, &value2) == 1);
    PICOTEST_ASSERT(value2 == value);
}
PICOTEST_CASE(testStringTrieMapSetUpdated, colibriFixture) {
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP(1), STRING_TRIE_KEY(0));
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP(2), STRING_TRIE_KEY(0));
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP(10), STRING_TRIE_KEY(0));
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP(10), STRING_TRIE_KEY(5));
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP(10), STRING_TRIE_KEY(9));
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP(100), STRING_TRIE_KEY(0));
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP(100), STRING_TRIE_KEY(33));
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP(100), STRING_TRIE_KEY(66));
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP(100), STRING_TRIE_KEY(99));
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP_PREFIX(10),
                                 STRING_TRIE_KEY_PREFIX(0));
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP_PREFIX(10),
                                 STRING_TRIE_KEY_PREFIX(5));
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP_PREFIX(10),
                                 STRING_TRIE_KEY_PREFIX(9));
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP_PREFIX(100),
                                 STRING_TRIE_KEY_PREFIX(0));
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP_PREFIX(100),
                                 STRING_TRIE_KEY_PREFIX(33));
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP_PREFIX(100),
                                 STRING_TRIE_KEY_PREFIX(66));
    checkStringTrieMapSetUpdated(STRING_TRIE_MAP_PREFIX(100),
                                 STRING_TRIE_KEY_PREFIX(99));
}

static void checkStringTrieMapSetUpdatedCopy(Col_Word map, Col_Word key) {
    Col_Word copy1 = Col_CopyTrieMap(map);

    checkStringTrieMapSetUpdated(map, key);
    checkStringTrieMapSetUpdated(copy1, key);

    Col_Word copy2 = Col_CopyTrieMap(map);
    Col_Word copy3 = Col_CopyTrieMap(copy1);

    checkStringTrieMapSetUpdated(copy2, key);
    checkStringTrieMapSetUpdated(copy3, key);
}
PICOTEST_CASE(testStringTrieMapSetUpdatedCopy, colibriFixture) {
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP(1), STRING_TRIE_KEY(0));
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP(2), STRING_TRIE_KEY(0));
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP(10), STRING_TRIE_KEY(0));
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP(10), STRING_TRIE_KEY(5));
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP(10), STRING_TRIE_KEY(9));
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP(100), STRING_TRIE_KEY(0));
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP(100), STRING_TRIE_KEY(33));
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP(100), STRING_TRIE_KEY(66));
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP(100), STRING_TRIE_KEY(99));
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP_PREFIX(10),
                                     STRING_TRIE_KEY_PREFIX(0));
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP_PREFIX(10),
                                     STRING_TRIE_KEY_PREFIX(5));
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP_PREFIX(10),
                                     STRING_TRIE_KEY_PREFIX(9));
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP_PREFIX(100),
                                     STRING_TRIE_KEY_PREFIX(0));
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP_PREFIX(100),
                                     STRING_TRIE_KEY_PREFIX(33));
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP_PREFIX(100),
                                     STRING_TRIE_KEY_PREFIX(66));
    checkStringTrieMapSetUpdatedCopy(STRING_TRIE_MAP_PREFIX(100),
                                     STRING_TRIE_KEY_PREFIX(99));
}

PICOTEST_SUITE(testStringTrieMapUnset, testStringTrieMapUnsetEmpty,
               testStringTrieMapUnsetNotFound, testStringTrieMapUnsetFound,
               testStringTrieMapUnsetFoundCopy);
PICOTEST_CASE(testStringTrieMapUnsetEmpty, colibriFixture) {
    Col_Word map = Col_NewStringTrieMap();
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    Col_Word key = Col_NewCharWord('a');
    PICOTEST_ASSERT(Col_TrieMapUnset(map, key) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
}
PICOTEST_CASE(testStringTrieMapUnsetNotFound, colibriFixture) {
    Col_Word map = Col_NewStringTrieMap();
    Col_Word key = Col_NewCharWord('a');
    Col_TrieMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    PICOTEST_ASSERT(Col_TrieMapUnset(map, Col_NewCharWord('b')) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value;
    PICOTEST_ASSERT(Col_TrieMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}

static void checkStringTrieMapUnsetFound(Col_Word map, Col_Word key) {
    size_t size = Col_MapSize(map);
    PICOTEST_ASSERT(Col_TrieMapUnset(map, key) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == size - 1);
    Col_Word value;
    PICOTEST_ASSERT(Col_TrieMapGet(map, key, &value) == 0);
}
PICOTEST_CASE(testStringTrieMapUnsetFound, colibriFixture) {
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP(1), STRING_TRIE_KEY(0));
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP(2), STRING_TRIE_KEY(0));
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP(10), STRING_TRIE_KEY(0));
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP(10), STRING_TRIE_KEY(5));
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP(10), STRING_TRIE_KEY(9));
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP(100), STRING_TRIE_KEY(0));
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP(100), STRING_TRIE_KEY(33));
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP(100), STRING_TRIE_KEY(66));
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP(100), STRING_TRIE_KEY(99));
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP_PREFIX(10),
                                 STRING_TRIE_KEY_PREFIX(0));
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP_PREFIX(10),
                                 STRING_TRIE_KEY_PREFIX(5));
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP_PREFIX(10),
                                 STRING_TRIE_KEY_PREFIX(9));
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP_PREFIX(100),
                                 STRING_TRIE_KEY_PREFIX(0));
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP_PREFIX(100),
                                 STRING_TRIE_KEY_PREFIX(33));
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP_PREFIX(100),
                                 STRING_TRIE_KEY_PREFIX(66));
    checkStringTrieMapUnsetFound(STRING_TRIE_MAP_PREFIX(100),
                                 STRING_TRIE_KEY_PREFIX(99));
}

static void checkStringTrieMapUnsetFoundCopy(Col_Word map, Col_Word key) {
    Col_Word copy1 = Col_CopyTrieMap(map);

    Col_Word copy2 = Col_CopyTrieMap(map);
    checkStringTrieMapUnsetFound(map, key);

    Col_Word copy3 = Col_CopyTrieMap(copy1);
    checkStringTrieMapUnsetFound(copy1, key);
    checkStringTrieMapUnsetFound(copy2, key);
    checkStringTrieMapUnsetFound(copy3, key);
}
PICOTEST_CASE(testStringTrieMapUnsetFoundCopy, colibriFixture) {
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP(1), STRING_TRIE_KEY(0));
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP(2), STRING_TRIE_KEY(0));
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP(10), STRING_TRIE_KEY(0));
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP(10), STRING_TRIE_KEY(5));
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP(10), STRING_TRIE_KEY(9));
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP(100), STRING_TRIE_KEY(0));
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP(100), STRING_TRIE_KEY(33));
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP(100), STRING_TRIE_KEY(66));
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP(100), STRING_TRIE_KEY(99));
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP_PREFIX(10),
                                     STRING_TRIE_KEY_PREFIX(0));
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP_PREFIX(10),
                                     STRING_TRIE_KEY_PREFIX(5));
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP_PREFIX(10),
                                     STRING_TRIE_KEY_PREFIX(9));
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP_PREFIX(100),
                                     STRING_TRIE_KEY_PREFIX(0));
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP_PREFIX(100),
                                     STRING_TRIE_KEY_PREFIX(33));
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP_PREFIX(100),
                                     STRING_TRIE_KEY_PREFIX(66));
    checkStringTrieMapUnsetFoundCopy(STRING_TRIE_MAP_PREFIX(100),
                                     STRING_TRIE_KEY_PREFIX(99));
}

PICOTEST_SUITE(testIntegerTrieMapAccessors, testIntTrieMapGet,
               testIntTrieMapSet, testIntTrieMapUnset);

PICOTEST_SUITE(testIntTrieMapGet, testIntTrieMapGetEmpty,
               testIntTrieMapGetFound, testIntTrieMapGetNotFound);
PICOTEST_CASE(testIntTrieMapGetEmpty, colibriFixture) {
    Col_Word map = Col_NewIntTrieMap();
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntTrieMapGet(map, key, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}
PICOTEST_CASE(testIntTrieMapGetFound, colibriFixture) {
    Col_Word map = Col_NewIntTrieMap();
    intptr_t key = 1;
    Col_IntTrieMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntTrieMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testIntTrieMapGetNotFound, colibriFixture) {
    Col_Word map = Col_NewIntTrieMap();
    intptr_t key = 1;
    Col_IntTrieMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntTrieMapGet(map, 2, &value) == 0);
    PICOTEST_ASSERT(value == 0xdeadbeef);
}

PICOTEST_SUITE(testIntTrieMapSet, testIntTrieMapSetCreated,
               testIntTrieMapSetCreatedCopy, testIntTrieMapSetUpdated,
               testIntTrieMapSetUpdatedCopy);
static void checkIntTrieSetCreatedIndex(Col_Word map, size_t index) {
    size_t size = Col_MapSize(map);
    Col_Word key = INT_TRIE_KEY(index);
    Col_Word value = INT_TRIE_VALUE(index);
    PICOTEST_ASSERT(Col_IntTrieMapSet(map, key, value) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == size + 1);
    Col_Word value2 = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntTrieMapGet(map, key, &value2) == 1);
    PICOTEST_ASSERT(value2 == value);
}
static void checkIntTrieSetCreated(size_t size) {
    for (size_t index = 0; index <= size; index++) {
        Col_Word map = INT_TRIE_MAP_SKIP(size + 1, index);
        checkIntTrieSetCreatedIndex(map, index);
    }
}
PICOTEST_CASE(testIntTrieMapSetCreated, colibriFixture) {
    checkIntTrieSetCreated(0);
    checkIntTrieSetCreated(1);
    checkIntTrieSetCreated(2);
    checkIntTrieSetCreated(3);
    checkIntTrieSetCreated(4);
}

static void checkIntTrieSetCreatedCopy(size_t size) {
    for (size_t index = 0; index <= size; index++) {
        Col_Word map = INT_TRIE_MAP_SKIP(size + 1, index);
        Col_Word copy = Col_CopyTrieMap(map);

        checkIntTrieSetCreatedIndex(map, index);
        checkIntTrieSetCreatedIndex(copy, index);
    }
}
PICOTEST_CASE(testIntTrieMapSetCreatedCopy, colibriFixture) {
    checkIntTrieSetCreatedCopy(0);
    checkIntTrieSetCreatedCopy(1);
    checkIntTrieSetCreatedCopy(2);
    checkIntTrieSetCreatedCopy(3);
    checkIntTrieSetCreatedCopy(4);
}

static void checkIntTrieMapSetUpdated(Col_Word map, Col_Word key) {
    size_t size = Col_MapSize(map);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntTrieMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value != 0xdeadbeef);

    value = (value == WORD_FALSE ? WORD_TRUE : WORD_FALSE);
    PICOTEST_ASSERT(Col_IntTrieMapSet(map, key, value) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == size);
    Col_Word value2 = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntTrieMapGet(map, key, &value2) == 1);
    PICOTEST_ASSERT(value2 == value);
}
PICOTEST_CASE(testIntTrieMapSetUpdated, colibriFixture) {
    checkIntTrieMapSetUpdated(INT_TRIE_MAP(1), INT_TRIE_KEY(0));
    checkIntTrieMapSetUpdated(INT_TRIE_MAP(2), INT_TRIE_KEY(0));
    checkIntTrieMapSetUpdated(INT_TRIE_MAP(10), INT_TRIE_KEY(0));
    checkIntTrieMapSetUpdated(INT_TRIE_MAP(10), INT_TRIE_KEY(5));
    checkIntTrieMapSetUpdated(INT_TRIE_MAP(10), INT_TRIE_KEY(9));
    checkIntTrieMapSetUpdated(INT_TRIE_MAP(100), INT_TRIE_KEY(0));
    checkIntTrieMapSetUpdated(INT_TRIE_MAP(100), INT_TRIE_KEY(33));
    checkIntTrieMapSetUpdated(INT_TRIE_MAP(100), INT_TRIE_KEY(66));
    checkIntTrieMapSetUpdated(INT_TRIE_MAP(100), INT_TRIE_KEY(99));
}

static void checkIntTrieMapSetUpdatedCopy(Col_Word map, Col_Word key) {
    Col_Word copy1 = Col_CopyTrieMap(map);

    checkIntTrieMapSetUpdated(map, key);
    checkIntTrieMapSetUpdated(copy1, key);

    Col_Word copy2 = Col_CopyTrieMap(map);
    Col_Word copy3 = Col_CopyTrieMap(copy1);

    checkIntTrieMapSetUpdated(copy2, key);
    checkIntTrieMapSetUpdated(copy3, key);
}
PICOTEST_CASE(testIntTrieMapSetUpdatedCopy, colibriFixture) {
    checkIntTrieMapSetUpdatedCopy(INT_TRIE_MAP(1), INT_TRIE_KEY(0));
    checkIntTrieMapSetUpdatedCopy(INT_TRIE_MAP(2), INT_TRIE_KEY(0));
    checkIntTrieMapSetUpdatedCopy(INT_TRIE_MAP(10), INT_TRIE_KEY(0));
    checkIntTrieMapSetUpdatedCopy(INT_TRIE_MAP(10), INT_TRIE_KEY(5));
    checkIntTrieMapSetUpdatedCopy(INT_TRIE_MAP(10), INT_TRIE_KEY(9));
    checkIntTrieMapSetUpdatedCopy(INT_TRIE_MAP(100), INT_TRIE_KEY(0));
    checkIntTrieMapSetUpdatedCopy(INT_TRIE_MAP(100), INT_TRIE_KEY(33));
    checkIntTrieMapSetUpdatedCopy(INT_TRIE_MAP(100), INT_TRIE_KEY(66));
    checkIntTrieMapSetUpdatedCopy(INT_TRIE_MAP(100), INT_TRIE_KEY(99));
}

PICOTEST_SUITE(testIntTrieMapUnset, testIntTrieMapUnsetEmpty,
               testIntTrieMapUnsetNotFound, testIntTrieMapUnsetFound,
               testIntTrieMapUnsetFoundCopy);
PICOTEST_CASE(testIntTrieMapUnsetEmpty, colibriFixture) {
    Col_Word map = Col_NewIntTrieMap();
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
    intptr_t key = 1;
    PICOTEST_ASSERT(Col_IntTrieMapUnset(map, key) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == 0);
}
PICOTEST_CASE(testIntTrieMapUnsetNotFound, colibriFixture) {
    Col_Word map = Col_NewIntTrieMap();
    intptr_t key = 1;
    Col_IntTrieMapSet(map, key, WORD_TRUE);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    PICOTEST_ASSERT(Col_IntTrieMapUnset(map, 2) == 0);
    PICOTEST_ASSERT(Col_MapSize(map) == 1);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntTrieMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}

static void checkIntTrieMapUnsetFound(Col_Word map, intptr_t key) {
    size_t size = Col_MapSize(map);
    PICOTEST_ASSERT(Col_IntTrieMapUnset(map, key) == 1);
    PICOTEST_ASSERT(Col_MapSize(map) == size - 1);
    Col_Word value;
    PICOTEST_ASSERT(Col_IntTrieMapGet(map, key, &value) == 0);
}
PICOTEST_CASE(testIntTrieMapUnsetFound, colibriFixture) {
    checkIntTrieMapUnsetFound(INT_TRIE_MAP(1), INT_TRIE_KEY(0));
    checkIntTrieMapUnsetFound(INT_TRIE_MAP(2), INT_TRIE_KEY(0));
    checkIntTrieMapUnsetFound(INT_TRIE_MAP(10), INT_TRIE_KEY(0));
    checkIntTrieMapUnsetFound(INT_TRIE_MAP(10), INT_TRIE_KEY(5));
    checkIntTrieMapUnsetFound(INT_TRIE_MAP(10), INT_TRIE_KEY(9));
    checkIntTrieMapUnsetFound(INT_TRIE_MAP(100), INT_TRIE_KEY(0));
    checkIntTrieMapUnsetFound(INT_TRIE_MAP(100), INT_TRIE_KEY(33));
    checkIntTrieMapUnsetFound(INT_TRIE_MAP(100), INT_TRIE_KEY(66));
    checkIntTrieMapUnsetFound(INT_TRIE_MAP(100), INT_TRIE_KEY(99));
}

static void checkIntTrieMapUnsetFoundCopy(Col_Word map, intptr_t key) {
    Col_Word copy1 = Col_CopyTrieMap(map);

    Col_Word copy2 = Col_CopyTrieMap(map);
    checkIntTrieMapUnsetFound(map, key);

    Col_Word copy3 = Col_CopyTrieMap(copy1);
    checkIntTrieMapUnsetFound(copy1, key);
    checkIntTrieMapUnsetFound(copy2, key);
    checkIntTrieMapUnsetFound(copy3, key);
}
PICOTEST_CASE(testIntTrieMapUnsetFoundCopy, colibriFixture) {
    checkIntTrieMapUnsetFoundCopy(INT_TRIE_MAP(1), INT_TRIE_KEY(0));
    checkIntTrieMapUnsetFoundCopy(INT_TRIE_MAP(2), INT_TRIE_KEY(0));
    checkIntTrieMapUnsetFoundCopy(INT_TRIE_MAP(10), INT_TRIE_KEY(0));
    checkIntTrieMapUnsetFoundCopy(INT_TRIE_MAP(10), INT_TRIE_KEY(5));
    checkIntTrieMapUnsetFoundCopy(INT_TRIE_MAP(10), INT_TRIE_KEY(9));
    checkIntTrieMapUnsetFoundCopy(INT_TRIE_MAP(100), INT_TRIE_KEY(0));
    checkIntTrieMapUnsetFoundCopy(INT_TRIE_MAP(100), INT_TRIE_KEY(33));
    checkIntTrieMapUnsetFoundCopy(INT_TRIE_MAP(100), INT_TRIE_KEY(66));
    checkIntTrieMapUnsetFoundCopy(INT_TRIE_MAP(100), INT_TRIE_KEY(99));
}

PICOTEST_SUITE(testTrieMapIteration, testTrieMapIteratorErrors,
               testTrieMapIteratorInitialize, testTrieMapIteratorAccess,
               testTrieMapIteratorMove, testTrieMapIteratorEmptyMap);

PICOTEST_SUITE(testTrieMapIteratorErrors, testTrieMapIteratorAtEndIsInvalid);
PICOTEST_CASE(testTrieMapIteratorAtEndIsInvalid, colibriFixture) {
    PICOTEST_VERIFY(trieMapIterSetValue_valueCheck(NULL) == 1);
    PICOTEST_VERIFY(trieMapIterNext_valueCheck(NULL) == 1);
}

PICOTEST_SUITE(testTrieMapIteratorInitialize, testTrieMapIterFirst,
               testTrieMapIterLast, testTrieMapIterFind,
               testIntTrieMapIterFind);

static void checkTrieMapIterFirst(Col_Word map) {
    Col_MapIterator it;
    Col_TrieMapIterFirst(it, map);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
}
PICOTEST_CASE(testTrieMapIterFirst, colibriFixture) {
    checkTrieMapIterFirst(STRING_TRIE_MAP(1));
    checkTrieMapIterFirst(STRING_TRIE_MAP(2));
    checkTrieMapIterFirst(STRING_TRIE_MAP(10));
    checkTrieMapIterFirst(STRING_TRIE_MAP(100));
    checkTrieMapIterFirst(STRING_TRIE_MAP_PREFIX(10));
    checkTrieMapIterFirst(STRING_TRIE_MAP_PREFIX(100));
    checkTrieMapIterFirst(INT_TRIE_MAP(1));
    checkTrieMapIterFirst(INT_TRIE_MAP(2));
    checkTrieMapIterFirst(INT_TRIE_MAP(10));
    checkTrieMapIterFirst(INT_TRIE_MAP(100));
}

static void checkTrieMapIterLast(Col_Word map) {
    Col_MapIterator it;
    Col_TrieMapIterLast(it, map);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
}
PICOTEST_CASE(testTrieMapIterLast, colibriFixture) {
    checkTrieMapIterLast(STRING_TRIE_MAP(1));
    checkTrieMapIterLast(STRING_TRIE_MAP(2));
    checkTrieMapIterLast(STRING_TRIE_MAP(10));
    checkTrieMapIterLast(STRING_TRIE_MAP(100));
    checkTrieMapIterLast(STRING_TRIE_MAP_PREFIX(10));
    checkTrieMapIterLast(STRING_TRIE_MAP_PREFIX(100));
    checkTrieMapIterLast(INT_TRIE_MAP(2));
    checkTrieMapIterLast(INT_TRIE_MAP(10));
    checkTrieMapIterLast(INT_TRIE_MAP(100));
}

PICOTEST_SUITE(testTrieMapIterFind, testTrieMapIterFindFound,
               testTrieMapIterFindNotFound, testTrieMapIterFindCreate);

static void checkTrieMapIterFindFound(Col_Word map, Col_Word key) {
    size_t size = Col_MapSize(map);
    Col_MapIterator it;

    Col_TrieMapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(Col_MapSize(map) == size);

    int create = 0;
    Col_TrieMapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(!create);
    PICOTEST_ASSERT(Col_MapSize(map) == size);
}
PICOTEST_CASE(testTrieMapIterFindFound, colibriFixture) {
    checkTrieMapIterFindFound(STRING_TRIE_MAP(1), STRING_TRIE_KEY(0));
    checkTrieMapIterFindFound(STRING_TRIE_MAP(2), STRING_TRIE_KEY(0));
    checkTrieMapIterFindFound(STRING_TRIE_MAP(10), STRING_TRIE_KEY(0));
    checkTrieMapIterFindFound(STRING_TRIE_MAP(10), STRING_TRIE_KEY(5));
    checkTrieMapIterFindFound(STRING_TRIE_MAP(10), STRING_TRIE_KEY(9));
    checkTrieMapIterFindFound(STRING_TRIE_MAP(100), STRING_TRIE_KEY(0));
    checkTrieMapIterFindFound(STRING_TRIE_MAP(100), STRING_TRIE_KEY(33));
    checkTrieMapIterFindFound(STRING_TRIE_MAP(100), STRING_TRIE_KEY(66));
    checkTrieMapIterFindFound(STRING_TRIE_MAP(100), STRING_TRIE_KEY(99));
    checkTrieMapIterFindFound(STRING_TRIE_MAP_PREFIX(10),
                              STRING_TRIE_KEY_PREFIX(0));
    checkTrieMapIterFindFound(STRING_TRIE_MAP_PREFIX(10),
                              STRING_TRIE_KEY_PREFIX(5));
    checkTrieMapIterFindFound(STRING_TRIE_MAP_PREFIX(10),
                              STRING_TRIE_KEY_PREFIX(9));
    checkTrieMapIterFindFound(STRING_TRIE_MAP_PREFIX(100),
                              STRING_TRIE_KEY_PREFIX(0));
    checkTrieMapIterFindFound(STRING_TRIE_MAP_PREFIX(100),
                              STRING_TRIE_KEY_PREFIX(33));
    checkTrieMapIterFindFound(STRING_TRIE_MAP_PREFIX(100),
                              STRING_TRIE_KEY_PREFIX(66));
    checkTrieMapIterFindFound(STRING_TRIE_MAP_PREFIX(100),
                              STRING_TRIE_KEY_PREFIX(99));
}

static void checkTrieMapIterFindNotFound(Col_Word map, Col_Word key) {
    size_t size = Col_MapSize(map);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_TrieMapGet(map, key, &value) == 0);

    Col_MapIterator it;

    Col_TrieMapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    PICOTEST_ASSERT(Col_MapSize(map) == size);
    PICOTEST_ASSERT(Col_TrieMapGet(map, key, &value) == 0);

    int create = 0;
    Col_TrieMapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    PICOTEST_ASSERT(!create);
    PICOTEST_ASSERT(Col_MapSize(map) == size);
    PICOTEST_ASSERT(Col_TrieMapGet(map, key, &value) == 0);
}
PICOTEST_CASE(testTrieMapIterFindNotFound, colibriFixture) {
    checkTrieMapIterFindNotFound(STRING_TRIE_MAP(0), STRING_TRIE_KEY(0));
    checkTrieMapIterFindNotFound(STRING_TRIE_MAP(1), STRING_TRIE_KEY(1));
    checkTrieMapIterFindNotFound(STRING_TRIE_MAP(2), STRING_TRIE_KEY(2));
    checkTrieMapIterFindNotFound(STRING_TRIE_MAP(10), STRING_TRIE_KEY(10));
    checkTrieMapIterFindNotFound(STRING_TRIE_MAP(100), STRING_TRIE_KEY(100));
    checkTrieMapIterFindNotFound(STRING_TRIE_MAP_PREFIX(10),
                                 STRING_TRIE_KEY_PREFIX(10));
    checkTrieMapIterFindNotFound(STRING_TRIE_MAP_PREFIX(100),
                                 STRING_TRIE_KEY_PREFIX(100));
}

static void checkTrieMapIterFindCreate(Col_Word map, Col_Word key) {
    size_t size = Col_MapSize(map);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_TrieMapGet(map, key, &value) == 0);

    Col_MapIterator it;
    int create = 1;
    Col_TrieMapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(create);
    PICOTEST_ASSERT(Col_MapSize(map) == size + 1);
    PICOTEST_ASSERT(Col_TrieMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_NIL);
}
PICOTEST_CASE(testTrieMapIterFindCreate, colibriFixture) {
    checkTrieMapIterFindCreate(STRING_TRIE_MAP(0), STRING_TRIE_KEY(0));
    checkTrieMapIterFindCreate(STRING_TRIE_MAP(1), STRING_TRIE_KEY(1));
    checkTrieMapIterFindCreate(STRING_TRIE_MAP(2), STRING_TRIE_KEY(2));
    checkTrieMapIterFindCreate(STRING_TRIE_MAP(10), STRING_TRIE_KEY(10));
    checkTrieMapIterFindCreate(STRING_TRIE_MAP(100), STRING_TRIE_KEY(100));
    checkTrieMapIterFindCreate(STRING_TRIE_MAP_PREFIX(10),
                               STRING_TRIE_KEY_PREFIX(10));
    checkTrieMapIterFindCreate(STRING_TRIE_MAP_PREFIX(100),
                               STRING_TRIE_KEY_PREFIX(100));
}

PICOTEST_SUITE(testIntTrieMapIterFind, testIntTrieMapIterFindFound,
               testIntTrieMapIterFindNotFound, testIntTrieMapIterFindCreate);

static void checkIntTrieMapIterFindFound(Col_Word map, intptr_t key) {
    size_t size = Col_MapSize(map);
    Col_MapIterator it;

    Col_IntTrieMapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(Col_MapSize(map) == size);

    int create = 0;
    Col_IntTrieMapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(!create);
    PICOTEST_ASSERT(Col_MapSize(map) == size);
}
PICOTEST_CASE(testIntTrieMapIterFindFound, colibriFixture) {
    checkIntTrieMapIterFindFound(INT_TRIE_MAP(1), INT_TRIE_KEY(0));
    checkIntTrieMapIterFindFound(INT_TRIE_MAP(2), INT_TRIE_KEY(0));
    checkIntTrieMapIterFindFound(INT_TRIE_MAP(10), INT_TRIE_KEY(0));
    checkIntTrieMapIterFindFound(INT_TRIE_MAP(10), INT_TRIE_KEY(5));
    checkIntTrieMapIterFindFound(INT_TRIE_MAP(10), INT_TRIE_KEY(9));
    checkIntTrieMapIterFindFound(INT_TRIE_MAP(100), INT_TRIE_KEY(0));
    checkIntTrieMapIterFindFound(INT_TRIE_MAP(100), INT_TRIE_KEY(33));
    checkIntTrieMapIterFindFound(INT_TRIE_MAP(100), INT_TRIE_KEY(66));
    checkIntTrieMapIterFindFound(INT_TRIE_MAP(100), INT_TRIE_KEY(99));
}

static void checkIntTrieMapIterFindNotFound(Col_Word map, intptr_t key) {
    size_t size = Col_MapSize(map);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntTrieMapGet(map, key, &value) == 0);

    Col_MapIterator it;

    Col_IntTrieMapIterFind(it, map, key, NULL);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    PICOTEST_ASSERT(Col_MapSize(map) == size);
    PICOTEST_ASSERT(Col_IntTrieMapGet(map, key, &value) == 0);

    int create = 0;
    Col_IntTrieMapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    PICOTEST_ASSERT(!create);
    PICOTEST_ASSERT(Col_MapSize(map) == size);
    PICOTEST_ASSERT(Col_IntTrieMapGet(map, key, &value) == 0);
}
PICOTEST_CASE(testIntTrieMapIterFindNotFound, colibriFixture) {
    checkIntTrieMapIterFindNotFound(INT_TRIE_MAP(0), INT_TRIE_KEY(0));
    checkIntTrieMapIterFindNotFound(INT_TRIE_MAP(1), INT_TRIE_KEY(1));
    checkIntTrieMapIterFindNotFound(INT_TRIE_MAP(2), INT_TRIE_KEY(2));
    checkIntTrieMapIterFindNotFound(INT_TRIE_MAP(10), INT_TRIE_KEY(10));
    checkIntTrieMapIterFindNotFound(INT_TRIE_MAP(100), INT_TRIE_KEY(100));
}

static void checkIntTrieMapIterFindCreate(Col_Word map, intptr_t key) {
    size_t size = Col_MapSize(map);
    Col_Word value = 0xdeadbeef;
    PICOTEST_ASSERT(Col_IntTrieMapGet(map, key, &value) == 0);

    Col_MapIterator it;
    int create = 1;
    Col_IntTrieMapIterFind(it, map, key, &create);
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapIterMap(it) == map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    PICOTEST_ASSERT(create);
    PICOTEST_ASSERT(Col_IntTrieMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_NIL);
}
PICOTEST_CASE(testIntTrieMapIterFindCreate, colibriFixture) {
    checkIntTrieMapIterFindCreate(INT_TRIE_MAP(0), INT_TRIE_KEY(0));
    checkIntTrieMapIterFindCreate(INT_TRIE_MAP(1), INT_TRIE_KEY(1));
    checkIntTrieMapIterFindCreate(INT_TRIE_MAP(2), INT_TRIE_KEY(2));
    checkIntTrieMapIterFindCreate(INT_TRIE_MAP(10), INT_TRIE_KEY(10));
    checkIntTrieMapIterFindCreate(INT_TRIE_MAP(100), INT_TRIE_KEY(100));
}

PICOTEST_SUITE(testTrieMapIteratorAccess, testTrieMapIterSetValue,
               testTrieMapIterSetValueCopy, testIntTrieMapIterSetValue,
               testIntTrieMapIterSetValueCopy);

static void checkTrieMapIterSetValue(Col_Word map, Col_Word key,
                                     Col_Word oldValue) {
    size_t size = Col_MapSize(map);
    Col_Word value;
    PICOTEST_ASSERT(Col_TrieMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == oldValue);

    Col_MapIterator it;
    Col_TrieMapIterFind(it, map, key, NULL);
    Col_TrieMapIterSetValue(it, WORD_TRUE);
    PICOTEST_ASSERT(Col_TrieMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testTrieMapIterSetValue, colibriFixture) {
    checkTrieMapIterSetValue(STRING_TRIE_MAP(10), STRING_TRIE_KEY(0),
                             STRING_TRIE_VALUE(0));
    checkTrieMapIterSetValue(STRING_TRIE_MAP(10), STRING_TRIE_KEY(5),
                             STRING_TRIE_VALUE(5));
    checkTrieMapIterSetValue(STRING_TRIE_MAP(10), STRING_TRIE_KEY(9),
                             STRING_TRIE_VALUE(9));
    checkTrieMapIterSetValue(STRING_TRIE_MAP(100), STRING_TRIE_KEY(0),
                             STRING_TRIE_VALUE(0));
    checkTrieMapIterSetValue(STRING_TRIE_MAP(100), STRING_TRIE_KEY(33),
                             STRING_TRIE_VALUE(33));
    checkTrieMapIterSetValue(STRING_TRIE_MAP(100), STRING_TRIE_KEY(66),
                             STRING_TRIE_VALUE(66));
    checkTrieMapIterSetValue(STRING_TRIE_MAP(100), STRING_TRIE_KEY(99),
                             STRING_TRIE_VALUE(99));
    checkTrieMapIterSetValue(STRING_TRIE_MAP_PREFIX(10),
                             STRING_TRIE_KEY_PREFIX(0), STRING_TRIE_VALUE(0));
    checkTrieMapIterSetValue(STRING_TRIE_MAP_PREFIX(10),
                             STRING_TRIE_KEY_PREFIX(5), STRING_TRIE_VALUE(5));
    checkTrieMapIterSetValue(STRING_TRIE_MAP_PREFIX(10),
                             STRING_TRIE_KEY_PREFIX(9), STRING_TRIE_VALUE(9));
    checkTrieMapIterSetValue(STRING_TRIE_MAP_PREFIX(100),
                             STRING_TRIE_KEY_PREFIX(0), STRING_TRIE_VALUE(0));
    checkTrieMapIterSetValue(STRING_TRIE_MAP_PREFIX(100),
                             STRING_TRIE_KEY_PREFIX(33), STRING_TRIE_VALUE(33));
    checkTrieMapIterSetValue(STRING_TRIE_MAP_PREFIX(100),
                             STRING_TRIE_KEY_PREFIX(66), STRING_TRIE_VALUE(66));
    checkTrieMapIterSetValue(STRING_TRIE_MAP_PREFIX(100),
                             STRING_TRIE_KEY_PREFIX(99), STRING_TRIE_VALUE(99));
}

static void checkTrieMapIterSetValueCopy(Col_Word map, Col_Word key,
                                         Col_Word oldValue) {
    Col_Word copy1 = Col_CopyTrieMap(map);

    Col_Word copy2 = Col_CopyTrieMap(map);
    checkTrieMapIterSetValue(map, key, oldValue);

    Col_Word copy3 = Col_CopyTrieMap(copy1);
    checkTrieMapIterSetValue(copy1, key, oldValue);
    checkTrieMapIterSetValue(copy2, key, oldValue);
    checkTrieMapIterSetValue(copy3, key, oldValue);
}
PICOTEST_CASE(testTrieMapIterSetValueCopy, colibriFixture) {
    checkTrieMapIterSetValueCopy(STRING_TRIE_MAP(10), STRING_TRIE_KEY(0),
                                 STRING_TRIE_VALUE(0));
    checkTrieMapIterSetValueCopy(STRING_TRIE_MAP(10), STRING_TRIE_KEY(5),
                                 STRING_TRIE_VALUE(5));
    checkTrieMapIterSetValueCopy(STRING_TRIE_MAP(10), STRING_TRIE_KEY(9),
                                 STRING_TRIE_VALUE(9));
    checkTrieMapIterSetValueCopy(STRING_TRIE_MAP(100), STRING_TRIE_KEY(0),
                                 STRING_TRIE_VALUE(0));
    checkTrieMapIterSetValueCopy(STRING_TRIE_MAP(100), STRING_TRIE_KEY(33),
                                 STRING_TRIE_VALUE(33));
    checkTrieMapIterSetValueCopy(STRING_TRIE_MAP(100), STRING_TRIE_KEY(66),
                                 STRING_TRIE_VALUE(66));
    checkTrieMapIterSetValueCopy(STRING_TRIE_MAP(100), STRING_TRIE_KEY(99),
                                 STRING_TRIE_VALUE(99));
    checkTrieMapIterSetValueCopy(STRING_TRIE_MAP_PREFIX(10),
                                 STRING_TRIE_KEY_PREFIX(0),
                                 STRING_TRIE_VALUE(0));
    checkTrieMapIterSetValueCopy(STRING_TRIE_MAP_PREFIX(10),
                                 STRING_TRIE_KEY_PREFIX(5),
                                 STRING_TRIE_VALUE(5));
    checkTrieMapIterSetValueCopy(STRING_TRIE_MAP_PREFIX(10),
                                 STRING_TRIE_KEY_PREFIX(9),
                                 STRING_TRIE_VALUE(9));
    checkTrieMapIterSetValueCopy(STRING_TRIE_MAP_PREFIX(100),
                                 STRING_TRIE_KEY_PREFIX(0),
                                 STRING_TRIE_VALUE(0));
    checkTrieMapIterSetValueCopy(STRING_TRIE_MAP_PREFIX(100),
                                 STRING_TRIE_KEY_PREFIX(33),
                                 STRING_TRIE_VALUE(33));
    checkTrieMapIterSetValueCopy(STRING_TRIE_MAP_PREFIX(100),
                                 STRING_TRIE_KEY_PREFIX(66),
                                 STRING_TRIE_VALUE(66));
    checkTrieMapIterSetValueCopy(STRING_TRIE_MAP_PREFIX(100),
                                 STRING_TRIE_KEY_PREFIX(99),
                                 STRING_TRIE_VALUE(99));
}

static void checkIntTrieMapIterSetValue(Col_Word map, intptr_t key,
                                        Col_Word oldValue) {
    size_t size = Col_MapSize(map);
    Col_Word value;
    PICOTEST_ASSERT(Col_IntTrieMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == oldValue);

    Col_MapIterator it;
    Col_IntTrieMapIterFind(it, map, key, NULL);
    Col_TrieMapIterSetValue(it, WORD_TRUE);
    PICOTEST_ASSERT(Col_IntTrieMapGet(map, key, &value) == 1);
    PICOTEST_ASSERT(value == WORD_TRUE);
}
PICOTEST_CASE(testIntTrieMapIterSetValue, colibriFixture) {
    checkIntTrieMapIterSetValue(INT_TRIE_MAP(10), INT_TRIE_KEY(0),
                                INT_TRIE_VALUE(0));
    checkIntTrieMapIterSetValue(INT_TRIE_MAP(10), INT_TRIE_KEY(5),
                                INT_TRIE_VALUE(5));
    checkIntTrieMapIterSetValue(INT_TRIE_MAP(10), INT_TRIE_KEY(9),
                                INT_TRIE_VALUE(9));
    checkIntTrieMapIterSetValue(INT_TRIE_MAP(100), INT_TRIE_KEY(0),
                                INT_TRIE_VALUE(0));
    checkIntTrieMapIterSetValue(INT_TRIE_MAP(100), INT_TRIE_KEY(33),
                                INT_TRIE_VALUE(33));
    checkIntTrieMapIterSetValue(INT_TRIE_MAP(100), INT_TRIE_KEY(66),
                                INT_TRIE_VALUE(66));
    checkIntTrieMapIterSetValue(INT_TRIE_MAP(100), INT_TRIE_KEY(99),
                                INT_TRIE_VALUE(99));
}

static void checkIntTrieMapIterSetValueCopy(Col_Word map, intptr_t key,
                                            Col_Word oldValue) {
    Col_Word copy1 = Col_CopyTrieMap(map);

    Col_Word copy2 = Col_CopyTrieMap(map);
    checkIntTrieMapIterSetValue(map, key, oldValue);

    Col_Word copy3 = Col_CopyTrieMap(copy1);
    checkIntTrieMapIterSetValue(copy1, key, oldValue);
    checkIntTrieMapIterSetValue(copy2, key, oldValue);
    checkIntTrieMapIterSetValue(copy3, key, oldValue);
}
PICOTEST_CASE(testIntTrieMapIterSetValueCopy, colibriFixture) {
    checkIntTrieMapIterSetValueCopy(INT_TRIE_MAP(10), INT_TRIE_KEY(0),
                                    INT_TRIE_VALUE(0));
    checkIntTrieMapIterSetValueCopy(INT_TRIE_MAP(10), INT_TRIE_KEY(5),
                                    INT_TRIE_VALUE(5));
    checkIntTrieMapIterSetValueCopy(INT_TRIE_MAP(10), INT_TRIE_KEY(9),
                                    INT_TRIE_VALUE(9));
    checkIntTrieMapIterSetValueCopy(INT_TRIE_MAP(100), INT_TRIE_KEY(0),
                                    INT_TRIE_VALUE(0));
    checkIntTrieMapIterSetValueCopy(INT_TRIE_MAP(100), INT_TRIE_KEY(33),
                                    INT_TRIE_VALUE(33));
    checkIntTrieMapIterSetValueCopy(INT_TRIE_MAP(100), INT_TRIE_KEY(66),
                                    INT_TRIE_VALUE(66));
    checkIntTrieMapIterSetValueCopy(INT_TRIE_MAP(100), INT_TRIE_KEY(99),
                                    INT_TRIE_VALUE(99));
}

PICOTEST_SUITE(testTrieMapIteratorMove, testTrieMapIterNext,
               testTrieMapIterPrevious);

PICOTEST_SUITE(testTrieMapIterNext, testTrieMapIterNextTraverse,
               testTrieMapIterNextFromLastIsAtEnd);

static void checkTrieMapIterNextTraverse(Col_Word map) {
    Col_MapIterator it;
    size_t i;
    for (i = 0, Col_TrieMapIterFirst(it, map); !Col_MapIterEnd(it);
         i++, Col_TrieMapIterNext(it))
        ;
    PICOTEST_ASSERT(i == Col_MapSize(map));
}
PICOTEST_CASE(testTrieMapIterNextTraverse, colibriFixture) {
    checkTrieMapIterNextTraverse(STRING_TRIE_MAP(0));
    checkTrieMapIterNextTraverse(STRING_TRIE_MAP(1));
    checkTrieMapIterNextTraverse(STRING_TRIE_MAP(2));
    checkTrieMapIterNextTraverse(STRING_TRIE_MAP(10));
    checkTrieMapIterNextTraverse(STRING_TRIE_MAP(100));
    checkTrieMapIterNextTraverse(INT_TRIE_MAP(0));
    checkTrieMapIterNextTraverse(INT_TRIE_MAP(1));
    checkTrieMapIterNextTraverse(INT_TRIE_MAP(2));
    checkTrieMapIterNextTraverse(INT_TRIE_MAP(10));
    checkTrieMapIterNextTraverse(INT_TRIE_MAP(100));
    checkTrieMapIterNextTraverse(STRING_TRIE_MAP_PREFIX(10));
    checkTrieMapIterNextTraverse(STRING_TRIE_MAP_PREFIX(100));
}
PICOTEST_CASE(testTrieMapIterNextFromLastIsAtEnd, colibriFixture) {
    Col_Word map = STRING_TRIE_MAP(10);
    Col_MapIterator it;

    Col_TrieMapIterLast(it, map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    Col_TrieMapIterNext(it);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
}

PICOTEST_SUITE(testTrieMapIterPrevious, testTrieMapIterPreviousTraverse,
               testTrieMapIterPreviousFromFirstIsAtEnd,
               testTrieMapIterPreviousFromEndIsLast);

static void checkTrieMapIterPreviousTraverse(Col_Word map) {
    Col_MapIterator it;
    size_t i;
    for (i = 0, Col_TrieMapIterLast(it, map); !Col_MapIterEnd(it);
         i++, Col_TrieMapIterPrevious(it))
        ;
    PICOTEST_ASSERT(i == Col_MapSize(map));
}
PICOTEST_CASE(testTrieMapIterPreviousTraverse, colibriFixture) {
    checkTrieMapIterPreviousTraverse(STRING_TRIE_MAP(0));
    checkTrieMapIterPreviousTraverse(STRING_TRIE_MAP(1));
    checkTrieMapIterPreviousTraverse(STRING_TRIE_MAP(2));
    checkTrieMapIterPreviousTraverse(STRING_TRIE_MAP(10));
    checkTrieMapIterPreviousTraverse(STRING_TRIE_MAP(100));
    checkTrieMapIterPreviousTraverse(INT_TRIE_MAP(0));
    checkTrieMapIterPreviousTraverse(INT_TRIE_MAP(1));
    checkTrieMapIterPreviousTraverse(INT_TRIE_MAP(2));
    checkTrieMapIterPreviousTraverse(INT_TRIE_MAP(10));
    checkTrieMapIterPreviousTraverse(INT_TRIE_MAP(100));
    checkTrieMapIterPreviousTraverse(STRING_TRIE_MAP_PREFIX(10));
    checkTrieMapIterPreviousTraverse(STRING_TRIE_MAP_PREFIX(100));
}
PICOTEST_CASE(testTrieMapIterPreviousFromFirstIsAtEnd, colibriFixture) {
    Col_Word map = STRING_TRIE_MAP(10);
    Col_MapIterator it;

    Col_TrieMapIterFirst(it, map);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
    Col_TrieMapIterPrevious(it);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
}
PICOTEST_CASE(testTrieMapIterPreviousFromEndIsLast, colibriFixture) {
    Col_Word map = STRING_TRIE_MAP(10);
    Col_MapIterator it;

    Col_TrieMapIterFirst(it, map);
    Col_TrieMapIterPrevious(it);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
    Col_TrieMapIterPrevious(it);
    PICOTEST_ASSERT(!Col_MapIterEnd(it));
}

PICOTEST_SUITE(testTrieMapIteratorEmptyMap, testEmptyTrieMapIterFirst,
               testEmptyTrieMapIterLast, testEmptyIntTrieMapIterFirst,
               testEmptyIntTrieMapIterLast, testEmptyTrieMapIterFind,
               testEmptyIntTrieMapIterFind);
static void checkMapIterEmpty(Col_MapIterator it) {
    PICOTEST_ASSERT(!Col_MapIterNull(it));
    PICOTEST_ASSERT(Col_MapSize(Col_MapIterMap(it)) == 0);
    PICOTEST_ASSERT(Col_MapIterEnd(it));
}
PICOTEST_CASE(testEmptyTrieMapIterFirst, colibriFixture) {
    Col_Word empty = Col_NewStringTrieMap();
    Col_MapIterator it;

    Col_TrieMapIterFirst(it, empty);
    checkMapIterEmpty(it);
}
PICOTEST_CASE(testEmptyIntTrieMapIterFirst, colibriFixture) {
    Col_Word empty = Col_NewIntTrieMap();
    Col_MapIterator it;

    Col_TrieMapIterFirst(it, empty);
    checkMapIterEmpty(it);
}
PICOTEST_CASE(testEmptyTrieMapIterLast, colibriFixture) {
    Col_Word empty = Col_NewStringTrieMap();
    Col_MapIterator it;

    Col_TrieMapIterLast(it, empty);
    checkMapIterEmpty(it);
}
PICOTEST_CASE(testEmptyIntTrieMapIterLast, colibriFixture) {
    Col_Word empty = Col_NewIntTrieMap();
    Col_MapIterator it;

    Col_TrieMapIterLast(it, empty);
    checkMapIterEmpty(it);
}
PICOTEST_CASE(testEmptyTrieMapIterFind, colibriFixture) {
    Col_Word empty = Col_NewStringTrieMap();
    Col_MapIterator it;

    Col_TrieMapIterFind(it, empty, Col_NewCharWord('a'), NULL);
    checkMapIterEmpty(it);
}
PICOTEST_CASE(testEmptyIntTrieMapIterFind, colibriFixture) {
    Col_Word empty = Col_NewIntTrieMap();
    Col_MapIterator it;

    Col_IntTrieMapIterFind(it, empty, 1, NULL);
    checkMapIterEmpty(it);
}
