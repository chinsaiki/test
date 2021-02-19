#!/bin/bash

total_nlines=`cat list.txt | wc -l`


match_lines=`cat list.txt | grep -E "^([0-9]\\w*[,|-])*[0-9]\\w*$"`
match_nlines=`cat list.txt | grep -E "^([0-9]\\w*[,|-])*[0-9]\\w*$" | wc -l`

echo "共 $total_nlines 行，匹配 $match_nlines 行，内容如下："
echo "$match_lines"
