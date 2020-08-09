CXX = clang++
CFLAGS = -Wall -Wextra -O2 -std=c++17 -I../include -I/Library/gurobi902/mac64/include
CLIBS = -L../lib -loer -L/Library/gurobi902/mac64/lib -lgurobi_c++ -lgurobi90

demo: demo.cpp 
	$(CXX) demo.cpp -o demo $(CLIBS) $(CFLAGS)
gen: gen.cpp
	$(CXX) gen.cpp -o gen $(CLIBS) $(CFLAGS)	
clean:
	rm -rf demo gen
.PHONY: clean
