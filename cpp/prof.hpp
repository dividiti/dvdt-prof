//
// 2015-2016 (c) dividiti
//

#ifndef DVDT_PROF_HPP
#define DVDT_PROF_HPP

#include <dlfcn.h>

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>

#include <string>
#include <vector>
#include <map>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

// FIXME: still needed?
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#if   (1 == DVDT_PROF_WALLCLOCK_BOOST)
#include <boost/date_time.hpp>
#elif (1 == DVDT_PROF_WALLCLOCK_TIMEOFDAY)
#include <sys/time.h>
#else
#error "Don't know how to measure wall-clock time"
#endif


#if (1 == DVDT_PROF_TEST)
#include <iomanip>
#define FIXED_WIDTH_PTR(ptr) "0x" << std::hex << std::setw(8) << std::setfill('0') << (size_t)(ptr) << std::dec
#else
#define FIXED_WIDTH_PTR(ptr) (ptr)
#endif


namespace dvdt
{

class Prof
{
public:
    class Interceptor
    {
    public:
        // Types of OpenCL API functions.
        typedef cl_int (*clBuildProgram_type)\
            (cl_program, cl_uint, const cl_device_id *, const char *, void (CL_CALLBACK *)(cl_program, void *), void *);

        typedef cl_mem (*clCreateBuffer_type)\
            (cl_context, cl_mem_flags, size_t, void *, cl_int *);

        typedef cl_command_queue (*clCreateCommandQueue_type)\
            (cl_context, cl_device_id, cl_command_queue_properties, cl_int *errcode_ret);

        typedef cl_kernel (*clCreateKernel_type)\
            (cl_program, const char * givenKernelName, cl_int * errcode_ret);

        typedef cl_program (*clCreateProgramWithSource_type)\
            (cl_context, cl_uint count, const char ** strings, const size_t * lengths, cl_int * errcode_ret);

        typedef cl_int (*clEnqueueNDRangeKernel_type)\
            (cl_command_queue, cl_kernel, cl_uint, const size_t *, const size_t *, const size_t *, cl_uint, const cl_event *, cl_event *);

        typedef cl_int (*clEnqueueReadBuffer_type)\
            (cl_command_queue, cl_mem, cl_bool, size_t, size_t, void *, cl_uint, const cl_event *, cl_event *);

        typedef cl_int (*clEnqueueWriteBuffer_type)\
            (cl_command_queue, cl_mem, cl_bool, size_t, size_t, const void *, cl_uint, const cl_event *, cl_event *);

        typedef cl_int (*clSetKernelArg_type)\
            (cl_kernel, cl_uint, size_t, const void *);

        // OpenCL API functions from the underlying vendor implementation.
        clBuildProgram_type            clBuildProgram_original;
        clCreateBuffer_type            clCreateBuffer_original;
        clCreateCommandQueue_type      clCreateCommandQueue_original;
        clCreateKernel_type            clCreateKernel_original;
        clCreateProgramWithSource_type clCreateProgramWithSource_original;
        clEnqueueNDRangeKernel_type    clEnqueueNDRangeKernel_original;
        clEnqueueReadBuffer_type       clEnqueueReadBuffer_original;
        clEnqueueWriteBuffer_type      clEnqueueWriteBuffer_original;
        clSetKernelArg_type            clSetKernelArg_original;

        // Mapping a kernel to a local work size tuple that will be used
        // to override the local work size specified in the program.
        const size_t * update_lws(const char * name, const size_t * program_lws);

        // Constructor.
        Interceptor() :
            clBuildProgram_original(NULL),
            clCreateBuffer_original(NULL),
            clCreateCommandQueue_original(NULL),
            clCreateKernel_original(NULL),
            clCreateProgramWithSource_original(NULL),
            clEnqueueNDRangeKernel_original(NULL),
            clEnqueueReadBuffer_original(NULL),
            clEnqueueWriteBuffer_original(NULL),
            clSetKernelArg_original(NULL),
            kernel_lws_null(false),
            context(NULL)
        {
            if (getenv("DVDT_PROF_LWS_NULL"))
            {
                kernel_lws_null = true;
            }
            else if (const char * kernel_lws_list = getenv("DVDT_PROF_LWS"))
            {
                update_kernel_lws_map(kernel_lws_list);
            }
        }

        // Destructor.
        ~Interceptor()
        {
            // Free local work size values.
            for (std::map<std::string, size_t*>::iterator i = kernel_lws_map.begin(),
                e = kernel_lws_map.end(); i != e; i++)
            {
                delete[] i->second;
            }
        }

