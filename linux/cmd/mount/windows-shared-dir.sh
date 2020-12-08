#!/bin/bash
# 将Windwos的目录共享到 Linux
# 步骤
#	1.在windows选择文件夹右键-属性-共享-共享
#	2.成功后会显示 “\\R11017001\OpenSource”
#	3.确保linux和windows网络通畅（ping一下）
#	4.挂载 
#		mount -t cifs -o username=rongtao,password=Rong1234 //10.170.6.24/OpenSource /mnt/windows/

USER=rongtao
PASSWD="Rong1234"

SRC_DIR="//10.170.6.24/OpenSource"
DST_DIR="/mnt/windows/"

mount -t cifs -o username=$USER,password=$PASSWD $SRC_DIR $DST_DIR
