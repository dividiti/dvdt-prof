# dv/dt prof: OpenCL API profiler

The `dv/dt prof` profiler (`libprof.so`) intercepts some OpenCL API calls and
prints their arguments before invoking the underlying OpenCL implementation.
The Python parser (`prof_parser.py`) converts the profiler's output into JSON.

Using the profiler can slow down the program for several reasons:

- To make parsing robust, the profiler uses formatted printing which is
  relatively expensive.

- To time non-blocking calls accurately, the profiler makes them blocking.

- Optionally, the profiler can alter the program behaviour in other ways, for
  example, by changing the local work size for one or more kernels in the
  program. This functionality requires keeping additional state.

The good news is that the kernel execution time and memory copy time are not
affected.

[OpenCL page at the Khronos Group](https://www.khronos.org/opencl)

# Building the profiler

## Prerequisites

- CMake 3.0.

- OpenCL headers and library.

## Building the profiler.

Place the source into `${SRC_DIR}`. Create `${BUILD_DIR}`.

```
$ cd ${BUILD_DIR}
$ cmake ${SRC_DIR}
$ make prof
```

`${SRC_DIR}/lib` now contains `libprof.so`.

To build and run tests:

```
$ make check
```

`${SRC_DIR}/lib` now contains `libprof_test.so` which is only useful for testing.

### Build options.

By default, the profiler uses `boost::chrono` to measure wall-clock time. This
can be disabled by setting the `WALLCLOCK` option as follows:

```
cmake ${SRC_DIR} -DWALLCLOCK=timeofday
```

(This is particularly handy for Android platforms.)

Alternative mechanisms like `gettimeofday()` are not supported at the moment.

# Using the profiler

## Collecting runtime information
```
$ LD_PRELOAD=<path to profiler e.g. "${BUILD_DIR}/lib/libprof.so"> <path to program to be profiled>
```

## Changing the program behaviour

Several environment variables can be defined when launching the program.

### DVDT_PROF_LWS

`DVDT_PROF_LWS` specifies changes that should be made to the local work size
when launching one or more kernels in the program.

```
DVDT_PROF_LWS="kernel_A:lws_A0,lws_A1,lws_A2 kernel_B:lws_B0,lws_B1,lws_B1 ..."
```
For example:
```
DVDT_PROF_LWS="transpose:8,8 gemm:4,16" LD_PRELOAD=<path to profiler> <path to program>
```

Namely, the per-kernel list elements are separated by spaces; the kernel names
(strings) are separated from the local work size tuple by colons; the tuple
elements (unsigned integers) are delimited by commas. The number of elements in
a tuple must match the number of work-group dimensions as specified in the
program or start with the value of `0` to use `NULL` as the local work size for
this kernel.

Note that the profiler cannot check the correctness of any given specification.
In particular, the usual execution constraints hold: the global work size
dimensions must be divisible by the local work size dimensions; the total
work-group size (the product of all the dimensions) cannot exceed
`CL_KERNEL_WORK_GROUP_SIZE`.

### DVDT_PROF_LWS_NULL

For convenience, if the environment defines `DVDT_PROF_LWS_NULL` then `NULL` is
used when launching any kernel in the program. (In this case, `DVDT_PROF_LWS`
gets ignored.)
