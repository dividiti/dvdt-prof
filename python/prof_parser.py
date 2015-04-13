import re

#
# Common definitions.
#

prefix = '(\[dv\/dt\])'
call_regex = '(cl[a-zA-Z]*)'
opts_regex = '([ \-\w_=]*)'
iso_regex  = '(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d{6})'
ptr_regex  = '((0x[0-9a-fA-F]{1,8})|(0))'
int_regex  = '(\d+)'
bool_regex = '(\d)'

#
# Parsers for API calls.
#

def match_clBuildProgram(output, result):
    call = 'clBuildProgram'

    # Arguments.
    result['program'] = re.search('%s %s %s (?P<program>%s)' % \
        (prefix, call, 'program', ptr_regex), output).group('program')
    result['options'] = re.search('%s %s %s (?P<options>%s)' % \
        (prefix, call, 'options', opts_regex), output).group('options')
    # TODO: device_list, pfn_notify, user_data

    # Return value.
    return_match = re.search('%s %s %s (?P<errcode>%s)' % \
        (prefix, call, 'errcode', int_regex), output)
    result['errcode'] = int(return_match.group('errcode'))

    return (output[return_match.end():], result)


def match_clCreateCommandQueue(output, result):
    call = 'clCreateCommandQueue'

    # Arguments.
    result['context'] = re.search('%s %s %s (?P<context>%s)' % \
        (prefix, call, 'context', ptr_regex), output).group('context')
    result['device'] = re.search('%s %s %s (?P<device>%s)'  % \
        (prefix, call, 'device', ptr_regex), output).group('device')
    result['properties'] = re.search('%s %s %s (?P<properties>%s)'   % \
        (prefix, call, 'properties', int_regex), output).group('properties')
    result['errcode_ret'] = re.search('%s %s %s (?P<errcode_ret>%s)' % \
        (prefix, call, 'errcode_ret', ptr_regex), output).group('errcode_ret')

    # Return value.
    return_match = re.search('%s %s %s (?P<queue>%s)' % \
        (prefix, call, 'queue', ptr_regex), output)
    result['queue'] = return_match.group('queue')

    return (output[return_match.end():], result)


def match_clCreateKernel(output, result):
    call = 'clCreateKernel'

    # Arguments.
    result['program'] = re.search('%s %s %s (?P<program>%s)' % \
        (prefix, call, 'program', ptr_regex), output).group('program')
    result['name']    = re.search('%s %s %s (?P<name>%s)' % \
        (prefix, call, 'name', opts_regex), output).group('name')
    result['errcode_ret'] = re.search('%s %s %s (?P<errcode_ret>%s)' % \
        (prefix, call, 'errcode_ret', ptr_regex), output).group('errcode_ret')

    # Return value.
    return_match = re.search('%s %s %s (?P<kernel>%s)' % \
        (prefix, call, 'kernel', ptr_regex), output)
    result['kernel'] = return_match.group('kernel')

    return (output[return_match.end():], result)


def match_clEnqueueNDRangeKernel(output, result):
    call = 'clEnqueueNDRangeKernel'

    # Arguments.
    result['queue']  = re.search('%s %s %s (?P<queue>%s)' % \
        (prefix, call, 'queue', ptr_regex), output).group('queue')
    result['kernel'] = re.search('%s %s %s (?P<kernel>%s)' % \
        (prefix, call, 'kernel', ptr_regex), output).group('kernel')
    result['offset'] = [int(i) for i in re.search('%s %s %s (?P<offset>%s)' % \
        (prefix, call, 'offset', '.*'), output).group('offset').split()]
    result['gws']    = [int(i) for i in re.search('%s %s %s (?P<gws>%s)' % \
        (prefix, call, 'gws', '.*'), output).group('gws').split()]
    result['lws']    = [int(i) for i in re.search('%s %s %s (?P<lws>%s)' % \
        (prefix, call, 'lws', '.*'), output).group('lws').split()]
    result['event_wait_list'] = re.search('%s %s %s(?P<event_wait_list>( %s)*)' % \
        (prefix, call, 'event_wait_list', ptr_regex), output).group('event_wait_list').split()
    result['event']  = re.search('%s %s %s (?P<event>%s)' % \
        (prefix, call, 'event', ptr_regex), output).group('event')

    # Return value.
    return_match = re.search('%s %s %s (?P<errcode>%s)' % \
        (prefix, call, 'errcode', int_regex), output)
    result['errcode'] = int(return_match.group('errcode'))

    # Profiling info.
    profiling_match = re.search('%s %s %s (?P<queued>%s) (?P<submit>%s) (?P<start>%s) (?P<end>%s)' % \
        (prefix, call, 'profiling', int_regex, int_regex, int_regex, int_regex), output)
    if profiling_match:
        result['profiling'] = {}
        result['profiling']['queued'] = int(profiling_match.group('queued'))
        result['profiling']['submit'] = int(profiling_match.group('submit'))
        result['profiling']['start']  = int(profiling_match.group('start'))
        result['profiling']['end']    = int(profiling_match.group('end'))

    last_match = return_match if not profiling_match else profiling_match

    return (output[last_match.end():], result)


