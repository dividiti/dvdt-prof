//
// 2015-2016 (c) dividiti
//

#ifndef DVDT_PROF_HPP
#define DVDT_PROF_HPP

#include <dlfcn.h>

#include <cstdlib>
#include <cassert>
#include <ostream>
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

#if (1 == DVDT_PROF_CJSON)
#include <cJSON.h>
#endif

// Log fixed width pointers.
#if (1 == DVDT_PROF_TEST)
#include <iomanip>
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

    class Logger
    {
    public:
        virtual inline void
        log_call(const char * call_name) = 0;

        virtual inline void
        log_gws(const char * call_name, cl_uint work_dim, const size_t * global_work_size) = 0;

        virtual inline void
        log_gwo(const char * call_name, cl_uint work_dim, const size_t * global_work_offset) = 0;

        // NB: Templated function cannot be virtual.
        template <typename elem_ty> inline void
        log_list(const char * call_name, const char * list_name, const elem_ty * list, cl_uint list_size) { };

        virtual inline void
        log_lws(const char * call_name, cl_uint work_dim, const size_t * local_work_size) = 0;

        // NB: Templated function cannot be virtual.
        template <typename num_ty> inline void
        log_num(const char * call_name, const char * arg_name, num_ty arg_value) { };

        virtual inline void
        log_profiling_info(const char * call_name, cl_event * prof_event) = 0;

        virtual inline void
        log_ptr(const char * call_name, const char * arg_name, const void * arg_value) = 0;

        virtual inline void
        log_src(const char * call_name, cl_uint count, const char **strings, const size_t *lengths) = 0;

        virtual inline void
        log_str(const char * call_name, const char * arg_name, const char * arg_value) = 0;

        virtual inline void
        log_timestamp_end(const char * call_name) = 0;

        virtual inline void
        log_timestamp_start(const char * call_name) = 0;

        inline std::string
        ptr_to_str(const void * ptr)
        {
            std::stringstream ss;
#if (1 == DVDT_PROF_TEST)
            ss << "0x" << std::hex << std::setw(8) << std::setfill('0') <<
                reinterpret_cast<uintptr_t>(ptr) << std::dec;
#else
            ss << ptr;
#endif
            return ss.str();
        }
    }; // abstract inner class Logger

    // Interceptor object.
    Interceptor interceptor;

    // Typical implementation-defined constants.
    // TODO: query the actual implementation.
    static const cl_uint max_work_dim = 3;

    // Default values of work size parameters.
    static const size_t default_local_work_size = 1;
    static const size_t null_local_work_size = 0;

    static const size_t default_global_work_size = 1;
    // NB: no null_global_work_size

    static const size_t default_global_work_offset = 0;
    static const size_t null_global_work_offset = 0;

}; // class Prof


class ostreamLogger : public Prof::Logger
{
private:
    std::ostream & stream;

    const char * prefix;
    const char sep;
    const char lf;

public:
    ostreamLogger(std::ostream & _stream=std::clog,
                  const char * _prefix="[dv/dt]",
                  const char _sep=' ',
                  const char _lf='\n') :
        stream(_stream), prefix(_prefix), sep(_sep), lf(_lf)
    {}

    inline void log_prefix() { stream << prefix; }
    inline void log_sep()    { stream << sep;    }
    inline void log_lf()     { stream << lf;     }

public:
    inline void
    log_call(const char * call_name)
    {
        stream << prefix << sep << call_name << lf;
    } // log_call()

    inline void
    log_gws(const char * call_name, cl_uint work_dim, const size_t * global_work_size)
    {
        stream << prefix << sep << call_name << sep << "gws";
        for (cl_uint d = 0; d < dvdt::Prof::max_work_dim; ++d)
        {
            stream << sep << (d < work_dim ? global_work_size[d] : dvdt::Prof::default_global_work_size);
        }
        stream << lf;
    } // log_gws()

    inline void
    log_gwo(const char * call_name, cl_uint work_dim, const size_t * global_work_offset)
    {
        stream << prefix << sep << call_name << sep << "offset";
        for (cl_uint d = 0; d < dvdt::Prof::max_work_dim; ++d)
        {
            if (global_work_offset)
            {
                stream << sep << (d < work_dim ? global_work_offset[d] : dvdt::Prof::default_global_work_offset);
            }
            else
            {
                stream << sep << dvdt::Prof::null_global_work_offset;
            }
        }
        stream << lf;
    } // log_gwo()

