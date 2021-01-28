/*******************************************************************************
*	Filename    :	scheduler.h
*	Developer	:	Lior Katz
*	Last Update :	2019-02-24
*   Version     :   3
*******************************************************************************/
#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <sys/types.h> /* size_t time_t*/

#include "general_types.h"
#include "types.h"
#include "uid.h"

enum state
{
	STOP,
	COMPLETE
};

/*******************************************************************************
 *  Description:   Holds the struct scheduler.
 */
typedef struct scheduler scheduler_t;

/******************************** SchedulerCreate ******************************
 *	Description:   Creates a new scheduler.
 *
 *	Input:		   None.
 *
 *	Return Values: On success    - returns a pointer to the new scheduler.
 *				   Otherwise     - returns NULL.
 *
 *	Complexity:	   O(1)
 */
scheduler_t *SchedulerCreate(void);

/******************************** SchedulerDestroy *****************************
 *	Description:   Destroys a scheduler and release the memory of the scheduler
 *				   and tasks inside.
 *tasks
 *	Input:		   scheduler_t*  - pointer to a scheduler to destroy.
 *
 *	Return Values: None.
 *
 *	Complexity:	   O(n)
 */
void SchedulerDestroy(scheduler_t *scheduler);

/****************************** SchedulerAddTask *******************************
 *	Description:   Adds new task to task to scheduler.
 *
 *	Input:		   scheduler_t * - pointer to a scheduler.
 *				   task_func	 - pointer to a function to be executed by task.
 *                 data		  	 - pointer to data to be used by task_func.
 *				   start_time 	 - time to start execution of the task.
 *								   start_time is absolute time defined by
 * 								   caller.
 *				   interval   	 - time interval to next execution in seconds
 *								   of the task, as long as the function has
 *								   returned REPEAT. end of task
 *
 *	Return Values: On success    - id of the added task.
 				   othrwise      - Bad id
 *
 *	Complexity:	   O(n)
 */
unique_id_t SchedulerAddTask(scheduler_t *scheduler, task_func_t task_func,
	 				void * data, time_t start_time, time_t interval);

/**************************** SchedulerRemoveTask *****
	FAIL   = -1,
	DONE   = 0,
	REPEAT = 1
};**************************
 *	Description:   Remove a task from a scheduler.
 *
 *	Input:		   scheduler_t * - pointer to a scheduler.
 *				   unique_id_t   - uid of a task to remove.
 *
 *	Return Values: SUCCESS 		 - task removed
 *				   FAILURE			 - task not removed/foundTODO: status enum
 *	Complexity:	   O(n)
 */
int SchedulerRemoveTask(scheduler_t *scheduler, unique_id_t id);

/****************************** SchedulerRun ***********************************
 *	Description:   Executes the scheduler to run all the tasks.
 *
 *	Input:		   scheduler_t * - pointer to a scheduler.
 *
 *	Return Values: STOP 		 - scheduler was stopped by task.
 *				   COMPLETE 	 - all tasks done.
 *
 *	Complexity:	   O(n)
 */
int SchedulerRun(scheduler_t *scheduler);

/****************************** SchedulerStop ***********************************
 *	Description:   Stop a scheduler from runnning.
 *
 *	Input:		   scheduler_t * - pointer to a scheduler.
 *
 *	Return Values: None.
 *
 *	Complexity:	   O(1)
 */
void SchedulerStop(scheduler_t *scheduler);

/****************************** SchedulerSize **********************************
 *	Description:   Number of tasks in scheduler.
 *
 *	Input:		   scheduler_t * - pointer to a scheduler.
 *
 *	Return Values: size_t        - number of tasks in the scheduler.
 *
 *	Complexity:	   O(n)
 */
size_t SchedulerSize(scheduler_t *scheduler);

/****************************** SchedulerIsEmpty *******************************
 *	Description:   Checks if a scheduler is empty.
 *
 *	Input:		   scheduler_t * - pointer to a scheduler.
 *
 *	Return Values: int           - (1) scheduler is empty.
 *							     - (0) otherwise.
 *
 *	Complexity:	   O(1)
 */
int SchedulerIsEmpty(scheduler_t *scheduler);

#endif     /* _SCHEDULER_H_ */
