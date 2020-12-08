/* Thread management routine
* Copyright (C) 1998, 2000 Kunihiro Ishiguro <kunihiro@cl_lib.org>
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
/*history:
          time               man                               action
--------------------------------------------------------------------------------------------------------------------
        2002-9-26         yuanfengfeng              delete #include "vos_semex.h"
*/
#include <sys/ioctl.h>

#include "config.h"

#define _REAL_SYSTEM_STACK_ 1

#define _IPSTACK_SYS_H_

#include "cl_lib_thread.h"
#include "cl_lib_avl.h"



//#define DEBUG

/* get the number of thread in avl thread tree ,used when debug */
int cl_lib_avl_get_thread_count( struct cl_lib_avl_tree *tree )
{
    struct cl_lib_avl_node *stack[ 32 ];
    int count;
    int thread_count = 0;
    struct cl_lib_avl_node *node;
    struct cl_lib_thread_list *threadlist;

    if ( tree == NULL )
    {
        VOS_ASSERT( 0 );
        return 0;
    }
    count = 0;
    node = tree->root;
    for ( ;; )
    {
        while ( node != NULL )
        {
            stack[ count++ ] = node;
            node = node->link[ 0 ];
        }
        if ( count == 0 )
            return thread_count;
        node = stack[ --count ];
        threadlist = ( struct cl_lib_thread_list * ) node->data;
        thread_count += threadlist->count;
        node = node->link[ 1 ];
    }
}


/* List allocation and head/tail print out. */
void cl_lib_thread_list_debug ( struct cl_lib_thread_list *list )
{
    VOS_Printf ( "count [%d] head [%p] tail [%p]\n",
             list->count, list->head, list->tail );
}

void cl_lib_thread_tree_debug( struct cl_lib_avl_tree *tree )
{
    VOS_Printf( "node-count [%d] thread-count [%d] tree-root [%p]\n",
            tree->count, cl_lib_avl_get_thread_count( tree ), tree->root );
}


/* Debug print for cl_lib_thread_master. */
void cl_lib_thread_master_debug ( struct cl_lib_thread_master *m )
{
    VOS_Printf ( "-----------\n" );
    VOS_Printf ( "readlist  : " );
    cl_lib_thread_list_debug ( &m->read );
    VOS_Printf ( "writelist : " );
    cl_lib_thread_list_debug ( &m->write );

    VOS_Printf( "timertree : " );
    cl_lib_thread_tree_debug( m->timer_tree );

    /*
    VOS_Printf ("timerlist : ");
    cl_lib_thread_list_debug (&m->timer);
    */
    VOS_Printf ( "eventlist : " );
    cl_lib_thread_list_debug ( &m->event );
    VOS_Printf ( "unuselist : " );
    cl_lib_thread_list_debug ( &m->unuse );
    VOS_Printf ( "total alloc: [%ld]\n", m->alloc );
    VOS_Printf ( "-----------\n" );
}


/* Thread types. */
#define cl_lib_THREAD_READ  0
#define cl_lib_THREAD_WRITE 1
#define cl_lib_THREAD_TIMER 2
#define cl_lib_THREAD_EVENT 3
#define cl_lib_THREAD_READY 4
#define cl_lib_THREAD_UNUSED 5

/*lint -save -e115*/
/*lint -save -e502*/

int ioctl_realSystem(int fd, int function, void *arg )
{
    #if (OS_LINUX)
         return ioctl( fd , function , arg );
    #else
        return 0 ;
    #endif
}

/* Make thread master. */
struct cl_lib_thread_master *cl_lib_thread_make_master ()
{
    struct cl_lib_thread_master *new;

    new = VOS_Malloc ( sizeof ( struct cl_lib_thread_master ), MODULE_RPU_CLI );
    if ( !new )
    {
        return NULL;
    }
    VOS_MemZero ( ( char * ) new,  sizeof ( struct cl_lib_thread_master ) );
    new->timer_tree = cl_lib_avl_thread_create();

    /*  kill telent or telnet relay */
//    new->ulSemKill = VOS_SemMCreate(VOS_SEM_Q_FIFO);
    sem_init(&new->ulSemKill, 0, 1);
//    if(new->ulSemKill == 0)
//    {
//        VOS_ASSERT(0);
//        cl_lib_avl_thread_destroy( new->timer_tree );
//        VOS_Free(new);
//        return NULL;
//    }
    /* end */

    return new;
}


/* Add a new thread to the list. */
void cl_lib_thread_list_add ( struct cl_lib_thread_list *list, struct cl_lib_thread *thread )
{
    thread->next = NULL;
    thread->prev = list->tail;
    if ( list->tail )
    {
        list->tail->next = thread;
    }
    else
    {
        list->head = thread;
    }
    list->tail = thread;
    list->count++;
}

/* Delete a thread from the list. */
struct cl_lib_thread *cl_lib_thread_list_delete ( struct cl_lib_thread_list *list, struct cl_lib_thread *thread )
{
    if ( thread->next )
    {
        thread->next->prev = thread->prev;
    }
    else
    {
        list->tail = thread->prev;
    }
    if ( thread->prev )
    {
        thread->prev->next = thread->next;
    }
    else
    {
        list->head = thread->next;
    }
    thread->next = thread->prev = NULL;
    list->count--;
    return thread;
}

/* Free all unused thread. */
static void cl_lib_thread_clean_unuse ( struct cl_lib_thread_master *m )
{
    struct cl_lib_thread *thread;

    thread = m->unuse.head;
    while ( thread )
    {
        struct cl_lib_thread * t;

        t = thread;
        thread = t->next;

        cl_lib_thread_list_delete ( &m->unuse, t );
        VOS_Free( t );
        m->alloc--;
    }
}

/* Move thread to unuse list. */
void cl_lib_thread_add_unuse ( struct cl_lib_thread_master *m, struct cl_lib_thread *thread )
{
    VOS_ASSERT ( m != NULL );
    VOS_ASSERT ( thread->next == NULL );
    VOS_ASSERT ( thread->prev == NULL );
    VOS_ASSERT ( thread->type == cl_lib_THREAD_UNUSED );

    cl_lib_thread_list_add ( &m->unuse, thread );
}

