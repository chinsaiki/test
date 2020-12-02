/* 
 * Copyright (c) 2013, Intel Corporation
 * Author: Andi Kleen
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * Simple port forwarder
 * proxy inport outip outport
 * Uses pipes to splice two sockets together. This should give something
 * approaching zero copy, if the NIC driver is capable. 
 * This method is rather file descriptor intensive (4 fds/eproxy_conn), so make sure you 
 * have enough. 
 * Written 2012 by Andi Kleen
 */
#define _GNU_SOURCE 1
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include "list.h"

#define err(x) perror(x), exit(1)
#define NEW(x) ((x) = eproxy_xmalloc(sizeof(*(x))))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

#ifdef DEBUG
#define eproxy_debug(fmt...) do{\
    fprintf(stderr,"[%s:%d]:", __func__, __LINE__);\
    fprintf(stderr,fmt);\
}while(0)
#else
#define eproxy_debug(fmt...)
#endif

struct eproxy_buffer {
	int pipe[2];
	int bytes;
};

struct eproxy_conn {
	struct eproxy_conn *other;
	int fd;
	struct eproxy_buffer buf;
	time_t expire;
	struct list_head expire_node;
};

LIST_HEAD(expire_list);

#define eproxy_MIN_EVENTS 32
struct epoll_event *eproxy_events;
int eproxy_num_events, eproxy_max_events;


int connection_timeout = 5; /* XXX configurable */

void eproxy_oom(void)
{
	fprintf(stderr, "Out of memory\n");
	exit(1);
}

void *eproxy_xmalloc(size_t size)
{
	void *p = calloc(size, 1);
	if (!p)
		eproxy_oom();
	return p;
}

void *eproxy_xrealloc(void *old, size_t size)
{
	void *p = realloc(old, size);
	if (!p)
		eproxy_oom();
	return p;
}

struct addrinfo *eproxy_resolve(char *name, char *port, int flags)
{
    eproxy_debug("\n");
	int ret;
	struct addrinfo *adr;
	struct addrinfo hint = { .ai_flags = flags };

	ret = getaddrinfo(name, port, &hint, &adr);
	if (ret) {
		fprintf(stderr, "proxy: Cannot eproxy_resolve %s %s: %s\n",
			name, port, gai_strerror(ret));
		exit(1);
	}
	return adr;
}

void eproxy_setnonblock(int fd, int *cache)
{
    eproxy_debug("\n");
	int flags;
	if (!cache || *cache == -1) {
	 	flags = fcntl(fd, F_GETFL, 0);
		if (cache)
			*cache = flags;
	} else
		flags = *cache;
	fcntl(fd, F_SETFL, flags|O_NONBLOCK);
}


int epoll_add(int efd, int fd, int revents, void *eproxy_conn)
{
    eproxy_debug("\n");
	struct epoll_event ev = { .events = revents, .data.ptr = eproxy_conn };
	if (++eproxy_num_events >= eproxy_max_events) {
		eproxy_max_events = MAX(eproxy_max_events * 2, eproxy_MIN_EVENTS);
		eproxy_events = eproxy_xrealloc(eproxy_events, 
				  sizeof(struct epoll_event) * eproxy_max_events);
	}
	return epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev);
}

int epoll_del(int efd, int fd)
{
    eproxy_debug("\n");
	eproxy_num_events--;
	assert(eproxy_num_events >= 0);
	return epoll_ctl(efd, EPOLL_CTL_DEL, fd, (void *)1L);
}

/* Create eproxy_buffer between two connections */
struct eproxy_buffer *eproxy_newbuffer(struct eproxy_buffer *buf)
{
    eproxy_debug("\n");
	if (pipe2(buf->pipe, O_NONBLOCK) < 0) {
		perror("pipe");
		return NULL;
	}
	return buf;
}

void eproxy_delbuffer(struct eproxy_buffer *buf)
{
    eproxy_debug("\n");
	close(buf->pipe[0]);
	close(buf->pipe[1]);
}

void eproxy_delconn(int efd, struct eproxy_conn *conn)
{
    eproxy_debug("%p:%p\n", &conn->expire_node, &expire_list);
//    if(!list_empty(&expire_list))
    list_del(&conn->expire_node);
    eproxy_debug("\n");
	eproxy_delbuffer(&conn->buf);
    eproxy_debug("\n");
	epoll_del(efd, conn->fd);
	close(conn->fd);
	free(conn);
}

