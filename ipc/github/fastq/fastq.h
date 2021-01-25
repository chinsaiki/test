/********************************************************************\
*  文件： fastq.h
*  介绍： 低时延队列
*  作者： 荣涛
*  日期：
*       2021年1月25日    
\********************************************************************/

#ifndef __nAnoMQ_H
#define __nAnoMQ_H 1

#include <stdbool.h>



// Forced inlining 
#ifndef always_inline
#define always_inline __attribute__ ((__always_inline__))
#endif


// POD for header data
struct fastq_header;

// POD for fastq_ring
struct fastq_ring;

/**
 *  队列上下文
 */
typedef struct fastq_context {
#define CTX_FNAME_LEN   512
    void *p_;
    struct fastq_header *header_;
    struct fastq_ring *ring_;
    char *data_;
};






always_inline bool fastq_create(struct fastq_context *self, unsigned int nodes, unsigned int size, unsigned int msg_size);
always_inline void fastq_ctx_print(FILE*fp, struct fastq_context *self);

always_inline bool fastq_sendto(struct fastq_context *self, unsigned int from, unsigned int to, const void *msg, size_t size);
always_inline bool fastq_sendnb(struct fastq_context *self, unsigned int from, unsigned int to, const void *msg, size_t size);
always_inline bool fastq_recvfrom(struct fastq_context *self, unsigned int from, unsigned int to, void *msg, size_t *size);
always_inline bool fastq_recvnb(struct fastq_context *self, unsigned int from, unsigned int to, void *s, size_t *size);



#endif /*<__nAnoMQ_H>*/
