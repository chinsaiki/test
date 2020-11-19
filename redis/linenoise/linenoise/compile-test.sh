#!/bin/bash

rm -f *.out


for file in `ls test_*.c`
do 
	echo "Compile $file -> ${file%.*}.out"
	gcc $file linenoise.c ../../sds/redis-6.0.5/sds.c -I../../sds/redis-6.0.5/ -I./  -pthread -o ${file%.*}.out -w -std=c99
done


