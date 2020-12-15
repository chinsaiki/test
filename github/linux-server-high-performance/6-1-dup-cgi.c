#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>



int main(int argc, const char *argv[])
{
    if (argc <= 2) 
    {
        fprintf(stderr, "Usage: %s ip_address port_number\n", basename(argv[0]));
        exit(1);
    }
    const char *ip = argv[1];
    int port       = atoi(argv[2]);

    struct sockaddr_in  address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port   = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int ret = bind(sock, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(sock, (struct sockaddr *)&client, &client_addrlength);
    if (connfd < 0) 
    {
        fprintf(stderr, "accept error, errno:%d, strerr:%s\n", errno, strerror(errno));
    }
    else
    {
        //int fd = open("b.txt.out", O_RDWR|O_CREAT|O_TRUNC, S_IWUSR|S_IWGRP|S_IWOTH);
        //
        //printf("open = %d\n", fd);
        //
        ////关闭了 标准输出
        //close(STDOUT_FILENO);
        //
        ////复制 打开文件的 fd， 这里会生成 fd = 1 （标准输出）
        //fprintf(stderr, "dup = %d\n", dup(fd));
        //
        ////再往 fd = 1 写入时， 将同步到 打开的 文件描述符中
        //printf("abcd pid = %d\n", getpid());
        //close(fd);
        close(STDOUT_FILENO);
        dup(connfd);
        printf("abcd\n");
        close(connfd);
    }

    close(sock);
    return 0;
}
