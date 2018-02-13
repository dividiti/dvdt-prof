#
# Copyright (c) 2018 cTuning foundation.
# See CK COPYRIGHT.txt for copyright details.
#
# SPDX-License-Identifier: BSD-3-Clause.
# See CK LICENSE.txt for licensing details.
#

#
# Developer(s):
#   - Grigori Fursin, cTuning foundation, 2018
#   - Anton Lokhmotov, dividiti, 2018
#   - Flavio Vella, dividiti, 2018
#
##############################################################################
# The program provide a friendly interface to parse and visualize information from 
# dividi-prof. 
# 

# MODULE 
# deps: tabulate. To install: $ pip install tabulate --user
#
import os
import dateutil.parser
import json
import argparse
from pprint import pprint
from tabulate import tabulate
#
import dvdt_function as tools
#
####
###  Options todo: --call=NDRange specify opencl operations. NDRange by default
#### Table options... Maybe we want raw data or cvs 

parser = argparse.ArgumentParser(description='DVDT PROF CLI')
#parser.add_argument("--filter_by", action="store", dest="bool_aggregate", help="Show information of unique kernel name")
parser.add_argument("--aggregate", action="store_true",  dest="aggregate_bool", default=None, help="Show information by of unique kernel names")
parser.add_argument("--verbose", action="store",  dest="verbose_lvl", default=0, help="verbose level")
parser.add_argument("--files", action="store", dest="files_name", default="tmp-dvdt-prof.json", type=str, help="pass dvdt-prof.json files column separated")
parser.add_argument("--filter-by-name", action="store", dest="filter_kernel_list", help="show information by a given kernel name")
parser.add_argument("--filter-by-percent", action="store", dest="filter_percent", default=0, type=float, help="show information by a given kernel name")
parser.add_argument("--sort", action="store_true",  dest="sort_bool", default=False, help="Show information sorted by time")

args=parser.parse_args()


### from here variable 
_is_aggregate = args.aggregate_bool
_verbose = args.verbose_lvl
_limit = tools.filter_percent_manager(args.filter_percent)  
_files_lst = tools.files_manager(args.files_name, "tmp-dvdt-prof.json")
_filters_kernel = tools.fiter_by_name_manager(args.filter_kernel_list )
_is_sorted = args.sort_bool

_data_list = []
print _files_lst
for i in _files_lst:
    _data_list.append(tools.json_manager(i))

config_dict = {}
##n Only 1 dvdt-prof file is supported at the moment. 
config_dict['data'] = _data_list
config_dict['filter_kernel'] = _filters_kernel
config_dict['verbose'] = _verbose
config_dict['percent_limit'] = _limit
config_dict['aggregate'] = _is_aggregate
config_dict['sort'] = _is_sorted
config_dict['files_list'] = _files_lst
config_dict['call_name'] = ['clEnqueueNDRangeKernel']
if _verbose > 2:
   tools.print_args
if _verbose > 1:
   print "Print configuration"
   tools.print_configuration(config_dict)

### just one file is supported and NRange is the only supported call
_NDRange_lst =  tools.get_data_from_call(config_dict['data'][0], config_dict['call_name'][0])
_kernel_stat_lst = tools.get_data_from_ndrange(_NDRange_lst)

if config_dict['sort'] is True:
    _kernel_stat_lst = sorted(_kernel_stat_lst, key=lambda k: k['total_time'], reverse=True)

_app_stat_lst = tools.get_application_stat(_kernel_stat_lst)

tools.computing_percent(_kernel_stat_lst, _app_stat_lst['total_kernel_time'])
# othee options here https://pypi.python.org/pypi/tabulate/
print "===== " + config_dict['files_list'][0] + " ====="
 
tools.print_table(_kernel_stat_lst, _app_stat_lst, config_dict['percent_limit'], config_dict['filter_kernel'], "simple")



