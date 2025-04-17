#ifndef CHANNEL_H
#define CHANNEL_H

#include <stdint.h>

/* Opaque handle to a thread-safe ring buffer for inter-thread communication. */
typedef struct channel_t channel_t;

typedef void (*channel_cleanup_fn)(void *data);

/* Initialize a new channel with the given capacity. Capacity provided must be a
 * power-of-two. Returns a pointer to the channel, otherwise NULL. */
channel_t *channel_init(uint32_t capacity);

/* Frees the memory used by the channel and its buffer. Destroys all associated
 * condition variables and mutex. If `cleanup_fn` is non-NULL, it is invoked
 * on each remaining element in the buffer, otherwise elements are ignored. */
void channel_free(channel_t *chan, channel_cleanup_fn cleanup_fn);

/* Writes the given data to the channel. Blocks if the channel is full until
 * space becomes available. */
void channel_write(channel_t *chan, void *data);

/* Reads and returns the next element from the channel. Blocks if the channel is
 * empty until data becomes available. Callers should be aware that elements may
 * be overwritten in the channel, so returned pointers must be managed properly.
 */
void *channel_read(channel_t *chan);

#endif  // CHANNEL_H
