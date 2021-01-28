/******************************************************************************
 * File name:   watchdog.c
 * Developer:   Lior Katz 
 * Date:        2019-05-06 12:53:56
 * Reviewer:
 ******************************************************************************/
#define _POSIX_C_SOURCE 199309L
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <signal.h>      /* sigaction */
#include <stdlib.h>      /* setenv */
#include <unistd.h>      /* getpid */
#include <string.h>      /* memset */
#include <time.h>         /* time */

#include "watchdog_utils.h"
#include "scheduler.h"
#include "types.h"
#include "shared_watchdog.h"

/********************************** macros ************************************/
#define MAX_PID_DIGITS (5)
#define RETRIES (5)
#define INTERVAL (2)

/*********************************** enum *************************************/
enum
{
    DIE = 0,
    REVIVE_ME = 1
};

/********************************* globals ************************************/
static size_t g_counter;
static int g_keep_alive = REVIVE_ME;

/********************************* typedef ************************************/
typedef void (*signal_handler_t)(int);

/************************ inner functions declarations ************************/
/*	Description:   revives the fallen process by fork and execvp, path to the
 *                 protected process is in PATH_ENV_NAME env variable, sets the
 *                 new process id in the PID_ENV_NAME env variable.
 *                 for the parent child is set in through the child pointer
 *                 for the child who communicate with is set from PID_ENV_NAME
 *
 *	Input:		   pointer to new child, argv of the caliing process.
 *
 *	Return Values: SUCCESS / FAILURE
 */
static status_t Revive(pid_t *child, char **argv);

/*	Description:   set signal handlers for the calling process
 *
 *	Input:		   sig_num -signal to set handler,
 *                 handler - pointer to type one handler
 *
 *	Return Values: SUCCESS / FAILURE
 */
static void SetHandler(int sig_num, signal_handler_t handler);

/****************************** Signal Handlers *******************************/
/* SIGUSR1 handler */
void HandleSigUsr1(int signal);

/* SIGUSR2 handler */
void HandleSigUsr2(int signal);

/*********************************** Tasks ************************************/
/********* SignalAlive signal i'm alive ********/
int SignalAlive(void *pid)
{
    printf("%d sending to %d\n", getpid(), *(pid_t *)pid);

    kill(*(pid_t *)pid, SIGUSR1);

    return (REPEAT);
}

/***** CheckAlive check if watched is alive *****/
int CheckAlive(void *scheduler)
{
    __sync_fetch_and_add(&g_counter, 1);
    if (RETRIES <= g_counter || DIE == g_keep_alive)
    {

        printf("%d will Revive.\n g_counter = %lu, keep_alive = %d\n", getpid(),g_counter,  g_keep_alive);
        SchedulerStop((scheduler_t *)scheduler);
    }

    return (REPEAT);
}

