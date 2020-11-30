#!/bin/bash

LIBS_BASIC=-ltcmalloc

gcc  $LIBS_BASIC -lpthread $*
