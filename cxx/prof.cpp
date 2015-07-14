//
// 2015 (c) dividiti
//


#include "prof.hpp"

// Static container for profiler methods and data.
static dvdt::Prof prof("[dv/dt]");

namespace dvdt {

// Wall-clock timestamps.
static void print_timestamp(const char * call_cstr, const char * kind_cstr)
{
#if   (1 == DVDT_PROF_WALLCLOCK_BOOST)
    const boost::posix_time::ptime time = boost::posix_time::microsec_clock::universal_time();
    const std::string time_str = boost::posix_time::to_iso_extended_string(time);
#elif (1 == DVDT_PROF_WALLCLOCK_TIMEOFDAY)
    const std::string time_str("1970-01-01 00:00:00.000");
#endif
    std::cout << prof.prefix << prof.sep << call_cstr << prof.sep << kind_cstr << prof.sep << time_str << prof.lf;
}

static void start_timestamp(const char * call) { print_timestamp(call, "start"); }
static void end_timestamp(const char * call)   { print_timestamp(call, "end"); }

//
// Internal profiling support.
//
void
output_profiling_info(const char * call, cl_event * prof_event)
{
    cl_ulong queued, submit, start, end;

    cl_int prof_errcode = CL_SUCCESS;
    prof_errcode |= clWaitForEvents(1, prof_event);
    prof_errcode |= clGetEventProfilingInfo(*prof_event, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queued, NULL);
    prof_errcode |= clGetEventProfilingInfo(*prof_event, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &submit, NULL);
    prof_errcode |= clGetEventProfilingInfo(*prof_event, CL_PROFILING_COMMAND_START,  sizeof(cl_ulong), &start,  NULL);
    prof_errcode |= clGetEventProfilingInfo(*prof_event, CL_PROFILING_COMMAND_END,    sizeof(cl_ulong), &end,    NULL);
    if (CL_SUCCESS != prof_errcode)
    {
        std::cout << prof.prefix << prof.sep << call << prof.sep << "output profiling info error: " << prof_errcode << prof.lf;
    }

    std::cout << prof.prefix << prof.sep << call << prof.sep << "profiling" <<
        prof.sep << queued << prof.sep << submit << prof.sep << start << prof.sep << end << prof.lf;
}


} // namespace dvdt


//
// Table of contents.
//
// - clCreateCommandQueue()
// - clBuildProgram()
// - clCreateKernel()
// - clEnqueueNDRangeKernel()
// - clEnqueueReadBuffer()
// - clEnqueueWriteBuffer()
//


