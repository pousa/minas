CCFLAGS= -fopenmp -g
INC=-I../include
LIBNUMA=-lnuma -lm
LIBMAI=-L../lib -lmai

FF=gfortran $(CCFLAGS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.f90) $(INC) 

LINKFF=gfortran $(CCFLAGS) -o $(SRC)$@

all: clean test_fortran

test_fortran.o: test_fortran.f90
	$(FF)

test_fortran: test_fortran.o
	$(LINKFF) test_fortran.o $(LIBNUMA) $(LIBMAI) $(INC)

clean:
	rm -f  *.o test_fortran

