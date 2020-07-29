/* Colibri error proc handler. */
Col_ErrorProc test_errorProc;
#define ERROR_PROC test_errorProc

/* Record the current PICOTEST_ABORT for the longjmp to work properly. */
extern void (*ABORT_HANDLER) ();
static void ABORT() {PICOTEST_FAILURE("COLIBRI", "ERROR"); PICOTEST_ABORT();}

/* Test failure log handler. */
PicoTestFailureLoggerProc logFailure;
#undef PICOTEST_FAILURE_LOGGER
#define PICOTEST_FAILURE_LOGGER logFailure

/* Test suite tracing hooks. */
PicoTestSuiteEnterProc enterTestSuite;
PicoTestSuiteLeaveProc leaveTestSuite;
PicoTestSuiteBeforeSubtestProc beforeSubtest;
PicoTestSuiteAfterSubtestProc afterSubtest;
#undef PICOTEST_SUITE_ENTER
#undef PICOTEST_SUITE_LEAVE
#undef PICOTEST_SUITE_BEFORE_SUBTEST
#undef PICOTEST_SUITE_AFTER_SUBTEST
#define PICOTEST_SUITE_ENTER enterTestSuite
#define PICOTEST_SUITE_LEAVE leaveTestSuite
#define PICOTEST_SUITE_BEFORE_SUBTEST beforeSubtest
#define PICOTEST_SUITE_AFTER_SUBTEST afterSubtest

/* Test case tracing hooks. Record the current ABORT in the process. */
PicoTestCaseEnterProc enterTestCase;
PicoTestCaseLeaveProc leaveTestCase;
#undef PICOTEST_CASE_ENTER
#undef PICOTEST_CASE_LEAVE
#define PICOTEST_CASE_ENTER (ABORT_HANDLER=ABORT, enterTestCase)
#define PICOTEST_CASE_LEAVE (ABORT_HANDLER=NULL, leaveTestCase)

/* Shorthands. */
#define ASSERT PICOTEST_ASSERT
#define VERIFY PICOTEST_VERIFY
