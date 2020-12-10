/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
//SCTP protocol.

#define	PF_INET		2	/* IP protocol family.  */

#define	SOCK_SEQPACKET	5		/* sequenced packet stream */

sctp_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_SCTP);
sctp_socket = socket(PF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);



//one-to-many
//---------------------------------------------------------------------------------------------------------
//A one-to-many style interface with 1 to MANY relationship between socket and associations where the  out‐
//bound association setup is implicit. The syntax of a one-to-many style socket() call is

sd = socket(PF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);

//A  typical  server  in  this style uses the following socket calls in sequence to prepare an endpoint for
//servicing requests.

 1. socket()
 2. bind()
 3. listen()
 4. recvmsg()
 5. sendmsg()
 6. close()

//A typical client uses the following calls in sequence to setup an association with a  server  to  request
//services.

 1. socket()
 2. sendmsg()
 3. recvmsg()
 4. close()


//one-to-one
//---------------------------------------------------------------------------------------------------------
//A  one-to-one  style  interface  with  a 1 to 1 relationship between socket and association which enables
//existing TCP applications to be ported to SCTP with very little effort. The syntax of a one-to-one  style
//socket() call is

sd = socket(PF_INET, SOCK_STREAM, IPPROTO_SCTP);

//A  typical server in one-to-one style uses the following system call sequence to prepare an SCTP endpoint
//for servicing requests:

  1. socket()
  2. bind()
  3. listen()
  4. accept()

//The accept() call blocks until a new association is set up. It returns with a new socket descriptor.  The
//server  then uses the new socket descriptor to communicate with the client, using recv() and send() calls
//to get requests and send back responses. Then it calls

  5. close()

//to terminate the association. A typical client uses the following system call sequence to setup an  asso‐
//ciation with a server to request services:

  1. socket()
  2. connect()

//After  returning from connect(), the client uses send() and recv() calls to send out requests and receive
//responses from the server. The client calls

  3. close()

//to terminate this association when done.


