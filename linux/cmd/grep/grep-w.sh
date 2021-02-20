#!/bin/bash
# 匹配一个单词

string="Hello World, My name is RTOAX."

echo $string
read -r -p "匹配单词：" word

echo 匹配单词 $word

echo $string | grep -w $word
