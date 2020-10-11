#include <stdio.h>

#include <colibri.h>
#include <picotest.h>

#include "errors.h"

/* Current PicoTest abort handler. */
void (*ABORT_HANDLER)();

/* Standard Colibri error proc handler. */
int test_errorProc(Col_ErrorLevel level, Col_ErrorDomain domain, int code,
                   va_list args) {
    const char *levelString;
    switch (level) {
    case COL_FATAL:
        levelString = "FATAL";
        break;

    case COL_ERROR:
        levelString = "ERROR";
        break;

    case COL_TYPECHECK:
        levelString = "TYPECHECK";
        break;

    case COL_VALUECHECK:
        levelString = "VALUECHECK";
        break;
    }
    printf("\n[%s] ", levelString);
    vprintf(domain[code], args);
    printf("\n");

    ABORT_HANDLER();
    return 0;
}
