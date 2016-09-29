# 2015-2016 (c) dividiti

# Add an interceptor library that intercepts some calls in 'libOpenCL.so'.
add_library(prof_test SHARED ${SOURCE})
set_target_properties(prof_test
  PROPERTIES
    COMPILE_FLAGS "-D DVDT_PROF_TEST=1 -D DVDT_PROF_WALLCLOCK_TIMEOFDAY=1")
target_link_libraries(prof_test dl "${CJSON_LIB_DIR}/${CJSON_LIB_NAME}")


# Get path to 'libprof_test.so'.
set(PROF_TEST $<TARGET_FILE:prof_test>)

# Add OpenCL API test sources.
file(GLOB cl_api_test_sources ${PROJECT_SOURCE_DIR}/tests/cl*.cpp)
list(LENGTH cl_api_test_sources num_cl_api_test_sources)
message(STATUS "Using ${num_cl_api_test_sources} OpenCL API tests")

# Build OpenCL API test binaries.
foreach(test_cpp ${cl_api_test_sources})
    get_filename_component(test ${test_cpp} NAME_WE)
    add_executable(${test} ${test_cpp})
    add_dependencies(${test} prof_test)
    target_link_libraries(${test} OpenCL)
    list(APPEND cl_api_test_binaries ${test})
endforeach()

# Add OpenCL API tests.
foreach(test ${cl_api_test_binaries})
    add_test(NAME ${test}
        COMMAND
            ${PROJECT_SOURCE_DIR}/tests/pipe.sh
            "${PROJECT_BINARY_DIR}/bin/${test}"
            "${PROJECT_SOURCE_DIR}/tests/${test}.py"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/tests"
    )
    string(CONCAT cl_api_test_regex "${test}" ": PASSED")
    set_tests_properties(${test}
        PROPERTIES
            PASS_REGULAR_EXPRESSION ${cl_api_test_regex}
            ENVIRONMENT LD_PRELOAD=${PROF_TEST})
endforeach()

set_property(TEST clEnqueueNDRangeKernel_LWS
    APPEND PROPERTY ENVIRONMENT DVDT_PROF_LWS="dvdt_prof_kernel:1,2")
set_property(TEST clEnqueueNDRangeKernel_LWS_NULL
    APPEND PROPERTY ENVIRONMENT DVDT_PROF_LWS_NULL=1)

# Custom target: "make check"
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND}
    DEPENDS prof_test ${cl_api_test_binaries}
)
