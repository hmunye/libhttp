#include "channel.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct channel_t {
    void **buf;
    uint32_t read_idx;  /* Index of the next item to read. */
    uint32_t write_idx; /* Index of the next slot to write to. */
    uint32_t mask;      /* Using bitmask for wrapping buffer indices. */

    pthread_cond_t empty_cond; /* Signals when the buffer has new data. */
    pthread_cond_t full_cond;  /* Signals when space becomes available. */
    pthread_mutex_t lock;      /* Synchronizes access to the buffer. */
};

channel_t *channel_init(uint32_t capacity) {
    assert((capacity & (capacity - 1)) == 0);

    channel_t *chan = malloc(sizeof(*chan));
    if (!chan) {
        perror("ERROR: channel_init (malloc)");
        return NULL;
    }

    chan->buf = calloc(capacity, sizeof(*chan->buf));
    if (!chan->buf) {
        perror("ERROR: channel_init (calloc)");
        free(chan);
        return NULL;
    }

    chan->read_idx = 0;
    chan->write_idx = 0;
    chan->mask = capacity - 1;

    if (pthread_cond_init(&chan->empty_cond, NULL) != 0) {
        fprintf(stderr, "ERROR: channel_init: failed to init empty_cond.\n");
        free(chan->buf);
        free(chan);
        return NULL;
    }

    if (pthread_cond_init(&chan->full_cond, NULL) != 0) {
        fprintf(stderr, "ERROR: channel_init: failed to init full_cond.\n");
        free(chan->buf);
        free(chan);
        return NULL;
    }

    // Function always returns 0.
    pthread_mutex_init(&chan->lock, NULL);

    return chan;
}

void channel_free(channel_t *chan, chan_cleanup_fn cleanup_fn) {
    assert(chan);

    pthread_mutex_lock(&chan->lock);

    if (cleanup_fn) {
        for (size_t i = 0; i < chan->mask + 1; ++i) {
            cleanup_fn(chan->buf[i]);
        }
    }

    free(chan->buf);

    // In the LinuxThreads implementation, no resources are associated with
    // condition variables, thus pthread_cond_destroy actually does nothing
    // except checking that the condition has no waiting threads.
    if (pthread_cond_destroy(&chan->empty_cond) != 0) {
        fprintf(stderr,
                "ERROR: channel_free: threads still waiting on empty_cond.\n");
        return;
    }

    if (pthread_cond_destroy(&chan->full_cond) != 0) {
        fprintf(stderr,
                "ERROR: channel_free: threads still waiting on full_cond.\n");
        return;
    }

    pthread_mutex_unlock(&chan->lock);

    // In the LinuxThreads implementation, no resources are associated with
    // mutex objects, thus pthread_mutex_destroy actually does nothing except
    // checking that the mutex is unlocked.
    pthread_mutex_destroy(&chan->lock);

    free(chan);
}

void channel_write(channel_t *chan, void *data) {
    assert(chan && chan->buf);

    pthread_mutex_lock(&chan->lock);

    // Buffer is full if advancing write index would equal read index.
    while (((chan->write_idx + 1) & chan->mask) == chan->read_idx) {
        pthread_cond_wait(&chan->full_cond, &chan->lock);
    }

    chan->buf[chan->write_idx] = data;
    chan->write_idx = (chan->write_idx + 1) & chan->mask;

    // Signal to waiting threads data is ready to read.
    pthread_cond_signal(&chan->empty_cond);

    pthread_mutex_unlock(&chan->lock);
}

void *channel_read(channel_t *chan) {
    assert(chan && chan->buf);

    pthread_mutex_lock(&chan->lock);

    // Buffer is empty is current write index equals current read index.
    while (chan->write_idx == chan->read_idx) {
        pthread_cond_wait(&chan->empty_cond, &chan->lock);
    }

    void *data = chan->buf[chan->read_idx];
    chan->read_idx = (chan->read_idx + 1) & chan->mask;

    // Signal to waiting threads slot is available to write.
    pthread_cond_signal(&chan->full_cond);

    pthread_mutex_unlock(&chan->lock);

    return data;
}
