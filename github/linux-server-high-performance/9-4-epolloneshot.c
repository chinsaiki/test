#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <stdbool.h>

//gcc 9-4-epolloneshot.c -pthread
//epoll的总结之四LT和ET使用EPOLLONESHOT
//
//https://blog.csdn.net/liuhengxiao/article/details/46911129
//-------------------------------------------------------
//
//在前面说过，epoll有两种触发的方式即LT（水平触发）和ET（边缘触发）两种，在前者，
//只要存在着事件就会不断的触发，直到处理完成，而后者只触发一次相同事件或者说只在
//从非触发到触发两个状态转换的时候儿才触发。
//
//
//这会出现下面一种情况，如果是多线程在处理，一个SOCKET事件到来，数据开始解析，
//这时候这个SOCKET又来了同样一个这样的事件，而你的数据解析尚未完成，那么程序会
//自动调度另外一个线程或者进程来处理新的事件，这造成一个很严重的问题，不同的线程或者进程在处
//理同一个SOCKET的事件，这会使程序的健壮性大降低而编程的复杂度大大增加！！即使在ET模
//式下也有可能出现这种情况！！
//
//
//解决这种现象有两种方法，一种是在单独的线程或进程里解析数据，也就是说，
//接收数据的线程接收到数据后立刻将数据转移至另外的线程。
//
//
//第一种方法
//第二种方法就是本文要提到的EPOLLONESHOT这种方法，可以在epoll上注册这个事件，注册这个事
//件后，如果在处理写成当前的SOCKET后不再重新注册相关事件，那么这个事件就不再响应了
//或者说触发了。要想重新注册事件则需要调用epoll_ctl重置文件描述符上的事件，这样前面的
//socket就不会出现竞态这样就可以通过手动的方式来保证同一SOCKET只能被一个线程处理，不会
//跨越多个线程。


#define MAX_EVENT_NUMBER     1024
#define BUFFER_SIZE          1024

struct file_fd_pair_s
{
    int epollfd;
    int sockfd;
};

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

/*将fd上的EPOLLIN和EPOLLET事件注册到epollfd指示的epoll内核事件表中，
    参数oneshot指定是否注册fd上的EPOLLONESHOT事件
 */
void addfd(int epollfd, int fd, bool oneshot)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events  = EPOLLIN | EPOLLET;
    if (oneshot) 
    {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

/*重置fd上的事件，这样操作后，尽管fd上的EPOLLONESHOT事件被注册，但是操作系统仍然会触发fd上的EPOLLIN事件，且之触发一次
 */

void reset_oneshot(int epollfd, int fd)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

/*工作线程*/
void* worker(void *arg)
{
    int sockfd = ((struct file_fd_pair_s *)arg)->sockfd;
    int epollfd = ((struct file_fd_pair_s *)arg)->epollfd;
    fprintf(stdout, "start new thread to receive data on fd: %d\n", sockfd);
    char buf[BUFFER_SIZE];
    memset(buf, '\0', BUFFER_SIZE);
    
    while(1)
    {
        int ret = recv(sockfd, buf, BUFFER_SIZE, 0); 
         if (ret == 0) 
         {
             close(sockfd);
             fprintf(stderr, "foreiner closed the connection\n");
             break;
         }
         else if (ret < 0) 
         {
             if (errno == EAGAIN) 
             {
                 reset_oneshot(epollfd, sockfd);
                 fprintf(stdout, "read later\n");
                 break;
             }
         }
         else
         {
             fprintf(stdout, "get content: %s\n", buf);
             sleep(5); //sleep 5 second
         }
    } 

    fprintf(stdout, "end thread receiveing data on fd: %d\n", sockfd);
    return NULL;
}


int main(int argc, char *argv[])
{
    if (argc <= 2) 
    {
        fprintf(stderr, "usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }

    const char * ip = argv[1];
    int port        = atoi(argv[2]);

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd > 0);

    ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(listenfd, 5);
    assert(ret != -1);

    struct epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);

    
    addfd(epollfd, listenfd, false);

    while(1)
    {
        int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if (ret < 0) 
        {
            fprintf(stderr, "epoll failure\n");
            break;
        }
        int i;
        for(i = 0; i < ret; i ++)
        {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd) 
            {
                struct sockaddr_in client_address;
                socklen_t client_addlength = sizeof(client_address);
                int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addlength);
                
                /**/
                addfd(epollfd, connfd, true);
            }
            else if (events[i].events & EPOLLIN) 
            {
                pthread_t thread;
                struct file_fd_pair_s fds_for_new_worker;
                fds_for_new_worker.epollfd = epollfd;
                fds_for_new_worker.sockfd  = sockfd;
                
                pthread_create(&thread, NULL, worker, (void *)&fds_for_new_worker);
            }
            else
            {
                fprintf(stderr, "something else happened\n"); 
            }
        }
    }
    close(listenfd);
    return 0;
}
