/**
 *  cas-queue.h
 *  
 *  提供简单低时延的 基于 CAS 操作的队列
 *  
 *  荣涛  2021年1月13日
 *  荣涛  2021年1月14日    添加枚举类型
 *  
 */
#ifndef __CAS_QUEUE_H
#define __CAS_QUEUE_H 1

#ifndef __x86_64__
# error "Not support Your Arch, just support x86-64"
#endif

#include <errno.h>
#include <assert.h>
#include <stdbool.h>
#include "bitset.h"

#define RDTSC() ({\
    register uint32_t a,d; \
    __asm__ __volatile__( "rdtsc" : "=a"(a), "=d"(d)); \
    (((uint64_t)a)+(((uint64_t)d)<<32)); \
    })

#define CAS(ptr, val_old, val_new) ({ \
    char ret; \
    __asm__ __volatile__("lock; "\
        "cmpxchgl %2,%0; setz %1"\
        : "+m"(*ptr), "=q"(ret)\
        : "r"(val_new),"a"(val_old)\
        : "memory"); \
    ret;})


#ifndef UNUSED
#define UNUSED __attribute__((unused))
#endif
#ifndef likely
#define likely(exp) __builtin_expect(!!exp, 1)
#endif
#ifndef unlikely
#define unlikely(exp) __builtin_expect(!!exp, 0)
#endif

#define PAGE_SIZE 4096
#define CACHE_LINE_SIZE 64
#define CACHE_ALIGNED __attribute__((aligned(CACHE_LINE_SIZE)))
#define DOUBLE_CACHE_ALIGNED __attribute__((aligned(2 * CACHE_LINE_SIZE)))


typedef enum  {
    READY_TO_CTOR = 1,  /* 准备填入参数 */
    CTORING = 1,        /* 正在填入参数 */
    DONE_TO_CTOR,       /* 填写完参数 */
    READY_TO_ENQUEUE,   /* 准备入队 */
    ENQUEUING,          /* 正在入队 */
    READY_TO_DEQUEUE,   /* 准备出队 */
    DEQUEUING,          /* 正在出队 */
}QUEUE_OPS;



typedef struct fast_queue_s {
    unsigned long que_id;
    int bitmap_maxbits;
    bits_set CACHE_ALIGNED bitmap_active;
    void    *data[BITS_SETSIZE];

#define FAST_QUEUE_INITIALIZER  {READY_TO_ENQUEUE, -1, BITS_SET_INITIALIZER, {NULL}}

}CACHE_ALIGNED fast_queue_t;


static inline int UNUSED fast_queue_init(fast_queue_t *queue) {

    int i;
    if(unlikely(!queue)) {
        fprintf(stderr, "NULL pointer error\n");
        return -EINVAL;
    }

    memset(queue, 0, sizeof(fast_queue_t));
    queue->que_id = READY_TO_CTOR;
    queue->bitmap_maxbits = -1;

    return 0;
}

static inline fast_queue_t *UNUSED fast_queue_create()
{
    fast_queue_t* queue = malloc(sizeof(fast_queue_t));
    assert(queue);

    fast_queue_init(queue);

    return queue;
}

static inline int UNUSED fast_queue_destroy(fast_queue_t *queue)
{
    if(unlikely(!queue)) {
        fprintf(stderr, "NULL pointer error\n");
        return -EINVAL;
    }
    free(queue);
    
    return 0;
}

#define fast_queue_can_add(queue) \
    CAS(&queue->que_id, READY_TO_CTOR, READY_TO_CTOR)

static inline int UNUSED fast_queue_add_msgs(fast_queue_t *queue, int idx, void *msg) {
    
    if(unlikely(!queue) || unlikely(!msg)) {
        fprintf(stderr, "NULL pointer error\n");
        return -EINVAL;
    }
    if(unlikely(idx<0) || unlikely(idx>BITS_SETSIZE)) {
        fprintf(stderr, "Message index must between %d and %d\n", 0, BITS_SETSIZE-1);
        return -EINVAL;
    }
    
    while(1) {
        if(CAS(&queue->que_id, READY_TO_CTOR, CTORING)) {
            if(BITS_ISSET(idx, &queue->bitmap_active)) {

                fprintf(stderr, "Already set slot %d\n", idx);

                while(!CAS(&queue->que_id, CTORING, READY_TO_CTOR));
                break;
            }
            if(queue->bitmap_maxbits < idx) {
                queue->bitmap_maxbits = idx;
            }
            
            queue->data[idx] = (void*)msg;
            BITS_SET(idx, &queue->bitmap_active);

            while(!CAS(&queue->que_id, CTORING, READY_TO_CTOR));
            break;
        }
    }
    return 0;
}

static inline int UNUSED fast_queue_add_msgs_done(fast_queue_t *queue) {
    while(!CAS(&queue->que_id, READY_TO_CTOR, READY_TO_ENQUEUE));
}

static inline int UNUSED fast_queue_send(fast_queue_t *queue) {
    while(!CAS(&queue->que_id, READY_TO_ENQUEUE, READY_TO_DEQUEUE));
}

static inline int UNUSED fast_queue_recv(fast_queue_t *queue, int (*msg_handler)(void *msg)) {
    
    if(unlikely(!queue)) {
        fprintf(stderr, "NULL pointer error\n");
        return -EINVAL;
    }
    int i = 0, nr_msg = 0;
    if(CAS(&queue->que_id, READY_TO_DEQUEUE, DEQUEUING)) {
        for(i=0; i<= queue->bitmap_maxbits; i++) {
            if(BITS_ISSET(i, &queue->bitmap_active)) {
                msg_handler(queue->data[i]);
                BITS_CLR(i, &queue->bitmap_active);
                nr_msg++;
            }
        }
        queue->bitmap_maxbits = -1;
        CAS(&queue->que_id, DEQUEUING, READY_TO_CTOR);            
    }
    return nr_msg;
}


#endif /*<__CAS_QUEUE_H>*/
