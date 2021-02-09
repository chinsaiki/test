#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <crypt.h>

//[rongtao@localhost unistd]$ gcc getpass-1.c  -lcrypt
//[rongtao@localhost unistd]$ ./a.out

//getpass函数通常会与crypt加密函数一同使用

int main()
{
    char passwd[13];
    char *key;
    char slat[2];
    key = getpass("Input first passward:");
    slat[0] = key[0];
    slat[1] = key[1];
    strcpy(passwd,crypt(key,slat));
    key = getpass("Input second passward:");
    slat[0] = passwd[0];
    slat[1] = passwd[1];
    printf("After crypt(),1st passwd:%s\n",passwd);
    printf("After crypt(),2st passwd:%s\n",passwd);
    return 0;
}

