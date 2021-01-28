/*******************************************************************************
*	Filename    :	scheduler.c
*	Developer	:	Boris Vaintrub
*	Last Update :	2019-02-21
*	Description :	scheduler version 1
*******************************************************************************/
/**** Includes ************************/
#include <stdlib.h> /*	malloc, free  */
#include <unistd.h>	/*	sleep 		  */
#include <assert.h>	/*	assert		  */
#include <stdio.h>	/*	stderr		  */
#include <time.h>	/*	time 		  */

#include "scheduler.h"
#include "task.h"
#include "pqueue.h"

/**** Macros **************************/
#define	UNUSED(x) (void)(x)

/**** Structs *************************/
struct scheduler
{
	pqueue_t *queue;
	int to_stop;
};

/**** Internal functions **************/
static int IsHappaningBefore(void *current_data, 
							 const void *new_data, void *param);
static int IsSameID (void *queue_data, void *id);

/**** Functions ***********************/
/******************************** SchedulerCreate *****************************/
scheduler_t *SchedulerCreate(void)
{
	scheduler_t *sc = NULL;

	sc = (scheduler_t *) malloc(sizeof(scheduler_t));
	if (NULL == sc)
	{
		return (NULL);
	}

	sc->queue = PQCreate(IsHappaningBefore);
	if (NULL == sc->queue)
	{
		free(sc);
		sc = NULL;
		return (NULL);
	}

	return (sc);
}

/******************************** SchedulerDestroy ****************************/
void SchedulerDestroy(scheduler_t *scheduler)
{
	assert(scheduler);

	while (!SchedulerIsEmpty(scheduler))
	{
		TaskDestroy(PQPop(scheduler->queue));
	}

	PQDestroy(scheduler->queue);
	scheduler->queue = NULL;
	scheduler->to_stop = 0;

	free(scheduler);
	scheduler = NULL;
}

/****************************** SchedulerAddTask ******************************/
unique_id_t SchedulerAddTask(scheduler_t *scheduler, task_func_t task_func,
	 				void * data, time_t start_time, time_t interval)
{
	task_t *new_task = NULL;

	assert(scheduler);
	assert(task_func);

	new_task = TaskCreate(task_func, data, start_time, interval);
	if (NULL == new_task)
	{
		return (UIDGenerateBad());
	}

	if (SUCCESS != PQPush(scheduler->queue, new_task, NULL))
	{
		return (UIDGenerateBad());
	}

	return (TaskGetId(new_task));
}

/**************************** SchedulerRemoveTask *****************************/
int SchedulerRemoveTask(scheduler_t *scheduler, unique_id_t id)
{
	void *data = NULL;

	assert(scheduler);

	data = PQErase(scheduler->queue, IsSameID, &id);

	if (NULL == data)
	{
		return (FAILURE);
	}

	TaskDestroy(data);

	return (SUCCESS);
}

/****************************** SchedulerRun **********************************/
int SchedulerRun(scheduler_t *scheduler)
{
	void *temp_data = NULL;
	time_t sleep_time =0;

	assert(scheduler);

	scheduler->to_stop = 0;

	while (!SchedulerIsEmpty(scheduler) && !(scheduler->to_stop))
	{
		/*wait to next task to execute*/
		temp_data = PQPeek(scheduler->queue);
		if (TaskGetRunTime(temp_data) > time(NULL))
		{
			sleep_time = TaskGetRunTime(temp_data) - time(NULL);
			while (sleep_time)
			{
				sleep_time = sleep(sleep_time);
			}
		}

		/*updating queue depending on task return value*/
		temp_data = PQPop(scheduler->queue);
		switch (TaskRun(temp_data))
		{
			case FAIL:
				fprintf(stderr, "function failed at: %ld\n", time(NULL));
				TaskDestroy(temp_data);
				break;
			case DONE:
				TaskDestroy(temp_data);
				break;
			case REPEAT:
				TaskUpdateRunTime(temp_data);
				if (SUCCESS != PQPush(scheduler->queue, temp_data, NULL))
				{
					TaskDestroy(temp_data);
					fprintf(stderr, "alloc failed at:%ld\n", time(NULL));
				}
			default:
				break;
		}
	}

	return (scheduler->to_stop ? STOP : COMPLETE);
}

/****************************** SchedulerStop **********************************/
void SchedulerStop(scheduler_t *scheduler)
{
	assert(scheduler);

	scheduler->to_stop = TRUE;
}

/****************************** SchedulerSize *********************************/
size_t SchedulerSize(scheduler_t *scheduler)
{
	assert(scheduler);

	return (PQSize(scheduler->queue));
}

/****************************** SchedulerIsEmpty ******************************/
int SchedulerIsEmpty(scheduler_t *scheduler)
{
	assert(scheduler);

	return (0 == SchedulerSize(scheduler));
}

/****************************** IsHappaningBefore *****************************/
static int IsHappaningBefore(void *current_data, 
							 	   const void *new_data, void *param)
{
	assert(current_data);
	assert(new_data);

	UNUSED(param);

	return (TaskGetRunTime((task_t *) current_data) > 
			TaskGetRunTime((task_t *) new_data));
}

/****************************** IsSameID **************************************/
static int IsSameID (void *queue_data, void *id)
{
	assert (queue_data);
	assert(!UIDIsBad(*(unique_id_t *) id));

	return (UIDIsSame(TaskGetId((task_t *) queue_data), *(unique_id_t *) id));
}