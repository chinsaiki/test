#!/bin/bash
# 删除文件中每行开头的两个字符，
# 比如你复制了一段代码，发现里面有行号

sed -i 's/^..//' test_string.c
