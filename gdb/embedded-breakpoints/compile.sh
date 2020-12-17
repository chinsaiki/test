#!/bin/bash

gcc -Wall -std=c99 -lbfd-2.25.1-31.base.el7 -o gdb-with-breakpoints.out gdb-with-breakpoints.c
gcc example.c -o example.out
