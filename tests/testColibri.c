#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#include <colibri.h>

#ifdef _DEBUG
#   define DO_TRACE
#endif

#ifdef DO_TRACE
#   include <stdio.h>
#   define TRACE(format, ...) (fprintf(stderr, format, __VA_ARGS__), fflush(stderr))
#else
#   define TRACE
#endif


#include "unitTest.h"
#include "testColibri.h"

/* Colibri error proc handler. */
int test_errorProc(Col_ErrorLevel level, Col_ErrorDomain domain, int code, va_list args) {
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

    TEST_ABORT();
}

/* Test failure log handler. */
void log_failure(const char *file, int line, const char *type, const char *test, const char *msg, va_list args) {
    printf("\n[%s] %s(%d) : %s", type, file, line, test);
    if (msg) {
        printf(" | "); 
        vprintf(msg, args);
    }
    printf("\n");
}

TEST_SUITE(testColibri, 
    testImmediateWords, testRopes, testLists, testMaps, testStrBufs
)

int main(int argc, char* argv[]) {
    int fail = 0;
#ifdef DO_TRACE
    freopen( "c:\\err.txt", "wt", stderr);
#endif

    printf("Hit return...\n");
    getc(stdin);

    /* Only execute tests given as command line arguments. */
    if (argc <= 1) {
        fail = testColibri(NULL);
    } else {
        int i;
        for (i=1; i < argc; i++) {
            char *name = argv[i];
            fail += testColibri(name);
        }
    }

    printf("Hit return...\n");
    getc(stdin);
    
    exit(fail);
}
