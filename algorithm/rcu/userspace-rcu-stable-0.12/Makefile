
all:
	gcc -shared -fpic src/*.c -I include/ -I src/ -DRCU_MEMBARRIER -o librcu.so -D_GNU_SOURCE

clean:
	rm -f librcu.so