struct eproxy_conn *eproxy_newconn(int efd, int fd, time_t now)
{
    eproxy_debug("\n");
	struct eproxy_conn *conn;
	NEW(conn);
	conn->fd = fd;
	if (!eproxy_newbuffer(&conn->buf)) {
		eproxy_delconn(efd, conn);
		return NULL;
	}
	if (epoll_add(efd, fd, EPOLLIN|EPOLLOUT|EPOLLET, conn) < 0) {
		perror("epoll");
		eproxy_delconn(efd, conn);
		return NULL;
	}
	conn->expire = now + connection_timeout;
	list_add_tail(&conn->expire_node, &expire_list);
	return conn;
}

/* Process incoming connection. */
void eproxy_new_request(int efd, int lfd, int *cache, time_t now)
{
    eproxy_debug("\n");
	int newsk = accept(lfd, NULL, NULL);
	if (newsk < 0) {
		perror("accept");
		return;
	}
	// xxx log
	eproxy_setnonblock(newsk, cache);
	eproxy_newconn(efd, newsk, now);
}

/* Open outgoing connection */
struct eproxy_conn *
eproxy_openconn(int efd, struct addrinfo *host, int *cache, struct eproxy_conn *other,
	 time_t now)
{
    eproxy_debug("\n");
	int outfd = socket(host->ai_family, SOCK_STREAM, 0);
	if (outfd < 0)
		return NULL;
	eproxy_setnonblock(outfd, cache);
	int n = connect(outfd, host->ai_addr, host->ai_addrlen);
	if (n < 0 && errno != EINPROGRESS) {
		perror("connect");
		close(outfd);
		return NULL;
	}
	struct eproxy_conn *conn = eproxy_newconn(efd, outfd, now);
	if (conn) {
		conn->other = other;
		other->other = conn;
	}
	return conn;
}

#define eproxy_BUFSZ 16384 /* XXX */

/* Move from socket to pipe */
bool eproxy_move_data_in(int srcfd, struct eproxy_buffer *buf)
{
    eproxy_debug("\n");
	for (;;) {	
		int n = splice(srcfd, NULL, buf->pipe[1], NULL, 
			   	eproxy_BUFSZ, SPLICE_F_NONBLOCK|SPLICE_F_MOVE);
		if (n > 0)
			buf->bytes += n;
		if (n == 0)
			return false;
		if (n < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return true;
			return false;
		}
			
	}
	return true;
}

/* From pipe to socket */
bool eproxy_move_data_out(struct eproxy_buffer *buf, int dstfd)
{ 
    eproxy_debug("\n");
	while (buf->bytes > 0) {
		int bytes = buf->bytes;
		if (bytes > eproxy_BUFSZ)
			bytes = eproxy_BUFSZ;
		int n = splice(buf->pipe[0], NULL, dstfd, NULL,
		 	 	 bytes, SPLICE_F_NONBLOCK|SPLICE_F_MOVE);
		if (n == 0)
			break;
		if (n < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			return false;
		}
		buf->bytes -= n;
	}
	/* bytes > 0, add dst to epoll set */
	/* else remove if it was added */
	return true;
}

void eproxy_closeconn(int efd, struct eproxy_conn *conn)
{
    eproxy_debug("\n");
//	if (conn->other){
//        eproxy_debug("Del other CONN\n");
//		eproxy_delconn(efd, conn->other);
//
//    }
    eproxy_debug("Del CONN\n");
	eproxy_delconn(efd, conn);
}

int eproxy_expire_connections(int efd, time_t now)
{
    eproxy_debug("\n");
	struct eproxy_conn *conn, *tmp;

//    if(!list_empty(&expire_list))
	list_for_each_entry_safe (conn, tmp, &expire_list, expire_node) {
		if (conn->expire > now)
			return (conn->expire - now) * 1000;
		eproxy_closeconn(efd, conn);
        conn = NULL;
	}
	return -1;
}

void eproxy_touch_conn(struct eproxy_conn *conn, time_t now)
{
    eproxy_debug("\n");
	conn->expire = now + connection_timeout;
	list_del(&conn->expire_node);
	list_add_tail(&conn->expire_node, &expire_list);
}

