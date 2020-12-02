.PHONY:test,clean

#obj := main.o sysfs_ops.o hugepage_memory.o hugepage_malloc.o runtime_info.o 
#OBJS = $(join ./objs/, $(obj) )


test.out:main.o sysfs_ops.o hugepage_memory.o hugepage_malloc.o runtime_info.o
#test.app:$(OBJS)
	gcc ./*.o -o test.out

test:test.out
	$(EXEC) ./test.out

main.o:main.c
	gcc -c main.c -o ./main.o -g3

sysfs_ops.o:sysfs_ops.c
	gcc -c sysfs_ops.c -o ./sys_fs.o -g3

hugepage_memory.o:hugepage_memory.c
	gcc -c hugepage_memory.c -o ./hugepage_memory.o -g3

hugepage_malloc.o:hugepage_malloc.c
	gcc -c hugepage_malloc.c -o ./hugepage_malloc.o -g3

runtime_info.o:runtime_info.c
	gcc -c runtime_info.c -o ./runtime_info.o -g3

clean:
	rm ./*.o test.out