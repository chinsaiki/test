#!/bin/bash
# 荣涛 2021年2月20日
# 文件系统相关的配置

ETC_FSTAB="/etc/fstab"


function mkfs_xfs_help {
	echo -e "[FS]\n[FS] \033[1m mkfs.xfs 帮助信息\033[m"	
	echo -e "[FS]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

	mkfs.xfs 2>&1 | sed -n '2,100p' | sed 's/^/[FS]  >>&/g'
}
function mkfs_ext2_help {
	echo -e "[FS]\n[FS] \033[1m mkfs.ext2 帮助信息\033[m"	
	echo -e "[FS]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

	mkfs.ext2 2>&1 | sed 's/^/[FS]  >>&/g'
}
function mkfs_ext3_help {
	echo -e "[FS]\n[FS] \033[1m mkfs.ext3 帮助信息\033[m"	
	echo -e "[FS]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

	mkfs.ext3 2>&1 | sed 's/^/[FS]  >>&/g'
}
function mount_help {
	echo -e "[FS]\n[FS] \033[1m mount 帮助信息\033[m"	
	echo -e "[FS]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

	mount --help | head -n30 | sed -n '2,30p' | sed 's/^/[FS]  >>&/g'
}


function __rt_config_fs_command_manual {
	echo -e "[FS]\n[FS] =============== 显示 文件系统相关 手册 信息"
	
	fs_manual_options="[mkfs.xfs|mkfs.ext2|mkfs.ext3|mount|回车退出]"
	while read -r -p "[FS] 选择手册 $fs_manual_options: " man_type
	do
		case $man_type in
		mkfs.xfs)
			mkfs_xfs_help
		;;
		mkfs.ext2)
			mkfs_ext2_help
		;;
		mkfs.ext3)
			mkfs_ext3_help
		;;
		mount)
			mount_help
		;;
		exit|quit|q)
			break
		;;
		*) 
			if [ -z $man_type ]; then
				break
			fi
			
		;;
		esac
	done
}

