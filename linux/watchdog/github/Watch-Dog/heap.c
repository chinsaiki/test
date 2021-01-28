/*******************************************************************************
* 	File name  	: heap.c 													   *
* 	Developer  	: Lior Katz											           *
* 	Date		: 2019-04-2 												   *
* 	Description	: version 2 												   *
*******************************************************************************/
/**** Includes ************************/
#include <assert.h>	/*	assert 	   	  */
#include <stdlib.h>	/*	malloc, free  */
#include <string.h> /*  memset		  */

#include "dynamic_vector_v1.h"
#include "heap.h"

/**** Macros **************************/
#define WORD_SIZE (sizeof(void *))
#define HEAP_SIZE(x) (HeapSize(x) - 1)
#define PARENT(x) ((size_t) (((long) (x) - 1) / 2))
#define LEFT(x) ((size_t) ((x) * 2 + 1))
#define RIGHT(x) ((size_t) (x) * 2 + 2)
#define GET_DATA(x) (*(void **) (x))
#define DIR_LEFT (0)
#define DIR_RIGHT (1)
#define NOT_EXIST (-1lu)

/**** Structs *************************/
struct heap_s
{
    heap_is_before_t is_before_func;
    void *param;
    dv_t *vector;
};

/**** Inner Functions *****************/
/***** HeapifyUp ***************************************************************
 *	Description: Rearrange a heap to maintain the heap property, that is, the
 *				 key of the root node is more extreme than or equal to the keys
 *				 of its children. If the root node’s key is not more extreme,
 *				 swap it with the most extreme child key and heapifies root
 *				 parent tree recursivly.
 *	Input:		 heap - pointer to the heap data structure.
 *	Output:		 None.
 *	Complexity:	 Most cases - O(log(n)).
 * 				 Worst case - O(n).
 */
static void HeapifyUp(heap_t *heap, size_t place);

/***** HeapifyDown *************************************************************
 *	Description: Rearrange a heap to maintain the heap property, that is, the
 *				 key of the root node is more extreme than or equal to the keys
 *				 of its children. If the root node’s key is not more extreme,
 *				 swap it with the most extreme child key and heapifies the
 *				 childs sub-tree recursivly.
 *	Input:		 heap - pointer to the heap data structure.
 *	Output:		 None.
 *	Complexity:	 Most cases - O(log(n)).
 * 				 Worst case - O(n).
 */
static void HeapifyDown(heap_t *heap, size_t place);

/***** SwapData ****************************************************************
 *	Description: Swapping two pointers pointed by first and second.
 *	Input:		 first - pointer to first element.
 *				 second - pointer to second element.
 *	Output:		 None.
 *	Complexity:	 O(1).
 */
static void SwapData(void *first, void *second);

/***** HeapFind ****************************************************************
 *	Description: Looking up for element matching to data in heap.
 *	Input:		 data - pointer to the data to be found.
 *				 heap - pointer to the heap data structure.
 *				 is_match_func.
 *	Output:		 On success - index of the elment.
 *				 0  - NULL.
 *	Complexity:	 O(n).
 */
static size_t HeapFind(heap_t *heap, void *data,
			   				 heap_is_match_t is_match_func);

/**** Functions ***********************/
/***** HeapCreate *************************************************************/
heap_t *HeapCreate(size_t capacity, heap_is_before_t is_before_func,
				   													void *param)
{
	heap_t *heap = NULL;
	dv_t *dv = NULL;

	if (NULL != is_before_func && capacity > 0)
	{
		heap = (heap_t *) malloc(sizeof(heap_t));
		if (NULL != heap)
		{
			dv = DVCreate(capacity, WORD_SIZE);
			if (NULL != dv)
			{
				heap->vector = dv;
				heap->param = param;
				heap->is_before_func = is_before_func;
			}
			else
			{
				free(heap);
				heap = NULL;
			}
		}
	}

	return (heap);
}

/***** HeapDestroy ************************************************************/
void HeapDestroy(heap_t *heap)
{
	assert(heap);

	DVDestroy(heap->vector);
	heap->vector = NULL;
	heap->is_before_func = NULL;
	free(heap);
	heap = NULL;
}

/***** HeapPush ***************************************************************/
status_t HeapPush(heap_t *heap, const void *data)
{
	status_t status = SUCCESS;

	assert(heap);

	status = DVPushBack(heap->vector, &data);
	if (FAILURE != status)
	{
	 	HeapifyUp(heap, HEAP_SIZE(heap));
	}

	return (status);
}

/***************************** HeapPop ****************************************/
status_t HeapPop(heap_t *heap)
{
	status_t status = SUCCESS;
	dv_t *dv = NULL;

	assert(heap);

	if (!HeapIsEmpty(heap))
	{
		dv = heap->vector;
		/*send root element to the end and pop*/
		SwapData(DVGetItem(dv, 0), DVGetItem(dv, HEAP_SIZE(heap)));
		status = DVPopBack(dv);
		/*heapify from the root down*/
		HeapifyDown(heap, 0);
	}

	return (status);
}

