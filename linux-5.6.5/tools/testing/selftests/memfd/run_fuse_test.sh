#!/bin/sh
# SPDX-License-Identifier: GPL-2.0

if test -d "./mnt" ; then
	fusermount -u ./mnt
	rmdir ./mnt
fi

set -e

mkdir mnt
./fuse_mnt.out ./mnt
./fuse_test.out ./mnt/memfd $@
fusermount -u ./mnt
rmdir ./mnt
