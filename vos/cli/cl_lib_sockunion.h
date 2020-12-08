/*
* Socket union header.
* Copyright (c) 1997 Kunihiro Ishiguro
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

#ifndef _cl_lib_SOCKUNION_H
#define _cl_lib_SOCKUNION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define CL_LIB_AF_INET6 10
#define CL_LIB_AF_INET  2

union cl_lib_sockunion
{
    struct sockaddr sa;
    struct sockaddr_in sin;
#ifdef HAVE_IPV6

    struct sockaddr_in6 sin6;
#endif /* HAVE_IPV6 */
};

enum cl_lib_connect_result
{
    cl_lib_connect_error,
    cl_lib_connect_success,
    cl_lib_connect_in_progress
};

/* Default address family. */
#ifdef HAVE_IPV6
#define cl_lib_AF_INET_UNION AF_INET6
#else
#define cl_lib_AF_INET_UNION AF_INET
#endif

/* Sockunion address string length.  Same as INET6_ADDRSTRLEN. */
#define cl_lib_SU_ADDRSTRLEN 46

/* Macro to set link local index to the IPv6 address.  For KAME IPv6
   stack. */
#ifdef KAME
#define	cl_lib_IN6_LINKLOCAL_IFINDEX(a)  ((a).s6_addr[2] << 8 | (a).s6_addr[3])
#define cl_lib_SET_IN6_LINKLOCAL_IFINDEX(a, i) \
  do { \
    (a).s6_addr[2] = ((i) >> 8) & 0xff; \
    (a).s6_addr[3] = (i) & 0xff; \
  } while (0)
#else
#define	cl_lib_IN6_LINKLOCAL_IFINDEX(a)
#define cl_lib_SET_IN6_LINKLOCAL_IFINDEX(a, i)
#endif /* KAME */

/* shortcut macro to specify address field of struct sockaddr */
#define cl_lib_sock2ip(X)   (((struct sockaddr_in *)(X))->sin_addr.s_addr)
#ifdef HAVE_IPV6
#define cl_lib_sock2ip6(X)  (((struct sockaddr_in6 *)(X))->sin6_addr.s6_addr)
#endif /* HAVE_IPV6 */

#define cl_lib_sockunion_family(X)  (X)->sa.sa_family

/* Prototypes. */
int cl_lib_str2sockunion ( char *, union cl_lib_sockunion * );
const char *cl_lib_sockunion2str ( union cl_lib_sockunion *, char *, unsigned long );
int cl_lib_sockunion_cmp ( union cl_lib_sockunion *, union cl_lib_sockunion * );
int cl_lib_sockunion_same ( union cl_lib_sockunion *, union cl_lib_sockunion * );
char *cl_lib_sockunion_su2str ( union cl_lib_sockunion *su,long moduleID );
union cl_lib_sockunion *cl_lib_sockunion_str2su ( char *str );
/*struct in_addr sockunion_get_in_addr ( union cl_lib_sockunion *su );*/
int cl_lib_sockunion_accept ( int sock, union cl_lib_sockunion * );
int cl_lib_sockunion_stream_socket ( union cl_lib_sockunion * );
int cl_lib_sockopt_reuseaddr ( int );
int cl_lib_sockopt_reuseport ( int );
int cl_lib_sockunion_bind ( int sock, union cl_lib_sockunion *, unsigned short, union cl_lib_sockunion * );
int cl_lib_sockunion_listen ( int sock, int backlog );
int cl_lib_sockopt_ttl ( int family, int sock, int ttl );
int cl_lib_sockunion_socket ( union cl_lib_sockunion *su );
const char *cl_lib_inet_sutop ( union cl_lib_sockunion *su, char *str );
char *cl_lib_sockunion_log ( union cl_lib_sockunion *su );
enum cl_lib_connect_result
cl_lib_sockunion_connect ( int fd, union cl_lib_sockunion *su, unsigned short port, unsigned int );
union cl_lib_sockunion *cl_lib_sockunion_getsockname ( int );
union cl_lib_sockunion *cl_lib_sockunion_getpeername ( int );
const char *cl_lib_inet_ntop ( int family, const void *addrptr, char *strptr, unsigned long len );
int cl_lib_inet_pton ( int family, char *strptr, void *addrptr );
int cl_lib_inet_aton ( char *cp, struct in_addr *inaddr );
void cl_lib_inet_ntoa_b( struct in_addr inetAddress,char *pString );
BOOL cl_lib_sockunion_is_localAddr ( union cl_lib_sockunion *su);

#endif /* _cl_lib_SOCKUNION_H */