// https://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clCreateCommandQueue.html
extern CL_API_ENTRY cl_command_queue CL_API_CALL
clCreateCommandQueue(
    cl_context context,
    cl_device_id device,
    cl_command_queue_properties properties,
    cl_int * errcode_ret) CL_API_SUFFIX__VERSION_1_0 
{
    // Return value.
    cl_command_queue queue = (cl_command_queue) 0x0;

    // API call.
    const char * call = "clCreateCommandQueue";
    std::cout << prof.prefix << prof.sep << call << prof.lf;

    if (NULL == prof.interceptor.clCreateCommandQueue_original)
    {
        prof.interceptor.clCreateCommandQueue_original = (dvdt::Prof::Interceptor::clCreateCommandQueue_type) dlsym(RTLD_NEXT, call);
    }

    if (NULL == prof.interceptor.context)
    {
        prof.interceptor.context = context;
    }

    // Arguments.
    std::cout << prof.prefix << prof.sep << call << prof.sep << "context"     << prof.sep << FIXED_WIDTH_PTR(context) << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "device"      << prof.sep << FIXED_WIDTH_PTR(device) << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "properties"  << prof.sep << properties << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "errcode_ret" << prof.sep << FIXED_WIDTH_PTR(errcode_ret) << prof.lf;

#ifndef DVDT_PROF_TEST
    dvdt::start_timestamp(call);

    // Original call.
    queue = prof.interceptor.clCreateCommandQueue_original(\
        context, device, properties | CL_QUEUE_PROFILING_ENABLE, errcode_ret);
    
    dvdt::end_timestamp(call);
#endif

    // Return value.
    std::cout << prof.prefix << prof.sep << call << prof.sep << "queue" << prof.sep << FIXED_WIDTH_PTR(queue) << prof.lf << prof.lf;

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
    // Return value.
    cl_int errcode = CL_SUCCESS;

    // API call.
    const char * call = "clBuildProgram";
    std::cout << prof.prefix << prof.sep << call << prof.lf;

    if (NULL == prof.interceptor.clBuildProgram_original)
    {
        prof.interceptor.clBuildProgram_original = (dvdt::Prof::Interceptor::clBuildProgram_type) dlsym(RTLD_NEXT, call);
    }

    // Arguments.
    std::cout << prof.prefix << prof.sep << call << prof.sep << "program" << prof.sep << FIXED_WIDTH_PTR(program) << prof.lf;
    // TODO: num_devices.
    // TODO: device_list.
    std::cout << prof.prefix << prof.sep << call << prof.sep << "options" << prof.sep << (options ? options : "") << prof.lf;
    // TODO: pfn_notify.
    // TODO: user_data.

#ifndef DVDT_PROF_TEST
    dvdt::start_timestamp(call);

    // Original call.
    errcode = prof.interceptor.clBuildProgram_original(\
        program, num_devices, device_list, options, pfn_notify, user_data);
    // TODO: Make the call blocking so (end - start) represents the actual program build time.

    dvdt::end_timestamp(call);
#endif

    // Return value.
    std::cout << prof.prefix << prof.sep << call << prof.sep << "errcode" << prof.sep << errcode << prof.lf << prof.lf;

    return errcode;
}


