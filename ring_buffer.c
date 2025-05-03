/******************************************************************************
 * File        : ring_buffer.c
 * Author      : Huseyink
 * Date        : Apr 22, 2025
 * Version     : 1.0.0
 * Description : Statically allocated ring buffer implementation
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "ring_buffer.h"

/* Defines/macros ------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Private Variables ---------------------------------------------------------*/

/* Private Function Prototypes -----------------------------------------------*/

/* Private Function Definitions ----------------------------------------------*/
static void ring_buffer_lock(const ring_buffer_t *rb);
static void ring_buffer_unlock(const ring_buffer_t *rb);

/* Function Definitions ------------------------------------------------------*/

/**
 * @brief Calls the user-defined lock function if one is set.
 *
 * This is typically used to ensure exclusive access to the buffer during
 * operations in multithreaded or interrupt-driven environments.
 *
 * @param rb Pointer to the ring buffer instance.
 */
static void ring_buffer_lock(const ring_buffer_t *rb)
{
    if (rb->lock)
    {
        rb->lock();
    }
}

/**
 * @brief Calls the user-defined unlock function if one is set.
 *
 * Complements the ring_buffer_lock() function. Should be called after
 * buffer operations to release exclusive access.
 *
 * @param rb Pointer to the ring buffer instance.
 */
static void ring_buffer_unlock(const ring_buffer_t *rb)
{
    if (rb->unlock)
    {
        rb->unlock();
    }
}

/**
 * @brief Initialize the ring buffer with a given size.
 * @param rb Pointer to ring buffer instance.
 */
void ring_buffer_init(ring_buffer_t *rb)
{
    rb->size = RING_BUFFER_MAX_SIZE;
    rb->head = 0;
    rb->tail = 0;
    rb->full = 0;
    rb->lock = 0;
    rb->unlock = 0;
}

/**
 * @brief Set optional lock/unlock hooks for thread or ISR safety.
 * @param rb Pointer to ring buffer instance.
 * @param lock Lock function (e.g. disable IRQ).
 * @param unlock Unlock function (e.g. enable IRQ).
 */
void ring_buffer_set_lock_hooks(ring_buffer_t *rb,
                                ring_buffer_lock_fn_t lock,
                                ring_buffer_unlock_fn_t unlock)
{
    rb->lock = lock;
    rb->unlock = unlock;
}

/**
 * @brief Reset the ring buffer to empty state.
 * @param rb Pointer to ring buffer instance.
 */
void ring_buffer_reset(ring_buffer_t *rb)
{
    ring_buffer_lock(rb);

    rb->head = 0;
    rb->tail = 0;
    rb->full = 0;

    ring_buffer_unlock(rb);
}

/**
 * @brief Check if the ring buffer is empty.
 * @param rb Pointer to ring buffer instance.
 * @return 1 if empty, 0 otherwise.
 */
uint8_t ring_buffer_is_empty(const ring_buffer_t *rb)
{
    return (!rb->full && (rb->head == rb->tail));
}

/**
 * @brief Check if the ring buffer is full.
 * @param rb Pointer to ring buffer instance.
 * @return 1 if full, 0 otherwise.
 */
uint8_t ring_buffer_is_full(const ring_buffer_t *rb)
{
    return rb->full;
}

/**
 * @brief Get the capacity (max usable size) of the buffer.
 * @param rb Pointer to ring buffer instance.
 * @return Size of the buffer.
 */
uint16_t ring_buffer_capacity(const ring_buffer_t *rb)
{
    return rb->size;
}

/**
 * @brief Get the number of bytes currently stored in the buffer.
 * @param rb Pointer to ring buffer instance.
 * @return Number of bytes available to read.
 */
uint16_t ring_buffer_size(const ring_buffer_t *rb)
{
    if (rb->full)
    {
        return rb->size;
    }

    if (rb->head >= rb->tail)
    {
        return rb->head - rb->tail;
    }

    return rb->size - (rb->tail - rb->head);
}

/**
 * @brief Get the number of free bytes in the buffer.
 * @param rb Pointer to ring buffer instance.
 * @return Number of bytes available to write.
 */
uint16_t ring_buffer_free(const ring_buffer_t *rb)
{
	uint16_t free = 0;

    if (rb->full)
    {
    	free = 0;
    }

	if (rb->head >= rb->tail)
	{
		free = rb->size - (rb->head - rb->tail);
	}
	else
	{
		free = rb->tail - rb->head;
	}

    return free;
}

/**
 * @brief Write a single byte into the ring buffer.
 * @param rb Pointer to ring buffer instance.
 * @param data Byte to write.
 * @return 1 on success, 0 if buffer is full.
 */
uint8_t ring_buffer_put(ring_buffer_t *rb, uint8_t data)
{
    uint8_t success = 0;

    ring_buffer_lock(rb);

    if (!rb->full)
    {
        rb->buffer[rb->head] = data;
        rb->head = (rb->head + 1) % rb->size;
        rb->full = (rb->head == rb->tail);
        success = 1;
    }

    ring_buffer_unlock(rb);

    return success;
}

/**
 * @brief Read a single byte from the ring buffer.
 * @param rb Pointer to ring buffer instance.
 * @param data Pointer to store the read byte.
 * @return 1 on success, 0 if buffer is empty.
 */
uint8_t ring_buffer_get(ring_buffer_t *rb, uint8_t *data)
{
    uint8_t success = 0;

    ring_buffer_lock(rb);

    if (!ring_buffer_is_empty(rb))
    {
        *data = rb->buffer[rb->tail];
        rb->tail = (rb->tail + 1) % rb->size;
        rb->full = 0;

        success = 1;
    }

    ring_buffer_unlock(rb);

    return success;
}

