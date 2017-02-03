#!/usr/bin/env python
import sys
import re
import os
import json

sys.path.append('../python')
from prof_parser import prof_parse
from prof_parser import ptr_regex

# Test info.
call = 'clCreateProgramWithBinary'
_id  = ''
print '%s%s' % (call, _id)

# Parse test source.
source = {}
with open(call + _id + '.cpp', 'r') as f:
    source['text'] = f.read()
    source['context'] = re.search('\(cl_context\) (?P<context>%s)' % ptr_regex, source['text']).group('context')
    source['num_devices'] = int(re.search('num_devices(\s*)=(\s*)(?P<num_devices>\d+)', source['text']).group('num_devices'))
    source['device_list'] = re.search('\(const cl_device_id \*\) (?P<device_list>%s)' % ptr_regex, source['text']).group('device_list')
    source['lengths'] = re.search('\(const size_t \*\) (?P<lengths>%s)' % ptr_regex, source['text']).group('lengths')
    source['binaries'] = re.search('\(const unsigned char \*\*\) (?P<binaries>%s)' % ptr_regex, source['text']).group('binaries')
    source['binary_status'] = re.search('binary_status(\s*)=(\s*)\(cl_int \*\) (?P<binary_status>%s)' % ptr_regex, source['text']).group('binary_status')
    source['errcode_ret'] = re.search('errcode_ret(\s*)=(\s*)\(cl_int \*\) (?P<errcode_ret>%s)' % ptr_regex, source['text']).group('errcode_ret')
    # The following should match the assert statement.
    source['program'] = re.search('\(cl_program\) (?P<program>%s)' % ptr_regex, source['text']).group('program')

# Read from stdin (via pipe).
output = sys.stdin.read()
print 'OUTPUT'
print output

result = prof_parse(output)[0]
print 'RESULT'
print result
print

status = True
status &= (source['context'].lower() == result['context'].lower())
status &= (source['num_devices'] == result['num_devices'])
status &= (source['device_list'].lower() == result['device_list'].lower())
status &= (source['lengths'].lower() == result['lengths'].lower())
status &= (source['binaries'].lower() == result['binaries'].lower())
status &= (source['binary_status'].lower() == result['binary_status'].lower())
status &= (source['errcode_ret'].lower() == result['errcode_ret'].lower())
status &= (source['program'].lower()  == result['program'].lower())

print '%s%s: %s' % (call, _id, 'PASSED' if status else 'FAILED')
print
