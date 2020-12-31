#!/bin/sh
# /*********************** self documentation **********************/
# COPYRIGHT - insert CSM COPYRIGHT lines at top of files in working directory
#
# Usage: copyright file(s)
#
# /**************** end self doc ********************************/
# 荣涛 

#Rong Tao's Comment 
function Comment
{
#Copyright=$PWD/Copyright
	Copyright=Copyright
	tmpfile=/tmp/$$.cright
	Date=`date | awk '{print $0}'`
	RT="Comment Rong Tao @Sylincom Beijing"

# have arguments
	if [ $# -gt 1 ]; then
	    echo "Usage: $ Comment a.c or $ Copyright"
	elif [ $# -eq 1 ]; then
	    echo -e "\033[1;32mComment $1\033[0m"
	    if [ -d $1 ]; then
	        echo ""
	        echo -e "Can't add comment to a directory."
	        echo ""
	    elif [ .c = `echo $1 | sed 's/.*\.c/.c/'` -o \
	          .cpp = `echo $1 | sed 's/.*\.cpp/.cpp/'` -o \
	          .cc = `echo $1 | sed 's/.*\.cc/.cc/'` -o \
	          .cxx = `echo $1 | sed 's/.*\.cxx/.cxx/'` -o \
	          .h = `echo $1 | sed 's/.*\.h/.h/'` -o \
	          .hxx = `echo $1 | sed 's/.*\.hxx/.hxx/'` -o \
	          .sql = `echo $1 | sed 's/.*\.sql/.sql/'` -o \
			  .php = `echo $1 | sed 's/.*\.php/.php/'` -o \
			  .js = `echo $1 | sed 's/.*\.js/.js/'` ]; then
	        >$tmpfile
	        echo "/* "$RT", $Date. */" >> $tmpfile
	        #echo "" >> $tmpfile
	        cat $1 >> $tmpfile
	        #cat $tmpfile | grep Copyright
	        mv -f $tmpfile $1
	        
	    elif [ .bash = `echo $1 | sed 's/.*\.bash/.bash/'` -o \
	           .sh = `echo $1 | sed 's/.*\.sh/.sh/'` -o \
			   .py = `echo $1 | sed 's/.*\.py/.py/'` ]; then
	        >$tmpfile
	        echo "# "$RT", $Date. " >> $tmpfile
	        #echo "" >> $tmpfile
	        cat $1 >> $tmpfile
	        #cat $tmpfile | grep Copyright
	        mv -f $tmpfile $1
	    elif [ .txt = `echo $1 | sed 's/.*\.txt/.txt/'` ]; then
	        >$tmpfile
	        echo "# "$RT", $Date. " >> $tmpfile
	        #echo "" >> $tmpfile
	        cat $1 >> $tmpfile
	        #cat $tmpfile | grep Copyright
	        mv -f $tmpfile $1
	    elif [ .asm = `echo $1 | sed 's/.*\.asm/.asm/'` ]; then
	        >$tmpfile
	        echo "; "$RT", $Date. " >> $tmpfile
	        #echo "" >> $tmpfile
	        cat $1 >> $tmpfile
	        #cat $tmpfile | grep Copyright
	        mv -f $tmpfile $1
	    fi  
	    return 0
	fi

# don't have arguments
	for i in `ls`
	do
	    echo -e "\033[1;32mComment $i\033[0m"
	    if [ -d $i ]; then
	        cd $i
	        $Copyright
	        cd ..
	    elif # C file
	        [ .c = `echo $i | sed 's/.*\.c/.c/'` -o \
	          .cpp = `echo $i | sed 's/.*\.cpp/.cpp/'` -o \
	          .cc = `echo $i | sed 's/.*\.cc/.cc/'` -o \
	          .cxx = `echo $i | sed 's/.*\.cxx/.cxx/'` -o \
	          .h = `echo $i | sed 's/.*\.h/.h/'` -o \
	          .hxx = `echo $i | sed 's/.*\.hxx/.hxx/'` -o \
	          .sql = `echo $i | sed 's/.*\.sql/.sql/'` -o \
			  .php = `echo $i | sed 's/.*\.php/.php/'` -o \
			  .js = `echo $i | sed 's/.*\.js/.js/'` ]
	        then
	        >$tmpfile
	        echo "/* "$RT", $Date. */" >> $tmpfile
	        #echo "" >> $tmpfile
	        cat $i >> $tmpfile
	        #cat $tmpfile | grep Copyright
	        mv -f $tmpfile $i
	    elif # shell file
	        [ .bash = `echo $i | sed 's/.*\.bash/.bash/'` -o \
	          .sh = `echo $i | sed 's/.*\.sh/.sh/'` -o \
			  .py = `echo $i | sed 's/.*\.py/.py/'` ]
	        then
	        >$tmpfile
	        echo "# "$RT", $Date. " >> $tmpfile
	        #echo "" >> $tmpfile
	        cat $i >> $tmpfile
	        #cat $tmpfile | grep Copyright
	        mv -f $tmpfile $i
	    elif # txt
	        [ .txt = `echo $i | sed 's/.*\.txt/.txt/'` ]
	        then
	        >$tmpfile
	        echo "# "$RT", $Date. " >> $tmpfile
	        cat $i >> $tmpfile
	        mv -f $tmpfile $i
	    elif # asm
	        [ .asm = `echo $i | sed 's/.*\.asm/.asm/'` ]
	        then
	        >$tmpfile
	        echo "; "$RT", $Date. " >> $tmpfile
	        cat $i >> $tmpfile
	        mv -f $tmpfile $i
	    fi

	done
}

#Rong Tao's Copyright 
function Copyright
{
#Copyright=$PWD/Copyright
	Copyright=Copyright
	tmpfile=/tmp/$$.cright
	Date=`date | awk '{print $0}'`
	RT="Copyright (C) Rong Tao @Sylincom Beijing"

# have arguments
	if [ $# -gt 1 ]; then
	    echo "Usage: $ Copyright a.c or $ Copyright"
	elif [ $# -eq 1 ]; then
	    echo -e "\033[1;32mCopyright $1\033[0m"
	    if [ -d $1 ]; then
	        echo ""
	        echo -e "Can't add copyright to a directory."
	        echo ""
	    elif [ .c = `echo $1 | sed 's/.*\.c/.c/'` -o \
	          .cpp = `echo $1 | sed 's/.*\.cpp/.cpp/'` -o \
	          .cc = `echo $1 | sed 's/.*\.cc/.cc/'` -o \
	          .cxx = `echo $1 | sed 's/.*\.cxx/.cxx/'` -o \
	          .h = `echo $1 | sed 's/.*\.h/.h/'` -o \
	          .hxx = `echo $1 | sed 's/.*\.hxx/.hxx/'` -o \
	          .sql = `echo $1 | sed 's/.*\.sql/.sql/'` -o \
			  .php = `echo $1 | sed 's/.*\.php/.php/'` -o \
			  .js = `echo $1 | sed 's/.*\.js/.js/'` ]; then
	        >$tmpfile
	        echo "/* "$RT", $Date. */" >> $tmpfile
	        #echo "" >> $tmpfile
	        cat $1 >> $tmpfile
	        #cat $tmpfile | grep Copyright
	        mv -f $tmpfile $1
	        
	    elif [ .bash = `echo $1 | sed 's/.*\.bash/.bash/'` -o \
	           .sh = `echo $1 | sed 's/.*\.sh/.sh/'` ]; then
	        >$tmpfile
	        echo "# "$RT", $Date. " >> $tmpfile
	        #echo "" >> $tmpfile
	        cat $1 >> $tmpfile
	        #cat $tmpfile | grep Copyright
	        mv -f $tmpfile $1
	    elif [ .txt = `echo $1 | sed 's/.*\.txt/.txt/'` ]; then
	        >$tmpfile
	        echo "# "$RT", $Date. " >> $tmpfile
	        #echo "" >> $tmpfile
	        cat $1 >> $tmpfile
	        #cat $tmpfile | grep Copyright
	        mv -f $tmpfile $1
	    elif [ .asm = `echo $1 | sed 's/.*\.asm/.asm/'` ]; then
	        >$tmpfile
	        echo "; "$RT", $Date. " >> $tmpfile
	        #echo "" >> $tmpfile
	        cat $1 >> $tmpfile
	        #cat $tmpfile | grep Copyright
	        mv -f $tmpfile $1
	    fi  
	    return 0
	fi

# don't have arguments
	for i in `ls`
	do
	    echo -e "\033[1;32mCopyright $i\033[0m"
	    if [ -d $i ]; then
	        cd $i
	        $Copyright
	        cd ..
	    elif # C file
	        [ .c = `echo $i | sed 's/.*\.c/.c/'` -o \
	          .cpp = `echo $i | sed 's/.*\.cpp/.cpp/'` -o \
	          .cc = `echo $i | sed 's/.*\.cc/.cc/'` -o \
	          .cxx = `echo $i | sed 's/.*\.cxx/.cxx/'` -o \
	          .h = `echo $i | sed 's/.*\.h/.h/'` -o \
	          .hxx = `echo $i | sed 's/.*\.hxx/.hxx/'` -o \
	          .sql = `echo $i | sed 's/.*\.sql/.sql/'` -o \
			  .php = `echo $i | sed 's/.*\.php/.php/'` -o \
			  .js = `echo $i | sed 's/.*\.js/.js/'` ]
	        then
	        >$tmpfile
	        echo "/* "$RT", $Date. */" >> $tmpfile
	        #echo "" >> $tmpfile
	        cat $i >> $tmpfile
	        #cat $tmpfile | grep Copyright
	        mv -f $tmpfile $i
	    elif # shell file
	        [ .bash = `echo $i | sed 's/.*\.bash/.bash/'` -o \
	          .sh = `echo $i | sed 's/.*\.sh/.sh/'` ]
	        then
	        >$tmpfile
	        echo "# "$RT", $Date. " >> $tmpfile
	        #echo "" >> $tmpfile
	        cat $i >> $tmpfile
	        #cat $tmpfile | grep Copyright
	        mv -f $tmpfile $i
	    elif # txt
	        [ .txt = `echo $i | sed 's/.*\.txt/.txt/'` ]
	        then
	        >$tmpfile
	        echo "# "$RT", $Date. " >> $tmpfile
	        cat $i >> $tmpfile
	        mv -f $tmpfile $i
	    elif # asm
	        [ .asm = `echo $i | sed 's/.*\.asm/.asm/'` ]
	        then
	        >$tmpfile
	        echo "; "$RT", $Date. " >> $tmpfile
	        cat $i >> $tmpfile
	        mv -f $tmpfile $i
	    fi

	done
}

#This is for pushd
#pushd /etc/jt_product/jt_sran/app
#pushd /home/rongtao

#GNU copyright
function Copyright_GNUv11
{
#Copyright=$PWD/Copyright
	Copyright=Copyright_GNU
	tmpfile=/tmp/$$.cright
	Date=`date | awk '{print $0}'`
	RT="\n \
	 Copyright (C) Rong Tao @Beijing\n\n \
	  Permission is granted to copy, distribute and/or modify this document\n \
	  under the terms of the GNU Free Documentation License, Version 1.1\n \
	  or any later version published by the Free Software Foundation;\n \
	  with the Invariant Sections being list their titles, with the\n \
	  Front-Cover Texts being list, and with the Back-Cover Texts being list.\n \
	  A copy of the license is included in the section entitled ‘‘GNU\n \
	  Free Documentation License’’.\n"

# have arguments
	if [ $# -gt 1 ]; then
	    echo "Usage: $ Copyright a.c or $ Copyright"
	elif [ $# -eq 1 ]; then
	    echo -e "\033[1;32mCopyright $1\033[0m"
	    if [ -d $1 ]; then
	        echo ""
	        echo -e "Can't add copyright to a directory."
	        echo ""
	    elif [ .c = `echo $1 | sed 's/.*\.c/.c/'` -o \
	          .cpp = `echo $1 | sed 's/.*\.cpp/.cpp/'` -o \
	          .cc = `echo $1 | sed 's/.*\.cc/.cc/'` -o \
	          .cxx = `echo $1 | sed 's/.*\.cxx/.cxx/'` -o \
	          .h = `echo $1 | sed 's/.*\.h/.h/'` -o \
	          .hxx = `echo $1 | sed 's/.*\.hxx/.hxx/'` ]; then
	        >$tmpfile
	        echo -e "/* "$RT"   $Date. \n*/" >> $tmpfile
	        cat $1 >> $tmpfile
	        mv -f $tmpfile $1        
	    fi  
	    return 0
	fi

# don't have arguments
	for i in `ls`
	do
	    echo -e "\033[1;32mCopyright $i\033[0m"
	    if [ -d $i ]; then
	        cd $i
	        $Copyright
	        cd ..
	    elif # C file
	        [ .c = `echo $i | sed 's/.*\.c/.c/'` -o \
	          .cpp = `echo $i | sed 's/.*\.cpp/.cpp/'` -o \
	          .cc = `echo $i | sed 's/.*\.cc/.cc/'` -o \
	          .cxx = `echo $i | sed 's/.*\.cxx/.cxx/'` -o \
	          .h = `echo $i | sed 's/.*\.h/.h/'` -o \
	          .hxx = `echo $i | sed 's/.*\.hxx/.hxx/'` -o \
	          .sql = `echo $i | sed 's/.*\.sql/.sql/'` ]
	        then
	        >$tmpfile
	        echo -e "/* "$RT"   $Date. \n*/" >> $tmpfile
	        cat $i >> $tmpfile
	        mv -f $tmpfile $i
	    fi
	done
}

#!/bin/bash
#GNU version1.3 copyright
function Copyright_GNUv13
{
#Copyright=$PWD/Copyright
	Copyright=Copyright_GNUv13
	tmpfile=/tmp/$$.cright
	Date=`date | awk '{print $0}'`
	RT="\n \
	 Copyright (C) Rong Tao @Beijing\n\n \
	  Permission is granted to copy, distribute and/or modify this document\n \
	  under the terms of the GNU Free Documentation License, Version 1.3\n \
	  or any later version published by the Free Software Foundation;\n \
	  with no Invariant Sections, no Front-Cover Texts, and no Back-Cover\n \
	  Texts. A copy of the license is included in the section entitled ‘‘GNU\n \
	  Free Documentation License’’.\n"

# have arguments
	if [ $# -gt 1 ]; then
	    echo "Usage: $ Copyright a.c or $ Copyright"
	elif [ $# -eq 1 ]; then
	    echo -e "\033[1;32mCopyright $1\033[0m"
	    if [ -d $1 ]; then
	        echo ""
	        echo -e "Can't add copyright to a directory."
	        echo ""
	    elif [ .c = `echo $1 | sed 's/.*\.c/.c/'` -o \
	          .cpp = `echo $1 | sed 's/.*\.cpp/.cpp/'` -o \
	          .cc = `echo $1 | sed 's/.*\.cc/.cc/'` -o \
	          .cxx = `echo $1 | sed 's/.*\.cxx/.cxx/'` -o \
	          .h = `echo $1 | sed 's/.*\.h/.h/'` -o \
	          .hxx = `echo $1 | sed 's/.*\.hxx/.hxx/'` -o \
	          .sql = `echo $1 | sed 's/.*\.sql/.sql/'` ]; then
	        >$tmpfile
	        echo -e "/* "$RT"   $Date. \n*/" >> $tmpfile
	        cat $1 >> $tmpfile
	        mv -f $tmpfile $1        
	    fi  
	    return 0
	fi

# don't have arguments
	for i in `ls`
	do
	    echo -e "\033[1;32mCopyright $i\033[0m"
	    if [ -d $i ]; then
	        cd $i
	        $Copyright
	        cd ..
	    elif # C file
	        [ .c = `echo $i | sed 's/.*\.c/.c/'` -o \
	          .cpp = `echo $i | sed 's/.*\.cpp/.cpp/'` -o \
	          .cc = `echo $i | sed 's/.*\.cc/.cc/'` -o \
	          .cxx = `echo $i | sed 's/.*\.cxx/.cxx/'` -o \
	          .h = `echo $i | sed 's/.*\.h/.h/'` -o \
	          .hxx = `echo $i | sed 's/.*\.hxx/.hxx/'` -o \
	          .sql = `echo $i | sed 's/.*\.sql/.sql/'` ]
	        then
	        >$tmpfile
	        echo -e "/* "$RT"   $Date. \n*/" >> $tmpfile
	        cat $i >> $tmpfile
	        mv -f $tmpfile $i
	    fi
	done
}

function copyright() {

	if [ $# -lt 1 ]; then
		echo "Usage : $0 [file|directory]"
		return 1
	fi

	tmpfile=/tmp/$$.cright
	THISYEAR=`date`

	for i in $*
	do
		if
			[ -d $i ]
		then
			cd $i
			echo " Change directory to >> $i"
			copyright `ls`
			echo " Leave directory to >> $i"
			cd ..
		elif # C files
			[ .c = `echo $i | sed 's/.*\.c/.c/'` \
			-o .h = `echo $i |sed 's/.*\.h/.h/'` \
			-o .cpp = `echo $i |sed 's/.*\.cpp/.cpp/'` \
			-o .cxx = `echo $i |sed 's/.*\.cxx/.cxx/'` \
			-o .hxx = `echo $i |sed 's/.*\.hxx/.hxx/'` \
			-o .hpp = `echo $i |sed 's/.*\.hpp/.hpp/'` ]
		then
			>$tmpfile
			echo " ===rtoax=== $i"
		echo "/* Copyright (c) RToax, $THISYEAR.*/" >>$tmpfile
		echo "/* All rights reserved.                       */" >>$tmpfile
			echo "" >>$tmpfile
			cat $i >>$tmpfile

			mv -f $tmpfile $i
		elif # Fortran files
			[ .f = `echo $i | sed 's/.*\.f/.f/'` ]
		then
			>$tmpfile			
			echo " ===rtoax=== $i"
		echo "* Copyright (c) RToax, $THISYEAR." >>$tmpfile
			echo "* All rights reserved." >>$tmpfile
			echo "" >>$tmpfile
			cat $i >>$tmpfile

			mv -f $tmpfile $i
		elif # bash files
			[ .sh = `echo $i | sed 's/.*\.sh/.sh/'` -o .bash = `echo $i | sed 's/.*\.bash/.bash/'` ]
		then
			>$tmpfile
			echo " ===rtoax=== $i"
		echo "# Copyright (c) RToax, $THISYEAR." >>$tmpfile
			echo "# All rights reserved." >>$tmpfile
			echo "" >>$tmpfile
			cat $i >>$tmpfile

			mv -f $tmpfile $i
		fi
	done
}


