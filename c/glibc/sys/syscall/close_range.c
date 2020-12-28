static inline int sys_close_range(unsigned int fd, unsigned int max_fd,
						  unsigned int flags)
{
	return syscall(__NR_close_range, fd, max_fd, flags);
}
