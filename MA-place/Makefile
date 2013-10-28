include ../config/vars.def
SRC=src/
LIB=lib/
INC=-Iinclude -I/data03/pousa/hwloc/include

AR=ar

all: clean libmaplace.a install

libmaplace.a: ma_place.o
	$(AR) cr libmaplace.a  $(SRC)ma_place.o 

ma_place.o: $(SRC)ma_place.c
	$(CC) $(CFLAGS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC)

install:
	mv libmaplace.a $(LIB)

clean:
	rm -f  *.o $(SRC)*.o $(SRC)*~ $(LIB)*.a
