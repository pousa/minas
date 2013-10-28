include ../config/vars.def
SRC=src/
SRCARCH=../numArch/src/
LIB=lib/
INC=-Iinclude
INCARCH=-I../numArch/include
LIBS=-L../numArch/lib -lnumarch
THREADS=-DOMP -DOMP_FIX

LINK=$(CC) $(LFLAGS) $(ARCH) $(THREADS) -o $(SRC)$@ $@ $(LIBS) $(LIBTHREAD)

all: clean numarch intfortran libmai.a install
		
libmai.a: change.o mai_fortran.o own_mempolicies.o common.o memory_policies.o thread_policies.o alloc_memory.o mai_statistics.o mai.o 
	$(AR) cr libmai.a $(SRCARCH)numArch.o $(SRC)change.o $(SRC)mai_fortran.o $(SRC)own_mempolicies.o $(SRC)common.o $(SRC)memory_policies.o $(SRC)thread_policies.o $(SRC)alloc_memory.o  $(SRC)mai_statistics.o $(SRC)mai.o

own_mempolicies.o: $(SRC)own_mempolicies.c
	$(CC) -w $(CFLAGS) $(ARCH) $(THREADS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC) $(INCARCH) $(LIBTHREAD) $(LIBS)

common.o: $(SRC)common.c
	$(CC) -w $(CFLAGS) $(ARCH) $(THREADS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC) $(INCARCH) $(LIBTHREAD) $(LIBS)

alloc_memory.o: $(SRC)alloc_memory.c
		$(CC) -w $(CFLAGS) $(ARCH) $(THREADS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC) $(INCARCH) $(LIBTHREAD) $(LIBS)

memory_policies.o: $(SRC)memory_policies.c 
	$(CC) -w $(CFLAGS) $(ARCH) $(THREADS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC) $(INCARCH) $(LIBTHREAD) $(LIBS)

thread_policies.o: $(SRC)thread_policies.c
	$(CC) -w $(CFLAGS) $(ARCH) $(THREADS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC) $(INCARCH) $(LIBTHREAD) $(LIBS)

mai_statistics.o: $(SRC)mai_statistics.c
	$(CC) -w $(CFLAGS) $(ARCH) $(THREADS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC) $(INCARCH) $(LIBTHREAD) $(LIBS)

mai.o: $(SRC)mai.c
	$(CC) -w $(CFLAGS) $(ARCH) $(THREADS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC) $(INCARCH) $(LIBTHREAD) $(LIBS)

mai_fortran.o: $(SRC)mai_fortran.c
	$(CC) -w $(CFLAGS) $(ARCH) $(THREADS) -c -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC) $(INCARCH) $(LIBTHREAD) $(LIBS)

change.o: $(SRC)change.f90 
	$(FF) -w -c -o $(SRC)$@ $(SRC)$(@F:.o=.f90) $(INC)

install:
	mv libmai.a $(LIB)

numarch:
	cd ../numArch; make; cd ../MAi-array; 

intfortran: 
	$(FF) -w -c $(SRC)ctof.f90 
	mv ctof.mod include/
	rm ctof.o

clean:
	rm -f  *.o $(SRC)*.o $(SRC)*~ $(SRC)*.mod $(SRCARCH)*.o $(LIB)*.a
