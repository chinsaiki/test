#!/bin/bash

rm -f *.out


for file in `ls test_*.c`
do 
	echo "Compile $file -> ${file%.*}.out"
	gcc $file ./../../zmalloc/redis-6.0.5/zmalloc.c adlist.c -I./../../zmalloc/redis-6.0.5 \
			-pthread -o ${file%.*}.out -w -std=c99 $* -DUSE_JEMALLOC -ljemalloc
done



