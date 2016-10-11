#!/usr/bin/env python
import sys
import re
import os
import json

sys.path.append('../python')
from prof_parser import prof_parse
from prof_parser import ptr_regex

# Test info.
call = 'clCreateBuffer'
_id  = ''
print '%s%s' % (call, _id)

# Parse test source.
source = {}
with open(call + _id + '.cpp', 'r') as f:
    source['text'] = f.read()
    source['context'] = re.search('\(cl_context\) (?P<context>%s)' % ptr_regex, source['text']).group('context')
    source['flags'] = int(re.search('\(cl_mem_flags\) (?P<flags>\d*)', source['text']).group('flags'))
    source['size'] = int(re.search('size(\s*)=(\s*)(?P<size>\d+)', source['text']).group('size'))
    source['host_ptr'] = re.search('\(void \*\) (?P<host_ptr>%s)' % ptr_regex, source['text']).group('host_ptr')
    source['errcode_ret'] = re.search('\(cl_int \*\) (?P<errcode_ret>%s)' % ptr_regex, source['text']).group('errcode_ret')
    # The following should match the assert statement.
    source['buffer'] = re.search('\(cl_mem\) (?P<buffer>%s)' % ptr_regex, source['text']).group('buffer')

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
status &= (source['flags'] == result['flags'])
status &= (source['size'] == result['size'])
status &= (source['host_ptr']  == result['host_ptr'])
status &= (source['errcode_ret']  == result['errcode_ret'])

print '%s%s: %s' % (call, _id, 'PASSED' if status else 'FAILED')
print
