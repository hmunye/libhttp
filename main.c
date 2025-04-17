#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "channel.h"
#include "listener.h"

// ENCODE_INT(n): int -> intptr_t -> void *
#define ENCODE_INT(n) ((void *)(intptr_t)(n))
// DECODE_INT(p): void * -> intptr_t -> int
#define DECODE_INT(p) ((int)(intptr_t)(p))

#define HOST "127.0.0.1"
#define PORT "8080"
// Maximum number of pending connections in the queue
#define BACKLOG 10

#define BUFFER_SIZE 8
#define LINE_SIZE 1024

#define CHANNEL_SIZE 2
channel_t *chan;

// consumer: reads client connections from channel and writes lines to stdout
static void *producer(void *arg) {
    (void)arg;

    while (1) {
        int clientfd = DECODE_INT(channel_read(chan));

        char buf[BUFFER_SIZE + 1];
        char line[LINE_SIZE + 1];

        size_t line_idx = 0;
        ssize_t bytes_read;
        while ((bytes_read = recv(clientfd, buf, (sizeof buf) - 1, 0)) > 0) {
            buf[bytes_read] = '\0';
            for (ssize_t i = 0; i < bytes_read; ++i) {
                if (buf[i] == '\n') {
                    line[line_idx] = '\0';
                    printf("%s\n", line);
                    line_idx = 0;
                } else {
                    if (line_idx < LINE_SIZE) {
                        line[line_idx++] = buf[i];
                    }
                }
            }
        }

        if (bytes_read == -1) {
            perror("ERROR: recv");
            close(clientfd);
            continue;
        }

        // Remote side has closed the connection
        if (bytes_read == 0) {
            if (line_idx > 0) {
                line[line_idx] = '\0';
                printf("%s\n", line);
                line_idx = 0;
            }

            printf("server: client connection closed\n");
            close(clientfd);
        }
    }

    return NULL;
}

// producer: accepts incoming connections and writes connections to channel
int main(void) {
    // Disable buffering for stdout (line-buffered by default)
    setbuf(stdout, NULL);

    listener_t *listener = &tcp_listener;

    chan = channel_init(CHANNEL_SIZE);
    if (!chan) {
        exit(1);
    }

    listener->listen(HOST, PORT, BACKLOG);

    printf("server: [%s:%s] waiting for connections...\n", HOST, PORT);

    pthread_t cons_thread;
    if (pthread_create(&cons_thread, NULL, producer, NULL) != 0) {
        perror("ERROR: pthread_create");
        channel_free(chan, NULL);
        listener->close();
        exit(1);
    }

    connection_t conn;
    while (1) {
        if (listener->accept(&conn) == -1) {
            continue;
        }

        printf("server: got connection from %s\n", conn.remote_addr);

        channel_write(chan, ENCODE_INT(conn.clientfd));
    }

    if (pthread_join(cons_thread, NULL) != 0) {
        fprintf(stderr, "ERROR: pthread_join: failed to join thread.\n");
        channel_free(chan, NULL);
        listener->close();
        exit(1);
    }

    channel_free(chan, NULL);
    listener->close();
    return 0;
}
