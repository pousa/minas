CC = gcc
CFLAGS = -O2 -fopenmp
INC=-I../../Minas/MAi-array/include
LIBNUMA=-lnuma -lm
LIBMAI=-L../../Minas/MAi-array/lib -lmai

all: stream_one stream_one_mai

stream_one: stream.c
	$(CC) $(CFLAGS) stream.c -o stream_one 

stream_one_mai: stream.c
	$(CC) $(CFLAGS) -DMAI stream.c -o stream_one_mai $(INC) $(LIBMAI) $(LIBNUMA)

clean:
	rm -f stream_one stream_one_mai *.o
