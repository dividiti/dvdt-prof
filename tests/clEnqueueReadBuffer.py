#!/usr/bin/env python
import sys, re

sys.path.append('.')
from clEnqueueReadOrWriteBuffer import run

# Test info.
call = 'clEnqueueReadBuffer'
_id  = ''

# Run test.
run(call, _id)
