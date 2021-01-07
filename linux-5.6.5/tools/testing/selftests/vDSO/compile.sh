#!/bin/bash

gcc vdso_test_getcpu.c parse_vdso.c -std=c99 -o vdso_test_getcpu.out
gcc vdso_test_gettimeofday.c parse_vdso.c -std=c99 -o vdso_test_gettimeofday.out