/* Stop thread scheduler. */
void cl_lib_thread_destroy_master ( struct cl_lib_thread_master *m )
{
    struct cl_lib_thread *thread;

    struct cl_lib_thread_list* threadlist;
    struct timeval *mintime;

    thread = m->read.head;
    while ( thread )
    {
        struct cl_lib_thread * t;

        t = thread;
        thread = t->next;

        cl_lib_thread_list_delete ( &m->read, t );
        t->type = cl_lib_THREAD_UNUSED;
        cl_lib_thread_add_unuse ( m, t );
    }

    thread = m->write.head;
    while ( thread )
    {
        struct cl_lib_thread * t;

        t = thread;
        thread = t->next;

        cl_lib_thread_list_delete ( &m->write, t );
        t->type = cl_lib_THREAD_UNUSED;
        cl_lib_thread_add_unuse ( m, t );
    }

    /*
    thread = m->timer.head;
    while (thread)
    {
        struct cl_lib_thread *t;

        t = thread;
        thread = t->next;

        cl_lib_thread_list_delete (&m->timer, t);
        t->type = cl_lib_THREAD_UNUSED;
        cl_lib_thread_add_unuse (m, t);
    }
    */

    while ( m->timer_tree->root )
    {
        mintime = cl_lib_avl_get_root_thread_key( m->timer_tree );
        threadlist = cl_lib_avl_get_thread_list( m->timer_tree, mintime );
        thread = threadlist->head;

        while ( thread )
        {
            struct cl_lib_thread * t;
            t = thread;
            thread = t->next;
            cl_lib_thread_list_delete( threadlist, t );
            t->type = cl_lib_THREAD_UNUSED;
            cl_lib_thread_list_add ( &m->unuse, t );
            /* alloc_dec (MTYPE_TIMER_THREAD);*/
        }

        VOS_Free( threadlist );
    }
    cl_lib_avl_thread_destroy( m->timer_tree );

    thread = m->event.head;
    while ( thread )
    {
        struct cl_lib_thread * t;

        t = thread;
        thread = t->next;

        cl_lib_thread_list_delete ( &m->event, t );
        t->type = cl_lib_THREAD_UNUSED;
        cl_lib_thread_add_unuse ( m, t );
    }

    thread = m->ready.head;
    while ( thread )
    {
        struct cl_lib_thread * t;

        t = thread;
        thread = t->next;

        cl_lib_thread_list_delete ( &m->ready, t );
        t->type = cl_lib_THREAD_UNUSED;
        cl_lib_thread_add_unuse ( m, t );
    }

    cl_lib_thread_clean_unuse ( m );
    /*  kill telent or telnet relay  */
//    if(m->ulSemKill != 0)
        {
////        VOS_SemDelete(m->ulSemKill);
        sem_destroy(&m->ulSemKill);

//        m->ulSemKill = 0;
        }
    /* end */

    VOS_Free( m );
}

/* Delete top of the list and return it. */
struct cl_lib_thread *cl_lib_thread_trim_head ( struct cl_lib_thread_list *list )
{
    if ( list->head )
    {
        return cl_lib_thread_list_delete ( list, list->head );
    }
    return NULL;
}

/* Make new thread. */
struct cl_lib_thread *cl_lib_thread_new ( struct cl_lib_thread_master *m )
{
    struct cl_lib_thread *new;

    if ( m->unuse.head )
    {
        return ( cl_lib_thread_trim_head ( &m->unuse ) );
    }

    new = VOS_Malloc ( sizeof ( struct cl_lib_thread ), MODULE_RPU_CLI );
    if ( new )
    {
        VOS_MemZero ( ( char * ) new,  sizeof ( struct cl_lib_thread ) );
        m->alloc++;
    }
    return new;
}

/* Add new read thread. */
struct cl_lib_thread *cl_lib_thread_add_read ( struct cl_lib_thread_master *m,
                int ( *func ) ( struct cl_lib_thread * ),
                void *arg,
                int fd )
{
    struct cl_lib_thread *thread;

#ifdef DEBUG

    VOS_Printf ( "add read\n" );
#endif /* DEBUG */

    VOS_ASSERT ( m != NULL );

    if ( FD_ISSET ( (unsigned int)fd, &m->readfd ) )
    {
        #ifdef PLAT_MODULE_SYSLOG
        VOS_log (MODULE_RPU_VOS, SYSLOG_ERR, "There is already read fd [%d]", fd );
        #endif
        return NULL;
    }

    thread = cl_lib_thread_new ( m );
    if ( thread )
    {
        thread->type = cl_lib_THREAD_READ;
        thread->id = 0;
        thread->master = m;
        thread->func = func;
        thread->arg = arg;
#ifdef DEBUG

        VOS_Printf ( "fd added [%d]\n", fd );
#endif /* DEBUG */

        FD_SET ( (unsigned int)fd, &m->readfd );
        thread->u.fd = fd;
        cl_lib_thread_list_add ( &m->read, thread );
    }
    return thread;
}

/* Add new write thread. */
struct cl_lib_thread *cl_lib_thread_add_write ( struct cl_lib_thread_master *m,
                    int ( *func ) ( struct cl_lib_thread * ),
                    void *arg,
                    int fd )
{
    struct cl_lib_thread *thread;

#ifdef DEBUG

    VOS_Printf ( "add write\n" );
#endif /* DEBUG */

    VOS_ASSERT ( m != NULL );

    if ( FD_ISSET ( (unsigned int)fd, &m->writefd ) )
    {
        #ifdef PLAT_MODULE_SYSLOG
        VOS_log (MODULE_RPU_VOS, SYSLOG_ERR, "There is already write fd [%d]", fd );
        #endif
        return NULL;
    }

    thread = cl_lib_thread_new ( m );
    if ( thread )
    {
        thread->type = cl_lib_THREAD_WRITE;
        thread->id = 0;
        thread->master = m;
        thread->func = func;
        thread->arg = arg;
        FD_SET ( (unsigned int)fd, &m->writefd );
        thread->u.fd = fd;
        cl_lib_thread_list_add ( &m->write, thread );
    }
    return thread;
}

/* timer compare */
/*lint -save -e18*/
int cl_lib_thread_timer_cmp ( struct timeval a, struct timeval b )
{
    if ( a.tv_sec > b.tv_sec )
        return 1;
    if ( a.tv_sec < b.tv_sec )
        return -1;
    if ( a.tv_usec > b.tv_usec )
        return 1;
    if ( a.tv_usec < b.tv_usec )
        return -1;
    return 0;
}
/*lint -restore */
/* Add timer event thread. */

