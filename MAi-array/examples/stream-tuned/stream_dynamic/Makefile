CC = gcc
CFLAGS = -O2 -fopenmp 
INC=-I../../Minas/MAi-array/include
LIBNUMA=-lnuma -lm
LIBMAI=-L../../Minas/MAi-array/lib -lmai


all: stream_dynamic stream_dynamic_mai

stream_dynamic: stream.c
	$(CC) $(CFLAGS) stream.c -o stream_dynamic

stream_dynamic_mai: stream.c
	$(CC) $(CFLAGS) -DMAI stream.c -o stream_dynamic_mai $(INC) $(LIBMAI) $(LIBNUMA)

clean:
	rm -f stream_dynamic stream_dynamic_mai *.o
