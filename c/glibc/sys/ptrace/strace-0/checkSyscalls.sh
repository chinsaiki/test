#!/bin/sh

db="syscall_db"
struct="syscalls.h"

while read -r line
do [ "$line" != "" ] && ( tmp=$(grep $line $struct) || echo "$line not found" )
done < $db

exit 0