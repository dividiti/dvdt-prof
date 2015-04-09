CXX := g++
CXXFLAGS := -O2 -g -W -Wall
LDFLAGS := -fPIC -shared -ldl

CXX_DIR   = ./cxx/
CXX_FILES = \
	prof.cpp

all: prof.so

prof.so: $(addprefix $(CXX_DIR), $(CXX_FILES))
	${CXX} $< -o $@ ${CXXFLAGS} ${LDFLAGS}

clean:
	rm -f *.so
