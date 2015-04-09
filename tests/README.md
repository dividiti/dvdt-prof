# Rationale.

The initial unit tests (for `clCreateKernel()`, `clEnqueueNDRangeKernel()`, etc.) are written using a C program and a Python program with the same base file name.

The C program gets compiled and run with `libprof_test.so` so that the output resembles that of `libprof.so`. The only difference is that `libprof_test.so` only intercepts the arguments but does not pass them further into a real `libOpenCL.so` library. Indeed, the C program uses some random values for pointer arguments (e.g. `0x12345678`) so calling `libOpenCL.so` would result in a segmentation fault.

The output of the C program is input into the Python program. The Python program parses the output using the parser in `python/prof.py` producing a dictionary called `result`. The Python program also parses the C program file to extract the original values producing a dictionary called `source`. Finally, the two dictionaries get compared.

Getting the original values by parsing the C program is arguably hard. (Perhaps harder than it should be.) Another approach would be to generate the C program from a template. On the other hand, writing the Python program first helps writing the parser (cf. test driven development.) Proper comparison of the approaches is left for future work.
