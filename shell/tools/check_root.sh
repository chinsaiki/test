#!/bin/bash

check_root()
{
	uid=$(id -u)
	if [ $uid -ne 0 ]; then
		echo $TCID: must be run as root >&2
		exit $ksft_skip
	fi
}

