
#include <unistd.h>

//扩展heap的上界，brk()设置新的上界地址，
int brk(void *addr);


//扩展heap的上界，sbrk()返回新的上界地址
void *sbrk(intptr_t increment);


#if 0
+-------------------------------------------------------+
|                                                       |
|       Kernel 空间
|                                                       |
+-------------------------------------------------------+ 0xC0000000
|                                                       |
|                                                       | 随机偏移
|                                                       |
+-------------------------------------------------------+
|                                                       |
|       栈 向下增长
|                                                       |
+-------------------------------------------------------+
|                                                       |
|                                                       |
|                                                       |
+-------------------------------------------------------+
|                                                       |
|                                                       |
|       内存映射段 (文件映射，匿名映射 /lib/libc.so)
|                                                       |
+-------------------------------------------------------+
|                                                       |
|                                                       |
|                                                       |
+-------------------------------------------------------+ brk();
|                                                       |
|       堆 (向上增长)
|                                                       | brk();开始
+-------------------------------------------------------+ 
|                                                       |
|                                                       | 随机 brk 偏移
|                                                       |
+-------------------------------------------------------+
|                                                       |
|       BSS段                                            |
|       静态变量(0填充，如 static int a;)                       |
|                                                       |
|                                                       |
+-------------------------------------------------------+
|                                                       | end data
|       数据段                                             |
|     被程序员初始化的静态变量(如 static int a = 0;)    |
|                                                       | start data
+-------------------------------------------------------+
|                                                       | end code
|       程序代码区
|       存放函数体的二进制代码
|                                                       | start code
+-------------------------------------------------------+ 0x08048000
|                                                       |
|                                                       |
|                                                       |
|                                                       |
+-------------------------------------------------------+ 0x00000000
    
#endif

SYSCALL_DEFINE1(brk, unsigned long, brk)
{
	unsigned long retval;
	unsigned long newbrk, oldbrk, origbrk;
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *next;
	unsigned long min_brk;
	bool populate;
	bool downgraded = false;
	LIST_HEAD(uf);

	if (down_write_killable(&mm->mmap_sem))
		return -EINTR;

	origbrk = mm->brk;

#ifdef CONFIG_COMPAT_BRK
	/*
	 * CONFIG_COMPAT_BRK can still be overridden by setting
	 * randomize_va_space to 2, which will still cause mm->start_brk
	 * to be arbitrarily shifted
	 */
	if (current->brk_randomized)
		min_brk = mm->start_brk;
	else
		min_brk = mm->end_data;
#else
	min_brk = mm->start_brk;
#endif
	if (brk < min_brk)
		goto out;

	/*
	 * Check against rlimit here. If this check is done later after the test
	 * of oldbrk with newbrk then it can escape the test and let the data
	 * segment grow beyond its set limit the in case where the limit is
	 * not page aligned -Ram Gupta
	 */
	if (check_data_rlimit(rlimit(RLIMIT_DATA), brk, mm->start_brk,
			      mm->end_data, mm->start_data))
		goto out;

	newbrk = PAGE_ALIGN(brk);
	oldbrk = PAGE_ALIGN(mm->brk);
	if (oldbrk == newbrk) {
		mm->brk = brk;
		goto success;
	}

	/*
	 * Always allow shrinking brk.
	 * __do_munmap() may downgrade mmap_sem to read.
	 */
	if (brk <= mm->brk) {
		int ret;

		/*
		 * mm->brk must to be protected by write mmap_sem so update it
		 * before downgrading mmap_sem. When __do_munmap() fails,
		 * mm->brk will be restored from origbrk.
		 */
		mm->brk = brk;
		ret = __do_munmap(mm, newbrk, oldbrk-newbrk, &uf, true);
		if (ret < 0) {
			mm->brk = origbrk;
			goto out;
		} else if (ret == 1) {
			downgraded = true;
		}
		goto success;
	}

	/* Check against existing mmap mappings. */
	next = find_vma(mm, oldbrk);
	if (next && newbrk + PAGE_SIZE > vm_start_gap(next))
		goto out;

	/* Ok, looks good - let it rip. */
	if (do_brk_flags(oldbrk, newbrk-oldbrk, 0, &uf) < 0)
		goto out;
	mm->brk = brk;

success:
	populate = newbrk > oldbrk && (mm->def_flags & VM_LOCKED) != 0;
	if (downgraded)
		up_read(&mm->mmap_sem);
	else
		up_write(&mm->mmap_sem);
	userfaultfd_unmap_complete(mm, &uf);
	if (populate)
		mm_populate(oldbrk, newbrk - oldbrk);
	return brk;

out:
	retval = origbrk;
	up_write(&mm->mmap_sem);
	return retval;
}

