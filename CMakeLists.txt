cmake_minimum_required(VERSION 3.11)

project(colibri LANGUAGES C)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/cmake ${CMAKE_CURRENT_LIST_DIR}/../lib)

find_package(PicoTest REQUIRED)

# Colibri library
add_library(colibri SHARED)
target_compile_definitions(colibri
	PRIVATE BUILD_colibri
)
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
endif()

install(TARGETS colibri EXPORT colibri-config
	RUNTIME DESTINATION bin/${CMAKE_BUILD_TYPE}
	ARCHIVE DESTINATION lib/${CMAKE_BUILD_TYPE}
	LIBRARY DESTINATION lib/${CMAKE_BUILD_TYPE}/static
)
install(DIRECTORY include/ DESTINATION include/colibri)
install(EXPORT colibri-config DESTINATION cmake)


# Colibri unit tests
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
