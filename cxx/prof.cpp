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
#define FIXED_WIDTH_PTR(ptr) "0x" << std::hex << std::setw(8) << std::setfill('0') << (cl_ulong)(ptr) << std::dec
#else
#define FIXED_WIDTH_PTR(ptr) (ptr)
#endif

cl_context cached_context = NULL;

static const cl_uint max_work_dim = 3;
static const size_t default_global_work_offset = 0;
static const size_t null_global_work_offset = 0;
static const size_t default_global_work_size = 1;
static const size_t default_local_work_size = 1;
static const size_t null_local_work_size = 0;

static const char * prefix = "[dv/dt]";
static const char sep = ' ';
static const char lf = '\n';

//
// Table of contents.
//
// - clCreateCommandQueue()
// - clBuildProgram()
// - clCreateKernel()
// - clEnqueueNDRangeKernel()
//

// https://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clCreateCommandQueue.html
extern CL_API_ENTRY cl_command_queue CL_API_CALL
clCreateCommandQueue(
    cl_context context,
    cl_device_id device,
    cl_command_queue_properties properties,
    cl_int * errcode_ret) CL_API_SUFFIX__VERSION_1_0 
{
    cl_command_queue queue;

    const char * call = "clCreateCommandQueue";

    if (NULL == cached_context)
    {
        cached_context = context;
    }

    // Arguments.
    std::cout << prefix << sep << call << sep << "context"    << sep << FIXED_WIDTH_PTR(context) << lf;
    std::cout << prefix << sep << call << sep << "device"     << sep << FIXED_WIDTH_PTR(device) << lf;
    std::cout << prefix << sep << call << sep << "properties" << sep << properties << lf;
    std::cout << prefix << sep << call << sep << "errcode"    << sep << FIXED_WIDTH_PTR(errcode_ret) << lf;

    // Start timestamp.
    const boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
    std::cout << prefix << sep << call << sep << "start" << sep << boost::posix_time::to_iso_extended_string(start) << lf;

    // Original call.
    clCreateCommandQueue_type clCreateCommandQueue_original = (clCreateCommandQueue_type) dlsym(RTLD_NEXT, call);
#ifndef DVDT_PROF_TEST
    queue = clCreateCommandQueue_original(context, device, properties | CL_QUEUE_PROFILING_ENABLE, errcode_ret);
#else
    queue = (cl_command_queue) 0x0;
#endif
    std::cout << prefix << sep << call << sep << "queue" << sep << FIXED_WIDTH_PTR(queue) << lf;
    
    // End timestamp.
    const boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
    std::cout << prefix << sep << call << sep << "end" << sep << boost::posix_time::to_iso_extended_string(end) << lf << lf;

    return queue;
}


// https://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clBuildProgram.html
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

    // Arguments.
    std::cout << prefix << sep << call << sep << "program" << sep << FIXED_WIDTH_PTR(program) << lf;
    // TODO: num_devices.
    // TODO: device_list.
    std::cout << prefix << sep << call << sep << "options" << sep << (options ? options : "") << lf;
    // TODO: pfn_notify.
    // TODO: user_data.

    // Start timestamp.
    const boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
    std::cout << prefix << sep << call << sep << "start" << sep << boost::posix_time::to_iso_extended_string(start) << lf;

    // Original call.
    clBuildProgram_type clBuildProgram_original = (clBuildProgram_type) dlsym(RTLD_NEXT, call);
#ifndef DVDT_PROF_TEST
    errcode = clBuildProgram_original(program, num_devices, device_list, options, pfn_notify, user_data);
    // TODO: Make the call blocking so (end - start) represents the actual program build time.
#else
    errcode = CL_SUCCESS;
#endif

    // End timestamp.
    const boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
    std::cout << prefix << sep << call << sep << "end" << sep << boost::posix_time::to_iso_extended_string(end) << lf << lf;

    return errcode;
}


