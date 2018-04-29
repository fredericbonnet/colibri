/**
 * @file picotest.h
 *
 * This file defines a minimalist unit testing framework for C programs.
 * 
 * It relies on setjmp()/longjmp(), which is OK since it's only supposed to be
 * used in a test environment and not a production application. Moreover,
 * longjmp() is only called upon failed assertions, a situation where the actual
 * process state is no longer reliable anyway.
 */

#ifndef _PICOTEST
#define _PICOTEST

#include <setjmp.h>

/**
 * Signature of test functions.
 * 
 * @param name  Name of test to execute, on NULL. Useful with test suites.
 * 
 * @return Number of failed tests.
 * 
 * @see TEST_SUITE
 * @see TEST_CASE
 */
typedef int (TestProc) (const char * name);

/**
 * Function signature of test traversal proc.
 * 
 * @param name  Name of traversed test.
 * @param nb    Number of subtests.
 * 
 * @see TEST_TRAVERSE
 */
typedef void (TestTraverseProc)(const char *name, int nb);

/**
 * Traverse a test hierarchy.
 * 
 * @param _testName     Name of the traversed test.
 * @param _proc         Test traversal proc.
 * 
 * @see TestTraverseProc
 */
#define TEST_TRAVERSE(_testName, _proc) \
    _testName##_traverse(_proc)

/**
 * Function signature of test failure log handlers.
 * 
 * @param file  File name where the test was defined.
 * @param line  Location of test in file.
 * @param type  Type of test that failed (e.g. "ASSERTION").
 * @param test  Tested expression.
 * @param msg   Optional message format string, or NULL.
 * @param args  Optional message string parameter list, or NULL.
 * 
 * @note *msg* and *args* are suitable arguments to vprintf().
 * 
 * @see TEST_FAILURE_LOGGER
 */
typedef void (TestFailureLoggerProc)(const char *file, int line, 
    const char *type, const char *test, const char *msg, va_list args);

/** @internal
 * Default test failure log handler. Does nothing.
 * 
 * @see TestFailureLoggerProc
 * @see TEST_FAILURE_LOGGER
 */
static void test_logFailure(const char *file, int line, const char *type,
    const char *test, const char *msg, va_list args) {}

/**
 * Define the test failure log handler.
 * 
 * Called when a test failed.
 * 
 * The default handler does nothing. Redefine this macro to use a custom
 * handler.
 * 
 * @see TestFailureLoggerProc
 */
#define TEST_FAILURE_LOGGER test_logFailure

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
static void test_assertFailed(TestFailureLoggerProc *proc, const char *file, 
        int line, const char *type, int count, const char *test, ...) {
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
    const char * name;                          /*!< Test name. */
    int (*test)(const char *);                  /*!< Test function. */
    void (*traverse)(TestTraverseProc *proc);   /*!< Test traversal. */
} TestDescr;

/** @internal
 * Utility to declare a test case in a suite.
 * 
 * @see TEST_SUITE
 */
