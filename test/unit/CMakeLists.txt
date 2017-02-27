find_package(Threads REQUIRED)

set(GTEST_ROOT "/usr/src/gtest")
set(GMOCK_ROOT "/usr/src/gmock")

include(ExternalProject)

ExternalProject_Add(gtest
	PREFIX gtest
	SOURCE_DIR ${GTEST_ROOT}
        INSTALL_COMMAND true)
ExternalProject_Add(gmock
	PREFIX gmock
	SOURCE_DIR ${GMOCK_ROOT}
        INSTALL_COMMAND true)

link_directories(${CMAKE_CURRENT_BINARY_DIR}/gtest/src/gtest-build)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/gmock/src/gmock-build)

set(GTEST_LIBRARIES "-lgtest -lgtest_main -lgmock -lapt-pkg")
set(GTEST_DEPENDENCIES "gtest")
set(GMOCK_DEPENDENCIES "gmock")

find_path(GTEST_INCLUDE_DIRS NAMES gtest/gtest.h)
find_path(GMOCK_INCLUDE_DIRS NAMES gmock/gmock.h)

message(STATUS "Found GTest at ${GTEST_ROOT}, headers at ${GTEST_INCLUDE_DIRS}")
message(STATUS "Found GMock at ${GMOCK_ROOT}, headers at ${GMOCK_INCLUDE_DIRS}")

file(GLOB SRCS *_test.cc)

add_executable(lib_test ${SRCS})
target_include_directories(lib_test PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(lib_test ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} aptc-private)
add_dependencies(lib_test ${GTEST_DEPENDENCIES} ${GMOCK_DEPENDENCIES})

add_test(NAME lib
         COMMAND lib_test)
