/**
 * @file `dpatch/include/status.h`
 *
 * `status.h` defines status codes used by dpatch.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date October 2020.
 */

#ifndef DPATCH_INCLUDE_STATUS_H_
#define DPATCH_INCLUDE_STATUS_H_

#include <stdbool.h>
#include <stdlib.h>

/**
 * Template for an exit on error macro,
 *
 * @warning This macro is for internal use only. See
 * `EXIT_ON_ERROR` for a user macro.
 *
 * @param x A predicate evaluating to a `dpatch_status`.
 */
#define _EXIT_ON_ERROR_TEMPLATE(x) \
    if ((x) != DPATCH_STATUS_OK) \
    { \
        syslog( \
            LOG_ERR, \
            "Error: %s. (%s:%d)", \
            str_status(x), \
            __FILE__, \
            __LINE__); \
        exit(EXIT_FAILURE); \
    }

/**
 * Template for an error logging macro,
 *
 * @warning This macro is for internal use only. See
 * `LOG_ON_ERROR` for a user macro.
 *
 * @param x A predicate evaluating to a `dpatch_status`.
 */
#define _LOG_ON_ERROR_TEMPLATE(x) \
    if ((x) != DPATCH_STATUS_OK) \
    { \
        syslog( \
            LOG_ERR, \
            "Error: %s. (%s:%d)", \
            str_status(x), \
            __FILE__, \
            __LINE__); \
    }

/**
 * Template for an error propagation macro.
 *
 * @warning This macro is for internal use only. See
 * `PROPAGATE_ERROR` for a user macro.
 *
 * @param statement Statement evaluating to a
 * `dpatch_status`
 * @param status `dpatch_status` variable for temporary
 * storage.
 */
#define _PROPAGATE_ERROR_TEMPLATE(statement, status) \
    status = statement; \
    if (status != DPATCH_STATUS_OK) \
    { \
        return status; \
    }

/**
 * Exit if an error occures.
 *
 * Exit if an error occures, logging an error to the
 * system log.
 *
 * @param x A predicate evaluating to a `dpatch_status`.
 */
#define EXIT_ON_ERROR(x) _EXIT_ON_ERROR_TEMPLATE(x)

/**
 * log if an error occures.
 *
 * Log an error message to the system log if an error
 * occures.
 *
 * @param x A predicate evaluating to a `dpatch_status`.
 */
#define LOG_ON_ERROR(x) _LOG_ON_ERROR_TEMPLATE(x)

/**
 * Error propagation macro.
 *
 * Progagates an error, by returning, if an error occures.
 *
 * @param statement Statement evaluating to a
 * `dpatch_status`
 * @param status `dpatch_status` variable for temporary
 * storage.
 */
#define PROPAGATE_ERROR(statement, status) \
    _PROPAGATE_ERROR_TEMPLATE(statement, status)

/**
 * Tests if a statement is a dpatch error.
 *
 * @param statement Statement evaluating to a `dpatch_status`.
 */
#define IS_ERROR(statement) \
    (statement == DPATCH_STATUS_OK ? false : true)

/**
 * Indicates the success or failure of an operation by
 * dpatch.
 */
typedef enum
{
    /** Success. */
    DPATCH_STATUS_OK = 0,

    /** General or unspecified error. */
    DPATCH_STATUS_ERROR,

    /** Memory (re)/allocation failed. */
    DPATCH_STATUS_ENOMEM,

    /** Failed to modify memory protection. */
    DPATCH_STATUS_EMPROT,

    /** Unsupported or unknown patch operation. */
    DPATCH_STATUS_EUNKNOWN,

    /** Error accessing dynamic symbols with `dlfcn.h` */
    DPATCH_STATUS_EDYN,

    /** File I/O error. */
    DPATCH_STATUS_EFILE,

    /** Script parsing error. */
    DPATCH_STATUS_ESYNTAX
} dpatch_status;

/**
 * Get a human readable string describing a status code.
 *
 * @param status Status code to translate.
 * @return Pointer to a string describing the error.
 */
const char* str_status(dpatch_status status);

#endif
