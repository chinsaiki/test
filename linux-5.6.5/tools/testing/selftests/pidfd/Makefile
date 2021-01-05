#!/bin/bash

all:
	gcc pidfd_test.c -pthread -o pidfd_test.out
	gcc pidfd_fdinfo_test.c -o pidfd_fdinfo_test.out
	gcc pidfd_poll_test.c -o pidfd_poll_test.out
	gcc pidfd_open_test.c -o pidfd_open_test.out

clean:	
	rm -f *.out
