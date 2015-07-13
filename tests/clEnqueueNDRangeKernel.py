#!/usr/bin/env python
import sys, re

sys.path.append('../python')
from prof_parser import prof_parse
from prof_parser import int_regex
from prof_parser import ptr_regex
from prof_parser import opts_regex

max_work_dim = 3
default_offset = 0
null_offset = 0
default_gws = 1
default_lws = 1
null_lws = 0

# Test info.
call = 'clEnqueueNDRangeKernel'
id_  = ''
print '%s%s' % (call, id_)

# Parse initialisation list of form: lhs = { elem, ... }.
def match_init_list(text, lhs_regex, elem_regex):
    result = []
    match = re.search('%s(\s*)=(\s*)\{(\s*)(?P<elem>%s)' % (lhs_regex, elem_regex), text)
    while match and match.group('elem') != '}':
        result.append(match.group('elem'))
        text = text[match.end():]
        match = re.search('(?P<elem>%s|\})' % elem_regex, text)
    return result

# Parse test source.
source = {}
with open(call + id_ + '.cpp', 'r') as f:
    source['text'] = f.read()
    source['queue'] = re.search('\(cl_command_queue\) (?P<queue>%s)' % ptr_regex, source['text']).group('queue')
    source['kernel'] = re.search('\(cl_kernel\) (?P<kernel>%s)' % ptr_regex, source['text']).group('kernel')
    
    work_dim = int(re.search('work_dim(\s*)=(\s*)(?P<work_dim>\d+)', source['text']).group('work_dim'))
    offset = match_init_list(source['text'], 'global_work_offset\[%d\]' % work_dim, int_regex)
    source['offset'] = ([int(i) for i in offset] + [default_offset] * (max_work_dim - work_dim)) if offset else [null_offset] * max_work_dim
    gws = match_init_list(source['text'], 'global_work_size\[%d\]' % work_dim, int_regex)
    source['gws'] = [int(i) for i in gws] + [default_gws] * (max_work_dim - work_dim)
    lws = match_init_list(source['text'], 'local_work_size\[%d\]' % work_dim, int_regex)
    source['lws'] = ([int(i) for i in lws] + [default_lws] * (max_work_dim - work_dim)) if lws else [null_lws] * max_work_dim

    num_events = int(re.search('num_events_in_wait_list(\s*)=(\s*)(?P<num_events>\d+)', source['text']).group('num_events'))
    cl_event_ptr_list = match_init_list(source['text'], 'event_wait_list\[%d\]' % num_events, '\(cl_event\) %s' % ptr_regex)
    source['event_wait_list'] = [re.match('\(cl_event\) (?P<ptr>%s)' % ptr_regex, cl_event_ptr).group('ptr') for cl_event_ptr in cl_event_ptr_list]

    source['event'] = re.search('\(cl_event \*\) (?P<event>%s)' % ptr_regex, source['text']).group('event')

    profiling_match = re.search('%s (?P<queued>%s) (?P<submit>%s) (?P<start>%s) (?P<end>%s)' % \
        ('profiling', int_regex, int_regex, int_regex, int_regex), source['text'])
    if profiling_match:
        source['profiling'] = {}
        source['profiling']['queued'] = int(profiling_match.group('queued'))
        source['profiling']['submit'] = int(profiling_match.group('submit'))
        source['profiling']['start']  = int(profiling_match.group('start'))
        source['profiling']['end']    = int(profiling_match.group('end'))


# Read from stdin (via pipe).
output = sys.stdin.read()
print 'OUTPUT'
print output

result = prof_parse(output)[0]
print 'RESULT'
print result
print

status = True
status &= (source['queue'] == result['queue'])
status &= (source['kernel'] == result['kernel'])
status &= (cmp(source['offset'], result['offset']) == 0)
status &= (cmp(source['gws'], result['gws']) == 0)
status &= (cmp(source['lws'], result['lws']) == 0)
status &= (cmp(source['event_wait_list'], result['event_wait_list']) == 0)
status &= (source['event'] == result['event'])
status &= (cmp(source['profiling'], result['profiling']) == 0)

print '%s%s: %s' % (call, id_, 'PASSED' if status else 'FAILED')
print
