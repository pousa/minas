CCFLAGS= -fopenmp -g
INC=-I../include
LIBNUMA=-lnuma -lm
LIBMAL=-L../lib -lmai

CC=gcc $(CCFLAGS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC)

LINKCC=gcc $(CCFLAGS) -o $(SRC)$@

all: clean test_struct

test_struct.o: test_struct.c
	$(CC)

test_struct: test_struct.o
	$(LINKCC) test_struct.o $(LIBNUMA) $(LIBMAL)

clean:
	rm -f  *.o test_struct

