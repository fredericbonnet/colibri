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
 * @see PICOTEST_SUITE
 * @see PICOTEST_CASE
 */
typedef int (PicoTestProc) (const char * name);

/**
 * Function signature of test traversal proc.
 * 
 * @param name  Name of traversed test.
 * @param nb    Number of subtests.
 * 
 * @see PICOTEST_TRAVERSE
 */
typedef void (PicoTestTraverseProc)(const char *name, int nb);

/**
 * Traverse a test hierarchy.
 * 
 * @param _testName     Name of the traversed test.
 * @param _proc         Test traversal proc.
 * 
 * @see PicoTestTraverseProc
 */
#define PICOTEST_TRAVERSE(_testName, _proc) \
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
 * @see PICOTEST_FAILURE_LOGGER
 */
typedef void (PicoTestFailureLoggerProc)(const char *file, int line, 
    const char *type, const char *test, const char *msg, va_list args);

/** @internal
 * Default test failure log handler. Does nothing.
 * 
 * @see PicoTestFailureLoggerProc
 * @see PICOTEST_FAILURE_LOGGER
 */
static void picoTest_logFailure(const char *file, int line, const char *type,
    const char *test, const char *msg, va_list args) {}

/**
 * Define the test failure log handler.
 * 
 * Called when a test failed.
 * 
 * The default handler does nothing. Redefine this macro to use a custom
 * handler.
 * 
 * @see PicoTestFailureLoggerProc
 */
#define PICOTEST_FAILURE_LOGGER picoTest_logFailure

/**
 * Abort a test case.
 * 
 * @see PICOTEST_CASE
 * @see picoTest_failureEnv
 */
#define PICOTEST_ABORT() \
    longjmp(picoTest_failureEnv, 1)

/** @internal
 * Tag used by setjmp() and longjmp() to jump out of failed tests.
 * 
 * @see PICOTEST_ABORT
 * @see PICOTEST_CASE
 */
static jmp_buf picoTest_failureEnv;

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
 * 
 * @see PICOTEST_ASSERT
 * @see PICOTEST_VERIFY
 */
