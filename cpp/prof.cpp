//
// 2015-2016 (c) dividiti
//


#include "prof.hpp"

// Static container for profiler's methods and data.
static dvdt::Prof prof;

// Static container for profiler's logger.
static dvdt::ostreamLogger logger(std::cerr, "[dv/dt]");

//
// Table of contents: OpenCL API functions in the alphabetical order.
//
// - clBuildProgram()
// - clCreateBuffer()
// - clCreateCommandQueue()
// - clCreateKernel()
// - clCreateProgramWithSource()
// - clEnqueueNDRangeKernel()
// - clEnqueueReadBuffer()
// - clEnqueueWriteBuffer()
// - clSetKernelArg()
//

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
    logger.log_call(call);

    if (NULL == prof.interceptor.clBuildProgram_original)
    {
        prof.interceptor.clBuildProgram_original = (dvdt::Prof::Interceptor::clBuildProgram_type) dlsym(RTLD_NEXT, call);
    }

    // Arguments.
    logger.log_ptr(call, "program", program);
    // TODO: num_devices.
    // TODO: device_list.
    logger.log_str(call, "options", options ? options : "");
    // TODO: pfn_notify.
    // TODO: user_data.

#ifndef DVDT_PROF_TEST
    logger.log_timestamp_start(call);

    // Original call.
    errcode = prof.interceptor.clBuildProgram_original(\
        program, num_devices, device_list, options, pfn_notify, user_data);
    // TODO: Make the call blocking so (end - start) represents the actual program build time.

    logger.log_timestamp_end(call);
#endif

    // Return value.
    logger.log_num<cl_int>(call, "errcode", errcode); logger.log_lf();

    return errcode;

} // clBuildProgram()


// https://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clCreateBuffer.html
extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateBuffer(
    cl_context context,
    cl_mem_flags flags,
    size_t size,
    void *host_ptr,
    cl_int *errcode_ret) CL_API_SUFFIX__VERSION_1_0
{
    // Return value.
    cl_mem buffer = (cl_mem) 0x0;

    // API call.
    const char * call = "clCreateBuffer";
    logger.log_call(call);

    if (NULL == prof.interceptor.clCreateBuffer_original)
    {
        prof.interceptor.clCreateBuffer_original = (dvdt::Prof::Interceptor::clCreateBuffer_type) dlsym(RTLD_NEXT, call);
    }

    if (NULL == prof.interceptor.context)
    {
        prof.interceptor.context = context;
    }

    // Arguments.
    logger.log_ptr(call, "context", context);
    logger.log_num<cl_mem_flags>(call, "flags", flags);
    logger.log_num<size_t>(call, "size", size);
    logger.log_ptr(call, "host_ptr", host_ptr);
    logger.log_ptr(call, "errcode_ret", errcode_ret);

#ifndef DVDT_PROF_TEST
    logger.log_timestamp_start(call);

    // Original call.
    buffer = prof.interceptor.clCreateBuffer_original(\
        context, flags, size, host_ptr, errcode_ret);

    logger.log_timestamp_end(call);
#endif

    // Return value.
    logger.log_ptr(call, "buffer", buffer); logger.log_lf();

    return buffer;

} // clCreateBuffer()


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
    logger.log_call(call);

    if (NULL == prof.interceptor.clCreateCommandQueue_original)
    {
        prof.interceptor.clCreateCommandQueue_original = (dvdt::Prof::Interceptor::clCreateCommandQueue_type) dlsym(RTLD_NEXT, call);
    }

    if (NULL == prof.interceptor.context)
    {
        prof.interceptor.context = context;
    }

    // Arguments.
    logger.log_ptr(call, "context", context);
    logger.log_ptr(call, "device", device);
    logger.log_num<cl_command_queue_properties>(call, "properties", properties);
    logger.log_ptr(call, "errcode_ret", errcode_ret);

#ifndef DVDT_PROF_TEST
    logger.log_timestamp_start(call);

    // Original call.
    queue = prof.interceptor.clCreateCommandQueue_original(\
        context, device, properties | CL_QUEUE_PROFILING_ENABLE, errcode_ret);

    logger.log_timestamp_end(call);
#endif

    // Return value.
    logger.log_ptr(call, "queue", queue); logger.log_lf();

    return queue;

} // clCreateCommandQueue()


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
    logger.log_call(call);

    if (NULL == prof.interceptor.clCreateKernel_original)
    {
        prof.interceptor.clCreateKernel_original = (dvdt::Prof::Interceptor::clCreateKernel_type) dlsym(RTLD_NEXT, call);
    }

    // Arguments.
    logger.log_ptr(call, "program", program);
    logger.log_str(call, "name", kernel_name);
    logger.log_ptr(call, "errcode_ret", errcode_ret);

