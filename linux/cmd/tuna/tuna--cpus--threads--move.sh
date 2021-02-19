#!/bin/bash
# Assigning Tasks to Specific CPUs
tuna --cpus=0,1 --threads=ssh\* --move --cpus=2,3 --threads=http\* --move
