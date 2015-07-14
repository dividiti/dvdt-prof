#!/usr/bin/env python
import sys, re

sys.path.append('../python')
from prof_parser import prof_parse
from prof_parser import ptr_regex

# Test info.
call = 'clCreateCommandQueue'
_id  = ''
print '%s%s' % (call, _id)

# Parse test source.
source = {}
with open(call + _id + '.cpp', 'r') as f:
    source['text'] = f.read()
    source['context'] = re.search('\(cl_context\) (?P<context>%s)' % ptr_regex, source['text']).group('context')
    source['device'] = re.search('\(cl_device_id\) (?P<device>%s)' % ptr_regex, source['text']).group('device')
    source['properties'] = re.search('\(cl_command_queue_properties\) (?P<props>\d*)', source['text']).group('props')
    source['errcode_ret'] = re.search('\(cl_int \*\) (?P<errcode_ret>%s)' % ptr_regex, source['text']).group('errcode_ret')
    # The following should match the assert statement.
    source['queue'] = re.search('\(cl_command_queue\) (?P<queue>%s)' % ptr_regex, source['text']).group('queue')

# Read from stdin (via pipe).
output = sys.stdin.read()
print 'OUTPUT'
print output

result = prof_parse(output)[0]
print 'RESULT'
print result
print

status = True
status &= (source['context'] == result['context'])
status &= (source['device'] == result['device'])
status &= (source['properties'] == result['properties'])
status &= (source['errcode_ret']  == result['errcode_ret'])

print '%s%s: %s' % (call, _id, 'PASSED' if status else 'FAILED')
print