    template <typename elem_ty> inline void
    log_list(const char * call_name, const char * list_name, const elem_ty * list, cl_uint list_size)
    {
        stream << prefix << sep << call_name << sep << list_name;
        for (cl_uint i = 0; i < list_size; ++i)
        {
            stream << sep << list[i];
        }
        stream << lf;
    } // log_list()

    inline void
    log_lws(const char * call_name, cl_uint work_dim, const size_t * local_work_size)
    {
        stream << prefix << sep << call_name << sep << "lws";
        for (cl_uint d = 0; d < dvdt::Prof::max_work_dim; ++d)
        {
            if (local_work_size)
            {
                stream << sep << (d < work_dim ? local_work_size[d] : dvdt::Prof::default_local_work_size);
            }
            else
            {
                stream << sep << dvdt::Prof::null_local_work_size;
            }
        }
        stream << lf;
    } // log_lws()

    template <typename num_ty> inline void
    log_num(const char * call_name, const char * arg_name, num_ty arg_value)
    {
        stream << prefix << sep << call_name << sep << arg_name << sep << arg_value << lf;
    } // log_num()

    inline void
    log_profiling_info(const char * call_name, cl_event * prof_event)
    {
        cl_ulong queued, submit, start, end;

        cl_int prof_errcode = CL_SUCCESS;
        prof_errcode |= clWaitForEvents(1, prof_event);
        prof_errcode |= clGetEventProfilingInfo(*prof_event, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queued, NULL);
        prof_errcode |= clGetEventProfilingInfo(*prof_event, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &submit, NULL);
        prof_errcode |= clGetEventProfilingInfo(*prof_event, CL_PROFILING_COMMAND_START,  sizeof(cl_ulong), &start,  NULL);
        prof_errcode |= clGetEventProfilingInfo(*prof_event, CL_PROFILING_COMMAND_END,    sizeof(cl_ulong), &end,    NULL);
        if (CL_SUCCESS != prof_errcode)
        {
            stream << prefix << sep << call_name << sep << "output profiling info error: " << prof_errcode << lf;
        }

        stream << prefix << sep << call_name << sep << "profiling" <<
            sep << queued << sep << submit << sep << start << sep << end << lf;
    } // log_profiling_info()

    inline void
    log_ptr(const char * call_name, const char * arg_name, const void * arg_value)
    {
        stream << prefix << sep << call_name << sep << arg_name << sep << ptr_to_str(arg_value) << lf;
    } // log_ptr()

    inline void
    log_src(const char * call_name, cl_uint count, const char **strings, const size_t *lengths)
    {
        for (cl_uint c = 0; c < count; ++c)
        {
            stream << prefix << sep << call_name << sep << "strings[" << c << "] <<" << lf;
            if (NULL == lengths || 0 == lengths[c])
            {
                // Program string is null-terminated.
                stream << strings[c];
            }
            else
            {
                // When program string it not null-terminated, only
                // print lengths[c] characters from strings[c].
                for (cl_uint k = 0; k < lengths[c]; ++ k)
                {
                    stream << strings[c][k];
                }
            }
            stream << std::endl;
            stream << prefix << sep << call_name << sep << "strings[" << c << "] >>" << lf;
        }
    } // log_src()

    inline void
    log_str(const char * call_name, const char * arg_name, const char * arg_value)
    {
        stream << prefix << sep << call_name << sep << arg_name << sep << arg_value << lf;
    } // log_str()

private:
    inline void
    log_timestamp(const char * call_name, const char * timestamp_kind)
    {
    #if   (1 == DVDT_PROF_WALLCLOCK_BOOST)
        const boost::posix_time::ptime time = boost::posix_time::microsec_clock::universal_time();
        const std::string time_str = boost::posix_time::to_iso_extended_string(time);
    #elif (1 == DVDT_PROF_WALLCLOCK_TIMEOFDAY)
        const std::string time_str("1970-01-01 00:00:00.000");
    #endif
        stream << prefix << sep << call_name << sep << timestamp_kind << sep << time_str << lf;
    } // log_timestamp()
public:
    inline void
    log_timestamp_end(const char * call_name)
    {
        log_timestamp(call_name, "end"  );
    } // log_timestamp_end()

    inline void
    log_timestamp_start(const char * call_name)
    {
        log_timestamp(call_name, "start");
    } // log_timestamp_start()
}; // class ostreamLogger : Logger


class cjsonLogger : public Prof::Logger
{
private:
    std::ostream & stream;

    const char * prefix;
    const char sep;
    const char lf;

    cJSON * calls;
    cJSON * call;

public:
    cjsonLogger(std::ostream & _stream=std::clog,
                  const char * _prefix="[cjson]",
                  const char _sep=' ',
                  const char _lf='\n') :
        stream(_stream), prefix(_prefix), sep(_sep), lf(_lf),
        calls(NULL), call(NULL)
    {
        calls = cJSON_CreateArray();
    }