// https://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clCreateKernel.html
extern CL_API_ENTRY cl_kernel CL_API_CALL
clCreateKernel(
    cl_program program,
    const char * kernel_name,
    cl_int * errcode_ret) CL_API_SUFFIX__VERSION_1_0
{
    // Return value.
    cl_kernel kernel = (cl_kernel) 0x0;

    // API call.
    const char * call = "clCreateKernel";
    std::cout << prof.prefix << prof.sep << call << prof.lf;

    if (NULL == prof.interceptor.clCreateKernel_original)
    {
        prof.interceptor.clCreateKernel_original = (dvdt::Prof::Interceptor::clCreateKernel_type) dlsym(RTLD_NEXT, call);
    }

    // Arguments.
    std::cout << prof.prefix << prof.sep << call << prof.sep << "program" << prof.sep << FIXED_WIDTH_PTR(program) << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "name" << prof.sep << kernel_name << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "errcode_ret" << prof.sep << FIXED_WIDTH_PTR(errcode_ret) << prof.lf;

#ifndef DVDT_PROF_TEST
    dvdt::start_timestamp(call);

    // Original call.
    kernel = prof.interceptor.clCreateKernel_original(
        program, kernel_name, errcode_ret);

    dvdt::end_timestamp(call);
#endif

    // Return value.
    std::cout << prof.prefix << prof.sep << call << prof.sep << "kernel" << prof.sep << FIXED_WIDTH_PTR(kernel) << prof.lf << prof.lf;

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
    // Return value.
    cl_int errcode = CL_SUCCESS;

    // API call.
    const char * call = "clEnqueueNDRangeKernel";
    std::cout << prof.prefix << prof.sep << call << prof.lf;

    if (NULL == prof.interceptor.clEnqueueNDRangeKernel_original)
    {
        prof.interceptor.clEnqueueNDRangeKernel_original = (dvdt::Prof::Interceptor::clEnqueueNDRangeKernel_type) dlsym(RTLD_NEXT, call);
    }

    // Kernel name.
#ifdef DVDT_PROF_TEST
    const char name[] = "dvdt_prof_kernel";
#else
    const size_t max_name_length = 80;
    char name[max_name_length];
    {
        size_t name_length;
        cl_int info_errcode = clGetKernelInfo(\
            kernel, CL_KERNEL_FUNCTION_NAME, max_name_length, name, &name_length);
        assert(info_errcode == CL_SUCCESS && "Failed to get kernel name");
        assert(name_length <= max_name_length);
    }

    local_work_size = prof.interceptor.update_lws(name, local_work_size);
#endif
    std::cout << prof.prefix << prof.sep << call << prof.sep << "name"  << prof.sep << name << prof.lf;

    // Arguments.
    std::cout << prof.prefix << prof.sep << call << prof.sep << "queue"  << prof.sep << FIXED_WIDTH_PTR(queue) << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "kernel" << prof.sep << FIXED_WIDTH_PTR(kernel) << prof.lf;
    // - global_work_offset
    std::cout << prof.prefix << prof.sep << call << prof.sep << "offset";
    for (cl_uint d = 0; d < dvdt::Prof::max_work_dim; ++d)
    {
        if (global_work_offset)
        {
            std::cout << prof.sep << (d < work_dim ? global_work_offset[d] : dvdt::Prof::default_global_work_offset);
        }
        else
        {
            std::cout << prof.sep << dvdt::Prof::null_global_work_offset;
        }
    }
    std::cout << prof.lf;
    // - global_work_size
    std::cout << prof.prefix << prof.sep << call << prof.sep << "gws";
    for (cl_uint d = 0; d < dvdt::Prof::max_work_dim; ++d)
    {
        std::cout << prof.sep << (d < work_dim ? global_work_size[d] : dvdt::Prof::default_global_work_size);
    }
    std::cout << prof.lf;
    // - local_work_size
    std::cout << prof.prefix << prof.sep << call << prof.sep << "lws";
    for (cl_uint d = 0; d < dvdt::Prof::max_work_dim; ++d)
    {
        if (local_work_size)
        {
            std::cout << prof.sep << (d < work_dim ? local_work_size[d] : dvdt::Prof::default_local_work_size);
        }
        else
        {
            std::cout << prof.sep << dvdt::Prof::null_local_work_size;
        }
    }
    std::cout << prof.lf;
    // - event_wait_list
    std::cout << prof.prefix << prof.sep << call << prof.sep << "event_wait_list";
    for (cl_uint e = 0; e < num_events_in_wait_list; ++e)
    {
        std::cout << prof.sep << event_wait_list[e];
    }
    std::cout << prof.lf;
    // - event
    std::cout << prof.prefix << prof.sep << call << prof.sep << "event" << prof.sep << FIXED_WIDTH_PTR(event) << prof.lf;

#ifndef DVDT_PROF_TEST
    dvdt::start_timestamp(call);

    // Event object needed if 'event' is NULL.
    cl_event prof_event_obj;
    cl_event * prof_event = (NULL != event ? event : &prof_event_obj);

    // Original call.
    errcode = prof.interceptor.clEnqueueNDRangeKernel_original(\
        queue, kernel, work_dim, global_work_offset, global_work_size, local_work_size,\
        num_events_in_wait_list, event_wait_list, prof_event);
    if (CL_SUCCESS != errcode)
    {
        return errcode;
    }

    dvdt::output_profiling_info(call, prof_event);

    dvdt::end_timestamp(call);
#endif

    // Return value.
    std::cout << prof.prefix << prof.sep << call << prof.sep << "errcode" << prof.sep << errcode << prof.lf << prof.lf;

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
    // Return value.
    cl_int errcode = CL_SUCCESS;

    // API call.
    const char * call = "clEnqueueReadBuffer";
    std::cout << prof.prefix << prof.sep << call << prof.lf;

    if (NULL == prof.interceptor.clEnqueueReadBuffer_original)
    {
        prof.interceptor.clEnqueueReadBuffer_original = (dvdt::Prof::Interceptor::clEnqueueReadBuffer_type) dlsym(RTLD_NEXT, call);
    }

    // Arguments.
    std::cout << prof.prefix << prof.sep << call << prof.sep << "queue"  << prof.sep << FIXED_WIDTH_PTR(queue) << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "buffer" << prof.sep << FIXED_WIDTH_PTR(buffer) << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "blocking" << prof.sep << blocking << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "offset" << prof.sep << offset << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "size" << prof.sep << size << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "ptr" << prof.sep << FIXED_WIDTH_PTR(ptr) << prof.lf;
    // - event_wait_list
    std::cout << prof.prefix << prof.sep << call << prof.sep << "event_wait_list";
    for (cl_uint e = 0; e < num_events_in_wait_list; ++e)
    {
        std::cout << prof.sep << event_wait_list[e];
    }
    std::cout << prof.lf;
    // - event
    std::cout << prof.prefix << prof.sep << call << prof.sep << "event" << prof.sep << FIXED_WIDTH_PTR(event) << prof.lf;

#ifndef DVDT_PROF_TEST
    dvdt::start_timestamp(call);

    // Event object needed if 'event' is NULL.
    cl_event prof_event_obj;
    cl_event * prof_event = (NULL != event ? event : &prof_event_obj);

    // Original call.
    errcode = prof.interceptor.clEnqueueReadBuffer_original(queue, buffer, blocking, offset, size, ptr,
        num_events_in_wait_list, event_wait_list, prof_event);
    if (CL_SUCCESS != errcode)
    {
        return errcode;
    }

    dvdt::output_profiling_info(call, prof_event);

    dvdt::end_timestamp(call);
#endif

    // Return value.
    std::cout << prof.prefix << prof.sep << call << prof.sep << "errcode" << prof.sep << errcode << prof.lf << prof.lf;

    return errcode;
}


