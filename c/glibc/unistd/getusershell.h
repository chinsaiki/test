//getusershell, setusershell, endusershell - get permitted user shells

#include <unistd.h>

///sbin/sh 
///usr/bin/sh 
///usr/bin/rsh 
///usr/bin/ksh 
///usr/bin/rksh
///usr/bin/csh 
///usr/bin/keysh 

char *getusershell(void);
void setusershell(void);
void endusershell(void);


char *getusershell_r(char **shell_datap); 
void setusershell_r(char **shell_datap); 
void endusershell_r(char **shell_datap); 
//https://docstore.mik.ua/manuals/hp-ux/en/B2355-60130/getusershell.3C.html

