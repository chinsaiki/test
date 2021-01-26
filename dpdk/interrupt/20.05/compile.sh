#!/bin/bash

gcc eal_interrupts.c test_interrupts.c -I. -pthread
