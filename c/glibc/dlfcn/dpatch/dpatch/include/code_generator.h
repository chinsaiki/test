/**
 * @file dpatch/include/code_generator.h
 *
 * `code_generator.h` defines an interface to a system which can
 * generate machine code for a target architecture.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date October 2020.
 */

#ifndef DPATCH_INCLUDE_CODE_GENERATOR_H_
#define DPATCH_INCLUDE_CODE_GENERATOR_H_

#include "machine_code.h"
#include "status.h"
#include <stdint.h>

/**
 * Append a guaranteed undefined opcode to a block of machine code.
 *
 * @param machine_code The binary container to append to.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status append_undefined_opcode(machine_code_t* machine_code);

/**
 * Generate an opcode guaranteed to be undefined.
 *
 * @param machine_code The binary container to append to.
 * @param addr Address to jump to.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status append_long_jump(machine_code_t* machine_code, intptr_t addr);

#endif
