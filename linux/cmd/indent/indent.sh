#!/bin/bash
for file in `find ./ -name "*.c"`; do
	indent -npro -nip -nlp -npsl -i4 -ts4 -sob -l200 -ss -bl -bli 0 -l80 $file
done

for file in `find ./ -name "*.h"`; do
	indent -npro -nip -nlp -npsl -i4 -ts4 -sob -l200 -ss -bl -bli 0 -l80 $file
done
