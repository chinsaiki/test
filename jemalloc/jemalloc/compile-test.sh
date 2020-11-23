#!/bin/bash

rm -f *.out


for file in `ls test_*.c`
do 
	echo "Compile $file -> ${file%.*}.out"
	gcc $file -I./  -pthread -o ${file%.*}.out -w $*
	gcc $file -I./  -pthread -o ${file%.*}_jemalloc.out -w $* -DUSE_JEMALLOC -ljemalloc
done




