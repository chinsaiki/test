#!/bin/bash

## 用及其读懂的形式显示修改添加过的文件
git status --porcelain --short | awk '{print $2}'
