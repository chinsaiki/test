/**
 *  ring.h
 *  
 */


#ifndef __RING_H 
#define __RING_H 1

#include <stdio.h>

#include "atomic.h"


enum {
    RING_IS_OK,
    RING_IS_ERR,
    RING_IS_FULL,
    RING_IS_EMPTY,
};


struct ring_element {
    void *data;
};
#define cachelinealigned __attribute__((aligned(64)))
struct ring_struct {
    char name[64];
#define RING_DEFAULT_NAME   "RING"    
    atomic64_t cachelinealigned count_enqueue;
    atomic64_t cachelinealigned count_dequeue;
    atomic64_t cachelinealigned stat;

    atomic64_t cachelinealigned head;
    atomic64_t cachelinealigned tail;
    
    unsigned long cachelinealigned elem_nr;
#define RING_DEFAULT_SIZE   1024    

    struct ring_element elems[];
}cachelinealigned;


int ring_init(struct ring_struct *ring, const char *name, size_t elem_num);

struct ring_struct *ring_create(const char *name, size_t elem_num);
int ring_destroy(struct ring_struct *ring);

int ring_try_enqueue(struct ring_struct *ring, void* addr);
int ring_force_enqueue(struct ring_struct *ring, void* addr);

int ring_try_dequeue(struct ring_struct *ring, void **data);
int ring_force_dequeue(struct ring_struct *ring, void **data);

int ring_dump(FILE *fp, struct ring_struct *ring);


#endif /*<__RING_H>*/