#ifndef DVDT_PROF_TEST
    logger.log_timestamp_start(call);

    // Original call.
    kernel = prof.interceptor.clCreateKernel_original(
        program, kernel_name, errcode_ret);

    logger.log_timestamp_end(call);
#endif

    // Return value.
    logger.log_ptr(call, "kernel", kernel); logger.log_lf();

    return kernel;

} // clCreateKernel()


// https://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clCreateProgramWithSource.html
extern CL_API_ENTRY cl_program CL_API_CALL
clCreateProgramWithSource(
    cl_context context,
    cl_uint count,
    const char **strings,
    const size_t *lengths,
    cl_int *errcode_ret) CL_API_SUFFIX__VERSION_1_0
{
    // Return value.
    cl_program program = (cl_program) 0x0;

    // API call.
    const char * call = "clCreateProgramWithSource";
    logger.log_call(call);

    if (NULL == prof.interceptor.clCreateProgramWithSource_original)
    {
        prof.interceptor.clCreateProgramWithSource_original = (dvdt::Prof::Interceptor::clCreateProgramWithSource_type) dlsym(RTLD_NEXT, call);
    }

    if (NULL == prof.interceptor.context)
    {
        prof.interceptor.context = context;
    }

    // Arguments.
    logger.log_ptr(call, "context", context);
    logger.log_num<cl_uint>(call, "count", count);
    logger.log_ptr(call, "strings", strings);
    logger.log_ptr(call, "lengths", lengths);
    logger.log_src(call, count, strings, lengths);
    logger.log_ptr(call, "errcode_ret", errcode_ret);

#ifndef DVDT_PROF_TEST
    logger.log_timestamp_start(call);

    // Original call.
    program = prof.interceptor.clCreateProgramWithSource_original(\
        context, count, strings, lengths, errcode_ret);

    logger.log_timestamp_end(call);
#endif

    // Return value.
    logger.log_ptr(call, "program", program); logger.log_lf();

    return program;

} // clCreateProgramWithSource()


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
    logger.log_call(call);

    if (NULL == prof.interceptor.clEnqueueNDRangeKernel_original)
    {
        prof.interceptor.clEnqueueNDRangeKernel_original = (dvdt::Prof::Interceptor::clEnqueueNDRangeKernel_type) dlsym(RTLD_NEXT, call);
    }

    // Kernel name.
#ifndef DVDT_PROF_TEST
    const size_t max_name_length = 80;
    char name[max_name_length];
    {
        size_t name_length;
        cl_int info_errcode = clGetKernelInfo(\
            kernel, CL_KERNEL_FUNCTION_NAME, max_name_length, name, &name_length);
        assert(info_errcode == CL_SUCCESS && "Failed to get kernel name");
        assert(name_length <= max_name_length);
    }
#else
    const char name[] = "dvdt_prof_kernel";
#endif
    logger.log_str(call, "name", name);

    local_work_size = prof.interceptor.update_lws(name, local_work_size);

    // Arguments.
    logger.log_ptr(call, "queue", queue);
    logger.log_ptr(call, "kernel", kernel);
    logger.log_gwo(call, work_dim, global_work_offset);
    logger.log_gws(call, work_dim, global_work_size);
    logger.log_lws(call, work_dim, local_work_size);
    logger.log_list<cl_event>(call, "event_wait_list", event_wait_list, num_events_in_wait_list);
    logger.log_ptr(call, "event", event);

#ifndef DVDT_PROF_TEST
    logger.log_timestamp_start(call);

    // Event object needed if 'event' is NULL.
    cl_event prof_event_obj;
    cl_event * prof_event = (NULL != event ? event : &prof_event_obj);

    // Original call.
    errcode = prof.interceptor.clEnqueueNDRangeKernel_original(\
        queue, kernel, work_dim, global_work_offset, global_work_size, local_work_size,\
        num_events_in_wait_list, event_wait_list, prof_event);

    logger.log_profiling_info(call, prof_event);

    logger.log_timestamp_end(call);
