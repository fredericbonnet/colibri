#ifndef _COLIBRI_UNITTEST
#define _COLIBRI_UNITTEST

#include <stdio.h>
#include <setjmp.h>

#if defined(_MSC_VER)
#   define snprintf sprintf_s
#   pragma warning(disable: 4100)
#endif

static jmp_buf test_failureEnv;
static int test_errorProc(Col_ErrorLevel level, Col_ErrorDomain domain, int code, va_list args) {
    switch (level) {
	case COL_FATAL:
	    printf("\n[FATAL] ");
	    break;

	case COL_ERROR:
	    printf("\n[ERROR] ");
	    break;

	case COL_TYPECHECK:
	    printf("\n[TYPECHECK] ");
	    break;

	case COL_VALUECHECK:
	    printf("\n[VALUECHECK] ");
	    break;
    }
    vprintf(domain[code], args);
    printf("\n");

    longjmp(test_failureEnv, 1);
}
static void test_failure(const char *format, ...) {
    va_list args;
    va_start(args, format);

    printf("\n[ASSERTION] ");
    vprintf(format, args);
    printf("\n");

    va_end(args);

    longjmp(test_failureEnv, 1);
}

#define ERROR_PROC test_errorProc

#define ASSERT(x) {if (!(x)) test_failure("%s(%d) : %s", __FILE__, __LINE__, #x);}
#define ASSERT_MSG(x, msg, ...) {if (!(x)) test_failure("%s(%d) : %s | " msg, __FILE__, __LINE__, #x, __VA_ARGS__);}

typedef struct TestDescr {
    const char * name;
    void (*test)(const char *);
    int (*testCaseRunner)(const char *);
} TestDescr;

#define TEST_SUITE_DECLARE_TEST_CASE(_testName) \
    {COL_STRINGIZE(_testName), _testName, _testName##_testCaseRunner},

#define TEST_SUITE_DECLARE_TEST(_testName) \
    void _testName(const char *); \
    int _testName##_testCaseRunner(const char *);

#define TEST_SUITE(_suiteName, ...) \
    COL_FOR_EACH(TEST_SUITE_DECLARE_TEST,__VA_ARGS__) \
    static TestDescr _suiteName##_tests[] = { \
	COL_FOR_EACH(TEST_SUITE_DECLARE_TEST_CASE,__VA_ARGS__) \
	NULL}; \
    int _suiteName##_testCaseRunner(const char * prefix) { \
	TestDescr * test=_suiteName##_tests; \
	const int nb=sizeof(_suiteName##_tests)/sizeof(*_suiteName##_tests)-1; \
	char buf[256]; \
	int fail=0; \
	printf("%s%s (%d subtests) ...\n", prefix, COL_STRINGIZE(_suiteName), nb); fflush(stdout); \
	for (; test->name; test++) { \
	    snprintf(buf, 256, "\t%s%d. ", prefix, test - _suiteName##_tests + 1); \
	    fail += test->testCaseRunner(buf); \
	} \
	printf("%s%s : %d failed\n", prefix, COL_STRINGIZE(_suiteName), fail); fflush(stdout); \
	return fail; \
    } \
    void _suiteName(const char *name) { \
	if (!name || strcmp(name, COL_STRINGIZE(_suiteName)) == 0) { \
	    _suiteName##_testCaseRunner(""); \
	} else { \
	    TestDescr * test=_suiteName##_tests; \
	    for (; test->name; test++) { \
		if (strcmp(name, test->name) == 0) test->test(NULL); \
	    } \
	} \
    }

#define TEST_FIXTURE_CONTEXT(_fixtureName) \
    struct _fixtureName##_Context

#define TEST_FIXTURE_SETUP(...) \
    COL_CONCATENATE(TEST_FIXTURE_SETUP_,COL_ARGCOUNT(__VA_ARGS__)) COL_LEFT_PAREN __VA_ARGS__ COL_RIGHT_PAREN
#define TEST_FIXTURE_SETUP_1(_fixtureName) \
    static void _fixtureName##_setup(void * _fixtureName##_DUMMY)
#define TEST_FIXTURE_SETUP_2(_fixtureName, _context) \
    static void _fixtureName##_setup(struct _fixtureName##_Context * _context)

#define TEST_FIXTURE_TEARDOWN(...) \
    COL_CONCATENATE(TEST_FIXTURE_TEARDOWN_,COL_ARGCOUNT(__VA_ARGS__)) COL_LEFT_PAREN __VA_ARGS__ COL_RIGHT_PAREN
#define TEST_FIXTURE_TEARDOWN_1(_fixtureName) \
    static void _fixtureName##_teardown(int TEST_FAIL, void * _fixtureName##_DUMMY)
#define TEST_FIXTURE_TEARDOWN_2(_fixtureName, _context) \
    static void _fixtureName##_teardown(int TEST_FAIL, struct _fixtureName##_Context * _context)

#define TEST_CASE(...) \
    COL_CONCATENATE(TEST_CASE_,COL_ARGCOUNT(__VA_ARGS__)) COL_LEFT_PAREN __VA_ARGS__ COL_RIGHT_PAREN

#define TEST_CASE_1(_testName) \
    static void _testName##_testCase(void); \
    int _testName##_testCaseRunner(const char * prefix) { \
	int fail; \
	printf("%s%s ... ", prefix, COL_STRINGIZE(_testName)); fflush(stdout); \
	fail = setjmp(test_failureEnv); \
	if (!fail) { \
	    _testName##_testCase(); \
	    printf("passed\n"); fflush(stdout); \
	} \
	return fail; \
    } \
    void _testName(const char *name) { \
	if (!name || strcmp(name, COL_STRINGIZE(_testName)) == 0) { \
	    _testName##_testCaseRunner(""); \
	} \
    } \
    static void _testName##_testCase(void)

#define TEST_CASE_2(_testName, _fixtureName) \
    static void _testName##_testCase(); \
    int _testName##_testCaseRunner(const char * prefix) { \
	int fail; \
	printf("%s%s ... ", prefix, COL_STRINGIZE(_testName)); fflush(stdout); \
	fail = setjmp(test_failureEnv); \
	if (!fail) { \
	    _fixtureName##_setup(NULL); \
	    _testName##_testCase(); \
	    printf("passed\n"); fflush(stdout); \
	} \
	_fixtureName##_teardown(fail, NULL); \
	return fail; \
    } \
    void _testName(const char *name) { \
	if (!name || strcmp(name, COL_STRINGIZE(_testName)) == 0) { \
	    _testName##_testCaseRunner(""); \
	} \
    } \
    static void _testName##_testCase()

#define TEST_CASE_3(_testName, _fixtureName, _context) \
    static void _testName##_testCase(struct _fixtureName##_Context *); \
    int _testName##_testCaseRunner(const char * prefix) { \
	struct _fixtureName##_Context context; \
	int fail; \
	printf("%s%s ... ", prefix, COL_STRINGIZE(_testName)); fflush(stdout); \
	fail = setjmp(test_failureEnv); \
	if (!fail) { \
	    _fixtureName##_setup(&context); \
	    _testName##_testCase(&context); \
	    printf("passed\n"); fflush(stdout); \
	} \
	_fixtureName##_teardown(fail, &context); \
	return fail; \
    } \
    void _testName(const char *name) { \
	if (!name || strcmp(name, COL_STRINGIZE(_testName)) == 0) { \
	    _testName##_testCaseRunner(""); \
	} \
    } \
    static void _testName##_testCase(struct _fixtureName##_Context * _context)

#endif /* _COLIBRI_UNITTEST */
