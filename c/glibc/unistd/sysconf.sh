#!/bin/bash
# 生成 sysconf 测试文件
function generate_integer_sysconf_test() {
	if [ $# -lt 1 ];then 
		echo "$0 [sysconf para name, _SC_PHYS_PAGES for example]"
		return 1 
	fi

	grep $1$ /usr/include/bits/confname.h > /dev/null
	if [ $? != 0 ]; then
		echo No $1 in /usr/include/bits/confname.h
		return 2
	fi

	filename="sysconf-$1.c"

	if [ -f $filename ]; then
		echo Alread exist $filename
		return 0
	fi

	echo "/* Test sysconf, rongtao `date` */" >> $filename
	echo "#include <stdio.h>" >> $filename
	echo "#include <unistd.h>" >> $filename
	echo "#include <unistd.h>" >> $filename
	echo "" >> $filename
	echo "int main(int argc, char *argv[])" >> $filename
	echo "{" >> $filename
	echo "    printf(\"$1 = %ld\\n\", sysconf($1));" >> $filename
	echo "    return 0;" >> $filename
	echo "}" >> $filename

	echo "Save to $filename"
	return 0
}

# 编译 generate_integer_sysconf_test 生成的 测试文件，如 sysconf-_SC_OPEN_MAX.c
function test_integer_sysconf() {
	generate_integer_sysconf_test $*
	if [ $? == 0 ]; then
		filename="sysconf-$1.c"
		echo "Compile >> $filename"
		gcc $filename
		echo "Running >> $filename"
		echo "----------------------------------------"

		./a.out
		return 0
	fi
	return 1
}

echo ""
echo "----------------------------------------"

test_integer_sysconf $*
