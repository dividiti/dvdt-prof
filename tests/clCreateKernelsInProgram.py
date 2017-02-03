#!/usr/bin/env python
import sys
import re
import os
import json

sys.path.append('../python')
from prof_parser import prof_parse
from prof_parser import ptr_regex
from prof_parser import opts_regex

# Test info.
call = 'clCreateKernelsInProgram'
_id  = ''
print '%s%s' % (call, _id)

# Parse test source.
source = {}
with open(call + _id + '.cpp', 'r') as f:
    source['text'] = f.read()
    # cl_program program = (cl_program) 0x01234567;
    source['program'] = re.search('\(cl_program\) (?P<program>%s)' % ptr_regex, source['text']).group('program')
    # cl_uint num_kernels = 1;
    source['num_kernels'] = int(re.search('num_kernels(\s*)=(\s*)(?P<num_kernels>\d+)', source['text']).group('num_kernels'))
    # cl_kernel * kernels = (cl_kernel *) 0x12345678;
    source['kernels'] = re.search('\(cl_kernel(\s*)\*\)(\s*)(?P<kernels>%s)' % ptr_regex, source['text']).group('kernels')
    # cl_uint * num_kernels_ret = (cl_uint *) 0x23456789;
    source['num_kernels_ret'] = re.search('\(cl_uint(\s*)\*\)(\s*)(?P<num_kernels_ret>%s)' % ptr_regex, source['text']).group('num_kernels_ret')

# Read from stdin (via pipe).
output = sys.stdin.read()
print 'OUTPUT'
print output

result = prof_parse(output)[0]
print 'RESULT'
print result
print

status = True
status &= (source['program'] == result['program'])
status &= (source['num_kernels'] == result['num_kernels'])
status &= (source['kernels'] == result['kernels'])
status &= (source['num_kernels_ret'] == result['num_kernels_ret'])
status &= (0 == result['errcode'])

print '%s%s: %s' % (call, _id, 'PASSED' if status else 'FAILED')
print
