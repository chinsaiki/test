/*******************************************************************************
 * File name  : dynamic_vector_v1.c
 * Developer  : Lior Katz
 * Date		  : 2019-02-03
 * Description: version 1
 ******************************************************************************/

/****Includes****/
#include <stdlib.h>				/*malloc, realloc, free*/
#include <string.h>				/*memcpy			   */
#include <assert.h>				/*assert			   */

#include "dynamic_vector_v1.h"

/*****Macros*****/
#define REDUCTIN_TRESHOLD 4
#define RESIZE_FACTOR 2
#define IS_EMPTY(dv) (0 == dv->size)
#define IS_FULL(dv) (((dv->capacity - dv->size) < dv->element_size))

/****Structs*****/
struct dynamic_vector
{
	void *vector;
	size_t capacity;
	size_t size;
	size_t element_size;
};

/**Declaretions**/
/*
 *  ResizeVector resizes the vector pointed by dv to new size.
 *  The function returns 0 in case of success and -1 in case of failure.
 */
static int ResizeVector(dv_t *dv, size_t new_size);

/****Functions***/
dv_t *DVCreate(size_t capacity, size_t element_size)
{
	dv_t *dy_vec = NULL;

	if (0 != capacity)
	{
		/*creating vector head*/
		dy_vec = malloc(sizeof(dv_t));

		if (NULL != dy_vec)
		{
			/*creating vector body and initializing*/
			dy_vec->vector = malloc(capacity * element_size);
			if (NULL != dy_vec->vector)
			{
				dy_vec->capacity = capacity * element_size;
				dy_vec->size = 0;
				dy_vec->element_size = element_size;

				return (dy_vec);
			}
			else
			{
				free(dy_vec);
				dy_vec = NULL;
			}
		}
	}

	return (NULL);
}

void DVDestroy(dv_t *dv)
{
	assert(dv);

	free(dv->vector);
	dv->vector = NULL;

	free(dv);
	dv = NULL;
}

int DVPushBack(dv_t *dv, const void *element)
{
	assert(dv);
	assert(element);

	/*resize*/
	if (IS_FULL(dv))
	{
		if (-1 == ResizeVector(dv, (RESIZE_FACTOR * dv->capacity)))
		{
			return (-1);
		}
	}

	/*write new element*/
	if (((void *) ((size_t) dv->vector + dv->size)) !=
		(memcpy((void *) ((size_t) dv->vector + dv->size), element,
															dv->element_size)))
	{
		return (-1);
	}

	dv->size += dv->element_size;

	return (0);
}

int DVPopBack(dv_t *dv)
{
	assert(dv);

	if (IS_EMPTY(dv))
	{
		return (-1);
	}

	/*resize if not enouth elements*/
	if (dv->size <= (dv->capacity / REDUCTIN_TRESHOLD))
	{
		if (-1 == ResizeVector(dv, (dv->capacity -
			(((dv->capacity / RESIZE_FACTOR) % dv->element_size) *
											RESIZE_FACTOR)) / RESIZE_FACTOR))
		{
			return (-1);
		}
	}

	/*delete element*/
	dv->size -= dv->element_size;

	return (0);
}

size_t DVSize(const dv_t *dv)
{
	assert(dv);

	return (dv->size / dv->element_size);
}

size_t DVCapacity(const dv_t *dv)
{
	assert(dv);

	return (dv->capacity / dv->element_size);
}

void *DVGetItem(const dv_t *dv, size_t index)
{
	assert(dv);

	if (index < DVSize(dv))
	{
		return ((void *) ((size_t) dv->vector + (index * dv->element_size)));
	}

	return (NULL);
}

int DVReserve(dv_t *dv, size_t additional_size)
{
	assert(dv);
	assert(dv->vector);

	return ((-1 != ResizeVector(dv,
			(additional_size * dv->element_size) + dv->capacity)) ? 0 : -1);
}

static int ResizeVector(dv_t *dv, size_t new_size)
{
	void *temp_vec;

	temp_vec = realloc(dv->vector, new_size);
	if (NULL != temp_vec)
	{
		dv->capacity = new_size;
		dv->vector = temp_vec;

		return (0);
	}

	return (-1);
}
