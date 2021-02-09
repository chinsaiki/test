#include <sys/types.h>
#include <unistd.h>

//whence
#define SEEK_SET	0	/* seek relative to beginning of file */
#define SEEK_CUR	1	/* seek relative to current file position */
#define SEEK_END	2	/* seek relative to end of file */


//lseek - reposition read/write file offset
off_t lseek(int fd, off_t offset, int whence);

