SRC=src/
INC=-Iinclude
LIB=lib/

all: clean libnumarch.a install

libnumarch.a: numArch.o
	$(AR) cr libnumarch.a $(SRC)numArch.o

numArch.o: $(SRC)numArch.c
	$(CC) -w -c -g -o $(SRC)$@ $(SRC)$(@F:.o=.c) $(INC)

install:
	mv libnumarch.a $(LIB)

clean:
	rm -f src/numArch.o src/*~  lib/libnumarch.a
