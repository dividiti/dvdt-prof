#
# 2015-2016 (c) dividiti
#

cmake_minimum_required(VERSION 3.0)

project(Prof)

# The Prof copyright messages.
set(Prof_COPYRIGHT_DIVIDITI "\"2015-2016 (c) dividiti\"")

# The Prof version.
set(Prof_VERSION_MAJOR 0)
set(Prof_VERSION_MINOR 1)
message("dividiti OpenCL API Profiler v${Prof_VERSION_MAJOR}.${Prof_VERSION_MINOR}")

# The WALLCLOCK option.
set(WALLCLOCK "boost" CACHE STRING "How to measure wall-clock time.")
if(WALLCLOCK STREQUAL "boost")
  message(STATUS "Measuring wall-clock time using boost::chrono")
  add_definitions(-D DVDT_PROF_WALLCLOCK_BOOST=1)
elseif(WALLCLOCK STREQUAL "timeofday")
  message(STATUS "Measuring wall-clock time using gettimeofday()")
  add_definitions(-D DVDT_PROF_WALLCLOCK_TIMEOFDAY=1)
else()
  message(WARNING "Unsupported WALLCLOCK option: ${WALLCLOCK}.")
endif()

# The CJSON option.
set(CJSON_SET "0" CACHE BOOLEAN "Parse JSON or default output.")
if("${CJSON_SET}" STREQUAL "1")
  add_definitions(-D DVDT_PROF_CJSON=1)
  include_directories("${CJSON_INCLUDE_DIR}")
  SET(CJSON_LIB_PATH "${CJSON_LIB_DIR}/${CJSON_LIB_NAME}")
endif()

# Set build options.
set(CMAKE_CXX_FLAGS "-O2 -W -Wall")
message(STATUS "Using compiler flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Using linker flags: ${CMAKE_SHARED_LINKER_FLAGS}")

# Output directory for executables.
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Output directory for libraries.
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Pass CMake settings to the source code via a header file.
configure_file(
  "${PROJECT_SOURCE_DIR}/cpp/prof_info.hpp.in"
  "${PROJECT_BINARY_DIR}/include/prof_info.hpp"
)

# Add the binary tree to the search path for include files
# so that 'prof_info.hpp' can be found.
include_directories("${PROJECT_BINARY_DIR}/include")

# Add source files.
set(SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/cpp/prof.cpp
)

# Add an interceptor library that intercepts some calls in 'libOpenCL.so'.
add_library(prof SHARED ${SOURCE})
target_link_libraries(prof dl "${CJSON_LIB_PATH}")

# Test descriptions are in a separate file.
include(CTest)
include(${PROJECT_SOURCE_DIR}/tests/CMakeLists.txt)
