IDIR =../include/
CXX=clang++
CFLAGS=-I$(IDIR) -Wall -Wextra -O2 -std=c++17 -I/Library/gurobi902/mac64/include
CLIBS=-L/Library/gurobi902/mac64/lib -lgurobi_c++ -lgurobi90
LIB = ../lib/liboer.a

ODIR=obj

_DEPS = gen.h mmcf.h ploter.h router.h graph.h layout.h io.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = gen.o mmcf.o ploter.o router.o layout.o graph.o io.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)

$(LIB): $(OBJ) $(DEPS)
	ar -rv $(LIB) $(OBJ)
	ranlib $(LIB)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(INCDIR)/*~

