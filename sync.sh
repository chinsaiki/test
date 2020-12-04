#!/bin/bash
# 该脚本备份本仓库到我的备份服务器上

backup_addr=root@10.170.6.59
backup_path=/work/workspace/test/

#备份还没有commit的git仓库文件到SCP
function backup_non_commit() {
	files=$(git status --porcelain --short | awk '{print $2}')
	if [ -z "$files" ]; then
		echo "Nothing to do."
	else
		scp -r $files $backup_addr:$backup_path
	fi
}

#备份已经commit的git仓库文件到SCP
# $1 : commit hash 字符串
function __backup_already_commit() {
	commit=$1
	files=$(git diff --stat  --name-only HEAD^ $commit)
	if [ -z "$files" ]; then
		echo "Nothing to do."
	else
		scp -r $files $backup_addr:$backup_path
	fi
}

#备份已经commit的git仓库文件到SCP
# $1 : 第几次 commit
function backup_already_H2icommit() {
	icommit=$1
	commit=$(git log | grep commit | head -n $icommit | tail -n 1 | awk '{print $2}')
	__backup_already_commit $commit	
}

#备份最后一次commit的git仓库文件到SCP
function backup_already_commit1() {
	__backup_already_commit HEAD
}

# main start
if [ $# -lt 2 ]; then
	echo "$0 commit <number>"
	backup_non_commit
	exit
elif [ $# -eq 2 ]; then
	backup_already_H2icommit $2
fi

# main end


