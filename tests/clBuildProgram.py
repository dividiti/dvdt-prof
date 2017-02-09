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
call = 'clBuildProgram'
_id  = ''
print '%s%s' % (call, _id)

# FIXME: taken from clBuildProgram.py - avoid duplication.
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
source = {}
with open(call + _id + '.cpp', 'r') as f:
    source['text'] = f.read()
    source['program'] = re.search('\(cl_program\) (?P<program>%s)' %
                                  ptr_regex, source['text']).group('program')

    num_events = int(re.search('num_devices(\s*)=(\s*)(?P<num_devices>\d+)', source['text']).group('num_devices'))
    cl_device_ptr_list = match_init_list(source['text'], 'device_list\[%d\]' % num_events, '\(cl_device_id\) %s' % ptr_regex)
    source['device_list'] = [re.match('\(cl_device_id\) (?P<ptr>%s)' % ptr_regex, cl_device_ptr).group('ptr') for cl_device_ptr in cl_device_ptr_list]

    source['options'] = re.search('options(\s*)=(\s*)\"(?P<options>%s)\"' % opts_regex, source['text']).group('options')

    source['pfn_notify'] = re.search('\(pfn_notify_t \*\) (?P<pfn_notify>%s)' % ptr_regex, source['text']).group('pfn_notify')
    source['user_data'] = re.search('\(void \*\) (?P<user_data>%s)' % ptr_regex, source['text']).group('user_data')

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
status &= (cmp(source['device_list'], result['device_list']) == 0)
status &= (source['options'] == result['options'])
status &= (source['pfn_notify'].lower() == result['pfn_notify'].lower())
status &= (source['user_data'].lower() == result['user_data'].lower())
status &= (0 == result['errcode'])

print '%s%s: %s' % (call, _id, 'PASSED' if status else 'FAILED')
print
