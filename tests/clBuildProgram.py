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

# Parse test source.
source = {}
with open(call + _id + '.cpp', 'r') as f:
  source['text'] = f.read()
  source['program'] = re.search('\(cl_program\) (?P<program>%s)' %
				ptr_regex, source['text']).group('program')
  source['options'] = re.search('options = \"(?P<options>%s)\"' %
				opts_regex, source['text']).group('options')

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
status &= (source['program'] == result['program'])
status &= (source['options'] == result['options'])

print '%s%s: %s' % (call, _id, 'PASSED' if status else 'FAILED')
print
