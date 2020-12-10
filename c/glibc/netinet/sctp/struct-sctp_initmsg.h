/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */


/*
 * 5.2.1 SCTP Initiation Structure (SCTP_INIT)
 *
 *   This cmsghdr structure provides information for initializing new
 *   SCTP associations with sendmsg().  The SCTP_INITMSG socket option
 *   uses this same data structure.  This structure is not used for
 *   recvmsg().
 *
 *   cmsg_level    cmsg_type      cmsg_data[]
 *   ------------  ------------   ----------------------
 *   IPPROTO_SCTP  SCTP_INIT      struct sctp_initmsg
 *
 */
struct sctp_initmsg {
	__u16 sinit_num_ostreams;
	__u16 sinit_max_instreams;
	__u16 sinit_max_attempts;
	__u16 sinit_max_init_timeo;
};

