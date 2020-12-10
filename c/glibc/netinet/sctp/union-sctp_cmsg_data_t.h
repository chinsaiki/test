/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */


typedef union {
	__u8   			raw;
	struct sctp_initmsg	init;
	struct sctp_sndrcvinfo	sndrcv;
} sctp_cmsg_data_t;

