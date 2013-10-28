CC = gcc
CFLAGS = -O2 -fopenmp
INC=-I../../Minas/MAi-array/include
LIBNUMA=-lnuma -lm
LIBMAI=-L../../Minas/MAi-array/lib -lmai

all: stream_random stream_random_mai

stream_random: stream.c
	$(CC) $(CFLAGS) stream.c -o stream_random

stream_random_mai: stream.c
	$(CC) $(CFLAGS) -DMAI stream.c -o stream_random_mai $(INC) $(LIBMAI) $(LIBNUMA)

clean:
	rm -f stream_random stream_random_mai *.o