# Auxiliary function for clEnqueueReadBuffer and clEnqueueWriteBuffer.
def _match_clEnqueueReadOrWriteBuffer(call, output, result):
    # Arguments.
    result['queue']  = re.search('%s %s %s (?P<queue>%s)' % \
        (prefix, call, 'queue', ptr_regex), output).group('queue')
    result['buffer'] = re.search('%s %s %s (?P<buffer>%s)' % \
        (prefix, call, 'buffer', ptr_regex), output).group('buffer')
    result['blocking'] = int(re.search('%s %s %s (?P<blocking>%s)' % \
        (prefix, call, 'blocking', bool_regex), output).group('blocking'))
    result['offset']  = int(re.search('%s %s %s (?P<offset>%s)' % \
        (prefix, call, 'offset', int_regex), output).group('offset'))
    result['size']  = int(re.search('%s %s %s (?P<size>%s)' % \
        (prefix, call, 'size', int_regex), output).group('size'))
    result['ptr']  = re.search('%s %s %s (?P<ptr>%s)' % \
        (prefix, call, 'ptr', ptr_regex), output).group('ptr')
    result['event_wait_list'] = re.search('%s %s %s(?P<event_wait_list>( %s)*)' % \
        (prefix, call, 'event_wait_list', ptr_regex), output).group('event_wait_list').split()
    result['event']  = re.search('%s %s %s (?P<event>%s)' % \
        (prefix, call, 'event', ptr_regex), output).group('event')

    # Return value.
    return_match = re.search('%s %s %s (?P<errcode>%s)' % \
        (prefix, call, 'errcode', int_regex), output)
    result['errcode'] = int(return_match.group('errcode'))

    # Profiling info.
    profiling_match = re.search('%s %s %s (?P<queued>%s) (?P<submit>%s) (?P<start>%s) (?P<end>%s)' % \
        (prefix, call, 'profiling', int_regex, int_regex, int_regex, int_regex), output)
    if profiling_match:
        result['profiling'] = {}
        result['profiling']['queued'] = int(profiling_match.group('queued'))
        result['profiling']['submit'] = int(profiling_match.group('submit'))
        result['profiling']['start']  = int(profiling_match.group('start'))
        result['profiling']['end']    = int(profiling_match.group('end'))

    last_match = return_match if not profiling_match else profiling_match

    return (output[last_match.end():], result)

def match_clEnqueueReadBuffer(output, result):
    call = 'clEnqueueReadBuffer'
    return _match_clEnqueueReadOrWriteBuffer(call, output, result)

def match_clEnqueueWriteBuffer(output, result):
    call = 'clEnqueueWriteBuffer'
    return _match_clEnqueueReadOrWriteBuffer(call, output, result)

# Map from API calls to parsers.
map_call_to_parser = {
    'clBuildProgram'         : match_clBuildProgram,
    'clCreateCommandQueue'   : match_clCreateCommandQueue,
    'clCreateKernel'         : match_clCreateKernel,
    'clEnqueueNDRangeKernel' : match_clEnqueueNDRangeKernel,
    'clEnqueueReadBuffer'    : match_clEnqueueReadBuffer,
    'clEnqueueWriteBuffer'   : match_clEnqueueWriteBuffer
}


def next_match(output):
    result = {}

    # For robustness, a new block starts with just an API call name.
    match = re.search('%s (?P<call>%s)\n' % (prefix, call_regex), output)
    if not match:
        return ('', {})

    result['call'] = match.group('call')
    parser = map_call_to_parser[result['call']]
    if not parser:
        raise Exception('OpenCL API call %s not supported!' % result['call'])

    # Start and end timestamps are optional (especially in tests) but common to all calls.
    start_match = re.search('%s %s start (?P<start>%s)' % (prefix, result['call'], iso_regex), output)
    if start_match:
        result['start'] = start_match.group('start')
    end_match = re.search('%s %s end (?P<end>%s)' % (prefix, result['call'], iso_regex), output)
    if end_match:
        result['end'] = end_match.group('end')

    return parser(output, result)


def prof_parse(output):
    results = []
    (output, result) = next_match(output)
    while result:
        results.append(result)
        (output, result) = next_match(output)
    return results
