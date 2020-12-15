#include <sys/uio.h>


struct iovec {
	void *iov_base; /* starting address of buffer */
	size_t iov_len; /* size of buffer */
};


//ssize_t readv(int filedes, const struct iovec *iov, int iovcnt);
ssize_t writev(int filedes, const struct iovec *iov, int iovcnt);
//Both return: number of bytes read or written, –1 on error

