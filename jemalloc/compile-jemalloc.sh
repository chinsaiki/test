#!/bin/bash

# 编译 jemalloc

gcc test1.c \
		-I`jemalloc-config --includedir` \
		-L`jemalloc-config --libdir` \
		-Wl,-rpath,`jemalloc-config --libdir` \
		-ljemalloc `jemalloc-config --libs`
