#!/bin/sh
# 荣涛 
#windows "\r\n" to "\n"
function win2linux()
{
	sed -i 's/\r$//' $*
}