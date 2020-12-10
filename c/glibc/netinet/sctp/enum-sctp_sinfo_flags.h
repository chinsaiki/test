/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */


/*
 *  sinfo_flags: 16 bits (unsigned integer)
 *
 *   This field may contain any of the following flags and is composed of
 *   a bitwise OR of these values.
 */

enum sctp_sinfo_flags {
	SCTP_UNORDERED = 1,  /* Send/receive message unordered. */
	SCTP_ADDR_OVER = 2,  /* Override the primary destination. */
	SCTP_ABORT=4,        /* Send an ABORT message to the peer. */
	SCTP_SACK_IMMEDIATELY = 8,      /* SACK should be sent without delay */
	SCTP_EOF=MSG_FIN,    /* Initiate graceful shutdown process. */
};

