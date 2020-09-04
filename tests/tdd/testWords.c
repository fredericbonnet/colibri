#include <colibri.h>
#include <picotest.h>

#include "hooks.h"

/*
 * Words
 */
PICOTEST_SUITE(testWords, testBasicWords, testRopes, testVectors,
               testMutableVectors, testLists, testMaps, testHashMaps,
               testTrieMaps);