int eproxy_listen_socket(int efd, char *lname, char *port)
{
    eproxy_debug("\n");
	struct addrinfo *laddr = eproxy_resolve(lname, port, AI_PASSIVE);
	
	int lfd = socket(laddr->ai_family, SOCK_STREAM, 0);
	if (lfd < 0) 
		err("socket");
	int opt = 1;
	if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0) 
		err("SO_REUSEADDR");
	if (bind(lfd, laddr->ai_addr, laddr->ai_addrlen) < 0) 
		err("bind");
	if (listen(lfd, 20) < 0) 
		err("listen");
	eproxy_setnonblock(lfd, NULL);
	freeaddrinfo(laddr);

	if (epoll_add(efd, lfd, EPOLLIN, NULL) < 0) 
		err("epoll add listen fd");

	return lfd;
}

int main(int ac, char **av)
{
    eproxy_debug("\n");
	if (ac != 4 && ac != 5) {
		fprintf(stderr,
			 "Usage: proxy inport outhost outport [listenaddr]\n");
		exit(1);
	}

    /* 创建一个转发的目的地址信息 */
	struct addrinfo *outhost = eproxy_resolve(av[2], av[3], 0);

	int efd = epoll_create(10);
	if (efd < 0) 
		err("epoll_create");

    /*  */
	int lfd = eproxy_listen_socket(efd, av[4] ? av[4] : "0.0.0.0", av[1]);

	int cache_in = -1, cache_out = -1;	
	int timeo = -1;

	for (;;) {
		int nfds = epoll_wait(efd, eproxy_events, eproxy_num_events, timeo);
		if (nfds <= 0) {
			eproxy_debug("epoll_wait return %d, %s\n", nfds, strerror(errno));
			continue;
		}
        /* 获取当前时间 */
		time_t now = time(NULL);
        eproxy_debug("nfds = %d, now = %ld.\n", nfds, now);
        
		int i;
		for (i = 0; i < nfds; i++) { 
            eproxy_debug("nfds = %d.\n", nfds);
			struct epoll_event *ev = &eproxy_events[i];
			struct eproxy_conn *conn = ev->data.ptr;

			/* listen socket */
			if (conn == NULL) {
				if (ev->events & EPOLLIN) {
                    eproxy_debug("A New Client.\n");
					eproxy_new_request(efd, lfd, &cache_in, now);
                }
                eproxy_debug("A New Client Done.\n");
				continue;
			} 
            eproxy_debug("-----------------------\n");
			if (ev->events & (EPOLLERR|EPOLLHUP)) {
                eproxy_debug("Close A Client.\n");
				eproxy_closeconn(efd, conn);
				continue;
			}
            eproxy_debug("-----------------------\n");
			struct eproxy_conn *other = conn->other;

			/* No attempt for partial close right now */
			if (ev->events & EPOLLIN) {
                eproxy_debug("Gotta something IN.\n");
				eproxy_touch_conn(conn, now);
				if (!other)
					other = eproxy_openconn(efd, outhost, &cache_out,
							 conn, now);
				bool in = eproxy_move_data_in(conn->fd, &conn->buf);
				bool out = eproxy_move_data_out(&conn->buf, other->fd);
				if (!in || !out) { 
					eproxy_closeconn(efd, conn);
					continue;
				}
				eproxy_touch_conn(other, now);
			}	
				
			if ((ev->events & EPOLLOUT) && other) {
                eproxy_debug("Gotta something OUT.\n");
				if (!eproxy_move_data_out(&other->buf, conn->fd))
					eproxy_delconn(efd, conn);
				else
					eproxy_touch_conn(conn, now);

				/* When the pipe filled up could have
 				   lost input eproxy_events.  Unfortunately
 				   splice doesn't tell us which end
 				   was responsible for 0, so have to ask
				   explicitely. */
				int len = 0;
				if (ioctl(other->fd, FIONREAD, &len) < 0)
					perror("ioctl");
				if (len > 0) {
					if (!eproxy_move_data_in(other->fd, 
							  &other->buf))
						eproxy_closeconn(efd, other);
				}
			}
		}	

        //过期连接
		timeo = eproxy_expire_connections(efd, now);
        eproxy_debug("timeo = %d\n", timeo);
	}
	return 0;
}

