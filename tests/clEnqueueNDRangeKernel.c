#include <CL/opencl.h>
#include <assert.h>

int main()
{
    cl_command_queue queue = (cl_command_queue) 0x01234567;
    cl_kernel kernel = (cl_kernel) 0x12345678;
    cl_uint work_dim = 2;
    size_t global_work_offset[2] = { 2, 1 };
    size_t global_work_size[2]   = { 1024, 512 };
    size_t * local_work_size = NULL;
    cl_uint num_events_in_wait_list = 2;
    cl_event event_wait_list[2] = { (cl_event) 0x23456789, (cl_event) 0x3456789a };
    cl_event * event = (cl_event *) 0x456789ab;

    cl_int errcode = clEnqueueNDRangeKernel(queue, kernel, \
        work_dim, global_work_offset, global_work_size, local_work_size, num_events_in_wait_list, event_wait_list, event);
    assert(CL_SUCCESS == errcode);

    return 0;
}
