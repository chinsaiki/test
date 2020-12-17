#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<arpa/inet.h>
#include<unistd.h>

#define PORT 8888
#define MAX 1024

int main()
{

    char buf[MAX] = {'0'};
    int sockfd;
    int n;
    socklen_t slen;
    slen = sizeof(struct sockaddr);
    struct sockaddr_in seraddr;

    bzero(&seraddr,sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(PORT);
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);


    //socket()
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        perror("socket");
        exit(-1);
    }
    //connect()
    if(connect(sockfd,(struct sockaddr *)&seraddr,slen) == -1)
    {
        perror("connect");
        exit(-1);
    }

    int ret = shutdown(sockfd , SHUT_RDWR);
    if(ret < 0)
    { //这里关闭了 客户端的 读写， server再发送数据 给 client 时， 会出现 write error: Broken pipe 错误
        perror("shutdown perror");
    }

    return 0;
}

