/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */


/* These are cmsg_types.  */
typedef enum sctp_cmsg_type {
	SCTP_INIT,              /* 5.2.1 SCTP Initiation Structure */
#define SCTP_INIT SCTP_INIT
	SCTP_SNDRCV,            /* 5.2.2 SCTP Header Information Structure */
#define SCTP_SNDRCV SCTP_SNDRCV
} sctp_cmsg_t;

