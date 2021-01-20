#!/bin/bash

kernel-is-rt
if [ $? -eq 0 ]; then
	echo "This `uname -r` kernel is rt kernel."
else
	echo "This `uname -r` kernel is not rt kernel."
fi
