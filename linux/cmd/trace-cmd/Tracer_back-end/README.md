IOKPP front-end (I/O key performance parameter )
============
Free use of this software is granted under the terms of the GNU Public License (GPL).

IOkpp(IO key performance parameter) is a storage subsystem timing analysis tool,
it provides a non-invasive method to collect timing data on the Linux storage
subsystem from user space on path to the storage device. It consists of front-end
tool (parser) and back-end tool.
This tool is a prelimilary back-end part,which runs on the target machine side,
and is used to install/enable trace event, trigger Linux I/O request event and 
collect trace event log.
The front-end tool runs on the target platform, which is used to analyze/parse
the log of IOKPP back-end tool, and provide the I/O performance
timing statistics.




At the initial version, this back-end tool is based on the ftrace and trace-cmd.

Dependencies
============

 * ftrace
 * trace-cmd



Usage
============

trace-c.sh -l <trace-cmd location>  [-o <file>] [-c <command>]...
        -o <file>       output file which copied from trace.

        -O <"flag string"> add new command option parameters  for trace-cmd
                        eg:
                        ./trace-c.sh -l ./trace-cmd -O "-b 8192 -O nooverwrite" -c "/data/iozone -eco ..."

        -c <"command">  the running command while tracing, here you also
                        can add additional option for the trace-cmd, but must be
                        prefixed before command string.
                        eg:
                        ./trace-c.sh -l ./trace-cmd -c "-b 8192 -O nooverwrite /data/iozone -eco ..."
                        
        -l <location>   specify trace-cmd location.
        -t <record|     record
            report>     report trace log
        -h|-H           Show usage infor.

FIXME list
============
 * None


Developers
============
 Bean Huo beanhuo@micron.com

