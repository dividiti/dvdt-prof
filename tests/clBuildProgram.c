#include <CL/opencl.h>
#include <assert.h>

int main()
{
    cl_program program = (cl_program) 0x01234567;
    cl_uint num_devices = 0;
    const cl_device_id * device_list = NULL;
    const char * options = "-Werror -DN=1024";
    void (CL_CALLBACK * pfn_notify)(cl_program program, void * user_data) = NULL;
    void * user_data = NULL;

    cl_int errcode = clBuildProgram(program, num_devices, device_list, options, pfn_notify, user_data);
    assert(CL_SUCCESS == errcode);

    return 0;
}
