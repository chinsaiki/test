/**
 * @file dpatch/include/patch_script.h
 *
 * `patch_script.h` declares functions for parsing a
 * script containing patch operations to perform.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date November 2020.
 */

#ifndef DPATCH_INCLUDE_PATCH_SCRIPT_H_
#define DPATCH_INCLUDE_PATCH_SCRIPT_H_

#include "patch_set.h"
#include "status.h"

/**
 * `patch_script_t` is a handle to a patch script.
 */
typedef struct patch_script patch_script_t;

/**
 * Allocate and initialise a new patch script reference in
 * memory.
 *
 * @param new Location to store a handle to the new patch.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status patch_script_new(patch_script_t** new);

/**
 * Deallocate a `patch_script_t` and free its resources.
 *
 * @param patch_script Handle to the patch_script to
 *      deallocate.
 */
void patch_script_free(patch_script_t* patch_script);

/**
 * Set the path to a patch script to parse.
 *
 * @param patch_script Handle to the patch script to set.
 * @param path Path to the patch file to parse.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status patch_script_path
(
    patch_script_t* patch_script,
    char* path
);

/**
 * Parse a patch script into memory.
 *
 * @param patch_script Handle to the patch script to parse.
 * @param patch_set Set to parse the script into.
 * @return `DPATCH_STATUS_OK` or an error code.
 */
dpatch_status patch_script_parse
(
    patch_script_t* patch_script,
    patch_set_t* patch_set
);

#endif
