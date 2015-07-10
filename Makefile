LIB = prof.so
all: ${LIB}

CXX := g++
CXXFLAGS := -O2 -g -W -Wall -DDVDT_PROF_WALLCLOCK_TIMEOFDAY=1
LDFLAGS := -fPIC -shared -ldl

CXX_DIR   = ./cxx/
CXX_FILES = prof.cpp

${LIB}: $(addprefix ${CXX_DIR}, ${CXX_FILES})
	${CXX} $< -o $@ ${CXXFLAGS} ${LDFLAGS}

clean:
	rm -f ${LIB}
