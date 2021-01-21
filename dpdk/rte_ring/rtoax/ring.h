/**
 *  ring.h
 *  
 */


#ifndef __RING_H 
#define __RING_H 1

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

struct ring_struct {
    atomic64_t count_enqueue;
    atomic64_t count_dequeue;
    atomic64_t stat;

    atomic64_t head;
    atomic64_t tail;
    
    unsigned long elem_nr;
    struct ring_element elems[];
}__attribute__((aligned(64)));


int ring_init(struct ring_struct *ring, size_t elem_num);

struct ring_struct *ring_create(size_t elem_num);

int ring_enqueue(struct ring_struct *ring, void* addr);
int ring_dequeue(struct ring_struct *ring, void **data);


#endif /*<__RING_H>*/
