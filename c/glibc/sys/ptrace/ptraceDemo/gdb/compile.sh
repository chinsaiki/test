#!/bin/bash

gcc -m32 hello.c -o b.out

gcc -m32 $*
