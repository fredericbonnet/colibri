/**
 * @file unitTest.h
 *
 * This file defines a minimalist unit testing framework for C programs.
 * 
 * It relies on setjmp()/longjmp(), which is OK since it's only supposed to be
 * used in a test environment and not a production application. Moreover,
 * longjmp() is only called upon failed assertions, a situation where the actual
 * process state is no longer reliable anyway.
 */

#ifndef _COLIBRI_UNITTEST
#define _COLIBRI_UNITTEST

#include <stdio.h>
#include <setjmp.h>

#if defined(_MSC_VER)
#   define snprintf sprintf_s
#   pragma warning(disable: 4100)
#endif

/**
 * Signature of test functions.
 * 
 * @param Name of test to execute, on NULL. Useful with test suites.
 * 
 * @return Number of failed tests.
 * 
 * @see TEST_SUITE
 * @see TEST_CASE
 */
typedef int (TestProc) (const char * name);

/**
 * Function signature of test failure log handlers.
 * 
 * @param file  File name where the test was defined.
 * @param line  Location of test in file.
 * @param type  Type of test that failed (e.g. "ASSERTION").
 * @param test  Tested expression.
 * @param msg   Optional message format string, or NULL.
 * @patam args  Optional message string parameter list, or NULL.
 * 
 * @note *msg* and *args* are suitable arguments to vprintf().
 */
typedef void (TestFailureLogProc)(const char *file, int line, const char *type,
    const char *test, const char *msg, va_list args);

/**
 * Abort a test case.
 * 
 * @see TEST_CASE
 * @see test_failureEnv
 */
#define TEST_ABORT() \
    longjmp(test_failureEnv, 1)

/** @internal
 * Tag used by setjmp() and longjmp() to jump out of failed tests.
 * 
 * @see TEST_ABORT
 * @see TEST_CASE
 */
static jmp_buf test_failureEnv;

/** @internal
 * 
 * Called when an assertion fails.
 * 
 * @param proc  Test failure log handler.
 * @param file  File name where the test was defined.
 * @param line  Location of test in file.
 * @param type  Type of test that failed (e.g. "ASSERTION").
 * @param count Number of arguments after *test*.
 * @param test  Tested expression.
 * @param ...   Optional message format string and parameters.
 */
static void test_assertFailed(TestFailureLogProc *proc, const char *file, int line, const char *type, int count, const char *test, ...) {
    if (count > 1) {
        /* Extra args after *test* */
        va_list args;
        const char *msg;
        va_start(args, test);
        msg = va_arg(args, const char *);
        proc(file, line, type, test, msg, args);
    } else {
        proc(file, line, type, test, NULL, NULL);
    }
}

/** @internal
 * Test descriptor for test suites.
 * 
 * @see TEST_SUITE
 */
typedef struct TestDescr {
    const char * name;                      /*!< Test name. */
    int (*test)(const char *);              /*!< Pointer to test function. */
    int (*testCaseRunner)(const char *);    /*!< Pointer to test runner. */
} TestDescr;

/** @internal
 * Utility to declare a test case in a suite.
 * 
 * @see TEST_SUITE
 */