    private:
        // The map is populated by parsing an environment variable DVDT_PROF_LWS
        // in the following format:
        //
        //   "kernel_A:lws_A0,lws_A1,lws_A2 kernel_B:lws_B0,lws_B1,lws_B1 ..."
        //
        // Namely, the list elements are separated by spaces; the kernel names
        // (strings) are separated from the local work size tuple by colons;
        // the tuple elements (unsigned integers) are delimited by commas.
        // The number of elements in a tuple must match the number of work-group
        // dimensions as specified in the program or start with the value of 0
        // to use NULL as the local work size for this kernel.
        // (For convenience, kernel_lws_null allows to use NULL for all kernels
        // in the program.)
        std::map<std::string, size_t*> kernel_lws_map;

        // True if NULL is to be used as the local work size for all kernels.
        bool kernel_lws_null;

        // Helper method for update_kernel_lws_map().
        std::vector<std::string> split(const std::string & str, char delim)
        {
            std::vector<std::string> elems;
            std::stringstream ss(str);
            std::string elem;
            while (std::getline(ss, elem, delim))
            {
                elems.push_back(elem);
            }
            return elems;
        }

        // See kernel_lws_map.
        void update_kernel_lws_map(const char * kernel_lws_list);

    public:
        // Cached OpenCL context. (Currently unused.)
        cl_context context;

    }; // inner class Interceptor

    // Interceptor object.
    Interceptor interceptor;

    // Typical implementation-defined constants.
    // TODO: query the actual implementation.
    static const cl_uint max_work_dim = 3;

    // Default values of work size parameters.
    static const size_t default_local_work_size = 1;
    static const size_t null_local_work_size = 0;

    static const size_t default_global_work_size = 1;
    // no null_global_work_size

    static const size_t default_global_work_offset = 0;
    static const size_t null_global_work_offset = 0;

    // Output formatting options.
    const char * prefix;
    const char sep;
    const char lf;

    Prof(const char * _prefix="[dv/dt]", const char _sep = ' ', const char _lf = '\n') :
        prefix(_prefix), sep(_sep), lf(_lf)
    {}

}; // class Prof


void
Prof::Interceptor::update_kernel_lws_map(const char * kernel_lws_list)
{
    // Strip surrounding double quotation marks if present.
    std::string kernel_lws_list_str(kernel_lws_list);
    {
        const char double_quote = '\"';
        const std::string::size_type first = kernel_lws_list_str.find(double_quote);
        const std::string::size_type last = kernel_lws_list_str.find_last_of(double_quote);
        kernel_lws_list_str = kernel_lws_list_str.substr(first+1, last-(first+1));
    }

    // Split space-separated list of elements into vector of elements.
    const char per_kernel_delim = ' ';
    const std::vector<std::string> per_kernel_elems = split(kernel_lws_list_str, per_kernel_delim);

    for (std::vector<std::string>::const_iterator elems_i = per_kernel_elems.begin(),
        elems_e = per_kernel_elems.end(); elems_i != elems_e; ++elems_i)
    {
        const std::string elem(*elems_i);

        // Split element into two colon-separated strings: kernel name and lws tuple.
        const char kernel_lws_delim = ':';
        const std::string::size_type pos = elem.find(kernel_lws_delim);
        assert(pos != std::string::npos);
        const std::string kernel = elem.substr(0, pos);
        const std::string lws_list = elem.substr(pos+1);

        // Split comma-separated lws tuple string into vector of lws dimensions.
        const char lws_delim = ',';
        const std::vector<std::string> lws_vector = split(lws_list, lws_delim);
        const std::vector<std::string>::size_type n = lws_vector.size();
        assert((1 <= n) && (n <= Prof::max_work_dim));
        size_t * lws = new size_t[n]; // To be deallocated in the destructor.
        for (std::vector<std::string>::size_type i = 0; i < n; ++i)
        {
            std::stringstream(lws_vector[i]) >> lws[i];
        }
        // TODO: allow updating the map (e.g. for runtime adaptation).
        assert(kernel_lws_map.count(kernel) == 0);
        kernel_lws_map.insert(std::pair<std::string, size_t*>(kernel, lws));
    }

    return;

} // Prof::Interceptor::update_kernel_lws_map()


const size_t *
Prof::Interceptor::update_lws(const char * name, const size_t * program_lws)
{
    if (kernel_lws_null)
    {
        return NULL;
    }
    std::map<std::string, size_t *>::iterator it = kernel_lws_map.find(std::string(name));
    if (kernel_lws_map.end() != it)
    {
        const size_t * lws = it->second;
        if (0 == lws[0])
        {
            program_lws = NULL;
        }
        else
        {
            program_lws = lws;
        }
    }

    return program_lws;

} // Prof::Interceptor::update_lws()

} // namespace dvdt


#endif // #ifndef DVDT_PROF_HPP
