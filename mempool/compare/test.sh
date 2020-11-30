#!/bin/bash

TIMES=200000

nthread=21
max_size=4096

EXEC=$1
MALLOC_PREFIX=$2


GREP_FILTER="-i $MALLOC_PREFIX"

for ((thread=1;thread<=$nthread;thread+=5)); do
	for ((size=8;size<=$max_size;size+=16)); do
		alloc_time=`./$EXEC $TIMES $size $thread | grep $GREP_FILTER`
#		echo "$alloc_time"
		talloc=`echo $alloc_time | awk '{print $3}'`
		tfree=`echo $alloc_time | awk '{print $5}'`
		mem_usage=`echo $alloc_time | awk '{print $6}'`
		echo "$thread/$nthread -> $size/$max_size $talloc $tfree $mem_usage"
		echo -e "$thread \t\t $size \t\t $talloc \t\t $tfree \t\t $mem_usage \t\t" >> $MALLOC_PREFIX-$thread.txt
	done
done


