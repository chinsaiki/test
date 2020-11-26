#!/bin/bash

nasm -felf64 factorial.asm 
gcc -std=c99 factorial.o callfactorial.c
./a.out