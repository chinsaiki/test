/**
 * @file dpatch/machine_code.c
 *
 * `machine_code.h` defines functions for manipulating
 * blocks of machine executable binary code, and injecting
 * them into process memory.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date November 2020.
 */

#include "machine_code.h"
#include "status.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define MACHINE_CODE_DEFAULT_LEN 8

/**
 * Storage for variable length chunks of executable binary.
 */
struct machine_code
{
    /** Length of the binary stored. */
    size_t length;

    /** Length of memory allocated to the binary. */
    size_t allocated_length;

    /** The binary data stored. */
    uint8_t* binary;
};

/**
 * Allocate and initialise a new machine code container.
 *
 * @param new Location to store new machine code handle.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status machine_code_new(machine_code_t** new)
{
    assert(new != NULL);
    machine_code_t* handle = malloc(sizeof(struct machine_code));
    *new = handle;
    if (handle == NULL)
    {
        return DPATCH_STATUS_ENOMEM;
    }
    handle->length = 0;
    handle->allocated_length = MACHINE_CODE_DEFAULT_LEN;
    handle->binary = malloc(handle->allocated_length);
    if (handle->binary == NULL)
    {
        machine_code_free(handle);
        *new = NULL;
        return DPATCH_STATUS_ENOMEM;
    }
    return DPATCH_STATUS_OK;
}

/**
 * Deallocate a machine code container and free its
 * contents.
 *
 * @param machine_code Handle to free and nullify.
 */
void machine_code_free(machine_code_t* machine_code)
{
    assert(machine_code != NULL);
    if (machine_code->binary)
    {
        free(machine_code->binary);
    }
    machine_code->binary = NULL;
    machine_code->allocated_length = 0;
    machine_code->length = 0;
    free(machine_code);
}

/**
 * Grow the memory allocated to a machine code container.
 *
 * @param machine_code Handle to the container to grow.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status machine_code_grow(machine_code_t* machine_code)
{
    void* realloc_result = NULL;
    assert(machine_code != NULL);
    machine_code->allocated_length *= 2;
    realloc_result = realloc(
        machine_code->binary,
        machine_code->allocated_length
    );
    if (realloc_result == NULL)
    {
        machine_code->allocated_length /= 2;
        return DPATCH_STATUS_ENOMEM;
    }
    machine_code->binary = realloc_result;
    return DPATCH_STATUS_OK;
}

/**
 * Get the length, in bytes, of binary stored in the
 * container referenced by handle `machine_code`.
 *
 * @param machine_code Handle to get the length of.
 * @return The length, in bytes, of the `machine_code`.
 */
size_t machine_code_length(machine_code_t* machine_code)
{
    assert(machine_code != NULL);
    return machine_code->length;
}

/**
 * Append a byte to the machine code.
 *
 * @param machine_code Handle to the machine code to append.
 * @param byte A byte to append to the machine code.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status machine_code_append(machine_code_t* machine_code, uint8_t byte)
{
    dpatch_status status = DPATCH_STATUS_OK;
    assert(machine_code != NULL);
    if (machine_code->length == machine_code->allocated_length)
    {
        PROPAGATE_ERROR(machine_code_grow(machine_code), status);
    }
    machine_code->binary[machine_code->length++] = byte;
    return DPATCH_STATUS_OK;
}

/**
 * Append bytes to the end of machine code.
 *
 * @param machine_code Handle to the machine code to append.
 * @param length Number of bytes to append to the machine code.
 * @param bytes Array of bytes to append.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status machine_code_append_array(machine_code_t* machine_code, size_t length, uint8_t bytes[])
{
    dpatch_status status;
    size_t i = 0;
    for (i = 0; i < length; i++)
    {
        status = machine_code_append(machine_code, bytes[i]);
        if (IS_ERROR(status))
        {
            machine_code->length -= i;
            return status;
        }
    }
    return DPATCH_STATUS_OK;
}

/**
 * Reset the contents of a machine code handle to the empty
 * container.
 *
 * @note This does not reset the size of the allocated memory.
 *
 * @param machine_code Handle to the machine code to empty.
 */
void machine_code_empty(machine_code_t* machine_code)
{
    machine_code->length = 0;
}

/**
 * Applies `mprotect` to a memory range, rounding if
 * required to meet page alignment requirements.
 *
 * @param address Location to modify permissions for.
 * @param length Length of the segment to adjust.
 * @param prot The new memory protection mode bits.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status mprotect_round_(intptr_t address, size_t length, int prot)
{
    intptr_t delta;
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size < 1)
    {
        // Sysconf many not support `_SC_PAGESIZE` on the host.
        return DPATCH_STATUS_EMPROT;
    }
    assert(page_size > 0);
    delta = address % page_size;
    address -= delta;
    length += delta;
    #pragma message "`mprotect` on memory not acquired by `mmap` is a non-POSIX Linux extention."
    if (mprotect((void*) address, length, prot) == -1)
    {
        return DPATCH_STATUS_EMPROT;
    }
    return DPATCH_STATUS_OK;
}

/**
 * Insert machine code into a program segment.
 *
 * @param machine_code Handle to the machine code to insert.
 * @param address Address to write the code into.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status machine_code_insert(machine_code_t* machine_code, intptr_t address)
{
    dpatch_status status = DPATCH_STATUS_OK;
    status = mprotect_round_(address, machine_code->length, PROT_READ | PROT_WRITE | PROT_EXEC);
    if (IS_ERROR(status))
    {
        return status;
    }
    memcpy((void*) address, (void*) machine_code->binary, machine_code->length);
    status = mprotect_round_(address, machine_code->length, PROT_READ | PROT_EXEC);
    if (IS_ERROR(status))
    {
        return status;
    }
    return DPATCH_STATUS_OK;
}