// https://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clCreateKernel.html
extern CL_API_ENTRY cl_kernel CL_API_CALL
clCreateKernel(
    cl_program program,
    const char * kernel_name,
    cl_int * errcode_ret) CL_API_SUFFIX__VERSION_1_0
{
    cl_kernel kernel;

    const char * call = "clCreateKernel";

    // Arguments.
    std::cout << prefix << sep << call << sep << "program" << sep << FIXED_WIDTH_PTR(program) << lf;
    std::cout << prefix << sep << call << sep << "name" << sep << kernel_name << lf;
    std::cout << prefix << sep << call << sep << "errcode" << sep << FIXED_WIDTH_PTR(errcode_ret) << lf;

    // Start timestamp.
    const boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
    std::cout << prefix << sep << call << sep << "start" << sep << boost::posix_time::to_iso_extended_string(start) << lf;

    // Original call.
    clCreateKernel_type clCreateKernel_original = (clCreateKernel_type) dlsym(RTLD_NEXT, call);
#ifndef DVDT_PROF_TEST
    kernel = clCreateKernel_original(program, kernel_name, errcode_ret);
#else
    kernel = (cl_kernel) 0x0;
#endif
    std::cout << prefix << sep << call << sep << "kernel" << sep << FIXED_WIDTH_PTR(kernel) << lf;

    // End timestamp.
    const boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
    std::cout << prefix << sep << call << sep << "end" << sep << boost::posix_time::to_iso_extended_string(end) << lf << lf;

    return kernel;
}


// https://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clEnqueueNDRangeKernel.html
extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueNDRangeKernel(
    cl_command_queue queue,
    cl_kernel kernel,
    cl_uint work_dim,
    const size_t *global_work_offset,
    const size_t *global_work_size,
    const size_t *local_work_size,
    cl_uint num_events_in_wait_list,
    const cl_event *event_wait_list,
    cl_event *event) CL_API_SUFFIX__VERSION_1_0
{
    cl_int errcode;

    const char * call = "clEnqueueNDRangeKernel";

    // Arguments.
    std::cout << prefix << sep << call << sep << "queue"  << sep << FIXED_WIDTH_PTR(queue) << lf;
    std::cout << prefix << sep << call << sep << "kernel" << sep << FIXED_WIDTH_PTR(kernel) << lf;
    // - global_work_offset
    std::cout << prefix << sep << call << sep << "offset";
    for (cl_uint d = 0; d < max_work_dim; ++d)
    {
        if (global_work_offset)
        {
            std::cout << sep << (d < work_dim ? global_work_offset[d] : default_global_work_offset);
        }
        else
        {
            std::cout << sep << null_global_work_offset;
        }
    }
    std::cout << lf;
    // - global_work_size
    std::cout << prefix << sep << call << sep << "gws";
    for (cl_uint d = 0; d < max_work_dim; ++d)
    {
        std::cout << sep << (d < work_dim ? global_work_size[d] : default_global_work_size);
    }
    std::cout << lf;
    // - local_work_size
    std::cout << prefix << sep << call << sep << "lws";
    for (cl_uint d = 0; d < max_work_dim; ++d)
    {
        if (local_work_size)
        {
            std::cout << sep << (work_dim ? local_work_size[d] : default_local_work_size);
        }
        else
        {
            std::cout << sep << null_local_work_size;
        }
    }
    std::cout << lf;
    // - event_wait_list
    std::cout << prefix << sep << call << sep << "event_wait_list";
    for (cl_uint e = 0; e < num_events_in_wait_list; ++e)
    {
        std::cout << sep << event_wait_list[e];
    }
    std::cout << lf;
    // - event
    std::cout << prefix << sep << call << sep << "event" << sep << FIXED_WIDTH_PTR(event) << lf;

    // Support for internal profiling.
    cl_event prof_event = 0;
    cl_event * prof_event_p = NULL;
    if (NULL == event)
    {
        cl_event prof_event = clCreateUserEvent(cached_context, NULL);
        prof_event_p = &prof_event;
    }
    else
    {
        prof_event_p = event;
    }

    // Start timestamp.
    const boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
    std::cout << prefix << sep << call << sep << "start" << sep << boost::posix_time::to_iso_extended_string(start) << lf;

    // Original call.
    clEnqueueNDRangeKernel_type clEnqueueNDRangeKernel_original = (clEnqueueNDRangeKernel_type) dlsym(RTLD_NEXT, call);
#ifndef DVDT_PROF_TEST
    errcode = clEnqueueNDRangeKernel_original(queue, kernel,\
        work_dim, global_work_offset, global_work_size, local_work_size, num_events_in_wait_list, event_wait_list, prof_event_p);
    if (CL_SUCCESS != errcode)
    {
        if (NULL == event)
        {
            clReleaseEvent(prof_event);
        }
        return errcode;
    }
    else
    {
        cl_int err = CL_SUCCESS;

        cl_ulong queued, submit, start, end;
        err |= clWaitForEvents(1, prof_event_p);
        err |= clGetEventProfilingInfo(*prof_event_p, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queued, NULL);
        err |= clGetEventProfilingInfo(*prof_event_p, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &submit, NULL);
        err |= clGetEventProfilingInfo(*prof_event_p, CL_PROFILING_COMMAND_START,  sizeof(cl_ulong), &start,  NULL);
        err |= clGetEventProfilingInfo(*prof_event_p, CL_PROFILING_COMMAND_END,    sizeof(cl_ulong), &end,    NULL);
        if (NULL == event)
        {
            err |= clReleaseEvent(prof_event);
        }

        std::cout << prefix << sep << call << sep << "profiling";
        if (CL_SUCCESS != err)
        {
            std::cout << " error: " << err << lf;
        }
        else
        {
            std::cout << sep << queued << sep << submit << sep << start << sep << end << lf;
        }
    }
#else
    errcode = CL_SUCCESS;
#endif // #ifdef DVDT_PROF_TEST

    // End timestamp.
    const boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
    std::cout << prefix << sep << call << sep << "end" << sep << boost::posix_time::to_iso_extended_string(end) << lf << lf;

    return errcode;
}


