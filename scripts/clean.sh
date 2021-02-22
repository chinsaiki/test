#!/bin/sh
# 荣涛 


function cleanall {
	filename=("*.o" "*.d" "*.gch" "*~"  "*.out" "core.*" "vgcore.*" "*.exe" "*.so" "*.a")
	for name in ${filename[@]}; do
	    #echo $name
	    find . -name $name -type f -print -exec rm -rf {} \;
	done 
	return 0
}

function clean {
	echo $PWD | grep "\/test" 2>&1 > /dev/null
	if [ $? != 0 ]; then
		return 1
	fi
	cleanall
	return 0
}