    ~cjsonLogger()
    {
        // Add last call object to calls array.
        if (call)
        {
            cJSON_AddItemToArray(calls, call);
        }
        // Print calls array.
        stream << "cJSON result:" << std::endl;
        {
            char * result = cJSON_Print(calls);
            stream << result << std::endl;
            free(result);
        }
        // Free calls array.
        cJSON_Delete(calls);
    }

    inline void log_prefix() { stream << prefix; }
    inline void log_sep()    { stream << sep;    }
    inline void log_lf()     { stream << lf;     }

public:
    inline void
    log_call(const char * call_name)
    {
        stream << prefix << sep << call_name << lf; // TBR
        // Add previous call object to calls array.
        if (call)
        {
            cJSON_AddItemToArray(calls, call);
        }
        // Create new call object.
        call = cJSON_CreateObject();
        cJSON_AddItemToObject(call, "call", cJSON_CreateString(call_name));
    } // log_call()

    inline void
    log_gws(const char * call_name, cl_uint work_dim, const size_t * global_work_size)
    {
        stream << prefix << sep << call_name << sep << "gws"; // TBR
        cJSON * gws = cJSON_CreateArray();
        for (cl_uint d = 0; d < dvdt::Prof::max_work_dim; ++d)
        {
            size_t gws_d;
            gws_d = d < work_dim ?
                    global_work_size[d] :
                    dvdt::Prof::default_global_work_size;
            stream << sep << gws_d; // TBR
            cJSON * gws_d_as_num = cJSON_CreateNumber(gws_d);
            cJSON_AddItemToArray(gws, gws_d_as_num);
        }
        stream << lf; // TBR
        cJSON_AddItemToObject(call, "gws", gws);
    } // log_gws()

    inline void
    log_gwo(const char * call_name, cl_uint work_dim, const size_t * global_work_offset)
    {
        stream << prefix << sep << call_name << sep << "offset"; // TBR
        cJSON * gwo = cJSON_CreateArray();
        for (cl_uint d = 0; d < dvdt::Prof::max_work_dim; ++d)
        {
            size_t gwo_d;
            if (global_work_offset)
            {
                gwo_d = d < work_dim ?
                        global_work_offset[d] :
                        dvdt::Prof::default_global_work_offset;
            }
            else
            {
                gwo_d = dvdt::Prof::null_global_work_offset;
            }
            stream << sep << gwo_d; // TBR
            cJSON * gwo_d_as_num = cJSON_CreateNumber(gwo_d);
            cJSON_AddItemToArray(gwo, gwo_d_as_num);
        }
        stream << lf; // TBR
        cJSON_AddItemToObject(call, "gwo", gwo);
    } // log_gwo()

    template <typename elem_ty> inline void
    log_list(const char * call_name, const char * list_name, const elem_ty * list, cl_uint list_size)
    {
        stream << prefix << sep << call_name << sep << list_name; // TBR
        cJSON * list_as_array = cJSON_CreateArray();
        for (cl_uint i = 0; i < list_size; ++i)
        {
            elem_ty list_i = list[i];
            stream << sep << list_i; // TBR
            // FIXME: Currently only used for lists of cl_event's,
            // which can be represented as pointers.
            cJSON * list_i_as_str =
                cJSON_CreateString(ptr_to_str(list_i).c_str());
            cJSON_AddItemToArray(list_as_array, list_i_as_str);
        }
        stream << lf;
        cJSON_AddItemToObject(call, list_name, list_as_array);
    } // log_list()

    inline void
    log_lws(const char * call_name, cl_uint work_dim, const size_t * local_work_size)
    {
        stream << prefix << sep << call_name << sep << "lws"; // TBR
        cJSON * lws = cJSON_CreateArray();
        for (cl_uint d = 0; d < dvdt::Prof::max_work_dim; ++d)
        {
            size_t lws_d;
            if (local_work_size)
            {
                lws_d = d < work_dim ?
                        local_work_size[d] :
                        dvdt::Prof::default_local_work_size;
            }
            else
            {
                lws_d = dvdt::Prof::null_local_work_size;
            }
            stream << sep << lws_d; // TBR
            cJSON * lws_d_as_num = cJSON_CreateNumber(lws_d);
            cJSON_AddItemToArray(lws, lws_d_as_num);
        }
        stream << lf; // TBR
        cJSON_AddItemToObject(call, "lws", lws);
    } // log_lws()

