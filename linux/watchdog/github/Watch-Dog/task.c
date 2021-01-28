/*******************************************************************************
*	Filename    :	task.c
*	Developer	:	Lior Katz
*	Last Update :	2019-02-20
*	Description :	task version 1
*******************************************************************************/
#include <assert.h>	/*assert		*/
#include <stdlib.h>	/*malloc, free	*/
#include <string.h> /*memset		*/
#include <time.h>	/*time 			*/

#include "task.h"

struct task
{
	unique_id_t id;
	time_t run_time;
	time_t interval;
	int(*task_func)(void *data);
	void *data;
};

/******************************** TaskCreate **********************************/
task_t *TaskCreate(task_func_t task_func, void *data,
		  		   time_t run_time, time_t interval)
{
	task_t *task = NULL;

	assert(task_func);

	task = (task_t *)malloc(sizeof(task_t));
	if (NULL != task)
	{
		task->id = UIDCreate();
		task->data = data;
		task->run_time = run_time;
		task->interval = interval;
		task->task_func = task_func;
	}

	return (task);
}

/******************************** TaskDestroy *********************************/
void TaskDestroy(task_t *task)
{
	assert(task);

	memset(task, 0, sizeof(task_t));
	free(task);
	task = NULL;
}

/******************************** TaskGetId ***********************************/
unique_id_t TaskGetId(task_t *task)
{
	assert(task);

	return (task->id);
}

/****************************** TaskGetRunTime ********************************/
time_t TaskGetRunTime(task_t *task)
{
	assert(task);

	return (task->run_time);
}

/*************************** TaskUpdateRunTime ********************************/
void TaskUpdateRunTime(task_t *task)
{
	assert(task);

	task->run_time = time(NULL) + task->interval;
}

/******************************** TaskRun *************************************/
int TaskRun(task_t *task)
{
	assert(task);

	return (task->task_func(task->data));
}
