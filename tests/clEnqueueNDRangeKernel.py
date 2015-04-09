#!/usr/bin/env python
import sys, re

sys.path.append('../python')
from prof import match
from prof import ptr_regex
from prof import opts_regex

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
with open(call + id_ + '.c', 'r') as f:
    source['text'] = f.read()
    source['queue'] = re.search('\(cl_command_queue\) (?P<queue>%s)' % ptr_regex, source['text']).group('queue')
    source['kernel'] = re.search('\(cl_kernel\) (?P<kernel>%s)' % ptr_regex, source['text']).group('kernel')
    
    work_dim = int(re.search('work_dim(\s*)=(\s*)(?P<work_dim>\d+)', source['text']).group('work_dim'))
    offset = match_init_list(source['text'], 'global_work_offset\[%d\]' % work_dim, '\d+')
    source['offset'] = ([int(i) for i in offset] + [default_offset] * (max_work_dim - work_dim)) if offset else null_offset * max_work_dim
    gws = match_init_list(source['text'], 'global_work_size\[%d\]' % work_dim, '\d+')
    source['gws'] = [int(i) for i in gws] + [default_gws] * (max_work_dim - work_dim)
    lws = match_init_list(source['text'], 'local_work_size\[%d\]' % work_dim, '\d+')
    source['lws'] = ([int(i) for i in lws] + [default_lws] * (max_work_dim - work_dim)) if lws else null_lws * max_work_dim

    num_events = int(re.search('num_events_in_wait_list(\s*)=(\s*)(?P<num_events>\d+)', source['text']).group('num_events'))
    cl_event_ptr_list = match_init_list(source['text'], 'event_wait_list\[%d\]' % num_events, '\(cl_event\) %s' % ptr_regex)
    source['event_wait_list'] = [re.match('\(cl_event\) (?P<ptr>%s)' % ptr_regex, cl_event_ptr).group('ptr') for cl_event_ptr in cl_event_ptr_list]

    source['event'] = re.search('\(cl_event \*\) (?P<event>%s)' % ptr_regex, source['text']).group('event')

# Read from stdin (via pipe).
output = sys.stdin.read()
print 'OUTPUT'
print output

result = match(output)[0]
print 'RESULT'
print result

status = True
status |= (source['queue'] == result['queue'])
status |= (source['kernel'] == result['kernel'])
status |= (cmp(source['offset'], result['offset']) == 0)
status |= (cmp(source['gws'], result['gws']) == 0)
status |= (cmp(source['lws'], result['lws']) == 0)
status |= (cmp(source['event_wait_list'], result['event_wait_list']) == 0)
status |= (source['event'] == result['event'])

print '%s%s: %s' % (call, id_, 'PASSED' if status else 'FAILED')
print
