//
// 2015 (c) dividiti
//

#ifndef DVDT_PROF_HPP
#define DVDT_PROF_HPP

#include <dlfcn.h>

#include <iostream>
#include <iomanip>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

// FIXME: still needed?
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#if   (1 == DVDT_PROF_WALLCLOCK_BOOST)
#include <boost/date_time.hpp>
#elif (1 == DVDT_PROF_WALLCLOCK_TIMEOFDAY)
#include <sys/time.h>
#else
#error "Don't know how to measure wall-clock time"
#endif


#if (1 == DVDT_PROF_TEST)
#define FIXED_WIDTH_PTR(ptr) "0x" << std::hex << std::setw(8) << std::setfill('0') << (size_t)(ptr) << std::dec
#else
#define FIXED_WIDTH_PTR(ptr) (ptr)
#endif


typedef cl_command_queue (*clCreateCommandQueue_type)(cl_context, cl_device_id, cl_command_queue_properties, cl_int *errcode_ret);

typedef cl_program (*clCreateProgramWithSource_type)(cl_context, cl_uint count, const char ** strings, const size_t * lengths, cl_int * errcode_ret);

typedef cl_int (*clBuildProgram_type)(cl_program, cl_uint, const cl_device_id *, const char *, void (CL_CALLBACK *)(cl_program, void *), void *);

typedef cl_kernel (*clCreateKernel_type)(cl_program, const char * givenKernelName, cl_int * errcode_ret);

typedef cl_int (*clSetKernelArg_type)(cl_kernel, cl_uint, size_t, const void *);

typedef cl_int (*clEnqueueNDRangeKernel_type)(cl_command_queue, cl_kernel, cl_uint, const size_t *, const size_t *, const size_t *, cl_uint, const cl_event *, cl_event *);

typedef cl_int (*clEnqueueReadBuffer_type)(cl_command_queue, cl_mem, cl_bool, size_t, size_t, void *, cl_uint, const cl_event *, cl_event *);

typedef cl_int (*clEnqueueWriteBuffer_type)(cl_command_queue, cl_mem, cl_bool, size_t, size_t, const void *, cl_uint, const cl_event *, cl_event *);

#endif // #ifndef DVDT_PROF_HPP
