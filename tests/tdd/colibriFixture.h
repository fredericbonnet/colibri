#ifndef _COLIBRI_FIXTURE_H_
#define _COLIBRI_FIXTURE_H_

#include "hooks.h"
#include "errors.h"

/*
 * Colibri fixture, performs init/cleanup for each test
 */
PICOTEST_FIXTURE_SETUP(colibriFixture) {
    Col_Init(COL_SINGLE);
    Col_SetErrorProc(ERROR_PROC);

    Col_PauseGC();
}
PICOTEST_FIXTURE_TEARDOWN(colibriFixture) {
    if (!PICOTEST_FAIL) {
        Col_ResumeGC();
    }
    Col_Cleanup();
}

#endif /* _COLIBRI_FIXTURE_H_ */