#禁用atime
#/etc/fstab使用您选择的文本编辑器 打开文件，然后找到根安装点的条目。
#/dev/mapper/rhel-root       /       xfs    defaults…
#编辑选项部分以包含术语noatime和nodiratime。noatime防止在读取文件时更新访问时间戳，并nodiratime停止更新目录inode的访问时间。
#/dev/mapper/rhel-root       /       xfs    noatime,nodiratime…
#重要
#一些应用程序依赖于atime更新。因此，此选项仅在不使用此类应用程序的系统上才是合理的。
#或者，您可以使用relatime安装选项，该选项可确保仅当先前的访问时间早于当前修改时间时才更新访问时间。
function __rt_config_fs_forbidden_atime() {
	
	echo -e "[FS]\n[FS] =============== 禁用 atime"
	echo -e "[FS]\n[FS] 一些应用程序依赖于atime更新。因此，此选项仅在不使用此类应用程序的系统上才是合理的。"
	echo -e "[FS] 或者，您可以使用relatime安装选项，该选项可确保仅当先前的访问时间早于当前修改时间时才更新访问时间。"

	echo -e "[FS]\n[FS] 系统中包含的文件系统如下："
	#[FS]    /dev/mapper/centos-root /                       xfs     defaults        0 0
	#[FS]    /dev/mapper/centos-swap swap                    swap    defaults        0 0
	cat $ETC_FSTAB | grep "^\/dev\/mapper" | sed "s/^/[FS]    &/g"
	
	echo -e "[FS]"

	#/dev/mapper/centos-root
	#/dev/mapper/centos-swap
	__FSs=`cat $ETC_FSTAB  | grep "^\/dev\/mapper" | awk '{print $1}'`

	#/dev/mapper/centos-root to centos-root
	#/dev/mapper/centos-swap to centos-swap
	FSs=(${__FSs//\/dev\/mapper\//})
	
	#centos-root centos-swap
	#echo ${FSs[@]} | sed "s/^/[FS]    &/g"

	#[centos-root|centos-swap]
	#echo ${FSs[@]} | sed "s/ /|/g" | awk '{print "["$1"]"}' | sed "s/^/[FS]    &/g"

	THIS_FS=""
	
	#[FS] 选择 [centos-root|centos-swap]: 
	all_fs_selector=`echo ${FSs[@]} | sed "s/ /|/g" | awk '{print "["$1"]"}'`
	while read -r -p "[FS] 选择下面的文件系统进行操作 $all_fs_selector: " infomation
	do
		#echo ">>> infomation=$infomation"
		#echo ">>> all_fs_selector=$all_fs_selector"

		case $infomation in
		exit|quit)
			break
		;;
		*) 
			if [ ! -z $infomation ]; then
				cat $ETC_FSTAB | awk '{print $1}' | grep "^\/dev\/mapper\/$infomation$" 2>&1 > /dev/null
				if [ $? == 0 ]; then

					#centos-root to /dev/mapper/centos-root
					THIS_FS=/dev/mapper/$infomation
					#echo $THIS_FS
					break
				fi
			fi
			echo -e '[FS]		\033[1;31m,,错误的输入！\033[m'需要 $all_fs_selector
		;;
		esac
		infomation=""
	done

	#/dev/mapper/centos-root
	#/dev/mapper/centos-swap
	#echo $THIS_FS
	PURE_THIS_FS=`basename $THIS_FS`
	#echo $PURE_THIS_FS
	
	MY_ETC_FSTAB=""
	defaults="defaults"
	noatime="noatime,nodiratime"

	# 是否使用默认的 "/etc/fstab"
	while true
	do
		read -r -p "[FS] 使用默认配置文件 \"$ETC_FSTAB\"? [Y/n] " input
		case $input in
			[yY][eE][sS]|[yY])
				MY_ETC_FSTAB=$ETC_FSTAB
				break
				;;

			[nN][oO]|[nN])
				while true
				do
					read -r -p "[FS] 输入文件名取代 \"$ETC_FSTAB\"? [Y/n] " fstab_name

					# 输入的 fstab 配置文件 存在
					if [ ! -z $fstab_name ]; then
						if [ -f $fstab_name ]; then
							MY_ETC_FSTAB=$fstab_name
							break
						else
							echo -e '[FS]		\033[1;31m文件不存在！\033[m'
						fi
					fi
				done
				# $MY_ETC_FSTAB 被设置了
				if [ ! -z $MY_ETC_FSTAB ]; then
					break
				fi
			   	;;
			exit|quit)
				break
			;;
			*)
				echo -e '[FS]		\033[1;31m错误的输入！"\033[m'
				;;
		esac
	done

	while read -r -p "[FS] 配置 FS 的 属性 [defaults|noatime]: " cfg_type
	do
		
		case $cfg_type in
		defaults)
			# 将 /dev/mapper/centos-root /                       xfs     defaults        0 0
			# 至 /dev/mapper/centos-root /                       xfs     noatime,nodiratime        0 0
			sed -i "/^\/dev\/mapper\/$PURE_THIS_FS/s/$noatime/$defaults/g" $MY_ETC_FSTAB
			break
		;;
		noatime)
			# 将 /dev/mapper/centos-root /                       xfs     defaults        0 0
			# 至 /dev/mapper/centos-root /                       xfs     noatime,nodiratime        0 0
			sed -i "/^\/dev\/mapper\/$PURE_THIS_FS/s/$defaults/$noatime/g" $MY_ETC_FSTAB
			break
		;;
		exit|quit)
			break
		;;
		*) 
			echo -e '[FS]		\033[1;31m错误的输入！"\033[m'
		;;
		esac
	done
}

function __rt_config_fs_more {
	
	echo -e "[FS] =============== 禁用 atime"
	echo -e "[FS] 一些应用程序依赖于atime更新。因此，此选项仅在不使用此类应用程序的系统上才是合理的。"
	echo -e "[FS] 或者，您可以使用relatime安装选项，该选项可确保仅当先前的访问时间早于当前修改时间时才更新访问时间。"
	
	read -r -p "[FS] 按任意键继续: " __tmp

}

function rt_config_fs {
	__rt_config_fs_forbidden_atime
	__rt_config_fs_command_manual
}

#rt_config_fs
#__rt_config_fs_command_manual