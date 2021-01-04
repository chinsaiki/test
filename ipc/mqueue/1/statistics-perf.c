/**
 *  统计 mqueue 消息队列的吞吐率和性能
 *  作者：荣涛  
 *  日期  ：2021年1月4日
 */
#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <sys/msg.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifndef TEST_SIZE
#define TEST_SIZE   1024
#endif
#ifndef TEST_NLOOP
#define TEST_NLOOP   10240
#endif

#define MQUEUE_NAME "/_rtoax_mq_"
//#define MSG_FILE "./_rtoax_msgq_"
#define MSG_FILE "./a"

#define MQUEUE_MAX_SIZE TEST_SIZE
#define MQUEUE_MAX_MSG 256

#define MSGQ_TYPE   888

#ifndef offsetof
#define offsetof(type, number) __builtin_offsetof(type, number)
#endif

struct dbg_msg {
    unsigned long msg_seq;
    unsigned long msg_ack;
    unsigned long msg_len;
    unsigned int crc32;
    char data[];
};

pthread_t task1, task2;
mqd_t mqd;
int msqid;

static unsigned int crc32(unsigned char const *p, unsigned int len)
{
	int i;
	unsigned int crc = 0;
    
#ifdef CRC
	while (len--) {
		crc ^= *p++;
		for (i = 0; i < 8; i++)
			crc = (crc >> 1) ^ ((crc & 1) ? 0xedb88320 : 0);
	}
#endif
	return crc;
}



static unsigned long int diff_timeval_usec(struct timeval *big, struct timeval *small)
{
    unsigned long int diffsec = big->tv_sec - small->tv_sec;
    unsigned long int diffusec = diffsec*1000000 + (big->tv_usec - small->tv_usec);
//    printf("%ld MicroSec\n", diffusec);
    return diffusec;
}

typedef ssize_t (*recv_hook_fn)(void *msg_ptr, size_t msg_len);
typedef int (*send_hook_fn)(void *msg_ptr, size_t msg_len);

ssize_t __hook_mq_receive(void *msg_ptr, size_t msg_len)
{
    return mq_receive(mqd, msg_ptr, msg_len, 0);
}
ssize_t __hook_msgrcv(void *msg_ptr, size_t msg_len)
{
    return msgrcv(msqid, msg_ptr, msg_len, MSGQ_TYPE, 0);
}

int __hook_mq_send(void *msg_ptr, size_t msg_len)
{
    return mq_send(mqd, (char*)msg_ptr, msg_len, 0);
}
int __hook_msgsnd(void *msg_ptr, size_t msg_len)
{
    return msgsnd(msqid, msg_ptr, msg_len, 0);
}

void printf_rate(const char *prefix, unsigned long bytes, unsigned long microsec)
{
    printf("%s rate %.3lf Mbits/sec\n", prefix, bytes*8.0/1024/1024*1000000/microsec);

}

void test_mqueue_send(send_hook_fn send_fn)
{
    int ret;
    ssize_t n;
    char send_buffer[MQUEUE_MAX_SIZE] ={'A'};
    char recv_buffer[MQUEUE_MAX_SIZE] ={'A'};
    struct dbg_msg *send_pdmsg = (struct dbg_msg *)&send_buffer;
    struct timespec timeout = {2,0};

    unsigned long nloop = 0;
    
    struct timeval start, end;
    unsigned long send_bytes = 0;

    memset(send_buffer, 'A', sizeof(send_buffer));
    
    send_pdmsg->msg_seq = MSGQ_TYPE;
    send_pdmsg->msg_ack = 0;
    send_pdmsg->msg_len = sizeof(send_buffer);
    send_pdmsg->crc32 = crc32(send_pdmsg->data, send_pdmsg->msg_len-offsetof(struct dbg_msg, data));
    
    gettimeofday(&start, NULL);
    while(++nloop <= TEST_NLOOP) {
        
        ret = send_fn((char*)send_pdmsg, send_pdmsg->msg_len);
        if(ret != 0) {
            continue;
        } else {
            send_bytes += send_pdmsg->msg_len;
            
            send_pdmsg->msg_ack++;
        }
    }
    gettimeofday(&end, NULL);
    printf_rate("TX", send_bytes, diff_timeval_usec(&end, &start));
    
}


void *task1_fn(void*arg) 
{
    test_mqueue_send(__hook_mq_send);
    test_mqueue_send(__hook_msgsnd);
    pthread_exit(NULL);
}

void test_mqueue_recv(recv_hook_fn recv_fn)
{
    int ret;
    ssize_t n;
    char buffer[MQUEUE_MAX_SIZE] ={0};
    
    struct dbg_msg *pdmsg;
    unsigned long nloop = 0;
    struct timeval start, end;
    unsigned long recv_bytes = 0;
    
    gettimeofday(&start, NULL);

    while(++nloop <= TEST_NLOOP) {
        n = recv_fn(buffer, MQUEUE_MAX_SIZE);
        if(n <= 0) {
            continue;
        } else {
            recv_bytes += n;
            pdmsg = (struct dbg_msg*)buffer;
//            printf("T2(%d/%ld): recv seq(%ld), ack(%ld), len(%ld)\n", \
//                        n, recv_bytes, pdmsg->msg_seq, pdmsg->msg_ack, pdmsg->msg_len);
            if(pdmsg->crc32 != crc32(pdmsg->data, pdmsg->msg_len-offsetof(struct dbg_msg, data))) {
                printf("crc32 error: %x vs %x\n", pdmsg->crc32, 
                            crc32(pdmsg->data, pdmsg->msg_len-offsetof(struct dbg_msg, data)));
            }
            pdmsg->msg_ack++;
        }
    }
    gettimeofday(&end, NULL);
    
    printf_rate("RX", recv_bytes, diff_timeval_usec(&end, &start));

}

void *task2_fn(void*arg) 
{
    test_mqueue_recv(__hook_mq_receive);
    test_mqueue_recv(__hook_msgrcv);
    pthread_exit(NULL);
}

void msgq_init()
{
	key_t key;

	/*获取key值*/
	if((key = ftok(MSG_FILE, 123)) < 0)
	{
		perror("ftok error");
		exit(1);
	}
	
	/*打印key值*/
	printf("Message Queue - Client key is: %d\n", key);
	
	/*打印队列消息*/
	if((msqid = msgget(key, IPC_CREAT|0777)) == -1)
	{
		perror("msgget error");
		exit(1);
	}
}

void mqueue_init()
{
    int ret;
    struct mq_attr attr;
    
    ret = mq_unlink(MQUEUE_NAME);

    attr.mq_flags = 0;
    attr.mq_msgsize = MQUEUE_MAX_SIZE;

    attr.mq_maxmsg = MQUEUE_MAX_MSG;
    mqd = mq_open(MQUEUE_NAME, O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO, &attr);
//    printf("Init mqeue, mqd =  %d, %s\n", mqd, strerror(errno));


}

void task_init()
{
    pthread_create(&task1, NULL, task1_fn, NULL);
    pthread_create(&task2, NULL, task2_fn, NULL);
}

void task_join()
{
    pthread_join(task1, NULL);
    pthread_join(task2, NULL);
}

int main()
{
    mqueue_init();
    msgq_init();
    task_init();
    task_join();
}


