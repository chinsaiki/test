all:../common/liblthread.so
	gcc main.c -I ../common/ -I. pthread_shim.c   -ldl -pthread ../common/liblthread.so

../common/liblthread.so:
	make -C ../common/

clean:
	rm -f a.out