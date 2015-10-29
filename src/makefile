headers := $(wildcard *.h)
sources := $(wildcard *.cpp)
CXX := g++-5

all: ../repl

../repl: $(headers) $(sources)
	$(CXX) -std=c++1y -g $(sources) -o $@ -ledit
	
clean:
	$(RM) ../repl
	
.PHONY: all clean
