/******************************************************************************
 * File name:   watchdog.c
 * Developer:   Lior Katz
 * Date:        2019-05-06 12:53:56
 * Reviewer:
 ******************************************************************************/
#define _XOPEN_SOURCE 600

#include <stdio.h>      /* sprintf */
#include <assert.h>     /* assert */
#include <unistd.h>     /* fork */
#include <pthread.h>    /* create */
#include <stdlib.h>     /* setenv */

#include "watchdog_utils.h"
#include "shared_watchdog.h"
#include "watchdog.h"

/*************************** CommunicationThread ******************************/

/*	Description:   Communication Thread - manages communication for the app
 *                 revives Watchdog incase it falls
 *	Input:		   app_argv - incase app falls watchdog will revive
  *                the app with the args list.
 *	Return Values: NULL.
 */
static void *CommunicateThread(void *app_argv);

/******************************* Globals **************************************/
static int g_semid;
static pthread_t g_comm_thread_id = 0;
/*************************** CommunicationThread ******************************/
static void *CommunicateThread(void *app_argv)
{
    assert(app_argv);

    /* start communication with watchdog */
    CommunicateAndRevive(app_argv);

    return (NULL);
}

/******************************************************************************/
int WDKeepMeAlive(int argc, char **app_argv)
{
    status_t status          = SUCCESS;
    short int init_sem_values[SEM_SET_SIZE] = {0};

    /* create and initialize semaphore wait operation struct */
    struct sembuf sem_wait = {WD_ALIVE_SEM_INDX, SEM_WAIT, 0};

    /* set path to revivee - who to revive */
    setenv(PATH_ENV_NAME, WD_PATH, OVERWRITE);

    UNUSED(argc);

    /* check if thread creation succeeded */
    if (0 == pthread_create(&g_comm_thread_id,
                            NULL,
                            CommunicateThread,
                            app_argv))
    {
        /* when the app invokes the watchdog fot the first time */
        if (!WatchDogExists())
        {
            /* gain access to the shared semaphore set */
            g_semid = semget(SEM_SET_KEY, SEM_SET_SIZE, 0666 | IPC_CREAT);

            if (-1 != g_semid)
            {
                /* initialize all semset */
                semctl(g_semid, SEM_SET_SIZE, SETALL, &init_sem_values);

                /* wait for watchdog process to post */
                while (-1 == semop(g_semid, &sem_wait, 1))
                {
                    ;
                }
            }
            else
            {
                status = FAILURE;
            }
        }
    }
    else
    {
        status = FAILURE;
    }


    return (status);
}

/******************************** LetMeDie ************************************/
void WDLetMeDie(void)
{
    char *communicate_pid_str = NULL;
    pid_t communicate_pid = 0;
    int status;

    communicate_pid_str = getenv(PID_ENV_NAME);

    assert(communicate_pid_str);

    communicate_pid = atoi(communicate_pid_str);

    /* send stop signals to both thread and watchdog */
    kill(communicate_pid, SIGUSR2);
    kill(getpid(), SIGUSR2);

    /* wait for the thread to complete */
    pthread_join(g_comm_thread_id, NULL);

    /* wait for the child processes to terminate
     * so no zombie processes are created,*/
    while (-1 == waitpid(communicate_pid, &status, 0) && (errno != ECHILD)){}

    /* deallocate semaphore set from memory */
    semctl(g_semid, SEM_SET_SIZE, IPC_RMID, 0);
}
