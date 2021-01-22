#!/bin/bash

rm -f *.out

for file in `ls test-*`
do 
	echo "Compile $file -> ${file%.*}.out"
	gcc $file nmq.c -lcrypt -pthread -I./include -o ${file%.*}.out -w
done


