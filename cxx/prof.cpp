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


// https://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/clCreateCommandQueue.html
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
    std::cout << prefix << sep << call << sep << "context"    << sep << FIXED_WIDTH_PTR(context) << lf;
    std::cout << prefix << sep << call << sep << "device"     << sep << FIXED_WIDTH_PTR(device) << lf;
    std::cout << prefix << sep << call << sep << "properties" << sep << properties << lf;
    std::cout << prefix << sep << call << sep << "errcode"    << sep << FIXED_WIDTH_PTR(errcode_ret) << lf;

    // Original call.
    clCreateCommandQueue_type clCreateCommandQueue_original = (clCreateCommandQueue_type) dlsym(RTLD_NEXT, call);
#ifdef DVDT_PROF_TEST
    queue = (cl_command_queue) 0x0;
#else
    queue = clCreateCommandQueue_original(context, device, properties | CL_QUEUE_PROFILING_ENABLE, errcode_ret);
#endif
    std::cout << prefix << sep << call << sep << "queue" << sep << FIXED_WIDTH_PTR(queue) << lf;
    
    return queue;
}


// https://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/clBuildProgram.html
extern CL_API_ENTRY cl_int CL_API_CALL
clBuildProgram(
    cl_program program,
    cl_uint num_devices,
    const cl_device_id * device_list,
    const char * options,
    void (CL_CALLBACK * pfn_notify)(cl_program program, void * user_data),
    void * user_data) CL_API_SUFFIX__VERSION_1_0
{
    cl_int errcode;

    const char * call = "clBuildProgram";

    // Timestamp.
    const boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
    std::cout << prefix << sep << call << sep << boost::posix_time::to_iso_extended_string(now) << lf;
    
    // Arguments.
    std::cout << prefix << sep << call << sep << "program" << sep << FIXED_WIDTH_PTR(program) << lf;
    // TODO: num_devices.
    // TODO: device_list.
    std::cout << prefix << sep << call << sep << "options" << sep << (options ? options : "") << lf;
    // TODO: pfn_notify.
    // TODO: user_data.

    // Original call.
    clBuildProgram_type clBuildProgram_original = (clBuildProgram_type) dlsym(RTLD_NEXT, call);
#ifndef DVDT_PROF_TEST
    errcode = clBuildProgram_original(program, num_devices, device_list, options, pfn_notify, user_data);
#else
    errcode = CL_SUCCESS;
#endif

    return errcode;
}
