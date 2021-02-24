#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>

#include "utils.h"

/*
    在实时内核下 /proc/interrupts 的下面中断会暴增
                CPU0       CPU1       CPU2       CPU3
     RES:  179392251  155305295  176785837  176676556   Rescheduling interrupts

    操作系统、内核和实时补丁版本例如下：
 # uname -r
    3.10.0-1127.rt56.1093.el7.x86_64
    
 # cat /etc/redhat-release 
    CentOS Linux release 7.4.1708 (Core)

    详情请见本仓库： test/linux/real-time/Rescheduling-interrupts
    
*/

#define MAX_EVENTS 10

struct epoll_context {
    int epollfd;

    struct epoll_event events[MAX_EVENTS];
    
};

static int epoll_context_init(struct epoll_context *ctx) {
    
    int epollfd = epoll_create(10);
    if (epollfd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }
    memset(ctx, 0, sizeof(struct epoll_context));
    
    ctx->epollfd = epollfd;
    
    return epollfd;
}

static void epoll_add_evenfd(struct epoll_context *ctx, int evt_fd) {
    struct epoll_event ev;

    ev.events = EPOLLIN;
    ev.data.fd = evt_fd;
    if (epoll_ctl(ctx->epollfd, EPOLL_CTL_ADD, evt_fd, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }
    
}

int eventfd_create() {
    int efd = eventfd(0, EFD_CLOEXEC);
    if (efd <= 0) {
        perror("eventfd");
        exit(EXIT_FAILURE);
    }
    return efd;
}



void *read_task(void*arg){
    struct epoll_context *ectx = (struct epoll_context *)arg;
    
    int i =0, nmsg = 0, imsg = 0, ret, nfds;
    eventfd_t count = 1;
    
    for (;;) {
        nfds = epoll_wait(ectx->epollfd, ectx->events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_pwait");
            exit(EXIT_FAILURE);
        }
        
        for (i = 0; i < nfds; ++i) {
            ret = eventfd_read(ectx->events[i].data.fd, &count);
            if(ret < 0) {
                break;
            }
//            log_dequeue("read ret = %d, count = %ld\n", ret, count);
            for(imsg = 0; imsg < count; ++imsg) {
                
            }
        }
    }
    pthread_exit(NULL);
}

void *write_task(void*arg) {
    
    struct epoll_context *ectx = (struct epoll_context *)arg;
    int evt_fd = eventfd_create();
    epoll_add_evenfd(ectx, evt_fd);
    
    int i =0;
    int ret;
    eventfd_t count = 1;
    
    while(1) {
//        sleep(1);
        ret = eventfd_write(evt_fd, count);
        if(ret < 0) {
            continue;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int i;
    int nwrite = 3;
    
    pthread_t dequeue, enqueue[10];

    struct epoll_context ectx;
    epoll_context_init(&ectx);

    pthread_create(&dequeue, NULL, read_task, &ectx);

    for(i=0;i<nwrite;i++) {
        pthread_create(&enqueue[i], NULL, write_task, &ectx);
    }

    pthread_join(dequeue, NULL);
    for(i=0;i<nwrite;i++) {
        pthread_join(enqueue[i], NULL);
    }

    return EXIT_SUCCESS;
}
