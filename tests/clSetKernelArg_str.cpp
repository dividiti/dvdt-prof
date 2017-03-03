#include <CL/opencl.h>
#include <cassert>

int main()
{
    cl_kernel kernel = (cl_kernel) 0x01234567;
    cl_uint arg_index = 2;
    char arg_value[] = "hello world";
    size_t arg_size = 11;
    assert(sizeof(arg_value) == arg_size+1);

    cl_int errcode = clSetKernelArg(kernel, arg_index, arg_size, (const void*) &arg_value);
    assert(CL_SUCCESS == errcode);

    return 0;
}
