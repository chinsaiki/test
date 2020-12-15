
#include <unistd.h>

int pipe(int fildes[2]);
/**
 *	该函数返回两个文件描述符，fd[0], fd[1]
 *	fd[0]：打开来读
 *	fd[1]：打开来写
 */


SYSCALL_DEFINE2(pipe2, int __user *, fildes, int, flags)
{
	return do_pipe2(fildes, flags);
}

SYSCALL_DEFINE1(pipe, int __user *, fildes)
{
	return do_pipe2(fildes, 0);

    /*
     * sys_pipe() is the normal C calling standard for creating
     * a pipe. It's not the way Unix traditionally does this, though.
     */
    static int do_pipe2(int __user *fildes, int flags)
    {
    	struct file *files[2];
    	int fd[2];
    	int error;

    	error = __do_pipe_flags(fd, files, flags);
    	if (!error) {
    		if (unlikely(copy_to_user(fildes, fd, sizeof(fd)))) {
    			fput(files[0]);
    			fput(files[1]);
    			put_unused_fd(fd[0]);
    			put_unused_fd(fd[1]);
    			error = -EFAULT;
    		} else {
    			fd_install(fd[0], files[0]);
    			fd_install(fd[1], files[1]);
    		}
    	}
    	return error;
    }
    void fd_install(unsigned int fd, struct file *file)
    {
    	__fd_install(current->files, fd, file);
    }
}