/*
struct cl_lib_thread *
cl_lib_thread_add_timer (struct cl_lib_thread_master *m,
		  int (*func)(struct cl_lib_thread *),
		  void *arg,
		  long timer)
{
  struct timeval timer_now;
  struct cl_lib_thread *thread;
  struct cl_lib_thread *tt;
 
#ifdef DEBUG
  VOS_Printf ("add timer\n");
#endif   
 
  VOS_ASSERT (m != NULL);
 
  thread = cl_lib_thread_new (m);
  thread->type = cl_lib_THREAD_TIMER;
  thread->id = 0;
  thread->master = m;
  thread->func = func;
  thread->arg = arg;
 
  cl_lib_thread_gettimeofday (&timer_now, NULL);
  timer_now.tv_sec += timer;
  thread->u.sands = timer_now;
 
 
    cl_lib_thread_list_add (&m->timer, thread);
 
  return thread;
}
*/

/* add timer event thread to avl tree. */
struct cl_lib_thread *cl_lib_thread_add_timer ( struct cl_lib_thread_master *m,
                    int ( *func ) ( struct cl_lib_thread * ),
                    void *arg,
                    long timer )
{
    struct timeval timer_now;
    struct cl_lib_thread *thread;

#ifdef DEBUG

    VOS_Printf ( "add timer\n" );
#endif /* DEBUG */

    VOS_ASSERT ( m != NULL );

    thread = cl_lib_thread_new ( m );
    if ( thread )
    {

        thread->type = cl_lib_THREAD_TIMER;
        thread->id = 0;
        thread->master = m;
        thread->func = func;
        thread->arg = arg;

        /* Do we need jitter here? */
        VOS_GetTimeOfDay ( &timer_now, NULL );
        timer_now.tv_sec += timer;
        thread->u.sands = timer_now;

        /* Sort by timeval. */
        /*  for (tt = m->timer.head; tt; tt = tt->next)
            if (cl_lib_thread_timer_cmp (thread->u.sands, tt->u.sands) <= 0)
              break;

          if (tt)
            cl_lib_thread_list_add_before (&m->timer, tt, thread);
          else
        */

        /*cl_lib_thread_list_add (&m->timer, thread);*/

        cl_lib_avl_thread_insert( m->timer_tree, thread );
    }
    return thread;
}

/* Add simple event thread. */
struct cl_lib_thread *cl_lib_thread_add_event ( struct cl_lib_thread_master *m,
                        int ( *func ) ( struct cl_lib_thread * ),
                        void *arg,
                        int val )
{
    struct cl_lib_thread *thread;

#ifdef DEBUG

    VOS_Printf ( "add event\n" );
#endif /* DEBUG */

    VOS_ASSERT ( m != NULL );

    thread = cl_lib_thread_new ( m );
    if ( thread )
    {
        thread->type = cl_lib_THREAD_EVENT;
        thread->id = 0;
        thread->master = m;
        thread->func = func;
        thread->arg = arg;
        thread->u.val = val;
        cl_lib_thread_list_add ( &m->event, thread );
    }
    return thread;
}


/* Cancel thread from scheduler. */
void cl_lib_thread_cancel ( struct cl_lib_thread *thread )
{
    /**/
    switch ( thread->type )
    {
        case cl_lib_THREAD_READ:

            #ifdef DEBUG
            VOS_Printf ( "cancel read\n" );
            #endif /* DEBUG */

            VOS_ASSERT ( FD_ISSET ( thread->u.fd, &thread->master->readfd ) );
            FD_CLR ( (thread->u.fd), &thread->master->readfd );
            cl_lib_thread_list_delete ( &thread->master->read, thread );
            break;
        case cl_lib_THREAD_WRITE:
            #ifdef DEBUG
            VOS_Printf ( "cancel write\n" );
            #endif /* DEBUG */

            VOS_ASSERT ( FD_ISSET ( thread->u.fd, &thread->master->writefd ) );
            FD_CLR ( (thread->u.fd), &thread->master->writefd );
            cl_lib_thread_list_delete ( &thread->master->write, thread );
            break;
        case cl_lib_THREAD_TIMER:
            #ifdef DEBUG
            VOS_Printf ( "cancel timer\n" );
            #endif /* DEBUG */


            /*  cl_lib_thread_list_delete (&thread->master->timer, thread); */
            cl_lib_avl_thread_delete( thread->master->timer_tree, thread );
            /*   alloc_dec (MTYPE_TIMER_THREAD);*/
            break;
        case cl_lib_THREAD_EVENT:
            #ifdef DEBUG
            VOS_Printf ( "cancel event\n" );
            #endif /* DEBUG */

            cl_lib_thread_list_delete ( &thread->master->event, thread );
            break;
        case cl_lib_THREAD_READY:
            #ifdef DEBUG
            VOS_Printf ( "cancel ready\n" );
            #endif /* DEBUG */

            cl_lib_thread_list_delete ( &thread->master->ready, thread );
            break;
        case cl_lib_THREAD_UNUSED:
            return ; 	   /* already set unused, ignore    gongzb 02/1/10 */
        default:
            VOS_ASSERT( 0 );   /* bad type, this is the only one place to call cl_lib_thread_add_unuse without calling cl_lib_thread_list_delete before    gongzb 02/1/10 */
            break;
    }
    thread->type = cl_lib_THREAD_UNUSED;
    cl_lib_thread_add_unuse ( thread->master, thread );

    #ifdef DEBUG
    cl_lib_thread_master_debug ( thread->master );
    #endif /* DEBUG */
}

/* Delete all events which has argument value arg. */
void cl_lib_thread_cancel_event ( struct cl_lib_thread_master *m, void *arg )
{
    struct cl_lib_thread *thread;

    thread = m->event.head;
    while ( thread )
    {
        struct cl_lib_thread * t;

        t = thread;
        thread = t->next;

        if ( t->arg == arg )
        {
            cl_lib_thread_list_delete ( &m->event, t );
            t->type = cl_lib_THREAD_UNUSED;
            cl_lib_thread_add_unuse ( m, t );
        }
    }

    thread = m->ready.head;
    while ( thread )
    {
        struct cl_lib_thread * t;

        t = thread;
        thread = t->next;

        if ( t->arg == arg )
        {
            cl_lib_thread_list_delete ( &m->ready, t );
            t->type = cl_lib_THREAD_UNUSED;
            cl_lib_thread_add_unuse ( m, t );
        }
    }

}