/**
 * @brief Peek the next byte without removing it from the buffer.
 * @param rb Pointer to ring buffer instance.
 * @param data Pointer to store the peeked byte.
 * @return 1 on success, 0 if buffer is empty.
 */
uint8_t ring_buffer_peek(const ring_buffer_t *rb, uint8_t *data)
{
    uint8_t success = 0;

    ring_buffer_lock(rb);

    if (!ring_buffer_is_empty(rb))
    {
        *data = rb->buffer[rb->tail];
        success = 1;
    }

    ring_buffer_unlock(rb);

    return success;
}

/**
 * @brief Write multiple bytes into the buffer.
 * @param rb Pointer to ring buffer instance.
 * @param data Pointer to the input data buffer.
 * @param length Number of bytes to write.
 * @return Number of bytes actually written.
 */
uint16_t ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, uint16_t length)
{
    uint16_t written = 0;

    ring_buffer_lock(rb);

    for (written = 0; written < length; written++)
    {
        if (!ring_buffer_put(rb, data[written]))
        {
            break;
        }
    }

    ring_buffer_unlock(rb);

    return written;
}

/**
 * @brief Read multiple bytes from the buffer.
 * @param rb Pointer to ring buffer instance.
 * @param data Pointer to output buffer.
 * @param length Maximum number of bytes to read.
 * @return Number of bytes actually read.
 */
uint16_t ring_buffer_read(ring_buffer_t *rb, uint8_t *data, uint16_t length)
{
    uint16_t read = 0;

    ring_buffer_lock(rb);

    for (read = 0; read < length; ++read)
    {
        if (!ring_buffer_get(rb, &data[read]))
        {
            break;
        }
    }

    ring_buffer_unlock(rb);

    return read;
}

/**
 * @brief Get pointer to the start of the linear writable block.
 *
 * Returns a direct pointer to the current head of the buffer where data can be
 * written without wrapping. Used for DMA or block memcpy operations.
 *
 * @param rb Pointer to ring buffer.
 * @return Pointer to the writable block or NULL if buffer is full.
 */
uint8_t *ring_buffer_get_linear_block_write_address(ring_buffer_t *rb)
{
    if ((rb == NULL) || (rb->full))
    {
        return NULL;
    }

    return &rb->buffer[rb->head];
}

/**
 * @brief Get pointer to the start of the linear readable block.
 *
 * Returns a direct pointer to the current tail of the buffer where data can be
 * read without wrapping. Useful for DMA or bulk data reads.
 *
 * @param rb Pointer to ring buffer.
 * @return Pointer to the readable block or NULL if buffer is empty.
 */
uint8_t *ring_buffer_get_linear_block_read_address(ring_buffer_t *rb)
{
    if (rb == NULL || ring_buffer_is_empty(rb))
    {
        return NULL;
    }

    return &rb->buffer[rb->tail];
}

/**
 * @brief Get length of the linear writable block.
 *
 * Returns the maximum number of bytes that can be written in one go
 * without wrapping.
 *
 * @param rb Pointer to ring buffer.
 * @return Number of bytes available for linear write.
 */
uint16_t ring_buffer_get_linear_block_write_length(const ring_buffer_t *rb)
{
    if (rb == NULL || rb->full)
    {
        return 0;
    }

    if (rb->tail > rb->head)
    {
        return rb->tail - rb->head;
    }
    else
    {
        return rb->size - rb->head;
    }
}

/**
 * @brief Get length of the linear readable block.
 *
 * Returns the maximum number of bytes that can be read in one go
 * without wrapping.
 *
 * @param rb Pointer to ring buffer.
 * @return Number of bytes available for linear read.
 */
uint16_t ring_buffer_get_linear_block_read_length(const ring_buffer_t *rb)
{
    if (rb == NULL || ring_buffer_is_empty(rb))
    {
        return 0;
    }

    if (rb->head > rb->tail)
    {
        return rb->head - rb->tail;
    }
    else
    {
        return rb->size - rb->tail;
    }
}

/**
 * @brief Commit written bytes to the ring buffer (advance head).
 *
 * This function must be called after a direct memory or DMA write into
 * the buffer to update the head pointer accordingly.
 *
 * @param rb Pointer to ring buffer.
 * @param length Number of bytes written.
 * @return 1 on success, 0 if buffer is full.
 */
uint8_t ring_buffer_commit_write(ring_buffer_t *rb, uint16_t length)
{
    if ((rb == NULL) || (length > ring_buffer_get_linear_block_write_length(rb)))
    {
        return 0;
    }

    ring_buffer_lock(rb);

    rb->head = (rb->head + length) % rb->size;
    rb->full = (rb->head == rb->tail);

    ring_buffer_unlock(rb);

    return 1;
}

/**
 * @brief Commit read bytes from the ring buffer (advance tail).
 *
 * This function must be called after a direct memory or DMA read from
 * the buffer to update the tail pointer accordingly.
 *
 * @param rb Pointer to ring buffer.
 * @param length Number of bytes read.
 * @return 1 on success, 0 if buffer is full.
 */
uint8_t ring_buffer_commit_read(ring_buffer_t *rb, uint16_t length)
{
    if ((rb == NULL) || (length > ring_buffer_get_linear_block_read_length(rb)))
    {
        return 0;
    }

    ring_buffer_lock(rb);

    rb->tail = (rb->tail + length) % rb->size;
    rb->full = 0;

    ring_buffer_unlock(rb);

    return 1;
}
