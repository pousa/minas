include ../../config/arch.def
INC=-I../../../MAi/include -I../../include
LIBMAI=-L../../../MAi/lib/ -lmai
LIBMEMON=-L../../lib -lmemon

LINKCC=$(CC) $(CCFLAGS) -o $(SRC)$@

all: clean mai_mem 

mai_mem.o: mai_mem.c
	$(CC) $(CCFLAGS) -g -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC)

mai_mem: mai_mem.o
	$(LINKCC) mai_mem.o $(LIBNUMA) $(LIBMAI) $(LIBMEMON) -lm

clean:
	rm -f  *.o mai_mem
