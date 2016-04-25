#include <CL/opencl.h>
#include <cassert>

int main()
{
    cl_context context = (cl_context) 0x01234567;
    cl_mem_flags flags = (cl_mem_flags) 17; // CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR;
    size_t size = 4096;
    void * host_ptr = (void *) 0x12345678;
    cl_int * errcode = (cl_int *) 0x23456789;
    
    cl_mem buffer = clCreateBuffer(context, flags, size, host_ptr, errcode);
    assert((cl_mem) 0x00000000 == buffer);

    return 0;
}