#define TEST_SUITE_DECLARE_TEST_CASE(_testName) \
    {COL_STRINGIZE(_testName), _testName, _testName##_traverse},

/** @internal
 * Utility to declare a test case in a suite.
 * 
 * @see TEST_SUITE
 */
#define TEST_SUITE_DECLARE_TEST(_testName) \
    int _testName(const char *); \
    void _testName##_traverse(TestTraverseProc *); \

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
#if defined(COL_PARENS)
#   define TEST_FIXTURE_TEARDOWN(...) \
        COL_CONCATENATE(TEST_FIXTURE_TEARDOWN_,COL_ARGCOUNT(__VA_ARGS__)) COL_PARENS(__VA_ARGS__)
#else
#   define TEST_FIXTURE_TEARDOWN(...) \
        COL_CONCATENATE(TEST_FIXTURE_TEARDOWN_,COL_ARGCOUNT(__VA_ARGS__))(__VA_ARGS__)
#endif

#define TEST_FIXTURE_TEARDOWN_1(_fixtureName) \
    static void _fixtureName##_teardown(int TEST_FAIL, \
        void * _fixtureName##_DUMMY)

#define TEST_FIXTURE_TEARDOWN_2(_fixtureName, _context) \
    static void _fixtureName##_teardown(int TEST_FAIL, \
        struct _fixtureName##_Context * _context)

/**
 * Function signature of test suite enter hooks.
 * 
 * Called before running the first subtest.
 * 
 * @param suiteName     Test suite name.
 * @param nb            Number of subtests.
 * 
 * @see TEST_SUITE_ENTER
 */
typedef void (TestSuiteEnterProc)(const char *suiteName, int nb);

/** @internal
 * Default test suite enter hook. Does nothing.
 * 
 * @see TestSuiteEnterProc
 * @see TEST_SUITE_ENTER
 */
static void test_enterTestSuite(const char *suiteName, int nb) {}

/**
 * Define the test suite enter hook.
 * 
 * The default hook does nothing. Redefine this macro to use a custom hook.
 * 
 * @see TestSuiteEnterProc
 * @see TEST_SUITE_LEAVE
 */
#define TEST_SUITE_ENTER test_enterTestSuite

/**
 * Function signature of test suite leave hooks.
 * 
 * @param suiteName     Test suite name.
 * @param nb            Number of subtests.
 * @param fail          Number of failed subtests (including the subtests' 
 *                      subtests if any).
 * 
 * @see TEST_SUITE_LEAVE
 */
typedef void (TestSuiteLeaveProc)(const char *suiteName, int nb, int fail);

/** @internal
 * Default test suite leave hook. Does nothing.
 * 
 * @see TestSuiteLeaveProc
 * @see TEST_SUITE_LEAVE
 */
static void test_leaveTestSuite(const char *suiteName, int nb, int fail) {}

/**
 * Define the test suite leave hook.
 * 
 * Called after running all subtests.
 * 
 * The default hook does nothing. Redefine this macro to use a custom hook.
 * 
 * @see TestSuiteLeaveProc
 * @see TEST_SUITE_ENTER
 */
#define TEST_SUITE_LEAVE test_leaveTestSuite

/**
 * Function signature of test suite before subtest hooks.
 * 
 * Called before running each subtest.
 * 
 * @param suiteName     Test suite name.
 * @param nb            Number of subtests.
 * @param index         Index of subtest.
 * @param testName      Name of subtest.
 * @param fail          Failed test suite subtests so far  (including its 
 *                      subtests' subtests if any).
 * 
 * @see TEST_SUITE_BEFORE_SUBTEST
 */
typedef void (TestSuiteBeforeSubtestProc)(const char *suiteName, int nb, 
    int index, const char *testName, int fail);

/** @internal
 * Default test suite before subtest hook. Does nothing.
 * 
 * @see TestSuiteBeforeSubtestProc
 * @see TEST_SUITE_BEFORE_SUBTEST
 */
static void test_beforeSubtest(const char *suiteName, int nb, int index, 
    const char *testName, int fail) {}

/**
 * Define the test suite before subset hook.
 * 
 * The default hook does nothing. Redefine this macro to use a custom hook.
 * 
 * @see TestSuiteBeforeSubtestProc
 * @see TEST_SUITE_AFTER_SUBTEST
 */
#define TEST_SUITE_BEFORE_SUBTEST test_beforeSubtest

/**
 * Function signature of test suite after subtest hooks.
 * 
 * Called before running each subtest.
 * 
 * @param suiteName     Test suite name.
 * @param nb            Number of subtests.
 * @param index         Index of subtest.
 * @param testName      Name of subtest.
 * @param fail          Failed test suite subtests so far (including its 
 *                      subtests' subtests if any).
 * @param sfail         The subtest's failed tests (including its subtests if
 *                      any).
 * 
 * @see TEST_SUITE_AFTER_SUBTEST
 */
typedef void (TestSuiteAfterSubtestProc)(const char *suiteName, int nb, 
    int index, const char *testName, int fail, int sfail);

/** @internal
 * Default test suite after subtest hook. Does nothing.
 * 
 * @see TestSuiteAfterSubtestProc
 * @see TEST_SUITE_AFTER_SUBTEST
 */
static void test_afterSubtest(const char *suiteName, int nb, int index, 
    const char *testName, int fail, int sfail) {}

/**
 * Define the test suite after subset hook.
 * 
 * The default hook does nothing. Redefine this macro to use a custom hook.
 * 
 * @see TestSuiteAfterSubtestProc
 * @see TEST_SUITE_BEFORE_SUBTEST
 */
#define TEST_SUITE_AFTER_SUBTEST test_afterSubtest

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
    void _suiteName##_traverse(TestTraverseProc *proc) { \
        const int nb=sizeof(_suiteName##_tests)/sizeof(*_suiteName##_tests)-1; \
        TestDescr * test=_suiteName##_tests; \
        proc(COL_STRINGIZE(_suiteName), nb); \
        for (; test->name; test++) { \
            test->traverse(proc); \
        } \
    } \
    int _suiteName##_testCaseRunner() { \
        const int nb=sizeof(_suiteName##_tests)/sizeof(*_suiteName##_tests)-1; \
        TestDescr * test=_suiteName##_tests; \
        int fail=0; \
        TEST_SUITE_ENTER(COL_STRINGIZE(_suiteName), nb); \
        for (; test->name; test++) { \
            const int index=(int) (test - _suiteName##_tests); \
            int sfail=0; \
            TEST_SUITE_BEFORE_SUBTEST(COL_STRINGIZE(_suiteName), nb, index, \
                test->name, fail); \
            sfail = test->test(NULL); \
            fail += sfail; \
            TEST_SUITE_AFTER_SUBTEST(COL_STRINGIZE(_suiteName), nb, index, \
                test->name, fail, sfail); \
        } \
        TEST_SUITE_LEAVE(COL_STRINGIZE(_suiteName), nb, fail); \
        return fail; \
    } \
    int _suiteName(const char *name) { \
        int fail=0; \
        if (!name || strcmp(name, COL_STRINGIZE(_suiteName)) == 0) { \
            fail += _suiteName##_testCaseRunner(); \
        } else { \
            TestDescr * test=_suiteName##_tests; \
            for (; test->name; test++) { \
                fail += test->test(name); \
            } \
        } \
        return fail; \
    }

/**
 * Function signature of test case enter hooks.
 * 
 * Called before running the test case.
 * 
 * @param testName      Test case name.
 * 
 * @see TEST_CASE_ENTER
 */
typedef void (TestCaseEnterProc)(const char *testName);

/** @internal
 * Default test case enter hook. Does nothing.
 * 
 * @see TestCaseEnterProc
 * @see TEST_CASE_ENTER
 */
static void test_enterTestCase(const char *testName) {}

/**
 * Define the test case enter hook.
 * 
 * The default hook does nothing. Redefine this macro to use a custom hook.
 * 
 * @see TestCaseEnterProc
 * @see TEST_CASE_LEAVE
 */
#define TEST_CASE_ENTER test_enterTestCase

/**
 * Function signature of test case leave hooks.
 * 
 * Called after running the test case.
 * 
 * @param testName      Test case name.
 * @param fail          Failed tests (including its subtests if any).
 * 
 * @see TEST_CASE_LEAVE
 */
typedef void (TestCaseLeaveProc)(const char *testName, int fail);

/** @internal
 * Default test case enter hook. Does nothing.
 * 
 * @see TestCaseLeaveProc
 * @see TEST_CASE_LEAVE
 */
static void test_leaveTestCase(const char *testName, int fail) {}

/**
 * Define the test case enter hook.
 * 
 * The default hook does nothing. Redefine this macro to use a custom hook.
 * 
 * @see TestCaseLeaveProc
 * @see TEST_CASE_ENTER
 */
#define TEST_CASE_LEAVE test_leaveTestCase

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

#define TEST_CASE_DECLARE(_testName) \
    int _testName##_testCaseRunner(void); \
    void _testName##_traverse(TestTraverseProc *proc) { \
        proc(COL_STRINGIZE(_testName), 0); \
    } \
    int _testName(const char *name) { \
        int fail=0; \
        if (!name || strcmp(name, COL_STRINGIZE(_testName)) == 0) { \
            fail += _testName##_testCaseRunner(); \
        } \
        return fail; \
    }

#define TEST_CASE_1(_testName) \
    TEST_CASE_DECLARE(_testName) \
    static void _testName##_testCase(void); \
    int _testName##_testCaseRunner() { \
        int fail; \
        TEST_CASE_ENTER(COL_STRINGIZE(_testName)); \
        fail = setjmp(test_failureEnv); \
        if (!fail) { \
            _testName##_testCase(); \
        } \
        TEST_CASE_LEAVE(COL_STRINGIZE(_testName), fail); \
        return fail; \
    } \
    static void _testName##_testCase(void)

