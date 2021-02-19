#!/bin/bash
# rongtao 2021年2月19日
# 测试 利用 echo 作为返回值的 的 函数

function hello() {
	read -r -p "Input>> " string
	echo $string
}

function test1() {
	echo argv = $*
}

#hello
test1 `hello`