// https://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clEnqueueReadBuffer.html
extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReadBuffer(
    cl_command_queue queue,
    cl_mem buffer,
    cl_bool blocking,
    size_t offset,
    size_t size,
    void *ptr,
    cl_uint num_events_in_wait_list,
    const cl_event *event_wait_list,
    cl_event *event) CL_API_SUFFIX__VERSION_1_0
{
    cl_int errcode;

    const char * call = "clEnqueueReadBuffer";

    // Arguments.
    std::cout << prefix << sep << call << sep << "queue"  << sep << FIXED_WIDTH_PTR(queue) << lf;
    std::cout << prefix << sep << call << sep << "buffer" << sep << FIXED_WIDTH_PTR(buffer) << lf;
    std::cout << prefix << sep << call << sep << "blocking" << sep << blocking << lf;
    std::cout << prefix << sep << call << sep << "offset" << sep << offset << lf;
    std::cout << prefix << sep << call << sep << "size" << sep << size << lf;
    std::cout << prefix << sep << call << sep << "ptr" << sep << FIXED_WIDTH_PTR(ptr) << lf;
    // - event_wait_list
    std::cout << prefix << sep << call << sep << "event_wait_list";
    for (cl_uint e = 0; e < num_events_in_wait_list; ++e)
    {
        std::cout << sep << event_wait_list[e];
    }
    std::cout << lf;
    // - event
    std::cout << prefix << sep << call << sep << "event" << sep << FIXED_WIDTH_PTR(event) << lf;

    // Support for internal profiling.
    cl_event prof_event = 0;
    cl_event * prof_event_p = NULL;
    if (NULL == event)
    {
        cl_event prof_event = clCreateUserEvent(cached_context, NULL);
        prof_event_p = &prof_event;
    }
    else
    {
        prof_event_p = event;
    }

    // Start timestamp.
    const boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
    std::cout << prefix << sep << call << sep << "start" << sep << boost::posix_time::to_iso_extended_string(start) << lf;

    // Original call.
    clEnqueueReadBuffer_type clEnqueueReadBuffer_original = (clEnqueueReadBuffer_type) dlsym(RTLD_NEXT, call);
#ifndef DVDT_PROF_TEST
    errcode = clEnqueueReadBuffer_original(queue, buffer, blocking, offset, size, ptr, num_events_in_wait_list, event_wait_list, prof_event_p);
    if (CL_SUCCESS != errcode)
    {
        if (NULL == event)
        {
            clReleaseEvent(prof_event);
        }
        return errcode;
    }
    else
    {
        cl_int err = CL_SUCCESS;

        cl_ulong queued, submit, start, end;
        err |= clWaitForEvents(1, prof_event_p);
        err |= clGetEventProfilingInfo(*prof_event_p, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queued, NULL);
        err |= clGetEventProfilingInfo(*prof_event_p, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &submit, NULL);
        err |= clGetEventProfilingInfo(*prof_event_p, CL_PROFILING_COMMAND_START,  sizeof(cl_ulong), &start,  NULL);
        err |= clGetEventProfilingInfo(*prof_event_p, CL_PROFILING_COMMAND_END,    sizeof(cl_ulong), &end,    NULL);
        if (NULL == event)
        {
            err |= clReleaseEvent(prof_event);
        }

        std::cout << prefix << sep << call << sep << "profiling";
        if (CL_SUCCESS != err)
        {
            std::cout << " error: " << err << lf;
        }
        else
        {
            std::cout << sep << queued << sep << submit << sep << start << sep << end << lf;
        }
    }
#else
    errcode = CL_SUCCESS;
#endif // #ifdef DVDT_PROF_TEST

    // End timestamp.
    const boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
    std::cout << prefix << sep << call << sep << "end" << sep << boost::posix_time::to_iso_extended_string(end) << lf << lf;

    return errcode;
}
