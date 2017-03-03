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
call = 'clSetKernelArg'
_id  = ''
print '%s%s' % (call, _id)

# Parse test source.
source = {}
with open(call + _id + '.cpp', 'r') as f:
    source['text'] = f.read()
    source['kernel'] = re.search('\(cl_kernel\) (?P<kernel>%s)' % ptr_regex, source['text']).group('kernel')
    source['arg_index'] = int(re.search('arg_index(\s*)=(\s*)(?P<arg_index>\d+)', source['text']).group('arg_index'))
    source['arg_value_as_int'] = int(re.search('arg_value(\s*)=(\s*)(?P<arg_value>\d+)', source['text']).group('arg_value'))
    source['arg_size'] = int(re.search('arg_size(\s*)=(\s*)(?P<arg_size>\d+)', source['text']).group('arg_size'))

# Read from stdin (via pipe).
output = sys.stdin.read()
print 'OUTPUT'
print output

result = prof_parse(output)[0]
print 'RESULT'
print result
print

if result.get('arg_value_as_int', -1)==-1:
    arg_value = result['arg_value']
    arg_value_reversed = ''.join(reversed(
        [ arg_value[n:n+2] for n in range(0,len(arg_value),2) ]
    ))
    result['arg_value_as_int'] = int(arg_value_reversed, 16)

status = True
status &= (source['kernel'] == result['kernel'])
status &= (source['arg_index'] == result['arg_index'])
status &= (source['arg_size'] == result['arg_size'])
status &= (source['arg_value_as_int'] == result['arg_value_as_int'])
status &= (0 == result['errcode'])

print '%s%s: %s' % (call, _id, 'PASSED' if status else 'FAILED')
print