#endif

    // Return value.
    logger.log_num<cl_int>(call, "errcode", errcode); logger.log_lf();

    return errcode;

} // clEnqueueNDRangeKernel()


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
    logger.log_call(call);

    if (NULL == prof.interceptor.clEnqueueReadBuffer_original)
    {
        prof.interceptor.clEnqueueReadBuffer_original = (dvdt::Prof::Interceptor::clEnqueueReadBuffer_type) dlsym(RTLD_NEXT, call);
    }

    // Arguments.
    logger.log_ptr(call, "queue", queue);
    logger.log_ptr(call, "buffer", buffer);
    logger.log_num<cl_bool>(call, "blocking", blocking);
    logger.log_num<size_t>(call, "offset", offset);
    logger.log_num<size_t>(call, "size", size);
    logger.log_ptr(call, "ptr", ptr);
    // - event_wait_list
    logger.log_list<cl_event>(call, "event_wait_list", event_wait_list, num_events_in_wait_list);
    // - event
    logger.log_ptr(call, "event", event);

#ifndef DVDT_PROF_TEST
    logger.log_timestamp_start(call);

    // Event object needed if 'event' is NULL.
    cl_event prof_event_obj;
    cl_event * prof_event = (NULL != event ? event : &prof_event_obj);

    // Original call.
    errcode = prof.interceptor.clEnqueueReadBuffer_original(queue, buffer, blocking, offset, size, ptr,
        num_events_in_wait_list, event_wait_list, prof_event);

    logger.log_profiling_info(call, prof_event);

    logger.log_timestamp_end(call);
#endif

    // Return value.
    logger.log_num<cl_int>(call, "errcode", errcode); logger.log_lf();

    return errcode;

} // clEnqueueReadBuffer()


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
    logger.log_call(call);

    if (NULL == prof.interceptor.clEnqueueWriteBuffer_original)
    {
        prof.interceptor.clEnqueueWriteBuffer_original = (dvdt::Prof::Interceptor::clEnqueueWriteBuffer_type) dlsym(RTLD_NEXT, call);
    }

    // Arguments.
    logger.log_ptr(call, "queue", queue);
    logger.log_ptr(call, "buffer", buffer);
    logger.log_num<cl_bool>(call, "blocking", blocking);
    logger.log_num<size_t>(call, "offset", offset);
    logger.log_num<size_t>(call, "size", size);
    logger.log_ptr(call, "ptr", ptr);
    logger.log_list<cl_event>(call, "event_wait_list", event_wait_list, num_events_in_wait_list);
    logger.log_ptr(call, "event", event);

#ifndef DVDT_PROF_TEST
    logger.log_timestamp_start(call);

    // Event object needed if 'event' is NULL.
    cl_event prof_event_obj;
    cl_event * prof_event = (NULL != event ? event : &prof_event_obj);

    // Original call.
    errcode = prof.interceptor.clEnqueueWriteBuffer_original(\
        queue, buffer, blocking, offset, size, ptr,
        num_events_in_wait_list, event_wait_list, prof_event);

    logger.log_profiling_info(call, prof_event);

    logger.log_timestamp_end(call);
#endif

    // Return value.
    logger.log_num<cl_int>(call, "errcode", errcode); logger.log_lf();

    return errcode;

} // clEnqueueWriteBuffer()


// https://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clSetKernelArg.html
extern CL_API_ENTRY cl_int CL_API_CALL
clSetKernelArg(
    cl_kernel kernel,
    cl_uint arg_index,
    size_t arg_size,
    const void *arg_value) CL_API_SUFFIX__VERSION_1_0
{
    // Return value.
    cl_int errcode = CL_SUCCESS;

    // API call.
    const char * call = "clSetKernelArg";
    logger.log_call(call);

    if (NULL == prof.interceptor.clSetKernelArg_original)
    {
        prof.interceptor.clSetKernelArg_original = (dvdt::Prof::Interceptor::clSetKernelArg_type) dlsym(RTLD_NEXT, call);
    }

    // Arguments.
    logger.log_ptr(call, "kernel", kernel);
    logger.log_num<cl_uint>(call, "arg_index", arg_index);
    logger.log_num<size_t>(call, "arg_size", arg_size);
    logger.log_ptr(call, "arg_value", arg_value);

#ifndef DVDT_PROF_TEST
    logger.log_timestamp_start(call);

    // Original call.
    errcode = prof.interceptor.clSetKernelArg_original(kernel, arg_index, arg_size, arg_value);

    logger.log_timestamp_end(call);
#endif

    // Return value.
    logger.log_num<cl_int>(call, "errcode", errcode); logger.log_lf();

    return errcode;

} // clSetKernelArg()
