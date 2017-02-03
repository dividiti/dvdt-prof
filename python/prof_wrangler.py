#
# 2015-2017 (c) dividiti
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
# and the timestamps in nanoseconds for a trace (nqs) with any enqueues
# (clEnqueueNDRangeKernel, clEnqueueReadBuffer, clEnqueueWriteBuffer, etc).
def df_enqueues_ns(nqs,
                   label_fn = lambda nq: '%s' % str(nq['call_index']).zfill(6)) :
    def _df_data():
	data = [
            {
                'p1 - p0' : nq['profiling']['submit'] - nq['profiling']['queued'], # command queueing time
                'p2 - p1' : nq['profiling']['start']  - nq['profiling']['submit'], # job queueing time
                'p3 - p2' : nq['profiling']['end']    - nq['profiling']['start'],  # kernel execution time
                'p3 - p0' : nq['profiling']['end']    - nq['profiling']['queued'], # total execution time
                't1 - t0' : ts_delta_ns(ts_end=nq['timestamp']['end'], ts_start=nq['timestamp']['start']), # chrono time
                '(t1 - t0) - (p3 - p0)' :
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

# Return a DataFrame containing kernel enqueue info.
def df_kernel_enqueues(nqs, unit='ms'):
    multiplier = {
        'ns' : { 'profiling' : 1e-0, 'timestamp' : 1e+9 },
        'us' : { 'profiling' : 1e-3, 'timestamp' : 1e+6 },
        'ms' : { 'profiling' : 1e-6, 'timestamp' : 1e+3 },
        's'  : { 'profiling' : 1e-9, 'timestamp' : 1e+0 }
    }

    df_kernel_enqueues = pd.DataFrame()
    df_kernel_enqueues_tmp = pd.DataFrame(nqs)

    # Flatten work size and offset lists.
    df_kernel_enqueues[['lws0','lws1','lws2']] = df_kernel_enqueues_tmp['lws'].apply(pd.Series)
    df_kernel_enqueues[['gws0','gws1','gws2']] = df_kernel_enqueues_tmp['gws'].apply(pd.Series)
    df_kernel_enqueues[['gwo0','gwo1','gwo2']] = df_kernel_enqueues_tmp['gwo'].apply(pd.Series)

    # Flatten timestamp dictionaries
    df_kernel_enqueues_tmp[['t0','t1']] = df_kernel_enqueues_tmp['timestamp'].apply(pd.Series)
    # Compute the timestamp difference.
    df_kernel_enqueues['t1 - t0 (%s)' % unit] = df_kernel_enqueues_tmp[['t0','t1']] \
        .apply(lambda x: multiplier[unit]['timestamp'] * ts_delta_s(x[0],x[1]), axis=1)

    # Flatten profiling dictionaries
    # NB: Note this approach is different from the one used for timestamps
    # due to non-intuitive order of flattening via .apply(pd.Series).
    df_kernel_enqueues_tmp['p0'] = df_kernel_enqueues_tmp['profiling'].apply(lambda x: x['queued'])
    df_kernel_enqueues_tmp['p1'] = df_kernel_enqueues_tmp['profiling'].apply(lambda x: x['submit'])
    df_kernel_enqueues_tmp['p2'] = df_kernel_enqueues_tmp['profiling'].apply(lambda x: x['start'])
    df_kernel_enqueues_tmp['p3'] = df_kernel_enqueues_tmp['profiling'].apply(lambda x: x['end'])
    # Compute the profiling differences.
    df_kernel_enqueues['p3 - p0 (%s)' % unit] = \
        multiplier[unit]['profiling'] * (df_kernel_enqueues_tmp['p3'] - df_kernel_enqueues_tmp['p0'])
    df_kernel_enqueues['p3 - p2 (%s)' % unit] = \
        multiplier[unit]['profiling'] * (df_kernel_enqueues_tmp['p3'] - df_kernel_enqueues_tmp['p2'])
    df_kernel_enqueues['p2 - p1 (%s)' % unit] = \
        multiplier[unit]['profiling'] * (df_kernel_enqueues_tmp['p2'] - df_kernel_enqueues_tmp['p1'])
    df_kernel_enqueues['p1 - p0 (%s)' % unit] = \
        multiplier[unit]['profiling'] * (df_kernel_enqueues_tmp['p1'] - df_kernel_enqueues_tmp['p0'])

    # Set the index.
    df_kernel_enqueues[['call_index','name']] = df_kernel_enqueues_tmp[['call_index','name']]
    df_kernel_enqueues.set_index(['call_index', 'name'], inplace=True)

    return df_kernel_enqueues


def df_kernel_enqueues_cumulative_time_num(df_kernel_enqueues_all, unit):
    # For each kernel enqueue, create the time column and column of all ones.
    df_time_num = df_kernel_enqueues_all[['p3 - p2 (%s)' % unit]].copy()
    df_time_num['1'] = 1

    # Compute the cumulative time and the number of enqueues.
    df_cumulative_time_num = df_time_num.groupby(level='name').sum()
    # Update the column labels.
    df_cumulative_time_num.columns = ['** Execution time (%s) **' % unit, '** Number of enqueues **']
    # Update the index label.
    df_cumulative_time_num.index.name = '** Kernel name **'

    # Compute the execution time percentage.
    df_cumulative_time_num['** Execution time (%) **'] = 100 * ( \
         df_cumulative_time_num['** Execution time (%s) **' % unit] / \
         df_cumulative_time_num['** Execution time (%s) **' % unit].sum())

    # Sort the columns so that the number of enqueues comes first, and sort the rows in descending order.
    return df_cumulative_time_num[
        ['** Number of enqueues **', '** Execution time (%s) **' % unit, '** Execution time (%) **']
    ].sort_values('** Execution time (%) **', ascending=False)