/* for struct timeval */
#define TIMER_SEC_MICRO 1000000

/* timer sub */
struct timeval cl_lib_thread_timer_sub ( struct timeval a, struct timeval b )
{
    struct timeval ret;

    ret.tv_usec = a.tv_usec - b.tv_usec;
    ret.tv_sec = a.tv_sec - b.tv_sec;

    if ( ret.tv_usec < 0 )
    {
        ret.tv_usec += TIMER_SEC_MICRO;
        ret.tv_sec--;
    }

    return ret;
}

/* For debug use. */
void cl_lib_thread_timer_dump ( struct timeval tv )
{
    VOS_Printf ( "Timer : %ld:%ld\n", ( long int ) tv.tv_sec, ( long int ) tv.tv_usec );
}

/********************************************************************
*   函数名: cl_lib_thread_fetch
*   
*   Fetch next ready thread.
*   
*   输入参数说明:
*   返回值说明:
*   comment 
*     Copyright(C), 2000-2006 GW TECHNOLOGIES CO.,LTD.
********************************************************************/
struct cl_lib_thread *cl_lib_thread_fetch ( struct cl_lib_thread_master *m,
                           struct cl_lib_thread *fetch )
{
    int ret;
    struct cl_lib_thread *thread;

    struct cl_lib_thread_list *threadlist = NULL;
    struct timeval *mintime;

    fd_set readfd;
    fd_set writefd;
    fd_set exceptfd;
    struct timeval timer_now;
    struct timeval timer_min;
    struct timeval *timer_wait;

    VOS_ASSERT ( m != NULL );

retry:    /* When thread can't fetch try to find next thread again. */

    if(m->exit_flag == 1)
    {
        return NULL;
    }
    /* If there is event process it first. */
    /* telent or telnet relay */
	//printf("\r\n ###11##%s.%d\r\n",__FUNCTION__,__LINE__);
    VOS_SemTake(m->ulSemKill, -1);
     /* end */
    while ( ( thread = cl_lib_thread_trim_head ( &m->event ) ) != NULL )
    {
        *fetch = *thread;
        thread->type = cl_lib_THREAD_UNUSED;
        cl_lib_thread_add_unuse ( m, thread );
		VOS_SemGive(m->ulSemKill);
        return fetch;
    }
    /*  kill telent or telnet relay */
    VOS_SemGive(m->ulSemKill);
     /* end */

    /* If there is ready threads process them */
    while ( ( thread = cl_lib_thread_trim_head ( &m->ready ) ) != NULL )
    {
        *fetch = *thread;
        thread->type = cl_lib_THREAD_UNUSED;
        cl_lib_thread_add_unuse ( m, thread );
        return fetch;
    }

    /* Calculate select wait timer. */

    if ( m->timer_tree->root )
    {
        mintime = cl_lib_avl_get_min_thread_key( m->timer_tree );
        if ( mintime )
        {
            VOS_GetTimeOfDay ( &timer_now, NULL );
            /*      VOS_ASSERT(mintime); by shenchen, this is checked. 2002 02*/
            timer_min = *mintime;
            timer_min = cl_lib_thread_timer_sub ( timer_min, timer_now );

            if ( timer_min.tv_sec < 0 )
            {
                timer_min.tv_sec = 0;
                timer_min.tv_usec = 10;
            }
            timer_wait = &timer_min;
            #ifdef DEBUG
            cl_lib_thread_timer_dump ( timer_min );
            #endif /* DEBUG */

        }
        else
            timer_wait = NULL;
    }
    else
    {
        #ifdef DEBUG
        VOS_Printf ( "timer_wait is NULL\n" );
        #endif /* DEBUG */

        timer_wait = NULL;
    }

    /* timer_wait is the time for next timer expire, or wait for ever */

    /* sometimes job is set by command, but our routing task is sleeping
    till socket select wakes up. so it's slow to send packets, etc.
    now, set it to wake up at least every 1 second.
    gongzb 01/11/8 */
    if ( timer_wait == NULL || timer_wait->tv_sec > 1 )
    {
        timer_min.tv_sec = 1;
        timer_min.tv_usec = 0;
        timer_wait = &timer_min;
    }

    /* Call select function. */
    readfd = m->readfd;
    writefd = m->writefd;
    exceptfd = m->exceptfd;


    #ifdef DEBUG
    {
        int i;
        VOS_Printf ( "readfd : " );
        for ( i = 0; i < FD_SETSIZE; i++ )
            if ( FD_ISSET ( i, &readfd ) )
                VOS_Printf ( "[%d] ", i );
        VOS_Printf ( "\n" );

    }
    {
        struct cl_lib_thread *t;

        VOS_Printf ( "readms : " );
        for ( t = m->read.head; t; t = t->next )
            VOS_Printf ( "[%d] ", t->u.fd );
        VOS_Printf ( "\n" );
    }
    #endif /* DEBUG */

    ret = select ( FD_SETSIZE, &readfd, &writefd, &exceptfd, &timer_min );

    #if 1
    if ( ret < 0 )
    {
        #if (OS_LINUX)
        /**********************************************************************
        Commented by Chen bin  2001.8.13
          *************************************************************************/
        if ( errno != EINTR )
        {
            /* Real error. */
            VOS_ASSERT ( 0 );
        }
        #endif

        /* Signal is coming. */
        goto retry;
    }
    #endif

		/* 在主任务中添加对命令行多任务的监视,包括串口的子任务和其他的telnet的子任务
		   有子任务死掉或者没有响应,就杀掉这个子任务,如果是串口就重新启动串口的描述符
		   用于再次登陆 by liuyanjun 2002/07/06 */

		cliSetTaskNoActiveAfterSelect();

		/* end */

    #ifdef DEBUG
    {
        int i;
        VOS_Printf ( "after select readfd : " );
        for ( i = 0; i < FD_SETSIZE; i++ )
            if ( FD_ISSET ( i, &readfd ) )
                VOS_Printf ( "[%d] ", i );
        VOS_Printf ( "\n" );
    }
    #endif /* DEBUG */

