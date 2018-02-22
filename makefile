CXX = gcc -std=c11 
CXXFLAGS = -Wall -Wextra
TESTFLAGS = `pkg-config --cflags --libs glib-2.0`
INC=./include
LIBS=./libs
SRC=./src
OBJ=./obj
BIN=./bin
TEST=./tests
TESTBIN=$(BIN)/tests
DOXIFILE=./doc/doxys/Doxyfile.in


# Build release
release: CXXFLAGS += -O3
release: $(BIN)/libunbrkfio.a

# Build debug
debug: CXXFLAGS += -g
debug: $(BIN)/libunbrkfio.a

# Build library 
$(BIN)/libunbrkfio.a: $(OBJ)/ub_fio.o
	mkdir -p $(BIN)
	ar rc $@ $^ 

$(OBJ)/ub_fio.o: $(SRC)/ub_fio.c $(LIBS) $(INC)/ub_fio.h $(INC)/ub_io_err.h
	mkdir -p $(OBJ)
	$(CXX) -I$(LIBS)/libunbrkcore/include -I$(INC) $(CXXFLAGS) -c -o $@ $<


# Clean
clean:
	rm $(BIN)/* $(INC)/~* $(OBJ)/*.o


# Build documentation
doc: $(INC)/* $(DOXIFILE)
	doxygen $(DOXIFILE)


# Build and run tests
tests: $(TESTBIN)/ub_fio_tests

$(TESTBIN)/ub_fio_tests: $(TEST)/ub_fio_tests.c $(INC)/ub_io_err.h debug libs 
	mkdir -p $(TESTBIN)
	$(CXX) -g -I$(INC) -I$(LIBS)/libunbrkcore/include $(CXXFLAGS) -o $@ $< -L$(BIN) -lunbrkfio -L$(LIBS)/libunbrkcore/bin -lunbrkcore $(TESTFLAGS)
	$@ 

libs: $(LIBS)/libunbrkcore/makefile
	make -C $(LIBS)/libunbrkcore
