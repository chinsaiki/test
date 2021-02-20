# Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:06:42 CST. 
# Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:51 CST. 
# Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:28:21 CST. 


option="${1}"
case ${option} in
-f) 
FILE="${2}"
echo "File name is $FILE"
;;
-d) 
DIR="${2}"
echo "Dir name is $DIR"
;;
*)
echo "`basename ${0}`:usage: [-f file] | [-d directory]"
exit 1 # Command to come out of the program with status 1
;;
esac