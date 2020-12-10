/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/sctp.h>

//Send a message from a SCTP socket.
int sctp_send(int sd, const void * msg, size_t len,
                const struct sctp_sndrcvinfo *sinfo,
                uint32_t flags);