    /* Read thead. */
    thread = m->read.head;

    while ( thread )
    {
        struct cl_lib_thread * t;

        t = thread;
        thread = t->next;

        if ( FD_ISSET ( t->u.fd, &readfd ) )
        {
            VOS_ASSERT ( FD_ISSET ( t->u.fd, &m->readfd ) );
            FD_CLR( (t->u.fd), &m->readfd );
            cl_lib_thread_list_delete ( &m->read, t );
            cl_lib_thread_list_add ( &m->ready, t );
            t->type = cl_lib_THREAD_READY;
            #if 0
            {
                VOS_Printf ( "readfd %d", t->u.fd);VOS_Printf ( "\n" );
            }
            #endif
        }
    }

    #ifdef DEBUG
    {
        struct cl_lib_thread *t;

        VOS_Printf ( "readms : " );
        for ( t = m->read.head; t; t = t->next )
            VOS_Printf ( "[%d] ", t->u.fd );
        VOS_Printf ( "\n" );
    }
    #endif /* DEBUG */

    #if 1
    /* Write thead. */
    thread = m->write.head;
    while ( thread )
    {
        struct cl_lib_thread * t;

        t = thread;
        thread = t->next;

        if ( FD_ISSET ( t->u.fd, &writefd ) )
        {
            VOS_ASSERT ( FD_ISSET ( t->u.fd, &m->writefd ) );
            FD_CLR( (t->u.fd), &m->writefd );
            cl_lib_thread_list_delete ( &m->write, t );
            cl_lib_thread_list_add ( &m->ready, t );
            t->type = cl_lib_THREAD_READY;

            #if 0
            {
                VOS_Printf ( "write fd %d", t->u.fd);VOS_Printf ( "\n" );
            }
            #endif
        }
    }
    #endif

    /* Exception thead. */
    /*...*/

    /* Timer update. */

    /*
    cl_lib_thread_gettimeofday (&timer_now, NULL);
    thread = m->timer.head;
    while (thread)
{
    struct cl_lib_thread *t;

      t = thread;
      thread = t->next;
      
        if (cl_lib_thread_timer_cmp (timer_now, t->u.sands) >= 0)
        {
        cl_lib_thread_list_delete (&m->timer, t);
        cl_lib_thread_list_add (&m->ready, t);
        t->type = cl_lib_THREAD_READY;
        }
        }
    */

    VOS_GetTimeOfDay ( &timer_now, NULL );

    while ( ( mintime = cl_lib_avl_get_min_thread_key( m->timer_tree ) ) != NULL )
    {
        timer_min = *mintime;
        if ( cl_lib_thread_timer_cmp( timer_now, timer_min ) >= 0 )
        {
            threadlist = cl_lib_avl_get_thread_list( m->timer_tree, mintime );
            thread = threadlist->head;
            while ( thread )
            {
                struct cl_lib_thread * t;
                t = thread;
                thread = t->next;
                cl_lib_thread_list_delete( threadlist, t );
                t->type = cl_lib_THREAD_READY;
                cl_lib_thread_list_add ( &m->ready, t );
            }
            VOS_Free( threadlist );
        }
        else
            break;
    }

    /* Return one event. */
    thread = cl_lib_thread_trim_head ( &m->ready );

    /* There is no ready thread. */
    if ( !thread )
    	{

		/* 在主任务中添加对命令行多任务的监视,包括串口的子任务和其他的telnet的子任务
		   有子任务死掉或者没有响应,就杀掉这个子任务,如果是串口就重新启动串口的描述符
		   用于再次登陆 by liuyanjun 2002/07/06 */
		cliCheckSetActiveTaskAfterSelect();

		/* end */

        goto retry;
    	}

    *fetch = *thread;
    thread->type = cl_lib_THREAD_UNUSED;
    cl_lib_thread_add_unuse ( m, thread );

    #ifdef DEBUG
    cl_lib_thread_master_debug ( m );
    #endif /* DEBUG */

    return fetch;
}


/* Make unique thread id for non pthread version of thread manager. */
unsigned long int cl_lib_thread_get_id ()
{
    static unsigned long int counter = 0;
    return ++counter;
}

/* Call thread ! */
void cl_lib_thread_call ( struct cl_lib_thread *thread )
{
    thread->id = cl_lib_thread_get_id ();
    ( *thread->func ) ( thread );
}


/* create an avl tree,return tree */
struct cl_lib_avl_tree* cl_lib_avl_thread_create()
{
    struct cl_lib_avl_tree* tree = cl_lib_avl_create();

    if ( tree )
    {
        tree->cmp = cl_lib_avl_thread_timer_cmp;
    }
    return tree;
}

/*timer timeval compare ,val1 and val2 are pointer pointing to timeval */
int cl_lib_avl_thread_timer_cmp( void *val1, void *val2 )
{
    struct timeval * a = ( struct timeval* ) val1;
    struct timeval *b = ( struct timeval* ) val2;
    if ( a->tv_sec > b->tv_sec )
        return 1;
    if ( a->tv_sec < b->tv_sec )
        return -1;
    if ( a->tv_usec > b->tv_usec )
        return 1;
    if ( a->tv_usec < b->tv_usec )
        return -1;
    return 0;
}

/* create an new cl_lib_thread_list and initialize */
static struct cl_lib_thread_list* cl_lib_avl_thread_list_new()
{
    struct cl_lib_thread_list *threadlist = VOS_Malloc( sizeof( struct cl_lib_thread_list ), MODULE_RPU_CLI );
    if ( threadlist == NULL )
        return NULL;
    VOS_MemZero ( ( char * ) threadlist,  sizeof ( struct cl_lib_thread_list ) );
    return threadlist;
}

/* insert timer thread to avl thread tree */
struct cl_lib_thread *cl_lib_avl_thread_insert( struct cl_lib_avl_tree *tree, struct cl_lib_thread* thread )
{
    struct cl_lib_avl_node *avlnode = cl_lib_avl_insert( tree, &thread->u.sands );
    if ( avlnode )
    {
        struct cl_lib_thread_list * threadlist = ( struct cl_lib_thread_list * ) avlnode->data;
        if ( threadlist == NULL )
        {
            struct timeval * tiv = VOS_Malloc( sizeof( struct timeval ), MODULE_RPU_CLI );
            if( tiv == NULL)
            {
                return NULL;
            }
            VOS_MemZero ( ( char * ) tiv,  sizeof ( struct timeval ) );
            avlnode->data = threadlist = cl_lib_avl_thread_list_new();
            /* copy sands of thread to tiv */
            *tiv = thread->u.sands;
            avlnode->key = tiv;
        }
        cl_lib_thread_list_add( threadlist, thread );

        /* for debug */
        /*   	 if(conf_debug_thread_insert)
           	 	zlog_info("insert a timer thread %p to avl tree %p",thread,tree);
        */ 
        return thread;
    }
    else
        return NULL;
}

