/******************************************************************************
 * File name:   watchdog.c
 * Developer:   Esty
 * Date:        2019-05-06 12:53:56
 * Reviewer:    Aya
 ******************************************************************************/
#define _XOPEN_SOURCE 600

#include <stdio.h>       /* printf */
#include <assert.h>      /* assert */

#include "watchdog_utils.h"
#include "shared_watchdog.h"

int main(int argc, char const *argv[])
{
    int semid = 0;
    int status;

    /* create and initialize semaphore operation struct */
    struct sembuf sem_post = {WD_ALIVE_SEM_INDX, SEM_POST, 0};

    printf("\n\n\tWATCHDOG IS ALIVE\n\n");

    /* gain access to the shared semaphore set */
    semid = semget(SEM_SET_KEY, SEM_SET_SIZE, 0666 | IPC_CREAT);

    /* signal watchdog is up and running */
    semop(semid, &sem_post, 1);

    /* communicate */
    CommunicateAndRevive((char **)argv);

    /* wait for the child processes to terminate
     * so no zombie processes are created */
    while (-1 == wait(&status) && (errno != ECHILD)){}

    printf("\n\n\tWATCHDOG IS DEAD\n\n");
    return 0;
}
