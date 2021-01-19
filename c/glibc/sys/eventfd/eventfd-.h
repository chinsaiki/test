#include <sys/eventfd.h>

/* Flags for eventfd.  */
enum
  {
    EFD_SEMAPHORE = 00000001,
#define EFD_SEMAPHORE EFD_SEMAPHORE
    EFD_CLOEXEC = 02000000,
#define EFD_CLOEXEC EFD_CLOEXEC
    EFD_NONBLOCK = 00004000
#define EFD_NONBLOCK EFD_NONBLOCK
  };

//eventfd - create a file descriptor for event notification
int eventfd(unsigned int initval, int flags);
//eventfd  - 计数器
//是linux2.6后系统提供的一个轻量级的进程间通信的系统调用，eventfd通过一个进程间共享的64位计数器
//完成进程间通信，这个计数器由在linux内核空间维护，用户可以通过调用write方法向内核写入一个64位的值
//也可以调用read方法读取这个值。 --- rtoax




SYSCALL_DEFINE2(eventfd2, unsigned int, count, int, flags)
{
	return do_eventfd(count, flags);
}

SYSCALL_DEFINE1(eventfd, unsigned int, count)
{
	return do_eventfd(count, 0);
}


static int do_eventfd(unsigned int count, int flags)
{
	struct eventfd_ctx *ctx;
	int fd;

	/* Check the EFD_* constants for consistency.  */
	BUILD_BUG_ON(EFD_CLOEXEC != O_CLOEXEC);
	BUILD_BUG_ON(EFD_NONBLOCK != O_NONBLOCK);

	if (flags & ~EFD_FLAGS_SET)
		return -EINVAL;

	ctx = kmalloc(sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	kref_init(&ctx->kref);
	init_waitqueue_head(&ctx->wqh);
	ctx->count = count;
	ctx->flags = flags;
	ctx->id = ida_simple_get(&eventfd_ida, 0, 0, GFP_KERNEL);

	fd = anon_inode_getfd("[eventfd]", &eventfd_fops, ctx,
			      O_RDWR | (flags & EFD_SHARED_FCNTL_FLAGS));
	if (fd < 0)
		eventfd_free_ctx(ctx);

	return fd;
}

