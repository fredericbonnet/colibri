#ifndef _FAILURE_FIXTURE_H_
#define _FAILURE_FIXTURE_H_

#include <picotest.h>

#include <stdio.h>

#include "errors.h"

/*
 * Fixture for failure test cases
 */

/* Failure information to check */
typedef struct FailureInfo {
    Col_ErrorLevel level;
    Col_ErrorDomain domain;
    int code;
} FailureInfo;

/* Expected failure, checked in error proc */
static FailureInfo *expectedFailure;

/* Error proc for failure cases */
static int failure_errorProc(Col_ErrorLevel level, Col_ErrorDomain domain,
                             int code, va_list args) {
    PICOTEST_FAILURE("COLIBRI", "ERROR");
    if (expectedFailure) {
        PICOTEST_VERIFY(expectedFailure->level == level);
        PICOTEST_VERIFY(expectedFailure->domain == domain);
        PICOTEST_VERIFY(expectedFailure->code == code);
    }
    if (level > COL_ERROR) {
        /*
         * Stop error processing for recoverable errors, this gives a chance to
         * check return values of failed operations in test cases.
         */
        return 1;
    }
    PICOTEST_ABORT();
}

/* Failure fixture context, used to optionally set expected failure info */
PICOTEST_FIXTURE_CONTEXT(failureFixture) {
    FailureInfo expectedFailure;
};

/* Original error proc is saved and restored before/after failure fixture */
static Col_ErrorProc *oldErrorProc;

/* Failure fixture setup */
PICOTEST_FIXTURE_SETUP(failureFixture, context) {
    oldErrorProc = Col_SetErrorProc(failure_errorProc);
    expectedFailure = context ? &context->expectedFailure : NULL;
}

/* Failure fixture teardown */
PICOTEST_FIXTURE_TEARDOWN(failureFixture) {
    Col_SetErrorProc(oldErrorProc);
}

/* Utility to set expected failure info in test case context */
#ifdef _DEBUG
#define EXPECT_FAILURE _EXPECT_FAILURE
#else
#define EXPECT_FAILURE(context, _level, _domain, _code)                        \
    PICOTEST_ASSERT(_level != COL_TYPECHECK);                                  \
    _EXPECT_FAILURE(context, _level, _domain, _code)
#endif
#define _EXPECT_FAILURE(context, _level, _domain, _code)                       \
    (context)->expectedFailure.level = (_level);                               \
    (context)->expectedFailure.domain = (_domain);                             \
    (context)->expectedFailure.code = (_code);

#endif /* _FAILURE_FIXTURE_H_ */
