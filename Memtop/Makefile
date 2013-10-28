include ../config/vars.def
SRC=src/
LIB=lib/
BIN=bin/
INC=-Iinclude

LINK=$(CC) $(LFLAGS)  -o $(SRC)$@
AR=ar

all: clean libmemon.a memtop install

libmemon.a: memon.o memonf.o
	$(AR) cr libmemon.a  $(SRC)memon.o $(SRC)memonf.o

memon.o: $(SRC)memon.c
	$(CC) $(CFLAGS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC)

memonf.o: $(SRC)memonf.c
	$(CC) $(CFLAGS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC)

memtop: memtop.o
	$(LINK) $(SRC)memtop.o $(LIBNUMA) $(LIBNCURSES)

memtop.o: $(SRC)memtop.c
	$(CC) $(CFLAGS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC)

install:
	mv libmemon.a $(LIB)
	mv $(SRC)memtop $(BIN)

clean:
	rm -f  *.o $(SRC)*.o $(SRC)*~ $(LIB)*.a $(BIN)memtop
