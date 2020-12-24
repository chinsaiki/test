#!/bin/bash

gcc __FILE__.c -m32
gcc readELF.c -m32 -o readelf.out 
