/**
 * @file dpatch/x64_code_generator.c
 *
 * Code generator for x64 CPUs.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date October 2020.
 */

#include "code_generator.h"
#include "machine_code.h"
#include "status.h"

/**
 * Append a guaranteed undefined opcode to a block of machine code.
 *
 * @param machine_code The binary container to append to.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status append_undefined_opcode(machine_code_t* machine_code)
{
    const uint8_t X64_UD2[] = {0x0f, 0x0b};
    return machine_code_append_array(machine_code, 2, (uint8_t*) X64_UD2);
}

/**
 * Generate a long jump to a 64-bit address.
 *
 * @param machine_code The binary container to append to.
 * @param addr Address to jump to.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status append_long_jump(machine_code_t* machine_code, intptr_t addr)
{
    dpatch_status status = DPATCH_STATUS_OK;
    const uint8_t LJMP_OPCODE = 0xff;
    const uint8_t LJMP_MODRM_EXTENSION = 0x1 << 5;
    const uint8_t MODRM_RIP_RELATIVE = 0x5;
    /* The jump pointer is immediately after this instruction. */
    const uint32_t LJMP_RIP_DISPLACEMENT = 0x0;
    PROPAGATE_ERROR(
        machine_code_append(machine_code, LJMP_OPCODE),
        status
    );
    PROPAGATE_ERROR(
        machine_code_append(machine_code,
            LJMP_MODRM_EXTENSION | MODRM_RIP_RELATIVE
        ),
        status
    );
    PROPAGATE_ERROR(
        machine_code_append_array(machine_code,
            sizeof LJMP_RIP_DISPLACEMENT,
            (uint8_t*) &LJMP_RIP_DISPLACEMENT
        ),
        status
    );
    PROPAGATE_ERROR(
        machine_code_append_array(machine_code,
            sizeof addr,
            (uint8_t*) &addr
        ),
        status
    );
    return DPATCH_STATUS_OK;
}
