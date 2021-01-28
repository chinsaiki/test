/*******************************************************************************
* File name  : heap.h
* Developer	 : Lior Katz
* Date		 : 2019-04-2
* Description: version 2
*******************************************************************************/
#ifndef _HEAP_H_
#define _HEAP_H_

#include <stddef.h> /* size_t */

#include "general_types.h"

typedef struct heap_s heap_t;

/* is_before_t determines the sorting policy for the heap as given by the user.
*  Function receives two arguments and a user parameter and returns a
*  boolean value.
*
*  Returns values:
*  ---------------
*  When function returns TRUE  (1) - the new_data is closer to the heap root
*									 then the heap_data.
*  When function returns FALSE (0) - the heap_data is closer to the heap root
*									 then the new_data.
*/
typedef boolean_t(*heap_is_before_t)(const void *heap_data,
							   		 const void *new_data,
							   		 void *param);

/* is_match_t determines if both new_data and heap_data are the same.
*  Function receives two arguments and returns a boolean value.
*
*  Returns values:
*  ---------------
*  When function returns TRUE  (1) - new_data is the same as heap_data.
*  When function returns FALSE (0) - new_data is different from heap_data.
*/
typedef boolean_t(*heap_is_match_t)(void *new_data, void *heap_data);


/***************************** HeapCreate **************************************
 *	Description: creates an empty heap.
 *
 *	Input:		 is_before function pointer type heap_is_before_t.
 *               param - auxilary parameter for the is_before_func.
 *				 capacity - Initial Capacity of the heap.
 *
 *	Output:		 if success - returns a pointer to the new heap
 *               Otherwise - returns NULL.
 *				 if is_before_func is NULL returns NULL.
 *               if capacity is zero (0) returns NULL
 *
 *	Complexity:	 O(1)
 */
heap_t *HeapCreate(size_t capacity,
				   heap_is_before_t is_before_func,
                   void *param                      );


/***************************** HeapDestroy *************************************
 *	Description: Frees all the resources occupied by the heap.
 *
 *	Input:		 heap - Pointer to the heap data structure to be destroyed.
 *
 *	Output:		 None.
 *
 *	Complexity:	 O(1)
 */
void HeapDestroy(heap_t *heap);


/***************************** HeapPush ****************************************
 *	Description: Adds an element to the heap according to the heap policy.
 *  		     In some cases the heap might grow dynamically.
 *
 *	Input:		 heap - pointer to the heap data structure.
 *				 data - pointer to the user data.
 *
 *	Output:		 If succeed - returns SUCCESS (0). Otherwise - FAILURE .
 *
 *	Complexity:	 Most cases - O(log(n))
 * 				 Worst case - O(n)
 */
status_t HeapPush(heap_t *heap, const void *data);


/***************************** HeapPop *****************************************
 *	Description: removes the first element of the heap. after removel heap.
 * 				 will remain organized according to the policy.
 * 				 In some cases the heap might change dynamically.
 *
 *	Input:		 Pointer to heap data structure.
 *
 *	Output:		 SUCCESS (0) -  first element was removed from the heap.
 *				 FAILURE (-1) - first element was removed from the heap,
 *								But the heap failed to dynamically change its
 *								capacity.
 *
 *	Complexity:	 Most cases - O(log(n))
 * 				 Worst case - O(n)
 */
status_t HeapPop(heap_t *heap);

/***************************** HeapRemove **************************************
 *	Description: Removes the pointer data from the heap.
 *
 *	Input:		 heap - Pointer to the heap data structure.
 *				 data - pointer to the data to be removed.
 *				 is_match_func.
 *
 *	Output:		 pointer to the element been removed.
 *				 if not found, NULL will be returned.
 *
 *	Complexity:	 O(n)
 */
void *HeapRemove(heap_t *heap, void *data, heap_is_match_t is_match_func);

/***************************** HeapPeek ****************************************
*	Description: fetches the data of the first element.
*
*	Input:		 Pointer to heap data structure.
*
*	Output:		 Pointer to the first element's data. NULL if empty.
*
*	Complexity:  O(1)
*/
void *HeapPeek(const heap_t *heap);


/***************************** HeapIsEmpty *************************************
 *	Description: Checks whether the heap is empty
 *
 *	Input:		 Pointer to heap
 *
 *	Output:		 True (1) if is empty. False (0) - otherwise
 *
 *	Complexity:	 O(1)
 */
boolean_t HeapIsEmpty(const heap_t *heap);


/***************************** HeapSize ****************************************
 *	Description: returns the number of elements in the heap
 *
 *	Input:		 Pointer to heap
 *
 *	Output:		 The size of the heap
 *
 *	Complexity:	 O(1)
 */
size_t HeapSize(const heap_t *heap);

#endif     /* _heap_H_ */
