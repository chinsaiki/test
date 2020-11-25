#!/bin/bash

#生成随即mac地址
openssl rand -hex 6 | sed -r 's/(..)/\1:/g' | sed 's/.$//'

openssl rand -hex 6 | sed -r 's/(..)(..)(..)(..)(..)(..)/\1:\2:\3:\4:\5:\6/g'

openssl rand -hex 6 | sed -r 's/..\B/&:/g'

# \B 表示非按此边界
# \b 表示单词边界
# <a 表示以a开头的单词
# a> 表示以b结尾的单词
