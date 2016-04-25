#!/usr/bin/env python
import sys, re

sys.path.append('../python')
from prof_parser import prof_parse
from prof_parser import ptr_regex

# Test info.
call = 'clCreateProgramWithSource'
_id  = ''
print '%s%s' % (call, _id)

# Parse test source.
source = {}
with open(call + _id + '.cpp', 'r') as f:
    source['text'] = f.read()
    source['context'] = re.search('\(cl_context\) (?P<context>%s)' % ptr_regex, source['text']).group('context')
    source['count'] = int(re.search('count(\s*)=(\s*)(?P<count>\d+)', source['text']).group('count'))
    source['string0'] = re.search('strings\[\d+\](\s*)=(\s*)\{(\s*)"(?P<string0>.*)"', source['text']).group('string0')
    source['lengths'] = re.search('\(const size_t \*\) (?P<lengths>%s)' % ptr_regex, source['text']).group('lengths')
    source['errcode_ret'] = re.search('\(cl_int \*\) (?P<errcode_ret>%s)' % ptr_regex, source['text']).group('errcode_ret')
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
status &= (source['context'] == result['context'])
status &= (source['count'] == result['count'])
status &= (source['string0'] == result['string'][0])
status &= (source['lengths'] == result['lengths'])
status &= (source['errcode_ret']  == result['errcode_ret'])
status &= (source['program']  == result['program'])

print '%s%s: %s' % (call, _id, 'PASSED' if status else 'FAILED')
print
