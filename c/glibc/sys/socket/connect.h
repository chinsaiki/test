#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

//initiate a connection on a socket
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);


