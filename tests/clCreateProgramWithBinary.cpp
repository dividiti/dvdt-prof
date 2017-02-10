#include <CL/opencl.h>
#include <cassert>

int main()
{
    cl_context context = (cl_context) 0x01234567;
    cl_uint num_devices = 2;
    cl_device_id device_list[2] = { (cl_device_id) 0x12345678, (cl_device_id) 0x12345678 };
    const size_t * lengths = (const size_t *) 0x23456789;
    const unsigned char ** binaries = (const unsigned char **) 0x3456789A;
    cl_int * binary_status = (cl_int *) 0x456789AB;
    cl_int * errcode_ret = (cl_int *) 0x56789ABC;

    cl_program program = clCreateProgramWithBinary(context,
        num_devices, device_list,
        lengths, binaries,
        binary_status, errcode_ret);
    assert((cl_program) 0x00000000 == program);

    return 0;
}
