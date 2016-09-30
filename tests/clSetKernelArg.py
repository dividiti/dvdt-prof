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
    source['arg_size'] = int(re.search('arg_size(\s*)=(\s*)(?P<arg_size>\d+)', source['text']).group('arg_size'))
    source['arg_value'] = re.search('\(const void \*\) (?P<arg_value>%s)' % ptr_regex, source['text']).group('arg_value')

# Read from stdin (via pipe).
output = sys.stdin.read()
print 'OUTPUT'
print output

# Parse JSON output, only if PARSE_JSON is defined to 1 (not to default 0).
if os.environ.get('PARSE_JSON', '0') == '1':
    print('Parsing JSON profiler output...')
    result = json.loads(output)[0]
else:
    print('Parsing standard profiler output...')
    result = prof_parse(output)[0]
print 'RESULT'
print result
print

status = True
status &= (source['kernel'] == result['kernel'])
status &= (source['arg_index'] == result['arg_index'])
status &= (source['arg_size'] == result['arg_size'])
status &= (source['arg_value'] == result['arg_value'])
status &= (0 == result['errcode'])

print '%s%s: %s' % (call, _id, 'PASSED' if status else 'FAILED')
print
