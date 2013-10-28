CC = gcc
CFLAGS = -O2 -fopenmp 
INC=-I../../Minas/MAi-array/include
LIBNUMA=-lnuma -lm
LIBMAI=-L../../Minas/MAi-array/lib -lmai

all: jacobi jacobi_irregular jacobi_mai jacobi_irregular_mai

jacobi: jacobi.c
	$(CC) $(CFLAGS) jacobi.c -o jacobi 

jacobi_mai: jacobi.c
	$(CC) $(CFLAGS) -DMAI jacobi.c -o jacobi_mai $(INC) $(LIBMAI) $(LIBNUMA)

jacobi_irregular: jacobi.c
	$(CC) $(CFLAGS) -DIRREGULAR jacobi.c -o jacobi_irregular 

jacobi_irregular_mai: jacobi.c
	$(CC) $(CFLAGS) -DIRREGULAR -DMAI jacobi.c -o jacobi_irregular_mai $(INC) $(LIBMAI) $(LIBNUMA)

clean:
	rm -f jacobi jacobi_irregular jacobi_mai jacobi_irregular_mai *.o
