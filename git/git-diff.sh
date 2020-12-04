#!/bin/bash

#查看修改了哪些文件
git diff --stat  --name-only HEAD^ HEAD
