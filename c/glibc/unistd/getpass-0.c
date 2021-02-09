#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

int main(void) 
{ 
   char *password; 
   password = getpass("Input a password:"); /*输入密码*/
    printf("The password is: <%s> \n", password); /*显示密码*/
   return 0; 
}

