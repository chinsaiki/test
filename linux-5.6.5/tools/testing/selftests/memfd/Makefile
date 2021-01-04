# rtoax 2021年1月5日

all:
	gcc memfd_test.c common.c -D_FILE_OFFSET_BITS=64 -o memfd_test.out
	gcc fuse_test.c -D_FILE_OFFSET_BITS=64 common.c -lfuse -o fuse_test.out 
	cc fuse_mnt.c -D_FILE_OFFSET_BITS=64 common.c -lfuse -o fuse_mnt.out 

clean:
	rm -f *.out
