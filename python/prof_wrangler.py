#
# 2015-2016 (c) dividiti
#

import re
import json

import dateutil.parser

import pandas as pd

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

# Return a DataFrame containing the differences between the profiling markers
# and the timestamps in nanoseconds for a trace with any enqueues.
def df_enqueues_ns(nqs, label_fn = lambda nq: '%s' % str(nq['call_index'])) :
    def _df_data():
	data = [
            {
                'p1 - p0' : nq['profiling']['submit'] - nq['profiling']['queued'], # command queueing time
                'p2 - p1' : nq['profiling']['start']  - nq['profiling']['submit'], # job queueing time
                'p3 - p2' : nq['profiling']['end']    - nq['profiling']['start'],  # kernel execution time
                'p3 - p0' : nq['profiling']['end']    - nq['profiling']['queued'], # total execution time
                't1 - t0' : ts_delta_ns(ts_end=nq['timestamp']['end'], ts_start=nq['timestamp']['start']), # chrono time
                't1 - t0 - (p3 - p0)' :
                            ts_delta_ns(ts_end=nq['timestamp']['end'], ts_start=nq['timestamp']['start']) -
                            nq['profiling']['end'] + nq['profiling']['queued'],    # chrono overhead
            }
            for nq in nqs
        ]
        return data

    def _df_index():
        index = pd.MultiIndex.from_tuples(
            names=('label', 'call'),
            tuples=[ (label_fn(nq), nq['call']) for nq in nqs ]
        )
        return index

    df = pd.DataFrame(data=_df_data(),index=_df_index())
    return df
