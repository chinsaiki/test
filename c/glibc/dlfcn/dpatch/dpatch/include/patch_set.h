/**
 * @file dpatch/include/patch_set.h
 *
 * `patch_set.h` declares a `patch_set_t` for outlining the
 * the contents of a patch, and provides functions
 * for operating on a `patch_set_t`.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date November 2020.
 */

#ifndef DPATCH_INCLUDE_PATCH_SET_H_
#define DPATCH_INCLUDE_PATCH_SET_H_

#include "patch.h"
#include "status.h"

/**
 * patch_set_t provides an opaque type for representing a
 * set of changes to be applied to a program.
 */
typedef struct patch_set patch_set_t;

/**
 * Allocates and initialises a new patch set.
 *
 * @param patch_set Pointer to address for storing the new
 *              patch handel.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status patch_set_new(patch_set_t** patch_set);

/**
 * Free and deallocate a patch set.
 *
 * @param patch_set Handle to the patch set to free.
 */
void patch_set_free(patch_set_t* patch_set);

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
 * @param lib Library the new symbol is in.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status patch_set_add_operation
(
    patch_set_t* patch_set,
    dpatch_operation op,
    char* old,
    char* new,
    char* lib
);

/**
 * Attempt to apply a patch_set to the target program.
 *
 * @param patch_set Handle to the patch_set to be applied.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status patch_set_apply(patch_set_t* patch_set);

#endif
