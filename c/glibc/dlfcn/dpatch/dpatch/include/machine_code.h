/**
 * @file dpatch/include/machine_code.h
 *
 * `machine_code.h` defines a `machine_code_t` representing
 * a block of executable binary code, and declares
 * functions for manipulating the binary and injecting it
 * into memory.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date November 2020.
 */

#ifndef DPATCH_INCLUDE_MACHINE_CODE_H_
#define DPATCH_INCLUDE_MACHINE_CODE_H_

#include "status.h"
#include <stddef.h>
#include <stdint.h>

/**
 * `machine_code_t` is a handle to a container for machine
 * executable binary code.
 */
typedef struct machine_code machine_code_t;

/**
 * Allocate and initialise a new machine code container.
 *
 * @param new Location to store new machine code handle.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status machine_code_new(machine_code_t** new);

/**
 * Deallocate a machine code container and free its
 * contents.
 *
 * @param machine_code Handle to free and nullify.
 */
void machine_code_free(machine_code_t* machine_code);

/**
 * Get the length, in bytes, of binary stored in the
 * container referenced by handle `machine_code`.
 *
 * @param machine_code Handle to get the length of.
 * @return The length, in bytes, of the `machine_code`.
 */
size_t machine_code_length(machine_code_t* machine_code);

/**
 * Append a byte to the machine code.
 *
 * @param machine_code Handle to the machine code to append.
 * @param byte A byte to append to the machine code.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status machine_code_append(machine_code_t* machine_code, uint8_t byte);
/**
 * Append bytes to the end of machine code.
 *
 * @param machine_code Handle to the machine code to append.
 * @param length Number of bytes to append to the machine code.
 * @param bytes Array of bytes to append.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status machine_code_append_array(machine_code_t* machine_code, size_t length, uint8_t bytes[]);

/**
 * Reset the contents of a machine code handle to the empty
 * container.
 *
 * @note This does not reset the size of the allocated memory.
 *
 * @param machine_code Handle to the machine code to empty.
 */
void machine_code_empty(machine_code_t* machine_code);

/**
 * Insert machine code into a program segment.
 *
 * @param machine_code Handle to the machine code to insert.
 * @param address Address to write the code into.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status machine_code_insert(machine_code_t* machine_code, intptr_t address);

#endif
