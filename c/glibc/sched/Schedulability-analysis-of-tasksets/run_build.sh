#!/bin/bash

gcc main_synthetic.c Schd_algorithms.c -o test1.out -lm
./test1.out
gcc main_integrate.c Schd_algorithms.c -o test2.out -lm
./test2.out