static void picoTest_assertFailed(PicoTestFailureLoggerProc *proc, 
        const char *file, int line, const char *type, int count, 
        const char *test, ...) {
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
 * @see PICOTEST_SUITE
 */
typedef struct PicoTestDescr {
    const char * name;                              /*!< Test name. */
    int (*test)(const char *);                      /*!< Test function. */
    void (*traverse)(PicoTestTraverseProc *proc);   /*!< Test traversal. */
} PicoTestDescr;

/**
 * Test fixture context declaration.
 * 
 * The block following the macro call defines a struct used to hold a test 
 * fixture context. For example:
 * 
 * @code
 *		PICOTEST_FIXTURE_CONTEXT(fixture) {
 *			int var1;
 *			const char *var2;
 *			void *var3;
 *		};
 * @endcode
 * 
 * @param _fixtureName      Name of the fixture.
 * 
 * @see PICOTEST_FIXTURE_SETUP
 * @see PICOTEST_FIXTURE_TEARDOWN
 */
#define PICOTEST_FIXTURE_CONTEXT(_fixtureName) \
    struct _fixtureName##_Context

/**
 * Test fixture context initialization.
 * 
 * The code block following the macro call initializes the test fixture context.
 * For example:
 * 
 * @code
 *		PICOTEST_FIXTURE_SETUP(fixture) {
 *          // Seed the random number generator.
 *			srand(1234);
 *		}
 * @endcode
 * 
 * The test fixture can optionally define a context structure:
 *  
 * @code
 *		PICOTEST_FIXTURE_CONTEXT(fixture) {
 *			int var1;
 *			const char *var2;
 *			void *var3;
 *		};
 *		PICOTEST_FIXTURE_SETUP(fixture, context) {
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
 * @see PICOTEST_FIXTURE_CONTEXT
 * @see PICOTEST_FIXTURE_TEARDOWN
 */
#if defined(COL_PARENS)
#   define PICOTEST_FIXTURE_SETUP(...) \
        COL_CONCATENATE(_PICOTEST_FIXTURE_SETUP_,COL_ARGCOUNT(__VA_ARGS__)) COL_PARENS(__VA_ARGS__)
#else
#   define PICOTEST_FIXTURE_SETUP(...) \
        COL_CONCATENATE(_PICOTEST_FIXTURE_SETUP_,COL_ARGCOUNT(__VA_ARGS__))(__VA_ARGS__)
#endif

#define _PICOTEST_FIXTURE_SETUP_1(_fixtureName) \
    static void _fixtureName##_setup(void * _fixtureName##_DUMMY)

#define _PICOTEST_FIXTURE_SETUP_2(_fixtureName, _context) \
    static void _fixtureName##_setup(struct _fixtureName##_Context * _context)

/**
 * Test fixture context cleanup.
 * 
 * The code block following the macro call cleans up the test fixture context.
 * For example:
 * 
 * @code
 *		PICOTEST_FIXTURE_SETUP(fixture) {
 *          // Seed the random number generator.
 *			srand(1234);
 *		}
 *		PICOTEST_FIXTURE_TEARDOWN(fixture) {
 *          // Reset the random number generator to a non-deterministic value.
 *			srand(time(NULL));
 *		}
 * @endcode
 * 
 * The test fixture can optionally define a context structure:
 *  
 * @code
 *		PICOTEST_FIXTURE_CONTEXT(fixture) {
 *			int var1;
 *			const char *var2;
 *			void *var3;
 *		};
 *		PICOTEST_FIXTURE_SETUP(fixture, context) {
 *          // Initialize context.
 *			context->var1 = 1;
 *			context->var2 = "test";
 *			context->var3 = (void *) malloc(1);
 *		}
 *		PICOTEST_FIXTURE_TEARDOWN(fixture, context) {
 *			free(context->var3);
 *		}
 * @endcode
 * 
 * The code block is called after each test of the fixture, even if the test 
 * failed. The special variable PICOTEST_FAIL can be used to test for such 
 * cases:
 * - for simple test cases it returns 1 in case of failure,
 * - for test suites it returns the number of failed subtests.
 * For example: 
 * 
 * @code
 *      PICOTEST_FIXTURE_CONTEXT(fixture) {
 *          void *buffer;
 *          int size;
 *      }
 *      PICOTEST_FIXTURE_SETUP(fixture, context) {
 *          context->buffer = NULL;
 *          context->size = 1024;
 *      }
 *      PICOTEST_CASE(testInitBuffer, fixture, context) {
 *          context->buffer = malloc(context->size);
 *          PICOTEST_ASSERT(context->buffer);
 *          memset(context->buffer, 0, context->size);
 *      }
 *      PICOTEST_FIXTURE_TEARDOWN(fixture, context) {
 *          if (!PICOTEST_FAIL) {
 *              free(context->buffer);
 *          }
 *      }
 * @endcode
 * 
 * @param _fixtureName      Name of the fixture.
 * @param _context          (Optional) Fixture context structure.
 * 
 * @see PICOTEST_FIXTURE_CONTEXT
 * @see PICOTEST_FIXTURE_SETUP
 */
#if defined(COL_PARENS)
#   define PICOTEST_FIXTURE_TEARDOWN(...) \
        COL_CONCATENATE(_PICOTEST_FIXTURE_TEARDOWN_,COL_ARGCOUNT(__VA_ARGS__)) COL_PARENS(__VA_ARGS__)
#else
#   define PICOTEST_FIXTURE_TEARDOWN(...) \
        COL_CONCATENATE(_PICOTEST_FIXTURE_TEARDOWN_,COL_ARGCOUNT(__VA_ARGS__))(__VA_ARGS__)
#endif

#define _PICOTEST_FIXTURE_TEARDOWN_1(_fixtureName) \
    static void _fixtureName##_teardown(int PICOTEST_FAIL, \
        void * _fixtureName##_DUMMY)

#define _PICOTEST_FIXTURE_TEARDOWN_2(_fixtureName, _context) \
    static void _fixtureName##_teardown(int PICOTEST_FAIL, \
        struct _fixtureName##_Context * _context)

/**
 * Function signature of test suite enter hooks.
 * 
 * Called before running the first subtest.
 * 
 * @param suiteName     Test suite name.
 * @param nb            Number of subtests.
 * 
 * @see PICOTEST_SUITE_ENTER
 */
typedef void (PicoTestSuiteEnterProc)(const char *suiteName, int nb);

/** @internal
 * Default test suite enter hook. Does nothing.
 * 
 * @see PicoTestSuiteEnterProc
 * @see PICOTEST_SUITE_ENTER
 */
static void picoTest_enterTestSuite(const char *suiteName, int nb) {}

/**
 * Define the test suite enter hook.
 * 
 * The default hook does nothing. Redefine this macro to use a custom hook.
 * 
 * @see PicoTestSuiteEnterProc
 * @see PICOTEST_SUITE_LEAVE
 */
#define PICOTEST_SUITE_ENTER picoTest_enterTestSuite

/**
 * Function signature of test suite leave hooks.
 * 
 * @param suiteName     Test suite name.
 * @param nb            Number of subtests.
 * @param fail          Number of failed subtests (including the subtests' 
 *                      subtests if any).
 * 
 * @see PICOTEST_SUITE_LEAVE
 */
typedef void (PicoTestSuiteLeaveProc)(const char *suiteName, int nb, int fail);

/** @internal
 * Default test suite leave hook. Does nothing.
 * 
 * @see PicoTestSuiteLeaveProc
 * @see PICOTEST_SUITE_LEAVE
 */
static void picoTest_leaveTestSuite(const char *suiteName, int nb, int fail) {}

/**
 * Define the test suite leave hook.
 * 
 * Called after running all subtests.
 * 
 * The default hook does nothing. Redefine this macro to use a custom hook.
 * 
 * @see PicoTestSuiteLeaveProc
 * @see PICOTEST_SUITE_ENTER
 */
#define PICOTEST_SUITE_LEAVE picoTest_leaveTestSuite

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
 * @see PICOTEST_SUITE_BEFORE_SUBTEST
 */
typedef void (PicoTestSuiteBeforeSubtestProc)(const char *suiteName, int nb, 
    int index, const char *testName, int fail);

/** @internal
 * Default test suite before subtest hook. Does nothing.
 * 
 * @see PicoTestSuiteBeforeSubtestProc
 * @see PICOTEST_SUITE_BEFORE_SUBTEST
 */
static void picoTest_beforeSubtest(const char *suiteName, int nb, int index, 
    const char *testName, int fail) {}

/**
 * Define the test suite before subset hook.
 * 
 * The default hook does nothing. Redefine this macro to use a custom hook.
 * 
 * @see PicoTestSuiteBeforeSubtestProc
 * @see PICOTEST_SUITE_AFTER_SUBTEST
 */
#define PICOTEST_SUITE_BEFORE_SUBTEST picoTest_beforeSubtest

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
 * @see PICOTEST_SUITE_AFTER_SUBTEST
 */
typedef void (PicoTestSuiteAfterSubtestProc)(const char *suiteName, int nb, 
    int index, const char *testName, int fail, int sfail);

/** @internal
 * Default test suite after subtest hook. Does nothing.
 * 
 * @see PicoTestSuiteAfterSubtestProc
 * @see PICOTEST_SUITE_AFTER_SUBTEST
 */
static void picoTest_afterSubtest(const char *suiteName, int nb, int index, 
    const char *testName, int fail, int sfail) {}

/**
 * Define the test suite after subset hook.
 * 
 * The default hook does nothing. Redefine this macro to use a custom hook.
 * 
 * @see PicoTestSuiteAfterSubtestProc
 * @see PICOTEST_SUITE_BEFORE_SUBTEST
 */
#define PICOTEST_SUITE_AFTER_SUBTEST picoTest_afterSubtest

/**
 * Test suite declaration.
 * 
 * A test suite is a test function that is made of one or several subtests.
 * 
 * This macro defines a PicoTestProc of the given name that can be called 
 * directly.
 * 
 * @param _suiteName    Name of the test suite.
 * @param ...           Names of the subtests in the suite.
 * 
 * @return Number of failed tests.
 * 
 * @see PicoTestProc
 * @see PICOTEST_CASE
 */ 
#define PICOTEST_SUITE(_suiteName, ...) \
    COL_FOR_EACH(_PICOTEST_SUITE_DECLARE_TEST,__VA_ARGS__) \
    static PicoTestDescr _suiteName##_tests[] = { \
        COL_FOR_EACH(_PICOTEST_SUITE_DECLARE_TEST_CASE,__VA_ARGS__) \
        {NULL, NULL, NULL}}; \
    void _suiteName##_traverse(PicoTestTraverseProc *proc) { \
        const int nb=sizeof(_suiteName##_tests)/sizeof(*_suiteName##_tests)-1; \
        PicoTestDescr * test=_suiteName##_tests; \
        proc(COL_STRINGIZE(_suiteName), nb); \
        for (; test->name; test++) { \
            test->traverse(proc); \
        } \
    } \
    int _suiteName##_testCaseRunner() { \
        const int nb=sizeof(_suiteName##_tests)/sizeof(*_suiteName##_tests)-1; \
        PicoTestDescr * test=_suiteName##_tests; \
        int fail=0; \
        PICOTEST_SUITE_ENTER(COL_STRINGIZE(_suiteName), nb); \
        for (; test->name; test++) { \
            const int index=(int) (test - _suiteName##_tests); \
            int sfail=0; \
            PICOTEST_SUITE_BEFORE_SUBTEST(COL_STRINGIZE(_suiteName), nb, index, \
                test->name, fail); \
            sfail = test->test(NULL); \
            fail += sfail; \
            PICOTEST_SUITE_AFTER_SUBTEST(COL_STRINGIZE(_suiteName), nb, index, \
                test->name, fail, sfail); \
        } \
        PICOTEST_SUITE_LEAVE(COL_STRINGIZE(_suiteName), nb, fail); \
        return fail; \
    } \
    int _suiteName(const char *name) { \
        int fail=0; \
        if (!name || strcmp(name, COL_STRINGIZE(_suiteName)) == 0) { \
            fail += _suiteName##_testCaseRunner(); \
        } else { \
            PicoTestDescr * test=_suiteName##_tests; \
            for (; test->name; test++) { \
                fail += test->test(name); \
            } \
        } \
        return fail; \
    }

#define _PICOTEST_SUITE_DECLARE_TEST_CASE(_testName) \
    {COL_STRINGIZE(_testName), _testName, _testName##_traverse},
#define _PICOTEST_SUITE_DECLARE_TEST(_testName) \
    int _testName(const char *); \
    void _testName##_traverse(PicoTestTraverseProc *); \

/**
 * Function signature of test case enter hooks.
 * 
 * Called before running the test case.
 * 
 * @param testName      Test case name.
 * 
 * @see PICOTEST_CASE_ENTER
 */
typedef void (PicoTestCaseEnterProc)(const char *testName);

/** @internal
 * Default test case enter hook. Does nothing.
 * 
 * @see PicoTestCaseEnterProc
 * @see PICOTEST_CASE_ENTER
 */
static void picoTest_enterTestCase(const char *testName) {}

/**
 * Define the test case enter hook.
 * 
 * The default hook does nothing. Redefine this macro to use a custom hook.
 * 
 * @see PicoTestCaseEnterProc
 * @see PICOTEST_CASE_LEAVE
 */
#define PICOTEST_CASE_ENTER picoTest_enterTestCase

/**
 * Function signature of test case leave hooks.
 * 
 * Called after running the test case.
 * 
 * @param testName      Test case name.
 * @param fail          Failed tests (including its subtests if any).
 * 
 * @see PICOTEST_CASE_LEAVE
 */
typedef void (PicoTestCaseLeaveProc)(const char *testName, int fail);

/** @internal
 * Default test case enter hook. Does nothing.
 * 
 * @see PicoTestCaseLeaveProc
 * @see PICOTEST_CASE_LEAVE
 */
static void picoTest_leaveTestCase(const char *testName, int fail) {}

/**
 * Define the test case enter hook.
 * 
 * The default hook does nothing. Redefine this macro to use a custom hook.
 * 
 * @see PicoTestCaseLeaveProc
 * @see PICOTEST_CASE_ENTER
 */
#define PICOTEST_CASE_LEAVE picoTest_leaveTestCase

/**
 * Test case declaration.
 * 
 * This macro defines a PicoTestProc of the given name that can be called 
 * directly.
 * 
 * @param _testName     Name of the test case.
 * @param _fixtureName  (Optional) Name of the test fixture used by the test.
 * @param _context      (Optional) Fixture context structure.
 * 
 * @return Number of failed tests.
 * 
 * @see PicoTestProc
 * @see PICOTEST_FIXTURE_CONTEXT
 */ 
#if defined(COL_PARENS)
#   define PICOTEST_CASE(...) \
        COL_CONCATENATE(_PICOTEST_CASE_,COL_ARGCOUNT(__VA_ARGS__)) COL_PARENS(__VA_ARGS__)
#else
#   define PICOTEST_CASE(...) \
        COL_CONCATENATE(_PICOTEST_CASE_,COL_ARGCOUNT(__VA_ARGS__))(__VA_ARGS__)
#endif

#define _PICOTEST_CASE_DECLARE(_testName) \
    int _testName##_testCaseRunner(void); \
    void _testName##_traverse(PicoTestTraverseProc *proc) { \
        proc(COL_STRINGIZE(_testName), 0); \
    } \
    int _testName(const char *name) { \
        int fail=0; \
        if (!name || strcmp(name, COL_STRINGIZE(_testName)) == 0) { \
            fail += _testName##_testCaseRunner(); \
        } \
        return fail; \
    }

#define _PICOTEST_CASE_1(_testName) \
    _PICOTEST_CASE_DECLARE(_testName) \
    static void _testName##_testCase(void); \
    int _testName##_testCaseRunner() { \
        int fail; \
        PICOTEST_CASE_ENTER(COL_STRINGIZE(_testName)); \
        fail = setjmp(picoTest_failureEnv); \
        if (!fail) { \
            _testName##_testCase(); \
        } \
        PICOTEST_CASE_LEAVE(COL_STRINGIZE(_testName), fail); \
        return fail; \
    } \
    static void _testName##_testCase(void)

