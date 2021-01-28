/*******************************************************************************
* File name  : pq.c
* Developer  : HRD8 - Boris Vaintrub
* Date		 : 2019-02-18
* Description: version 1
*******************************************************************************/
/**** Includes ************************/
#include <assert.h> 	/*assert	  */
#include <stdlib.h>		/*malloc, free*/

#include "heap.h"
#include "pqueue.h"

/**** Macros **************************/
#define BASIC_CAPASITY 1
#define UNUSED(x) ((void) (x))

/**** Structs *************************/
struct pqueue
{
	heap_t *heap;
};

/**** Functions ***********************/
/****** PQCreate **************************************************************/
pqueue_t *PQCreate(has_higher_priority_t func)
{
	pqueue_t *pq = (pqueue_t *) malloc(sizeof(pqueue_t));

	assert(func);

	if (NULL != pq)
	{
		pq->heap = HeapCreate(BASIC_CAPASITY, (heap_is_before_t) func, NULL);
		if (NULL == pq->heap)
		{
			free(pq);
			pq = NULL;
		}
	}

	return (pq);
}

/****** PQDestroy *************************************************************/
void PQDestroy(pqueue_t *pqueue)
{
	assert(pqueue);

	HeapDestroy(pqueue->heap);
	pqueue->heap = NULL;

	free(pqueue);
	pqueue = NULL;
}
/****** PQPush ****************************************************************/
int PQPush(pqueue_t *pqueue, void *data, void *param)
{	
	assert(pqueue);

	UNUSED(param);
	
	return (HeapPush(pqueue->heap, data));
}

/****** PQPop *****************************************************************/
void *PQPop(pqueue_t *pqueue)
{
	void *data = NULL;

	assert(pqueue);

	data = HeapPeek(pqueue->heap);
	HeapPop(pqueue->heap);

	return (data);
}

/****** PQIsEmpty *************************************************************/
int PQIsEmpty(const pqueue_t *pqueue)
{
	assert(pqueue);

	return (0 == PQSize(pqueue));
}

/****** PQPeek ****************************************************************/
void *PQPeek(const pqueue_t *pqueue)
{
	assert(pqueue);

	return (HeapPeek(pqueue->heap));
}

/****** PQSize ****************************************************************/
size_t PQSize(const pqueue_t *pqueue)
{
	assert(pqueue);

	return (HeapSize(pqueue->heap));
}

/****** PQClear ***************************************************************/
void PQClear(pqueue_t *pqueue)
{
	assert(pqueue);

	while (!PQIsEmpty(pqueue))
	{
		PQPop(pqueue);
	}
}

/****** PQErase ***************************************************************/
void *PQErase(pqueue_t *pqueue, pq_is_match_t func, void *param)
{
	assert(pqueue);
	assert(func);

	/*Remove fitting element from the heap according to given func and param*/
	return (HeapRemove(pqueue->heap, param, (heap_is_match_t) func));
}