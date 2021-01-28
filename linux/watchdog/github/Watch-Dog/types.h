/*******************************************************************************
*	Filename    :	types.h
*	Developer	:   Esty
*	Last Update :	2019-02-20
*	Description :	Types header 1
*******************************************************************************/

#ifndef _TYPES_H_
#define _TYPES_H_

enum
{
	FAIL   = -1,
	DONE   = 0,
	REPEAT = 1
};
/*******************************************************************************
 *  Description:   task_func_t is a pointer to a function which executes a task.
 *
 *  Input:         data is a pointer to user data.
 *
 *  Return values: FAIL   (-1): task has failed.
 *       		   DONE   (0) : task is done and doesn't require any more calls.
 *			       REPEAT (1) : task has executed successfully and requires
 *				   				repeat calls to complete.
 */
typedef int (*task_func_t)(void *data);

#endif     /* _TYPES_H_ */
