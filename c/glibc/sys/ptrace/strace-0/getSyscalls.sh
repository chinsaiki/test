#!/bin/sh

inc_file="/usr/include/asm/unistd_64.h"
begin_line="#define __NR_"
db="syscall_db"

rm -rf $db

while read -r line
do [ "$line" != "" ] && [ "${line%%$begin_line*}" = "" ] && echo "${line##$begin_line}" | cut -d' ' -f 1 | cut -f 1 >> $db
done < $inc_file

exit 0