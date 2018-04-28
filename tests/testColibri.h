/* Colibri error proc handler. */
Col_ErrorProc test_errorProc;
#define ERROR_PROC test_errorProc

/* Test failure log handler. */
TestFailureLoggerProc logFailure;
#undef TEST_FAILURE_LOGGER
#define TEST_FAILURE_LOGGER logFailure

/* Test suite tracing hooks. */
TestSuiteEnterProc enterTestSuite;
TestSuiteLeaveProc leaveTestSuite;
TestSuiteBeforeSubtestProc beforeSubtest;
TestSuiteAfterSubtestProc afterSubtest;
#undef TEST_SUITE_ENTER
#undef TEST_SUITE_LEAVE
#undef TEST_SUITE_BEFORE_SUBTEST
#undef TEST_SUITE_AFTER_SUBTEST
#define TEST_SUITE_ENTER enterTestSuite
#define TEST_SUITE_LEAVE leaveTestSuite
#define TEST_SUITE_BEFORE_SUBTEST beforeSubtest
#define TEST_SUITE_AFTER_SUBTEST afterSubtest

/* Test case tracing hooks. */
TestCaseEnterProc enterTestCase;
TestCaseLeaveProc leaveTestCase;
#undef TEST_CASE_ENTER
#undef TEST_CASE_LEAVE
#define TEST_CASE_ENTER enterTestCase
#define TEST_CASE_LEAVE leaveTestCase
