#!/bin/bash

LIBS_PROFILER=-ltcmalloc_and_profiler

gcc  $LIBS_PROFILER -lpthread $*

