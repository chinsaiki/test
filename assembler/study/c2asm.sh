#!/bin/bash
# 显示从C语言到汇编语言的转化过程代码
# 荣涛 2021年2月22日

if [ $# -lt 1 ]; then
	echo Usage: $0 [prog.c]
	exit 1
fi

prog=$1				# program.c
name=${prog%.*}		# program
subfix=${prog##*.}	# c

#echo "$prog = $name . $subfix"

if [ $subfix != "c" ]; then
	echo "Must be C source code."
	exit 1
fi

gcc $prog -S -o $name.s.out

cat $prog | sed 's/^/[ c ] &/g'
echo ""
cat $name.s.out | grep -E -v  $'\t'\\. | sed 's/^/[asm] &/g'

rm -f $name.s.out
