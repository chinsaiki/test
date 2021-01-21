#!/bin/bash

function compile_for_so() {
	gcc -shared -fpic src/*.c -I include/ -I src/ -DRCU_MEMBARRIER -o librcu.so -D_GNU_SOURCE
}

compile_for_so
