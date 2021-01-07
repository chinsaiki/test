# SPDX-License-Identifier: GPL-2.0-only
#CFLAGS += -std=c99 -pthread -iquote../../../../include/uapi -Wall

#TEST_GEN_PROGS := get_syscall_info peeksiginfo vmaccess

#include ../lib.mk


all:
	gcc get_syscall_info.c -o get_syscall_info.out
	gcc peeksiginfo.c -o peeksiginfo.out
	gcc vmaccess.c -o vmaccess.out -pthread
	
clean:
	rm -f *.out
