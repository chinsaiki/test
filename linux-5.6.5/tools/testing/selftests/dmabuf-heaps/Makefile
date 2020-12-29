# SPDX-License-Identifier: GPL-2.0
#CFLAGS += -static -O3 -Wl,-no-as-needed -Wall -I../../../../usr/include

TEST_GEN_PROGS = dmabuf-heap

#include ../lib.mk

all:
	gcc ${TEST_GEN_PROGS}.c -o ${TEST_GEN_PROGS}.out
