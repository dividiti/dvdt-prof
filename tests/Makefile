TEST_LIB = libprof_test.so


EXEC_EXT = exe

TEST_FILES = \
	clBuildProgram.cpp \
	clCreateCommandQueue.cpp \
	clCreateKernel.cpp \
	clEnqueueNDRangeKernel.cpp \
	clEnqueueReadBuffer.cpp \
	clEnqueueWriteBuffer.cpp

TEST_EXECS = $(TEST_FILES:.cpp=.${EXEC_EXT})


CXX := g++
CXXFLAGS := -O2 -g -W -Wall
LDFLAGS := -fPIC -shared -ldl

CXX_DIR   = ../cxx/
CXX_FILES = prof.cpp


all: ${TEST_LIB} ${TEST_EXECS}
run: all $(TEST_FILES:.cpp=.run)

${TEST_LIB}: $(addprefix ${CXX_DIR}, ${CXX_FILES})
	@echo "Building $@..."
	${CXX} $< -o $@ -DDVDT_PROF_TEST -DDVDT_PROF_WALLCLOCK_TIMEOFDAY ${CXXFLAGS} ${LDFLAGS}

%.${EXEC_EXT}: %.cpp
	@echo "Building $@..."
	${CXX} $< -o $@ ${CXX_FLAGS} -Wl,-rpath,. -L. -lprof_test -lOpenCL

%.run: %.${EXEC_EXT}
	@./$< | ./$*.py

clean:
	rm -f ${TEST_LIB}
	rm -f ${TEST_EXECS}
	rm -f *.pyc
