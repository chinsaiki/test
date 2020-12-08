#!/bin/bash

gcc *.c -o test.out -pthread -lrt -w -DOS_LINUX=1