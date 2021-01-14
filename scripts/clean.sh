#!/bin/sh
# 荣涛 
function clean {
	filename=("*.o" "*.d" "*.gch" "*~"  "*.out" "core.*" "vgcore.*" "*.exe")
	for name in ${filename[@]}; do
	    #echo $name
	    find . -name $name -type f -print -exec rm -rf {} \;
	done 
}
