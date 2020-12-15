#define _GNU_SOURCE  1
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <fcntl.h>
#include <stdio.h>


/** 百度百科 **
 *
 *	poll是linux中的字符设备驱动中的一个函数，linux2.5.44版本后，poll被epoll取代。
 *	和select实现的功能差不多，poll的作用是把当前的文件指针挂到等待队列。
 *
 *	作用：把当前的文件指针挂到等待队列；
 *	功能：和select差不多；
 *
 */
/**
  POLLIN        普通或优先级带数据可读
  POLLRDNORM    普通数据可读
  POLLRDBAND    优先级带数据可读
  POLLPRI       高优先级数据可读
  
  POLLOUT       普通数据可写
  POLLWRNORM    普通数据可写
  POLLWRBAND    优先级带数据可写
  
  POLLERR       发生错误
  POLLHUP       发生挂起
  POLLNVAL      描述符不是一个可打开的文件

 */


#define BUFFER_SIZE   64

int main(int argc, const char *argv[])
{
    if (argc < 2) 
    {
        fprintf(stderr, "usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char *ip = argv[1];
    int port       = atoi(argv[2]);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &server_address.sin_addr);
    server_address.sin_port = htons(port);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);
    if (connect(sockfd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) 
    {
        fprintf(stderr, "connection failed\n");
        close(sockfd);
        return 1;
    }
    
    struct pollfd fds[2];
    /*注册文件描述符0（标准输入)和文件描述符sockfd上的可读事件*/
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[0].revents= 0;
    fds[1].fd = sockfd;
    fds[1].events = POLLIN | POLLRDHUP;
    fds[1].revents = 0;
    
    char read_buf[BUFFER_SIZE];
    int pipefd[2];
    int ret = pipe(pipefd);
    assert(ret != -1);

    while(1)
    {
        ret = poll(fds, 2, -1);
        if (ret < 0) 
        {
            fprintf(stderr, "poll failure\n");
            break;
        }

        if (fds[1].revents & POLLRDHUP) 
        {
            fprintf(stderr, "server close the connection\n");
            break;
        }
        else if (fds[1].revents & POLLIN) 
        {
            memset(read_buf, '\0', sizeof(BUFFER_SIZE));
            recv(fds[1].fd, read_buf, BUFFER_SIZE-1, 0);
            fprintf(stderr, "%s\n", read_buf);
        }

        if (fds[0].revents & POLLIN) 
        {
            ret = splice(0, NULL, pipefd[1], NULL, 32678, SPLICE_F_MORE | SPLICE_F_MOVE);
            ret = splice(pipefd[0], NULL, sockfd, NULL, 32678, SPLICE_F_MORE | SPLICE_F_MOVE);
        }
    }
    
    close(sockfd);
    return 0;
}
