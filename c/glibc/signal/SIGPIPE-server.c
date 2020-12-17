#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define port 8888

void handle(int sig)
{
    printf("SIGPIPE : %d\n",sig);
}

void mysendmsg(int fd)
{
    // 写入第一条消息
    char* msg1 = "first msg"; 
    int n = write(fd, msg1, strlen(msg1));

    if(n > 0)  //成功写入第一条消息,server 接收到 client 发送的 RST
    {
        printf("success write %d bytes\n", n);
    }

    // 写入第二条消息,触发SIGPIPE
    char* msg2 = "second msg";
    n = write(fd, msg2, strlen(msg2));
    if(n < 0)
    {
        printf("write error: %s\n", strerror(errno));
    }
}
int main()
{
    signal(SIGPIPE , handle); //注册信号捕捉函数
//    signal(SIGPIPE, SIG_IGN);
    
    struct sockaddr_in server_addr;

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    int listenfd = socket(AF_INET , SOCK_STREAM , 0);

    bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    listen(listenfd, 128);

    int fd = accept(listenfd, NULL, NULL);
    if(fd < 0)
    {
        perror("accept");
        exit(1);
    }

    mysendmsg(fd);

    return 0;
}

