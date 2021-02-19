#!/bin/bash
# Viewing Current Configurations

tuna --threads=http* --show_threads \
	 --cpus=0 --move --show_threads \
	 --cpus=1 --move --show_threads \
	 --cpus=+0 --move --show_threads
