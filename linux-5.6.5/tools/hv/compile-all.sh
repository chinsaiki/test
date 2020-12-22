#!/bin/bash

files=`ls *.c`
for file in $files
do 
	gcc $file -o $file.out
done