// https://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clEnqueueWriteBuffer.html
extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueWriteBuffer(
    cl_command_queue queue,
    cl_mem buffer,
    cl_bool blocking,
    size_t offset,
    size_t size,
    const void *ptr,
    cl_uint num_events_in_wait_list,
    const cl_event *event_wait_list,
    cl_event *event) CL_API_SUFFIX__VERSION_1_0
{
    // Return value.
    cl_int errcode = CL_SUCCESS;

    // API call.
    const char * call = "clEnqueueWriteBuffer";
    std::cout << prof.prefix << prof.sep << call << prof.lf;

    if (NULL == prof.interceptor.clEnqueueWriteBuffer_original)
    {
        prof.interceptor.clEnqueueWriteBuffer_original = (dvdt::Prof::Interceptor::clEnqueueWriteBuffer_type) dlsym(RTLD_NEXT, call);
    }

    // Arguments.
    std::cout << prof.prefix << prof.sep << call << prof.sep << "queue"  << prof.sep << FIXED_WIDTH_PTR(queue) << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "buffer" << prof.sep << FIXED_WIDTH_PTR(buffer) << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "blocking" << prof.sep << blocking << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "offset" << prof.sep << offset << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "size" << prof.sep << size << prof.lf;
    std::cout << prof.prefix << prof.sep << call << prof.sep << "ptr" << prof.sep << FIXED_WIDTH_PTR(ptr) << prof.lf;
    // - event_wait_list
    std::cout << prof.prefix << prof.sep << call << prof.sep << "event_wait_list";
    for (cl_uint e = 0; e < num_events_in_wait_list; ++e)
    {
        std::cout << prof.sep << event_wait_list[e];
    }
    std::cout << prof.lf;
    // - event
    std::cout << prof.prefix << prof.sep << call << prof.sep << "event" << prof.sep << FIXED_WIDTH_PTR(event) << prof.lf;

#ifndef DVDT_PROF_TEST
    dvdt::start_timestamp(call);

    // Event object needed if 'event' is NULL.
    cl_event prof_event_obj;
    cl_event * prof_event = (NULL != event ? event : &prof_event_obj);

    // Original call.
    errcode = prof.interceptor.clEnqueueWriteBuffer_original(\
        queue, buffer, blocking, offset, size, ptr,
        num_events_in_wait_list, event_wait_list, prof_event);
    if (CL_SUCCESS != errcode)
    {
        return errcode;
    }

    dvdt::output_profiling_info(call, prof_event);

    dvdt::end_timestamp(call);
#endif

    // Return value.
    std::cout << prof.prefix << prof.sep << call << prof.sep << "errcode" << prof.sep << errcode << prof.lf << prof.lf;

    return errcode;
}
