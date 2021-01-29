/**
 * @file dpatch/include/patch.h
 *
 * `patch.h` defines a `patch_t` for describing a single
 * patch operation to be applied, and declares functions
 * for manipulating and applying the patch.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date November 2020.
 */

#ifndef DPATCH_INCLUDE_PATCH_H_
#define DPATCH_INCLUDE_PATCH_H_

#include "code_generator.h"
#include "machine_code.h"
#include "status.h"
#include <dlfcn.h>
#include <stdint.h>

/**
 * A type of patch operation to perform.
 */
typedef enum
{
    /** 
     * Replace a function with another from the same
     * object.
     */
    DPATCH_OP_REPLACE_FUNCTION_INTERNAL,

    /**
     * A dummy operation. Perform no patch.
     */
    DPATCH_OP_NOP,
} dpatch_operation;

/**
 * `patch_t` is a handle to a patch.
 */
typedef struct patch patch_t;

/**
 * Convert a string to a `dpatch_operation`.
 *
 * @param str String to convert.
 * @param op  Address to store operation in.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status str_to_patch_operation
(
    char* str,
    dpatch_operation* op
);

/**
 * Allocate and initialise a new patch in memory.
 *
 * @param new Location to store a handle to a new patch.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status patch_new(patch_t** new);

/**
 * Configure a patch with an operation to perform.
 *
 * @param patch Handle to the `patch_t` to configure.
 * @param op Patch operation to perform.
 * @param old_sym Old symbol to be replaced.
 * @param new_sym New symbol to patch in.
 * @param library Library containing the new symbol.
 */
dpatch_status patch_operation
(
    patch_t* patch,
    dpatch_operation op,
    char* old_sym,
    char* new_sym,
    char* library
);

/**
 * Deallocate a `patch_t` and free its resources.
 *
 * @param patch Handle to the patch to deallocate.
 */
void patch_free(patch_t* patch);

/**
 * Attempt to apply a patch to the running program.
 *
 * @param patch Handle to the patch to apply.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status patch_apply(patch_t* patch);

#endif
