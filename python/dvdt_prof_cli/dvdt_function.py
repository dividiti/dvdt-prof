import os.path
import json
from tabulate import tabulate



def print_args(args):  
    if args.aggregate_bool is not None:
         print args.aggregate_bool
    if args.filter_kernel_list is not None:
         print args.filter_kernel_list
    print args.files_name
    print args.filter_percent
    print args.verbose_lvl

def print_configuration(conf_dict):
    for i in conf_dict:
        if i != 'data':
            print i + ": ", conf_dict[i]
 
def files_manager(files_str, file_default, verbosity=0):
    files_lst = []
    ### manage default case 
    if files_str.lower() == file_default:
        return files_lst.append(files_str.lower())
    ### manage string seperated by columns
    file_list_all = files_str.split(',')
    for f in file_list_all:
        if os.path.isfile(f) is True:
           files_lst.append(f) 
    ### here all the file should exist 
    return files_lst


def json_manager(fp):
    with open(fp, 'r') as f:
       dvp = json.load(f)
    ## return dictionary
    return dvp


def filter_percent_manager(percent, verbosity=0):
    # silent 
    new_percent = percent
    if percent > 100.0:
        new_percent = 0.0
    elif percent < 0.0:
        new_percent = 0.0    
    return new_percent

def fiter_by_name_manager(filter_kernel_str, verbosity=0):
    filter_kernel_lst = []
    if filter_kernel_str is None:
        return filter_kernel_lst
    else:
        filter_kernel_lst = filter_kernel_str.split(',')
    return filter_kernel_lst


def get_data_from_call(dvp, call_name="clEnqueueNDRangeKernel"):
    call_list = []
    for opencl_function in dvp:
        if opencl_function['call'] == call_name:
            call_list.append(opencl_function)
    return call_list



##### Function per call/applcation and general function

### !!!! THE Function modifies k
def computing_percent(k_lst, total_time):
    for i in k_lst:
        t_i = float(i['total_time'])
        percent = (t_i*100.0)/total_time
        i['percent'] = percent



 
def get_application_stat(kernel_stat_lst):
    application_statistics = {}
    application_statistics['total_kernel_num'] = len(kernel_stat_lst)
    total_time = 0.0 
    ## compute total time 
    for i in kernel_stat_lst:
        total_time += i['total_time']
        application_statistics['total_kernel_time'] = total_time
    application_statistics['unit'] = kernel_stat_lst[0]['unit']
    return application_statistics
### NDRange

def get_data_from_ndrange(NDRange_list, unit="ms"):
    stat_lst = []
    ## Select unit
    ## To Do. Put in scientific format
    ms = 1000000.0
    sec = 1000000000.0
    if unit == "ms":
       unit_scale = ms
    else:
       unit_scale = sec

    for k in NDRange_list:
        ## kernel is in nano seconds
        tmp = {}
        tmp['kernel_id'] = str(k['kernel'])
        tmp['kernel_name'] = k['name']
        total_kernel_time = int(k['profiling']['end']) -int (k['profiling']['start'])
        t_scale = total_kernel_time/unit_scale
        tmp ['total_time'] = t_scale
        tmp ['configuration'] = {'gws': k['gws'], 'lws':k['lws']}
        tmp ['unit'] = unit
        stat_lst.append(tmp)
    return stat_lst




##### VISUALIZATION 

# add option to save format 
# add regex per file name
def print_table(k_lst, app_lst, limit=0, by_kernel=[], view="simple"):
     header =  ["Kernel_id", "Kernel_name", "Time ("+ app_lst['unit']+ ")", "Percent (%)", "GWS", "LWS"] 
     total_calls = 0
     partial_percent = 0.0
     partial_time = 0.0
     value  = []
     if len(by_kernel) == 0:
         for i in k_lst:
             if i["percent"] > limit:
                 total_calls = total_calls + 1
                 gws = i['configuration']['gws']
                 lws = i['configuration']['lws']
                 value.append( [i["kernel_id"], i["kernel_name"], format(i['total_time'],'.2f'), format(i['percent'],'.2f'), gws, lws ])
                 partial_time    += i['total_time']
                 partial_percent += i["percent"]

     else: 
         for i in k_lst:
             if i["percent"] > limit and i["kernel_name"] in by_kernel:
                 total_calls = total_calls + 1
                 gws = i['configuration']['gws']
                 lws = i['configuration']['lws']
                 value.append( [i["kernel_id"], i["kernel_name"], format(i['total_time'],'.2f'), format(i['percent'],'.2f'), gws, lws ])
                 partial_time    += i['total_time']
                 partial_percent += i["percent"]
     ## APP INFO
     app_header = ["Filter_by", "Threshold % > ", "Calls", "Partial time", "Partial percent" ]
     app_value = [[by_kernel, limit, total_calls, partial_time, partial_percent ]]
     #value.append([total_calls,"-", format(partial_time,'.2f') , format(partial_percent,'.2f')])
     print "\n"
     print tabulate(value, header, tablefmt=view)        
     print "\n"
     print tabulate(app_value, app_header, tablefmt="rst")

