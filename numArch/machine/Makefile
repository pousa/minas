all: compute-machine

compute-machine: compute-machine.c
	gcc -g -O0 compute-machine.c -o compute-machine  ../../MA-place/lib/libmaplace.a -lhwloc

clean:
	rm -rf *.o *~ compute-machine
