#!/bin/bash

rm -f *.out

for file in `ls test_*`
do 
	echo "Compile $file -> ${file%.*}.out"
	gcc $file crc*.c -std=c99 -pthread -I./ -o ${file%.*}.out -w
done


