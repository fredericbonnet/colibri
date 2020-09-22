#include <colibri.h>
#include <picotest.h>

#include "hooks.h"

/*
 * Words
 */
PICOTEST_SUITE(testWords, testBasicWords, testRopes, testStringBuffers,
               testVectors, testMutableVectors, testLists, testMutableLists,
               testMaps, testHashMaps, testTrieMaps);
