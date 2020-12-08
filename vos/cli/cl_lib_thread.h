/*
* Thread management routine header.
* Copyright (C) 1998 Kunihiro Ishiguro
*
* This file is part of GNU cl_lib.
*
* GNU cl_lib is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2, or (at your option) any
* later version.
*
* GNU cl_lib is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with GNU cl_lib; see the file COPYING.  If not, write to the Free
* Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
* 02111-1307, USA.  
*/

#ifndef _cl_lib_THREAD_H
#define _cl_lib_THREAD_H

#include <sys/select.h>

/* Linked list of thread. */
struct cl_lib_thread_list
{
    struct cl_lib_thread *head;
    struct cl_lib_thread *tail;
    int count;
};

/* Master of the theads. */
struct cl_lib_thread_master
{
    struct cl_lib_thread_list read;
    struct cl_lib_thread_list write;
    /*  struct cl_lib_thread_list timer;*/
    struct cl_lib_avl_tree *timer_tree;

    struct cl_lib_thread_list event;
    struct cl_lib_thread_list ready;
    struct cl_lib_thread_list unuse;

     /*������Ա�Ķ����ڴ�֮ǰ����Ϊvxworks��linux��fd_set���ܲ���һ��*/
    unsigned long alloc;
    /* kill telent or telnet relay */
    sem_t ulSemKill;
    /*  telnet or console exit*/
    int exit_flag;
    
    fd_set readfd;
    fd_set writefd;
    fd_set exceptfd;
    
};

/* Thread itself. */
struct cl_lib_thread
{
    unsigned long id;
    unsigned char type;		/* thread type */
    struct cl_lib_thread *next;		/* next pointer of the thread */
    struct cl_lib_thread *prev;		/* previous pointer of the thread */
    struct cl_lib_thread_master *master;	/* pointer to the struct cl_lib_thread_master. */
    int ( *func ) ( struct cl_lib_thread * ); /* event function */
    void *arg;			/* event argument */
    union {
        int val;			/* second argument of the event. */
        unsigned int fd;			/* file descriptor in case of read/write. */
        struct timeval sands;	/* rest of time sands value. */
    } u;
};

/* Macros. */
#define cl_lib_THREAD_ARG(X) ((X)->arg)
#define cl_lib_THREAD_FD(X)  ((X)->u.fd)
#define cl_lib_THREAD_VAL(X) ((X)->u.val)

/* Thread types. */
#define cl_lib_THREAD_READ  0
#define cl_lib_THREAD_WRITE 1
#define cl_lib_THREAD_TIMER 2
#define cl_lib_THREAD_EVENT 3
#define cl_lib_THREAD_READY 4
#define cl_lib_THREAD_UNUSED 5

/* Prototypes. */
struct cl_lib_thread_master *cl_lib_thread_make_master ();

struct cl_lib_thread *
            cl_lib_thread_add_read ( struct cl_lib_thread_master *m,
                              int ( *func ) ( struct cl_lib_thread * ),
                              void *arg,
                              int fd );

struct cl_lib_thread *
            cl_lib_thread_add_write ( struct cl_lib_thread_master *m,
                               int ( *func ) ( struct cl_lib_thread * ),
                               void *arg,
                               int fd );

struct cl_lib_thread *
            cl_lib_thread_add_timer ( struct cl_lib_thread_master *m,
                               int ( *func ) ( struct cl_lib_thread * ),
                               void *arg,
                               long timer );

struct cl_lib_thread *
            ospf_thread_add_timer ( struct cl_lib_thread_master *m,
                                    int ( *func ) ( struct cl_lib_thread * ),
                                    void *arg,
                                    long timer );


struct cl_lib_thread *
            cl_lib_thread_add_event ( struct cl_lib_thread_master *m,
                               int ( *func ) ( struct cl_lib_thread * ),
                               void *arg,
                               int val );

void
cl_lib_thread_cancel ( struct cl_lib_thread *thread );

void
cl_lib_thread_cancel_event ( struct cl_lib_thread_master *m, void *arg );

struct cl_lib_thread *
            cl_lib_thread_fetch ( struct cl_lib_thread_master *m,
                           struct cl_lib_thread *fetch );

/*
struct cl_lib_thread *
            cl_lib_thread_fetch_set_priority ( struct cl_lib_thread_master *m,
                           struct cl_lib_thread *fetch , int MyTaskPriority);
*/

void
cl_lib_thread_call ( struct cl_lib_thread *thread );

/*
struct cl_lib_thread *
            cl_lib_thread_execute ( struct cl_lib_thread_master *m,
                             int ( *func ) ( struct cl_lib_thread * ),
                             void *arg,
                             int val );
*/                             
int
cl_lib_thread_timer_cmp ( struct timeval a, struct timeval b );

void
cl_lib_thread_list_add ( struct cl_lib_thread_list *list, struct cl_lib_thread *thread );

struct cl_lib_thread *
            cl_lib_thread_list_delete ( struct cl_lib_thread_list *list, struct cl_lib_thread *thread );

void
cl_lib_thread_add_unuse ( struct cl_lib_thread_master *m, struct cl_lib_thread *thread );

struct cl_lib_thread *
            cl_lib_thread_trim_head ( struct cl_lib_thread_list *list );


struct cl_lib_avl_tree* cl_lib_avl_thread_create();
int cl_lib_avl_thread_timer_cmp( void *val1, void *val2 );
struct cl_lib_thread *cl_lib_avl_thread_insert( struct cl_lib_avl_tree *tree, struct cl_lib_thread* thread );
struct cl_lib_thread *cl_lib_avl_thread_delete( struct cl_lib_avl_tree *tree, struct cl_lib_thread* thread );
struct timeval *cl_lib_avl_get_min_thread_key( struct cl_lib_avl_tree *tree );
struct cl_lib_thread_list *cl_lib_avl_get_thread_list( struct cl_lib_avl_tree *tree, struct timeval* timeval );
struct timeval *cl_lib_avl_get_root_thread_key( struct cl_lib_avl_tree *tree );
void cl_lib_avl_thread_destroy( struct cl_lib_avl_tree *tree );
/*
int cl_lib_avl_get_thread_count( struct cl_lib_avl_tree *tree );
void cl_lib_avl_thread_destroy_all( struct cl_lib_avl_tree *tree );
void cl_lib_avl_thread_traverse( struct cl_lib_avl_tree *tree );
*/
void cl_lib_thread_destroy_master ( struct cl_lib_thread_master *m );
int ioctl_realSystem(int fd, int function, void * arg );
int cl_lib_console_select_fetch ( int sock, unsigned int timeout);
#endif /* _cl_lib_THREAD_H */

