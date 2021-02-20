#!/bin/bash
taskset --help | sed 's/^/[SCHED]  &/g'
