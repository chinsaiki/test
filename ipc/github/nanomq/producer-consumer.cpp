/*
    Copyright (C) 2010 Erik Rigtorp <erik@rigtorp.com>. 
    All rights reserved.

    This file is part of NanoMQ.

    NanoMQ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    NanoMQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NanoMQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>
#include <nmq.hpp>


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>


void *enqueue_task(void*arg){
    nmq::node_t *node = (nmq::node_t *)arg;
    int i =0;
    while(1) {
        node->send(1, "test", 5);
        sleep(1);
    }
    pthread_exit(NULL);
}

void *dequeue_task(void*arg){
    nmq::node_t *node = (nmq::node_t *)arg;

    int i =0;
    char buf[100];
    size_t sz = 100;
    while(1) {
        node->recv(0, &buf, &sz);
        printf("recv %s\n", buf);
    }
    pthread_exit(NULL);
}



int main()
{
    pthread_t enqueue_taskid, dequeue_taskid;
    
    // Open context
    char *fname = tempnam(NULL, "nmq"); // UGLY
    nmq::context_t context(fname);
    context.create(4, 10, 100);
    
    nmq::node_t node0(context, 0);
    nmq::node_t node1(context, 1);

    pthread_create(&enqueue_taskid, NULL, enqueue_task, &node0);
    pthread_create(&dequeue_taskid, NULL, dequeue_task, &node1);

    pthread_join(enqueue_taskid, NULL);
    pthread_join(dequeue_taskid, NULL);

    return EXIT_SUCCESS;
}

