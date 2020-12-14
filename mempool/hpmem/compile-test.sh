#!/bin/bash

rm -f *.out


HP_INCLUDE=-I./src

HP_LIBS=-pthread

HP_DEFINE=-DHPMEM_DEBUG=1

for file in `ls test_*.c`
do 
	echo "Compile $file -> ${file%.*}.out"
	gcc $file src/*.c -I./src -I./   $HP_LIBS -o ${file%.*}-jemalloc-dbg.out -w $* -DUSE_JEMALLOC -ljemalloc $HP_DEFINE
	gcc $file src/*.c -I./src -I./   $HP_LIBS -o ${file%.*}-tcmalloc-dbg.out -w $* -DUSE_TCMALLOC -ltcmalloc $HP_DEFINE
	gcc $file src/*.c -I./src -I./   $HP_LIBS -o ${file%.*}-glibc-dbg.out -w $*  $HP_DEFINE
	gcc $file src/*.c -I./src -I./   $HP_LIBS -o ${file%.*}-jemalloc.out -w $* -DUSE_JEMALLOC -ljemalloc 
	gcc $file src/*.c -I./src -I./   $HP_LIBS -o ${file%.*}-tcmalloc.out -w $* -DUSE_TCMALLOC -ltcmalloc 
	gcc $file src/*.c -I./src -I./   $HP_LIBS -o ${file%.*}-glibc.out -w $*  
done




