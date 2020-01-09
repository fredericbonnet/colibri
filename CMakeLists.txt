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
		colibri.c
		colAlloc.c
		colGc.c
		colRope.c
		colStrBuf.c
		colWord.c
		colVector.c
		colList.c
		colMap.c
		colHash.c
		colTrie.c
)
if (WIN32)
	target_sources(colibri
		PRIVATE
			platform/win32/colWin32Platform.c
	)
else()
	target_sources(colibri
		PRIVATE
			platform/unix/colUnixPlatform.c
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