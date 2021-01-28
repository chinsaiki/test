/******************************************************************************
 * File name:   shared_watchdog.h
 * Developer:   Lior Katz
 * Date:        2019-05-06
 ******************************************************************************/

#ifndef __SHARED_WATCHDOG_H__
#define __SHARED_WATCHDOG_H__

#include <stddef.h>      /* size_t */
#include "general_types.h"

/*	Description:   Checks if watchdog exists already or not
 *	Input:		   n/a.
 *	Return Values: TRUE if exists, FALSE if not.
 */
boolean_t WatchDogExists(void);

/*	Description:   Communicate and revive if fallen/ doesnt exist.
 *                 recives the argv of the process, the path to the protected
 *                 process is in PATH_ENV_NAME env variable, if it doesnt exist
 *                 it will revive the process and establish communication
 *                 by sync, first is the app that revives the watchdog so its
 *                 argv argumnets are past on during the cycle
 *
 *	Input:		   argv
 *
 *	Return Values: SUCCESS / FAILURE
 */
status_t CommunicateAndRevive(char **argv);

#endif     /* __SHARED_WATCHDOG_H__ */
