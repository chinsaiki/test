/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

//sctp_getladdrs - Returns all locally bound addresses on a socket.

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/sctp.h>

//int sctp_getladdrs(int sd, sctp_assoc_t assoc_id,
//                  struct sockaddr **addrs);

void sctp_freeladdrs(struct sockaddr *addrs);

