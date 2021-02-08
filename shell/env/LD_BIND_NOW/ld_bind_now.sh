#!/bin/sh
	
LD_BIND_NOW=1
export LD_BIND_NOW
	
chrt --fifo 1 /opt/myapp/myapp-server &


