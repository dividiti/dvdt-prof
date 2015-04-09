#!/usr/bin/env python
import sys, re

sys.path.append('../python')
from prof import match
from prof import ptr_regex
from prof import opts_regex

# Test info.
call = 'clCreateKernel'
id_  = ''
print '%s%s' % (call, id_)

# Parse test source.
source = {}
with open(call + id_ + '.c', 'r') as f:
    source['text'] = f.read()
    source['program'] = re.search('\(cl_program\) (?P<program>%s)' % ptr_regex, source['text']).group('program')
    source['kernel_name'] = re.search('kernel_name = \"(?P<kernel_name>%s)\"' % opts_regex, source['text']).group('kernel_name')

# Read from stdin (via pipe).
output = sys.stdin.read()
print 'OUTPUT'
print output

result = match(output)[0]
print 'RESULT'
print result

status = True
status |= (source['program'] == result['program'])
status |= (source['kernel_name'] == result['kernel_name'])

print '%s%s: %s' % (call, id_, 'PASSED' if status else 'FAILED')
print
