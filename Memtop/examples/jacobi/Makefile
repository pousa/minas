include ../../config/arch.def
INC=-I../include
LIBMEMON=-L../lib -lmemon

LINKCC=$(CC) $(LFLAGS) -o $(SRC)$@

all: clean jacobi

jacobi.o: jacobi.c
	$(CC) $(CFLAGS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC)

jacobi: jacobi.o
	$(LINKCC) jacobi.o $(LIBMEMON) $(LIBTHREAD)

clean:
	rm -f  *.o jacobi
