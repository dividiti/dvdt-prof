#!/usr/bin/env python
import sys, re

sys.path.append('../python')
from prof_parser import prof_parse
from prof_parser import int_regex
from prof_parser import ptr_regex
from prof_parser import opts_regex

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
def get_source(call, id_):
    source = {}
    with open('%s%s.c' % (call, id_), 'r') as f:
        text = f.read()
    source['queue'] = re.search('\(cl_command_queue\)(\s*)(?P<queue>%s)' % ptr_regex, text).group('queue')
    source['buffer'] = re.search('\(cl_mem\)(\s*)(?P<buffer>%s)' % ptr_regex, text).group('buffer')
    source['blocking'] = int(re.search('blocking(\s*)=(\s*)(?P<blocking>\d)', text).group('blocking'))
    source['offset'] = int(re.search('offset(\s*)=(\s*)(?P<offset>\d+)', text).group('offset'))
    source['size'] = int(re.search('size(\s*)=(\s*)(?P<size>\d+)', text).group('size'))
    source['ptr'] = re.search('\(%s(\s*)void(\s*)\*\)(\s*)(?P<ptr>%s)' % \
        ('const' if call == 'clEnqueueWriteBuffer' else '', ptr_regex), text).group('ptr')
    num_events = int(re.search('num_events_in_wait_list(\s*)=(\s*)(?P<num_events>\d+)', text).group('num_events'))
    cl_event_ptr_list = match_init_list(text, 'event_wait_list\[%d\]' % num_events, '\(cl_event\) %s' % ptr_regex)
    source['event_wait_list'] = [re.match('\(cl_event\) (?P<ptr>%s)' % ptr_regex, cl_event_ptr).group('ptr') for cl_event_ptr in cl_event_ptr_list]
    source['event'] = re.search('\(cl_event \*\) (?P<event>%s|0)' % ptr_regex, text).group('event')

    profiling_match = re.search('%s (?P<queued>%s) (?P<submit>%s) (?P<start>%s) (?P<end>%s)' % \
        ('profiling', int_regex, int_regex, int_regex, int_regex), text)
    if profiling_match:
        source['profiling'] = {}
        source['profiling']['queued'] = int(profiling_match.group('queued'))
        source['profiling']['submit'] = int(profiling_match.group('submit'))
        source['profiling']['start']  = int(profiling_match.group('start'))
        source['profiling']['end']    = int(profiling_match.group('end'))
    return source

# Get result.
def get_result(output):
    return prof_parse(output)[0]

# Test source and result for comparison.
def cmp_source_and_result(source, result):
    status = True
    status &= (source['queue'] == result['queue'])
    status &= (source['buffer'] == result['buffer'])
    status &= (source['blocking'] == result['blocking'])
    status &= (source['offset'] == result['offset'])
    status &= (source['size'] == result['size'])
    status &= (source['ptr'] == result['ptr'])
    status &= (cmp(source['event_wait_list'], result['event_wait_list']) == 0)
    # FIXME: watch for NULL pointers (0 != '0x00000000').
    # status &= (source['event'] == result['event'])
    status &= (cmp(source['profiling'], result['profiling']) == 0)
    return status

def run(call, id_):
    print '%s%s' % (call, id_)

    # Parse test source file.
    source = get_source(call, id_)
    print 'SOURCE'
    print source

    # Read test executable output from stdin (via pipe).
    output = sys.stdin.read()
    print 'OUTPUT'
    print output

    # Parse test executable output.
    result = get_result(output)
    print 'RESULT'
    print result

    print '%s%s: %s' % (call, id_, 'PASSED' if cmp_source_and_result(source, result) else 'FAILED')
    print
