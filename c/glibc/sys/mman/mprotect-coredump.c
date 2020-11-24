
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define debug() printf("%s:%d\n", __func__, __LINE__);
#ifndef offsetof
#define offsetof(type, number) __builtin_offsetof(type, number)
#endif


struct vos_task {
    char task_name[32];
    
    unsigned long protect1;
    unsigned long task_pid;
    unsigned long protect2;
};

static int nr_tasks;
static struct vos_task* tasks;


void segv_handler (int signal_number)
{
	printf ("You should not access tasks vars(SIGSEGV) = %d\n", signal_number);
    exit(1);
}
int main ()
{
	int fd;
	struct sigaction sa;
    
	/* 注册新号回调 */
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &segv_handler;
	sigaction (SIGSEGV, &sa, NULL);
	
	/* 总任务数 */
	nr_tasks = 10;
    
	fd = open("/dev/zero", O_RDONLY);
	tasks = mmap(NULL, sizeof(struct vos_task)*nr_tasks, PROT_WRITE|PROT_READ, MAP_PRIVATE, fd, 0);
	close (fd);
    
	/* 初始化任务结构体 */
	strcpy(tasks[0].task_name, "OM-DU");
    tasks[0].task_pid = 1234;

    printf("%p\n", tasks);
    printf("%p,(%d)\n", tasks + offsetof(struct vos_task, protect1), offsetof(struct vos_task, protect1));
    printf("%p,(%d)\n", tasks + offsetof(struct vos_task, task_pid), offsetof(struct vos_task, task_pid));
    printf("%p,(%d)\n", tasks + offsetof(struct vos_task, protect2), offsetof(struct vos_task, protect2));

    
	/* 只读 */
#if 1 /* 这里保护不起作用,需要时pagesize的大小 4K  */
	mprotect(tasks + offsetof(struct vos_task, protect1), sizeof(struct vos_task), PROT_NONE);
	mprotect(tasks + offsetof(struct vos_task, protect2), sizeof(struct vos_task), PROT_NONE);
#else
	mprotect(tasks, sizeof(struct vos_task)*nr_tasks, PROT_READ);
#endif

	strcpy(tasks[0].task_name, "OM-DU-new");
    tasks[0].protect1 = 1234;
    tasks[0].task_pid = 1234;
    tasks[0].protect2 = 1234;

    printf("TASK: %s %ld\n", tasks[0].task_name, tasks[0].task_pid);
    
	munmap (tasks, sizeof(struct vos_task)*nr_tasks);
	
	return 0;
}


