#!/bin/bash
# 荣涛 2021年1月6日08:32:10

SRCS=(fd-002-posix-eq.c \
		proc-loadavg-001.c \
		proc-multiple-procfs.c \
		proc-self-map-files-001.c \
		proc-self-map-files-002.c \
		proc-self-syscall.c \
		proc-self-wchan.c \
		proc-uptime-001.c \
		proc-uptime-002.c \
		read.c \
		self.c \
		setns-dcache.c \
		setns-sysvipc.c \
		thread-self.c)

for src in ${SRCS[@]}; do
	echo 编译 $src
	gcc $src -o ${src%.*}.out
done


