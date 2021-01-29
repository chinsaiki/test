/**
 * @file dpatch/patch_set.c
 *
 * `patch_set.c` defines functions for generating and
 * applying a set of patches.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date November 2020.
 */

#include "patch.h"
#include "patch_set.h"
#include "status.h"
#include <assert.h>
#include <stdlib.h>

#define PATCH_DEFAULT_LENGTH 8

struct patch_set
{
    /** The number of `patches` allocated in memory. */
    size_t allocated_length;

    /** The number of patches in the patch set. */
    size_t length;

    /** Array of handles to patches to apply. */
    patch_t** patches;
};

/**
 * Allocates and initialises a new patch_set.
 *
 * @param patch_set Pointer to address for storing the new
 *              patch_set handel.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status patch_set_new(patch_set_t** patch_set)
{
    patch_set_t* new_set = malloc(sizeof(struct patch_set));
    *patch_set = new_set;
    if (*patch_set == NULL)
    {
        return DPATCH_STATUS_ENOMEM; 
    }
    new_set->length = 0;
    new_set->allocated_length = PATCH_DEFAULT_LENGTH;
    new_set->patches = malloc(sizeof(patch_t*) * new_set->allocated_length);
    if (new_set->patches == NULL)
    {
        patch_set_free(new_set);
        *patch_set = NULL;
        return DPATCH_STATUS_ENOMEM;
    }
    return DPATCH_STATUS_OK;
}

/**
 * Free and deallocate a patch set.
 *
 * @param patch_set Handle to the patch set to free.
 */
void patch_set_free(patch_set_t* patch_set)
{
    assert(patch_set != NULL);
    if (patch_set->patches != NULL)
    {
        for(size_t i = 0; i < patch_set->length; i++)
        {
            patch_free(patch_set->patches[i]);
        }
        free(patch_set->patches);
    }
    patch_set->patches = NULL;
    patch_set->allocated_length = 0;
    patch_set->length = 0;
    free(patch_set);
}

/**
 * Grow the memory allocated to a patch set.
 *
 * @param patch_set Handle to the patch set to grow.
 * @return `DPATCH_STATUS_OK` on success, or an error.
 */
dpatch_status patch_set_grow(patch_set_t* patch_set)
{
    patch_t** realloc_result = NULL;
    assert(patch_set != NULL);
    patch_set->allocated_length *= 2;
    realloc_result = realloc
    (
        patch_set->patches,
        sizeof(patch_t*) * patch_set->allocated_length
    );
    if (realloc_result == NULL)
    {
        patch_set->allocated_length /= 2;
        return DPATCH_STATUS_ENOMEM;
    }
    patch_set->patches = realloc_result;
    return DPATCH_STATUS_OK;
}

/**
 * Add a patch operation to the set of operations
 * associated with a patchset.
 *
 * @note The exact semantics of the `old` and `new` symbol
 * parameters will depend on the patch operation `op`
 * selected. `old` is generally the symbol to be replaced
 * and `new` is the new symbol to replace `old`.
 *
 * @param patch_set Handle to the patch_set an operation to.
 * @param op Patch operation to perform.
 * @param old Symbol to be updated.
 * @param new Symbol to update to.
 * @param lib Library new symbol is in.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status patch_set_add_operation
(
    patch_set_t* patch_set,
    dpatch_operation op,
    char* old,
    char* new,
    char* lib
)
{
    patch_t* new_patch = NULL;
    dpatch_status status = DPATCH_STATUS_OK;
    if (patch_set->length == patch_set->allocated_length)
    {
        PROPAGATE_ERROR(patch_set_grow(patch_set), status);
    }
    PROPAGATE_ERROR(patch_new(&new_patch), status);
    PROPAGATE_ERROR(patch_operation(new_patch, op, old, new, lib), status);
    patch_set->patches[patch_set->length++] = new_patch;
    return DPATCH_STATUS_OK;
}

/**
 * Attempt to apply a patch_set to the target program.
 *
 * @param patch_set Handle to the patch_set to be applied.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status patch_set_apply(patch_set_t* patch_set)
{
    size_t i = 0;
    dpatch_status status = DPATCH_STATUS_OK;
    for (i = 0; i < patch_set->length; i++)
    {
        PROPAGATE_ERROR(patch_apply(patch_set->patches[i]), status);
    }
    return DPATCH_STATUS_OK;
}
