#include <sys/uio.h>

struct iovec {
   void  *iov_base;    /* Starting address */
   size_t iov_len;     /* Number of bytes to transfer */
};


//read or write data into multiple buffers
ssize_t readv(int fd, const struct iovec *iov, int iovcnt);

ssize_t writev(int fd, const struct iovec *iov, int iovcnt);

ssize_t preadv(int fd, const struct iovec *iov, int iovcnt,
              off_t offset);

ssize_t pwritev(int fd, const struct iovec *iov, int iovcnt,
               off_t offset);

