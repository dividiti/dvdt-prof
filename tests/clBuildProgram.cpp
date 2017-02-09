#include <CL/opencl.h>
#include <cassert>

typedef void (CL_CALLBACK pfn_notify_t)(cl_program program, void * user_data);

int main()
{
    cl_program program = (cl_program) 0x01234567;
    const cl_uint num_devices = 2;
    cl_device_id device_list[2] = { (cl_device_id) 0x12345678, (cl_device_id) 0x23456789 };
    const char * options = "-Werror -DN=1024";
    pfn_notify_t * pfn_notify = (pfn_notify_t *) 0x3456789a;
    void * user_data = (void *) 0x456789ab;

    cl_int errcode = clBuildProgram(program, num_devices, device_list, options, pfn_notify, user_data);
    assert(CL_SUCCESS == errcode);

    return 0;
}
