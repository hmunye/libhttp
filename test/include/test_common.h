#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* Since TCP is a streaming protocol, chunks of the request could be sent, so
 * the parser potentially won't get the full HTTP request the first time. Using
 * a chunk reader during testing to simulate reading variable number of bytes at
 * a time.
 */
typedef struct {
    char *data;
    size_t bytes_per_read;
    size_t pos;
} chunk_reader_t;

size_t chunk_reader_read(chunk_reader_t *cr, char *buf, size_t size);
void print_escaped_buf(const char *buf, size_t len);

#endif  // TEST_COMMON_H
