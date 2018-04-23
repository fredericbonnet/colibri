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

TEST_SUITE(testColibri, 
    testImmediateWords, testRopes, testLists, testMaps, testStrBufs
)

int main(int argc, char* argv[]) {
#ifdef DO_TRACE
    freopen( "c:\\err.txt", "wt", stderr);
#endif

    printf("Hit return...\n");
    getc(stdin);

    /* Only execute tests given as command line arguments. */
    if (argc == 1) {
        testColibri(NULL);
    } else {
        int i;
        for (i=1; i < argc; i++) {
            char *name = argv[i];
            testColibri(name);
        }
    }

//    testCustomRopes();
//    testGeneration();
//    testGrow();

//    testAllocInts();
//    testAllocIntVects();
//    testRandomIntVects();
//    testChildren();
//    testChildVect();

    printf("Hit return...\n");
    getc(stdin);
    
    exit(0);
}
