#include <CL/opencl.h>
#include <assert.h>

int main()
{
    cl_context context = (cl_context) 0x01234567;
    cl_device_id device = (cl_device_id) 0x12345678;
    cl_command_queue_properties properties = (cl_command_queue_properties) 0;
    cl_int * errcode = (cl_int *) 0x23456789;
    
    cl_command_queue queue = clCreateCommandQueue(context, device, properties, errcode);
    assert(((cl_command_queue)0x0 == queue));

    return 0;
}
