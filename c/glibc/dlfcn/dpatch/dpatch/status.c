/**
 * @file dpatch/status.c
 *
 * `dpatch/status.c` defines functions for querying the status of dpatch.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date November 2020.
 */

#include "status.h"

char* status_strings[] =
{
    [DPATCH_STATUS_OK] = "Success",
    [DPATCH_STATUS_ERROR] = "General, unspecified, or unknown error",
    [DPATCH_STATUS_ENOMEM] = "Memory (re)allocation failed",
    [DPATCH_STATUS_EMPROT] = "Failed to modify memory protection",
    [DPATCH_STATUS_EUNKNOWN] = "Unsupported or unknown patch operation",
    [DPATCH_STATUS_EDYN] = "Error accessing dynamic symbols",
    [DPATCH_STATUS_EFILE] = "File I/O error",
    [DPATCH_STATUS_ESYNTAX] = "Script parsing error"
};

/**
 * Get a human readable string describing a status code.
 *
 * @param status Status code to translate.
 * @return Pointer to a string describing the error.
 */
const char* str_status(dpatch_status status)
{
    return status_strings[status];
}