/* delete timer thread from avl thread tree */
struct cl_lib_thread *cl_lib_avl_thread_delete( struct cl_lib_avl_tree *tree, struct cl_lib_thread* thread )
{
    struct cl_lib_avl_node* avlnode = cl_lib_avl_search( tree, &thread->u.sands );
    if ( avlnode )
    {
        struct cl_lib_thread_list * threadlist = ( struct cl_lib_thread_list * ) avlnode->data;
        cl_lib_thread_list_delete( threadlist, thread );
        if ( threadlist->head == NULL )
        {
            struct timeval * ptime = ( struct timeval * ) avlnode->key;
            cl_lib_avl_delete( tree, &thread->u.sands );

            VOS_Free( ptime );
            VOS_Free( threadlist );
        }

        /* for debug */
        /*    if(conf_debug_thread_delete)
            	zlog_info("cancel a thread %p from avl thread tree %p",thread,tree);*/ 
        return thread;
    }
    return NULL;
}

/* get minimal timeval from avl thread tree */
/*lint -save -e18*/
struct timeval *cl_lib_avl_get_min_thread_key( struct cl_lib_avl_tree *tree )
{
    struct cl_lib_avl_node *avlnode = cl_lib_avl_get_min_node( tree );
    if ( avlnode )
        return ( struct timeval * ) avlnode->key;
    else
        return NULL;
}
/*lint -restore */

/* return cl_lib_thread_list that timeval specified by parameter */

/*lint -save -e18*/
struct cl_lib_thread_list *cl_lib_avl_get_thread_list( struct cl_lib_avl_tree *tree, struct timeval *timeval )
{
    struct cl_lib_avl_node *avlnode = cl_lib_avl_search( tree, timeval );
    struct cl_lib_thread_list *threadlist = ( struct cl_lib_thread_list * ) avlnode->data;
    struct timeval *ptime = ( struct timeval * ) avlnode->key;
    cl_lib_avl_delete( tree, timeval );

    VOS_Free( ptime );

    /* for debug */
    /*  if(conf_debug_thread_delete)
      {
        struct cl_lib_thread *thread=threadlist->head;
        while(thread)
        {
        zlog_info("delete timer thread %p from avl thread tree %p",thread,tree);
        zlog_info("add thread %p to ready thread list %p",thread,threadlist);
        thread=thread->next;
        }
      }
    */ 
    return threadlist;
}
/*lint -restore */

/* get tree root timeval from avl thread tree */
/*lint -save -e18*/
struct timeval *cl_lib_avl_get_root_thread_key( struct cl_lib_avl_tree *tree )
{
    if ( tree->root == NULL )
        return NULL;
    else
        return ( struct timeval * ) tree->root->key;
}
/*lint -restore */

/* destroy avl thread tree */
void cl_lib_avl_thread_destroy( struct cl_lib_avl_tree *tree )
{
    VOS_ASSERT( tree != NULL );

    VOS_Free( tree );
}


/********************************************************************
*   函数名: cl_lib_console_select_fetch
*   
*   Fetch the fd read.
*   
*   输入参数说明:
*   返回值说明:
*   0:成功的选中console口,串口上有数据可以读取
*   -1:超时
*     Copyright(C), 2000-2006 GW TECHNOLOGIES CO.,LTD.
********************************************************************/
int cl_lib_console_select_fetch ( int sock, unsigned int timeout)
{
    fd_set readfd;
    fd_set writefd;
    fd_set exceptfd;
    struct timeval timer_min;
    
#if 0
    struct timeval *timer_wait;
#endif

    int ret;
    ULONG ulTimeCount = 0;

nextTime:
    timer_min.tv_sec =1;
    timer_min.tv_usec = 0;

    #if 0
    timer_wait = &timer_min;
    #endif

    FD_ZERO(&readfd);
    FD_ZERO(&writefd);
    FD_ZERO(&exceptfd);
    FD_SET ( (unsigned int)sock, &readfd );

    ret = select ( FD_SETSIZE, &readfd, &writefd, &exceptfd, &timer_min );
    if ( ret < 0 )
        {
#if (OS_LINUX)
        if ( errno != EINTR )
            {
            ASSERT( 0 );
            }
#endif
        goto nextTime;
        }

    ulTimeCount++;
    if(timeout != 0)
        {
        if(ulTimeCount >= timeout)
            {
            return -1;
            }
        }
    if ( FD_ISSET ( (unsigned int)sock, &readfd ) )
        {
        ASSERT ( FD_ISSET ( (unsigned int)sock, &readfd ) );
        return 0;
        }
    
    goto nextTime;

}
/*lint -restore*/

/*lint -restore*/
#if 0



/* Prints all the nodes,the number of thread in node and data keys in TREE
   in in-order, using an iterative algorithm. */

void cl_lib_avl_thread_traverse( struct cl_lib_avl_tree *tree )
{
    struct cl_lib_avl_node *stack[ 32 ];
    int count;
    struct cl_lib_avl_node *node;
    struct cl_lib_thread_list *threadlist;
    struct timeval *ptime;

    VOS_SysLog( 0, 0, "tree %p contains thread:", tree );

    VOS_ASSERT( tree != NULL );
    count = 0;
    node = tree->root;
    for ( ;; )
    {
        while ( node != NULL )
        {
            stack[ count++ ] = node;
            node = node->link[ 0 ];
        }
        if ( count == 0 )
            return ;
        node = stack[ --count ];
        threadlist = ( struct cl_lib_thread_list* ) node->data;
        ptime = ( struct timeval* ) node->key;
        VOS_SysLog( 0, 0, "avl node %p threadlist->count %d nodekey: %ld sec %ld usec", node, threadlist->count,
                   ptime->tv_sec, ptime->tv_usec );
        node = node->link[ 1 ];
    }
}

