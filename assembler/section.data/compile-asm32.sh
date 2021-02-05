# Copyright (C) Rong Tao @Sylincom Beijing, 2021年 02月 05日 星期五 08:46:47 CST. 
#!/bin/bash

if [ $# -lt 1 ]; then
	echo 请输入 32 位 汇编程序
	exit 1
fi

nasm -f elf32 $* -o example.o
ld -m elf_i386 example.o -o a.out
./a.out

echo exit code is $?
