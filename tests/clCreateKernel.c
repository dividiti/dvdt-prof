#include <CL/opencl.h>
#include <assert.h>

int main()
{
    cl_program program = (cl_program) 0x01234567;
    const char * kernel_name = "sgemm";
    cl_int * errcode = (cl_int *) 0x23456789;

    cl_kernel kernel = clCreateKernel(program, kernel_name, errcode);
    assert((cl_kernel)0x0 == kernel);

    return 0;
}
