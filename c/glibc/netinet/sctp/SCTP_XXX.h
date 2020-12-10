/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:19 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:28 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:53:58 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 11日 星期一 08:57:54 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:12 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:50 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:26:56 CST. */


setsockopt(sock_fd, IPPROTO_SCTP, SCTP_AUTOCLOSE, &close_time, sizeof(close_time));


/* The following symbols come from the Sockets API Extensions for
 * SCTP <draft-ietf-tsvwg-sctpsocket-07.txt>.
 */
#define SCTP_RTOINFO	0
#define SCTP_ASSOCINFO  1
#define SCTP_INITMSG	2
#define SCTP_NODELAY	3		/* Get/set nodelay option. */
#define SCTP_AUTOCLOSE	4
#define SCTP_SET_PEER_PRIMARY_ADDR 5
#define SCTP_PRIMARY_ADDR	6
#define SCTP_ADAPTATION_LAYER	7
#define SCTP_DISABLE_FRAGMENTS	8
#define SCTP_PEER_ADDR_PARAMS	9
#define SCTP_DEFAULT_SEND_PARAM	10
#define SCTP_EVENTS	11
#define SCTP_I_WANT_MAPPED_V4_ADDR 12	/* Turn on/off mapped v4 addresses  */
#define SCTP_MAXSEG	13		/* Get/set maximum fragment. */
#define SCTP_STATUS	14
#define SCTP_GET_PEER_ADDR_INFO	15
#define SCTP_DELAYED_ACK_TIME	16
#define SCTP_DELAYED_ACK SCTP_DELAYED_ACK_TIME
#define SCTP_DELAYED_SACK SCTP_DELAYED_ACK_TIME
#define SCTP_CONTEXT	17
#define SCTP_FRAGMENT_INTERLEAVE	18
#define SCTP_PARTIAL_DELIVERY_POINT	19 /* Set/Get partial delivery point */
#define SCTP_MAX_BURST	20		/* Set/Get max burst */
#define SCTP_AUTH_CHUNK	21	/* Set only: add a chunk type to authenticate */
#define SCTP_HMAC_IDENT	22
#define SCTP_AUTH_KEY	23
#define SCTP_AUTH_ACTIVE_KEY	24
#define SCTP_AUTH_DELETE_KEY	25
#define SCTP_PEER_AUTH_CHUNKS	26	/* Read only */
#define SCTP_LOCAL_AUTH_CHUNKS	27	/* Read only */
#define SCTP_GET_ASSOC_NUMBER	28	/* Read only */

