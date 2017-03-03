#
# 2015-2017 (c) dividiti
#

import re
import json

#
# Common definitions.
#

prefix = '(\[dv\/dt\])'
call_regex = '(cl[a-zA-Z]*)'
opts_regex = '([ \-\w_=]*)'
iso_regex  = '(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d{6})'
ptr_regex  = '((0x[0-9a-fA-F]{1,8})|(0))'
int_regex  = '(\d+)'
hex_regex  = '([a-fA-F\d]+)'
bool_regex = '(\d)'

# Check that definitions from this file are available.
def test():
    print ("prof_common.py")

# Convert hexadecimal string into integer.
def hex_str_as_int(hex_str):
    hex_str_reversed = ''.join(reversed(
        [ hex_str[n:n+2] for n in range(0,len(hex_str),2) ]
    ))
    return int(hex_str_reversed, 16)

# Convert hexadecimal string into text string.
def hex_str_as_str(hex_str):
    return hex_str.decode('hex')
