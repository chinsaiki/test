#!/bin/bash

TIMES=200000

nthread=20
max_size=4096

GLIBC_PREFIX="glibc-"
JEMALLOC_PREFIX="jemalloc-"
TCMALLOC_PREFIX="tcmalloc-"

rm -f $GLIBC_PREFIX* $JEMALLOC_PREFIX*

for ((thread=5;thread<=$nthread;thread+=5)); do
	for ((size=8;size<=$max_size;size+=16)); do
#		echo "$size $thread"
		alloc_time=`./main.out $TIMES $size $thread | grep -e tcmalloc -e jemalloc | awk '{print $3}'`
		glibc_time=`echo $alloc_time | awk '{print $1}'`
		jemalloc_time=`echo $alloc_time | awk '{print $2}'`
		echo "$thread/$nthread -> $size/$max_size"
		echo -e "$thread \t\t $size \t\t $glibc_time \t\t $jemalloc_time \t\t" >> $JEMALLOC_PREFIX$TCMALLOC_PREFIX$thread.txt
	done

done
