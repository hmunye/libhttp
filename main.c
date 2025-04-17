#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "channel.h"

#define BUFFER_SIZE 8
#define LINE_SIZE 1024

#define CHANNEL_SIZE 2
#define CONSUMER_SIGNAL NULL

channel_t *chan;

// producer: reads lines from file and writes to channel
static void *producer(void *arg) {
    FILE *file = (FILE *)arg;

    char buf[BUFFER_SIZE + 1];
    char line[LINE_SIZE + 1];

    size_t bytes_read;
    size_t line_idx = 0;
    while ((bytes_read = fread(buf, sizeof *buf, (sizeof buf) - 1, file)) > 0) {
        buf[bytes_read] = '\0';
        for (size_t i = 0; i < bytes_read; ++i) {
            if (buf[i] == '\n') {
                line[line_idx] = '\0';

                char *alloc_line = malloc(line_idx + 1);
                if (!alloc_line) {
                    perror("ERROR: producer (malloc)");
                    pthread_exit(NULL);
                }
                memcpy(alloc_line, line, line_idx + 1);

                channel_write(chan, alloc_line);

                line[0] = '\0';
                line_idx = 0;
            } else {
                if (line_idx < LINE_SIZE) {
                    line[line_idx++] = buf[i];
                }
            }
        }
    }

    if (line_idx > 0) {
        line[line_idx] = '\0';

        char *alloc_line = malloc(line_idx + 1);
        if (!alloc_line) {
            perror("ERROR: producer (malloc)");
            pthread_exit(NULL);
        }
        memcpy(alloc_line, line, line_idx + 1);

        channel_write(chan, alloc_line);
    }

    if (feof(file)) {
        goto cleanup;
    }

    if (ferror(file)) {
        fprintf(stderr, "ERROR: producer: failed to read from file.");
        goto cleanup;
    }

cleanup:
    channel_write(chan, CONSUMER_SIGNAL);
    fclose(file);
    return NULL;
}

// consumer: reads lines from the channel and prints to stdout
int main(void) {
    FILE *file = fopen("messages.txt", "r");
    if (!file) {
        perror("ERROR: fopen");
        exit(1);
    }

    chan = channel_init(CHANNEL_SIZE);
    if (!chan) {
        exit(1);
    }

    pthread_t prod_thread;
    if (pthread_create(&prod_thread, NULL, producer, file) != 0) {
        perror("ERROR: pthread_create");
        channel_free(chan, NULL);
        exit(1);
    }

    char *line;
    while ((line = channel_read(chan)) != NULL) {
        printf("read: %s\n", line);
        free(line);
    }

    pthread_join(prod_thread, NULL);
    channel_free(chan, NULL);
    return 0;
}
