CXX := g++
CXXFLAGS := -O2 -g -W -Wall
LDFLAGS := -fPIC -shared -ldl

CXX_DIR   = ../cxx/
CXX_FILES = prof.cpp

TEST_LIB   = libprof_test.so

TEST_FILES = \
	clBuildProgram.c \
	clCreateCommandQueue.c \
	clCreateKernel.c
EXEC_EXT = exe
TEST_EXECS = $(TEST_FILES:.c=.${EXEC_EXT})

all: ${TEST_LIB} ${TEST_EXECS}
run: all $(TEST_FILES:.c=.run)

${TEST_LIB}: $(addprefix ${CXX_DIR}, ${CXX_FILES})
	@echo "Building $@..."
	${CXX} $< -o $@ -DDVDT_PROF_TEST ${CXXFLAGS} ${LDFLAGS}

%.${EXEC_EXT}: %.c
	@echo "Building $@..."
	${CXX} $< -o $@ ${CXX_FLAGS} -Wl,-rpath,. -L. -lprof_test -lOpenCL

%.run: %.${EXEC_EXT}
	@./$< | ./$*.py

clean:
	rm -f ${TEST_LIB}
	rm -f ${TEST_EXECS}
