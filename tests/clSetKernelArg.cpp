#include <CL/opencl.h>
#include <cassert>

int main()
{
    cl_kernel kernel = (cl_kernel) 0x01234567;
    cl_uint arg_index = 0;
    size_t arg_size = 4;
    const void * arg_value = (const void *) 0x12345678;

    cl_int errcode = clSetKernelArg(kernel, arg_index, arg_size, arg_value);
    assert(CL_SUCCESS == errcode);

    return 0;
}
