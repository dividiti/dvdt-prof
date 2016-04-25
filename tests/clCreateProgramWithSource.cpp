#include <CL/opencl.h>
#include <cassert>

int main()
{
    cl_context context = (cl_context) 0x01234567;
    cl_uint count = 1;
    const char * strings[1] = { "void kernel() {}" };
    const size_t * lengths = (const size_t *) 0x00000000;
    cl_int * errcode = (cl_int *) 0x12345678;

    cl_program program = clCreateProgramWithSource(context, count, strings, lengths, errcode);
    assert((cl_program) 0x00000000 == program);

    return 0;
}
