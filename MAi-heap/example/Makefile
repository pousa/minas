CC=gcc
CCFLAGS= -g 
INC=-I../include
LIBTHREAD=-lpthread
LIBNUMA=-lnuma
LIB=-L../lib -lnumaheap -L../../numArch/lib -lnumarch

CC=gcc $(CCFLAGS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC) $(LIBTHREAD) $(LIBNUMA) $(LIB)

LINKCC=gcc $(CCFLAGS) -o $(SRC)$@ 

all: clean jacobi jacobi_original mem_bench mem_bench_original

jacobi_original.o: jacobi_original.c
	$(CC) 

jacobi_original: jacobi_original.o
	$(LINKCC) jacobi_original.o  $(LIBTHREAD) $(LIBNUMA) $(LIB)


jacobi.o: jacobi.c
	$(CC) 

jacobi: jacobi.o
	$(LINKCC) jacobi.o  $(LIBTHREAD) $(LIBNUMA) $(LIB)

mem_bench.o: mem_bench.c
	 $(CC) 

mem_bench: mem_bench.o
	 $(LINKCC) mem_bench.o $(LIBTHREAD) $(LIBNUMA) $(LIB)


mem_bench_original.o: mem_bench_original.c
	 $(CC) 

mem_bench_original: mem_bench_original.o
	 $(LINKCC) mem_bench_original.o $(LIBTHREAD) $(LIBNUMA) $(LIB)


clean:
	rm -f  *.o jacobi jacobi_original mem_bench mem_bench_original