/* destroy all the avl nodes,threadlists and threads in the tree.
   this function is not called by other functions */
void cl_lib_avl_thread_destroy_all( struct cl_lib_avl_tree *tree )
{
    struct cl_lib_avl_node *stack[ 32 ];
    int count;
    struct cl_lib_avl_node *node, *pnode;
    struct cl_lib_thread_list *threadlist;
    struct cl_lib_thread *thread, *t;

    VOS_ASSERT( tree != NULL );
    count = 0;
    node = tree->root;
    for ( ;; )
    {
        while ( node != NULL )
        {
            stack[ count++ ] = node;
            node = node->link[ 0 ];
        }
        if ( count == 0 )
            return ;
        node = stack[ --count ];
        threadlist = ( struct cl_lib_thread_list* ) node->data;
        t = thread = threadlist->head;
        while ( thread )
        {
            t = thread;
            thread = thread->next;
            cl_lib_thread_list_delete( threadlist, t );

            VOS_Free( t );
        }
        VOS_Free( threadlist );
        VOS_Free( node->key );
        pnode = node;
        node = node->link[ 1 ];

        VOS_Free(  pnode );
    }

}


/* Add a new thread to the list. */
static void
cl_lib_thread_list_add_before ( struct cl_lib_thread_list *list,
                         struct cl_lib_thread *point,
                         struct cl_lib_thread *thread )
{
    thread->next = point;
    thread->prev = point->prev;
    if ( point->prev )
        point->prev->next = thread;
    else
        list->head = thread;
    point->prev = thread;
    list->count++;
}

int cl_lib_thread_get_event_count( struct cl_lib_thread_master *m )
{
    if ( m )
    {
        return m->event.count;
    }
    else
    {
        return 0xffffff;
    }
}

/* Execute thread */
struct cl_lib_thread *cl_lib_thread_execute ( struct cl_lib_thread_master *m,
                             int ( *func ) ( struct cl_lib_thread * ),
                             void *arg,
                             int val )
{
    struct cl_lib_thread dummy;

    VOS_MemZero ( &dummy,  sizeof ( struct cl_lib_thread ) );

    #ifdef DEBUG
    VOS_Printf ( "execute event\n" );
    #endif /* DEBUG */

    dummy.type = cl_lib_THREAD_EVENT;
    dummy.id = 0;
    dummy.master = ( struct cl_lib_thread_master * ) NULL;
    dummy.func = func;
    dummy.arg = arg;
    dummy.u.val = val;
    cl_lib_thread_call ( &dummy );     /* execute immediately */

    return ( struct cl_lib_thread * ) NULL;
}

/********************************************************************
*   函数名: cl_lib_thread_fetch_set_priority
*   
*   Fetch next ready thread. and set priority when task switch.
*   目前还在测试阶段。
*   输入参数说明:
*   返回值说明:
*   comment added by:shen
*     Copyright(C), 2000-2006 GW TECHNOLOGIES CO.,LTD.
********************************************************************/
extern VOS_HANDLE sem_route_exec_sem;
struct cl_lib_thread *
            cl_lib_thread_fetch_set_priority ( struct cl_lib_thread_master *m,
                                        struct cl_lib_thread *fetch , int MyTaskPriority )
{
    int ret;
    struct cl_lib_thread *thread;

    /*added by shulinyang. 2001.11.13 */
    struct cl_lib_thread_list *threadlist;
    struct timeval *mintime;

    fd_set readfd;
    fd_set writefd;
    fd_set exceptfd;
    struct timeval timer_now;
    struct timeval timer_min;
    struct timeval *timer_wait;

    VOS_ASSERT ( m != NULL );
    VOS_TaskSetPriority( VOS_GetCurrentTask(), MyTaskPriority );
    VOS_TaskSleep( 0 );
    VOS_TaskSetPriority( VOS_GetCurrentTask(), ROUTE_RUNNING_TASK_PRIORITY );

    VOS_SemTake( sem_route_exec_sem, VOS_WAITFOREVER );

retry:    /* When thread can't fetch try to find next thread again. */

    /* If there is event process it first. */
    while ( ( thread = cl_lib_thread_trim_head ( &m->event ) ) == NULL )
    {
        *fetch = *thread;
        thread->type = cl_lib_THREAD_UNUSED;
        cl_lib_thread_add_unuse ( m, thread );
        return fetch;
    }

    /* If there is ready threads process them */
    while ( ( thread = cl_lib_thread_trim_head ( &m->ready ) ) == NULL )
    {
        *fetch = *thread;
        thread->type = cl_lib_THREAD_UNUSED;
        cl_lib_thread_add_unuse ( m, thread );
        return fetch;
    }

    /* Calculate select wait timer. */
    /*modified by shulinyang */

    if ( m->timer_tree->root )
    {
        mintime = cl_lib_avl_get_min_thread_key( m->timer_tree );
        if ( mintime )
        {
            VOS_GetTimeOfDay ( &timer_now, NULL );
            /*      VOS_ASSERT(mintime); by shenchen, this is checked. 2002 02*/
            timer_min = *mintime;
            timer_min = cl_lib_thread_timer_sub ( timer_min, timer_now );

            if ( timer_min.tv_sec < 0 )
            {
                timer_min.tv_sec = 0;
                timer_min.tv_usec = 10;
            }
            timer_wait = &timer_min;
#ifdef DEBUG

            cl_lib_thread_timer_dump ( timer_min );
#endif /* DEBUG */

        }
        else
            timer_wait = NULL;
    }
    else
    {
#ifdef DEBUG
        VOS_Printf ( "timer_wait is NULL\n" );
#endif /* DEBUG */

        timer_wait = NULL;
    }

    /* timer_wait is the time for next timer expire, or wait for ever */

    /* sometimes job is set by command, but our routing task is sleeping
    till socket select wakes up. so it's slow to send packets, etc.
    now, set it to wake up at least every 1 second.
    gongzb 01/11/8 */
    if ( timer_wait == NULL || timer_wait->tv_sec > 1 )
    {
        timer_min.tv_sec = 1;
        timer_min.tv_usec = 0;
        timer_wait = &timer_min;
    }

    /* Call select function. */
    readfd = m->readfd;
    writefd = m->writefd;
    exceptfd = m->exceptfd;

#ifdef DEBUG