#define TEST_CASE_2(_testName, _fixtureName) \
    TEST_CASE_DECLARE(_testName) \
    static void _testName##_testCase(void); \
    int _testName##_testCaseRunner() { \
        int fail; \
        TEST_CASE_ENTER(COL_STRINGIZE(_testName)); \
        fail = setjmp(test_failureEnv); \
        if (!fail) { \
            _fixtureName##_setup(NULL); \
            _testName##_testCase(); \
        } \
        _fixtureName##_teardown(fail, NULL); \
        TEST_CASE_LEAVE(COL_STRINGIZE(_testName), fail); \
        return fail; \
    } \
    static void _testName##_testCase()

#define TEST_CASE_3(_testName, _fixtureName, _context) \
    TEST_CASE_DECLARE(_testName) \
    static void _testName##_testCase(struct _fixtureName##_Context *); \
    int _testName##_testCaseRunner() { \
        struct _fixtureName##_Context context; \
        int fail; \
        TEST_CASE_ENTER(COL_STRINGIZE(_testName)); \
        fail = setjmp(test_failureEnv); \
        if (!fail) { \
            _fixtureName##_setup(&context); \
            _testName##_testCase(&context); \
        } \
        _fixtureName##_teardown(fail, &context); \
        TEST_CASE_LEAVE(COL_STRINGIZE(_testName), fail); \
        return fail; \
    } \
    static void _testName##_testCase(struct _fixtureName##_Context * _context)

