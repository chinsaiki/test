#include <unistd.h>
#include <stdio.h>
#include <sys/eventfd.h>

int main()
{
    int efd = eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);

    eventfd_write(efd, 2);
    eventfd_write(efd, 3);
    eventfd_write(efd, 4);


    eventfd_t count;

    eventfd_read(efd, &count);
    printf("count = %d\n", count);
    eventfd_read(efd, &count);
    printf("count = %d\n", count);
    eventfd_read(efd, &count);
    printf("count = %d\n", count);
    eventfd_read(efd, &count);
    printf("count = %d\n", count);
    eventfd_write(efd, -4);
    eventfd_write(efd, -4);
    eventfd_write(efd, 4);
    eventfd_read(efd, &count);
    printf("count = %d\n", count);
    eventfd_read(efd, &count);
    printf("count = %d\n", count);
    eventfd_read(efd, &count);
    printf("count = %d\n", count);
    eventfd_read(efd, &count);
    printf("count = %d\n", count);
    
//    [root@localhost eventfd]# gcc demo-multi-readwrite.c 
//    [root@localhost eventfd]# ./a.out 
//    count = 9
//    count = 9
//    count = 9


    close(efd);
}

