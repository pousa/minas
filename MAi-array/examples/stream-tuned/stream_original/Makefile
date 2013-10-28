CC = gcc
CFLAGS = -O2 -fopenmp
INC=-I../../Minas/MAi-array/include
LIBNUMA=-lnuma -lm
LIBMAI=-L../../Minas/MAi-array/lib -lmai

all: stream_original stream_original_mai

stream_original: stream.c
	$(CC) $(CFLAGS) stream.c -o stream_original

stream_original_mai: stream.c
        $(CC) $(CFLAGS) -DMAI stream.c -o stream_original_mai $(INC) $(LIBMAI) $(LIBNUMA)

clean:
	rm -f stream_original stream_original_mai *.o
