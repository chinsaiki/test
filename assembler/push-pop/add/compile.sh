#!/bin/bash
# Copyright (C) Rong Tao @Sylincom Beijing, 2021年 02月 05日 星期五 09:09:49 CST. 

nasm -f elf32 add42.asm -o add42.o
gcc -m32 add42.o main.c -o a.out
