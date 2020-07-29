#ifndef _FAILURE_FIXTURE_H_
#define _FAILURE_FIXTURE_H_

#include <picotest.h>

#include <stdio.h>

#include "errors.h"

/*
 * Fixture for failure cases
 */
static int failure_errorProc(Col_ErrorLevel level, Col_ErrorDomain domain,
                             int code, va_list args) {
    PICOTEST_FAILURE("COLIBRI", "ERROR");
    PICOTEST_ABORT();
    return 0;
}
static Col_ErrorProc *oldErrorProc;
PICOTEST_FIXTURE_SETUP(failureFixture, context) {
    oldErrorProc = Col_SetErrorProc(failure_errorProc);
}
PICOTEST_FIXTURE_TEARDOWN(failureFixture, context) {
    Col_SetErrorProc(oldErrorProc);
}

#endif /* _FAILURE_FIXTURE_H_ */
