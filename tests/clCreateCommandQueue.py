#!/usr/bin/env python
import sys, re

sys.path.append('../python')
from prof import match
from prof import ptr_regex

# Test info.
call = 'clCreateCommandQueue'
id_  = ''
print '%s%s' % (call, id_)

# Parse test source.
source = {}
with open(call + id_ + '.c', 'r') as f:
    source['text'] = f.read()
    source['context']    = re.search('\(cl_context\) (?P<context>%s)' % ptr_regex, source['text']).group('context')
    source['device']     = re.search('\(cl_device_id\) (?P<device>%s)' % ptr_regex, source['text']).group('device')
    source['properties'] = re.search('\(cl_command_queue_properties\) (?P<props>\d*)', source['text']).group('props')
    source['errcode']    = re.search('\(cl_int \*\) (?P<errcode>%s)' % ptr_regex, source['text']).group('errcode')

# Read from stdin (via pipe).
output = sys.stdin.read()
print 'OUTPUT'
print output

result = match(output)[0]
print 'RESULT'
print result
print

status = True
status &= (source['context']    == result['context'])
status &= (source['device']     == result['device'])
status &= (source['properties'] == result['properties'])
status &= (source['errcode']    == result['errcode'])

print '%s%s: %s' % (call, id_, 'PASSED' if status else 'FAILED')
print
