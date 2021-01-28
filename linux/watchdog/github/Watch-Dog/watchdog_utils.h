/*******************************************************************************
 * File name  : watchdog_utils.h
 * Developer:   Lior Katz
 * Date		  : 2019-05-8
 ******************************************************************************/
#ifndef __WATCHDOG_UTILS_H__
#define __WATCHDOG_UTILS_H__

#include <fcntl.h>      /* S_IRWXU */
#include <sys/ipc.h>    /* semaphore */
#include <sys/sem.h>    /* semaphore */
#include <errno.h>	    /* errno */

#include <sys/wait.h>
#include <sys/types.h>

#define UNUSED(var) ((void)(var))

#define OVERWRITE (1)
#define WD_PATH ("./main_watchdog.out")
#define PATH_ENV_NAME ("revivee_path")
#define PID_ENV_NAME ("comm_pid")

/* semaphore macros */
#define SEM_SET_KEY (37)
#define SEM_SET_SIZE (3)

/* short int */
#define SEM_WAIT (-1)
#define SEM_POST (1)

/* 0 and 1 are used for inner sync */
#define WATCHDOG_SEM_INDX (0)
#define APP_SEM_INDX (1)
#define WD_ALIVE_SEM_INDX (2)


#endif     /* __WATCHDOG_UTILS_H__ */
