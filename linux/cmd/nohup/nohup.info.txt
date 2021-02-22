File: coreutils.info,  Node: nohup invocation,  Next: stdbuf invocation,  Prev: nice invocation,  Up: Modified command invocation

23.4 'nohup': Run a command immune to hangups
=============================================

'nohup' runs the given COMMAND with hangup signals ignored, so that the
command can continue running in the background after you log out.
Synopsis:

     nohup COMMAND [ARG]...

   If standard input is a terminal, it is redirected from '/dev/null' so
that terminal sessions do not mistakenly consider the terminal to be
used by the command.  This is a GNU extension; programs intended to be
portable to non-GNU hosts should use 'nohup COMMAND [ARG]... </dev/null'
instead.

   If standard output is a terminal, the command's standard output is
appended to the file 'nohup.out'; if that cannot be written to, it is
appended to the file '$HOME/nohup.out'; and if that cannot be written
to, the command is not run.  Any 'nohup.out' or '$HOME/nohup.out' file
created by 'nohup' is made readable and writable only to the user,
regardless of the current umask settings.

   If standard error is a terminal, it is normally redirected to the
same file descriptor as the (possibly-redirected) standard output.
However, if standard output is closed, standard error terminal output is
instead appended to the file 'nohup.out' or '$HOME/nohup.out' as above.

   To capture the command's output to a file other than 'nohup.out' you
can redirect it.  For example, to capture the output of 'make':

     nohup make > make.log

   'nohup' does not automatically put the command it runs in the
background; you must do that explicitly, by ending the command line with
an '&'.  Also, 'nohup' does not alter the niceness of COMMAND; use
'nice' for that, e.g., 'nohup nice COMMAND'.

   COMMAND must not be a special built-in utility (*note Special
built-in utilities::).

   The only options are '--help' and '--version'.  *Note Common
options::.  Options must precede operands.

   Exit status:

     125 if 'nohup' itself fails, and 'POSIXLY_CORRECT' is not set
     126 if COMMAND is found but cannot be invoked
     127 if COMMAND cannot be found
     the exit status of COMMAND otherwise

   If 'POSIXLY_CORRECT' is set, internal failures give status 127
instead of 125.

