#include <bits/uio.h>

/* Structure for scatter/gather I/O.  */
struct iovec
{
    void *iov_base;	/* Pointer to data.  */
    size_t iov_len;	/* Length of data.  */
};
