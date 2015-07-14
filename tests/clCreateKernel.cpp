#include <CL/opencl.h>
#include <cassert>

int main()
{
    cl_program program = (cl_program) 0x01234567;
    const char * kernel_name = "DGEMM_NT_2x2";
    cl_int * errcode = (cl_int *) 0x12345678;

    cl_kernel kernel = clCreateKernel(program, kernel_name, errcode);
    assert((cl_kernel) 0x00000000 == kernel);

    return 0;
}
