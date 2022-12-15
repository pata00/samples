CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra -Wno-unused-parameter -O3 -pthread
SOURCE=$(wildcard *.cpp)
PROGRAMS=$(SOURCE:.cpp=)
DSYM=$(SOURCE:.cpp=.dSYM)

all: $(PROGRAMS)

clean:
	rm -rf $(PROGRAMS) $(DSYM)
