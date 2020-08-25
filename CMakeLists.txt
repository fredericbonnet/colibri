cmake_minimum_required(VERSION 3.15)

project(colibri LANGUAGES C)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(USE_THREADS "Build with thread support" ON)

find_package(PicoTest)

# Colibri library
add_library(colibri)
target_compile_definitions(colibri
	PRIVATE BUILD_colibri
)
if (NOT BUILD_SHARED_LIBS)
	target_compile_definitions(colibri
		PUBLIC COL_STATIC_BUILD
	)
endif()
if (USE_THREADS)
	target_compile_definitions(colibri
		PUBLIC COL_USE_THREADS
	)
endif()
target_include_directories(colibri
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include/colibri>
)
target_sources(colibri
	PRIVATE
		src/colibri.c
		src/colAlloc.c
		src/colGc.c
		src/colRope.c
		src/colStrBuf.c
		src/colWord.c
		src/colVector.c
		src/colList.c
		src/colMap.c
		src/colHash.c
		src/colTrie.c
)
if (WIN32)
	target_sources(colibri
		PRIVATE
			src/platform/win32/colWin32Platform.c
	)
else()
	target_sources(colibri
		PRIVATE
			src/platform/unix/colUnixPlatform.c
	)
	# FIXME
	target_compile_definitions(colibri
		PUBLIC 
			HAVE_STDINT_H
	)
	target_link_libraries(colibri
		PUBLIC
			pthread
	)
endif()

install(TARGETS colibri EXPORT colibri-config)
install(DIRECTORY include/ DESTINATION include/colibri)
install(EXPORT colibri-config DESTINATION cmake)


# Colibri unit tests
if (PicoTest_FOUND)
	# TDD tests
	add_executable(tdd_colibri)
	target_sources(tdd_colibri
		PRIVATE
			tests/tdd/main.c
			tests/tdd/json.c
			tests/tdd/traverse.c
			tests/tdd/hooks.c
			tests/tdd/errors.c
			tests/tdd/testStrings.c
			tests/tdd/testWords.c
			tests/tdd/testBasicWords.c
			tests/tdd/testRopes.c
			tests/tdd/testVectors.c
			tests/tdd/testMutableVectors.c
			tests/tdd/testLists.c
			tests/tdd/testHashMaps.c
	)
	target_link_libraries(tdd_colibri
		PRIVATE
			colibri
			PicoTest
	)
	install(TARGETS tdd_colibri EXPORT tdd_colibri
		RUNTIME DESTINATION bin/${CMAKE_BUILD_TYPE}
	)

	# Historic tests
	add_executable(test_colibri)
	target_sources(test_colibri
		PRIVATE
			tests/testColibri.c
			tests/testImmediateWords.c
			tests/testRopes.c
			tests/testStrBufs.c
			tests/testLists.c
			tests/testMaps.c
	)
	target_link_libraries(test_colibri
		PRIVATE
			colibri
			PicoTest
	)
	install(TARGETS test_colibri EXPORT test_colibri
		RUNTIME DESTINATION bin/${CMAKE_BUILD_TYPE}
	)

	enable_testing()
	picotest_discover_tests(test_colibri
		TEST_LIST_OPTION "-l"
	)
endif()