/***************************** HeapRemove *************************************/
void *HeapRemove(heap_t *heap, void *data, heap_is_match_t is_match_func)
{
	size_t element_index = 0;
	void *current = NULL;
	void *parent = NULL;
	dv_t *dv = NULL;

	assert(heap);

	/*Look up*/
	element_index = HeapFind(heap, data, is_match_func);
	data = NULL;
	if (NOT_EXIST != element_index)
	{
		/*Init*/
		dv = heap->vector;
		current = DVGetItem(dv, element_index);
		data = *(void **) current;
		parent = DVGetItem(dv, PARENT(element_index));
		SwapData(DVGetItem(dv, HEAP_SIZE(heap)), current);

		/*Heapify*/
	 	if (heap->is_before_func(GET_DATA(parent),
	 							 GET_DATA(current), heap->param))
		{
			DVPopBack(dv);
			HeapifyDown(heap, element_index);
		}
		else
		{
			DVPopBack(dv);
			HeapifyUp(heap, element_index);
		}
	}

	return (data);
}

/***************************** HeapPeek ***************************************/
void *HeapPeek(const heap_t *heap)
{
	assert(heap);

	return (HeapIsEmpty(heap) ? NULL : GET_DATA(DVGetItem((heap->vector), 0)));
}

/***************************** HeapIsEmpty ************************************/
boolean_t HeapIsEmpty(const heap_t *heap)
{
	assert(heap);

	return (0 == HeapSize(heap));
}

/***************************** HeapSize ***************************************/
size_t HeapSize(const heap_t *heap)
{
	assert(heap);

	return (DVSize(heap->vector));
}

/**** Inner Functions *****************/
/***** HeapifyUp **************************************************************/
static void HeapifyUp(heap_t *heap, size_t place)
{
	void *current = NULL;
	void *parent = NULL;

	assert(heap);
	/*Init*/
	current = DVGetItem(heap->vector, place);
	parent = DVGetItem(heap->vector, PARENT(place));

	/*assert(current);
	assert(parent);
	assert(GET_DATA(parent));
	assert(GET_DATA(current));
	assert(HEAP_SIZE(heap) + 1);*/

	/*Check - dont need to be before his parrent*/
	if (place < HeapSize(heap) &&
		heap->is_before_func(GET_DATA(parent), GET_DATA(current), heap->param))
	{
		/*Swap*/
		SwapData(current, parent);
		/*Call*/
		HeapifyUp(heap, PARENT(place));
	}
}

/***** HeapifyDown ************************************************************/
static void HeapifyDown(heap_t *heap, size_t place)
{
	void *current = NULL;
	void *children[2] = {NULL};
	int child = DIR_LEFT;
	size_t size = 0;
	void *param = NULL;
	dv_t *dv = NULL;

	assert(heap);

	size = HeapSize(heap);
	/*Check for children*/
	if (LEFT(place) >= size)
	{
		return;
	}
	/*Init*/
	dv = heap->vector;
	param = heap->param;
	current = DVGetItem(dv, place);
	children[DIR_LEFT] = DVGetItem(dv, LEFT(place));
	/*two children*/
	if (RIGHT(place) < size)
	{
		children[DIR_RIGHT] = DVGetItem(dv, RIGHT(place));
		child = heap->is_before_func(GET_DATA(children[DIR_LEFT]),
									 GET_DATA(children[DIR_RIGHT]), param);
	}
	/*Check and swap*/
	if (heap->is_before_func(GET_DATA(current),
							 GET_DATA(children[child]), param))
	{
		SwapData(current, children[child]);
	}
	else
	{
		return;
	}
	/*Call*/
	HeapifyDown(heap, LEFT(place) + child);
}

/***** SwapData ***************************************************************/
static void SwapData(void *first, void *second)
{
	void *temp = NULL;

	assert(first);
	assert(second);

	temp = GET_DATA(first);
	GET_DATA(first) = GET_DATA(second);
	GET_DATA(second) = temp;
}

/***** HeapFind ***************************************************************/
static size_t HeapFind(heap_t *heap, void *data, heap_is_match_t is_match_func)

{
	void *current = NULL;
	size_t size = 0;
	size_t place = 0;
	dv_t *dv = NULL;

	assert(heap);

	dv = heap->vector;
	size = HeapSize(heap);
	current = DVGetItem(dv, place);

	while (place < size && !is_match_func(data, *(void **) current))
	{
		++place;
		current = DVGetItem(dv, place);
	}

	return ((place == size) ? (NOT_EXIST) : place);
}
