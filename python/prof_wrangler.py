#
# 2015-2016 (c) dividiti
#

import re
import json

import dateutil.parser

def test():
    print ("prof_wrangler.py")

# Return the difference between the end and start timestamps in seconds.
def ts_delta_s(ts_end, ts_start):
    delta = dateutil.parser.parse(ts_end) - dateutil.parser.parse(ts_start)
    delta_s = delta.total_seconds()
    return delta_s

# Return the difference between the end and start timestamps in nanoseconds.
def ts_delta_ns(ts_end, ts_start):
    delta_s = ts_delta_s(ts_end, ts_start)
    delta_ns = int(delta_s * 1e9)
    return delta_ns

# For each call in the trace, add its index to the call dictionary.
def index_calls(trace):
    indexed_trace = [
        dict(trace, call_index=index)
        for trace, index in zip(trace, range(len(trace))) 
    ]
    return indexed_trace

# Return calls in the trace whose names are in the call_names list.
# For example, when calls_names=['clEnqueueNDRangeKernel'], return
# only kernel enqueues.
def filter_calls(trace, call_names):
    filtered_trace = [
        call for call in trace if call['call'] in call_names
    ]
    return filtered_trace