#define _PICOTEST_CASE_2(_testName, _fixtureName) \
    _PICOTEST_CASE_DECLARE(_testName) \
    static void _testName##_testCase(void); \
    int _testName##_testCaseRunner() { \
        int fail; \
        PICOTEST_CASE_ENTER(COL_STRINGIZE(_testName)); \
        fail = setjmp(picoTest_failureEnv); \
        if (!fail) { \
            _fixtureName##_setup(NULL); \
            _testName##_testCase(); \
        } \
        _fixtureName##_teardown(fail, NULL); \
        PICOTEST_CASE_LEAVE(COL_STRINGIZE(_testName), fail); \
        return fail; \
    } \
    static void _testName##_testCase()

#define _PICOTEST_CASE_3(_testName, _fixtureName, _context) \
    _PICOTEST_CASE_DECLARE(_testName) \
    static void _testName##_testCase(struct _fixtureName##_Context *); \
    int _testName##_testCaseRunner() { \
        struct _fixtureName##_Context context; \
        int fail; \
        PICOTEST_CASE_ENTER(COL_STRINGIZE(_testName)); \
        fail = setjmp(picoTest_failureEnv); \
        if (!fail) { \
            _fixtureName##_setup(&context); \
            _testName##_testCase(&context); \
        } \
        _fixtureName##_teardown(fail, &context); \
        PICOTEST_CASE_LEAVE(COL_STRINGIZE(_testName), fail); \
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
 * @see PICOTEST_FAILURE_LOGGER
 * @see PICOTEST_ABORT
 * @see PICOTEST_VERIFY
 */
#define PICOTEST_ASSERT(x, /* msg, */ ...) \
    _PICOTEST_ASSERT(x, #x, __VA_ARGS__)

#define _PICOTEST_ASSERT(x, ...) \
    {if (!(x)) { \
        picoTest_assertFailed(PICOTEST_FAILURE_LOGGER, __FILE__, __LINE__, \
            "ASSERT", COL_ARGCOUNT(__VA_ARGS__), __VA_ARGS__); \
        PICOTEST_ABORT(); \
    } }

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
 * @see PICOTEST_FAILURE_LOGGER
 * @see PICOTEST_ASSERT
 */
#define PICOTEST_VERIFY(x, /* msg, */ ...) \
    _PICOTEST_VERIFY(x, #x, __VA_ARGS__)

#define _PICOTEST_VERIFY(x, ...) \
    {if (!(x)) { \
        picoTest_assertFailed(PICOTEST_FAILURE_LOGGER, __FILE__, __LINE__, \
            "VERIFY", COL_ARGCOUNT(__VA_ARGS__), __VA_ARGS__); \
    } }

#endif /* _PICOTEST */
