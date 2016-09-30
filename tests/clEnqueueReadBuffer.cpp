#include <CL/opencl.h>
#include <cassert>
#include <iostream>

int main()
{
    cl_command_queue queue = (cl_command_queue) 0x01234567;
    cl_mem buffer = (cl_mem) 0x12345678;
    cl_bool blocking = 1;
    size_t offset = 44;
    size_t size = 55;
    void *ptr = (void *) 0x23456789;
    cl_uint num_events_in_wait_list = 3;
    cl_event event_wait_list[3] = { (cl_event) 0x3456789a, (cl_event) 0x456789ab, (cl_event) 0x56789abc };
    cl_event * event = (cl_event *) 0;

    cl_int errcode = clEnqueueReadBuffer(queue, buffer, blocking, offset, size, ptr, num_events_in_wait_list, event_wait_list, event);
    assert(CL_SUCCESS == errcode);

    // Uncomment to emulate profiling output (deprecated approach).
    // NB: Pattern matching still works even when it's commented out.
    // std::cout << "[dv/dt] clEnqueueReadBuffer profiling 100200300400 100200300500 100200300600 100200300700\n";

    return 0;
}
