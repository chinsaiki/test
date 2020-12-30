#!/bin/bash

git log --oneline --decorate --graph

# 结果如下
#* 0ce0658 (HEAD, origin/master, master) slab 介绍2
#* 871a58b slab 介绍
#* 88d67f6 valgrind 调试和性能分析工具
#* 423b74e rpm 查询 和 汇编的 memset

# 添加 tag
git tag -a v0.9 0ce0658

# 提交到远程
git push origin --tags
