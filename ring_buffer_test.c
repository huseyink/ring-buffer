
#include <stdio.h>
#include <string.h>
#include "ring_buffer.h"

// Enhanced assert macro with reason
#define ASSERT(cond, msg, reason)                   \
    do                                              \
    {                                               \
        if (!(cond))                                \
        {                                           \
            printf("FAIL: %s - %s\n", msg, reason); \
            return 1;                               \
        }                                           \
        else                                        \
        {                                           \
            printf("PASS: %s\n", msg);              \
        }                                           \
    } while (0)

int main(void)
{
    ring_buffer_t rb;
    ring_buffer_init(&rb);

    ASSERT(ring_buffer_is_empty(&rb), "Buffer should be empty after init", "Expected empty, got not empty");
    ASSERT(!ring_buffer_is_full(&rb), "Buffer should not be full after init", "Expected not full, got full");

    // Test single-byte put/get
    ASSERT(ring_buffer_put(&rb, 0xAB), "Put one byte", "Put failed on non-full buffer");
    ASSERT(!ring_buffer_is_empty(&rb), "Buffer should not be empty after put", "Expected not empty, got empty");

    uint8_t byte;
    ASSERT(ring_buffer_get(&rb, &byte), "Get one byte", "Get failed on non-empty buffer");
    ASSERT(byte == 0xAB, "Received byte should match written byte", "Expected 0xAB, got different value");
    ASSERT(ring_buffer_is_empty(&rb), "Buffer should be empty after get", "Expected empty, got not empty");

    // Fill the buffer
    for (int i = 0; i < RING_BUFFER_MAX_SIZE; i++)
    {
        ASSERT(ring_buffer_put(&rb, (uint8_t)i), "Filling buffer", "Put failed before buffer full");
    }

    ASSERT(ring_buffer_is_full(&rb), "Buffer should be full", "Expected full, got not full");
    ASSERT(!ring_buffer_put(&rb, 0xFF), "Put should fail when full", "Put succeeded even though buffer should be full");

    // Empty the buffer
    for (int i = 0; i < RING_BUFFER_MAX_SIZE; i++)
    {
        ASSERT(ring_buffer_get(&rb, &byte), "Reading filled buffer", "Get failed before expected empty");
        ASSERT(byte == (uint8_t)i, "Read value should match written value", "Mismatch between written and read data");
    }

    ASSERT(ring_buffer_is_empty(&rb), "Buffer should be empty after reading all", "Expected empty, got not empty");

    // Test multi-byte write/read
    ring_buffer_reset(&rb);
    uint8_t data_in[] = {10, 20, 30, 40, 50};
    uint8_t data_out[5] = {0};

    ASSERT(ring_buffer_write(&rb, data_in, 5) == 5, "Multi-byte write", "Write did not return expected count");
    ASSERT(ring_buffer_read(&rb, data_out, 5) == 5, "Multi-byte read", "Read did not return expected count");
    ASSERT(memcmp(data_in, data_out, 5) == 0, "Data should match after read/write", "Mismatch in read/write data content");

    printf("All tests completed successfully.\n");
    return 0;
}
