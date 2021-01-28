/*******************************************************************************
*	Filename 	:	pqueue.h
*	Developer	:	Lior Katz
*	Last Update :	2019-02-18
*	Description :	pqueue header file version 1
*******************************************************************************/
#ifndef _PQUEUE_H_
#define _PQUEUE_H_

#include <stddef.h> /* size_t */

#include "general_types.h"

/*  has_higher_priority_t is a user defined function, that determines the
 *	priority policy for the queue.it is given to PQCreate as an argument.
 *  Function receives three arguments and returns a boolean value.
 *
 *  Arguments:
 *  ----------
 *  list_data represents the existing data in the list.
 *  new_data represents the new data to be inserted.
 *  param is used by the user as a parameter for the function to determine
 *  the sorting policy he wishes to implement.
 *
 *  Returns values:
 *  ---------------
 *  When function returns TRUE(1) -  new_data has higher priority than
 *	current_data.
 *  When function returns FALSE(0) - new_data has lower priority than
 *	current_data.
 */
typedef int(*has_higher_priority_t)(void *current_data,
									const void *new_data, void *param);

typedef int(*pq_is_match_t)(void *data, void *param);


typedef struct pqueue pqueue_t;

/***************************** PQCreate ****************************************
 *	Description: creates an empty priority queue
 *	Input:		 priority function pointer type priority_t
 *	Output:		 if success - returns a pointer to the new queue
 *				 Otherwise - returns NULL
 *	Complexity:	 O(1)
 */
pqueue_t *PQCreate(has_higher_priority_t func);

/***************************** PQDestroy ***************************************
 *	Description: Destroys the priority queue and all of its content
 *	Input:		 Pointer to the priority queue to be destroyed
 *	Output:		 void
 *	Complexity:	 O(n)
 */
void PQDestroy(pqueue_t *pqueue);

/***************************** PQPush ******************************************
 *	Description: Adds an element to the priority queue according to its priority
 *	Input:		 The function receives pqueue_t, an address of the element and
 * 				 param.
 *	Output:		 If succeed - returns 0. Otherwise - returns 1.
 *	Complexity:	 O(n)
 */
int PQPush(pqueue_t *pqueue, void *data, void *param);

/***************************** PQPop ******************************************
 *	Description: Dequeues the element with the highest priority
 *	Input:		 Pointer to priority queue.
 *	Output:		 A pointer to the data of the deleted dequeued element. If the
 				 queue is empty returns NULL.
 *	Complexity:	 O(1)
 */
void *PQPop(pqueue_t *pqueue);

/***************************** PQIsEmpty ***************************************
 *	Description: Checks whether the priority queue is empty
 *	Input:		 Pointer to priority queue
 *	Output:		 True (1) if is empty. False (0) - otherwise
 *	Complexity:	 O(1)
 */
int PQIsEmpty(const pqueue_t *pqueue);

/***************************** PQPeek ******************************************
 *	Description: Shows the element with the highest priority
 *	Input:		 Pointer to priority queue
 *	Output:		 Pointer to the 1st element's data. NULL if empty.
 *	Complexity:  O(1)
 */
void *PQPeek(const pqueue_t *pqueue);

/***************************** PQSize ******************************************
 *	Description: Checks the size of the queue
 *	Input:		 Pointer to priority queue
 *	Output:		 The size of the queue
 *	Complexity:	 O(n)
 */
size_t PQSize(const pqueue_t *pqueue);

/***************************** PQClear *****************************************
 *	Description: Removes all of the elements in the queue without destroying it
 *	Input:		 Pointer to priority queue
 *	Output:		 void
 *	Complexity:	 O(n)
 */
void PQClear(pqueue_t *pqueue);

/***************************** PQErase *****************************************
 *	Description: Erases from the queue the 1st matching element.
 *	Input:		 Pointer to priority queue, matching function pointer and param.
 *	Output:		 A pointer to the data of the dequeued element.
 *				 If an element with a corresponding data was not found -
 *				 returns NULL.
 *	Complexity:	 O(n)
 */
void *PQErase(pqueue_t *pqueue, pq_is_match_t func, void *param);

#endif     /* _PQUEUE_H_ */
