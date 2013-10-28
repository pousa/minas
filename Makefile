all:
	make -C MAi-array
	make -C MAi-heap
	make -C Memtop
	make -C MA-place      

clean:
	make -C MAi-array clean
	make -C MAi-heap clean
	make -C Memtop clean
	make -C numArch clean
	make -C MA-place clean
