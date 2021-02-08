#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <sched.h>
#include <unistd.h>

#include <stdbool.h>
#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <syscall.h>

#define gettid() syscall(__NR_gettid)

/* Both Process and CPU ids should be positive numbers. */
static int convert_str_to_int(char* begin)
{
    if (!begin)
    {
        errx(1, "Invalid arguments for %s", __func__);
    }

    errno = 0;
    char *end = NULL;
    long num = strtol(begin, &end, 10);
    if (errno || (*end != '\0') || (num > INT_MAX) || (num < 0))
    {
        errx(1, "Invalid integer: %s", begin);
    }
    return (int)num;
}

/*
 * The cpu list should like 1-3,6
 */
static void parse_cpu_list(char* cpu_list, cpu_set_t* cpu_set)
{
    if (!cpu_list || !cpu_set)
    {
        errx(1, "Invalid arguments for %s", __func__);
    }
	int i;
    char* begin = cpu_list;
    while (1)
    {
        bool last_token = false;
        char* end = strchr(begin, ',');
        if (!end)
        {
            last_token = true;
        }
        else
        {
            *end = '\0';
        }

        char* hyphen = strchr(begin, '-');
        if (hyphen)
        {
            *hyphen = '\0';
            int first_cpu = convert_str_to_int(begin);
            int last_cpu = convert_str_to_int(hyphen + 1);
            if ((first_cpu > last_cpu) || (last_cpu >= CPU_SETSIZE))
            {
                errx(1, "Invalid cpu list: %s", cpu_list);
            }
            for (i = first_cpu; i <= last_cpu; i++)
            {
                CPU_SET(i, cpu_set);
            }
        }
        else
        {
            CPU_SET(convert_str_to_int(begin), cpu_set);
        }

        if (last_token)
        {
            break;
        }
        else
        {
            begin = end + 1;
        }
    }
}

int my_taskset(int pid, char *cpu_list)
{
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    parse_cpu_list(cpu_list, &cpu_set);
    sched_setaffinity(pid, sizeof(cpu_set_t), &cpu_set);
    
}

struct task_arg {
    char cpu_list[128];
};

void *task_routine(void*arg)
{
    struct task_arg *Arg = ( struct task_arg *)arg;
    cpu_set_t cpu_set;
    int i;
    my_taskset(gettid(), Arg->cpu_list);

    while(1) {
        if (sched_getaffinity(gettid(), sizeof(cpu_set_t), &cpu_set))
        {
            err(1, "sched_getaffinity");
        }
        printf("Process (%d) bonds to CPU:", gettid());
        for (i = 0; i < CPU_SETSIZE; i++)
        {
            if (CPU_ISSET(i, &cpu_set))
            {
                printf(" %d", i);
            }
        }
        printf("\n");
        sleep(3);
    }
    
    pthread_exit(NULL);
}


int main(int argc, char**argv)
{   

    pthread_t task1, task2;

    struct task_arg task_args[] = {
        {"1,3"},
        {"2"},
    };

    pthread_create(&task1, NULL, task_routine, &task_args[0]);
    pthread_create(&task2, NULL, task_routine, &task_args[1]);

	pthread_setname_np(task1, "t1");
	pthread_setname_np(task2, "t2");

    pthread_join(task1, NULL);
    pthread_join(task2, NULL);

    return 0;
}

