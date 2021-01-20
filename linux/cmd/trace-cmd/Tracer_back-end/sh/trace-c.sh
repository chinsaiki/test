#!/bin/sh
# 
# This is a shell script file which is used to generate ftrace log.
# 
#
# Dependencies: trace-cmd


source ./io-cfg.sh   # load configure file

export Command=""
export Flags=""
export opts="t:l:o:O:c:Hh"
export Trace_C="trace-cmd"
export OP="record"
export ENABLE=1
export DISABLE=0
export TraceFs="/sys/kernel/debug/tracing"
export DEBUG=$ENABLE
export LogFile="./IOKPP.dat"


function print() {
	if [ $DEBUG -eq $ENABLE ]; then
	local msg=$1
	echo $msg
	fi
}

function usage() {
cat << EOF
Usage:
$0 [-o <file>] [-c <command>]...
	-o <file>	output file which copied from trace.

	-O <"flag string"> add new command option parameters  for trace-cmd
			eg:
			./trace-c.sh -l ./trace-cmd -O "-b 8192 -O nooverwrite" -c "/data/iozone -eco ..."

	-c <"command">	the running command while tracing, here you also
			can add additional option for the trace-cmd, but must be
			prefixed before command string.
			eg:
			./trace-c.sh -l ./trace-cmd -c "-b 8192 -O nooverwrite /data/iozone -eco ..."
			
	-l <location>	specify trace-cmd location.
	-t <record|	record
	    report>	report trace log
	-h|-H		Show usage infor.
EOF
}

function check_env() {

if type ${Trace_C} >/dev/null 2>&1; then
	echo ""
else
	usage
	echo "${Trace_C} doesn't work!"
	echo "I require trace-cmd, but it's not installed. Please install firstly. Or"
	echo " using -l option to specify it."
	echo " Aborting."
	exit
fi
}

function search_for_tracefs() {

	TraceFs=`sed -ne 's/^tracefs \(.*\) tracefs.*/\1/p' /proc/mounts`

	print "The TraceFs location is  ${TraceFs}"
}

#
#### Main()
#

#function parse_args() {

while getopts ${opts}  OPTION;do
	case $OPTION in
	o)
	LogFile=$OPTARG
	print "Output file specified is  ${LogFile}"
	;;
	c)
	Command=$OPTARG
	print "Command specified is  ${Command}"
	;;
	l)
	Trace_C=$OPTARG
	print "Trace_C specified is ${Trace_C}"
	;;
	t)
	OP=$OPTARG
	print "OP specified is ${OP}"
	;;
	O)
	Flags=$OPTARG
	print "Flags specified is ${Flags}"
	;;
	h|H)
	usage
	exit 1
	;;
	*)
	usage
	exit 1
	;;
	esac
done

#}


search_for_tracefs

#parse_args $@ ###getops doesn't work in the sub-function, will fix later.

echo 0 > ${TraceFs}/trace

check_env

${Trace_C} ${OP} ${FLAGS} ${Flags} ${Command}

if [ $? -eq 0 ];
then
	cat ${TraceFs}/trace > ${LogFile}
else
	
	print "${Trace_C} Error Failed."
fi
