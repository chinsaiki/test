#!/bin/bash

rm -f *.out


for file in `ls test_*.c`
do 
	echo "Compile $file -> ${file%.*}.out"
	gcc $file zmalloc.c -I./  -pthread -o ${file%.*}-jemalloc.out -w -std=c99 $* -DUSE_JEMALLOC -DJEMALLOC_FRAG_HINT -ljemalloc
	gcc $file zmalloc.c -I./  -pthread -o ${file%.*}-tcmalloc.out -w -std=c99 $* -DUSE_TCMALLOC -ltcmalloc
	gcc $file zmalloc.c -I./  -pthread -o ${file%.*}-glibc.out -w -std=c99 $* 
done



