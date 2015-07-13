# Copyright (c) 2015 dividiti Limited

# Add an interceptor library that intercepts some calls in 'libOpenCL.so'.
add_library(prof_test SHARED ${SOURCE})
set_target_properties(prof_test
  PROPERTIES
    COMPILE_FLAGS "-D DVDT_PROF_TEST=1 -D DVDT_PROF_WALLCLOCK_TIMEOFDAY=1")
target_link_libraries(prof_test dl)

## Copy ${PROJECT_SOURCE_DIR}/tests/* to ${PROJECT_BINARY_DIR}/tests.
#install(DIRECTORY ${PROJECT_SOURCE_DIR}/tests/
#    DESTINATION ${PROJECT_BINARY_DIR}/tests)

# Get path to libprof_test.so.
#set(PROF_TEST $<TARGET_FILE:prof_test>)

# Add definitions for sources in the current directory and below.
add_definitions(-D DVDT_PROF_TEST)
add_definitions(-D DVDT_PROF_WALLCLOCK_TIMEOFDAY)

# Add test sources.
file(GLOB test_sources ${PROJECT_SOURCE_DIR}/tests/*.cpp)
list(LENGTH test_sources num_test_sources)
message(STATUS "Using ${num_test_sources} tests")

# Build test binaries.
foreach(test_cpp ${test_sources})
    get_filename_component(test ${test_cpp} NAME_WE)
    add_executable(${test} ${test_cpp})
    add_dependencies(${test} prof_test)
    target_link_libraries(${test} prof_test OpenCL)
    list(APPEND test_binaries ${test})
endforeach()

# Add tests.
foreach(test ${test_binaries})
    add_test(NAME ${test}
        COMMAND ${PROJECT_SOURCE_DIR}/tests/pipe.sh
            "${PROJECT_BINARY_DIR}/bin/${test}"
            "${PROJECT_SOURCE_DIR}/tests/${test}.py"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/tests"
    )
    string(CONCAT test_regex "${test}" ": PASSED")
    set_tests_properties(${test}
        PROPERTIES
            PASS_REGULAR_EXPRESSION ${test_regex})
endforeach()

# Custom target: "make check"
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND}
    DEPENDS prof_test ${test_binaries}
)
