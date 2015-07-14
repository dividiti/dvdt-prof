//
// 2015 (c) dividiti
//

#ifndef DVDT_PROF_HPP
#define DVDT_PROF_HPP

#include <dlfcn.h>

#include <cstdlib>
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
        typedef cl_command_queue (*clCreateCommandQueue_type)\
            (cl_context, cl_device_id, cl_command_queue_properties, cl_int *errcode_ret);
        
        typedef cl_program (*clCreateProgramWithSource_type)\
            (cl_context, cl_uint count, const char ** strings, const size_t * lengths, cl_int * errcode_ret);
        
        typedef cl_int (*clBuildProgram_type)\
            (cl_program, cl_uint, const cl_device_id *, const char *, void (CL_CALLBACK *)(cl_program, void *), void *);
        
        typedef cl_kernel (*clCreateKernel_type)\
            (cl_program, const char * givenKernelName, cl_int * errcode_ret);
        
        typedef cl_int (*clSetKernelArg_type)\
            (cl_kernel, cl_uint, size_t, const void *);
        
        typedef cl_int (*clEnqueueNDRangeKernel_type)\
            (cl_command_queue, cl_kernel, cl_uint, const size_t *, const size_t *, const size_t *, cl_uint, const cl_event *, cl_event *);
        
        typedef cl_int (*clEnqueueReadBuffer_type)\
            (cl_command_queue, cl_mem, cl_bool, size_t, size_t, void *, cl_uint, const cl_event *, cl_event *);
        
        typedef cl_int (*clEnqueueWriteBuffer_type)\
            (cl_command_queue, cl_mem, cl_bool, size_t, size_t, const void *, cl_uint, const cl_event *, cl_event *);

        // OpenCL API functions from the underlying vendor implementation.
        clCreateCommandQueue_type   clCreateCommandQueue_original;
        clBuildProgram_type         clBuildProgram_original;
        clCreateKernel_type         clCreateKernel_original;
        clEnqueueNDRangeKernel_type clEnqueueNDRangeKernel_original;
        clEnqueueReadBuffer_type    clEnqueueReadBuffer_original;
        clEnqueueWriteBuffer_type   clEnqueueWriteBuffer_original;

        // Cached OpenCL context.
        cl_context context;

        // Mapping a kernel to a local work size tuple that will be used
        // to override the local work size specified in the program.
        std::map<std::string, size_t*> kernel_lws_map;

        // Constructor.
        Interceptor() :
            clCreateCommandQueue_original(NULL),
            clBuildProgram_original(NULL),
            clCreateKernel_original(NULL),
            clEnqueueNDRangeKernel_original(NULL),
            clEnqueueReadBuffer_original(NULL),
            clEnqueueWriteBuffer_original(NULL),
            context(NULL)
        {
            if (const char * kernel_lws_list = getenv("DVDT_PROF_LWS"))
            {
                // DVDT_PROF_LWS="kernel0:lid0,lid1,lid2 kernel1:lid0,lid1,lid2 ..."
                // Spaces delimit elems per kernel; colons delimit kernel names from
                // local work size; commas delimit local work size dimensions.
                const char per_kernel_delim = ' ';
                const std::vector<std::string> per_kernel_elems =
                    split(std::string(kernel_lws_list), per_kernel_delim);

                for (std::vector<std::string>::const_iterator elems_i = per_kernel_elems.begin(),
                    elems_e = per_kernel_elems.end(); elems_i != elems_e; ++elems_i) 
                {
                    const std::string elem(*elems_i);
                    
                    const char kernel_lws_delim = ':';
                    const std::string::size_type pos = elem.find(kernel_lws_delim);
                    assert(pos != std::string::npos);
                    const std::string kernel = elem.substr(0, pos);
                    const std::string lws_list = elem.substr(pos+1);
                    
                    const char lws_delim = ',';
                    const std::vector<std::string> lws_vector = split(lws_list, lws_delim);
                    const std::vector<std::string>::size_type n = lws_vector.size();
                    assert((1 <= n) && (n <= Prof::max_work_dim));
                    // To be deallocated in the destructor.
                    size_t * lws = new size_t[n];
                    for (std::vector<std::string>::size_type i = 0; i < n; ++i)
                    {
                        std::stringstream(lws_vector[i]) >> lws[i];
                    }
                    kernel_lws_map.insert(std::pair<std::string, size_t*>(kernel, lws));
                }
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


} // namespace dvdt



#endif // #ifndef DVDT_PROF_HPP
