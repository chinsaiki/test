#include <sys/mman.h>

//lock and unlock memory
//mlock()  locks  pages in the address range starting at addr and continuing for len bytes.
/**
 *	The mlock family of system calls allows a program 
 *	to lock some or all of its address space into
 *	physical memory. This prevents Linux from paging 
 *	this memory to swap space, even if the
 *	program hasn't accessed it for a while.
 * 禁止系统将该内存分页至swap交换空间
 */
int mlock(const void *addr, size_t len);
int munlock(const void *addr, size_t len);

//lock and unlock memory
//mlockall() locks all pages mapped into the address space of the calling process.
int mlockall(int flags);
int munlockall(void);

//MCL_CURRENT Lock all pages which are currently mapped into the address space of the process.
//
//MCL_FUTURE  Lock all pages which will become mapped into the address space of the process in the  future.
//            These  could  be  for  instance new pages required by a growing heap and stack as well as new
//            memory mapped files or shared memory regions.


SYSCALL_DEFINE2(mlock, unsigned long, start, size_t, len)
{
	return do_mlock(start, len, VM_LOCKED);
}

SYSCALL_DEFINE3(mlock2, unsigned long, start, size_t, len, int, flags)
{
	vm_flags_t vm_flags = VM_LOCKED;

	if (flags & ~MLOCK_ONFAULT)
		return -EINVAL;

	if (flags & MLOCK_ONFAULT)
		vm_flags |= VM_LOCKONFAULT;

	return do_mlock(start, len, vm_flags);
}

SYSCALL_DEFINE2(munlock, unsigned long, start, size_t, len)
{
	int ret;

	start = untagged_addr(start);

	len = PAGE_ALIGN(len + (offset_in_page(start)));
	start &= PAGE_MASK;

	if (down_write_killable(&current->mm->mmap_sem))
		return -EINTR;
	ret = apply_vma_lock_flags(start, len, 0);
	up_write(&current->mm->mmap_sem);

	return ret;
}


SYSCALL_DEFINE1(mlockall, int, flags)
{
	unsigned long lock_limit;
	int ret;

	if (!flags || (flags & ~(MCL_CURRENT | MCL_FUTURE | MCL_ONFAULT)) ||
	    flags == MCL_ONFAULT)
		return -EINVAL;

	if (!can_do_mlock())
		return -EPERM;

	lock_limit = rlimit(RLIMIT_MEMLOCK);
	lock_limit >>= PAGE_SHIFT;

	if (down_write_killable(&current->mm->mmap_sem))
		return -EINTR;

	ret = -ENOMEM;
	if (!(flags & MCL_CURRENT) || (current->mm->total_vm <= lock_limit) ||
	    capable(CAP_IPC_LOCK))
		ret = apply_mlockall_flags(flags);
	up_write(&current->mm->mmap_sem);
	if (!ret && (flags & MCL_CURRENT))
		mm_populate(0, TASK_SIZE);

	return ret;
}

SYSCALL_DEFINE0(munlockall)
{
	int ret;

	if (down_write_killable(&current->mm->mmap_sem))
		return -EINTR;
	ret = apply_mlockall_flags(0);
	up_write(&current->mm->mmap_sem);
	return ret;
}

