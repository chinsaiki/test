/*******************************************************************************
*	Filename    :	task.h
*	Developer	:	Lior Katz
*	Last Update :	2019-02-20
*	Description :	task version 1
*******************************************************************************/

#ifndef _TASK_H_
#define _TASK_H_

#include <stddef.h> /* size_t */

#include "types.h"
#include "uid.h"

/*******************************************************************************
 *  Description:   Holds the struct task.
 */
typedef struct task task_t;


/******************************** TaskCreate ***********************************
 *	Description:   Creates a new task.
 *
 *	Input:		   task_func  - pointer to a function to be executed.
 *                 data		  - pointer to data to be used by task_func.
 *				   start_time - time to start execution of the task.
 *								start_time is absolute defined by caller.
 *				   interval   - time interval to next execution of the task,in
 * 								seconds,as long as the function has returned
 *								REPEAT.
 *
 *	Return Values: On success - returns a pointer to the new task.
 *				   Otherwise  - returns NULL.
 *
 *	Complexity:	   O(1)
 */
task_t *TaskCreate(task_func_t task_func, void * data,
		  		   time_t start_time, time_t interval);

/******************************** TaskDestroy **********************************
 *	Description:   Destroys a task.
 *
 *	Input:		   task_t*    - pointer to a task to destroy.
 *
 *	Return Values: None.
 *
 *	Complexity:	   O(1)
 */
void TaskDestroy(task_t *task);

/******************************** TaskGetId ************************************
 *	Description:   Returns a task id.
 *
 *	Input:		   task_t *   - pointer to task.
 *
 *	Return Values: Returns the task unique ID.
 *
 *	Complexity:	   O(1)
 */
unique_id_t TaskGetId(task_t *task);

/****************************** TaskGetRunTime *********************************
 *	Description:   Returns a task start time.
 *
 *	Input:		   task_t *   - pointer to task.
 *                 time_to_add
 *
 *	Return Values: Returns the task start time.
 *
 *	Complexity:	   O(1)
 */
time_t TaskGetRunTime(task_t *task);

/*************************** TaskUpdateRunTime *********************************
 *	Description:   Updates the task run time.
 * 				   run time will be updated to current time + interval.
 *
 *	Input:		   task_t *   - pointer to task.
 *
 *	Return Values: None.
 *
 *	Complexity:	   O(1)
 */
void TaskUpdateRunTime(task_t *task);

/******************************** TaskRun **************************************
 *	Description:   Runs task by executing task_func.
 *
 *	Input:		   task_t *   - pointer to task.
 *
 *  Return values: FAIL   (-1): task has failed.
 *       		   DONE   (0) : task is done and doesn't require any more calls.
 *			       REPEAT (1) : task has executed successfully and requires
 *				   repeat calls to complete.
 *
 *	Complexity:	   O(1)
 */
int TaskRun(task_t *task);

#endif     /* _TASK_H_ */
