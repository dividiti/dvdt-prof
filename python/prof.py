import re

#
# Common definitions.
#

prefix = '\[dv\/dt\]'
call_regex = 'cl[a-zA-Z]*?' # non-greedy
opts_regex = '[\w=\- ]*'
iso_regex  = '\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d{6}' 
ptr_regex  = '0x[0-9a-fA-F]{1,8}'
int_regex  = '\d*'

#
# Parsers for API calls.
#

def match_clBuildProgram(output, result):
    call = 'clBuildProgram'
    # Arguments.
    result['program']    = re.search('%s %s %s (?P<program>%s)' % \
        (prefix, call, 'program', ptr_regex), output).group('program')
    result['options']    = re.search('%s %s %s (?P<options>%s)' % \
        (prefix, call, 'options', opts_regex), output).group('options')
    # Return value.
    return (output, result)


def match_clCreateCommandQueue(output, result):
    call = 'clCreateCommandQueue'
    # Arguments.
    result['context']    = re.search('%s %s %s (?P<context>%s)' % \
        (prefix, call, 'context', ptr_regex), output).group('context')
    result['device']     = re.search('%s %s %s (?P<device>%s)'  % \
        (prefix, call, 'device', ptr_regex), output).group('device')
    result['properties'] = re.search('%s %s %s (?P<properties>%s)'   % \
        (prefix, call, 'properties', int_regex), output).group('properties')
    result['errcode']    = re.search('%s %s %s (?P<errcode>%s)' % \
        (prefix, call, 'errcode', ptr_regex), output).group('errcode')
    # Return value.
    retval_match = re.search('%s %s %s (?P<queue>%s)' % \
        (prefix, call, 'queue', ptr_regex), output)
    result['queue'] = retval_match.group('queue')
    return (output[retval_match.end():], result)


# Map from API calls to parsers.
map_call_to_parser = {
    'clBuildProgram' : match_clBuildProgram,
    'clCreateCommandQueue' : match_clCreateCommandQueue
}


def next_match(output):
    match = re.search('%s (?P<call>%s) (?P<now>%s)' % (prefix, call_regex, iso_regex), output) 
    if not match:
        return ('', {})
    result = {}
    result['call'] = match.group('call')
    result['now'] = match.group('now')
    return (output[match.end():], result)


def match(output):
    results = []

    (output, result) = next_match(output)
    while result:
        call = result['call']
        parser = map_call_to_parser[call]
        if not parser:
            raise Exception('OpenCL API call %s not supported!' % call)
        (output, result) = parser(output, result)
        results.append(result)

        (output, result) = next_match(output)

    return results
