#!/usr/bin/env python
import sys, re

sys.path.append('.')
from clEnqueueReadOrWriteBuffer import run

# Test info.
call = 'clEnqueueWriteBuffer'
id_  = ''

# Run test.
run(call, id_)
