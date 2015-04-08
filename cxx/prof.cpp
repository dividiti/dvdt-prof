#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "prof.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include <boost/date_time.hpp>
#include <iomanip>


#ifdef DVDT_PROF_TEST 
#define FIXED_WIDTH_PTR(ptr) "0x" << std::hex << std::setw(8) << std::setfill('0') << (cl_ulong)(ptr)
#else
#define FIXED_WIDTH_PTR(ptr) (ptr)
#endif


static const char * prefix = "[dv/dt]";
static const char sep = ' ';
static const char lf = '\n';


extern CL_API_ENTRY cl_command_queue CL_API_CALL
clCreateCommandQueue(
    cl_context context,
    cl_device_id device,
    cl_command_queue_properties properties,
    cl_int * errcode_ret) CL_API_SUFFIX__VERSION_1_0 
{
    cl_command_queue queue;

    const char * call = "clCreateCommandQueue";

    // Timestamp.
    const boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
    std::cout << prefix << sep << call << sep << boost::posix_time::to_iso_extended_string(now) << lf;

    // Arguments.
    std::cout << prefix << sep << call << sep << "cl_context" << sep << FIXED_WIDTH_PTR(context) << lf;
    std::cout << prefix << sep << call << sep << "cl_device_id" << sep << FIXED_WIDTH_PTR(device) << lf;
    std::cout << prefix << sep << call << sep << "cl_command_queue_properties" << sep << properties << lf;
    std::cout << prefix << sep << call << sep << "errcode_ret" << sep << FIXED_WIDTH_PTR(errcode_ret) << lf;

    // Original call.
    clCreateCommandQueue_type clCreateCommandQueue_original = (clCreateCommandQueue_type) dlsym(RTLD_NEXT, call);
#ifdef DVDT_PROF_TEST
    queue = (cl_command_queue) 0x0;
#else
    queue = clCreateCommandQueue_original(context, device, properties | CL_QUEUE_PROFILING_ENABLE, errcode_ret);
#endif
    std::cout << prefix << sep << call << sep << "cl_command_queue" << sep << FIXED_WIDTH_PTR(queue) << lf;
    
    return queue;
}