/**
 * Hard assertion, throws an error if the given value is false.
 * 
 * @param x     Value to test. Evaluated once, so it can be an expression with 
 *              side effects.
 * @param msg   (optional) Message format string.
 * @param ...   (optional) Message string arguments.
 * 
 * @note *msg* and following arguments arguments are suitable arguments to 
 * printf().
 * 
 * @see TEST_FAILURE_LOGGER
 * @see TEST_ABORT
 * @see VERIFY
 */
#define ASSERT(x, /* msg, */ ...) \
    _ASSERT(x, #x, __VA_ARGS__)

#define _ASSERT(x, ...) \
    {if (!(x)) { test_assertFailed(TEST_FAILURE_LOGGER, __FILE__, __LINE__, \
        "ASSERT", COL_ARGCOUNT(__VA_ARGS__), __VA_ARGS__); TEST_ABORT(); } }

/**
 * Soft assertion, logs an error if the given value is false.
 * 
 * @param x     Value to test. Evaluated once, so it can be an expression with 
 *              side effects.
 * @param msg   (optional) Message format string.
 * @param ...   (optional) Message string arguments.
 * 
 * @note *msg* and following arguments arguments are suitable arguments to 
 * printf().
 * 
 * @see TEST_FAILURE_LOGGER
 * @see ASSERT
 */
#define VERIFY(x, /* msg, */ ...) \
    _VERIFY(x, #x, __VA_ARGS__)

#define _VERIFY(x, ...) \
    {if (!(x)) { test_assertFailed(TEST_FAILURE_LOGGER, __FILE__, __LINE__, \
        "VERIFY", COL_ARGCOUNT(__VA_ARGS__), __VA_ARGS__); } }

#endif /* _PICOTEST */
