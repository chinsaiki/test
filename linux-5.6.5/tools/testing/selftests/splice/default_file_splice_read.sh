#!/bin/sh
# SPDX-License-Identifier: GPL-2.0
n=`./default_file_splice_read.out </dev/null | wc -c`

test "$n" = 0 && exit 0

echo "default_file_splice_read.out broken: leaked $n"
exit 1