    {
        int i;
        VOS_Printf ( "readfd : " );
        for ( i = 0; i < FD_SETSIZE; i++ )
            if ( FD_ISSET ( i, &readfd ) )
                VOS_Printf ( "[%d] ", i );
        VOS_Printf ( "\n" );

    }
    {
        struct cl_lib_thread *t;

        VOS_Printf ( "readms : " );
        for ( t = m->read.head; t; t = t->next )
            VOS_Printf ( "[%d] ", t->u.fd );
        VOS_Printf ( "\n" );
    }
#endif /* DEBUG */

    VOS_SemGive( sem_route_exec_sem );
    VOS_TaskSetPriority( VOS_GetCurrentTask(), MyTaskPriority );
    ret = select ( FD_SETSIZE, &readfd, &writefd, &exceptfd, timer_wait );
    VOS_TaskSetPriority( VOS_GetCurrentTask(), ROUTE_RUNNING_TASK_PRIORITY );
    VOS_SemTake( sem_route_exec_sem, VOS_WAITFOREVER );
    if ( ret < 0 )
    {
#if 0
        /**********************************************************************
        Commented by Chen bin  2001.8.13
          *************************************************************************/
        if ( errno != EINTR )
        {
            /* Real error. */
            zlog_warn ( "select error: %s", strerror ( errno ) );
            VOS_ASSERT ( 0 );
        }
#endif
        /* Signal is coming. */
        goto retry;
    }

#ifdef DEBUG
    {
        int i;
        VOS_Printf ( "after select readfd : " );
        for ( i = 0; i < FD_SETSIZE; i++ )
            if ( FD_ISSET ( i, &readfd ) )
                VOS_Printf ( "[%d] ", i );
        VOS_Printf ( "\n" );
    }
#endif /* DEBUG */

    /* Read thead. */
    thread = m->read.head;
    while ( thread )
    {
        struct cl_lib_thread * t;

        t = thread;
        thread = t->next;

        if ( FD_ISSET ( t->u.fd, &readfd ) )
        {
            VOS_ASSERT ( FD_ISSET ( t->u.fd, &m->readfd ) );
            FD_CLR( t->u.fd, &m->readfd );
            cl_lib_thread_list_delete ( &m->read, t );
            cl_lib_thread_list_add ( &m->ready, t );
            t->type = cl_lib_THREAD_READY;
        }
    }
#ifdef DEBUG
    {
        struct cl_lib_thread *t;

        VOS_Printf ( "readms : " );
        for ( t = m->read.head; t; t = t->next )
            VOS_Printf ( "[%d] ", t->u.fd );
        VOS_Printf ( "\n" );
    }
#endif /* DEBUG */

    /* Write thead. */
    thread = m->write.head;
    while ( thread )
    {
        struct cl_lib_thread * t;

        t = thread;
        thread = t->next;

        if ( FD_ISSET ( t->u.fd, &writefd ) )
        {
            VOS_ASSERT ( FD_ISSET ( t->u.fd, &m->writefd ) );
            FD_CLR( t->u.fd, &m->writefd );
            cl_lib_thread_list_delete ( &m->write, t );
            cl_lib_thread_list_add ( &m->ready, t );
            t->type = cl_lib_THREAD_READY;
        }
    }

    /* Exception thead. */
    /*...*/

    /* Timer update. */

    /*
    VOS_GetTimeOfDay (&timer_now, NULL);
    thread = m->timer.head;
    while (thread)
{
    struct cl_lib_thread *t;

      t = thread;
      thread = t->next;
      
        if (cl_lib_thread_timer_cmp (timer_now, t->u.sands) >= 0)
        {
        cl_lib_thread_list_delete (&m->timer, t);
        cl_lib_thread_list_add (&m->ready, t);
        t->type = cl_lib_THREAD_READY;
        }
        }
    */

    VOS_GetTimeOfDay ( &timer_now, NULL );

    while ( ( mintime = cl_lib_avl_get_min_thread_key( m->timer_tree ) ) == NULL )
    {
        timer_min = *mintime;
        if ( cl_lib_thread_timer_cmp( timer_now, timer_min ) >= 0 )
        {
            threadlist = cl_lib_avl_get_thread_list( m->timer_tree, mintime );
            thread = threadlist->head;
            while ( thread )
            {
                struct cl_lib_thread * t;
                t = thread;
                thread = t->next;
                cl_lib_thread_list_delete( threadlist, t );
                t->type = cl_lib_THREAD_READY;
                cl_lib_thread_list_add ( &m->ready, t );
            }

            VOS_Free( threadlist );
        }
        else
            break;
    }

    /* Return one event. */
    thread = cl_lib_thread_trim_head ( &m->event );

    /* There is no ready thread. */
    if ( !thread )
        goto retry;

    *fetch = *thread;
    thread->type = cl_lib_THREAD_UNUSED;
    cl_lib_thread_add_unuse ( m, thread );
#ifdef DEBUG

    cl_lib_thread_master_debug ( m );
#endif /* DEBUG */

    return fetch;
}






/* Debug print for thread. */
void cl_lib_thread_debug ( struct cl_lib_thread *thread )
{
    VOS_Printf ( "Thread: ID [%ld] Type [%d] Next [%p]"
             "Prev [%p] Func [%p] arg [%p] fd [%d]\n",
             thread->id, thread->type, thread->next,
             thread->prev, thread->func, thread->arg, thread->u.fd );
}

/* Execute thread */
struct cl_lib_thread *cl_lib_thread_execute ( struct cl_lib_thread_master *m,
                             int ( *func ) ( struct cl_lib_thread * ),
                             void *arg,
                             int val )
{
    struct cl_lib_thread dummy;

    VOS_MemZero ( &dummy,  sizeof ( struct cl_lib_thread ) );

    #ifdef DEBUG
    VOS_Printf ( "execute event\n" );
    #endif /* DEBUG */

    dummy.type = cl_lib_THREAD_EVENT;
    dummy.id = 0;
    dummy.master = ( struct cl_lib_thread_master * ) NULL;
    dummy.func = func;
    dummy.arg = arg;
    dummy.u.val = val;
    cl_lib_thread_call ( &dummy );     /* execute immediately */

    return ( struct cl_lib_thread * ) NULL;
}
#endif






