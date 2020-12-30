#!/bin/bash
# 包查询
package=graphviz

files=$(rpm -ql $package)
echo $files
