#!/bin/bash

./configure --prefix=/usr \
	--enable-autogen \
	--enable-documentation \
	--enable-shared \
	--enable-static \
	--enable-debug \
	--enable-experimental-smallocx \
	--enable-prof \
	--enable-prof-libunwind \
	--enable-utrace \
	--enable-xmalloc --enable-log \
	--enable-readlinkat \
	--enable-opt-safety-checks \
	--enable-opt-size-checks \
	--enable-lazy-lock \
	--with-mangling \
	--with-jemalloc-prefix=je_ \
	--with-malloc-conf=/etc/jemalloc.conf 
