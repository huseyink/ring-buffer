# Ring Buffer Module (C)

A lightweight, statically allocated ring (circular) buffer implementation in C, designed for embedded systems. This module is thread- and ISR-safe via optional locking hooks.

## Features

- Fixed-size buffer with user-defined capacity
- Single-byte and multi-byte read/write APIs
- Optional lock/unlock hooks for safe access in concurrent environments
- Support for buffer peek, reset, and size queries
- Minimal dependencies (`<stdint.h>` only)

## Files

- `ring_buffer.h` – Header file with API declarations and `ring_buffer_t` structure
- `ring_buffer.c` – Implementation of all ring buffer functions

## Usage

### Initialization

```c
ring_buffer_t rb;
ring_buffer_init(&rb);
```

### (Optional) Thread/ISR-Safety

You can provide locking hooks to ensure atomic operations:

```c
void lock_fn(void) { __disable_irq(); }
void unlock_fn(void) { __enable_irq(); }

ring_buffer_set_lock_hooks(&rb, lock_fn, unlock_fn);
```

### Writing to the Buffer

```c
ring_buffer_put(&rb, 0x55); // Write one byte

uint8_t data[] = {1, 2, 3};
ring_buffer_write(&rb, data, sizeof(data)); // Write multiple bytes
```

### Reading from the Buffer

```c
uint8_t byte;
if (ring_buffer_get(&rb, &byte)) {
    // byte contains valid data
}

uint8_t out[3];
ring_buffer_read(&rb, out, 3); // Read up to 3 bytes
```

### Peek and Status

```c
uint8_t peeked;
ring_buffer_peek(&rb, &peeked); // Peek next byte without removing

ring_buffer_is_empty(&rb);
ring_buffer_is_full(&rb);
ring_buffer_size(&rb);
```

### Resetting

```c
ring_buffer_reset(&rb); // Clears buffer
```

## Configuration

- Maximum size is defined in `ring_buffer.h`:
  ```c
  #define RING_BUFFER_MAX_SIZE (256)
  ```
