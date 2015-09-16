headers := $(wildcard *.h)
sources := $(wildcard *.cpp)

all: ../repl

../repl: $(headers) $(sources)
	$(CXX) -std=c++11 -g $(sources) -o $@ -ledit
	
clean:
	$(RM) ../repl
	
.PHONY: all clean
