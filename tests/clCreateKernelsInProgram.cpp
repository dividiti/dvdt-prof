#include <CL/opencl.h>
#include <cassert>

int main()
{
    cl_program program = (cl_program) 0x01234567;
    cl_uint num_kernels = 2;
    cl_kernel * kernels = (cl_kernel *) 0x12345678;
    cl_uint * num_kernels_ret_ptr = (cl_uint *) 0x23456789;

    cl_int errcode = clCreateKernelsInProgram(program, num_kernels, kernels, num_kernels_ret_ptr);
    assert(CL_SUCCESS == errcode);

    return 0;
}
