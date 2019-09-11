#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#include <colibri.h>

#include <picotest.h>

#ifdef _DEBUG
#   define DO_TRACE
#endif

#ifdef DO_TRACE
#   include <stdio.h>
#   define TRACE(format, ...) (fprintf(stderr, format, __VA_ARGS__), fflush(stderr))
#else
#   define TRACE
#endif

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

    PICOTEST_ABORT();
}

/* Test failure log handler. */
void logFailure(const char *file, int line, const char *type, const char *test, const char *msg, va_list args) {
    printf("\n[%s] %s(%d) : %s", type, file, line, test);
    if (msg) {
        printf(" | "); 
        vprintf(msg, args);
    }
    printf("\n");
}

/* Test tracing hooks. */
static char prefix[256] = "";
static int prefixLength = 0;
void enterTestSuite(const char *suiteName, int nb) {
    printf("%s%s (%d subtests) ...\n", prefix, suiteName, nb);
    fflush(stdout);
    prefix[prefixLength++] = ' ';
    prefix[prefixLength] = '\0';
}
void leaveTestSuite(const char *suiteName, int nb, 
        int fail) {
    prefix[--prefixLength] = '\0';
    printf("%s%s : %d failed (total)\n", prefix, suiteName, fail);
    fflush(stdout);
}
void beforeSubtest(const char *suiteName, int nb, int fail, int index, const char *testName) {
}
void afterSubtest(const char *suiteName, int nb, int fail, int index, const char *testName, int sfail) {
}
void enterTestCase(const char *testName) {
    printf("%s%s ... ", prefix, testName);
    fflush(stdout);
}
void leaveTestCase(const char *testName, int fail) {
    if (fail) {
        printf("failed");
    } else {
        printf("passed");
    }
    printf("\n");
    fflush(stdout);
}

/* Test traversal procs used to list tests in suite. */
void printTestCase(const char *name, int nb) {
    if (nb == 0) printf("%s\n", name);
}
void printTestName(const char *name, int nb) {
    printf("%s\n", name);
}

PICOTEST_SUITE(testColibri, 
    testImmediateWords, testRopes, testLists, testMaps, testStrBufs
)

int main(int argc, char* argv[]) {
    int fail = 0;
#ifdef DO_TRACE
    freopen( "c:\\err.txt", "wt", stderr);
#endif

    if (argc > 1 && argv[1][0] == '-') {
        if (strcmp(argv[1], "-l") == 0 || strcmp(argv[1], "--list") == 0) {
            /* List test cases. */
            PICOTEST_TRAVERSE(testColibri, printTestCase);
        } else if (strcmp(argv[1], "-a") == 0 || strcmp(argv[1], "--list-all") == 0) {
            /* List all tests. */
            PICOTEST_TRAVERSE(testColibri, printTestName);
        }
        return 0;
    }

    if (argc <= 1) {
        /* Execute all tests. */
        fail = testColibri(NULL);
    } else {
        /* Only execute tests given as command line arguments. */
        int i;
        for (i=1; i < argc; i++) {
            char *name = argv[i];
            fail += testColibri(name);
        }
    }
    
    exit(fail);
}
