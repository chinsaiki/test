#!/bin/sh
# 荣涛 
# say hello
function say_hi_to_you()
{
	fortune -s | cowsay
	fortune -s | cowsay -f ghostbusters
}

#say_hi_to_you
function say_hi_to_other_pts()
{
	wall `whoami`:`tty` Login.
}

