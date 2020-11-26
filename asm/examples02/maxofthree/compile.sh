#!/bin/bash

nasm -felf64 maxofthree.asm 
gcc callmaxofthree.c maxofthree.o
./a.out
