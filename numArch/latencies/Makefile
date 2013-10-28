all: compute-latencies

compute-latencies: compute-latencies.c
	gcc -g -O0 compute-latencies.c -o compute-latencies -lhwloc lmbench.a ../lib/libnumarch.a

clean:
	rm -rf *.o *~ compute-latencies
