/******************************************************************************
 * File        : ring_buffer.h
 * Author      : Huseyink
 * Date        : Apr 22, 2025
 * Version     : 1.0.0
 * Description : Statically allocated ring buffer implementation
 ******************************************************************************/

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Defines/macros ------------------------------------------------------------*/
#ifndef NULL
#define NULL ((void *)0)
#endif

#define RING_BUFFER_MAX_SIZE (256)

/* Types ---------------------------------------------------------------------*/

/**
 * @brief Lock/unlock function pointer types for optional thread safety.
 */
typedef void (*ring_buffer_lock_fn_t)(void);
typedef void (*ring_buffer_unlock_fn_t)(void);

/**
 * @brief Ring buffer structure (statically allocated buffer).
 */
typedef struct ring_buffer_t
{
    uint8_t buffer[RING_BUFFER_MAX_SIZE];  /**< Internal data buffer. */
    uint16_t size;                         /**< Maximum size of the buffer. */
    uint16_t head;                         /**< Write index. */
    uint16_t tail;                         /**< Read index. */
    uint8_t  full;                         /**< Indicates if buffer is full. */

    ring_buffer_lock_fn_t lock;            /**< Optional lock function. */
    ring_buffer_unlock_fn_t unlock;        /**< Optional unlock function. */
} ring_buffer_t;

/* Function Prototypes -------------------------------------------------------*/

void ring_buffer_init(ring_buffer_t *rb);
void ring_buffer_set_lock_hooks(ring_buffer_t *rb, ring_buffer_lock_fn_t lock, ring_buffer_unlock_fn_t unlock);
void ring_buffer_reset(ring_buffer_t *rb);
uint8_t ring_buffer_is_empty(const ring_buffer_t *rb);
uint8_t ring_buffer_is_full(const ring_buffer_t *rb);
uint16_t ring_buffer_capacity(const ring_buffer_t *rb);
uint16_t ring_buffer_size(const ring_buffer_t *rb);
uint16_t ring_buffer_free(const ring_buffer_t *rb);
uint8_t ring_buffer_put(ring_buffer_t *rb, uint8_t data);
uint8_t ring_buffer_get(ring_buffer_t *rb, uint8_t *data);
uint8_t ring_buffer_peek(const ring_buffer_t *rb, uint8_t *data);
uint16_t ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, uint16_t length);
uint16_t ring_buffer_read(ring_buffer_t *rb, uint8_t *data, uint16_t length);
uint8_t *ring_buffer_get_linear_block_write_address(ring_buffer_t *rb);
uint8_t *ring_buffer_get_linear_block_read_address(ring_buffer_t *rb);
uint16_t ring_buffer_get_linear_block_write_length(const ring_buffer_t *rb);
uint16_t ring_buffer_get_linear_block_read_length(const ring_buffer_t *rb);
uint8_t ring_buffer_commit_read(ring_buffer_t *rb, uint16_t length);
uint8_t ring_buffer_commit_write(ring_buffer_t *rb, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* RING_BUFFER_H_ */
