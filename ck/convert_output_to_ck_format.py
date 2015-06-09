#
# Converting raw slambench timing to CK universal 
# autotuning and machine learning format
#
# Collective Knowledge (CK)
#
# See CK LICENSE.txt for licensing details.
# See CK Copyright.txt for copyright details.
#
# Developer: Grigori Fursin
#

import json
import sys

d={}

print ('Parsing dvdt opencl profiler output ...')

i=sys.argv

if len(i)<3:
   print ('')
   print ('Usage:')
   print ('  python convert_output_to_ck_format.py <file_with_program_output_to_parse> <output_json_file>')
   exit(1)

fi=i[1]
fo=i[2]

f=open(fi, 'r')
x=f.read()
f.close()

xx=x.split('\n')

for q in xx:
  if q.startswith('[dv/dt] '):
     k=''
     q1=q[8:]
     q2=q1.find(' ')
     if q2<0:
        k=q1
        if k not in d: d[k]={}
     else:
        k=q1[:q2]
        q3=q1.find(' ',q2+1)
        if q3>0:
           kk=q1[q2+1:q3]
           v=q1[q3+1:]

           if kk not in d[k]: d[k][kk]=[]
           d[k][kk].append(v)

# Write CK json
f=open(fo,'wt')
f.write(json.dumps(d, indent=2, sort_keys=True)+'\n')
f.close()
