#!/bin/bash

rm -f *.out

for file in `ls test_*`
do 
	echo "Compile $file -> ${file%.*}.out"
	gcc $file rax*.c -pthread -std=c99 -I./ -o ${file%.*}.out -w -lm
done


