#!/bin/bash
# Rong tao

function QA() {
    while true
    do
        echo -n "$1 [Y/N]: "
        read x
        case "$x" in
        Y|y|yes|Yes|YES ) return 0;;
        N|n|no|No|NO  ) return 1;;
        * ) echo "Input yes or no!"
        esac
    done
}

# 支持传入一个参数 
# 循环往复提示是否 yes 或 no
# yes 返回 0
# no 返回 1
function QA2() {
	hint="[Y/n] "
	if [ $# == 1 ]; then
		hint=$1$hint
	else
		hint="Are you sure?"$hint
	fi
	while true
	do
		read -r -p "$hint" input
		case $input in
			[yY][eE][sS]|[yY])
				#echo "Yes"
				return 0
				;;
			[nN][oO]|[nN])
				#echo "No"
				return 1
			   	;;
			*)
				#echo "Invalid input..."
				#exit 1
				;;
		esac
	done
}

