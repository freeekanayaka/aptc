project (aptc)
cmake_minimum_required(VERSION 3.4.0)

include_directories(${PROJECT_BINARY_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/lib)

enable_testing()

set(PACKAGE ${PROJECT_NAME})
set(PACKAGE_VERSION "0.0.0")

if (NOT DEFINED COMMON_ARCH)
  execute_process(COMMAND dpkg-architecture -qDEB_HOST_ARCH
                  OUTPUT_VARIABLE COMMON_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake/")
set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -std=gnu++11")

if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
endif()

find_package(AptPkg REQUIRED)

configure_file(CMake/config.h.in ${PROJECT_BINARY_DIR}/include/config.h)

add_subdirectory(lib)
add_subdirectory(cmd)
add_subdirectory(test)
