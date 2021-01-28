/*******************************************************************************
 * File name  : dynamic_vector_v1.h
 * Developer  : Lior Katz
 * Date		  : 2019-02-03
 * Description: version 1
 ******************************************************************************/

#ifndef _DYNAMIC_VECTOR_V1_H_
#define _DYNAMIC_VECTOR_V1_H_

#include <stddef.h>     /* size_t */

/*
 *  Dynamic Vector type definition.
 */
typedef struct dynamic_vector dv_t;

/*
 *  DV_Create creates a Dynamic Vector.
 *  in case of success the create function returns a pointer to the vector.
 *  in case of failure NULL will be returned.
 *  The function receives two arguments: capacity (total number of elements),
 *  and element_size - the size of each element.
 */
dv_t *DVCreate(size_t capacity, size_t element_size);

/*
 *  DV_Destroy function will release the memory pointed to by dv.
 */
void DVDestroy(dv_t *dv);

/*
 *  DV_PushBack function inserts element into the end of the vector pointed
 *  by dv. the function will not alter the value of element.
 *  the function returns 0 in case of success and -1 in case of failure.
 */
int DVPushBack(dv_t *dv, const void *element);

/*
 *  DV_PopBack removes the element at the end of the vector pointed by dv.
 *  the function returns 0 in case of success and -1 in case of failure.
 */
int DVPopBack(dv_t *dv);

/*
 *  DV_Size returns the number of elements in the vector pointed by dv.
 */
size_t DVSize(const dv_t *dv);

/*
 *  DV_Capacity returns the capacity of elements in the vector pointed by dv.
 */
size_t DVCapacity(const dv_t *dv);

/*
 *  DV_GetItem returns a pointer to the indexd element.
 *  the pointer may be invalid after changing the capacity of the vector.
 *  in case of failure NULL will be returned.
 */
void *DVGetItem(const dv_t *dv, size_t index);

/*
 *  DV_Reserve adds size bytes to the vector pointed by dv.
 *  the content will be unchanged in the range from start to old size.
 *  the function returns 0 in case of success and -1 in case of failure.
 */
int DVReserve(dv_t *dv, size_t additional_size);

#endif     /* _DYNAMIC_VECTOR_V1_H_ */
