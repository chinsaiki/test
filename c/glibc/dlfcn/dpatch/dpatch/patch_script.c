/**
 * @file dpatch/patch_script.c
 *
 *1 `patch_script.c` defines functions for parsing a
 * script containing patch operations to perform.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date November 2020.
 */

#include "patch_script.h"
#include "status.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PATCH_SCRIPT_MAX_LINE_LEN 255
#define XSTR(x) STR(x)
#define STR(x) #x

#define DEFAULT_SCRIPT_PATH "/usr/etc/patch.dpatch"
#define SCRIPT_PATH_ENV_VAR "DPATCH_SCRIPT"

/**
 * A patch script to be processed.
 */
struct patch_script
{
    /** The path to the script on disk. */
    char* script_path;
};

/**
 * Sets the patch path to the default path, or a path
 * indicated by an environment variable if available.
 *
 * @param patch Path to init path into.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status patch_script_init_path(patch_script_t* patch)
{
    assert(patch != NULL);
    char* path = getenv(SCRIPT_PATH_ENV_VAR);
    path = path == NULL ? DEFAULT_SCRIPT_PATH : path;
    return patch_script_path(patch, path);
}


/**
 * Allocate and initialise a new patch script reference in
 * memory.
 *
 * @param new Location to store a handle to the new patch.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status patch_script_new(patch_script_t** new)
{
    assert(new != NULL);
    dpatch_status status = DPATCH_STATUS_OK;
    patch_script_t* handle = calloc(1, sizeof *handle);
    *new = handle;
    if (handle == NULL)
    {
        return DPATCH_STATUS_ENOMEM;
    }
    status = patch_script_init_path(handle);
    if (IS_ERROR(status))
    {
        *new = NULL;
        patch_script_free(handle);
        return status;
    }
    return DPATCH_STATUS_OK;
}


/**
 * Deallocate a `patch_script_t` and free its resources.
 *
 * @param patch_script Handle to the patch_script to
 *      deallocate.
 */
void patch_script_free(patch_script_t* patch_script)
{
    assert(patch_script != NULL);
    if (patch_script->script_path != NULL)
    {
        free(patch_script->script_path);
    }
    free(patch_script);
}

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
)
{
    assert(patch_script != NULL);
    assert(path != NULL);
    if (patch_script->script_path == NULL)
    {
        patch_script->script_path = calloc(1, strlen(path) + 1);
    }
    else if (
        strlen(patch_script->script_path) < strlen(path)
    )
    {
        patch_script->script_path = realloc(
            patch_script->script_path,
            strlen(path) + 1
        );
    }
    if (patch_script->script_path == NULL)
    {
        return DPATCH_STATUS_ENOMEM;
    }
    strcpy(patch_script->script_path, path);
    return DPATCH_STATUS_OK;
}

/**
 * Parse a single line (instruction) of a patch script.
 *
 * @param line The script line to be parsed.
 * @param patch_set Patch to parse the line into.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status parse_script_line
(
    char* line,
    patch_set_t* patch_set
)
{
    char operation_str[PATCH_SCRIPT_MAX_LINE_LEN];
    char op_from[PATCH_SCRIPT_MAX_LINE_LEN];
    char op_to[PATCH_SCRIPT_MAX_LINE_LEN];
    char* new_symbol_name = NULL;
    char* new_symbol_lib = NULL;
    dpatch_operation operation = DPATCH_OP_NOP;
    dpatch_status status = DPATCH_STATUS_OK;
    if (sscanf(line, "%s %s %s", operation_str, op_from, op_to) != 3)
    {
        return DPATCH_STATUS_ESYNTAX;
    }
    new_symbol_name = strtok(op_to, ":");
    new_symbol_lib = strtok(NULL, ":");
    PROPAGATE_ERROR(
        str_to_patch_operation(operation_str, &operation),
        status
    );
    PROPAGATE_ERROR(
        patch_set_add_operation
        (
            patch_set,
            operation,
            op_from,
            new_symbol_name,
            new_symbol_lib
        ),
        status
    );
    return DPATCH_STATUS_OK;
}

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
)
{
    (void) patch_set;
    FILE* script = fopen(patch_script->script_path, "r");
    char line[PATCH_SCRIPT_MAX_LINE_LEN];
    dpatch_status status = DPATCH_STATUS_OK;
    if (script == NULL)
    {
        return DPATCH_STATUS_EFILE;
    }
    while (fscanf(script, "%" XSTR(PATCH_SCRIPT_MAX_LINE_LEN) "[^\n]\n", line) == 1)
    {
        PROPAGATE_ERROR(parse_script_line(line, patch_set), status);      
    }
    return DPATCH_STATUS_OK;
}