/********************** CommunicateAndRevive **********************************/
status_t CommunicateAndRevive(char **argv)
{
    scheduler_t *sched = NULL;
    status_t status = SUCCESS;
    time_t start_tm = 0;
    pid_t communicate_with_pid = 0;
    pid_t parent_pid = 0;
    char *pid_env_ptr = NULL;
    int semid = 0;

    /* create and initialize semaphore operation structs */
    struct sembuf sem_post = {0, SEM_POST, 0};
    struct sembuf sem_wait = {0, SEM_WAIT, 0};

    /* gain access to the shared semaphore set */
    semid = semget(SEM_SET_KEY, SEM_SET_SIZE, 0666 | IPC_CREAT);

    /* set handlers for exec failure detection */
    SetHandler(SIGUSR1, HandleSigUsr1);
    SetHandler(SIGUSR2, HandleSigUsr2);

    /* set scheduler start time */
    start_tm = time(NULL);

    /* create scheduler */
    sched = SchedulerCreate();
    if ((NULL != sched) && (-1 != semid))
    {
        /* add tasks to the scheduler */
        SchedulerAddTask(sched,SignalAlive,(void *)&communicate_with_pid, start_tm, INTERVAL);
        SchedulerAddTask(sched,CheckAlive,(void *)sched, start_tm, INTERVAL);

        /* keep the other process alive */
        while (g_keep_alive)
        {
            pid_env_ptr = getenv(PID_ENV_NAME);
            parent_pid = getppid();

            /* watchdog doesnt exist - Revive him */
            if (!WatchDogExists())
            {
                /* Revive the process */
                status = Revive((pid_t *)&communicate_with_pid ,argv);
            }
            /* if im the child */
            else if (parent_pid == atoi(pid_env_ptr))
            {
                communicate_with_pid = parent_pid;
            }

            printf("%s %d Communicating with %d\n",argv[0],getpid(),communicate_with_pid);
            /* sync the two processes before Communicating */

            /* set who to wait/post */
            sem_post.sem_num = (0 == strcmp(argv[0], WD_PATH)) ?
                                          (WATCHDOG_SEM_INDX) :
                                          (APP_SEM_INDX);


            sem_wait.sem_num = !(sem_post.sem_num);

            /* post to the second process */
            semop(semid, &sem_post, 1);

            /* wait for the second process to post */
            while (-1 == semop(semid, &sem_wait, 1)){}

            /* Communicate */
            SchedulerRun(sched);

            /* process down - Revive or not */
            if (g_keep_alive)
            {
                printf("keep_alive: %d\n", g_keep_alive);
                status = Revive((pid_t *)&communicate_with_pid ,argv);
            }

        }

        status = SUCCESS;

        SchedulerDestroy(sched);
        sched = NULL;
    }

    return (status);
}

/****************************** SetHandler ************************************/
static void SetHandler(int sig_num, signal_handler_t handler)
{
    struct sigaction sa_action;

    memset(&sa_action, 0, sizeof(struct sigaction));
    sa_action.sa_handler = handler;
    sigaction (sig_num, &sa_action, NULL);
}

/****************************** Signal Handlers *******************************/
/* SIGUSR1 handler */
void HandleSigUsr1(int signal)
{
    UNUSED(signal);
    printf("im %d Handler recieved !!\n", getpid() );
    g_counter = 0;
}

/* SIGUSR2 handler */
void HandleSigUsr2(int signal)
{
    UNUSED(signal);
    printf("%d got sig 2 kepp\n", getpid());

    g_keep_alive = DIE;
}

/****************************** Signal Handlers *******************************/
/* return failure/success */
static status_t Revive(pid_t *communicate_with_pid, char **argv)
{
    int status = SUCCESS;
    pid_t child = 0;
    char *swap_tmp = NULL;
    char child_pid_str[MAX_PID_DIGITS + 1] = "";
    char parent_pid_str[MAX_PID_DIGITS + 1] = "";

    child = fork();

    if (0 == child)
    {
        /* swap the exec command */
        swap_tmp = getenv(PATH_ENV_NAME);
        setenv(PATH_ENV_NAME, argv[0], OVERWRITE);
        argv[0] = swap_tmp;
        printf(".............child................| %s %s\n", argv[0], argv[1]);

        /* set who to Communicate with */
        sprintf(parent_pid_str, "%d", getppid());
        setenv(PID_ENV_NAME, parent_pid_str, OVERWRITE);

        /* exec fallen process */
        execvp(argv[0], (char *const *)argv);

        /* child */
        perror("error");

        abort();
    }
    else if (child > 0)
    {
        /*parent*/
        *communicate_with_pid = child;

        /* set the child pid into env PID_ENV_NAME */
        sprintf(child_pid_str, "%d", child);
        setenv(PID_ENV_NAME, child_pid_str, OVERWRITE);

        printf(".............parent................| %s %s\n", argv[0], argv[1]);
    }
    else
    {
        perror("");
        status = FAILURE;
    }

    return (status);

}

/*************************** IsWatchDogExists ********************************/
boolean_t WatchDogExists(void)
{
    /* if comm_pid doesnt exist -> watchdog doesnt exist */
    return (NULL != getenv(PID_ENV_NAME));
}
