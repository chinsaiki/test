# Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:06:42 CST. 
# Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:51 CST. 
# Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:28:21 CST. 


echo 'Input a number between 1 to 4'
echo 'Your number is:\c'
read aNum
case $aNum in
1) 
echo -e "\033[1;31m"
echo 'You select 1'
echo -e "\033[m"
;;
2) 
echo -e "\033[1;31m"
echo 'You select 2'
echo -e "\033[m"
;;
3) 
echo -e "\033[1;31m"
echo 'You select 3'
echo -e "\033[m"
;;
4) 
echo -e "\033[1;31m"
echo 'You select 4'
echo -e "\033[m"
;;
*) 
echo -e "\033[1;31m"
echo 'You do not select a number between 1 to 4'
echo -e "\033[m"
;;
esac

