#!/bin/bash

gcc -std=gnu99 -pthread event.c rbtree.c sample_work.c util.c work.c
