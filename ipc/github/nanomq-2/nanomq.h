#ifndef __nAnoMQ_H
#define __nAnoMQ_H 1

#include <stdbool.h>


#if (!defined(__i386__) && !defined(__x86_64__))
# error Unsupported CPU
#endif

#ifndef likely
#define likely(x)    __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
#define unlikely(x)  __builtin_expect(!!(x), 0)
#endif

// Forced inlining 
#ifndef force_inline
#define force_inline __attribute__ ((__always_inline__))
#endif

// POD for header data
struct nmq_header;

// POD for nmq_ring
struct nmq_ring;

typedef struct nmq_context {
#define CTX_FNAME_LEN   512

    char fname_[CTX_FNAME_LEN];
    void *p_;
    struct nmq_header *header_;
    struct nmq_ring *ring_;
    char *data_;
  
};

typedef struct nmq_node {
    struct nmq_context *context_;
    unsigned int node_;
};

force_inline void ctx_init(struct nmq_context *self, const char *fname);
force_inline bool ctx_create(struct nmq_context *self, unsigned int nodes, unsigned int size, unsigned int msg_size);
force_inline bool ctx_open(struct nmq_context *self, unsigned int nodes, unsigned int size, unsigned int msg_size);
force_inline void ctx_print(struct nmq_context *self);
force_inline bool ctx_sendto(struct nmq_context *self, unsigned int from, unsigned int to, const void *msg, size_t size);
force_inline bool ctx_sendnb(struct nmq_context *self, unsigned int from, unsigned int to, const void *msg, size_t size);
force_inline bool ctx_recvfrom(struct nmq_context *self, unsigned int from, unsigned int to, void *msg, size_t *size);
force_inline bool ctx_recvnb(struct nmq_context *self, unsigned int from, unsigned int to, void *s, size_t *size);

force_inline void node_init(struct nmq_node *self, struct nmq_context *context, unsigned int node);
force_inline bool node_send(struct nmq_node *self, unsigned int to, const void *msg, size_t size);
force_inline bool node_sendnb(struct nmq_node *self, unsigned int to, const void *msg, size_t size);
force_inline  bool node_recv(struct nmq_node *self, unsigned int from, void *msg, size_t *size);
force_inline  bool node_recvnb(struct nmq_node *self, unsigned int from, void *msg, size_t *size);






#endif /*<__nAnoMQ_H>*/
