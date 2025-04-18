#include "test_common.h"

size_t chunk_reader_read(chunk_reader_t *cr, char *buf, size_t size) {
    size_t data_len = strlen(cr->data);

    if (cr->pos >= data_len) {
        // No more data to read, since pos has caught up to or exceeded data's
        // length.
        return 0;
    }

    // Calculate how many bytes to read from current position.
    size_t end_index = cr->pos + cr->bytes_per_read;
    if (end_index > data_len) {
        end_index = data_len;
    }

    size_t idx = 0;
    // Copy bytes into `buf` in the range [cr->pos, end_index).
    for (size_t i = cr->pos; i < end_index && idx < size - 1; ++i, ++idx) {
        buf[idx] = cr->data[i];
    }
    buf[idx] = '\0';

    // Increment the position by how many bytes were copied.
    cr->pos += idx;

    return idx;
}

void print_escaped_buf(const char *buf, size_t len) {
    printf("'");

    for (size_t i = 0; i < len; ++i) {
        if (buf[i] == '\r') {
            printf("\\r");
            continue;
        }

        if (buf[i] == '\n') {
            printf("\\n");
            continue;
        }

        putchar(buf[i]);
    }

    printf("'");
}
