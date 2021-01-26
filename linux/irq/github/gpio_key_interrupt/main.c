#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    int fd;
    int value;
    unsigned char key_value[11];
	int cnt = 0;

    fd = open("/dev/hubuyu", O_RDONLY);
    if(fd < 0)
    {
         perror("cannot open device button");
         exit(1);
    }

    do{
        read(fd, &key_value, sizeof(key_value));
        printf("loop-%d key_value is %s\n",cnt, key_value);
    }while(cnt++<=20);

        close(fd);
        return 0;

}