    template <typename num_ty> inline void
    log_num(const char * call_name, const char * arg_name, num_ty arg_value)
    {
        stream << prefix << sep << call_name << sep << arg_name << sep << arg_value << lf; // TBR
        cJSON * arg_value_as_num = cJSON_CreateNumber(
                                       static_cast<double>(arg_value));
        cJSON_AddItemToObject(call, arg_name, arg_value_as_num);
    } // log_num()

    inline void
    log_profiling_info(const char * call_name, cl_event * prof_event)
    {
        cl_ulong queued, submit, start, end;

        cl_int prof_errcode = CL_SUCCESS;
        prof_errcode |= clWaitForEvents(1, prof_event);
        prof_errcode |= clGetEventProfilingInfo(*prof_event, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queued, NULL);
        prof_errcode |= clGetEventProfilingInfo(*prof_event, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &submit, NULL);
        prof_errcode |= clGetEventProfilingInfo(*prof_event, CL_PROFILING_COMMAND_START,  sizeof(cl_ulong), &start,  NULL);
        prof_errcode |= clGetEventProfilingInfo(*prof_event, CL_PROFILING_COMMAND_END,    sizeof(cl_ulong), &end,    NULL);
        if (CL_SUCCESS != prof_errcode)
        {
            stream << prefix << sep << call_name << sep << "output profiling info error: " << prof_errcode << lf; // TBR
            cJSON * prof_errcode_as_num = cJSON_CreateNumber(prof_errcode);
            cJSON_AddItemToObject(call,
                "output profiling_error", prof_errcode_as_num);
        }

        stream << prefix << sep << call_name << sep << "profiling" <<
            sep << queued << sep << submit << sep << start << sep << end << lf; // TBR
        cJSON * profiling = cJSON_CreateObject();
        cJSON * queued_as_num = cJSON_CreateNumber(queued);
        cJSON * submit_as_num = cJSON_CreateNumber(submit);
        cJSON * start_as_num  = cJSON_CreateNumber(start);
        cJSON * end_as_num    = cJSON_CreateNumber(end);
        cJSON_AddItemToObject(profiling, "queued", queued_as_num);
        cJSON_AddItemToObject(profiling, "submit", submit_as_num);
        cJSON_AddItemToObject(profiling, "start",  start_as_num);
        cJSON_AddItemToObject(profiling, "end",    end_as_num);
        cJSON_AddItemToObject(call, "profiling", profiling);
    } // log_profiling_info()

    inline void
    log_ptr(const char * call_name, const char * arg_name, const void * arg_value)
    {
        stream << prefix << sep << call_name << sep << arg_name << sep << ptr_to_str(arg_value) << lf;
    } // log_ptr()

    inline void
    log_src(const char * call_name, cl_uint count, const char **strings, const size_t *lengths)
    {
        for (cl_uint c = 0; c < count; ++c)
        {
            stream << prefix << sep << call_name << sep << "strings[" << c << "] <<" << lf;
            if (NULL == lengths || 0 == lengths[c])
            {
                // Program string is null-terminated.
                stream << strings[c];
            }
            else
            {
                // When program string it not null-terminated, only
                // print lengths[c] characters from strings[c].
                for (cl_uint k = 0; k < lengths[c]; ++ k)
                {
                    stream << strings[c][k];
                }
            }
            stream << std::endl;
            stream << prefix << sep << call_name << sep << "strings[" << c << "] >>" << lf;
        }
    } // log_src()

    inline void
    log_str(const char * call_name, const char * arg_name, const char * arg_value)
    {
        stream << prefix << sep << call_name << sep << arg_name << sep << arg_value << lf;
    } // log_str()

private:
    inline void
    log_timestamp(const char * call_name, const char * timestamp_kind)
    {
    #if   (1 == DVDT_PROF_WALLCLOCK_BOOST)
        const boost::posix_time::ptime time = boost::posix_time::microsec_clock::universal_time();
        const std::string time_str = boost::posix_time::to_iso_extended_string(time);
    #elif (1 == DVDT_PROF_WALLCLOCK_TIMEOFDAY)
        const std::string time_str("1970-01-01 00:00:00.000");
    #endif
        stream << prefix << sep << call_name << sep << timestamp_kind << sep << time_str << lf;
    } // log_timestamp()
public:
    inline void
    log_timestamp_end(const char * call_name)
    {
        log_timestamp(call_name, "end"  );
    } // log_timestamp_end()

    inline void
    log_timestamp_start(const char * call_name)
    {
        log_timestamp(call_name, "start");
    } // log_timestamp_start()
}; // class cjsonLogger : Logger


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