#define TEST_SUITE_DECLARE_TEST_CASE(_testName) \
    {COL_STRINGIZE(_testName), _testName, _testName##_testCaseRunner},

/** @internal
 * Utility to declare a test case in a suite.
 * 
 * @see TEST_SUITE
 */
#define TEST_SUITE_DECLARE_TEST(_testName) \
    int _testName(const char *); \
    int _testName##_testCaseRunner(const char *);

/**
 * Test fixture context declaration.
 * 
 * The block following the macro call defines a struct used to hold a test 
 * fixture context. For example:
 * 
 * @code
 *		TEST_FIXTURE_CONTEXT(fixture) {
 *			int var1;
 *			const char *var2;
 *			void *var3;
 *		};
 * @endcode
 * 
 * @param _fixtureName      Name of the fixture.
 * 
 * @see TEST_FIXTURE_SETUP
 * @see TEST_FIXTURE_TEARDOWN
 */
#define TEST_FIXTURE_CONTEXT(_fixtureName) \
    struct _fixtureName##_Context

/**
 * Test fixture context initialization.
 * 
 * The code block following the macro call initializes the test fixture context.
 * For example:
 * 
 * @code
 *		TEST_FIXTURE_SETUP(fixture) {
 *          // Seed the random number generator.
 *			srand(1234);
 *		}
 * @endcode
 * 
 * The test fixture can optionally define a context structure:
 *  
 * @code
 *		TEST_FIXTURE_CONTEXT(fixture) {
 *			int var1;
 *			const char *var2;
 *			void *var3;
 *		};
 *		TEST_FIXTURE_SETUP(fixture, context) {
 *          // Initialize context.
 *			context->var1 = 1;
 *			context->var2 = "test";
 *			context->var3 = (void *) malloc(1);
 *		}
 * @endcode
 * 
 * The code block is called before each test of the fixture.
 * 
 * @param _fixtureName      Name of the fixture.
 * @param _context          (Optional) Fixture context structure.
 * 
 * @see TEST_FIXTURE_CONTEXT
 * @see TEST_FIXTURE_TEARDOWN
 */
#if defined(COL_PARENS)
#   define TEST_FIXTURE_SETUP(...) \
        COL_CONCATENATE(TEST_FIXTURE_SETUP_,COL_ARGCOUNT(__VA_ARGS__)) COL_PARENS(__VA_ARGS__)
#else
#   define TEST_FIXTURE_SETUP(...) \
        COL_CONCATENATE(TEST_FIXTURE_SETUP_,COL_ARGCOUNT(__VA_ARGS__))(__VA_ARGS__)
#endif

#define TEST_FIXTURE_SETUP_1(_fixtureName) \
    static void _fixtureName##_setup(void * _fixtureName##_DUMMY)

#define TEST_FIXTURE_SETUP_2(_fixtureName, _context) \
    static void _fixtureName##_setup(struct _fixtureName##_Context * _context)

/**
 * Test fixture context cleanup.
 * 
 * The code block following the macro call cleans up the test fixture context.
 * For example:
 * 
 * @code
 *		TEST_FIXTURE_SETUP(fixture) {
 *          // Seed the random number generator.
 *			srand(1234);
 *		}
 *		TEST_FIXTURE_TEARDOWN(fixture) {
 *          // Reset the random number generator to a non-deterministic value.
 *			srand(time(NULL));
 *		}
 * @endcode
 * 
 * The test fixture can optionally define a context structure:
 *  
 * @code
 *		TEST_FIXTURE_CONTEXT(fixture) {
 *			int var1;
 *			const char *var2;
 *			void *var3;
 *		};
 *		TEST_FIXTURE_SETUP(fixture, context) {
 *          // Initialize context.
 *			context->var1 = 1;
 *			context->var2 = "test";
 *			context->var3 = (void *) malloc(1);
 *		}
 *		TEST_FIXTURE_TEARDOWN(fixture, context) {
 *			free(context->var3);
 *		}
 * @endcode
 * 
 * The code block is called after each test of the fixture, even if the test 
 * failed. The special variable TEST_FAIL can be used to test for such cases:
 * - for simple test cases it returns 1 in case of failure,
 * - for test suites it returns the number of failed subtests.
 * For example: 
 * 
 * @code
 *      TEST_FIXTURE_CONTEXT(fixture) {
 *          void *buffer;
 *          int size;
 *      }
 *      TEST_FIXTURE_SETUP(fixture, context) {
 *          context->buffer = NULL;
 *          context->size = 1024;
 *      }
 *      TEST_CASE(testInitBuffer, fixture, context) {
 *          context->buffer = malloc(context->size);
 *          ASSERT(context->buffer);
 *          memset(context->buffer, 0, context->size);
 *      }
 *      TEST_FIXTURE_TEARDOWN(fixture, context) {
 *          if (!TEST_FAIL) {
 *              free(context->buffer);
 *          }
 *      }
 * @endcode
 * 
 * @param _fixtureName      Name of the fixture.
 * @param _context          (Optional) Fixture context structure.
 * 
 * @see TEST_FIXTURE_CONTEXT
 * @see TEST_FIXTURE_SETUP
 */
// TODO rename TEST_FAIL to TEST_FAILED / TEST_FAILURE?
#if defined(COL_PARENS)
#   define TEST_FIXTURE_TEARDOWN(...) \
        COL_CONCATENATE(TEST_FIXTURE_TEARDOWN_,COL_ARGCOUNT(__VA_ARGS__)) COL_PARENS(__VA_ARGS__)
#else
#   define TEST_FIXTURE_TEARDOWN(...) \
        COL_CONCATENATE(TEST_FIXTURE_TEARDOWN_,COL_ARGCOUNT(__VA_ARGS__))(__VA_ARGS__)
#endif

#define TEST_FIXTURE_TEARDOWN_1(_fixtureName) \
    static void _fixtureName##_teardown(int TEST_FAIL, void * _fixtureName##_DUMMY)

#define TEST_FIXTURE_TEARDOWN_2(_fixtureName, _context) \
    static void _fixtureName##_teardown(int TEST_FAIL, struct _fixtureName##_Context * _context)

/**
 * Test suite declaration.
 * 
 * A test suite is a test function that is made of one or several subtests.
 * 
 * This macro defines a TestProc of the given name that can be called directly.
 * 
 * @param _suiteName    Name of the test suite.
 * @param ...           Names of the subtests in the suite.
 * 
 * @return Number of failed tests.
 * 
 * @see TestProc
 * @see TEST_CASE
 */ 
#define TEST_SUITE(_suiteName, ...) \
    COL_FOR_EACH(TEST_SUITE_DECLARE_TEST,__VA_ARGS__) \
    static TestDescr _suiteName##_tests[] = { \
        COL_FOR_EACH(TEST_SUITE_DECLARE_TEST_CASE,__VA_ARGS__) \
        {NULL, NULL, NULL}}; \
    int _suiteName##_testCaseRunner(const char * prefix) { \
        TestDescr * test=_suiteName##_tests; \
        const int nb=sizeof(_suiteName##_tests)/sizeof(*_suiteName##_tests)-1; \
        char buf[256]; \
        int fail=0; \
        printf("%s%s (%d subtests) ...\n", prefix, COL_STRINGIZE(_suiteName), nb); fflush(stdout); \
        for (; test->name; test++) { \
            snprintf(buf, 256, "\t%s%d. ", prefix, (int) (test - _suiteName##_tests + 1)); \
            fail += test->testCaseRunner(buf); \
        } \
        printf("%s%s : %d failed\n", prefix, COL_STRINGIZE(_suiteName), fail); fflush(stdout); \
        return fail; \
    } \
    int _suiteName(const char *name) { \
        int fail=0; \
        if (!name || strcmp(name, COL_STRINGIZE(_suiteName)) == 0) { \
            fail += _suiteName##_testCaseRunner(""); \
        } else { \
            TestDescr * test=_suiteName##_tests; \
            for (; test->name; test++) { \
                fail += test->test(name); \
            } \
        } \
        return fail; \
    }

/**
 * Test case declaration.
 * 
 * This macro defines a TestProc of the given name that can be called directly.
 * 
 * @param _testName     Name of the test case.
 * @param _fixtureName  (Optional) Name of the test fixture used by the test.
 * @param _context      (Optional) Fixture context structure.
 * 
 * @return Number of failed tests.
 * 
 * @see TestProc
 * @see TEST_FIXTURE_CONTEXT
 */ 
#if defined(COL_PARENS)
#   define TEST_CASE(...) \
        COL_CONCATENATE(TEST_CASE_,COL_ARGCOUNT(__VA_ARGS__)) COL_PARENS(__VA_ARGS__)
#else
#   define TEST_CASE(...) \
        COL_CONCATENATE(TEST_CASE_,COL_ARGCOUNT(__VA_ARGS__))(__VA_ARGS__)
#endif

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
    int _testName(const char *name) { \
        int fail=0; \
        if (!name || strcmp(name, COL_STRINGIZE(_testName)) == 0) { \
            fail += _testName##_testCaseRunner(""); \
        } \
        return fail; \
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
    int _testName(const char *name) { \
        int fail=0; \
        if (!name || strcmp(name, COL_STRINGIZE(_testName)) == 0) { \
            fail += _testName##_testCaseRunner(""); \
        } \
        return fail; \
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
    int _testName(const char *name) { \
        int fail=0; \
        if (!name || strcmp(name, COL_STRINGIZE(_testName)) == 0) { \
            _testName##_testCaseRunner(""); \
        } \
        return fail; \
    } \
    static void _testName##_testCase(struct _fixtureName##_Context * _context)

/**
 * Throws an error if the given value is false.
 * 
 * @param x     Value to test. Evaluated once, so it can be an expression with 
 *              side effects.
 * @param msg   (optional) Format string passed to vprintf()
 * @maram ...   Remaining arguments passed to vprintf()
 * 
 * @see test_failure TODO
 * @see ASSERT_MSG
 */
#define ASSERT(x, /* msg, */ ...) \
    _ASSERT(x, #x, __VA_ARGS__)

#define _ASSERT(x, ...) \
    {if (!(x)) { test_assertFailed(TEST_LOG_FAILURE, __FILE__, __LINE__, \
        "ASSERTION", COL_ARGCOUNT(__VA_ARGS__), __VA_ARGS__); TEST_ABORT(); } }

#endif /* _COLIBRI_UNITTEST */
