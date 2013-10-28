SRC=src/
SRCARCH=../numArch/src/
LIB=lib/
INC=-Iinclude -I../numArch/include
LIBS=-L../numArch/lib -lnumarch
LINK=$(CC) $(LFLAGS) $(ARCH) $(THREADS) -o $(SRC)$@ $@ $(LIBS) 

all: clean numarch libnumaheap.a install

libnumaheap.a: heap_alloc_numa.o heap_alloc.o heap_numa.o
	$(AR) cr libnumaheap.a $(SRCARCH)numArch.o $(SRC)heap_alloc_numa.o $(SRC)heap_alloc.o $(SRC)heap_numa.o

heap_alloc_numa.o: $(SRC)heap_alloc_numa.c
	$(CC) -w $(CFLAGS) $(ARCH) $(THREADS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC) $(LIBTHREAD)  $(LIBS)

heap_alloc.o: $(SRC)heap_alloc.c 
	$(CC) -w $(CFLAGS) $(ARCH) $(THREADS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC) $(LIBTHREAD)  $(LIBS)

heap_numa.o: $(SRC)heap_numa.c 
	$(CC) -w $(CFLAGS) $(ARCH) $(THREADS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC) $(LIBTHREAD)  $(LIBS)

install:
	mv libnumaheap.a $(LIB)

numarch:
	cd ../numArch; make; cd ../MAi-heap; 

clean:
	rm -f  *.o $(SRC)*.o $(LIB)*.a
